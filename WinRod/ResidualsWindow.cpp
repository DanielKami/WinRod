
/***************************************************************************/
/*      This file gives support for residulal window.
		Created be: Daniel Kaminski
		09.09.2014 Lublin
*/
/***************************************************************************/
#include <stdafx.h>
#include "ResidualsWindow.h"
#include <math.h>
#include "variables.h"			//Variables defined in structure not critical for thread calculations
#include "Thread.h"
#include "calc.h"
#include "fit.h"

extern Calc calc;

ResidualsWindow::ResidualsWindow()
{
	ResidualsBank = NULL;
	ColorBank = NULL;

	hPen = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
	hBru = CreateSolidBrush(RGB(220, 220, 220));
	hBruWhite = CreateSolidBrush(RGB(255, 255, 255));

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
	MyFont_Residuals = CreateFontIndirect(&MyFont_str);
}


ResidualsWindow::~ResidualsWindow()
{
	delete[] ResidualsBank;
	DeleteObject(MyFont_Residuals);
	DeleteObject(hPen);
	DeleteObject(hBru);
	DeleteObject(hBruWhite);
	DeleteDC(hdc);
	DeleteDC(hdcBack);
}


//-----------------------------------------------------------------------------
//Create the window 
// 13.07.2014 modified by Daniel Kamisnki
//-----------------------------------------------------------------------------
HWND ResidualsWindow::CreateWin(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect)
{

	DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;//|WS_EX_COMPOSITED

	DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VSCROLL |
		WS_CLIPSIBLINGS | WS_SIZEBOX;// // WS_CLIPCHILDREN


	HWND hWnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
		wndStyle, 0, 0, 0, 0, 0, 0, wcl.hInstance, 0);

	if (hWnd)
	{
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
LRESULT CALLBACK ResidualsWindow::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;

	switch (msg)
	{
	case WM_ACTIVATE:
		Max = (int)(thread.NDAT * 20.5) + 10;
		UpdateResiduals(hWnd);
		Resize(hWnd, 100);

		break;

	case WM_CREATE:
	{
		// Initialize the vertical scrolling variables. 
		Max = (int)(thread.NDAT * 20.5) + 10;
		yMinScroll = 0;
		yCurrentScroll = 0;
		yMaxScroll = Max;
		fScroll = FALSE;

		Resize(hWnd, 100);



		//Create back buffer for drawing
		hdc = GetDC(hWnd);
		hdcBack = CreateCompatibleDC(hdc);

		HBITMAP hbmBack = CreateCompatibleBitmap(hdc, 512, 2048);
		SelectObject(hdcBack, (HBITMAP)hbmBack);
		PatBlt(hdcBack, 0, 0, 512, 2048, COLOR_WINDOW); //PATINVERT
		DeleteObject(hbmBack);
		UpdateResiduals(hWnd);		
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

		//ScrollWindowEx(hWnd, 0, -yDelta, (CONST RECT *) NULL,
		//	(CONST RECT *) NULL, (HRGN)NULL, (LPRECT)NULL,
		//	SW_INVALIDATE);
		//UpdateWindow(hWnd);

		// Reset the scroll bar. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		GetClientRect(hWnd, &rect);
		rect.right = 512;
		
		InvalidateRect(hWnd, &rect, 0);
	}
		break;


	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		DrawScene(hdcBack, rect);
		DrawHeader(hdcBack, rect);
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBack, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);

	}
		break;


	case WM_SIZE: 
		Resize(hWnd, HIWORD(lParam));
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ResidualsWindow::Resize(HWND hWnd, int yNewSize)
{

	RECT rect;


	yMaxScroll = max(Max - yNewSize, 0);
	yCurrentScroll = min(yCurrentScroll, yMaxScroll);
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = yMinScroll;
	si.nMax = Max;
	si.nPage = yNewSize;
	si.nPos = yCurrentScroll;
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

	GetWindowRect(hWnd, &rect);
	if (rect.right - rect.left > 512)
	{
		MoveWindow(hWnd, rect.left, rect.top, 512, rect.bottom - rect.top, TRUE);
	}
	InvalidateRect(hWnd, &rect, 0);

}

void ResidualsWindow::InitCalculateResiduals(){
	if (thread.NDAT == 0) return;

	if (ResidualsBank != NULL) delete[] ResidualsBank;
	ResidualsBank = new double[thread.NDAT+1];


	if (ColorBank != NULL) delete[] ColorBank;
	ColorBank = new int[thread.NDAT + 1];

	for (int i = 0; i < thread.NDAT; ++i)
	{
		ResidualsBank[i] = 0;
		ColorBank[i] = RGB(255, 255, 255);
	}

	//Calculate Chi^2
	/* Calculate and store constant values, in order to speed up fitting */
	calc.f_calc_fit_init(&thread);

	//Calculate data weights
	ComputeWeights();

}

void ResidualsWindow::CalculateResiduals(){
	if (ResidualsBank == NULL) return;//protect in case


	//Find max and min
	MaxResidual = 0;
	MinResidual = 10000000000;
	double y, temp, chisqr = 0;
	for (int i = 1; i < thread.NDAT; ++i){
		y = calc.f_calc_fit(&thread, i);
		temp = y - thread.FDAT[i];
		ResidualsBank[i] = temp*temp*thread.FWEIGHT[i] / thread.NDAT;

		if (ResidualsBank[i] < MinResidual) MinResidual = ResidualsBank[i];
		if (ResidualsBank[i] > MaxResidual) MaxResidual = ResidualsBank[i];

	}

	temp = 255/(MaxResidual - MinResidual);
	for (int i = 0; i < thread.NDAT; ++i){
		ColorBank[i] = (int)( (ResidualsBank[i] - MinResidual) * temp);
	}
}

