\//--------------------
------------------------------------------------------------------------------------------------------------
// Purpose: The CallBack.cpp file containing all functions for dialog boxes.
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
//      include files                                                     
//--------------------------------------------------------------------------------------------------------------------------------
#include <stdafx.h>
#include <commctrl.h>
#include <Commdlg.h >
#include <Windowsx.h>
#include "CallBack.h"
#include "resource.h"
#include "definitions.h"
#include "Thread.h"
#include "fit.h"
#include "plot.h"
#include "set.h"
#include "variables.h"
#include "vfsr_funct.h"
#include "keating.h"
#include "elements.h"
#include "ReadFile.h"
#include "3Ddef.h"
#include "UndoRedo.h"
#include "GlobalFunctions.h"
#include "Import.h"
#include "dispersion.h"
#include "Scene3D.h"
#include "calc.h"
#include "ExMath.h"

extern UndoRedo						myUndoRedo;
extern Import						myImport;
CGlobalFunctions					myVersion;					//to retrive version info
Dispersion							myDispersion;
extern Calc							calc;


extern Scene3D		myScene;


extern VFSR_par vfsr_par;


HWND  hItemList;
int LeftListLimit;
int RightListLimit;
WNDPROC oldEditProc; //for handling mesages from Edit1 window
static int nItem, nSubItem;


