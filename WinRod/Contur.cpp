/***************************************************************************/
/*      include files                                                      */
/***************************************************************************/
#include <stdafx.h>

#include <windows.h>
//#include <stdio.h>
#include <commdlg.h>
#include <math.h>
#include <Windowsx.h>
#include <commctrl.h>
#include "resource.h"
#include "definitions.h"
#include "Contur.h"
#include "toolbar.h"
#include "2DGraph.h"
#include "ReadFile.h"
//For pictures save
#include "Save.h"


ConturClass::ConturClass()
{
}


ConturClass::~ConturClass()
{
	Shutdown();
	Contur.FreeMemory();
	DeleteObject(hFont);
	//				delete[] Mask;
}





static int BMP_Width = 800, BMP_Height = 600;  //must be here to avoid problems with class
COLORREF rgbBackground;   //Background color





//-----------------------------------------------------------------------------
//Create the window 
// 21.01.2013 modified by Daniel Kamisnki
//-----------------------------------------------------------------------------
HWND ConturClass::CreateAppWindow(const WNDCLASSEX &wcl, TCHAR *pszTitle, int left, int top, int halfScreenWidth, int halfScreenHeight)
{
	// Create a window that is centered on the desktop. It's exactly 1/4 the
	// size of the desktop.

	DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;//|WS_EX_COMPOSITED

	DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
		WS_CLIPSIBLINGS | WS_SIZEBOX;// // WS_CLIPCHILDREN


	HWND hWnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
		wndStyle, 0, 0, 0, 0, 0, 0, wcl.hInstance, 0);

	if (hWnd)
	{
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		if (halfScreenWidth == 0) halfScreenWidth = screenWidth / 3;
		if (halfScreenHeight == 0) halfScreenHeight = screenHeight / 3;
		if (left == 0) left = (screenWidth - halfScreenWidth) / 2;
		if (top == 0) top = (screenHeight - halfScreenHeight) / 2;
		RECT rc = { 0 };

		SetRect(&rc, left, top, left + halfScreenWidth, top + halfScreenHeight);
		AdjustWindowRectEx(&rc, wndStyle, FALSE, wndExStyle);
		MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

	}

	return hWnd;
}


//-----------------------------------------------------------------------------
// Create Status Bar.
//PURPOSE: 
//  Registers the StatusBar control class and creates a statusbar.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
HWND WINAPI ConturClass::CreateStatusBar(HWND hwnd, HINSTANCE hInst){
	//Initiate controls
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_BAR_CLASSES; // toolbary, statusbary i tooltipy
	InitCommonControlsEx(&icc);

	HWND hWndSB = CreateWindowEx(0, STATUSCLASSNAME, NULL, SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE | SBT_TOOLTIPS
		, 0, 0, 0, 0, hwnd, (HMENU)200, hInst, NULL);

	if (!hWndSB){
		MessageBox(hwnd, TEXT("Can't open status bar window"), NULL, MB_ICONERROR);
		return NULL;
	}
	ShowWindow(hWndSB, SW_SHOW);
	UpdateWindow(hWndSB);

	return hWndSB;
}





//-----------------------------------------------------------------------------
// CreateToolbar.
//PURPOSE: 
//  Registers the TOOLBAR control class and creates a toolbar.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
HWND WINAPI ConturClass::CreateToolbar(HWND hwnd, HINSTANCE hInst, int Width)
{

	DWORD dwStyle;              // Style of the toolbar

	INITCOMMONCONTROLSEX iccex; // INITCOMMONCONTROLSEX structure
	iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&iccex);

	if (!hFont) hFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tachoma");

	dwStyle = WS_CHILD | RBS_AUTOSIZE | RBS_BANDBORDERS | WS_VISIBLE | RBS_TOOLTIPS | WS_CLIPSIBLINGS | WS_BORDER;//  | RBS_VARHEIGHT;


	if (!(hRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, dwStyle, 0, 0, 0, 0, hwnd, NULL, hInst, NULL)))
	{
		MessageBox(hwnd, TEXT("Can't create rebar for 2D window"), NULL, MB_ICONERROR);
		return NULL;
	}

	//Add rebar
	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof(REBARBANDINFO);
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RB_BEGINDRAG;
	rbbi.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS | RBBS_BREAK;//

	//  Create the toolbar control.
	//dwStyle = WS_VISIBLE | WS_CHILD |  CCS_NOPARENTALIGN;
	dwStyle = TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | TBSTYLE_FLAT;//

	if (!(hwndTB = CreateToolbarEx(hRebar, dwStyle, (UINT)ID_TOOLBAR, NUMIMAGES2D, hInst, IDB_TOOLBAR, tbButton, sizeof (tbButton) / sizeof (TBBUTTON),
		BUTTONWIDTH, BUTTONHEIGHT, IMAGEWIDTH, IMAGEHEIGHT, sizeof (TBBUTTON))))
	{
		MessageBox(hwnd, TEXT("Can't create toolbar for 2D window"), NULL, MB_ICONERROR);
		return NULL;
	}

	//Add tolbar to rebar
	rbbi.lpText = L"";
	rbbi.hwndChild = hwndTB;
	rbbi.cxMinChild = 400;
	rbbi.cyMinChild = MENU_HEIGHT + 2;
	rbbi.cx = 200;
	SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)& rbbi);

	return hRebar;
}

