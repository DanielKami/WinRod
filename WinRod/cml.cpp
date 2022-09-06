//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: cml.cpp contains all functions vital for command line interpretation and drawing
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#include <stdafx.h>
//--------------------------------------------------------------------------------------------------------------------------------
//      include files                                                  
//--------------------------------------------------------------------------------------------------------------------------------
#define CML
#include <stdio.h>
#include "definitions.h"


#include "set.h"
#include "cml.h"
#include "plot.h"

#include "Thread.h"
#include "variables.h"
#include "ReadFile.h"
#include "Save.h"
#include "fit.h"
#include "WinRod.h"
#include "Scene3D.h"
#include "GlobalFunctions.h"

extern CGlobalFunctions	myVersion;					//to retrive version info


extern Scene3D myScene;
Menu menu;


SCROLLINFO si; 
// These variables are required for horizontal scrolling. 
static int xMinScroll;       // minimum horizontal scroll value 
static int xCurrentScroll;   // current horizontal scroll value 
static int xMaxScroll;       // maximum horizontal scroll value 
 
// These variables are required for vertical scrolling. 
static int yMinScroll;       // minimum vertical scroll value 
static int yCurrentScroll;   // current vertical scroll value 
static int yMaxScroll;       // maximum vertical scroll value 

static BOOL fBlt;            // TRUE if BitBlt occurred 
static BOOL fScroll;         // TRUE if scrolling occurred 
static BOOL fSize;           // TRUE if fBlt & WM_SIZE 

static BITMAP bmp;           // bitmap data structure 


//-----------------------------------------------------------------------------
	//Create the window 
	// 21.01.2013 modified by Daniel Kamisnki