//Fot molecule edit second window
HWND  hItemList3;
int LeftListLimit2;
int RightListLimit2;
WNDPROC oldEditProc2; //for handling mesages from Edit2 window
static int nItem2, nSubItem2;


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for multiple plot box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK MultiplePlot(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	double temp_h = 1000, temp_k = 1000;
	int temp_energy = 0;

	static double Storage_h[MAX_STORAGE_FOR_COMBO_RODS];
	static double Storage_k[MAX_STORAGE_FOR_COMBO_RODS];
	static int Storage_N[MAX_STORAGE_FOR_COMBO_RODS];
	static int Storage_E[MAX_STORAGE_FOR_COMBO_RODS];

	static int NumberOfRods = 0;
	int points_in_rod = 0;


	WCHAR WStr[100];
	WCHAR pszNr[10];

	LVCOLUMN		colNr;
	LVCOLUMN		colRod;

	LVITEM			lvTest;
	UINT			iIndex = 0;

	hItemList = GetDlgItem(hDlg, IDC_LIST1);

	switch (message)
	{
	case WM_INITDIALOG:
		swprintf_s(WStr, 20, L"%d", variable.Multi_L_points);
		SetDlgItemText(hDlg, IDC_EDIT1, WStr);

		for (i = 0; i < 10; ++i)
		{
			swprintf_s(WStr, 30, L"%d ", i+1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)WStr);
		}
		if (variable.Multi_N_column == 0) variable.Multi_N_column = 1;
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)(variable.Multi_N_column-1), 0);



		//Find rods
		NumberOfRods = 0;

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
					Storage_N[NumberOfRods] = points_in_rod;
					Storage_h[NumberOfRods] = temp_h;
					Storage_k[NumberOfRods] = temp_k;

					NumberOfRods++;
					if (NumberOfRods >= MAX_STORAGE_FOR_COMBO_RODS){
						MessageBox(NULL, TEXT("Maximum number of ROD entries in ListBox exceded."), NULL, MB_ICONERROR);
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
					Storage_N[NumberOfRods] = points_in_rod;
					Storage_h[NumberOfRods] = temp_h;
					Storage_k[NumberOfRods] = temp_k;
					Storage_E[NumberOfRods] = temp_energy;
					NumberOfRods++;//number of rods
					if (NumberOfRods >= MAX_STORAGE_FOR_COMBO_RODS){
						MessageBox(NULL, TEXT("Maximum number of ROD entries in ComboBox exceded."), NULL, MB_ICONERROR);
						break;
					}
				}
			}
		}

		//List view
		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);//

		//Create the columns in the list control
		//Name
		colNr.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr.pszText = TEXT("fix/free Nr");
		colNr.cchTextMax = 100;
		colNr.cx = 65;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 0, &colNr);

		//fix/free Parameter
		colRod.mask = LVCF_TEXT | LVCF_WIDTH;
		colRod.pszText = TEXT("Parameter");
		colRod.cchTextMax = 120;
		colRod.cx = 280;
		colRod.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colRod);

		//plot in table
		lvTest.mask = LVIF_TEXT | LVIF_STATE;
		lvTest.iItem = 0;
		lvTest.pszText = pszNr;
		lvTest.iIndent = 0;
		lvTest.stateMask = 0;
		lvTest.state = 0;
		lvTest.iSubItem = 0;


		variable.NumberOfMultiplots = NumberOfRods;
		//Rods
		for (i = NumberOfRods - 1; i >= 0; --i){

				swprintf_s(pszNr, 10, L"%4d", i);
				if (variable.ENERGYTOT == 0)
					swprintf_s(WStr, 100, L"Rod (%4.4f %4.4f) number of points: %d", Storage_h[i], Storage_k[i], Storage_N[i]);
				else
					swprintf_s(WStr, 100, L"Rod (%4.4f %4.4f) Energy:%d Nr of points:%d", Storage_h[i], Storage_k[i], Storage_E[i], Storage_N[i]);

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText(hItemList, iIndex, 1, WStr);
			}


			//Check boxes
			for (i = 0; i < NumberOfRods; ++i)
			{
				if (!variable.Multi_SelectPlot[i]){ ListView_SetCheckState(hItemList, i + 1, FALSE); }
				else ListView_SetCheckState(hItemList, i, TRUE);
			}


		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{

			//serial = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0) + 1;


			variable.Multi_L_points = (int)GetDlgItemReal(hDlg, IDC_EDIT1);
			if (variable.Multi_L_points < 2 || variable.Multi_L_points>MAXTHEO){
				swprintf_s(WStr, 100, L"Number of points is out of range 1 < x < %d ", MAXTHEO);
				MessageBox(hDlg, WStr, NULL, MB_OK);
				variable.Multi_L_points = MAXTHEO / 4;
				break;
			}

			variable.Multi_N_column = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0)+1;

			//Check boxes
			for (i = 0; i<NumberOfRods; ++i){
				if (ListView_GetCheckState(hItemList, i) == TRUE) variable.Multi_SelectPlot[i] = true;
				else variable.Multi_SelectPlot[i] = false;
			}
			Multiplot(variable.Multi_SelectPlot, variable.Multi_N_column, variable.Multi_L_points);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for coefficients plot box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK PlotCoefficients(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int serial=1;
	static int L_points = MAXTHEO;
	static double energy_Start = 0.5;
	static double energy_End= 25;
	WCHAR WStr[100];

	switch (message)
	{
	case WM_INITDIALOG:
		swprintf_s(WStr, 20, L"%4.3f", energy_Start);
		SetDlgItemText(hDlg, IDC_EDIT1, WStr);
		swprintf_s(WStr, 20, L"%4.3f", energy_End);
		SetDlgItemText(hDlg, IDC_EDIT2, WStr);
		swprintf_s(WStr, 20, L"%d", L_points);
		SetDlgItemText(hDlg, IDC_EDIT3, WStr);

		WCHAR Element[3];
		for (int i = 0; i <93; ++i)
		{
			AtoT(Element, elements[i]);
			swprintf_s(WStr, 30, L"%s ", Element);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)WStr);
		}
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM) (serial-1), 0);
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{

			serial = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0) + 1;

			energy_Start = GetDlgItemReal(hDlg, IDC_EDIT1);
			if (energy_Start < 0.1 || energy_Start>100.){
				MessageBox(hDlg, L"Energy is out of range, should be between 0.1 < x < 100 keV", NULL, MB_OK);
				break;
			}
			energy_End = GetDlgItemReal(hDlg, IDC_EDIT2);
			if (energy_End < 0.1 || energy_End>100.){
				MessageBox(hDlg, L"Energy is out of range, should be between 0.1 < x < 100 keV", NULL, MB_OK);
				break;
			}
			L_points = (int)GetDlgItemReal(hDlg, IDC_EDIT3);
			if (L_points < 2 || L_points>MAXTHEO){
				swprintf_s(WStr, 100, L"Number of points os out of range 1 < x < %d ", MAXTHEO);
				MessageBox(hDlg, WStr, NULL, MB_OK);
				break;
			}


			PlotCoeficients(serial, energy_Start, energy_End, L_points);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam)  == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK CalculateCoefficients(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static double energy = 0;
	static int serial = 1;
	WCHAR WStr[100];
	//Needs correction

	switch (message)
	{
	case WM_INITDIALOG:

		if (variable.ENERGYTOT == 0){
			MessageBox(hDlg, L"First read data file with the serial numbers of energy value! ", NULL, MB_OK);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		swprintf_s(WStr, 20, L"%4.3f", energy);

		SetDlgItemText(hDlg, IDC_EDIT1, WStr);
		swprintf_s(WStr, 20, L"");
		SetDlgItemText(hDlg, IDC_EDIT2, WStr);

		for (int i = 1; i < variable.ENERGYTOT + 1; ++i)
		{
			swprintf_s(WStr, 30, L"%d ", i);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)WStr);
		}
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)(variable.Energy), 0);

		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDOK)
		{
			int serial = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0) + 1;
			energy = GetDlgItemReal(hDlg, IDC_EDIT1);
			if (energy < 0.1 || energy>100.){
				MessageBox(hDlg, L"Energy is out of range, should be between 0.1 < x < 100 keV ", NULL, MB_OK);
				break;
			}

			int element = 50;
			double fi, fii;

			for (int j = 0; j < thread.NTYPES; ++j)
			{
				for (int i = 0; i < MAX_ELEMENTS; ++i)
				{
					if (CompareStr(elements[i], variable.ELEMENT[j]))
					{
						myDispersion.cromer(i+1, energy * 1000, &fi, &fii);
						thread.F1[j][serial] = fi;
						thread.F2[j][serial] = fii;
						break;
					}
				}
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	//int Reg_code;
	UNREFERENCED_PARAMETER(lParam);
	WCHAR WStr[9000];
	//Needs correction

	static WCHAR ProcesorVersion[50];



	switch (message){
	case WM_INITDIALOG:
#if defined _M_IX86
	swprintf_s(ProcesorVersion, 50, L"x32");
#elif defined _M_IA64
	swprintf_s(ProcesorVersion, 50, L"IA64");
#elif defined _M_X64
	swprintf_s(ProcesorVersion, 50, L"x64");
#endif
		swprintf_s(WStr, 9000, L"Support and testing: \r\n"
			L"Dr. Nikolay Vinogradov \r\n"
			L"Visiting Postdoctoral fellow\r\n"
			L"ID03 - Surface Diffraction Beamline\r\n"
			L"Experimental Division\r\n"
			L"ESRF - The European Synchrotron\r\n"
			L"71, avenue des Martyrs\r\n"
			L"38000 Grenoble\r\n"
			L"Postal address :\r\n"
			L"ESRF - The European Synchrotron\r\n"
			L"CS 40220\r\n"
			L"38043 Grenoble Cedex 9\r\n"
			L"tel : +33(0)476882969\r\n"
			L"mob : +33(0)698637426\r\n"
			L"email : nikolay.vinogradov@esrf.fr\r\n "
			
	//Rainer
			L"\r\n\r\n"
			L"Code analysis: \r\n"
			L"Rainer Wilcke \r\n"
			L"ISDD Software Group - Data Analysis Unit ESRF\r\n"
			L"CS 40220\r\n"
			L"F-38043 Grenoble Cedex 9\r\n"
			L"ESRF - The European Synchrotron\r\n"
			L"FRANCE\r\n"
			L"tel : (+33) 476 88 25 16\r\n"
			L"fax: (+33) 476 88 25 42\r\n"
			L"e-mail: wilcke@esrf.fr\r\n "

	//Sean Brennan
			L"\r\n\r\n"
			L"Fortran code for anomalious coefficients\r\n"
			L"bases on algorithm of Cromer and Lieberman was provided \r\n"
			L"by Sean Brennan \r\n"
			L"for more details see:\r\n"
			L"http://www.ccp14.ac.uk/ccp/ccp14/ftp-mirror/cross-section-sfac/pub/cross-ection_codes/\r\n"
						
			);

		SetDlgItemText(hDlg, IDC_EDIT1, WStr);




		swprintf_s(WStr, 5000, L"WinRod version %s (%s)", myVersion.GetVersionNumber(), ProcesorVersion);
		SetDlgItemText(hDlg, IDC_EDIT2, WStr);

		/*
		Reg_code = RegReadCode();
		if (Reg_code!=REGISTRATION_CODE)
		SetDlgItemInt(hDlg, IDC_EDIT1, 0, 0);
		else SetDlgItemInt(hDlg, IDC_EDIT1, Reg_code, 0);
		*/
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){
			/*
			Reg_code = GetDlgItemInt(hDlg, IDC_EDIT1,0 , 0);
			if(RegReadCode()!=REGISTRATION_CODE && Reg_code==REGISTRATION_CODE){
			RegSaveCode( Reg_code);
			MessageBox(hDlg, TEXT("Thank you for suport!"), TEXT("Registration"), MB_OK);
			}
			if(RegReadCode()!=REGISTRATION_CODE && Reg_code!=REGISTRATION_CODE){
			MessageBox(hDlg, TEXT("Wrong code!"), TEXT("Registration"), MB_OK);
			}*/
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for option box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Option(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	WCHAR TFileName[MAX_FILE_NAME];

	switch (message){
	case WM_INITDIALOG:
		if (variable.PrintTestFitFile) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);

		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 5));  // min. & max. positions
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETPAGESIZE,	0, (LPARAM)1); 
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETSEL, (WPARAM)FALSE, (LPARAM)MAKELONG(0, 5));		
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETPOS, (WPARAM)TRUE,  (LPARAM)variable.DetailsInFile);
		
		AtoT(TFileName, variable.NameTestFitFile);
		SetDlgItemText(hDlg, IDC_EDIT1, TFileName);
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK){
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) variable.PrintTestFitFile = true;
			else variable.PrintTestFitFile = false;

			variable.DetailsInFile = (int) SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_GETPOS, 0, 0);
			
			GetDlgItemText(hDlg, IDC_EDIT1, TFileName, MAX_FILE_NAME);
			TtoA(variable.NameTestFitFile, TFileName);
			OpenPrintFitFile();
			myUndoRedo.AddNewRecord(L"Options file save");
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL){
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for option box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Option3D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	switch (message){
	case WM_INITDIALOG:
		//if (variable.PrintTestFitFile) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);

		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 60));  // min. & max. positions
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETPAGESIZE, 0, (LPARAM)1);
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETSEL, (WPARAM)FALSE, (LPARAM)MAKELONG(0, 5));
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)thread.Refresh3DIdle);

		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK){
			//if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) variable.PrintTestFitFile = true;
			//else variable.PrintTestFitFile = false;

			thread.Refresh3DIdle = (int)SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_GETPOS, 0, 0);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL){
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK SetAsa(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){


	switch (message)
	{
	case WM_INITDIALOG:
	{

		if (vfsr_par.TEMPERATURE_RATIO_SCALE == 0.) vfsr_par.TEMPERATURE_RATIO_SCALE = _TEMPERATURE_RATIO_SCALE;
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SetSlider(GetDlgItem(hDlg, IDC_SLIDER1), 1.0e-8, 1.0e-6, vfsr_par.TEMPERATURE_RATIO_SCALE);
		SetDlgItemReal(hDlg, IDC_EDIT1, vfsr_par.TEMPERATURE_RATIO_SCALE);

		if (vfsr_par.COST_PARAMETER_SCALE == 0.) vfsr_par.COST_PARAMETER_SCALE = _COST_PARAMETER_SCALE;
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER2), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SetSlider(GetDlgItem(hDlg, IDC_SLIDER2), 0.5, 1.0, vfsr_par.COST_PARAMETER_SCALE);
		SetDlgItemReal(hDlg, IDC_EDIT2, vfsr_par.COST_PARAMETER_SCALE);

		if (vfsr_par.ACCEPTED_TO_GENERATED_RATIO == 0.) vfsr_par.ACCEPTED_TO_GENERATED_RATIO = _ACCEPTED_TO_GENERATED_RATIO;
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER3), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SetSlider(GetDlgItem(hDlg, IDC_SLIDER3), 1.0e-11, 1.0e-5, vfsr_par.ACCEPTED_TO_GENERATED_RATIO);
		SetDlgItemReal(hDlg, IDC_EDIT3, vfsr_par.ACCEPTED_TO_GENERATED_RATIO);

		if (vfsr_par.INITIAL_PARAMETER_TEMPERATURE == 0.) vfsr_par.INITIAL_PARAMETER_TEMPERATURE = _INITIAL_PARAMETER_TEMPERATURE;
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER4), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SetSlider(GetDlgItem(hDlg, IDC_SLIDER4), 1, 100, vfsr_par.INITIAL_PARAMETER_TEMPERATURE);
		SetDlgItemReal(hDlg, IDC_EDIT4, vfsr_par.INITIAL_PARAMETER_TEMPERATURE);

		if (vfsr_par.LIMIT_ACCEPTANCES == 0) vfsr_par.LIMIT_ACCEPTANCES = _LIMIT_ACCEPTANCES;
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER5), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SetSlider(GetDlgItem(hDlg, IDC_SLIDER5), 1000, 10000, vfsr_par.LIMIT_ACCEPTANCES);
		SetDlgItemReal(hDlg, IDC_EDIT5, vfsr_par.LIMIT_ACCEPTANCES);

		if (vfsr_par.TESTING_FREQUENCY_MODULUS == 0.) vfsr_par.TESTING_FREQUENCY_MODULUS = _TESTING_FREQUENCY_MODULUS;
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER6), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
		SetSlider(GetDlgItem(hDlg, IDC_SLIDER6), 50, 300, vfsr_par.TESTING_FREQUENCY_MODULUS);
		SetDlgItemReal(hDlg, IDC_EDIT6, (double)vfsr_par.TESTING_FREQUENCY_MODULUS);

		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK)
		{

			vfsr_par.TEMPERATURE_RATIO_SCALE = GetSlider(GetDlgItem(hDlg, IDC_SLIDER1), 1.0e-8, 1.0e-6);
			vfsr_par.COST_PARAMETER_SCALE = GetSlider(GetDlgItem(hDlg, IDC_SLIDER2), 0.5, 1.0);
			vfsr_par.ACCEPTED_TO_GENERATED_RATIO = GetSlider(GetDlgItem(hDlg, IDC_SLIDER3), 1.0e-11, 1.0e-5);
			vfsr_par.INITIAL_PARAMETER_TEMPERATURE = GetSlider(GetDlgItem(hDlg, IDC_SLIDER4), 1., 100.);
			vfsr_par.LIMIT_ACCEPTANCES = (int)GetSlider(GetDlgItem(hDlg, IDC_SLIDER5), 1000., 10000.);
			vfsr_par.TESTING_FREQUENCY_MODULUS = (int)GetSlider(GetDlgItem(hDlg, IDC_SLIDER6), 50., 300.);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}


		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == ID_DEFAULT)
		{
			vfsr_par.TEMPERATURE_RATIO_SCALE = _TEMPERATURE_RATIO_SCALE;
			vfsr_par.COST_PARAMETER_SCALE = _COST_PARAMETER_SCALE;
			vfsr_par.ACCEPTED_TO_GENERATED_RATIO = _ACCEPTED_TO_GENERATED_RATIO;
			vfsr_par.INITIAL_PARAMETER_TEMPERATURE = _INITIAL_PARAMETER_TEMPERATURE;
			vfsr_par.LIMIT_ACCEPTANCES = _LIMIT_ACCEPTANCES;
			vfsr_par.TESTING_FREQUENCY_MODULUS = _TESTING_FREQUENCY_MODULUS;

			SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
			SetSlider(GetDlgItem(hDlg, IDC_SLIDER1), 1.0e-8, 1.0e-6, vfsr_par.TEMPERATURE_RATIO_SCALE);
			SetDlgItemReal(hDlg, IDC_EDIT1, vfsr_par.TEMPERATURE_RATIO_SCALE);

			SendMessage(GetDlgItem(hDlg, IDC_SLIDER2), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
			SetSlider(GetDlgItem(hDlg, IDC_SLIDER2), 0.5, 1.0, vfsr_par.COST_PARAMETER_SCALE);
			SetDlgItemReal(hDlg, IDC_EDIT2, vfsr_par.COST_PARAMETER_SCALE);

			SendMessage(GetDlgItem(hDlg, IDC_SLIDER3), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
			SetSlider(GetDlgItem(hDlg, IDC_SLIDER3), 1.0e-11, 1.0e-5, vfsr_par.ACCEPTED_TO_GENERATED_RATIO);
			SetDlgItemReal(hDlg, IDC_EDIT3, vfsr_par.ACCEPTED_TO_GENERATED_RATIO);

			SendMessage(GetDlgItem(hDlg, IDC_SLIDER4), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
			SetSlider(GetDlgItem(hDlg, IDC_SLIDER4), 1, 100, vfsr_par.INITIAL_PARAMETER_TEMPERATURE);
			SetDlgItemReal(hDlg, IDC_EDIT4, vfsr_par.INITIAL_PARAMETER_TEMPERATURE);

			SendMessage(GetDlgItem(hDlg, IDC_SLIDER5), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
			SetSlider(GetDlgItem(hDlg, IDC_SLIDER5), 1000, 10000, vfsr_par.LIMIT_ACCEPTANCES);
			SetDlgItemReal(hDlg, IDC_EDIT5, vfsr_par.LIMIT_ACCEPTANCES);

			SendMessage(GetDlgItem(hDlg, IDC_SLIDER6), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
			SetSlider(GetDlgItem(hDlg, IDC_SLIDER6), 50, 300, vfsr_par.TESTING_FREQUENCY_MODULUS);
			SetDlgItemReal(hDlg, IDC_EDIT6, vfsr_par.TESTING_FREQUENCY_MODULUS);

		}	
		
	}
		
	case WM_NOTIFY://sliders
	{
		//sliders
		if (LOWORD(wParam) == IDC_SLIDER1)
		{
			SetDlgItemReal(hDlg, IDC_EDIT1, GetSlider(GetDlgItem(hDlg, IDC_SLIDER1), 1.0e-8, 1.0e-6));
			break;
		}

		if (LOWORD(wParam) == IDC_SLIDER2)
		{
			SetDlgItemReal(hDlg, IDC_EDIT2, GetSlider(GetDlgItem(hDlg, IDC_SLIDER2), 0.5, 1.0));
			break;
		}
		if (LOWORD(wParam) == IDC_SLIDER3)
		{
			SetDlgItemReal(hDlg, IDC_EDIT3, GetSlider(GetDlgItem(hDlg, IDC_SLIDER3), 1.0e-11, 1.0e-5));
			break;
		}
		if (LOWORD(wParam) == IDC_SLIDER4)
		{
			SetDlgItemReal(hDlg, IDC_EDIT4, GetSlider(GetDlgItem(hDlg, IDC_SLIDER4), 1, 100));
			break;
		}
		if (LOWORD(wParam) == IDC_SLIDER5)
		{
			SetDlgItemReal(hDlg, IDC_EDIT5, GetSlider(GetDlgItem(hDlg, IDC_SLIDER5), 1000, 10000));
			break;
		}
		if (LOWORD(wParam) == IDC_SLIDER6)
		{
			SetDlgItemReal(hDlg, IDC_EDIT6, GetSlider(GetDlgItem(hDlg, IDC_SLIDER6), 50, 300));
			break;
		}
	}


	}
		return (INT_PTR)FALSE;
}


void SetSlider(HWND hSlider, double Min, double Max, double value)
{
	int position = (int)myRound((value - Min) / (Max - Min) * 100);
	SendMessage(hSlider, TBM_SETPOS,
		(WPARAM)TRUE,                  // redraw myScene.flag 
		(LPARAM)position);
}

//-----------------------------------------------------------------------------
// Function: Get values of all sliders
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
double GetSlider(HWND hSlider, double Min, double Max)
{
	double position = (double)SendMessage(hSlider, TBM_GETPOS, 0, 0);
	return (((Max - Min)*position / 100. + Min));

}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for Error increase near bragg refl. box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK ErrorIncrease(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	static double interval = 0.2, fraction_error = 0.1;


	switch (message){
	case WM_INITDIALOG:

		SetDlgItemReal(hDlg, IDC_EDIT6, interval);
		SetDlgItemReal(hDlg, IDC_EDIT2, fraction_error);


		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{
			interval = (GetDlgItemReal(hDlg, IDC_EDIT6));
			fraction_error = (GetDlgItemReal(hDlg, IDC_EDIT2));

			decrease_bragg_weight(interval, fraction_error);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	}
	return (INT_PTR)FALSE;
}



//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for Data reduction near bragg refl. box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Data_reduction(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {



	switch (message) {
	case WM_INITDIALOG:

		SetDlgItemReal(hDlg, IDC_EDIT1, variable.reduction_step);
		SetDlgItemReal(hDlg, IDC_EDIT2, variable.reduction_minF);
		SetDlgItemReal(hDlg, IDC_EDIT9, variable.reduction_MaxL);

		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{
			variable.reduction_step = (int)(GetDlgItemReal(hDlg, IDC_EDIT1));
			variable.reduction_minF = (GetDlgItemReal(hDlg, IDC_EDIT2));
			variable.reduction_MaxL = (GetDlgItemReal(hDlg, IDC_EDIT9));


			ReduceData();
			myUndoRedo.AddNewRecord(L"Data reduction");

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	}
	return (INT_PTR)FALSE;
}
//--------------------------------------------------------------------------------------------------------------------------------
//                                                    
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK ElectronDensityProfileSettings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	//int Reg_code;
	UNREFERENCED_PARAMETER(lParam);


	switch (message){
	case WM_INITDIALOG:
		SetDlgItemReal(hDlg, IDC_EDIT1, variable.ell_dens_h);
		SetDlgItemReal(hDlg, IDC_EDIT2, variable.ell_dens_k);


		SetDlgItemInt(hDlg, IDC_EDIT6, variable.N_points, 0);
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK){

			if (GetDlgItemReal(hDlg, IDC_EDIT1) <1000 && GetDlgItemReal(hDlg, IDC_EDIT1) >-1000)
				variable.ell_dens_h = GetDlgItemReal(hDlg, IDC_EDIT1);
			if (GetDlgItemReal(hDlg, IDC_EDIT2) <1000 && GetDlgItemReal(hDlg, IDC_EDIT2)>-1000)
				variable.ell_dens_k = GetDlgItemReal(hDlg, IDC_EDIT2);


			if (GetDlgItemReal(hDlg, IDC_EDIT6) <9000 && GetDlgItemReal(hDlg, IDC_EDIT6)>1)
				variable.N_points = (int)GetDlgItemReal(hDlg, IDC_EDIT6);
			

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL){
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for calculate box.  
//modified 05.07.2014
//Created by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
 INT_PTR CALLBACK Calculate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	 WCHAR WStr[100];

	switch (message){
		case WM_INITDIALOG:

			SetDlgItemReal(hDlg, IDC_EDIT1, variable.h);		SetDlgItemReal(hDlg, IDC_EDIT2, variable.k);		SetDlgItemReal(hDlg, IDC_EDIT12, variable.l);
			SetDlgItemReal(hDlg, IDC_EDIT5, variable.h_start);  SetDlgItemReal(hDlg, IDC_EDIT10, variable.k_start); SetDlgItemReal(hDlg, IDC_EDIT3, variable.l_start);
			SetDlgItemReal(hDlg, IDC_EDIT9, variable.h_end);	SetDlgItemReal(hDlg, IDC_EDIT11, variable.k_end);	SetDlgItemReal(hDlg, IDC_EDIT4, variable.l_end);
			SetDlgItemInt(hDlg, IDC_EDIT7, variable.N_points, 0);

			if (variable.ENERGYTOT==0)
				EnableWindow(GetDlgItem(hDlg, IDC_COMBO1), false);
			else
			{
				for (int i = 1; i < variable.ENERGYTOT + 1; ++i)
				{
					swprintf_s(WStr, 30, L"%d ", i);
					SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)WStr);
				}
				SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)(variable.Energy - 1), 0);
			}
		return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK){	
				if (GetDlgItemReal(hDlg, IDC_EDIT1) <1000 && GetDlgItemReal(hDlg, IDC_EDIT1) >-1000)
				variable.h = GetDlgItemReal(hDlg, IDC_EDIT1);
				if (GetDlgItemReal(hDlg, IDC_EDIT2) <1000 && GetDlgItemReal(hDlg, IDC_EDIT2)>-1000)
				variable.k = GetDlgItemReal(hDlg, IDC_EDIT2);	
				if (GetDlgItemReal(hDlg, IDC_EDIT12)<1000 && GetDlgItemReal(hDlg, IDC_EDIT12)>-1000)
				variable.l = GetDlgItemReal(hDlg, IDC_EDIT12);
				if (GetDlgItemReal(hDlg, IDC_EDIT5) <100 && GetDlgItemReal(hDlg, IDC_EDIT5)>-100)
				variable.h_start = GetDlgItemReal(hDlg, IDC_EDIT5);
				if (GetDlgItemReal(hDlg, IDC_EDIT10) <100 && GetDlgItemReal(hDlg, IDC_EDIT10)>-100)
				variable.k_start = GetDlgItemReal(hDlg, IDC_EDIT10);
				if (GetDlgItemReal(hDlg, IDC_EDIT3) <100 && GetDlgItemReal(hDlg, IDC_EDIT3)>-100)
				variable.l_start = GetDlgItemReal(hDlg, IDC_EDIT3);
				if (GetDlgItemReal(hDlg, IDC_EDIT9) <100 && GetDlgItemReal(hDlg, IDC_EDIT9)>-100)
				variable.h_end = GetDlgItemReal(hDlg, IDC_EDIT9);
				if (GetDlgItemReal(hDlg, IDC_EDIT11) <100 && GetDlgItemReal(hDlg, IDC_EDIT11)>-100)
				variable.k_end = GetDlgItemReal(hDlg, IDC_EDIT11);
				if (GetDlgItemReal(hDlg, IDC_EDIT4) <100 && GetDlgItemReal(hDlg, IDC_EDIT4)>-100)
				variable.l_end = GetDlgItemReal(hDlg, IDC_EDIT4);

				int tmpi=GetDlgItemInt(hDlg, IDC_EDIT7, 0, 0);
				if(tmpi>=0 && tmpi<MAXTHEO) variable.N_points = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value ", NULL, MB_OK);

				variable.Energy = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0)+1;

				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDCANCEL){	
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
		break;
	}
 return (INT_PTR)FALSE;
 }


 //--------------------------------------------------------------------------------------------------------------------------------
 // Message handler for calculate box.  
 //modified 05.07.2014
 //Created by Daniel Kaminski
 //--------------------------------------------------------------------------------------------------------------------------------
 INT_PTR CALLBACK Sort_Bulk(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {

//	 WCHAR WStr[100];

	 switch (message) {
	 case WM_INITDIALOG:


		 return (INT_PTR)TRUE;

	 case WM_COMMAND:
		 if (LOWORD(wParam) == IDOK) {


			 FunctionSort();

			 EndDialog(hDlg, LOWORD(wParam));
			 return (INT_PTR)TRUE;
		 }

		 if (LOWORD(wParam) == IDCANCEL) {
			 EndDialog(hDlg, LOWORD(wParam));
			 return (INT_PTR)TRUE;
		 }
		 break;
	 }
	 return (INT_PTR)FALSE;
 }


 void FunctionSort(void)
 {
	 int i;
	 //Sort atoms to be closest to the first one one by one
	 bool fix[MAXATOMS];
	 for ( i = 1; i < 100; i += 2)
		 Find_closest(i, fix);

	 for (i = 1; i < thread.NBULK; i++)
		 fix[i] = false;

	 for (i = 1; i < 200; i += 2)
		 Find_closest(i, fix);

	 for (i = 1; i < thread.NBULK; i++)
		 fix[i] = false;

	 for (i = 1; i < 500; i += 1)
		 Find_closest(i, fix);

	 for (i = 1; i < thread.NBULK; i++)
		 fix[i] = false;

	 for (i = 1; i < 500; i += 1)
		 Find_closest(i, fix);

 }

 void  Find_closest(double radius, bool * fix)
 {
	 int n, i;
	 int xi, yi, zi;
	 //int xs, ys, zs;
	 double v_dist, v_min;


	 for (n = 0; n < thread.NBULK - 1; n++) //skan reference atoms
	 {
		 for (i = n + 1; i < thread.NBULK; i++) //skan next atoms
		 {
			 v_min = 1000.0;
			 //Scan translation equivalents
			 for (xi = -1; xi < 2; xi++)
			 {
				 for (yi = -1; yi < 2; yi++)
				 {
					 for (zi = -1; zi < 2; zi++)
					 {
						 v_dist = distance(thread.XB[n], thread.XB[i] + xi, thread.YB[n], thread.YB[i] + yi, thread.ZB[n], thread.ZB[i] + zi);

						 if (v_dist < v_min && v_dist < radius && !fix[i])
						 {
							 v_min = v_dist;
							 thread.XB[i] += xi;
							 thread.YB[i] += yi;
							 thread.ZB[i] += zi;
							 fix[i] = true;
						 }
					 }
				 }
			 }

		 }
	 }
 }

 //x,y,z - reference location atom
 //rx,ry,rz- tested atom
 //nx, ny, nz - translation equvalents
 double distance(double x, double rx, double y, double ry, double z, double rz)
 {

	 double x1 =  x * variable.a_sin_alp +  y * variable.b_cos_gam +  z * variable.c_cos_bet__sin_alp;
	 double y1 =  y * variable.b_sin_Gam +  z * variable.c_cos_alp;
	 double z1 =  z * variable.c_sin_bet__sin_alp;

	 double  x2 = rx * variable.a_sin_alp + ry * variable.b_cos_gam + rz * variable.c_cos_bet__sin_alp;
	 double  y2 = ry * variable.b_sin_Gam + rz * variable.c_cos_alp;
	 double  z2 = rz * variable.c_sin_bet__sin_alp;

	 double sx = x1 -  x2;
	 double sy = y1 -  y2;
	 double sz = z1 -  z2;
	 return sqrt(sx*sx + sy * sy + sz * sz);
 }





//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for domains box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
 INT_PTR CALLBACK Domains(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	 static int current_domain;
	 int i;
	 static double DOMMAT11[MAXDOM];
	 static double DOMMAT12[MAXDOM];
	 static double DOMMAT21[MAXDOM];
	 static double DOMMAT22[MAXDOM];
	 static double DOMOCCUP[MAXDOM];
	 static int	NumberOfDomaind;

	 switch (message){
	 case WM_INITDIALOG:

		 for (i = 0; i < thread.NDOMAIN; i++){
			 DOMMAT11[i] = thread.DOMMAT11[i];
			 DOMMAT12[i] = thread.DOMMAT12[i];
			 DOMMAT21[i] = thread.DOMMAT21[i];
			 DOMMAT22[i] = thread.DOMMAT22[i];
			 DOMOCCUP[i] = thread.DOMOCCUP[i];
		 }

		 SetDlgItemReal(hDlg, IDC_EDIT1, DOMMAT11[current_domain]);
		 SetDlgItemReal(hDlg, IDC_EDIT2, DOMMAT12[current_domain]);
		 SetDlgItemReal(hDlg, IDC_EDIT4, DOMMAT21[current_domain]);
		 SetDlgItemReal(hDlg, IDC_EDIT5, DOMMAT22[current_domain]);

		 //set number of domains
		 WCHAR TempStr[3];
		 for (i = 1; i < MAXDOM + 1; ++i)
		 {
			 swprintf(TempStr, 3, L"%d", i);
			 SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TempStr);
		 }
		 SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)thread.NDOMAIN - 1, 0);

		 NumberOfDomaind = thread.NDOMAIN;
		 //actual domain to work with
		 SetDlgItemInt(hDlg, IDC_EDIT3, current_domain + 1, 0);
		 SetDlgItemReal(hDlg, IDC_EDIT7, DOMOCCUP[current_domain]);

		 if (variable.DOMEQUAL == true)
			 SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);


		 if (variable.DOMOCCUPAUTO == true)
		 {
			 SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_SETCHECK, BST_CHECKED, 0);

			 EnableWindow(GetDlgItem(hDlg, IDC_CHECK1), false);
			 EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), false);
		 }
		 else
		 {

			 EnableWindow(GetDlgItem(hDlg, IDC_CHECK1), true);
			 if (variable.DOMEQUAL == true)
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), false);
			 else
				 EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), true);
		 }


		 if (variable.DOMEQUAL == true)
		 {
			 SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);
			 EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), false);
		 }

		 return (INT_PTR)TRUE;

	 case WM_COMMAND:


		 if (LOWORD(wParam) == IDOK){

			 //Copy current domain
			 DOMMAT11[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT1);
			 DOMMAT12[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT2);
			 DOMMAT21[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT4);
			 DOMMAT22[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT5);
			 DOMOCCUP[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT7);


			 thread.NDOMAIN = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0) + 1;
			 for (i = 0; i < thread.NDOMAIN; ++i){
				 thread.DOMMAT11[i] = DOMMAT11[i];
				 thread.DOMMAT12[i] = DOMMAT12[i];
				 thread.DOMMAT21[i] = DOMMAT21[i];
				 thread.DOMMAT22[i] = DOMMAT22[i];
				 thread.DOMOCCUP[i] = DOMOCCUP[i];
			 }

			 if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED)
			 {
				 variable.DOMEQUAL = true;
				 for (i = 0; i < thread.NDOMAIN; i++)
				 {
					 thread.DOMOCCUP[i] = 1. / thread.NDOMAIN;
				 }
			 }
			 else
				 variable.DOMEQUAL = false;

			 if (SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED)
			 {
				 variable.DOMOCCUPAUTO = true;

				 for (i = 0; i < thread.NDOMAIN; ++i)
				 {
					 variable.DOMOCCUPLIM[i][1] = 1;
					 variable.DOMOCCUPPEN[i] = true;
				 }

			 }
			 else
				 variable.DOMOCCUPAUTO = false;

			 if (current_domain>NumberOfDomaind) current_domain = 0;

			 //Add record to UndoRedo
			 myUndoRedo.AddNewRecord(L"Domains");

			 EndDialog(hDlg, LOWORD(wParam));
			 return (INT_PTR)TRUE;
		 }

		 if (LOWORD(wParam) == IDCANCEL){
			 EndDialog(hDlg, LOWORD(wParam));
			 return (INT_PTR)TRUE;
		 }


		 if (LOWORD(wParam) == IDC_COMBO1){
			 NumberOfDomaind = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0) + 1;
			 if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED){

				 for (i = 0; i < thread.NDOMAIN; i++){
					 DOMOCCUP[i] = 1. / NumberOfDomaind;
				 }
			 }
			 SetDlgItemReal(hDlg, IDC_EDIT7, DOMOCCUP[current_domain]);
			 return (INT_PTR)FALSE;
		 }

		 if (LOWORD(wParam) == IDC_CHECK1)
		 {
			 if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), false);
			 else EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), true);
			 return (INT_PTR)FALSE;
		 }


		 if (LOWORD(wParam) == IDC_CHECK2)
		 {
			 if (SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED)
			 {
				 EnableWindow(GetDlgItem(hDlg, IDC_CHECK1), false);
				 EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), false);
			 }
			 else
			 {
				 EnableWindow(GetDlgItem(hDlg, IDC_CHECK1), true);
				 EnableWindow(GetDlgItem(hDlg, IDC_EDIT7), true);
			 }
			 return (INT_PTR)FALSE;
		 }


		 if (LOWORD(wParam) == IDC_BUTTON1){
			 if (current_domain > 0) current_domain--;
			 SetDlgItemInt(hDlg, IDC_EDIT3, current_domain + 1, 0);
			 SetDlgItemReal(hDlg, IDC_EDIT1, DOMMAT11[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT2, DOMMAT12[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT4, DOMMAT21[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT5, DOMMAT22[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT7, DOMOCCUP[current_domain]);
			 return (INT_PTR)FALSE;
		 }

		 if (LOWORD(wParam) == IDC_BUTTON2){
			 NumberOfDomaind = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0) + 1;
			 if (current_domain < NumberOfDomaind - 1) current_domain++;
			 SetDlgItemInt(hDlg, IDC_EDIT3, current_domain + 1, 0);

			 SetDlgItemReal(hDlg, IDC_EDIT1, DOMMAT11[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT2, DOMMAT12[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT4, DOMMAT21[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT5, DOMMAT22[current_domain]);
			 SetDlgItemReal(hDlg, IDC_EDIT7, DOMOCCUP[current_domain]);
			 return (INT_PTR)FALSE;
		 }

		 if (LOWORD(wParam) == IDC_BUTTON5){
			 DOMMAT11[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT1);
			 DOMMAT12[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT2);
			 DOMMAT21[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT4);
			 DOMMAT22[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT5);
			 DOMOCCUP[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT7);
			 return (INT_PTR)FALSE;
		 }
	 }
	 return (INT_PTR)FALSE;
 }

void FillKettingList( HWND  hItemList){

 int i, j;
 WCHAR Tstr[300];
 char  str[300];

 WCHAR pszNr[8];
 WCHAR pszElement[100];
 WCHAR pszAtRadius[100];
 WCHAR pszBonded[1000];

 LVITEM			lvTest;
 UINT			iIndex = 0;

	ListView_DeleteAllItems( hItemList);

 	//plot in table
	lvTest.mask = LVIF_TEXT | LVIF_STATE;
	lvTest.iItem = 0;
	lvTest.pszText = pszNr;
	lvTest.iIndent = 0;
	lvTest.stateMask = 0;
	lvTest.state = 0;
	lvTest.iSubItem = 0;


	for (i = thread.NSURF-1; i >= 0 ; i--){

		swprintf_s(pszNr,		8,	L"%4d", i);
		str[0]=variable.ELEMENT[thread.TS[i]][0]; str[1]=variable.ELEMENT[thread.TS[i]][1]; str[2]=variable.ELEMENT[thread.TS[i]][2];
		AtoT(Tstr, str);
		swprintf_s(pszElement, 100,	L"%s%d", Tstr, i+1  );
		swprintf_s(pszAtRadius,	100, L"%6.4f", ket.ATOM_RAD[thread.TS[i]]);

		//bonded to
		int n=0;
		swprintf_s(pszBonded,	1000,	L"" );
		for (j = 0; j < ket.NBOND[i]; j++){
			int atom = ket.BOND_LIST[i][j];
			str[0]=variable.ELEMENT[thread.TS[atom]][0]; str[1]=variable.ELEMENT[thread.TS[atom]][1]; str[2]=variable.ELEMENT[thread.TS[atom]][2];
			AtoT(Tstr, str);
			n+=swprintf_s(pszBonded+n,	1000,	L"%s%d, ", Tstr, atom +1);
			if(n>=MAXBOND*6)
				break;
		}

		lvTest.pszText = pszNr;

		iIndex = ListView_InsertItem(hItemList, &lvTest);
		ListView_SetItemText( hItemList, iIndex, 1, pszElement);	
		ListView_SetItemText( hItemList, iIndex, 2, pszAtRadius);	
		ListView_SetItemText( hItemList, iIndex, 3, pszBonded);
	}
	//set check boxes
	for (j = 0; j < thread.NSURF; j++)
		if(ket.IN_KEAT[j] )
			ListView_SetCheckState(  hItemList, j , 1);
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for ketting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Keating(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	static int nItem, nSubItem;
	static int ntot;

	LVCOLUMN		colNr;
	LVCOLUMN		colElement;
	LVCOLUMN		colAtRadius;
	LVCOLUMN		colBonded;

	HWND  hItemList = GetDlgItem(hDlg, IDC_LIST2);

	switch (message){
	case WM_INITDIALOG:
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		SetDlgItemReal(hDlg, IDC_EDIT10, ket.EQU_ANGLE*RAD);
		SetDlgItemReal(hDlg, IDC_EDIT2, ket.ALPHA_KEAT);
		SetDlgItemReal(hDlg, IDC_EDIT3, ket.BETA_KEAT);
		if (ket.POTENTIAL == VKEATING) SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
		if (ket.POTENTIAL == VLENNARDJONES) SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, BST_CHECKED, 0);
		if (ket.KEAT_PLUS_CHI == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create the columns in the list control
		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//Name
		colNr.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr.pszText = TEXT("Check Nr");
		colNr.cchTextMax = 100;
		colNr.cx = 65;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 0, &colNr);

		//Element
		colElement.mask = LVCF_TEXT | LVCF_WIDTH;
		colElement.pszText = TEXT("Element");
		colElement.cchTextMax = 40;
		colElement.cx = 120;
		colElement.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colElement);

		//Atomic radius
		colAtRadius.mask = LVCF_TEXT | LVCF_WIDTH;
		colAtRadius.pszText = TEXT("Atomic radius");
		colAtRadius.cchTextMax = 100;
		colAtRadius.cx = 70;
		colAtRadius.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 2, &colAtRadius);

		//Bonded
		colBonded.mask = LVCF_TEXT | LVCF_WIDTH;
		colBonded.pszText = TEXT("Bonded to");
		colBonded.cchTextMax = 1000;
		colBonded.cx = 1000;
		colBonded.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 3, &colBonded);

		//Set values in table
		FillKettingList(hItemList);

		break;


		/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

	case WM_NOTIFY:
		switch (((NMHDR *)lParam)->code)
		{
		case NM_CLICK://IDC_LIST1
			OnClickList(hDlg, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)lParam, &nItem, &nSubItem, 2, 100);
			break;
		}
		break;

	case WM_COMMAND:
		switch LOWORD(wParam){

		case IDOK:
		{
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED)
				ket.KEAT_PLUS_CHI = true;
			else ket.KEAT_PLUS_CHI = false;

			if (SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_GETCHECK, 0, 0) == BST_CHECKED)
				ket.POTENTIAL = VKEATING;
			else
				ket.POTENTIAL = VLENNARDJONES;

			double temp;
			temp = GetDlgItemReal(hDlg, IDC_EDIT10);
			if (temp >= -360 && temp < 360) ket.EQU_ANGLE = temp / RAD;
			else MessageBox(hDlg, L"Wrong angle", NULL, MB_OK);

			temp = GetDlgItemReal(hDlg, IDC_EDIT2);
			if (temp >= 0 && temp < 10000000) ket.ALPHA_KEAT = temp;
			else MessageBox(hDlg, L"Wrong alpha keating", NULL, MB_OK);

			temp = GetDlgItemReal(hDlg, IDC_EDIT3);
			if (temp >= 0 && temp < 10000000) ket.BETA_KEAT = temp;
			else MessageBox(hDlg, L"Wrong beta keating", NULL, MB_OK);

			CopyValuesFromKeatingListBox(hItemList);
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
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				InvalidateRect(hItemList, 0, 0);
			}
		}
			break;

		case IDC_BUTTON3:
		{
			CopyValuesFromKeatingListBox(hItemList);
			bond_search(&thread);
			FillKettingList(hItemList);
		}
			break;

		case IDC_BUTTON4:
		{
			CopyValuesFromKeatingListBox(hItemList);
			angle_search(&thread);
		}
			break;

		case IDC_BUTTON5:
		{
			InitKeating(myScene.AtProp.AtomRadius);
			FillKettingList(hItemList);
		}
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for settings box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	double temp;
	int tmpi;

	switch (message) {
	case WM_INITDIALOG:
		EnterCriticalSection(&myScene.CriticalSection);

		SetDlgItemReal(hDlg, IDC_EDIT1, thread.ATTEN, L"%3.8f");
		SetDlgItemReal(hDlg, IDC_EDIT2, thread.BETA, L"%3.4f");
		SetDlgItemReal(hDlg, IDC_EDIT9, thread.SCALE, L"%3.4f");

		SetDlgItemReal(hDlg, IDC_EDIT5, thread.SURFFRAC, L"%3.3f");
		SetDlgItemReal(hDlg, IDC_EDIT3, thread.SURF2FRAC, L"%3.3f");
		SetDlgItemInt(hDlg, IDC_EDIT8, thread.NSURF, 0);
		SetDlgItemInt(hDlg, IDC_EDIT7, thread.NSURF2, 0);

		SetDlgItemReal(hDlg, IDC_EDIT10, thread.LBR[0], L"%3.2f");
		SetDlgItemInt(hDlg, IDC_EDIT11, thread.NLAYERS, 0);


		//SetDlgItemInt (hDlg, IDC_EDIT9, thread.NLAYERS, 0);
		//if(FLARES==TRUE ) SendMessage(GetDlgItem(hDlg, IDC_FLARES), BM_SETCHECK, BST_CHECKED, 0);

		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("APPROXBETA"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("NUMBETA"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("POISSONROUGH"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("GAUSSROUGH"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("LINEARROUGH"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("COSINEROUGH"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("TWOLEVEL"));

		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)thread.ROUGHMODEL, 0);

		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("2"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("3"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("4"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("5"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("6"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("7"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)TEXT("8"));

		if (thread.NumberOfCores<1) 
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, (WPARAM)1, 0);
		else
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, (WPARAM)thread.NumberOfCores - 1, 0);

		//Priority
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)TEXT("LOWEST"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)TEXT("BELOW NORMAL"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)TEXT("NORMAL"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)TEXT("ABOVE NORMAL"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_ADDSTRING, 0, (LPARAM)TEXT("THREAD_PRIORITY_HIGHEST"));

		if (thread.priority == THREAD_PRIORITY_LOWEST) SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_SETCURSEL, (WPARAM)0, 0); //THREAD_PRIORITY_LOWEST  THREAD_PRIORITY_BELOW_NORMAL
		if (thread.priority == THREAD_PRIORITY_BELOW_NORMAL) SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_SETCURSEL, (WPARAM)1, 0);
		if (thread.priority == THREAD_PRIORITY_NORMAL) SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_SETCURSEL, (WPARAM)2, 0);
		if (thread.priority == THREAD_PRIORITY_ABOVE_NORMAL) SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_SETCURSEL, (WPARAM)3, 0);
		if (thread.priority == THREAD_PRIORITY_HIGHEST) SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_SETCURSEL, (WPARAM)4, 0);


		SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_ADDSTRING, 0, (LPARAM)TEXT("CHISQR"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_ADDSTRING, 0, (LPARAM)TEXT("COVARIANCE"));
	
		SendMessage(GetDlgItem(hDlg, IDC_COMBO5), CB_ADDSTRING, 0, (LPARAM)TEXT("NONE"));
		SendMessage(GetDlgItem(hDlg, IDC_COMBO5), CB_ADDSTRING, 0, (LPARAM)TEXT("MODEL1"));


		if (variable.LSQ_ERRORCALC == CHISQR) SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_SETCURSEL, (WPARAM)0, 0);
		if (variable.LSQ_ERRORCALC == COVARIANCE) SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_SETCURSEL, (WPARAM)1, 0);

		if (variable.NFACTOR == 0) SendMessage(GetDlgItem(hDlg, IDC_COMBO5), CB_SETCURSEL, (WPARAM)0, 0);
		if (variable.NFACTOR == 3) SendMessage(GetDlgItem(hDlg, IDC_COMBO5), CB_SETCURSEL, (WPARAM)1, 0);

		if (thread.ZEROFRACT == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);
		if (thread.BULK_ZEROFRACT == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_SETCHECK, BST_CHECKED, 0);
		if (thread.STRUCFAC == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_SETCHECK, BST_CHECKED, 0);
		if (thread.LIQUID_PROFILE == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_SETCHECK, BST_CHECKED, 0);
		if (thread.COHERENTDOMAINS == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK6), BM_SETCHECK, BST_CHECKED, 0);

		if (thread.INCOMENSURATE == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK11), BM_SETCHECK, BST_CHECKED, 0);

		LeaveCriticalSection(&myScene.CriticalSection);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDOK)
		{

			//Stop all fitting threads
			StopThreads();

			EnterCriticalSection(&myScene.CriticalSection);
			temp = GetDlgItemReal(hDlg, IDC_EDIT1);
			if (temp >= 0.0 && temp < 1000.0) thread.ATTEN = temp;
			else
			{
				MessageBox(hDlg, L"Wrong ATTENUATION value ", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT2);
			if (temp >= 0.0 && temp <= 1.0) thread.BETA = temp;
			else
			{
				MessageBox(hDlg, L"Wrong BETA value ", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT9);
			if (temp >= 0.0 && temp < 10000.0) thread.SCALE = temp;
			else
			{
				MessageBox(hDlg, L"Wrong SCALE value", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}
			temp = GetDlgItemReal(hDlg, IDC_EDIT5);
			if (temp >= 0.0 && temp <= 1.0) thread.SURFFRAC = temp;
			else
			{
				MessageBox(hDlg, L"Wrong SURFFRAC value", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT3);
			if (temp >= 0.0 && temp <= 1.0) thread.SURF2FRAC = temp;
			else
			{
				MessageBox(hDlg, L"Wrong SURF2FRAC value", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}

			temp = GetDlgItemReal(hDlg, IDC_EDIT10);
			if (temp >= -10.0 && temp < 10.0) thread.LBR[0] = (int)temp;
			else
			{
				MessageBox(hDlg, L"Wrong L BRAG value", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}

			tmpi = GetDlgItemInt(hDlg, IDC_EDIT11, 0, 0);
			if (tmpi >= 0 && tmpi < 100) thread.NLAYERS = tmpi;
			else
			{
				MessageBox(hDlg, L"Wrong NLAYERS value", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}

			tmpi = GetDlgItemInt(hDlg, IDC_EDIT7, 0, 0);
			if (tmpi >= 0 && tmpi <= variable.NSURFTOT)
			{
				thread.NSURF2 = tmpi;
				thread.NSURF = variable.NSURFTOT - tmpi;
			}
			else
			{
				MessageBox(hDlg, L"Wrong NSURF2 value", NULL, MB_OK);
				return (INT_PTR)FALSE;
			}

			thread.ROUGHMODEL = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);

			thread.NumberOfCores = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0) + 1;


			//Calculate fractional coordinate
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) thread.ZEROFRACT = true;
			else thread.ZEROFRACT = false;

			//Calculate fractional coordinate for bulk
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED) thread.BULK_ZEROFRACT = true;
			else thread.BULK_ZEROFRACT = false;

			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_GETCHECK, 0, 0) == BST_CHECKED) thread.STRUCFAC = true;
			else thread.STRUCFAC = false;

			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_GETCHECK, 0, 0) == BST_CHECKED) variable.LSQ_ERRORCALC = CHISQR;
			else variable.LSQ_ERRORCALC = COVARIANCE;

			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_GETCHECK, 0, 0) == BST_CHECKED) thread.LIQUID_PROFILE = true;
			else thread.LIQUID_PROFILE = false;

			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK6), BM_GETCHECK, 0, 0) == BST_CHECKED) thread.COHERENTDOMAINS = true;
			else thread.COHERENTDOMAINS = false;

			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK11), BM_GETCHECK, 0, 0) == BST_CHECKED) thread.INCOMENSURATE = true;
			else thread.INCOMENSURATE = false;



			LeaveCriticalSection(&myScene.CriticalSection);

			//Priority
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0) == 0) thread.priority = THREAD_PRIORITY_LOWEST;
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0) == 1) thread.priority = THREAD_PRIORITY_BELOW_NORMAL;
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0) == 2) thread.priority = THREAD_PRIORITY_NORMAL;
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0) == 3) thread.priority = THREAD_PRIORITY_ABOVE_NORMAL;
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO3), CB_GETCURSEL, 0, 0) == 4) thread.priority = THREAD_PRIORITY_HIGHEST;

			//Error
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_GETCURSEL, 0, 0) == 0) variable.LSQ_ERRORCALC = CHISQR;
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_GETCURSEL, 0, 0) == 1) variable.LSQ_ERRORCALC = COVARIANCE;

			//Thin layer
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO5), CB_GETCURSEL, 0, 0) == 0) variable.NFACTOR = 0;
			if ((int)SendMessage(GetDlgItem(hDlg, IDC_COMBO5), CB_GETCURSEL, 0, 0) == 1) variable.NFACTOR = 3;


			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Settings");

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for Settings3D box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Settings3D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 int tmpi;
 static COLORREF tmpRGBbackground, tmpRGBlight, tmpRGBtext;
					
 static int display_mode;				
 static HWND Hslider1;
 static HWND Hslider2;
 static HWND Hslider3;
 static HWND Hslider4;//text size
 static HWND Hslider5;

 static int Scroll1Value, Scroll2Value, Scroll3Value, Scroll4Value, Scroll5Value;
 int selected;

 //EnterCriticalSection(&myScene.CriticalSection);

	switch (message){
		case WM_INITDIALOG:
			
			WCHAR Tstr[5];
			
			for (int i = 0; i < myScene.g_msaaTotalModes+1; i++)
			{
				if (i==0)
					swprintf_s(Tstr, 5, L"None");
				else
					swprintf_s(Tstr, 5, L"%dx", (int)myScene.g_msaaSamples[i]);
				SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Tstr);
			}
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, (WPARAM)myScene.g_msaaCurentModes, 0);
			

			tmpRGBbackground = myScene.flag.rgbCurrent;
			tmpRGBlight = myScene.flag.rgbLight;
			tmpRGBtext = myScene.flag.rgbText;

			if(myScene.flag.g_texture == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);
			if(myScene.flag.g_planeOnOff == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_SETCHECK, BST_CHECKED, 0);
			if(myScene.flag.g_transparency == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_SETCHECK, BST_CHECKED, 0);
			if(myScene.flag.g_spring == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_SETCHECK, BST_CHECKED, 0);
			if (myScene.flag.g_light_follows_cam == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK7), BM_SETCHECK, BST_CHECKED, 0);
			if (myScene.g_enableVerticalSync == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK8), BM_SETCHECK, BST_CHECKED, 0);

			if (myScene.flag.g_individual_color == true) SendMessage(GetDlgItem(hDlg, IDC_INDYVIDUAL_COLOR), BM_SETCHECK, BST_CHECKED, 0);
			
			//Check if something is selected
			selected = 0;
			for (int i = 0; i<myScene.nr_bulk_atoms_toplot; i++)
			{
				if (myScene.D3DbulkAtom[i].Selected) selected++;
			}
			for (int i = 0; i<myScene.nr_surf_atoms_toplot; i++)
			{
				if (myScene.D3DsurfAtom[i].Selected) selected++;
			}

			if (selected == 0 && myScene.nr_bulk_atoms_toplot>0)
			{
				display_mode = myScene.D3DbulkAtom[1].display_mode;
			}

			//if(display_mode == 0 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
			//if(display_mode == 1 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, BST_CHECKED, 0);
			//if(display_mode == 2 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO3), BM_SETCHECK, BST_CHECKED, 0);
			//if(display_mode == 3 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO4), BM_SETCHECK, BST_CHECKED, 0);

			//Bulk repetitions
			SetDlgItemInt(hDlg, IDC_EDIT1, myScene.g_BRepetitionX, 1);
			SetDlgItemInt(hDlg, IDC_EDIT2, myScene.g_BRepetitionY, 1);
			SetDlgItemInt(hDlg, IDC_EDIT9, myScene.g_BRepetitionZ, 0);

			//Surface repetitions
			SetDlgItemInt(hDlg, IDC_EDIT11, myScene.g_SRepetitionX, 1);
			SetDlgItemInt(hDlg, IDC_EDIT12, myScene.g_SRepetitionY, 1);
			SetDlgItemInt (hDlg, IDC_EDIT3, (int)(myScene.flag.DWscale*PIPI16),  0);

			Hslider1 = GetDlgItem(hDlg,IDC_SLIDER1);
			Hslider2 = GetDlgItem(hDlg,IDC_SLIDER2);
			Hslider3 = GetDlgItem(hDlg, IDC_SLIDER8);
			Hslider4 = GetDlgItem(hDlg, IDC_TEXT_SIZE);
			Hslider5 = GetDlgItem(hDlg, IDC_CHARGE_SCALE);
			
			Scroll1Value = (int)(myScene.flag.BallSize*200);
			Scroll2Value = (int)(myScene.flag.RodThickness*200);
			Scroll3Value = (int)(myScene.flag.g_specular);
			Scroll4Value = (int)(myScene.flag.g_text_size);
			Scroll5Value = (int)(myScene.flag.g_scale_color*50);

			SendMessage(Hslider1,TBM_SETRANGE, (WPARAM)1,(LPARAM)MAKELONG(0,100));
            SendMessage(Hslider2,TBM_SETRANGE, (WPARAM)1,(LPARAM)MAKELONG(0,100));
			SendMessage(Hslider3, TBM_SETRANGE, (WPARAM)1, (LPARAM)MAKELONG(0, 100));
			SendMessage(Hslider4, TBM_SETRANGE, (WPARAM)1, (LPARAM)MAKELONG(1, 100));
			SendMessage(Hslider5, TBM_SETRANGE, (WPARAM)1, (LPARAM)MAKELONG(0, 100));

			SendMessage(Hslider1,TBM_SETPOS, (WPARAM)TRUE, (int)Scroll1Value);
			SendMessage(Hslider2,TBM_SETPOS, (WPARAM)TRUE, (int)Scroll2Value);
			SendMessage(Hslider3, TBM_SETPOS, (WPARAM)TRUE, (int)Scroll3Value);
			SendMessage(Hslider4, TBM_SETPOS, (WPARAM)TRUE, (int)Scroll4Value);
			SendMessage(Hslider5, TBM_SETPOS, (WPARAM)TRUE, (int)Scroll5Value);

			SetDlgItemInt (hDlg, IDC_EDIT4, (int)Scroll1Value,  0);
			SetDlgItemInt (hDlg, IDC_EDIT5, (int)Scroll2Value,  0);
			SetDlgItemInt(hDlg, IDC_EDIT10, (int)Scroll3Value, 0);
			SetDlgItemInt(hDlg, IDC_EDIT18, (int)Scroll4Value, 0);
			SetDlgItemInt(hDlg, IDC_EDIT17, (int)Scroll5Value, 0);
		return (INT_PTR)TRUE;

		case WM_HSCROLL:
			{
			HWND hwndScrollBar = (HWND) lParam;
			if( hwndScrollBar==Hslider1){
				Scroll1Value = (int)SendMessage(Hslider1, TBM_GETPOS, 0, 0);
				SetDlgItemInt (hDlg, IDC_EDIT4, Scroll1Value,  0);
				if (LOWORD(wParam)==SB_THUMBPOSITION){
					Scroll1Value = (int)SendMessage(Hslider1, TBM_GETPOS, 0, 0);
					SetDlgItemInt (hDlg, IDC_EDIT4, Scroll1Value,  0);
				}
				return (INT_PTR)TRUE;
			}
			if( hwndScrollBar==Hslider2){
				Scroll2Value = (int)SendMessage(Hslider2, TBM_GETPOS, 0, 0);
				SetDlgItemInt (hDlg, IDC_EDIT5, Scroll2Value,  0);
				if (LOWORD(wParam)==SB_THUMBPOSITION){
					Scroll2Value = (int)SendMessage(Hslider2, TBM_GETPOS, 0, 0);
					SetDlgItemInt (hDlg, IDC_EDIT5, Scroll2Value,  0);
				}
				return (INT_PTR)TRUE;
			}
			if (hwndScrollBar == Hslider3){
				Scroll3Value = (int)SendMessage(Hslider3, TBM_GETPOS, 0, 0);
				SetDlgItemInt(hDlg, IDC_EDIT10, Scroll3Value, 0);
				if (LOWORD(wParam) == SB_THUMBPOSITION){
					Scroll3Value = (int)SendMessage(Hslider3, TBM_GETPOS, 0, 0);
					SetDlgItemInt(hDlg, IDC_EDIT10, Scroll3Value, 0);
				}
				return (INT_PTR)TRUE;
			}

			if (hwndScrollBar == Hslider4) {
				Scroll4Value = (int)SendMessage(Hslider4, TBM_GETPOS, 0, 0);
				SetDlgItemInt(hDlg, IDC_EDIT18, Scroll4Value, 0);
				if (LOWORD(wParam) == SB_THUMBPOSITION) {
					Scroll4Value = (int)SendMessage(Hslider4, TBM_GETPOS, 0, 0);
					SetDlgItemInt(hDlg, IDC_EDIT18, Scroll4Value, 0);
				}
				return (INT_PTR)TRUE;
			}

			if (hwndScrollBar == Hslider5) {
				Scroll5Value = (int)SendMessage(Hslider5, TBM_GETPOS, 0, 0);
				SetDlgItemInt(hDlg, IDC_EDIT17, Scroll5Value, 0);
				if (LOWORD(wParam) == SB_THUMBPOSITION) {
					Scroll5Value = (int)SendMessage(Hslider5, TBM_GETPOS, 0, 0);
					SetDlgItemInt(hDlg, IDC_EDIT17, Scroll5Value, 0);
				}
				return (INT_PTR)TRUE;
			}

			}
			break;
			//return (INT_PTR)TRUE;

		case WM_CTLCOLORSTATIC:
			{
			HWND hCtl =( HWND ) lParam;
		
			if (hCtl == GetDlgItem(hDlg, IDC_BACKGROUND)){
				return  (ULONG_PTR)CreateSolidBrush(tmpRGBbackground);
			}
			if (hCtl == GetDlgItem(hDlg, IDC_LIGHT_COLOR)){
				return (ULONG_PTR)CreateSolidBrush(tmpRGBlight);
			}
			if (hCtl == GetDlgItem(hDlg, IDC_TEXT_COLOR)){
				return (ULONG_PTR)CreateSolidBrush(tmpRGBtext);
			}
			}
		break;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDOK)
			{	
	
				//Antyaliasing
				myScene.g_msaaCurentModes = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);


				//Sliders
				myScene.flag.BallSize =     0.005f * SendMessage(Hslider1,TBM_GETPOS, 0, 0);
				myScene.flag.RodThickness = 0.005f * SendMessage(Hslider2,TBM_GETPOS, 0, 0);
				myScene.flag.g_specular =  1.f * SendMessage(Hslider3, TBM_GETPOS, 0, 0);
				myScene.flag.g_text_size = (int) SendMessage(Hslider4, TBM_GETPOS, 0, 0);
				myScene.flag.g_scale_color = 1.f*SendMessage(Hslider5, TBM_GETPOS, 0, 0) / 50;
				
				//font size
				SAFE_RELEASE(myScene.g_pFont);
				if (!myScene.InitFont(L"Arial", myScene.flag.g_text_size, myScene.g_pFont))
					MessageBox(NULL, TEXT("Failed to create font."), NULL, MB_ICONERROR);

				if(SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED) myScene.flag.g_planeOnOff = true;
				else myScene.flag.g_planeOnOff = false;
				if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) myScene.flag.g_texture = true;
				else myScene.flag.g_texture = false;
				if(SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_GETCHECK, 0, 0) == BST_CHECKED) myScene.flag.g_transparency = true;
				else myScene.flag.g_transparency = false;
				if(SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_GETCHECK, 0, 0) == BST_CHECKED) myScene.flag.g_spring = true;
				else myScene.flag.g_spring = false;
				if (SendMessage(GetDlgItem(hDlg, IDC_CHECK7), BM_GETCHECK, 0, 0) == BST_CHECKED) myScene.flag.g_light_follows_cam = true;
				else myScene.flag.g_light_follows_cam = false;
				if (SendMessage(GetDlgItem(hDlg, IDC_CHECK8), BM_GETCHECK, 0, 0) == BST_CHECKED) myScene.g_enableVerticalSync = true;
				else myScene.g_enableVerticalSync = false;
				if (SendMessage(GetDlgItem(hDlg, IDC_INDYVIDUAL_COLOR), BM_GETCHECK, 0, 0) == BST_CHECKED) myScene.flag.g_individual_color = true;
				else myScene.flag.g_individual_color = false;


				

				//if(SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 0 ;
				//if(SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 1 ;
				//if(SendMessage(GetDlgItem(hDlg, IDC_RADIO3), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 2 ;
				//if(SendMessage(GetDlgItem(hDlg, IDC_RADIO4), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 3 ;


				
				myScene.flag.display_mode = display_mode;

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT1, 0, 1);
				if (tmpi >= -100 && tmpi<100) myScene.g_BRepetitionX = tmpi;
				else
				{
					MessageBox(hDlg, L"Wrong NLAYERS value in X direction", NULL, MB_OK);
					return (INT_PTR)FALSE;
				}

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT2, 0, 1);
				if (tmpi >= -100 && tmpi<100) myScene.g_BRepetitionY = tmpi;
				else
				{
					MessageBox(hDlg, L"Wrong NLAYERS value in Y direction", NULL, MB_OK);
					return (INT_PTR)FALSE;
				}

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT11, 0, 1);
				if (tmpi >= -100 && tmpi<100) myScene.g_SRepetitionX = tmpi;
				else
				{
					MessageBox(hDlg, L"Wrong NLAYERS value in X direction", NULL, MB_OK);
					return (INT_PTR)FALSE;
				}

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT12, 0, 1);
				if (tmpi >= -100 && tmpi<100) myScene.g_SRepetitionY = tmpi;
				else
				{
					MessageBox(hDlg, L"Wrong NLAYERS value in Y direction", NULL, MB_OK);
					return (INT_PTR)FALSE;
				}

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT9, 0, 0);
				if (tmpi >= 0 && tmpi<100) myScene.g_BRepetitionZ = tmpi;
				else
				{
					MessageBox(hDlg, L"Wrong NLAYERS value in Z direction ", NULL, MB_OK);
					return (INT_PTR)FALSE;
				}

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT3, 0, 0);
				if(tmpi>=0 && tmpi<100) myScene.flag.DWscale = (double) tmpi/PIPI16;
				else
				{
					MessageBox(hDlg, L"Wrong %propability value", NULL, MB_OK);
					return (INT_PTR)FALSE;
				}

				//----------------------------
				myScene.flag.rgbCurrent	= tmpRGBbackground;
				myScene.flag.rgbLight = tmpRGBlight;
				myScene.flag.rgbText = tmpRGBtext;

				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			if (LOWORD(wParam) == IDCANCEL)
			{	
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}


			if (LOWORD(wParam) == IDC_BACKGROUND)
			{
				ColorSet(hDlg, IDC_BACKGROUND, &tmpRGBbackground);
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDC_LIGHT_COLOR)
			{
				ColorSet(hDlg, IDC_LIGHT_COLOR, &tmpRGBlight);
			return (INT_PTR)TRUE;
			}


			if (LOWORD(wParam) == IDC_TEXT_COLOR)
			{
				ColorSet(hDlg, IDC_TEXT_COLOR, &tmpRGBtext);
			return (INT_PTR)TRUE;
			}
		
		break;
	}

