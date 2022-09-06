#include "stdafx.h"
#include <Windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include "math.h"
#include "2DGraphHelper.h"
#include "resource.h"


  

D2_helper::D2_helper()
{

	XYMinMax.bottom = 0;
	XYMinMax.top = 1;
	XYMinMax.left = 0;
	XYMinMax.right = 1;
	//X section
	XYMinMax.majorXticks = 1; //number of major ticks
	XYMinMax.minorXticks = 2;//number of minor ticks
	XYMinMax.Xprecision = 1; //number of digits after coma
	XYMinMax.flag_autoXprecision = true;
	XYMinMax.flag_autoXscaleMin = true;
	XYMinMax.flag_autoXscaleMax = true;
	XYMinMax.flag_autoXmajorTicks = true;
	XYMinMax.flag_autoXminorTicks = true;
	//Y section
	XYMinMax.majorYticks = 1; //number of major ticks
	XYMinMax.minorYticks = 5;//number of minor ticks
	XYMinMax.Yprecision = 1; //number of digits after coma
	XYMinMax.flag_autoYprecision = true;
	XYMinMax.flag_autoYscaleMin = true;
	XYMinMax.flag_autoYscaleMax = true;
	XYMinMax.flag_autoYmajorTicks = true;
	XYMinMax.flag_autoYminorTicks = true;
	XYMinMax.SelectedMenu = 0;


	XYMinMax.MyFont_str.lfHeight = 15;
	XYMinMax.MyFont_str.lfWidth = 0;
	XYMinMax.MyFont_str.lfEscapement = 0;
	XYMinMax.MyFont_str.lfOrientation = 0;
	XYMinMax.MyFont_str.lfWeight = 0;
	XYMinMax.MyFont_str.lfItalic = FALSE;
	XYMinMax.MyFont_str.lfUnderline = FALSE;
	XYMinMax.MyFont_str.lfStrikeOut = 0;
	XYMinMax.MyFont_str.lfCharSet = DEFAULT_CHARSET;
	XYMinMax.MyFont_str.lfOutPrecision = OUT_DEFAULT_PRECIS;
	XYMinMax.MyFont_str.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	XYMinMax.MyFont_str.lfPitchAndFamily = VARIABLE_PITCH;
	//Create the font
	XYMinMax.MyFont = CreateFontIndirect(&XYMinMax.MyFont_str);

	//Small font
	XYMinMax.MyFont_str.lfHeight = 15;

	//Create the font
	XYMinMax.MyFont_Small = CreateFontIndirect(&XYMinMax.MyFont_str);

	//X sale font
	XYMinMax.MyFont_strX.lfHeight = 15;
	XYMinMax.MyFont_strX.lfWidth = 0;
	XYMinMax.MyFont_strX.lfEscapement = 0;
	XYMinMax.MyFont_strX.lfOrientation = 0;
	XYMinMax.MyFont_strX.lfWeight = 0;
	XYMinMax.MyFont_strX.lfItalic = FALSE;
	XYMinMax.MyFont_strX.lfUnderline = FALSE;
	XYMinMax.MyFont_strX.lfStrikeOut = 0;
	XYMinMax.MyFont_strX.lfCharSet = DEFAULT_CHARSET;
	XYMinMax.MyFont_strX.lfOutPrecision = OUT_DEFAULT_PRECIS;
	XYMinMax.MyFont_strX.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	XYMinMax.MyFont_strX.lfPitchAndFamily = VARIABLE_PITCH;
	//Create the font
	XYMinMax.MyFont_X = CreateFontIndirect(&XYMinMax.MyFont_strX);

	//Y sale font
	XYMinMax.MyFont_strY.lfHeight = 15;
	XYMinMax.MyFont_strY.lfWidth = 0;
	XYMinMax.MyFont_strY.lfEscapement = 900;
	XYMinMax.MyFont_strY.lfOrientation = 900;
	XYMinMax.MyFont_strY.lfWeight = 0;
	XYMinMax.MyFont_strY.lfItalic = FALSE;
	XYMinMax.MyFont_strY.lfUnderline = FALSE;
	XYMinMax.MyFont_strY.lfStrikeOut = 0;
	XYMinMax.MyFont_strY.lfCharSet = DEFAULT_CHARSET;
	XYMinMax.MyFont_strY.lfOutPrecision = OUT_DEFAULT_PRECIS;
	XYMinMax.MyFont_strY.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	XYMinMax.MyFont_strY.lfPitchAndFamily = VARIABLE_PITCH;
	//Create the font
	XYMinMax.MyFont_Y = CreateFontIndirect(&XYMinMax.MyFont_strY);
}


