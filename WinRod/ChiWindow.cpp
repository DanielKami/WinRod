/***************************************************************************/
/*      This file contain all the functions necessary to operate chi windows.    
*/
/***************************************************************************/

#include <stdafx.h>
#include <ppl.h>			//For multithread calculations
#include <Windowsx.h>
#include <commdlg.h>
#include <math.h>
#include "resource.h"
#include <commctrl.h>
#include "ChiWindow.h"
#include "variables.h"		//Variables defined in structure not critical for thread calculations
#include "Thread.h"
#include "calc.h"
#include "Toolbar_Chi.h"
#include "ReadFile.h" ///T2A
#include "Save.h"
#include "Scene3D.h"
#include "fit.h"

extern Scene3D myScene;
//extern ModelStruct	Fit;
//-----------------------------------------------------------------------------
//Create the window 
// 03.07.2014 modified by Daniel Kamisnki
//-----------------------------------------------------------------------------
HWND ChiWin::CreateChiWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect)
{
	// Create a window that is centered on the desktop. It's exactly 1/4 the
	// size of the desktop.

	DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;//|WS_EX_COMPOSITED

	DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;// // WS_CLIPCHILDREN


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
// CreateToolbar.
//PURPOSE: 
//  Registers the TOOLBAR control class and creates a toolbar.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
HWND WINAPI ChiWin::CreateToolbar(HWND hwnd, HINSTANCE hInst)
{

	DWORD dwStyle;              // Style of the toolbar
	HFONT hFontRebar;

	hFontRebar = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tachoma");

	INITCOMMONCONTROLSEX iccex; // INITCOMMONCONTROLSEX structure
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&iccex);

	dwStyle = WS_CHILD | RBS_BANDBORDERS | RBS_AUTOSIZE | WS_CLIPSIBLINGS | WS_VISIBLE | RBS_TOOLTIPS | WS_BORDER;


	if (!(ChiWin::hRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, dwStyle, 0, 0, 0, 0, hwnd, NULL, hInst, NULL)))
	{
		MessageBox(hwnd, TEXT("Can't create rebar for chi_sqr window"), NULL, MB_ICONERROR);
		return NULL;
	}

	//Add rebar
	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof(REBARBANDINFO);
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RB_BEGINDRAG;
	rbbi.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS;// |RBBS_BREAK

	//  Create the toolbar control.
	//dwStyle = WS_VISIBLE | WS_CHILD |  CCS_NOPARENTALIGN;
	dwStyle = TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | TBSTYLE_FLAT;//

	if (!(hwndTB = CreateToolbarEx(ChiWin::hRebar, dwStyle, (UINT)ID_TOOLBAR_CHI, NUMIMAGES, hInst, IDB_TOOLBAR, tbButton, sizeof(tbButton) / sizeof(TBBUTTON),
		BUTTONWIDTH, BUTTONHEIGHT, IMAGEWIDTH, IMAGEHEIGHT, sizeof(TBBUTTON))))
	{
		MessageBox(hwnd, TEXT("Can't create toolbar for 2D window"), NULL, MB_ICONERROR);
		return NULL;
	}

	//Add tolbar to rebar
	rbbi.lpText = L"";
	rbbi.hwndChild = hwndTB;
	rbbi.cxMinChild = 400;
	rbbi.cyMinChild = MENU_HEIGHT + 4;
	rbbi.cx = 300;
	SendMessage(ChiWin::hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)& rbbi);



	//-----------------------------------------------------------------------------------

	DeleteObject(hFontRebar);
	return  ChiWin::hRebar;
}