//-----------------------------------------------------------------------------
// Process all events related to 2D window.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
LRESULT CALLBACK ConturClass::WinProc(HWND hWnd, HINSTANCE hinst, UINT msg, WPARAM wParam, LPARAM lParam)
{

	static HDC hdc;
	static int MenuHeight;
	TCHAR Mesage[200];
	double variable_x, variable_y;

	static POINT pt_old;
	static bool sizing;


	switch (msg)
	{
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case TTN_GETDISPINFO:
			Tooltip2D(lParam);
			break;
		}
		break;
	case WM_CREATE:
	{

					  SELECT_GRAPH = 0; //set the standard graph
					  grid = true;
					  if ((SendMessage(hwndTB, TB_GETSTATE, IDM_LINE, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) line_points = true;
					  else line_points = false;

					  Initialize_Graphisc(hWnd);

					  //if neccesary
					  if (data.NrOfPoints == 0 && fit[0].NrOfPoints == 0)
						  Initialize_borders();
					  else
						  ResetScale();
					  AutomaticSetTheScale();

					  hdc = GetDC(hWnd);
					  hdcBack = CreateCompatibleDC(hdc);
					  GetClientRect(hWnd, &WindowsRect);

					  // Get the horizontal and vertical screen sizes in pixel
					  RECT desktop;
					  // Get a handle to the desktop window
					  const HWND hDesktop = GetDesktopWindow();
					  // Get the size of screen to the variable desktop
					  GetWindowRect(hDesktop, &desktop);

					  HBITMAP hbmBack = CreateCompatibleBitmap(hdc, desktop.right, desktop.bottom);
					  SelectObject(hdcBack, (HBITMAP)hbmBack);
					  PatBlt(hdcBack, 0, 0, desktop.right, desktop.bottom, WHITENESS); //PATINVERT
					  DeleteObject(hbmBack);

					  // Creates the toolbar.
					  CreateToolbar(hWnd, hinst, desktop.right);
					  hwndSB = CreateStatusBar(hWnd, hinst);

					  int iStatusBarWidths[] = { 250, 350, 450, 550, -1 };
					  SendMessage(hwndSB, SB_SETPARTS, 5, (LPARAM)iStatusBarWidths);
					  SendMessage(hwndSB, SB_SETTIPTEXT, 0, (LPARAM) "Data file name.");
					  SendMessage(hwndSB, SB_SETTIPTEXT, 2, (LPARAM) "x position in the drawing");
					  SendMessage(hwndSB, SB_SETTIPTEXT, 3, (LPARAM) "y position in the drawing");


					  //			SetNewMaskSize();
					  //swprintf(Mesage, L"%d",SendMessage( hwndTB, TB_GETSTATE, IDM_SUR, 0));
					  //MessageBox(hWnd, Mesage, NULL, MB_ICONERROR);
	}
		//		break; //Continue with the button settings

	case WM_ACTIVATE:
		arrow_flag = true;

		if (lin_log == true){
			SendMessage(hwndTB, TB_SETSTATE, IDM_LIN, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_LOG, MAKELONG(TBSTATE_ENABLED, 0));
		}
		else{
			SendMessage(hwndTB, TB_SETSTATE, IDM_LIN, MAKELONG(TBSTATE_ENABLED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_LOG, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED, 0));
		}

		if (zoom_flag == true){
			SendMessage(hwndTB, TB_SETSTATE, IDM_ZOOM, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_SCALE_MOVE, MAKELONG(TBSTATE_ENABLED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_ARROW, MAKELONG(TBSTATE_ENABLED, 0));
			scale_move = false;
			arrow_flag = false;
		}
		if (scale_move == true){
			SendMessage(hwndTB, TB_SETSTATE, IDM_ZOOM, MAKELONG(TBSTATE_ENABLED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_SCALE_MOVE, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_ARROW, MAKELONG(TBSTATE_ENABLED, 0));
			zoom_flag = false;
			arrow_flag = false;
		}
		if (arrow_flag == true){
			SendMessage(hwndTB, TB_SETSTATE, IDM_ZOOM, MAKELONG(TBSTATE_ENABLED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_SCALE_MOVE, MAKELONG(TBSTATE_ENABLED, 0));
			SendMessage(hwndTB, TB_SETSTATE, IDM_ARROW, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED, 0));
			zoom_flag = false;
			scale_move = false;

		}
		//What to plot
		if (fit[2].status == true) SendMessage(hwndTB, TB_SETSTATE, IDM_SUR, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_SUR, MAKELONG(TBSTATE_ENABLED, 0));
		if (fit[1].status == true) SendMessage(hwndTB, TB_SETSTATE, IDM_BUL, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_BUL, MAKELONG(TBSTATE_ENABLED, 0));
		if (fit[0].status == true) SendMessage(hwndTB, TB_SETSTATE, IDM_BOT, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_BOT, MAKELONG(TBSTATE_ENABLED, 0));
		if (data_plot == true) SendMessage(hwndTB, TB_SETSTATE, IDM_DATA, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_DATA, MAKELONG(TBSTATE_ENABLED, 0));
		if (errors_plot == true) SendMessage(hwndTB, TB_SETSTATE, IDM_ERROR, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_ERROR, MAKELONG(TBSTATE_ENABLED, 0));
		if (grid == true) SendMessage(hwndTB, TB_SETSTATE, IDM_GRID2D, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_GRID2D, MAKELONG(TBSTATE_ENABLED, 0));


		break;

	case WM_SIZE:
		//GetClientRect (hWnd, &rect);
		GetWindowRect(hRebar, &WindowsRect);
		sizing = true;
		MenuHeight = WindowsRect.bottom - WindowsRect.top + 2;
		SendMessage(hwndSB, WM_SIZE, 0, 0);
		SendMessage(hRebar, WM_SIZE, 0, 0);

		//Contur map Mask size
		//			if(SELECT_GRAPH == 5) CalculateContur();
		break;


	case WM_COMMAND:
		// Parse the menu selections:
		switch (LOWORD(wParam))
		{


		case IDM_FILE_SAVE: //Save the bitmap
		{
								SaveBitmapFunction(hWnd);
		}
			break;

		case IDM_2D_SETTINGS: //Save the bitmap
		{
								  DialogBox(hinst, MAKEINTRESOURCE(IDD_2D_SETTINGS), hWnd, Settings2D);

		}
			break;

		case IDM_ZOOM:
			zoom_flag = true;
			scale_move = false;
			arrow_flag = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_ARROW:
			zoom_flag = false;
			arrow_flag = true;
			scale_move = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_LIN:
			lin_log = true;
			AutomaticSetTheScale();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_LOG:
			lin_log = false;
			ResetScale();
			AutomaticSetTheScale();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_LINE:
			line_points = true;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_POINTS:
			line_points = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_SCALE_MOVE:
			zoom_flag = false;
			scale_move = true;
			arrow_flag = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_GRID2D:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_GRID2D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) grid = true;
			else grid = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_SUR:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_SUR, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) fit[2].status = true;
			else fit[2].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_BUL:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_BUL, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) fit[1].status = true;
			else fit[1].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_BOT:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_BOT, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) fit[0].status = true;
			else fit[0].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_DATA:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_DATA, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) data_plot = true;
			else data_plot = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_ERROR:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_ERROR, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) errors_plot = true;
			else errors_plot = false;
			InvalidateRect(hWnd, 0, 0);
			break;



		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		ps.fErase = 1;
		GetClientRect(hWnd, &WindowsRect);
		hdc = BeginPaint(hWnd, &ps);

		if (sizing){
			FillRect(hdc, &WindowsRect, (HBRUSH)(COLOR_3DFACE));
			sizing = false;
		}

		ConturPlot(hWnd, hdcBack, pt, WindowsRect);
		// FilledConturPlot(hWnd, hdcBack, pt, WindowsRect);

		BitBlt(hdc, 0, MenuHeight, WindowsRect.right, WindowsRect.bottom - MenuHeight - 22, hdcBack, 0, MenuHeight, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		DeleteDC(hdc);
		Shutdown();
		break;

	case WM_MOUSEMOVE:

		ExtractMousePosition(&pt, hWnd, lParam);
		InvalidateRectangle(hWnd, &WindowsRect);
		GetClientRect(hWnd, &WindowsRect);
		DrawStatusBar(hwndSB, pt, WindowsRect);

		switch (wParam){

		case MK_LBUTTON:
			if (zoom_flag){
				zoom_frame.left = pt.x;
				zoom_frame.top = pt.y;
			}

			if (scale_move){
				double shift_scaleX = SHIFT_FACTOR * (XYMinMax.right - XYMinMax.left);
				double shift_scaleY = SHIFT_FACTOR * (XYMinMax.top - XYMinMax.bottom);
				if (pt.y - pt_old.y > 0){ XYMinMax.bottom += shift_scaleY; XYMinMax.top += shift_scaleY; }
				if (pt.y - pt_old.y < 0){ XYMinMax.bottom -= shift_scaleY; XYMinMax.top -= shift_scaleY; }

				if (pt.x - pt_old.x < 0){ XYMinMax.left += shift_scaleX; XYMinMax.right += shift_scaleX; }
				if (pt.x - pt_old.x > 0){ XYMinMax.left -= shift_scaleX; XYMinMax.right -= shift_scaleX; }
				pt_old.y = pt.y;
				pt_old.x = pt.x;
			}
			break;

		case MK_RBUTTON:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		if (zoom_flag){//we do zooming
			zoom_track_stop = true;
			zoom_done = false;
			ExtractMousePosition(&pt, hWnd, lParam);

			zoom_frame.left = zoom_frame.right = pt.x;
			zoom_frame.top = zoom_frame.bottom = pt.y;
		}

		if (scale_move){
			ExtractMousePosition(&pt_old, hWnd, lParam);
		}

		break;

	case WM_LBUTTONUP:

		if (zoom_flag){//we do zooming
			zoom_done = true;

			if (storage_XYMinMax.left>XYMinMax.left)  storage_XYMinMax.left = XYMinMax.left;
			if (storage_XYMinMax.right<XYMinMax.right) storage_XYMinMax.right = XYMinMax.right;
			if (storage_XYMinMax.top<XYMinMax.top) storage_XYMinMax.top = XYMinMax.top;
			if (storage_XYMinMax.bottom>XYMinMax.bottom) storage_XYMinMax.bottom = XYMinMax.bottom;
		}

		if (arrow_flag){
			int coordinateX, coordinateY;
			ExtractMousePosition(&pt, hWnd, lParam);
			GetClientRect(hWnd, &WindowsRect);

			for (int i = 0; i<data.NrOfPoints; i++){
				if (lin_log) ConvertAtoX(XYMinMax, data.x[i], data.y[i], WindowsRect, &coordinateX, &coordinateY);
				else ConvertAtoX(XYMinMax, data.x[i], log10(data.y[i]), WindowsRect, &coordinateX, &coordinateY);
				if (coordinateX < pt.x + 5 && coordinateX > pt.x - 5 && coordinateY < pt.y + 5 && coordinateY > pt.y - 5) {
					if (SELECT_GRAPH == 0) swprintf_s(Mesage, 200, L"x = %4.3f,  y = %4.3f, error = %4.3f", data.x[i], data.y[i], data.sigma[i]);
					if (SELECT_GRAPH == 1) swprintf_s(Mesage, 200, L"x = %4.3f,  y = %4.3f, error = %4.3f", data.x[i], data.y[i], data.sigma[i]);
					//if(SELECT_GRAPH == 2) swprintf_s(Mesage, 200, L"x = %4.3f,  y = %4.3f", data.x[i] , data.y[i]);
					MessageBox(hWnd, Mesage, L"Info", MB_ICONASTERISK);
					break;
				}
			}
		}
		WindowsRect.bottom -= STATUS_WINDOW_HIGH;
		InvalidateRectangle(hWnd, &WindowsRect);

		break;

	case WM_RBUTTONDOWN:
		if (zoom_flag || scale_move || arrow_flag){//we do zooming
			zoom_track_stop = false;
			zoom_done = false;
			if (data.NrOfPoints == 0 && fit[0].NrOfPoints == 0)
				Initialize_borders();
			else
				ResetScale();
			AutomaticSetTheScale();
			InvalidateRectangle(hWnd, &WindowsRect);
		}

		break;

	case WM_RBUTTONUP:
	{
						 if (!zoom_flag){
							 GetClientRect(hWnd, &WindowsRect);
							 ExtractMousePosition(&pt, hWnd, lParam);

							 ConvertXtoA(XYMinMax, &variable_x, &variable_y, WindowsRect, pt.x, pt.y);
							 //Reciprocal_space();
							 InvalidateRectangle(hWnd, &WindowsRect);
						 }
	}
		break;

	case WM_MOUSEWHEEL:
	{
						  double wheelDelta;
						  double save_bottom = 0., save_top = 0.;
						  wheelDelta = MOUSE_WHEEL_DOLLY_SPEED * static_cast<float>(static_cast<int>(wParam) >> 16);

						  if (SELECT_GRAPH == 0 || SELECT_GRAPH == 4){
							  if (lin_log) wheelDelta *= 10.;
							  XYMinMax.bottom -= wheelDelta;
							  XYMinMax.top += wheelDelta;

							  if (XYMinMax.top - XYMinMax.bottom > .0001){
								  save_bottom = XYMinMax.bottom;
								  save_top = XYMinMax.top;
							  }
							  else {
								  XYMinMax.bottom = save_bottom;
								  XYMinMax.top = save_top;
							  }
						  }
						  if (SELECT_GRAPH == 1){
							  point_scale += wheelDelta;
						  }

						  if (SELECT_GRAPH == 2){
							  wheelDelta *= .1;
							  XYMinMax.bottom -= wheelDelta;
							  XYMinMax.top += wheelDelta;
							  XYMinMax.left -= wheelDelta;
							  XYMinMax.right += wheelDelta;
							  if (XYMinMax.top - XYMinMax.bottom > .0001){
								  save_bottom = XYMinMax.bottom;
								  save_top = XYMinMax.top;
							  }
							  else {
								  XYMinMax.bottom = save_bottom;
								  XYMinMax.top = save_top;
							  }
						  }


						  InvalidateRectangle(hWnd, &WindowsRect);
	}
		break;

	case WM_KEYDOWN:
		//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
		//	Keys( wParam, lParam); //The place to implement hot key shortcuts
		//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
		break;
		break;

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Refresh the painting area except toolbar, flickering effect.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::InvalidateRectangle(HWND hwnd, RECT *rect){

	GetClientRect(hwnd, rect);
	int temp = rect->top; //make a copy
	rect->top += MENU_HEIGHT + 3;

	InvalidateRect(hwnd, rect, 1);
	rect->top = temp;
}




//-----------------------------------------------------------------------------
// Set the borders for painting.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::Initialize_borders(void){
	XYMinMax.left = 0;
	XYMinMax.right = 1;
	XYMinMax.top = 10.1;
	XYMinMax.bottom = 1.;
}


//-----------------------------------------------------------------------------
// Extract mouse position.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::ExtractMousePosition(POINT *lpt, HWND hwnd, LPARAM lparam)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	lpt->x = (GET_X_LPARAM(lparam));
	lpt->y = (GET_Y_LPARAM(lparam));
}