//	LeaveCriticalSection(&myScene.CriticalSection);
 return (INT_PTR)FALSE;
}


void ColorSet(HWND hDlg, int IDC_PictureBox, COLORREF *color)
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

//-----------------------------------------------------------------------------
// Functions: Repleasment for standard Callback Edit procedure. This is to get information about keys
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			//Do your stuff
			break;  //or return 0; if you don't want to pass it further to def proc

		case VK_UP:
			nItem--;
			if (nItem < 0) nItem = 0;
			KeyMoveEdit(wnd, hItemList, nItem, nSubItem);
			return 0;

		case VK_DOWN:
			nItem++;	
			if (nItem > ListView_GetItemCount(hItemList)-1) nItem = ListView_GetItemCount(hItemList)-1;
			KeyMoveEdit(wnd, hItemList, nItem, nSubItem);
		return 0;

		case VK_LEFT:
			nSubItem--;
			if (nSubItem < LeftListLimit) nSubItem = LeftListLimit;
			KeyMoveEdit(wnd, hItemList, nItem, nSubItem);
			return 0;

		case VK_RIGHT:
			nSubItem++;
			if (nSubItem > RightListLimit) nSubItem = RightListLimit;
			KeyMoveEdit(wnd, hItemList, nItem, nSubItem);
			return 0;

		}

	default://If not your key, skip to default:
		return CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Functions: Repleasment for standard Callback Edit procedure. This is to get information about keys
