//--------------------------------------------------------------------------------------------------------------------------------
// All save functions in Windows standard
// created by: Daniel Kaminski 2014
//--------------------------------------------------------------------------------------------------------------------------------

#include <stdafx.h>
#include <stdio.h>
#include <commdlg.h>
#include <d3dx9.h>
#include <Windowsx.h>

#include "definitions.h"
#include "Save.h"
#include "3Ddef.h"				//Definition for 3D graphisc
#include "Scene3D.h"
#include "2DGraph.h"			//Add the 2D graphisc window nad all necessary libraries for graphs
#include "variables.h"
#include "Thread.h"
#include "ReadFile.h"
#include "Elements.h"
#include "Import.h"
#include "vfsr_funct.h"


//Printer
#include <Winspool.h>			//Printer functions
#include <commctrl.h>


extern Graph2D						my2D[];

extern Import						myImport;
extern VFSR_par						vfsr_par;


extern Scene3D		myScene;

#ifdef FIT_TEST_PRINT
#include <stdio.h>
#include "ReadFile.h" // needed for character type conversion AtoT
#include "vfsr_funct.h"
extern FILE*   file_fit;

extern VFSR_par vfsr_par;

#endif

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the 3D scene to file as a one of the graphisc formats
//Created by Daniel Kaminski
//
// 19.02.2013
//-----------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
int SaveFileFunction(HWND hwnd){
	//--------------------------------------------------------------------------------------------------------------------------------

	HANDLE			hFile;
	WCHAR szFile[MAX_PATH] = TEXT("\0");
	char str[MAX_PATH] = "\0";

	DWORD  dwsize = 0;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Parameter file (*.par)\0*.par\0Fitting file (*.fit)\0*.fit\0Bulk file (*.bul)\0*.bul\0Data file (*.dat)\0 *.dat\0xyz molecular file (*.xyz)\0*.xyz\0Molecular fit file (*.mft)\0 *.mft\0");
	ofn.lpstrTitle = TEXT("Save File");
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = TEXT("par");
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;



	if (GetSaveFileName(&ofn)){
		hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			MessageBox(hwnd, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
			CloseHandle(hFile);
			return 0;
		}
	}
	else return 0;
	CloseHandle(hFile);

	TtoA(str, ofn.lpstrFile);

	if (ofn.nFilterIndex == 1) SaveParameterFile(str);
	if (ofn.nFilterIndex == 2) SaveFitFile(str);
	if (ofn.nFilterIndex == 3) SaveBulFile(str);
	if (ofn.nFilterIndex == 4) SaveDatFile(str);
	if (ofn.nFilterIndex == 5) SaveXYZFile(str);
	if (ofn.nFilterIndex == 6) SaveMolecularFitFile(str);

	return ofn.nFilterIndex;
}


//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the 3D scene to file as a one of the graphisc formats
//Created by Daniel Kaminski
//
// 18.02.2013
//-----------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
int Save3DFunction(HWND hwnd){
	//--------------------------------------------------------------------------------------------------------------------------------

	HANDLE			hFile;
	WCHAR szFile[MAX_PATH] = TEXT("\0");

	DWORD  dwsize = 0;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("*.bmp\0*.bmp\0*.jpg\0*.jpg\0*.tga\0*.tga\0*.png\0*.png\0*.dds\0*.dds\0*.dib\0*.dib\0\0");
	ofn.lpstrTitle = TEXT("Save File");
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = TEXT("bmp");
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;



	if (GetSaveFileName(&ofn)){
		hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			MessageBox(hwnd, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
			CloseHandle(hFile);
			return 0;
		}
	}
	else return 0;
	CloseHandle(hFile);

	LPDIRECT3DSURFACE9 pFrontBuffer;
	myScene.g_pDevice->CreateOffscreenPlainSurface(1920, 1080, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &pFrontBuffer, NULL);

	if (D3D_OK != myScene.g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer))
	{
		MessageBox(NULL, L"Can't Get Back Buffer", L"Error", MB_OK);
	}

	D3DXSaveSurfaceToFile(ofn.lpstrFile, (D3DXIMAGE_FILEFORMAT)(ofn.nFilterIndex - 1), pFrontBuffer, NULL, NULL);

	SAFE_RELEASE(pFrontBuffer);

	return ofn.nFilterIndex;
}


int Print3DFunction(HWND hwnd)
{
	WCHAR szfile[] = L"temp.tmp";
	HBITMAP hBitmap;
	HANDLE			hFile;
	LPDIRECT3DSURFACE9 pFrontBuffer;
	int bxpage;
	int bypage;

	//-------------------------------------------------------------------------------------------------------
	//We have printer page size so use it for DirectX buffer size settings

	//Create file for storing large bitmap
	hFile = CreateFile(szfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hwnd, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		return 0;
	}
	CloseHandle(hFile);

	bxpage = myScene.g_params.BackBufferWidth;
	bypage = myScene.g_params.BackBufferHeight;


	myScene.g_pDevice->CreateOffscreenPlainSurface(bxpage, bypage, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &pFrontBuffer, NULL);
	if (D3D_OK != myScene.g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer))
	{
		MessageBox(NULL, L"Can't Get Back Buffer", L"Error", MB_OK);
	}
	D3DXSaveSurfaceToFile(szfile, (D3DXIMAGE_FILEFORMAT)0, pFrontBuffer, NULL, NULL);//0-bmp
	SAFE_RELEASE(pFrontBuffer);

	//--------------------------------------------------------------------------------------------------------------
	ZeroMemory(&hBitmap, sizeof(HBITMAP));
	hBitmap = (HBITMAP)LoadImage(NULL, szfile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);

	if (hBitmap)
	{
		HDC prn;
		prn = GetPrinterDC(hwnd);
		if (prn == 0)
		{
			MessageBox(NULL, L"Can't open printer.", L"Error", MB_OK | MB_ICONERROR);
			return 0;
		}

		int cxpage = GetDeviceCaps(prn, HORZRES);
		int cypage = GetDeviceCaps(prn, VERTRES);

		HDC hdcMem = CreateCompatibleDC(prn);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

		DOCINFO di = { sizeof(DOCINFO), TEXT("Printing 3D scene...") };
		StartDoc(prn, &di);
		StartPage(prn);
		SetMapMode(prn, MM_ISOTROPIC);
		SetWindowExtEx(prn, cxpage, cypage, NULL);
		SetViewportExtEx(prn, cxpage, cypage, NULL);
		SetViewportOrgEx(prn, 0, 0, NULL);
		StretchBlt(prn, 0, 0, cxpage, cypage, hdcMem, 0, 0, bxpage, bypage, SRCCOPY);

		EndPage(prn);
		EndDoc(prn);
		DeleteDC(prn);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteBitmap(hBitmap);
	}
	else
		MessageBox(NULL, L"Can't read 3D screen data", L"Error", MB_OK | MB_ICONERROR);

	return 0;
}