D2_helper::~D2_helper()
{
	DeleteObject(XYMinMax.MyFont);
	DeleteObject(XYMinMax.MyFont_X);
	DeleteObject(XYMinMax.MyFont_Y);
	DeleteObject(XYMinMax.MyFont_Small);
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK D2_helper::Settings2D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){


	static COLORREF tmpRGBbackground;
	static COLORREF tmpRGBbackframe;
	static COLORREF tmpRGBtext;
	static int tmpWidth;
	static int tmpHeight;
	static Colors *colors;

	switch (message){
	case WM_INITDIALOG:
	{
		colors = (Colors*)lParam;// transport all colors to windows dialog (can't be done directly)
		tmpRGBbackground = colors->background;
		tmpRGBbackframe = colors->framecolor;
		tmpRGBtext = colors->text;
		tmpWidth = colors->BMP_Width;
		tmpHeight = colors->BMP_Height;
		SetDlgItemInt(hDlg, IDC_EDIT1, tmpWidth, 0);
		SetDlgItemInt(hDlg, IDC_EDIT2, tmpHeight, 0);
	}
		break;


	case WM_CTLCOLORSTATIC:
	{
		HWND hCtl = (HWND)lParam;

		if (hCtl == GetDlgItem(hDlg, IDC_COLOR))
			return (ULONG_PTR)CreateSolidBrush(tmpRGBbackground);

		if (hCtl == GetDlgItem(hDlg, IDC_COLOR2))
			return (ULONG_PTR)CreateSolidBrush(tmpRGBbackframe);

		if (hCtl == GetDlgItem(hDlg, IDC_COLOR3))
			return (ULONG_PTR)CreateSolidBrush(tmpRGBtext);

	}
		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{
			int tmpi;
			tmpi = GetDlgItemInt(hDlg, IDC_EDIT1, 0, 0);
			if (tmpi >= 0 && tmpi < 10000) tmpWidth = tmpi;
			else MessageBox(hDlg, L"Width out of range should be lower than 10000", NULL, MB_OK);

			tmpi = GetDlgItemInt(hDlg, IDC_EDIT2, 0, 0);
			if (tmpi >= 0 && tmpi < 10000) tmpHeight = tmpi;
			else MessageBox(hDlg, L"Height out of range should be lower than 10000", NULL, MB_OK);

			colors->background = tmpRGBbackground;
			colors->framecolor = tmpRGBbackframe;
			colors->text = tmpRGBtext;
			colors->BMP_Width = tmpWidth;
			colors->BMP_Height = tmpHeight;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL){
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDC_COLOR)
		{
			ColorSet(hDlg, IDC_COLOR, &tmpRGBbackground);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_COLOR2)
		{
			ColorSet(hDlg, IDC_COLOR2, &tmpRGBbackframe);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_COLOR3)
		{
			ColorSet(hDlg, IDC_COLOR3, &tmpRGBtext);
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK D2_helper::SettingsScale(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	static HFONT hFontHelper;
	static HWND hTab;
	static GRAPHSETTINGS *scale;

	switch (message)
	{

	case WM_INITDIALOG:
	{
		if (!hFontHelper) hFontHelper = CreateFont(14, 0, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tachoma");


		HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);

		INITCOMMONCONTROLSEX icc;
		icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icc.dwICC = (DWORD)ICC_TAB_CLASSES;
		InitCommonControlsEx(&icc);
	

		hTab = CreateWindowEx(0, WC_TABCONTROL, 0, WS_CHILD | WS_VISIBLE | TCS_OWNERDRAWFIXED
			,//| TCS_FIXEDWIDTH TCS_OWNERDRAWFIXED TCS_BUTTONS | TCS_FLATBUTTONS 
			0, 0, 444, 275, hDlg, 0, hInstance, NULL);

		//TabCtrl_SetItemSize(hTab, 150, 20);
		//SendMessage(hTab, WM_SETFONT, (WPARAM)hFontHelper, 0);

		TCITEM tci1, tci2, tci3, tci4, tci5;
		tci1.mask = TCIF_TEXT;
		tci2.mask = TCIF_TEXT;
		tci3.mask = TCIF_TEXT;
		tci4.mask = TCIF_TEXT;
		tci5.mask = TCIF_TEXT;
		tci1.pszText = L"Scale X";
		tci2.pszText = L"Scale Y";
		tci3.pszText = L"Title";
		tci4.pszText = L"Title X";
		tci5.pszText = L"Title Y";
		tci1.cchTextMax = sizeof(tci1.pszText);
		tci2.cchTextMax = sizeof(tci2.pszText);
		tci3.cchTextMax = sizeof(tci3.pszText);
		tci4.cchTextMax = sizeof(tci4.pszText);
		tci5.cchTextMax = sizeof(tci5.pszText);

		TabCtrl_InsertItem(hTab, ID_TAB_SCALE_X, &tci1);
		TabCtrl_InsertItem(hTab, ID_TAB_SCALE_Y, &tci2);
		TabCtrl_InsertItem(hTab, ID_TAB_TITLE, &tci3);
		TabCtrl_InsertItem(hTab, ID_TAB_TITLE_X, &tci4);
		TabCtrl_InsertItem(hTab, ID_TAB_TITLE_Y, &tci5);

		scale = (GRAPHSETTINGS*)lParam;// transport all colors to windows dialog (can't be done directly)


		//------------------------------------Scale X---------------------------------------------------------------------
		SetDlgItemReal(hDlg, IDC_EDIT1, scale->left, L"%3.3f");
		SetDlgItemReal(hDlg, IDC_EDIT2, scale->right, L"%3.3f");
		SetDlgItemReal(hDlg, IDC_EDIT3, scale->majorXticks, L"%3.0f");
		SetDlgItemReal(hDlg, IDC_EDIT4, scale->minorXticks, L"%3.0f");
		SetDlgItemText(hDlg, IDC_EDIT10, scale->Title);
		SetDlgItemText(hDlg, IDC_EDIT15, scale->TitleX);
		SetDlgItemText(hDlg, IDC_EDIT16, scale->TitleY);

		if (scale->flag_autoXscaleMin == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoXscaleMax == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoXmajorTicks == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoXminorTicks == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoXprecision == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_SETCHECK, BST_CHECKED, 0);

		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("0"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("2"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("3"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("4"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("5"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("6"));

		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)scale->Xprecision, 0);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT3), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT3), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT4), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT4), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_COMBO1), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_COMBO1), true);

		//-----------------------------------------Scale Y----------------------------------------------------------------------------------
		SetDlgItemReal(hDlg, IDC_EDIT5, scale->bottom, L"%3.3f");
		SetDlgItemReal(hDlg, IDC_EDIT7, scale->top, L"%3.3f");
		SetDlgItemReal(hDlg, IDC_EDIT8, scale->majorYticks, L"%3.0f");
		SetDlgItemReal(hDlg, IDC_EDIT9, scale->minorYticks, L"%3.0f");

		if (scale->flag_autoYscaleMin == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK6), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoYscaleMax == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK7), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoYmajorTicks == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK8), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoYminorTicks == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK9), BM_SETCHECK, BST_CHECKED, 0);
		if (scale->flag_autoYprecision == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK10), BM_SETCHECK, BST_CHECKED, 0);

		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("0"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("2"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("3"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("4"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("5"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("6"));

		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, (WPARAM)scale->Yprecision, 0);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK6), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT5), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT5), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK7), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK8), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT8), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT8), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK9), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT9), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_EDIT9), true);

		if (SendMessage(GetDlgItem(hDlg, IDC_CHECK10), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_COMBO2), false);
		else EnableWindow(GetDlgItem(hDlg, IDC_COMBO2), true);


		//-------------------------------------------Title---------------------------------------------------------------------------------
		int state0 = SW_HIDE, state1 = SW_HIDE, state2 = SW_HIDE, state3 = SW_HIDE, state4 = SW_HIDE, state5 = SW_HIDE;
		
		switch (scale->SelectedMenu)
		{
		case 0:
			TabCtrl_SetCurSel(hTab, 0);
			state0 = SW_SHOW;
			state1 = SW_SHOW;
			break;
		case 1:
			TabCtrl_SetCurSel(hTab, 1);
			state0 = SW_SHOW;
			state2 = SW_SHOW;
			break;
		case 2:
			TabCtrl_SetCurSel(hTab, 2);
			state3 = SW_SHOW;
			break;
		case 3:
			TabCtrl_SetCurSel(hTab, 3);
			state4 = SW_SHOW;
			break;
		case 4:
			TabCtrl_SetCurSel(hTab, 4);
			state5 = SW_SHOW;
			break;	
		}

		//Statics
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), state0);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC2), state0);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), state0);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC4), state0);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC5), state0);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC6), state0);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC7), state0);
		//X
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT3), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT4), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK1), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK2), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK3), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK4), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK5), state1);
		ShowWindow(GetDlgItem(hDlg, IDC_COMBO1), state1);
		//Y
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT5), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT7), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT8), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT9), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK6), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK7), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK8), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK9), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_CHECK10), state2);
		ShowWindow(GetDlgItem(hDlg, IDC_COMBO2), state2);
		//Tille
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT10), state3);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT15), state4);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT16), state5);

		ShowWindow(GetDlgItem(hDlg, IDC_BUTTON5), state3);
		ShowWindow(GetDlgItem(hDlg, IDC_BUTTON7), state4);
		ShowWindow(GetDlgItem(hDlg, IDC_BUTTON8), state5);

		ShowWindow(GetDlgItem(hDlg, IDC_STATIC8), state3);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC9), state4);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC10), state5);
		break;
	}

	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam; // item drawing information
		if (hTab == lpdis->hwndItem)
		{
			WCHAR szTabName[20];
			static TCITEM tci;
			tci.cchTextMax = 20;
			tci.mask = TCIF_TEXT;
			tci.pszText = szTabName;

			TabCtrl_GetItem(hTab, lpdis->itemID, &tci);
			//FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)GetStockObject(GRAY_BRUSH));
			//SetBkColor(lpdis->hDC, RGB(255, 0, 0));
			if (lpdis->itemState)
				SetTextColor(lpdis->hDC, RGB(255, 0, 0));
			else
				SetTextColor(lpdis->hDC, RGB(150, 150, 150));

			SelectObject(lpdis->hDC, hFontHelper);
			TextOut(lpdis->hDC, lpdis->rcItem.left+4, lpdis->rcItem.top+4, tci.pszText, lstrlen(tci.pszText));

		}

		break;
	}
	case WM_NOTIFY:
		LPNMHDR n;
		n = (LPNMHDR)lParam;
		if (n->code == TCN_SELCHANGE && n->hwndFrom == hTab) // Sprawdza, czy uchwyt pochodzi od hTab
		{
			int index = TabCtrl_GetCurSel(hTab);
			switch (index) // indeks aktualnej kontrolki
			{
			case ID_TAB_SCALE_X:

				//Statics
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC2), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC4), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC5), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC6), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC7), SW_SHOW);
				//X
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT3), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT4), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK1), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK2), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK3), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK4), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK5), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO1), SW_SHOW);
				//Y
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK6), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO2), SW_HIDE);

				//Tille
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT15), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT16), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON8), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC10), SW_HIDE);
				break;

			case ID_TAB_SCALE_Y:
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC2), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC4), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC5), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC6), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC7), SW_SHOW);

				//X
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO1), SW_HIDE);
				//Y
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT5), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT7), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT8), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT9), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK6), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK7), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK8), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK9), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK10), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO2), SW_SHOW);

				//Tille
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT15), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT16), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON8), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC10), SW_HIDE);
				break;

			case ID_TAB_TITLE:

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC6), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC7), SW_HIDE);

				//X
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO1), SW_HIDE);
				//Y
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK6), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO2), SW_HIDE);

				//Tille
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT10), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT15), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT16), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON5), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON8), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC8), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC10), SW_HIDE);
				break;


			case ID_TAB_TITLE_X:

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC6), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC7), SW_HIDE);

				//X
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO1), SW_HIDE);
				//Y
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK6), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO2), SW_HIDE);

				//Tille
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT15), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT16), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON7), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON8), SW_HIDE);

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC9), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC10), SW_HIDE);
				break;

			case ID_TAB_TITLE_Y:

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC6), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC7), SW_HIDE);

				//X
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK3), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK4), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO1), SW_HIDE);
				//Y
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK6), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COMBO2), SW_HIDE);

				//Tille
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT10), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT15), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT16), SW_SHOW);

				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON5), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON7), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BUTTON8), SW_SHOW);

				ShowWindow(GetDlgItem(hDlg, IDC_STATIC8), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC9), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC10), SW_SHOW);
				break;
			}
		}
		break;


	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{
			//X scale
			double temp;
			temp = GetDlgItemReal(hDlg, IDC_EDIT1);
			if (temp >= -1e9 && temp < 1e9) scale->left = temp;
			else
			{
				MessageBox(hDlg, L"Border out of range", NULL, MB_OK);
				break;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT2);
			if (temp >= -1e9 && temp < 1e9) scale->right = temp;
			else
			{
				MessageBox(hDlg, L"Border out of range", NULL, MB_OK);
				break;
			}
			temp = GetDlgItemReal(hDlg, IDC_EDIT3);
			if (temp > 0 && temp < 100) scale->majorXticks = temp;
			else
			{
				MessageBox(hDlg, L"Major ticks out of range", NULL, MB_OK);
				break;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT4);
			if (temp >= 1 && temp < 100) scale->minorXticks = temp;
			else
			{
				MessageBox(hDlg, L"Minor ticks out of range", NULL, MB_OK);
				break;
			}

			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoXscaleMin = true;
			else scale->flag_autoXscaleMin = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoXscaleMax = true;
			else scale->flag_autoXscaleMax = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoXmajorTicks = true;
			else scale->flag_autoXmajorTicks = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoXminorTicks = true;
			else scale->flag_autoXminorTicks = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoXprecision = true;
			else scale->flag_autoXprecision = false;

			scale ->Xprecision = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);

			//Y scale
			temp = GetDlgItemReal(hDlg, IDC_EDIT5);
			if (temp >= -1e9 && temp < 1e9) scale->bottom = temp;
			else
			{
				MessageBox(hDlg, L"Border out of range", NULL, MB_OK);
				break;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT7);
			if (temp >= -1e9 && temp < 1e9) scale->top = temp;
			else
			{
				MessageBox(hDlg, L"Border out of range", NULL, MB_OK);
				break;
			}
			temp = GetDlgItemReal(hDlg, IDC_EDIT8);
			if (temp > 0 && temp < 1e9) scale->majorYticks = temp;
			else
			{
				MessageBox(hDlg, L"Major ticks out of range", NULL, MB_OK);
				break;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT9);
			if (temp >= 1 && temp < 100) scale->minorYticks = temp;
			else
			{
				MessageBox(hDlg, L"Minor ticks out of range", NULL, MB_OK);
				break;
			}

			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK6), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoYscaleMin = true;
			else scale->flag_autoYscaleMin = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK7), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoYscaleMax = true;
			else scale->flag_autoYscaleMax = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK8), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoYmajorTicks = true;
			else scale->flag_autoYmajorTicks = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK9), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoYminorTicks = true;
			else scale->flag_autoYminorTicks = false;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK10), BM_GETCHECK, 0, 0) == BST_CHECKED) scale->flag_autoYprecision = true;
			else scale->flag_autoYprecision = false;

			scale->Yprecision = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);

			scale->MyFont_strY.lfEscapement = 900;
			scale->MyFont_strY.lfOrientation = 900;
			scale->MyFont = CreateFontIndirect(&scale->MyFont_str);
			scale->MyFont_X = CreateFontIndirect(&scale->MyFont_strX);
			scale->MyFont_Y = CreateFontIndirect(&scale->MyFont_strY);

			GetDlgItemText(hDlg, IDC_EDIT10, scale->Title, sizeof(WCHAR)*HEADER);
			GetDlgItemText(hDlg, IDC_EDIT15, scale->TitleX, sizeof(WCHAR)*HEADER);
			GetDlgItemText(hDlg, IDC_EDIT16, scale->TitleY, sizeof(WCHAR)*HEADER);
			DeleteObject(hFontHelper);
			hFontHelper = NULL;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			DeleteObject(hFontHelper);
			hFontHelper = NULL;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDC_CHECK1)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK2)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK3)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT3), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT3), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK4)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT4), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT4), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK5)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_COMBO1), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_COMBO1), true);
			break;
			break;
		}

		//Y scale
		if (LOWORD(wParam) == IDC_CHECK6)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK6), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT5), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT5), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK7)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK7), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK8)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK8), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT8), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT8), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK9)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK9), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT9), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT9), true);
			break;
		}

		if (LOWORD(wParam) == IDC_CHECK10)
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK10), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_COMBO2), false);
			else EnableWindow(GetDlgItem(hDlg, IDC_COMBO2), true);
			break;
		}
		
		if (LOWORD(wParam) == IDC_BUTTON5)
		{
			
			CHOOSEFONT cf;
			//Setting the CHOOSEFONT structure
			cf.lStructSize = sizeof(CHOOSEFONT);
			cf.hwndOwner = (HWND)NULL;
			cf.lpLogFont = &scale->MyFont_str;
			cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
			cf.rgbColors = RGB(0, 0, 0);
			cf.lCustData = 0L;
			cf.lpfnHook = (LPCFHOOKPROC)NULL;
			cf.lpTemplateName = (LPCWSTR)NULL;
			cf.hInstance = (HINSTANCE)NULL;
			cf.lpszStyle = (LPWSTR)NULL;
			cf.nFontType = SCREEN_FONTTYPE;
			cf.nSizeMin = 24;
			ChooseFont(&cf);
			
			break;
		}

		if (LOWORD(wParam) == IDC_BUTTON7)
		{

			CHOOSEFONT cf;
			//Setting the CHOOSEFONT structure
			cf.lStructSize = sizeof(CHOOSEFONT);
			cf.hwndOwner = (HWND)NULL;
			cf.lpLogFont = &scale->MyFont_strX;
			cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
			cf.rgbColors = RGB(0, 0, 0);
			cf.lCustData = 0L;
			cf.lpfnHook = (LPCFHOOKPROC)NULL;
			cf.lpTemplateName = (LPCWSTR)NULL;
			cf.hInstance = (HINSTANCE)NULL;
			cf.lpszStyle = (LPWSTR)NULL;
			cf.nFontType = SCREEN_FONTTYPE;
			cf.nSizeMin = 24;
			ChooseFont(&cf);

			break;
		}
		if (LOWORD(wParam) == IDC_BUTTON8)
		{

			CHOOSEFONT cf;
			//Setting the CHOOSEFONT structure
			cf.lStructSize = sizeof(CHOOSEFONT);
			cf.hwndOwner = (HWND)NULL;
			cf.lpLogFont = &scale->MyFont_strY;
			cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
			cf.rgbColors = RGB(0, 0, 0);
			cf.lCustData = 0L;
			cf.lpfnHook = (LPCFHOOKPROC)NULL;
			cf.lpTemplateName = (LPCWSTR)NULL;
			cf.hInstance = (HINSTANCE)NULL;
			cf.lpszStyle = (LPWSTR)NULL;
			cf.nFontType = SCREEN_FONTTYPE;
			cf.nSizeMin = 24;
			ChooseFont(&cf);

			break;
		}
	}
	return (INT_PTR)FALSE;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for ketting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK D2_helper::OpenDataPoints(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	static int nItem, nSubItem;
	static int ntot;
	static COLORREF tmpRGBcolor[MAX_OPEN];
	static COLORREF tmpRGBcolor_fill[MAX_OPEN];

	static COLORREF tmpRGBbackground = 1;
	static Data_struct* dataStr;

	LVCOLUMN		colNr;
	LVCOLUMN		colElement;
	LVCOLUMN		colStyle;
	LVCOLUMN		colThickness;
	LVCOLUMN		colColor;
	LVCOLUMN		colColor_fill;
	static HWND  hItemList;

	switch (message){
	case WM_INITDIALOG:



		dataStr = (Data_struct*)lParam;

		for (int i = 0; i < MAX_OPEN; i++){
			if (dataStr[i].readed == true){
				tmpRGBcolor[i] = dataStr[i].color;
				tmpRGBcolor_fill[i] = dataStr[i].color_fill;
			}
		}


		hItemList = GetDlgItem(hDlg, IDC_LIST2);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT2, WM_SETFOCUS, 0, 0);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create the columns in the list control
		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//nr
		colNr.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colNr.pszText = TEXT("Nr");
		colNr.cchTextMax = 100;
		colNr.cx = 65;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 0, &colNr);

		//header
		colElement.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colElement.pszText = TEXT("Readed data");
		colElement.cchTextMax = 40;
		colElement.cx = 70;
		colElement.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colElement);

		//style
		colStyle.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colStyle.pszText = TEXT("Symbol");
		colStyle.cchTextMax = 100;
		colStyle.cx = 70;
		colStyle.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 2, &colStyle);

		// thickness
		colThickness.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colThickness.pszText = TEXT("Thickness");
		colThickness.cchTextMax = 40;
		colThickness.cx = 40;
		colThickness.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 3, &colThickness);

		//color
		colColor.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colColor.pszText = TEXT("Color");
		colColor.cchTextMax = 40;
		colColor.cx = 40;
		colColor.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 4, &colColor);

		//color fill
		colColor_fill.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colColor_fill.pszText = TEXT("Color fill");
		colColor_fill.cchTextMax = 40;
		colColor_fill.cx = 40;
		colColor_fill.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 5, &colColor_fill);

		//Set values in table
		FillProperitiesList(hItemList, dataStr);

		ShowWindow(hDlg, SW_NORMAL);
		UpdateWindow(hDlg);
		break;


		/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

	case WM_NOTIFY:

	case IDC_LIST2:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

		if (pnm->hdr.hwndFrom == hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
		{
			SetWindowLong(hDlg, 0, (LONG)D2_helper::ProcessCustomDraw(lParam, 5, tmpRGBcolor_fill));
			SetWindowLong(hDlg, 0, (LONG)D2_helper::ProcessCustomDraw(lParam, 4, tmpRGBcolor));

			return TRUE;
		}
	}

		switch (((NMHDR *)lParam)->code)
		{

		case NM_CLICK://IDC_LIST1
			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= 2 && nSubItem <= 3)
			{
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT2), nItem, nSubItem, true);
			}

			D2_helper::OnClickListText(hDlg, GetDlgItem(hDlg, IDC_EDIT2), (NMHDR*)lParam, &nItem, &nSubItem, 2, 3);

			D2_helper::OnClickListColor(hDlg, GetDlgItem(hDlg, IDC_EDIT2), (NMHDR*)lParam, &nItem, &nSubItem, 5, &tmpRGBbackground, (COLORREF**)&tmpRGBcolor_fill);

			D2_helper::OnClickListColor(hDlg, GetDlgItem(hDlg, IDC_EDIT2), (NMHDR*)lParam, &nItem, &nSubItem, 4, &tmpRGBbackground, (COLORREF**)&tmpRGBcolor);

			SendMessage(GetDlgItem(hDlg, IDC_EDIT2), EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
			//SendDlgItemMessage(hDlg, IDC_LIST2, WM_KILLFOCUS, 0, 0);
			if (nItem > ListView_GetItemCount(hItemList) || nSubItem < 2 || nSubItem > 3)
			{
				SendDlgItemMessage(hDlg, IDC_EDIT2, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
			}
			break;
		}
		break;




	case WM_COMMAND:
		switch LOWORD(wParam){

		case IDC_EDIT2:

			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= 2 && nSubItem <= 3)
			{
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT2), nItem, nSubItem, true);
			}
			break;

		case IDOK:
		{
			for (int i = 0; i < MAX_OPEN; i++){
				dataStr[i].color = tmpRGBcolor[i];
				dataStr[i].color_fill = tmpRGBcolor_fill[i];
				if (ListView_GetCheckState(hItemList, i) == TRUE)
					dataStr[i].status = true;
				else
					dataStr[i].status = false;

				dataStr[i].style = GetItemInt(hItemList, i, 2);
				dataStr[i].thickness = GetItemInt(hItemList, i, 3);
			}
			nItem = 0; nSubItem = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
		{
			nItem = 0; nSubItem = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDC_BUTTON1:
		{
			HWND pwndCtrl = GetFocus();
			if (pwndCtrl == GetDlgItem(hDlg, IDC_EDIT2)){
				//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT2), nItem, nSubItem, true);
				SendDlgItemMessage(hDlg, IDC_EDIT2, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
				InvalidateRect(hDlg, 0, 0);
			}
		}
			break;

		case IDC_BUTTON2:

			for (int i = 0; i < MAX_OPEN; i++){
				ZeroMemory(&dataStr[i], sizeof(Data_struct));
			}

			SendMessage(hItemList, LVN_DELETEALLITEMS, 0, 0); // deleteall items
			for (int i = ListView_GetItemCount(hItemList); i > 0; i--){
				SendMessage(hItemList, LVM_DELETEITEM, ListView_GetItemCount(hItemList) - 1, 0); // delete the item selected		
			}
			break;

		case IDC_BUTTON3:
			SendMessage(hItemList, LVM_DELETEITEM, nItem, 0); // delete the item selected

			int j = 0;
			for (int i = 0; i < MAX_OPEN; i++){

				if (i == nItem)
					i++;
				CopyMemory(&dataStr[j], &dataStr[i], sizeof(Data_struct));
				tmpRGBcolor[j] = tmpRGBcolor[i];
				tmpRGBcolor_fill[j] = tmpRGBcolor_fill[i];
				j++;
			}
			break;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for ketting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK D2_helper::OpenFitPoints(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	static int nItem, nSubItem;
	static int ntot;
	static COLORREF tmpRGBcolor[MAX_OPEN];
	static COLORREF tmpRGBbackground = 1;
	static Data_struct* dataStr;

	LVCOLUMN		colNr;
	LVCOLUMN		colElement;
	LVCOLUMN		colStyle;
	LVCOLUMN		colThickness;
	LVCOLUMN		colColor;

	static HWND  hItemList;

	switch (message){
	case WM_INITDIALOG:

		dataStr = (Data_struct*)lParam;

		for (int i = 0; i < MAX_OPEN; i++){
			if (dataStr[i].readed == true){
				tmpRGBcolor[i] = dataStr[i].color;
			}
		}

		hItemList = GetDlgItem(hDlg, IDC_LIST2);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
		//SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create the columns in the list control
		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//nr
		colNr.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colNr.pszText = TEXT("Nr");
		colNr.cchTextMax = 100;
		colNr.cx = 65;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 0, &colNr);

		//header
		colElement.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colElement.pszText = TEXT("Readed data");
		colElement.cchTextMax = 40;
		colElement.cx = 70;
		colElement.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colElement);

		//style
		colStyle.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colStyle.pszText = TEXT("Style");
		colStyle.cchTextMax = 100;
		colStyle.cx = 70;
		colStyle.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 2, &colStyle);

		// thickness
		colThickness.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colThickness.pszText = TEXT("Thickness");
		colThickness.cchTextMax = 40;
		colThickness.cx = 40;
		colThickness.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 3, &colThickness);

		//color
		colColor.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colColor.pszText = TEXT("Color");
		colColor.cchTextMax = 40;
		colColor.cx = 40;
		colColor.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 4, &colColor);

		//Set values in table
		FillProperitiesList(hItemList, dataStr);

		ShowWindow(hDlg, SW_NORMAL);
		UpdateWindow(hDlg);
		break;


		/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

	case WM_NOTIFY:

	case IDC_LIST2:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

		if (pnm->hdr.hwndFrom == hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
		{
			SetWindowLong(hDlg, 0, (LONG)D2_helper::ProcessCustomDraw(lParam, 4, tmpRGBcolor));
			return TRUE;
		}
	}

		switch (((NMHDR *)lParam)->code)
		{

		case NM_CLICK://IDC_LIST1
			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= 2 && nSubItem <= 3)
			{
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
			}

			D2_helper::OnClickListText(hDlg, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)lParam, &nItem, &nSubItem, 2, 3);
			D2_helper::OnClickListColor(hDlg, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)lParam, &nItem, &nSubItem, 4, &tmpRGBbackground, (COLORREF**)&tmpRGBcolor);

			SendMessage(GetDlgItem(hDlg, IDC_EDIT1), EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
			if (nItem > ListView_GetItemCount(hItemList) || nSubItem < 2 || nSubItem > 3)
			{
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
			}
			//SendDlgItemMessage(hDlg, IDC_LIST2, WM_KILLFOCUS, 0, 0);
			break;
		}
		break;

	case WM_COMMAND:
		switch LOWORD(wParam){

		case IDC_EDIT1:

			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= 2 && nSubItem <= 3)
			{
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
			}
			break;

		case IDOK:
		{
			for (int i = 0; i < MAX_OPEN; i++){
				dataStr[i].color = tmpRGBcolor[i];
				if (ListView_GetCheckState(hItemList, i) == TRUE)
					dataStr[i].status = true;
				else
					dataStr[i].status = false;

				dataStr[i].style = GetItemInt(hItemList, i, 2);
				dataStr[i].thickness = GetItemInt(hItemList, i, 3);
			}
			nItem = 0; nSubItem = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
		{
			nItem = 0; nSubItem = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDC_BUTTON1:
		{
			HWND pwndCtrl = GetFocus();
			if (pwndCtrl == GetDlgItem(hDlg, IDC_EDIT1)){
				//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				InvalidateRect(hDlg, 0, 0);
			}
		}
			break;

		case IDC_BUTTON2:

			for (int i = 0; i < MAX_OPEN; i++){
				ZeroMemory(&dataStr[i], sizeof(Data_struct));
			}

			SendMessage(hItemList, LVN_DELETEALLITEMS, 0, 0); // deleteall items
			for (int i = ListView_GetItemCount(hItemList); i > 0; i--){
				SendMessage(hItemList, LVM_DELETEITEM, ListView_GetItemCount(hItemList) - 1, 0); // delete the item selected		
			}
			break;

		case IDC_BUTTON3:
			SendMessage(hItemList, LVM_DELETEITEM, nItem, 0); // delete the item selected

			int j = 0;
			for (int i = 0; i < MAX_OPEN; i++){

				if (i == nItem)
					i++;
				CopyMemory(&dataStr[j], &dataStr[i], sizeof(Data_struct));
				tmpRGBcolor[j] = tmpRGBcolor[i];
				j++;
			}
			InvalidateRect(hDlg, 0, 0);
			break;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

void D2_helper::FillProperitiesList(HWND  hItemList, Data_struct* dataStr){

	int i;
	WCHAR Tstr[256];


	LVITEM			LvItem;
	ListView_DeleteAllItems(hItemList);

	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of test

	for (i = 0; i <MAX_OPEN; i++){
		if (dataStr[i].readed){
			LvItem.iItem = i;
			swprintf_s(Tstr, 256, L"%4d", i + 1);
			LvItem.pszText = Tstr;
			LvItem.iSubItem = 0;
			SendMessage(hItemList, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send to the Listview
		}
	}

	LvItem.mask = LVIF_TEXT | LVIF_STATE | LVCF_SUBITEM;
	for (i = 0; i <MAX_OPEN; i++){
		if (dataStr[i].readed){
			LvItem.iItem = i;
			swprintf_s(Tstr, 256, L"%s", dataStr[i].header);
			LvItem.pszText = Tstr;
			LvItem.iSubItem = 1;
			SendMessage(hItemList, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter etxt to SubItems
			if (dataStr[i].status)	ListView_SetCheckState(hItemList, i, 1);
		}
	}


	for (i = 0; i <MAX_OPEN; i++){
		if (dataStr[i].readed){
			LvItem.iItem = i;
			swprintf_s(Tstr, 256, L"%d", dataStr[i].style);
			LvItem.pszText = Tstr;
			LvItem.iSubItem = 2;
			SendMessage(hItemList, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter etxt to SubItems
		}
	}

	//thickness
	for (i = 0; i <MAX_OPEN; i++){
		if (dataStr[i].readed){
			LvItem.iItem = i;
			swprintf_s(Tstr, 256, L"%d", dataStr[i].thickness);
			LvItem.pszText = Tstr;
			LvItem.iSubItem = 3;
			SendMessage(hItemList, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter etxt to SubItems
		}
	}

}

LRESULT D2_helper::ProcessCustomDraw(LPARAM lParam, int col, COLORREF* color)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT: //Before the paint cycle begins
		//request notifications for individual listview items
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_ITEMPREPAINT: //Before an item is drawn
	{
		return CDRF_NOTIFYSUBITEMDRAW;
	}
		break;
		//Before a subitem is drawn
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:

	{
		int i = (int)lplvcd->nmcd.dwItemSpec;
		if (col == lplvcd->iSubItem) //Color of atoms
		{
			//customize subitem appearance for column 0

			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = color[i];

			//To set a custom font:
			//SelectObject(lplvcd->nmcd.hdc, 
			//    <your custom HFONT>);

			return CDRF_NEWFONT;
		}
		else{
			//	lplvcd->clrText = RGB(0, 0, 0);
			//	lplvcd->clrTextBk = RGB(255, 255, 255);
			//	return CDRF_NEWFONT;
		}

	}
	}
	return CDRF_DODEFAULT;
}

void D2_helper::OnClickListColor(HWND hDlg, HWND hWndEditBox, NMHDR* pNMHDR, int *nItem, int *nSubItem, int max_SubItem, COLORREF *tmpRGBbackground, COLORREF **color)
{


	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	//get the row number
	*nItem = temp->iItem;
	//get the column number
	*nSubItem = temp->iSubItem;
	//Skip the second column this are names of parameters
	if (*nSubItem != max_SubItem || *nItem == -1)
		return;

	CHOOSECOLOR cc;                 // common dialog box structure 

	COLORREF BackgroundColor[16]; // array of custom colors
	int i = *nItem;
	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hDlg;
	cc.lpCustColors = (LPDWORD)BackgroundColor;
	cc.rgbResult = ((COLORREF*)color)[i];
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE){
		((COLORREF*)color)[i] = cc.rgbResult;//(*dataStr)[i].color if dataStr is ** type
	}
}

//this function will returns the item 
//text depending on the item and SubItem Index
int D2_helper::GetItemInt(HWND hWnd, int nItem, int nSubItem){

	int Value;
	WCHAR Wstr[128] = L"";
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iSubItem = nSubItem;
	lvi.cchTextMax = 128;
	lvi.pszText = Wstr;
	int nRes = (int)SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&lvi);
	if (swscanf_s(Wstr, L"%d", &Value) == 0) return 0;

	return Value;
}

//this function will returns the item 
//text depending on the item and SubItem Index
void D2_helper::GetItemText(HWND hWnd, WCHAR *Wstr, int nLen, int nItem, int nSubItem)
{
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iSubItem = nSubItem;
	lvi.cchTextMax = nLen;
	lvi.pszText = Wstr;
	int nRes = (int)SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&lvi);

}

//This function Displays an EditBox at the position where user clicks on a particular SubItem with 
//Rectangle are equal to the SubItem, thus allows to modify the value
void D2_helper::OnClickListText(HWND hDlg, HWND hWndEditBox, NMHDR* pNMHDR, int *nItem, int *nSubItem, int SkipColumnLow, int SkipColumnUp)
{
	//    InvalidateRect(hWndListBox, 0 ,0);

	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	RECT rect;
	//get the row number
	*nItem = temp->iItem;
	//get the column number
	*nSubItem = temp->iSubItem;
	//Skip the second column this are names of parameters
	if (*nSubItem < SkipColumnLow || *nSubItem > SkipColumnUp || *nItem == -1)
		return;

	RECT rect1, rect2;
	// this macro is used to retrieve the Rectanle  of the selected SubItem
	ListView_GetSubItemRect(temp->hdr.hwndFrom, temp->iItem, temp->iSubItem, LVIR_BOUNDS, &rect);
	//Get the Rectange of the listControl
	GetWindowRect(temp->hdr.hwndFrom, &rect1);
	//Get the Rectange of the Dialog
	GetWindowRect(hDlg, &rect2);

	int x = rect1.left - rect2.left - 3;
	int y = rect1.top - rect2.top;

	if (*nItem != -1)
		SetWindowPos(hWndEditBox, HWND_TOP, rect.left + x, rect.top + 13, rect.right - rect.left - 3, rect.bottom - rect.top - 1, SWP_DRAWFRAME);

	//Draw a Rectangle around the SubItem
	//    Rectangle(GetDC(temp->hdr.hwndFrom), rect.left,rect.top-1,rect.right,rect.bottom);
	//Set the listItem text in the EditBox
	WCHAR Wstr[128];
	GetItemText(temp->hdr.hwndFrom, Wstr, sizeof(Wstr), *nItem, *nSubItem);
	SetWindowText(hWndEditBox, Wstr);

	InvalidateRect(temp->hdr.hwndFrom, 0, 0);
	ShowWindow(hWndEditBox, SW_SHOW);
	SetFocus(hWndEditBox);
}

void D2_helper::SetCell(HWND hWnd1, WCHAR* szString, int nRow, int nCol){

	//Fill the LVITEM structure with the values given as parameters.
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = szString;
	lvItem.iSubItem = nCol;
	if (nCol >0)
		//set the value of listItem
		SendMessage(hWnd1, LVM_SETITEM, (WPARAM)0, (WPARAM)&lvItem);
	else
		//Insert the value into List
		ListView_InsertItem(hWnd1, &lvItem);
}

void D2_helper::SetCell(HWND hWnd1, double value, int nRow, int nCol, bool Int){

	WCHAR     szString[256];
	if (Int)
		swprintf_s(szString, L"%d", (int)value);
	else
		swprintf_s(szString, L"%7.4f", value);

	//Fill the LVITEM structure with the values given as parameters.
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = szString;
	lvItem.iSubItem = nCol;
	if (nCol >0)
		//set the value of listItem
		SendMessage(hWnd1, LVM_SETITEM, (WPARAM)0, (WPARAM)&lvItem);
	else
		//Insert the value into List
		ListView_InsertItem(hWnd1, &lvItem);

}

/*--------------------------------------------------------------------------------------------------------------------------------
Modules:      GetDlgItemReal
description:  GetReal value from dialog
Created by:   Daniel Kaminski
Modyfied by:
Data:         2007
Another:      Extension for API
/--------------------------------------------------------------------------------------------------------------------------------*/
double D2_helper::GetDlgItemReal(HWND hDlg, int nIDD){

	float temp2;
	WCHAR temp_string[20];   //read line of text only 20 characters

	GetDlgItemText(hDlg, nIDD, temp_string, 20); //STRING_SIZE only 20 characters!
	if (temp_string[0] == L'-' && temp_string[1] == 0) temp_string[1] = L'0';
	if (temp_string[0] == L'-' && temp_string[1] == L'.' && temp_string[2] == 0){ temp_string[1] = L'0'; temp_string[2] = L'.'; }
	swscanf_s(temp_string, L"%f", &temp2);

	return (double)temp2;
}

/*--------------------------------------------------------------------------------------------------------------------------------
Modules:      SetDlgItemReal
description:  SetReal value from dialog
Created by:   Daniel Kaminski
Modyfied by:
Data:         2007
Another:      Extension for API
/--------------------------------------------------------------------------------------------------------------------------------*/
void D2_helper::SetDlgItemReal(HWND hDlg, int nIDD, double Value, WCHAR *FORMAT)
{

	WCHAR temp_str[100];
	if (fabs(Value) > 0.0000001)
		swprintf_s(temp_str, 100, FORMAT, Value);
	else
		if (Value == 0)
			swprintf_s(temp_str, 100, TEXT("%1.1f"), Value);
		else
			swprintf_s(temp_str, 100, TEXT("%5.2e"), Value);
	SetDlgItemText(hDlg, nIDD, temp_str);
}

void D2_helper::ColorSet(HWND hDlg, int IDC_PictureBox, COLORREF *color)
{
	HWND PictureBox = GetDlgItem(hDlg, IDC_PictureBox);

	CHOOSECOLOR cc;                 // common dialog box structure 
	COLORREF BackgroundColor[16]; // array of custom colors
	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hDlg;
	cc.lpCustColors = (LPDWORD)BackgroundColor;
	cc.rgbResult = *color;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE){
		*color = cc.rgbResult;
		InvalidateRect(PictureBox, 0, 0);
	}
}