//-----------------------------------------------------------------------------
// Point convertion from variable to screen.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::ConvertAtoX(BORDERS MinMax, double A_x, double A_y, RECT ScreenSize, int *X, int *Y){
	// input: A_x, A_y - the coordinates from user
	//output: X, Y - screen cordinates


	//X coordinates
	double  Xmax;//Xmin,

	//corection for scale start
	A_x -= MinMax.left;
	//correction for left right frames 
	Xmax = ScreenSize.right - RamkaX_left - RamkaX_right;
	//Xmin = Xmax * MinMax.left / MinMax.right;
	//*X = (int)( (Xmax * A_x / MinMax.right) * ( Xmax / (Xmax - Xmin)) );
	*X = (int)floor(Xmax * A_x / (MinMax.right - MinMax.left));
	*X += RamkaX_left;


	//Y coordinates
	double  Ymax;//Ymin,
	//corection for scale start
	A_y -= MinMax.bottom;
	Ymax = ScreenSize.bottom - RamkaY_top - RamkaY_bottom;
	//Ymin = Ymax * MinMax.bottom / MinMax.top;
	//*Y = (int)(Ymax- (Ymax * A_y / MinMax.top)*( Ymax / (Ymax - Ymin)) );
	*Y = (int)floor(Ymax - (Ymax * A_y / (MinMax.top - MinMax.bottom)));
	*Y += RamkaY_top;

}

//-----------------------------------------------------------------------------
// Convert position from the screen to variables x, y.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::ConvertXtoA(BORDERS MinMax, double *A_x, double *A_y, RECT ScreenSize, int X, int Y){

	//X coordinates
	double  Xmax;//Xmin,

	//corection for scale start
	X -= RamkaX_left;//

	Xmax = ScreenSize.right - RamkaX_left - RamkaX_right;
	if (Xmax <= 0.0) Xmax = 0.001;
	//Xmin = Xmax * MinMax.left / MinMax.right;

	//*A_x =  X /  (Xmax / (Xmax - Xmin)) * MinMax.right / Xmax;
	*A_x = X *(MinMax.right - MinMax.left) / (Xmax);
	*A_x += MinMax.left;

	//Y coordinates
	double Ymax;// Ymin, 
	Y -= RamkaY_top;
	Ymax = ScreenSize.bottom - RamkaY_top - RamkaY_bottom;
	if (Ymax <= 0.0) Ymax = 0.001;
	//Ymin = Ymax * MinMax.bottom / MinMax.top;

	*A_y = (Ymax - Y) * (MinMax.top - MinMax.bottom) / (Ymax);

	*A_y += MinMax.bottom;
}


//-----------------------------------------------------------------------------
// Initialization.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::Initialize_Graphisc(HWND hWnd){

	point_scale = 1.;
	fit[0].NrOfPoints = fit[1].NrOfPoints = fit[2].NrOfPoints = 0;

	LOGFONT MyFont_str;
	MyFont_str.lfHeight = 15;
	MyFont_str.lfWidth = 0;
	MyFont_str.lfEscapement = 0;
	MyFont_str.lfOrientation = 0;
	MyFont_str.lfWeight = 0;
	MyFont_str.lfItalic = FALSE;
	MyFont_str.lfUnderline = FALSE;
	MyFont_str.lfStrikeOut = 0;
	MyFont_str.lfCharSet = DEFAULT_CHARSET;
	MyFont_str.lfOutPrecision = OUT_DEFAULT_PRECIS;
	MyFont_str.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	//MyFont_str.lfQualit=PROOF_QUALITY ; 
	MyFont_str.lfPitchAndFamily = VARIABLE_PITCH;
	//Create the font
	MyFont = CreateFontIndirect(&MyFont_str);

	MyFont_str.lfHeight = 30;
	MyFont_str.lfWidth = 0;
	MyFont_str.lfEscapement = 0;
	MyFont_str.lfOrientation = 0;
	MyFont_str.lfWeight = 1;
	MyFont_str.lfItalic = FALSE;
	MyFont_str.lfUnderline = FALSE;
	MyFont_str.lfStrikeOut = 0;
	MyFont_str.lfCharSet = DEFAULT_CHARSET;
	MyFont_str.lfOutPrecision = OUT_DEFAULT_PRECIS;
	MyFont_str.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	//MyFont_str.lfQualit=PROOF_QUALITY ; 
	MyFont_str.lfPitchAndFamily = VARIABLE_PITCH;
	//Create the font
	MyFont_Large = CreateFontIndirect(&MyFont_str);

	MyFont_str.lfHeight = 15;
	MyFont_str.lfWidth = 0;
	MyFont_str.lfEscapement = 0;
	MyFont_str.lfOrientation = 0;
	MyFont_str.lfWeight = 1;
	MyFont_str.lfItalic = FALSE;
	MyFont_str.lfUnderline = FALSE;
	MyFont_str.lfStrikeOut = 0;
	MyFont_str.lfCharSet = DEFAULT_CHARSET;
	MyFont_str.lfOutPrecision = OUT_DEFAULT_PRECIS;
	MyFont_str.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	//MyFont_str.lfQualit=PROOF_QUALITY ; 
	MyFont_str.lfPitchAndFamily = VARIABLE_PITCH;
	//Create the font
	MyFont_Small = CreateFontIndirect(&MyFont_str);



	Bacground_hBru = CreateSolidBrush(RGB(255, 255, 255));
	hBru[0] = CreateSolidBrush(RGB(0, 0, 0));
	hBru[1] = CreateSolidBrush(RGB(250, 250, 230));
	hBru[2] = CreateSolidBrush(RGB(200, 0, 0));
	hBru[3] = CreateSolidBrush(RGB(255, 255, 255));
	hBru[4] = CreateSolidBrush(RGB(0, 0, 180));
	hBru[5] = CreateSolidBrush(RGB(100, 100, 250));
	hBru[6] = CreateSolidBrush(RGB(66, 180, 50));
	hBru[7] = CreateSolidBrush(RGB(255, 255, 255));
	hBru[8] = CreateSolidBrush(RGB(200, 200, 200));
	hBru[9] = CreateSolidBrush(RGB(0, 0, 70));

	hPen[0] = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hPen[1] = CreatePen(PS_SOLID, 1, RGB(200, 0, 0));
	hPen[2] = CreatePen(PS_SOLID, 1, RGB(160, 160, 255));
	hPen[3] = CreatePen(PS_SOLID, 1, RGB(0, 200, 0));
	hPen[4] = CreatePen(PS_SOLID, 1, RGB(0, 0, 200));
	hPen[5] = CreatePen(PS_SOLID, 1, RGB(255, 100, 0));
	hPen[6] = CreatePen(PS_SOLID, 2, RGB(180, 180, 250));//contur second line
	hPen[7] = CreatePen(PS_SOLID, 2, RGB(250, 180, 180));//contur second line
	hPen[8] = CreatePen(PS_SOLID, 1, RGB(200, 200, 200)); //grid in 2D
	hPen[9] = CreatePen(PS_SOLID, 2, RGB(50, 0, 50));
	//curve colors


	color[10] = RGB(150, 0, 0);
	color[11] = RGB(0, 0, 0);
	color[12] = RGB(0, 100, 0);
	hPen[10] = CreatePen(PS_SOLID, 1, color[10]);
	hPen[11] = CreatePen(PS_SOLID, 1, color[11]);
	hPen[12] = CreatePen(PS_SOLID, 1, color[12]);
	hBru[10] = CreateSolidBrush(color[10]);
	hBru[11] = CreateSolidBrush(color[11]);
	hBru[12] = CreateSolidBrush(color[12]);

	rgbBackground = RGB(250, 250, 230);
}