//02.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------
LRESULT CALLBACK subEditProc2(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			//Do your stuff
			break;  //or return 0; if you don't want to pass it further to def proc

		case VK_UP:
			nItem2--;
			if (nItem2 < 0) nItem2 = 0;
			KeyMoveEdit(wnd, hItemList3, nItem2, nSubItem2);
			return 0;

		case VK_DOWN:
			nItem2++;
			if (nItem2 > ListView_GetItemCount(hItemList3) - 1) nItem2 = ListView_GetItemCount(hItemList3) - 1;
			KeyMoveEdit(wnd, hItemList3, nItem2, nSubItem2);
			return 0;

		case VK_LEFT:
			nSubItem2--;
			if (nSubItem2 < LeftListLimit2) nSubItem2 = LeftListLimit2;
			KeyMoveEdit(wnd, hItemList3, nItem2, nSubItem2);
			return 0;

		case VK_RIGHT:
			nSubItem2++;
			if (nSubItem2 > RightListLimit2) nSubItem2 = RightListLimit2;
			KeyMoveEdit(wnd, hItemList3, nItem2, nSubItem2);
			return 0;

		}

	default://If not your key, skip to default:
		return CallWindowProc(oldEditProc2, wnd, msg, wParam, lParam);
	}
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------
// Updating Edit1 according to position on the List item
//Created by D.K.
//04.07.2014
//--------------------------------------------------------------------------------------------------------------------------------
void KeyMoveEdit(HWND hEdit, HWND ListEdit, int nIt, int nSub){

	RECT rect, rect1, rect2,rect3;
	// this macro is used to retrieve the Rectanle  of the selected SubItem
	ListView_GetSubItemRect(ListEdit, nIt, nSub, LVIR_BOUNDS, &rect);
	//Get the Rectange of the listControl
	GetWindowRect(ListEdit, &rect1);
	GetClientRect(ListEdit, &rect3);

	int delta = rect1.bottom - rect1.top - (rect3.bottom - rect3.top); //correction for appeared x slider


	//Get the Rectange of the Dialog
	GetWindowRect(GetParent(hEdit), &rect2);
	int height = rect.bottom - rect.top;

	if (rect.bottom > rect1.bottom - rect1.top - delta)
	{
		ListView_Scroll(ListEdit, 0, height);
		ListView_GetSubItemRect(ListEdit, nIt, nSub, LVIR_BOUNDS, &rect);
	}

	else if (rect.top < height)
	{
		ListView_Scroll(ListEdit, 0, -height);
		ListView_GetSubItemRect(ListEdit, nIt, nSub, LVIR_BOUNDS, &rect);
	}

	int x = rect1.left - rect2.left - 4;
	int y = rect1.top - rect2.top - 29;

	if (nItem != -1)
		SetWindowPos(hEdit, HWND_TOP, rect.left + x, rect.top + y, rect.right - rect.left - 2, rect.bottom - rect.top - 1, SWP_DRAWFRAME);

	WCHAR Wstr[128];
	GetItemText(ListEdit, Wstr, sizeof(Wstr), nIt, nSub);
	SetWindowText(hEdit, Wstr);
	SendMessage(hEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
	InvalidateRect(ListEdit, 0, 0);

	ShowWindow(hEdit, SW_SHOW);
	SetFocus(hEdit);


}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box. 
//Created by D.K.
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Fitting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	HWND  hItemEdit;
	LeftListLimit = 2;
	RightListLimit = 4;

	int j;
	bool AreSelectedItems;
	static int ntot;
	static RECT rect;

	//list
	WCHAR pszNr[8];
	WCHAR pszParameter[50];
	WCHAR pszLowLimit[20];
	WCHAR pszValue[20];
	WCHAR pszUplimit[20];


	LVCOLUMN		colNr;
	LVCOLUMN		colParam;
	LVCOLUMN		colLoLimit;
	LVCOLUMN		colValue;
	LVCOLUMN		colUpLimit;
	LVCOLUMN		colMessage;

	// bool *ValueStatus; //To informwhich values are not corect in table min<value<max;
	static bool  ValueStatus[100];
	LVITEM			lvTest;
	UINT			iIndex = 0;
	static HWND* hProgressBar;
	static HWND* hText;


	hItemList = GetDlgItem(hDlg, IDC_LIST1);
	hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);

	switch (message){

	case WM_SIZE:
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 95, rect.bottom - rect.top - 43, TRUE);

		MoveButton(hDlg, IDC_CALL, rect, 10, 170);
		MoveButton(hDlg, IDC_AUTO_LIMITS, rect, 10, 140);
		MoveButton(hDlg, ID_ASA, rect, 10, 110);
		MoveButton(hDlg, ID_LEV, rect, 10, 80);
		MoveButton(hDlg, IDCANCEL, rect, 10, 50);
		MoveButton(hDlg, IDOK, rect, 10, 20);

		MoveWindow(GetDlgItem(hDlg, IDC_STATIC), rect.left, rect.bottom - 30, 150, 20, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDC_PROGRESS1), rect.left, rect.bottom - 10, rect.right - rect.left, 10, TRUE);

		ShowWindow(hItemEdit, SW_HIDE);
		break;


	case WM_INITDIALOG:
	{
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 95, rect.bottom - rect.top - 43, TRUE);

		MoveButton(hDlg, IDC_CALL, rect, 10, 170);
		MoveButton(hDlg, IDC_AUTO_LIMITS, rect, 10, 140);
		MoveButton(hDlg, ID_ASA, rect, 10, 110);
		MoveButton(hDlg, ID_LEV, rect, 10, 80);
		MoveButton(hDlg, IDCANCEL, rect, 10, 50);
		MoveButton(hDlg, IDOK, rect, 10, 20);


		MoveWindow(GetDlgItem(hDlg, IDC_STATIC), rect.left, rect.bottom - 30, 150, 20, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDC_PROGRESS1), rect.left, rect.bottom - 10, rect.right - rect.left, 20, TRUE);
		InvalidateRect(GetDlgItem(hDlg, IDC_PROGRESS1), 0, 0);
		//Repleasing the standard mesaage handling of EditBoX with the new one important for substracting mesages related to keys
		oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);

		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);

		ntot = NSF +  variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT + variable.NOCCTOT2  + variable.NSUBSCALETOT;
		if (variable.NFACTOR > 0)
			ntot += variable.NFACTOR;

		if (variable.MODE == 1)
			ntot += variable.ROTATIONTOTX + variable.ROTATIONTOTY + variable.ROTATIONTOTX;
		if (variable.DOMOCCUPAUTO)
			ntot += thread.NDOMAIN ;//we dont fit the first domain occupation this is why is -1
		if (thread.NSURF2 > 0)
			ntot++;

		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);//

		HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		hProgressBar = new HWND[ntot * 2 + 2]; //The last one (+2 not +1) is for accepted states progress bar.
		hText = new HWND[ntot + 1];
		//ValueStatus  = new bool[ntot+1];


		int shift = 0;
		for (j = 0; j < ntot; ++j){

			shift += 12;

			hText[j] = CreateWindowEx(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE   |WS_DISABLED , shift, 20, 22, 20, hDlg, NULL, hInstance, NULL);
			swprintf_s(pszNr, L"%d", j);
			SetWindowText(hText[j], pszNr);
			ShowWindow(hText[j], SW_HIDE);
			//values
			hProgressBar[j * 2] = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_VERTICAL | PBS_SMOOTHREVERSE | PBS_SMOOTH,
				shift, 50, 8, 100, hDlg, (HMENU)150, hInstance, NULL);
			ShowWindow(hProgressBar[j * 2], SW_HIDE);
			//value temperature
			//shift += 12;
			hProgressBar[j * 2 + 1] = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_VERTICAL | PBS_SMOOTHREVERSE | PBS_SMOOTH,
				shift, 160, 8, 50, hDlg, (HMENU)200, hInstance, NULL);
			ShowWindow(hProgressBar[j * 2 + 1], SW_HIDE);

		}
		hProgressBar[ntot*2 + 1] = GetDlgItem(hDlg, IDC_PROGRESS1); //The last proggres bar for accepted states


		ShowWindow(hItemEdit, SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		//Create the columns in the list control
		//Name
		colNr.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr.pszText = TEXT("fix/free Nr");
		colNr.cchTextMax = 100;
		colNr.cx = 65;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 0, &colNr);

		//fix/free Parameter
		colParam.mask = LVCF_TEXT | LVCF_WIDTH;
		colParam.pszText = TEXT("Parameter");
		colParam.cchTextMax = 20;
		colParam.cx = 120;
		colParam.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colParam);

		//Low limit
		colLoLimit.mask = LVCF_TEXT | LVCF_WIDTH;
		colLoLimit.pszText = TEXT("Low limit");
		colLoLimit.cchTextMax = 90;
		colLoLimit.cx = 70;
		colLoLimit.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 2, &colLoLimit);

		//Value
		colValue.mask = LVCF_TEXT | LVCF_WIDTH;
		colValue.pszText = TEXT("Value");
		colValue.cchTextMax = 90;
		colValue.cx = 70;
		colValue.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 3, &colValue);

		//Up limit
		colUpLimit.mask = LVCF_TEXT | LVCF_WIDTH;
		colUpLimit.pszText = TEXT("Up limit");
		colUpLimit.cchTextMax = 90;
		colUpLimit.cx = 70;
		colUpLimit.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 4, &colUpLimit);

		//message
		colMessage.mask = LVCF_TEXT | LVCF_WIDTH;
		colMessage.pszText = TEXT("message");
		colMessage.cchTextMax = 160;
		colMessage.cx = 150;
		colMessage.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 5, &colMessage);



		//plot in table
		lvTest.mask = LVIF_TEXT | LVIF_STATE;
		lvTest.iItem = 0;
		lvTest.pszText = pszNr;
		lvTest.iIndent = 0;
		lvTest.stateMask = 0;
		lvTest.state = 0;
		lvTest.iSubItem = 0;

		int i = ntot + 1;


		//Domain occupancy
		if (variable.DOMOCCUPAUTO)
		{
			for (j = thread.NDOMAIN - 1; j >= 0; j--){
				i--;
				swprintf_s(pszNr, 100, L"%4d", i);
				swprintf_s(pszParameter, 100, L"Domain occ. %d", j + 1);
				swprintf_s(pszLowLimit, 100, L"%6.4f", variable.DOMOCCUPLIM[j][0]);
				swprintf_s(pszValue, 100, L"%6.4f", thread.DOMOCCUP[j]);
				swprintf_s(pszUplimit, 100, L"%6.4f", variable.DOMOCCUPLIM[j][1]);
				if (variable.DOMOCCUPPEN[j])	ListView_SetCheckState(hItemList, i-1, 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
				ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
				ListView_SetItemText(hItemList, iIndex, 3, pszValue);
				ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
			}
		}
		//Rotation
		if (variable.MODE == 1)
		{
			for (j = variable.ROTATIONTOTZ - 1; j >= 0; j--){
				i--;
				swprintf_s(pszNr, 100, L"%4d", i);
				swprintf_s(pszParameter, 100, L"Rotation Z %d", j + 1);
				swprintf_s(pszLowLimit, 100, L"%6.4f", variable.ROTATIONLIMZ[j][0]);
				swprintf_s(pszValue, 100, L"%6.4f", variable.ROTATIONZ[j]);
				swprintf_s(pszUplimit, 100, L"%6.4f", variable.ROTATIONLIMZ[j][1]);
				if (variable.ROTATIONPENZ[j])	ListView_SetCheckState(hItemList, i - 1, 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
				ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
				ListView_SetItemText(hItemList, iIndex, 3, pszValue);
				ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
			}

			for (j = variable.ROTATIONTOTY - 1; j >= 0; j--){
				i--;
				swprintf_s(pszNr, 100, L"%4d", i);
				swprintf_s(pszParameter, 100, L"Rotation Y %d", j + 1);
				swprintf_s(pszLowLimit, 100, L"%6.4f", variable.ROTATIONLIMY[j][0]);
				swprintf_s(pszValue, 100, L"%6.4f", variable.ROTATIONY[j]);
				swprintf_s(pszUplimit, 100, L"%6.4f", variable.ROTATIONLIMY[j][1]);
				if (variable.ROTATIONPENY[j])	ListView_SetCheckState(hItemList, i - 1, 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
				ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
				ListView_SetItemText(hItemList, iIndex, 3, pszValue);
				ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
			}


			for (j = variable.ROTATIONTOTX - 1; j >= 0; --j){
				i--;
				swprintf_s(pszNr, 100, L"%4d", i);
				swprintf_s(pszParameter, 100, L"Rotation X %d", j + 1);
				swprintf_s(pszLowLimit, 100, L"%6.4f", variable.ROTATIONLIMX[j][0]);
				swprintf_s(pszValue, 100, L"%6.4f", variable.ROTATIONX[j]);
				swprintf_s(pszUplimit, 100, L"%6.4f", variable.ROTATIONLIMX[j][1]);
				if (variable.ROTATIONPENX[j])	ListView_SetCheckState(hItemList, i - 1, 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
				ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
				ListView_SetItemText(hItemList, iIndex, 3, pszValue);
				ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
			}

		}


		//FACTOR
		if (variable.NFACTOR > 0)
		for (j = variable.NFACTOR - 1; j >= 0; --j) {
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Factor %d", j + 1);
			swprintf_s(pszLowLimit, 100, L"%6.4f", variable.FACTOR_LIM[j][0]);
			swprintf_s(pszValue, 100, L"%6.4f", thread.FACTOR[j]);
			swprintf_s(pszUplimit, 100, L"%6.4f", variable.FACTOR_LIM[j][1]);
			if (variable.FACTOR_PEN[j])	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}

		//Occupancy2
		for (j = variable.NOCCTOT2 - 1; j >= 0; --j) {
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Occupancy2 %d", j + 1);
			swprintf_s(pszLowLimit, 100, L"%6.4f", variable.OCCUP2LIM[j][0]);
			swprintf_s(pszValue, 100, L"%6.4f", thread.OCCUP2[j]);
			swprintf_s(pszUplimit, 100, L"%6.4f", variable.OCCUP2LIM[j][1]);
			if (variable.OCCUP2PEN[j])	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}

		//Occupation
		for (j = variable.NOCCTOT - 1; j >= 0; --j){
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Occupancy %d", j + 1);
			swprintf_s(pszLowLimit, 100, L"%6.4f", variable.OCCUPLIM[j][0]);
			swprintf_s(pszValue, 100, L"%6.4f", thread.OCCUP[j]);
			swprintf_s(pszUplimit, 100, L"%6.4f", variable.OCCUPLIM[j][1]);
			if (variable.OCCUPPEN[j])	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}

		//Debye Wallers2
		for (j = thread.NDWTOT2 - 1; j >= 0; --j){
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Debyw-Waller2 %d", j + 1);
			swprintf_s(pszLowLimit, 100, L"%6.4f", variable.DEBWAL2LIM[j][0]);
			swprintf_s(pszValue, 100, L"%6.4f", thread.DEBWAL2[j]);
			swprintf_s(pszUplimit, 100, L"%6.4f", variable.DEBWAL2LIM[j][1]);
			if (variable.DEBWAL2PEN[j])	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}

		//Debye Wallers
		for (j = thread.NDWTOT - 1; j >= 0; --j){
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Debye-Waller %d", j + 1);
			swprintf_s(pszLowLimit, 100, L"%6.4f", variable.DEBWALLIM[j][0]);
			swprintf_s(pszValue, 100, L"%6.4f", thread.DEBWAL[j]);
			swprintf_s(pszUplimit, 100, L"%6.4f", variable.DEBWALLIM[j][1]);
			if (variable.DEBWALPEN[j])	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}

		//displacement
		for (j = variable.NDISTOT - 1; j >= 0; --j){
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Displacement %d", j + 1);
			swprintf_s(pszLowLimit, 100, L"%4.4f", variable.DISPLLIM[j][0]);
			swprintf_s(pszValue, 100, L"%4.4f", variable.DISPL[j]);
			swprintf_s(pszUplimit, 100, L"%4.4f", variable.DISPLLIM[j][1]);
			if (variable.DISPLPEN[j])	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}

		//surf. 2 fraction
		if (thread.NSURF2 > 0)
		{
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Surface 2 fraction");
			swprintf_s(pszLowLimit, 100, L"%6.4f", variable.SURF2FRACLIM[0]);
			swprintf_s(pszValue, 100, L"%6.4f", thread.SURF2FRAC);
			swprintf_s(pszUplimit, 100, L"%6.4f", variable.SURF2FRACLIM[1]);
			if (variable.SURF2FRACPEN)	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}
		//surf. fraction
		i--;
		swprintf_s(pszNr, 100, L"%4d", i);
		swprintf_s(pszParameter, 100, L"Surface fraction");
		swprintf_s(pszLowLimit, 100, L"%6.4f", variable.SURFFRACLIM[0]);
		swprintf_s(pszValue, 100, L"%6.4f", thread.SURFFRAC);
		swprintf_s(pszUplimit, 100, L"%6.4f", variable.SURFFRACLIM[1]);
		if (variable.SURFFRACPEN)	ListView_SetCheckState(hItemList, i - 1, 1);
		lvTest.pszText = pszNr;

		iIndex = ListView_InsertItem(hItemList, &lvTest);
		ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
		ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
		ListView_SetItemText(hItemList, iIndex, 3, pszValue);
		ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);

		//beta
		i--;
		swprintf_s(pszNr, 100, L"%4d", i);
		swprintf_s(pszParameter, 100, L"Beta");
		swprintf_s(pszLowLimit, 100, L"%6.4f", variable.BETALIM[0]);
		swprintf_s(pszValue, 100, L"%6.4f", thread.BETA);
		swprintf_s(pszUplimit, 100, L"%6.4f", variable.BETALIM[1]);
		lvTest.pszText = pszNr;


		iIndex = ListView_InsertItem(hItemList, &lvTest);
		ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
		ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
		ListView_SetItemText(hItemList, iIndex, 3, pszValue);
		ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);

		//Subscale
		for (j = variable.NSUBSCALETOT - 1; j >= 0; --j){
			i--;
			swprintf_s(pszNr, 100, L"%4d", i);
			swprintf_s(pszParameter, 100, L"Subscale %d", j + 1);
			swprintf_s(pszLowLimit, 100, L"%6.4f", variable.SUBSCALELIM[j][0]);
			swprintf_s(pszValue, 100, L"%6.4f", thread.SUBSCALE[j]);
			swprintf_s(pszUplimit, 100, L"%6.4f", variable.SUBSCALELIM[j][1]);
			if (variable.SUBSCALEPEN[j])	ListView_SetCheckState(hItemList, i - 1, 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
			ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
			ListView_SetItemText(hItemList, iIndex, 3, pszValue);
			ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);
		}



		i--;
		//SCALE
		swprintf_s(pszNr, 100, L"%4d", i);
		swprintf_s(pszParameter, 100, L"Scale");
		swprintf_s(pszLowLimit, 100, L"%6.4f", variable.SCALELIM[0]);
		swprintf_s(pszValue, 100, L"%6.4f", thread.SCALE);
		swprintf_s(pszUplimit, 100, L"%6.4f", variable.SCALELIM[1]);

		iIndex = ListView_InsertItem(hItemList, &lvTest);
		ListView_SetItemText(hItemList, iIndex, 1, pszParameter);
		ListView_SetItemText(hItemList, iIndex, 2, pszLowLimit);
		ListView_SetItemText(hItemList, iIndex, 3, pszValue);
		ListView_SetItemText(hItemList, iIndex, 4, pszUplimit);


		int TOTAL = 0;
		if (!variable.SCALEPEN){ ListView_SetCheckState(hItemList, TOTAL, FALSE); }
		else ListView_SetCheckState(hItemList, TOTAL, TRUE);

		TOTAL++;
		for (j = 0; j < variable.NSUBSCALETOT; j++){
			if (!variable.SUBSCALEPEN[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
			else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
		}
		TOTAL += variable.NSUBSCALETOT;
		if (!variable.BETAPEN){ ListView_SetCheckState(hItemList, TOTAL, FALSE); }
		else ListView_SetCheckState(hItemList, TOTAL, TRUE);

		TOTAL++;
		if (!variable.SURFFRACPEN){ ListView_SetCheckState(hItemList, TOTAL, FALSE); }
		else ListView_SetCheckState(hItemList, TOTAL, TRUE);

		if (thread.NSURF2 > 0)
		{
			TOTAL++;
			if (!variable.SURF2FRACPEN){ ListView_SetCheckState(hItemList, TOTAL, FALSE); }
			else ListView_SetCheckState(hItemList, TOTAL, TRUE);
		}

		TOTAL++;
		for (j = 0; j < variable.NDISTOT; j++){
			if (!variable.DISPLPEN[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
			else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
		}
		TOTAL += variable.NDISTOT;

		for (j = 0; j < thread.NDWTOT; j++){
			if (!variable.DEBWALPEN[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
			else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
		}

		TOTAL += thread.NDWTOT;
		for (j = 0; j < thread.NDWTOT2; j++){
			if (!variable.DEBWAL2PEN[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
			else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
		}

		TOTAL += thread.NDWTOT2;
		for (j = 0; j < variable.NOCCTOT; j++){
			if (!variable.OCCUPPEN[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
			else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
		}
	
			TOTAL += variable.NOCCTOT;
			for (j = 0; j < variable.NOCCTOT2; j++) {
				if (!variable.OCCUP2PEN[j]) { ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
				else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
			}
			TOTAL += variable.NOCCTOT2;

			if (variable.NFACTOR > 0)
			{
				for (j = 0; j < variable.NFACTOR; j++) {
					if (!variable.FACTOR_PEN[j]) { ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
					else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
				}
				TOTAL += variable.NFACTOR;
			}
//Rotation
			for (j = 0; j < variable.ROTATIONTOTX; j++){
				if (!variable.ROTATIONPENX[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
				else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
			}

			TOTAL += variable.ROTATIONTOTX;
			for (j = 0; j < variable.ROTATIONTOTY; j++){
				if (!variable.ROTATIONPENY[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
				else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
			}

			TOTAL += variable.ROTATIONTOTY;
			for (j = 0; j < variable.ROTATIONTOTZ; j++){
				if (!variable.ROTATIONPENZ[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
				else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
			}
	
		//Domain occ.
		if (variable.DOMOCCUPAUTO)
		{
			TOTAL += variable.ROTATIONTOTZ;
			for (j = 0; j < thread.NDOMAIN; j++)
			{
				if (!variable.DOMOCCUPPEN[j]){ ListView_SetCheckState(hItemList, TOTAL + j, FALSE); }
				else ListView_SetCheckState(hItemList, TOTAL + j, TRUE);
			}
		}

		CopyValuesFromListBox(hItemList, ValueStatus, &AreSelectedItems);
		CheckRange(hItemList);
	}
		return (INT_PTR)TRUE;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	case WM_NOTIFY:

	case IDC_LIST1:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;
		if (pnm->hdr.hwndFrom == hItemList && pnm->hdr.code == NM_CUSTOMDRAW)
		{
			CopyValuesFromListBox(hItemList, ValueStatus, &AreSelectedItems);//check the value status					
			SetWindowLong(hDlg, 0, (LONG)ProcessCustomDrawFitting(lParam, ValueStatus));
			return TRUE;
		}
	}

		switch (((NMHDR *)lParam)->code)
		{
		case NM_CLICK://IDC_LIST1
			//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				CheckRange(hItemList);
			}

			OnClickList(hDlg, hItemEdit, (NMHDR*)lParam, &nItem, &nSubItem, 2, 4);
			CopyValuesFromListBox(hItemList, ValueStatus, &AreSelectedItems);//check the value status
			//Make selction in Edit1
			SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
			if (nItem > ListView_GetItemCount(hItemList) || nSubItem < LeftListLimit || nSubItem > RightListLimit)
			{
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(hItemEdit, SW_HIDE);
			}
			break;
		}
		break;


	case WM_COMMAND:
		switch LOWORD(wParam){

		case IDC_EDIT1:

			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				CheckRange(hItemList);
			}
			break;

		case IDOK:
		{
			nItem = 0; nSubItem = 0;
			CopyValuesFromListBox(hItemList, ValueStatus, &AreSelectedItems);
			SendDlgItemMessage(hDlg, IDC_LIST1, WM_KILLFOCUS, 0, 0);
			for (j = 0; j < ntot; j++){
				if (ValueStatus[j] == false) return (INT_PTR)FALSE;
			}
			SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
			delete[] hProgressBar;
			delete[] hText;
			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Fit");
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
		{
			nItem = 0; nSubItem = 0;
			SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
			delete[] hProgressBar;
			delete[] hText;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDC_BUTTON1:
		{
			HWND pwndCtrl = GetFocus();
			if (pwndCtrl == GetDlgItem(hDlg, IDC_EDIT1)){
				//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
				if (nItem < ListView_GetItemCount(hItemList) && nSubItem>1 && nSubItem < 5){
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
					SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
					ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);

					InvalidateRect(hItemList, 0, 0);
				}
			}
		}
			break;

		case IDC_CALL:
			SetCALL(hItemList);
			CheckRange(hItemList);
			break;

		case IDC_AUTO_LIMITS:
		{
			SetAutoLimits(hItemList);
			CheckRange(hItemList);
		}
			break;

		case ID_LEV:
		{
			if (!CheckRange(hItemList))
				break;
			EnableWindow(GetDlgItem(hDlg, IDOK), false);
			EnableWindow(GetDlgItem(hDlg, IDCANCEL), false);
			EnableWindow(GetDlgItem(hDlg, ID_LEV), false);
			EnableWindow(GetDlgItem(hDlg, ID_ASA), false);

			if (thread.NDAT < 1) {
				MessageBox(hDlg, TEXT("No data read in!"), NULL, MB_ICONERROR);
				delete[] hProgressBar;
				delete[] hText;
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			CopyValuesFromListBox(hItemList, ValueStatus, &AreSelectedItems);
			SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
			ShowWindow(hItemEdit, SW_HIDE);
			nItem = 0; nSubItem = 0;
			for (j = 0; j < ntot; ++j){
				if (ValueStatus[j] == false)  return (INT_PTR)FALSE;
			}
			if (AreSelectedItems == false){
				MessageBox(hDlg, TEXT("Select at least one parametr for fittng"), NULL, MB_ICONERROR);
				break;
			}

			fit(LSQ_MRQ, 0);
			delete[] hProgressBar;
			delete[] hText;
			
			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Fitting Levenberg");
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
			break;

		case ID_ASA:
		{
			if (!CheckRange(hItemList))
				break;
			EnableWindow(GetDlgItem(hDlg, ID_LEV), false);
			EnableWindow(GetDlgItem(hDlg, ID_ASA), false);
			EnableWindow(GetDlgItem(hDlg, IDOK), false);
			EnableWindow(GetDlgItem(hDlg, IDCANCEL), false);
			nItem = 0; nSubItem = 0;

			if (thread.NDAT < 1)
			{
				MessageBox(hDlg, TEXT("No data read in!"), NULL, MB_ICONERROR);
				delete[] hProgressBar;
				delete[] hText;
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
			ShowWindow(hItemEdit, SW_HIDE);

			CopyValuesFromListBox(hItemList, ValueStatus, &AreSelectedItems);
			for (j = 0; j < ntot; ++j){
				if (ValueStatus[j] == false)
					return (INT_PTR)FALSE;
			}
			if (AreSelectedItems == false)
			{
				MessageBox(hDlg, TEXT("Select at least one parametr for fittng"), NULL, MB_ICONERROR);
				break;
			}

			ShowWindow(hItemList, SW_HIDE);

			for (j = 0; j < ntot; ++j)
			{
				ShowWindow(hText[j], SW_SHOW);
			}
	

			for (j = 0; j < ntot; ++j)
			{
				int value = (int)(abs(GetItemReal(hItemList, j, 3) / (GetItemReal(hItemList, j, 4) - GetItemReal(hItemList, j, 2)))) * 100;
				SendMessage(hProgressBar[j * 2], PBM_SETRANGE, 0, (LPARAM)MAKELPARAM(0, 100));
				SendMessage(hProgressBar[j * 2], PBM_SETPOS, (WPARAM)value, 0);
				SendMessage(hProgressBar[j * 2], PBM_SETBARCOLOR, (WPARAM)0, (LPARAM)(RGB(255, 0, 0)));//


				ShowWindow(hProgressBar[j * 2], SW_SHOW);

				SendMessage(hProgressBar[j * 2 + 1], PBM_SETRANGE, 0, (LPARAM)MAKELPARAM(0, 100));
				SendMessage(hProgressBar[j * 2 + 1], PBM_SETPOS, (WPARAM)0, 100);//
				ShowWindow(hProgressBar[j * 2 + 1], SW_SHOW);
			}

			//progress bar I 
			if (vfsr_par.LIMIT_ACCEPTANCES == 0) vfsr_par.LIMIT_ACCEPTANCES = _LIMIT_ACCEPTANCES/2;
			SendMessage(hProgressBar[ntot*2+1], PBM_SETRANGE, 0, (LPARAM)MAKELPARAM(0, vfsr_par.LIMIT_ACCEPTANCES/2));

			fit(LSQ_ASA, hProgressBar);
			//ShowWindow(hProgressBar, SW_HIDE);
			ShowWindow(hItemList, SW_SHOW);

			delete[] hProgressBar;
			delete[] hText;
			//delete[] ValueStatus;

			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Fitting Asa");

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
			break;

		}
		break;

	}
	return (INT_PTR)FALSE;
}

                                           
/*--------------------------------------------------------------------------------------------------------------------------------
 Modules:      SetDlgItemReal
 description:  SetReal value from dialog
 Created by:   Daniel Kaminski
 Modyfied by:  
 Data:         2007     
 Another:      Extension for API
/--------------------------------------------------------------------------------------------------------------------------------*/
void SetDlgItemReal(HWND hDlg, int nIDD, double Value, WCHAR *FORMAT){

	WCHAR temp_str[100];
	if (abs(Value) > 0.0000001)
		swprintf_s(temp_str, 100, FORMAT, Value);
	else
		if (Value == 0)
			swprintf_s(temp_str, 100, TEXT("%1.1f"), Value);
		else
			swprintf_s(temp_str, 100, TEXT("%5.2e"), Value);
	SetDlgItemText(hDlg, nIDD, temp_str);
}

/*--------------------------------------------------------------------------------------------------------------------------------
 Modules:      GetDlgItemReal
 description:  GetReal value from dialog
 Created by:   Daniel Kaminski
 Modyfied by:  
 Data:         2007     
 Another:      Extension for API
/--------------------------------------------------------------------------------------------------------------------------------*/
double GetDlgItemReal(HWND hDlg, int nIDD){

 float temp2;
 WCHAR temp_string[20];   //read line of text only 20 characters

	GetDlgItemText(hDlg, nIDD, temp_string, 20); //STRING_SIZE only 20 characters!
	if (temp_string[0] == L'-' && temp_string[1] == 0) temp_string[1] = L'0';
	if (temp_string[0] == L'-' && temp_string[1] == L'.' && temp_string[2] == 0){ temp_string[1] = L'0'; temp_string[2] = L'.'; }
	swscanf_s(temp_string, L"%f", &temp2 );

 return (double) temp2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function set the text in the specified SubItem depending on the Row and Column values
void SetCell(HWND hWnd1, double value, int nRow, int nCol, bool Int){

 WCHAR     szString [256];
	if(Int)
		swprintf_s(szString, L"%d", (int)value);
	else
		swprintf_s(szString, L"%7.4f", value);

    //Fill the LVITEM structure with the values given as parameters.
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = nRow;
    lvItem.pszText = szString;
    lvItem.iSubItem = nCol;
    if(nCol >0)
        //set the value of listItem
        SendMessage(hWnd1,LVM_SETITEM, (WPARAM)0,(WPARAM)&lvItem);
    else
        //Insert the value into List
        ListView_InsertItem(hWnd1,&lvItem);

}

void SetCell(HWND hWnd1, WCHAR* szString, int nRow, int nCol){

    //Fill the LVITEM structure with the values given as parameters.
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = nRow;
    lvItem.pszText = szString;
    lvItem.iSubItem = nCol;
    if(nCol >0)
        //set the value of listItem
        SendMessage(hWnd1,LVM_SETITEM, (WPARAM)0,(WPARAM)&lvItem);
    else
        //Insert the value into List
        ListView_InsertItem(hWnd1,&lvItem);
}

//this function will returns the item 
//text depending on the item and SubItem Index
void GetItemText(HWND hWnd, WCHAR *Wstr, int nLen, int nItem, int nSubItem)
{
    LVITEM lvi;
    memset(&lvi, 0, sizeof(LVITEM));
    lvi.iSubItem = nSubItem;
    lvi.cchTextMax = nLen;
    lvi.pszText = Wstr;
    int nRes  = (int)SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&lvi);

}

//this function will returns the item 
//text depending on the item and SubItem Index
double GetItemReal(HWND hWnd, int nItem, int nSubItem){
 
 float Value;
 WCHAR Wstr[128];
    LVITEM lvi;
    memset(&lvi, 0, sizeof(LVITEM));
    lvi.iSubItem = nSubItem;
    lvi.cchTextMax = 128;
    lvi.pszText = Wstr;
    int nRes  = (int)SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&lvi);
	if(swscanf_s(Wstr, L"%f", &Value)==0) return 0.;

return (double)Value;
}


//This function Displays an EditBox at the position where user clicks on a particular SubItem with 
//Rectangle are equal to the SubItem, thus allows to modify the value
void OnClickList(HWND hDlg, HWND hWndEditBox, NMHDR* pNMHDR, int *nItem, int *nSubItem, int SkipColumnLow, int SkipColumnUp ) 
{
//    InvalidateRect(hWndListBox, 0 ,0);

    LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE) pNMHDR;
    RECT rect;
    //get the row number
    *nItem = temp->iItem;
    //get the column number
    *nSubItem = temp->iSubItem;
	//Skip the second column this are names of parameters
    if(*nSubItem < SkipColumnLow || *nSubItem > SkipColumnUp ||  *nItem == -1)
		return ;

    RECT rect1,rect2;
    // this macro is used to retrieve the Rectanle  of the selected SubItem
    ListView_GetSubItemRect(temp->hdr.hwndFrom, temp->iItem, temp->iSubItem,LVIR_BOUNDS,&rect);

	
	//First approach
	//Get the Rectange of the listControl
	//GetWindowRect(temp->hdr.hwndFrom,&rect1);
	//Get the Rectange of the Dialog
	// GetWindowRect(hDlg, &rect2);

	//int x=rect1.left-rect2.left-5;
	//int y=rect1.top-rect2.top-25-5;
	
	//Second approach
	//Get the Rectange of the listControl
	GetClientRect(temp->hdr.hwndFrom, &rect1);
    //Get the Rectange of the Dialog
	GetClientRect(hDlg, &rect2);
    
	int x = rect1.left - rect2.left+3;
	int y = rect1.top - rect2.top+12;

    if(*nItem != -1) 
		SetWindowPos(hWndEditBox,HWND_TOP, rect.left+x,rect.top+y, rect.right-rect.left , rect.bottom-rect.top , SWP_DRAWFRAME);

    //Draw a Rectangle around the SubItem
//    Rectangle(GetDC(temp->hdr.hwndFrom), rect.left,rect.top-1,rect.right,rect.bottom);
    //Set the listItem text in the EditBox
	WCHAR Wstr[128];
	GetItemText(temp->hdr.hwndFrom, Wstr, sizeof(Wstr), *nItem, *nSubItem);
    SetWindowText(hWndEditBox, Wstr);
	InvalidateRect(temp->hdr.hwndFrom, 0 ,0);
	ShowWindow(hWndEditBox,SW_SHOW);
    SetFocus(hWndEditBox);
}

void CopyValuesFromFitEditListBox(HWND hItemList){

char Symbol[3];
WCHAR Wsymbol[3];

	for (int i = 0; i < thread.NTYPES; ++i){
			thread.TS[i] = 0;
	}
	//Reset completly
	thread.NTYPES = 0;

	//Copy bulk elements before reset
	char BulkSaveElement[MAX_ELEMENTS][3];
	for (int j = 0; j < thread.NBULK; ++j)
		CopyMemory(BulkSaveElement[j], variable.ELEMENT[thread.TB[j]], sizeof(char) * 3);

	//We have to check first bulk for different types
	for (int j = 0; j < thread.NBULK; ++j)
	{
		// Find the different element types and store them in ELEMENT 
		FindAtomTypes(BulkSaveElement[j]);

		for (int i = 0; i < thread.NTYPES; ++i){
			if (BulkSaveElement[j][0] == variable.ELEMENT[i][0] && BulkSaveElement[j][1] == variable.ELEMENT[i][1]) thread.TB[j] = i;
		}
	}
	
	for(int j = 0; j<thread.NSURF; ++j)
	{

		GetItemText(hItemList, Wsymbol, 3, j, 1);
		TtoA(Symbol, Wsymbol);

		// Find the different element types and store them in ELEMENT 
		FindAtomTypes(Symbol);

		// Assign a type number to atoms in the model 
		for (int i= 0; i < thread.NTYPES; ++i){
			if (Symbol[0]==variable.ELEMENT[i][0] && Symbol[1]==variable.ELEMENT[i][1]) thread.TS[j] = i;
		}
	///////////////////////////////
		variable.XS[j] = GetItemReal(hItemList, j, 2);
		variable.XCONST[j] = GetItemReal(hItemList, j, 3);
		variable.NXDIS[j] = (int)GetItemReal(hItemList, j, 4);
		if(variable.NDISTOT < variable.NXDIS[j]) variable.NDISTOT = variable.NXDIS[j];
		variable.X2CONST[j] = GetItemReal(hItemList, j, 5);
		variable.NX2DIS[j] = (int)GetItemReal(hItemList, j, 6);
		if(variable.NDISTOT < variable.NX2DIS[j]) variable.NDISTOT = variable.NX2DIS[j];
		//Y
		variable.YS[j] = GetItemReal(hItemList, j, 7);
		variable.YCONST[j] = GetItemReal(hItemList, j, 8);
		variable.NYDIS[j] = (int)GetItemReal(hItemList, j, 9);
		if(variable.NDISTOT < variable.NYDIS[j]) variable.NDISTOT = variable.NYDIS[j];
		variable.Y2CONST[j] = GetItemReal(hItemList, j, 10);
		variable.NY2DIS[j] = (int)GetItemReal(hItemList, j, 11);
		if(variable.NDISTOT < variable.NY2DIS[j]) variable.NDISTOT = variable.NY2DIS[j];
		//Z
		variable.ZS[j] = GetItemReal(hItemList, j, 12);
		variable.ZCONST[j] = GetItemReal(hItemList, j, 13);
		variable.NZDIS[j] = (int)GetItemReal(hItemList, j, 14);
		if(variable.NDISTOT < variable.NZDIS[j]) variable.NDISTOT = variable.NZDIS[j];
		variable.Z2CONST[j] = GetItemReal(hItemList, j, 15);
		variable.NZ2DIS[j] = (int)GetItemReal(hItemList, j, 16);
		if (variable.NDISTOT < variable.NZ2DIS[j]) variable.NDISTOT = variable.NZ2DIS[j];


		//DW1
		thread.NDWS[j] = (int)GetItemReal(hItemList, j, 17);
		if(thread.NDWTOT < thread.NDWS[j]) thread.NDWTOT = thread.NDWS[j];
		//DW2
		thread.NDWS2[j] = (int)GetItemReal(hItemList, j, 18);
		if(thread.NDWTOT2 < thread.NDWS2[j]) thread.NDWTOT2 = thread.NDWS2[j];
		//Occ
		thread.NOCCUP[j] = (int)abs(GetItemReal(hItemList, j, 19));
		if(variable.NOCCTOT < thread.NOCCUP[j]) variable.NOCCTOT = thread.NOCCUP[j];
		if (GetItemReal(hItemList, j, 19) < 0)
			thread.OCCUP_FLAG[j] = true;
		else
			thread.OCCUP_FLAG[j] = false;
		//Occ2
		thread.NOCCUP2[j] = (int)abs(GetItemReal(hItemList, j, 20));
		if (variable.NOCCTOT2 < thread.NOCCUP2[j]) variable.NOCCTOT2 = thread.NOCCUP2[j];

	}
}

void CopyAtomsFromMoleculeFitEditListBox(HWND hItemList, int Molecule){

	WCHAR Wsymbol[4];
	int i;
	for (int i = 0; i < thread.NTYPES; ++i){
		thread.TS[i] = 0;
	}
	myImport.SetMolecule(false);

	for (int j = myImport.myMolecule[Molecule].start; j < myImport.myMolecule[Molecule].end; ++j){
		i = j - myImport.myMolecule[Molecule].start;
		GetItemText(hItemList, Wsymbol, 3, i, 1);
		TtoA(myImport.element_from_file[j], Wsymbol);

		myImport.x_cartesian[j] = (float)GetItemReal(hItemList, i, 2);
		myImport.y_cartesian[j] = (float)GetItemReal(hItemList, i, 3);
		myImport.z_cartesian[j] = (float)GetItemReal(hItemList, i, 4);
	}
}

void CopyValuesFromMoleculeFitEditListBox(HWND hItemList){

	WCHAR Wsymbol[MOLECULENAME];

	for (int j = 0; j<myImport.TotalMolecules; j++){
		//AtoT(pszAtom, variable.ELEMENT[thread.TS[j]], 3);
		GetItemText(hItemList, Wsymbol, MOLECULENAME, j, 1);
		TtoA(myImport.myMolecule[j].MoleculeName, Wsymbol);
		////////////////////////////////
		
		myImport.myMolecule[j].MolecularPosition_x = (float)GetItemReal(hItemList, j, 2);

		myImport.myMolecule[j].NXDIS = (int)GetItemReal(hItemList, j, 3);
		if (variable.NDISTOT < myImport.myMolecule[j].NXDIS) variable.NDISTOT = myImport.myMolecule[j].NXDIS;

		//Y
		myImport.myMolecule[j].MolecularPosition_y = (float)GetItemReal(hItemList, j, 4);
		myImport.myMolecule[j].NYDIS = (int)GetItemReal(hItemList, j, 5);
		if (variable.NDISTOT < myImport.myMolecule[j].NYDIS) variable.NDISTOT = myImport.myMolecule[j].NYDIS;

		//Z
		myImport.myMolecule[j].MolecularPosition_z = (float)GetItemReal(hItemList, j, 6);
		myImport.myMolecule[j].NZDIS = (int)GetItemReal(hItemList, j, 7);
		if (variable.NDISTOT < myImport.myMolecule[j].NZDIS) variable.NDISTOT = myImport.myMolecule[j].NZDIS;
		//DW1
		myImport.myMolecule[j].NDWS = (int)GetItemReal(hItemList, j, 8);
		if (thread.NDWTOT < myImport.myMolecule[j].NDWS) thread.NDWTOT = myImport.myMolecule[j].NDWS;
		//DW2
		myImport.myMolecule[j].NDWS2 = (int)GetItemReal(hItemList, j, 9);
		if (thread.NDWTOT2 < myImport.myMolecule[j].NDWS2) thread.NDWTOT2 = myImport.myMolecule[j].NDWS2;
		//Occ
		myImport.myMolecule[j].NOCCUP = (int)abs(GetItemReal(hItemList, j, 10));
		if (variable.NOCCTOT < myImport.myMolecule[j].NOCCUP) variable.NOCCTOT = myImport.myMolecule[j].NOCCUP;
		//Rot x
		myImport.myMolecule[j].NROTATIONX = (int)abs(GetItemReal(hItemList, j, 11));
		if (variable.ROTATIONTOTX < myImport.myMolecule[j].NROTATIONX) variable.ROTATIONTOTX = myImport.myMolecule[j].NROTATIONX;
		//Rot y
		myImport.myMolecule[j].NROTATIONY = (int)abs(GetItemReal(hItemList, j, 12));
		if (variable.ROTATIONTOTY < myImport.myMolecule[j].NROTATIONY) variable.ROTATIONTOTY = myImport.myMolecule[j].NROTATIONY;
		//Rot z
		myImport.myMolecule[j].NROTATIONZ = (int)abs(GetItemReal(hItemList, j, 13));
		if (variable.ROTATIONTOTZ < myImport.myMolecule[j].NROTATIONZ) variable.ROTATIONTOTZ = myImport.myMolecule[j].NROTATIONZ;

	}

}

void CopyValuesFromDataEditListBox(HWND hItemList){
	

	for (int i = 0; i < thread.NDAT; ++i){
		//h
		thread.HDAT[i] = GetItemReal(hItemList, i, 1);
		//k
		thread.KDAT[i] = GetItemReal(hItemList, i, 2);
		//l
		thread.LDAT[i] = GetItemReal(hItemList, i, 3);
		//F
		thread.FDAT[i] = GetItemReal(hItemList, i, 4);
		//dF
		variable.ERRDAT[i] = GetItemReal(hItemList, i, 5);
		//Energy
		thread.ENERGY[i] = (int)GetItemReal(hItemList, i, 6);
		//Subscale
		thread.SUBSC[i] = (int)GetItemReal(hItemList, i, 7);
		//LBragg
		thread.LBR[i] = (int)GetItemReal(hItemList, i, 8);
		//Frac
		if ((int)GetItemReal(hItemList, i, 9) == 0)
			thread.FRAC[i] = false;
		else 
			thread.FRAC[i] = true;
	}
}


void ArrangeAtomPositions(HWND hItemList)
{
	double t; //2-x, 3-y, 4-z

	//x
	for (int j = 0; j < thread.NBULK; j++)
	{
		t = GetItemReal(hItemList, j, 2);
		if (t < 0) t += 1;
		SetCell(hItemList, t, j, 2, false);
	}

	//y
	for (int j = 0; j < thread.NBULK; j++)
	{
		t = GetItemReal(hItemList, j, 3);
		if (t < 0) t += 1;
		SetCell(hItemList, t, j, 3, false);
	}

	//z
	for (int j = 0; j < thread.NBULK; j++)
	{
		t = GetItemReal(hItemList, j, 4);
		if (t < 0) t += 1;
		SetCell(hItemList, t, j, 4, false);
	}
}


void CopyValuesFromBulkEditListBox(HWND hItemList){

char Symbol[3];
WCHAR Wsymbol[3];

	//Clear the St tables before procesing
	for (int i= 0; i < thread.NTYPES;i++){
			thread.TB[i] = 0;
	}
	for(int j = 0; j<thread.NBULK; j++){
				//AtoT(pszAtom, variable.ELEMENT[thread.TS[j]], 3);
		GetItemText(hItemList, Wsymbol, 3, j, 1);
		TtoA(Symbol, Wsymbol);
	////////////////////////////////
		/* Find the different element types and store them in ELEMENT */



		if (thread.NTYPES == 0){
			thread.NTYPES = 1;
			CopyMemory (variable.ELEMENT[0], Symbol, sizeof(Symbol));
			get_coeff(variable.ELEMENT[0], thread.F_COEFF[0]);
		}
		bool newtype = TRUE;
		for (int i = 0; i < thread.NTYPES; i++){
			if (Symbol[0]==variable.ELEMENT[i][0] && Symbol[1]==variable.ELEMENT[i][1]) newtype = FALSE;   
		}
		if (newtype){
			if (thread.NTYPES == MAXTYPES){
				MessageBox(NULL, TEXT("ERROR, too many atom types in unit cell!"), NULL, MB_ICONERROR);
				return;
			}
			thread.NTYPES++;
			CopyMemory(variable.ELEMENT[thread.NTYPES-1], Symbol, sizeof(Symbol));
			/* Store the coefficients for the computation of the atomic scattering
				factors for all the different elements in F_COEFF */		
			get_coeff(variable.ELEMENT[thread.NTYPES-1], thread.F_COEFF[thread.NTYPES-1]);
		}

		/* Assign a type number to atoms in the model */
		for (int i= 0; i < thread.NTYPES;i++){
			if (Symbol[0]==variable.ELEMENT[i][0] && Symbol[1]==variable.ELEMENT[i][1]) thread.TB[j] = i;
		}
	///////////////////////////////
		thread.XB[j] = GetItemReal(hItemList, j, 2);
		//Y
		thread.YB[j] = GetItemReal(hItemList, j, 3);
		//Z
		thread.ZB[j] = GetItemReal(hItemList, j, 4);
		//DW
		thread.NDWB[j] = (int)GetItemReal(hItemList, j, 5);
		if(thread.NDWTOT < thread.NDWB[j]) thread.NDWTOT = thread.NDWB[j];
	}

}

void CopyValuesFromKeatingListBox(HWND hItemList){
 int i, j;


	for (i = 0; i < thread.NSURF; i++){
		ket.ATOM_RAD[thread.TS[i]] = GetItemReal(hItemList, i, 2);

		if(ListView_GetCheckState(  hItemList, i) == TRUE)
			ket.IN_KEAT[i] = true;
		else 
			ket.IN_KEAT[i] = false;
	}

	for (i = 0; i < thread.NSURF; i++){
		int n=0;
		for (j = 0; j < thread.NSURF; j++){
			//if(ket.IN_KEAT[j]){
			//	ket.BOND_LIST[i][j] = (int)GetItemReal(hItemList, i, j+3);
			//}
		}
	}
}

void CopyValuesFromListBox(HWND hItemList, bool *ValueFlag, bool *IsSomthingSelected){
 int j;
 int TOTAL=0;

 int Nr_selected = 0;
 for (int i = 0; i < ListView_GetItemCount(hItemList); ++i)
 {
	 if (ListView_GetCheckState(hItemList, i) == TRUE) Nr_selected++;
 }
 if (Nr_selected == 0) *IsSomthingSelected = false;
 else *IsSomthingSelected = true;


 if (ListView_GetCheckState(hItemList, TOTAL) == TRUE) variable.SCALEPEN = true;
	else variable.SCALEPEN = false;

	TOTAL++;

	for (j = 0; j<variable.NSUBSCALETOT; j++){
		if (ListView_GetCheckState(hItemList, TOTAL + j) == TRUE) variable.SUBSCALEPEN[j] = true;
		else variable.SUBSCALEPEN[j] = false;
	}

	TOTAL += variable.NSUBSCALETOT;

	if (ListView_GetCheckState(hItemList, TOTAL) == TRUE) variable.BETAPEN = true;
	else variable.BETAPEN = false;

	TOTAL++;

	if (ListView_GetCheckState(hItemList, TOTAL) == TRUE) variable.SURFFRACPEN = true;
	else variable.SURFFRACPEN = false;

	TOTAL++;

	if (thread.NSURF2 > 0)
	{
		if (ListView_GetCheckState(hItemList, TOTAL) == TRUE) variable.SURF2FRACPEN = true;
		else variable.SURF2FRACPEN = false;

		TOTAL++;
	}

	for(j = 0; j<variable.NDISTOT; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL+j)==TRUE) variable.DISPLPEN[j] = true;
		else variable.DISPLPEN[j] = false;
	}

	TOTAL += variable.NDISTOT;

	for(j = 0; j<thread.NDWTOT; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL + j)==TRUE) variable.DEBWALPEN[j] = true;
		else variable.DEBWALPEN[j] = false;
	}

	TOTAL += thread.NDWTOT;

	for(j = 0; j<thread.NDWTOT2; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL + j)==TRUE) variable.DEBWAL2PEN[j] = true;
		else variable.DEBWAL2PEN[j] = false;
	}

	TOTAL += thread.NDWTOT2;

	for(j = 0; j<variable.NOCCTOT; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL + j)==TRUE) variable.OCCUPPEN[j] = true;
		else variable.OCCUPPEN[j] = false;
	}

	TOTAL += variable.NOCCTOT;
	for (j = 0; j<variable.NOCCTOT2; j++) {
		if (ListView_GetCheckState(hItemList, TOTAL + j) == TRUE) variable.OCCUP2PEN[j] = true;
		else variable.OCCUP2PEN[j] = false;
	}
	TOTAL += variable.NOCCTOT2;
	
		if (variable.NFACTOR > 0)
	{
		for (j = 0; j < variable.NFACTOR; j++) {
			if (ListView_GetCheckState(hItemList, TOTAL + j) == TRUE) variable.FACTOR_PEN[j] = true;
			else variable.FACTOR_PEN[j] = false;
		}
		TOTAL += variable.NFACTOR;
	}

	//Rotation
	if (variable.MODE == 1)
	{
		
		for (j = 0; j<variable.ROTATIONTOTX; ++j){
			if (ListView_GetCheckState(hItemList, TOTAL + j) == TRUE) variable.ROTATIONPENX[j] = true;
			else variable.ROTATIONPENX[j] = false;
		}
		TOTAL += variable.ROTATIONTOTX;

		for (j = 0; j<variable.ROTATIONTOTY; ++j){
			if (ListView_GetCheckState(hItemList, TOTAL + j) == TRUE) variable.ROTATIONPENY[j] = true;
			else variable.ROTATIONPENY[j] = false;
		}
		TOTAL += variable.ROTATIONTOTY;

		for (j = 0; j<variable.ROTATIONTOTZ; ++j){
			if (ListView_GetCheckState(hItemList, TOTAL + j) == TRUE) variable.ROTATIONPENZ[j] = true;
			else variable.ROTATIONPENZ[j] = false;
		}
	}

	//Domain occ.
	if (variable.DOMOCCUPAUTO)
	{
		TOTAL += variable.ROTATIONTOTZ;
		for (j = 0; j<thread.NDOMAIN; ++j){
			if (ListView_GetCheckState(hItemList, TOTAL + j) == TRUE) variable.DOMOCCUPPEN[j] = true;
			else variable.DOMOCCUPPEN[j] = false;
		}
	}


	//Copy values
	TOTAL = 0;
	variable.SCALELIM[0] = GetItemReal(hItemList, TOTAL, 2);
	variable.SCALELIM[1] = GetItemReal(hItemList, TOTAL, 4);
	thread.SCALE = GetItemReal(hItemList, TOTAL, 3);
	TestBorders(TOTAL, hItemList, ValueFlag);

	//Subscale
	TOTAL++;
	for (j = 0; j<variable.NSUBSCALETOT; j++){
		variable.SUBSCALELIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.SUBSCALELIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.SUBSCALE[j] = GetItemReal(hItemList, TOTAL + j, 3);
		TestBorders(TOTAL + j, hItemList, ValueFlag);
	}

	TOTAL += variable.NSUBSCALETOT;
	variable.BETALIM[0] = GetItemReal(hItemList, TOTAL, 2);
	variable.BETALIM[1] = GetItemReal(hItemList, TOTAL, 4);
	thread.BETA = GetItemReal(hItemList, TOTAL, 3);
	TestBorders(TOTAL, hItemList, ValueFlag);

	TOTAL++;
	variable.SURFFRACLIM[0] = GetItemReal(hItemList, TOTAL, 2);
	variable.SURFFRACLIM[1] = GetItemReal(hItemList, TOTAL, 4);
	thread.SURFFRAC = GetItemReal(hItemList, TOTAL, 3);
	TestBorders(TOTAL, hItemList, ValueFlag);

	if (thread.NSURF2 > 0)
	{
		TOTAL++;
		variable.SURF2FRACLIM[0] = GetItemReal(hItemList, TOTAL, 2);
		variable.SURF2FRACLIM[1] = GetItemReal(hItemList, TOTAL, 4);
		thread.SURF2FRAC = GetItemReal(hItemList, TOTAL, 3);
		TestBorders(TOTAL, hItemList, ValueFlag);
	}


	TOTAL++;
	for(j = 0; j<variable.NDISTOT; j++){
		variable.DISPLLIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.DISPLLIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		variable.DISPL[j]       = GetItemReal(hItemList, TOTAL + j, 3);
		TestBorders(TOTAL + j, hItemList, ValueFlag);

	}
	//Deb. Wall.
	TOTAL += variable.NDISTOT;
	for(j = 0; j<thread.NDWTOT; j++){
		variable.DEBWALLIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.DEBWALLIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.DEBWAL[j]         = GetItemReal(hItemList, TOTAL + j, 3);
		TestBorders(TOTAL + j, hItemList, ValueFlag);
	}
	//Deb. Wall. 2
	TOTAL += thread.NDWTOT;
	for(j = 0; j<thread.NDWTOT2; j++){
		variable.DEBWAL2LIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.DEBWAL2LIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.DEBWAL2[j]         = GetItemReal(hItemList, TOTAL + j, 3);
		TestBorders(TOTAL + j, hItemList, ValueFlag);
	}
	//Occ.
	TOTAL += thread.NDWTOT2;
	for(j = 0; j<variable.NOCCTOT; j++){
		variable.OCCUPLIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.OCCUPLIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.OCCUP[j]         = GetItemReal(hItemList, TOTAL + j, 3);
		TestBorders(TOTAL + j, hItemList, ValueFlag);
	}

	//Occ2.
	TOTAL += variable.NOCCTOT;
	for (j = 0; j<variable.NOCCTOT2; j++) {
		variable.OCCUP2LIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.OCCUP2LIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.OCCUP2[j] = GetItemReal(hItemList, TOTAL + j, 3);
		TestBorders(TOTAL + j, hItemList, ValueFlag);
	}

	TOTAL += variable.NOCCTOT2;

	//Factor.
	if (variable.NFACTOR > 0)
	{
		for (j = 0; j < variable.NFACTOR; j++) {
			variable.FACTOR_LIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
			variable.FACTOR_LIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
			thread.FACTOR[j] = GetItemReal(hItemList, TOTAL + j, 3);
			TestBorders(TOTAL + j, hItemList, ValueFlag);
		}
		TOTAL += variable.NFACTOR;
	}
	//Rotation
	if (variable.MODE == 1)
	{
		
		for (j = 0; j<variable.ROTATIONTOTX; j++){
			variable.ROTATIONLIMX[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
			variable.ROTATIONLIMX[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
			variable.ROTATIONX[j] = GetItemReal(hItemList, TOTAL + j, 3);
			TestBorders(TOTAL + j, hItemList, ValueFlag);
		}

		TOTAL += variable.ROTATIONTOTX;
		for (j = 0; j<variable.ROTATIONTOTY; j++){
			variable.ROTATIONLIMY[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
			variable.ROTATIONLIMY[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
			variable.ROTATIONY[j] = GetItemReal(hItemList, TOTAL + j, 3);
			TestBorders(TOTAL + j, hItemList, ValueFlag);
		}

		TOTAL += variable.ROTATIONTOTY;
		for (j = 0; j<variable.ROTATIONTOTZ; j++){
			variable.ROTATIONLIMZ[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
			variable.ROTATIONLIMZ[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
			variable.ROTATIONZ[j] = GetItemReal(hItemList, TOTAL + j, 3);
			TestBorders(TOTAL + j, hItemList, ValueFlag);
		}
	}

	//Domain occ.
	if (variable.DOMOCCUPAUTO)
	{
		TOTAL += variable.ROTATIONTOTZ;
		for (j = 0; j<thread.NDOMAIN; j++){
			variable.DOMOCCUPLIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
			variable.DOMOCCUPLIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
			thread.DOMOCCUP[j] = GetItemReal(hItemList, TOTAL + j, 3);
			TestBorders(TOTAL + j, hItemList, ValueFlag);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------
// Check boders of the fit list box values min<value<max                                                  
//--------------------------------------------------------------------------------------------------------------------------------

void TestBorders(int CellNumber, HWND hItemList, bool *_ValueFlag)
{
	if(ListView_GetCheckState(  hItemList, CellNumber) == TRUE && ( GetItemReal(hItemList, CellNumber, 2) > GetItemReal(hItemList, CellNumber, 3) || GetItemReal(hItemList, CellNumber, 3)> GetItemReal(hItemList, CellNumber, 4)))
		_ValueFlag[CellNumber] = false;
	else
		_ValueFlag[CellNumber] = true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fourier box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK FourierConturMap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	static int mode;
	static unsigned int tmpi;
	static unsigned int NxCon = 250, NyCon = 250;
	static unsigned int Nlevels = 50;

	static double min_x = 0., min_y = 0., max_x = 1., max_y =1.;
	double temp;

	switch (message){
		case WM_INITDIALOG:
			SetDlgItemInt (hDlg, IDC_EDIT1, NxCon, 0);
			SetDlgItemInt (hDlg, IDC_EDIT2, NyCon, 0);
			SetDlgItemInt (hDlg, IDC_EDIT3, Nlevels, 0);

			SetDlgItemInt (hDlg, IDC_EDIT4, 0, 0);

			SetDlgItemReal (hDlg, IDC_EDIT5,  min_x);
			SetDlgItemReal (hDlg, IDC_EDIT9,  max_x);
			SetDlgItemReal (hDlg, IDC_EDIT10, min_x);
			SetDlgItemReal (hDlg, IDC_EDIT11, max_y);

			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("Patterson of experimental structure factors"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("Patterson of calculated structure factors"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("Electron density difference map"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("Electron density of model"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)mode, 0 );

		return (INT_PTR)TRUE;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDOK){	
				tmpi=GetDlgItemInt(hDlg, IDC_EDIT1, 0, 0);
				if(tmpi>=0 && tmpi<1000) NxCon = tmpi;
				else MessageBox(hDlg, L"Wrong value on N points in x direction!", NULL, MB_OK);

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT2, 0, 0);
				if(tmpi>=0 && tmpi<1000) NyCon = tmpi;
				else MessageBox(hDlg, L"Wrong value on N points in y direction!", NULL, MB_OK);

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT3, 0, 0);
				if(tmpi>=0 && tmpi<1000) Nlevels = tmpi;
				else MessageBox(hDlg, L"Wrong number of levels!", NULL, MB_OK);

				//min, max of x and y
				temp=GetDlgItemReal(hDlg, IDC_EDIT5);
				if(temp>=-10. && temp<10.) min_x = temp;
				else MessageBox(hDlg, L"Wrong min. x value!", NULL, MB_OK);

				temp=GetDlgItemReal(hDlg, IDC_EDIT9);
				if(temp>=-10. && temp<10.) max_x = temp;
				else MessageBox(hDlg, L"Wrong max. x value", NULL, MB_OK);

				temp=GetDlgItemReal(hDlg, IDC_EDIT10);
				if(temp>=-10. && temp<10.) min_y = temp;
				else MessageBox(hDlg, L"Wrong min. y value!", NULL, MB_OK);

				temp=GetDlgItemReal(hDlg, IDC_EDIT11);
				if(temp>=-10. && temp<10.) max_y = temp;
				else MessageBox(hDlg, L"Wrong max. y value", NULL, MB_OK);



				//Set the progres bar
				SendMessage(GetDlgItem(hDlg, IDC_PROGRESS1), PBM_SETRANGE, 0,(LPARAM) MAKELPARAM(0, NyCon )); 
				SendMessage(GetDlgItem(hDlg, IDC_PROGRESS1), PBM_SETPOS,( WPARAM ) 0, 0 );//

				mode = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);
				plot_fourier(hDlg, mode+1,
					         NxCon, //number of points in x direction in contur plot
							 NyCon, //number of points in y direction in contur plot
							 min_x, //x start
							 max_x, //x end
							 min_y, //y start
							 max_y, //y end
							 Nlevels //number of levels in contur plot
							 );

				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDCANCEL){	
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
	
		break;
	}
 return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fourier box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK ChiConturMap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	static int mode;
	static unsigned int tmpi;
	static unsigned int NxCon = 100, NyCon = 100;
	static unsigned int Nlevels = 10;
	WCHAR Wstr[100];
	WCHAR XTitle[50]; //AXIS_TITLE_LENGHT
	WCHAR YTitle[50]; //AXIS_TITLE_LENGHT
	double *Parametr1;
	double LowLim1;
	double UpLim1;

	double *Parametr2;
	double LowLim2;
	double UpLim2;

	static double min_x = 0., min_y = 0., max_x = 1., max_y = 1.;
	HFONT hFont = NULL;

	switch (message){
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_EDIT1, NxCon, 0);
		SetDlgItemInt(hDlg, IDC_EDIT2, NyCon, 0);
		SetDlgItemInt(hDlg, IDC_EDIT3, Nlevels, 0);

		if (!hFont) hFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tachoma");

		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), WM_SETFONT, (WPARAM)hFont, 0);

		//add strings
		swprintf_s(Wstr, L"scale");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);

		for (int i = 0; i < variable.NSUBSCALETOT; i++){
			swprintf_s(Wstr, L"subscale %d", i + 1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}

		swprintf_s(Wstr, L"beta");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);

		swprintf_s(Wstr, L"sur. fraction");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);

		if (thread.NSURF2 > 0)
		{
			swprintf_s(Wstr, L"sur.2 fraction");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}

		for (int i = 0; i < variable.NDISTOT; i++){
			swprintf_s(Wstr, L"disp. %d", i+1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}

		for (int i = 0; i < thread.NDWTOT; i++){
			swprintf_s(Wstr, L"DW %d", i+1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}

		for (int i = 0; i < thread.NDWTOT2; i++){
			swprintf_s(Wstr, L"DW2 %d", i+1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}

		for (int i = 0; i < variable.NOCCTOT; i++){
			swprintf_s(Wstr, L"occ. %d", i+1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}
		for (int i = 0; i < variable.NOCCTOT2; i++) {
			swprintf_s(Wstr, L"occ.2 %d", i + 1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}
		for (int i = 0; i < variable.NFACTOR; i++) {
			swprintf_s(Wstr, L"Factor %d", i + 1);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)Wstr);
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)Wstr);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK){
			tmpi = GetDlgItemInt(hDlg, IDC_EDIT1, 0, 0);
			if (tmpi >= 0 && tmpi<3000) NxCon = tmpi;
			else MessageBox(hDlg, L"Wrong value on N points in x direction!", NULL, MB_OK);

			tmpi = GetDlgItemInt(hDlg, IDC_EDIT2, 0, 0);
			if (tmpi >= 0 && tmpi<3000) NyCon = tmpi;
			else MessageBox(hDlg, L"Wrong value on N points in y direction!", NULL, MB_OK);

			tmpi = GetDlgItemInt(hDlg, IDC_EDIT3, 0, 0);
			if (tmpi >= 0 && tmpi<500) Nlevels = tmpi;
			else MessageBox(hDlg, L"Wrong number of levels!", NULL, MB_OK);


			//Set the progres bar
			SendMessage(GetDlgItem(hDlg, IDC_PROGRESS1), PBM_SETRANGE, 0, (LPARAM)MAKELPARAM(0, NyCon));
			SendMessage(GetDlgItem(hDlg, IDC_PROGRESS1), PBM_SETPOS, (WPARAM)0, 0);//

			//////////////////////////combo1//////////////////////////
			

			int element = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_COMBO1));
			if (element < 0)
			{
				MessageBox(hDlg, L"Select parameters!", NULL, MB_OK);
				break;
			}
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, 0, 0);
			
			int i, ndx;
			ndx = 0;
			if (element == ndx)
			{
				Parametr1 = &thread.SCALE;
				LowLim1 = variable.SCALELIM[0];
				UpLim1 = variable.SCALELIM[1];
				swprintf_s(XTitle, L"scale");
			}

			for (i = 0; i < variable.NSUBSCALETOT; ++i)
			{
				ndx++;
				if (element == ndx) 
				{
					Parametr1 = &thread.SUBSCALE[i];
					LowLim1 = variable.SUBSCALELIM[i][0];
					UpLim1 = variable.SUBSCALELIM[i][1];
					swprintf_s(XTitle, L"subsc. %d", i + 1);
				}

				ndx++;
				if (element == ndx) {
					Parametr1 = &thread.BETA;
					LowLim1 = variable.BETALIM[0];
					UpLim1 = variable.BETALIM[1];
					swprintf_s(XTitle, L"beta");
				}
			}

			ndx++;
			if (element == ndx) {
				Parametr1 = &thread.SURFFRAC;
				LowLim1 = variable.SURFFRACLIM[0];
				UpLim1 = variable.SURFFRACLIM[1];
				swprintf_s(XTitle, L"sur. frac.");
			}

			if (thread.NSURF2 > 0)
			{
				ndx++;
				if (element == ndx) {
					Parametr1 = &thread.SURF2FRAC;
					LowLim1 = variable.SURF2FRACLIM[0];
					UpLim1 = variable.SURF2FRACLIM[1];
					swprintf_s(XTitle, L"sur.2 frac.");
				}
			}

			for (i = 0; i < variable.NDISTOT; ++i){
				ndx++;
				if (element == ndx) {
					Parametr1 = &variable.DISPL[i];
					LowLim1 = variable.DISPLLIM[i][0];
					UpLim1 = variable.DISPLLIM[i][1];
					swprintf_s(XTitle, L"disp. %d", i+1);
				}
			}
		
			for (i = 0; i < thread.NDWTOT; ++i){
				ndx++;
				if (element == ndx) {
					Parametr1 = &thread.DEBWAL[i];
					LowLim1 = variable.DEBWALLIM[i][0];
					UpLim1 = variable.DEBWALLIM[i][1];
					swprintf_s(XTitle, L"DW %d", i + 1);
				}
			}
			for (i = 0; i < thread.NDWTOT2; ++i){
				ndx++;
				if (element == ndx) {
					Parametr1 = &thread.DEBWAL2[i];
					LowLim1 = variable.DEBWAL2LIM[i][0];
					UpLim1 = variable.DEBWAL2LIM[i][1];
					swprintf_s(XTitle, L"DW2 %d", i + 1);
				}
			}
			for (i = 0; i < variable.NOCCTOT; ++i){
				ndx++;
				if (element == ndx) {
					Parametr1 = &thread.OCCUP[i];
					LowLim1 = variable.OCCUPLIM[i][0];
					UpLim1 = variable.OCCUPLIM[i][1];
					swprintf_s(XTitle, L"occ. %d", i + 1);
				}
			}
			for (i = 0; i < variable.NOCCTOT2; ++i) {
				ndx++;
				if (element == ndx) {
					Parametr1 = &thread.OCCUP2[i];
					LowLim1 = variable.OCCUP2LIM[i][0];
					UpLim1 = variable.OCCUP2LIM[i][1];
					swprintf_s(XTitle, L"occ2. %d", i + 1);
				}
			}
			if (variable.NFACTOR > 0)
			{
				for (i = 0; i < variable.NFACTOR; ++i) {
					ndx++;
					if (element == ndx) {
						Parametr1 = &thread.FACTOR[i];
						LowLim1 = variable.FACTOR_LIM[i][0];
						UpLim1 = variable.FACTOR_LIM[i][1];
						swprintf_s(XTitle, L"factor %d", i + 1);
					}
				}
			}


			//////////////////////////combo2///////////////////////////////////////////
			int element2 = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_COMBO2));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, 0, 0);
			ndx = 0;
			if (element2 == ndx){
				Parametr2 = &thread.SCALE;
				LowLim2 = variable.SCALELIM[0];
				UpLim2 = variable.SCALELIM[1];
				swprintf_s(YTitle, L"scale");
			}

			for (i = 0; i < variable.NSUBSCALETOT; ++i)
			{
				ndx++;
				if (element2 == ndx) {
					Parametr2 = &thread.SUBSCALE[i];
					LowLim2 = variable.SUBSCALELIM[i][0];
					UpLim2 = variable.SUBSCALELIM[i][1];
					swprintf_s(YTitle, L"subsc. %d", i + 1);
				}
}
				ndx++;
				if (element2 == ndx) {
					Parametr2 = &thread.BETA;
					LowLim2 = variable.BETALIM[0];
					UpLim2 = variable.BETALIM[1];
					swprintf_s(YTitle, L"beta");
				}
			
			ndx++;
				if (element2 == ndx) {
					Parametr2 = &thread.SURFFRAC;
					LowLim2 = variable.SURFFRACLIM[0];
					UpLim2 = variable.SURFFRACLIM[1];
					swprintf_s(YTitle, L"sur. frac.");
				}
				if (thread.NSURF2 > 0)
				{
					ndx++;
					if (element2 == ndx) {
						Parametr2 = &thread.SURFFRAC;
						LowLim2 = variable.SURFFRACLIM[0];
						UpLim2 = variable.SURFFRACLIM[1];
						swprintf_s(YTitle, L"sur. frac.");
					}
				}

				for (i = 0; i < variable.NDISTOT; ++i){
					ndx++;
					if (element2 == ndx) {
						Parametr2 = &variable.DISPL[i];
						LowLim2 = variable.DISPLLIM[i][0];
						UpLim2 = variable.DISPLLIM[i][1];
						swprintf_s(YTitle, L"disp. %d", i + 1);
					}
				}

				for (i = 0; i < thread.NDWTOT; ++i){
					ndx++;
					if (element2 == ndx) {
						Parametr2 = &thread.DEBWAL[i];
						LowLim2 = variable.DEBWALLIM[i][0];
						UpLim2 = variable.DEBWALLIM[i][1];
						swprintf_s(YTitle, L"DW %d", i + 1);
					}
				}
				for (i = 0; i < thread.NDWTOT2; ++i){
					ndx++;
					if (element2 == ndx) {
						Parametr2 = &thread.DEBWAL2[i];
						LowLim2 = variable.DEBWAL2LIM[i][0];
						UpLim2 = variable.DEBWAL2LIM[i][1];
						swprintf_s(YTitle, L"DW2 %d", i + 1);
					}
				}
				for (i = 0; i < variable.NOCCTOT; ++i){
					ndx++;
					if (element2 == ndx) {
						Parametr2 = &thread.OCCUP[i];
						LowLim2 = variable.OCCUPLIM[i][0];
						UpLim2 = variable.OCCUPLIM[i][1];
						swprintf_s(YTitle, L"occ. %d", i + 1);
					}
				}
				for (i = 0; i < variable.NOCCTOT2; ++i) {
					ndx++;
					if (element2 == ndx) {
						Parametr2 = &thread.OCCUP2[i];
						LowLim2 = variable.OCCUP2LIM[i][0];
						UpLim2 = variable.OCCUP2LIM[i][1];
						swprintf_s(YTitle, L"occ2. %d", i + 1);
					}
				}
				if (variable.NFACTOR > 0)
				{
					for (i = 0; i < variable.NFACTOR; ++i) {
						ndx++;
						if (element2 == ndx) {
							Parametr2 = &thread.FACTOR[i];
							LowLim2 = variable.FACTOR_LIM[i][0];
							UpLim2 = variable.FACTOR_LIM[i][1];
							swprintf_s(YTitle, L"factor %d", i + 1);
						}
					}
				}
			/////////////////////////////////////////////////////////////////
			plot_chi_map(
				hDlg,
				NxCon,
				NyCon,
				Nlevels,
				Parametr1,
				LowLim1,
				UpLim1,
				XTitle,
				Parametr2,
				LowLim2,
				UpLim2,
				YTitle
				);


			DeleteObject(hFont);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL){
			DeleteObject(hFont);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for set symmetry box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK SetSymmetry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 int PlaneGroup;

	switch (message){
		case WM_INITDIALOG:
			//SetDlgItemReal(hDlg, IDC_EDIT1, thread.ATTEN);
		    //SetDlgItemReal(hDlg, IDC_EDIT2, thread.BETA);

			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p1   (Plane group no. 1)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p2   (Plane group no. 2)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("pm   (Plane group no. 3)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("pg   (Plane group no. 4)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("cm   (Plane group no. 5)"));			
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p2mm (Plane group no. 6)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p2mg (Plane group no. 7)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p2gg (Plane group no. 8)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("c2mm (Plane group no. 9)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p4   (Plane group no. 10)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p4mm (Plane group no. 11)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p4gm (Plane group no. 12)"));			
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p3   (Plane group no. 13)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p3m1 (Plane group no. 14)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p31m (Plane group no. 15)"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p6   (Plane group no. 16)"));			
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("p6mm (Plane group no. 17)"));

			char Str[5];
			for (int i=0; i<18; i++){
				CopyMemory(Str, symmetry[i], sizeof(char)*5);
				if(CompareStr(variable.PLANEGROUP, Str ))
					PlaneGroup = i;
			}
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)PlaneGroup, 0 );	

		return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL){	
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDOK){	
		
				PlaneGroup = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0)+1;
				set_symmetry(hDlg, PlaneGroup);
				CopyMemory(variable.PLANEGROUP, symmetry[PlaneGroup], sizeof(char)*5);

				//Add record to UndoRedo
				myUndoRedo.AddNewRecord(L"Set symmetry");

				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
		break;
	}
 return (INT_PTR)FALSE;
}

void MoveButton(HWND hwnd, int iDDl, RECT rect, int rightC, int bottomC){

	RECT rectButton;
	GetClientRect(GetDlgItem(hwnd, iDDl), &rectButton);
		
	int width =  rectButton.right - rectButton.left;
	int height = rectButton.bottom - rectButton.top;


	MoveWindow(GetDlgItem(hwnd, iDDl), rect.right - width - rightC, rect.bottom - height - bottomC, width, height, TRUE);

}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditFitFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 HWND  hItemEdit;
 LeftListLimit = 1;
 RightListLimit = 19;
 static RECT rect;

 int j;
 static int ntot;
 static int NSURF_temp;
 //list
 WCHAR pszNr[8];
 WCHAR pszAtom[30];
 WCHAR pszXpos[20];
 WCHAR pszXposConst1[20];
 WCHAR pszXposParam1[20];
 WCHAR pszXposConst2[20];
 WCHAR pszXposParam2[20];
 WCHAR pszYpos[20];
 WCHAR pszYposConst1[20];
 WCHAR pszYposParam1[20];
 WCHAR pszYposConst2[20];
 WCHAR pszYposParam2[20];
 WCHAR pszZpos[20];
 WCHAR pszZposConst1[20];
 WCHAR pszZposParam[20];
 WCHAR pszZposConst2[20];
 WCHAR pszZposParam2[20];
 WCHAR pszDW1Param[20];
 WCHAR pszDW2Param[20];
 WCHAR pszOccParam[20];
 WCHAR pszOcc2Param[20];

 LVCOLUMN		colNr;
 LVCOLUMN		colAtom;
 LVCOLUMN		colXpos;
 LVCOLUMN		colXposConst1;
 LVCOLUMN		colXposParam1;
 LVCOLUMN		colXposConst2;
 LVCOLUMN		colXposParam2;
 LVCOLUMN		colYpos;
 LVCOLUMN		colYposConst1;
 LVCOLUMN		colYposParam1;
 LVCOLUMN		colYposConst2;
 LVCOLUMN		colYposParam2;
 LVCOLUMN		colZpos;
 LVCOLUMN		colZposConst1;
 LVCOLUMN		colZposParam;
 LVCOLUMN		colZposConst2;
 LVCOLUMN		colZposParam2;
 LVCOLUMN		colDW1Param;
 LVCOLUMN		colDW2Param;
 LVCOLUMN		colOccParam;
 LVCOLUMN		colOcc2Param;

 LVITEM			lvTest;
 UINT			iIndex = 0;

 hItemList = GetDlgItem(hDlg, IDC_LIST1);
 hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);

	switch (message){

	case WM_SIZE:
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 1, rect.bottom - rect.top - 50, TRUE);

		MoveButton(hDlg, IDOK, rect, 100, 10);
		MoveButton(hDlg, IDCANCEL, rect, 10, 10);
		MoveButton(hDlg, IDC_BUTTON2, rect, 600, 10);
		MoveButton(hDlg, IDC_BUTTON3, rect, 500, 10);
		MoveButton(hDlg, IDC_CHECK, rect, 400, 10);

		ShowWindow(hItemEdit, SW_HIDE);
		
		break;

		case WM_INITDIALOG:
			{

			GetClientRect(hDlg, &rect);
			InvalidateRect(hDlg, 0, 0);
			MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 1, rect.bottom - rect.top - 50, TRUE);

			MoveButton(hDlg, IDOK, rect, 100, 10);
			MoveButton(hDlg, IDCANCEL, rect, 10, 10);
			MoveButton(hDlg, IDC_BUTTON2, rect, 600, 10);
			MoveButton(hDlg, IDC_BUTTON3, rect, 500, 10);
			MoveButton(hDlg, IDC_CHECK, rect, 400, 10);

			NSURF_temp = thread.NSURF+thread.NSURF2;

			oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);

			ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT);

			ShowWindow(hItemEdit, SW_HIDE);
			SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

			//Create the columns in the list control
			//Name
			colNr.mask = LVCF_TEXT | LVCF_WIDTH;
			colNr.pszText = TEXT("Nr.");
			colNr.cchTextMax = 60;
			colNr.cx = 30;
			colNr.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 1, &colNr);

			//nr
			colAtom.mask = LVCF_TEXT | LVCF_WIDTH;
			colAtom.pszText = TEXT("Atom");
			colAtom.cchTextMax = 30;
			colAtom.cx = 40;
			colAtom.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 2, &colAtom);			

			//Low limit
			colXpos.mask = LVCF_TEXT | LVCF_WIDTH;
			colXpos.pszText = TEXT("Xpos.");
			colXpos.cchTextMax = 120;
			colXpos.cx = 60;
			colXpos.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 3, &colXpos);
		
			//Value
			colXposConst1.mask = LVCF_TEXT | LVCF_WIDTH;
			colXposConst1.pszText = TEXT("Xconst.1");
			colXposConst1.cchTextMax = 110;
			colXposConst1.cx = 60;
			colXposConst1.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 4, &colXposConst1);

			//Up limit
			colXposParam1.mask = LVCF_TEXT | LVCF_WIDTH;
			colXposParam1.pszText = TEXT("Xpar.1");
			colXposParam1.cchTextMax = 110;
			colXposParam1.cx = 50;
			colXposParam1.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 5, &colXposParam1);

			//Value
			colXposConst2.mask = LVCF_TEXT | LVCF_WIDTH;
			colXposConst2.pszText = TEXT("Xconst.2");
			colXposConst2.cchTextMax = 100;
			colXposConst2.cx = 60;
			colXposConst2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 6, &colXposConst2);

			//Up limit
			colXposParam2.mask = LVCF_TEXT | LVCF_WIDTH;
			colXposParam2.pszText = TEXT("Xpar.2");
			colXposParam2.cchTextMax = 110;
			colXposParam2.cx = 50;
			colXposParam2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 7, &colXposParam2);

			//Low limit
			colYpos.mask = LVCF_TEXT | LVCF_WIDTH;
			colYpos.pszText = TEXT("Y pos.");
			colYpos.cchTextMax = 120;
			colYpos.cx = 60;
			colYpos.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 8, &colYpos);

			//Value
			colYposConst1.mask = LVCF_TEXT | LVCF_WIDTH;
			colYposConst1.pszText = TEXT("Yconst.1");
			colYposConst1.cchTextMax = 120;
			colYposConst1.cx = 60;
			colYposConst1.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 9, &colYposConst1);

			//Up limit
			colYposParam1.mask = LVCF_TEXT | LVCF_WIDTH;
			colYposParam1.pszText = TEXT("Ypar.1");
			colYposParam1.cchTextMax = 120;
			colYposParam1.cx = 50;
			colYposParam1.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 10, &colYposParam1);

			//y cons 2
			colYposConst2.mask = LVCF_TEXT | LVCF_WIDTH;
			colYposConst2.pszText = TEXT("Yconst.2");
			colYposConst2.cchTextMax = 120;
			colYposConst2.cx = 60;
			colYposConst2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 11, &colYposConst2);

			//y par2
			colYposParam2.mask = LVCF_TEXT | LVCF_WIDTH;
			colYposParam2.pszText = TEXT("Y par.2");
			colYposParam2.cchTextMax = 120;
			colYposParam2.cx = 50;
			colYposParam2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 12, &colYposParam2);

			//z pos
			colZpos.mask = LVCF_TEXT | LVCF_WIDTH;
			colZpos.pszText = TEXT("Z pos.");
			colZpos.cchTextMax = 120;
			colZpos.cx = 60;
			colZpos.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 13, &colZpos);

			//z cons 2
			colZposConst1.mask = LVCF_TEXT | LVCF_WIDTH;
			colZposConst1.pszText = TEXT("Zconst.1");
			colZposConst1.cchTextMax = 120;
			colZposConst1.cx = 60;
			colZposConst1.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 14, &colZposConst1);

			//z par
			colZposParam.mask = LVCF_TEXT | LVCF_WIDTH;
			colZposParam.pszText = TEXT("Z par.1");
			colZposParam.cchTextMax = 120;
			colZposParam.cx = 50;
			colZposParam.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 15, &colZposParam);

			//z cons 2
			colZposConst2.mask = LVCF_TEXT | LVCF_WIDTH;
			colZposConst2.pszText = TEXT("Zconst.2");
			colZposConst2.cchTextMax = 120;
			colZposConst2.cx = 60;
			colZposConst2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 16, &colZposConst2);

			//z par2
			colZposParam2.mask = LVCF_TEXT | LVCF_WIDTH;
			colZposParam2.pszText = TEXT("Z par.2");
			colZposParam2.cchTextMax = 120;
			colZposParam2.cx = 50;
			colZposParam2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 17, &colZposParam2);

			//DW
			colDW1Param.mask = LVCF_TEXT | LVCF_WIDTH;
			colDW1Param.pszText = TEXT("DW1");
			colDW1Param.cchTextMax = 120;
			colDW1Param.cx = 40;
			colDW1Param.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 18, &colDW1Param);

			//DW2
			colDW2Param.mask = LVCF_TEXT | LVCF_WIDTH;
			colDW2Param.pszText = TEXT("DW2");
			colDW2Param.cchTextMax = 120;
			colDW2Param.cx = 40;
			colDW2Param.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 19, &colDW2Param);

			//occ.
			colOccParam.mask = LVCF_TEXT | LVCF_WIDTH;
			colOccParam.pszText = TEXT("Occ.");
			colOccParam.cchTextMax = 120;
			colOccParam.cx = 40;
			colOccParam.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 20, &colOccParam);

			//occ2.
			colOcc2Param.mask = LVCF_TEXT | LVCF_WIDTH;
			colOcc2Param.pszText = TEXT("Occ2.");
			colOcc2Param.cchTextMax = 120;
			colOcc2Param.cx = 40;
			colOcc2Param.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 20, &colOcc2Param);

			//plot in table
			lvTest.mask = LVIF_TEXT | LVIF_STATE;
			lvTest.iItem = 0;
			lvTest.pszText = pszNr;
			lvTest.iIndent = 0;
			lvTest.stateMask = 0;
			lvTest.state = 0;
			lvTest.iSubItem = 0;

			//molecule
			for (j = (thread.NSURF+thread.NSURF2)-1; j >= 0 ; --j){

				swprintf_s(pszNr, 100, L"%4d", j);
				//X
				AtoT(pszAtom, variable.ELEMENT[thread.TS[j]]);
				swprintf_s(pszXpos,	100, L"%6.4f", variable.XS[j]);
				swprintf_s(pszXposConst1,	100, L"%6.4f", variable.XCONST[j]);
				swprintf_s(pszXposParam1,	100, L"%d", variable.NXDIS[j]);
				swprintf_s(pszXposConst2,	100, L"%6.4f", variable.X2CONST[j]);
				swprintf_s(pszXposParam2,	100, L"%d", variable.NX2DIS[j]);
				//Y
				swprintf_s(pszYpos,	100, L"%6.4f", variable.YS[j]);
				swprintf_s(pszYposConst1,	100, L"%6.4f", variable.YCONST[j]);
				swprintf_s(pszYposParam1,	100, L"%d", variable.NYDIS[j]);
				swprintf_s(pszYposConst2,	100, L"%6.4f", variable.Y2CONST[j]);
				swprintf_s(pszYposParam2,	100, L"%d", variable.NY2DIS[j]);
				//Z
				swprintf_s(pszZpos,	100, L"%6.4f", variable.ZS[j]);
				swprintf_s(pszZposConst1, 100, L"%6.4f", variable.ZCONST[j]);
				swprintf_s(pszZposParam,	100, L"%d", variable.NZDIS[j]);
				swprintf_s(pszZposConst2, 100, L"%6.4f", variable.Z2CONST[j]);
				swprintf_s(pszZposParam2, 100, L"%d", variable.NZ2DIS[j]);

				//DW1
				swprintf_s(pszDW1Param,	100, L"%d", thread.NDWS[j]);
				//DW2
				swprintf_s(pszDW2Param,	100, L"%d", thread.NDWS2[j]);
				//Occ
				if (!thread.OCCUP_FLAG[j])
					swprintf_s(pszOccParam,	100, L"%d", thread.NOCCUP[j]);
				else
					swprintf_s(pszOccParam, 100, L"-%d", thread.NOCCUP[j]);
				//Occ2
				swprintf_s(pszOcc2Param, 100, L"%d", thread.NOCCUP2[j]);
				//if(variable.OCCUPPEN[j] )	ListView_SetCheckState(  hItemList, i-1 , 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText( hItemList, iIndex, 1, pszAtom);	
				//X
				ListView_SetItemText( hItemList, iIndex, 2, pszXpos);	
				ListView_SetItemText( hItemList, iIndex, 3, pszXposConst1);
				ListView_SetItemText( hItemList, iIndex, 4, pszXposParam1);
				ListView_SetItemText( hItemList, iIndex, 5, pszXposConst2);
				ListView_SetItemText( hItemList, iIndex, 6, pszXposParam2);
				//Y
				ListView_SetItemText( hItemList, iIndex, 7, pszYpos);	
				ListView_SetItemText( hItemList, iIndex, 8, pszYposConst1);
				ListView_SetItemText( hItemList, iIndex, 9, pszYposParam1);
				ListView_SetItemText( hItemList, iIndex, 10, pszYposConst2);
				ListView_SetItemText( hItemList, iIndex, 11, pszYposParam2);
				//Z
				ListView_SetItemText( hItemList, iIndex, 12, pszZpos);	
				ListView_SetItemText(hItemList, iIndex, 13, pszZposConst1);
				ListView_SetItemText( hItemList, iIndex, 14, pszZposParam);
				ListView_SetItemText(hItemList, iIndex, 15, pszZposConst2);
				ListView_SetItemText(hItemList, iIndex, 16, pszZposParam2);

				//DW
				ListView_SetItemText( hItemList, iIndex, 17, pszDW1Param);
				//DW2
				ListView_SetItemText( hItemList, iIndex, 18, pszDW2Param);
				//occ
				ListView_SetItemText( hItemList, iIndex, 19, pszOccParam);
				//occ
				ListView_SetItemText(hItemList, iIndex, 20, pszOcc2Param);
			}

		}

