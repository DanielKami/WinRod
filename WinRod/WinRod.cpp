//---------------------------------------------------------------------------------------------------------------
// Copyright (c) 2013 Kaminski. All Rights Reserved.
// This program contains parts of ROD writen oryginally by Prof. Elias Vlieg,
// those parts are under ROD licence!
//
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//--------------------------------------------------------------------------------------------------------------

/**************************************************************************************************************/
/*      The main program entry. The file contain functions responsible for comunication with windows, 
        main program loop with the WinProc functions responsible for translating windows signals to the program tasks.
*/
/**************************************************************************************************************/
#pragma once
#include <stdafx.h>				//Header necessary for Visual Studio
#include <shellapi.h>
#include "WinRod.h"				//The main header of WinRod
#include "ReadFile.h"			//Read and save data form/to file and convert them to proper structures variables and thread variables
#include "3Ddef.h"				//Definition for 3D graphisc
#include "2DGraph.h"			//Add the 2D graphisc window nad all necessary libraries for graphs
#include "Sliders.h"			//Class containing all functions suporting sliders
#include "CallBack.h"			//Call back functions for dialog boxes 
#include "variables.h"			//Variables defined in structure not critical for thread calculations
#include "calc.h"				//The heart of caclulations derived from program ROD writen oryginally by Prof. Elias Vlieg
#include "set.h"				//All settings functions for proper translation and settings values used for variables and thread variables
#include "plot.h"				//All necessary rutines for 2D and 3D object preparations
#include "fit.h"				//Fitting procedures
#include "cml.h"				//Command line procedures
#include "Save.h"				//Save functions
#include "Toolbars.h"			//Toolbars definitions
#include "TooltipMain.h"		//Tool tips for main toolbar
#include "keating.h"			//Keating file created by Elias almost not changed
#include "ChiWindow.h"			//Chi windows
#include "UndoRedo.h"			//Header of class containing undo/redo functions and storage
#include "ResidualsWindow.h"	//Residual window
#include "Scene3D.h"			//Support for 3D scene and all 3D model operations
#include "elements.h"			//Important for elements initialization
#include "import.h"				//Important molecular files


DWORD threadID_3D;
HANDLE Thread3D;
HANDLE WaitHandle3D;



//-----------------------------------------------------------------------------
// Globals.
//-----------------------------------------------------------------------------
HWND hWndX[11];

WNDCLASSEX wcl[11];
	//Multiplot window

HWND						ListBox_h, ListBox_k, ListBox_l;
HWND						ComboRODs;
HWND						hwndTB_Main;				//Main aplication tolbar
HWND						hwndTB_Windows;
HWND						hwndSB;						//Main status bar

//Register class for 2D window
Graph2D						my2D[TOTAL_2D_WINDOWS];	//0 - Rod data amd model, 1 - In plane model and data, 2 - Z electron density profile, 3 - Fourier map, 4 - Multiplot

//Register class
Sliders						mySliders;					//Sliders registration
ChiWin						myChiWin;					//Chi window registration
ResidualsWindow				myResiduals;				//Registration residuals class
UndoRedo					myUndoRedo(&thread, &variable);//registration of UndoRedo class and its initiallization
Import						myImport;					//Molecular files suport
extern DataStruct			Dat;						//Read section

extern Scene3D				myScene;


double  Storage_h[MAX_STORAGE_FOR_COMBO_RODS];
double  Storage_k[MAX_STORAGE_FOR_COMBO_RODS];
int     Storage_E[MAX_STORAGE_FOR_COMBO_RODS];
bool FirstStart = true;

HDC hdc;
HDC hdcBack;
HWND hWndTB4;
bool FitUpdate = true;

//-----------------------------------------------------------------------------
// The program entry point WinMain.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

	Reset_Rod();
	ReadMacroInit();

	//Make local copy
	thread.g_hInstance = hInstance;

	MSG msg = { 0 };


	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Register class 0 (For main window)
	if (!RegisterMyClass(&wcl[0], hInstance, L"WindowClass0", Window0Proc, IDC_WINROD))//
		return 0;
	//Register class 10 (For residuals)
	if (!RegisterMyClass(&wcl[1], hInstance, L"WindowClass1", Window1Proc, NULL))
		return 0;
	//Register class 2 (2D rod)
	if (!RegisterMyClass(&wcl[2], hInstance, L"WindowClass2", Window2Proc))
		return 0;
	//Register class 3 (For multiplot)
	if (!RegisterMyClass(&wcl[3], hInstance, L"WindowClass3", Window3Proc, NULL))
		return 0;
	//Register class 4 (2D in-plane)
	if (!RegisterMyClass(&wcl[4], hInstance, L"WindowClass4", Window4Proc))
		return 0;
	//Register class 5 (CML)
	if (!RegisterMyClass(&wcl[5], hInstance, L"WindowClass5", Window5Proc, NULL))
		return 0;
	//Register class 6 (For electron density profile)
	if (!RegisterMyClass(&wcl[6], hInstance, L"WindowClass6", Window6Proc, NULL))
		return 0;
	//Register class 7 (For fourier map)
	if (!RegisterMyClass(&wcl[7], hInstance, L"WindowClass7", Window7Proc, NULL))
		return 0;
	//Register class 8 (For sliders)
	if (!RegisterMyClass(&wcl[8], hInstance, L"WindowClass8", Window8Proc, NULL))
		return 0;
	//Register class 9 (For chi )
	if (!RegisterMyClass(&wcl[9], hInstance, L"WindowClass9", Window9Proc, NULL))
		return 0;



	//If not available set the defoult size for all windows
	for (int nr = 0; nr < MAX_WINDOWS_NUMBER; ++nr)
	{
		if (variable.WindowsRect[nr].left == 0 && variable.WindowsRect[nr].top == 0 &&
			variable.WindowsRect[nr].right == 0 &&variable.WindowsRect[nr].bottom == 0)
		{
			DefaultWindows(2, nr);
		}
	}
	variable.WindowsVisible[0] = true;

	hWndX[0] = my2D[0].CreateAppWindow(wcl[0], WINDOW_TITLE, variable.WindowsRect[0]);
	if (!hWndX[0])
		MessageBox(NULL, TEXT("Can't create window 0"), NULL, MB_ICONERROR);
	hWndX[1] = myChiWin.CreateChiWindow(wcl[1], WINDOW_RESIDUALS, variable.WindowsRect[1]);
	if (!hWndX[1])
		MessageBox(NULL, TEXT("Can't create window 1"), NULL, MB_ICONERROR);
	hWndX[2] = my2D[2].CreateAppWindow(wcl[2], WINDOW_2D_ROD, variable.WindowsRect[2]);
	if (!hWndX[2])
		MessageBox(NULL, TEXT("Can't create window 2"), NULL, MB_ICONERROR);
	hWndX[3] = my2D[3].CreateAppWindow(wcl[3], WINDOW_MuLTiPLOT, variable.WindowsRect[3]);
	if (!hWndX[3])
		MessageBox(NULL, TEXT("Can't create window 3"), NULL, MB_ICONERROR);
	hWndX[4] = my2D[4].CreateAppWindow(wcl[4], WINDOW_2D_INPLANE, variable.WindowsRect[4]);
	if (!hWndX[4])
		MessageBox(NULL, TEXT("Can't create window 4"), NULL, MB_ICONERROR);
	hWndX[5] = cml.CreateAppWindow(wcl[5], WINDOW_CML, variable.WindowsRect[5].left, variable.WindowsRect[5].top, variable.WindowsRect[5].right - variable.WindowsRect[5].left, variable.WindowsRect[5].bottom - variable.WindowsRect[5].top);
	if (!hWndX[5])
		MessageBox(NULL, TEXT("Can't create window 5"), NULL, MB_ICONERROR);
	hWndX[6] = my2D[6].CreateAppWindow(wcl[6], WINDOW_ELL_DENS, variable.WindowsRect[6]);
	if (!hWndX[6])
		MessageBox(NULL, TEXT("Can't create window 6"), NULL, MB_ICONERROR);
	hWndX[7] = my2D[7].CreateAppWindow(wcl[7], WINDOW_FOURIER_MAP, variable.WindowsRect[7]);
	if (!hWndX[7])
		MessageBox(NULL, TEXT("Can't create window 7"), NULL, MB_ICONERROR);
	hWndX[8] = mySliders.CreateSliderWindow(wcl[8], WINDOW_FAST_CHECK, variable.WindowsRect[8]);
	if (!hWndX[8])
		MessageBox(NULL, TEXT("Can't create window 8"), NULL, MB_ICONERROR);
	hWndX[9] = myChiWin.CreateChiWindow(wcl[9], WINDOW_CHI2, variable.WindowsRect[9]);
	if (!hWndX[9])
		MessageBox(NULL, TEXT("Can't create window 9"), NULL, MB_ICONERROR);


	if (!SetTimer(hWndX[5], ID_TIMER, 250, 0))// set timer for window 5 (CML)
		return 0;

	//If windows are registered start  else exit
	//--------------------------------------------------------------------------------------------------
	if (hWndX[0] && hWndX[1] && hWndX[2] && hWndX[3] && hWndX[4] && hWndX[5] && hWndX[6] && hWndX[7] && hWndX[8] && hWndX[9])
	{

		variable.WindowsVisible[0] = true;
		for (int i = 0; i < MAX_WINDOWS_NUMBER - 1; ++i)//
		{
			if (variable.WindowsVisible[i]){
				ShowWindow(hWndX[i], nShowCmd);
				UpdateWindow(hWndX[i]);
			}
		}

		//Start the 3D graphics thread
		if (!Thread3D)
			StartThread3D(&thread);


		//Main program loop /message pumping
		while (true)
		{
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT)
			{
				break;
			}
			WaitMessage();
		}
	}

	//Unregister all classes
	for (int i = 0; i < 10; ++i)
		UnregisterClass(wcl[i].lpszClassName, hInstance);

	return static_cast<int>(msg.wParam);
}