//-----------------------------------------------------------------------------
// Shutdown.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::Shutdown()
{
	DeleteObject(MyFont);
	DeleteObject(MyFont_Large);
	DeleteObject(MyFont_Small);

	for (int i = 0; i < MAX_PEN; i++){
		DeleteObject(hPen[i]);
		DeleteObject(hBru[i]);

	}
	DeleteObject(Bacground_hBru);
	DeleteDC(hdcBack);
}

void ConturClass::ResetScale(){
	XYMinMax.left = 1000.;
	XYMinMax.right = -1000.;
	XYMinMax.top = 0.1;
	XYMinMax.bottom = 100.;
}

void ConturClass::AutomaticSetTheScale(void){
	int i;
	double marygin, tmp_marg;
	bool Saved_lin_log;

	//Reset the scale only if nothing is read
	if (data.NrOfPoints == 0 && fit[0].NrOfPoints == 0){
		ResetScale();
	}

	Saved_lin_log = lin_log;
	if (SELECT_GRAPH == 1){
		lin_log = true;
		tmp_marg = 2.;
	}
	else{
		tmp_marg = 1.;
	}




	if (SELECT_GRAPH != 2){
		//Automatic set the y range from data
		if (lin_log){
			for (i = 0; i<data.NrOfPoints; i++){
				marygin = MARYGIN * (XYMinMax.top - XYMinMax.bottom);
				if (data.y[i] > XYMinMax.top)    XYMinMax.top = data.y[i] + tmp_marg * marygin;
				if (data.y[i] < XYMinMax.bottom) XYMinMax.bottom = data.y[i] - marygin;
			}
		}
		else {
			for (i = 0; i<data.NrOfPoints; i++){
				marygin = MARYGIN * (XYMinMax.top - XYMinMax.bottom);
				if (log10(data.y[i]) > XYMinMax.top)    XYMinMax.top = log10(data.y[i]) + marygin / 2;
				if (log10(data.y[i]) < XYMinMax.bottom) XYMinMax.bottom = log10(data.y[i]) - marygin / 100;
				if (XYMinMax.bottom < 0.1) XYMinMax.bottom = 1.;
			}
		}


		//Automatic set the y range from fit
		if (lin_log){
			for (i = 0; i<fit[0].NrOfPoints; i++){
				marygin = MARYGIN * (XYMinMax.top - XYMinMax.bottom);
				if (fit[0].y[i] > XYMinMax.top)    XYMinMax.top = fit[0].y[i] + tmp_marg *  marygin;
				if (fit[0].y[i] < XYMinMax.bottom) XYMinMax.bottom = fit[0].y[i] - marygin;
			}
		}
		else {
			for (i = 0; i<fit[0].NrOfPoints; i++){
				marygin = MARYGIN * (XYMinMax.top - XYMinMax.bottom);
				if (log10(fit[0].y[i]) > XYMinMax.top)    XYMinMax.top = log10(fit[0].y[i]) + marygin;
				if (log10(fit[0].y[i]) < XYMinMax.bottom) XYMinMax.bottom = log10(fit[0].y[i]) - marygin;
				if (XYMinMax.bottom < 0.1) XYMinMax.bottom = 0.1;
			}
		}


		//Automatic set the x range from data
		for (i = 0; i<data.NrOfPoints; i++){
			if (data.x[i] > XYMinMax.right) XYMinMax.right = data.x[i] + MARYGIN * (XYMinMax.right - XYMinMax.left);
			if (data.x[i] < XYMinMax.left)  XYMinMax.left = data.x[i] - MARYGIN * (XYMinMax.right - XYMinMax.left);
		}

		//Automatic set the x range from fit
		for (i = 0; i<fit[0].NrOfPoints; i++){
			if (fit[0].x[i] > XYMinMax.right) XYMinMax.right = fit[0].x[i] + MARYGIN * (XYMinMax.right - XYMinMax.left);
			if (fit[0].x[i] < XYMinMax.left)  XYMinMax.left = fit[0].x[i];//-  MARYGIN * (XYMinMax.right - XYMinMax.left);
		}

		//To avoid problems with plot rescaling in in-plane mode
		lin_log = Saved_lin_log;
	}

	if (SELECT_GRAPH == 2){
		lin_log = true;
		ResetScale();
		//Automatic set the x range from data
		for (i = 0; i<Contur.NxCon; i++){
			marygin = MARYGIN * (XYMinMax.right - XYMinMax.left);
			if (Contur.x[i] > XYMinMax.right) XYMinMax.right = Contur.x[i];
			if (Contur.x[i] < XYMinMax.left)  XYMinMax.left = Contur.x[i];
		}
		for (i = 0; i<Contur.NyCon; i++){
			marygin = MARYGIN * (XYMinMax.top - XYMinMax.bottom);
			if (Contur.y[i] > XYMinMax.top)    XYMinMax.top = Contur.y[i];
			if (Contur.y[i] < XYMinMax.bottom) XYMinMax.bottom = Contur.y[i];
		}


	}


}



void ConturClass::DrawStatusBar(HWND hwnd, POINT Pt, RECT rect){

	TCHAR lpBuffer1[200];
	TCHAR lpBuffer2[200];
	double variable_x, variable_y;		//for position in variable space

	swprintf_s(lpBuffer1, 200, L"%s", data.Title);
	SendMessage(hwndSB, SB_SETTEXT, 0, (LPARAM)lpBuffer1);

	//draw position
	if (SELECT_GRAPH == 0 || SELECT_GRAPH == 4){
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		if (lin_log){
			swprintf_s(lpBuffer1, L"x = %6.3f", variable_x);
			swprintf_s(lpBuffer2, L"y = %6.3f", variable_y);
		}
		else {
			swprintf_s(lpBuffer1, L"x = %3.3f", variable_x);
			swprintf_s(lpBuffer2, L"y = %3.3f", pow(10., variable_y));
		}

	}
	if (SELECT_GRAPH == 1){
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		swprintf_s(lpBuffer1, L"h = %6.3f", variable_x - variable_y * cos(fit[0].RLAT[5]));
		swprintf_s(lpBuffer2, L"k = %6.3f", variable_y);
	}
	if (SELECT_GRAPH == 2){
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		swprintf_s(lpBuffer1, L"x = %6.3f", variable_x);
		swprintf_s(lpBuffer2, L"y = %6.3f", variable_y);
	}


	SendMessage(hwnd, SB_SETTEXT, 2, (LPARAM)lpBuffer1);
	SendMessage(hwnd, SB_SETTEXT, 3, (LPARAM)lpBuffer2);
}



