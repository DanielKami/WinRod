/***************************************************************************/
/*     All functions needed to create and operate sliders.                                                     */
/***************************************************************************/
#include <stdafx.h>
#include <windows.h>
#include "Sliders.h"
#include <commdlg.h>
#include <math.h>
#include <Windowsx.h>
#include <commctrl.h>
#include "variables.h"		
#include "Thread.h"
#include <stdio.h>


//-----------------------------------------------------------------------------
//Create the window 
// 03.07.2014 modified by Daniel Kamisnki
//-----------------------------------------------------------------------------
HWND Sliders::CreateSliderWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect)
{
	// Create a window that is centered on the desktop. It's exactly 1/4 the
	// size of the desktop.

	DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;//|WS_EX_COMPOSITED

	DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VSCROLL |
		WS_CLIPSIBLINGS | WS_SIZEBOX;// // WS_CLIPCHILDREN


	HWND hWnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
		wndStyle, 0, 0, 0, 0, 0, 0, wcl.hInstance, 0);

	if (hWnd)
	{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	if (WinRect.right == 0)WinRect.right = screenWidth / 3;
	if (WinRect.bottom == 0) WinRect.bottom = screenHeight / 3;
	if (WinRect.left == 0) WinRect.left = (screenWidth - WinRect.right) / 2;
	if (WinRect.top == 0) WinRect.top = (screenHeight - WinRect.bottom) / 2;
	RECT rc = { 0 };

	SetRect(&rc, WinRect.left, WinRect.top, WinRect.right, WinRect.bottom);
		//AdjustWindowRectEx(&rc, wndStyle, FALSE, wndExStyle);
		MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

	}

	return hWnd;
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the 8th API window (fast parameter check)
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
void CALLBACK Sliders::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	RECT rect;

	// These variables are required for vertical scrolling. 
	static int yMinScroll;       // minimum vertical scroll value 
	static int yCurrentScroll;   // current vertical scroll value 
	static int yMaxScroll;       // maximum vertical scroll value 

	static bool fScroll;
	static SCROLLINFO si;

	switch (msg)
	{
	case WM_ACTIVATE:
		UpdateSliders(hWnd);
		break;

	case WM_CREATE:
	{
		CalculateNumberOfParameters();
		//MoveWindow(hWnd, 1, 200, 130, 400, TRUE);
		//ShowWindow(hWnd, SW_SHOW);
		// Initialize the vertical scrolling variables. 
		yMinScroll = 0;
		yCurrentScroll = 0;
		yMaxScroll = 0;
		fScroll = FALSE;

		yMaxScroll = MaxParameters * 40;
		yCurrentScroll = min(yCurrentScroll, yMaxScroll);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = yMinScroll;
		si.nMax = 1000;
		si.nPage = 170;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		LOGFONT MyFont_str;
		MyFont_str.lfHeight = 14;
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

		WindowWidth = 130;
		SliderInitialisation(hWnd);

	}
		break;

	case WM_HSCROLL://sliders
	{
		switch (LOWORD(wParam))
		{
		case TB_ENDTRACK:
			UpdateValuesFromSliders();
			break;
		case SB_THUMBTRACK:
			UpdateValuesFromSliders();
			break;
		}
		updata_windows(false);
	}
		break;

	case WM_VSCROLL: //window vertical slider
	{
		int yDelta;     // yDelta = new_pos - current_pos 
		int yNewPos;    // new position 

		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft above the scroll box. 
		case SB_PAGEUP:
			yNewPos = yCurrentScroll - 50;
			break;

			// User clicked the scroll bar shaft belkow the scroll box. 
		case SB_PAGEDOWN:
			yNewPos = yCurrentScroll + 50;
			break;

			// User clicked the top arrow. 
		case SB_LINEUP:
			yNewPos = yCurrentScroll - 5;
			break;

			// User clicked the bottom arrow. 
		case SB_LINEDOWN:
			yNewPos = yCurrentScroll + 5;
			break;

			// User dragged the scroll box. 
		case SB_THUMBPOSITION:
			yNewPos = HIWORD(wParam);
			break;

		default:
			yNewPos = yCurrentScroll;
		}

		// New position must be between 0 and the screen height. 
		yNewPos = max(0, yNewPos);
		yNewPos = min(yMaxScroll, yNewPos);

		// If the current position does not change, do not scroll.
		if (yNewPos == yCurrentScroll)
			break;

		// Set the scroll flag to TRUE. 
		fScroll = TRUE;

		// Determine the amount scrolled (in pixels). 
		yDelta = yNewPos - yCurrentScroll;

		// Reset the current scroll position. 
		yCurrentScroll = yNewPos;

		// Scroll the window. (The system repaints most of the 
		// client area when ScrollWindowEx is called; however, it is 
		// necessary to call UpdateWindow in order to repaint the 
		// rectangle of pixels that were invalidated.) 

		ScrollWindowEx(hWnd, 0, -yDelta, (CONST RECT *) NULL,
			(CONST RECT *) NULL, (HRGN)NULL, (LPRECT)NULL,
			SW_INVALIDATE);
		UpdateWindow(hWnd);

		// Reset the scroll bar. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		InvalidateRect(hWnd, NULL, TRUE);
		MoveElements(-yCurrentScroll, WindowWidth);

	}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		SelectObject(hdc, MyFont_Small);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 0));
		//BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBack, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);

	}
		break;

	case WM_DESTROY:
	{
		DeleteDC(hdc);
		DeleteObject(MyFont_Small);
		/*
		for (int i = 0; i < MaxParameters; i++)
		{
			if (hSlider) delete[] hSlider;
			if (hEdit) delete[] hEdit;
			if (hEditValue) delete[] hEditValue;
			if (hEditState) delete[] hEditState;
		}
		*/
	}
		break;

	case WM_SIZE:

		int yNewSize;
		yNewSize = HIWORD(lParam);

		yMaxScroll = max(MaxParameters * 40 - yNewSize, 0);
		yCurrentScroll = min(yCurrentScroll, yMaxScroll);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = yMinScroll;
		si.nMax = MaxParameters * 40;
		si.nPage = yNewSize;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		GetClientRect(hWnd, &rect);
		WindowWidth = rect.right - rect.left - 35;
		MoveElements(-yCurrentScroll, WindowWidth);
		InvalidateRect(hWnd, 0, 0);

		break;
	}

}