/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

		case WM_NOTIFY:

			case IDC_LIST1: 
				{
					LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;
					
					if(pnm->hdr.hwndFrom ==hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
					{
						SetWindowLong(hDlg, 0, (LONG)ProcessCustomDrawFit(lParam));
						return TRUE;
					}
				}
			switch(((NMHDR *)lParam)->code)
			{
				case NM_CLICK://IDC_LIST1

					if (nItem<ListView_GetItemCount(hItemList) )
					{
						if (nSubItem == 4 || nSubItem == 6 || nSubItem == 9 || nSubItem == 11 || nSubItem == 14 || nSubItem == 16 || nSubItem == 17 || nSubItem == 18 || nSubItem == 19 || nSubItem == 20)
							SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
						if (nSubItem == 2 || nSubItem == 3 || nSubItem == 5 || nSubItem == 7 || nSubItem == 8 || nSubItem == 10 || nSubItem == 12 || nSubItem == 13 || nSubItem == 15)
							SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
						if (nSubItem == 1){
							WCHAR temp_string[3];   //read line of text only 20 characters
							GetDlgItemText(hDlg, IDC_EDIT1, temp_string, 3); //STRING_SIZE only 20 characters!
							SetCell(hItemList, temp_string, nItem, nSubItem);
						}
					}
					SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
					OnClickList(hDlg, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)  lParam, &nItem, &nSubItem, 1, 100);
					SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));

					break;
				break;
			}
			break;



		case WM_COMMAND:
			switch LOWORD(wParam){

			case IDC_EDIT1:

				if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
				{
					if (nSubItem == 4 || nSubItem == 6 || nSubItem == 9 || nSubItem == 11 || nSubItem == 14 || nSubItem == 16 || nSubItem == 17 || nSubItem == 18 || nSubItem == 19 || nSubItem == 20)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
					if (nSubItem == 2 || nSubItem == 3 || nSubItem == 5 || nSubItem == 7 || nSubItem == 8 || nSubItem == 10 || nSubItem == 12 || nSubItem == 13 || nSubItem == 15)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
					if (nSubItem == 1){
						WCHAR temp_string[3];   //read line of text only 20 characters
						GetDlgItemText(hDlg, IDC_EDIT1, temp_string, 3); //STRING_SIZE only 20 characters!
						SetCell(hItemList, temp_string, nItem, nSubItem);
					}
				}
				break;

			case IDOK:
			{
				nItem = 0; nSubItem = 0;
				variable.NSURFTOT = ListView_GetItemCount(hItemList);
				thread.NSURF = variable.NSURFTOT - thread.NSURF2;

				if(!CheckFitEdit(hItemList))
					return (INT_PTR)FALSE;
				CopyValuesFromFitEditListBox(hItemList);

				//Add record to UndoRedo
				myUndoRedo.AddNewRecord(L"Edit fit file");

				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			case IDCANCEL:
			{
				nItem = 0; nSubItem = 0;
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}


			case IDC_CHECK:
				nItem = 0; nSubItem = 0;
				CheckFitEdit(hItemList);
				break;

			case IDC_BUTTON1:
			{
				HWND pwndCtrl = GetFocus();
				if (pwndCtrl == GetDlgItem(hDlg, IDC_EDIT1)){
					//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
					if (nSubItem == 4 || nSubItem == 6 || nSubItem == 9 || nSubItem == 11 || nSubItem == 14 || nSubItem == 16 || nSubItem == 17 || nSubItem == 18 || nSubItem == 19 || nSubItem == 20)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
					if (nSubItem == 2 || nSubItem == 3 || nSubItem == 5 || nSubItem == 7 || nSubItem == 8 || nSubItem == 10 || nSubItem == 12 || nSubItem == 13 || nSubItem == 15)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
					if (nSubItem == 1){
						WCHAR temp_string[3];   //read line of text only 20 characters
						GetDlgItemText(hDlg, IDC_EDIT1, temp_string, 3); //STRING_SIZE only 20 characters!
						SetCell(hItemList, temp_string, nItem, nSubItem);
					}
					SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
					ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
					InvalidateRect(hItemList, 0, 0);
				}
			}
				break;

				//Add atom
			case IDC_BUTTON2:

				lvTest.iItem = nItem;           // choose item  
				iIndex = nItem;
				lvTest.iSubItem = 0;        // Put in first coluom
				lvTest.pszText = pszNr; // Text to display 

				if (SendMessage(hItemList, LVM_INSERTITEM, 0, (LPARAM)&lvTest) > -1)
				{ // Send to the Listview
					//iIndex=SendMessage(hItemList,LVM_GETITEMCOUNT,0,0); 
					//iIndex = ListView_InsertItem(hItemList, &lvTest);

					swprintf_s(pszAtom, 20, L"H");
					//X
					swprintf_s(pszXpos, 20, L"%6.4f", 0.);
					swprintf_s(pszXposConst1, 20, L"%6.4f", 0.);
					swprintf_s(pszXposParam1, 20, L"%d", 0);
					swprintf_s(pszXposConst2, 20, L"%6.4f", 0.);
					swprintf_s(pszXposParam2, 20, L"%d", 0);
					//Y
					swprintf_s(pszYpos, 100, L"%6.4f", 0.);
					swprintf_s(pszYposConst1, 20, L"%6.4f", 0.);
					swprintf_s(pszYposParam1, 20, L"%d", 0);
					swprintf_s(pszYposConst2, 20, L"%6.4f", 0.);
					swprintf_s(pszYposParam2, 20, L"%d", 0);
					//Z
					swprintf_s(pszZpos, 100, L"%6.4f", 0.);
					swprintf_s(pszZposConst1, 20, L"%6.4f", 0.);
					swprintf_s(pszZposParam, 20, L"%d", 0);
					swprintf_s(pszZposConst2, 20, L"%6.4f", 0.);
					swprintf_s(pszZposParam2, 20, L"%d", 0);
					//DW1
					swprintf_s(pszDW1Param, 20, L"%d", 0);
					//DW2
					swprintf_s(pszDW2Param, 20, L"%d", 0);
					//Occ
					swprintf_s(pszOccParam, 20, L"%d", 0);
					//Occ2
					swprintf_s(pszOcc2Param, 20, L"%d", 0);

					swprintf_s(pszNr, 20, L"%4d", (int)SendMessage(hItemList, LVM_GETITEMCOUNT, 0, 0) - 1);
					ListView_SetItemText(hItemList, iIndex, 0, pszNr);
					ListView_SetItemText(hItemList, iIndex, 1, pszAtom);
					//X
					ListView_SetItemText(hItemList, iIndex, 2, pszXpos);
					ListView_SetItemText(hItemList, iIndex, 3, pszXposConst1);
					ListView_SetItemText(hItemList, iIndex, 4, pszXposParam1);
					ListView_SetItemText(hItemList, iIndex, 5, pszXposConst2);
					ListView_SetItemText(hItemList, iIndex, 6, pszXposParam2);
					//Y
					ListView_SetItemText(hItemList, iIndex, 7, pszYpos);
					ListView_SetItemText(hItemList, iIndex, 8, pszYposConst1);
					ListView_SetItemText(hItemList, iIndex, 9, pszYposParam1);
					ListView_SetItemText(hItemList, iIndex, 10, pszYposConst2);
					ListView_SetItemText(hItemList, iIndex, 11, pszYposParam2);
					//Z
					ListView_SetItemText(hItemList, iIndex, 12, pszZpos);
					ListView_SetItemText(hItemList, iIndex, 13, pszZposConst1);
					ListView_SetItemText(hItemList, iIndex, 14, pszZposParam);
					ListView_SetItemText(hItemList, iIndex, 15, pszZposConst2);
					ListView_SetItemText(hItemList, iIndex, 16, pszZposParam2);
					//DW
					ListView_SetItemText(hItemList, iIndex, 17, pszDW1Param);
					//DW2
					ListView_SetItemText(hItemList, iIndex, 18, pszDW2Param);
					//occ
					ListView_SetItemText(hItemList, iIndex, 19, pszOccParam);
					//occ2
					ListView_SetItemText(hItemList, iIndex, 20, pszOcc2Param);
				}
				ListView_Update(hItemList, iIndex);
				InvalidateRect(hItemList, 0, 0);
				break;

				//Delete itemm
			case IDC_BUTTON3:
				iIndex = static_cast<UINT>(SendMessage(hItemList, LVM_GETITEMCOUNT, 0, 0));
				for (int i = iIndex-1; i >= 0; i--)
					if (ListView_GetItemState(hItemList, i, LVIS_SELECTED) == LVIS_SELECTED)
						SendMessage(hItemList, LVM_DELETEITEM, i, 0); // delete the item selected
				break;
			}
		break;


	}
 return (INT_PTR)FALSE;

}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditMolecularFitFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	static HWND  hItemEdit;
	static HWND  hItemEdit2;
	static bool use_autocenter=false;



	WCHAR temp_string[MOLECULENAME];   //read line of text only 20 characters

	LeftListLimit = 1;
	RightListLimit = 13;

	LeftListLimit2 = 1;
	RightListLimit2 = 4;

	static RECT rect;

	int j;
	static int ntot;
	static int NSURF_temp;
	//list
	WCHAR pszNr[8];
	WCHAR pszAtom[100];
	WCHAR pszXpos[20];
	WCHAR pszXposParam1[20];
	WCHAR pszYpos[20];
	WCHAR pszYposParam1[20];
	WCHAR pszZpos[20];
	WCHAR pszZposParam[20];
	WCHAR pszDW1Param[20];
	WCHAR pszDW2Param[20];
	WCHAR pszOccParam[20];

	WCHAR pszROTXParam[20];
	WCHAR pszROTYParam[20];
	WCHAR pszROTZParam[20];

	
	LVCOLUMN		colNr;
	LVCOLUMN		colAtom;
	LVCOLUMN		colXpos;
	LVCOLUMN		colXposParam1;
	LVCOLUMN		colYpos;
	LVCOLUMN		colYposParam1;
	LVCOLUMN		colZpos;
	LVCOLUMN		colZposParam;
	LVCOLUMN		colDW1Param;
	LVCOLUMN		colDW2Param;
	LVCOLUMN		colOccParam;
	LVCOLUMN		colROTXParam;
	LVCOLUMN		colROTYParam;
	LVCOLUMN		colROTZParam;

	//Table 2
	LVCOLUMN		colNr2;
	LVCOLUMN		colAtom2;
	LVCOLUMN		colXpos2;
	LVCOLUMN		colYpos2;
	LVCOLUMN		colZpos2;

	LVITEM			lvTest;
	UINT			iIndex = 0;

	hItemList = GetDlgItem(hDlg, IDC_LIST1);
	hItemList3 = GetDlgItem(hDlg, IDC_LIST3);
	hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);
	hItemEdit2 = GetDlgItem(hDlg, IDC_EDIT2);

	switch (message)
	{

	case WM_SIZE:
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 1, rect.bottom / 2 - 10, TRUE);
		MoveWindow(hItemList3, rect.left, rect.bottom / 2 + 11, rect.right - rect.left - 1, rect.bottom / 2 - rect.top * 2 - 50, TRUE);

		MoveWindow(GetDlgItem(hDlg, IDC_CHECK1), rect.right - 100 - 300, rect.bottom - 20 - 10, 100, 20, TRUE);

		MoveButton(hDlg, IDOK, rect, 100, 10);
		MoveButton(hDlg, IDCANCEL, rect, 10, 10);
		MoveButton(hDlg, IDC_CHECK, rect, 200, 10);

		ShowWindow(hItemEdit, SW_HIDE);
		ShowWindow(hItemEdit2, SW_HIDE);

		break;

	case WM_INITDIALOG:
	{

		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 1, rect.bottom/2-10 , TRUE);
		MoveWindow(hItemList3, rect.left, rect.bottom / 2 + 11, rect.right - rect.left - 1, rect.bottom/2 - rect.top*2 - 50, TRUE);

		MoveWindow(GetDlgItem(hDlg, IDC_CHECK1), rect.right - 100 - 300, rect.bottom - 20 - 10, 100, 20, TRUE);

		MoveButton(hDlg, IDOK, rect, 100, 10);
		MoveButton(hDlg, IDCANCEL, rect, 10, 10);
		MoveButton(hDlg, IDC_CHECK, rect, 200, 10);
			
		NSURF_temp = thread.NSURF+thread.NSURF2;

		oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);
		oldEditProc2 = (WNDPROC)SetWindowLongPtr(hItemEdit2, GWLP_WNDPROC, (LONG_PTR)subEditProc2);

		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
		ListView_SetExtendedListViewStyle(hItemList3, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		ShowWindow(hItemEdit, SW_HIDE);
		ShowWindow(hItemEdit2, SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		//Copy atoms for cancel
		for (int i = 0; i < thread.NSURF; ++i)
		{
			memcpy(myImport.copy_element_from_file[i], myImport.element_from_file[i], sizeof(char) * 2);
			myImport.copy_x_cartesian[i] = myImport.x_cartesian[i];
			myImport.copy_y_cartesian[i] = myImport.y_cartesian[i];
			myImport.copy_z_cartesian[i] = myImport.z_cartesian[i];
		}


		if (use_autocenter == true) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);



		SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_ADDSTRING, 0, (LPARAM)TEXT("CHISQR"));
		if (static_cast<int>(SendMessage(GetDlgItem(hDlg, IDC_COMBO4), CB_GETCURSEL, 0, 0)) == 0) variable.LSQ_ERRORCALC = CHISQR;

		//Create the columns in the list control
		//nr
		colNr.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr.pszText = TEXT("Nr.");
		colNr.cchTextMax = 60;
		colNr.cx = 30;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colNr);

		//name
		colAtom.mask = LVCF_TEXT | LVCF_WIDTH;
		colAtom.pszText = TEXT("Molecule");
		colAtom.cchTextMax = 80;
		colAtom.cx = 120;
		colAtom.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 2, &colAtom);

		//x
		colXpos.mask = LVCF_TEXT | LVCF_WIDTH;
		colXpos.pszText = TEXT("Xpos.");
		colXpos.cchTextMax = 120;
		colXpos.cx = 60;
		colXpos.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 3, &colXpos);

		//Nx 
		colXposParam1.mask = LVCF_TEXT | LVCF_WIDTH;
		colXposParam1.pszText = TEXT("Xpar.1");
		colXposParam1.cchTextMax = 110;
		colXposParam1.cx = 50;
		colXposParam1.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 4, &colXposParam1);

		//
		colYpos.mask = LVCF_TEXT | LVCF_WIDTH;
		colYpos.pszText = TEXT("Y pos.");
		colYpos.cchTextMax = 120;
		colYpos.cx = 60;
		colYpos.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 5, &colYpos);

		//
		colYposParam1.mask = LVCF_TEXT | LVCF_WIDTH;
		colYposParam1.pszText = TEXT("Ypar.1");
		colYposParam1.cchTextMax = 120;
		colYposParam1.cx = 50;
		colYposParam1.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 6, &colYposParam1);

		//
		colZpos.mask = LVCF_TEXT | LVCF_WIDTH;
		colZpos.pszText = TEXT("Z pos.");
		colZpos.cchTextMax = 120;
		colZpos.cx = 60;
		colZpos.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 7, &colZpos);

		//
		colZposParam.mask = LVCF_TEXT | LVCF_WIDTH;
		colZposParam.pszText = TEXT("Z par.");
		colZposParam.cchTextMax = 120;
		colZposParam.cx = 50;
		colZposParam.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 8, &colZposParam);

		//
		colDW1Param.mask = LVCF_TEXT | LVCF_WIDTH;
		colDW1Param.pszText = TEXT("DW1");
		colDW1Param.cchTextMax = 120;
		colDW1Param.cx = 40;
		colDW1Param.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 9, &colDW1Param);

		//DW2
		colDW2Param.mask = LVCF_TEXT | LVCF_WIDTH;
		colDW2Param.pszText = TEXT("DW2");
		colDW2Param.cchTextMax = 120;
		colDW2Param.cx = 40;
		colDW2Param.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 10, &colDW2Param);

		//Occ.
		colOccParam.mask = LVCF_TEXT | LVCF_WIDTH;
		colOccParam.pszText = TEXT("Occ.");
		colOccParam.cchTextMax = 120;
		colOccParam.cx = 40;
		colOccParam.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 11, &colOccParam);

		//ROTX.
		colROTXParam.mask = LVCF_TEXT | LVCF_WIDTH;
		colROTXParam.pszText = TEXT("Rot. X");
		colROTXParam.cchTextMax = 120;
		colROTXParam.cx = 60;
		colROTXParam.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 12, &colROTXParam);

		//ROTY.
		colROTYParam.mask = LVCF_TEXT | LVCF_WIDTH;
		colROTYParam.pszText = TEXT("Rot. Y");
		colROTYParam.cchTextMax = 120;
		colROTYParam.cx = 60;
		colROTYParam.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 13, &colROTYParam);

		//ROTZ.
		colROTZParam.mask = LVCF_TEXT | LVCF_WIDTH;
		colROTZParam.pszText = TEXT("Rot. Z");
		colROTZParam.cchTextMax = 120;
		colROTZParam.cx = 60;
		colROTZParam.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 14, &colROTZParam);

		//plot in table
		lvTest.mask = LVIF_TEXT | LVIF_STATE;
		lvTest.iItem = 0;
		lvTest.pszText = pszNr;
		lvTest.iIndent = 0;
		lvTest.stateMask = 0;
		lvTest.state = 0;
		lvTest.iSubItem = 0;

		//Input values
		for (j = myImport.TotalMolecules - 1; j >= 0; j--)
		{

			swprintf_s(pszNr, 100, L"%4d", j);
			//X
			AtoT(pszAtom, myImport.myMolecule[j].MoleculeName);
			swprintf_s(pszXpos, 100, L"%6.4f", myImport.myMolecule[j].MolecularPosition_x);
			swprintf_s(pszXposParam1, 100, L"%d", myImport.myMolecule[j].NXDIS);
			//Y
			swprintf_s(pszYpos, 100, L"%6.4f", myImport.myMolecule[j].MolecularPosition_y);
			swprintf_s(pszYposParam1, 100, L"%d", myImport.myMolecule[j].NYDIS);

			//Z
			swprintf_s(pszZpos, 100, L"%6.4f", myImport.myMolecule[j].MolecularPosition_z);
			swprintf_s(pszZposParam, 100, L"%d", myImport.myMolecule[j].NZDIS);
			//DW1
			swprintf_s(pszDW1Param, 100, L"%d", myImport.myMolecule[j].NDWS);
			//DW2
			swprintf_s(pszDW2Param, 100, L"%d", myImport.myMolecule[j].NDWS2);
			//Occ
			swprintf_s(pszOccParam, 100, L"%d", myImport.myMolecule[j].NOCCUP);
			//Rotx
			swprintf_s(pszROTXParam, 100, L"%d", myImport.myMolecule[j].NROTATIONX);
			//Roty
			swprintf_s(pszROTYParam, 100, L"%d", myImport.myMolecule[j].NROTATIONY);
			//Rotz
			swprintf_s(pszROTZParam, 100, L"%d", myImport.myMolecule[j].NROTATIONZ);


			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszAtom);
			//X
			ListView_SetItemText(hItemList, iIndex, 2, pszXpos);
			ListView_SetItemText(hItemList, iIndex, 3, pszXposParam1);
			//Y
			ListView_SetItemText(hItemList, iIndex, 4, pszYpos);
			ListView_SetItemText(hItemList, iIndex, 5, pszYposParam1);
			//Z
			ListView_SetItemText(hItemList, iIndex, 6, pszZpos);
			ListView_SetItemText(hItemList, iIndex, 7, pszZposParam);
			//DW
			ListView_SetItemText(hItemList, iIndex, 8, pszDW1Param);
			//DW2
			ListView_SetItemText(hItemList, iIndex, 9, pszDW2Param);
			//occ
			ListView_SetItemText(hItemList, iIndex, 10, pszOccParam);
			//rot x
			ListView_SetItemText(hItemList, iIndex, 11, pszROTXParam);
			//rot y
			ListView_SetItemText(hItemList, iIndex, 12, pszROTYParam);
			//rot z
			ListView_SetItemText(hItemList, iIndex, 13, pszROTZParam);
		}

		//Create the columns in second list control
		//Name
		colNr2.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr2.pszText = TEXT("Nr.");
		colNr2.cchTextMax = 60;
		colNr2.cx = 30;
		colNr2.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList3, 1, &colNr2);

		//nr
		colAtom2.mask = LVCF_TEXT | LVCF_WIDTH;
		colAtom2.pszText = TEXT("Atom");
		colAtom2.cchTextMax = 60;
		colAtom2.cx = 100;
		colAtom2.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList3, 2, &colAtom2);

		//cartesian x position
		colXpos2.mask = LVCF_TEXT | LVCF_WIDTH;
		colXpos2.pszText = TEXT("X pos. cartesian");
		colXpos2.cchTextMax = 120;
		colXpos2.cx = 90;
		colXpos2.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList3, 3, &colXpos2);

		//cartesian y position
		colYpos2.mask = LVCF_TEXT | LVCF_WIDTH;
		colYpos2.pszText = TEXT("Y pos. cartesian");
		colYpos2.cchTextMax = 120;
		colYpos2.cx = 90;
		colYpos2.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList3, 4, &colYpos2);

		//cartesian z position
		colZpos2.mask = LVCF_TEXT | LVCF_WIDTH;
		colZpos2.pszText = TEXT("Z pos. cartesian");
		colZpos2.cchTextMax = 120;
		colZpos2.cx = 90;
		colZpos2.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList3, 5, &colZpos2);

		FillSecondTable(hItemList3, 0);
	}

	//----------------------------------------------------------------------------------------
		return (INT_PTR)TRUE;

	case WM_NOTIFY:


	case IDC_LIST1:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

		if (pnm->hdr.hwndFrom == hItemList &&pnm->hdr.code == NM_CUSTOMDRAW){
			SetWindowLong(hDlg, 0, (LONG)ProcessCustomDrawFitMolecule(lParam));
			return TRUE;
		}
	}

	switch (((NMHDR *)lParam)->code)
		{
		case NM_CLICK:

			HWND pwndCtrl = GetFocus();
			if (pwndCtrl == hItemList)
			{
				if (nItem < ListView_GetItemCount(hItemList))
				{
					if (nSubItem == 3 || nSubItem == 5 || nSubItem == 7 || nSubItem == 8 || nSubItem == 9 || nSubItem == 10 || nSubItem == 11 || nSubItem == 12 || nSubItem == 13 || nSubItem == 14 || nSubItem == 15)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
					if (nSubItem == 2 || nSubItem == 4 || nSubItem == 6)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
					if (nSubItem == 1)
					{
						GetDlgItemText(hDlg, IDC_EDIT1, temp_string, MOLECULENAME); //STRING_SIZE only HEADER characters!
						SetCell(hItemList, temp_string, nItem, nSubItem);
					}
				}

				//Copy atoms
				CopyAtomsFromMoleculeFitEditListBox(hItemList3, nItem);
				SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
				OnClickList(hDlg, hItemEdit, (NMHDR*)lParam, &nItem, &nSubItem, 1, 100);
				SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));

				
				FillSecondTable(hItemList3, nItem);
				break;
			}

			if (pwndCtrl == hItemList3)
			{
				if (nItem2 < ListView_GetItemCount(hItemList3))
				{
					if (nSubItem2 == 2 || nSubItem2 == 3 || nSubItem2 == 4)
						SetCell(hItemList3, GetDlgItemReal(hDlg, IDC_EDIT2), nItem2, nSubItem2);
					if (nSubItem2 == 1)
					{
						GetDlgItemText(hDlg, IDC_EDIT2, temp_string, 3); //STRING_SIZE only HEADER characters!
						SetCell(hItemList3, temp_string, nItem2, nSubItem2);
					}
				}
				SendMessage(hItemEdit2, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
				OnClickList(hDlg, hItemEdit2, (NMHDR*)lParam, &nItem2, &nSubItem2, 1, 100);
				SendMessage(hItemEdit2, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));

				break;
			}

		}
		break;

	case WM_COMMAND:
		switch LOWORD(wParam)
		{
		case IDC_EDIT1:

			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				if (nSubItem == 3 || nSubItem == 5 || nSubItem == 7 || nSubItem == 8 || nSubItem == 9 || nSubItem == 10 || nSubItem == 11 || nSubItem == 12 || nSubItem == 13 || nSubItem == 14 || nSubItem == 15)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
				if (nSubItem == 2 || nSubItem == 4 || nSubItem == 6)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				if (nSubItem == 1)
				{
					GetDlgItemText(hDlg, IDC_EDIT1, temp_string, MOLECULENAME);
					SetCell(hItemList, temp_string, nItem, nSubItem);
				}
			}
			break;

		case IDC_EDIT2:

			if (nItem2 < ListView_GetItemCount(hItemList3) && nSubItem2 >= LeftListLimit2 && nSubItem2 <= RightListLimit2)
			{
				if (nSubItem2 == 2 || nSubItem2 == 3 || nSubItem2 == 4)
					SetCell(hItemList3, GetDlgItemReal(hDlg, IDC_EDIT2), nItem2, nSubItem2);
				if (nSubItem2 == 1)
				{
					GetDlgItemText(hDlg, IDC_EDIT2, temp_string, 3);
					SetCell(hItemList3, temp_string, nItem2, nSubItem2);
				}
			}
			break;

		case IDOK:
		{
			
			nItem2 = 0; nSubItem2 = 0;
			CheckFitMoleculeEdit(hItemList);

			//Copy molecule values
			CopyValuesFromMoleculeFitEditListBox(hItemList);

			//Copy atoms
			CopyAtomsFromMoleculeFitEditListBox(hItemList3, nItem);
			nItem = 0; nSubItem = 0;
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) use_autocenter = true;
			else use_autocenter = false;

			//Apply changes
			myImport.SetMolecule(use_autocenter);

			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Edit molecule fit file");

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
		{
			nItem = 0; nSubItem = 0;
			nItem2 = 0; nSubItem2 = 0;
			//Copy atoms for cancel
			for (int i = 0; i < thread.NSURF; ++i)
			{
				memcpy(myImport.element_from_file[i], myImport.copy_element_from_file[i], sizeof(char) * 2);
				myImport.x_cartesian[i] = myImport.copy_x_cartesian[i];
				myImport.y_cartesian[i] = myImport.copy_y_cartesian[i];
				myImport.z_cartesian[i] = myImport.copy_z_cartesian[i];
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}


		case 	IDC_CHECK:
			nItem = 0; nSubItem = 0;
			nItem2 = 0; nSubItem2 = 0;
			CheckFitMoleculeEdit(hItemList);
			break;

		case IDC_BUTTON1:
		{
			HWND pwndCtrl = GetFocus();
			if (pwndCtrl == GetDlgItem(hDlg, IDC_EDIT1)){
				//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
				if (nSubItem == 3 || nSubItem == 5 || nSubItem == 7 || nSubItem == 8 || nSubItem == 9 || nSubItem == 10 || nSubItem == 11 || nSubItem == 12 || nSubItem == 13 || nSubItem == 14 || nSubItem == 15)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
				if (nSubItem == 2 || nSubItem == 4 || nSubItem == 6)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				if (nSubItem == 1)
				{
					GetDlgItemText(hDlg, IDC_EDIT1, temp_string, MOLECULENAME); //STRING_SIZE only HEADER characters!
					SetCell(hItemList, temp_string, nItem, nSubItem);
				}
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				InvalidateRect(hItemList, 0, 0);
			}
			break;

			if (pwndCtrl == GetDlgItem(hDlg, IDC_EDIT2)){
				//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
				if (nItem2 < ListView_GetItemCount(hItemList3) && nSubItem2 >= LeftListLimit2 && nSubItem2 <= RightListLimit2)
				{
					if (nSubItem2 == 2 || nSubItem2 == 3 || nSubItem2 == 4)
						SetCell(hItemList3, GetDlgItemReal(hDlg, IDC_EDIT2), nItem2, nSubItem2);
					if (nSubItem2 == 1)
					{
						GetDlgItemText(hDlg, IDC_EDIT2, temp_string, 3);
						SetCell(hItemList3, temp_string, nItem2, nSubItem2);
					}
				}
				SendDlgItemMessage(hDlg, IDC_EDIT2, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT2), SW_HIDE);
				InvalidateRect(hItemList3, 0, 0);
			}
			break;
		}
		}

	}
		return (INT_PTR)FALSE;
}

