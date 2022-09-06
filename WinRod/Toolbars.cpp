
//-----------------------------------------------------------------------------
// CreateToolbar.
//PURPOSE: 
//  Registers the TOOLBAR control class and creates a toolbar.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	

#include <stdafx.h>				//Header necessary for Visual Studio
#include <commctrl.h>
#include "Toolbars.h"


  HFONT hFont;


//                           hwnd       hinstance            Toolbar id      bitmap id     buttons properity    toltips
HWND WINAPI CreateToolbar(HWND hwnd, HWND *hwndTB4, HINSTANCE hInst, HWND *ComboBox_h, HWND *ComboBox_k, HWND *ComboBox_l, HWND *hComboRODs, int Width)
{
  HWND hRebar;
  HWND hwndTB;
  HWND hwndTB2;
  HWND hwndTB3;					//for hkl selection
//  HWND hwndTB4;					//Edit toolbar
  HWND hwndTB5;

  DWORD dwStyle;				// Style of the toolbar
  RECT rect; 

	if(!hFont) hFont = CreateFont (16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	                        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tachoma");
 
	INITCOMMONCONTROLSEX iccex; // INITCOMMONCONTROLSEX structure
	iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
	iccex.dwICC =  ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx( & iccex );

	dwStyle = WS_CHILD | RBS_AUTOSIZE | RBS_BANDBORDERS| WS_VISIBLE | RBS_TOOLTIPS  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER;//  | RBS_VARHEIGHT;

	if(!(hRebar = CreateWindowEx( WS_EX_TOOLWINDOW, REBARCLASSNAME, 0, dwStyle,	0, 0, 0, 0, hwnd, NULL, hInst, NULL ))){
		MessageBox(hwnd, TEXT("Can't create main toolbar "), NULL, MB_ICONERROR);
		return NULL;
	}

	//Add band
	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof( REBARBANDINFO );
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RB_BEGINDRAG;
	rbbi.fStyle = RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS ;//|RBBS_BREAK

	//  Create the toolbar control.
	//dwStyle = WS_VISIBLE | WS_CHILD |  CCS_NOPARENTALIGN;
	dwStyle = TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NODIVIDER | TBSTYLE_FLAT ;//


	if (!(hwndTB = CreateToolbarEx (hRebar, dwStyle, (UINT) ID_TOOLBAR_MAIN, 8, hInst, IDB_TOOLBAR_MAIN,
		tbButtonMain, sizeof (tbButtonMain) / sizeof (TBBUTTON), BUTTONWIDTH, BUTTONHEIGHT, IMAGEWIDTH, IMAGEHEIGHT, sizeof (TBBUTTON))))
		{
		MessageBox(hwnd, TEXT("Can't create main toolbar "), NULL, MB_ICONERROR);
		return NULL;
		}



	if (!(hwndTB2 = CreateToolbarEx (hRebar, dwStyle,(UINT) ID_TOOLBAR_WINDOWS, 9, hInst, IDB_TOOLBAR_WINDOWS, tbButton2, sizeof (tbButton2) / sizeof (TBBUTTON),
                                  BUTTONWIDTH, BUTTONHEIGHT, IMAGEWIDTH, IMAGEHEIGHT, sizeof (TBBUTTON))))
	{
		MessageBox(hwnd, TEXT("Can't create toolbar2 "), NULL, MB_ICONERROR);
		return NULL;
	}

	//For combo box ROD selection
	if (!(hwndTB5 = CreateToolbarEx(hRebar, dwStyle, (UINT)ID_TOOLBAR_RODS, 3, hInst, IDB_TOOLBAR_MAIN, tbButton4, 0,
		BUTTONWIDTH, BUTTONHEIGHT+4, IMAGEWIDTH, IMAGEHEIGHT, sizeof (TBBUTTON)))){
		MessageBox(hwnd, TEXT("Can't create toolbar for Combo RODs "), NULL, MB_ICONERROR);
		return NULL;
	}

	//Add tolbar to rebar 1
	rbbi.lpText = L"";
	rbbi.hwndChild = hwndTB;
	rbbi.cxMinChild = 200;
	rbbi.cyMinChild = MENU_HEIGHT+2;
	rbbi.cx = 200;
	SendMessage( hRebar, RB_INSERTBAND,( WPARAM ) - 1,( LPARAM ) & rbbi );

	//Add tolbar2 to rebar 1
	rbbi.lpText = L"";
	rbbi.hwndChild = hwndTB2;
	rbbi.cxMinChild = 240;
	rbbi.cyMinChild = MENU_HEIGHT+2;
	rbbi.cx = 240;
	SendMessage( hRebar, RB_INSERTBAND,( WPARAM ) - 1,( LPARAM ) & rbbi );

	//Edit toolbar
	if (!(*hwndTB4 = CreateToolbarEx(hRebar, dwStyle, (UINT)ID_TOOLBAR_EDIT, 2, hInst, IDB_TOOLBAR_EDIT, tbButtonsEdit, sizeof (tbButtonsEdit) / sizeof (TBBUTTON),
		BUTTONWIDTH, BUTTONHEIGHT, IMAGEWIDTH, IMAGEHEIGHT, sizeof (TBBUTTON))))
	{
		MessageBox(hwnd, TEXT("Can't create edit toolbar "), NULL, MB_ICONERROR);
		return NULL;
	}

	//Add tolbar4 to rebar 1
	rbbi.lpText = L"";
	rbbi.hwndChild = *hwndTB4;
	rbbi.cxMinChild = 70;
	rbbi.cyMinChild = MENU_HEIGHT+2;
	rbbi.cx = 30;
	SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)& rbbi);



