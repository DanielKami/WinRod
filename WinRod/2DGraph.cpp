/***************************************************************************/
/*      This file contains all the functions necessary for ploting and printing 
        2D windows scene. It is replesment for the other PLOT programs/libraries
		used in other ROD versions.
*/
/***************************************************************************/
#include <stdafx.h>
#include <commdlg.h>
#include <stdio.h>
#include <math.h>
#include <Windowsx.h>
#include <commctrl.h>
#include "resource.h"
#include "toolbar2d.h"
#include "2DGraph.h"
#include "ReadFile.h"

	
//-----------------------------------------------------------------------------
	//Create the window 
	// 21.01.2013 modified by Daniel Kamisnki
//-----------------------------------------------------------------------------
HWND Graph2D::CreateAppWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect )
{
    // Create a window that is centered on the desktop. It's exactly 1/4 the
    // size of the desktop.

	DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_COMPOSITED;//|WS_EX_COMPOSITED

    DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
					 WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
					 WS_CLIPSIBLINGS |  WS_SIZEBOX;// // WS_CLIPCHILDREN


	HWND hwnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
        wndStyle, 0, 0, 0, 0, 0, 0, wcl.hInstance, 0);

	if (hwnd)
    {
        RECT rc = {0};

		SetRect(&rc, WinRect.left, WinRect.top, WinRect.right, WinRect.bottom);
       // AdjustWindowRectEx(&rc, wndStyle, FALSE, wndExStyle);
		MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }

	return hwnd;
}

//-----------------------------------------------------------------------------
// Create Status Bar.
//PURPOSE: 
//  Registers the StatusBar control class and creates a statusbar.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
HWND WINAPI Graph2D::CreateStatusBar (HWND hwnd, HINSTANCE hInst){
    //Initiate controls
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof( INITCOMMONCONTROLSEX );
	icc.dwICC = ICC_BAR_CLASSES; // toolbary, statusbary i tooltipy
	InitCommonControlsEx( & icc );

	HWND hWndSB = CreateWindowEx( 0, STATUSCLASSNAME, NULL, SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE | SBT_TOOLTIPS
									, 0, 0, 0, 0, hwnd,( HMENU ) 200, hInst, NULL );

	if (!hWndSB){
         MessageBox(hwnd, TEXT("Can't open status bar window"), NULL, MB_ICONERROR); 
         return NULL;
    }
    ShowWindow(hWndSB, SW_SHOW);
    UpdateWindow(hWndSB);

	int iStatusBarWidths[] = { 300,500,  -1 };
	SendMessage(hWndSB, SB_SETPARTS, 3, (LPARAM)iStatusBarWidths);
	SendMessage(hWndSB, SB_SETTIPTEXT, 0, (LPARAM) "Data file name.");
	SendMessage(hWndSB, SB_SETTIPTEXT, 1, (LPARAM) "Nr of points");
	SendMessage(hWndSB, SB_SETTIPTEXT, 2, (LPARAM) "x, y position in the drawing");

return hWndSB;
}


//-----------------------------------------------------------------------------
// CreateToolbar.
//PURPOSE: 
//  Registers the TOOLBAR control class and creates a toolbar.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
HWND WINAPI Graph2D::CreateToolbar (HWND hwnd, HINSTANCE hInst, int Width)
{

  DWORD dwStyle;              // Style of the toolbar


	INITCOMMONCONTROLSEX iccex; // INITCOMMONCONTROLSEX structure
	iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
	iccex.dwICC =  ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx( & iccex );
		
	dwStyle = WS_VISIBLE | WS_CHILD | RBS_BANDBORDERS | RBS_AUTOSIZE | WS_CLIPSIBLINGS | RBS_TOOLTIPS | WS_BORDER;// | RBS_VARHEIGHT;
	
	if (!(Graph2D::hRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, dwStyle, 0, 0, 0, 0, hwnd, NULL, hInst, NULL)))
  {
 		MessageBox(hwnd, TEXT("Can't create rebar for 2D window"), NULL, MB_ICONERROR);
		return NULL;
  }

	//Add rebar
	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof( REBARBANDINFO );
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RB_BEGINDRAG | RBBIM_SIZE | RBBIM_IDEALSIZE;//
	rbbi.fStyle = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS | RBBS_USECHEVRON | RBBS_TOPALIGN | RBBS_CHILDEDGE;//|RBBS_BREAK| RBBS_TOPALIGN RBBS_FIXEDBMP |    


	//  Create the toolbar control.
	//dwStyle = WS_VISIBLE | WS_CHILD |  CCS_NOPARENTALIGN;
	dwStyle = TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | TBSTYLE_FLAT ;//

	if (!(Graph2D::hwndTB = CreateToolbarEx(hRebar, dwStyle, (UINT)ID_TOOLBAR, NUMIMAGES2D_HOR, hInst, IDB_TOOLBAR, tbButton, sizeof(tbButton) / sizeof(TBBUTTON),
                                    BUTTONWIDTH, BUTTONHEIGHT, IMAGEWIDTH, IMAGEHEIGHT, sizeof (TBBUTTON))))
		{
 		MessageBox(hwnd, TEXT("Can't create toolbar for 2D window"), NULL, MB_ICONERROR);
		return NULL;
		}
  
	//Add tolbar to rebar
	rbbi.lpText = L"";
	rbbi.hwndChild = Graph2D::hwndTB;
	rbbi.cxMinChild = 200;
	rbbi.cyMinChild = MENU_HEIGHT;
	rbbi.cx = 100;
	SendMessage( hRebar, RB_INSERTBAND,( WPARAM ) - 1,( LPARAM ) & rbbi );

  return hRebar;
}

//-----------------------------------------------------------------------------
// CreateToolbarVertical.
//PURPOSE: 
//  Registers the side TOOLBAR control class and creates a toolbar.
// 14.08.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------	
HWND WINAPI Graph2D::CreateToolbarVertical(HWND hwnd, HINSTANCE hInst, int Width)
{
	DWORD dwStyle;              // Style of the toolbar

	INITCOMMONCONTROLSEX iccex; // INITCOMMONCONTROLSEX structure
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&iccex);

	dwStyle = TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | TBSTYLE_FLAT | CCS_VERT | CCS_NORESIZE;//

	if (!(hwndTBVer = CreateToolbarEx(hwnd, dwStyle, (UINT)ID_TOOLBAR, NUMIMAGES2D_VERT, hInst, IDB_TOOLBAR, tbButtonVert, sizeof(tbButtonVert) / sizeof(TBBUTTON),
		BUTTONWIDTH, BUTTONHEIGHT, IMAGEWIDTH, IMAGEHEIGHT, sizeof(TBBUTTON))))
	{
		MessageBox(hwnd, TEXT("Can't create vertical toolbar for 2D window"), NULL, MB_ICONERROR);
		return NULL;
	}

	return hwndTBVer;
}



void Graph2D::ButtonsState()
{

	//What to plot
	if (fit[2].status == true)	SendMessage(hwndTB, TB_SETSTATE, IDM_SUR, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
	else						SendMessage(hwndTB, TB_SETSTATE, IDM_SUR, MAKELONG(TBSTATE_ENABLED, 0));
	if (fit[1].status == true)	SendMessage(hwndTB, TB_SETSTATE, IDM_BUL, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
	else						SendMessage(hwndTB, TB_SETSTATE, IDM_BUL, MAKELONG(TBSTATE_ENABLED, 0));
	if (fit[0].status == true)	SendMessage(hwndTB, TB_SETSTATE, IDM_BOT, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
	else						SendMessage(hwndTB, TB_SETSTATE, IDM_BOT, MAKELONG(TBSTATE_ENABLED, 0));
	if (data_plot == true)		SendMessage(hwndTB, TB_SETSTATE, IDM_DATA, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
	else						SendMessage(hwndTB, TB_SETSTATE, IDM_DATA, MAKELONG(TBSTATE_ENABLED, 0));
	if (errors_plot == true)	SendMessage(hwndTB, TB_SETSTATE, IDM_ERROR, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
	else						SendMessage(hwndTB, TB_SETSTATE, IDM_ERROR, MAKELONG(TBSTATE_ENABLED, 0));
	if (grid == true)			SendMessage(hwndTBVer, TB_SETSTATE, IDM_GRID2D, MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
	else						SendMessage(hwndTBVer, TB_SETSTATE, IDM_GRID2D, MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));


	//button grayed
	switch (SELECT_GRAPH)
	{
	case WINDOW_ROD:
		SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_FI, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_FII, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LOG, MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LIN, MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_CURVE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND_SIMPLE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LINE, MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_POINTS, MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_ENABLED, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_ENABLED, 0));
		
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_DATA, MAKELONG(TBSTATE_ENABLED, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_FIT, MAKELONG(TBSTATE_ENABLED, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_DATA, MAKELONG(TBSTATE_ENABLED, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_FIT, MAKELONG(TBSTATE_ENABLED, 0));


		if (fit[3].status == true)
			SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else
			SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_ENABLED, 0));

		if (show_LBragg == true)
			SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else
			SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_ENABLED, 0));
		return;


	case WINDOW_IN_PLANE:
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_FI, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_FII, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_CURVE, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LINE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_POINTS, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_FIT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_FIT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND_SIMPLE, MAKELONG(TBSTATE_HIDDEN, 0));
		return;


	case WINDOW_CONTUR:
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_FI, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_FII, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_CURVE, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LINE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_POINTS, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_FIT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_FIT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_SUR, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_BUL, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_BOT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ERROR, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND_SIMPLE, MAKELONG(TBSTATE_HIDDEN, 0));
		return;


	case WINDOW_DENSITY:
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_FI, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_FII, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LOG, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LIN, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LINE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_POINTS, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_SUR, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_BUL, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_BOT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ERROR, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND_SIMPLE, MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND, MAKELONG(TBSTATE_HIDDEN, 0));


		if (fit[0].status == true)
			SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_CURVE, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else
			SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_CURVE, MAKELONG(TBSTATE_ENABLED, 0));

		return;

	case WINDOW_DISPERSION:
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_CURVE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_SUR, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_BUL, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_BOT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ERROR, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LOG, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LIN, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LINE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_POINTS, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_FIT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_FIT, MAKELONG(TBSTATE_HIDDEN, 0));

		if (fit[0].status == true) SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_FI, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_FI, MAKELONG(TBSTATE_ENABLED, 0));
		if (fit[1].status == true) SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_FII, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else                      SendMessage(hwndTB, TB_SETSTATE, IDM_SHOW_FII, MAKELONG(TBSTATE_ENABLED, 0));
		fit[3].status = fit[2].status = false;
		show_LBragg = false;
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND_SIMPLE, MAKELONG(TBSTATE_HIDDEN, 0));
		return;

	case WINDOW_MULTIPLOT:
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SHOW_CURVE, MAKELONG(TBSTATE_HIDDEN, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LOG, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_LIN, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_ENABLED, 0));

		SendMessage(hwndTBVer, TB_SETSTATE, IDM_ZOOM, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_SCALE_MOVE, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_ENABLED, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_OPEN_FIT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_DATA, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTB, TB_SETSTATE, IDM_ADD_FIT, MAKELONG(TBSTATE_HIDDEN, 0));
		SendMessage(hwndTBVer, TB_SETSTATE, IDM_EXPAND_SIMPLE, MAKELONG(TBSTATE_HIDDEN, 0));
		
		if (fit[3].status == true)
			SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else
			SendMessage(hwndTB, TB_SETSTATE, IDM_PHASE, MAKELONG(TBSTATE_ENABLED, 0));
		
		if (show_LBragg == true)
			SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, 0));
		else
			SendMessage(hwndTB, TB_SETSTATE, IDM_LBRAGG, MAKELONG(TBSTATE_ENABLED, 0));

		return;
	}
}