LRESULT CALLBACK ChiWin::WinProc(HWND hWnd, UINT msg, HINSTANCE hinst, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc, hdcBack;
	RECT rect;
	static LOGFONT MyFont_str;
	static int size;

	switch (msg)
	{

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case TTN_GETDISPINFO:
			TooltipChi(lParam);
			break;
		}
		break;


	case WM_CREATE:
	{

		//Create back buffer for drawing
		hdc = GetDC(hWnd);
		hdcBack = CreateCompatibleDC(hdc);

		HBITMAP hbmBack = CreateCompatibleBitmap(hdc, 1024, 1024);
		SelectObject(hdcBack, (HBITMAP)hbmBack);
		PatBlt(hdcBack, 0, 0, 1024, 1024, COLOR_WINDOW); //PATINVERT
		DeleteObject(hbmBack);


		LOGFONT MyFont_str;
		MyFont_str.lfHeight = 16;
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
		MyFont_Medium = CreateFontIndirect(&MyFont_str);

		CreateToolbar(hWnd, hinst);

		Chi_Sqr = 0;
		MoveWindow(hWnd, variable.WindowsRect[9].left, variable.WindowsRect[9].top, variable.WindowsRect[9].right - variable.WindowsRect[9].left, variable.WindowsRect[9].bottom - variable.WindowsRect[9].top, TRUE);
		InvalidateRect(hWnd, 0, 0);

	}
		break;
	case WM_COMMAND:
		// Parse the menu selections:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_SAVE:
			ChiWin::SaveFileFunction(hWnd);
			break;

		case IDM_PRINT:
			ChiWin::PrintFunction(hWnd);
			break;

		case IDM_COPY_TO_CLIPOBARD:
			ChiWin::CopyToClipobard(hWnd);
			break;
		}


	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);

		FillRect(hdcBack, &rect, (HBRUSH)(COLOR_WINDOW + 0));
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBack, 0, 0, SRCCOPY);
		Scene(hWnd, hdcBack, size);
		BitBlt(hdc, 0, MENU_HEIGHT+10, rect.right, rect.bottom, hdcBack, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
		break;

	case WM_SIZE:
	{

		GetClientRect(hWnd, &rect);
		if (rect.right - rect.left < rect.bottom - rect.top)
			size = rect.right - rect.left;
		else
			size = rect.bottom - rect.top;

		if (size > 70) size = 70;
		MyFont_str.lfHeight = size / 5;
		MyFont_Medium = CreateFontIndirect(&MyFont_str);
		SendMessage(ChiWin::hwndTB, WM_SIZE, 0, 0);
		SendMessage(ChiWin::hRebar, WM_SIZE, 0, 0);
		InvalidateRect(hWnd, 0, 0);
	}
		break;

	case WM_DESTROY:
		DeleteDC(hdc);
		DeleteDC(hdcBack);
		break;

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Calculate Chi_sqr in fly after any model changes
//05.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
double ChiWin::CalculateChi_sqr(void){
	

	if (thread.NDAT == 0) return 0;
	//Calculate Chi^2
	/* Calculate and store constant values, in order to speed up fitting */
	calc.f_calc_fit_init(&thread);
	//Calculate data weights
	ComputeWeights();


	double y, temp, chisqr = 0, tempR = 0, R_coef = 0;

	for (int i = 0; i < thread.NDAT; ++i)
	{
		y = calc.f_calc_fit(&thread, i);
		temp = y - thread.FDAT[i];
		chisqr += temp*temp*thread.FWEIGHT[i];

		tempR = abs(thread.FDAT[i]) - abs(y);
		R_coef += abs(temp) / abs(thread.FDAT[i]);
	}


	chisqr /= thread.NDAT;
	if (chisqr < 0) chisqr = 0;
	Chi_Sqr = chisqr;
	sqrt_chi_Sqr = sqrt(Chi_Sqr);
	R = R_coef;
	return chisqr;
}


void ChiWin::PrintFunction(HWND hwnd)
{

	RECT rect; POINT pt;
	rect.bottom = 800; rect.right = 300;
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
		return;
	}
	StartPage(prn);


	SetMapMode(prn, MM_ISOTROPIC);
	SetWindowExtEx(prn, rect.bottom, rect.right, NULL);
	SetViewportExtEx(prn, cxpage, cypage, NULL);
	SetViewportOrgEx(prn, 0, 0, NULL);
	MoveToEx(prn, 0, cypage / 2, NULL);
	LineTo(prn, cxpage, (cypage / 2));

	prn2 = CreateCompatibleDC(prn);
	HBITMAP hBitmap = CreateCompatibleBitmap(prn, cxpage, cypage);
	SelectObject(prn2, (HBITMAP)hBitmap);
	DeleteBitmap(hBitmap);
	PatBlt(prn2, 0, 0, cxpage, cypage, WHITENESS);

	Scene(hwnd, prn, 70);

	if (EndPage(prn) <= 0)
	{
		MessageBox(NULL, L"It is a problem with printing. ", NULL, MB_OK);
	}
	EndDoc(prn);

	DeleteDC(prn);
	DeleteDC(prn2);
}

HDC ChiWin::GetPrinterDC(HWND Hwnd)
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



