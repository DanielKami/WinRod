//--------------------------------------------------------------------------------------------------------------------------------
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
#include "CallBack.h"
#include "resource.h"
#include "definitions.h"
#include "Thread.h"
#include "fit.h"
#include "plot.h"
#include "set.h"
#include "Camera.h"
#include "variables.h"
#include "vfsr_funct.h"
#include "keating.h"
#include "elements.h"
#include "ReadFile.h"


extern Camera		MyCamera;					//Main camera class

//Read section
extern ModelStruct	Bul;
extern ModelStruct	Sur;
extern DataStruct	Dat;

extern				AtomParam *D3DbulkAtom;
extern				AtomParam *D3DsurfAtom;

extern int			nr_bulk_atoms_toplot;			//3D
extern int			nr_surf_atoms_toplot;			//3D

int g_SRepetitionX, g_SRepetitionY;
int g_BRepetitionX, g_BRepetitionY, g_BRepetitionZ;


FLAG				flag;							//All necessary flags to comunicate between callback windows and WinRod 

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 //int Reg_code;
 UNREFERENCED_PARAMETER(lParam);

	switch (message){
		case WM_INITDIALOG:
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
// Message handler for calculate Patterson box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK CalculatePatterson(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	switch (message){
		case WM_INITDIALOG:

		return (INT_PTR)TRUE;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){	

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			}
		break;
	}
 return (INT_PTR)FALSE;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for calculate box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
 INT_PTR CALLBACK Calculate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){


	switch (message){
		case WM_INITDIALOG:

			SetDlgItemReal(hDlg, IDC_EDIT1, variable.h);		SetDlgItemReal(hDlg, IDC_EDIT2, variable.k);		SetDlgItemReal(hDlg, IDC_EDIT12, variable.l);
			SetDlgItemReal(hDlg, IDC_EDIT5, variable.h_start);  SetDlgItemReal(hDlg, IDC_EDIT10, variable.k_start); SetDlgItemReal(hDlg, IDC_EDIT3, variable.l_start);
			SetDlgItemReal(hDlg, IDC_EDIT9, variable.h_end);	SetDlgItemReal(hDlg, IDC_EDIT11, variable.k_end);	SetDlgItemReal(hDlg, IDC_EDIT4, variable.l_end);
			SetDlgItemInt(hDlg, IDC_EDIT7, variable.N_points, 0);

		return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK){	
				variable.h = GetDlgItemReal(hDlg, IDC_EDIT1);		variable.k = GetDlgItemReal(hDlg, IDC_EDIT2);			variable.l = GetDlgItemReal(hDlg, IDC_EDIT12);
				variable.h_start = GetDlgItemReal(hDlg, IDC_EDIT5);	variable.k_start = GetDlgItemReal(hDlg, IDC_EDIT10);	variable.l_start = GetDlgItemReal(hDlg, IDC_EDIT3);
				variable.h_end = GetDlgItemReal(hDlg, IDC_EDIT9);	variable.k_end = GetDlgItemReal(hDlg, IDC_EDIT11);		variable.l_end = GetDlgItemReal(hDlg, IDC_EDIT4);

				int tmpi=GetDlgItemInt(hDlg, IDC_EDIT7, 0, 0);
				if(tmpi>=0 && tmpi<MAXTHEO) variable.N_points = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value ", NULL, MB_OK);

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

			//nr of domains
			SetDlgItemInt(hDlg, IDC_EDIT8, thread.NDOMAIN, FALSE);
			NumberOfDomaind = thread.NDOMAIN;
			//actual domain to work with
			SetDlgItemInt(hDlg, IDC_EDIT3, current_domain+1, 0);
			SetDlgItemReal(hDlg, IDC_EDIT7, DOMOCCUP[current_domain]);
			if(variable.DOMEQUAL == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);


		return (INT_PTR)TRUE;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDOK){	
				thread.NDOMAIN = NumberOfDomaind;
				for (i = 0; i < thread.NDOMAIN; i++){
					thread.DOMMAT11[i] = DOMMAT11[i];
					thread.DOMMAT12[i] = DOMMAT12[i];
					thread.DOMMAT21[i] = DOMMAT21[i];
					thread.DOMMAT22[i] = DOMMAT22[i];
					thread.DOMOCCUP[i] = DOMOCCUP[i];
				}
				
				if(SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED){
					variable.DOMEQUAL = true;
					for( i = 0; i < thread.NDOMAIN; i++){
						thread.DOMOCCUP[i] = 1./thread.NDOMAIN;
					}
				}
				else variable.DOMEQUAL  = false;
				if (current_domain>NumberOfDomaind) current_domain= 0;
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDCANCEL){	
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDC_BUTTON1){	
				if(current_domain>0) current_domain--;
				SetDlgItemInt(hDlg, IDC_EDIT3, current_domain+1, 0);
				SetDlgItemReal(hDlg, IDC_EDIT1, DOMMAT11[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT2, DOMMAT12[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT4, DOMMAT21[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT5, DOMMAT22[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT7, DOMOCCUP[current_domain]);
			}

 			if (LOWORD(wParam) == IDC_BUTTON2){	

				if(current_domain<NumberOfDomaind-1) current_domain++;
				SetDlgItemInt(hDlg, IDC_EDIT3, current_domain+1, 0);

				SetDlgItemReal(hDlg, IDC_EDIT1, DOMMAT11[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT2, DOMMAT12[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT4, DOMMAT21[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT5, DOMMAT22[current_domain]);
				SetDlgItemReal(hDlg, IDC_EDIT7, DOMOCCUP[current_domain]);
			}

			if (LOWORD(wParam) == IDC_BUTTON5){	
				DOMMAT11[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT1);
				DOMMAT12[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT2); 
				DOMMAT21[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT4); 
				DOMMAT22[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT5); 
				DOMOCCUP[current_domain] = GetDlgItemReal(hDlg, IDC_EDIT7); 
			}

			if (LOWORD(wParam) == IDC_BUTTON4){	
				NumberOfDomaind = GetDlgItemInt(hDlg, IDC_EDIT8, 0, 0);
				if(NumberOfDomaind > MAXDOM) NumberOfDomaind = MAXDOM;
			}


		break;
	}
 return (INT_PTR)FALSE;
  }

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for el. density map box.                                                   
//-------------------------------------------------------------------------------------------------------------------------------- 
INT_PTR CALLBACK ElectronDensity(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	switch (message){
		case WM_INITDIALOG:




		return (INT_PTR)TRUE;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){	

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			}
		break;
	}
 return (INT_PTR)FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for ketting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Keating(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 static int nItem, nSubItem;
 int j;
 static int ntot;
 //list
 WCHAR Tstr[100];
 WCHAR pszNr[8];
 WCHAR pszElement[100];
 WCHAR pszAtRadius[100];
 WCHAR pszBonded[1000];


 LVCOLUMN		colNr;
 LVCOLUMN		colElement;
 LVCOLUMN		colAtRadius;
 LVCOLUMN		colBonded;


 LVITEM			lvTest;
 UINT			iIndex = 0;

 HWND  hItemList = GetDlgItem(hDlg, IDC_LIST2);

	switch (message){
		case WM_INITDIALOG:

			SetDlgItemReal(hDlg, IDC_EDIT1, ket.EQU_ANGLE*RAD);
			SetDlgItemReal(hDlg, IDC_EDIT2, ket.ALPHA_KEAT);
			SetDlgItemReal(hDlg, IDC_EDIT3, ket.BETA_KEAT);
			if(ket.POTENTIAL == VKEATING ) SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
			if(ket.POTENTIAL == VLENNARDJONES ) SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, BST_CHECKED, 0);
			if(ket.KEAT_PLUS_CHI == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//Create the columns in the list control
			ListView_SetExtendedListViewStyle(hItemList, LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT);

			//Name
			colNr.mask = LVCF_TEXT | LVCF_WIDTH;
			colNr.pszText = TEXT("fix/free Nr");
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
			colBonded.cchTextMax = 90;
			colBonded.cx = 70;
			colBonded.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 3, &colBonded);

			//plot in table
			lvTest.mask = LVIF_TEXT | LVIF_STATE;
			lvTest.iItem = 0;
			lvTest.pszText = pszNr;
			lvTest.iIndent = 0;
			lvTest.stateMask = 0;
			lvTest.state = 0;
			lvTest.iSubItem = 0;

			char str[100];
			for (int i =  thread.NSURF-1; i >= 0 ; i--){

				swprintf_s(pszNr,		100,	L"%4d", i);
				str[0]=variable.ELEMENT[thread.TS[i]][0]; str[1]=variable.ELEMENT[thread.TS[i]][1]; str[2]=variable.ELEMENT[thread.TS[i]][2];
				AtoT(Tstr, str, 3);
				swprintf_s(pszElement, 100,	L"%s%d", Tstr, i+1  );
				swprintf_s(pszAtRadius,	100, L"%6.4f", ket.ATOM_RAD[thread.TS[i]]);
				//bonded to
				
				int n=0;
				for (j = 0; j < thread.NSURF; j++)
					{
						if(ket.IN_KEAT[j]){
							str[0]=variable.ELEMENT[thread.TS[j]][0]; str[1]=variable.ELEMENT[thread.TS[j]][1]; str[2]=variable.ELEMENT[thread.TS[j]][2];
							AtoT(Tstr, str, 3);
							n=swprintf_s(pszBonded+n,	1000,	L"%s%d, ", Tstr, j );
						}
					}
				
				if(ket.IN_KEAT[i] )	ListView_SetCheckState(  hItemList, i , 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText( hItemList, iIndex, 1, pszElement);	
				ListView_SetItemText( hItemList, iIndex, 2, pszAtRadius);	
				ListView_SetItemText( hItemList, iIndex, 3, pszBonded);
			}
			break;
		

/////////////////////////////////////////////////////////////
			return (INT_PTR)TRUE;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){	

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			}
		break;
	}
 return (INT_PTR)FALSE;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for settings box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 double temp;
 int tmpi;

	switch (message){
		case WM_INITDIALOG:
			SetDlgItemReal(hDlg, IDC_EDIT1, thread.ATTEN);
		    SetDlgItemReal(hDlg, IDC_EDIT2, thread.BETA);
			SetDlgItemReal(hDlg, IDC_EDIT9, thread.SCALE);
			SetDlgItemReal(hDlg, IDC_EDIT4, thread.SCALE2);
			SetDlgItemReal(hDlg, IDC_EDIT5, thread.SURFFRAC);
			SetDlgItemReal(hDlg, IDC_EDIT3, thread.SURF2FRAC);
			SetDlgItemReal(hDlg, IDC_EDIT10, thread.LBR[0]);
			SetDlgItemInt (hDlg, IDC_EDIT11, thread.NLAYERS, 0);
			//SetDlgItemInt (hDlg, IDC_EDIT9, thread.NLAYERS, 0);
			//if(FLARES==TRUE ) SendMessage(GetDlgItem(hDlg, IDC_FLARES), BM_SETCHECK, BST_CHECKED, 0);

			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("APPROXBETA"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("EXACTBETA"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("NUMBETA"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("POISSONROUGH"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("GAUSSROUGH"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("LINEARROUGH"));			
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("COSINEROUGH"));
			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("TWOLEVEL"));

			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_SETCURSEL, (WPARAM)thread.ROUGHMODEL, 0 );	



		return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL){	
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDOK){	

				temp = GetDlgItemReal(hDlg, IDC_EDIT1);
				if(temp>=0.0 && temp<1000.0) thread.ATTEN = temp;
				else MessageBox(hDlg, L"Wrong ATTENUATION value ", NULL, MB_OK);
				temp = GetDlgItemReal(hDlg, IDC_EDIT2);
				if(temp>=0.0 && temp<=1.0) thread.BETA = temp;
				else MessageBox(hDlg, L"Wrong BETA value ", NULL, MB_OK);
				temp = GetDlgItemReal(hDlg, IDC_EDIT9);
				if(temp>=0.0 && temp<10000.0) thread.SCALE = temp;
				else MessageBox(hDlg, L"Wrong SCALE value ", NULL, MB_OK);				
				temp = GetDlgItemReal(hDlg, IDC_EDIT4);
				if(temp>=0.0 && temp<10000.0) thread.SCALE2 = temp;
				else MessageBox(hDlg, L"Wrong SCALE2 value ", NULL, MB_OK);
				temp = GetDlgItemReal(hDlg, IDC_EDIT5);
				if(temp>=0.0 && temp<=1.0) thread.SURFFRAC = temp;
				else MessageBox(hDlg, L"Wrong SURFFRAC value ", NULL, MB_OK);
				temp = GetDlgItemReal(hDlg, IDC_EDIT3);
				if(temp>=0.0 && temp<=1.0) thread.SURF2FRAC = temp;
				else MessageBox(hDlg, L"Wrong SURF2FRAC value ", NULL, MB_OK);
				temp = GetDlgItemReal(hDlg, IDC_EDIT10);
				if(temp>=0.0 && temp<10.0) thread.LBR[0]= temp;
				else MessageBox(hDlg, L"Wrong L BRAG value ", NULL, MB_OK);

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT11, 0, 0);
				if(tmpi>=0 && tmpi<100) thread.NLAYERS = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value ", NULL, MB_OK);

				thread.ROUGHMODEL = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);

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
 static HBRUSH hBr1, hBr2, hBr3;
 static HWND PictureBox1, PictureBox2, PictureBox3;					
 static int display_mode;				

	switch (message){
		case WM_INITDIALOG:
			//3D Background
			PictureBox1 = GetDlgItem(hDlg, IDC_BACKGROUND);
			PictureBox2 = GetDlgItem(hDlg, IDC_LIGHT_COLOR);
			PictureBox3 = GetDlgItem(hDlg, IDC_TEXT_COLOR);
			tmpRGBbackground = flag.rgbCurrent;
			tmpRGBlight = flag.rgbLight;
			tmpRGBtext = flag.rgbText;

			if(flag.g_planeOnOff == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_SETCHECK, BST_CHECKED, 0);
			if(flag.g_plot_shadows == true ) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);

			if(display_mode == 0 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
			if(display_mode == 1 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, BST_CHECKED, 0);
			if(display_mode == 2 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO3), BM_SETCHECK, BST_CHECKED, 0);
			if(display_mode == 3 ) SendMessage(GetDlgItem(hDlg, IDC_RADIO4), BM_SETCHECK, BST_CHECKED, 0);

			//Bulk repetitions
			SetDlgItemInt (hDlg, IDC_EDIT1, g_BRepetitionX, 1);
			SetDlgItemInt (hDlg, IDC_EDIT2, g_BRepetitionY, 1);
			SetDlgItemInt (hDlg, IDC_EDIT9, g_BRepetitionZ, 0);

			//Surface repetitions
			SetDlgItemInt (hDlg, IDC_EDIT11, g_SRepetitionX, 1);
			SetDlgItemInt (hDlg, IDC_EDIT12, g_SRepetitionY, 1);



			SetDlgItemInt (hDlg, IDC_EDIT3, (int)(flag.DWscale*PIPI16),  0);



		return (INT_PTR)TRUE;


		case WM_CTLCOLORSTATIC:
			{
			HWND hCtl =( HWND ) lParam;
		
			if( hCtl == PictureBox1 ){
				if(hBr1) DeleteObject(hBr1);
				hBr1 = CreateSolidBrush(tmpRGBbackground);
				return (LONG)hBr1;
			}
			if( hCtl == PictureBox2 ){
				if(hBr2) DeleteObject(hBr2);
				hBr2 = CreateSolidBrush(tmpRGBlight);
				return (LONG)hBr2;
			}
			if( hCtl == PictureBox3 ){
				if(hBr3) DeleteObject(hBr3);
				hBr3 = CreateSolidBrush(tmpRGBtext);
				return (LONG)hBr3;
			}
			}
		break;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDOK){	
				
				if(SendMessage(GetDlgItem(hDlg, IDC_CHECK2), BM_GETCHECK, 0, 0) == BST_CHECKED) flag.g_planeOnOff = true;
				else flag.g_planeOnOff = false;
				if(SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) flag.g_plot_shadows = true;
				else flag.g_plot_shadows = false;


				if(SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 0 ;
				if(SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 1 ;
				if(SendMessage(GetDlgItem(hDlg, IDC_RADIO3), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 2 ;
				if(SendMessage(GetDlgItem(hDlg, IDC_RADIO4), BM_GETCHECK, 0, 0) == BST_CHECKED) display_mode = 3 ;

				//Check if something is selected
				int selected = 0;
				for(int i=0; i<nr_bulk_atoms_toplot; i++){
					if(D3DbulkAtom[i].Selected) selected++;
				}
				for(int i=0; i<nr_surf_atoms_toplot; i++){
					if(D3DsurfAtom[i].Selected) selected++;
				}

				if (selected==0){
					//Set the same style to all atoms
					for(int i=0; i<nr_bulk_atoms_toplot; i++){
						D3DbulkAtom[i].display_mode = display_mode;
					}
					for(int i=0; i<nr_surf_atoms_toplot; i++){
						D3DsurfAtom[i].display_mode = display_mode;
					}
				}
				else{
					//Set the style to the selected atoms
					for(int i=0; i<nr_bulk_atoms_toplot; i++){
						if(D3DbulkAtom[i].Selected)
							D3DbulkAtom[i].display_mode = display_mode;
					}
					for(int i=0; i<nr_surf_atoms_toplot; i++){
						if(D3DsurfAtom[i].Selected)
							D3DsurfAtom[i].display_mode = display_mode;
					}

				}


				tmpi=GetDlgItemInt(hDlg, IDC_EDIT1, 0, 1);
				if(tmpi>=-100 && tmpi<100) g_BRepetitionX = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value in X direction", NULL, MB_OK);

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT2, 0, 1);
				if(tmpi>=-100 && tmpi<100) g_BRepetitionY = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value in Y direction", NULL, MB_OK);

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT11, 0, 1);
				if(tmpi>=-100 && tmpi<100) g_SRepetitionX = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value in X direction", NULL, MB_OK);

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT12, 0, 1);
				if(tmpi>=-100 && tmpi<100) g_SRepetitionY = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value in Y direction", NULL, MB_OK);



				tmpi=GetDlgItemInt(hDlg, IDC_EDIT9, 0, 0);
				if(tmpi>=0 && tmpi<100) g_BRepetitionZ = tmpi;
				else MessageBox(hDlg, L"Wrong NLAYERS value in Z direction ", NULL, MB_OK);

				tmpi=GetDlgItemInt(hDlg, IDC_EDIT3, 0, 0);
				if(tmpi>=0 && tmpi<100) flag.DWscale = (double) tmpi/PIPI16;
				else MessageBox(hDlg, L"Wrong %propability value", NULL, MB_OK);



				//----------------------------
				flag.rgbCurrent	= tmpRGBbackground;
				flag.rgbLight = tmpRGBlight;
				flag.rgbText = tmpRGBtext;

				DeleteObject(hBr1);
				DeleteObject(hBr2);
				DeleteObject(hBr3);
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			if (LOWORD(wParam) == IDCANCEL){	
				DeleteObject(hBr1);
				DeleteObject(hBr2);
				DeleteObject(hBr3);
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}


			if (LOWORD(wParam) == ID_BACKGROUND){	

				CHOOSECOLOR cc;                 // common dialog box structure 
 
				COLORREF BackgroundColor[16]; // array of custom colors
				//HBRUSH hbrush;                  // brush handle
				

				// Initialize CHOOSECOLOR 
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hDlg;
				cc.lpCustColors = (LPDWORD) BackgroundColor;
				cc.rgbResult = tmpRGBbackground;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;
 
				if (ChooseColor(&cc)==TRUE){
					tmpRGBbackground = cc.rgbResult; 
					InvalidateRect(PictureBox1, 0 ,0);
				}
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDC_LIGHT){	

				CHOOSECOLOR cc;                 // common dialog box structure 
 
				COLORREF BackgroundColor[16]; // array of custom colors
				//HBRUSH hbrush;                  // brush handle
				
				// Initialize CHOOSECOLOR 
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hDlg;
				cc.lpCustColors = (LPDWORD)BackgroundColor ;
				cc.rgbResult = tmpRGBlight;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;
 
				if (ChooseColor(&cc)==TRUE){
					 tmpRGBlight = cc.rgbResult; 
					InvalidateRect(PictureBox2, 0 ,0);
				}
			return (INT_PTR)TRUE;
			}




			if (LOWORD(wParam) == IDC_TEXT){	

				CHOOSECOLOR cc;                 // common dialog box structure 
 
				COLORREF BackgroundColor[16]; // array of custom colors
				//HBRUSH hbrush;                  // brush handle
				

				// Initialize CHOOSECOLOR 
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hDlg;
				cc.lpCustColors = (LPDWORD) BackgroundColor;
				cc.rgbResult = flag.rgbText;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;
 
				if (ChooseColor(&cc)==TRUE){
					tmpRGBtext = cc.rgbResult; 
					InvalidateRect(PictureBox3, 0 ,0);
				}
			return (INT_PTR)TRUE;
			}
		
		break;
	}
 return (INT_PTR)FALSE;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK Fitting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 static int nItem, nSubItem;
 int j;
 static int ntot;
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

 LVITEM			lvTest;
 UINT			iIndex = 0;
 static HWND* hProgressBar;
 static HWND* hText;

HWND  hItemList = GetDlgItem(hDlg, IDC_LIST1);




	switch (message){
		case WM_INITDIALOG:
			{
			ntot = NSF+variable.NDISTOT+thread.NDWTOT+thread.NDWTOT2+variable.NOCCTOT;

			ListView_SetExtendedListViewStyle(hItemList, LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT);

			HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
			hProgressBar = new HWND[ntot*2+1]; 
			hText        = new HWND[ntot+1];

			int shift =0;
			for(j = 0; j< ntot; j++){
				
				shift += 20;

				hText[j] = CreateWindowEx( 0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, shift, 20, 22, 20, hDlg, NULL, hInstance, NULL );
				swprintf_s( pszNr, L"%d", j);
				SetWindowText( hText[j], pszNr );
				ShowWindow(hText[j], SW_HIDE);
				//values
				hProgressBar[j*2] = CreateWindowEx( 0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_VERTICAL |PBS_SMOOTHREVERSE | PBS_SMOOTH,
															shift, 50, 10, 200, hDlg,( HMENU ) 200, hInstance, NULL );
				ShowWindow(hProgressBar[j*2], SW_HIDE);
				//value temperature
				shift += 12;
				hProgressBar[j*2+1] = CreateWindowEx( 0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_VERTICAL |PBS_SMOOTHREVERSE | PBS_SMOOTH,
															shift, 50, 10, 200, hDlg,( HMENU ) 200, hInstance, NULL );
				ShowWindow(hProgressBar[j*2+1], SW_HIDE);

			}



			ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
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

			int i = ntot+1;

			//plot in table
			lvTest.mask = LVIF_TEXT | LVIF_STATE;
			lvTest.iItem = 0;
			lvTest.pszText = pszNr;
			lvTest.iIndent = 0;
			lvTest.stateMask = 0;
			lvTest.state = 0;
			lvTest.iSubItem = 0;

			//Occupation
			for (j = variable.NOCCTOT-1; j >= 0 ; j--){
				i--;
				swprintf_s(pszNr,		100,	L"%4d", i);
				swprintf_s(pszParameter,100,	L"Occupancy %d", j+1  );
				swprintf_s(pszLowLimit,	100,	L"%6.4f", variable.OCCUPLIM[j][0]);
				swprintf_s(pszValue,	100,	L"%6.4f", thread.OCCUP[j]);
				swprintf_s(pszUplimit,	100,	L"%6.4f", variable.OCCUPLIM[j][1]);
				if(variable.OCCUPPEN[j] )	ListView_SetCheckState(  hItemList, i-1 , 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
				ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
				ListView_SetItemText( hItemList, iIndex, 3, pszValue);
				ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);
			}

			//Debye Wallers2
			for (j = thread.NDWTOT2-1; j >= 0 ; j--){
				i--;
				swprintf_s(pszNr,		100,	L"%4d", i);
				swprintf_s(pszParameter,100,	L"DEBYE WALLER2 %d", j+1 );
				swprintf_s(pszLowLimit,	100,	L"%6.4f", variable.DEBWAL2LIM[j][0]);
				swprintf_s(pszValue,	100,	L"%6.4f", thread.DEBWAL2[j]);
				swprintf_s(pszUplimit,	100,	L"%6.4f", variable.DEBWAL2LIM[j][1]);
				if(variable.DEBWAL2PEN[j] )	ListView_SetCheckState(  hItemList, i-1 , 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
				ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
				ListView_SetItemText( hItemList, iIndex, 3, pszValue);
				ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);
			}

			//Debye Wallers
			for (j = thread.NDWTOT-1; j >= 0 ; j--){
				i--;
				swprintf_s(pszNr,		100,	L"%4d", i);
				swprintf_s(pszParameter,100,	L"DEBYE WALLER %d", j+1 );
				swprintf_s(pszLowLimit,	100,	L"%6.4f", variable.DEBWALLIM[j][0]);
				swprintf_s(pszValue,	100,	L"%6.4f", thread.DEBWAL[j]);
				swprintf_s(pszUplimit,	100,	L"%6.4f", variable.DEBWALLIM[j][1]);
				if(variable.DEBWALPEN[j] )	ListView_SetCheckState(  hItemList, i-1 , 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
				ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
				ListView_SetItemText( hItemList, iIndex, 3, pszValue);
				ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);
			}

			//displacement
			for (j = variable.NDISTOT-1; j >= 0; j--){
				i--;
				swprintf_s(pszNr,		100,	L"%4d", i);
				swprintf_s(pszParameter,100,	L"DISPLACEMENT %d", j+1  );
				swprintf_s(pszLowLimit,	100,	L"%4.4f", variable.DISPLLIM[j][0]);
				swprintf_s(pszValue,	100,	L"%4.4f", variable.DISPL[j]);
				swprintf_s(pszUplimit,	100,	L"%4.4f", variable.DISPLLIM[j][1]);
				if(variable.DISPLPEN[j] )	ListView_SetCheckState(  hItemList, i-1 , 1);
				lvTest.pszText = pszNr;

				iIndex = ListView_InsertItem(hItemList, &lvTest);
				ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
				ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
				ListView_SetItemText( hItemList, iIndex, 3, pszValue);
				ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);
			}

			//surf. fraction
			i--;
			swprintf_s(pszNr,		100,	L"%4d", i);
			swprintf_s(pszParameter,100,	L"SURF. FRACTION"  );
			swprintf_s(pszLowLimit,	100,	L"%6.4f", variable.SURFFRACLIM[0]);
			swprintf_s(pszValue,	100,	L"%6.4f", thread.SURFFRAC);
			swprintf_s(pszUplimit,	100,	L"%6.4f", variable.SURFFRACLIM[1]);
			if(variable.SURFFRACPEN )	ListView_SetCheckState(  hItemList, i-1 , 1);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
			ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
			ListView_SetItemText( hItemList, iIndex, 3, pszValue);
			ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);

			//beta
			i--;
			swprintf_s(pszNr,		100,	L"%4d", i);
			swprintf_s(pszParameter,100,	L"BETA"  );
			swprintf_s(pszLowLimit,	100,	L"%6.4f", variable.BETALIM[0]);
			swprintf_s(pszValue,	100,	L"%6.4f", thread.BETA);
			swprintf_s(pszUplimit,	100,	L"%6.4f", variable.BETALIM[1]);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
			ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
			ListView_SetItemText( hItemList, iIndex, 3, pszValue);
			ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);

			//scale2
			i--;
			swprintf_s(pszNr,		100,	L"%4d", i);
			swprintf_s(pszParameter,100,	L"SCALE2"  );
			swprintf_s(pszLowLimit,	100,	L"%6.4f", variable.SCALE2LIM[0]);
			swprintf_s(pszValue,	100,	L"%6.4f", thread.SCALE2);
			swprintf_s(pszUplimit,	100,	L"%6.4f", variable.SCALE2LIM[1]);
			lvTest.pszText = pszNr;

			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
			ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
			ListView_SetItemText( hItemList, iIndex, 3, pszValue);
			ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);

			i--;
			//SCALE
			swprintf_s(pszNr,		100,	L"%4d", i);
			swprintf_s(pszParameter,100,	L"SCALE"  );
			swprintf_s(pszLowLimit,	100,	L"%6.4f", variable.SCALELIM[0]);
			swprintf_s(pszValue,	100,	L"%6.4f", thread.SCALE);
			swprintf_s(pszUplimit,	100,	L"%6.4f", variable.SCALELIM[1]);
			
			iIndex = ListView_InsertItem(hItemList, &lvTest);
			ListView_SetItemText( hItemList, iIndex, 1, pszParameter);	
			ListView_SetItemText( hItemList, iIndex, 2, pszLowLimit);	
			ListView_SetItemText( hItemList, iIndex, 3, pszValue);
			ListView_SetItemText( hItemList, iIndex, 4, pszUplimit);
			

			if(variable.SCALEPEN ){	ListView_SetCheckState(  hItemList, 0, FALSE);}
			else ListView_SetCheckState(  hItemList, 0, TRUE);

			if(variable.SCALE2PEN ){ListView_SetCheckState(  hItemList, 1, FALSE);}
			else ListView_SetCheckState(  hItemList, 1, TRUE);

			if(variable.BETAPEN  ){	ListView_SetCheckState(  hItemList, 2, FALSE);}
			else ListView_SetCheckState(  hItemList, 2, TRUE);

			if(variable.SURFFRACPEN  ){	ListView_SetCheckState(  hItemList, 3, FALSE);}
			else ListView_SetCheckState(  hItemList, 3, TRUE);
		
			int TOTAL = NSF;
			for(j = 0; j<variable.NDISTOT; j++){
				if(variable.DISPLPEN[j]  ){	ListView_SetCheckState(  hItemList, TOTAL+j, FALSE);}
				else ListView_SetCheckState(  hItemList, TOTAL + j, TRUE);
			}
			TOTAL = NSF + variable.NDISTOT;
			for(j = 0; j<thread.NDWTOT; j++){
				if(variable.DEBWALPEN[j]  ){	ListView_SetCheckState(  hItemList, TOTAL + j, FALSE);}
				else ListView_SetCheckState(  hItemList, TOTAL + j, TRUE);
			}
			TOTAL = NSF + variable.NDISTOT + thread.NDWTOT;
			for(j = 0; j<thread.NDWTOT2; j++){
				if(variable.DEBWAL2PEN[j]  ){	ListView_SetCheckState(  hItemList, TOTAL + j, FALSE);}
				else ListView_SetCheckState(  hItemList, TOTAL + j, TRUE);
			}
			TOTAL = NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2;
			for(j = 0; j<variable.NOCCTOT; j++){
				if(variable.OCCUPPEN[j]  ){	ListView_SetCheckState(  hItemList, TOTAL + j, FALSE);}
				else ListView_SetCheckState(  hItemList, TOTAL + j, TRUE);
			}
	
		}

/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case NM_CLICK://IDC_LIST1
					OnClickList(hDlg, hItemList, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)  lParam, &nItem, &nSubItem, 2);
					break;
				break;
			}
			break;

		case WM_COMMAND:
			switch LOWORD(wParam){

				case IDOK:
					{
					CopyValuesFromListBox(hItemList);
					delete[] hProgressBar;
					delete[] hText;
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

				case IDCANCEL:
					{
					delete[] hProgressBar;
					delete[] hText;
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

				case IDC_BUTTON1:
					{
						HWND pwndCtrl = GetFocus();
						if (pwndCtrl == GetDlgItem (hDlg,IDC_EDIT1)){
							//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
							SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem);
							SendDlgItemMessage(hDlg, IDC_EDIT1, WM_KILLFOCUS, 0, 0);
							ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
							InvalidateRect(hItemList, 0 ,0);
						}
					}
				break;


				case ID_LEV:
					{
					CopyValuesFromListBox(hItemList);
					fit(hDlg, LSQ_MRQ, 0);
					delete[] hProgressBar;
					delete[] hText;
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;

				}
				break;
				case ID_ASA:
					{
					ShowWindow(hItemList, SW_HIDE);
					for(j = 0; j< ntot; j++){
						int value =(int)( abs(  GetItemReal(hItemList, j, 3)/(GetItemReal(hItemList, j, 4) - GetItemReal(hItemList, j, 2)))  )*100;
						SendMessage(hProgressBar[j*2], PBM_SETRANGE, 0,(LPARAM) MAKELPARAM(0, 100 )); 
						SendMessage(hProgressBar[j*2], PBM_SETPOS,( WPARAM ) value, 0 );//
						ShowWindow (hProgressBar[j*2], SW_SHOW);

						SendMessage(hProgressBar[j*2+1], PBM_SETRANGE, 0,(LPARAM) MAKELPARAM(0, 100 )); 
						SendMessage(hProgressBar[j*2+1], PBM_SETPOS,( WPARAM ) 0, 100 );//
						ShowWindow (hProgressBar[j*2+1], SW_SHOW);

						ShowWindow(hText[j], SW_SHOW);
					}

					//progress bar I for satellites
					SendMessage(GetDlgItem(hDlg, IDC_PROGRESS1), PBM_SETRANGE, 0,(LPARAM) MAKELPARAM(0, LIMIT_ACCEPTANCES));

					CopyValuesFromListBox(hItemList);
					//TestValue break;
					
 					fit(hDlg, LSQ_ASA, hProgressBar);	
					//ShowWindow(hProgressBar, SW_HIDE);
					ShowWindow(hItemList, SW_SHOW);

					delete[] hProgressBar;
					delete[] hText;
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
				}
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
void SetDlgItemReal(HWND hDlg, int nIDD, double Value){

 WCHAR temp_str[100];  
 swprintf_s(temp_str, 100, TEXT("%4.4f"), Value);

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
	swscanf_s(temp_string, TEXT("%5f"), &temp2 );

 return (double) temp2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function set the text in the specified SubItem depending on the Row and Column values
void SetCell(HWND hWnd1, double value, int nRow, int nCol, bool Int){

 WCHAR     szString [256];
	if(Int)
		swprintf_s(szString, L"%d", (int)value ,0);
	else
		swprintf_s(szString, L"%f", value ,0);

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
void OnClickList(HWND hDlg, HWND hWndListBox, HWND hWndEditBox, NMHDR* pNMHDR, int *nItem, int *nSubItem, int SkipColumn) 
{
    InvalidateRect(hWndListBox, 0 ,0);

    LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE) pNMHDR;
    RECT rect;
    //get the row number
    *nItem = temp->iItem;
    //get the column number
    *nSubItem = temp->iSubItem;
	//Skip the second column this are names of parameters
    if(*nSubItem < SkipColumn ||  *nItem == -1)
		return ;

    RECT rect1,rect2;
    // this macro is used to retrieve the Rectanle  of the selected SubItem
    ListView_GetSubItemRect(hWndListBox, temp->iItem, temp->iSubItem,LVIR_BOUNDS,&rect);
    //Get the Rectange of the listControl
    GetWindowRect(temp->hdr.hwndFrom,&rect1);
    //Get the Rectange of the Dialog
    GetWindowRect(hDlg, &rect2);

    int x=rect1.left-rect2.left;
    int y=rect1.top-rect2.top;
    
    if(*nItem != -1) 
		SetWindowPos(hWndEditBox,HWND_TOP, rect.left+x,rect.top+13, rect.right-rect.left - 3, rect.bottom-rect.top -1, SWP_DRAWFRAME);
    ShowWindow(hWndEditBox,SW_SHOW);
    SetFocus(hWndEditBox);
    //Draw a Rectangle around the SubItem
    Rectangle(GetDC(temp->hdr.hwndFrom), rect.left,rect.top-1,rect.right,rect.bottom);
    //Set the listItem text in the EditBox
	WCHAR Wstr[128];
	GetItemText(hWndListBox, Wstr, sizeof(Wstr), *nItem , *nSubItem);
    SetWindowText(hWndEditBox, Wstr);

}


void CopyValuesFromFitEditListBox(HWND hItemList){

char Symbol[3];
WCHAR Wsymbol[3];

	for (int i= 0; i < thread.NTYPES;i++){
			thread.TS[i] = 0;
	}
	for(int j = 0; j<thread.NSURF; j++){
				//AtoT(pszAtom, variable.ELEMENT[thread.TS[j]], 3);
		GetItemText(hItemList, Wsymbol, 3, j, 1);
		TtoA(Symbol, Wsymbol,  3);
	////////////////////////////////
		/* Find the different element types and store them in ELEMENT */
		//Clear the St tables before procensing


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
		variable.NZDIS[j] = (int)GetItemReal(hItemList, j, 13);
		if(variable.NDISTOT < variable.NZDIS[j]) variable.NDISTOT = variable.NZDIS[j];
		//DW1
		thread.NDWS[j] = (int)GetItemReal(hItemList, j, 14);
		if(thread.NDWTOT < thread.NDWS[j]) thread.NDWTOT = thread.NDWS[j];
		//DW2
		thread.NDWS2[j] = (int)GetItemReal(hItemList, j, 15);
		if(thread.NDWTOT2 < thread.NDWS2[j]) thread.NDWTOT2 = thread.NDWS2[j];
		//Occ
		thread.NOCCUP[j] = (int)GetItemReal(hItemList, j, 16);
		if(variable.NOCCTOT < thread.NOCCUP[j]) variable.NOCCTOT = thread.NOCCUP[j];
	}

}


void CopyValuesFromBulkEditListBox(HWND hItemList){

char Symbol[3];
WCHAR Wsymbol[3];

	//Clear the St tables before procensing
	for (int i= 0; i < thread.NTYPES;i++){
			thread.TB[i] = 0;
	}
	for(int j = 0; j<thread.NBULK; j++){
				//AtoT(pszAtom, variable.ELEMENT[thread.TS[j]], 3);
		GetItemText(hItemList, Wsymbol, 3, j, 1);
		TtoA(Symbol, Wsymbol,  3);
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




void CopyValuesFromListBox(HWND hItemList){
 int j;
 int TOTAL;

	if(ListView_GetCheckState(  hItemList, 0)==TRUE) variable.SCALEPEN  = false;
	else variable.SCALEPEN = true;
	if(ListView_GetCheckState(  hItemList, 1)==TRUE) variable.SCALE2PEN = false;
	else variable.SCALE2PEN = true;
	if(ListView_GetCheckState(  hItemList, 2)==TRUE) variable.BETAPEN = false;
	else variable.BETAPEN = true;
	if(ListView_GetCheckState(  hItemList, 3)==TRUE) variable.SURFFRACPEN = false;
	else variable.SURFFRACPEN = true;

	TOTAL = NSF;
	for(j = 0; j<variable.NDISTOT; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL+j)==TRUE) variable.DISPLPEN[j] = false;
		else variable.DISPLPEN[j] = true;
	}
	TOTAL += variable.NDISTOT;
	for(j = 0; j<thread.NDWTOT; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL + j)==TRUE) variable.DEBWALPEN[j] = false;
		else variable.DEBWALPEN[j] = true;
	}
	TOTAL += thread.NDWTOT;
	for(j = 0; j<thread.NDWTOT2; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL + j)==TRUE) variable.DEBWAL2PEN[j] = false;
		else variable.DEBWAL2PEN[j] = true;
	}
	TOTAL += thread.NDWTOT2;
	for(j = 0; j<variable.NOCCTOT; j++){
		if(ListView_GetCheckState(  hItemList, TOTAL + j)==TRUE) variable.OCCUPPEN[j] = false;
		else variable.OCCUPPEN[j] = true;
	}


	//Copy values
	variable.SCALELIM[0] = GetItemReal(hItemList, 0, 2);
	variable.SCALELIM[1] = GetItemReal(hItemList, 0, 4);
	thread.SCALE         = GetItemReal(hItemList, 0, 3);

	variable.SCALE2LIM[0] = GetItemReal(hItemList, 1, 2);
	variable.SCALE2LIM[1] = GetItemReal(hItemList, 1, 4);
	thread.SCALE2         = GetItemReal(hItemList, 1, 3);

	variable.BETALIM[0] = GetItemReal(hItemList, 2, 2);
	variable.BETALIM[1] = GetItemReal(hItemList, 2, 4);
	thread.BETA         = GetItemReal(hItemList, 2, 3);

	variable.SURFFRACLIM[0] = GetItemReal(hItemList, 3, 2);
	variable.SURFFRACLIM[1] = GetItemReal(hItemList, 3, 4);
	thread.SURFFRAC         = GetItemReal(hItemList, 3, 3);

	TOTAL = NSF;
	for(j = 0; j<variable.NDISTOT; j++){
		variable.DISPLLIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.DISPLLIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		variable.DISPL[j]       = GetItemReal(hItemList, TOTAL + j, 3);
	}
	//Deb. Wall.
	TOTAL += variable.NDISTOT;
	for(j = 0; j<thread.NDWTOT; j++){
		variable.DEBWALLIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.DEBWALLIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.DEBWAL[j]         = GetItemReal(hItemList, TOTAL + j, 3);
	}
	//Deb. Wall. 2
	TOTAL += thread.NDWTOT;
	for(j = 0; j<thread.NDWTOT2; j++){
		variable.DEBWAL2LIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.DEBWAL2LIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.DEBWAL2[j]         = GetItemReal(hItemList, TOTAL + j, 3);
	}
	//Occ.
	TOTAL += thread.NDWTOT2;
	for(j = 0; j<variable.NOCCTOT; j++){
		variable.OCCUPLIM[j][0] = GetItemReal(hItemList, TOTAL + j, 2);
		variable.OCCUPLIM[j][1] = GetItemReal(hItemList, TOTAL + j, 4);
		thread.OCCUP[j]         = GetItemReal(hItemList, TOTAL + j, 3);
	}


}




