#include "stdafx.h"
#include <Windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include "D2_helper.h"
#include "resource.h"


  

D2_helper::D2_helper()
{
}


D2_helper::~D2_helper()
{
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
			return (LONG)CreateSolidBrush(tmpRGBbackground);

		if (hCtl == GetDlgItem(hDlg, IDC_COLOR2))
			return (LONG)CreateSolidBrush(tmpRGBbackframe);

		if (hCtl == GetDlgItem(hDlg, IDC_COLOR3))
			return (LONG)CreateSolidBrush(tmpRGBtext);

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