//-----------------------------------------------------------------------------
// Functions: Updates position of sliders after changes
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Sliders::UpdateSliders(HWND hwnd){

	int TOTAL = 0;
	int j;

	//Scale 1
	if (variable.SCALEPEN == true)
	{
		SetSlider(TOTAL, variable.SCALELIM[0], variable.SCALELIM[1], thread.SCALE);
		AddStateToEdit(TOTAL, variable.SCALEPEN);
		AddTextToEdit(TOTAL, thread.SCALE);
		TOTAL++;
	}

	//Subscale
	for (j = 0; j<variable.NSUBSCALETOT; j++){
		if (variable.SUBSCALEPEN[j] == true)
		{
			SetSlider(TOTAL, variable.SUBSCALELIM[j][0], variable.SUBSCALELIM[j][1], thread.SUBSCALE[j]);
			AddStateToEdit(TOTAL, variable.SUBSCALEPEN[j]);
			AddTextToEdit(TOTAL, thread.SUBSCALE[j]);
			TOTAL++;
		}
	}

	//Beta
	if (variable.BETAPEN == true)
	{
		SetSlider(TOTAL, variable.BETALIM[0], variable.BETALIM[1], thread.BETA);
		AddStateToEdit(TOTAL, variable.BETAPEN);
		AddTextToEdit(TOTAL, thread.BETA);
		TOTAL++;
	}

	//Surf. Frac.
	if (variable.SURFFRACPEN == true)
	{
		SetSlider(TOTAL, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1], thread.SURFFRAC);
		AddStateToEdit(TOTAL, variable.SURFFRACPEN);
		AddTextToEdit(TOTAL, thread.SURFFRAC);
		TOTAL++;
	}

	//Surf.2 Frac.
	if (thread.NSURF2>0)
	if (variable.SURF2FRACPEN == true)
	{
		SetSlider(TOTAL, variable.SURF2FRACLIM[0], variable.SURF2FRACLIM[1], thread.SURF2FRAC);
		AddStateToEdit(TOTAL, variable.SURF2FRACPEN);
		AddTextToEdit(TOTAL, thread.SURF2FRAC);
		TOTAL++;
	}

	//Displacement
	for (j = 0; j < variable.NDISTOT; j++){
		if (variable.DISPLPEN[j] == true)
		{
			SetSlider(TOTAL, variable.DISPLLIM[j][0], variable.DISPLLIM[j][1], variable.DISPL[j]);
			AddStateToEdit(TOTAL, variable.DISPLPEN[j]);
			AddTextToEdit(TOTAL, variable.DISPL[j]);
			TOTAL++;
		}
	}

	//DebWall
	for (j = 0; j < thread.NDWTOT; j++){
		if (variable.DEBWALPEN[j] == true)
		{
			SetSlider(TOTAL, variable.DEBWALLIM[j][0], variable.DEBWALLIM[j][1], thread.DEBWAL[j]);
			AddStateToEdit(TOTAL, variable.DEBWALPEN[j]);
			AddTextToEdit(TOTAL, thread.DEBWAL[j]);
			TOTAL++;
		}
	}

	//DebWall2
	for (j = 0; j < thread.NDWTOT2; j++){
		if (variable.DEBWAL2PEN[j] == true)
		{
			SetSlider(TOTAL, variable.DEBWAL2LIM[j][0], variable.DEBWAL2LIM[j][1], thread.DEBWAL2[j]);
			AddStateToEdit(TOTAL, variable.DEBWAL2PEN[j]);
			AddTextToEdit(TOTAL, thread.DEBWAL2[j]);
			TOTAL++;
		}
	}

	//Occ
	for (j = 0; j < variable.NOCCTOT; j++){
		if (variable.OCCUPPEN[j] == true)
		{
			SetSlider(TOTAL, variable.OCCUPLIM[j][0], variable.OCCUPLIM[j][1], thread.OCCUP[j]);
			AddStateToEdit(TOTAL, variable.OCCUPPEN[j]);
			AddTextToEdit(TOTAL, thread.OCCUP[j]);
			TOTAL++;
		}
	}

	//Occ2
	for (j = 0; j < variable.NOCCTOT2; j++) {
		if (variable.OCCUP2PEN[j] == true)
		{
			SetSlider(TOTAL, variable.OCCUP2LIM[j][0], variable.OCCUP2LIM[j][1], thread.OCCUP2[j]);
			AddStateToEdit(TOTAL, variable.OCCUP2PEN[j]);
			AddTextToEdit(TOTAL, thread.OCCUP2[j]);
			TOTAL++;
		}
	}

	//factor
	if (variable.NFACTOR > 0)
	{
		for (j = 0; j < variable.NFACTOR; j++) {
			if (variable.FACTOR_PEN[j] == true)
			{
				SetSlider(TOTAL, variable.FACTOR_LIM[j][0], variable.FACTOR_LIM[j][1], thread.FACTOR[j]);
				AddStateToEdit(TOTAL, variable.FACTOR_PEN[j]);
				AddTextToEdit(TOTAL, thread.FACTOR[j]);
				TOTAL++;
			}
		}
	}

	//Rotations
	for (j = 0; j < variable.ROTATIONTOTX; j++){
		if (variable.ROTATIONPENX[j] == true)
		{
			SetSlider(TOTAL, variable.ROTATIONLIMX[j][0], variable.ROTATIONLIMX[j][1], variable.ROTATIONX[j]);
			AddStateToEdit(TOTAL, variable.ROTATIONPENX[j]);
			AddTextToEdit(TOTAL, variable.ROTATIONX[j]);
			TOTAL++;
		}
	}

	for (j = 0; j < variable.ROTATIONTOTY; j++){
		if (variable.ROTATIONPENY[j] == true)
		{
			SetSlider(TOTAL, variable.ROTATIONLIMY[j][0], variable.ROTATIONLIMY[j][1], variable.ROTATIONY[j]);
			AddStateToEdit(TOTAL, variable.ROTATIONPENY[j]);
			AddTextToEdit(TOTAL, variable.ROTATIONY[j]);
			TOTAL++;
		}
	}

	for (j = 0; j < variable.ROTATIONTOTZ; j++){
		if (variable.ROTATIONPENZ[j] == true)
		{
			SetSlider(TOTAL, variable.ROTATIONLIMZ[j][0], variable.ROTATIONLIMZ[j][1], variable.ROTATIONZ[j]);
			AddStateToEdit(TOTAL, variable.ROTATIONPENZ[j]);
			AddTextToEdit(TOTAL, variable.ROTATIONZ[j]);
			TOTAL++;
		}
	}

	//Domain occ.
	if (variable.DOMOCCUPAUTO)
	{
		for (j = 0; j < thread.NDOMAIN; j++){
			if (variable.DOMOCCUPPEN[j] == true)
			{
				SetSlider(TOTAL, variable.DOMOCCUPLIM[j][0], variable.DOMOCCUPLIM[j][1], thread.DOMOCCUP[j]);
				AddStateToEdit(TOTAL, variable.DOMOCCUPPEN[j]);
				AddTextToEdit(TOTAL, thread.DOMOCCUP[j]);
				TOTAL++;
			}
		}
	}

	InvalidateRect(hwnd, 0, 0);
}