void ChiWin::Scene(HWND hwnd, HDC hdc_scene, int size)
{
	WCHAR Wstr[200];
	RECT rect2;
	SelectObject(hdc_scene, MyFont_Medium);

	SetBkMode(hdc_scene, TRANSPARENT);

	SetTextColor(hdc_scene, RGB(60, 0, 0));
	int spacer = 0;// size / 18;
	int text_height = size / 5;



	rect2.left = 2;
	rect2.right = rect2.left + 500;
	rect2.top = 2;
	int line = 0;
	rect2.top = text_height * line + spacer * line;
	rect2.bottom = rect2.top + text_height;

	swprintf_s(Wstr, 100, TEXT("Chi² = %4.3f normalized to %d data points, R = %4.3f"), Chi_Sqr, thread.NDAT, R);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	SetTextColor(hdc_scene, RGB(0, 0, 0));


	//Header
	//-----------------------------------------------------------------------------------
	int j;

	int left_position1 = 20, left_position2 = 130, left_position3 = 210;
	line++;
	rect2.top = text_height * line + spacer * line + 10;
	rect2.bottom = rect2.top + text_height;

	rect2.left = left_position1; rect2.right = rect2.left + 100;
	swprintf_s(Wstr, 100, TEXT("Parameter"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect2.left = left_position2; rect2.right = rect2.left + 50;
	swprintf_s(Wstr, 100, TEXT("Value"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	rect2.left = left_position3 + 10; rect2.right = rect2.left + 170;
	if (variable.LSQ_ERRORCALC == CHISQR) swprintf_s(Wstr, 100, TEXT("Error (CHISQR) • sqrt(Chi²)"));
	if (variable.LSQ_ERRORCALC == COVARIANCE) swprintf_s(Wstr, 100, TEXT("Error (COVARIANCE)"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	//-----------------------------------------------------------------------------------

	//Scale
	//-----------------------------------------------------------------------------------
	line++;
	rect2.top = text_height * line + spacer * line;
	rect2.bottom = rect2.top + text_height;

	rect2.left = left_position1; rect2.right = rect2.left + 300;
	swprintf_s(Wstr, 200, TEXT("______________________________________________________________________________________________________________________________"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	//Scale
	//-----------------------------------------------------------------------------------
	line++;
	rect2.top = text_height * line + spacer * line;
	rect2.bottom = rect2.top + text_height;

	rect2.left = left_position1; rect2.right = rect2.left + 100;
	swprintf_s(Wstr, 100, TEXT("Scale"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect2.left = left_position2; rect2.right = rect2.left + 50;
	swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.SCALE);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	rect2.left = left_position3; rect2.right = rect2.left + 60;
	if (!variable.SCALEPEN)
		swprintf_s(Wstr, 100, TEXT("---"));
	else
		if (variable.LSQ_ERRORCALC == CHISQR)
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SCALE_ERR* sqrt_chi_Sqr);
		else
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SCALE_ERR);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	//-----------------------------------------------------------------------------------

	//Subscales
	for (j = 0; j < variable.NSUBSCALETOT; j++){
		line++;
		rect2.top = text_height * line + spacer * line;
		rect2.bottom = rect2.top + text_height;

		rect2.left = left_position1; rect2.right = rect2.left + 100;
		swprintf_s(Wstr, 100, TEXT("Subscale %d"), j + 1);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect2.left = left_position2; rect2.right = rect2.left + 50;
		swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.SUBSCALE[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		rect2.left = left_position3; rect2.right = rect2.left + 60;
		if (!variable.SUBSCALEPEN[j])
			swprintf_s(Wstr, 100, TEXT("---"));
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SUBSCALE_ERR[j] * sqrt_chi_Sqr);
			else
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SUBSCALE_ERR[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}

	//Beta
	//----------------------------------------------------------------------------------------------------
	line++;
	rect2.top = text_height * line + spacer * line;
	rect2.bottom = rect2.top + text_height;

	rect2.left = left_position1; rect2.right = rect2.left + 100;
	swprintf_s(Wstr, 100, TEXT("Beta"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect2.left = left_position2; rect2.right = rect2.left + 50;
	swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.BETA);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	rect2.left = left_position3; rect2.right = rect2.left + 60;
	if (!variable.BETAPEN)
		swprintf_s(Wstr, 100, TEXT("---"));
	else
		if (variable.LSQ_ERRORCALC == CHISQR)
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.BETA_ERR*sqrt_chi_Sqr);
		else
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.BETA_ERR);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	//-----------------------------------------------------------------------------------------------------

	//Surf. Frac.
	//-----------------------------------------------------------------------------------------------------
	line++;
	rect2.top = text_height * line + spacer * line;
	rect2.bottom = rect2.top + text_height;

	rect2.left = left_position1; rect2.right = rect2.left + 100;
	swprintf_s(Wstr, 100, TEXT("Surf. Fraction"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect2.left = left_position2; rect2.right = rect2.left + 50;
	swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.SURFFRAC);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	rect2.left = left_position3; rect2.right = rect2.left + 60;
	if (!variable.SURFFRACPEN)
		swprintf_s(Wstr, 100, TEXT("---"));
	else
		if (variable.LSQ_ERRORCALC == CHISQR)
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SURFFRAC_ERR*sqrt_chi_Sqr);
		else
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SURFFRAC_ERR);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	//------------------------------------------------------------------------------------------------------


	//Surf. Frac.
	//-----------------------------------------------------------------------------------------------------
	
	if(thread.NSURF2>0)
	{
		line++;
	rect2.top = text_height * line + spacer * line;
	rect2.bottom = rect2.top + text_height;

	rect2.left = left_position1; rect2.right = rect2.left + 100;
	swprintf_s(Wstr, 100, TEXT("Surf2. Fraction"));
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect2.left = left_position2; rect2.right = rect2.left + 50;
	swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.SURF2FRAC);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	rect2.left = left_position3; rect2.right = rect2.left + 60;
	if (!variable.SURF2FRACPEN)
		swprintf_s(Wstr, 100, TEXT("---"));
	else
		if (variable.LSQ_ERRORCALC == CHISQR)
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SURF2FRAC_ERR*sqrt_chi_Sqr);
		else
			swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.SURF2FRAC_ERR);
	DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
}
	//-------------------------------------------------------------------------------------------------



	//Displacement
	//------------------------------------------------------------------------------------------------------
	for (j = 0; j < variable.NDISTOT; j++){
		line++;
		rect2.top = text_height * line + spacer * line;
		rect2.bottom = rect2.top + text_height;

		rect2.left = left_position1; rect2.right = rect2.left + 100;
		swprintf_s(Wstr, 100, TEXT("Displacement %d"), j + 1);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect2.left = left_position2; rect2.right = rect2.left + 50;
		swprintf_s(Wstr, 100, TEXT("%4.4f"), variable.DISPL[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		rect2.left = left_position3; rect2.right = rect2.left + 60;
		if (!variable.DISPLPEN[j])
			swprintf_s(Wstr, 100, TEXT("---"));
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.DISPL_ERR[j] * sqrt_chi_Sqr);
			else
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.DISPL_ERR[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}
	//------------------------------------------------------------------------------------------------------


	//DebWall
	//------------------------------------------------------------------------------------------------------
	for (j = 0; j < thread.NDWTOT; j++){
		line++;
		rect2.top = text_height * line + spacer * line;
		rect2.bottom = rect2.top + text_height;

		rect2.left = left_position1; rect2.right = rect2.left + 100;
		swprintf_s(Wstr, 100, TEXT("Debye-Waller %d"), j + 1);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect2.left = left_position2; rect2.right = rect2.left + 50;
		swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.DEBWAL[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		rect2.left = left_position3; rect2.right = rect2.left + 60;
		if (!variable.DEBWALPEN[j])
			swprintf_s(Wstr, 100, TEXT("---"));
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.DEBWAL_ERR[j] * sqrt_chi_Sqr);
			else
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.DEBWAL_ERR[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}
	//------------------------------------------------------------------------------------------------------

	//DebWall2
	//------------------------------------------------------------------------------------------------------
	for (j = 0; j < thread.NDWTOT2; j++){
		line++;
		rect2.top = text_height * line + spacer * line;
		rect2.bottom = rect2.top + text_height;

		rect2.left = left_position1; rect2.right = rect2.left + 100;
		swprintf_s(Wstr, 100, TEXT("Debye-Waller2 %d"), j + 1);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect2.left = left_position2; rect2.right = rect2.left + 50;
		swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.DEBWAL2[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		rect2.left = left_position3; rect2.right = rect2.left + 60;
		if (!variable.DEBWAL2PEN[j])
			swprintf_s(Wstr, 100, TEXT("---"));
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.DEBWAL2_ERR[j] * sqrt_chi_Sqr);
			else
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.DEBWAL2_ERR[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}
	//------------------------------------------------------------------------------------------------------

	//Occ
	//------------------------------------------------------------------------------------------------------
	for (j = 0; j < variable.NOCCTOT; j++){
		line++;
		rect2.top = text_height * line + spacer * line;
		rect2.bottom = rect2.top + text_height;

		rect2.left = left_position1; rect2.right = rect2.left + 100;
		swprintf_s(Wstr, 100, TEXT("Occ. %d"), j + 1);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect2.left = left_position2; rect2.right = rect2.left + 50;
		swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.OCCUP[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		rect2.left = left_position3; rect2.right = rect2.left + 60;
		if (!variable.OCCUPPEN[j])
			swprintf_s(Wstr, 100, TEXT("---"));
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.OCCUP_ERR[j] * sqrt_chi_Sqr);
			else
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.OCCUP_ERR[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}
	//------------------------------------------------------------------------------------------------------
	//Factor
	//------------------------------------------------------------------------------------------------------
	for (j = 0; j < variable.NFACTOR; j++) {
		line++;
		rect2.top = text_height * line + spacer * line;
		rect2.bottom = rect2.top + text_height;

		rect2.left = left_position1; rect2.right = rect2.left + 100;
		swprintf_s(Wstr, 100, TEXT("Factor %d"), j + 1);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect2.left = left_position2; rect2.right = rect2.left + 50;
		swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.FACTOR[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

		rect2.left = left_position3; rect2.right = rect2.left + 60;
		if (!variable.FACTOR_PEN[j])
			swprintf_s(Wstr, 100, TEXT("---"));
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.FACTOR_ERR[j] * sqrt_chi_Sqr);
			else
				swprintf_s(Wstr, 100, TEXT("±%4.4f"), thread.FACTOR_ERR[j]);
		DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}

	//rotation
	//------------------------------------------------------------------------------------------------------
	if (variable.MODE == 1)
	{
		for (j = 0; j < variable.ROTATIONTOTX; j++){
			line++;
			rect2.top = text_height * line + spacer * line;
			rect2.bottom = rect2.top + text_height;

			rect2.left = left_position1; rect2.right = rect2.left + 100;
			swprintf_s(Wstr, 100, TEXT("Rotation x %d"), j + 1);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

			rect2.left = left_position2; rect2.right = rect2.left + 50;
			swprintf_s(Wstr, 100, TEXT("%4.4f"), variable.ROTATIONX[j]);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left = left_position3; rect2.right = rect2.left + 60;
			if (!variable.ROTATIONPENX[j])
				swprintf_s(Wstr, 100, TEXT("---"));
			else
				if (variable.LSQ_ERRORCALC == CHISQR)
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.ROTATION_ERRX[j] * sqrt_chi_Sqr);
				else
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.ROTATION_ERRX[j]);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		}
		for (j = 0; j < variable.ROTATIONTOTY; j++){
			line++;
			rect2.top = text_height * line + spacer * line;
			rect2.bottom = rect2.top + text_height;

			rect2.left = left_position1; rect2.right = rect2.left + 100;
			swprintf_s(Wstr, 100, TEXT("Rotation y %d"), j + 1);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

			rect2.left = left_position2; rect2.right = rect2.left + 50;
			swprintf_s(Wstr, 100, TEXT("%4.4f"), variable.ROTATIONY[j]);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left = left_position3; rect2.right = rect2.left + 60;
			if (!variable.ROTATIONPENY[j])
				swprintf_s(Wstr, 100, TEXT("---"));
			else
				if (variable.LSQ_ERRORCALC == CHISQR)
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.ROTATION_ERRY[j] * sqrt_chi_Sqr);
				else
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.ROTATION_ERRY[j]);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		}
		for (j = 0; j < variable.ROTATIONTOTZ; j++){
			line++;
			rect2.top = text_height * line + spacer * line;
			rect2.bottom = rect2.top + text_height;

			rect2.left = left_position1; rect2.right = rect2.left + 100;
			swprintf_s(Wstr, 100, TEXT("Rotation z %d"), j + 1);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

			rect2.left = left_position2; rect2.right = rect2.left + 50;
			swprintf_s(Wstr, 100, TEXT("%4.4f"), variable.ROTATIONZ[j]);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left = left_position3; rect2.right = rect2.left + 60;
			if (!variable.ROTATIONPENZ[j])
				swprintf_s(Wstr, 100, TEXT("---"));
			else
				if (variable.LSQ_ERRORCALC == CHISQR)
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.ROTATION_ERRZ[j] * sqrt_chi_Sqr);
				else
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.ROTATION_ERRZ[j]);

			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		}
	}
	//Domain occ
	//------------------------------------------------------------------------------------------------------
	if (variable.DOMOCCUPAUTO)
	{
		double temp = 0;
		for (j = 1; j < thread.NDOMAIN; j++){
			temp += variable.DOMOCCUP_ERR[j] * variable.DOMOCCUP_ERR[j];
		}
		variable.DOMOCCUP_ERR[0] = sqrt(temp);

		for (j = 0; j < thread.NDOMAIN; j++){
			line++;
			rect2.top = text_height * line + spacer * line;
			rect2.bottom = rect2.top + text_height;

			rect2.left = left_position1; rect2.right = rect2.left + 100;
			swprintf_s(Wstr, 100, TEXT("Domain occ. %d"), j + 1);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

			rect2.left = left_position2; rect2.right = rect2.left + 50;
			swprintf_s(Wstr, 100, TEXT("%4.4f"), thread.DOMOCCUP[j]);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

			rect2.left = left_position3; rect2.right = rect2.left + 60;
			if (variable.DOMOCCUP_ERR[0] == 0 || (!variable.DOMOCCUPPEN[j] && j != 0))
				swprintf_s(Wstr, 100, TEXT("---"));
			else
				if (variable.LSQ_ERRORCALC == CHISQR)
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.DOMOCCUP_ERR[j] * sqrt_chi_Sqr);
				else
					swprintf_s(Wstr, 100, TEXT("±%4.4f"), variable.DOMOCCUP_ERR[j]);
			DrawText(hdc_scene, Wstr, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		}
	}
	//------------------------------------------------------------------------------------------------------


	POINT  Line[2];

	Line[0].x = left_position2; Line[1].x = left_position2;
	Line[0].y = 40;  Line[1].y = 1024;
	Polyline(hdc_scene, Line, 2);

	Line[0].x = left_position3; Line[1].x = left_position3;
	Line[0].y = 40;  Line[1].y = 1024;
	Polyline(hdc_scene, Line, 2);

}


//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the 3D scene to file as a one of the graphisc formats
//Created by Daniel Kaminski
//
// 19.02.2013
//-----------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
void ChiWin::SaveFileFunction(HWND hwnd)
{
	//--------------------------------------------------------------------------------------------------------------------------------

	HANDLE			hFile;
	WCHAR szFile[MAX_PATH] = TEXT("\0");
	char str[MAX_PATH] = "\0";

	DWORD  dwsize = 0;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Parameter file (*.par)\0*.par\0");
	ofn.lpstrTitle = TEXT("Save File");
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = TEXT("par");
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;



	if (GetSaveFileName(&ofn))
	{
		hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hwnd, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
			CloseHandle(hFile);
			return;
		}
	}
	else
		return;
	CloseHandle(hFile);

	TtoA(str, ofn.lpstrFile);

	sprintf_s(variable.FitHeader, 100, "Chi_sqr = %4.3f normalized to %d data points, R = %4.3f", Chi_Sqr, thread.NDAT, R);
	SaveParameterFile(str);
}

void ChiWin::CopyToClipobard(HWND hwnd)
{
	char Str[10000];
	TextToCopy(Str);
	const size_t len = strlen(Str) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), Str, len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();

}