//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for about box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK FourierConturMap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

	static int mode;
	static unsigned int tmpi;
	static unsigned int NxCon = 200, NyCon = 200;
	static unsigned int Nlevels = 10;

	static double min_x = 0., min_y = 0., max_x = 1., max_y =1.;
	double temp;

	switch (message){
		case WM_INITDIALOG:
			SetDlgItemInt (hDlg, IDC_EDIT1, NxCon, 0);
			SetDlgItemInt (hDlg, IDC_EDIT2, NyCon, 0);
			SetDlgItemInt (hDlg, IDC_EDIT3, Nlevels, 0);

			SetDlgItemInt (hDlg, IDC_EDIT4, 4, 0);

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
				if(tmpi>=0 && tmpi<200) Nlevels = tmpi;
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
// Message handler for set symmetry box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK SetSymmetry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 int PlaneGroup;

	switch (message){
		case WM_INITDIALOG:
			//SetDlgItemReal(hDlg, IDC_EDIT1, thread.ATTEN);
		    //SetDlgItemReal(hDlg, IDC_EDIT2, thread.BETA);


			SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)TEXT("None"));
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
		
				PlaneGroup = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, 0, 0);
				set_symmetry(hDlg, PlaneGroup);
				CopyMemory(variable.PLANEGROUP, symmetry[PlaneGroup], sizeof(char)*5);
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
		break;
	}
 return (INT_PTR)FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditFitFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 static int nItem, nSubItem;
 int j;
 static int ntot;

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
 WCHAR pszZposParam[20];
 WCHAR pszDW1Param[20];
 WCHAR pszDW2Param[20];
 WCHAR pszOccParam[20];


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
 LVCOLUMN		colZposParam;
 LVCOLUMN		colDW1Param;
 LVCOLUMN		colDW2Param;
 LVCOLUMN		colOccParam;

 LVITEM			lvTest;
 UINT			iIndex = 0;

 HWND  hItemList = GetDlgItem(hDlg, IDC_LIST1);

	switch (message){
		case WM_INITDIALOG:
			{
			ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT);



			ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
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

			//Value
			colYposConst2.mask = LVCF_TEXT | LVCF_WIDTH;
			colYposConst2.pszText = TEXT("Yconst.2");
			colYposConst2.cchTextMax = 120;
			colYposConst2.cx = 60;
			colYposConst2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 11, &colYposConst2);

			//Up limit
			colYposParam2.mask = LVCF_TEXT | LVCF_WIDTH;
			colYposParam2.pszText = TEXT("Y par.2");
			colYposParam2.cchTextMax = 120;
			colYposParam2.cx = 50;
			colYposParam2.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 12, &colYposParam2);

			//Value
			colZpos.mask = LVCF_TEXT | LVCF_WIDTH;
			colZpos.pszText = TEXT("Z pos.");
			colZpos.cchTextMax = 120;
			colZpos.cx = 60;
			colZpos.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 13, &colZpos);

			//Up limit
			colZposParam.mask = LVCF_TEXT | LVCF_WIDTH;
			colZposParam.pszText = TEXT("Z par.");
			colZposParam.cchTextMax = 120;
			colZposParam.cx = 50;
			colZposParam.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 14, &colZposParam);

			//Up limit
			colDW1Param.mask = LVCF_TEXT | LVCF_WIDTH;
			colDW1Param.pszText = TEXT("DW1");
			colDW1Param.cchTextMax = 120;
			colDW1Param.cx = 40;
			colDW1Param.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 15, &colDW1Param);

			//Up limit
			colDW2Param.mask = LVCF_TEXT | LVCF_WIDTH;
			colDW2Param.pszText = TEXT("DW2");
			colDW2Param.cchTextMax = 120;
			colDW2Param.cx = 40;
			colDW2Param.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 16, &colDW2Param);

			//Up limit
			colOccParam.mask = LVCF_TEXT | LVCF_WIDTH;
			colOccParam.pszText = TEXT("Occ.");
			colOccParam.cchTextMax = 120;
			colOccParam.cx = 40;
			colOccParam.fmt = LVCFMT_LEFT;
			ListView_InsertColumn(hItemList, 17, &colOccParam);

			//plot in table
			lvTest.mask = LVIF_TEXT | LVIF_STATE;
			lvTest.iItem = 0;
			lvTest.pszText = pszNr;
			lvTest.iIndent = 0;
			lvTest.stateMask = 0;
			lvTest.state = 0;
			lvTest.iSubItem = 0;

			//Occupation
			for (j = thread.NSURF-1; j >= 0 ; j--){

				swprintf_s(pszNr, 100, L"%4d", j);
				//X
				AtoT(pszAtom, variable.ELEMENT[thread.TS[j]], 3);
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
				swprintf_s(pszZposParam,	100, L"%d", variable.NZDIS[j]);
				//DW1
				swprintf_s(pszDW1Param,	100, L"%d", thread.NDWS[j]);
				//DW2
				swprintf_s(pszDW2Param,	100, L"%d", thread.NDWS2[j]);
				//Occ
				swprintf_s(pszOccParam,	100, L"%d", thread.NOCCUP[j]);

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
				ListView_SetItemText( hItemList, iIndex, 13, pszZposParam);
				//DW
				ListView_SetItemText( hItemList, iIndex, 14, pszDW1Param);
				//DW2
				ListView_SetItemText( hItemList, iIndex, 15, pszDW2Param);
				//occ
				ListView_SetItemText( hItemList, iIndex, 16, pszOccParam);
			}

		}