//-----------------------------------------------------------------
//  h, k, l band
	if (!(hwndTB3 = CreateToolbarEx (hRebar, dwStyle,(UINT) ID_TOOLBAR_HKL, 1, hInst, NULL, tbButton3, 0,
                                  BUTTONWIDTH, BUTTONHEIGHT+4, IMAGEWIDTH, IMAGEHEIGHT, sizeof (TBBUTTON))))
	{
		MessageBox(hwnd, TEXT("Can't create toolbar3 "), NULL, MB_ICONERROR);
		return NULL;
	}

    //Combobox h       
	int TOOL_PLACEHOLDER = 0;
	int ComboWidth = 50;


	//Register combo
	TBBUTTON tbb[ 5 ];
	ZeroMemory( tbb, sizeof( tbb ) );
	tbb[ 0 ].idCommand = 0;
	tbb[ 0 ].fsState = TBSTATE_ENABLED;
	tbb[ 0 ].fsStyle = TBSTYLE_SEP;

	tbb[ 1 ].idCommand = 1;
	tbb[ 1 ].fsStyle = TBSTATE_ENABLED;
	tbb[ 1 ].fsStyle = TBSTYLE_SEP;

	tbb[ 2 ].idCommand = 2;
	tbb[ 2 ].fsState = TBSTATE_ENABLED;
	tbb[ 2 ].fsStyle = TBSTYLE_SEP;

	tbb[ 3 ].idCommand = 3;
	tbb[ 3 ].fsStyle = TBSTATE_ENABLED;
	tbb[ 3 ].fsStyle = TBSTYLE_SEP;

	tbb[ 4 ].idCommand = 4;
	tbb[ 4 ].fsState = TBSTATE_ENABLED;
	tbb[ 4 ].fsStyle = TBSTYLE_SEP;

	SendMessage( hwndTB3, TB_ADDSTRING, 0,( LPARAM ) "" );
	SendMessage( hwndTB3, TB_ADDBUTTONS, 5,( LPARAM ) & tbb );


	
	TBBUTTONINFO tbbi;
	ZeroMemory( & tbbi, sizeof( tbbi ) );