//--------------------------------------------------------------------------------------------------------------------------------
void ConturClass::SaveBitmapFunction(HWND hwnd){
	//--------------------------------------------------------------------------------------------------------------------------------

	HANDLE			hFile;
	TCHAR szFile[MAX_PATH] = TEXT("\0");

	DWORD  dwsize = 0;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("*.bmp\0*.bmp\0*.txt\0*.txt\0");
	ofn.lpstrTitle = TEXT("Save File");
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = TEXT("bmp");
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;



	if (GetSaveFileName(&ofn)){
		hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			MessageBox(hwnd, TEXT("I can't create the file!"), NULL, MB_ICONERROR);
			CloseHandle(hFile);
			return;
		}
	}
	else return;

	if (ofn.nFilterIndex == 1){
		RECT rect; POINT pt;
		rect.bottom = BMP_Height; rect.right = BMP_Width;
		pt.x = 0; pt.y = 0;
		///HDC hdcsave;
		HBITMAP hbm = CreateCompatibleBitmap(hdcBack, rect.right, rect.bottom);
		SelectObject(hdcBack, (HBITMAP)hbm);

		ConturPlot(hwnd, hdcBack, pt, rect);
		SaveBitmap(hFile, hbm, hwnd, hdcBack);

		if (hbm)  DeleteObject(hbm);
		//if(hdcsave) ReleaseDC(NULL,hdcsave);
	}

	CloseHandle(hFile);

	if (ofn.nFilterIndex == 2){
		char str[MAX_PATH];
		TtoA(str, ofn.lpstrFile, sizeof(str));
		SaveTextXYZFile(str);

	}


	/*
	if(ofn.nFilterIndex ==2){
	RECT rect; POINT pt;
	rect.bottom = BMP_Height; rect.right = BMP_Width;
	pt.x = 0; pt.y = 0;

	HBITMAP hbm = CreateCompatibleBitmap(hdcBack , rect.right, rect.bottom);
	SelectObject(hdcBack, (HBITMAP)hbm);

	if(SELECT_GRAPH == 0) Draw(hwnd, hdcBack, pt, rect);
	if(SELECT_GRAPH == 1) DrawInPlane(hwnd, hdcBack, pt, rect);
	if(SELECT_GRAPH == 2) ConturPlot(hwnd, hdcBack, pt, rect);
	SaveBitmap(hFile, hbm, hwnd, ofn.nFilterIndex);
	DeleteObject(hbm);
	}
	*/


}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the xyz file with atom coordinates
//Function created by Daniel Kaminski
//27.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool ConturClass::SaveTextXYZFile(char *FileName){

	FILE*   fF = NULL;
	char str[MAX_PATH] = {};
	int i, j;

	errno_t err = fopen_s(&fF, FileName, "wt");
	if (err != NULL){
		sprintf_s(str, COMMLENGHT, "%s", FileName);
		MessageBox(NULL, L"Unable to create *.txt file", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	/* Save parameters for rod and in plane data */
	if (SELECT_GRAPH == 0 || SELECT_GRAPH == 1){
		fprintf(fF, "fit x y \n");
		for (i = 0; i < fit[0].NrOfPoints; i++){
			fprintf(fF, "%8.4f %8.4f\n", fit[0].x[i], fit[0].y[i]);
		}
	}

	/* Save parameters for contur plot */
	if (SELECT_GRAPH == 2){
		fprintf(fF, "fit x y \n");
		for (i = 0; i < Contur.NxCon; i++){
			for (j = 0; j<Contur.NyCon; j++){
				fprintf(fF, "%8.4f %8.4f %8.4f\n", Contur.x[i], Contur.y[j], Contur.z[Contur.NxCon*j + i]);
			}
		}
	}


	/* Save parameters for electron density profile */
	if (SELECT_GRAPH == 4){
		fprintf(fF, "   z  f1(z) f2(z) f3(z)  f4(z) .....\n");
		for (i = 0; i < fit[0].NrOfPoints; i++){
			fprintf(fF, "%10.2f  ", fit[0].x[i]);
			for (j = 0; j<MAX_FIT_CURVES; j++){
				fprintf(fF, "%10.2f  ", fit[j].y[i]);
			}
			fprintf(fF, "\n");
		}
	}


	if (fF)      fclose(fF);

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Settings2D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	static HWND PictureBox;
	static HBRUSH hBrBox;
	static COLORREF tmpRGBbackground;

	switch (message){
	case WM_INITDIALOG:
	{
						  PictureBox = GetDlgItem(hDlg, IDC_COLOR);
						  tmpRGBbackground = rgbBackground;

						  SetDlgItemInt(hDlg, IDC_EDIT1, BMP_Width, 0);
						  SetDlgItemInt(hDlg, IDC_EDIT2, BMP_Height, 0);
	}
		break;


	case WM_CTLCOLORSTATIC:
	{
							  HWND hCtl = (HWND)lParam;

							  if (hCtl == PictureBox){
								  hBrBox = CreateSolidBrush(tmpRGBbackground);;
								  return (LONG)hBrBox;
							  }
	}
		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK){
			int tmpi;
			tmpi = GetDlgItemInt(hDlg, IDC_EDIT1, 0, 0);
			if (tmpi >= 0 && tmpi<10000) BMP_Width = tmpi;
			else MessageBox(hDlg, L"Width out of range should be lower than 10000", NULL, MB_OK);

			tmpi = GetDlgItemInt(hDlg, IDC_EDIT2, 0, 0);
			if (tmpi >= 0 && tmpi<10000) BMP_Height = tmpi;
			else MessageBox(hDlg, L"Height out of range should be lower than 10000", NULL, MB_OK);

			rgbBackground = tmpRGBbackground;
			DeleteObject(hBrBox);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL){
			DeleteObject(hBrBox);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == ID_BACKGROUND){

			CHOOSECOLOR cc;                 // common dialog box structure 
			COLORREF BackgroundColor[16]; // array of custom colors
			// Initialize CHOOSECOLOR 
			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)BackgroundColor;
			cc.rgbResult = rgbBackground;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE){
				tmpRGBbackground = cc.rgbResult;
				InvalidateRect(PictureBox, 0, 0);
			}
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



//-----------------------------------------------------------------------------
// The main paint function for 2D window.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::ConturPlot(HWND hwnd, HDC hdcBack, POINT Pt, RECT rect){


	POINT line[2];

	int coordinateX, coordinateY;		//for position on the screen
	double variable_x, variable_y;		//for position in variable space
	double step_scale;					//Step of the scales x and y
	BORDERS temp_zoom;					//for selection of the zoom area

	RECT rect2;
	TCHAR plot_text[HEADER];

	//SetBkColor(hdcBack,RGB(255,255,255));
	SetBkMode(hdcBack, TRANSPARENT);
	FillRect(hdcBack, &rect, hBru[7]);


	//safety
	if (fabs(XYMinMax.left - XYMinMax.right)<.000001) Initialize_borders();
	if (fabs(XYMinMax.top - XYMinMax.bottom)<.000001) Initialize_borders();


	//--------------------------------------------------------------------------------------------------
	//fill plot area
	//--------------------------------------------------------------------------------------------------
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.left = coordinateX; rect2.top = coordinateY;
	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	rect2.bottom = coordinateY; rect2.right = coordinateX;

	DeleteObject(SelectObject(hdcBack, hBru[1]));
	hBru[1] = CreateSolidBrush(rgbBackground);
	FillRect(hdcBack, &rect2, hBru[1]);

	//--------------------------------------------------------------------------------------------------
	//zoom draw the rectangular
	//--------------------------------------------------------------------------------------------------
	if (fabs(XYMinMax.top - XYMinMax.bottom) < 0.0000001) XYMinMax.bottom += 0.0000001;
	if (fabs(XYMinMax.right - XYMinMax.left) < 0.0000001) XYMinMax.right += 0.0000001;


	if (zoom_frame.left == zoom_frame.right)  zoom_frame.right = zoom_frame.left + 1; //safety
	if (zoom_frame.top == zoom_frame.bottom) zoom_frame.top = zoom_frame.bottom + 1;
	if (zoom_track_stop && !zoom_done){
		SelectObject(hdcBack, hPen[4]);
		Rectangle(hdcBack, zoom_frame.left + 1, zoom_frame.top + 1, zoom_frame.right - 1, zoom_frame.bottom - 1);
		FillRect(hdcBack, &zoom_frame, hBru[5]);
	}

	if (!zoom_track_stop){  //we are not in zooming action
	}
	else {
		if (zoom_done){
			BORDERS temp_XYMinMax;



			temp_XYMinMax.bottom = XYMinMax.bottom;
			temp_XYMinMax.top = XYMinMax.top;
			temp_XYMinMax.left = XYMinMax.left;
			temp_XYMinMax.right = XYMinMax.right;

			if (zoom_frame.top - zoom_frame.bottom<0){
				temp_zoom.bottom = zoom_frame.top;
				temp_zoom.top = zoom_frame.bottom;
			}
			else{
				temp_zoom.bottom = zoom_frame.bottom;
				temp_zoom.top = zoom_frame.top;
			}

			if (zoom_frame.right < zoom_frame.left){
				temp_zoom.left = zoom_frame.right;
				temp_zoom.right = zoom_frame.left;
			}
			else{
				temp_zoom.left = zoom_frame.left;
				temp_zoom.right = zoom_frame.right;
			}

			ConvertXtoA(temp_XYMinMax, &variable_x, &variable_y, rect, (int)temp_zoom.left, (int)temp_zoom.top);
			XYMinMax.left = variable_x; XYMinMax.bottom = variable_y;
			ConvertXtoA(temp_XYMinMax, &variable_x, &variable_y, rect, (int)temp_zoom.right, (int)temp_zoom.bottom);
			XYMinMax.right = variable_x; XYMinMax.top = variable_y;
			zoom_done = false;
			zoom_track_stop = false;

		}
	}

	//--------------------------------------------------------------------------------------------------
	//scala X
	//--------------------------------------------------------------------------------------------------
	double tmp_left, tmp_right;
	double scale;
	if (grid){
		SelectObject(hdcBack, hPen[8]);
		step_scale = (XYMinMax.right - XYMinMax.left) / rect.right*NR_OF_POINTS_ON_SCALE_X;
		if (fabs(step_scale)<0.0001) step_scale = 0.0001;

		if (step_scale>1.0) step_scale = floor(step_scale);
		if (step_scale>0.1) step_scale = 1.;
		if (fabs(XYMinMax.right - XYMinMax.left)<2.) {
			tmp_left = XYMinMax.left;
			tmp_right = XYMinMax.right;
		}
		else
		{
			tmp_left = floor(XYMinMax.left);
			tmp_right = floor(XYMinMax.right);
		}
		for (scale = tmp_left; scale <= tmp_right; scale += step_scale){
			ConvertAtoX(XYMinMax, scale, XYMinMax.top, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = (int)coordinateY;
			ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
			line[1].x = coordinateX; line[1].y = (int)coordinateY + 3;
			if (line[0].x>0 && line[1].x>0 && line[0].y>0)
				Polyline(hdcBack, line, 2);
		}

		//--------------------------------------------------------------------------------------------------
		//skala y
		//--------------------------------------------------------------------------------------------------


		for (scale = XYMinMax.bottom; scale <= XYMinMax.top; scale += step_scale){//
			step_scale = (XYMinMax.top - XYMinMax.bottom) / rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
			if (fabs(step_scale)<0.0001) step_scale = XYMinMax.top;
			ConvertAtoX(XYMinMax, XYMinMax.right, scale, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = coordinateY;
			ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
			line[1].x = coordinateX - 4; line[1].y = coordinateY;
			Polyline(hdcBack, line, 2);
		}

	}


	//--------------------------------------------------------------------------------------------------
	//draw  contur
	//--------------------------------------------------------------------------------------------------
	SelectObject(hdcBack, hPen[0]);
	double x, y;
	double temp_z;
	int test;
	for (int level = 0; level < Contur.Nlevel; level++){ //do it for each level
		//		int level=1;
		double level_up = Contur.zmin + Contur.zstep * (level + 1);
		double level_down = Contur.zmin + Contur.zstep * level;
		double midle = level_down + (level_up - level_down) / 2;

		for (int iy = 2; iy < Contur.NyCon - 2; ++iy){ //iterate the net for y
			for (int ix = 2; ix < Contur.NxCon - 2; ++ix){//iterate the net for x


				temp_z = Contur.z[Contur.NxCon*iy + ix];
				if (temp_z >= level_down && temp_z<level_up){
					//Point 0 defined by the ix and iy coordinates in the net

					test = Intersec(ix, iy, midle, &x, &y);
					if (test > 0) {
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[0].x = coordinateX; line[0].y = coordinateY; //scaling to the declared size of graph
						//Ellipse( hdcBack, coordinateX-2, coordinateY-2, coordinateX+2, coordinateY+2);

						//It is enought to check only 4 surrounded points
						//Poit 1
						Intersec(ix - 1, iy + 1, midle, &x, &y);
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if (temp_z <0.){
							//SelectObject(hdcBack,hPen[6]); 
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[4]);
							Polyline(hdcBack, line, 2);
						}
						else{
							//SelectObject(hdcBack,hPen[7]);
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[1]);
							Polyline(hdcBack, line, 2);
						}

						//Poit 2
						Intersec(ix, iy + 1, midle, &x, &y);
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if (temp_z <0.){
							///SelectObject(hdcBack,hPen[6]); 
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[4]);
							Polyline(hdcBack, line, 2);
						}
						else{
							//SelectObject(hdcBack,hPen[7]);
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[1]);
							Polyline(hdcBack, line, 2);
						}

						//Poit 3
						Intersec(ix + 1, iy + 1, midle, &x, &y);
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if (temp_z <0.){
							//SelectObject(hdcBack,hPen[6]); 
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[4]);
							Polyline(hdcBack, line, 2);
						}
						else{
							//SelectObject(hdcBack,hPen[7]);
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[1]);
							Polyline(hdcBack, line, 2);
						}

						//Poit 4
						Intersec(ix + 1, iy, midle, &x, &y);
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if (temp_z <0.){
							//SelectObject(hdcBack,hPen[6]); 
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[4]);
							Polyline(hdcBack, line, 2);
						}
						else{
							//SelectObject(hdcBack,hPen[7]);
							//Polyline(hdcBack, line, 2);
							SelectObject(hdcBack, hPen[1]);
							Polyline(hdcBack, line, 2);
						}

					}
				} //for point 0
			} //for x
		}//for y
	}//level


	//Draw cell edges
	SelectObject(hdcBack, hPen[0]);
	ConvertAtoX(XYMinMax, Contur.CellVertices[0].x, Contur.CellVertices[0].y, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY; //scaling to the declared size of graph
	for (int i = 0; i<5; i++){
		ConvertAtoX(XYMinMax, Contur.CellVertices[i].x, Contur.CellVertices[i].y, rect, &coordinateX, &coordinateY);
		line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
		Polyline(hdcBack, line, 2);
		line[0].x = line[1].x;  line[0].y = line[1].y;
	}




	//--------------------------------------------------------------------------------------------------
	//draw blank area
	//--------------------------------------------------------------------------------------------------
	//clean left area
	rect2.left = 0;
	rect2.right = RamkaX_left;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack, &rect2, Bacground_hBru);

	//clean right area
	rect2.left = rect.right - RamkaX_right + 1;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack, &rect2, Bacground_hBru);

	//clean top area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = RamkaY_top;
	FillRect(hdcBack, &rect2, Bacground_hBru);

	//clean bottom area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = rect.bottom - RamkaY_bottom + 1;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack, &rect2, Bacground_hBru);

	//frame x
	SelectObject(hdcBack, hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack, line, 2);

	//frame Y
	SelectObject(hdcBack, hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY + 1;

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY - 1;
	Polyline(hdcBack, line, 2);

	//--------------------------------------------------------------------------------------------------
	//Draw text
	//--------------------------------------------------------------------------------------------------
	//Draw title
	SelectObject(hdcBack, MyFont);
	rect2.left = RamkaX_left; rect2.top = MENU_HEIGHT + 6; rect2.bottom = rect2.top + 20; rect2.right = rect2.left + 800;
	swprintf_s(plot_text, HEADER, L"%s", Contur.Title);
	DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	//axes description I
	swprintf_s(plot_text, L"%s", Contur.TitleX);
	SetTextColor(hdcBack, RGB(0, 0, 0));
	ConvertAtoX(XYMinMax, XYMinMax.left, (XYMinMax.top + XYMinMax.bottom) / 2, rect, &coordinateX, &coordinateY);
	rect2.left = coordinateX - 80; rect2.top = coordinateY; rect2.bottom = rect2.top - 30; rect2.right = rect2.left + 32;
	DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	//axes description q
	swprintf_s(plot_text, L"%s", Contur.TitleY);
	SetTextColor(hdcBack, RGB(0, 0, 0));
	ConvertAtoX(XYMinMax, (XYMinMax.right + XYMinMax.left) / 2., XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	rect2.left = coordinateX - 15; rect2.top = coordinateY + 20; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 42;
	DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);


	//--------------------------------------------------------------------------------------------------
	//Plot scales and marks
	//--------------------------------------------------------------------------------------------------
	SetTextColor(hdcBack, RGB(0., 0., 0.));
	SelectObject(hdcBack, hPen[0]);
	//scala X
	step_scale = (XYMinMax.right - XYMinMax.left) / rect.right*NR_OF_POINTS_ON_SCALE_X;
	if (fabs(step_scale)<0.0001) step_scale = 0.0001;
	if (step_scale>1.0) step_scale = floor(step_scale);
	if (step_scale>0.1) step_scale = 1.;
	if (fabs(XYMinMax.right - XYMinMax.left)<2.) {
		tmp_left = XYMinMax.left;
		tmp_right = XYMinMax.right;
	}
	else
	{
		tmp_left = floor(XYMinMax.left) + 1;
		tmp_right = floor(XYMinMax.right);
	}
	for (double scale = tmp_left; scale <= tmp_right; scale += step_scale){
		ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
		line[0].x = coordinateX; line[0].y = (int)coordinateY;
		line[1].x = coordinateX; line[1].y = (int)coordinateY + 3;
		Polyline(hdcBack, line, 2);

		rect2.left = coordinateX - 22; rect2.top = coordinateY + 7; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 50;
		if (XYMinMax.right>3.) swprintf_s(plot_text, 100, L"%6.0f", scale);
		else swprintf_s(plot_text, 100, L"%6.2f", scale);
		DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}

	//skala y
	for (scale = XYMinMax.bottom; scale <= XYMinMax.top; scale += step_scale){//
		step_scale = (XYMinMax.top - XYMinMax.bottom) / rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
		if (fabs(step_scale)<0.0001) step_scale = XYMinMax.top;
		ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
		line[0].x = coordinateX; line[0].y = coordinateY;
		line[1].x = coordinateX - 4; line[1].y = coordinateY;
		Polyline(hdcBack, line, 2);
		rect2.right = coordinateX - 5; rect2.left = rect2.right - 70; rect2.top = coordinateY - 7; rect2.bottom = rect2.top + 15;
		if (XYMinMax.top<100.) swprintf_s(plot_text, 100, L"%3.2f", scale);
		else swprintf_s(plot_text, 100, L"%3.0f", scale);
		DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}



	//--------------------------------------------------------------------------------------------------
	//end plot
	//--------------------------------------------------------------------------------------------------
}


