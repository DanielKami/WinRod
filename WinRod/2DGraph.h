
//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of the 2D graphisc library. The library is also responsible for window creation and window signals interpretation. 
//
// Writen by Daniel Kaminski 
//
// 01/01/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#include "2DGraphHelper.h"




//----------------------------------------------------------------------------------------------------------------------
//The main 2D graphisc class
//----------------------------------------------------------------------------------------------------------------------
class Graph2D : public  D2_helper
{


public:
	Graph2D(int type_of_graph = WINDOW_ROD)
	{
		SELECT_GRAPH = type_of_graph;
		rgbColors.BMP_Width = 800;
		rgbColors.BMP_Height = 600;
		Multiplot = NULL;
		nrOpenFit = 0;
		nrOpenDat = 0;
		ClientWindowDataPoint = -1;
	}

	//destructor
	~Graph2D(){
				Shutdown();
				Contur.FreeMemory();
				DeleteObject(hFont);
				if (Multiplot != NULL)
				delete[] Multiplot;

				
//				delete[] Mask;
	};


  
	 Colors rgbColors;

	 //Multiplot variables
	 MULTIPLOT *Multiplot;
	 int NumberOfRods;
	 int N_columns;

	//Windows variable:
	HDC hdc;
	HDC hdcBack;						//Back buffer for painting
	HDC hdcBack2;						//Back buffer for painting multiplot

	Data_struct data;					//Structure which storing the experimental points
	Data_struct fit[MAX_FIT_CURVES+1];	//Structure which storing the calculated fit points for different curves
	CONTUR Contur;						//Structure which holds all the important information for contur plot
	int SELECT_GRAPH;					//0 - the rod graph; 1 - in plane graph; 2 - contur plot, 4 -el. density, 6 - dispersion, 30 multiplot
	int ScaleType;						//Typ of scale calculations. 0- fit the scale to curves and data, 1-fit the scale only to data

	//flags
	bool lin_log;						//linear/logaritmic graph
	bool data_plot;						//Plot data
	bool errors_plot;					//plot errors
	bool STRUCFAC;                      //structure factor are f or f*f