void FillSecondTable(HWND hItemList, int Molecule)
{
	WCHAR pszNr[8];
	WCHAR pszAtom[100];
	WCHAR pszXpos[20];
	WCHAR pszYpos[20];
	WCHAR pszZpos[20];
	LVITEM			lv2;
	UINT			iIndex = 0;
	int nr = 0;
	//plot in table
	lv2.mask = LVIF_TEXT | LVIF_STATE;
	lv2.iItem = 0;
	lv2.pszText = pszNr;
	lv2.iIndent = 0;
	lv2.stateMask = 0;
	lv2.state = 0;
	lv2.iSubItem = 0;

	for (int j = ListView_GetItemCount(hItemList) - 1; j >= 0; j--)
	SendMessage(hItemList, LVM_DELETEITEM, j, 0); // delete the item selected


	//Input values table 2
	nr = myImport.myMolecule[Molecule].end - myImport.myMolecule[Molecule].start+1;
	for (int j = myImport.myMolecule[Molecule].end - 1; j >= myImport.myMolecule[Molecule].start; j--)
	{	
		nr--;
		swprintf_s(pszNr, 100, L"%4d", nr);

		AtoT(pszAtom, variable.ELEMENT[thread.TS[j]]);
		//X
		swprintf_s(pszXpos, 100, L"%6.4f", myImport.x_cartesian[j]);
		//Y
		swprintf_s(pszYpos, 100, L"%6.4f", myImport.y_cartesian[j]);
		//Z
		swprintf_s(pszZpos, 100, L"%6.4f", myImport.z_cartesian[j]);

		lv2.pszText = pszNr;

		iIndex = ListView_InsertItem(hItemList, &lv2);
		ListView_SetItemText(hItemList, iIndex, 1, pszAtom);
		//X
		ListView_SetItemText(hItemList, iIndex, 2, pszXpos);
		//Y
		ListView_SetItemText(hItemList, iIndex, 3, pszYpos);
		//Z
		ListView_SetItemText(hItemList, iIndex, 4, pszZpos);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditBulkFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	HWND  hItemEdit;
	LeftListLimit = 1;
	RightListLimit = 5;

 int j;
 static int ntot;
 static RECT rect;


 //list
 WCHAR pszNr[8];
 WCHAR pszAtom[30];
 WCHAR pszXpos[20];
 WCHAR pszYpos[20];
 WCHAR pszZpos[20];
 WCHAR pszDW1Param[20];


 LVCOLUMN		colNr;
 LVCOLUMN		colAtom;
 LVCOLUMN		colXpos;
 LVCOLUMN		colYpos;
 LVCOLUMN		colZpos;
 LVCOLUMN		colDW1Param;

 LVITEM			lvTest;
 UINT			iIndex = 0;

 hItemList = GetDlgItem(hDlg, IDC_LIST1);
 hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);

	switch (message){



	case WM_SIZE:
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top +11, rect.right - rect.left - 95, rect.bottom - rect.top - 40, TRUE);

		MoveButton(hDlg, IDC_ARANGE, rect, 10, 80);
		MoveButton(hDlg, IDCANCEL, rect, 10, 50);
		MoveButton(hDlg, IDOK, rect, 10, 20);

		ShowWindow(hItemEdit, SW_HIDE);
		break;

		case WM_INITDIALOG:
			{

			GetClientRect(hDlg, &rect);
			InvalidateRect(hDlg, 0, 0);
			MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 95, rect.bottom - rect.top - 40, TRUE);

			MoveButton(hDlg, IDC_ARANGE, rect, 10, 80);
			MoveButton(hDlg, IDCANCEL, rect, 10, 50);
			MoveButton(hDlg, IDOK, rect, 10, 20);

			oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);

			ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT);



			ShowWindow(hItemEdit, SW_HIDE);
			SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

			//Create the columns in the list control
			//Name
			colNr.mask = LVCF_TEXT | LVCF_WIDTH;
			colNr.pszText = TEXT("Nr.");
			colNr.cchTextMax = 60;
			colNr.cx = 30;
			colNr.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 1, &colNr);

			//nr
			colAtom.mask = LVCF_TEXT | LVCF_WIDTH;
			colAtom.pszText = TEXT("Atom");
			colAtom.cchTextMax = 60;
			colAtom.cx = 40;
			colAtom.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 2, &colAtom);			

			//Low limit
			colXpos.mask = LVCF_TEXT | LVCF_WIDTH;
			colXpos.pszText = TEXT("Xpos.");
			colXpos.cchTextMax = 120;
			colXpos.cx = 60;
			colXpos.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 3, &colXpos);

			//Low limit
			colYpos.mask = LVCF_TEXT | LVCF_WIDTH;
			colYpos.pszText = TEXT("Ypos.");
			colYpos.cchTextMax = 120;
			colYpos.cx = 60;
			colYpos.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 4, &colYpos);

			//Value
			colZpos.mask = LVCF_TEXT | LVCF_WIDTH;
			colZpos.pszText = TEXT("Zpos.");
			colZpos.cchTextMax = 120;
			colZpos.cx = 60;
			colZpos.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 5, &colZpos);

			//Up limit
			colDW1Param.mask = LVCF_TEXT | LVCF_WIDTH;
			colDW1Param.pszText = TEXT("DW");
			colDW1Param.cchTextMax = 120;
			colDW1Param.cx = 40;
			colDW1Param.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 6, &colDW1Param);


			//plot in table
			lvTest.mask = LVIF_TEXT | LVIF_STATE;
			lvTest.iItem = 0;
			lvTest.pszText = pszNr;
			lvTest.iIndent = 0;
			lvTest.stateMask = 0;
			lvTest.state = 0;
			lvTest.iSubItem = 0;

			//Occupation
			for (j = thread.NBULK-1; j >= 0 ; j--){

				swprintf_s(pszNr, 100, L"%4d", j);
				//X
				AtoT(pszAtom, variable.ELEMENT[thread.TB[j]]);
				swprintf_s(pszXpos,	100, L"%6.4f", thread.XB[j]);
				//Y
				swprintf_s(pszYpos,	100, L"%6.4f", thread.YB[j]);
				//Z
				swprintf_s(pszZpos,	100, L"%6.4f", thread.ZB[j]);
				//DW1
				swprintf_s(pszDW1Param,	100, L"%d", thread.NDWB[j]);

				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText( hItemList, iIndex, 1, pszAtom);	
				//X
				ListView_SetItemText( hItemList, iIndex, 2, pszXpos);	
				//Y
				ListView_SetItemText( hItemList, iIndex, 3, pszYpos);	
				//Z
				ListView_SetItemText( hItemList, iIndex, 4, pszZpos);	
				//DW
				ListView_SetItemText( hItemList, iIndex, 5, pszDW1Param);
			}
		}