//-----------------------------------------------------------------------------
// Functions: Register classes
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
bool RegisterMyClass(WNDCLASSEX *wcl, HINSTANCE hInstance, WCHAR *ClassTitle, WIN_PROC WindowProc, int IDC_MENU ){
	
	ZeroMemory(wcl, sizeof(WNDCLASSEX));

    wcl->cbSize = sizeof(*wcl);
    wcl->style =   CS_HREDRAW | CS_VREDRAW;// | CS_OWNDC |
    wcl->lpfnWndProc = *WindowProc;
    wcl->cbClsExtra = 0;
    wcl->cbWndExtra = 0;
    wcl->hInstance = hInstance;
    wcl->hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINROD));
	wcl->hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINROD));
    wcl->hCursor = LoadCursor(0, IDC_ARROW);
    wcl->hbrBackground = 0;
    wcl->lpszMenuName = MAKEINTRESOURCE(IDC_MENU);
    wcl->lpszClassName = ClassTitle;
    wcl->hIconSm = 0;

    if (!RegisterClassEx(wcl))
        return false;
 return true;
}


//-----------------------------------------------------------------------------
// Create Status Bar.
//PURPOSE: 
//  Registers the StatusBar control class and creates a statusbar.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------	
HWND WINAPI CreateStatusBar(HWND hwnd, HINSTANCE hInst){
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
// Functions: Main call back function for the main API window 
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window0Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool sizing = true;
	WCHAR Str[128];

	switch (msg)
	{

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case TTN_GETDISPINFO:
			TooltipMain(lParam);
			myUndoRedo.Tooltip(lParam);
			break;
		}
		break;

	case WM_ACTIVATE:
		switch (wParam)
		{
		case WA_ACTIVE:
			//needed for hot reading files at startup
			if (FirstStart) 
			{
				StatusBarsUpdate();
				AddRodsToComboMain();
				SmallRodUpdate();		
				UpdateAllWindows();
				FirstStart = false;
			}
			break;
		}
		break;

	case WM_CREATE:
	{
		WinRod_init(hWnd);
		//Initiallize  UndoRedo
		myUndoRedo.hWndTB4 = hWndTB4;
		myUndoRedo.Initialize();
		//Read any arguments if they exist after WinRod.exe	
		if (ReadCommandLineArguments()) //Somthing readed from real command line
		{
			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Begining");
			ReopenSlidersWindow();

			//Restart multiplot
			my2D[4].NumberOfRods = 0;
			Multiplot(variable.Multi_SelectPlot, variable.Multi_N_column, variable.Multi_L_points);	
			UpdateStatusBar();
			UpdateWindow(hwndSB);
		}
	}
		break;


	case WM_COMMAND:

		// Parse the menu selections:
		switch LOWORD(wParam)
		{

		case ID_NEW_MAIN:
			SendMessage(ComboRODs, CB_RESETCONTENT, 0, 0);
			Reset_Rod();
			myUndoRedo.Initialize();
			InitStatusBar();

			ZeroMemory(&variable.Multi_SelectPlot, sizeof(bool)*MAXMULTIPLOT);

			my2D[0].ResetOpen();
			my2D[1].ResetOpen();
			my2D[2].ResetOpen();
			my2D[3].ResetOpen();
			my2D[4].ResetOpen();
			
			PostThreadMessage(threadID_3D, WM_3D, CLEAR_MODEL, 0);
			InitialiseKeating();
			//An elegant way to update the 3D content with out memory conflict between threads
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		//case ID_TEST_TEST:
		//	test();
		//	break;

		case ID_FILE_OPEN_MAIN:
		case ID_FILE_OPEN: // open file window
		{
			int FilterIndex = OpenFileFunction(hWnd);
			calc.MemoryAllocateCalc(thread.NDAT);
			if (FilterIndex == 3) {
				AtoT(my2D[0].data.Title, Dat.Header);
				AtoT(my2D[1].data.Title, Dat.Header);
			}
			InitKeating(myScene.AtProp.AtomRadius);

			StatusBarsUpdate();
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Open");
			ReopenSlidersWindow();


			//Restart multiplot
			my2D[4].NumberOfRods = 0;
			Multiplot(variable.Multi_SelectPlot, variable.Multi_N_column, variable.Multi_L_points);

			//Add values to ComboROD
			AddRodsToComboMain();
			SmallRodUpdate();
			UpdateAllWindows();
			//Status bar
			HICON hIcon = (HICON)LoadImage(thread.g_hInstance, MAKEINTRESOURCE(100), IMAGE_ICON, 16, 16, LR_SHARED | LR_LOADTRANSPARENT);

			UpdateStatusBar();
			break;
		}

		case ID_FILE_SAVE_MAIN:
		case ID_FILE_SAVE:
		{
			SaveFileFunction(hWnd);// the 3D scene
		}
			break;


		case ID_FILE_IMPORT:
			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Import");
			myImport.OpenFileFunction(hWnd);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			SmallRodUpdate();

			//The sliders window has to be reopened to avoid problems with number of sliders
			ReopenSlidersWindow();
			SendMessage(hwndSB, SB_SETTEXT, 6, (LPARAM)L"Molecular mode");
			if (thread.NSURF > 0)
			{
				SendMessage(hwndSB, SB_SETTEXT, 1, (LPARAM)L"Sur file read");
				swprintf(Str, 126, L"Bulk %d, Surf %d", thread.NBULK, thread.NSURF);
				SendMessage(hwndSB, SB_SETTEXT, 5, (LPARAM)Str);
			}

			UpdateAllWindows();
			break;

		case ID_EDIT_REFRESH:
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			SmallRodUpdate();
			UpdateAllWindows();
			//Add values to ComboROD
			AddRodsToComboMain();
			break;
	
		case ID_OPTIONS_OPTONS:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hWnd, Option);
			break;

		case ID_OPTIONS_3DOPTIONS:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS3D), hWnd, Option3D);
			break;
						
		case ID_FILE_3DEXPORT:
		{
			Save3DFunction(hWnd);// the 3D scene
		}
			break;

		case ID_FILE_PRINT:
			Print3DFunction(hWnd);
			break;

		case ID_EDIT_COPY3DTOCLIPOBARD:
			CopySurface(hWnd);
			break;
			

		case ID_DISPERSIONENERGY_CALCULATECOEFFICIENTS:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_CALCULATE_COEFFICIENTS), hWnd, CalculateCoefficients);
			break;

		case ID_CALC_DISPERSIONCOEFICIENTS:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_PLOT_COEFFICIENTS), hWnd, PlotCoefficients);
			UpdateAllWindows();
			break;

		case ID_CALC_ELECTRONDENSITY:
			GetWindowsSize();
			//The fourier window has to be started before sending the data to it in case if it could be closed
			RestartWindow(7);

			//Start fourier dialog
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_CONTUR_MAP), hWnd, FourierConturMap);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			SmallRodUpdate();
			InvalidateRect(hWndX[7], 0, 0);
			break;

		case ID_CALC_CHICONTURMAP:			//The fourier window has to be started before sending the data to it in case if it could be closed
			RestartWindow(7);
			
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_CHI_CONTUR), hWnd, ChiConturMap);
			InvalidateRect(hWndX[7], 0, 0);
			break;

		case IDM_FIT:
		case ID_FIT:
		{
			//Compare atoms before fitting
			PostThreadMessage(threadID_3D, WM_3D, UPDATE_COMPARE, 0);
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_FIT), hWnd, Fitting);
			ReopenSlidersWindow();
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, NO_CHANGES);
			SmallRodUpdate();
			UpdateAllWindows();
		}
			break;

		case ID_DISPERSIONENERGY_EDITF1:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_EDIT_ENERGY), hWnd, EditEnergyF1);
			SmallRodUpdate();
			UpdateAllWindows();
				break;

		case ID_DISPERSIONENERGY_EDITF2:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_EDIT_ENERGY), hWnd, EditEnergyF2);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE_COMPARE, NO_CHANGES);
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case ID_CALC_ROD:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_CALCULATE), hWnd, Calculate);
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case ID_CALC_SORTBULKATOM:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_SORT_BULK), hWnd, Sort_Bulk);
			//Update3Datoms();
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case ID_SET_DOMAINS:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_DOMAINS), hWnd, Domains);
			ReopenSlidersWindow();
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case ID_SET_SETTINGS:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_SET), hWnd, Settings);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case ID_SET_SYMMETRY:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_SYMMETRY), hWnd, SetSymmetry);
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case ID_SET_ASASETTINGS:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_ASA_SETTINKGS), hWnd, SetAsa);
			break;

		case IDM_KEATING:
		case ID_SET_KEATING:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_KETTING), hWnd, Keating);
			break;

		case ID_EDIT_BULK_FILE:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_EDIT_BULK_FILE), hWnd, EditBulkFile);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case ID_EDIT_DATA_FILE:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_EDIT_DATA_FILE), hWnd, EditDataFile);
		
			StatusBarsUpdate();
			AddRodsToComboMain();
			UpdateStatusBar();
			UpdateWindow(hwndSB);


			//The sliders window has to be reopened to avoid problems with number of sliders
			ReopenSlidersWindow();
			SmallRodUpdate();
			UpdateAllWindows();
			break;

		case IDM_EDIT_FIT:
		case ID_EDIT_FIT_FILE:
			if (variable.MODE == 0)
				DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_EDIT_FIT_FILE), hWnd, EditFitFile);
			if (variable.MODE == 1)
				DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_MOLECULAR_FIT), hWnd, EditMolecularFitFile);
			//The sliders window has to be reopened to avoid problems with number of sliders
			swprintf(Str, 126, L"Bulk %d Surf. %d", thread.NBULK, variable.NSURFTOT);
			SendMessage(hwndSB, SB_SETTEXT, 4, (LPARAM)Str);
			calc.MemoryAllocateCalc(thread.NDAT);
			ReopenSlidersWindow();
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);

			SmallRodUpdate();
			UpdateAllWindows();
			//Add record to UndoRedo

			break;

		case ID_EDIT_ATOMPROPERITIES:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_ATOM_PROPERITIES), hWnd, AtomProperities);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			SmallRodUpdate();
			break;

		case IDM_Z_Ell_PROFILE:
			
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_ELECTRON_DENSITY_SETTINGS), hWnd, ElectronDensityProfileSettings);
			RestartWindow(6);
			break;

		case ID_3D:
		{
			Sleep(100);
			RestartWindow(10);
		}
			break;

		case ID_2D_ROD:
		{
			RestartWindow(2);
			SmallRodUpdate();
		}
			break;

		case ID_2D_IN_PLANEFILE:
		{
			RestartWindow(4);
			SmallRodUpdate();
		}
			break;

		case ID_MULTIPLE:
		{
			RestartWindow(3);
			SmallRodUpdate();
		}
			break;
			
		case ID_PLOT_MULTIPLOT:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_MULTIPLOT), hWnd, MultiplePlot);
			InvalidateRect(hWndX[3], 0, 0);
			break;

		case ID_CML:
		{
			RestartWindow(5);
			
			SetTimer(hWndX[5], ID_TIMER, 250, 0);// set timer for window 5 (CML)
		}
			break;

		case ID_FOURIER:
		{
			RestartWindow(7);
		}
			break;

		case ID_SLIDERS:
		{
			GetWindowsSize();
			//The sliders window has to be reopened to avoid problems with number of sliders
			ReopenSlidersWindow();
			InvalidateRect(hWndX[8], 0, 0);
		}
			break;

		case ID_CHI2:
		{
			RestartWindow(9);
		}
			break;

		case  ID_RESIDUALS_BUTTON:
			RestartWindow(1);
			break;

		case  ID_UNDO:										//suport for undo/redo

			myUndoRedo.Undo();

			//The sliders window has to be reopened to avoid problems with number of sliders
			ReopenSlidersWindow();
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE_COMPARE, 0);

			AddRodsToComboMain();
			SmallRodUpdate();
			UpdateAllWindows();
			UpdateStatusBar();
			SetFocus(hWndX[0]);
			break;

		case  ID_REDO:
			myUndoRedo.Redo();

			//The sliders window has to be reopened to avoid problems with number of sliders
			ReopenSlidersWindow();
			PostThreadMessage(threadID_3D, WM_3D, UPDATE, 0);
			PostThreadMessage(threadID_3D, WM_3D, UPDATE_COMPARE, 0);
			AddRodsToComboMain();
			SmallRodUpdate();
			UpdateAllWindows();
			UpdateStatusBar();
			SetFocus(hWndX[0]);
			break;