void Sliders::SliderInitialisation(HWND hWnd){
	int j;
	int TOTAL;
	static int oldNumberOfParameters = 0;
	int shift = 10;
	int shift_increment = 40;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	//Just in case calculate
	CalculateNumberOfParameters();

	if (oldNumberOfParameters != MaxParameters)
	{
		if (hSlider) delete[] hSlider;
		if (hEdit) delete[] hEdit;
		if (hEditValue) delete[] hEditValue;
		if (hEditState) delete[] hEditState;

		hSlider = new HWND[MaxParameters + 2];
		hEdit = new HWND[MaxParameters + 2];
		hEditValue = new HWND[MaxParameters + 2];
		hEditState = new HWND[MaxParameters + 2];
	}
	oldNumberOfParameters = MaxParameters;

	TOTAL = 0;

	//Scale 1
	if (variable.SCALEPEN == true)
	{
		CreateSlider(TOTAL, hWnd, 1, shift, L"Scale", -1, variable.SCALELIM[0], variable.SCALELIM[1], thread.SCALE, variable.SCALEPEN);
		shift += shift_increment;
		TOTAL++;
	}


	//Subscales
	for (j = 0; j < variable.NSUBSCALETOT; ++j){
		if (variable.SUBSCALEPEN[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Subscale", j, variable.SUBSCALELIM[j][0], variable.SUBSCALELIM[j][1], thread.SUBSCALE[j], variable.SUBSCALEPEN[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}


	//Beta
	if (variable.BETAPEN == true)
	{
		CreateSlider(TOTAL, hWnd, 1, shift, L"Beta", -1, variable.BETALIM[0], variable.BETALIM[1], thread.BETA, variable.BETAPEN);
		TOTAL++;
		shift += shift_increment;
	}

	//Surf. Frac.
	if (variable.SURFFRACPEN == true )
	{
		CreateSlider(TOTAL, hWnd, 1, shift, L"Surf. Fraction", -1, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1], thread.SURFFRAC, variable.SURFFRACPEN);
		TOTAL++;
		shift += shift_increment + 10;
	}

	if (thread.NSURF2>0)

		//Surf. 2Frac.
		if (variable.SURF2FRACPEN == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Surf.2 Fraction", -1, variable.SURF2FRACLIM[0], variable.SURF2FRACLIM[1], thread.SURF2FRAC, variable.SURF2FRACPEN);
			TOTAL++;
			shift += shift_increment + 10;
		}
	//Displacement
	for (j = 0; j < variable.NDISTOT; j++){
		if (variable.DISPLPEN[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Displacement", j, variable.DISPLLIM[j][0], variable.DISPLLIM[j][1], variable.DISPL[j], variable.DISPLPEN[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}

	//DebWall
	for (j = 0; j < thread.NDWTOT; j++){
		if (variable.DEBWALPEN[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Debye-Waller", j, variable.DEBWALLIM[j][0], variable.DEBWALLIM[j][1], thread.DEBWAL[j], variable.DEBWALPEN[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}

	//DebWall2
	for (j = 0; j < thread.NDWTOT2; j++){
		if (variable.DEBWAL2PEN[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Debye-Waller2", j, variable.DEBWAL2LIM[j][0], variable.DEBWAL2LIM[j][1], thread.DEBWAL2[j], variable.DEBWAL2PEN[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}


	//Occ
	for (j = 0; j < variable.NOCCTOT; j++){
		if (variable.OCCUPPEN[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Occupancy", j, variable.OCCUPLIM[j][0], variable.OCCUPLIM[j][1], thread.OCCUP[j], variable.OCCUPPEN[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}

	//Occ2
	for (j = 0; j < variable.NOCCTOT2; j++) {
		if (variable.OCCUP2PEN[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Occupancy2", j, variable.OCCUP2LIM[j][0], variable.OCCUP2LIM[j][1], thread.OCCUP2[j], variable.OCCUP2PEN[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}

	//factor
	if (variable.NFACTOR > 0)
	{
		for (j = 0; j < variable.NFACTOR; j++) {
			if (variable.FACTOR_PEN[j] == true)
			{
				CreateSlider(TOTAL, hWnd, 1, shift, L"Factor", j, variable.FACTOR_LIM[j][0], variable.FACTOR_LIM[j][1], thread.FACTOR[j], variable.FACTOR_PEN[j]);
				shift += shift_increment;
				TOTAL++;
			}
		}
	}
	//Rotations
	for (j = 0; j < variable.ROTATIONTOTX; j++){
		if (variable.ROTATIONPENX[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Rotation X", j, variable.ROTATIONLIMX[j][0], variable.ROTATIONLIMX[j][1], variable.ROTATIONX[j], variable.ROTATIONPENX[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}

	for (j = 0; j < variable.ROTATIONTOTY; j++){
		if (variable.ROTATIONPENY[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Rotation Y", j, variable.ROTATIONLIMY[j][0], variable.ROTATIONLIMY[j][1], variable.ROTATIONY[j], variable.ROTATIONPENY[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}

	for (j = 0; j < variable.ROTATIONTOTZ; j++){
		if (variable.ROTATIONPENZ[j] == true)
		{
			CreateSlider(TOTAL, hWnd, 1, shift, L"Rotation Z", j, variable.ROTATIONLIMZ[j][0], variable.ROTATIONLIMZ[j][1], variable.ROTATIONZ[j], variable.ROTATIONPENZ[j]);
			shift += shift_increment;
			TOTAL++;
		}
	}

	//Domain occ.
	if (variable.DOMOCCUPAUTO)
	{
		for (j = 0; j < thread.NDOMAIN; ++j){
			if (variable.DOMOCCUPPEN[j] == true)
			{
				CreateSlider(TOTAL, hWnd, 1, shift, L"Domain occ.", j, variable.DOMOCCUPLIM[j][0], variable.DOMOCCUPLIM[j][1], thread.DOMOCCUP[j], variable.DOMOCCUPPEN[j]);
				shift += shift_increment;
				TOTAL++;
			}
		}
	}

	MoveElements(MaxParameters, 0);
	SetFocus(hSlider[0]);
}


//-----------------------------------------------------------------------------
// Functions: Creates trackbar and sets its value according to the fit paramete value and its borders
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/windows/desktop/bb760151(v=vs.85).aspx
HWND Sliders::CreateSlider(int SliderNumber, HWND hWnd, int PositionX, int PositionY, WCHAR *title, int item, double Min, double Max, double value, bool  state){
	
	WCHAR TextValue[100];
	HINSTANCE g_hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	//Add title
	hEdit[SliderNumber] = CreateWindowEx(
		0, L"EDIT",										// predefined class 
		NULL,											// no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
		PositionX, PositionY, 150, 12,					// set size in WM_SIZE message 
		hWnd,											// parent window 
		(HMENU)NULL,									// edit control ID 
		(HINSTANCE)g_hInstance,									//GetWindowLong(hWnd, GWL_HINSTANCE)
		NULL);											// pointer not needed 

	ShowWindow(hEdit[SliderNumber], SW_SHOW);
	SendMessage(hEdit[SliderNumber], WM_SETFONT, (WPARAM)MyFont_Small, 0);//hFont
	EnableWindow(hEdit[SliderNumber], false);
	// Add text to the window. 
	if (item<0)
		swprintf_s(TextValue, 30, L"%s", title);
	else
		swprintf_s(TextValue, 30, L"%s %d", title, item+1);
	SendMessage(hEdit[SliderNumber], WM_SETTEXT, 0, (LPARAM)TextValue);

	// Add slider
	hSlider[SliderNumber] = CreateWindowEx(
		0,												// no extended styles 
		TRACKBAR_CLASS,									// class name 
		L"Trackbar Control",							// title (caption) 
		WS_CHILD |
		WS_VISIBLE |
		TBS_AUTOTICKS |
		TBS_ENABLESELRANGE,								// style 
		PositionX, PositionY + 15,						// position 
		150, 18,										// size 
		hWnd,											// parent window 
		NULL,										// control identifier 
		(HINSTANCE)g_hInstance,  // instance 
		NULL											// no WM_CREATE parameter 
		);

	SendMessage(hSlider[SliderNumber], TBM_SETRANGE,
		(WPARAM)TRUE,                   // redraw flag 
		(LPARAM)MAKELONG(0, 100));  // min. & max. positions

	SendMessage(hSlider[SliderNumber], TBM_SETPAGESIZE,
		0, (LPARAM)10);                  // new page size 

	SendMessage(hSlider[SliderNumber], TBM_SETSEL,
		(WPARAM)FALSE,                  // redraw flag 
		(LPARAM)MAKELONG(0, 100));


	SetSlider(SliderNumber, Min, Max, value);

	//Add value
	hEditValue[SliderNumber] = CreateWindowEx(
		0, L"EDIT",										// predefined class 
		NULL,											// no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
		PositionX + 160, PositionY, 50, 12,					// set size in WM_SIZE message 
		hWnd,											// parent window 
		(HMENU)NULL,									// edit control ID 
		(HINSTANCE)g_hInstance,
		NULL);											// pointer not needed 

	ShowWindow(hEditValue[SliderNumber], SW_SHOW);
	SendMessage(hEditValue[SliderNumber], WM_SETFONT, (WPARAM)MyFont_Small, 0);//hFont

	// Add text to the window. 

	swprintf_s(TextValue, 30, L"%4.3f", value);
	SendMessage(hEditValue[SliderNumber], WM_SETTEXT, 0, (LPARAM)TextValue);

	//Add parameter status free/fixed
	hEditState[SliderNumber] = CreateWindowEx(
		0, L"EDIT",										// predefined class 
		NULL,											// no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
		PositionX + 100, PositionY,						// position 
		50, 12,										// set size in WM_SIZE message 
		hWnd,											// parent window 
		(HMENU)NULL,									// edit control ID 
		(HINSTANCE)g_hInstance,
		NULL);											// pointer not needed 

	ShowWindow(hEditState[SliderNumber], SW_SHOW);
	SendMessage(hEditState[SliderNumber], WM_SETFONT, (WPARAM)MyFont_Small, 0);//hFont

	// Add text to the window. 
	AddStateToEdit(SliderNumber, state);


	return hSlider[SliderNumber];
}

//-----------------------------------------------------------------------------
// Function: Sets values of all sliders
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Sliders::SetSlider(int SliderNumber, double Min, double Max, double value){
	if (hSlider == NULL) return;

	//Protection
	if (Min > Max)
		Min = Max = value;

	if (hSlider[SliderNumber] != 0)
	{
		int position = (int)((value - Min) / (Max - Min) * 100);
		SendMessage(hSlider[SliderNumber], TBM_SETPOS,
			(WPARAM)TRUE,                  // redraw flag 
			(LPARAM)position);
	}
}

//-----------------------------------------------------------------------------
// Function: Get values of all sliders
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
double Sliders::GetSlider(int SliderNumber, double Min, double Max){
	if (hSlider == NULL) return 0;
	if (hSlider[SliderNumber] != 0)
	{
		double position = (double)SendMessage(hSlider[SliderNumber], TBM_GETPOS, 0, 0);
		return (((Max - Min)*position / 100. + Min));
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Functions: Get value from slider
// by Daniel Kaminski
//05.07.2014
//-----------------------------------------------------------------------------
void Sliders::AddTextToEdit(int SliderNumber, double value){
	if (hEditValue == NULL) return;
	// Add text to the value text editor. 
	if (hEdit == NULL) return;
	WCHAR TextValue[30];
	swprintf_s(TextValue, 30, L"%4.3f", value);
	SendMessage(hEditValue[SliderNumber], WM_SETTEXT, 0, (LPARAM)TextValue);
	ShowWindow(hEditValue[SliderNumber], SW_SHOW);
	InvalidateRect(hEditState[SliderNumber], 0, 0);

}

//-----------------------------------------------------------------------------
// Functions: Get value from slider
// by Daniel Kaminski
//05.07.2014
//-----------------------------------------------------------------------------
void Sliders::AddStateToEdit(int SliderNumber, bool state){
	if (hEditState == NULL) return;
	// Add text to the window. 
	if (state)
	{
		SendMessage(hEditState[SliderNumber], WM_SETTEXT, 0, (LPARAM)L"Free");
		EnableWindow(hSlider[SliderNumber], true);
	}
	else
	{
		SendMessage(hEditState[SliderNumber], WM_SETTEXT, 0, (LPARAM)L"Fixed");
		EnableWindow(hSlider[SliderNumber], false);
	}

	InvalidateRect(hEditState[SliderNumber], 0, 0);
}

//-----------------------------------------------------------------------------
// Functions: Updates position of sliders after changes
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Sliders::UpdateValuesFromSliders(void){

	int TOTAL = 0;
	int j;

	//Scale 1
	if (variable.SCALELIM[1] - variable.SCALELIM[0] != 0. && variable.SCALEPEN == true)
	{
		thread.SCALE = GetSlider(TOTAL, variable.SCALELIM[0], variable.SCALELIM[1]);
		AddTextToEdit(TOTAL, thread.SCALE);
		TOTAL++;
	}

	//Subscale
	for (j = 0; j < variable.NSUBSCALETOT; ++j){
		if ( variable.SUBSCALEPEN[j] == true)
		{
			thread.SUBSCALE[j] = GetSlider(TOTAL, variable.SUBSCALELIM[j][0], variable.SUBSCALELIM[j][1]);
			AddTextToEdit(TOTAL, thread.SUBSCALE[j]);
			TOTAL++;
		}
	}

	//Beta
	if ( variable.BETAPEN == true)
	{
		thread.BETA = GetSlider(TOTAL, variable.BETALIM[0], variable.BETALIM[1]);
		AddTextToEdit(TOTAL, thread.BETA);
		TOTAL++;
	}

	//Surf. Frac.
	if (variable.SURFFRACLIM[1] - variable.SURFFRACLIM[0] != 0. && variable.SURFFRACPEN == true)
	{
		//thread.SURFFRAC = (variable.SURFFRACLIM[1] - variable.SURFFRACLIM[0]) / 100.* SendMessage(hSlider[TOTAL], TBM_GETPOS, 0, 0);
		thread.SURFFRAC=GetSlider(TOTAL, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1]);
		AddTextToEdit(TOTAL, thread.SURFFRAC);
		TOTAL++;
	}

	//Surf.2 Frac.
	if (thread.NSURF2>0)
	if (variable.SURF2FRACLIM[1] - variable.SURF2FRACLIM[0] != 0. && variable.SURF2FRACPEN == true)
	{
		//thread.SURF2FRAC = (variable.SURF2FRACLIM[1] - variable.SURF2FRACLIM[0]) / 100.* SendMessage(hSlider[TOTAL], TBM_GETPOS, 0, 0);
		thread.SURF2FRAC = GetSlider(TOTAL, variable.SURF2FRACLIM[0], variable.SURF2FRACLIM[1]);
		AddTextToEdit(TOTAL, thread.SURF2FRAC);
		TOTAL++;
	}
	//Displacement
	for (j = 0; j < variable.NDISTOT; ++j){
		if (variable.DISPLLIM[j][1] - variable.DISPLLIM[j][0] != 0. && variable.DISPLPEN[j] == true)
		{
			variable.DISPL[j] = GetSlider(TOTAL, variable.DISPLLIM[j][0], variable.DISPLLIM[j][1]);
			AddTextToEdit(TOTAL, variable.DISPL[j]);
			TOTAL++;
		}
	}


	//DebWall
	for (j = 0; j<thread.NDWTOT; ++j){
		if (variable.DEBWALLIM[j][1] - variable.DEBWALLIM[j][0] != 0. && variable.DEBWALPEN[j] == true)
		{
			thread.DEBWAL[j] = GetSlider(TOTAL, variable.DEBWALLIM[j][0], variable.DEBWALLIM[j][1]);
			AddTextToEdit(TOTAL, thread.DEBWAL[j]);
			TOTAL++;
		}
	}

	//DebWall2
	for (j = 0; j < thread.NDWTOT2; ++j){
		if (variable.DEBWAL2LIM[j][1] - variable.DEBWAL2LIM[j][0] != 0. && variable.DEBWAL2PEN[j] == true)
		{
			thread.DEBWAL2[j] = GetSlider(TOTAL, variable.DEBWAL2LIM[j][0], variable.DEBWAL2LIM[j][1]);
			AddTextToEdit(TOTAL, thread.DEBWAL2[j]);
			TOTAL++;
		}
	}

	//Occ
	for (j = 0; j < variable.NOCCTOT; ++j){
		if (variable.OCCUPLIM[j][1] - variable.OCCUPLIM[j][0] != 0. && variable.OCCUPPEN[j] == true)
		{
			thread.OCCUP[j] = GetSlider(TOTAL, variable.OCCUPLIM[j][0], variable.OCCUPLIM[j][1]);
			AddTextToEdit(TOTAL, thread.OCCUP[j]);
			TOTAL++;
		}
	}

	//Occ2
	for (j = 0; j < variable.NOCCTOT2; ++j) {
		if (variable.OCCUP2LIM[j][1] - variable.OCCUP2LIM[j][0] != 0. && variable.OCCUP2PEN[j] == true)
		{
			thread.OCCUP2[j] = GetSlider(TOTAL, variable.OCCUP2LIM[j][0], variable.OCCUP2LIM[j][1]);
			AddTextToEdit(TOTAL, thread.OCCUP2[j]);
			TOTAL++;
		}
	}

	//factor
	if (variable.NFACTOR > 0)
	{
		for (j = 0; j < variable.NFACTOR; ++j) {
			if (variable.FACTOR_LIM[j][1] - variable.FACTOR_LIM[j][0] != 0. && variable.FACTOR_PEN[j] == true)
			{
				thread.FACTOR[j] = GetSlider(TOTAL, variable.FACTOR_LIM[j][0], variable.FACTOR_LIM[j][1]);
				AddTextToEdit(TOTAL, thread.FACTOR[j]);
				TOTAL++;
			}
		}
	}

	//Rotations
	for (j = 0; j < variable.ROTATIONTOTX; ++j){
		if (variable.ROTATIONLIMX[j][1] - variable.ROTATIONLIMX[j][0] != 0. &&  variable.ROTATIONPENX[j] == true)
		{
			variable.ROTATIONX[j] = GetSlider(TOTAL, variable.ROTATIONLIMX[j][0], variable.ROTATIONLIMX[j][1]);
			AddTextToEdit(TOTAL, variable.ROTATIONX[j]);
			TOTAL++;
		}
	}

	for (j = 0; j < variable.ROTATIONTOTY; ++j){
		if (variable.ROTATIONLIMY[j][1] - variable.ROTATIONLIMY[j][0] != 0. &&  variable.ROTATIONPENY[j] == true)
		{
			variable.ROTATIONY[j] = GetSlider(TOTAL, variable.ROTATIONLIMY[j][0], variable.ROTATIONLIMY[j][1]);
			AddTextToEdit(TOTAL, variable.ROTATIONY[j]);
			TOTAL++;
		}
	}

		for (j = 0; j< variable.ROTATIONTOTZ; ++j){
			if (variable.ROTATIONLIMZ[j][1] - variable.ROTATIONLIMZ[j][0] != 0. &&  variable.ROTATIONPENZ[j] == true)
			{
				variable.ROTATIONZ[j] = GetSlider(TOTAL, variable.ROTATIONLIMZ[j][0], variable.ROTATIONLIMZ[j][1]);
				AddTextToEdit(TOTAL, variable.ROTATIONZ[j]);
			TOTAL++;
			}
		}

	//Domain occ.
	if (variable.DOMOCCUPAUTO)
	{
		for (j = 0; j < thread.NDOMAIN; ++j){
			if (variable.DOMOCCUPLIM[j][1] - variable.DOMOCCUPLIM[j][0] != 0. &&  variable.DOMOCCUPPEN[j] == true)
			{
				thread.DOMOCCUP[j] = GetSlider(TOTAL, variable.DOMOCCUPLIM[j][0], variable.DOMOCCUPLIM[j][1]);
				AddTextToEdit(TOTAL, thread.DOMOCCUP[j]);
				TOTAL++;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Functions: Move win8 elements according to window slider
//03.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Sliders::MoveElements( int YPosition, int width){
	int i;
	int shift = 0;
	//CalculateNumberOfParameters();

	for (i = 0; i < MaxParameters + 1; ++i)
	{
		shift = i * 40;
		MoveWindow(hEdit[i], 1, YPosition + shift, 125, 12, TRUE);
		MoveWindow(hSlider[i], 1, YPosition + shift + 15, width-3, 18, TRUE);
		MoveWindow(hEditValue[i], 1 + width-3, YPosition + shift + 15, 130, 18, TRUE);
		MoveWindow(hEditState[i], 1 + width-3, YPosition + shift, 130, 18, TRUE);
	}
}

int Sliders::CalculateNumberOfParameters()
{

	MaxParameters = NSF + variable.NSUBSCALETOT + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT + variable.NOCCTOT2+ variable.NFACTOR;
	if (variable.MODE == 1)
		MaxParameters += variable.ROTATIONTOTX + variable.ROTATIONTOTY + variable.ROTATIONTOTZ;
	if (variable.DOMOCCUPAUTO)
		MaxParameters += thread.NDOMAIN;

	return MaxParameters;

}