/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

		case WM_KEYDOWN://WM_CHAR:
							switch (wParam)
							{
							case VK_UP:
								nItem--;								
								break;
							}
							break;


		case WM_NOTIFY:

			case IDC_LIST1: 
			{
				LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;
					
                if(pnm->hdr.hwndFrom ==hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
                {
                    SetWindowLong(hDlg, 0, (LONG)ProcessCustomDrawBulk(lParam));
                    return TRUE;
                }
			}

			switch(((NMHDR *)lParam)->code)
			{
				case NM_CLICK://IDC_LIST1
					if (nItem < ListView_GetItemCount(hItemList))
					{
						if (nSubItem == 5)
							SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
						if (nSubItem == 2 || nSubItem == 3 || nSubItem == 4)
							SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
						if (nSubItem == 1){
							WCHAR temp_string[3];   //read line of text only 20 characters
							GetDlgItemText(hDlg, IDC_EDIT1, temp_string, 3); //STRING_SIZE only 20 characters!
							SetCell(hItemList, temp_string, nItem, nSubItem);
						}
					}
					OnClickList(hDlg, hItemEdit, (NMHDR*)lParam, &nItem, &nSubItem, 1, 100);
					SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));

				break;
			}
			break;

		case WM_COMMAND:
			switch LOWORD(wParam){

			case IDC_EDIT1:

				if (nItem<ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
				{
					if (nSubItem == 5)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
					if (nSubItem == 2 || nSubItem == 3 || nSubItem == 4)
						SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
					if (nSubItem == 1){
						WCHAR temp_string[3];   //read line of text only 3 characters
						GetDlgItemText(hDlg, IDC_EDIT1, temp_string, 3); //STRING_SIZE only 3 characters!
						SetCell(hItemList, temp_string, nItem, nSubItem);
					}
				}
				break;


				case IDOK:
					{
					nItem = 0; nSubItem = 0;
					CopyValuesFromBulkEditListBox(hItemList);

					//Add record to UndoRedo
					myUndoRedo.AddNewRecord(L"Edit bulk file");
					CheckBulEdit(hItemList);
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

				case IDCANCEL:
					{
					nItem = 0; nSubItem = 0;
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

				case IDC_ARANGE:
				{
					ArrangeAtomPositions(hItemList);

				}
				break;



				case IDC_BUTTON1:
					{
						HWND pwndCtrl = GetFocus();
						if (pwndCtrl == GetDlgItem (hDlg,IDC_EDIT1)){
							//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
							if(nSubItem==5)
								SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
							if(nSubItem==2 || nSubItem==3 || nSubItem==4)
								SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
							if(nSubItem==1){
								 WCHAR temp_string[3];   //read line of text only 20 characters
								 GetDlgItemText(hDlg, IDC_EDIT1, temp_string, 3); //STRING_SIZE only 20 characters!
								 SetCell(hItemList, temp_string, nItem, nSubItem);
							}
							SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
							ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
							InvalidateRect(hItemList, 0 ,0);
						}
					}
				break;
			}
		break;
	}
 return (INT_PTR)FALSE;

}

Thread_t tmpThr;
Variable_t tmpVar;
//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box. 
// 12.07.2014 added by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditDataFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	HWND  hItemEdit;
	LeftListLimit = 1;
	RightListLimit = 9;

	int j;
	static int ntot;
	static RECT rect;


	//list
	WCHAR pszNr[8];
	WCHAR pszh[30];
	WCHAR pszk[20];
	WCHAR pszl[20];
	WCHAR pszF[20];
	WCHAR pszdF[20];
	WCHAR pszLBragg[4];
	WCHAR pszEnergy[3];
	WCHAR pszFrac[1];
	WCHAR pszSub[4];

	LVCOLUMN		colNr;
	LVCOLUMN		colh;
	LVCOLUMN		colk;
	LVCOLUMN		coll;
	LVCOLUMN		colF;
	LVCOLUMN		coldF;
	LVCOLUMN		colLBrag;
	LVCOLUMN		colEnergr;
	LVCOLUMN		colFrac;
	LVCOLUMN		coSub;

	LVITEM			lvTest;
	UINT			iIndex = 0;

	hItemList = GetDlgItem(hDlg, IDC_LIST1);
	hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);

	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);



	switch (message){


	case WM_SIZE:
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 100, rect.bottom - rect.top - 15, TRUE);

		MoveButton(hDlg, IDCANCEL, rect, 10, 40);
		MoveButton(hDlg, IDOK, rect, 10, 10);
		MoveButton(hDlg, IDC_BUTTON3, rect, 10, 70);
		MoveButton(hDlg, IDC_BUTTON6, rect, 10, 100);
		MoveButton(hDlg, IDC_BUTTON9, rect, 10, 130);
		ShowWindow(hItemEdit, SW_HIDE);
		break;


	case WM_INITDIALOG:
	{

		CopyMemory(&tmpThr, &thread, sizeof(Thread_t));
		CopyMemory(&tmpVar, &variable, sizeof(Variable_t));

		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 100, rect.bottom - rect.top - 15, TRUE);

		MoveButton(hDlg, IDCANCEL, rect, 10, 40);
		MoveButton(hDlg, IDOK, rect, 10, 10);
		MoveButton(hDlg, IDC_BUTTON3, rect, 10, 70);
		MoveButton(hDlg, IDC_BUTTON6, rect, 10, 100);
		MoveButton(hDlg, IDC_BUTTON9, rect, 10, 130);
		oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);

		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		ShowWindow(hItemEdit, SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		//Create the columns in the list control
		//Name
		colNr.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr.pszText = TEXT("Nr.");
		colNr.cchTextMax = 60;
		colNr.cx = 30;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colNr);

		//h
		colh.mask = LVCF_TEXT | LVCF_WIDTH;
		colh.pszText = TEXT("h");
		colh.cchTextMax = 90;
		colh.cx = 60;
		colh.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 2, &colh);

		//k
		colk.mask = LVCF_TEXT | LVCF_WIDTH;
		colk.pszText = TEXT("k");
		colk.cchTextMax = 90;
		colk.cx = 60;
		colk.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 3, &colk);

		//l
		coll.mask = LVCF_TEXT | LVCF_WIDTH;
		coll.pszText = TEXT("l");
		coll.cchTextMax = 90;
		coll.cx = 60;
		coll.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 4, &coll);

		//F
		colF.mask = LVCF_TEXT | LVCF_WIDTH;
		colF.pszText = TEXT("F");
		colF.cchTextMax = 120;
		colF.cx = 70;
		colF.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 5, &colF);

		//dF
		coldF.mask = LVCF_TEXT | LVCF_WIDTH;
		coldF.pszText = TEXT("dF");
		coldF.cchTextMax = 120;
		coldF.cx = 70;
		coldF.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 6, &coldF);

		//Energy
		colEnergr.mask = LVCF_TEXT | LVCF_WIDTH;
		colEnergr.pszText = TEXT("Energy");
		colEnergr.cchTextMax = 30;
		colEnergr.cx = 20;
		colEnergr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 7, &colEnergr);

		//Subscale
		coSub.mask = LVCF_TEXT | LVCF_WIDTH;
		coSub.pszText = TEXT("Subscale");
		coSub.cchTextMax = 30;
		coSub.cx = 20;
		coSub.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 8, &coSub);

		//LBrag
		colLBrag.mask = LVCF_TEXT | LVCF_WIDTH;
		colLBrag.pszText = TEXT("LBragg");
		colLBrag.cchTextMax = 30;
		colLBrag.cx = 20;
		colLBrag.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 9, &colLBrag);

		//Frac
		colFrac.mask = LVCF_TEXT | LVCF_WIDTH;
		colFrac.pszText = TEXT("Frac");
		colFrac.cchTextMax = 30;
		colFrac.cx = 20;
		colFrac.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 10, &colFrac);


		//plot in table
		lvTest.mask = LVIF_TEXT | LVIF_STATE;
		lvTest.iItem = 0;
		lvTest.pszText = pszNr;
		lvTest.iIndent = 0;
		lvTest.stateMask = 0;
		lvTest.state = 0;
		lvTest.iSubItem = 0;

		//Occupation
		for (j = thread.NDAT - 1; j >= 0; --j){
			//Nr
			swprintf_s(pszNr, 100, L"%4d", j);
			//h
			swprintf_s(pszh, 100, L"%6.4f", thread.HDAT[j]);
			//k
			swprintf_s(pszk, 100, L"%6.4f", thread.KDAT[j]);
			//l
			swprintf_s(pszl, 100, L"%6.4f", thread.LDAT[j]);
			//F
			swprintf_s(pszF, 100, L"%6.4f", thread.FDAT[j]);
			//dF
			swprintf_s(pszdF, 100, L"%6.4f", variable.ERRDAT[j]);
			//Energy
			swprintf_s(pszEnergy, 3, L"%d", thread.ENERGY[j]);
			//Subscale
			swprintf_s(pszSub, 4, L"%d", thread.SUBSC[j]);
			//LBragg
			swprintf_s(pszLBragg, 4, L"%d", thread.LBR[j]);
			//Frac
			if (thread.FRAC[j])
				swprintf_s(pszFrac, 100, L"1");
			else
				swprintf_s(pszFrac, 100, L"0");

			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText(hItemList, iIndex, 1, pszh);
			//k
			ListView_SetItemText(hItemList, iIndex, 2, pszk);
			//l
			ListView_SetItemText(hItemList, iIndex, 3, pszl);
			//F
			ListView_SetItemText(hItemList, iIndex, 4, pszF);
			//dF
			ListView_SetItemText(hItemList, iIndex, 5, pszdF);
			//LBragg
			ListView_SetItemText(hItemList, iIndex, 6, pszEnergy);
			//LBragg
			ListView_SetItemText(hItemList, iIndex, 7, pszSub);
			//LBragg
			ListView_SetItemText(hItemList, iIndex, 8, pszLBragg);
			//LBragg
			ListView_SetItemText(hItemList, iIndex, 9, pszFrac);
		}

	}

		/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;


	case WM_NOTIFY:

	case IDC_LIST1:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

		if (pnm->hdr.hwndFrom == hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
		{
			SetWindowLong(hDlg, 0, (LONG)ProcessCustomDrawBulk(lParam));
			return TRUE;
		}
	}

		switch (((NMHDR *)lParam)->code)
		{
		case NM_CLICK://IDC_LIST1
			if (nItem < ListView_GetItemCount(hItemList))
			{
				if (nSubItem == 1 || nSubItem == 2 || nSubItem == 3 || nSubItem == 4 || nSubItem == 5 )
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				if (nSubItem == 6 || nSubItem == 7 || nSubItem == 8 || nSubItem == 9)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
			}
			OnClickList(hDlg, hItemEdit, (NMHDR*)lParam, &nItem, &nSubItem, 1, 100);
			SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));

			break;
		}
		break;

	case WM_COMMAND:
		switch LOWORD(wParam){

		case IDC_EDIT1:

			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				if (nSubItem == 1 || nSubItem == 2 || nSubItem == 3 || nSubItem == 4 || nSubItem == 5)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				if (nSubItem == 6 || nSubItem == 7 || nSubItem == 8 || nSubItem == 9)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
			}
			break;

		case IDOK:
		{
			nItem = 0; nSubItem = 0;
			CopyValuesFromDataEditListBox(hItemList);

			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Edit data file");

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
		{
			nItem = 0; nSubItem = 0;

			CopyMemory(&thread,&tmpThr,  sizeof(Thread_t));
			CopyMemory(&variable,&tmpVar,  sizeof(Variable_t));

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDC_BUTTON1:
		{
			HWND pwndCtrl = GetFocus();
			if (pwndCtrl == GetDlgItem(hDlg, IDC_EDIT1)){
				//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
				if (nSubItem == 1 || nSubItem == 2 || nSubItem == 3 || nSubItem == 4 || nSubItem == 5)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				if (nSubItem == 6 || nSubItem == 7 || nSubItem == 8 || nSubItem == 9)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);

				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				InvalidateRect(hItemList, 0, 0);
			}
		}
			break;

		case IDC_BUTTON3:
		{
			nItem = 0; nSubItem = 0;

			

			if (!variable.BRAGG_WEIGHT_DONE)// do it only ones
			{
				DialogBox(hInstance, MAKEINTRESOURCE(IDD_NEAR_BRAGG_INCREASE), hDlg, ErrorIncrease);
				for (int i = 0; i < thread.NDAT; ++i)
				{
					SetCell(hItemList, variable.COPYERRDAT[i], i, 5);
				}
			}
			else
				MessageBox(NULL, TEXT("Errors already increased."), NULL, MB_ICONERROR);

			variable.BRAGG_WEIGHT_DONE = TRUE;//myScene.flag indicating that error increase is done
			return (INT_PTR)FALSE;
		}

		//Find LBrags
		case IDC_BUTTON6:
		{
			nItem = 0; nSubItem = 0;
			WCHAR Tstr[20];
			calc.FindLBraggs(&thread);
			//HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
			//DialogBox(hInstance, MAKEINTRESOURCE(IDD_NEAR_BRAGG_INCREASE), hDlg, ErrorIncrease);
			for (int i = 0; i < thread.NDAT; ++i)
			{
				swprintf_s(Tstr, 20, L"%d", thread.LBR[i]);
				SetCell(hItemList, Tstr, i, 8);

				if(thread.FRAC[i]) SetCell(hItemList, L"1", i, 9);
				else
					SetCell(hItemList, L"0", i, 9);
			}

			return (INT_PTR)FALSE;
		}

		//Find LBrags
		case IDC_BUTTON9:
		{
			nItem = 0; nSubItem = 0;
			//myUndoRedo.AddNewRecord(L"Data manipulation");
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DATA_REDUCTION), hDlg, Data_reduction);
			
////////////////////////////////////////////////////////
			ListView_DeleteAllItems(hItemList);

			//plot in table
			lvTest.mask = LVIF_TEXT | LVIF_STATE;
			lvTest.iItem = 0;
			lvTest.pszText = pszNr;
			lvTest.iIndent = 0;
			lvTest.stateMask = 0;
			lvTest.state = 0;
			lvTest.iSubItem = 0;
			for (j = thread.NDAT - 1; j >= 0; --j) {
				//Nr
				swprintf_s(pszNr, 100, L"%4d", j);
				//h
				swprintf_s(pszh, 100, L"%6.4f", thread.HDAT[j]);
				//k
				swprintf_s(pszk, 100, L"%6.4f", thread.KDAT[j]);
				//l
				swprintf_s(pszl, 100, L"%6.4f", thread.LDAT[j]);
				//F
				swprintf_s(pszF, 100, L"%6.4f", thread.FDAT[j]);
				//dF
				swprintf_s(pszdF, 100, L"%6.4f", variable.ERRDAT[j]);
				//Energy
				swprintf_s(pszEnergy, 3, L"%d", thread.ENERGY[j]);
				//Subscale
				swprintf_s(pszSub, 4, L"%d", thread.SUBSC[j]);
				//LBragg
				swprintf_s(pszLBragg, 4, L"%d", thread.LBR[j]);
				//Frac
				if (thread.FRAC[j])
					swprintf_s(pszFrac, 100, L"1");
				else
					swprintf_s(pszFrac, 100, L"0");

				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText(hItemList, iIndex, 1, pszh);
				//k
				ListView_SetItemText(hItemList, iIndex, 2, pszk);
				//l
				ListView_SetItemText(hItemList, iIndex, 3, pszl);
				//F
				ListView_SetItemText(hItemList, iIndex, 4, pszF);
				//dF
				ListView_SetItemText(hItemList, iIndex, 5, pszdF);
				//LBragg
				ListView_SetItemText(hItemList, iIndex, 6, pszEnergy);
				//LBragg
				ListView_SetItemText(hItemList, iIndex, 7, pszSub);
				//LBragg
				ListView_SetItemText(hItemList, iIndex, 8, pszLBragg);
				//LBragg
				ListView_SetItemText(hItemList, iIndex, 9, pszFrac);
			}

			return (INT_PTR)FALSE;
		
		}


		}
		break;
	}
	return (INT_PTR)FALSE;

}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box. 
// 12.07.2014 added by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditEnergyF1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	HWND  hItemEdit;
	LeftListLimit = 1;
	RightListLimit = thread.NTYPES;

	int i, j;

	static RECT rect;

	//list
	WCHAR pszNr[8];
	WCHAR pszName[20];



	LVCOLUMN		colNr;
	LVCOLUMN		colElement[MAX_ELEMENTS+1];

	LVITEM			lvTest;
	UINT			iIndex = 0;

	hItemList = GetDlgItem(hDlg, IDC_LIST1);
	hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);

	switch (message){

	case WM_SIZE:
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 100, rect.bottom - rect.top - 15, TRUE);

		MoveButton(hDlg, IDCANCEL, rect, 10, 40);
		MoveButton(hDlg, IDOK, rect, 10, 10);

		ShowWindow(hItemEdit, SW_HIDE);
		break;


	case WM_INITDIALOG:
	{
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 100, rect.bottom - rect.top - 15, TRUE);

		MoveButton(hDlg, IDCANCEL, rect, 10, 40);
		MoveButton(hDlg, IDOK, rect, 10, 10);

		oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);

		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		ShowWindow(hItemEdit, SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		//Create the columns in the list control
		//Name
		colNr.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr.pszText = TEXT("F'");
		colNr.cchTextMax = 60;
		colNr.cx = 30;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colNr);
		
		for (j = 0; j < thread.NTYPES; ++j)
		{
			AtoT(pszName, variable.ELEMENT[j]);
			colElement[j].mask = LVCF_TEXT | LVCF_WIDTH;
			colElement[j].pszText = pszName;
			colElement[j].cchTextMax = 90;
			colElement[j].cx = 90;
			colElement[j].fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, j+1, &colElement[j]);
		}


		//plot in table
		lvTest.mask = LVIF_TEXT | LVIF_STATE;
			lvTest.iItem = 0;
			lvTest.pszText = pszNr;
			lvTest.iIndent = 0;
			lvTest.stateMask = 0;
			lvTest.state = 0;
			lvTest.iSubItem = 0;
		
		for (i = variable.ENERGYTOT; i >= 0; --i)
		{
			swprintf_s(pszNr, 7, L"%4d", i);
			lvTest.pszText = pszNr;
			iIndex = ListView_InsertItem(hItemList, &lvTest);

			for (j = 0; j < thread.NTYPES; ++j)
			{
				swprintf_s(pszName, 20, L"%6.4f", thread.F1[j][i]);
				ListView_SetItemText(hItemList, iIndex, j + 1, pszName);
			}
		}
	}

		/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;


	case WM_NOTIFY:

	case IDC_LIST1:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

		if (pnm->hdr.hwndFrom == hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
		{			
			SetWindowLong(hDlg, 0, (LONG)ProcessCustomDrawBulk(lParam));
			return TRUE;
		}
	}

		switch (((NMHDR *)lParam)->code)
		{
			
		case NM_CLICK://IDC_LIST1
			if (nItem < ListView_GetItemCount(hItemList))
			{
				if (nSubItem > 0 && nSubItem<RightListLimit+1)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
			}

			OnClickList(hDlg, hItemEdit, (NMHDR*)lParam, &nItem, &nSubItem, 1, 100);
			SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
			break;
		}
		break;

	case WM_COMMAND:
		switch LOWORD(wParam){

		case IDC_EDIT1:

			if ( nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				if (nSubItem > 0 && nSubItem<RightListLimit + 1)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
			}
			break;

		case IDOK:
		{
			nItem = 0; nSubItem = 0;

			for (i = 1; i <variable.ENERGYTOT+1; ++i)
			{
				for (j = 0; j < thread.NTYPES; ++j)
				{
					//F1
					thread.F1[j][i] = GetItemReal(hItemList, i, j+1);
				}
			}

			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Edit energy file");

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
				if (nSubItem > 0 && nSubItem<RightListLimit + 1)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);

				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				InvalidateRect(hItemList, 0, 0);
			}
		}
			break;


		}
		break;
	}
	return (INT_PTR)FALSE;

}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box. 
// 12.07.2014 added by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditEnergyF2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	HWND  hItemEdit;
	LeftListLimit = 1;
	RightListLimit = thread.NTYPES;

	int i, j;

	static RECT rect;

	//list
	WCHAR pszNr[8];
	WCHAR pszName[20];



	LVCOLUMN		colNr;
	LVCOLUMN		colElement[MAX_ELEMENTS + 1];

	LVITEM			lvTest;
	UINT			iIndex = 0;

	hItemList = GetDlgItem(hDlg, IDC_LIST1);
	hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);

	switch (message){

	case WM_SIZE:
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 100, rect.bottom - rect.top - 15, TRUE);

		MoveButton(hDlg, IDCANCEL, rect, 10, 40);
		MoveButton(hDlg, IDOK, rect, 10, 10);

		ShowWindow(hItemEdit, SW_HIDE);
		break;


	case WM_INITDIALOG:
	{
		GetClientRect(hDlg, &rect);
		InvalidateRect(hDlg, 0, 0);
		MoveWindow(hItemList, rect.left, rect.top + 11, rect.right - rect.left - 100, rect.bottom - rect.top - 15, TRUE);

		MoveButton(hDlg, IDCANCEL, rect, 10, 40);
		MoveButton(hDlg, IDOK, rect, 10, 10);

		oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);

		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		ShowWindow(hItemEdit, SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		//Create the columns in the list control
		//Name
		colNr.mask = LVCF_TEXT | LVCF_WIDTH;
		colNr.pszText = TEXT("F''");
		colNr.cchTextMax = 60;
		colNr.cx = 30;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colNr);

		for (j = 0; j < thread.NTYPES; ++j)
		{
			AtoT(pszName, variable.ELEMENT[j]);
			colElement[j].mask = LVCF_TEXT | LVCF_WIDTH;
			colElement[j].pszText = pszName;
			colElement[j].cchTextMax = 90;
			colElement[j].cx = 90;
			colElement[j].fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, j + 1, &colElement[j]);
		}


		//plot in table
		lvTest.mask = LVIF_TEXT | LVIF_STATE;
		lvTest.iItem = 0;
		lvTest.pszText = pszNr;
		lvTest.iIndent = 0;
		lvTest.stateMask = 0;
		lvTest.state = 0;
		lvTest.iSubItem = 0;

		for (i = variable.ENERGYTOT; i >= 0; --i)
		{
			swprintf_s(pszNr, 7, L"%4d", i);
			lvTest.pszText = pszNr;
			iIndex = ListView_InsertItem(hItemList, &lvTest);

			for (j = 0; j < thread.NTYPES; ++j)
			{
				swprintf_s(pszName, 20, L"%6.4f", thread.F2[j][i]);
				ListView_SetItemText(hItemList, iIndex, j + 1, pszName);
			}
		}
	}

		/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;


	case WM_NOTIFY:

	case IDC_LIST1:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

		if (pnm->hdr.hwndFrom == hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
		{
			SetWindowLong(hDlg, 0, (LONG)ProcessCustomDrawBulk(lParam));
			return TRUE;
		}
	}

		switch (((NMHDR *)lParam)->code)
		{

		case NM_CLICK://IDC_LIST1
			if (nItem < ListView_GetItemCount(hItemList))
			{
				if (nSubItem > 0 && nSubItem<RightListLimit + 1)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
			}

			OnClickList(hDlg, hItemEdit, (NMHDR*)lParam, &nItem, &nSubItem, 1, 100);
			SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
			break;
		}
		break;

	case WM_COMMAND:
		switch LOWORD(wParam){

		case IDC_EDIT1:

			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				if (nSubItem > 0 && nSubItem<RightListLimit + 1)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
			}
			break;

		case IDOK:
		{
			nItem = 0; nSubItem = 0;

			for (i = 1; i <variable.ENERGYTOT+1; ++i)
			{
				for (j = 0; j < thread.NTYPES; ++j)
				{
					//F1
					thread.F2[j][i] = GetItemReal(hItemList, i, j + 1);
				}
			}

			//Add record to UndoRedo
			myUndoRedo.AddNewRecord(L"Edit F2 coeficient" );

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
				if (nSubItem > 0 && nSubItem<RightListLimit + 1)
					SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);

				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				InvalidateRect(hItemList, 0, 0);
			}
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
INT_PTR CALLBACK AtomProperities(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	//static int nItem, nSubItem;
	static int ntot;
	LeftListLimit = 2;
	RightListLimit = 2;
	static COLORREF tmpRGBbackground = 0;
	LVCOLUMN		colNr;
	LVCOLUMN		colElement;
	LVCOLUMN		colAtRadius;
	LVCOLUMN		colBonded;

	hItemList = GetDlgItem(hDlg, IDC_LIST2);
	HWND  hItemEdit = GetDlgItem(hDlg, IDC_EDIT1);

	switch (message){
	case WM_INITDIALOG:

		ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, 0, 0);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create the columns in the list control
		ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//Repleasing the standard mesaage handling of EditBoX with the new one important for substracting mesages related to keys
		oldEditProc = (WNDPROC)SetWindowLongPtr(hItemEdit, GWLP_WNDPROC, (LONG_PTR)subEditProc);


		//Name
		colNr.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colNr.pszText = TEXT("Nr");
		colNr.cchTextMax = 100;
		colNr.cx = 65;
		colNr.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 0, &colNr);

		//Element
		colElement.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colElement.pszText = TEXT("Element");
		colElement.cchTextMax = 40;
		colElement.cx = 70;
		colElement.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 1, &colElement);

		//Atomic radius
		colAtRadius.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colAtRadius.pszText = TEXT("Van der Waals radius");
		colAtRadius.cchTextMax = 100;
		colAtRadius.cx = 70;
		colAtRadius.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 2, &colAtRadius);

		// 
		colBonded.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		colBonded.pszText = TEXT("Color");
		colBonded.cchTextMax = 40;
		colBonded.cx = 40;
		colBonded.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hItemList, 3, &colBonded);

		//Set values in table
		FillAtomProperitiesList(hItemList);

		ShowWindow(hDlg, SW_NORMAL);
		UpdateWindow(hDlg);
		break;


		/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

		//http://www.codeproject.com/Articles/2890/Using-ListView-control-under-Win32-API
	case WM_NOTIFY:

	case IDC_LIST2:
	{
		LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

		if (pnm->hdr.hwndFrom == hItemList &&pnm->hdr.code == NM_CUSTOMDRAW)
		{
			SetWindowLong(hDlg, 0, (LONG)ProcessCustomDraw(lParam));
			return TRUE;
		}
	}

		switch (((NMHDR *)lParam)->code)
		{

		case NM_CLICK://IDC_LIST1
			//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				//ShowWindow(hItemEdit, SW_SHOW);
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
			}

			OnClickList(hDlg, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)lParam, &nItem, &nSubItem, 2, 2);
			OnClickListColor(hDlg, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)lParam, &nItem, &nSubItem, &tmpRGBbackground);

			//Make selction in Edit1
			SendMessage(hItemEdit, EM_SETSEL, 0, MAKELONG(0xffff, 0xffff));
			if (nItem > ListView_GetItemCount(hItemList) || nSubItem < LeftListLimit || nSubItem > RightListLimit)
			{
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(hItemEdit, SW_HIDE);
			}
			break;
			break;
		}
		break;

	case WM_COMMAND:
		switch LOWORD(wParam)
		{

		case IDC_EDIT1:

			if (nItem < ListView_GetItemCount(hItemList) && nSubItem >= LeftListLimit && nSubItem <= RightListLimit)
			{
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
			}
			break;

		case IDOK:
		{
			CopyValuesFromAtomProperitiesListBox(hItemList);
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
				SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
				InvalidateRect(hDlg, 0, 0);
			}
		}
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void FillAtomProperitiesList(HWND  hItemList){

	int i;
	WCHAR Tstr[256];

	LVITEM			LvItem;
	ListView_DeleteAllItems(hItemList);

	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of test

	for (i = 0; i < 98; i++){
		LvItem.iItem = i;

		swprintf_s(Tstr, 256, L"%4d", i + 1);
		LvItem.pszText = Tstr;
		LvItem.iSubItem = 0;
		SendMessage(hItemList, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send to the Listview
	}

	LvItem.mask = LVIF_TEXT | LVIF_STATE | LVCF_SUBITEM;
	for (i = 0; i < 98; i++){
		LvItem.iItem = i;

		AtoT(Tstr, elements[i]);
		LvItem.pszText = Tstr;
		LvItem.iSubItem = 1;
		SendMessage(hItemList, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter etxt to SubItems

		swprintf_s(Tstr, 256, L"%3.4f", myScene.AtProp.AtomRadius[i + 1]);
		LvItem.pszText = Tstr;
		LvItem.iSubItem = 2;
		SendMessage(hItemList, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter etxt to SubItems

		LvItem.pszText = L"";
		LvItem.iSubItem = 3;
		SendMessage(hItemList, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter etxt to SubItems
	}
}

LRESULT ProcessCustomDraw(LPARAM lParam)
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
		/*
			   case CDDS_ITEMPREPAINT: //Before an item is drawn
			   if (((int)lplvcd->nmcd.dwItemSpec%2)==0)
			   {
			   //customize item appearance
			   lplvcd->clrText   = RGB(255,0,0);
			   lplvcd->clrTextBk = RGB(200,200,200);
			   return CDRF_NEWFONT;
			   }
			   break;
			   */
		//Before a subitem is drawn
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		//        if (((int)lplvcd->nmcd.dwItemSpec)==iRow)

	{
		int i = (int)lplvcd->nmcd.dwItemSpec + 1;
		if (3 == lplvcd->iSubItem) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(myScene.AtProp.AtomColor[i][0] * 255, myScene.AtProp.AtomColor[i][1] * 255, myScene.AtProp.AtomColor[i][2] * 255);

			//To set a custom font:
			//SelectObject(lplvcd->nmcd.hdc, 
			//    <your custom HFONT>);

			return CDRF_NEWFONT;
		}
		else{
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(255, 255, 255);
			return CDRF_NEWFONT;
		}

	}
	}
	return CDRF_DODEFAULT;
}

LRESULT ProcessCustomDrawFitting(LPARAM lParam, bool*RowCollor)
{
	int i;

	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT: //Before the paint cycle begins
		//request notifications for individual listview items
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_POSTPAINT: //After the paint cycle ends
		//request notifications for individual listview items

		return CDRF_NOTIFYITEMDRAW;


		/*
			   case CDDS_ITEMPREPAINT: //Before an item is drawn
			   {
			   return CDRF_NOTIFYSUBITEMDRAW;
			   }
			   break;

			   */
	case CDDS_ITEMPREPAINT: //Before an item is drawn

		i = (int)lplvcd->nmcd.dwItemSpec;
		if (RowCollor[i] == false)
		{
			//customize item appearance
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(255, 100, 100);//background color of wrong limit range
			return CDRF_NEWFONT;

		}


		lplvcd->clrText = RGB(0, 0, 0);
		lplvcd->clrTextBk = RGB(255, 255, 255);
		//return CDRF_NEWFONT;
		return CDRF_NOTIFYSUBITEMDRAW;
		break;
		/*
				case CDDS_ITEMPOSTPAINT: //After an item is drawn

				i = (int)lplvcd->nmcd.dwItemSpec;
				if (RowCollor[i] == false)
				{
				//customize item appearance
				lplvcd->clrText = RGB(0, 0, 0);
				lplvcd->clrTextBk = RGB(255, 0, 0);
				return CDRF_NEWFONT;

				}


				lplvcd->clrText = RGB(0, 0, 0);
				lplvcd->clrTextBk = RGB(255, 255, 255);
				//return CDRF_NEWFONT;
				return CDRF_NOTIFYSUBITEMDRAW;
				break;
				*/

		//Before a subitem is drawn
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	{
		i = (int)lplvcd->iSubItem;

		if (i == 1) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(100, 255, 128);
			return CDRF_NEWFONT;
		}
		if (i == 3) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(250, 252, 128);
			return CDRF_NEWFONT;
		}

		lplvcd->clrText = RGB(0, 0, 0);
		lplvcd->clrTextBk = RGB(255, 255, 255);
		return CDRF_NEWFONT;
	}

	}
	return CDRF_DODEFAULT;
}