HDC GetPrinterDC(HWND Hwnd)
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

//-----------------------------------------------------------------------------------------------------------------
//Purpose: copy 3d sufrace to clipobard
//Created by Daniel Kaminski
// 18.02.2013 Lublin
//-----------------------------------------------------------------------------------------------------------------
void CopySurface(HWND hwnd)
{ 
	WCHAR szfile[] = L"temp.tmp";
	HBITMAP hBitmap;
	HANDLE			hFile;
	LPDIRECT3DSURFACE9 pFrontBuffer;
	int bxpage;
	int bypage;

	//Create file for storing large bitmap
	hFile = CreateFile(szfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hwnd, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		return;
	}
	CloseHandle(hFile);

	bxpage = myScene.g_params.BackBufferWidth;
	bypage = myScene.g_params.BackBufferHeight;


	myScene.g_pDevice->CreateOffscreenPlainSurface(bxpage, bypage, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &pFrontBuffer, NULL);
	if (D3D_OK != myScene.g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer))
	{
		MessageBox(NULL, L"Can't Get Back Buffer", L"Error", MB_OK);
	}
	D3DXSaveSurfaceToFile(szfile, (D3DXIMAGE_FILEFORMAT)0, pFrontBuffer, NULL, NULL);//0-bmp
	SAFE_RELEASE(pFrontBuffer);

	//--------------------------------------------------------------------------------------------------------------
	ZeroMemory(&hBitmap, sizeof(HBITMAP));
	hBitmap = (HBITMAP)LoadImage(NULL, szfile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);

	if (hBitmap)
	{

		if (!::OpenClipboard(hwnd))
			return;
		::EmptyClipboard();

		BITMAP bm;
		::GetObject(hBitmap, sizeof(bm), &bm);

		BITMAPINFOHEADER bi;
		::ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = bm.bmWidth;
		bi.biHeight = bm.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = bm.bmBitsPixel;
		bi.biCompression = BI_RGB;
		if (bi.biBitCount <= 1)	// make sure bits per pixel is valid
			bi.biBitCount = 1;
		else if (bi.biBitCount <= 4)
			bi.biBitCount = 4;
		else if (bi.biBitCount <= 8)
			bi.biBitCount = 8;
		else // if greater than 8-bit, force to 24-bit
			bi.biBitCount = 24;

		// Get size of color table.
		SIZE_T dwColTableLen = (bi.biBitCount <= 8) ? (1i64 << bi.biBitCount) * sizeof(RGBQUAD) : 0;

		// Create a device context with palette
		HDC hDC = ::GetDC(NULL);
		HPALETTE hPal = static_cast<HPALETTE>(::GetStockObject(DEFAULT_PALETTE));
		HPALETTE hOldPal = ::SelectPalette(hDC, hPal, FALSE);
		::RealizePalette(hDC);

		// Use GetDIBits to calculate the image size.
		::GetDIBits(hDC, hBitmap, 0, static_cast<UINT>(bi.biHeight), NULL,
			reinterpret_cast<LPBITMAPINFO>(&bi), DIB_RGB_COLORS);
		// If the driver did not fill in the biSizeImage field, then compute it.
		// Each scan line of the image is aligned on a DWORD (32bit) boundary.
		if (0 == bi.biSizeImage)
			bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) * bi.biHeight;

		// Allocate memory
		HGLOBAL hDIB = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + dwColTableLen + bi.biSizeImage);
		if (hDIB)
		{
			union tagHdr_u
			{
				LPVOID             p;
				LPBYTE             pByte;
				LPBITMAPINFOHEADER pHdr;
				LPBITMAPINFO       pInfo;
			} Hdr;

			Hdr.p = ::GlobalLock(hDIB);
			// Copy the header
			::CopyMemory(Hdr.p, &bi, sizeof(BITMAPINFOHEADER));
			// Convert/copy the image bits and create the color table
			int nConv = ::GetDIBits(hDC, hBitmap, 0, static_cast<UINT>(bi.biHeight),
				Hdr.pByte + sizeof(BITMAPINFOHEADER) + dwColTableLen,
				Hdr.pInfo, DIB_RGB_COLORS);
			::GlobalUnlock(hDIB);
			if (!nConv)
			{
				::GlobalFree(hDIB);
				hDIB = NULL;
			}
		}
		if (hDIB)
			::SetClipboardData(CF_DIB, hDIB);

		DeleteBitmap(hBitmap);
		::CloseClipboard();
		::SelectPalette(hDC, hOldPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return;// NULL != hDib;
	}
	else
		MessageBox(NULL, L"Can't read temporary data", L"Error", MB_OK | MB_ICONERROR);
	DeleteBitmap(hBitmap);
	return;

}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the settings to macro file after program is closed
//Function created by Prof. Elias Vlieg
//Modified by Daniel Kaminski 18.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool SaveMacroFile(){

 FILE*   init_mac = NULL;
 WCHAR Wstr[MAX_PATH];
 char str [MAX_PATH]={};
 char *item;
 errno_t err;

	//find the program patch
	if(GetModuleFileName(NULL, Wstr, MAX_PATH))
		{
		WCHAR* t = wcsrchr(Wstr,'\\');
		if(t!=NULL)
			{
			wcscpy_s(t, MAX_PATH, L"\\\\");
			t = wcsrchr(Wstr,'\\');
			wcscpy_s(t, MAX_PATH, WINROD_INIT_MACRO); //Vlad
			}
		else MessageBox(NULL, TEXT("Incorrect mesh file  path!"), NULL, MB_ICONERROR); //DK
	}
	TtoA(str, Wstr);

	err = fopen_s(&init_mac, str, "wt" );
	if(err != NULL){
		MessageBox( NULL, L"Unable to create WinRod macro file", L"Error", MB_OK|MB_ICONERROR);
		return false;
	} 

   /* Save parameters for rod calculation */
    fprintf (init_mac, "!Go to set calculate menu \n");
    fprintf (init_mac, "set calculate     \n");
    fprintf (init_mac, "structure    %s   \n", yesnostr (thread.STRUCFAC));
    fprintf (init_mac, "hstart   %10.4f   \n", variable.h_start);
    fprintf (init_mac, "hend     %10.4f   \n", variable.h_end);
	fprintf (init_mac, "kstart   %10.4f   \n", variable.k_start);
	fprintf (init_mac, "kend     %10.4f   \n", variable.k_end);
	fprintf (init_mac, "lstart   %10.4f   \n", variable.l_start);
	fprintf (init_mac, "lend     %10.4f   \n", variable.l_end);

    fprintf (init_mac, "npoints     %4d   \n", variable.N_points);
    fprintf (init_mac, "atten      %10.6f \n", thread.ATTEN);
    fprintf (init_mac, "beta       %10.6f \n", thread.BETA);
    fprintf (init_mac, "lbragg     %d \n", thread.LBR[0]);
    fprintf (init_mac, "nlayers      %1d  \n", thread.NLAYERS);
    fprintf (init_mac, "scale      %10.6f \n", thread.SCALE);
    fprintf (init_mac, "sfraction  %10.6f \n", thread.SURFFRAC);
	if (thread.NSURF2>0)
		fprintf(init_mac, "s2fraction  %10.6f \n", thread.SURF2FRAC);
	fprintf (init_mac, "fractional  %s     \n", yesnostr(thread.ZEROFRACT));      //new line for surface fractional coordinate
	fprintf (init_mac, "bul_fractional  %s \n", yesnostr(thread.BULK_ZEROFRACT));  //new line for bulk fractional coordinate
    fprintf (init_mac, "z_mean     %10.6f \n", variable.z_mean);    //new line for bulk liquid
    fprintf (init_mac, "sigma      %10.6f \n", variable.sigma);      //new line for bulk liquid


    switch (thread.ROUGHMODEL)
	{
	case APPROXBETA:
	    item = "approx";
	    break;
	case POISSONROUGH:
	    item = "poisson";
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
    fprintf (init_mac, "rough        %s \n", item);
    fprintf (init_mac, "return return \n");

    /* Save space group */
    fprintf (init_mac, "\n");
    fprintf (init_mac, "!Go to set symmetry menu \n");
    fprintf (init_mac, "set symmetry %s return return \n", variable.PLANEGROUP);

    /* Save matrices for multi-domain structure calculation */

    fprintf (init_mac, "!Go to set domain menu \n");
    fprintf (init_mac, "set domain \n");
    fprintf (init_mac, "ndomain      %1d \n", thread.NDOMAIN);

	for (int i = 0; i < thread.NDOMAIN; i++)
	{
	fprintf (init_mac, "matrix       %1d %8.4f %8.4f %8.4f %8.4f \n",
		 i + 1, thread.DOMMAT11[i], thread.DOMMAT12[i], thread.DOMMAT21[i], thread.DOMMAT22[i]);
	}

    fprintf (init_mac, "fractional   %s \n", yesnostr (thread.ZEROFRACT));
    fprintf (init_mac, "equal        %s \n", yesnostr (variable.DOMEQUAL));
    if (!variable.DOMEQUAL)
	for (int i = 0; i < thread.NDOMAIN; i++)
	    {
	    fprintf (init_mac, "occup    %1d %7.4f \n", i + 1, thread.DOMOCCUP[i]);
	    }
    fprintf (init_mac,"coherent     %s \n", yesnostr(thread.COHERENTDOMAINS));
    fprintf (init_mac, "return return \n");

	//Save the error calculation method
	if (variable.LSQ_ERRORCALC == CHISQR)
		fprintf(init_mac, "LSQ_ERRORCALC   CHISQR \n");
	if (variable.LSQ_ERRORCALC == COVARIANCE)
		fprintf(init_mac, "LSQ_ERRORCALC   COVARIANCE \n");


	/* Save setting of windows */
	fprintf(init_mac, "\n\n!Go to windows settings\n");
	for (int i = 0; i < MAX_WINDOWS_NUMBER; i++){
		fprintf(init_mac, "windows_set     %1d %1d %1d %1d %1d %s \n",
			i + 1, variable.WindowsRect[i].left, variable.WindowsRect[i].top, variable.WindowsRect[i].right, variable.WindowsRect[i].bottom, yesnostr(variable.WindowsVisible[i]));
	}

	/* Save setting of windows colors */
	fprintf(init_mac, "\n\n!Go to windows color settings\n");
	fprintf(init_mac, "Window_colors  %1d   %8d %8d %8d \n\n", 0, myScene.flag.rgbCurrent, myScene.flag.rgbLight, myScene.flag.rgbText);
	for (int i = 0; i < TOTAL_2D_WINDOWS; ++i)
	{
		fprintf(init_mac, "Window_colors  %1d   %8d %8d %8d \n", i+1, my2D[i].rgbColors.background, my2D[i].rgbColors.framecolor, my2D[i].rgbColors.text);
	}
	fprintf(init_mac, "\n\n");

    /* Save setting of thread */
    fprintf (init_mac, "!Go to set fit menu\n");
	fprintf(init_mac, "Procesor_cores %d\n\n", thread.NumberOfCores);

	//Priority
	if (thread.priority == THREAD_PRIORITY_LOWEST) 	fprintf(init_mac, "Priority THREAD_PRIORITY_LOWEST");
	if (thread.priority == THREAD_PRIORITY_BELOW_NORMAL) 	fprintf(init_mac, "Priority THREAD_PRIORITY_BELOW_NORMAL");
	if (thread.priority == THREAD_PRIORITY_NORMAL) 	fprintf(init_mac, "Priority THREAD_PRIORITY_NORMAL");
	if (thread.priority == THREAD_PRIORITY_ABOVE_NORMAL) 	fprintf(init_mac, "Priority THREAD_PRIORITY_ABOVE_NORMAL");
	if (thread.priority == THREAD_PRIORITY_HIGHEST) 	fprintf(init_mac, "Priority THREAD_PRIORITY_HIGHEST");


	
	/* Save setting of ASA */
	fprintf(init_mac, "\n\n!Go to set fit asa menu\n");
	fprintf(init_mac, "TEMPERATURE_RATIO_SCALE %5.1e\n", vfsr_par.TEMPERATURE_RATIO_SCALE);
	fprintf(init_mac, "COST_PARAMETER_SCALE %5.2f\n", vfsr_par.COST_PARAMETER_SCALE);
	fprintf(init_mac, "ACCEPTED_TO_GENERATED_RATIO %5.1e\n", vfsr_par.ACCEPTED_TO_GENERATED_RATIO);
	fprintf(init_mac, "INITIAL_PARAMETER_TEMPERATURE %5.1e\n", vfsr_par.INITIAL_PARAMETER_TEMPERATURE);
	fprintf(init_mac, "LIMIT_ACCEPTANCES %d\n", vfsr_par.LIMIT_ACCEPTANCES);
	fprintf(init_mac, "TESTING_FREQUENCY_MODULUS %d\n", vfsr_par.TESTING_FREQUENCY_MODULUS);
	
	fprintf(init_mac, "\nFit_file %d %s %s\n", variable.DetailsInFile, variable.NameTestFitFile, yesnostr(variable.PrintTestFitFile));

 //   if (FULLMODEL)
//	fprintf (init_mac, "set fit fullmodel yes return return\n");
//    else
//	fprintf (init_mac, "set fit fullmodel no return return\n");

    /* Save parameters for plotting */

	fprintf(init_mac, "\n\n!Go to multiplot\n");
	fprintf(init_mac, "Multiplot %d %d %d \n", variable.NumberOfMultiplots, variable.Multi_N_column, variable.Multi_L_points);

	for (int i = 0; i < variable.NumberOfMultiplots; i++){
		fprintf(init_mac, "Multiplot_select %d %s \n",
			i, yesnostr(variable.Multi_SelectPlot[i]));
	}




#ifdef ALLOW_PLOT
    fprintf (init_mac, "!Go to set plot menu\n");
    fprintf (init_mac, "set plot\n");
    fprintf (init_mac, "size %7.4f\n", FSIZE);
    fprintf (init_mac, "threshold %7.4f\n", FTHRESHOLD);
    for (i = 0; i < NTYPES; i++)
	fprintf (init_mac, "radius %1d %5.3f\n", i + 1, ATRAD[i]);
    fprintf (init_mac, "xmincon %7.4f\n", XMINCON);
    fprintf (init_mac, "xmaxcon %7.4f\n", XMAXCON);
    fprintf (init_mac, "nxcon %1d\n", NXCON);
    fprintf (init_mac, "ymincon %7.4f\n", YMINCON);
    fprintf (init_mac, "ymaxcon %7.4f\n", YMAXCON);
    fprintf (init_mac, "nycon %1d\n", NYCON);
    fprintf (init_mac, "minlevel %7.4f\n", MINCON);
    fprintf (init_mac, "maxlevel %7.4f\n", MAXCON);
    fprintf (init_mac, "nlevel %1d\n", NLEVEL);
//  fprintf (init_mac, "file %s\n", PATFILENAME);
    fprintf (init_mac, "return return\n");
#endif

    /* Save parameters for Keating */
#ifdef KEATING
    fprintf (init_mac, "!Go to lattice energy menu\n");
    fprintf (init_mac, "energy\n");
    if (POTENTIAL == VKEATING) fprintf (init_mac, "keating\n");
    if (POTENTIAL == VLENNARDJONES) fprintf (init_mac, "lennard\n");
//  fprintf (init_mac, "chisqr %s\n",yesnostr(KEAT_PLUS_CHI));
    fprintf (init_mac, "alpha %7.4f\n", ALPHA_KEAT);
    fprintf (init_mac, "beta %7.4f\n", BETA_KEAT);
    fprintf (init_mac, "equangle %7.4f\n",EQU_ANGLE*RAD);
    for (i = 0; i < NTYPES; i++)
	{
	fprintf (init_mac, "radius %1d %8.5f\n", i + 1, ATOM_RAD[i]);
	}
    fprintf (init_mac, "return\n");
#endif
	/*
	sprintf_s( str, 100, "Test drutu");
	size_t lenght = strlen(str);
	fwrite(str, lenght, 1, init_mac);
	*/

	if(init_mac)      fclose(init_mac);


return true;
}