//-----------------------------------------------------------------------------
HWND Cml::CreateAppWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, int left, int top, int halfScreenWidth, int halfScreenHeight )
{
    // Create a window that is centered on the desktop. It's exactly 1/4 the
    // size of the desktop.

    DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;

    DWORD wndStyle = WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU |WS_MINIMIZEBOX | WS_MAXIMIZEBOX |	 WS_CLIPSIBLINGS |  WS_SIZEBOX  |WS_OVERLAPPED | WS_HSCROLL | WS_VSCROLL;// 

    HWND hWnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
        wndStyle, CW_USEDEFAULT, 0, 0, 0, 0, 0, wcl.hInstance, 0);

    if (hWnd)
    {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        if(halfScreenWidth  == 0) halfScreenWidth = screenWidth / 3;
        if(halfScreenHeight == 0) halfScreenHeight = screenHeight / 3;
        if(left == 0) left = (screenWidth - halfScreenWidth) / 2;
        if(top  == 0) top = (screenHeight - halfScreenHeight) / 2;
        RECT rc = {0};

        SetRect(&rc, left, top, left + halfScreenWidth, top + halfScreenHeight);
       // AdjustWindowRectEx(&rc, wndStyle, FALSE, wndExStyle);
        MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

    }

 return hWnd;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Process all events related to command line.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK  Cml::ProcessCmlInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){


 static HDC hdc;
 static HDC hdcBack;
 static HBITMAP hbmBack;
 static int nCount;
 static bool carrete;
 static int CaretPosition;
 static int nSaveCount;
 static WCHAR Wstring[MAX_STRING];
 static WCHAR SavedWstring[MAX_SAVED_STRING][MAX_STRING];
 static RECT desktop;

	switch (msg)
    {
		case WM_CREATE:
			{
			// Initialize the flags. 
			fBlt = FALSE; 
			fScroll = FALSE; 
			fSize = FALSE; 
 
			// Initialize the horizontal scrolling variables. 
			xMinScroll = 0; 
			xCurrentScroll = 0; 
			xMaxScroll = 0; 
 
			// Initialize the vertical scrolling variables. 
	        yMinScroll = 0; 
			yCurrentScroll = 0; 
			yMaxScroll = 0; 

			hdc = GetDC(hWnd);
			hdcBack = CreateCompatibleDC(hdc);


			// Get the horizontal and vertical screen sizes in pixel
            
			// Get a handle to the desktop window
			const HWND hDesktop = GetDesktopWindow();
			 // Get the size of screen to the variable desktop
			GetWindowRect(hDesktop, &desktop);

			hbmBack = CreateCompatibleBitmap(hdc, desktop.right, desktop.bottom);
			SelectObject(hdcBack, (HBITMAP)hbmBack);
			PatBlt(hdcBack, 0, 0, desktop.right, desktop.bottom, RGB(0, 0, 50) ); //PATINVERT

			bmp.bmWidth = GetDeviceCaps(hdcBack, HORZRES); 
			bmp.bmHeight = GetDeviceCaps(hdcBack, VERTRES); 

			for (int i = 0; i < MAX_SAVED_STRING; i++){
				ZeroMemory(SavedWstring[i], sizeof(SavedWstring[0]));
			}
			}
		break;

		case WM_SIZE: 
		{ 
	        int xNewSize; 
		    int yNewSize; 
 
	        xNewSize = LOWORD(lParam); 
		    yNewSize = HIWORD(lParam); 
 
			if (fBlt) 
				fSize = TRUE; 
 
			// The horizontal scrolling range is defined by 
			// (bitmap_width) - (client_width). The current horizontal 
			// scroll value remains within the horizontal scrolling range. 
 
			xMaxScroll = max(bmp.bmWidth-xNewSize, 0); 
			xCurrentScroll = min(xCurrentScroll, xMaxScroll); 
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = xMinScroll; 
			si.nMax   = bmp.bmWidth; 
			si.nPage  = xNewSize; 
			si.nPos   = xCurrentScroll; 
			SetScrollInfo(hWnd, SB_HORZ, &si, FALSE); 
 
			// The vertical scrolling range is defined by 
			// (bitmap_height) - (client_height). The current vertical 
			// scroll value remains within the vertical scrolling range. 
 
			yMaxScroll = max(bmp.bmHeight - yNewSize, 0); 
			yCurrentScroll = min(yCurrentScroll, yMaxScroll); 
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = yMinScroll; 
			si.nMax   = bmp.bmHeight; 
			si.nPage  = yNewSize; 
			si.nPos   = yCurrentScroll; 
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE); 
 
		} 
			break; 

		case WM_HSCROLL: 
		{ 
			int xDelta;     // xDelta = new_pos - current_pos  
			int xNewPos;    // new position 
			int yDelta = 0; 
 
			switch (LOWORD(wParam)) 
			{ 
				// User clicked the scroll bar shaft left of the scroll box. 
				case SB_PAGEUP: 
				    xNewPos = xCurrentScroll - 50; 
				    break; 
 
				// User clicked the scroll bar shaft right of the scroll box. 
				 case SB_PAGEDOWN: 
					 xNewPos = xCurrentScroll + 50; 
				     break; 
 
				// User clicked the left arrow. 
				case SB_LINEUP: 
				   xNewPos = xCurrentScroll - 5; 
				   break; 
 
				// User clicked the right arrow. 
				case SB_LINEDOWN: 
					xNewPos = xCurrentScroll + 5; 
					break; 
 
				// User dragged the scroll box. 
				case SB_THUMBPOSITION: 
					xNewPos = HIWORD(wParam); 
					break; 
 
				default: 
					xNewPos = xCurrentScroll; 
			} 
 
			// New position must be between 0 and the screen width. 
			xNewPos = max(0, xNewPos); 
			xNewPos = min(xMaxScroll, xNewPos); 
 
			// If the current position does not change, do not scroll.
			if (xNewPos == xCurrentScroll) 
				break; 
 
			// Set the scroll flag to TRUE. 
			fScroll = TRUE; 
 
			// Determine the amount scrolled (in pixels). 
			xDelta = xNewPos - xCurrentScroll; 
 
			// Reset the current scroll position. 
			xCurrentScroll = xNewPos; 
 
			// Scroll the window. (The system repaints most of the 
			// client area when ScrollWindowEx is called; however, it is 
			// necessary to call UpdateWindow in order to repaint the 
			// rectangle of pixels that were invalidated.) 
			ScrollWindowEx(hWnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
				(CONST RECT *) NULL, (HRGN) NULL, (LPRECT) NULL, 
				SW_INVALIDATE); 
			UpdateWindow(hWnd); 
 
			// Reset the scroll bar. 
	        si.cbSize = sizeof(si); 
			si.fMask  = SIF_POS; 
			si.nPos   = xCurrentScroll; 
			// Set the horizontal scrolling range and page size.
			SetScrollInfo(hWnd, SB_HORZ, &si, TRUE); 
          
			InvalidateRect(hWnd, NULL, TRUE);
	    } 
			break; 
 
		case WM_VSCROLL: 
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
				(CONST RECT *) NULL, (HRGN) NULL, (LPRECT) NULL, 
				SW_INVALIDATE); 
			UpdateWindow(hWnd); 
 
			// Reset the scroll bar. 
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_POS; 
			si.nPos   = yCurrentScroll; 
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE); 

			InvalidateRect(hWnd, NULL, TRUE);
		} 
			break; 

	case WM_CHAR:
	{
		//Copy the keybard entry to buffer
		WCHAR wChar;
		wChar= (WCHAR)(wParam); 
		if(nCount>COMMLENGHT) break;
		if(swprintf_s(Wstring+nCount,1000, L"%s", &wChar)>0)
		nCount++;
		wsprintf(Wstring+nCount,L"");
		InvalidateRect(hWnd, 0, 0);
	}


	case WM_KEYDOWN://WM_CHAR:
	{
        switch (wParam)
        {
        case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        case VK_RETURN:
			Interprete_line(hWnd, Wstring);
			nCount=0;//for carette
			nSaveCount++;
			if(nSaveCount<MAX_SAVED_STRING && wcslen(Wstring)>1)  CopyMemory(SavedWstring[nSaveCount], Wstring, sizeof(Wstring)); //add shifting strings in table to have the most recent 20 one
			else {
				nSaveCount--;
				for(int i= 0; i<MAX_SAVED_STRING-1; i++){
					CopyMemory(SavedWstring[i], SavedWstring[i+1], sizeof(SavedWstring[nSaveCount]));
				}
				CopyMemory(SavedWstring[nSaveCount], Wstring, sizeof(Wstring));

			}
			wsprintf(Wstring, L"");
			UpdateAllWindows(true);
            break;

        case VK_BACK:
			if(nCount>2){
				nCount--;
				wsprintf(Wstring+nCount,L"");
			}
			else {
				nCount = 0;
				wsprintf(Wstring,L"");
			}
			break;

        case VK_LEFT: //move caret
			if(CaretPosition>0){
				CaretPosition--;
				wsprintf(Wstring+nCount,L"");
			}
			break;

        case VK_RIGHT://move caret
			if(CaretPosition>0){
				CaretPosition++;
				wsprintf(Wstring+nCount,L"");
			}
			break;

        case VK_UP:
			if(nSaveCount>0){
				nSaveCount--;
				if(nSaveCount<1) nSaveCount = 1;
				if(nSaveCount<MAX_SAVED_STRING){
					nCount = wsprintf(Wstring,L"%s", SavedWstring[nSaveCount]);	
				}
			}
			break;

        case VK_DOWN:
			if(nSaveCount>0){
				nSaveCount++;
				if(nSaveCount<MAX_SAVED_STRING){
					nCount = wsprintf(Wstring,L"%s", SavedWstring[nSaveCount]);
				}
				else nSaveCount-= 1;
			}
			break;

        default:
            break;
        }
		InvalidateRect(hWnd, 0, 0);
        break;
	}
    break;

		case WM_PAINT:
			{
			PRECT prect; 
			PAINTSTRUCT ps;
			//ps.fErase;
			HBRUSH CmlBack = CreateSolidBrush( RGB(0, 0, 50));
 			hdc = BeginPaint(hWnd, &ps);
			FillRect(hdcBack, &desktop, CmlBack );
			CmlDraw(hWnd, hdcBack, carrete, Wstring);
            prect = &ps.rcPaint;
			BitBlt(hdc, prect->left, prect->top, (prect->right - prect->left), (prect->bottom - prect->top), hdcBack,prect->left + xCurrentScroll, prect->top + yCurrentScroll, SRCCOPY); 
            fScroll = FALSE; 

			RECT rect;
			GetClientRect(hWnd, &rect);
			CmlLine(hWnd, hdcBack, carrete, Wstring);
			BitBlt(hdc, rect.left, rect.bottom-20,  (prect->right - prect->left), rect.bottom, hdcBack, prect->left, rect.bottom-20, SRCCOPY); 

			EndPaint(hWnd, &ps);
			DeleteObject(CmlBack);
			}
		break;

	case WM_TIMER:
		if(carrete) carrete = false;
		else carrete = true;
		InvalidateRect(hWnd, 0, 0);
		break;

		case WM_DESTROY:
			DeleteDC(hdc);
			DeleteDC(hdcBack);
        break;
	}


    return DefWindowProc(hWnd, msg, wParam, lParam);


}