/*
	tbbi.cx = ComboWidth;
	tbbi.dwMask = TBIF_SIZE;//
	tbbi.pszText = L"k";
	tbbi.cbSize = sizeof( tbbi );
	tbbi.cchText = sizeof(tbbi.pszText);
	SendMessage( hwndTB3, TB_SETBUTTONINFO, 1,( LPARAM ) & tbbi );

	tbbi.cx = ComboWidth;
	tbbi.dwMask =TBIF_SIZE;//TBIF_TEXT| 
	tbbi.pszText = L"l";
	tbbi.cbSize = sizeof( tbbi );
	tbbi.cchText = sizeof(tbbi.pszText);
	SendMessage( hwndTB3, TB_SETBUTTONINFO, 3,( LPARAM ) & tbbi );
	SendMessage( hwndTB3, TB_ADDSTRING, 0,( LPARAM ) "" );
*/
	ZeroMemory( & tbbi, sizeof( tbbi ) );
	tbbi.cbSize = sizeof( tbbi );
	tbbi.dwMask = TBIF_SIZE;
	tbbi.cx = ComboWidth;
	//Set placeholder for combo
	SendMessage( hwndTB3, TB_SETBUTTONINFO, TOOL_PLACEHOLDER,( LPARAM ) & tbbi );
	//Get size of the combo
	SendMessage( hwndTB3, TB_GETITEMRECT, TOOL_PLACEHOLDER,( LPARAM ) & rect );
	//put the combo
	*ComboBox_h = CreateWindowEx( 0L, L"COMBOBOX", NULL,
								WS_CHILD | WS_BORDER | WS_VISIBLE | CBS_DROPDOWNLIST,
								rect.left, rect.top, rect.right - rect.left, 0,// rect.bottom - rect.top
								hwnd,( HMENU ) IDC_COMBOBOX_H, hInst, 0 );


	SendMessage(*ComboBox_h, WM_SETFONT, (WPARAM)hFont, 0);
	//Add controls for combo
	WCHAR wstr[5];	
	for(int i=-10; i<11; i++)
	{
		swprintf_s(wstr, 5, L"%d", i);
		SendMessage( *ComboBox_h, CB_ADDSTRING, 0,( LPARAM ) wstr );
	}

	//change the parent for combo
	SetParent( *ComboBox_h, hwndTB3);
//---
//CombpBox_k

    //Combobox k       
	TOOL_PLACEHOLDER = 2;
	ZeroMemory( & tbbi, sizeof( tbbi ) );
	tbbi.cbSize = sizeof( tbbi );
	tbbi.dwMask = TBIF_SIZE;
	tbbi.cx = ComboWidth;
	//Set placeholder for combo
	SendMessage( hwndTB3, TB_SETBUTTONINFO, TOOL_PLACEHOLDER,( LPARAM ) & tbbi );
	//Get size of the combo
	SendMessage( hwndTB3, TB_GETITEMRECT, TOOL_PLACEHOLDER,( LPARAM ) & rect );
	//put the combo
	*ComboBox_k = CreateWindowEx( 0L, L"COMBOBOX", NULL,
								WS_CHILD | WS_BORDER | WS_VISIBLE | CBS_DROPDOWNLIST,
								rect.left, rect.top, rect.right - rect.left, 0,// rect.bottom - rect.top
								hwnd,( HMENU ) IDC_COMBOBOX_K, hInst, 0 );


	SendMessage(*ComboBox_k, WM_SETFONT, (WPARAM)hFont, 0);
	//Add controls for combo
	for(int i=-10; i<11; i++){
		swprintf_s(wstr, 5, L"%d", i);
		SendMessage( *ComboBox_k, CB_ADDSTRING, 0,( LPARAM ) wstr );
	}

	//change the parent for combo
	SetParent( *ComboBox_k, hwndTB3);