//-----------------------------------------------------------------------------
// Process all events related to 2D windows.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
LRESULT CALLBACK Graph2D::WinProc(HWND hWnd, HINSTANCE hinst, UINT msg, WPARAM wParam, LPARAM lParam)
{

	
	static int MenuHeight;
	RECT rect;
	WCHAR Mesage[200]=L"";
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

	case WM_ACTIVATE:
		switch (wParam)
		{
		case WA_ACTIVE:
			arrow_flag = true;
			ButtonsState();
			break;
		}
		break;

	case WM_CREATE:
	{
		hwndWindow = hWnd;
		SELECT_GRAPH = WINDOW_ROD; //set the standard graph
		ScaleType = SCALE_TO_CURVE_AND_POINTS;  //fit scale to curves and data
		grid = true;

		line_points = false;

		Initialize_Graphisc(hWnd);

		InitializeContentMenu(hinst, hWnd);
		

		//if neccesary
		if (data.NrOfPoints == 0 && fit[0].NrOfPoints == 0)
			Initialize_borders();
		else
			ResetScale();
		AutomaticSetTheScale();

		hdc = GetDC(hWnd);
		hdcBack = CreateCompatibleDC(hdc);
		hdcBack2 = CreateCompatibleDC(hdc);
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

		HBITMAP hbmBack2 = CreateCompatibleBitmap(hdc, desktop.right , desktop.bottom );
		SelectObject(hdcBack2, (HBITMAP)hbmBack2);
		PatBlt(hdcBack2, 0, 0, desktop.right*2, desktop.bottom*2, WHITENESS); //PATINVERT
		DeleteObject(hbmBack2);

		// Creates the toolbar.
		CreateToolbar(hWnd, hinst, desktop.right);
		CreateToolbarVertical(hWnd, hinst, desktop.right);
		hwndSB = CreateStatusBar(hWnd, hinst);

		fit[0].status = true;
		fit[1].status = true;
		fit[2].status = false;
		data.status = true;
		errors_plot = true;
	}
		break;


	case WM_SIZE:

		GetClientRect (hWnd, &rect);
		GetWindowRect(hRebar, &WindowsRect);
		sizing = true;
		MenuHeight = WindowsRect.bottom - WindowsRect.top + 2;
		SendMessage(hwndSB, WM_SIZE, 0, 0);
		SendMessage(hRebar, WM_SIZE, 0, 0);
		pt.x = -1;
		DrawStatusBar(hwndSB, pt, rect);

		rect.top+= MENU_HEIGHT+7;
		rect.bottom -= MENU_HEIGHT;
		rect.right = SIDE_BAR_WIDTH;
		//SendMessage(hRebarVer, WM_SIZE,0, (LPARAM)&rect);
		MoveWindow(hwndTBVer, rect.left, rect.top, rect.right, rect.bottom-rect.top, 1);
	
		if (WindowsRect.right - WindowsRect.left < 100)
			FrameCorrection = 60;
		else
			FrameCorrection = 0;
		break;


	case WM_COMMAND:

		ContentMenuMsgTranslate(hinst, hWnd, LOWORD(wParam));
		// Parse the menu selections:
		switch (LOWORD(wParam))
		{

		case IDM_FILE_OPEN: 
		{
			OpenFileFunction(hWnd);
		}
			break;

		case IDM_FILE_SAVE: //Save the bitmap
		{
			Save2DFunction(hWnd);
		}
			break;

		case IDM_PRINT: 
		{
			PrintFunction(hWnd);			
		}
			break;

		case IDM_COPY_TO_CLIPOBARD: 
		{
			CopyToClipobard(hWnd);
		}
			break;
			
		case IDM_2D_SETTINGS: 
		{
			DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_2D_SETTINGS), hWnd, Settings2D, (LPARAM)&rgbColors);

			DeleteObject(Background_hBru);
			DeleteObject(hBru[1]);
			hBru[1] = CreateSolidBrush(rgbColors.background);
			Background_hBru = CreateSolidBrush(rgbColors.framecolor);
			InvalidateRect(hWnd, 0, 0);
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
			ConvertLogScaleToLinScale(); //To avoid problems with manual change of limits
			AutomaticSetTheScale();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_LOG:
			lin_log = false;
			ResetScale();
			ConvertLogScaleToLinScale(); //To avoid problems with manual change of limits
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
			//if ((SendMessage(hwndTBVer, TB_GETSTATE | TBSTATE_WRAP, IDM_GRID2D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
			if ((SendMessage(hwndTBVer, TB_GETSTATE, IDM_GRID2D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				grid = true;
			else
				grid = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_LBRAGG:
			if ((SendMessage(hwndTB, TB_GETSTATE | TBSTATE_WRAP, IDM_LBRAGG, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				show_LBragg = true;
			else
				show_LBragg = false;				
			InvalidateRect(hWnd, 0, 0);
			break;
			

		case IDM_EXPAND:
			ScaleType = SCALE_TO_CURVE_AND_POINTS;
			AutomaticSetTheScale();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_EXPAND_DATA:
			ScaleType = SCALE_TO_POINTS;
			AutomaticSetTheScale();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_EXPAND_SIMPLE:
			AutomaticSetTheScale();
			InvalidateRect(hWnd, 0, 0);
			break;
			
		case IDM_SUR:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_SUR, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				fit[2].status = true;
			else fit[2].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_BUL:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_BUL, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				fit[1].status = true;
			else fit[1].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_PHASE:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_PHASE, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				fit[3].status = true;
			else
				fit[3].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_SHOW_CURVE:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_SHOW_CURVE, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) fit[0].status = true;
			else fit[0].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_SHOW_FI:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_SHOW_FI, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) fit[0].status = true;
			else fit[0].status = false;
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_SHOW_FII:
			if ((SendMessage(hwndTB, TB_GETSTATE, IDM_SHOW_FII, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED) fit[1].status = true;
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

		case IDM_OPEN_DATA:
			DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_OPEN_DATA_POINTS), hWnd, OpenDataPoints, (LPARAM)&Opendat);
			{
				int readed = 0;
				for (int i = 0; i < MAX_OPEN; i++){

					if (Opendat[i].readed) readed++;
				}
				nrOpenDat = readed;
			}
			CopyColorsToOpenBruPen();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_OPEN_FIT:
			DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_FIT_OPEN), hWnd, OpenFitPoints, (LPARAM)&Openfit);
			{
				int readed=0;
				for (int i = 0; i < MAX_OPEN; ++i){

					if (Openfit[i].readed) readed++;
				}
				nrOpenFit = readed;
			}
			CopyColorsToOpenBruPen();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_ADD_DATA:
			CopyDataToOpenDat();
			CopyColorsToOpenBruPen();
			InvalidateRect(hWnd, 0, 0);
			break;

		case IDM_ADD_FIT:
			CopyFitToCurve();
			CopyColorsToOpenBruPen();
			InvalidateRect(hWnd, 0, 0);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		//ps.fErase = 1;

		GetClientRect(hWnd, &rect);
		hdc = BeginPaint(hWnd, &ps);

		if (sizing)
		{
			FillRect(hdc, &rect, (HBRUSH)(COLOR_3DFACE));
			sizing = false;
		}

		//ButtonsState();
		SetFrames();
		
		//rect.left = SIDE_BAR_WIDTH; //correct print area in respect to left toolbar
		if (SELECT_GRAPH == WINDOW_ROD) Draw(hWnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_IN_PLANE) DrawInPlane(hWnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_CONTUR) ConturPlot(hWnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_DENSITY) Draw(hWnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_DISPERSION) Draw(hWnd, hdcBack, pt, rect);//Dispersion
		if (SELECT_GRAPH == WINDOW_MULTIPLOT) MultiDraw(hWnd, hdcBack, hdcBack2, rect);
		//			if (SELECT_GRAPH == 5) FilledConturPlot(hWnd, hdcBack, pt, rect);

		BitBlt(hdc, SIDE_BAR_WIDTH, MenuHeight, rect.right, rect.bottom - MenuHeight - 23, hdcBack, SIDE_BAR_WIDTH, MenuHeight, SRCCOPY);
		
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
		if (zoom_flag)//we do zooming
		{
			zoom_track_stop = true;
			zoom_done = false;
			ExtractMousePosition(&pt, hWnd, lParam);

			zoom_frame.left = zoom_frame.right = pt.x;
			zoom_frame.top = zoom_frame.bottom = pt.y;
		}

		if (scale_move)
			ExtractMousePosition(&pt_old, hWnd, lParam);

		break;

	case WM_LBUTTONUP:

		if (zoom_flag){//we do zooming
			zoom_done = true;

			if (storage_XYMinMax.left > XYMinMax.left)  storage_XYMinMax.left = XYMinMax.left;
			if (storage_XYMinMax.right < XYMinMax.right) storage_XYMinMax.right = XYMinMax.right;
			if (storage_XYMinMax.top < XYMinMax.top) storage_XYMinMax.top = XYMinMax.top;
			if (storage_XYMinMax.bottom > XYMinMax.bottom) storage_XYMinMax.bottom = XYMinMax.bottom;
		}

		if (arrow_flag && (SELECT_GRAPH == WINDOW_ROD || SELECT_GRAPH == WINDOW_IN_PLANE || SELECT_GRAPH == WINDOW_DENSITY))
		{			
			ExtractMousePosition(&pt, hWnd, lParam);
			int DataNumber = FindDataNumber(hWnd);

				hPositionMenu = CreatePopupMenu();
				hSubMenu = CreatePopupMenu();
				if (!hSubMenu)
					break;
				if (DataNumber >= 0)
				{
					swprintf_s(Mesage, 200, L"x = %4.3f,  y = %4.3f, error = %4.3f", data.x[DataNumber], data.y[DataNumber], data.sigma[DataNumber]);
					InsertMenu(hSubMenu, 0, MF_BYPOSITION | MF_STRING, 0, Mesage);
				}
				else
				{
					XYMinMax.Title = data.header;
					XYMinMax.TitleX = data.TitleX;
					XYMinMax.TitleY = data.TitleY;

					int coordinateX, coordinateY, coordinateY2;
					ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, WindowsRect, &coordinateX, &coordinateY);
					ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, WindowsRect, &coordinateX, &coordinateY2);
					//Scale Y
					if (pt.x < coordinateX && pt.x > coordinateX - 20)
					{
						XYMinMax.SelectedMenu = 1;
						DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_SCALE), hWnd, SettingsScale, (LPARAM)&XYMinMax);
					}
					//Title Y
					else if (pt.x < coordinateX - 20)
					{
						XYMinMax.SelectedMenu = 4;
						DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_SCALE), hWnd, SettingsScale, (LPARAM)&XYMinMax);
					}
					//Scale X
					else if (pt.y > coordinateY && pt.y < coordinateY + 20)
					{
						XYMinMax.SelectedMenu = 0;
						DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_SCALE), hWnd, SettingsScale, (LPARAM)&XYMinMax);
					}
					//Title X
					else if (pt.y > coordinateY + 20)
					{
						XYMinMax.SelectedMenu = 3;
						DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_SCALE), hWnd, SettingsScale, (LPARAM)&XYMinMax);
					}
					//Title
					else if (pt.y < coordinateY2)
					{
						XYMinMax.SelectedMenu = 2;
						DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_SCALE), hWnd, SettingsScale, (LPARAM)&XYMinMax);
					}				
				}

				// fill MENUITEMINFO structure
				MENUITEMINFO mi = { 0 };
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_SUBMENU;
				mi.hSubMenu = hSubMenu;
	
				// insert menu item with submenu to hPopupMenu
				InsertMenuItem(hPositionMenu, 0,false, &mi);

				GetClientRect(hWnd, &WindowsRect);
				ClientToScreen(hWnd, &pt);

				TrackPopupMenu(GetSubMenu(hPositionMenu, 0), 0, pt.x, pt.y, 0, hWnd, NULL);
				DestroyMenu(hPositionMenu);
				DestroyMenu(hSubMenu);
				break;
		
		}
		WindowsRect.bottom -= STATUS_WINDOW_HIGH;
		InvalidateRectangle(hWnd, &WindowsRect);
		break;

	case WM_RBUTTONDOWN:
		if (zoom_flag || scale_move )
		{//we do zooming
			zoom_track_stop = false;
			zoom_done = false;
			if (data.NrOfPoints == 0 && fit[0].NrOfPoints == 0)
				Initialize_borders();

			AutomaticSetTheScale();
			InvalidateRectangle(hWnd, &WindowsRect);
		}
		if (arrow_flag && (SELECT_GRAPH == WINDOW_ROD || SELECT_GRAPH == WINDOW_IN_PLANE || SELECT_GRAPH == WINDOW_DENSITY))
		{
			ExtractMousePosition(&pt, hWnd, lParam);
			ClientWindowDataPoint = FindDataNumber(hWnd);
			
			//Set state of delete item
			if (ClientWindowDataPoint >= 0) EnableMenuItem(hContextMenu, ID_PLOT_DELETE, MF_ENABLED	);
			else  EnableMenuItem(hContextMenu, ID_PLOT_DELETE, MF_GRAYED);

			if (SELECT_GRAPH == WINDOW_DENSITY) EnableMenuItem(hContextMenu, ID_PLOT_DELETE, MF_GRAYED);

			GetClientRect(hWnd, &WindowsRect);
			ClientToScreen(hWnd, &pt);
			TrackPopupMenu(GetSubMenu(hContextMenu, 0), 0, pt.x, pt.y, 0, hWnd, NULL);
		}
		break;

	case WM_RBUTTONUP:
	{
		if (!zoom_flag)
		{
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

		wheelDelta = MOUSE_WHEEL_DOLLY_SPEED * static_cast<float>(static_cast<int>(wParam) >> 16);

		if (SELECT_GRAPH == WINDOW_ROD || SELECT_GRAPH == WINDOW_DENSITY)
		{
			if (lin_log)
				wheelDelta *= 10.;
			else
				wheelDelta /= 2.;
			XYMinMax.bottom -= wheelDelta;
			XYMinMax.top += wheelDelta;

			if (XYMinMax.top - XYMinMax.bottom < .0001)
			{
				XYMinMax.top -= wheelDelta;
				XYMinMax.bottom += wheelDelta;
			}
		}
		if (SELECT_GRAPH == WINDOW_IN_PLANE)
		{
			point_scale += wheelDelta;

			if (point_scale < -10)
				point_scale = -10;

		}

		if (SELECT_GRAPH == WINDOW_CONTUR)
		{
			wheelDelta *= .1;
			XYMinMax.bottom -= wheelDelta;
			XYMinMax.top += wheelDelta;
			XYMinMax.left -= wheelDelta;
			XYMinMax.right += wheelDelta;
			if (XYMinMax.top - XYMinMax.bottom < .0001)
			{
				XYMinMax.top -= wheelDelta;
				XYMinMax.bottom += wheelDelta;
			}
		}
		InvalidateRectangle(hWnd, &WindowsRect);
	}
		break;


	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Refresh the painting area except toolbar, flickering effect.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Graph2D::InvalidateRectangle(HWND hwnd, RECT *rect){
	
	GetClientRect(hwnd, rect);
	int temp=rect->top; //make a copy
	rect->top += MENU_HEIGHT+3;

	InvalidateRect(hwnd, rect, 1);
	rect->top = temp;				
}




//-----------------------------------------------------------------------------
// Set the borders for painting.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Graph2D::Initialize_borders(void)
{
	XYMinMax.left=0;
	XYMinMax.right=1;
	XYMinMax.top= 10.1;
	XYMinMax.bottom=1.;
}


//-----------------------------------------------------------------------------
// Extract mouse position.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Graph2D::ExtractMousePosition(POINT *lpt, HWND hwnd, LPARAM lparam)
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
void Graph2D::ConvertAtoX(GRAPHSETTINGS MinMax, double A_x, double A_y, RECT ScreenSize, int *X, int *Y){
// input: A_x, A_y - the coordinates from user
//MinMax - minimum and maximum for x and y on the graph coordinates (not screen) 
//output: X, Y - screen cordinates


//X coordinates
double  Xmax;//Xmin,

//corection for scale start
A_x -= MinMax.left; 
//correction for left right frames 
Xmax = ScreenSize.right - RamkaX_left - RamkaX_right;
//Xmin = Xmax * MinMax.left / MinMax.right;
//*X = (int)( (Xmax * A_x / MinMax.right) * ( Xmax / (Xmax - Xmin)) );
*X = (int) floor( Xmax * A_x / (MinMax.right -  MinMax.left ) );
*X += RamkaX_left;


//Y coordinates
double  Ymax;//Ymin,
//corection for scale start
A_y -= MinMax.bottom; 
Ymax = ScreenSize.bottom - RamkaY_top - RamkaY_bottom;
//Ymin = Ymax * MinMax.bottom / MinMax.top;
//*Y = (int)(Ymax- (Ymax * A_y / MinMax.top)*( Ymax / (Ymax - Ymin)) );
*Y = (int) floor( Ymax- (Ymax * A_y / (  MinMax.top - MinMax.bottom )) );
*Y += RamkaY_top;

}

//-----------------------------------------------------------------------------
// Convert position from the screen to variables x, y.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Graph2D::ConvertXtoA(GRAPHSETTINGS MinMax, double *A_x, double *A_y, RECT ScreenSize, int X, int Y){

//X coordinates
double  Xmax;//Xmin,

//corection for scale start
X -= RamkaX_left;//

Xmax = ScreenSize.right - RamkaX_left - RamkaX_right;
if (Xmax<=0.0) Xmax=0.001;
//Xmin = Xmax * MinMax.left / MinMax.right;

//*A_x =  X /  (Xmax / (Xmax - Xmin)) * MinMax.right / Xmax;
*A_x =  X *( MinMax.right  -  MinMax.left ) / ( Xmax )   ;
*A_x += MinMax.left;

//Y coordinates
double Ymax;// Ymin, 
Y -= RamkaY_top;
Ymax = ScreenSize.bottom - RamkaY_top - RamkaY_bottom;
if (Ymax<=0.0) Ymax=0.001;
//Ymin = Ymax * MinMax.bottom / MinMax.top;

*A_y = (Ymax - Y) * (MinMax.top - MinMax.bottom ) / ( Ymax )   ;

*A_y += MinMax.bottom; 
}