//--------------------------------------------------------------------------------------------------------------------------------
//
//  Purpose: Remove the command from beginning of the string 
//  Created by: Daniel Kaminski
//
//  14.02.2013 Lublin
//
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::RemoveCommand(WCHAR* InString) {


 WCHAR TmpString[COMMLENGHT];
 WCHAR OutString[COMMLENGHT];

 ZeroMemory(TmpString, sizeof(TmpString));
 ZeroMemory(TmpString, sizeof(OutString));

 unsigned int start_sentence=0;
	//Find the first occuring sentence in the string
	swscanf_s(InString, L"%15s", &TmpString, (unsigned int)sizeof(TmpString));
	unsigned int InString_size = (unsigned int) wcslen(InString);
	unsigned int sentence_size = (unsigned int) wcslen(TmpString);
	//Go throught the whole InString to find first character from the sentence
	for (unsigned int i = 0; i< InString_size+1; i++){
		if(InString[i] == TmpString[0] ){
			start_sentence = i + sentence_size; //the ond of sentence
			break;
		}
	}
	//Copy everythink after the sentence to OutString
	for (unsigned int i = 0; i< InString_size-sentence_size+1; i++){		//Remove the sentence
		if(i+start_sentence < InString_size)  OutString[i] = InString[i+start_sentence];
	}
	//Copy the OutString to InString
	swprintf_s(TmpString, COMMLENGHT, L"%s", OutString);
	swprintf_s(InString, COMMLENGHT, L"%s", TmpString);


return true;
}