	HWND CreateAppWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect);
	LRESULT CALLBACK WinProc(HWND hWnd, HINSTANCE hinst, UINT msg, WPARAM wParam, LPARAM lParam);
	void AutomaticSetTheScale(bool ScaleFromData = false);
	bool InitializeMultiDraw();
	void UpdateStatusBar();
	void ResetOpen();
	void RefreshWindow();
	HWND hwndWindow;						// Handle to the statusbar
	//void CalculateContur();
	
 private:

	//variable:
	POINT pt;

	HFONT hFont;						//Font for toolbar
	HWND hwndTB;						// Handle to the toolbar
	HWND hwndTBVer;						// Handle to the toolbar
	HWND hwndSB;						// Handle to the statusbar

	HWND hRebar;
	HMENU hContextMenu;
	HMENU hPositionMenu;
	HMENU hSubMenu;

	//Flags
	bool arrow_flag;					//Arrow selected from toolbar
	bool zoom_flag;						//we want to zoom true we want to pickup position false
	bool zoom_track_stop, zoom_done;
	bool line_points;					//add line to the points
	bool scale_move;					//scale move by mouse
	bool grid;							//grid on/off
	bool show_LBragg;					//Shows arrow indicating LBragg

	double point_scale;					//For SELECT_GRAPH=In-plane chang the point size by mouse wheel
	RECT WindowsRect;					//Client rectangle
	RECT zoom_frame;
	
	GRAPHSETTINGS storage_XYMinMax;		//to store parameters after zoom out
	
	//The frames can change depending on the mode thus these values have to be adapted acording to needs
	int	RamkaX_left;
	int RamkaX_right;
	int RamkaY_bottom;
	int RamkaY_top;
	int FrameCorrection;

	//Function prototypes
	HWND WINAPI CreateToolbar (HWND hwnd, HINSTANCE hInst, int Width);
	HWND WINAPI CreateToolbarVertical(HWND hwnd, HINSTANCE hInst, int Width);
	HWND WINAPI CreateStatusBar(HWND hwnd, HINSTANCE hInst);


	void ExtractMousePosition(POINT *lpt, HWND hwnd, LPARAM param);
	void ConvertAtoX(GRAPHSETTINGS MinMax, double A_x, double A_y, RECT ScreenSize, int *X, int *Y);
	void ConvertXtoA(GRAPHSETTINGS MinMax, double *A_x, double *A_y, RECT ScreenSize, int X, int Y);
	void Draw(HWND hwnd, HDC hDc, POINT Pt, RECT rect);
	void DrawInPlane(HWND hwnd, HDC hDc, POINT Pt, RECT rect);
	void ConturPlot(HWND hwnd, HDC hDc, POINT Pt, RECT rect);
	void MultiDraw(HWND, HDC, HDC, RECT);
	void DrawSymbol(POINT* line, int j);
	void DrawLine(POINT *Point, int, bool reset=false);
	bool LineStyle(int, int, double count);
	double SetStepScale(GRAPHSETTINGS XYMinMax, RECT rect, double *tmp_left, double *tmp_right);
	void ButtonsState();

	void Initialize_Graphisc(HWND hWnd);
	void ResetScale();
	void Initialize_borders(void);
	void InvalidateRectangle(HWND, RECT*);
	void Shutdown();
	void DrawStatusBar(HWND hwnd, POINT Pt, RECT rect);
	void SetYrangeFromData(double, double);
	void SetYrangeFromOpenData(double marygin_up, double marygin_bottom);
	void TestBorderY(double marygin_up, double marygin_bottom);
	void SetXrangeFromData();
	void SetXrangeFromOpenData();
	void SetYrangeFromFit(double, double);
	void SetYrangeFromOpenFit(double marygin_up, double marygin_bottom);
	void SetXrangeFromFit(double);
	void SetXrangeFromOpenFit(double marygin);
	void RoundLeftBorder();
	void RoundRightBorder();
	void ConvertLogScaleToLinScale();
	void SetFrames();

	void SaveBitmap(HANDLE	hFile, HBITMAP hBitmap, HWND hwnd, HDC hdc);
	void Save2DFunction(HWND hwnd);
	bool SaveTextDataAndFFile(char *FileName);
	bool SaveTextXYZFile(char *FileName);
	bool SaveTextDatFile(char *FileName);
	void CopyToClipobard(HWND hwnd);
	void PrintFunction(HWND hwnd);
	HDC  GetPrinterDC(HWND Hwnd);

	int Intersec(int ix, int iy, double, double *x, double *y);
	int PointTest( int ix, int iy, int delx, int dely, double midle, double *x, double *y );

	//Storage curves and data
	Data_struct Opendat[MAX_OPEN+1];	//Structure which storing the calculated fit points for different curves
	Data_struct Openfit[MAX_OPEN+1];	//Structure which storing the calculated fit points for different curves
	Data_struct TempOpenData;		//Structure holding the temporary data readed from file

	int  OpenFileFunction(HWND hwnd);
	bool Read_Dat(char *line, int nr);
	bool ReadData(LPSTR TStr, int DataType, DWORD size);
	int nrOpenFit;
	int nrOpenDat;

	void CopyColorsToOpenBruPen();
	void CopyFitToCurve();
	void CopyDataToOpenDat();

	//Context menu
	void InitializeContentMenu(HINSTANCE hinst, HWND hwnd);
	void ContentMenuMsgTranslate(HINSTANCE hinst, HWND hwnd, int msg);
	int FindDataNumber(HWND hwnd);
	int ClientWindowDataPoint;
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

	HBRUSH  Background_hBru;
	//MAX_PEN must be higher than MAX_FIT_CURVES +10 
	HBRUSH hBru[MAX_PEN];  
	HPEN hPen[MAX_PEN];
	COLORREF color[MAX_PEN];

	HPEN hPenOpen[MAX_OPEN];
	HPEN hPenOpenDat[MAX_OPEN];
};











