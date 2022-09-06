#include <stdafx.h>
#include <windows.h>
#include <commdlg.h>
#include <math.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <math.h>
#include "Graph2D.h"
#include "Graph2D_Draw.h"



Graph2D_Draw::Graph2D_Draw()
{
}


Graph2D_Draw::~Graph2D_Draw()
{
}

void Graph2D_Draw::FillProperitiesList(HWND  hItemList, Data_struct* dataStr){

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