//--------------------------------------------------------------------------------------------------------------------------------
// Interpretate line.
// still under construction
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::Interprete_line(HWND hwnd, WCHAR *String){

WCHAR Command[100];
WCHAR  Str[COMMLENGHT];
	

//for(int i=0; i<4; i++){ //loop 10 times to see how many commands is in the line
	//if the string size is 0
	size_t lenght = wcslen(String);
//Scan the first element in the string
	swscanf_s(String, L"%s", &Command, (unsigned int)sizeof(Command));
	//is it return?
	swprintf_s(Str, COMMLENGHT, L"re"); 


	//menu.level - this is wherw we want to be (from comman line)
	//menu.At - this is where we are (this will be changed acording to our wish from menu.level
	if(CompareStr(String, Str)==true || lenght==0){ //if string is re or empty move menu one level down
		if(menu.level >  LEVEL0) menu.level -= 1; //move one menu level down
		if(menu.level == LEVEL0) menu.At[LEVEL1] = KEYWORD_EMPTY;
		if(menu.level == LEVEL1) menu.At[LEVEL2] = KEYWORD_EMPTY;
		if(menu.level == LEVEL2) menu.At[LEVEL3] = KEYWORD_EMPTY;
		if(menu.level == LEVEL3) menu.At[LEVEL4] = KEYWORD_EMPTY;
		RemoveCommand(String); //remove re command
		swprintf_s(FeedbackStr, 500, L"");
	}
	else
	{
	//scan for the next element it should be keyword
	swscanf_s(String, L"%s", &Command, (unsigned int)sizeof(Command));



	if(CommandInterprete(hwnd, String, Command, L"ca", LEVEL1, KEYWORD_CALC))
		interpretate_calc(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"set", LEVEL1, KEYWORD_SET))
		interpretate_set(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"f", LEVEL1, KEYWORD_FIT))
		interpretate_fit(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"save", LEVEL1, KEYWORD_SAVE))
		interpretate_save(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"read", LEVEL1, KEYWORD_READ))
		interpretate_read(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"l", LEVEL1, KEYWORD_LIST))
		interpretate_list(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"?", LEVEL1, KEYWORD_HELP))
		interpretate_empty(hwnd, String);
	}

//}
 return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
//
//  Purpose: Prepare the string shown in the command line 
//  Created by: Daniel Kaminski
//
//  14.02.2013 Lublin
//
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::ComposeMenuTreeText(WCHAR *MenuText){

	int n=0;

	if (menu.level == LEVEL0) swprintf_s(MenuText, 10, L"");

	if(	menu.At[LEVEL1] == KEYWORD_SET){
		n=swprintf_s(MenuText, 10, L"Set>");
		return 1;
	}
	if(	menu.At[LEVEL1] == KEYWORD_HELP){
		n=swprintf_s(MenuText, 10, L"help>");
		return 1;
	}

	if(	menu.At[LEVEL1] == KEYWORD_FIT){
		n = swprintf_s(MenuText, 10, L"Fit>");
		//level 2
		if(	menu.At[LEVEL2] == KEYWORD_SET){
			n = swprintf_s(MenuText+n, 10, L"Set>");
		}
		if(	menu.At[LEVEL2] == KEYWORD_FIT_HELP){
			n = swprintf_s(MenuText+n, 10, L"help>");
		}
		return 1;
	}
	if(	menu.At[LEVEL1] == KEYWORD_PLOT){
		n = swprintf_s(MenuText, 10, L"Plot>");
		return 1;
	}
	if(	menu.At[LEVEL1] == KEYWORD_CALC){
		n = swprintf_s(MenuText, 10, L"Calc>");
		if(	menu.At[LEVEL2] == KEYWORD_CALC_RO){
			n = swprintf_s(MenuText+n, 20, L"Rod>");
		}
		if(	menu.At[LEVEL2] == KEYWORD_CALC_INP){
			n = swprintf_s(MenuText+n, 20, L"In-plane>");
		}
		return 1;
	}
	if(	menu.At[LEVEL1] == KEYWORD_READ){
		n = swprintf_s(MenuText, 10, L"Read>");
		return 1;
	}
	if(	menu.At[LEVEL1] == KEYWORD_SAVE){
		n = swprintf_s(MenuText, 10, L"Save>");
		return 1;
	}
	if(	menu.At[LEVEL1] == KEYWORD_LIST ){
		n = swprintf_s(MenuText, 10, L"List>");
		if(	menu.At[LEVEL2] == KEYWORD_LIST_BULK){
			n = swprintf_s(MenuText+n, 20, L"bul>");
		}
		if(	menu.At[LEVEL2] == KEYWORD_LIST_SURF){
			n = swprintf_s(MenuText+n, 20, L"surf>");	
		}
		if(	menu.At[LEVEL2] == KEYWORD_LIST_HELP){
			n = swprintf_s(MenuText+n, 20, L"help>");	
		}
		return 1;
	}
return false;

}


