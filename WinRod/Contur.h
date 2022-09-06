


//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of the 2D graphisc library. The library is also responsible for window creation and window signals interpretation. 
//
// Writen by Daniel Kaminski 
//
// 01/01/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

#pragma once

#define			MAX_POINTS					MAXTHEO
#define			MAX_FIT_CURVES				3				//Maximum number of theoretical calculated data used py DRAW functions	
#define			MAX_PEN						10 + MAX_FIT_CURVES	//number odf pens used for drawing 

#define			AXIS_TITLE_LENGHT			100
//The frames of 2D graphs
#define			RamkaX_left					70
#define			RamkaX_right				10 
#define			RamkaY_bottom				60 
#define			RamkaY_top					50 
#define     	MARYGIN						0.1						//Extension of the scale
//Additional variable for scale
#define			NR_OF_POINTS_ON_SCALE_X		50
#define			NR_OF_POINTS_ON_SCALE_Y		60

//Defined valuse for proper mouse behaviour during action for 2D graphs
#define			SHIFT_FACTOR				0.003				//factor to set the shift of scale acoording to the mouse position
#define			MOUSE_WHEEL_DOLLY_SPEED		0.002

//Dimension of status window
#define			STATUS_WINDOW_HIGH			20			//The hight of status window




class ConturClass
{
public:
	ConturClass();
	virtual ~ConturClass();

public:

	//Data structure from which points and model are draw
	typedef struct tag_t{

		bool status;				//to plot or not
		int		NrOfPoints;			//Number of points to draw
		double x[MAX_POINTS + 1];
		double y[MAX_POINTS + 1];
		double z[MAX_POINTS + 1];
		double sigma[MAX_POINTS + 1];

		double	RLAT[6];			//Lattice parameter to draw rec. space

		WCHAR	Title[HEADER];			//
		WCHAR	header[HEADER];
		WCHAR	TitleX[AXIS_TITLE_LENGHT];
		WCHAR	TitleY[AXIS_TITLE_LENGHT];
	}
	Data_struct;

	//Structure for contur plot

	typedef struct vector2D_t{

		double x;
		double y;
	}
	Vector2D;

	//Structures for 2D graphisc eg. zooming
	typedef struct BORDERS_t{
		double bottom, top, left, right;
	}
	BORDERS;
	//Contur plot struct
	typedef struct contur_t{

		bool	status;				//to plot or not

		int		NxCon, NyCon;			//Number of points to draw
		int		Nlevel;						//Number of levels for given zmin - zmax range
		double	*x;
		double	*y;
		double	*z;
		double	zmin, zstep, zmax;

		Vector2D CellVertices[5];

		WCHAR	Title[HEADER];
		WCHAR	TitleX[AXIS_TITLE_LENGHT];
		WCHAR	TitleY[AXIS_TITLE_LENGHT];

		bool AllocateMemory(){
			//Just check it in case.
			FreeMemory();
			// Allocate memory for the arrays neccesary for the Fourier map 
			x = new double[NxCon + 1];
			y = new double[NyCon + 1];
			z = new double[NxCon*NyCon + 1];

			return true;
		}

		bool FreeMemory(){
			// Allocate memory for the arrays neccesary for the Fourier map 
			if (x) delete[] x;
			if (y) delete[] y;
			if (z) delete[] z;
			return true;
		}
	}
	CONTUR;

	//variable:
	HDC hdcBack;						//Back buffer for painting

	int NrOfPoints;						//MAX_POINTS
	Data_struct data;					//Structure which storing the experimental points
	Data_struct fit[MAX_FIT_CURVES];	//Structure which storing the calculated fit points for different curves
	CONTUR Contur;						//Structure which holds all the important information for contur plot
	int SELECT_GRAPH;					//0 - the rod graph; 1 - in plane graph; 2 - contur plot, 4 -el. density

	//flags
	bool lin_log;						//linear/logaritmic graph
	bool data_plot;
	bool errors_plot;

	HWND CreateAppWindow(const WNDCLASSEX &wcl, TCHAR *pszTitle, int left = 0, int top = 0, int halfScreenWidth = 0, int halfScreenHeight = 0);
	HWND WINAPI CreateStatusBar(HWND hwnd, HINSTANCE hInst);
	LRESULT CALLBACK WinProc(HWND hWnd, HINSTANCE hinst, UINT msg, WPARAM wParam, LPARAM lParam);
	void AutomaticSetTheScale();

	//void CalculateContur();


	private:

		//variable:
		POINT pt;
		HFONT MyFont, MyFont_Large, MyFont_Small;
		HFONT hFont;						//Font for toolbar
		HWND hwndTB;						// Handle to the toolbar
		HWND hwndSB;						// Handle to the statusbar
		HWND hRebar;

		//Flags
		bool arrow_flag;					//Arrow selected from toolbar
		bool zoom_flag;						//we want to zoom true we want to pickup position false
		bool zoom_track_stop, zoom_done;
		bool line_points;					//add line to the points
		bool scale_move;					//scale move by mouse
		bool grid;							//grid on/off

		double point_scale;					//For SELECT_GRAPH=1 chang the point size by mouse wheel
		RECT WindowsRect;					//Client rectangle
		RECT zoom_frame;
		BORDERS XYMinMax;
		BORDERS storage_XYMinMax;			//to store parameters after zoom out

		//Function prototypes
		HWND WINAPI CreateToolbar(HWND hwnd, HINSTANCE hInst, int Width);


		void ExtractMousePosition(POINT *lpt, HWND hwnd, LPARAM param);
		void ConvertAtoX(BORDERS MinMax, double A_x, double A_y, RECT ScreenSize, int *X, int *Y);
		void ConvertXtoA(BORDERS MinMax, double *A_x, double *A_y, RECT ScreenSize, int X, int Y);
		void ConturPlot(HWND hwnd, HDC hDc, POINT Pt, RECT rect);

		void Initialize_Graphisc(HWND hWnd);
		void ResetScale();
		void Initialize_borders(void);
		void InvalidateRectangle(HWND, RECT*);
		void Shutdown();
		void DrawStatusBar(HWND hwnd, POINT Pt, RECT rect);

		void SaveBitmapFunction(HWND hwnd);
		bool SaveTextXYZFile(char *FileName);

		int Intersec(int ix, int iy, double, double *x, double *y);
		int PointTest(int ix, int iy, int delx, int dely, double midle, double *x, double *y);


		/*
		//contur map bondary fill
		int *Mask = NULL; //two dimensional array represented as a [x+y*width] array


		struct Pixel2
		{
		int x;
		int y;

		Pixel2 *Next;
		};

		Pixel2 *Entry = NULL;
		Pixel2 *Start = NULL;
		Pixel2 *Last = NULL;

		void SetNewMaskSize();
		void insert_pixel(const int x, const int y);
		void BoundaryFill(const int _x, const int _y, int Width, int Height, int fill_color, int boundary_color);
		void LineContur(POINT Pos1, POINT Pos2, int Color);
		void FillConturIntoMask();

		void FilledConturPlot(HWND hwnd, HDC hdcBack, POINT Pt, RECT rect);
		*/

		HBRUSH  Bacground_hBru;
		//MAX_PEN must be higher than MAX_FIT_CURVES +10 
		HBRUSH hBru[MAX_PEN];
		HPEN hPen[MAX_PEN];
		COLORREF color[MAX_PEN];


};

//INT_PTR CALLBACK Settings2D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);