int ConturClass::Intersec(int ix, int iy, double midle, double *x, double *y){

	int result;
	//(1 0)
	result = PointTest(ix, iy, 1, 0, midle, x, y);
	if (result > 0) return result;

	//(-1 0)
	result = PointTest(ix, iy, -1, 0, midle, x, y);
	if (result > 0) return result + 3;

	//(0 1)
	result = PointTest(ix, iy, 0, -1, midle, x, y);
	if (result > 0) return result + 3;
	/*


	//(1 1)
	result = PointTest(ix, iy, 1, 1, midle, x, y );
	if( result > 0) return result;
	//(0 1)
	result = PointTest(ix, iy, 0, 1, midle, x, y );
	if( result > 0) return result;



	//(-1 1)
	result = PointTest(ix, iy, -1, 1, midle, x, y );
	if( result > 0) return result;

	//(1 -1)
	result = PointTest(ix, iy, 1, -1, midle, x, y );
	if( result > 0) return result;

	//(-1 -1)
	result = PointTest(ix, iy, -1, -1, midle, x, y );
	if( result > 0) return result;
	*/
	return -1;
}




int ConturClass::PointTest(int ix, int iy, int delx, int dely, double midle, double *x, double *y){

	//y line
	int temp_x = ix + delx;
	int temp_y = iy + dely;

	if (Contur.z[Contur.NxCon * temp_y + temp_x] <= midle &&
		Contur.z[Contur.NxCon*iy + ix] >= midle){

		if (dely<0)
			*y = (Contur.y[temp_y - 1] + Contur.y[iy]) / 2.;
		else
			*y = (Contur.y[temp_y] + Contur.y[iy]) / 2.;

		*x = (Contur.x[temp_x] + Contur.x[ix]) / 2.;
		return 1;
	}


	if (Contur.z[Contur.NxCon * temp_y + temp_x] >= midle &&
		Contur.z[Contur.NxCon*iy + ix] <= midle){

		if (dely<0)
			*y = (Contur.y[temp_y - 1] + Contur.y[iy]) / 2.;
		else
			*y = (Contur.y[temp_y] + Contur.y[iy]) / 2.;
		*x = (Contur.x[temp_x] + Contur.x[ix]) / 2.;

		return 2;
	}

	return -1;
}
/*************************************************************************///----------------------------  FloodFill( )  ---------------------------///*************************************************************************/struct Pixel