//--------------------------------------------------------------------------------------------------------------------------------
//
//  Purpose: Interprete the command bases on the ID of command and the menu level 
//  Created by: Daniel Kaminski
//
//  14.02.2013 Lublin
//
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::CommandInterprete(HWND hwnd, WCHAR* String, WCHAR* Command, WCHAR* ConstString, int LEVEL, int KEYWORD){ 
	
	if( menu.At[LEVEL] == KEYWORD)	
		return true;

	if(CompareStr(Command, ConstString) && menu.level==LEVEL-1 || (menu.At[LEVEL] == KEYWORD)){ 
			menu.level = LEVEL;
			menu.At[LEVEL] = KEYWORD;
			RemoveCommand(String); //remove the found command 
			return true;
		}

return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
// interprete empty command. Just do nothing.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_empty(HWND, WCHAR*){
return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// interprete command set.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_set(HWND hwnd, WCHAR *String){

return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// interprete command list.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_fit(HWND hwnd, WCHAR *String){
	WCHAR Command[COMMAND];
	int nPar;
	swscanf_s(String, L"%s %d", &Command, (unsigned int)sizeof(Command), &nPar);


	if(CommandInterprete(hwnd, String, Command, L"l", LEVEL2, KEYWORD_FIT_LIST)){
		interpretate_empty(hwnd, String);
		return true;
	}
	if(CommandInterprete(hwnd, String, Command, L"?", LEVEL2, KEYWORD_FIT_HELP)){
		interpretate_empty(hwnd, String);
		return true;
	}

//	if(CompareStr(String, L"lo")){
//		
//	}
 	if(CompareStr(String, L"ru")){
		fit(LSQ_MRQ, NULL);
		myScene.Update3DModel(false);
		SmallRodUpdate();
	}

 	if(CompareStr(String, L"as")){
		fit(LSQ_ASA, NULL);
		myScene.Update3DModel(false);
		SmallRodUpdate();
	}


return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// interprete command list.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_plot(HWND hwnd, WCHAR *String){

return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
// interprete command read.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_read(HWND hwnd, WCHAR *String){

return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// interprete command save.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_save(HWND hwnd, WCHAR *String){
	SaveFileFunction(hwnd);
return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// interprete command list.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_list(HWND hwnd, WCHAR *String){
	WCHAR Command[COMMAND];
	swscanf_s(String, L"%s", &Command, (unsigned int)sizeof(Command));


	
	if(CommandInterprete(hwnd, String, Command, L"bul", LEVEL2, KEYWORD_LIST_BULK))
		interpretate_empty(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"sur", LEVEL2, KEYWORD_LIST_SURF))
		interpretate_empty(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"?", LEVEL2, KEYWORD_LIST_HELP))
		interpretate_empty(hwnd, String);

return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
// interprete command calculate.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_calc(HWND hwnd, WCHAR *String){
 WCHAR Command[COMMAND];

	swscanf_s(String, L"%s", &Command, (unsigned int)sizeof(Command));

	if(CommandInterprete(hwnd, String, Command, L"ro", LEVEL2, KEYWORD_CALC_RO))
		interpretate_rod(hwnd, String);

	if(CommandInterprete(hwnd, String, Command, L"inp", LEVEL2, KEYWORD_CALC_INP))
		interpretate_in_plane(hwnd, String);

return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// interprete command rod.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_rod(HWND hwnd, WCHAR *String){

 float h = 0, k = 0;
 int energy = 0;
	if(wcslen(String)<2){
		swprintf_s(cml.FeedbackStr, 500, L"Enter h, k and energy: ");	 
		return false;
	}
 swscanf_s(String, L"%f%f%d",  &h, &k, &energy);



 if(thread.NSURF == 0 && thread.NBULK == 0){
	 MessageBox(hwnd, TEXT("Read model data bul, sur and/or fit."), TEXT("Error"), MB_ICONERROR);	 
	 return false;
 }
 CalculateRod((double)h, (double)k, energy);
 FindRodData((double)h, (double)k, 0);
return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// interprete command in_plane.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool Cml::interpretate_in_plane(HWND hwnd, WCHAR *String){
 float h_start= 0, h_end = 0, k_start =0, k_end = 0, l_plane = 0;

	if(wcslen(String)<2){
		swprintf_s(cml.FeedbackStr, 500, L"Enter start h, end h, start k, end k and l: ");	 
		return false;
	}
 swscanf_s(String, L"%f%f%f%f%f", &h_start, &h_end, &k_start, &k_end, &l_plane);

 if(thread.NSURF == 0 && thread.NBULK == 0){
	 MessageBox(hwnd, TEXT("Read model data bul, sur and/or fit."), TEXT("Error"), MB_ICONERROR);	 
	 return false;
 }

 if(h_start == 0.) h_start = -6.;
 if(k_start == 0.) k_start = -6.;
 if(h_end == 0.) h_end = 6.;
 if(k_end == 0.) k_end = 6.;
 CalculateInPlane((double)h_start, (double)h_end, (double)k_start, (double)k_end, (double)l_plane);

 FindInPlaneData( (double)l_plane);

return true;

}



//--------------------------------------------------------------------------------------------------------------------------------
// Draw the command window.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::CmlDraw(HWND hwnd, HDC hdcBack, bool carret, WCHAR *command_linetext  ){
 RECT rect;
	SetBkMode(hdcBack, TRANSPARENT );
	HBRUSH CmlBack = CreateSolidBrush( RGB(0, 0, 50));
	GetClientRect(hwnd, &rect);		
	FillRect(hdcBack, &rect, CmlBack);
	DeleteObject(CmlBack);

	if(menu.level == 0) WelcomeCMD(hwnd, hdcBack);
	if(menu.At[LEVEL1] == KEYWORD_SET) ListParametersSet(hdcBack);	
//	if(menu.At[LEVEL1] == KEYWORD_FIT) ListParametersFit(hdcBack);
	if(menu.At[LEVEL1] == KEYWORD_HELP) ListParametersAsk(hdcBack);

	if(menu.At[LEVEL2] == KEYWORD_LIST_BULK) ListParametersListBulk(hdcBack);
	if(menu.At[LEVEL2] == KEYWORD_LIST_SURF) ListParametersListSurf(hdcBack);
	if(menu.At[LEVEL2] == KEYWORD_LIST_HELP) ListParametersListAsk(hdcBack);

	if(menu.At[LEVEL2] == KEYWORD_FIT_LIST) ListParametersFit(hdcBack);
	if(menu.At[LEVEL2] == KEYWORD_FIT_HELP) ListParametersFitHelp(hdcBack);
}