//-------------------------------------------------------------------------------------------------------------------------------------
// The main paint function for 2D window.
// 21.01.2013 by Daniel Kaminski
// 01.2015 serious changes in the code  
//-------------------------------------------------------------------------------------------------------------------------------------
void Graph2D::Draw(HWND hwnd, HDC hdcBack_, POINT Pt, RECT rect){

	int i;
	double temp;
	POINT line[2];

	int coordinateX, coordinateY;		//for position on the screen
	double variable_x, variable_y;		//for position in variable space
	double step_scale;					//Step of the scales x and y
	GRAPHSETTINGS temp_zoom;					//for selection of the zoom area

	RECT rect2;
	WCHAR plot_text[1500];

	//SetBkColor(hdcBack_,RGB(255,255,255));
	SetBkMode(hdcBack_, TRANSPARENT);
	FillRect(hdcBack_, &rect, hBru[7]);


	//safety
	if (fabs(XYMinMax.left - XYMinMax.right) < .000001) Initialize_borders();
	if (fabs(XYMinMax.top - XYMinMax.bottom) < .000001) Initialize_borders();


	//-------------------------------------------------------------------------------------------------------------------------------------
	//fill plot area
	//-------------------------------------------------------------------------------------------------------------------------------------
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.left = coordinateX; rect2.top = coordinateY;
	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);

	rect2.bottom = coordinateY; rect2.right = coordinateX;

	FillRect(hdcBack_, &rect2, hBru[1]);

	//-------------------------------------------------------------------------------------------------------------------------------------
	//zoom draw the rectangular
	//-------------------------------------------------------------------------------------------------------------------------------------
	if (fabs(XYMinMax.top - XYMinMax.bottom) < 0.0000001) XYMinMax.bottom += 0.0000001;
	if (fabs(XYMinMax.right - XYMinMax.left) < 0.0000001) XYMinMax.right += 0.0000001;


	if (zoom_frame.left == zoom_frame.right)  zoom_frame.right = zoom_frame.left + 1;
	if (zoom_frame.top == zoom_frame.bottom) zoom_frame.top = zoom_frame.bottom + 1;
	if (zoom_track_stop && !zoom_done){
		SelectObject(hdcBack_, hPen[4]);
		Rectangle(hdcBack_, zoom_frame.left + 1, zoom_frame.top + 1, zoom_frame.right - 1, zoom_frame.bottom - 1);
		FillRect(hdcBack_, &zoom_frame, hBru[5]);
	}

	if (!zoom_track_stop)
	{  //we are not in zooming action
	}
	else {
		if (zoom_done)
		{
			GRAPHSETTINGS temp_XYMinMax;

			temp_XYMinMax.bottom = XYMinMax.bottom;
			temp_XYMinMax.top = XYMinMax.top;
			temp_XYMinMax.left = XYMinMax.left;
			temp_XYMinMax.right = XYMinMax.right;

			if (zoom_frame.top - zoom_frame.bottom < 0){
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

			//set new min, max
			ConvertXtoA(temp_XYMinMax, &variable_x, &variable_y, rect, (int)temp_zoom.left, (int)temp_zoom.top);
			XYMinMax.left = variable_x; XYMinMax.bottom = variable_y;
			ConvertXtoA(temp_XYMinMax, &variable_x, &variable_y, rect, (int)temp_zoom.right, (int)temp_zoom.bottom);
			XYMinMax.right = variable_x; XYMinMax.top = variable_y;
			zoom_done = false;
			zoom_track_stop = false;
		}
	}
	//
	double MinPowY, tmpMinPowY;
	double MaxPowY = pow(10., XYMinMax.top);
	double precision = pow(10., XYMinMax.Yprecision - 1);


	tmpMinPowY = pow(10., XYMinMax.bottom);
	if (tmpMinPowY >= 1)
		MinPowY = ceil(tmpMinPowY);
	else
		MinPowY = tmpMinPowY;

	//Auto scale major ticks
	if (XYMinMax.flag_autoYmajorTicks)
	{
		XYMinMax.majorYticks = (MaxPowY - MinPowY) / 6.;
		if (XYMinMax.majorYticks < 0.001) XYMinMax.majorYticks = 0.001;
		if (XYMinMax.majorYticks > 1) XYMinMax.majorYticks = 1;
	}


	//-------------------------------------------------------------------------------------------------------------------------------------
	//scala X
	//-------------------------------------------------------------------------------------------------------------------------------------
	double tmp_left, tmp_right;
	if (grid){
		SelectObject(hdcBack_, hPen[8]);
		step_scale = SetStepScale(XYMinMax, rect, &tmp_left, &tmp_right);

		if (!XYMinMax.flag_autoXminorTicks)
			step_scale /= XYMinMax.minorXticks;

		for (double scale = tmp_left; scale <= tmp_right; scale += step_scale){
			ConvertAtoX(XYMinMax, scale, XYMinMax.top, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = (int)coordinateY;
			ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
			line[1].x = coordinateX; line[1].y = (int)coordinateY + 3;
			if (line[0].x > 0 && line[1].x > 0 && line[0].y > 0)
				Polyline(hdcBack_, line, 2);
		}

		//-------------------------------------------------------------------------------------------------------------------------------------
		//skala y
		//-------------------------------------------------------------------------------------------------------------------------------------
		if (lin_log) {
			double scale;
			for (scale = XYMinMax.bottom; scale <= XYMinMax.top; scale += step_scale){//
				if (XYMinMax.flag_autoYmajorTicks)
					step_scale = (XYMinMax.top - XYMinMax.bottom) / rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
				else
					step_scale = XYMinMax.majorYticks;
				if (fabs(step_scale) < 0.0001) step_scale = XYMinMax.top;
				ConvertAtoX(XYMinMax, XYMinMax.right, scale, rect, &coordinateX, &coordinateY);
				line[0].x = coordinateX; line[0].y = coordinateY;
				ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
				line[1].x = coordinateX - 4; line[1].y = coordinateY;
				Polyline(hdcBack_, line, 2);
			}
		}
		//log
		else{
			SelectObject(hdcBack_, hPen[8]);
			double MinPowY = pow(10., XYMinMax.bottom);
			double MaxPowY = pow(10., XYMinMax.top);
			double skok;
			double scale;
			double log_scale;
			for (double i = 0; i < XYMinMax.top + XYMinMax.majorYticks; i += XYMinMax.majorYticks)
			{
				skok = pow(10., XYMinMax.top - i);
				if (XYMinMax.flag_autoYminorTicks)
					step_scale = (MaxPowY / skok - MinPowY) / 10.0;
				else
					step_scale = (MaxPowY / skok - MinPowY) / XYMinMax.minorYticks;
				if (fabs(step_scale) < 0.0001) step_scale = XYMinMax.top;

				for (scale = MinPowY; scale <= MaxPowY / skok + 0.001; scale += step_scale)
				{
					log_scale = log10(scale);
					if (log_scale > XYMinMax.bottom && scale < MaxPowY)
					{
						ConvertAtoX(XYMinMax, XYMinMax.right, log_scale, rect, &coordinateX, &coordinateY);
						line[0].x = coordinateX; line[0].y = coordinateY;
						ConvertAtoX(XYMinMax, XYMinMax.left, log_scale, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX - 4; line[1].y = coordinateY;
						Polyline(hdcBack_, line, 2);
					}
				}
				MinPowY = MaxPowY / skok;
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//errors
	//-------------------------------------------------------------------------------------------------------------------------------------
	if (errors_plot){

		SelectObject(hdcBack_, hPen[0]);
		for (i = 0; i < data.NrOfPoints; i++)//
		{
			if (lin_log)
				temp = data.y[i] + data.sigma[i];
			else
				temp = log10(data.y[i] + data.sigma[i]);

			if (temp > XYMinMax.top)
				temp = XYMinMax.top;

			ConvertAtoX(XYMinMax, data.x[i], temp, rect, &coordinateX, &coordinateY);
			line[0].x = line[1].x = coordinateX;//no refraction correction
			line[0].y = coordinateY;

			if (lin_log) 
				temp = data.y[i] - data.sigma[i];
			else
			{
				if (data.y[i] - data.sigma[i] > 1e-20)
					temp = log10(data.y[i] - data.sigma[i]);
				else
					temp = log10(1e-20);
			}
			if (temp < XYMinMax.bottom)
				temp = XYMinMax.bottom;

			ConvertAtoX(XYMinMax, data.x[i], temp, rect, &coordinateX, &coordinateY);
			line[1].y = coordinateY;
			Polyline(hdcBack_, line, 2);

			//top line
			line[0].x -= 3; line[1].x += 4; line[1].y = line[0].y;
			Polyline(hdcBack_, line, 2);

			//bottom line
			line[1].y = line[0].y = (int)(line[1].y = coordinateY);
			Polyline(hdcBack_, line, 2);
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//open data errors
	//-------------------------------------------------------------------------------------------------------------------------------------
	if (errors_plot){
		for (int j = 0; j < MAX_OPEN; ++j)
		{
			if (Opendat[j].status)
			{
				SelectObject(hdcBack_, hPenOpenDat[j]);
				for (i = 0; i < Opendat[j].NrOfPoints; i++)
				{
					if (lin_log)
						temp = Opendat[j].y[i] + Opendat[j].sigma[i];
					else
						temp = log10(Opendat[j].y[i] + Opendat[j].sigma[i]);
					
					if (temp > XYMinMax.top)
						temp = XYMinMax.top;

					ConvertAtoX(XYMinMax, Opendat[j].x[i], temp, rect, &coordinateX, &coordinateY);
					line[0].x = line[1].x = coordinateX;//no refraction correction
					line[0].y = coordinateY;

					if (lin_log)
						temp = Opendat[j].y[i] - Opendat[j].sigma[i];
					else
					{
						if (Opendat[j].y[i] - Opendat[j].sigma[i] > 1e-20)
						temp = log10(Opendat[j].y[i] - Opendat[j].sigma[i]);
						else
							temp = log10(1e-20);
					}

					if (temp < XYMinMax.bottom)
						temp = XYMinMax.bottom;

					ConvertAtoX(XYMinMax, Opendat[j].x[i], temp, rect, &coordinateX, &coordinateY);
					line[1].y = coordinateY;

					Polyline(hdcBack_, line, 2);

					//top line
					line[0].x -= 3; line[1].x += 4; line[1].y = line[0].y;
					Polyline(hdcBack_, line, 2);

					//bottom line
					line[1].y = line[0].y = (int)(line[1].y = coordinateY);
					Polyline(hdcBack_, line, 2);
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//LBragg
	//-------------------------------------------------------------------------------------------------------------------------------------
	if (fit[0].LBragg > 0 && show_LBragg)
	{
		SelectObject(hdcBack_, hPen[5]);
		ConvertAtoX(XYMinMax, fit[0].LBragg, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
		line[0].x = coordinateX;
		line[0].y = coordinateY - 13;
		line[1].x = coordinateX;
		line[1].y = coordinateY;

		Polyline(hdcBack_, line, 2);
		line[1].x = coordinateX - 3;
		line[1].y = coordinateY - 9;
		Polyline(hdcBack_, line, 2);
		line[1].x = coordinateX + 3;
		Polyline(hdcBack_, line, 2);
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//data
	//-------------------------------------------------------------------------------------------------------------------------------------
	if (data_plot){
		if (lin_log) temp = data.y[0];
		else temp = log10(data.y[0]);
		ConvertAtoX(XYMinMax, data.x[0], temp, rect, &coordinateX, &coordinateY);
		line[0].x = coordinateX;
		line[0].y = coordinateY;
		for (i = 0; i < data.NrOfPoints; ++i){//3
			if (lin_log) temp = data.y[i];
			else temp = log10(data.y[i]);
			ConvertAtoX(XYMinMax, data.x[i], temp, rect, &coordinateX, &coordinateY);
			line[1].x = coordinateX;
			line[1].y = coordinateY;
			SelectObject(hdcBack_, hPen[4]);
			Ellipse(hdcBack_, line[1].x - 4, line[1].y - 4, line[1].x + 4, line[1].y + 4);
			if (line_points){
				SelectObject(hdcBack_, hPen[2]);
				Polyline(hdcBack_, line, 2);
				line[0].x = line[1].x; line[0].y = line[1].y;
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//draw open data
	//-------------------------------------------------------------------------------------------------------------------------------------

	for (int j = 0; j < MAX_OPEN; ++j){
		if (Opendat[j].status){
			if (lin_log) temp = Opendat[j].y[0];
			else temp = log10(Opendat[j].y[0]);
			ConvertAtoX(XYMinMax, Opendat[j].x[0], temp, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX;
			line[0].y = coordinateY;
			for (i = 0; i < Opendat[j].NrOfPoints; ++i){
				if (lin_log) temp = Opendat[j].y[i];
				else temp = log10(Opendat[j].y[i]);
				ConvertAtoX(XYMinMax, Opendat[j].x[i], temp, rect, &coordinateX, &coordinateY);
				line[1].x = coordinateX;
				line[1].y = coordinateY;
				DrawSymbol(line, j);
				if (line_points){
					SelectObject(hdcBack_, hPenOpenDat[j]);
					Polyline(hdcBack_, line, 2);
					line[0].x = line[1].x; line[0].y = line[1].y;
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//draw  models
	//-------------------------------------------------------------------------------------------------------------------------------------
	for (int curve = 0; curve < MAX_FIT_CURVES; curve++)
	{
		if (fit[curve].status){

			SelectObject(hdcBack_, hPen[curve + 10]);  //to use only curve collors, later the colors can be editet in fly

			if (lin_log || curve == 3) //curve=3 is phase it should be always linear
			{
				temp = fit[curve].y[0];
				ConvertAtoX(XYMinMax, fit[curve].x[0], temp, rect, &coordinateX, &coordinateY);
				line[0].x = coordinateX; line[0].y = coordinateY;

				for (i = 0; i < fit[curve].NrOfPoints; ++i){
					temp = fit[curve].y[i];
					ConvertAtoX(XYMinMax, fit[curve].x[i], temp, rect, &coordinateX, &coordinateY);
					line[1].x = coordinateX;
					line[1].y = coordinateY; //scaling to the declared size of graph
					Polyline(hdcBack_, line, 2);
					line[0].x = line[1].x; line[0].y = line[1].y;
				}
			}
			else
			{
				temp = log10(fit[curve].y[0]);
				ConvertAtoX(XYMinMax, fit[curve].x[0], temp, rect, &coordinateX, &coordinateY);
				line[0].x = coordinateX; line[0].y = coordinateY;
				for (i = 0; i < fit[curve].NrOfPoints; ++i)
				{
					temp = log10(fit[curve].y[i]);
					ConvertAtoX(XYMinMax, fit[curve].x[i], temp, rect, &coordinateX, &coordinateY);
					line[1].x = coordinateX;
					line[1].y = coordinateY; //scaling to the declared size of graph
					Polyline(hdcBack_, line, 2);
					line[0].x = line[1].x; line[0].y = line[1].y;
				}
			}
		}
	}



	//-------------------------------------------------------------------------------------------------------------------------------------
	//draw  open curves
	//-------------------------------------------------------------------------------------------------------------------------------------
	for (int curve = 0; curve < MAX_OPEN; curve++){
		if (Openfit[curve].status){
			DrawLine(NULL, 0, true);
			SelectObject(hdcBack_, hPenOpen[curve]);  //to use only curve collors, later the colors can be editet in fly
			if (lin_log) temp = fit[curve].y[0];
			else temp = log10(fit[curve].y[0]);
			ConvertAtoX(XYMinMax, fit[curve].x[0], temp, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = coordinateY;
			for (i = 0; i < Openfit[curve].NrOfPoints; ++i){
				if (lin_log) temp = Openfit[curve].y[i];
				else temp = log10(Openfit[curve].y[i]);
				ConvertAtoX(XYMinMax, Openfit[curve].x[i], temp, rect, &coordinateX, &coordinateY);
				line[1].x = coordinateX;
				line[1].y = coordinateY; //scaling to the declared size of graph
				//Polyline(hdcBack_, line, 2);
				DrawLine(line, curve);
				line[0].x = line[1].x; line[0].y = line[1].y;
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//draw blank area
	//-------------------------------------------------------------------------------------------------------------------------------------
	//clean left area
	rect2.left = 0;
	rect2.right = RamkaX_left;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//clean right area
	rect2.left = rect.right - RamkaX_right + 1;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//clean top area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = RamkaY_top;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//clean bottom area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = rect.bottom - RamkaY_bottom + 1;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//frame x
	SelectObject(hdcBack_, hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack_, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack_, line, 2);

	//frame Y
	SelectObject(hdcBack_, hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY + 1;

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack_, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY - 1;
	Polyline(hdcBack_, line, 2);

	//-------------------------------------------------------------------------------------------------------------------------------------
	//Draw text
	//-------------------------------------------------------------------------------------------------------------------------------------
	//Draw title
	//SelectObject(hdcBack_, XYMinMax.MyFont);
	//rect2.left = 300; rect2.top = MENU_HEIGHT + 8; rect2.bottom = rect2.top + 20; rect2.right = rect2.left + 550;
	//swprintf_s(plot_text, HEADER, L"%s", data.Title);
	//DrawText(hdcBack_, plot_text, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	//axes description y
	SelectObject(hdcBack_, XYMinMax.MyFont_Y);
	SetTextColor(hdcBack_, rgbColors.text);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	rect2.left = coordinateX - 55;  rect2.bottom = coordinateY+25; rect2.right = rect2.left + 32;
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.top = coordinateY;
	DrawText(hdcBack_, data.TitleY, -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	//axes description x
	SelectObject(hdcBack_, XYMinMax.MyFont_X);
	ConvertAtoX(XYMinMax, (XYMinMax.right + XYMinMax.left) / 2., XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	rect2.left = coordinateX - 70; rect2.top = coordinateY + 22; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 100;
	DrawText(hdcBack_, data.TitleX, -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	//title
	SelectObject(hdcBack_, XYMinMax.MyFont);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.left = coordinateX; rect2.top = coordinateY - 18; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 400;
	DrawText(hdcBack_, data.header, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	//fill line descryption
	for (int curve = 0; curve < MAX_FIT_CURVES; curve++){
		swprintf_s(plot_text, HEADER, L"%s", fit[curve].header);
		SetTextColor(hdcBack_, color[curve + 10]);
		ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
		rect2.right = coordinateX - 10; rect2.left = rect2.right - 200; rect2.top = coordinateY + 12 * curve; rect2.bottom = rect2.top + 15;
		DrawText(hdcBack_, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//Plot scales and marks
	//-------------------------------------------------------------------------------------------------------------------------------------
	SetTextColor(hdcBack_, RGB(0., 0., 0.));
	//SetTextColor(hdcBack_, rgbColors.text);
	SelectObject(hdcBack_, hPen[0]);
	//scala X
	step_scale = SetStepScale(XYMinMax, rect, &tmp_left, &tmp_right);

	for (double scale = tmp_left; scale <= tmp_right; scale += step_scale)
	{
		ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
		line[0].x = coordinateX; line[0].y = (int)coordinateY;
		line[1].x = coordinateX; line[1].y = (int)coordinateY + 4;
		Polyline(hdcBack_, line, 2);

		rect2.left = coordinateX -30; rect2.top = coordinateY + 7; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 50;
		
		if (XYMinMax.flag_autoXprecision)
		{
			if (XYMinMax.right - XYMinMax.left > 10.)
				swprintf_s(plot_text, 100, L"%6.0f", scale);
			else if (XYMinMax.right - XYMinMax.left > 1.) swprintf_s(plot_text, 100, L"%6.1f", scale);
			else if (XYMinMax.right - XYMinMax.left > 0.1) swprintf_s(plot_text, 100, L"%6.2f", scale);
			else if (XYMinMax.right - XYMinMax.left > 0.01)swprintf_s(plot_text, 100, L"%6.3f", scale);
			else swprintf_s(plot_text, 100, L"%6.4f", scale);
		}
		else
		{
			WCHAR Tformat[10];
			swprintf_s(Tformat, 100, L"%%6.%df", XYMinMax.Xprecision);
			swprintf_s(plot_text, 100, Tformat, scale);
		}

		DrawText(hdcBack_, plot_text, -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}


	//Minor ticks scale X
	if (!XYMinMax.flag_autoXminorTicks)
	{
		step_scale /= XYMinMax.minorXticks;
		for (double scale = tmp_left; scale <= tmp_right; scale += step_scale)
		{
			ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = (int)coordinateY;
			line[1].x = coordinateX; line[1].y = (int)coordinateY + 2;
			Polyline(hdcBack_, line, 2);
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//skala y
	//-------------------------------------------------------------------------------------------------------------------------------------
	if (lin_log) 
	{
		double scale;
		for (scale = XYMinMax.bottom; scale <= XYMinMax.top; scale += step_scale)
		{
			if (XYMinMax.flag_autoYmajorTicks)
			{
				step_scale = (XYMinMax.top - XYMinMax.bottom) / rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
				XYMinMax.majorYticks = step_scale;
			}
			else
				step_scale = XYMinMax.majorYticks;
			if (fabs(step_scale) < 0.0001) step_scale = XYMinMax.top;
			ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = coordinateY;
			line[1].x = coordinateX - 4; line[1].y = coordinateY;
			Polyline(hdcBack_, line, 2);

			rect2.right = coordinateX - 5; rect2.left = rect2.right - 70; rect2.top = coordinateY - 7; rect2.bottom = rect2.top + 15;
			if (XYMinMax.flag_autoYprecision)
			{
				if (XYMinMax.top < 100.) swprintf_s(plot_text, 100, L"%3.2f", scale);
				else
				{
				//	_set_output_format(_TWO_DIGIT_EXPONENT);
					swprintf_s(plot_text, 100, L"%1.0e", scale);
				}
			}
			else 
			{
				WCHAR Tformat[10];
				swprintf_s(Tformat, 100, L"%%6.%df", XYMinMax.Xprecision);
				swprintf_s(plot_text, 100, Tformat, scale);
			}
			DrawText(hdcBack_, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		}
	}
	//-------------------------------------------------------------------------------------------------------------------------------------
	//log
	//-------------------------------------------------------------------------------------------------------------------------------------
	else
	{
		double skok;
		double scale;
		double log_scale;
		WCHAR Tformat[10];

		for (double i = 0; i <= XYMinMax.top + XYMinMax.majorYticks; i += XYMinMax.majorYticks)
		{
			skok = pow(10, (XYMinMax.top - i));

			if (XYMinMax.flag_autoYminorTicks)
				step_scale = (MaxPowY / skok - MinPowY) / 10.0;
			else
				step_scale = (MaxPowY / skok - MinPowY) / XYMinMax.minorYticks;

			if (fabs(step_scale) < 0.00001) step_scale = XYMinMax.top;


			for (scale = MinPowY; scale <= MaxPowY / skok + 0.001; scale += step_scale)
			{
				log_scale = log10(scale);
				if (log_scale > XYMinMax.bottom && scale < MaxPowY)
				{
					ConvertAtoX(XYMinMax, XYMinMax.left, log_scale, rect, &coordinateX, &coordinateY);
					line[0].x = coordinateX;   line[0].y = coordinateY;
					line[1].x = coordinateX - 3; line[1].y = coordinateY;
					Polyline(hdcBack_, line, 2);
				}
			} 
			scale -= step_scale;
			log_scale = log10(scale);
			ConvertAtoX(XYMinMax, XYMinMax.left, log_scale, rect, &coordinateX, &coordinateY);
			rect2.right = coordinateX - 5; rect2.left = rect2.right - 70; rect2.top = coordinateY - 7; rect2.bottom = rect2.top + 15;
			if (log_scale > XYMinMax.bottom && log_scale < XYMinMax.top)
			{
				if (XYMinMax.flag_autoYprecision)
				{
					if (XYMinMax.top < 1.)
						swprintf_s(plot_text, 100, L"%3.2f", log_scale);
					else
						swprintf_s(plot_text, 100, L"%3.1f", log_scale);
				}
				else
				{
					swprintf_s(Tformat, 100, L"%%3.%df", XYMinMax.Yprecision);
					swprintf_s(plot_text, 100, Tformat, log_scale);
				}
				DrawText(hdcBack_, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

				line[0].x = coordinateX;   line[0].y = coordinateY;
				line[1].x = coordinateX - 5; line[1].y = coordinateY;
				Polyline(hdcBack_, line, 2);
			}
			MinPowY = MaxPowY / skok;
		}

	}

	//-------------------------------------------------------------------------------------------------------------------------------------
	//Nice line on the left
	//-------------------------------------------------------------------------------------------------------------------------------------
	SelectObject(hdcBack_, hPen[8]);
	line[1].x = line[0].x = SIDE_BAR_WIDTH + 1;
	line[0].y = 0; line[1].y = rect.bottom;
	Polyline(hdcBack_, line, 2);

	//-------------------------------------------------------------------------------------------------------------------------------------
	//end plot
	//-------------------------------------------------------------------------------------------------------------------------------------
}

//Set the scale x step in drwa function
double Graph2D::SetStepScale(GRAPHSETTINGS XYMinMax, RECT rect, double *tmp_left, double *tmp_right)
{
	double width;
	double step_scale;

	if (XYMinMax.flag_autoXmajorTicks)
	{
		width = (XYMinMax.right - XYMinMax.left);
		step_scale = width / rect.right*NR_OF_POINTS_ON_SCALE_X;

		int Nr_marks = 1 + (int)floor(5.*NR_OF_POINTS_ON_SCALE_X / (rect.right + 0.0001));// floor((1 + rect.right) / (floor(step_scale)*NR_OF_POINTS_ON_SCALE_X));


		step_scale *= Nr_marks;
		if (fabs(step_scale) < 0.0001) step_scale = 0.0001;
		if (step_scale >= 0.01) step_scale = floor(step_scale * 100) / 100;
		if (step_scale >= 0.1) step_scale = floor(step_scale * 10) / 10;
		if (step_scale >= 1.0) step_scale = floor(step_scale);

		XYMinMax.majorXticks = step_scale;
	}

	else
		step_scale = XYMinMax.majorXticks;
	//if (step_scale>0.2) step_scale = 1.;


	//if (fabs(XYMinMax.right - XYMinMax.left) < 2.)
	{
		*tmp_left = XYMinMax.left;
		*tmp_right = XYMinMax.right;
	}
	//else
	{
	//	*tmp_left = floor(XYMinMax.left) + 1;
	//	*tmp_right = floor(XYMinMax.right);
	}
	return step_scale;

}


void Graph2D::DrawSymbol(POINT* line, int j)
{
	HBRUSH hBrush = CreateSolidBrush(Opendat[j].color_fill);
	SelectObject(hdcBack, hPenOpenDat[j]);
	SelectObject(hdcBack, hBrush);

	int size = Opendat[j].thickness;
	POINT p[7];
	RECT rc;
	rc.left = line[1].x - size; rc.top = line[1].y - size; rc.right = line[1].x + size, rc.bottom = line[1].y + size;

	switch (Opendat[j].style){

	case 0:
		Ellipse(hdcBack, rc.left, rc.top, rc.right, rc.bottom);
		
		break;
	case 1:	
		Rectangle(hdcBack, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
		FillRect(hdcBack, &rc, hBrush);
		break;

	case 2:
		Rectangle(hdcBack, rc.left, rc.top, rc.right, rc.bottom);
		break;

	case 3:
		RoundRect(hdcBack, rc.left, rc.top, rc.right, rc.bottom, 7, 7);
		break;

	case 4:			//tringle down
		p[0].x = rc.left; p[0].y = rc.top;
		p[1].x = rc.right; p[1].y = rc.top;
		p[2].x = line[1].x; p[2].y = rc.bottom;
		p[3].x = rc.left; p[3].y = rc.top;
		Polygon(hdcBack, p, 4);
		break;

	case 5:			//tringle up
		p[0].x = line[1].x; p[0].y = rc.top;
		p[1].x = rc.right; p[1].y = rc.bottom;
		p[2].x = rc.left; p[2].y = rc.bottom;
		p[3].x = line[1].x; p[3].y = rc.top;
		Polygon(hdcBack, p, 4);
		break;

	case 6://star
		p[0].x = rc.left; p[0].y = rc.top + size/3;
		p[1].x = rc.right; p[1].y = rc.top + size/3;
		p[2].x = line[1].x; p[2].y = rc.bottom + size/3;
		p[3].x = rc.left; p[3].y = rc.top + size/3;
		Polygon(hdcBack, p, 4);

		p[0].x = line[1].x; p[0].y = rc.top - size/3;
		p[1].x = rc.right; p[1].y = rc.bottom - size/3;
		p[2].x = rc.left; p[2].y = rc.bottom - size/3;
		p[3].x = line[1].x; p[3].y = rc.top - size/3;
		Polygon(hdcBack, p, 4);

		break;

	case 7:			//
		p[0].x = line[1].x; p[0].y = rc.top;
		p[1].x = rc.right; p[1].y = line[1].y;
		p[2].x = line[1].x; p[2].y = rc.bottom;
		p[3].x = rc.left; p[3].y = line[1].y;
		p[4].x = line[1].x; p[4].y = rc.top;
		Polygon(hdcBack, p, 5);
		break;

	case 8:
		MoveToEx(hdcBack, rc.left, rc.top, NULL);
		LineTo(hdcBack, rc.right, rc.bottom);

		MoveToEx(hdcBack, rc.left, rc.bottom, NULL);
		LineTo(hdcBack, rc.right, rc.top);
		break;

	}
	DeleteObject(hBrush);
}

//Fill intermediate points
void Graph2D::DrawLine(POINT *Point,  int j, bool reset){

	HBRUSH hBrush = CreateSolidBrush(Openfit[j].color);
	SelectObject(hdcBack, hPenOpen[j]);
	SelectObject(hdcBack, hBrush);


	static double count = 0;
	float delta_x;
	float delta_y;
	int delta_i;
	RECT rc;

	if (reset)
	{
		count = 0;
		DeleteObject(hBrush);
		return;
	}

	delta_x = (float)(Point[1].x - Point[0].x);
	delta_y = (float)(Point[1].y - Point[0].y);
	//take longer vector
	if (abs((int)delta_x)>abs((int)delta_y))
		delta_i = abs((int)delta_x);
	else
		delta_i = abs((int)delta_y);
	if (delta_i == 0)delta_i= 1;

	delta_x /= delta_i;
	delta_y /= delta_i;
	
	int thickness = Openfit[j].thickness;
	if (thickness == 0)
		thickness = 1;

	for (int i = 0; i<delta_i; ++i)
	{
		int x = (int)(Point[0].x + delta_x*i);
		int y = (int)(Point[0].y + delta_y*i);
		if (x < 0 || y < 0)
		{
			DeleteObject(hBrush);
			return;
		}

		if (LineStyle(j, thickness, count))
		{
			if (Openfit[j].thickness == 0)
				SetPixel(hdcBack, x, y, Openfit[j].color);
			else
			{
				x -= (int)(Openfit[j].thickness / 2);
				y -= (int)(Openfit[j].thickness / 2);
				rc.top = y-1;
				rc.left = x-1;
				rc.right = x + Openfit[j].thickness;
				rc.bottom = y + Openfit[j].thickness;

				Ellipse(hdcBack, rc.left, rc.top, rc.right, rc.bottom);
			}
		}

		if (count > 11 * thickness)
			count = 0;
		count++;
	}
	DeleteObject(hBrush);
}

bool Graph2D::LineStyle(int curve, int thickness, double count){

	bool result = false;

	switch (Openfit[curve].style)
	{
	case 0:
		result = true;
		break;

	case 1:
		if (count>2 * thickness) result = true;
		break;

	case 2:
		if (count>4 * thickness) result = true;
		break;

	case 3:
		if (count>6 * thickness) result = true;
		break;

	case 4:
		if (count>8 * thickness) result = true;
		break;

	case 5:
		if (count>9 * thickness) result = true;
		break;

	case 6:
		if ((count > 2 * thickness && count < 3 * thickness) || (count > 5 * thickness && count < 6 * thickness) || count>8 * thickness) result = true;
		break;

	case 7:
		if ((count > 1 * thickness && count < 4 * thickness) || count>6 * thickness) result = true;
		break;

	case 8:
		if ((count > 2 * thickness && count < 4 * thickness) || count>6 * thickness) result = true;
		break;

	case 9:
		if ((count > 1 * thickness && count < 2 * thickness) || count>9 * thickness) result = true;
		break;
	default:
		result = true;
		break;
	}



		return (result);
}

//-----------------------------------------------------------------------------
// Initialization.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Graph2D::Initialize_Graphisc(HWND hWnd){

	point_scale = -5;
	fit[0].NrOfPoints=fit[1].NrOfPoints=fit[2].NrOfPoints = 0;
	show_LBragg = true;



	if (rgbColors.framecolor==0) rgbColors.framecolor = RGB(250, 245, 245);
	Background_hBru = CreateSolidBrush(rgbColors.framecolor);
	hBru[0] = CreateSolidBrush( RGB(0, 0, 0));
	hBru[1] = CreateSolidBrush( RGB(250, 250, 230));
	hBru[2] = CreateSolidBrush( RGB(200, 0, 0));
	hBru[3] = CreateSolidBrush( RGB(255, 255, 255));
	hBru[4] = CreateSolidBrush( RGB(0, 0, 180));
	hBru[5] = CreateSolidBrush( RGB(100, 100, 250));
	hBru[6] = CreateSolidBrush( RGB(66, 180, 50));
	hBru[7] = CreateSolidBrush( RGB(255, 255, 255));
	hBru[8] = CreateSolidBrush( RGB(200, 200, 200));
	hBru[9] = CreateSolidBrush( RGB(0, 0, 70));

	hPen[0] = CreatePen(PS_SOLID, 1,  RGB(0, 0, 0));
	hPen[1] = CreatePen(PS_SOLID, 1,  RGB(200, 0, 0));
	hPen[2] = CreatePen(PS_SOLID, 1,  RGB(160, 160, 255));
	hPen[3] = CreatePen(PS_SOLID, 1,  RGB(0, 200, 0));
	hPen[4] = CreatePen(PS_SOLID, 1,  RGB(0, 0, 200));
	hPen[5] = CreatePen(PS_SOLID, 1,  RGB(255, 100, 0));
	hPen[6] = CreatePen(PS_SOLID, 2,  RGB(180, 180, 250));//contur second line
	hPen[7] = CreatePen(PS_SOLID, 2,  RGB(250, 180, 180));//contur second line
	hPen[8] = CreatePen(PS_SOLID, 1,  RGB(200, 200, 200)); //grid in 2D
	hPen[9] = CreatePen(PS_SOLID, 2,  RGB(50, 0, 50));
	//curve colors


	color[10] = RGB(150, 0, 0);
	color[11] = RGB(0,   0, 0);
	color[12] = RGB(0, 100, 0);
	color[13] = RGB(0, 10, 100);

	hPen[10] = CreatePen(PS_SOLID, 1,  color[10]);
	hPen[11] = CreatePen(PS_SOLID, 1,  color[11]);
	hPen[12] = CreatePen(PS_SOLID, 1,  color[12]);
	hPen[13] = CreatePen(PS_SOLID, 1, color[13]);

	hBru[10] = CreateSolidBrush( color[10]);
	hBru[11] = CreateSolidBrush( color[11]);
	hBru[12] = CreateSolidBrush( color[12]);	

	if (rgbColors.background==0) rgbColors.background = RGB(250, 250, 230);
	if (rgbColors.text==0) rgbColors.text = RGB(0, 0, 0);
	for (int i = 0; i < MAX_OPEN; i++)
	{
		hPenOpen[i] = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		hPenOpenDat[i] = CreatePen(PS_SOLID, 1, RGB(9, 0, 0));
		
		ZeroMemory(&Opendat[i], sizeof(Data_struct));
		ZeroMemory(&Openfit[i], sizeof(Data_struct));
		Opendat[i].color = RGB(1, 0, 0);
		Openfit[i].color = RGB(1, 0, 0);
		Openfit[i].color_fill = RGB(1, 0, 0);
		Openfit[i].style = 2;
		Opendat[i].readed = false;
		Openfit[i].readed = false;
	}
}


//Set size of frames for different 2d windows
void Graph2D::SetFrames()
{
	//initialize frames for drawing
	switch (SELECT_GRAPH)
	{

	case WINDOW_ROD:
		RamkaX_left = RamkaX_left_2d;
		RamkaX_right = RamkaX_right_2d;
		RamkaY_bottom = RamkaY_bottom_2d;
		RamkaY_top = RamkaY_top_2d + FrameCorrection;
		break;

	case WINDOW_IN_PLANE:
		RamkaX_left = RamkaX_left_in_plane;
		RamkaX_right = RamkaX_right_in_plane;
		RamkaY_bottom = RamkaY_bottom_in_plane;
		RamkaY_top = RamkaY_top_in_plane + FrameCorrection;
		break;

	default:
		RamkaX_left = RamkaX_left_2d;
		RamkaX_right = RamkaX_right_2d;
		RamkaY_bottom = RamkaY_bottom_2d;
		RamkaY_top = RamkaY_top_2d + FrameCorrection;
		break;
	}

}
//-----------------------------------------------------------------------------
// Shutdown.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Graph2D::Shutdown()
{

	for(int i = 0; i < MAX_PEN; i++){ 
		DeleteObject(hPen[i]);
		DeleteObject(hBru[i]);
	}

	for (int i = 0; i < MAX_OPEN; i++){
		DeleteObject(hPenOpen[i]); 
		DeleteObject(hPenOpenDat[i]);
	}


	DeleteObject(Background_hBru);
	DeleteDC(hdcBack);
	DeleteDC(hdcBack2);
}

void Graph2D::ResetScale(){
	if (XYMinMax.flag_autoXscaleMin) XYMinMax.left = 100000.;
	if (XYMinMax.flag_autoXscaleMax) XYMinMax.right = -100000.;
	if (XYMinMax.flag_autoYscaleMax) XYMinMax.top = .00000001;
	if (XYMinMax.flag_autoYscaleMin) XYMinMax.bottom = 10000000;
}

void Graph2D::AutomaticSetTheScale(bool ScaleFromData)
{
	int i;
	double range;
	bool Saved_lin_log;

	//Auto scale is off so nothing to do

	if (ScaleFromData)
		ScaleType = SCALE_TO_POINTS;

	//Reset the scale only if nothing is read
	if (data.NrOfPoints == 0 && fit[0].NrOfPoints == 0)
	{
		ResetScale();
	}

	if (data.NrOfPoints == 0)
	{
		ScaleFromData = false;
		ScaleType = SCALE_TO_CURVE_AND_POINTS;
	}

	Saved_lin_log = lin_log;
	switch (SELECT_GRAPH)
	{

	case WINDOW_MULTIPLOT:
	case WINDOW_ROD:
		ResetScale();
		SetYrangeFromData(.1,.1);
		SetYrangeFromOpenData(.1, .1);
		////Automatic set the y range from fit
		if (ScaleType == SCALE_TO_CURVE_AND_POINTS)//if we want the scale to be calculated only from data
		{
			SetYrangeFromFit(0.05, .05);
			SetYrangeFromOpenFit(0.05, .05);
		}

		SetXrangeFromData();
		SetXrangeFromOpenData();

		range = XYMinMax.right - XYMinMax.left;
		if (XYMinMax.flag_autoXscaleMax) XYMinMax.right += 0.02 * range;
		if (XYMinMax.flag_autoXscaleMin) XYMinMax.left -= 0.02 * range;

		if (ScaleType == SCALE_TO_CURVE_AND_POINTS)//if we want the scale to be calculated only from data
		{ 
			SetXrangeFromFit(0.0);
			SetXrangeFromOpenFit(0.0);
		}
		if (XYMinMax.flag_autoXscaleMin) RoundLeftBorder();
		break;

	case WINDOW_IN_PLANE:
		ResetScale();
		lin_log = true;

		//Automatic set the y range from data
		SetYrangeFromData(0.2, 0.2);
		//Automatic set the y range from fit
		if (ScaleType == SCALE_TO_CURVE_AND_POINTS) //if we want the scale to be calculated only from data
			SetYrangeFromFit(0, 0);

		SetXrangeFromData();
		if (ScaleType == SCALE_TO_CURVE_AND_POINTS) //if we want the scale to be calculated only from data
			SetXrangeFromFit(0.1);

		break;

	case WINDOW_CONTUR:
		lin_log = true;
		ResetScale();
		//Automatic set the x range from data
		for (i = 0; i < Contur.NxCon; i++){
			//marygin = MARYGIN * (XYMinMax.right - XYMinMax.left);
			if (Contur.x[i] > XYMinMax.right) XYMinMax.right = Contur.x[i];
			if (Contur.x[i] < XYMinMax.left)  XYMinMax.left = Contur.x[i];
		}

		for (i = 0; i < Contur.NyCon; i++){

			if (Contur.y[i] > XYMinMax.top)    XYMinMax.top = Contur.y[i];
			if (Contur.y[i] < XYMinMax.bottom) XYMinMax.bottom = Contur.y[i];
		}
		range = MARYGIN * (XYMinMax.top - XYMinMax.bottom);
		if (XYMinMax.flag_autoYscaleMax) XYMinMax.top += range;
		break;

		//Electron density
	case WINDOW_DENSITY:
		ResetScale();
		lin_log = true;

		//Automatic set the y range from data
		//SetYrangeFromData(0, 0);
		//Automatic set the y range from fit
		//if (ScaleType == SCALE_TO_CURVE_AND_POINTS) //if we want the scale to be calculated only from data
		SetYrangeFromFit(0.2, 0);

		//SetXrangeFromData(0.1);
		//if (ScaleType == SCALE_TO_CURVE_AND_POINTS) //if we want the scale to be calculated only from data
		SetXrangeFromFit(0.1);
		if (XYMinMax.flag_autoXscaleMax) RoundLeftBorder();
		break;

	case WINDOW_DISPERSION:
		ResetScale();
		lin_log = true;
		//Automatic set the y range from fit
		SetYrangeFromFit(0.03, 0.03);
		SetXrangeFromFit(0.01);
		if (XYMinMax.flag_autoXscaleMax) RoundLeftBorder();
		break;

	}
	//To avoid problems with plot rescaling in in-plane mode
	lin_log = Saved_lin_log;

}

void Graph2D::RoundLeftBorder()
{
	//Round the left border
	//to start scale marks and real scale from the same value
	double range = (XYMinMax.right - XYMinMax.left);

	if (range > 1)
		XYMinMax.left = floor(XYMinMax.left );
	else if (range > 0.1)
		XYMinMax.left = floor(XYMinMax.left*10)/10;
	else if (range < 0.01)
		XYMinMax.left = floor(XYMinMax.left * 100) / 100;
	else
		XYMinMax.left = floor(XYMinMax.left*1000)/1000;

}

void Graph2D::RoundRightBorder()
{
	//Round the left border
	//to start scale marks and real scale from the same value
	double range = 1.0 / (XYMinMax.right - XYMinMax.left);
	if (range == 0) range = 1.;
	if (range < 0.03)
		XYMinMax.right = ceil(XYMinMax.right * 100) / 100;
	else if (range < 0.3)
		XYMinMax.right = ceil(XYMinMax.right * 10) / 10;
	else if (range < 3)
		XYMinMax.right = ceil(XYMinMax.right);
}


//Necessary in manual mode
void Graph2D::ConvertLogScaleToLinScale()
{

	if (lin_log)
	{
		if (!XYMinMax.flag_autoYscaleMax) XYMinMax.top = pow(10.0, XYMinMax.top);
		if (!XYMinMax.flag_autoYscaleMin) XYMinMax.bottom = pow(10.0, XYMinMax.bottom);
	}
	else
	{
		if (!XYMinMax.flag_autoYscaleMax)
		{
			if (XYMinMax.top <= 0) XYMinMax.top = 0.1;
			XYMinMax.top = log10(XYMinMax.top);
		}
		if (!XYMinMax.flag_autoYscaleMin){
			if (XYMinMax.bottom <= 0) XYMinMax.bottom = 0.1;
			XYMinMax.bottom = log10(XYMinMax.bottom);
		}
	}
}

void Graph2D::SetYrangeFromData(double marygin_up, double marygin_bottom)
{
	int i;
	if (lin_log)
	{
		for (i = 0; i < data.NrOfPoints; ++i){
			if (XYMinMax.flag_autoYscaleMax && data.y[i] > XYMinMax.top)    XYMinMax.top = data.y[i];
			if (XYMinMax.flag_autoYscaleMin &&data.y[i] < XYMinMax.bottom) XYMinMax.bottom = data.y[i];
		}

		TestBorderY(marygin_up, marygin_bottom);
	}
	else {
		for (i = 0; i < data.NrOfPoints; ++i)
		{
			if (XYMinMax.flag_autoYscaleMax && log10(data.y[i]) > XYMinMax.top)    XYMinMax.top = log10(data.y[i]);
			if (data.y[i] > 0)
			if (XYMinMax.flag_autoYscaleMin && log10(data.y[i]) < XYMinMax.bottom) XYMinMax.bottom = log10(data.y[i]);
		//	if (XYMinMax.flag_autoYscaleMin && XYMinMax.bottom < 1e-10) XYMinMax.bottom = 1e-10;
		}

		TestBorderY(marygin_up, marygin_bottom);
	}
}

void Graph2D::SetYrangeFromOpenData(double marygin_up, double marygin_bottom)
{

	int i;
	for (int j = 0; j < MAX_OPEN; ++j)
	{
		if (Opendat[j].status)
		{
			if (lin_log){
				for (i = 0; i < Opendat[j].NrOfPoints; ++i)
				{
					if (XYMinMax.flag_autoYscaleMax && Opendat[j].y[i] > XYMinMax.top)    XYMinMax.top = Opendat[j].y[i];
					if (XYMinMax.flag_autoYscaleMin && Opendat[j].y[i] < XYMinMax.bottom) XYMinMax.bottom = Opendat[j].y[i];
				}

				TestBorderY(marygin_up, marygin_bottom);
			}
			else {
				for (i = 0; i < Opendat[j].NrOfPoints; ++i)
				{
					if (XYMinMax.flag_autoYscaleMax && log10(Opendat[j].y[i]) > XYMinMax.top)    XYMinMax.top = log10(Opendat[j].y[i]);
					if (Opendat[j].y[i] > 0)
					if (XYMinMax.flag_autoYscaleMin && log10(Opendat[j].y[i]) < XYMinMax.bottom) XYMinMax.bottom = log10(Opendat[j].y[i]);
					//if (XYMinMax.flag_autoYscaleMin && XYMinMax.bottom < 1e-10) XYMinMax.bottom = 1e-10;
				}

				TestBorderY(marygin_up, marygin_bottom);
			}
		}
	}
}

void Graph2D::SetYrangeFromFit(double marygin_up, double marygin_bottom)
{

	int i;
	for (int j = 0; j < 2; ++j)
	{
		if (fit[j].status) {
			if (lin_log) {
				for (i = 0; i < fit[j].NrOfPoints; ++i)
				{
					if (XYMinMax.flag_autoYscaleMax && fit[j].y[i] > XYMinMax.top)    XYMinMax.top = fit[j].y[i];
					if (XYMinMax.flag_autoYscaleMin && fit[j].y[i] < XYMinMax.bottom) XYMinMax.bottom = fit[j].y[i];
				}

				TestBorderY(marygin_up, marygin_bottom);
			}
			else {
				for (i = 0; i < fit[j].NrOfPoints; ++i)
				{
					if (XYMinMax.flag_autoYscaleMax && log10(fit[j].y[i]) > XYMinMax.top)    XYMinMax.top = log10(fit[j].y[i]);
					if (fit[j].y[i] > 0)
						if (XYMinMax.flag_autoYscaleMin && log10(fit[j].y[i]) < XYMinMax.bottom) XYMinMax.bottom = log10(fit[j].y[i]);
					//if (XYMinMax.flag_autoYscaleMin && XYMinMax.bottom < 1e-20) XYMinMax.bottom = 1e-20;
				}

				TestBorderY(marygin_up, marygin_bottom);
			}
		}
	}
}

void Graph2D::SetYrangeFromOpenFit(double marygin_up, double marygin_bottom)
{

	int i;
	for (int j = 0; j < MAX_OPEN; ++j)
	{
		if (Openfit[j].status)
		{
			if (lin_log)
			{
				for (i = 0; i < Openfit[j].NrOfPoints; i++)
				{
					if (XYMinMax.flag_autoYscaleMax && Openfit[j].y[i] > XYMinMax.top)    XYMinMax.top = Openfit[j].y[i];
					if (XYMinMax.flag_autoYscaleMin && Openfit[j].y[i] < XYMinMax.bottom) XYMinMax.bottom = Openfit[j].y[i];
				}

				TestBorderY(marygin_up, marygin_bottom);
			}
			else {
				for (i = 0; i < Openfit[j].NrOfPoints; i++)
				{
					if (XYMinMax.flag_autoYscaleMax && log10(Openfit[j].y[i]) > XYMinMax.top)    XYMinMax.top = log10(Openfit[j].y[i]);
					if (Openfit[j].y[i] >0)
					if (XYMinMax.flag_autoYscaleMin && log10(Openfit[j].y[i]) < XYMinMax.bottom) XYMinMax.bottom = log10(Openfit[j].y[i]);
					//if (XYMinMax.flag_autoYscaleMin && XYMinMax.bottom < 0.1) XYMinMax.bottom = 0.1;
				}
				
				TestBorderY(marygin_up, marygin_bottom);
			}
		}
	}
}




void Graph2D::TestBorderY(double marygin_up, double marygin_bottom)
{

	if (XYMinMax.flag_autoYscaleMax && XYMinMax.flag_autoYscaleMin)
	{
		double	marygin = XYMinMax.top - XYMinMax.bottom;
		if (marygin < 1e-10)
		{
			marygin = 1e-10;
			return;
		}
		if (marygin > 100) return;
		XYMinMax.top += marygin * marygin_up;
		XYMinMax.bottom -= marygin * marygin_bottom;
	}
}

//Automatic set the x range from data values
void Graph2D::SetXrangeFromData()
{
	for (int i = 0; i < data.NrOfPoints; i++)
	{
		if (XYMinMax.flag_autoXscaleMax && data.x[i] > XYMinMax.right) XYMinMax.right = data.x[i];
		if (XYMinMax.flag_autoXscaleMin && data.x[i] < XYMinMax.left)  XYMinMax.left = data.x[i];
	}
}

void Graph2D::SetXrangeFromOpenData()
{
	for (int j = 0; j < MAX_OPEN; ++j)
	{
		if (Opendat[j].status)
		{
			for (int i = 0; i < Opendat[j].NrOfPoints; i++)
			{
				if (XYMinMax.flag_autoXscaleMax && Opendat[j].x[i] > XYMinMax.right) XYMinMax.right = Opendat[j].x[i];
				if (XYMinMax.flag_autoXscaleMin && Opendat[j].x[i] < XYMinMax.left)  XYMinMax.left = Opendat[j].x[i];
			}
		}
	}

}


//Automatic set the x range from fit values
void Graph2D::SetXrangeFromFit(double marygin)
{

	for (int i = 0; i < fit[0].NrOfPoints; i++)
	{
		if (XYMinMax.flag_autoXscaleMax && fit[0].x[i] > XYMinMax.right) XYMinMax.right = (fit[0].x[i]);
		if (XYMinMax.flag_autoXscaleMin && fit[0].x[i] < XYMinMax.left)  XYMinMax.left = floor(fit[0].x[i]);//-  MARYGIN * (XYMinMax.right - XYMinMax.left);
	}
	if (XYMinMax.flag_autoXscaleMax) XYMinMax.right += marygin * (XYMinMax.right - XYMinMax.left);
}


void Graph2D::SetXrangeFromOpenFit(double marygin)
{
	for (int j = 0; j < MAX_OPEN; ++j)
	{
		if (Openfit[j].status){
			for (int i = 0; i < Openfit[j].NrOfPoints; i++){
				if (XYMinMax.flag_autoXscaleMax && Openfit[j].x[i] > XYMinMax.right) XYMinMax.right = Openfit[j].x[i];
				if (XYMinMax.flag_autoXscaleMin && Openfit[j].x[i] < XYMinMax.left)  XYMinMax.left = Openfit[j].x[i];//-  MARYGIN * (XYMinMax.right - XYMinMax.left);
			}
		}
	}

	if (XYMinMax.flag_autoXscaleMax) XYMinMax.right += marygin * (XYMinMax.right - XYMinMax.left);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// The main paint function for 2D window.
// 21.01.2013 by Daniel Kaminski
//This function plots in-plane data (calculated and/or experimental)
//The logaritmic scale is removed because is usless
//-----------------------------------------------------------------------------
void Graph2D::DrawInPlane(HWND hwnd, HDC hdcBack_, POINT Pt, RECT rect){

int i;
POINT line[2];
int temp_z;
int coordinateX, coordinateY;		//for position on the screen
double variable_x, variable_y;		//for position in variable space

double step_scale;					//Step of the scales x and y
GRAPHSETTINGS temp_zoom;					//for selection of the zoom area

RECT rect2;
WCHAR plot_text[1500];
SetBkMode(hdcBack_, TRANSPARENT );
FillRect(hdcBack_, &rect, hBru[7] );

//safety
if( fabs(XYMinMax.left-XYMinMax.right)<.0001) Initialize_borders();
if( fabs(XYMinMax.top-XYMinMax.bottom)<.0001) Initialize_borders();


//--------------------------------------------------------------------------------------------------
//fill plot area
//--------------------------------------------------------------------------------------------------
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.left=coordinateX; rect2.top=coordinateY;
	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	
	rect2.bottom = coordinateY; rect2.right = coordinateX; 
	FillRect(hdcBack_, &rect2, hBru[1] );

//--------------------------------------------------------------------------------------------------
//zoom draw the rectangular
//--------------------------------------------------------------------------------------------------
	if(fabs(XYMinMax.top-XYMinMax.bottom) < 0.0000001) XYMinMax.bottom += 0.0000001;
	if(fabs(XYMinMax.right-XYMinMax.left) < 0.0000001) XYMinMax.right  += 0.0000001;
	
	
	if(zoom_frame.left == zoom_frame.right)  zoom_frame.right = zoom_frame.left  + 1; //safety
	if(zoom_frame.top  == zoom_frame.bottom) zoom_frame.top  = zoom_frame.bottom + 1;
	if ( zoom_track_stop && !zoom_done){
		SelectObject(hdcBack_,hPen[4]);
		Rectangle(hdcBack_, zoom_frame.left+1, zoom_frame.top+1, zoom_frame.right-1, zoom_frame.bottom-1 );
		FillRect(hdcBack_, &zoom_frame,  hBru[5] );
	}

if ( !zoom_track_stop ){  //we are not in zooming action
}
else {
	if (zoom_done){
		GRAPHSETTINGS temp_XYMinMax;

			temp_XYMinMax.bottom=XYMinMax.bottom;
			temp_XYMinMax.top=XYMinMax.top;
			temp_XYMinMax.left=XYMinMax.left;
			temp_XYMinMax.right=XYMinMax.right;

		if (zoom_frame.top-zoom_frame.bottom<0   ){
			temp_zoom.bottom=zoom_frame.top;
			temp_zoom.top=zoom_frame.bottom;
		}
		else{
			temp_zoom.bottom=zoom_frame.bottom;
			temp_zoom.top=zoom_frame.top;
		}

		if (zoom_frame.right < zoom_frame.left   ){
			temp_zoom.left=zoom_frame.right;
			temp_zoom.right=zoom_frame.left;
		}
		else{
			temp_zoom.left=zoom_frame.left;
			temp_zoom.right=zoom_frame.right;
		}

		ConvertXtoA(temp_XYMinMax,  &variable_x, &variable_y, rect, (int)temp_zoom.left, (int)temp_zoom.top);
		XYMinMax.left=variable_x; XYMinMax.bottom=variable_y;
		ConvertXtoA(temp_XYMinMax,  &variable_x, &variable_y, rect, (int)temp_zoom.right, (int)temp_zoom.bottom);
		XYMinMax.right=variable_x; XYMinMax.top=variable_y;
		zoom_done=false;
		zoom_track_stop=false;


	}
}

//--------------------------------------------------------------------------------------------------
//scala X
//--------------------------------------------------------------------------------------------------
	double scale;
	if(grid)
	{
		SelectObject(hdcBack_,hPen[8]);
		if (XYMinMax.flag_autoXmajorTicks)
			step_scale = 1;
		else
			step_scale = XYMinMax.majorXticks;

		for (scale = (int)XYMinMax.left; scale <= (int)XYMinMax.right; scale += (int)step_scale)
		{
			ConvertAtoX(XYMinMax, scale + XYMinMax.top*cos(fit[0].RLAT[5]), XYMinMax.top, rect, &coordinateX, &coordinateY);
			line[0].x= coordinateX; line[0].y=(int)coordinateY-1;
			ConvertAtoX(XYMinMax,  scale + XYMinMax.bottom*cos(fit[0].RLAT[5]), XYMinMax.bottom, rect, &coordinateX, &coordinateY);
			line[1].x= coordinateX; line[1].y=(int)coordinateY-1;
			SelectObject(hdcBack_,hPen[8]);
			Polyline(hdcBack_, line, 2);
		}

		ConvertAtoX(XYMinMax, 0+ XYMinMax.top*cos(fit[0].RLAT[5]), XYMinMax.top, rect, &coordinateX, &coordinateY);
		line[0].x = coordinateX; line[0].y = (int)coordinateY - 1;
		ConvertAtoX(XYMinMax, 0 + XYMinMax.bottom*cos(fit[0].RLAT[5]), XYMinMax.bottom, rect, &coordinateX, &coordinateY);
		line[1].x = coordinateX; line[1].y = (int)coordinateY - 1;
		SelectObject(hdcBack_, hPen[0]);
		Polyline(hdcBack_, line, 2);
//--------------------------------------------------------------------------------------------------
//skala y
//--------------------------------------------------------------------------------------------------
		step_scale = 1;
		for (scale = (int)XYMinMax.bottom; scale <= (int)XYMinMax.top; scale += (int)step_scale)
		{//
			ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
			line[0].x= coordinateX; line[0].y=coordinateY;
			ConvertAtoX(XYMinMax, XYMinMax.right, scale, rect, &coordinateX, &coordinateY);
			line[1].x= coordinateX-4; line[1].y=coordinateY;
			SelectObject(hdcBack_,hPen[8]);
			Polyline(hdcBack_, line, 2);
		}
	}

	ConvertAtoX(XYMinMax, XYMinMax.left, 0, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;
	ConvertAtoX(XYMinMax, XYMinMax.right, 0, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX - 4; line[1].y = coordinateY;
	SelectObject(hdcBack_, hPen[0]);
	Polyline(hdcBack_, line, 2);

	double SCALE_FACTOR_LOG = 10.+point_scale;//0./(XYMinMax.bottom-XYMinMax.top);
	double SCALE_FACTOR_LIN = 0.2+point_scale/35;//./(XYMinMax.bottom-XYMinMax.top);
//--------------------------------------------------------------------------------------------------
//draw  model
//--------------------------------------------------------------------------------------------------
	for (int curve = 0; curve < MAX_FIT_CURVES; ++curve){
		if(fit[curve].status == true){ //if status is true than plot
			SelectObject(hdcBack_,hPen[curve + 10]); SelectObject(hdcBack_,hBru[curve + 10]); //to use only curve collors, later the colors can be editet in fly
			for (i=0; i<fit[curve].NrOfPoints; i++){ 
				if (lin_log) temp_z = (int)(fit[curve].z[i] * SCALE_FACTOR_LIN);
				else  temp_z = (int)(log10(fit[curve].z[i]) * SCALE_FACTOR_LOG);
				if (temp_z<0) temp_z=0;
				//temp_x = fit.x[i] + fit.y[i]*cos(fit.RLAT[5]); //corection for gamma
				ConvertAtoX(XYMinMax, fit[curve].x[i], fit[curve].y[i], rect, &coordinateX, &coordinateY);
				//SelectObject(hdcBack_,hPen[4]); SelectObject(hdcBack_,hBru3);
				if(!data_plot || data.NrOfPoints==0)
					Ellipse( hdcBack_, coordinateX-temp_z, coordinateY-temp_z, coordinateX+temp_z, coordinateY+temp_z);
				else
					Pie( hdcBack_, coordinateX-temp_z, coordinateY-temp_z, coordinateX+temp_z, coordinateY+temp_z,
					     coordinateX, coordinateY-temp_z, coordinateX, coordinateY+temp_z);
			}
		}
	}


	if (errors_plot){
		SelectObject(hdcBack_,hPen[0]);
		for(i=0; i<data.NrOfPoints; i++){
 			if (lin_log) temp_z = (int)((data.z[i] + data.sigma[i]) * SCALE_FACTOR_LIN);
			else         temp_z = (int)(log10(data.z[i] + data.sigma[i]) * SCALE_FACTOR_LOG);
			ConvertAtoX(XYMinMax, data.x[i], data.y[i], rect, &coordinateX, &coordinateY);
			line[0].x = line[1].x = coordinateX;//no refraction correction
			line[0].y = coordinateY+temp_z;	
			int temp3=line[1].y = coordinateY-temp_z;
			Polyline(hdcBack_, line, 2);
			//top line
			line[0].x-=3; line[1].x+=4; line[1].y=line[0].y;
			Polyline(hdcBack_, line, 2);
			//bottom line
			line[1].y=line[0].y = temp3;
			Polyline(hdcBack_, line, 2);		
		}
	}

//--------------------------------------------------------------------------------------------------
//draw  data
//--------------------------------------------------------------------------------------------------
	if (data_plot)
	{
		for(i=0; i<data.NrOfPoints; ++i){
			if (lin_log) temp_z = (int)(      (data.z[i] * SCALE_FACTOR_LIN)); 
			else         temp_z = (int)(log10(data.z[i]) * SCALE_FACTOR_LOG);
			//temp_x = data.x[i] + data.y[i]*cos(fit.RLAT[5]); //corection for gamma
			ConvertAtoX(XYMinMax, data.x[i], data.y[i], rect, &coordinateX, &coordinateY);
			SelectObject(hdcBack_,hPen[4]); SelectObject(hdcBack_,hBru[3]);

			if (fit[0].status == false && fit[1].status == false && fit[2].status == false)
				Ellipse( hdcBack_, coordinateX-temp_z, coordinateY-temp_z, coordinateX+temp_z, coordinateY+temp_z);
			else
				Pie( hdcBack_, coordinateX-temp_z, coordinateY-temp_z, coordinateX+temp_z, coordinateY+temp_z,
				     coordinateX, coordinateY+temp_z, coordinateX, coordinateY-temp_z);
		}
	}


//--------------------------------------------------------------------------------------------------
//Plot scales and marks
//--------------------------------------------------------------------------------------------------
	if (grid)
	{
		SelectObject(hdcBack_, hPen[0]);
		SelectObject(hdcBack_, XYMinMax.MyFont_Small);
		//scala X
		if (XYMinMax.flag_autoXmajorTicks)
			step_scale = 1;
		else
			step_scale = XYMinMax.majorXticks;

		for (scale = (int)XYMinMax.left; scale <= (int)XYMinMax.right; scale += (int)step_scale)
		{
			ConvertAtoX(XYMinMax, scale, 0, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = (int)coordinateY;
			line[1].x = coordinateX; line[1].y = (int)coordinateY + 3;
			Polyline(hdcBack_, line, 2);

			rect2.left = coordinateX - 16; rect2.top = coordinateY + 9; rect2.bottom = rect2.top + 13; rect2.right = rect2.left + 33;
			if (XYMinMax.flag_autoXprecision)
				swprintf_s(plot_text, 100, L"%1.0f", scale);
			else
			{
				WCHAR Tformat[10];
				swprintf_s(Tformat, 100, L"%%2.%df", XYMinMax.Xprecision);
				swprintf_s(plot_text, 100, Tformat, scale);
			}
			DrawText(hdcBack_, plot_text, -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}

		//skala y
		step_scale = 1;
		for (scale = (int)XYMinMax.bottom; scale <= (int)XYMinMax.top; scale += (int)step_scale)
		{//

			ConvertAtoX(XYMinMax, 0 + scale * cos(fit[0].RLAT[5]), scale, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = coordinateY;
			line[1].x = coordinateX - 4; line[1].y = coordinateY;
			Polyline(hdcBack_, line, 2);
			rect2.left = coordinateX - 25; rect2.top = coordinateY - 7; rect2.bottom = rect2.top + 15; rect2.right = rect2.left + 15;
			if (XYMinMax.top < 10.) swprintf_s(plot_text, 100, L"%1.0f", scale);
			else swprintf_s(plot_text, 100, L"%1.0f", scale);
			DrawText(hdcBack_, plot_text, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		}
	}


	//--------------------------------------------------------------------------------------------------
	//draw blank area
	//--------------------------------------------------------------------------------------------------
	//clean left area
	rect2.left = 0;
	rect2.right = RamkaX_left;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//clean right area
	rect2.left = rect.right - RamkaX_right + 1;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//clean top area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = RamkaY_top;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//clean bottom area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = rect.bottom - RamkaY_bottom + 1;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru);

	//frame x
	SelectObject(hdcBack_, hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack_, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack_, line, 2);

	//frame Y
	SelectObject(hdcBack_, hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY + 1;

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY;
	Polyline(hdcBack_, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x = coordinateX; line[1].y = coordinateY - 1;
	Polyline(hdcBack_, line, 2);

//--------------------------------------------------------------------------------------------------
//Draw text
//--------------------------------------------------------------------------------------------------
	
	SelectObject(hdcBack_, XYMinMax.MyFont);
	//Draw title
	swprintf_s(plot_text, HEADER, L"%s", data.header);
	//if(data.h==0. && data.k==0. && data.l==0) wsprintf(plot_text, L"");

	SetTextColor(hdcBack_, rgbColors.text);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.left=coordinateX; rect2.top=coordinateY-18; rect2.bottom=rect2.top+20; rect2.right=rect2.left+350;  
	DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
/*
	//draw position
	ConvertXtoA(XYMinMax,  &variable_x, &variable_y, rect, Pt.x, Pt.y);
	rect2.left=10; rect2.top=MENU_HEIGHT+3; rect2.bottom=rect2.top+15; rect2.right=rect2.left+170; 
	swprintf_s(plot_text, L"h = %3.2f, k = %3.2f", variable_x+ variable_y * cos(fit.RLAT[5]), variable_y);
	DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
*/
	//axes description h
	wsprintf(plot_text, L"%s", data.TitleX);
	//SetTextColor(hdcBack_, RGB(0,0,0));
	ConvertAtoX(XYMinMax, XYMinMax.right, 0, rect, &coordinateX, &coordinateY);
	rect2.left=coordinateX-15; rect2.top=coordinateY+3; rect2.bottom=rect2.top+15; rect2.right=rect2.left+10;  
	DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	//axes description k
	swprintf_s(plot_text,10, L"%s", data.TitleY);
	//SetTextColor(hdcBack_, RGB(0,0,0));
	ConvertAtoX(XYMinMax, 0 + XYMinMax.top * cos(fit[0].RLAT[5]) , XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.left=coordinateX-25; rect2.right=rect2.left+22; rect2.bottom=coordinateY+1; rect2.top=rect2.bottom+14; 
	DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);


	SelectObject(hdcBack_, hPen[8]);
	line[1].x = line[0].x = SIDE_BAR_WIDTH + 1;
	line[0].y = 0; line[1].y = rect.bottom;
	Polyline(hdcBack_, line, 2);
//--------------------------------------------------------------------------------------------------
//end plot
//--------------------------------------------------------------------------------------------------
}

void Graph2D::UpdateStatusBar()
{
	WCHAR lpBuffer1[HEADER];
	swprintf_s(lpBuffer1, HEADER, L"%s", data.Title);
	SendMessage(hwndSB, SB_SETTEXT, 0, (LPARAM)lpBuffer1);
	swprintf_s(lpBuffer1, HEADER, L" %d", data.NrOfPoints);
	SendMessage(hwndSB, SB_SETTEXT, 1, (LPARAM)lpBuffer1);
	swprintf_s(lpBuffer1, HEADER, L"---, ---");
	SendMessage(hwndSB, SB_SETTEXT, 2, (LPARAM)lpBuffer1);
}

void Graph2D::DrawStatusBar(HWND hwnd, POINT Pt, RECT rect){

	WCHAR lpBuffer1[HEADER], lpBuffer2[HEADER];
double variable_x, variable_y;		//for position in variable space

swprintf_s(lpBuffer1, HEADER, L"%s", data.Title);
SendMessage(hwndSB, SB_SETTEXT, 0, (LPARAM)lpBuffer1);

if (pt.x == -1)
{
	int iStatusBarWidths[] = { rect.right - 200, rect.right - 150, - 1 };
	SendMessage(hwnd, SB_SETPARTS, 3, (LPARAM)iStatusBarWidths);
	swprintf_s(lpBuffer1, 200, L"---, ---");
	SendMessage(hwndSB, SB_SETTEXT, 2, (LPARAM)lpBuffer1);
	return;
}
swprintf_s(lpBuffer2, HEADER, L"");
	//draw position
	switch (SELECT_GRAPH)
	{
	case WINDOW_ROD:
		swprintf_s(lpBuffer2, HEADER, L" %d", data.NrOfPoints);
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		if (!STRUCFAC)
		{
			if (lin_log)
				swprintf_s(lpBuffer1, L"l = %6.3f, F² = %6.3f", variable_x, variable_y);
			else
				swprintf_s(lpBuffer1, L"l = %3.3f, F² = %3.3f", variable_x, pow(10., variable_y));
			break;
		}
		else
		{
			if (lin_log)
				swprintf_s(lpBuffer1, L"l = %6.3f, F = %6.3f", variable_x, variable_y);
			else
				swprintf_s(lpBuffer1, L"l = %3.3f, F = %3.3f", variable_x, pow(10., variable_y));
			break;
		}
		break;
	case  WINDOW_DENSITY:
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		if (lin_log){
			swprintf_s(lpBuffer1, L"z = %6.3f, E = %6.3f", variable_x, variable_y);
		}
		else {
			swprintf_s(lpBuffer1, L"z = %3.3f, E = %3.3f", variable_x, pow(10., variable_y));
		}
		break;

	case   WINDOW_DISPERSION:
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		if (lin_log){
			swprintf_s(lpBuffer1, L"En = %6.3f, y = %6.3f", variable_x, variable_y);
		}
		else {
			swprintf_s(lpBuffer1, L"En = %3.3f, y = %3.3f", variable_x, pow(10., variable_y));
		}
		break;

	case   WINDOW_IN_PLANE:
		swprintf_s(lpBuffer2, HEADER, L" %d", data.NrOfPoints);
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		swprintf_s(lpBuffer1, L"h = %6.3f, k = %6.3f", variable_x - variable_y * cos(fit[0].RLAT[5]), variable_y);
		break;

	case    WINDOW_CONTUR:
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		swprintf_s(lpBuffer1, L"x = %6.3f, y = %6.3f", variable_x, variable_y);
		break;

	case   WINDOW_MULTIPLOT:
		ConvertXtoA(XYMinMax, &variable_x, &variable_y, rect, Pt.x, Pt.y);
		swprintf_s(lpBuffer1, L"");
		break;
	}
	SendMessage(hwnd, SB_SETTEXT, 1, (LPARAM)lpBuffer2);
	SendMessage(hwnd, SB_SETTEXT, 2, (LPARAM)lpBuffer1);
}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: copy 3d sufrace to clipobard
//Created by Daniel Kaminski
// 18.02.2013 Lublin
//-----------------------------------------------------------------------------------------------------------------
void Graph2D::CopyToClipobard(HWND hwnd)
{
	
	RECT rect; POINT pt;
	rect.bottom = rgbColors.BMP_Height; rect.right = rgbColors.BMP_Width;
	pt.x = 0; pt.y = 0;
	
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcBack, rect.right, rect.bottom);
	SelectObject(hdcBack, (HBITMAP)hBitmap);

	if (SELECT_GRAPH == WINDOW_ROD) Draw(hwnd, hdcBack, pt, rect);
	if (SELECT_GRAPH == WINDOW_IN_PLANE) DrawInPlane(hwnd, hdcBack, pt, rect);
	if (SELECT_GRAPH == WINDOW_CONTUR) ConturPlot(hwnd, hdcBack, pt, rect);
	if (SELECT_GRAPH == WINDOW_DENSITY) Draw(hwnd, hdcBack, pt, rect);
	if (SELECT_GRAPH == WINDOW_DISPERSION) Draw(hwnd, hdcBack, pt, rect);//Dispersion
	if (SELECT_GRAPH == WINDOW_MULTIPLOT) MultiDraw(hwnd, hdcBack, hdcBack2, rect);

	if (hBitmap)
	{
		if (!::OpenClipboard(hwnd))
			return;
		::EmptyClipboard();

		BITMAP bm;
		::GetObject(hBitmap, sizeof(bm), &bm);

		BITMAPINFOHEADER bi;
		::ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = bm.bmWidth;
		bi.biHeight = bm.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = bm.bmBitsPixel;
		bi.biCompression = BI_RGB;
		if (bi.biBitCount <= 1)	// make sure bits per pixel is valid
			bi.biBitCount = 1;
		else if (bi.biBitCount <= 4)
			bi.biBitCount = 4;
		else if (bi.biBitCount <= 8)
			bi.biBitCount = 8;
		else // if greater than 8-bit, force to 24-bit
			bi.biBitCount = 24;

		// Get size of color table.
		SIZE_T dwColTableLen = (bi.biBitCount <= 8) ? (1i64 << bi.biBitCount) * sizeof(RGBQUAD) : 0;

		// Create a device context with palette
		HDC hDC = ::GetDC(NULL);
		HPALETTE hPal = static_cast<HPALETTE>(::GetStockObject(DEFAULT_PALETTE));
		HPALETTE hOldPal = ::SelectPalette(hDC, hPal, FALSE);
		::RealizePalette(hDC);

		// Use GetDIBits to calculate the image size.
		::GetDIBits(hDC, hBitmap, 0, static_cast<UINT>(bi.biHeight), NULL,
			reinterpret_cast<LPBITMAPINFO>(&bi), DIB_RGB_COLORS);
		// If the driver did not fill in the biSizeImage field, then compute it.
		// Each scan line of the image is aligned on a DWORD (32bit) boundary.
		if (0 == bi.biSizeImage)
			bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) * bi.biHeight;

		// Allocate memory
		HGLOBAL hDIB = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + dwColTableLen + bi.biSizeImage);
		if (hDIB)
		{
			union tagHdr_u
			{
				LPVOID             p;
				LPBYTE             pByte;
				LPBITMAPINFOHEADER pHdr;
				LPBITMAPINFO       pInfo;
			} Hdr;

			Hdr.p = ::GlobalLock(hDIB);
			// Copy the header
			::CopyMemory(Hdr.p, &bi, sizeof(BITMAPINFOHEADER));
			// Convert/copy the image bits and create the color table
			int nConv = ::GetDIBits(hDC, hBitmap, 0, static_cast<UINT>(bi.biHeight),
				Hdr.pByte + sizeof(BITMAPINFOHEADER) + dwColTableLen,
				Hdr.pInfo, DIB_RGB_COLORS);
			::GlobalUnlock(hDIB);
			if (!nConv)
			{
				::GlobalFree(hDIB);
				hDIB = NULL;
			}
		}
		if (hDIB)
			::SetClipboardData(CF_DIB, hDIB);

		if (hBitmap)
			DeleteBitmap(hBitmap);
		::CloseClipboard();
		::SelectPalette(hDC, hOldPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return;// NULL != hDib;
	}
	else
		MessageBox(NULL, L"Can't read temporary data", L"Error", MB_OK | MB_ICONERROR);
	
	if (hBitmap)
		DeleteBitmap(hBitmap);
	return;

}

void Graph2D::PrintFunction(HWND hwnd)
{

	RECT rect; POINT pt;
	rect.bottom = rgbColors.BMP_Height; rect.right = rgbColors.BMP_Width;
	pt.x = 0; pt.y = 0;
	static DOCINFO document_Information = { sizeof(DOCINFO), TEXT("Printing 2D graph.....") };

	HDC prn, prn2;
	prn = GetPrinterDC(hwnd); // Get Printer Device Context  
	
	if (prn == 0)
		return;

	int cxpage = GetDeviceCaps(prn, HORZRES);
	int cypage = GetDeviceCaps(prn, VERTRES);

	// initialization of the printing!
	//static DOCINFO document_Information;
	//memset(&document_Information, 0, sizeof(document_Information));
	//ZeroMemory(&document_Information, sizeof(DOCINFO));
	//document_Information.cbSize = sizeof(DOCINFO);

	if (StartDoc(prn, &document_Information) <= 0)
	{
		DeleteDC(prn);
		MessageBox(NULL, L"Can't start printing ", NULL, MB_OK);
		return ;
	}
	StartPage(prn);


	SetMapMode(prn, MM_ISOTROPIC);
	SetWindowExtEx(prn, rgbColors.BMP_Width, rgbColors.BMP_Height, NULL);
	SetViewportExtEx(prn, cxpage, cypage, NULL);
	SetViewportOrgEx(prn, 0, 0, NULL);
	MoveToEx(prn, 0, cypage / 2, NULL);
	LineTo(prn, cxpage, (cypage / 2));

	prn2 = CreateCompatibleDC(prn);
	HBITMAP hBitmap = CreateCompatibleBitmap(prn, cxpage, cypage);
	SelectObject(prn2, (HBITMAP)hBitmap);
	DeleteBitmap(hBitmap);
	PatBlt(prn2, 0, 0, cxpage, cypage, WHITENESS);



	if (SELECT_GRAPH == WINDOW_ROD) Draw(hwnd, prn, pt, rect);
	if (SELECT_GRAPH == WINDOW_IN_PLANE) DrawInPlane(hwnd, prn, pt, rect);
	if (SELECT_GRAPH == WINDOW_CONTUR) ConturPlot(hwnd, prn, pt, rect);
	if (SELECT_GRAPH == WINDOW_DENSITY) Draw(hwnd, prn, pt, rect);
	if (SELECT_GRAPH == WINDOW_DISPERSION) Draw(hwnd, prn, pt, rect);//Dispersion
	if (SELECT_GRAPH == WINDOW_MULTIPLOT) MultiDraw(hwnd, prn, prn2, rect);

	if (EndPage(prn) <= 0)
	{
		MessageBox(NULL, L"It is a problem with printing. ", NULL, MB_OK);
	}
	EndDoc(prn);

	DeleteDC(prn);
	DeleteDC(prn2);
}

HDC Graph2D::GetPrinterDC(HWND Hwnd)
{
	// Initialize a PRINTDLG structure's size and set the PD_RETURNDC flag set the Owner flag to hwnd.  
	// The PD_RETURNDC flag tells the dialog to return a printer device context.  

	PRINTDLG pd = { 0 };
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = Hwnd;
	pd.Flags = PD_RETURNDC | PD_PRINTSETUP;

	// Retrieves the printer DC  
	PrintDlg(&pd);
	HDC hdc = pd.hDC;
	return hdc;
}

void Graph2D::Save2DFunction(HWND hwnd)
{

	HANDLE			hFile;
	WCHAR szFile[MAX_PATH] = TEXT("\0");

	DWORD  dwsize = 0;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	if (SELECT_GRAPH == WINDOW_IN_PLANE)
		ofn.lpstrFilter = TEXT("*.bmp\0*.bmp\0*.cur\0*.cur\0*.da_\0*.da_\0*.inp\0*.inp\0\0");
	else
		ofn.lpstrFilter = TEXT("*.bmp\0*.bmp\0*.cur\0*.cur\0*.da_\0*.da_\0\0");	ofn.lpstrTitle = TEXT("Save File");
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

	if (ofn.nFilterIndex == 1)
	{
		RECT rect; POINT pt;
		rect.bottom = rgbColors.BMP_Height; rect.right = rgbColors.BMP_Width;
		pt.x = 0; pt.y = 0;
		///HDC hdcsave;
		HBITMAP hbm = CreateCompatibleBitmap(hdcBack, rect.right, rect.bottom);
		SelectObject(hdcBack, (HBITMAP)hbm);

		if (SELECT_GRAPH == WINDOW_ROD) Draw(hwnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_IN_PLANE) DrawInPlane(hwnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_CONTUR) ConturPlot(hwnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_DENSITY) Draw(hwnd, hdcBack, pt, rect);
		if (SELECT_GRAPH == WINDOW_DISPERSION) Draw(hwnd, hdcBack, pt, rect);//Dispersion
		if (SELECT_GRAPH == WINDOW_MULTIPLOT) MultiDraw(hwnd, hdcBack, hdcBack2, rect);

		SaveBitmap(hFile, hbm, hwnd, hdcBack);
		if (hbm)  DeleteObject(hbm);
	}

	CloseHandle(hFile);

	if (ofn.nFilterIndex == 2){
		char str[MAX_PATH];
		TtoA(str, ofn.lpstrFile);
		SaveTextXYZFile(str);
	}

	if (ofn.nFilterIndex == 3){
		char str[MAX_PATH];
		TtoA(str, ofn.lpstrFile);
		SaveTextDatFile(str);
	}

	//inplane data save
	if (ofn.nFilterIndex == 4) {
		char str[MAX_PATH];
		TtoA(str, ofn.lpstrFile);
		SaveTextDataAndFFile(str);
	}

}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the xyz file with atom coordinates
//Function created by Daniel Kaminski
//27.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool Graph2D::SaveTextXYZFile(char *FileName){

 FILE*   fF = NULL;
 char str [MAX_PATH]={};
 int i, j;

	errno_t err = fopen_s(&fF, FileName ,"wt" );
	if (err != NULL || fF==NULL){
		sprintf_s(str, COMMLENGHT, "%s", FileName);
		MessageBox( NULL, L"Unable to create *.txt file", L"Error", MB_OK | MB_ICONERROR);
		return false;
	} 

   /* Save parameters for rod and in plane data */
	if (SELECT_GRAPH == WINDOW_ROD || SELECT_GRAPH == WINDOW_IN_PLANE)
	{
		fprintf (fF, "fit x y \n");
		for (i = 0; i < fit[0].NrOfPoints; i++){
			fprintf (fF, "%8.4f %8.4f\n",fit[0].x[i], fit[0].y[i]);
		}
	}

   /* Save parameters for contur plot */
	if (SELECT_GRAPH == WINDOW_CONTUR)
	{
		fprintf (fF, "fit x y \n");
		for (i = 0; i < Contur.NxCon; i++){
			for(j=0; j<Contur.NyCon; j++){ 
				fprintf (fF, "%8.4f %8.4f %8.4f\n",Contur.x[i], Contur.y[j], Contur.z[Contur.NxCon*j+i]);
			}
		}
	}


   /* Save parameters for electron density profile */
	if (SELECT_GRAPH == WINDOW_DENSITY){
		fprintf (fF, "   z  f1(z) f2(z) f3(z)  f4(z) .....\n");
		for (i = 0; i < fit[0].NrOfPoints; i++)
		{
			fprintf (fF, "%10.2f  ",fit[0].x[i]);
			for(j = 0; j<MAX_FIT_CURVES; j++)
			{
			fprintf (fF, "%10.2f  ", fit[j].y[i]);
			}
			fprintf (fF, "\n");
		}
	}

	if(fF)      fclose(fF);

return true;
}


//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the hk file with data and structure factors
//Function created by Daniel Kaminski
//20.012.2016
//-----------------------------------------------------------------------------------------------------------------
bool Graph2D::SaveTextDataAndFFile(char *FileName) {

	FILE*   fF = NULL;
	char str[MAX_PATH] = {};
	int i;

	errno_t err = fopen_s(&fF, FileName, "wt");
	if (err != NULL)
	{
		sprintf_s(str, MAX_PATH, "%s", FileName);
		MessageBox(NULL, L"Unable to create *.inp file", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	/* Save parameters for rod and in plane data */
	fprintf(fF, " Plane l %8.4f\n", data.InPlaneL);
	fprintf(fF, " h k Fdata sigma Fcalc \n");
	for (i = 0; i < data.NrOfPoints; i++) {
		for (int j = 0; j < fit[0].NrOfPoints; j++) {
			if (fit[0].x[j] == data.x[i] && fit[0].y[j] == data.y[i])
				fprintf(fF, "%8.4f %8.4f %8.4f %8.4f  %8.4f \n", data.x[i], data.y[i], data.z[i], data.sigma[i], fit[0].z[j]);
		}
	}

	if (fF)      fclose(fF);

	return true;
}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the xyz file with atom coordinates
//Function created by Daniel Kaminski
//27.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool Graph2D::SaveTextDatFile(char *FileName){

	FILE*   fF = NULL;
	char str[MAX_PATH] = {};
	int i;

	errno_t err = fopen_s(&fF, FileName, "wt");
	if (err != NULL)
	{
		sprintf_s(str, MAX_PATH, "%s", FileName);
		MessageBox(NULL, L"Unable to create *.txt file", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	/* Save parameters for rod and in plane data */
	if (SELECT_GRAPH == WINDOW_ROD)
	{
		fprintf(fF, "data x y \n");
		for (i = 0; i < data.NrOfPoints; i++){
			fprintf(fF, "%8.4f %8.4f %8.4f \n", data.x[i], data.y[i], data.sigma [i]);
		}
	}

	if (fF)      fclose(fF);

	return true;
}

//-----------------------------------------------------------------------------
// The main paint function for 2D window.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Graph2D::ConturPlot(HWND hwnd, HDC hdcBack_, POINT Pt, RECT rect){


POINT line[2];

int coordinateX, coordinateY;		//for position on the screen
double variable_x, variable_y;		//for position in variable space
double step_scale;					//Step of the scales x and y
GRAPHSETTINGS temp_zoom;					//for selection of the zoom area

RECT rect2;
WCHAR plot_text[HEADER];

	//SetBkColor(hdcBack_,RGB(255,255,255));
	SetBkMode(hdcBack_, TRANSPARENT );
	FillRect(hdcBack_, &rect, hBru[7] );



//safety
if( fabs(XYMinMax.left-XYMinMax.right)<.000001) Initialize_borders();
if( fabs(XYMinMax.top-XYMinMax.bottom)<.000001) Initialize_borders();


//--------------------------------------------------------------------------------------------------
//fill plot area
//--------------------------------------------------------------------------------------------------
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	rect2.left=coordinateX; rect2.top=coordinateY;
	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	rect2.bottom = coordinateY; rect2.right = coordinateX; 

	FillRect(hdcBack_, &rect2, hBru[1] );

//--------------------------------------------------------------------------------------------------
//zoom draw the rectangular
//--------------------------------------------------------------------------------------------------
	if(fabs(XYMinMax.top-XYMinMax.bottom) < 0.0000001) XYMinMax.bottom += 0.0000001;
	if(fabs(XYMinMax.right-XYMinMax.left) < 0.0000001) XYMinMax.right  += 0.0000001;
	
	
	if(zoom_frame.left == zoom_frame.right)  zoom_frame.right = zoom_frame.left  + 1; //safety
	if(zoom_frame.top  == zoom_frame.bottom) zoom_frame.top  = zoom_frame.bottom + 1;
	if ( zoom_track_stop && !zoom_done){
		SelectObject(hdcBack_,hPen[4]);
		Rectangle(hdcBack_, zoom_frame.left+1, zoom_frame.top+1, zoom_frame.right-1, zoom_frame.bottom-1 );
		FillRect(hdcBack_, &zoom_frame,  hBru[5] );
	}

if ( !zoom_track_stop ){  //we are not in zooming action
}
else {
	if (zoom_done){
		GRAPHSETTINGS temp_XYMinMax;



			temp_XYMinMax.bottom=XYMinMax.bottom;
			temp_XYMinMax.top=XYMinMax.top;
			temp_XYMinMax.left=XYMinMax.left;
			temp_XYMinMax.right=XYMinMax.right;

		if (zoom_frame.top-zoom_frame.bottom<0   ){
			temp_zoom.bottom=zoom_frame.top;
			temp_zoom.top=zoom_frame.bottom;
		}
		else{
			temp_zoom.bottom=zoom_frame.bottom;
			temp_zoom.top=zoom_frame.top;
		}

		if (zoom_frame.right < zoom_frame.left   ){
			temp_zoom.left=zoom_frame.right;
			temp_zoom.right=zoom_frame.left;
		}
		else{
			temp_zoom.left=zoom_frame.left;
			temp_zoom.right=zoom_frame.right;
		}

		ConvertXtoA(temp_XYMinMax,  &variable_x, &variable_y, rect, (int)temp_zoom.left, (int)temp_zoom.top);
		XYMinMax.left=variable_x; XYMinMax.bottom=variable_y;
		ConvertXtoA(temp_XYMinMax,  &variable_x, &variable_y, rect, (int)temp_zoom.right, (int)temp_zoom.bottom);
		XYMinMax.right=variable_x; XYMinMax.top=variable_y;
		zoom_done=false;
		zoom_track_stop=false;

	}
}
	
//--------------------------------------------------------------------------------------------------
//scala X
//--------------------------------------------------------------------------------------------------
	double tmp_left, tmp_right;
	double scale;
	if (grid){
		SelectObject(hdcBack_, hPen[8]);
		step_scale = (XYMinMax.right - XYMinMax.left) / rect.right*NR_OF_POINTS_ON_SCALE_X;
		if (fabs(step_scale) < 0.0001) step_scale = 0.0001;

		if (step_scale > 1.0) step_scale = floor(step_scale);
		if (step_scale > 0.1) step_scale = 1.;
		if (fabs(XYMinMax.right - XYMinMax.left) < 2.) {
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
			if (line[0].x > 0 && line[1].x > 0 && line[0].y > 0)
				Polyline(hdcBack_, line, 2);
		}

		//--------------------------------------------------------------------------------------------------
		//skala y
		//--------------------------------------------------------------------------------------------------


		for (scale = XYMinMax.bottom; scale <= XYMinMax.top; scale += step_scale){//
			step_scale = (XYMinMax.top - XYMinMax.bottom) / rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
			if (fabs(step_scale) < 0.0001) step_scale = XYMinMax.top;
			ConvertAtoX(XYMinMax, XYMinMax.right, scale, rect, &coordinateX, &coordinateY);
			line[0].x = coordinateX; line[0].y = coordinateY;
			ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
			line[1].x = coordinateX - 4; line[1].y = coordinateY;
			Polyline(hdcBack_, line, 2);
		}
	}


//--------------------------------------------------------------------------------------------------
//draw  contur
//--------------------------------------------------------------------------------------------------
	SelectObject(hdcBack_,hPen[0]); 
	double x, y;
	double temp_z;
	int test;
	for (int level = 0; level < Contur.Nlevel; level++){ //do it for each level
//		int level=1;
		double level_up   = Contur.zmin + Contur.zstep * (level+1);
		double level_down = Contur.zmin + Contur.zstep * level;
		double midle =level_down + (level_up-level_down)/2;

		for (int iy = 2; iy < Contur.NyCon-2; ++iy){ //iterate the net for y
			for(int ix = 2; ix < Contur.NxCon-2; ++ix){//iterate the net for x
	

				temp_z = Contur.z[Contur.NxCon*iy+ix];
				if(temp_z>=level_down && temp_z<level_up){
					//Point 0 defined by the ix and iy coordinates in the net

					test = Intersec(ix, iy, midle, &x, &y);
					if(test > 0) {
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[0].x = coordinateX; line[0].y = coordinateY; //scaling to the declared size of graph
 						//Ellipse( hdcBack_, coordinateX-2, coordinateY-2, coordinateX+2, coordinateY+2);

						//It is enought to check only 4 surrounded points
						//Poit 1
						Intersec(ix-1, iy+1, midle, &x, &y);
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if(temp_z <0.){
							//SelectObject(hdcBack_,hPen[6]); 
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[4]); 
							Polyline(hdcBack_, line, 2);
						}
						else{ 
							//SelectObject(hdcBack_,hPen[7]);
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[1]); 
							Polyline(hdcBack_, line, 2);
						}

						//Poit 2
						Intersec(ix, iy+1, midle, &x, &y);
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if(temp_z <0.){
							///SelectObject(hdcBack_,hPen[6]); 
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[4]); 
							Polyline(hdcBack_, line, 2);
						}
						else{ 
							//SelectObject(hdcBack_,hPen[7]);
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[1]); 
							Polyline(hdcBack_, line, 2);
						}

						//Poit 3
						Intersec(ix+1, iy+1, midle, &x, &y);
						ConvertAtoX(XYMinMax,x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if(temp_z <0.){
							//SelectObject(hdcBack_,hPen[6]); 
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[4]); 
							Polyline(hdcBack_, line, 2);
						}
						else{ 
							//SelectObject(hdcBack_,hPen[7]);
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[1]); 
							Polyline(hdcBack_, line, 2);
						}

						//Poit 4
						Intersec(ix+1, iy, midle, &x, &y);
						ConvertAtoX(XYMinMax, x, y, rect, &coordinateX, &coordinateY);
						line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
						if(temp_z <0.){
							//SelectObject(hdcBack_,hPen[6]); 
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[4]); 
							Polyline(hdcBack_, line, 2);
						}
						else{ 
							//SelectObject(hdcBack_,hPen[7]);
							//Polyline(hdcBack_, line, 2);
							SelectObject(hdcBack_,hPen[1]); 
							Polyline(hdcBack_, line, 2);
						}

					}
				} //for point 0
			} //for x
		}//for y
	}//level


//Draw cell edges
	SelectObject(hdcBack_,hPen[0]);
	ConvertAtoX(XYMinMax, Contur.CellVertices[0].x, Contur.CellVertices[0].y, rect, &coordinateX, &coordinateY);
	line[0].x = coordinateX; line[0].y = coordinateY; //scaling to the declared size of graph
	for (int i = 0; i<5; ++i){
		ConvertAtoX(XYMinMax, Contur.CellVertices[i].x, Contur.CellVertices[i].y, rect, &coordinateX, &coordinateY);
		line[1].x = coordinateX; line[1].y = coordinateY; //scaling to the declared size of graph
		Polyline(hdcBack_, line, 2);
		line[0].x = line[1].x;  line[0].y = line[1].y;
	}




//--------------------------------------------------------------------------------------------------
//draw blank area
//--------------------------------------------------------------------------------------------------
	//clean left area
	rect2.left=0;
	rect2.right=RamkaX_left;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru );

	//clean right area
	rect2.left=rect.right-RamkaX_right+1;
	rect2.right=rect.right;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru );

	//clean top area
	rect2.left=0;
	rect2.right=rect.right;
	rect2.top = 0;
	rect2.bottom = RamkaY_top;
	FillRect(hdcBack_, &rect2, Background_hBru );

	//clean bottom area
	rect2.left=0;
	rect2.right=rect.right;
	rect2.top = rect.bottom-RamkaY_bottom+1;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack_, &rect2, Background_hBru );

//frame x
	SelectObject(hdcBack_,hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x=coordinateX; line[0].y=coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[1].x=coordinateX; line[1].y=coordinateY;
	Polyline(hdcBack_, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[0].x=coordinateX; line[0].y=coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x=coordinateX; line[1].y=coordinateY;
	Polyline(hdcBack_, line, 2);

	//frame Y
	SelectObject(hdcBack_,hPen[0]);
	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x=coordinateX; line[0].y=coordinateY+1;

	ConvertAtoX(XYMinMax, XYMinMax.left, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x=coordinateX; line[1].y=coordinateY;
	Polyline(hdcBack_, line, 2);

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	line[0].x=coordinateX; line[0].y=coordinateY;

	ConvertAtoX(XYMinMax, XYMinMax.right, XYMinMax.top, rect, &coordinateX, &coordinateY);
	line[1].x=coordinateX; line[1].y=coordinateY-1;
	Polyline(hdcBack_, line, 2);

//--------------------------------------------------------------------------------------------------
//Draw text
//--------------------------------------------------------------------------------------------------
//Draw title
	SelectObject(hdcBack_, XYMinMax.MyFont);
	SetTextColor(hdcBack_, rgbColors.text);
	rect2.left=RamkaX_left; rect2.top=MENU_HEIGHT+6; rect2.bottom=rect2.top+20; rect2.right=rect2.left+800; 
	swprintf_s(plot_text, HEADER, L"%s", Contur.Title);
	DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

//axes description I
	swprintf_s(plot_text, L"%s", Contur.TitleX);
	//SetTextColor(hdcBack_, RGB(0,0,0));
	ConvertAtoX(XYMinMax, XYMinMax.left, (XYMinMax.top+XYMinMax.bottom)/2, rect, &coordinateX, &coordinateY);
	rect2.left=coordinateX-80; rect2.top=coordinateY; rect2.bottom=rect2.top-30; rect2.right=rect2.left+32; 
	DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

//axes description q
	swprintf_s(plot_text, L"%s", Contur.TitleY);
	//SetTextColor(hdcBack_, RGB(0,0,0));
	ConvertAtoX(XYMinMax, (XYMinMax.right+XYMinMax.left)/2., XYMinMax.bottom, rect, &coordinateX, &coordinateY);
	rect2.left=coordinateX-15; rect2.top=coordinateY+20; rect2.bottom=rect2.top+15; rect2.right=rect2.left+42;  
	DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);


//--------------------------------------------------------------------------------------------------
//Plot scales and marks
//--------------------------------------------------------------------------------------------------
	SetTextColor(hdcBack_, RGB(0., 0., 0.));
	SelectObject(hdcBack_,hPen[0]);
//scala X
	step_scale = (XYMinMax.right-XYMinMax.left)/rect.right*NR_OF_POINTS_ON_SCALE_X;
	if(fabs(step_scale)<0.0001) step_scale = 0.0001;
	if(step_scale>1.0) step_scale = floor( step_scale); 
	 if(step_scale>0.1) step_scale =1.;
	if(fabs(XYMinMax.right-XYMinMax.left)<2.) {
		tmp_left  = XYMinMax.left; 
		tmp_right = XYMinMax.right;
	}
	else
	{
		tmp_left  = floor (XYMinMax.left )+1; 
		tmp_right = floor (XYMinMax.right);
	}
	for(double scale=tmp_left; scale<=tmp_right; scale += step_scale){
		ConvertAtoX(XYMinMax, scale, XYMinMax.bottom, rect, &coordinateX, &coordinateY);
		line[0].x= coordinateX; line[0].y=(int)coordinateY;
		line[1].x= coordinateX; line[1].y=(int)coordinateY+3;
		Polyline(hdcBack_, line, 2);

		rect2.left=coordinateX-22; rect2.top=coordinateY+7; rect2.bottom=rect2.top+15; rect2.right=rect2.left+50; 
		if(XYMinMax.right>3.) swprintf_s(plot_text, 100, L"%6.0f", scale);
		else swprintf_s(plot_text, 100, L"%6.2f", scale);
		DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}

//skala y
	for(scale=XYMinMax.bottom; scale<=XYMinMax.top; scale += step_scale){//
		step_scale = (XYMinMax.top-XYMinMax.bottom)/rect.bottom*NR_OF_POINTS_ON_SCALE_Y;
		if(fabs(step_scale)<0.0001) step_scale = XYMinMax.top;
		ConvertAtoX(XYMinMax, XYMinMax.left, scale, rect, &coordinateX, &coordinateY);
		line[0].x= coordinateX; line[0].y=coordinateY;
		line[1].x= coordinateX-4; line[1].y=coordinateY;
		Polyline(hdcBack_, line, 2);
		rect2.right=coordinateX-5; rect2.left=rect2.right-70; rect2.top=coordinateY-7; rect2.bottom=rect2.top+15; 
		if( XYMinMax.top<100.) swprintf_s(plot_text, 100, L"%3.2f", scale);
		else swprintf_s(plot_text, 100, L"%3.0f", scale);
		DrawText (hdcBack_, plot_text,-1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}

	SelectObject(hdcBack_, hPen[8]);
	line[1].x = line[0].x = SIDE_BAR_WIDTH + 1;
	line[0].y = 0; line[1].y = rect.bottom;
	Polyline(hdcBack_, line, 2);

//--------------------------------------------------------------------------------------------------
//end plot
//--------------------------------------------------------------------------------------------------
}

int Graph2D::Intersec(int ix, int iy, double midle, double *x, double *y){
	
int result;
	//(1 0)
	result = PointTest(ix, iy, 1, 0, midle, x, y );
	if( result > 0) return result;

	//(-1 0)
	result = PointTest(ix, iy, -1, 0, midle, x, y );
	if( result > 0) return result+3;

	//(0 1)
	result = PointTest(ix, iy, 0, -1, midle, x, y );
	if( result > 0) return result+3;
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

int Graph2D::PointTest(int ix, int iy, int delx, int dely, double midle, double *x, double *y){

	//y line
	int temp_x = ix + delx;
	int temp_y = iy + dely;

	if (Contur.z[Contur.NxCon * temp_y + temp_x] <= midle &&
		Contur.z[Contur.NxCon*iy + ix] >= midle){

		if (dely < 0)
			*y = (Contur.y[temp_y - 1] + Contur.y[iy]) / 2.;
		else
			*y = (Contur.y[temp_y] + Contur.y[iy]) / 2.;

		*x = (Contur.x[temp_x] + Contur.x[ix]) / 2.;
		return 1;
	}


	if (Contur.z[Contur.NxCon * temp_y + temp_x] >= midle &&
		Contur.z[Contur.NxCon*iy + ix] <= midle){

		if (dely < 0)
			*y = (Contur.y[temp_y - 1] + Contur.y[iy]) / 2.;
		else
			*y = (Contur.y[temp_y] + Contur.y[iy]) / 2.;
		*x = (Contur.x[temp_x] + Contur.x[ix]) / 2.;

		return 2;
	}

	return -1;
}

void Graph2D::ResetOpen(){
	nrOpenFit = 0;
	nrOpenDat = 0;
	for (int i = 0; i < MAX_OPEN; ++i){
		ZeroMemory(&Openfit[i], sizeof(Data_struct));
		ZeroMemory(&Opendat[i], sizeof(Data_struct));
	}

	NumberOfRods = 0;
}

int Graph2D::OpenFileFunction(HWND hwnd){

	HANDLE			hFile;
	WCHAR szFile[MAX_PATH] = TEXT("\0");
	DWORD			dwSave;
	DWORD  dwsize = 0;
	//LPSTR szTemporary;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("*.cur\0*.cur\0*.da_\0*.da_\0\0");
	ofn.lpstrTitle = TEXT("Open File");
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = TEXT("*.txt");
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.nFilterIndex = 1;

	if (GetOpenFileName(&ofn)){
		hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			MessageBox(NULL, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
			CloseHandle(hFile);
			return 0;
		}
	}
	else return 0;
	dwsize = GetFileSize(hFile, 0);
	//szTemporary = (LPSTR)HeapAlloc(GetProcessHeap(), 0, (dwsize + 1) * sizeof(CHAR));       // buforfor data 
	char* szTemporary = new char[dwsize+1];

	if (!ReadFile(hFile, szTemporary, dwsize, &dwSave, NULL)){
		MessageBox(NULL, TEXT("Read file failed!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		delete[] szTemporary;
		return 0;
	}

	ZeroMemory(&TempOpenData, sizeof(TempOpenData));

	//Read parameter file curve
	if (ofn.nFilterIndex == 1)
		if (ReadData(szTemporary, ofn.nFilterIndex, dwsize))
		{
		TempOpenData.status = true;
		TempOpenData.readed = true;
		TempOpenData.style=1 ;
		TempOpenData.thickness= 1;
		TempOpenData.color = RGB(rand() % 255, rand() % 255, rand() % 255);
		memcpy(&Openfit[nrOpenFit], &TempOpenData, sizeof(TempOpenData));
		nrOpenFit++;
		}

	//Read parameter file dat
	if (ofn.nFilterIndex == 2)
		if (ReadData(szTemporary, ofn.nFilterIndex, dwsize))
		{
		TempOpenData.status = true;
		TempOpenData.readed = true;
		TempOpenData.thickness = 4;
		TempOpenData.style = 0;
		TempOpenData.color = RGB(rand() % 255, rand() % 255, rand() % 255);
		TempOpenData.color_fill = RGB(rand() % 255, rand() % 255, rand() % 255);
		memcpy(&Opendat[nrOpenDat], &TempOpenData, sizeof(TempOpenData));
		nrOpenDat++;
		}

	AutomaticSetTheScale();
	CopyColorsToOpenBruPen();
	//HeapFree(GetProcessHeap(), 0, szTemporary);
	delete[] szTemporary;
	CloseHandle(hFile);

	return ofn.nFilterIndex;
}

bool Graph2D::ReadData(LPSTR TStr, int DataType, DWORD size)
{
	char OutLine[COMMLENGHT];
	unsigned int Position = 0;

	for (int i = 0; i < MAXDATA + 2; ++i){
		if (!Take_line(TStr, size, OutLine, COMMLENGHT, &Position)) break;
			if (!Read_Dat(OutLine, i)) return false;

	}
	return true;
}

bool Graph2D::Read_Dat(char *line, int nr){
	WCHAR ErrorMsg[100];
	float tmp1 = 0, tmp2 = 0, tmp3 = 0;

	if (nr == 0){
		WCHAR Wchar[COMMLENGHT];
		AtoT(Wchar, line);
			memcpy(TempOpenData.header, Wchar, sizeof(TempOpenData.header));
		
		return true;
	}

	//read the unit cell parameters
	if (nr > 0 )
	{
		//omit empty line
		if (strlen(line) < 1) return true;
		if (sscanf_s(line, "%f %f %f", &tmp1, &tmp2, &tmp3) == 0) return false;
		else{
			TempOpenData.x[TempOpenData.NrOfPoints] = tmp1; //x
			TempOpenData.y[TempOpenData.NrOfPoints] = tmp2; //y
			TempOpenData.sigma[TempOpenData.NrOfPoints] = tmp3; //error
			TempOpenData.NrOfPoints++;
		}

		if (TempOpenData.NrOfPoints == MAX_DATA_POINTS){
			wsprintf(ErrorMsg, L"ERROR, maximum number of data points exceeded %d, file truncated", MAX_DATA_POINTS);
			MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
			return false;
		}

		//not realy necessary line
		if (TempOpenData.NrOfPoints == 0){
			wsprintf(ErrorMsg, L"No data in file");
			MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
			return false;
		}
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the 2d sufrace to file
//Created by Daniel Kaminski
//
// 18.02.2013
//-----------------------------------------------------------------------------------------------------------------

void Graph2D::SaveBitmap(HANDLE	hFile, HBITMAP hBitmap, HWND hwnd, HDC hdc)
{

	DWORD			   dwSave;
	LPVOID              pBuf = NULL;
	BITMAPINFO          bmpInfo;
	BITMAPFILEHEADER    bmpFileHeader;

	// do{ 

	ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

	if (bmpInfo.bmiHeader.biSizeImage <= 0)
		bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount + 7) / 8;

	if ((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage)) == NULL)
	{
		MessageBox(NULL, L"Unable to Allocate Bitmap Memory", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	bmpInfo.bmiHeader.biCompression = BI_RGB;

	GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, pBuf, &bmpInfo, DIB_RGB_COLORS);


	bmpFileHeader.bfReserved1 = 0;

	bmpFileHeader.bfReserved2 = 0;

	bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfo.bmiHeader.biSizeImage;

	bmpFileHeader.bfType = 'MB';

	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


	if (!WriteFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &dwSave, NULL)){
		MessageBox(hwnd, TEXT("Write to file failed!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		return;
	}

	if (!WriteFile(hFile, &bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), &dwSave, NULL)){
		MessageBox(hwnd, TEXT("Write to file failed!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		return;
	}

	if (!WriteFile(hFile, pBuf, bmpInfo.bmiHeader.biSizeImage, &dwSave, NULL)){
		MessageBox(hwnd, TEXT("Write to file failed!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		return;
	}

	if (pBuf)    free(pBuf);
}

void Graph2D::CopyColorsToOpenBruPen()
{
	for (int i = 0; i < MAX_OPEN; ++i){
		DeleteObject(hPenOpen[i]);
		DeleteObject(hPenOpenDat[i]);
	}

	for (int i = 0; i < MAX_OPEN; ++i)
	{	
		hPenOpen[i] = CreatePen(Openfit[i].style, Openfit[i].thickness, Openfit[i].color);
		hPenOpenDat[i] = CreatePen(PS_SOLID, 1, Opendat[i].color);//Opendat[i].thickness
	}

}

void Graph2D::CopyFitToCurve()
{
	if (nrOpenFit <MAX_OPEN){
		CopyMemory(&Openfit[nrOpenFit], &fit[0], sizeof(Data_struct));
		Openfit[nrOpenFit].readed = true;
		Openfit[nrOpenFit].status = true;
		Openfit[nrOpenFit].color = RGB(rand() % 255, rand() % 255, rand() % 255);
		nrOpenFit++;
	}
	else
		MessageBox(NULL, TEXT("Number of entries exceded!"), NULL, MB_ICONERROR);
}

void Graph2D::CopyDataToOpenDat(){
	if (nrOpenDat < MAX_OPEN){
		CopyMemory(&Opendat[nrOpenDat], &data, sizeof(Data_struct));
		Opendat[nrOpenDat].readed = true;
		Opendat[nrOpenDat].status = true;
		Opendat[nrOpenDat].thickness = 4;
		Opendat[nrOpenDat].color = RGB(rand() % 255, rand() % 255, rand() % 255);
		Opendat[nrOpenDat].color_fill = RGB(rand() % 255, rand() % 255, rand() % 255);
		nrOpenDat++;
	}
	else
		MessageBox(NULL, TEXT("Number of entries exceded!"), NULL, MB_ICONERROR);

}

bool Graph2D::InitializeMultiDraw()
{
	ButtonsState();

	if (NumberOfRods >= MAXMULTIPLOT)
	{
		//Multiplot = NULL;
		NumberOfRods = MAXMULTIPLOT-1;
	}


	if (Multiplot!=NULL)
		delete[] Multiplot;
	Multiplot = new MULTIPLOT[NumberOfRods+1];

	for (int i = 0; i < NumberOfRods; ++i)
		ZeroMemory(&Multiplot[i], sizeof(MULTIPLOT));

	return true;
}

void Graph2D::MultiDraw(HWND hWnd, HDC hdcBack_, HDC hdcBack2_, RECT rect)
{
	
	DeleteObject(SelectObject(hdcBack, Background_hBru));
	Background_hBru = CreateSolidBrush(rgbColors.framecolor);

	RECT rect2, rect_oryginal;
	int scale = 1;

	int shift_x, shift_y;
	int i_col = 0;
	shift_y = 30;
	shift_x = 30;

	if (N_columns < 1) N_columns = 1;
	double NumberOfRows = (NumberOfRods - 1) / N_columns;//not corrected
	double correction = (NumberOfRows - (int)(NumberOfRows))*N_columns;//the missing rows
	correction = N_columns - correction;
	if (correction < 1)correction = 1;
	NumberOfRows = (int)(NumberOfRows)+correction / N_columns;
	if (NumberOfRows < 1)NumberOfRows = 1;
	pt.x = 0; pt.y = 0;



	rect_oryginal.left = 0;
	rect_oryginal.top = 0;
	rect_oryginal.right = (int)(rect.right / N_columns *1.5);
	rect_oryginal.bottom = (int)(rect.bottom / NumberOfRows *1.5);

	rect2.left = 0;
	rect2.right = (int)(rect.right / (N_columns)-shift_x / N_columns);
	rect2.top = 0;
	rect2.bottom = (int)(rect.bottom / NumberOfRows - (shift_y + 30) / (NumberOfRows));


	FillRect(hdcBack_, &rect, hBru[7]);
	FillRect(hdcBack2_, &rect, hBru[7]);

	for (int i = 0; i < NumberOfRods; ++i)//
	{
		//Copy data

		CopyMemory(fit[0].header, Multiplot[i].fit[0].header, sizeof(WCHAR)*HEADER);

		fit[0].NrOfPoints = Multiplot[i].fit[0].NrOfPoints;
		fit[1].NrOfPoints = Multiplot[i].fit[1].NrOfPoints;
		fit[2].NrOfPoints = Multiplot[i].fit[2].NrOfPoints;
		fit[3].NrOfPoints = Multiplot[i].fit[3].NrOfPoints;

		fit[0].LBragg = Multiplot[i].fit[0].LBragg;

		for (int j = 0; j < Multiplot[i].fit[0].NrOfPoints; ++j)
		{
			fit[0].x[j] = Multiplot[i].fit[0].x[j];
			fit[1].x[j] = Multiplot[i].fit[1].x[j];
			fit[2].x[j] = Multiplot[i].fit[2].x[j];
			fit[3].x[j] = Multiplot[i].fit[3].x[j];

			fit[0].y[j] = Multiplot[i].fit[0].y[j];
			fit[1].y[j] = Multiplot[i].fit[1].y[j];
			fit[2].y[j] = Multiplot[i].fit[2].y[j];
			fit[3].y[j] = Multiplot[i].fit[3].y[j];
		}
		//Copy data
		for (int j = 0; j < Multiplot[i].data.NrOfPoints; ++j){

			data.x[j] = Multiplot[i].data.x[j];
			data.y[j] = Multiplot[i].data.y[j];
			data.sigma[j] = Multiplot[i].data.sigma[j];
		}

		data.NrOfPoints = Multiplot[i].data.NrOfPoints;

		AutomaticSetTheScale();
		//rect_oryginal.bottom = 400; rect_oryginal.right = 400;
	
		Draw(hWnd, hdcBack2_, pt, rect_oryginal);//Dispersion
		StretchBlt(hdcBack_, shift_x, shift_y, rect2.right, rect2.bottom, hdcBack2_, rect_oryginal.left + 60, rect_oryginal.top + 40, rect_oryginal.right - 60, rect_oryginal.bottom - 80, SRCCOPY);

		shift_x += rect2.right;// -cut_left;
		if (i_col >= N_columns - 1)
		{
			i_col = -1;
			shift_y += rect2.bottom;// -cut_top * 2;
			shift_x = 30;
		}
		i_col++;
	}
}

void Graph2D::RefreshWindow()
{
	ButtonsState();
	InvalidateRect(hwndWindow, 0, 0);
	UpdateStatusBar();

}

void Graph2D::InitializeContentMenu(HINSTANCE hinst, HWND hwnd)
{
	hContextMenu = LoadMenu(hinst, MAKEINTRESOURCE(IDR_CONTEXT_MENU));

	
	MENUITEMINFO mii;
	ZeroMemory(&mii, sizeof(mii));
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_ID | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.wID = IDM_EXPAND;
	mii.dwTypeData = L"Extend view";

	InsertMenuItem(hContextMenu, ID_PLOT_SETXSCALE, FALSE, &mii);


}

void Graph2D::ContentMenuMsgTranslate(HINSTANCE hinst, HWND hwnd, int msg)
{

	switch (msg)
	{
	case ID_PLOT_DELETE:
		if (ClientWindowDataPoint >= 0)
		{
			for (int i = ClientWindowDataPoint; i < data.NrOfPoints-1; i++)
			{
				data.x[i] = data.x[i + 1];
				data.y[i] = data.y[i + 1];
				data.z[i] = data.z[i + 1];
				data.sigma[i] = data.sigma[i + 1];
			}
			data.NrOfPoints -= 1;
		}
		return;

	case ID_PLOT_SETSCALE:
		XYMinMax.Title = data.header;
		XYMinMax.TitleX = data.TitleX;
		XYMinMax.TitleY = data.TitleY;
		DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd, SettingsScale, (LPARAM)&XYMinMax);
		InvalidateRect(hwnd, NULL, 0);
		return;
	}

}

int Graph2D::FindDataNumber(HWND hwnd)
{
	if (arrow_flag)
	{
		int coordinateX, coordinateY;
		GetClientRect(hwnd, &WindowsRect);

		for (int i = 0; i < data.NrOfPoints; ++i)
		{
			if (lin_log) ConvertAtoX(XYMinMax, data.x[i], data.y[i], WindowsRect, &coordinateX, &coordinateY);
			else ConvertAtoX(XYMinMax, data.x[i], log10(data.y[i]), WindowsRect, &coordinateX, &coordinateY);
			if (coordinateX < pt.x + 5 && coordinateX > pt.x - 5 && coordinateY < pt.y + 5 && coordinateY > pt.y - 5)
			{
				return i;
			}
		}
	}
	return -1;
}





/*************************************************************************///----------------------------  FloodFill( )  ---------------------------///*************************************************************************/struct Pixel

/*

void Graph2D::SetNewMaskSize(){
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


void Graph2D::BoundaryFill(const int _x, const int _y, int Width, int Height, int fill_color, int boundary_color){

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


void Graph2D::FillConturIntoMask(){

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
void Graph2D::FilledConturPlot(HWND hwnd, HDC hdcBack, POINT Pt, RECT rect){


	POINT line[2];

	int coordinateX, coordinateY;		//for position on the screen
	double variable_x, variable_y;		//for position in variable space
	double step_scale;					//Step of the scales x and y
	BORDERS temp_zoom;					//for selection of the zoom area

	RECT rect2;
	WCHAR plot_text[HEADER];

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
	FillRect(hdcBack, &rect2, Background_hBru);

	//clean right area
	rect2.left = rect.right - RamkaX_right + 1;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack, &rect2, Background_hBru);

	//clean top area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = RamkaY_top;
	FillRect(hdcBack, &rect2, Background_hBru);

	//clean bottom area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = rect.bottom - RamkaY_bottom + 1;
	rect2.bottom = rect.bottom;
	FillRect(hdcBack, &rect2, Background_hBru);

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
void Graph2D::LineContur(POINT Pos1, POINT Pos2, int Color){


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