/*

void ConturClass::SetNewMaskSize(){
if (Mask != NULL)
delete[] Mask;

int Height = WindowsRect.bottom - WindowsRect.top;
int Width = WindowsRect.right - WindowsRect.left;

if (Height < 2000) Height = 2000;
if (Width < 2000) Width = 2000;
Mask = new int[(Height + 1)*(Width + 1)];
}

void Graph2D::insert_pixel(const int x, const int y)
{
Entry = new Pixel2;

Entry->x = x;
Entry->y = y;

Entry->Next = NULL;
Last->Next = Entry;
Last = Entry;
}


void ConturClass::BoundaryFill(const int _x, const int _y, int Width, int Height, int fill_color, int boundary_color){

if (Mask[_x + _y*Width] == boundary_color || Mask[_x + _y*Width] == fill_color)
return;

int x = _x;
int y = _y;

Start = new Pixel2;

Start->x = x;
Start->y = y;

Start->Next = NULL;
Last = Start;

while (Start != NULL)
{
Mask[_x + _y*Width] = fill_color;

if ((x - 1) >= 0 && Mask[x - 1 + y*Width] != boundary_color &&
Mask[x - 1 + y*Width] != fill_color)
{
Mask[x - 1 + y*Width] = fill_color;

insert_pixel((x - 1), y);
}

if ((y - 1) >= 0 && Mask[x + (y - 1)*Width] != boundary_color &&
Mask[x + (y - 1)*Width] != fill_color)
{
Mask[x + (y - 1)*Width] = fill_color;

insert_pixel(x, (y - 1));
}

if ((x + 1) <= Width && Mask[x + 1 + y*Width] != boundary_color &&
Mask[x + 1 + y*Width] != fill_color)
{
Mask[x + 1 + y*Width] = fill_color;

insert_pixel((x + 1), y);
}

if ((y + 1) <= Height && Mask[x + (y + 1)*Width] != boundary_color &&
Mask[x + (y + 1)*Width] != fill_color)
{
Mask[x + (y + 1)*Width] = fill_color;

insert_pixel(x, (y + 1));
}

Entry = Start;
Start = Start->Next;

if (Start)x = Start->x;
if (Start)y = Start->y;

delete Entry;
}

}


void ConturClass::FillConturIntoMask(){

//--------------------------------------------------------------------------------------------------
//draw  contur to Mask array
//--------------------------------------------------------------------------------------------------
double x, y;					//values from physical model
int coordinateX, coordinateY;	//pixel coordinate on the screen
double temp_z;
int test;
POINT line[2];

if (Mask == NULL) return;
Mask = new int[10];

for (int level = 0; level < Contur.Nlevel; level++){ //do it for each level
//		int level=1;
double level_up = Contur.zmin + Contur.zstep * (level + 1);
double level_down = Contur.zmin + Contur.zstep * level;
double midle = level_down + (level_up - level_down) / 2;


for (int iy = 2; iy < Contur.NyCon - 2; ++iy){ //iterate the net for y
for (int ix = 2; ix < Contur.NxCon - 2; ++ix){//iterate the net for x


temp_z = Contur.z[Contur.NxCon*iy + ix];
if (temp_z >= level_down && temp_z<level_up){
//Point 0 defined by the ix and iy coordinates in the net

test = Intersec(ix, iy, midle, &x, &y);
if (test > 0) {
ConvertAtoX(XYMinMax, x, y, WindowsRect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY; //scaling to the declared size of graph
//Ellipse( hdcBack, coordinateX-2, coordinateY-2, coordinateX+2, coordinateY+2);

//It is enought to check only 4 surrounded points
//Poit 1
Intersec(ix - 1, iy + 1, midle, &x, &y);
ConvertAtoX(XYMinMax, x, y, WindowsRect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
LineContur(line[0], line[1], 1);


//Poit 2
Intersec(ix, iy + 1, midle, &x, &y);
ConvertAtoX(XYMinMax, x, y, WindowsRect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
LineContur(line[0], line[1], 1);

//Poit 3
Intersec(ix + 1, iy + 1, midle, &x, &y);
ConvertAtoX(XYMinMax, x, y, WindowsRect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
LineContur(line[0], line[1], 1);

//Poit 4
Intersec(ix + 1, iy, midle, &x, &y);
ConvertAtoX(XYMinMax, x, y, WindowsRect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
LineContur(line[0], line[1], 1);
}
} //for point 0
} //for x
}//for y
}//leve
}

void Graph2D::CalculateContur(){
SetNewMaskSize();
FillConturIntoMask();
//add filling to the contur

}



//-----------------------------------------------------------------------------
// The main paint function for 2D window.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void ConturClass::FilledConturPlot(HWND hwnd, HDC hdcBack, POINT Pt, RECT rect){


POINT line[2];

int coordinateX, coordinateY;		//for position on the screen
double variable_x, variable_y;		//for position in variable space
double step_scale;					//Step of the scales x and y
BORDERS temp_zoom;					//for selection of the zoom area

RECT rect2;
TCHAR plot_text[HEADER];

//SetBkColor(hdcBack,RGB(255,255,255));
SetBkMode(hdcBack, TRANSPARENT);
FillRect(hdcBack, &rect, hBru[7]);


//safety
if (fabs(XYMinMax.left - XYMinMax.right)<.000001) Initialize_borders();
if (fabs(XYMinMax.top - XYMinMax.bottom)<.000001) Initialize_borders();


//--------------------------------------------------------------------------------------------------
//fill plot area
//--------------------------------------------------------------------------------------------------
ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
rect2.left = coordinateX; rect2.top = coordinateY;
ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);

rect2.bottom = coordinateY; rect2.right = coordinateX;

DeleteObject(SelectObject(hdcBack, hBru[1]));
hBru[1] = CreateSolidBrush(rgbBackground);
FillRect(hdcBack, &rect2, hBru[1]);

//--------------------------------------------------------------------------------------------------
//zoom draw the rectangular
//--------------------------------------------------------------------------------------------------
if (fabs(XYMinMax.top - XYMinMax.bottom) < 0.0000001) XYMinMax.bottom += 0.0000001;
if (fabs(XYMinMax.right - XYMinMax.left) < 0.0000001) XYMinMax.right += 0.0000001;


if (zoom_frame.left == zoom_frame.right)  zoom_frame.right = zoom_frame.left + 1; //safety
if (zoom_frame.top == zoom_frame.bottom) zoom_frame.top = zoom_frame.bottom + 1;
if (zoom_track_stop && !zoom_done){
SelectObject(hdcBack, hPen[4]);
Rectangle(hdcBack, zoom_frame.left + 1, zoom_frame.top + 1, zoom_frame.right - 1, zoom_frame.bottom - 1);
FillRect(hdcBack, &zoom_frame, hBru[5]);
}

if (!zoom_track_stop){  //we are not in zooming action
}
else {
if (zoom_done){
BORDERS temp_XYMinMax;



temp_XYMinMax.bottom = XYMinMax.bottom;
temp_XYMinMax.top = XYMinMax.top;
temp_XYMinMax.left = XYMinMax.left;
temp_XYMinMax.right = XYMinMax.right;

if (zoom_frame.top - zoom_frame.bottom<0){
temp_zoom.bottom = zoom_frame.top;
temp_zoom.top = zoom_frame.bottom;
}
else{
temp_zoom.bottom = zoom_frame.bottom;
temp_zoom.top = zoom_frame.top;
}

if (zoom_frame.right < zoom_frame.left){
temp_zoom.left = zoom_frame.right;
temp_zoom.right = zoom_frame.left;
}
else{
temp_zoom.left = zoom_frame.left;
temp_zoom.right = zoom_frame.right;
}

ConvertXtoA(temp_XYMinMax, &variable_x, &variable_y, rect, (int)temp_zoom.left, (int)temp_zoom.top);
XYMinMax.left = variable_x; XYMinMax.bottom = variable_y;
ConvertXtoA(temp_XYMinMax, &variable_x, &variable_y, rect, (int)temp_zoom.right, (int)temp_zoom.bottom);
XYMinMax.right = variable_x; XYMinMax.top = variable_y;
zoom_done = false;
zoom_track_stop = false;


}
}

//--------------------------------------------------------------------------------------------------
//scala X
//--------------------------------------------------------------------------------------------------
double tmp_left, tmp_right;
double scale;
if (grid){
SelectObject(hdcBack, hPen[8]);
step_scale = (XYMinMax.right - XYMinMax.left) / rect.right*NR_OF_POINTS_ON_SCALE_X;
if (fabs(step_scale)<0.0001) step_scale = 0.0001;

if (step_scale>1.0) step_scale = floor(step_scale);
if (step_scale>0.1) step_scale = 1.;
if (fabs(XYMinMax.right - XYMinMax.left)<2.) {
tmp_left = XYMinMax.left;
tmp_right = XYMinMax.right;
}
else
{
tmp_left = floor(XYMinMax.left);
tmp_right = floor(XYMinMax.right);
}
for (scale = tmp_left; scale <= tmp_right; scale += step_scale){
ConvertAtoX(XYMinMax, scale, XYMinMax.top, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = (int)coordinateY;
ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = (int)coordinateY + 3;
if (line[0].x>0 && line[1].x>0 && line[0].y>0)
Polyline(hdcBack, line, 2);
}

//--------------------------------------------------------------------------------------------------
//skala y
//--------------------------------------------------------------------------------------------------


for (scale = XYMinMax.bottom; scale <= XYMinMax.top; scale += step_scale){//
step_scale = (XYMinMax.top - XYMinMax.bottom) / rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
if (fabs(step_scale)<0.0001) step_scale = XYMinMax.top;
ConvertAtoX(XYMinMax, XYMinMax.right, scale, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY;
ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
line[1].x = coordinateX - 4; line[1].y = coordinateY;
Polyline(hdcBack, line, 2);
}

}


//--------------------------------------------------------------------------------------------------
//draw  contur
//--------------------------------------------------------------------------------------------------
SelectObject(hdcBack, hPen[0]);
if (Mask == NULL) return;
int width = 500; WindowsRect.right - WindowsRect.left;
if(width < 1)return;
int itemp;
for (int yi = 0; yi <WindowsRect.bottom - WindowsRect.top; ++yi){
itemp = yi*width;
for (int xi = 0; xi < width; ++xi){
if (Mask[xi + itemp] == 1) SetPixel(hdcBack, xi, yi, RGB(0, 0, 0));
}
}


//Draw cell edges
SelectObject(hdcBack, hPen[0]);
ConvertAtoX(XYMinMax, Contur.CellVertices[0].x, Contur.CellVertices[0].y, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY; //scaling to the declared size of graph
for (int i = 0; i<5; i++){
ConvertAtoX(XYMinMax, Contur.CellVertices[i].x, Contur.CellVertices[i].y, rect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
Polyline(hdcBack, line, 2);
line[0].x = line[1].x;  line[0].y = line[1].y;
}




//--------------------------------------------------------------------------------------------------
//draw blank area
//--------------------------------------------------------------------------------------------------
//clean left area
rect2.left = 0;
rect2.right = RamkaX_left;
rect2.top = 0;
rect2.bottom = rect.bottom;
FillRect(hdcBack, &rect2, Bacground_hBru);

//clean right area
rect2.left = rect.right - RamkaX_right + 1;
rect2.right = rect.right;
rect2.top = 0;
rect2.bottom = rect.bottom;
FillRect(hdcBack, &rect2, Bacground_hBru);

//clean top area
rect2.left = 0;
rect2.right = rect.right;
rect2.top = 0;
rect2.bottom = RamkaY_top;
FillRect(hdcBack, &rect2, Bacground_hBru);

//clean bottom area
rect2.left = 0;
rect2.right = rect.right;
rect2.top = rect.bottom - RamkaY_bottom + 1;
rect2.bottom = rect.bottom;
FillRect(hdcBack, &rect2, Bacground_hBru);

//frame x
SelectObject(hdcBack, hPen[0]);
ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY;

ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY;
Polyline(hdcBack, line, 2);

ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY;

ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY;
Polyline(hdcBack, line, 2);

//frame Y
SelectObject(hdcBack, hPen[0]);
ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY + 1;

ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY;
Polyline(hdcBack, line, 2);

ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY;

ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
line[1].x = coordinateX; line[1].y = coordinateY - 1;
Polyline(hdcBack, line, 2);

//--------------------------------------------------------------------------------------------------
//Draw text
//--------------------------------------------------------------------------------------------------
//Draw title
SelectObject(hdcBack, MyFont);
rect2.left = RamkaX_left; rect2.top = MENU_HEIGHT + 6; rect2.bottom = rect2.top + 20; rect2.right = rect2.left + 800;
swprintf_s(plot_text, HEADER, L"%s", Contur.Title);
DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

//axes description I
swprintf_s(plot_text, L"%s", Contur.TitleX);
SetTextColor(hdcBack, RGB(0, 0, 0));
ConvertAtoX(XYMinMax, XYMinMax.left, (XYMinMax.top + XYMinMax.bottom) / 2, rect, &coordinateX, &coordinateY);
rect2.left = coordinateX - 80; rect2.top = coordinateY; rect2.bottom = rect2.top - 30; rect2.right = rect2.left + 32;
DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

//axes description q
swprintf_s(plot_text, L"%s", Contur.TitleY);
SetTextColor(hdcBack, RGB(0, 0, 0));
ConvertAtoX(XYMinMax, (XYMinMax.right + XYMinMax.left) / 2., XYMinMax.bottom, rect, &coordinateX, &coordinateY);
rect2.left = coordinateX - 15; rect2.top = coordinateY + 20; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 42;
DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);


//--------------------------------------------------------------------------------------------------
//Plot scales and marks
//--------------------------------------------------------------------------------------------------
SetTextColor(hdcBack, RGB(0., 0., 0.));
SelectObject(hdcBack, hPen[0]);
//scala X
step_scale = (XYMinMax.right - XYMinMax.left) / rect.right*NR_OF_POINTS_ON_SCALE_X;
if (fabs(step_scale)<0.0001) step_scale = 0.0001;
if (step_scale>1.0) step_scale = floor(step_scale);
if (step_scale>0.1) step_scale = 1.;
if (fabs(XYMinMax.right - XYMinMax.left)<2.) {
tmp_left = XYMinMax.left;
tmp_right = XYMinMax.right;
}
else
{
tmp_left = floor(XYMinMax.left) + 1;
tmp_right = floor(XYMinMax.right);
}
for (double scale = tmp_left; scale <= tmp_right; scale += step_scale){
ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = (int)coordinateY;
line[1].x = coordinateX; line[1].y = (int)coordinateY + 3;
Polyline(hdcBack, line, 2);

rect2.left = coordinateX - 22; rect2.top = coordinateY + 7; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 50;
if (XYMinMax.right>3.) swprintf_s(plot_text, 100, L"%6.0f", scale);
else swprintf_s(plot_text, 100, L"%6.2f", scale);
DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}

//skala y
for (scale = XYMinMax.bottom; scale <= XYMinMax.top; scale += step_scale){//
step_scale = (XYMinMax.top - XYMinMax.bottom) / rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
if (fabs(step_scale)<0.0001) step_scale = XYMinMax.top;
ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
line[0].x = coordinateX; line[0].y = coordinateY;
line[1].x = coordinateX - 4; line[1].y = coordinateY;
Polyline(hdcBack, line, 2);
rect2.right = coordinateX - 5; rect2.left = rect2.right - 70; rect2.top = coordinateY - 7; rect2.bottom = rect2.top + 15;
if (XYMinMax.top<100.) swprintf_s(plot_text, 100, L"%3.2f", scale);
else swprintf_s(plot_text, 100, L"%3.0f", scale);
DrawText(hdcBack, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
}



//--------------------------------------------------------------------------------------------------
//end plot
//--------------------------------------------------------------------------------------------------
}

//Fill intermediate points
void ConturClass::LineContur(POINT Pos1, POINT Pos2, int Color){


float delta_x;
float delta_y;
int i;
int delta_i;
int width = 500;// WindowsRect.right - WindowsRect.left;

//	Mask[Pos1.x + Pos1.y*width] = Color;
//	return;

delta_x = (Pos2.x - Pos1.x);
delta_y = (Pos2.y - Pos1.y);
//take longer vector
if (abs((int)delta_x)>abs((int)delta_y))
delta_i = abs((int)delta_x);
else
delta_i = abs((int)delta_y);
delta_x /= delta_i;
delta_y /= delta_i;

for (i = 0; i<delta_i; ++i){

Mask[(int)(Pos1.x + delta_x*i) + (int)(Pos1.y + delta_y*i)*width] = Color;
}

}
*/