/////////////////////////////////////////////////////////////
		return (INT_PTR)TRUE;

		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case NM_CLICK://IDC_LIST1
					OnClickList(hDlg, hItemList, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)  lParam, &nItem, &nSubItem, 1);
					break;
				break;
			}
			break;

		case WM_COMMAND:
			switch LOWORD(wParam){

				case IDOK:
					{
					CopyValuesFromFitEditListBox(hItemList);
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

				case IDCANCEL:
					{
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

				case IDC_BUTTON1:
					{
						HWND pwndCtrl = GetFocus();
						if (pwndCtrl == GetDlgItem (hDlg,IDC_EDIT1)){
							//get the text from the EditBox and set the value in the listContorl with the specified Item & SubItem values
							if(nSubItem==4 || nSubItem==6 || nSubItem==9 || nSubItem==11 || nSubItem==13 || nSubItem==14 ||nSubItem==15 ||nSubItem==16)
								SetCell(hItemList, GetDlgItemReal(hDlg, IDC_EDIT1), nItem, nSubItem, true);
							if(nSubItem==2 || nSubItem==3 || nSubItem==5 || nSubItem==7 || nSubItem==8 || nSubItem==10 ||nSubItem==12)
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



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------------
// Message handler for fitting box.                                                   
//--------------------------------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK EditBulkFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){

 static int nItem, nSubItem;
 int j;
 static int ntot;

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

 HWND  hItemList = GetDlgItem(hDlg, IDC_LIST1);

	switch (message){
		case WM_INITDIALOG:
			{
			ListView_SetExtendedListViewStyle(hItemList, LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT);



			ShowWindow(GetDlgItem(hDlg, IDC_EDIT1), SW_HIDE);
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
				AtoT(pszAtom, variable.ELEMENT[thread.TB[j]], 3);
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

		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case NM_CLICK://IDC_LIST1
					OnClickList(hDlg, hItemList, GetDlgItem(hDlg, IDC_EDIT1), (NMHDR*)  lParam, &nItem, &nSubItem, 1);
					break;
				break;
			}
			break;

		case WM_COMMAND:
			switch LOWORD(wParam){

				case IDOK:
					{
					CopyValuesFromBulkEditListBox(hItemList);
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

				case IDCANCEL:
					{
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
					}

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