//---
//CombpBox_l

    //Combobox l      
	TOOL_PLACEHOLDER = 4;
	ZeroMemory( & tbbi, sizeof( tbbi ) );
	tbbi.cbSize = sizeof( tbbi );
	tbbi.dwMask = TBIF_SIZE;
	tbbi.cx = ComboWidth;
	//Set placeholder for combo
	SendMessage( hwndTB3, TB_SETBUTTONINFO, TOOL_PLACEHOLDER,( LPARAM ) & tbbi );
	//Get size of the combo
	SendMessage( hwndTB3, TB_GETITEMRECT, TOOL_PLACEHOLDER,( LPARAM ) & rect );
	//put the combo
	*ComboBox_l = CreateWindowEx( 0L, L"COMBOBOX", NULL,
								WS_CHILD | WS_BORDER | WS_VISIBLE | CBS_DROPDOWNLIST,
								rect.left, rect.top, rect.right - rect.left, 0,// rect.bottom - rect.top
								hwnd,( HMENU ) IDC_COMBOBOX_L, hInst, 0 );


	SendMessage(*ComboBox_l, WM_SETFONT, (WPARAM)hFont, 0);
	//Add controls for combo
	for(int i=1; i<30; i++)
	{
		swprintf_s(wstr, 5, L"%2.1f", 0.1*i);
		SendMessage( *ComboBox_l, CB_ADDSTRING, 0,( LPARAM ) wstr );
	}

	//change the parent for combo
	SetParent( *ComboBox_l, hwndTB3);

	DWORD dwBtnSize =  (DWORD)SendMessage(hwndTB3, TB_GETBUTTONSIZE, 0,0);
	//Add tolbar to rebar 
	rbbi.lpText = L"Indices h, k, l";
	rbbi.hwndChild = hwndTB3;
	rbbi.cxMinChild = 200;
	rbbi.cyMinChild = HIWORD(dwBtnSize);//RebarHeight-6;
	rbbi.cx = 200;
	SendMessage( hRebar, RB_INSERTBAND,( WPARAM ) - 1,( LPARAM ) & rbbi );

	//Combo for RODs
	//__________________________________________________________________________________________________________________________

	int Combo2Width = 220;
	TOOL_PLACEHOLDER = 0;
	//Register combo
	ZeroMemory(tbb, sizeof(tbb));
	tbb[0].idCommand = TOOL_PLACEHOLDER;
	tbb[0].fsState = TBSTATE_ENABLED;
	tbb[0].fsStyle = TBSTYLE_SEP;
	SendMessage(hwndTB5, TB_ADDBUTTONS, 1, (LPARAM)& tbb);

	//TBBUTTONINFO tbbi;
	ZeroMemory(&tbbi, sizeof(tbbi));
	tbbi.cbSize = sizeof(tbbi);
	tbbi.dwMask = TBIF_SIZE;
	tbbi.cx = Combo2Width;
	//Set placeholder for combo
	SendMessage(hwndTB5, TB_SETBUTTONINFO, TOOL_PLACEHOLDER, (LPARAM)& tbbi);
	//Get size of the combo
	SendMessage(hwndTB5, TB_GETITEMRECT, TOOL_PLACEHOLDER, (LPARAM)& rect);
	//put the combo
	*hComboRODs = CreateWindowEx(0L, L"COMBOBOX", NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | CBS_DROPDOWNLIST,
		rect.left, rect.top, rect.right - rect.left, 0,
		hwnd, (HMENU)IDC_COMBOBOX_RODS, hInst, 0);

	SendMessage(*hComboRODs, WM_SETFONT, (WPARAM)hFont, 0);
	//Add controls for combo
//	SendMessage(*hComboRODs, CB_ADDSTRING, 0, (LPARAM)L"VDS");


	//change the parent for combo
	SetParent(*hComboRODs, hwndTB5);

	dwBtnSize =  (DWORD)SendMessage(hwndTB5, TB_GETBUTTONSIZE, 0,0);
	//Add tolbar to rebar 
	rbbi.lpText = L"RODs";
	rbbi.hwndChild = hwndTB5;
	rbbi.cxMinChild = 220;
	rbbi.cyMinChild = HIWORD(dwBtnSize);//HIWORD(dwBtnSize)RebarHeight-6;
	rbbi.cx = 90;
	SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)& rbbi);

 return hRebar;
}