LRESULT ProcessCustomDrawBulk(LPARAM lParam)
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
		int i = (int)lplvcd->iSubItem;
		if (i == 5) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(250, 252, 228);
			return CDRF_NEWFONT;
		}

		lplvcd->clrText = RGB(0, 0, 0);
		lplvcd->clrTextBk = RGB(255, 255, 255);
		return CDRF_NEWFONT;
	}
	}
	return CDRF_DODEFAULT;
}

LRESULT ProcessCustomDrawFit(LPARAM lParam)
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
		/*
			   case CDDS_ITEMPREPAINT: //Before an item is drawn
			   if (((int)lplvcd->nmcd.dwItemSpec%2)==0)
			   {
			   //customize item appearance
			   lplvcd->clrText   = RGB(255,0,0);
			   lplvcd->clrTextBk = RGB(200,200,200);
			   return CDRF_NEWFONT;
			   }
			   break;
			   */
		//Before a subitem is drawn
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		//        if (((int)lplvcd->nmcd.dwItemSpec)==iRow)

	{
		int i = (int)lplvcd->iSubItem;
		if (i == 4 || i == 9 || i == 14) //Color of xyz position
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(250, 252, 218);
			return CDRF_NEWFONT;
		}
		if (i == 3 || i == 5 || i == 8 || i == 10 || i == 13 || i == 15) //Color of constants
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(210, 210, 255);
			return CDRF_NEWFONT;
		}


		if (i == 6 || i == 11 || i == 16) //Color of parameters 1
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(255, 210, 220);
			return CDRF_NEWFONT;
		}

		if (i == 17) //Color DW1
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(196, 255, 150);
			return CDRF_NEWFONT;
		}
		if (i == 18) //Color of DW2
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(196, 255, 200);
			return CDRF_NEWFONT;
		}
		if (i == 19) //Color of Occ
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(196, 255, 255);
			return CDRF_NEWFONT;
		}

		lplvcd->clrText = RGB(0, 0, 0);
		lplvcd->clrTextBk = RGB(255, 255, 255);
		return CDRF_NEWFONT;

	}
	}
	return CDRF_DODEFAULT;
}

LRESULT ProcessCustomDrawFitMolecule(LPARAM lParam)
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
		/*
		case CDDS_ITEMPREPAINT: //Before an item is drawn
		if (((int)lplvcd->nmcd.dwItemSpec%2)==0)
		{
		//customize item appearance
		lplvcd->clrText   = RGB(255,0,0);
		lplvcd->clrTextBk = RGB(200,200,200);
		return CDRF_NEWFONT;
		}
		break;
		*/
		//Before a subitem is drawn
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		//        if (((int)lplvcd->nmcd.dwItemSpec)==iRow)

	{
		int i = (int)lplvcd->iSubItem;
		if (i == 2 || i == 4 || i == 6) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(250, 252, 218);
			return CDRF_NEWFONT;
		}
		if (i == 3 || i == 5 || i == 7) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(255, 221, 186);
			return CDRF_NEWFONT;
		}


		if (i == 8) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(255, 210, 220);
			return CDRF_NEWFONT;
		}

		if (i == 9) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(196, 255, 197);
			return CDRF_NEWFONT;
		}
		if (i == 10) //Color of atoms
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(197, 254, 254);
			return CDRF_NEWFONT;
		}
		if (i == 11) //Color Rot x
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(210, 219, 255);
			return CDRF_NEWFONT;
		}
		if (i == 12) //Color Rot y
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(200, 219, 255);
			return CDRF_NEWFONT;
		}
		if (i == 13) //Color of Rot z
		{
			//customize subitem appearance for column 0
			lplvcd->clrText = RGB(0, 0, 0);
			lplvcd->clrTextBk = RGB(180, 219, 255);
			return CDRF_NEWFONT;
		}

		lplvcd->clrText = RGB(0, 0, 0);
		lplvcd->clrTextBk = RGB(255, 255, 255);
		return CDRF_NEWFONT;

	}
	}
	return CDRF_DODEFAULT;
}

void OnClickListColor(HWND hDlg, HWND hWndEditBox, NMHDR* pNMHDR, int *nItem, int *nSubItem, COLORREF *tmpRGBbackground)
{
	//    InvalidateRect(hWndListBox, 0 ,0);

	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	//get the row number
	*nItem = temp->iItem;
	//get the column number
	*nSubItem = temp->iSubItem;
	//Skip the second column this are names of parameters
	if (*nSubItem < 3 || *nItem == -1)
		return;

	CHOOSECOLOR cc;                 // common dialog box structure 

	COLORREF BackgroundColor[16]; // array of custom colors
	//HBRUSH hbrush;                  // brush handle
	int i = *nItem + 1;
	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hDlg;
	cc.lpCustColors = (LPDWORD)BackgroundColor;
	cc.rgbResult = RGB(myScene.AtProp.AtomColor[i][0] * 255, myScene.AtProp.AtomColor[i][1] * 255, myScene.AtProp.AtomColor[i][2] * 255);
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE){

		myScene.AtProp.AtomColor[i][0] = 1.0f / 255 * GetRValue(cc.rgbResult);
		myScene.AtProp.AtomColor[i][1] = 1.0f / 255 * GetGValue(cc.rgbResult);
		myScene.AtProp.AtomColor[i][2] = 1.0f / 255 * GetBValue(cc.rgbResult);
	}

}

void CopyValuesFromAtomProperitiesListBox(HWND hItemList){
 int i;

	for (i = 0; i < 98; i++){
		myScene.AtProp.AtomRadius[i+1] = (float)GetItemReal(hItemList, i, 2);
	}
}

void SetAutoLimits(HWND hItemList){

	int j;
	int TOTAL = 0;
	double value;
	double ScaleIncrease = 0.5;
	double DisplIncrease = 0.3;
	double temp;
	//                       item,  subitem
	//Scale 1
	value = GetItemReal(hItemList, TOTAL, 3);
	SetCell(hItemList, 0., TOTAL, 2); SetCell(hItemList, value + 50, TOTAL, 4);
	if (value < 0) value = 0;
	SetCell(hItemList, value, TOTAL, 3);
	TOTAL++;
	//Scale 2
	value = GetItemReal(hItemList, TOTAL, 3);
	SetCell(hItemList, 0., TOTAL, 2); SetCell(hItemList, value + value*ScaleIncrease, TOTAL, 4);
	if (value < 0) value = 0;
	SetCell(hItemList, value, TOTAL, 3);
	TOTAL++;
	//Beta
	value = GetItemReal(hItemList, TOTAL, 3);
	if (value > .5) temp = value*1.1;
	else temp = .5;
	SetCell(hItemList, 0., TOTAL, 2); SetCell(hItemList, temp, TOTAL, 4);
	if (value < 0) value = 0;
	SetCell(hItemList, value, TOTAL, 3);
	TOTAL++;
	//Surf. Frac.
	value = GetItemReal(hItemList, TOTAL, 3);
	SetCell(hItemList, 0., TOTAL, 2); SetCell(hItemList, 1., TOTAL, 4);
	if (value < 0) value = 0;
	SetCell(hItemList, value, TOTAL, 3);
	TOTAL++;

	//Displacement
	for (j = TOTAL; j < TOTAL + variable.NDISTOT; j++){
		value = GetItemReal(hItemList, j, 3);
		if (value < 0.1) {
			SetCell(hItemList, value - 0.1, j, 2); SetCell(hItemList, value + 0.1, j, 4);
		}
		else
			SetCell(hItemList, value - abs(value)*DisplIncrease, j, 2); SetCell(hItemList, value + abs(value)*DisplIncrease, j, 4);
	}
	TOTAL += variable.NDISTOT;

	//DebWall
	for (j = TOTAL; j<TOTAL + thread.NDWTOT; j++){
		value = GetItemReal(hItemList, j, 3);
		temp = 0;
		SetCell(hItemList, temp, j, 2);
		if (value >5.) temp = value*1.5;
		else temp = 5.;
		SetCell(hItemList, temp, j, 4);
		if (value < 0) value = 0;
		SetCell(hItemList, value, j, 3);
	}
	TOTAL += thread.NDWTOT;

	//DebWall2
	for (j = TOTAL; j<TOTAL + thread.NDWTOT2; j++){
		value = GetItemReal(hItemList, j, 3);
		temp = 0;

		SetCell(hItemList, temp, j, 2);
		if (value >5.) temp = value*1.5;
		else temp = 5.;
		SetCell(hItemList, temp, j, 4);
		if (value < 0) value = 0;
		SetCell(hItemList, value, j, 3);
	}
	TOTAL += thread.NDWTOT2;
	//Occ

	for (j = TOTAL; j < TOTAL + variable.NOCCTOT; j++){
		value = GetItemReal(hItemList, j, 3);
		SetCell(hItemList, 0., j, 2); SetCell(hItemList, 1., j, 4);
		if (value < 0) value = 0;
		SetCell(hItemList, value, j, 3);
	}

	TOTAL += variable.NOCCTOT;
	//Occ2


		for (j = TOTAL; j < TOTAL + variable.NOCCTOT2; j++) {
			value = GetItemReal(hItemList, j, 3);
			SetCell(hItemList, 0., j, 2); SetCell(hItemList, 1., j, 4);
			if (value < 0) value = 0;
			SetCell(hItemList, value, j, 3);
		}
	TOTAL += variable.NOCCTOT2;

	//Nfactor
	if (variable.NFACTOR > 0)
	{
		for (j = TOTAL; j < TOTAL + variable.NFACTOR; j++) {
			value = GetItemReal(hItemList, j, 3);
			SetCell(hItemList, 0., j, 2); SetCell(hItemList, 1., j, 4);
			if (value < 0) value = 0;
			SetCell(hItemList, value, j, 3);
		}
	}
}

void SetCALL(HWND hItemList){

	int j;
	int TOTAL = 0;
	double value, LeftLim, RightLim;

	//Scale 1
	if (ListView_GetCheckState(hItemList, TOTAL) == TRUE){
		LeftLim = GetItemReal(hItemList, TOTAL, 2);
		RightLim = GetItemReal(hItemList, TOTAL, 4);
		value = (RightLim + LeftLim) / 2.;
		SetCell(hItemList, value, TOTAL, 3);
	}
	TOTAL++;

	//Scale 2
	if (ListView_GetCheckState(hItemList, TOTAL) == TRUE){
		LeftLim = GetItemReal(hItemList, TOTAL, 2);
		RightLim = GetItemReal(hItemList, TOTAL, 4);
		value = (RightLim + LeftLim) / 2;
		SetCell(hItemList, value, TOTAL, 3);
	}
	TOTAL++;

	//Beta
	if (ListView_GetCheckState(hItemList, TOTAL) == TRUE){
		LeftLim = GetItemReal(hItemList, TOTAL, 2);
		RightLim = GetItemReal(hItemList, TOTAL, 4);
		value = (RightLim + LeftLim) / 2;
		SetCell(hItemList, value, TOTAL, 3);
	}
	TOTAL++;

	//Displacement
	for (j = TOTAL; j < TOTAL + variable.NDISTOT; j++){
		if (ListView_GetCheckState(hItemList, j) == TRUE){
			LeftLim = GetItemReal(hItemList, j, 2);
			RightLim = GetItemReal(hItemList, j, 4);
			value = (RightLim + LeftLim) / 2;
			SetCell(hItemList, value, j, 3);
		}
	}
	TOTAL += variable.NDISTOT;

	//DebWall
	for (j = TOTAL; j < TOTAL + thread.NDWTOT; j++){
		if (ListView_GetCheckState(hItemList, j) == TRUE){
			LeftLim = GetItemReal(hItemList, j, 2);
			RightLim = GetItemReal(hItemList, j, 4);
			value = (RightLim + LeftLim) / 2;
			SetCell(hItemList, value, j, 3);
		}
	}
	TOTAL += thread.NDWTOT;

	//DebWall2
	for (j = TOTAL; j < TOTAL + thread.NDWTOT2; j++){
		if (ListView_GetCheckState(hItemList, j) == TRUE){
			LeftLim = GetItemReal(hItemList, j, 2);
			RightLim = GetItemReal(hItemList, j, 4);
			value = (RightLim + LeftLim) / 2;
			SetCell(hItemList, value, j, 3);
		}
	}
	TOTAL += thread.NDWTOT2;
	//Occ

	for (j = TOTAL; j < TOTAL + variable.NOCCTOT; j++){
		if (ListView_GetCheckState(hItemList, j) == TRUE){
			LeftLim = GetItemReal(hItemList, j, 2);
			RightLim = GetItemReal(hItemList, j, 4);
			value = (RightLim + LeftLim) / 2;
			SetCell(hItemList, value, j, 3);
		}
	}

	TOTAL += variable.NOCCTOT;
	//domain Occ

	for (j = TOTAL; j < TOTAL + thread.NDOMAIN; j++){
		if (ListView_GetCheckState(hItemList, j) == TRUE){
			LeftLim = GetItemReal(hItemList, j, 2);
			RightLim = GetItemReal(hItemList, j, 4);
			value = (RightLim + LeftLim) / 2;
			SetCell(hItemList, value, j, 3);
		}
	}
}

bool CheckFitEdit(HWND hItemList){

	int j;
	int parameter_max = 0;
	int temp;
	bool OrderTest[MAXPAR + 1];
	WCHAR text[100];

	for (j = 0; j <= MAXPAR; j++)
		OrderTest[j] = false;

	//Displacement
	for (j = 0; j<thread.NSURF; ++j){
		temp = (int)GetItemReal(hItemList, j, 4);
		if (temp>parameter_max && parameter_max<MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

		temp = (int)GetItemReal(hItemList, j, 6);
		if (temp> parameter_max && parameter_max<MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

		temp = (int)GetItemReal(hItemList, j, 9);
		if (temp> parameter_max && parameter_max<MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

		temp = (int)GetItemReal(hItemList, j, 11);
		if (temp> parameter_max && parameter_max<MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

		temp = (int)GetItemReal(hItemList, j, 14);
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

		temp = (int)GetItemReal(hItemList, j, 16);
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 0; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing displacement parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return false;
		}
	variable.NDISTOT = parameter_max;
	parameter_max = 0;

	for (j = 0; j <= MAXPAR; j++)
		OrderTest[j] = false;


	//DebWall in Bulk file
	for (j = 0; j<thread.NBULK; ++j){
		temp = thread.NDWB[j];
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	//DebWall
	for (j = 0; j<thread.NSURF; j++){
		temp = (int)GetItemReal(hItemList, j, 17);
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; j++)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing DW1 parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return false;
		}

	thread.NDWTOT = parameter_max;
	parameter_max = 0;

	for (j = 1; j <= MAXPAR; j++)
		OrderTest[j] = false;

	//DebWall2
	for (j = 0; j<thread.NSURF; j++){
		temp = (int)GetItemReal(hItemList, j, 18);
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; j++)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing DW2 parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return false;
		}
	thread.NDWTOT2 = parameter_max;
	parameter_max = 0;

	for (j = 0; j <= MAXPAR; j++)
		OrderTest[j] = false;

	//Occ
	for (j = 0; j<thread.NSURF; j++){
		temp = (int)abs(GetItemReal(hItemList, j, 19));
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; j++)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing Occ parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return false;
		}
	variable.NOCCTOT = parameter_max;

	return true;
}

void CheckFitMoleculeEdit(HWND hItemList){

	int j;
	int parameter_max = 0;
	int temp;
	bool OrderTest[MAXPAR + 1];
	WCHAR text[100];

	for (j = 0; j <= MAXPAR; j++)
		OrderTest[j] = false;

	//Displacement
	for (j = 0; j<myImport.TotalMolecules; ++j){
		temp = (int)GetItemReal(hItemList, j, 3);
		if (temp>parameter_max && parameter_max<MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

		temp = (int)GetItemReal(hItemList, j, 5);
		if (temp> parameter_max && parameter_max<MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

		temp = (int)GetItemReal(hItemList, j, 7);
		if (temp> parameter_max && parameter_max<MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;

	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing displacement parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}
	variable.NDISTOT = parameter_max;
	parameter_max = 0;

	//Reset
	for (j = 0; j <= MAXPAR; j++)
		OrderTest[j] = false;


	//DebWall in Bulk file
	for (j = 0; j<thread.NBULK; ++j){
		temp = thread.NDWB[j];
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	//DebWall
	for (j = 0; j<myImport.TotalMolecules; ++j){
		temp = (int)GetItemReal(hItemList, j, 8);
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing DW1 parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}

	thread.NDWTOT = parameter_max;
	parameter_max = 0;

	for (j = 1; j <= MAXPAR; ++j)
		OrderTest[j] = false;

	//DebWall2
	for (j = 0; j<myImport.TotalMolecules; ++j){
		temp = (int)GetItemReal(hItemList, j, 9);
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing DW2 parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}
	thread.NDWTOT2 = parameter_max;
	parameter_max = 0;

	for (j = 0; j <= MAXPAR; ++j)
		OrderTest[j] = false;

	//Occ
	for (j = 0; j<myImport.TotalMolecules; j++){
		temp = (int)abs(GetItemReal(hItemList, j, 10));
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing Occ parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}
	variable.NOCCTOT = parameter_max;
	parameter_max = 0;

	for (j = 0; j <= MAXPAR; ++j)
		OrderTest[j] = false;

	//Rot x
	for (j = 0; j<myImport.TotalMolecules; ++j){
		temp = (int)abs(GetItemReal(hItemList, j, 11));
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing Rotation X parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}
	variable.ROTATIONTOTX = parameter_max;
	parameter_max = 0;

	for (j = 0; j <= MAXPAR; ++j)
		OrderTest[j] = false;

	//Rot y
	for (j = 0; j<myImport.TotalMolecules; ++j){
		temp = (int)abs(GetItemReal(hItemList, j, 12));
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing Rotation Y parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}
	variable.ROTATIONTOTY = parameter_max;
	parameter_max = 0;

	for (j = 0; j <= MAXPAR; ++j)
		OrderTest[j] = false;

	//Rot z
	for (j = 0; j<myImport.TotalMolecules; ++j){
		temp = (int)abs(GetItemReal(hItemList, j, 13));
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing Rotation Z parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}
	variable.ROTATIONTOTZ = parameter_max;
	parameter_max = 0;

	swprintf_s(text, L"All parameters are correctly set.");
	MessageBox(NULL, text, TEXT(""), MB_OK);
	
}

void CheckBulEdit(HWND hItemList){

	int j;
	int parameter_max = 0;
	int temp;
	bool OrderTest[MAXPAR + 1];
	WCHAR text[100];

	for (j = 0; j <= MAXPAR; j++)
		OrderTest[j] = false;


	//DebWall in Bulk file
	for (j = 0; j<thread.NBULK; ++j){
		temp = (int)GetItemReal(hItemList, j, 5);
		if (temp> parameter_max && parameter_max < MAXPAR)
			parameter_max = temp;
		OrderTest[temp] = true;
	}

	for (j = 1; j <= parameter_max; ++j)
		if (OrderTest[j] == false) {
		swprintf_s(text, L"There is missing DW1 parameter with nr. %d", j);
		MessageBox(NULL, text, NULL, MB_ICONWARNING);
		return;
		}

	//It is just a test. The total DW parameter list depends also on fit file
	if (thread.NDWTOT < 1) thread.NDWTOT = parameter_max;
	parameter_max = 0;

	swprintf_s(text, L"All parameters are correctly set.");
	MessageBox(NULL, text, TEXT(""), MB_OK);
}

bool PerformRangeTest(HWND hItemList, int row){

	bool test = true;
	double value, LeftLim, RightLim;
	WCHAR mesage[64];

	swprintf_s(mesage, 64, L"");

	//Scale 1
	LeftLim = GetItemReal(hItemList, row, 2);
	value = GetItemReal(hItemList, row, 3);
	RightLim = GetItemReal(hItemList, row, 4);
	BOOL res = ListView_GetCheckState(hItemList, row);

	if (LeftLim <= value && value <= RightLim){
		swprintf_s(mesage, 64, L"range ok");
	}
	else	if (res == TRUE) test = false;
	
	if (LeftLim > value){
		swprintf_s(mesage, 64, L"Left limit to high");
		if (res == TRUE) test = false;
	}
	if (RightLim < value){
		swprintf_s(mesage, 64, L"Right limit to low"); 
		if (res == TRUE) test = false;
	}
	if (value > 1000){
		swprintf_s(mesage, 64, L"Value is really high");
		if (res == TRUE) test = false;
	}
	if (value < -1000){
		swprintf_s(mesage, 64, L"Value is really low"); 
		if (res == TRUE) test = false;
	}	
	SetCell(hItemList, mesage, row, 5);

	return test;
}

bool CheckRange(HWND hItemList){
	int j;
	int TOTAL = 0;
	bool test = true;

	//Scale 1
	if (!PerformRangeTest(hItemList, TOTAL))test = false;
	TOTAL++;

	//Subscale
	for (j = TOTAL; j < TOTAL + variable.NSUBSCALETOT; j++)
		if (!PerformRangeTest(hItemList, j))test = false;

	TOTAL += variable.NSUBSCALETOT;

	//Beta
	if (!PerformRangeTest(hItemList, TOTAL))test = false;
	TOTAL++;

	//Surf. Frac.
	if (!PerformRangeTest(hItemList, TOTAL))test = false;
	TOTAL++;

	//Surf.2 Frac.
	if (thread.NSURF2 > 0)
	{
		if (!PerformRangeTest(hItemList, TOTAL))test = false;
		TOTAL++;
	}

	//Displacement
	for (j = TOTAL; j < TOTAL + variable.NDISTOT; j++)
		if (!PerformRangeTest(hItemList, j))test = false;
	TOTAL += variable.NDISTOT;

	//DebWall
	for (j = TOTAL; j < TOTAL + thread.NDWTOT; j++)
		if (!PerformRangeTest(hItemList, j))test = false;
	TOTAL += thread.NDWTOT;

	//DebWall2
	for (j = TOTAL; j < TOTAL + thread.NDWTOT2; j++)
		if (!PerformRangeTest(hItemList, j))test = false;
	TOTAL += thread.NDWTOT2;

	//Occ
	for (j = TOTAL; j < TOTAL + variable.NOCCTOT; j++)
		if (!PerformRangeTest(hItemList, j))test = false;
	TOTAL += variable.NOCCTOT;

	//Occ2
	for (j = TOTAL; j < TOTAL + variable.NOCCTOT2; j++)
		if (!PerformRangeTest(hItemList, j))test = false;
	TOTAL += variable.NOCCTOT2;

	//Factor
	if (variable.NFACTOR > 0)
	{
		for (j = TOTAL; j < TOTAL + variable.NFACTOR; j++)
			if (!PerformRangeTest(hItemList, j))test = false;
		TOTAL += variable.NFACTOR;
	}

	//Rotation
	if (variable.MODE==1)
	{
		for (j = TOTAL; j < TOTAL + variable.ROTATIONTOTX; j++)
			if (!PerformRangeTest(hItemList, j))test = false;
		TOTAL += variable.ROTATIONTOTX;

		for (j = TOTAL; j < TOTAL + variable.ROTATIONTOTY; j++)
			if (!PerformRangeTest(hItemList, j))test = false;
		TOTAL += variable.ROTATIONTOTY;

		for (j = TOTAL; j < TOTAL + variable.ROTATIONTOTZ; j++)
			if (!PerformRangeTest(hItemList, j))test = false;
		TOTAL += variable.ROTATIONTOTZ;
	}

	//Domain occ.
	if (variable.DOMOCCUPAUTO)
	{
		for (j = TOTAL; j < TOTAL + thread.NDOMAIN; j++)
			if (!PerformRangeTest(hItemList, j))test = false;
	}



	return test;
}