void ResidualsWindow::UpdateResiduals(HWND hWnd){
	InitCalculateResiduals();
	CalculateResiduals();
	InvalidateRect(hWnd, 0, 0);
}


void ResidualsWindow::DrawScene(HDC hdc_in, RECT rect)
{
	RECT rect2;
	WCHAR Wstr[200];
	POINT line[2];

	FillRect(hdc_in, &rect, (HBRUSH)(COLOR_WINDOW + 1));
	if (ColorBank == NULL || ResidualsBank == NULL) return;
	SelectObject(hdc_in, MyFont_Residuals);
	SetTextColor(hdc_in, RGB(0, 0, 0));
	SelectObject(hdc_in, hPen);

	rect2.left = 2;
	rect2.right = rect2.left + 500;

	line[0].x = 0; line[1].x = rect.right;

	int spacer = 70; int width = 65;
	int start;
	start = yCurrentScroll / 20;
	if (start > -1)
		for (int i = start; i < thread.NDAT - 1; ++i)
		{

		rect2.top = 22 - yCurrentScroll + i * 20;
		if (rect2.top > 0)
		{
			rect2.bottom = rect2.top + 20;
			if (rect2.bottom > rect.bottom) break;

			line[1].y = line[0].y = rect2.top - 1;
			Polyline(hdc_in, line, 2);

			//Asign color
			if (ColorBank[i] < 128)
				SetBkColor(hdc_in, RGB(255, 255, 255 - ColorBank[i] * 2));
			if (ColorBank[i] >= 128)
				SetBkColor(hdc_in, RGB(255, 255 - ColorBank[i], 0));

			rect2.left = 1; rect2.right = rect2.left + 35;
			swprintf_s(Wstr, 200, TEXT("%7d"), i);
			DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left = 40; rect2.right = rect2.left + width;
			swprintf_s(Wstr, 200, TEXT("%15.5f"), thread.HDAT[i]);
			DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left += spacer; rect2.right = rect2.left + width;
			swprintf_s(Wstr, 200, TEXT("%15.5f"), thread.KDAT[i]);
			DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left += spacer; rect2.right = rect2.left + width;
			swprintf_s(Wstr, 200, TEXT("%15.5f"), thread.LDAT[i]);
			DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left += spacer; rect2.right = rect2.left + width;
			swprintf_s(Wstr, 200, TEXT("%15.5f"), thread.FDAT[i]);
			DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left += spacer; rect2.right = rect2.left + width;
			swprintf_s(Wstr, 200, TEXT("%15.5f"), variable.ERRDAT[i]);
			DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left += spacer; rect2.right = rect2.left + width;
			swprintf_s(Wstr, 200, TEXT("%15.5f"), ResidualsBank[i]);
			DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		}
		}
}

void ResidualsWindow::DrawHeader(HDC hdc_in, RECT rect)
{
	RECT rect2;
	WCHAR Wstr[200];
	POINT line[2];

	SelectObject(hdc_in, MyFont_Residuals);
	SetTextColor(hdc_in, RGB(0, 0, 0));
	SelectObject(hdc_in, hPen);

	//clean top area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = 18;
	rect2.bottom = rect2.top + 20;
	FillRect(hdc_in, &rect2, hBruWhite);

	line[1].y = line[0].y = rect2.bottom;
	line[1].x = 512; line[0].x = 0;
	Polyline(hdc_in, line, 2);

	line[1].y = line[0].y = rect2.bottom - 1;
	line[1].x = 512; line[0].x = 0;
	Polyline(hdc_in, line, 2);


	SetBkColor(hdc_in, RGB(255, 255, 255));
	swprintf_s(Wstr, 200, TEXT("   Nr                h                      k                       l                      F                    dF                residual"));
	rect2.top = 18;
	rect2.bottom = rect2.top + 20;
	DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	int increase = 70;
	line[0].x = 37; line[0].y = 0;
	line[1].x = 37; line[1].y = rect.bottom;
	Polyline(hdc_in, line, 2);

	line[0].x += increase; line[0].y = 0;
	line[1].x = line[0].x; line[1].y = rect.bottom;
	Polyline(hdc_in, line, 2);

	line[0].x += increase; line[0].y = 0;
	line[1].x = line[0].x; line[1].y = rect.bottom;
	Polyline(hdc_in, line, 2);

	line[0].x += increase; line[0].y = 0;
	line[1].x = line[0].x; line[1].y = rect.bottom;
	Polyline(hdc_in, line, 2);

	line[0].x += increase; line[0].y = 0;
	line[1].x = line[0].x; line[1].y = rect.bottom;
	Polyline(hdc_in, line, 2);

	line[0].x += increase; line[0].y = 0;
	line[1].x = line[0].x; line[1].y = rect.bottom;
	Polyline(hdc_in, line, 2);

	//clean top area
	rect2.left = 0;
	rect2.right = rect.right;
	rect2.top = 0;
	rect2.bottom = 18;
	FillRect(hdc_in, &rect2, hBru);

	SetBkColor(hdcBack, RGB(220, 220, 220));
	swprintf_s(Wstr, 200, TEXT(" Min residual = %4.5f   Max residual = %4.5f"), MinResidual, MaxResidual);
	rect2.top = 2;
	rect2.bottom = 18;
	DrawText(hdc_in, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}