//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the parameter file
//Function created by Prof. Elias Vlieg
//Modified by Daniel Kaminski 18.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool SaveParameterFile(char *FileName){

 FILE*   pF = NULL;
 char str [MAX_PATH]={};
 char *item;
 int i, j;

	errno_t err = fopen_s(&pF, FileName ,"wt" );
	if(err != NULL){
		sprintf_s(str, COMMLENGHT, "%s", FileName);
		MessageBox( NULL, L"Unable to create parameter file", L"Error", MB_OK|MB_ICONERROR);
		return false;
	} 

   /* Save parameters for rod calculation */

	fprintf(pF, "%s\n", variable.FitHeader);
	fprintf (pF, "________________________________________________________________\n"   );
	fprintf (pF, "#Parameter           #Value    #Low lim.   #Up lim.  #Fixed\n");
	fprintf (pF, "________________________________________________________________\n"  );
//                x               
    fprintf (pF, "scale             %10.6f %10.3f %10.3f   %s\n",     thread.SCALE,    variable.SCALELIM[0],    variable.SCALELIM[1],    yesnostr (variable.SCALEPEN)   );
	//subscale
	for (i = 0; i < variable.NSUBSCALETOT; i++){
		fprintf(pF, "subscale  %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.SUBSCALE[i], variable.SUBSCALELIM[i][0], variable.SUBSCALELIM[i][1], yesnostr(variable.SUBSCALEPEN[i]));
	}
	fprintf (pF, "beta              %10.6f %10.3f %10.3f   %s\n",      thread.BETA,     variable.BETALIM[0],     variable.BETALIM[1],     yesnostr (variable.BETAPEN)    );
    fprintf (pF, "sfraction         %10.6f %10.3f %10.3f   %s\n", thread.SURFFRAC, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1], yesnostr (variable.SURFFRACPEN));
	if (thread.NSURF2>0)
		fprintf(pF, "s2fraction         %10.6f %10.3f %10.3f   %s\n", thread.SURF2FRAC, variable.SURF2FRACLIM[0], variable.SURF2FRACLIM[1], yesnostr(variable.SURF2FRACPEN));


    for (i = 0; i < variable.NDISTOT; i++){
//                    x               x
		fprintf (pF, "displace  %4d    %10.6f %10.3f %10.3f   %s\n", i+1, variable.DISPL[i], variable.DISPLLIM[i][0], variable.DISPLLIM[i][1], yesnostr (variable.DISPLPEN[i]));
	}

    for (i = 0; i < thread.NDWTOT; i++){
		fprintf (pF, "b1        %4d    %10.6f %10.3f %10.3f   %s\n", i+1, thread.DEBWAL[i], variable.DEBWALLIM[i][0], variable.DEBWALLIM[i][1], yesnostr (variable.DEBWALPEN[i]));
	}

    for (i = 0; i < thread.NDWTOT2; i++){
		fprintf (pF, "b2        %4d    %10.6f %10.3f %10.3f   %s\n", i+1, thread.DEBWAL2[i], variable.DEBWAL2LIM[i][0], variable.DEBWAL2LIM[i][1], yesnostr (variable.DEBWAL2PEN[i]));
	}

    for (i = 0; i < variable.NOCCTOT; i++){
		fprintf (pF, "occupancy %4d    %10.6f %10.3f %10.3f   %s\n", i+1, thread.OCCUP[i], variable.OCCUPLIM[i][0], variable.OCCUPLIM[i][1], yesnostr (variable.OCCUPPEN[i]));
	}

	for (i = 0; i < variable.NOCCTOT2; i++) {
		fprintf(pF, "occupancy2 %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.OCCUP2[i], variable.OCCUP2LIM[i][0], variable.OCCUP2LIM[i][1], yesnostr(variable.OCCUP2PEN[i]));
	}

	for (i = 0; i < variable.NFACTOR; i++) {
		fprintf(pF, "factor %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.FACTOR[i], variable.FACTOR_LIM[i][0], variable.FACTOR_LIM[i][1], yesnostr(variable.FACTOR_PEN[i]));
	}
	//Rotations
	for (i = 0; i < variable.ROTATIONTOTX; i++){
		fprintf(pF, "rotation x %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, variable.ROTATIONX[i], variable.ROTATIONLIMX[i][0], variable.ROTATIONLIMX[i][1], yesnostr(variable.ROTATIONPENX[i]));
	}

	for (i = 0; i < variable.ROTATIONTOTY; i++){
		fprintf(pF, "rotation y %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, variable.ROTATIONY[i], variable.ROTATIONLIMY[i][0], variable.ROTATIONLIMY[i][1], yesnostr(variable.ROTATIONPENY[i]));
	}

	for (i = 0; i < variable.ROTATIONTOTZ; i++){
		fprintf(pF, "rotation z %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, variable.ROTATIONZ[i], variable.ROTATIONLIMZ[i][0], variable.ROTATIONLIMZ[i][1], yesnostr(variable.ROTATIONPENZ[i]));
	}

	if (variable.DOMOCCUPAUTO)
		for (i = 0; i < thread.NDOMAIN; i++){
		fprintf(pF, "domain_occ %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.DOMOCCUP[i], variable.DOMOCCUPLIM[i][0], variable.DOMOCCUPLIM[i][1], yesnostr(variable.DOMOCCUPPEN[i]));
		}

	fprintf(pF, "\n");
	for (i = 0; i < thread.NTYPES; i++)
	{
		for (j = 0; j < variable.ENERGYTOT + 1; j++)
		{
			fprintf(pF, "dispersion %2s %2d %8.4f %8.4f\n",
				variable.ELEMENT[i], j, thread.F1[i][j], thread.F2[i][j]);
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
    fprintf (pF, "rough %s\n", item);

	
	if (thread.LIQUID_PROFILE)
		fprintf(pF, "liquid_profile  yes\n");
	else
		fprintf(pF, "liquid_profile  no\n");

	//Second surface
	if (thread.NSURF2>0)
		fprintf(pF, "nsurf2 %d\n", thread.NSURF2);

	if(pF)      fclose(pF);

return true;
}


//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the parameter file
//Function created by Prof. Elias Vlieg
//Modified by Daniel Kaminski 18.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool SaveFitFile(char *FileName){

 FILE*   fF = NULL;
 char str [MAX_PATH]={};
 int i;

	errno_t err = fopen_s(&fF, FileName ,"wt" );
	if(err != NULL){
		sprintf_s(str, COMMLENGHT, "%s", FileName);
		MessageBox( NULL, L"Unable to create parameter file", L"Error", MB_OK|MB_ICONERROR);
		return false;
	} 

   /* Save parameters for rod calculation */

	fprintf(fF, "%s\n", variable.FitHeader);
	fprintf (fF, "%4.4f %4.4f %4.4f %4.3f %4.3f %4.3f\n", thread.DLAT[0], thread.DLAT[1], thread.DLAT[2], thread.DLAT[3]*rad2deg, thread.DLAT[4]*rad2deg, thread.DLAT[5]*rad2deg);
    for (i = 0; i < thread.NSURF+thread.NSURF2; ++i){
		fprintf (fF, "%s %7.4f  %7.4f %1d %7.4f %1d ", variable.ELEMENT[thread.TS[i]], variable.XS[i], variable.XCONST[i], variable.NXDIS[i], variable.X2CONST[i], variable.NX2DIS[i]  );
		fprintf (fF, "%7.4f %7.4f %1d %7.4f %1d ", variable.YS[i], variable.YCONST[i], variable.NYDIS[i], variable.Y2CONST[i], variable.NY2DIS[i]  );
		fprintf(fF, "%7.4f %7.4f %d %7.4f %1d ", variable.ZS[i], variable.ZCONST[i], variable.NZDIS[i], variable.Z2CONST[i], variable.NZ2DIS[i]);

		if (!thread.OCCUP_FLAG[i])
		fprintf (fF, "%1d %1d %1d %1d\n", thread.NDWS[i], thread.NDWS2[i], thread.NOCCUP[i], thread.NOCCUP2[i]);
		else
			fprintf(fF, "%1d %1d -%1d %1d\n", thread.NDWS[i], thread.NDWS2[i], thread.NOCCUP[i], thread.NOCCUP2[i]);

	}

	if(fF)      fclose(fF);

return true;
}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the molecular fit file
//Function created by Daniel Kaminski
//08.09.2014
//-----------------------------------------------------------------------------------------------------------------

bool SaveMolecularFitFile(char *FileName)
{
FILE*   fF = NULL;
char str[MAX_PATH] = {};
int i;
int molecule_nr = 0;


errno_t err = fopen_s(&fF, FileName, "wt");
if (err != NULL){
	sprintf_s(str, MAX_PATH, "%s", FileName);
	MessageBox(NULL, L"Unable to create molecular parameter file", L"Error", MB_OK | MB_ICONERROR);
	return false;
}

/* Save parameters for rod calculation */

fprintf(fF, "%s\n", variable.FitHeader);
for (i = 0; i < thread.NSURF; ++i)
{
	//Add spacer for new molecule
	if (myImport.NewMoleculePositionInData[i])
	{
		fprintf(fF, "molecule #%d    %2s\n", molecule_nr, myImport.myMolecule[molecule_nr].MoleculeName);
		//fprintf(fF, "%4.4f %4.4f %4.4f    (fractional coordinates of molecular center)\n", myImport.MolecularPosition_x[molecule_nr], myImport.MolecularPosition_y[molecule_nr], myImport.MolecularPosition_z[molecule_nr]);

		fprintf(fF, "%7.4f %1d  ", myImport.myMolecule[molecule_nr].MolecularPosition_x, variable.NXDIS[i]);
		fprintf(fF, "%7.4f  %1d  ", myImport.myMolecule[molecule_nr].MolecularPosition_y, variable.NYDIS[i]);
		fprintf(fF, "%7.4f %1d  ", myImport.myMolecule[molecule_nr].MolecularPosition_z, variable.NZDIS[i]);
		fprintf(fF, "%1d %1d %1d ", thread.NDWS[i], thread.NDWS2[i], thread.NOCCUP[i]);
		fprintf(fF, "%1d %1d %1d \n", variable.NROTATIONX[molecule_nr], variable.NROTATIONY[molecule_nr], variable.NROTATIONZ[molecule_nr]);

		molecule_nr++;
		if (molecule_nr >= MAXMOLECULES)
		{
			MessageBox(NULL, L"Maximum number of molecules reach", L"Error", MB_OK | MB_ICONERROR);
			break;
		}
	}
	fprintf(fF, "%2s %7.4f  %7.4f %7.4f %d %d\n", variable.ELEMENT[thread.TS[i]], myImport.x_cartesian[i], myImport.y_cartesian[i], myImport.z_cartesian[i], thread.NDWS[i], thread.NOCCUP[i]);

}
if (fF)      fclose(fF);

return true;
}


//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the parameter file
//Function created by Prof. Elias Vlieg
//Modified by Daniel Kaminski 18.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool SaveBulFile(char *FileName){

 FILE*   fF = NULL;
 char str [MAX_PATH]={};
 int i;

	errno_t err = fopen_s(&fF, FileName ,"wt" );
	if(err != NULL){
		sprintf_s(str, COMMLENGHT, "%s", FileName);
		MessageBox( NULL, L"Unable to create parameter file", L"Error", MB_OK|MB_ICONERROR);
		return false;
	} 

   /* Save parameters for rod calculation */

	fprintf(fF, "%s\n", variable.BulHeader);
	fprintf (fF, "%4.4f %4.4f %4.4f %4.3f %4.3f %4.3f\n", thread.DLAT[0], thread.DLAT[1], thread.DLAT[2], thread.DLAT[3]*rad2deg, thread.DLAT[4]*rad2deg, thread.DLAT[5]*rad2deg);
    for (i = 0; i < thread.NBULK; ++i){
		fprintf (fF, "%2s %7.4f  %7.4f  %7.4f %1d\n", variable.ELEMENT[thread.TB[i]], thread.XB[i], thread.YB[i], thread.ZB[i], thread.NDWB[i]  );
	}

	if(fF)      fclose(fF);

return true;
}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves data file
//Function created by PDaniel Kaminski
// 14.10.2014
//-----------------------------------------------------------------------------------------------------------------
bool SaveDatFile(char *FileName){

	/*
	Read file with truncation rod data.
	First line: comments
	Data:       h       k       l       f       error   dataflag

	dataflag is a float of the form mnnii.x with
	m  serial number of energy parameter (0's if only one energy)
	nn serial number of subscale parameter
	ii (integer) value of nearest Bragg peak (for approximate beta model)
	x  if non-zero, data point is treated as fractional data point
	*/


	FILE*   fF = NULL;
	char str[MAX_PATH] = {};
	int i;

	errno_t err = fopen_s(&fF, FileName, "wt");
	if (err != NULL){
		sprintf_s(str, MAX_PATH, "%s", FileName);
		MessageBox(NULL, L"Unable to create data file", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	/* Save parameters for rod calculation */
	int frac;
	fprintf(fF, "(Modified dat file) %s\n", variable.BulHeader);
	for (i = 0; i < thread.NDAT; ++i){
		if (thread.FRAC[i])
			frac = 1;
		else
			frac = 0;
				//    h       k       l        F       dF       NBragg
		fprintf(fF, "%7.6f   %7.6f   %7.6f    %7.6f   %7.6f     %d%2.2d%2.2d.%d  \n", thread.HDAT[i], thread.KDAT[i], thread.LDAT[i], thread.FDAT[i], variable.ERRDAT[i], thread.ENERGY[i], thread.SUBSC[i], thread.LBR[i], frac);
	}

	if (fF)      fclose(fF);

	return true;
}

//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the xyz file with atom coordinates
//Function created by Daniel Kaminski
//27.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool SaveXYZFile(char *FileName){

 FILE*   fF = NULL;
 char str [MAX_PATH]={};
 int i;

	errno_t err = fopen_s(&fF, FileName ,"wt" );
	if(err != NULL){
		sprintf_s(str, MAX_PATH, "%s", FileName);
		MessageBox( NULL, L"Unable to create *.xyz file", L"Error", MB_OK | MB_ICONERROR);
		return false;
	} 

   /* Save parameters for rod calculation */
	fprintf (fF, "%d\n", thread.NBULK+thread.NSURF  );
	fprintf(fF, "%s\n", variable.FitHeader);

	for (i = 0; i < myScene.nr_surf_atoms_toplot; i++){
		fprintf(fF, "%2s %8.4f  %8.4f %8.4f\n", elements[myScene.D3DsurfAtom[i].AtomicNumber-1], myScene.D3DsurfAtom[i].x, myScene.D3DsurfAtom[i].y, myScene.D3DsurfAtom[i].z);
	}
	for (i = 0; i < myScene.nr_bulk_atoms_toplot; i++){
		fprintf(fF, "%2s %8.4f  %8.4f %8.4f\n", elements[myScene.D3DbulkAtom[i].AtomicNumber-1], myScene.D3DbulkAtom[i].x, myScene.D3DbulkAtom[i].y, myScene.D3DbulkAtom[i].z);
	}
	if(fF)      fclose(fF);

return true;
}




//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the settings to macro file after program is closed
//Function created by Daniel Kaminski it should be something simmilar in Rod code but I could not find it.
//Therefore I  recreated it.
//
//18.02.2013
//-----------------------------------------------------------------------------------------------------------------
char* yesnostr(bool value){

static char str[4];

	if (value) sprintf_s(str, 4, "yes");
	else sprintf_s(str, 4, "no");

return str;
}



//-----------------------------------------------------------------------------------------------------------------
//Purpose: Saves the settings to macro file after program is closed
//Function created by Prof. Elias Vlieg
//Modified by Daniel Kaminski 18.02.2013
//-----------------------------------------------------------------------------------------------------------------
bool SaveAtomProperitiesFile(ATOM_PROPERITIES AtProp ){


 FILE*   AtomFile = NULL;
 WCHAR Wstr[MAX_PATH];
 char str [MAX_PATH]={};
 errno_t err;

	//find the probram patch
	if(GetModuleFileName(NULL, Wstr, MAX_PATH))
		{
		WCHAR* t = wcsrchr(Wstr,'\\');
		if(t!=NULL)
			{
			wcscpy_s(t, MAX_PATH, L"\\\\");
			t = wcsrchr(Wstr,'\\');
			wcscpy_s(t, MAX_PATH, ATOMS_INIT_FILE); //Vlad
			}
		else MessageBox(NULL, TEXT("Incorrect mesh file  path!"), NULL, MB_ICONERROR); //DK
	}
	TtoA(str, Wstr);

	err = fopen_s(&AtomFile, str, "wt" );
	if(err != NULL){
		MessageBox( NULL, L"Unable to create file with atomic properities", L"Error", MB_OK|MB_ICONERROR);
		return false;
	} 

   /// Save parameters for atoms 
    fprintf (AtomFile, "WdW radious   Atom color \n");

	for(int i=1; i<98; i++){
		fprintf (AtomFile, "%4s %8.4f  %4.3f %4.3f %4.3f \n", elements[i-1], AtProp.AtomRadius[i], AtProp.AtomColor[i][0], AtProp.AtomColor[i][1], AtProp.AtomColor[i][2]);
	}

	if(AtomFile)      fclose(AtomFile);

return true;
}


void SaveFittingResults(WCHAR *ExitCode, int TimeEnd, int TimeStart, double chisqr)
{
	int i;

	if (file_fit)
	{
		static int FitNr = 1;
		fprintf(file_fit, "Fit number #%d\n", FitNr);
		FitNr++;
		if (variable.DetailsInFile > 4)
		{
			fprintf(file_fit, "TempRatioScale: %4.4g  ", vfsr_par.TEMPERATURE_RATIO_SCALE);
			fprintf(file_fit, "TempAnnScale: %4.4f  ", vfsr_par.TEMPERATURE_ANNEAL_SCALE);
			fprintf(file_fit, "CostParamScale: %4.4f ", vfsr_par.COST_PARAMETER_SCALE);
			fprintf(file_fit, "AcceptToGenRatio: %4.4g ", vfsr_par.ACCEPTED_TO_GENERATED_RATIO);
			fprintf(file_fit, "DeltaX: %4.4g ", vfsr_par.DELTA_X);
			fprintf(file_fit, "InitParTemp: %4.4g ", vfsr_par.INITIAL_PARAMETER_TEMPERATURE);
			fprintf(file_fit, "TESTING_FREQUENCY_MODULUS %d\n", vfsr_par.TESTING_FREQUENCY_MODULUS);
			fprintf(file_fit, "\n ");
		}

		if (variable.DetailsInFile > 1)
		{
			/* Save parameters for rod calculation */

			fprintf(file_fit, "________________________________________________________________\n");
			fprintf(file_fit, "#Parameter           #Value    #Low lim.   #Up lim.  #Fixed\n");
			fprintf(file_fit, "________________________________________________________________\n");
			//                x               
			fprintf(file_fit, "scale             %10.6f %10.3f %10.3f   %s\n", thread.SCALE, variable.SCALELIM[0], variable.SCALELIM[1], yesnostr(variable.SCALEPEN));
			//subscale
			for (i = 0; i < variable.NSUBSCALETOT; i++){
				fprintf(file_fit, "subscale  %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.SUBSCALE[i], variable.SUBSCALELIM[i][0], variable.SUBSCALELIM[i][1], yesnostr(variable.SUBSCALEPEN[i]));
			}
			fprintf(file_fit, "beta              %10.6f %10.3f %10.3f   %s\n", thread.BETA, variable.BETALIM[0], variable.BETALIM[1], yesnostr(variable.BETAPEN));
			fprintf(file_fit, "sfraction         %10.6f %10.3f %10.3f   %s\n", thread.SURFFRAC, variable.SURFFRACLIM[0], variable.SURFFRACLIM[1], yesnostr(variable.SURFFRACPEN));

			for (i = 0; i < variable.NDISTOT; i++){
				//                    x               x
				fprintf(file_fit, "displace  %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, variable.DISPL[i], variable.DISPLLIM[i][0], variable.DISPLLIM[i][1], yesnostr(variable.DISPLPEN[i]));
			}

			for (i = 0; i < thread.NDWTOT; i++){
				fprintf(file_fit, "b1        %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.DEBWAL[i], variable.DEBWALLIM[i][0], variable.DEBWALLIM[i][1], yesnostr(variable.DEBWALPEN[i]));
			}

			for (i = 0; i < thread.NDWTOT2; i++){
				fprintf(file_fit, "b2        %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.DEBWAL2[i], variable.DEBWAL2LIM[i][0], variable.DEBWAL2LIM[i][1], yesnostr(variable.DEBWAL2PEN[i]));
			}

			for (i = 0; i < variable.NOCCTOT; i++){
				fprintf(file_fit, "occupancy %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.OCCUP[i], variable.OCCUPLIM[i][0], variable.OCCUPLIM[i][1], yesnostr(variable.OCCUPPEN[i]));
			}

			for (i = 0; i < variable.NOCCTOT2; i++) {
				fprintf(file_fit, "occupancy2 %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.OCCUP2[i], variable.OCCUP2LIM[i][0], variable.OCCUP2LIM[i][1], yesnostr(variable.OCCUP2PEN[i]));
			}

			for (i = 0; i < variable.NFACTOR; i++) {
				fprintf(file_fit, "factor %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.FACTOR[i], variable.FACTOR_LIM[i][0], variable.FACTOR_LIM[i][1], yesnostr(variable.FACTOR_PEN[i]));
			}
			//Rotations
			for (i = 0; i < variable.ROTATIONTOTX; i++){
				fprintf(file_fit, "rotation x %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, variable.ROTATIONX[i], variable.ROTATIONLIMX[i][0], variable.ROTATIONLIMX[i][1], yesnostr(variable.ROTATIONPENX[i]));
			}

			for (i = 0; i < variable.ROTATIONTOTY; i++){
				fprintf(file_fit, "rotation y %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, variable.ROTATIONY[i], variable.ROTATIONLIMY[i][0], variable.ROTATIONLIMY[i][1], yesnostr(variable.ROTATIONPENY[i]));
			}

			for (i = 0; i < variable.ROTATIONTOTZ; i++){
				fprintf(file_fit, "rotation z %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, variable.ROTATIONZ[i], variable.ROTATIONLIMZ[i][0], variable.ROTATIONLIMZ[i][1], yesnostr(variable.ROTATIONPENZ[i]));
			}

			if (variable.DOMOCCUPAUTO)
				for (i = 0; i < thread.NDOMAIN; i++){
				fprintf(file_fit, "domain_occ %4d    %10.6f %10.3f %10.3f   %s\n", i + 1, thread.DOMOCCUP[i], variable.DOMOCCUPLIM[i][0], variable.DOMOCCUPLIM[i][1], yesnostr(variable.DOMOCCUPPEN[i]));
				}

			fprintf(file_fit, "\n");
			for (i = 0; i < thread.NTYPES; i++)
			{
				for (int j = 0; j < variable.ENERGYTOT + 1; j++)
				{
					fprintf(file_fit, "dispersion %2s %2d %8.4f %8.4f\n",
						variable.ELEMENT[i], j, thread.F1[i][j], thread.F2[i][j]);
				}
			}

			char *item;
			switch (thread.ROUGHMODEL)
			{
			case APPROXBETA:
				item = "approx. beta";
				break;
			case POISSONROUGH:
				item = "poisson";
				break;
			case NUMBETA:
				item = "nuerical beta";
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
			fprintf(file_fit, "rough %s\n", item);
		}

		char str[EXIT_CODE_LENGHT];
		TtoA(str, ExitCode);
		fprintf(file_fit, "Time: %d ms Chsqr= %4.4f exit_code:%s\n", TimeEnd - TimeStart, chisqr, str);
	}
}