//--------------------------------------------------------------------------------------------------------------------------------
// Draw the command window.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::CmlLine(HWND hwnd, HDC hdcBack, bool carret, WCHAR *command_linetext  ){

WCHAR plot_text[1000];
WCHAR MenuTree[500];
RECT rect;


	SetBkMode(hdcBack, TRANSPARENT );
	HBRUSH CmlBack = CreateSolidBrush( RGB(0, 0, 50));
	GetClientRect(hwnd, &rect);	
	FillRect(hdcBack, &rect, CmlBack);
	ComposeMenuTreeText(MenuTree);
	//Command line
	SetTextColor(hdcBack, RGB(200,200,10));
	rect.bottom-=5; rect.top = rect.bottom -20; rect.left+=10; rect.right = rect.left + 1000;

	if(carret==true)
		swprintf_s(plot_text, L"WinRod>%s%s%s\137", MenuTree, FeedbackStr, command_linetext );
	else 
		swprintf_s(plot_text, L"WinRod>%s%s%s", MenuTree, FeedbackStr, command_linetext);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	DeleteObject(CmlBack);
}



bool Cml::Change_parameters(bool state, WCHAR *wchar, HDC hdc ){

	if(state){
		swprintf_s(wchar, 50, L"free");
		SetTextColor(hdc, RGB(200, 200, 200));
	}
	else {
		swprintf_s(wchar, 50, L"fixed");
		SetTextColor(hdc, RGB(200, 0, 0));
	}
return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Draw the welcome screen. Index 0
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::ListParametersListAsk(HDC hdcBack  ){

WCHAR plot_text[COMMLENGHT];
RECT rect;
int step_line = 20;

		SetTextColor(hdcBack, RGB(200,200,10));
		rect.left = 10; rect.top = 10; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L"List menu:");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		
		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line+5 ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">List bulk atoms [bul]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">List surface atoms [sur]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}

//--------------------------------------------------------------------------------------------------------------------------------
// Draw the welcome screen. Index 0
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::ListParametersFitHelp(HDC hdcBack  ){

WCHAR plot_text[COMMLENGHT];
RECT rect;
int step_line = 20;

		SetTextColor(hdcBack, RGB(200,200,10));
		rect.left = 10; rect.top = 10; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L"Fit menu:");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		
		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line+5 ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Set parameter [v[nr]]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Set low limit [lo[nr]]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Set up limit [up[nr]]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">List [l]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Fit asa [asa]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Fit Levenberg-Marquard [ru]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

}



//--------------------------------------------------------------------------------------------------------------------------------
// Draw the welcome screen. Index 0
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::ListParametersAsk(HDC hdcBack  ){

WCHAR plot_text[COMMLENGHT];
RECT rect;
int step_line = 20;

		SetTextColor(hdcBack, RGB(200,200,10));
		rect.left = 10; rect.top = 10; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L"Main menu:");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		
		SetTextColor(hdcBack, RGB(200,200,200));
		rect.left = 10; rect.top +=  step_line+5 ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Read [r]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Save [s]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Fit [fit]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Set [set]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Plot [pl]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		rect.left = 10; rect.top +=  step_line ; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
		swprintf_s(plot_text, 400, L">Calculate [ca]");
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

}