void ChiWin::TextToCopy(char *str)
{
	char *item;
	int i, j;
	char *count = str;
	/* Copy to clipobard parameters from rod calculation by DK*/
	count += sprintf_s(count, 200, "Chi_sqr = %4.3f normalized to %d data points, R = %4.3f", Chi_Sqr, thread.NDAT, R);
	count += sprintf_s(count, 200, "%s\n", variable.FitHeader);
	count += sprintf_s(count, 200, "________________________________________________________________\r\n");
	if (variable.LSQ_ERRORCALC == CHISQR)
	count += sprintf_s(count, 200, "#Parameter             #Value    #Low lim.   #Up lim.  #Error (CHISQR) • sqrt(Chi_sqr)\r\n");
	else
		count += sprintf_s(count, 200, "#Parameter             #Value    #Low lim.   #Up lim.  #Error (COVARIANCE)\r\n");
	count += sprintf_s(count, 200, "________________________________________________________________\r\n");
	//                x               

	if (!variable.SCALEPEN)
		count += sprintf_s(count, 200, "scale               %10.6f %10.3f %10.3f   ----\r\n", thread.SCALE, variable.SCALELIM[0], variable.SCALELIM[1]);
	else
	if (variable.LSQ_ERRORCALC == CHISQR)
		count += sprintf_s(count, 200, "scale               %10.6f %10.3f %10.3f   %4.4f\r\n", thread.SCALE, variable.SCALELIM[0], variable.SCALELIM[1], thread.SCALE_ERR* sqrt_chi_Sqr);
	else
		count += sprintf_s(count, 200, "scale               %10.6f %10.3f %10.3f   %4.4f\r\n", thread.SCALE, variable.SCALELIM[0], variable.SCALELIM[1], thread.SCALE_ERR);

	//subscale
	for (i = 0; i < variable.NSUBSCALETOT; i++)
	{
		if (!variable.SUBSCALEPEN[i])
			count += sprintf_s(count, 200, "subscale    %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, thread.SUBSCALE[i], variable.SUBSCALELIM[i][0], variable.SUBSCALELIM[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "subscale    %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.SUBSCALE[i], variable.SUBSCALELIM[i][0], variable.SUBSCALELIM[i][1], thread.SUBSCALE_ERR[i] * sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "subscale    %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.SUBSCALE[i], variable.SUBSCALELIM[i][0], variable.SUBSCALELIM[i][1], thread.SUBSCALE_ERR[i]);
	}

	//beta
	if (!variable.BETAPEN)
		count += sprintf_s(count, 200, "beta                %10.6f %10.3f %10.3f   ----\r\n", thread.BETA, variable.BETALIM[0], variable.BETALIM[1]);
	else
	if (variable.LSQ_ERRORCALC == CHISQR)
		count += sprintf_s(count, 200, "beta                %10.6f %10.3f %10.3f   %4.4f\r\n", thread.BETA, variable.BETALIM[0], variable.BETALIM[1], thread.BETA_ERR*sqrt_chi_Sqr);
	else
		count += sprintf_s(count, 200, "beta                %10.6f %10.3f %10.3f   %4.4f\r\n", thread.BETA, variable.BETALIM[0], variable.BETALIM[1], thread.BETA_ERR);

	//SURFFRAC
	if (!variable.SURFFRACPEN)
		count += sprintf_s(count, 200, "sfraction           %10.6f %10.3f %10.3f   ----\r\n", thread.SURFFRAC, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1]);
	else
	if (variable.LSQ_ERRORCALC == CHISQR)
		count += sprintf_s(count, 200, "sfraction           %10.6f %10.3f %10.3f   %4.4f\r\n", thread.SURFFRAC, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1], thread.SURFFRAC_ERR*sqrt_chi_Sqr);
	else
		count += sprintf_s(count, 200, "sfraction           %10.6f %10.3f %10.3f   %4.4f\r\n", thread.SURFFRAC, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1], thread.SURFFRAC_ERR);

	if (thread.NSURF2 > 0)
	{
		//SURF2FRAC
		if (!variable.SURF2FRACPEN)
			count += sprintf_s(count, 200, "s2fraction           %10.6f %10.3f %10.3f   ----\r\n", thread.SURF2FRAC, variable.SURF2FRACLIM[0], variable.SURF2FRACLIM[1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "s2fraction           %10.6f %10.3f %10.3f   %4.4f\r\n", thread.SURF2FRAC, variable.SURF2FRACLIM[0], variable.SURF2FRACLIM[1], thread.SURF2FRAC_ERR*sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "s2fraction           %10.6f %10.3f %10.3f   %4.4f\r\n", thread.SURF2FRAC, variable.SURF2FRACLIM[0], variable.SURF2FRACLIM[1], thread.SURF2FRAC_ERR);

	}

	//DISPL
	for (i = 0; i < variable.NDISTOT; i++)
	{
		if (!variable.DISPLPEN[i])
			count += sprintf_s(count, 200, "displace    %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, variable.DISPL[i], variable.DISPLLIM[i][0], variable.DISPLLIM[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "displace    %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.DISPL[i], variable.DISPLLIM[i][0], variable.DISPLLIM[i][1], variable.DISPL_ERR[i] * sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "displace    %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.DISPL[i], variable.DISPLLIM[i][0], variable.DISPLLIM[i][1], variable.DISPL_ERR[i]);
	}

	//DW
	for (i = 0; i < thread.NDWTOT; i++)
	{
		if (!variable.DEBWALPEN[i])
			count += sprintf_s(count, 200, "Debye-Waller %3d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, thread.DEBWAL[i], variable.DEBWALLIM[i][0], variable.DEBWALLIM[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)

				count += sprintf_s(count, 200, "Debye-Waller %3d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.DEBWAL[i], variable.DEBWALLIM[i][0], variable.DEBWALLIM[i][1], thread.DEBWAL_ERR[i] * sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "Debye-Waller %3d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.DEBWAL[i], variable.DEBWALLIM[i][0], variable.DEBWALLIM[i][1], thread.DEBWAL_ERR[i]);
	}

	//DW2
	for (i = 0; i < thread.NDWTOT2; i++)
	{
		if (!variable.DEBWAL2PEN[i])
			count += sprintf_s(count, 200, "Debye-Waller2%3d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, thread.DEBWAL2[i], variable.DEBWAL2LIM[i][0], variable.DEBWAL2LIM[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "Debye-Waller2%3d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.DEBWAL2[i], variable.DEBWAL2LIM[i][0], variable.DEBWAL2LIM[i][1], thread.DEBWAL2_ERR[i] * sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "Debye-Waller2%3d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.DEBWAL2[i], variable.DEBWAL2LIM[i][0], variable.DEBWAL2LIM[i][1], thread.DEBWAL2_ERR[i]);
	}

	//occ
	for (i = 0; i < variable.NOCCTOT; i++)
	{
		if (!variable.OCCUPPEN[i])
			count += sprintf_s(count, 200, "occupancy   %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, thread.OCCUP[i], variable.OCCUPLIM[i][0], variable.OCCUPLIM[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "occupancy   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.OCCUP[i], variable.OCCUPLIM[i][0], variable.OCCUPLIM[i][1], thread.OCCUP_ERR[i] * sqrt_chi_Sqr);

			else
				count += sprintf_s(count, 200, "occupancy   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.OCCUP[i], variable.OCCUPLIM[i][0], variable.OCCUPLIM[i][1], thread.OCCUP_ERR[i]);
	}

	//occ2
	for (i = 0; i < variable.NOCCTOT2; i++)
	{
		if (!variable.OCCUP2PEN[i])
			count += sprintf_s(count, 200, "occupancy2   %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, thread.OCCUP2[i], variable.OCCUP2LIM[i][0], variable.OCCUP2LIM[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "occupancy2   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.OCCUP2[i], variable.OCCUP2LIM[i][0], variable.OCCUP2LIM[i][1], thread.OCCUP2_ERR[i] * sqrt_chi_Sqr);

			else
				count += sprintf_s(count, 200, "occupancy2   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.OCCUP2[i], variable.OCCUP2LIM[i][0], variable.OCCUP2LIM[i][1], thread.OCCUP2_ERR[i]);
	}

	//factor
	for (i = 0; i < variable.NFACTOR; i++)
	{
		if (!variable.FACTOR_PEN[i])
			count += sprintf_s(count, 200, "factor   %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, thread.FACTOR[i], variable.FACTOR_LIM[i][0], variable.FACTOR_LIM[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "factor   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.FACTOR[i], variable.FACTOR_LIM[i][0], variable.FACTOR_LIM[i][1], thread.FACTOR_ERR[i] * sqrt_chi_Sqr);

			else
				count += sprintf_s(count, 200, "factor   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.FACTOR[i], variable.FACTOR_LIM[i][0], variable.FACTOR_LIM[i][1], thread.FACTOR_ERR[i]);
	}

	//Rotations x
	for (i = 0; i < variable.ROTATIONTOTX; i++)
	{
		if (!variable.ROTATIONPENX[i])
			count += sprintf_s(count, 200, "rotation x   %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, variable.ROTATIONX[i], variable.ROTATIONLIMX[i][0], variable.ROTATIONLIMX[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "rotation x   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.ROTATIONX[i], variable.ROTATIONLIMX[i][0], variable.ROTATIONLIMX[i][1], variable.ROTATION_ERRX[i] * sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "rotation x   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.ROTATIONX[i], variable.ROTATIONLIMX[i][0], variable.ROTATIONLIMX[i][1], variable.ROTATION_ERRX[i]);
	}

	//Rotations y
	for (i = 0; i < variable.ROTATIONTOTY; i++)
	{
		if (!variable.ROTATIONPENY[i])
			count += sprintf_s(count, 200, "rotation y   %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, variable.ROTATIONY[i], variable.ROTATIONLIMY[i][0], variable.ROTATIONLIMY[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "rotation y   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.ROTATIONY[i], variable.ROTATIONLIMY[i][0], variable.ROTATIONLIMY[i][1], variable.ROTATION_ERRY[i] * sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "rotation y   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.ROTATIONY[i], variable.ROTATIONLIMY[i][0], variable.ROTATIONLIMY[i][1], variable.ROTATION_ERRY[i]);
	}

	//Rotations z
	for (i = 0; i < variable.ROTATIONTOTZ; i++)
	{
		if (!variable.ROTATIONPENZ[i])
			count += sprintf_s(count, 200, "rotation z   %4d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, variable.ROTATIONZ[i], variable.ROTATIONLIMZ[i][0], variable.ROTATIONLIMZ[i][1]);
		else
			if (variable.LSQ_ERRORCALC == CHISQR)
				count += sprintf_s(count, 200, "rotation z   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.ROTATIONZ[i], variable.ROTATIONLIMZ[i][0], variable.ROTATIONLIMZ[i][1], variable.ROTATION_ERRZ[i] * sqrt_chi_Sqr);
			else
				count += sprintf_s(count, 200, "rotation z   %4d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, variable.ROTATIONZ[i], variable.ROTATIONLIMZ[i][0], variable.ROTATIONLIMZ[i][1], variable.ROTATION_ERRZ[i]);
	}

	//DOMOCCUPAUTO
	if (variable.DOMOCCUPAUTO)
	{
		for (i = 0; i < thread.NDOMAIN; i++)
		{
			if (variable.DOMOCCUP_ERR[0] == 0 || (!variable.DOMOCCUPPEN[i] && i != 0))
				count += sprintf_s(count, 200, "domain_occ   %3d    %10.6f %10.3f %10.3f   ----\r\n", i + 1, thread.DOMOCCUP[i], variable.DOMOCCUPLIM[i][0], variable.DOMOCCUPLIM[i][1]);
			else
				if (variable.LSQ_ERRORCALC == CHISQR)
					count += sprintf_s(count, 200, "domain_occ   %3d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.DOMOCCUP[i], variable.DOMOCCUPLIM[i][0], variable.DOMOCCUPLIM[i][1], variable.DOMOCCUP_ERR[i] * sqrt_chi_Sqr);
				else
					count += sprintf_s(count, 200, "domain_occ   %3d    %10.6f %10.3f %10.3f   %4.4f\r\n", i + 1, thread.DOMOCCUP[i], variable.DOMOCCUPLIM[i][0], variable.DOMOCCUPLIM[i][1], variable.DOMOCCUP_ERR[i]);
		}

		count += sprintf_s(count, 200, "\r\n");
		for (i = 0; i < thread.NTYPES; i++)
		{
			for (j = 0; j < variable.ENERGYTOT + 1; j++)
			{
				count += sprintf_s(count, 200, "dispersion %2s %2d %8.4f %8.4f\r\n",
					variable.ELEMENT[i], j, thread.F1[i][j], thread.F2[i][j]);
			}
		}
	}
	switch (thread.ROUGHMODEL)
	{
	case APPROXBETA:
		item = "approx";
		break;
	case POISSONROUGH:
		item = "poisson";
		break;
	case NUMBETA:
		item = "beta";
		break;
	case GAUSSROUGH:
		item = "gaussian";
		break;
	case LINEARROUGH:
		item = "linear";
		break;
	case COSINEROUGH:
		item = "cosine";
		break;
	case TWOLEVEL:
		item = "twolevel";
		break;
	}
	count += sprintf_s(count, 200, "rough %s\r\n", item);

	//Second surface
	if (thread.NSURF2>0)
		count += sprintf_s(count, 200, "NSURF2 %d\r\n", thread.NSURF2);

}