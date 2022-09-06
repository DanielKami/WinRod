#pragma once
#include "definitions.h"

#define			MAX_POINTS					MAXTHEO
#define			MAX_FIT_CURVES				4				//Maximum number of theoretical calculated data used Plot functions	0-bulk, 1-surf, 2- both, 3-phase
#define			MAX_PEN						10 + MAX_FIT_CURVES	//number of pens used for drawing 

#define			AXIS_TITLE_LENGHT			100

//The frames of 2D graphs
#define			RamkaX_left_2d				95
#define			RamkaX_right_2d				10 
#define			RamkaY_bottom_2d			60 
#define			RamkaY_top_2d				50 
#define     	MARYGIN						0.1						//Extension of the scale

//The frames of 2D in-plane graphs
#define			RamkaX_left_in_plane		50
#define			RamkaX_right_in_plane		10 
#define			RamkaY_bottom_in_plane		30 
#define			RamkaY_top_in_plane			50 


//Additional variable for scale
#define			NR_OF_POINTS_ON_SCALE_X		50
#define			NR_OF_POINTS_ON_SCALE_Y		60

//Defined valuse for proper mouse behaviour during action for 2D graphs
#define			SHIFT_FACTOR				0.003				//factor to set the shift of scale acoording to the mouse position
#define			MOUSE_WHEEL_DOLLY_SPEED		0.002

//Dimension of status window
#define			STATUS_WINDOW_HIGH			20			//The hight of status window

//Open
#define			MAX_OPEN					32			//Number of maximum stored curves


//windows type 0 - the rod graph; 1 - in plane graph; 2 - contur plot, 4 -el. density, 6 - dispersion
#define WINDOW_ROD							0
#define WINDOW_IN_PLANE						1
#define WINDOW_CONTUR						2
#define WINDOW_DENSITY						4
#define WINDOW_DISPERSION					6
#define WINDOW_MULTIPLOT					9

//Scale fit types
#define SCALE_TO_CURVE_AND_POINTS			0
#define SCALE_TO_POINTS						1
#define SCALE_TO_CURVE						2

class D2_helper
{
public:
	D2_helper();
	~D2_helper();

	//Structure for contur plot
	typedef struct colors_t{
		COLORREF framecolor;
		COLORREF background; //Background color of graphs
		COLORREF text; //Background color of graphs
		int BMP_Width = 800;
		int BMP_Height = 600;
	}
	Colors;

	//Structure for contur plot
	typedef struct vector2D_t{
		double x;
		double y;
	}
	Vector2D;

	//Contur plot struct
	typedef struct contur_t{
		bool	status;				//to plot or not
		int	NxCon, NyCon;		//Number of points to draw
		int	Nlevel;				//Number of levels for given zmin - zmax range
		double	*x;
		double	*y;
		double	*z;
		double	zmin, zstep, zmax;

		Vector2D CellVertices[5];

		WCHAR	Title[HEADER];
		WCHAR	TitleX[AXIS_TITLE_LENGHT];
		WCHAR	TitleY[AXIS_TITLE_LENGHT];

		bool AllocateMemory()
		{
			//Just check it in case.
			FreeMemory();
			// Allocate memory for the arrays neccesary for the Fourier map 
			x = new double[NxCon + 1];
			y = new double[NyCon + 1];
			z = new double[NxCon*NyCon + 1];
			return true;
		}

		bool FreeMemory()
		{
			// Allocate memory for the arrays neccesary for the Fourier map 
			if (x) delete[] x;
			if (y) delete[] y;
			if (z) delete[] z;
			return true;
		}
	}
	CONTUR;

	//Data structure from which points and model are draw
	typedef struct tag_t
	{
		bool readed;					//a flag
		bool status;					//to plot or not
		int  NrOfPoints;				//Number of points to draw
		double x[MAX_POINTS + 1];		//coordynate x
		double y[MAX_POINTS + 1];
		double z[MAX_POINTS + 1];
		double sigma[MAX_POINTS + 1];	//sigma of pint
		double LBragg;					//LBragg position for ROD
		double	RLAT[6];				//Lattice parameter to draw rec. space

		WCHAR	Title[HEADER];
		WCHAR	header[HEADER];
		WCHAR	TitleX[AXIS_TITLE_LENGHT];
		WCHAR	TitleY[AXIS_TITLE_LENGHT];

		COLORREF color;				//Line color
		COLORREF color_fill;			//Point fill color
		int style;						//Line style
		int thickness;					//Line thickness
	}
	Data_struct;

	//Structures for 2D graphisc eg. zooming
	typedef struct BORDERS_t
	{
		double bottom, top, left, right;
	}
	BORDERS;

	//Structure for multiplot
	typedef struct MULTIPLOT_t
	{
		Data_struct fit[MAX_FIT_CURVES];
		Data_struct data;
	}
	MULTIPLOT;



	//Static functins to operate 2D window
	static INT_PTR CALLBACK Settings2D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK OpenDataPoints(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK OpenFitPoints(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	//Functions for add data and add curves windows
	static void FillProperitiesList(HWND  hItemList, Data_struct*);
	static LRESULT ProcessCustomDraw(LPARAM lParam, int, COLORREF*);
	static void OnClickListColor(HWND hDlg, HWND hWndEditBox, NMHDR* pNMHDR, int *nItem, int *nSubItem, int max_SubItem, COLORREF *tmpRGBbackground, COLORREF **color);
	static int GetItemInt(HWND hWnd, int nItem, int nSubItem);
	static void GetItemText(HWND hWnd, WCHAR *Wstr, int nLen, int nItem, int nSubItem);
	static void OnClickListText(HWND hDlg, HWND hWndEditBox, NMHDR* pNMHDR, int *nItem, int *nSubItem, int SkipColumnLow, int SkipColumnUp);
	static void SetCell(HWND hWnd1, WCHAR* szString, int nRow, int nCol);
	static double GetDlgItemReal(HWND hDlg, int nIDD);
	static void SetCell(HWND hWnd1, double value, int nRow, int nCol, bool Int);
	static void ColorSet(HWND, int, COLORREF *color);

};