//--------------------------------------------------------------------------------------------------------------------------------
// Draw the welcome screen. Index 0
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::ListParametersListBulk(HDC hdcBack  ){

int i;
WCHAR plot_text[COMMLENGHT];
WCHAR TmpStr[COMMLENGHT];
RECT rect;
int step_line = 30;

	rect.left = 10; rect.top = 10; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
	swprintf_s(plot_text, 400, L"List menu:");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect.top += step_line;
	SetTextColor(hdcBack, RGB(200,200,200));


	rect.top += step_line;
	swprintf_s(plot_text, 400, L"Bulk atoms");
	for (i = 0; i < thread.NBULK; i++){
		rect.top += step_line;
		AtoT( TmpStr, variable.ELEMENT[thread.TB[i]]);
		//							  Atom  x      y     z      DW
		swprintf_s(plot_text, 400, L"[%4d]	%4s  %6.3f  %6.3f  %6.2f  ",i, TmpStr, thread.XB[i], thread.YB[i], thread.ZB[i] );
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------
// Draw the welcome screen. Index 0
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::ListParametersListSurf(HDC hdcBack  ){

int i;
WCHAR plot_text[COMMLENGHT];
WCHAR TmpStr[COMMLENGHT];
RECT rect;
int step_line = 30;

	rect.left = 10; rect.top = 10; rect.right = rect.left + 700; rect.bottom=rect.top + 20;
	swprintf_s(plot_text, 400, L"List menu:");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect.top += step_line;
	SetTextColor(hdcBack, RGB(200,200,200));

	rect.top += step_line;
	swprintf_s(plot_text, 400, L"Surface atoms");
	for (i = 0; i < thread.NSURF; i++){
		rect.top += step_line;
		AtoT( TmpStr, variable.ELEMENT[thread.TS[i]]);
		//							  Atom  x      y     z      DW     DW-out  occ
		swprintf_s(plot_text, 400, L"[%4d]	%4s  %6.3f  %6.3f  %6.2f  %6.2f  %6.2f   %6.2f  %6.2f",i, TmpStr, thread.XSFIT[i], thread.YSFIT[i], thread.ZSFIT[i], thread.DEBWAL[thread.NDWS[i]-1], thread.DEBWAL2[thread.NDWS2[i]-1], thread.OCCUP[i], thread.OCCUP2[i]);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
}



//--------------------------------------------------------------------------------------------------------------------------------
// Draw the welcome screen. Index 0
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::WelcomeCMD(HWND hwnd,HDC hdcBack  ){

WCHAR plot_text[1000];
RECT rect;
int step_line;

    GetClientRect (hwnd, &rect);
	rect.right = rect.left + 500;
	step_line = 40;

	WCHAR ProcesorVersion[50];

#if defined _M_IX86
	swprintf_s(ProcesorVersion, 50, L"x32");
#elif defined _M_IA64
	swprintf_s(ProcesorVersion, 50, L"IA64");
#elif defined _M_X64
	swprintf_s(ProcesorVersion, 50, L"x64");
#endif

	SetTextColor(hdcBack, RGB(200,200,10));
	//rect.left = 10;
	rect.top = 10;
	//rect.right = rect.left + 400;
	rect.bottom=rect.top + 20;
	swprintf_s(plot_text, 100, L"WinRod v. %s (2014), %s edition", myVersion.GetVersionNumber(), ProcesorVersion);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	rect.top += step_line;
	swprintf_s(plot_text, L"by Daniel M. Kaminski");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	rect.top += step_line;
	swprintf_s(plot_text, L"WinRod bases on the excelent ROD program");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	rect.top += step_line;
	SetTextColor(hdcBack, RGB(200,100,10));
	swprintf_s(plot_text, L"written oryginally by Prof. Elias Vlieg ");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	SetTextColor(hdcBack, RGB(200,0,0));

	SetTextColor(hdcBack, RGB(150,150,150));	


}



//--------------------------------------------------------------------------------------------------------------------------------
// Draw the list from fit menu. Index 1
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::ListParametersFit(HDC hdcBack  ){

int i;
WCHAR plot_text[1000];
WCHAR true_false[10];
RECT rect;
int step_line;

	step_line = 40;
	SetTextColor(hdcBack, RGB(200,200,10));
	rect.left = 10; rect.top = 10; rect.right = rect.left + 1000; rect.bottom=rect.top + 20;
	swprintf_s(plot_text, 100, L"Fitting results:");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	SetTextColor(hdcBack, RGB(200,200,200));	
	rect.top += step_line;
	swprintf_s(plot_text,      L"                                Low lim.          parameter                Up lim.          status");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	//Scale
	rect.top += step_line;
	Change_parameters(variable.SCALEPEN, true_false, hdcBack );
	swprintf_s(plot_text, 400, L"Scale                      %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", variable.SCALELIM[0], thread.SCALE, thread.SCALE_ERR, variable.SCALELIM[1], true_false);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect.top += step_line;
	Change_parameters(variable.BETAPEN, true_false, hdcBack );
	swprintf_s(plot_text, 400, L"Beta                        %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", variable.BETALIM[0], thread.BETA, thread.BETA_ERR, variable.BETALIM[1], true_false);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	//Surf. fraction
	rect.top += step_line;
	Change_parameters(variable.SURFFRACPEN, true_false,  hdcBack );
	swprintf_s(plot_text, 400, L"Surface fraction      %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", variable.SURFFRACLIM[0], thread.SURFFRAC, thread.SURFFRAC_ERR, variable.SURFFRACLIM[1], true_false);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	if (thread.NSURF2 > 0)
	{
		//Surf. fraction
		rect.top += step_line;
		Change_parameters(variable.SURF2FRACPEN, true_false, hdcBack);
		swprintf_s(plot_text, 400, L"Surface2 fraction      %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", variable.SURF2FRACLIM[0], thread.SURF2FRAC, thread.SURFFRAC_ERR, variable.SURF2FRACLIM[1], true_false);
		DrawText(hdcBack, plot_text, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	}

    for (i = 0; i < variable.NDISTOT; i++){
		rect.top += step_line;
		Change_parameters(variable.DISPLPEN[i], true_false, hdcBack );
		swprintf_s(plot_text, 400, L"Displacement [%d]    %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", i+1, variable.DISPLLIM[i][0], variable.DISPL[i], variable.DISPL_ERR[i], variable.DISPLLIM[i][1], true_false);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}

    for (i = 0; i < thread.NDWTOT; i++){
		rect.top += step_line;
		Change_parameters(variable.DEBWALPEN[i], true_false, hdcBack );
		swprintf_s(plot_text, 800, L"Debye-Waller [%d]    %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", i+1, variable.DEBWALLIM[i][0], thread.DEBWAL[i], thread.DEBWAL_ERR[i], variable.DEBWALLIM[i][1], true_false);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}

    for (i = 0; i < thread.NDWTOT2; i++){
		rect.top += step_line;
		Change_parameters(variable.DEBWAL2PEN[i], true_false, hdcBack );
		swprintf_s(plot_text, 400, L"Debye-Waller2 [%d]  %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", i+1, variable.DEBWAL2LIM[i][0], thread.DEBWAL2[i], thread.DEBWAL2_ERR[i], variable.DEBWAL2LIM[i][1], true_false);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}

    for (i = 0; i < variable.NOCCTOT; i++){
		rect.top += step_line;
		Change_parameters(variable.OCCUPPEN[i], true_false, hdcBack );
		swprintf_s(plot_text, 400, L"occupation [%d]         %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", i+1, variable.OCCUPLIM[i][0], thread.OCCUP[i], thread.OCCUP_ERR[i], variable.OCCUPLIM[i][1], true_false);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}

	for (i = 0; i < variable.NOCCTOT2; i++) {
		rect.top += step_line;
		Change_parameters(variable.OCCUP2PEN[i], true_false, hdcBack);
		swprintf_s(plot_text, 400, L"occupation2 [%d]         %6.3f       <      %4.3f\261 %4.3f          <         %4.3f          %s", i + 1, variable.OCCUP2LIM[i][0], thread.OCCUP2[i], thread.OCCUP2_ERR[i], variable.OCCUP2LIM[i][1], true_false);
		DrawText(hdcBack, plot_text, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
}



//--------------------------------------------------------------------------------------------------------------------------------
// Draw the list from set menu.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void Cml::ListParametersSet(HDC hdcBack  ){

int i;
WCHAR plot_text[1000];
WCHAR Tstr[100];
RECT rect;
int step_line;
#define KOLUMN_SHIFT 250

	step_line = 40;
	SetTextColor(hdcBack, RGB(200,200,10));
	rect.left = 10; rect.top = 10; rect.right = rect.left + 1000; rect.bottom=rect.top + 20;
	swprintf_s(plot_text, 100, L"WinRod settings:");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	
	SetTextColor(hdcBack, RGB(100,100,100));
	rect.top += step_line;
	swprintf_s(plot_text, 400, L"Attenuation");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%6.3f", thread.ATTEN);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	rect.top += step_line;
	swprintf_s(plot_text, 400, L"Beta" );
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%6.3f", thread.BETA);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	rect.top += step_line;
	swprintf_s(plot_text, 400, L"lBragg" );
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%d", thread.LBR[0]);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	rect.top += step_line;
	swprintf_s(plot_text, 400, L"nlayers");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%d", thread.NLAYERS);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	rect.top += step_line;
	switch(thread.ROUGHMODEL){
		case APPROXBETA:
			swprintf_s(Tstr, L"APPROXBETA use approximation for beta roughness");
			break;
		case NUMBETA:
			swprintf_s(Tstr, L"NUMBETA use numberical beta model");
			break;
		case POISSONROUGH:
			swprintf_s(Tstr, L"POISSONROUGH use poisson roughness");
			break;
		case GAUSSROUGH:
			swprintf_s(Tstr, L"GAUSSROUGH use Gaussian roughness ");
			break;
		case LINEARROUGH:
			swprintf_s(Tstr, L"LINEARROUGH use linear decay roughness ");
			break;
		case COSINEROUGH:
			swprintf_s(Tstr, L"COSINEROUGH use cosine decay roughness ");
			break;
		case TWOLEVEL:
			swprintf_s(Tstr, L"TWOLEVEL use two-level roughness ");
			break;
	}
	swprintf_s(plot_text, 400, L"Roughness model");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%s", Tstr);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	rect.top += step_line;
	swprintf_s(plot_text, 400, L"ndomain");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%d", thread.NDOMAIN);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

    for (i = 0; i < thread.NDOMAIN; i++){
		rect.top += step_line;
		swprintf_s(plot_text, 400, L"domain %d", thread.NDOMAIN);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
		swprintf_s(plot_text, 400, L"%4.3f     %4.3f", thread.DOMMAT11[i], thread.DOMMAT12[i]);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;
		rect.top += step_line;
		rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
		swprintf_s(plot_text, 400, L"%4.3f     %4.3f", thread.DOMMAT21[i], thread.DOMMAT22[i]);
		DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	}

	rect.top += step_line;
	if(variable.FRACTIONAL) swprintf_s(Tstr, L"yes");
	else swprintf_s(Tstr, L"no");
	swprintf_s(plot_text, 400, L"Calculate fractional order reflections");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%s", Tstr);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	rect.top += step_line;
	if(variable.DOMEQUAL) swprintf_s(Tstr, L"yes");
	else swprintf_s(Tstr, L"no");
	swprintf_s(plot_text, 400, L"Domains are equal");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%s", Tstr);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;

	rect.top += step_line;
	if(thread.COHERENTDOMAINS) swprintf_s(Tstr, L"yes");
	else swprintf_s(Tstr, L"no");
	swprintf_s(plot_text, 400, L"Domains are coherent");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%s", Tstr);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;


	rect.top += step_line;
	swprintf_s(plot_text, 400, L"Scale");
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left += KOLUMN_SHIFT; rect.right = rect.left + 1000;
	swprintf_s(plot_text, 400, L"%4.3f", thread.SCALE);
	DrawText (hdcBack, plot_text,-1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	rect.left -= KOLUMN_SHIFT; rect.right = rect.left + 1000;
}