//Windows___________________________________________________________________________________________
		case  ID_WINDOWS_CASCADE:
			CascadeWindows();
			break;

		case ID_WINDOWS_CAFLE:
			CafleWindows();
			break;

		case ID_WINDOWS_MINIATURES:
			MiniaturesWindows();
			break;

		case ID_DEFAULT_STYLE1:
			DefaultWindows(1);
			break;

		case ID_DEFAULT_STYLE2:
			DefaultWindows(2);
			break;

		case ID_DEFAULT_STYLE3:
			DefaultWindows(3);
			break;
//Windows___________________________________________________________________________________________

		case IDM_ABOUT:
			DialogBox(thread.g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			PostThreadMessage(threadID_3D, WM_3D, UPDATE_WINDOWS_SIZE, 0);
			Sleep(200);//Just give some time for thread to finish task, no flags are necesary
			GetWindowsSize();
			SaveMacroFile();
			DestroyWindow(hWnd);
			break;
			/////

		case IDC_COMBOBOX_H:
		{
			if (HIWORD(wParam) == CBN_SELENDOK){
				variable.h = ComboBox_GetCurSel(ListBox_h) - 10;
				SetFocus(hWnd);
				SmallRodUpdate();
				UpdateAllWindows();
			}
		}
			break;
		case IDC_COMBOBOX_K:
		{
			if (HIWORD(wParam) == CBN_SELENDOK){
				variable.k = ComboBox_GetCurSel(ListBox_k) - 10;
				SetFocus(hWnd);
				SmallRodUpdate();
				UpdateAllWindows();
			}
		}
			break;

		case IDC_COMBOBOX_L:
		{
			if (HIWORD(wParam) == CBN_SELENDOK){
				variable.l = 0.1*(1 + ComboBox_GetCurSel(ListBox_l));
				SetFocus(hWnd);
				SmallRodUpdate();
				UpdateAllWindows();
			}
		}
			break;

		case IDC_COMBOBOX_RODS:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELENDOK:
			{
				int number = ComboBox_GetCurSel(ComboRODs);
				//SendMessage(ComboRODs, CB_SETCURSEL, 0, 0);

				variable.h = Storage_h[number];
				variable.k = Storage_k[number];
				variable.Energy = Storage_E[number];

				SmallRodUpdate();
				UpdateAllWindows();
				SetFocus(hWnd);
			}
			}
			break;
		}			
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;

	case WM_PAINT:
	{
		if (sizing)
		{

			sizing = false;

			RECT rect;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rect);
			FillRect(hdcBack, &rect, (HBRUSH)(COLOR_WINDOW + 1));
			BitBlt(hdc, 0, 0, rect.right, rect.bottom - 23, hdcBack, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
		}
	}
		break;

	case WM_DESTROY:
	{
		Shutdown();
		PostQuitMessage(0);
	}
		break;
	

	case WM_SIZE:
		sizing = true;
		RECT rect;
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, 0, 0);
		SendMessage(hwndSB, WM_SIZE, 0, 0);
		SendMessage(hwndTB_Main, WM_SIZE, 0, 0);
		GetWindowRect(hWndX[0], &variable.WindowsRect[0]);
		break;


	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ReopenSlidersWindow()
{
	//The slider window has to be reopened to avoid problems with number of sliders
	RestartWindow(8);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the residuals  window
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window1Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return myResiduals.WinProc(hWnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the second API window 
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window2Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return my2D[0].WinProc(hWnd, thread.g_hInstance, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the 4th API window 
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window4Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return my2D[1].WinProc(hWnd, thread.g_hInstance, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the 5th API window command line
//07.02.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window5Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
return cml.ProcessCmlInput(hWnd, msg,wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the 6th API window El. dens. profile
//07.02.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window6Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return my2D[2].WinProc(hWnd, thread.g_hInstance, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the 7th API window Fourier map
//07.02.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window7Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return my2D[3].WinProc(hWnd, thread.g_hInstance, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the 8th API window, sliders  (fast parameter check)
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window8Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	
	mySliders.WinProc(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_HSCROLL://sliders	
		PostThreadMessage(threadID_3D, WM_3D, UPDATE_COMPARE, NO_CHANGES);
		SmallRodUpdate();
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Main call back function for the chi^2  window
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window9Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return myChiWin.WinProc(hWnd, msg, thread.g_hInstance, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Functions: Main call back function for the multiplot  window
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window3Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return my2D[4].WinProc(hWnd, thread.g_hInstance, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Functions: Add RODs from data file to Combo RODs in toolbar
//12.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
void AddRodsToComboMain(){

	int i;
	double temp_h = 1000, temp_k = 1000;
	int temp_energy = 0;
	WCHAR WStr[100];
	int nr = 0;
	int points_in_rod = 0;
	int max_points, i_max;
	int number = ComboBox_GetCurSel(ComboRODs);

	SendMessage(ComboRODs, CB_RESETCONTENT, 0, 0);

	if (variable.ENERGYTOT == 0)
	{
		for (i = 0; i < thread.NDAT; ++i)
		{
			points_in_rod++;
			if (thread.HDAT[i] != temp_h || thread.KDAT[i] != temp_k)
			{
				temp_h = thread.HDAT[i];
				temp_k = thread.KDAT[i];
				points_in_rod = 0;
				for (int j = 0; j < thread.NDAT; ++j)
				{
					if (thread.HDAT[j] == temp_h && thread.KDAT[j] == temp_k)
						points_in_rod++;
				}
				if (points_in_rod > max_points)
				{
					max_points = points_in_rod;
					i_max = i;
				}
				swprintf_s(WStr, 100, L" (%4.5f  %4.5f) points %d", temp_h, temp_k, points_in_rod);
				SendMessage(ComboRODs, CB_ADDSTRING, 0, (LPARAM)WStr);
				Storage_h[nr] = temp_h;
				Storage_k[nr] = temp_k;

				nr++;
				if (nr >= MAX_STORAGE_FOR_COMBO_RODS){
					MessageBox(NULL, TEXT("Maximum number of ROD entries in ComboBox exceded."), NULL, MB_ICONERROR);
					break;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < thread.NDAT; ++i)
		{
			points_in_rod++;
			if (thread.HDAT[i] != temp_h || thread.KDAT[i] != temp_k || thread.ENERGY[i] != temp_energy)
			{
				temp_h = thread.HDAT[i];
				temp_k = thread.KDAT[i];
				temp_energy = thread.ENERGY[i];
				points_in_rod = 0;
				for (int j = 0; j < thread.NDAT; ++j)
				{
					if (thread.HDAT[j] == temp_h && thread.KDAT[j] == temp_k && thread.ENERGY[j] == temp_energy)
						points_in_rod++;
				}
				if (points_in_rod > max_points)
				{
					max_points = points_in_rod;
					i_max = i;
				}
				swprintf_s(WStr, 100, L" (%4.5f  %4.5f) En. %d points %d", temp_h, temp_k, temp_energy, points_in_rod);
				SendMessage(ComboRODs, CB_ADDSTRING, 0, (LPARAM)WStr);
				Storage_h[nr] = temp_h;
				Storage_k[nr] = temp_k;
				Storage_E[nr] = temp_energy;
				nr++;
				if (nr >= MAX_STORAGE_FOR_COMBO_RODS){
					MessageBox(NULL, TEXT("Maximum number of ROD entries in ComboBox exceded."), NULL, MB_ICONERROR);
					break;
				}
			}
		}
	}

	if (number = 0 && i_max > 0) number = 1;
	SendMessage(ComboRODs, CB_SETCURSEL, number,i_max);
	variable.h = Storage_h[number];
	variable.k = Storage_k[number];
	variable.Energy = Storage_E[number];
	
}

void DrawStatusBar(HWND hwnd, HDC hdc, POINT Pt, RECT rect)
{

	WCHAR lpBuffer[200];
	RECT rect2;

	SetBkMode(hdc, TRANSPARENT);
	GetClientRect(hwnd, &rect2);

	rect.left += 5;
	swprintf_s(lpBuffer, 200, L"gla");
	DrawText(hdc, lpBuffer, -1, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}

inline void GetWindowsSize(bool correction){

	RECT rect = { 0 };


	for (int i = 0; i < MAX_WINDOWS_NUMBER-1; ++i)
	{
		GetWindowRect(hWndX[i], &rect);
		if (rect.bottom - rect.top > 0 && rect.right - rect.left > 0)
			CopyRect(i, rect);
	}

	for (int i = 0; i < MAX_WINDOWS_NUMBER-1; i++)
	{
		if (IsWindow(hWndX[i]) == 0) 
			variable.WindowsVisible[i] = false;
		else
			variable.WindowsVisible[i] = true;
	}


}

void CopyRect(int i, RECT rect)
{
	if (rect.bottom - rect.top > 0 && rect.right - rect.left > 0)
	{
		variable.WindowsRect[i].right = rect.right;
		variable.WindowsRect[i].left = rect.left;
		variable.WindowsRect[i].top = rect.top;
		variable.WindowsRect[i].bottom = rect.bottom;
	}
}



void UpdateAllWindows(bool upate_sliders_flag)
{
	//It updates all windows except XD window for 3D 

	if (upate_sliders_flag)
		mySliders.UpdateSliders(hWndX[8]);

	myResiduals.UpdateResiduals(hWndX[1]);
	my2D[0].AutomaticSetTheScale();
	my2D[1].AutomaticSetTheScale();
	myChiWin.CalculateChi_sqr();

	for (int i = 1; i < MAX_WINDOWS_NUMBER-1; ++i)
		if (i!= 8)
		InvalidateRect(hWndX[i], 0, 0);

	PostThreadMessage(threadID_3D, WM_3D, UPDATE, NO_CHANGES);
}

void WinRod_init(HWND hWnd)
{
	RECT rect;
	
	//Create back buffer for drawing
	hdc = GetDC(hWnd);
	hdcBack = CreateCompatibleDC(hdc);
	GetWindowRect(hWnd, &rect);
	// Get the horizontal and vertical screen sizes in pixel
	RECT desktop;
	// Get a handle to the desktop window
	HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);

	HBITMAP hbmBack = CreateCompatibleBitmap(hdc, desktop.right, desktop.bottom);
	SelectObject(hdcBack, (HBITMAP)hbmBack);
	PatBlt(hdcBack, 0, 0, desktop.right, desktop.bottom, WHITENESS); //PATINVERT
	DeleteObject(hbmBack);


	// Create main toolbar
	hwndTB_Main = CreateToolbar(hWnd, &hWndTB4, thread.g_hInstance, &ListBox_h, &ListBox_k, &ListBox_l, &ComboRODs, desktop.right);

	//Create main status bar
	hwndSB = CreateStatusBar(hWnd, thread.g_hInstance);

	//Elegant pas function pointer to slider class
	mySliders.updata_windows = UpdateAllWindows;

	//There is nothing to undo, redo lets gray it
	SendMessage(hWndTB4, TB_ENABLEBUTTON, ID_UNDO, FALSE);
	SendMessage(hWndTB4, TB_ENABLEBUTTON, ID_REDO, FALSE);

	ComboBox_SetCurSel(ListBox_h, 10);
	ComboBox_SetCurSel(ListBox_k, 10);
	ComboBox_SetCurSel(ListBox_l, 0);

	my2D[1].lin_log = true;

	ReadAtomInit(hWnd, &myScene.AtProp);

	InitialiseKeating();

	int iStatusBarWidths[] = { 120, 240, 400, 500, 660, 800, -1 };
	SendMessage(hwndSB, SB_SETPARTS, 7, (LPARAM)iStatusBarWidths);
	InitStatusBar();

}

bool ReadCommandLineArguments()
{
	//Read command lines arguments
	int NumberOfArguments;
	LPTSTR CommandLineString = GetCommandLine();

	//Parse the string to arguments
	LPWSTR*  CommandLineArg = CommandLineToArgvW(CommandLineString, &NumberOfArguments);
	variable.PrintTestFitFile = false;

	if (NumberOfArguments > 0 && CommandLineArg[1] != NULL)
	{
	//Extract extension a simple way. It  consider several dots in the string
	WCHAR *Ext = FindLastDot(CommandLineArg[1]);

	ReadFileFromPatch(CommandLineArg[1], Ext);
	}
	LocalFree(CommandLineArg);

	UpdateStatusBar();
	calc.MemoryAllocateCalc(thread.NDAT);
	return false;
}

void RestartWindow(int i)
{
	if (i < MAX_WINDOWS_NUMBER-1 && IsWindow(hWndX[i]))
	{
		ShowWindow(hWndX[i], SW_RESTORE);
		GetWindowsSize();
		DestroyWindow(hWndX[i]);
	} 


	switch (i)
	{
	case 0:
		
		hWndX[0] = my2D[0].CreateAppWindow(wcl[0], WINDOW_TITLE, variable.WindowsRect[0]);
		if (!hWndX[0])
			MessageBox(NULL, TEXT("Can't open window 0"), NULL, MB_ICONERROR);
		break;
	case 1:
		//Create hWndX[1] window from class 1 residuals
		hWndX[1] = myResiduals.CreateWin(wcl[1], WINDOW_RESIDUALS, variable.WindowsRect[1]);
		if (!hWndX[1])
			MessageBox(NULL, TEXT("Can't open window 1"), NULL, MB_ICONERROR);
		break;
	case 2:
		//Create hWndX[2] window from class 2  2D (rod)
		hWndX[2] = my2D[2].CreateAppWindow(wcl[2], WINDOW_2D_ROD, variable.WindowsRect[2]);
		if (!hWndX[2])
			MessageBox(NULL, TEXT("Can't open window 2"), NULL, MB_ICONERROR);
		break;
	case 3:
		//Create hWndX[3] window from class 3 (2D Fourier map)
		hWndX[3] = my2D[3].CreateAppWindow(wcl[3], WINDOW_MuLTiPLOT, variable.WindowsRect[3]);
		if (!hWndX[3])
			MessageBox(NULL, TEXT("Can't open window 3"), NULL, MB_ICONERROR);
		break;
	case 4:
		//Create hWndX[4] window from class 4 (2D in-plane)
		hWndX[4] = my2D[4].CreateAppWindow(wcl[4], WINDOW_2D_INPLANE, variable.WindowsRect[4]);
		if (!hWndX[4])
			MessageBox(NULL, TEXT("Can't open window 4"), NULL, MB_ICONERROR);
		break;
	case 5:
		//Create hWndX[5] window from class 5 (CML)
		hWndX[5] = cml.CreateAppWindow(wcl[5], WINDOW_CML, variable.WindowsRect[5].left, variable.WindowsRect[5].top, variable.WindowsRect[5].right - variable.WindowsRect[5].left, variable.WindowsRect[5].bottom - variable.WindowsRect[5].top);
		if (!hWndX[5])
			MessageBox(NULL, TEXT("Can't open window 5"), NULL, MB_ICONERROR);
		break;
	case 6:
		//Create hWndX[6] window from class 6 (2D electron density profile)
		hWndX[6] = my2D[6].CreateAppWindow(wcl[6], WINDOW_ELL_DENS, variable.WindowsRect[6]);
		if (!hWndX[6])
			MessageBox(NULL, TEXT("Can't open window 6"), NULL, MB_ICONERROR);
		break;
	case 7:
		//Create hWndX[7] window from class 7 (2D Fourier map)
		hWndX[7] = my2D[7].CreateAppWindow(wcl[7], WINDOW_FOURIER_MAP, variable.WindowsRect[7]);
		if (!hWndX[7])
			MessageBox(NULL, TEXT("Can't open window 7"), NULL, MB_ICONERROR);
		break;
	case 8:
		//Create hWndX[8] window from class 8 (sliders window)
		hWndX[8] = mySliders.CreateSliderWindow(wcl[8], WINDOW_FAST_CHECK, variable.WindowsRect[8]);
		if (!hWndX[8])
			MessageBox(NULL, TEXT("Can't open window 8"), NULL, MB_ICONERROR);
		break;
	case 9:
		//Create hWndX[9] window from class 9 (chi2 window)
		hWndX[9] = myChiWin.CreateChiWindow(wcl[9], WINDOW_CHI2, variable.WindowsRect[9]);
		if (!hWndX[9])
			MessageBox(NULL, TEXT("Can't open window 9"), NULL, MB_ICONERROR);
		break;

	case 10://DX	
		RestartDX_Window();
		break;

	}

	if (i < MAX_WINDOWS_NUMBER-1)
	{
		ShowWindow(hWndX[i], SW_SHOW);
		AnimateWindow(hWndX[i], 200, AW_BLEND);
		UpdateWindow(hWndX[i]);
		InvalidateRect(hWndX[i], 0, 0);
	}
}

void InitStatusBar()
{
	SendMessage(hwndSB, SB_SETTEXT, 0, (LPARAM)L"Bul not read");
	SendMessage(hwndSB, SB_SETTEXT, 1, (LPARAM)L"Sur/Fit not read");
	SendMessage(hwndSB, SB_SETTEXT, 2, (LPARAM)L"Dat not read");
	SendMessage(hwndSB, SB_SETTEXT, 3, (LPARAM)L"Par not read");
	SendMessage(hwndSB, SB_SETTEXT, 4, (LPARAM)L"Model not read");
	SendMessage(hwndSB, SB_SETTEXT, 5, (LPARAM)L"Atomic mode");
}

void CascadeWindows()
{
	RECT rc;
	for (int i = 0; i < MAX_WINDOWS_NUMBER-1; ++i)
	{
		rc.top = 50 + i * 30;
		rc.left = 50 + i * 50;
		rc.right = 500;
		rc.bottom = 400;
		MoveWindow(hWndX[i], rc.left, rc.top, rc.right, rc.bottom, TRUE);
		InvalidateRect(hWndX[i], 0, 0);
	}
	//And move the XD window
	rc.top = 50 + 10* 30;
	rc.left = 50 + 10 * 50;
	variable.WindowsRect[10].left = rc.left; variable.WindowsRect[10].right = rc.right + rc.left; variable.WindowsRect[10].top = rc.top; variable.WindowsRect[10].bottom = rc.top + rc.bottom;
	RestartDX_Window(true);
	
}

void CafleWindows()
{
	RECT rc;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	for (int i = 0; i < 6; ++i)
	{
		rc.top =  0;
		rc.left = i * screenWidth/5;
		MoveWindow(hWndX[i], rc.left, rc.top, screenWidth / 5, screenHeight/2, TRUE);
		InvalidateRect(hWndX[i], 0, 0);
	}
	for (int i = 0; i < 5; ++i)
	{
		rc.top =  screenHeight / 2;
		rc.left = i * screenWidth / 6;
		MoveWindow(hWndX[i+5], rc.left, rc.top, screenWidth / 6, ( screenHeight-72) / 2, TRUE);
		InvalidateRect(hWndX[i+5], 0, 0);
	}
	//And move the XD window
	rc.left = 5 * screenWidth / 6; rc.right = rc.left + screenWidth / 6; rc.bottom = rc.top + (screenHeight - 72) / 2;
	variable.WindowsRect[10].left = rc.left; variable.WindowsRect[10].right = rc.right; variable.WindowsRect[10].top = rc.top; variable.WindowsRect[10].bottom = rc.bottom;
	RestartDX_Window(true);
}

void MiniaturesWindows()
{
	RECT rc;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN)-50;

	MoveWindow(hWndX[0], 1, 1, screenWidth, 120, TRUE);
	InvalidateRect(hWndX[0], 0, 0);
	for (int i = 0; i < 6; ++i)
	{
		rc.left = i * 220;
		rc.top = screenHeight-100;
		
		MoveWindow(hWndX[i+1], rc.left, rc.top,220, 45, TRUE);
	}
	for (int i = 0; i < 5; ++i)
	{
		rc.left = i * 220;
		rc.top = screenHeight - 50;
		
		MoveWindow(hWndX[i + 6], rc.left, rc.top, 220, 45, TRUE);
	}

	//And move the XD window
	rc.left = 4 * 220; rc.right = rc.left + 220; rc.bottom = rc.top + 45;
	variable.WindowsRect[10].left = rc.left; variable.WindowsRect[10].right = rc.right; variable.WindowsRect[10].top = rc.top; variable.WindowsRect[10].bottom = rc.bottom;
	RestartDX_Window(true);
}

void DefaultWindows(int Style, int windows_nr)
{
	//Frames of all windows if not initillize from file.
	int style1[][4] =
	{
		{ 1, 1, 806, 153 },
		{ 1, 412, 233, 863 },
		{ 231, 151, 805, 414 },
		{ 804, 409, 1431, 864 },
		{ 230, 412, 806, 659 },
		{ 203, 886, 351, 927 },
		{ 231, 657, 807, 863 },
		{ 1431, 771, 166, 863 },
		{ 1431, 1, 1606, 772 },
		{ 1, 150, 232, 406 },
		{ 805, 1, 1432, 413 }  
	};

	int style2[][4] =
	{
		{ 1, 1, 798, 147 },
		{ 335, 601, 566, 861 },
		{ 1003, 446, 1431, 861 },
		{ 1, 146, 568, 601 },
		{ 566, 446, 1004, 861 },
		{ 203, 886, 351, 927 },
		{ 1, 600, 335, 860 },
		{ 1430, 772, 1601, 861 },
		{ 1431, 1, 1606, 772 },
		{ 567, 147, 798, 446 },
		{ 798, 1, 1433, 447 },
	};

	int style3[][4] =
	{
		{ 0, 1, 531, 175 },
		{ 1229, 521, 1600, 774 },
		{ 693, 521, 1229, 861 },
		{ 531, 1, 1132, 521 },
		{ 0, 521, 348, 861 },
		{ 1228, 773, 1419, 861 },
		{ 347, 521, 693, 860 },
		{ 1418, 772, 1600, 859 },
		{ 298, 175, 531, 521 },
		{ 0, 175, 298, 522 },
		{ 1132, 1, 1600, 521 },	
	};

	double CorrectionX = 1.0*GetSystemMetrics(SM_CXSCREEN) / 1600;
	double CorrectionY = 1.0* GetSystemMetrics(SM_CYSCREEN) / 900;

	//Correct only ine window
	if (windows_nr != 0)
		SizeWindow(windows_nr, style1, CorrectionX, CorrectionY);
	else

		for (int i = 0; i < MAX_WINDOWS_NUMBER; ++i)
		{
		if (Style == 1)
			SizeWindow(i, style1, CorrectionX, CorrectionY);
		if (Style == 2)
			SizeWindow(i, style2, CorrectionX, CorrectionY);
		if (Style == 3)
			SizeWindow(i, style3, CorrectionX, CorrectionY);
		}
}


void SizeWindow(int nr, int St[][4], double CoorX, double CorrY)
{
	int *Style = St[nr];

	variable.WindowsRect[nr].left = (int)(Style[0] * CoorX);
	variable.WindowsRect[nr].top = (int)(Style[1] * CorrY);
	variable.WindowsRect[nr].right = (int)((Style[2] ) * CoorX);
	variable.WindowsRect[nr].bottom = (int)((Style[3]) * CorrY);

	if (nr < MAX_WINDOWS_NUMBER - 1)
	{
		MoveWindow(hWndX[nr], variable.WindowsRect[nr].left, variable.WindowsRect[nr].top, variable.WindowsRect[nr].right - variable.WindowsRect[nr].left, variable.WindowsRect[nr].bottom - variable.WindowsRect[nr].top, TRUE);
		InvalidateRect(hWndX[nr], 0, 0);
	}
	if (nr == 10)
		RestartDX_Window(true);
}


void StatusBarsUpdate(void)
{
	swprintf_s(my2D[0].data.Title, HEADER, Dat.THeader);
	swprintf_s(my2D[1].data.Title, HEADER, Dat.THeader);
	my2D[0].UpdateStatusBar();
	my2D[1].UpdateStatusBar();
}



void test(void)
{
	WCHAR str[200];
	int delta;
	int TimeStart, TimeEnd;
	
#define FFF 10000

	double *test = new double[FFF];
	double temp = 0;

	//See how long it takes
	TimeStart = (int)GetTickCount();

	for (int j = 0; j < 10000; ++j)
	{
		for (int i = 1; i < FFF; ++i)
			test[i] = i*i+1;

		for (int i = 1; i < FFF; ++i)
			temp += test[i]*j+2;
	}

	TimeEnd = (int)GetTickCount();
	delta = TimeEnd - TimeStart;
	swprintf_s(str, L" %3.3g delta = %dms",temp, delta);
	MessageBox(NULL, str, NULL, MB_ICONERROR);
	delete[] test;



	TimeStart = (int)GetTickCount();

	double test1[FFF];
	temp = 0;
	for (int j = 0; j < 10000; ++j)
	{
		for (int i = 1; i < FFF; ++i)
			test1[i] = i*i+1;
		for (int i = 1; i < FFF; ++i)
			temp += test1[i]*j+2;
	}

	TimeEnd = (int)GetTickCount();
	delta = TimeEnd - TimeStart;
	swprintf_s(str, L" %3.3g delta = %dms", temp, delta);
	MessageBox(NULL, str, NULL, MB_ICONERROR);

}


//-----------------------------------------------------------------------------
// Shutdown.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Shutdown()
{
	ClosePrintFitFile();
	SaveAtomProperitiesFile(myScene.AtProp);
	StopThreads(); //Fitting threads

	//Close 3D objects
	ResetEvent(WaitHandle3D);
	PostThreadMessage(threadID_3D, WM_QUIT, 0, 0);
	//Wait until 3D is finished
	WaitForSingleObject(WaitHandle3D, 2000);
	Sleep(1000);
	//Now the thread can be safetly closed

	DeleteDC(hdc);
	DeleteDC(hdcBack);
}

void UpdateStatusBar()
{
	WCHAR Str[HEADER];

	if (thread.NBULK > 0)
	{
		SendMessage(hwndSB, SB_SETTEXT, 0, (LPARAM)L"Bul file read");
	}
	if (variable.NSURFTOT > 0)
	{
		if (variable.MODE == 0)
		{
			SendMessage(hwndSB, SB_SETTEXT, 1, (LPARAM)L"Sur/Fit file read");
			SendMessage(hwndSB, SB_SETTEXT, 5, (LPARAM)L"Atomic mode");
		}

		if (variable.MODE == 1)
		{
			SendMessage(hwndSB, SB_SETTEXT, 1, (LPARAM)L"Mft file read");
			SendMessage(hwndSB, SB_SETTEXT, 5, (LPARAM)L"Molecular mode");
		}
	}
	if (thread.NDAT > 0)
	{
		swprintf(Str, 126, L"Dat %d points", thread.NDAT);
		SendMessage(hwndSB, SB_SETTEXT, 2, (LPARAM)Str);

	}
	int ndx = variable.NOCCTOT + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NSUBSCALETOT + variable.ROTATIONTOTX + variable.ROTATIONTOTY + variable.ROTATIONTOTZ;
	if (ndx > 0)
	{
		SendMessage(hwndSB, SB_SETTEXT, 3, (LPARAM)L"Par file read");
	}
	swprintf(Str, 126, L"Bulk %d Surf. %d", thread.NBULK, variable.NSURFTOT);
	SendMessage(hwndSB, SB_SETTEXT, 4, (LPARAM)Str);

}

//-------------------------------------------------------------------------------------------------------------------------
//Thread 3D functions added by Daniel Kaminski 01.01.2015
//Descryption: The scene 3D generated b DirectX is done in a thread so it is possible to observe any changes of paraneters 
//             in the same time as they are changing.
//-------------------------------------------------------------------------------------------------------------------------

void StartThread3D(Thread_t* lpThread)
{
	if (Thread3D == NULL)
	{
		Thread3D = CreateThread(NULL, 0, ThreadProc3D, lpThread, 0, &threadID_3D);
		SetThreadPriority(Thread3D, thread.priority);
	}
	else
	{
		CloseHandle(Thread3D);
		Thread3D = CreateThread(NULL, 0, ThreadProc3D, lpThread, 0, &threadID_3D);
		SetThreadPriority(Thread3D, THREAD_PRIORITY_NORMAL);
	}

	if (!WaitHandle3D) 
		WaitHandle3D = CreateEvent(
									NULL, // default security attributes
									FALSE, // auto-reset event object
									FALSE, // initial state is nonsignaled
									NULL); // unnamed object

}

void StopThreads3D()
{
	if (Thread3D != NULL)
	{
		CloseHandle(Thread3D);
		Thread3D = NULL;
	}
}

DWORD WINAPI  ThreadProc3D(LPVOID lpThread_)
{
	Thread_t *tx = (Thread_t*)lpThread_;

	WNDCLASSEX wclx;
	HWND hWndDX;
	MSG msgDX = { 0 };

	if (!RegisterMyClass(&wclx, tx->g_hInstance, L"WindowClassXD", WindowScene3DProc))
	{
		MessageBox(NULL, TEXT("Can't create window 3D class"), NULL, MB_ICONINFORMATION);
		goto stop;
	}

	hWndDX = myScene.CreateAppWindow(wclx, L"3D", variable.WindowsRect[10]);
	if (!hWndDX)
	{
		MessageBox(NULL, TEXT("Can't create window 3D"), NULL, MB_ICONERROR);
		goto stop;
	}
	else
	{
		OnDestroyDevice();
		if (!myScene.InitDX(hWndDX, OnInitDevice))
		{
			MessageBox(NULL, TEXT("Can't initialise DirectX device"), NULL, MB_ICONINFORMATION);
			goto stop;
		}
	}

	ShowWindow(hWndDX, SW_SHOW);
	UpdateWindow(hWndDX);

	if (!SetTimer(hWndDX, ID_TIMER, 30, 0))// set timer for window 1 (3D)
	{
		MessageBox(NULL, TEXT("Can't start timer DX"), NULL, MB_ICONINFORMATION);
		goto stop;
	}

	//Main thread loop/message pumping
	//--------------------------------------------------------------------
	while (Thread3D != NULL)
	{
		while (PeekMessage(&msgDX, 0, 0, 0, PM_REMOVE))
		{
			if (msgDX.message == WM_QUIT)
				goto stop;
			if (msgDX.message == WM_3D)
				msgDX.hwnd = hWndDX;

			TranslateMessage(&msgDX);
			DispatchMessage(&msgDX);
		}

		if (IsWindow(hWndDX))
			OnFrameRender(hWndDX, tx);

		if (!myScene.g_hasFocus)
			Sleep(1);
	}
	//--------------------------------------------------------------------

stop:
	OnThreadExit(hWndDX);
	OnDestroyDevice();
	DestroyWindow(hWndDX);

	if (!UnregisterClass(wclx.lpszClassName, tx->g_hInstance))
		MessageBox(NULL, TEXT("Can't unregister DX class"), NULL, MB_ICONERROR);

	SetEvent(WaitHandle3D);

	return 0;
}

void RestartDX_Window(bool store_size)
{
	RECT tmp_rect;
	if (store_size)
		CopyMemory(&tmp_rect, &variable.WindowsRect[10], sizeof(RECT));

	ResetEvent(WaitHandle3D);
	//Close 3D objects related to DirectX and DirectX
	PostThreadMessage(threadID_3D, WM_QUIT, 0, 0);
	//Wait until 3D is finished
	WaitForSingleObject(WaitHandle3D, INFINITE);
	Sleep(100);
	//Now the thread is safetly closed

	if (store_size)
		CopyMemory(&variable.WindowsRect[10], &tmp_rect, sizeof(RECT));

	StartThread3D(&thread);
}