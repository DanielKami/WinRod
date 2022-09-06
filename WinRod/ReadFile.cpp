
//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: ReadFile.cpp provides all the neccesary control for read/write
// Writen by Daniel Kaminski based on the code of Prof. Elias Vlieg
// The read function ferst copy all the data from files to a buffet and than read it.
// This mecanism is different than in ROD which uses line by line reading and translating.
// Parts of the code are comming directly form ROD oryginally writen by Prof. Elias Vlieg
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
//      include files                                                    
//--------------------------------------------------------------------------------------------------------------------------------
#include <stdafx.h>
#include <stdio.h>
#include <commdlg.h>
#include <math.h>
#include <windows.h>

#include "ReadFile.h"
#include "elements.h"
#include "Set.h"

#include "variables.h"
#include "Thread.h"
#include "keating.h"
#include "fit.h"
#include "Import.h"



#ifdef FIT_TEST_PRINT
 FILE*   file_fit = NULL;
#endif

 extern Import						myImport;


DataStruct  Dat;


WCHAR ErrorMsg[100];
//extern Calc calc;
extern double co[MAX_ELEMENTS][9];
//--------------------------------------------------------------------------------------------------------------------------------
  

//--------------------------------------------------------------------------------------------------------------------------------
bool ReadMacroInit(){


 WCHAR szFile[MAX_PATH];

	//find the patch
	if(GetModuleFileName(NULL, szFile, MAX_PATH))
		{
		WCHAR* t = wcsrchr(szFile,'\\');
		wcscpy_s(t, MAX_PATH, L"\\\\");
		t = wcsrchr(szFile,'\\');
		if(t!=NULL)
			{
			wcscpy_s(t, MAX_PATH, WINROD_INIT_MACRO);
			}
		else{
			MessageBox(NULL, TEXT("Incorrect init macro file path!"), NULL, MB_ICONERROR);
			return 0;
		}
	}	

	if( ReadFileFromPatch(szFile, L"mac")==0)
		return false;

	return true;
}


int ReadFileFromPatch(WCHAR *szFile, WCHAR* Extension)
{
	HANDLE			hFile;
	DWORD dwFlag;
	DWORD dwsize;
	//LPSTR szTemporary;

	hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE){
		MessageBox(NULL, TEXT("I can't open the init macro file!!"), NULL, MB_ICONERROR);
		return 0;
	}

	dwsize = GetFileSize(hFile, 0);
	if (dwsize>LENGHT_STR) dwsize = LENGHT_STR;

	//szTemporary = (LPSTR)HeapAlloc(GetProcessHeap(), 0, (dwsize + 1) * sizeof(CHAR));//init macro file buffer
	char* szTemporary = new char[dwsize + 1];

	if (!ReadFile(hFile, szTemporary, dwsize, &dwFlag, NULL))
	{ //fill the buffer
		MessageBox(NULL, TEXT("Read WinRod file failed!"), NULL, MB_ICONERROR);
		delete[] szTemporary;
		return 0;
	}
	/*
	if (!ReadMacroFromBuffer(szTemporary, dwsize)){
		MessageBox(NULL, TEXT("Translating WinRod  macro file failed!"), NULL, MB_ICONERROR);
		return 0;
	}
	*/
	ApplyExtension(szTemporary, Extension, dwsize);

	CloseHandle(hFile);
	delete[] szTemporary;

	//Set domain equal
	if (variable.DOMEQUAL && thread.NDOMAIN>0){
		for (int i = 0; i<thread.NDOMAIN; i++){
			thread.DOMOCCUP[i] = 1. / thread.NDOMAIN;
		}
	}

	return 1;



}
//--------------------------------------------------------------------------------------------------------------------------------
bool ReadAtomInit(HWND hwnd, ATOM_PROPERITIES *AtProp){

 HANDLE	hFile;

 LPSTR szTemporary;
 DWORD dwFlag;
 int dwsize;
 WCHAR szFile[MAX_PATH];

	//find the patch
	if(GetModuleFileName(NULL, szFile, MAX_PATH))
		{
		WCHAR* t = wcsrchr(szFile,'\\');
		wcscpy_s(t, MAX_PATH, L"\\\\");
		t = wcsrchr(szFile,'\\');
		if(t!=NULL)
			{
			wcscpy_s(t, MAX_PATH, ATOMS_INIT_FILE);
			}
		else{
			MessageBox(hwnd, TEXT("Incorrect atom properities file path!"), NULL, MB_ICONERROR);
			return 0;
		}
	}	

	hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if(hFile == INVALID_HANDLE_VALUE){
		MessageBox(hwnd, TEXT("I can't open the atom properities file!"), NULL, MB_ICONERROR);
		return 0;
	}

	dwsize = GetFileSize(hFile, 0);
	if (dwsize>LENGHT_STR) dwsize = LENGHT_STR;

	szTemporary = new char[dwsize + 1];// (LPSTR)HeapAlloc(GetProcessHeap(), 0, (dwsize + 1) * sizeof(CHAR));       // bufor uzywany do zapisu strimu 
	
	if(!ReadFile(hFile, szTemporary, dwsize, &dwFlag, NULL)){ //fill the buffer
		MessageBox(hwnd, TEXT("Read atom properities file failed!"), NULL, MB_ICONERROR); 
		CloseHandle(hFile);
		return 0;
	}

	//Read atom data
	unsigned int Position=0;
	char OutLine[COMMLENGHT];
	char symbol[3];
	float temp1, temp2, temp3, temp4;
	//Read loop
	for (int i=0; i<98; i++){
		if(	Take_line( szTemporary, dwsize, OutLine, COMMLENGHT, &Position)==false) break;
		if (i>0){
			//                    symbol radious color
			sscanf_s(OutLine, "%s%f%f%f%f", symbol, (unsigned int)sizeof(symbol), &temp1, &temp2, &temp3, &temp4);
			AtProp->AtomRadius[i] = temp1;
			AtProp->AtomColor[i][0] = temp2;
			AtProp->AtomColor[i][1] = temp3;
			AtProp->AtomColor[i][2] = temp4;
			}
	}

	delete[] szTemporary;
	CloseHandle(hFile);

return 1;
}



//--------------------------------------------------------------------------------------------------------------------------------
int OpenFileFunction(HWND hwnd){
	//--------------------------------------------------------------------------------------------------------------------------------

	HANDLE			hFile;
	WCHAR szFile[MAX_PATH] = TEXT("\0");
	DWORD			dwSave;
	DWORD  dwsize = 0;
	LPSTR szTemporary;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("All WinRod extensions\0*.bul;*.sur;*.fit;*.dat;*.mac;*.fat;*.par;*.mft\0(*.bul)\0*.bul\0(*.sur)\0*.sur\0(*.dat)\0*.dat\0(*.fit)\0*.fit\0(*.par)\0*.par\0(*.mac)\0*.mac\0(*.fat)\0*.fat\0(*.mft)\0*.mft\0\0");
	ofn.lpstrTitle = TEXT("Open File");
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.lpstrDefExt = TEXT("*.*");
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.nFilterIndex = 1;



	if (GetOpenFileName(&ofn)){
		hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			MessageBox(hwnd, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
			CloseHandle(hFile);
			return 0;
		}
	}
	else return 0;

	dwsize = GetFileSize(hFile, 0);
	szTemporary = new char[dwsize + 1];// (LPSTR)HeapAlloc(GetProcessHeap(), 0, (dwsize + 1) * sizeof(CHAR));       // bufor uzywany do zapisu strimu 
	
	//Extract extension a simple way. It  consider several dots in the string
	WCHAR *Ext = FindLastDot(ofn.lpstrFile);


	//Read file into buffer
	if (!ReadFile(hFile, szTemporary, dwsize, &dwSave, NULL)){
		MessageBox(hwnd, TEXT("Read file failed!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		return 0;
	}

	ApplyExtension(szTemporary, Ext, dwsize);
	
	delete[] szTemporary;
	CloseHandle(hFile);
	
return ofn.nFilterIndex;
}

//Extract extension a simple way. It  consider several dots in the string
//Created 29.11.2014 DK
WCHAR *FindLastDot(WCHAR *Input)
{
	WCHAR *Ext, *Ext_tmp;
	Ext = wcsstr(Input, L".") + 1;
	for (int i = 0; i < 50; ++i)
	{
		Ext_tmp = wcsstr(Ext, L".");
		if (Ext_tmp == NULL)
			return Ext;
		if (wcslen(Ext_tmp) == 0)
			return Ext;
		else
			Ext = Ext_tmp + 1;
	}
	return NULL;
}

int ApplyExtension(char *szTemporary, WCHAR *Extension, int dwsize)
{
	if (CompareStr(Extension, L"bul"))
	{
		ReadDataFromBuffer(szTemporary, 1, dwsize);
		if (variable.MODE == 1)
			//correct atom positions according to the new cell vectors
			myImport.SetMolecule();
		return true;
	}

	if (CompareStr(Extension, L"sur"))
	{
		ReadDataFromBuffer(szTemporary, 2, dwsize);
		if (variable.MODE == 1)
			//correct atom positions according to the new cell vectors
			myImport.SetMolecule();
		return true;
	}

	if (CompareStr(Extension, L"dat"))
	{
		ReadDataFromBuffer(szTemporary, 3, dwsize);
		return true;
	}

	if (CompareStr(Extension, L"fit"))
	{
		ReadDataFromBuffer(szTemporary, 4, dwsize);
		if (variable.MODE == 1)
			//correct atom positions according to the new cell vectors
			myImport.SetMolecule();
		return true;
	}

	if (CompareStr(Extension, L"par"))
	{
		ReadDataFromBuffer(szTemporary, 5, dwsize);
		return true;
	}

	//Read parameter file //typ 6
	if (CompareStr(Extension, L"mac"))
	{
		//myImport.Reset();
		ReadMacroFromBuffer(szTemporary, dwsize);
		return true;
	}

	//Read atomic scattering factors file type 7
	if (CompareStr(Extension, L"fat"))
	{
		ReadScatteringFactorsFromBuffer(szTemporary, dwsize);
		//Calculate new atomic scattering factors
		for (int i = 0; i < thread.NTYPES; ++i)
		{
			get_coeff(variable.ELEMENT[i], thread.F_COEFF[i]);
		}

		//Update coeficients also for organic molecules
		if (variable.MODE == 1)
			myImport.SetMolecule();
		return true;
	}

	//Read molecules
	if (CompareStr(Extension, L"mft"))
	{
		myImport.Reset();
		variable.MODE = 1;

		ReadDataFromBuffer(szTemporary, 8, dwsize);
		myImport.TotalMolecules++;
		myImport.SetMolecule();
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
bool ReadDataFromBuffer(LPSTR TStr, int DataType, DWORD size){
	//--------------------------------------------------------------------------------------------------------------------------------
	char OutLine[COMMLENGHT];

	unsigned int Position = 0;
	switch (DataType){
	case 1:
		thread.NBULK = 0;
		break;
	case 2:
		thread.NSURF = 0;
		break;
	case 3:
		thread.NDAT = 0;
		break;
	case 8:
	case 4:
		thread.NSURF = 0;
		break;
	}

	int i = 0;
	//for (int i=0; i<max_iterations; ++i){
	while (Take_line(TStr, size, OutLine, COMMLENGHT, &Position))
	{
		if (DataType == 1)
			if (!Read_Bul(OutLine, i)) return false;
		if (DataType == 2)
			if (!Read_Sur(OutLine, i)) return false;
		if (DataType == 3)
			if (!Read_Dat(OutLine, i)) return false;
		if (DataType == 4)
			if (!Read_Fit(OutLine, i)) return false;
		if (DataType == 5)
			if (!Read_Par(OutLine, i)) return false;
		if (DataType == 8)
			if (!Read_Mft(OutLine, i)) return false;
		i++;
	}


	if (DataType == 2 || DataType == 4)
	{
		variable.NSURFTOT = thread.NSURF;
	}

	if (DataType == 4)
	{
		update_model();
	}

	if (DataType == 5)
	{
		update_model();
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
bool ReadMacroFromBuffer(LPSTR TStr, DWORD size){
	//--------------------------------------------------------------------------------------------------------------------------------
	char OutLine[COMMLENGHT];
	int max_iterations = MAX_MACRO_LINES;//Maximum number of line in Mac file
	unsigned int Position = 0;


#ifdef FIT_TEST_PRINT
	if (variable.PrintTestFitFile)
	{
		OpenPrintFitFile();
	}
#endif


	for (int i = 0; i < max_iterations; ++i)
	{
		if (!Take_line(TStr, size, OutLine, COMMLENGHT, &Position))
			break;
		if (!Read_Macro(OutLine, i, &Position))
			break;
	}

	if (variable.DOMEQUAL)
	{
		for (int i = 0; i < thread.NDOMAIN; i++)
		{
			thread.DOMOCCUP[i] = 1. / thread.NDOMAIN;
		}
	}



	return true;
}

#ifdef FIT_TEST_PRINT

void OpenPrintFitFile()
{
	if (file_fit){
		fclose(file_fit);
		file_fit = NULL;
	}
	if (!file_fit){
		errno_t err = fopen_s(&file_fit, variable.NameTestFitFile, "wt");
		if (err != NULL){
			MessageBox(NULL, L"Unable to create WinRod test print file", L"Error", MB_OK | MB_ICONERROR);
			
		}
	}
}

void ClosePrintFitFile()
{
	if (file_fit)
	{
		fclose(file_fit);
	}
}
#endif

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
bool ReadScatteringFactorsFromBuffer(LPSTR TStr, DWORD size){
	//--------------------------------------------------------------------------------------------------------------------------------
	char OutLine[COMMLENGHT];
	int max_iterations = MAX_ELEMENTS;//Maximum number of line 
	unsigned int Position = 0;

	for (int i = 0; i<max_iterations; i++)
	{
		if (Take_line(TStr, size, OutLine, COMMLENGHT, &Position) == false) break;
		if (!Read_ScatteringFactors(OutLine, i, &Position)) return false;
	}

	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------
bool Take_line(char *instring, DWORD InpStringSize, char *outstring, unsigned  int OutStringSize, unsigned int *Position){
//--------------------------------------------------------------------------------------------------------------------------------

  unsigned int x, y;

	ZeroMemory(	outstring, sizeof(char)*OutStringSize);
	y=0;
	for (x=*Position;  x<InpStringSize; x++){
		if(instring[x]!=13 && instring[x+1]!=10){
			outstring[y]=instring[x];
			y++;
			if(y>=OutStringSize-1) break;
		}
		else break;
	}
	outstring[y]=0;

	*Position=x+2;//move to the beginning of the new line
	if (*Position>InpStringSize+2) {
		*Position=0;
		return false;
	}
	return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
bool Read_Bul(char *line, int line_number){
	//--------------------------------------------------------------------------------------------------------------------------------

	if (line_number >= MAXATOMS + 2) 
		return false;

	int tmp_i = 0;
	int tmp_i2 = 0;
	char Symbol[3] = "";
	char Name[5] = "";
	float tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, tmp5 = 0, tmp6 = 0;
	double dlat[6];
	int i;

	if (line_number == 0)
	{
		memcpy(variable.BulHeader, line, sizeof(variable.BulHeader));
		return true;
	}

	//read the unit cell parameters
	if (line_number == 1){
		if (sscanf_s(line, "%f%f%f%f%f%f", &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6) == 0) return false;
		dlat[0] = (double)tmp1; dlat[1] = (double)tmp2; dlat[2] = (double)tmp3; dlat[3] = tmp4 / RAD; dlat[4] = tmp5 / RAD; dlat[5] = tmp6 / RAD;

		/* If a surf file has already been read in, check the lattice
		parameters for consistency */

		if (thread.NSURF == 0 || variable.MODE == 1){
			for (i = 0; i < 6; i++) thread.DLAT[i] = dlat[i];
			calc_rlat(thread.DLAT, thread.RLAT);
			//Calculate all neccesary dlat, rlat sin cos constants
			CalculateAcceleratores();
		}
		else{
			for (i = 0; i < 6; i++){
				if (fabs(dlat[i] - thread.DLAT[i]) > 1e-4){
					wsprintf(ErrorMsg, L"ERROR, lattice parameters different from surface!");
					return false;
				}
			}

		}
		return true;
	}


	if (line_number >= 2){
		if (strlen(line) < 1) return true;
		int j = thread.NBULK;
		if (sscanf_s((char*)line, "%s%f%f%f%d%f%s", &Symbol, (unsigned int)sizeof(Symbol), &tmp1, &tmp2, &tmp3, &tmp_i, &tmp4, &Name, (unsigned int)sizeof(Name)) < 2) return true; //emty line

		thread.XB[j] = tmp1; thread.YB[j] = tmp2; thread.ZB[j] = tmp3;
		thread.NDWB[j] = tmp_i;
		thread.ChargeB[j] = tmp4;
		memcpy(thread.Name[j], Name, sizeof(Name));
		thread.NBULK++;


		/* Find the different element types and store them in ELEMENT */
		FindAtomTypes(Symbol);

		/* Assign a type number to atoms in the model */
		for (int i = 0; i < thread.NTYPES; i++){
			if (Symbol[0] == variable.ELEMENT[i][0] && Symbol[1] == variable.ELEMENT[i][1]) thread.TB[j] = i;
		}

		tmp_i = get_element_number(Symbol);
		if (tmp_i == -1){
			wsprintf(ErrorMsg, L"Not asigned atomic number");
			return false;
		}

		/* Find highest serial number of Debye-Waller, if read in */
		if (thread.NDWB[j] > thread.NDWTOT)
		{
			if (thread.NDWB[j] > MAXPAR)
			{
				wsprintf(ErrorMsg, L"ERROR, total # Debye-Wallers should be < %1d", MAXPAR);
				return false;
			}
			else
			{
				thread.NDWTOT = thread.NDWB[j];
			}
		}
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
bool Read_Sur(char *line, int line_number){
	//--------------------------------------------------------------------------------------------------------------------------------

	if (line_number >= MAXATOMS + 2)
		return false;

	char Symbol[3];
	float tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
	int tmp1_i, tmp2_i;
	int i;
	double dlat[6];


	if (line_number == 0){
		memcpy(variable.FitHeader, line, sizeof(variable.FitHeader));
		return true;
	}


	if (line_number == 1){ //read the unit cell parameters
		if (sscanf_s(line, "%f%f%f%f%f%f", &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6) == 0) return false;
		dlat[0] = tmp1; dlat[1] = tmp2; dlat[2] = tmp3; dlat[3] = tmp4 / RAD; dlat[4] = tmp5 / RAD; dlat[5] = tmp6 / RAD;

		/* If a bulk file has already been read in, check the lattice
		parameters for consistency */
		if (thread.NBULK == 0){
			for (i = 0; i < 6; ++i) thread.DLAT[i] = dlat[i];
			calc_rlat(thread.DLAT, thread.RLAT);

			//Calculate all neccesary dlat, rlat sin cos constants
			CalculateAcceleratores();
		}
		else{
			for (i = 0; i < 6; ++i){
				if (fabs(dlat[i] - thread.DLAT[i]) > 1e-4){
					wsprintf(ErrorMsg, L"ERROR, lattice parameters different from bulk");
					return false;
				}
			}
		}
		return true;
	}

	if (line_number >= 2){
		if (strlen(line) < 1) return true;
		tmp1 = tmp2 = tmp3 = tmp4 = tmp5 = 0.;
		tmp1_i = 0; tmp2_i = 0;
		int j = thread.NSURF;
		if (sscanf_s((char*)line, "%s%f%f%f%d%d", &Symbol, (unsigned int)sizeof(Symbol), &tmp1, &tmp2, &tmp3, &tmp1_i, &tmp2_i) < 2)return true;  //empty line

		variable.XS[j] = tmp1; variable.YS[j] = tmp2; variable.ZS[j] = tmp3;
		thread.NDWS[j] = tmp1_i; thread.NDWS2[j] = tmp2_i;


		if (thread.NSURF == MAXATOMS){
			wsprintf(ErrorMsg, L"ERROR, maximum number of model atoms exceeded");
			return false;
		}

		/* Find the different element types and store them in ELEMENT */
		FindAtomTypes(Symbol);

		/* Assign a type number to atoms in the model */
		for (int i = 0; i < thread.NTYPES; i++){
			if (Symbol[0] == variable.ELEMENT[i][0] && Symbol[1] == variable.ELEMENT[i][1]) thread.TS[j] = i;
		}

		// Find highest serial number of Debye-Waller, if read in 
		if (thread.NDWS[j] > thread.NDWTOT){
			if (thread.NDWS[j] > MAXPAR){
				wsprintf(ErrorMsg, L"ERROR, total # Debye-Wallers should be < %1d", MAXPAR);
				return false;
			}
			else{
				thread.NDWTOT = thread.NDWS[j];
			}
		}

		if (thread.NDWS2[j] > thread.NDWTOT2){
			if (thread.NDWS2[j] > MAXPAR){
				wsprintf(ErrorMsg, L"ERROR, total # Debye-Wallers should be < %1d", MAXPAR);
				return false;
			}
			else{
				thread.NDWTOT2 = thread.NDWS2[j];
			}
		}


		tmp1_i = get_element_number(Symbol);
		if (tmp1_i == -1){
			wsprintf(ErrorMsg, L"Not asigned atomic number");
			return false;
		}

		thread.NSURF++;

	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
bool Read_Dat(char *line, int nr){
	//--------------------------------------------------------------------------------------------------------------------------------
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
	if (nr >= MAXDATA + 2)
		return false;

	float tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, tmp5 = 0;
	float dataflag = 0;

	if (nr == 0)
	{
		memcpy(Dat.Header, line, sizeof(Dat.Header));
		AtoT(Dat.THeader, line);
		return true;
	}

	//read the unit cell parameters
	if (nr >= 1)
	{
		if (strlen(line) < 1) return true;
		if (sscanf_s(line, "%f %f %f %f %f %f", &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &dataflag) < 4) return false;
		thread.HDAT[thread.NDAT] = tmp1; thread.KDAT[thread.NDAT] = tmp2; thread.LDAT[thread.NDAT] = tmp3;
		thread.FDAT[thread.NDAT] = tmp4; variable.ERRDAT[thread.NDAT] = tmp5;


		/*
		* Assign the various data point parameters.
		* In previous version, a negative value meant a second scale
		* factor. Now convert such a number to new convention.
		*/
		if (dataflag < 0)
			dataflag = (float)fabs(dataflag) + 100;

		if (fmod(dataflag, 1) > 1e-2)
			thread.FRAC[thread.NDAT] = TRUE;
		else
			thread.FRAC[thread.NDAT] = FALSE;
		//-----------------------------------------------------------------------------------------
		dataflag = dataflag - (float)fmod(dataflag, 1);
		thread.LBR[thread.NDAT] = (int)fmod(dataflag, 100);
		//-----------------------------------------------------------------------------------------
		thread.SUBSC[thread.NDAT] = (int)fmod(dataflag / 100, 100);
		if ((thread.SUBSC[thread.NDAT] < 0) || (thread.SUBSC[thread.NDAT] > MAXPAR))
		{
			thread.SUBSC[thread.NDAT] = 0;
			wsprintf(ErrorMsg, L"ERROR, illegal subscale value, set to 0");
			return false;
		}
		if (thread.SUBSC[thread.NDAT] > variable.NSUBSCALETOT)
			variable.NSUBSCALETOT = thread.SUBSC[thread.NDAT];
		//-----------------------------------------------------------------------------------------
		thread.ENERGY[thread.NDAT] = (int)floor(dataflag / 10000);
		if ((thread.ENERGY[thread.NDAT] < 0) || (thread.ENERGY[thread.NDAT] > MAXENERGIES))
		{
			wsprintf(ErrorMsg, L"ERROR, illegal energy serial value, set to 0");
			thread.ENERGY[thread.NDAT] = 0;
		}
		if (thread.ENERGY[thread.NDAT] > variable.ENERGYTOT)
			variable.ENERGYTOT = thread.ENERGY[thread.NDAT];

		thread.NDAT++;

		if (thread.NDAT == MAX_DATA_POINTS)
		{
			wsprintf(ErrorMsg, L"ERROR, maximum number of data points exceeded %d, file truncated", MAX_DATA_POINTS);
			return false;
		}

		if (thread.NDAT == 0)
		{
			wsprintf(ErrorMsg, L"No data in file");
			return false;
		}

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
    /*
    Read file with coordinates of surface atoms and parameters used for
    fitting.
    First line: comments
    Second:     direct lattice parameters
    Data lines: El X XCONST NXDIS X2CONST NX2DIS
		Y YCONST NYDIS Y2CONST NY2DIS Z NZDIS NDWS NDWS2 NOCCUP
		where
			El      element symbol
			X       x-position in reduced coordinates
			XCONST  multiplication factor of NXDIS
			NXDIS   serial number of x-displacement parameter
			X2CONST multiplication factor of NX2DIS
			NX2DIS  serial number of 2nd x-displacement parameter
			Y       y-position in reduced coordinates
			YCONST  multiplication factor of NYDIS
			NYDIS   serial number of y-displacement parameter
			Y2CONST multiplication factor of NY2DIS
			NY2DIS  serial number of 2nd y-displacement parameter
			Z       z-position in reduced coordinates
			NZDIS   serial number of z-displacement parameter
			NDWS    serial number of Debye-Waller parameter in
				the in-plane direction
			NDWS2   serial number of Debye-Waller parameter in
				the perpendicular direction.
			NOCCUP  serial number of occupancy parameter
    */

//--------------------------------------------------------------------------------------------------------------------------------
bool Read_Fit(char *line, int line_number){
//--------------------------------------------------------------------------------------------------------------------------------
 
	if (line_number >= MAXATOMS + 2)
		return false;

 char Symbol[3];
 float tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, tmp5 = 0, tmp6 = 0, tmp7 = 0, tmp9 = 0, tmp11 = 0, tmp12 = 0, tmp13 = 0;
 int   tmp1_i=0, tmp3_i, tmp5_i, tmp8_i, tmp10_i, tmp12_i, tmp13_i, tmp14_i, tmp15_i, tmp16_i, tmp17_i, tmp18_i;

 int i;
 double dlat[6];


	if(line_number==0){ 
		memcpy(variable.FitHeader, line, sizeof(variable.FitHeader));
	 return true;
	}

	
	if(line_number==1){ //read the unit cell parameters
		if(sscanf_s(line, "%f%f%f%f%f%f", &tmp1, &tmp2, &tmp3, &tmp4 ,&tmp5, &tmp6)==0) return false; 
		dlat[0] = tmp1; dlat[1] = tmp2; dlat[2] = tmp3; dlat[3] = tmp4/RAD; dlat[4] = tmp5/RAD; dlat[5] = tmp6/RAD;
		
		
		/* If a bulk file has already been read in, check the lattice
		parameters for consistency */

		if (thread.NBULK == 0){
			for (i = 0; i < 6; i++) thread.DLAT[i]=dlat[i];
			calc_rlat(thread.DLAT, thread.RLAT);

			//Calculate all neccesary dlat, rlat sin cos constants
			CalculateAcceleratores();
		}
		else{
			for (i = 0; i < 6; ++i){
				if (fabs(thread.DLAT[i]-dlat[i]) > 1e-4){
					wsprintf(ErrorMsg, L"ERROR, lattice parameters in fit file are different from bulk");
					return false;
				}
			}
		}
		return true;
	}

	int number_of_elements = 0;
	if(line_number>=2){ 
		if (strlen(line) < 1) return true;
		tmp1_i = tmp3_i = tmp5_i = tmp8_i = tmp10_i = tmp12_i = tmp13_i = tmp14_i = tmp15_i = tmp16_i = 0;//                            x      s      p        s2      p2       y      s1     p1      s2       p2       z     zconst     p1      s2      p2        dw       dwper      occ       occ2
								//   1  2  3  4  5  6  7  8  9  10 zs zc 13 14 15 16 17                                                 1f     2f     3i       4f      5i       6f     7f     8i      9f      10i       11f   12         12i     13f     14i       15i      16i        17i       18i
		if ((number_of_elements=sscanf_s((char*)line, "%s %f %f %d %f %d %f %f %d %f %d %f %f %d %f %d %d %d %d %d", &Symbol, (unsigned int)sizeof(Symbol), &tmp1, &tmp2, &tmp3_i, &tmp4, &tmp5_i, &tmp6, &tmp7, &tmp8_i, &tmp9, &tmp10_i, &tmp11, &tmp12, &tmp12_i, &tmp13, &tmp14_i, &tmp15_i, &tmp16_i, &tmp17_i, &tmp18_i))<2)return true;

	
	    variable.XS[thread.NSURF] = tmp1;
		variable.XCONST[thread.NSURF] = tmp2;
		variable.NXDIS[thread.NSURF] = tmp3_i;
		variable.X2CONST[thread.NSURF] = tmp4;
		variable.NX2DIS[thread.NSURF] = tmp5_i;

	    variable.YS[thread.NSURF] = tmp6;
		variable.YCONST[thread.NSURF] = tmp7;
		variable.NYDIS[thread.NSURF] = tmp8_i;
	    variable.Y2CONST[thread.NSURF] = tmp9;
		variable.NY2DIS[thread.NSURF] = tmp10_i;

	    variable.ZS[thread.NSURF] = tmp11;
		variable.ZCONST[thread.NSURF] = tmp12;
		variable.NZDIS[thread.NSURF]  = tmp12_i;
		variable.Z2CONST[thread.NSURF] = tmp13;
		variable.NZ2DIS[thread.NSURF] = tmp14_i;


		if (tmp15_i<MAXPAR)
	    thread.NDWS[thread.NSURF] = tmp15_i;

		if (tmp16_i<MAXPAR)
		thread.NDWS2[thread.NSURF]  = tmp16_i;

		if (abs(tmp17_i) < MAXPAR)
		{
			thread.NOCCUP[thread.NSURF] = abs(tmp17_i); //absolute to clear from the potential - which indicate for 1-occ parameter
			if (tmp17_i < 0) thread.OCCUP_FLAG[thread.NSURF] = true;
			else
				thread.OCCUP_FLAG[thread.NSURF] = false;
		}

		if (number_of_elements > 19)
		{
			if (tmp18_i < MAXPAR)
				thread.NOCCUP2[thread.NSURF] = tmp18_i; //absolute to clear from the potential - which indicate for 1-occ parameter
		}
		else
			thread.NOCCUP2[thread.NSURF] = 0;  //To make it more compatybile with old formats of fit file without occ2

		if (thread.NSURF == MAXATOMS){
			wsprintf(ErrorMsg, L"ERROR, maximum number of surface model atoms exceeded");
			return false;
	    }
	
		/* Find the different element types and store them in ELEMENT */
		FindAtomTypes(Symbol);

		/* Assign a type number to atoms in the model */
		for (int i = 0; i < thread.NTYPES; ++i){
			if (Symbol[0] == variable.ELEMENT[i][0] && Symbol[1] == variable.ELEMENT[i][1]) thread.TS[thread.NSURF] = i;
		}

		/* Assign the values of the surface positions to the refined positions */
		thread.XSFIT[thread.NSURF] = variable.XS[thread.NSURF];
		thread.YSFIT[thread.NSURF] = variable.YS[thread.NSURF];
		thread.ZSFIT[thread.NSURF] = variable.ZS[thread.NSURF];
	
		    /* Find highest serial number of the displacement, Debye-Waller and
       occupancy parameters */
		if (variable.NXDIS[thread.NSURF]	> variable.NDISTOT)	variable.NDISTOT =	variable.NXDIS[thread.NSURF];
		if (variable.NX2DIS[thread.NSURF]	> variable.NDISTOT)	variable.NDISTOT =	variable.NX2DIS[thread.NSURF];
		if (variable.NYDIS[thread.NSURF]	> variable.NDISTOT)	variable.NDISTOT =	variable.NYDIS[thread.NSURF];
		if (variable.NY2DIS[thread.NSURF]	> variable.NDISTOT)	variable.NDISTOT =	variable.NY2DIS[thread.NSURF];
		if (variable.NZDIS[thread.NSURF]	> variable.NDISTOT)	variable.NDISTOT =	variable.NZDIS[thread.NSURF];
		if (variable.NZ2DIS[thread.NSURF]	> variable.NDISTOT)	variable.NDISTOT = variable.NZ2DIS[thread.NSURF];
		if (thread.NDWS[thread.NSURF]		> thread.NDWTOT)	thread.NDWTOT =		thread.NDWS[thread.NSURF];
		if (thread.NDWS2[thread.NSURF]		> thread.NDWTOT2)	thread.NDWTOT2 =	thread.NDWS2[thread.NSURF];
		if (thread.NOCCUP[thread.NSURF]		> variable.NOCCTOT) variable.NOCCTOT =	thread.NOCCUP[thread.NSURF];
		if (thread.NOCCUP2[thread.NSURF]	> variable.NOCCTOT2) variable.NOCCTOT2 = thread.NOCCUP2[thread.NSURF];

		if (variable.NDISTOT > MAXPAR){
			variable.NDISTOT = MAXPAR;
			wsprintf(ErrorMsg, L"Error, total number of displacement parameters should be < %1d", MAXPAR+1);
			return false;
		}

		if (thread.NDWTOT > MAXPAR){
			thread.NDWTOT = MAXPAR;
			wsprintf(ErrorMsg, L"Error, total no. of paral. Debye-Waller param. should be < %1d", MAXPAR+1);
			return false;
		}

		if (thread.NDWTOT2 > MAXPAR){
			thread.NDWTOT2 = MAXPAR;
			wsprintf(ErrorMsg, L"Error, total no. of perp. Debye-Waller param. should be < %1d", MAXPAR+1);
			return false;
		}


		if (variable.NOCCTOT > MAXPAR){
			variable.NOCCTOT = MAXPAR;
			wsprintf(ErrorMsg, L"Error, total number of occupancy parameters should be < %1d", MAXPAR+1);
			return false;
		}



		tmp1_i = get_element_number(Symbol);
		if (tmp1_i == -1){
			wsprintf(ErrorMsg, L"Not asigned atomic number");
			return false;
		}
	
		thread.NSURF++;

	}

return true;
}

bool Read_Mft(char *line, int line_number)
{
	if (line_number >= MAXATOMS + 2)
		return false;
 
	char Symbol[3];
	char String[COMMAND];
	static bool read_molecule_header = false;


	if (line_number == 0)
	{
		memcpy(variable.FitHeader, line, (unsigned int)sizeof(variable.FitHeader));
		return true;
	}

	if (sscanf_s((char*)line, "%s", String, (unsigned int)(sizeof(char) * COMMAND)) < 1) return false;
	if (CompareStr(String, "molecule"))
	{
		read_molecule_header = true;

		myImport.NewMoleculePositionInData[thread.NSURF] = true;
		myImport.TotalMolecules++;
		if (sscanf_s((char*)line, "%s%s%s", String, (unsigned int)(sizeof(char) * COMMAND), String, (unsigned int)(sizeof(char) * COMMAND), myImport.myMolecule[myImport.TotalMolecules].MoleculeName, (unsigned int)(sizeof(char) * MOLECULENAME)) < 1) return false;
		return true;
	}


	if (read_molecule_header)
	{
		//Molecule header is readed so set the flag to false
		read_molecule_header = false;

		float tmp_f[3];
		int tmp_i[9];

		//                                                        rotation
		if (sscanf_s((char*)line, "%f%d  %f%d  %f%d  %d %d %d  %d%d%d", &tmp_f[0], &tmp_i[0], &tmp_f[1], &tmp_i[1], &tmp_f[2], &tmp_i[2], &tmp_i[3], &tmp_i[4], &tmp_i[5], &tmp_i[6], &tmp_i[7], &tmp_i[8]) < 2)return true;


		if (tmp_f[0] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].MolecularPosition_x = tmp_f[0];
		if (tmp_i[0] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NXDIS = tmp_i[0];
		if (tmp_f[1] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].MolecularPosition_y = tmp_f[1];
		if (tmp_i[1] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NYDIS = tmp_i[1];
		if (tmp_f[2] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].MolecularPosition_z = tmp_f[2];
		if (tmp_i[2] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NZDIS = tmp_i[2];

		if (tmp_i[3] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NDWS = tmp_i[3];
		if (tmp_i[4] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NDWS2 = tmp_i[4];
		if (tmp_i[5] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NOCCUP = tmp_i[5];

		if (tmp_i[6] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NROTATIONX = tmp_i[6];
		if (tmp_i[7] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NROTATIONY = tmp_i[7];
		if (tmp_i[8] < MAXPAR)
			myImport.myMolecule[myImport.TotalMolecules].NROTATIONZ = tmp_i[8];

		/* Find highest serial number of the displacement, Debye-Waller and
		occupancy parameters */
		if (myImport.myMolecule[myImport.TotalMolecules].NXDIS > variable.NDISTOT)	variable.NDISTOT = myImport.myMolecule[myImport.TotalMolecules].NXDIS;
		if (myImport.myMolecule[myImport.TotalMolecules].NYDIS > variable.NDISTOT)	variable.NDISTOT = myImport.myMolecule[myImport.TotalMolecules].NYDIS;
		if (myImport.myMolecule[myImport.TotalMolecules].NZDIS > variable.NDISTOT)	variable.NDISTOT = myImport.myMolecule[myImport.TotalMolecules].NZDIS;
		if (myImport.myMolecule[myImport.TotalMolecules].NDWS > thread.NDWTOT)	thread.NDWTOT = myImport.myMolecule[myImport.TotalMolecules].NDWS;
		if (myImport.myMolecule[myImport.TotalMolecules].NDWS2 > thread.NDWTOT2)	thread.NDWTOT2 = myImport.myMolecule[myImport.TotalMolecules].NDWS2;
		if (myImport.myMolecule[myImport.TotalMolecules].NOCCUP > variable.NOCCTOT) variable.NOCCTOT = myImport.myMolecule[myImport.TotalMolecules].NOCCUP;
		if (myImport.myMolecule[myImport.TotalMolecules].NROTATIONX > variable.ROTATIONTOTX)	variable.ROTATIONTOTX = variable.NZDIS[thread.NSURF] = myImport.myMolecule[myImport.TotalMolecules].NROTATIONX;
		if (myImport.myMolecule[myImport.TotalMolecules].NROTATIONY > variable.ROTATIONTOTY)	variable.ROTATIONTOTY = variable.NZDIS[thread.NSURF] = myImport.myMolecule[myImport.TotalMolecules].NROTATIONY;
		if (myImport.myMolecule[myImport.TotalMolecules].NROTATIONZ > variable.ROTATIONTOTZ)	variable.ROTATIONTOTZ = variable.NZDIS[thread.NSURF] = myImport.myMolecule[myImport.TotalMolecules].NROTATIONZ;

		return true;
	}


	if (!read_molecule_header){
		if (strlen(line) < 1) return true;

		if (sscanf_s((char*)line, "%s %f %f %f", Symbol, (unsigned int)sizeof(char) * 3, &myImport.x_cartesian[thread.NSURF], &myImport.y_cartesian[thread.NSURF], &myImport.z_cartesian[thread.NSURF]) < 2)return true;
		CopyMemory(myImport.element_from_file[thread.NSURF], Symbol, sizeof(char) * 3);
		thread.OCCUP_FLAG[thread.NSURF] = false;

		//////////////////////////////////////////////Read molecule crtesian atom positions//////////////////////////////////////////////////////
		if (thread.NSURF >= MAXATOMS){
			wsprintf(ErrorMsg, L"ERROR, maximum number of surface model atoms exceeded");
			return false;
		}

		thread.NSURF++;
		variable.NSURFTOT = thread.NSURF;
	}
	return true;
}


bool FindAtomTypes(char *AtomSymbol)
{
	/* Find the different element types and store them in ELEMENT */
	bool newtype = TRUE;
	for (int i = 0; i < thread.NTYPES; ++i){
		if (AtomSymbol[0] == variable.ELEMENT[i][0] && AtomSymbol[1] == variable.ELEMENT[i][1]) newtype = FALSE;
	}
	if (newtype){
		if (thread.NTYPES == MAXTYPES){
			wsprintf(ErrorMsg, L"ERROR, too many atom types in unit cell");
			return false;
		}

		CopyMemory(variable.ELEMENT[thread.NTYPES], AtomSymbol, sizeof(char)*3);
		/* Store the coefficients for the computation of the atomic scattering
		factors for all the different elements in F_COEFF */
		get_coeff(variable.ELEMENT[thread.NTYPES], thread.F_COEFF[thread.NTYPES]);
		thread.NTYPES++;
	}

	return true;
}



//--------------------------------------------------------------------------------------------------------------------------------
bool Read_Par(char *line, int line_number){
//--------------------------------------------------------------------------------------------------------------------------------
	/*
	This will be changed in future to better fullfit the ROD standards regarding stuctural menu
	*/
	if (line_number >= MAXPAR + 2)
		return false;

bool Result=true;

	if(line_number>=2) 
		Result=FindTheValues(line);

return Result;
}

//--------------------------------------------------------------------------------------------------------------------------------
bool Read_Macro(char *line, int line_number, unsigned int* PositionInFile){
	//--------------------------------------------------------------------------------------------------------------------------------

	char Symbol1[60];
	char Symbol2[60];
	char Symbol3[1000];
	static int menu = 0;
	static int loop = 1;
	static int IterationNr = 0;
	static int bad_lines = 0;
	static int LoopBegining;
	if (line_number == 0) return true;//The first line is not interesting

	sscanf_s((char*)line, "%s%s%s", Symbol1, (unsigned int)sizeof(Symbol1), Symbol2, (unsigned int)sizeof(Symbol2), Symbol3, (unsigned int)sizeof(Symbol3));

	if (CompareStr(Symbol1, "set")) //Go to the set menu
		menu = 1;
	if (CompareStr(Symbol1, "re")) //go to the main menu
		menu = 0;
	if (CompareStr(Symbol1, "read")) //go to read menu
		menu = 2;
	if (CompareStr(Symbol1, "fit")) //go to read menu
		menu = 3;



	switch (menu){
	case 0:
		break;
		//set
	case 1:
		if (!FindTheValues(line)) bad_lines++;
		break;

	case 2:
		if (CompareStr(Symbol2, "bul")){ //go to the main menu
			if (!ReadFilesFromMacro(1, Symbol3))return false;
			return true;
		}
		if (CompareStr(Symbol2, "sur")){ //go to sur
			ReadFilesFromMacro(2, Symbol3);
			return true;
		}
		if (CompareStr(Symbol2, "dat")){ //go to dat
			ReadFilesFromMacro(3, Symbol3);
			//calc.MemoryAllocateCalc(thread.NDAT);
			return true;
		}
		if (CompareStr(Symbol2, "fit")){ //go to fit
			ReadFilesFromMacro(4, Symbol3);
			return true;
		}
		if (CompareStr(Symbol2, "par")){ //go to par
			ReadFilesFromMacro(5, Symbol3);
			return true;
		}
		if (CompareStr(Symbol2, "fat")){ //go to fat
			ReadFilesFromMacro(7, Symbol3);
			return true;
		}
		if (CompareStr(Symbol2, "mft")){ //go to mft
			myImport.Reset();
			variable.MODE = 1;
			ReadFilesFromMacro(8, Symbol3);
			myImport.TotalMolecules++;
			myImport.SetMolecule();
			return true;
		}
		break;

	case 3:
		update_model();
		if (CompareStr(Symbol2, "loop")){
			sscanf_s((char*)line, "%s%s%d", &Symbol1, (unsigned int)sizeof(Symbol1), &Symbol2, (unsigned int)sizeof(Symbol2), &loop);
			LoopBegining = *PositionInFile;//It is position of the beginning of the next line in macro file
			IterationNr = 0;
			return true;
		}

		if (CompareStr(Symbol2, "end_loop")){
			sscanf_s((char*)line, "%s%s", &Symbol1, (unsigned int)sizeof(Symbol1), &Symbol2, (unsigned int)sizeof(Symbol2));
			if (IterationNr < loop - 1){
				IterationNr++;
				*PositionInFile = LoopBegining;
			}
			return true;
		}


		if (CompareStr(Symbol2, "call")){
			SetCall();
			return true;
		}

		if (CompareStr(Symbol2, "ru")){
			SetCall();
			fit(LSQ_MRQ, NULL, false);
			return true;
		}

		if (CompareStr(Symbol2, "asa")){
			fit(LSQ_ASA, NULL, false);
			return true;
		}
		break;
	}


	if (bad_lines > 20){
		MessageBox(NULL, ErrorMsg, L"File has more than 20 empty lines.", MB_ICONERROR);
		return false; //If 20 lines is empty than return error
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
bool Read_ScatteringFactors(char *line, int line_number, unsigned int* PositionInFile){
	//--------------------------------------------------------------------------------------------------------------------------------
	int nr;
	float f[9];

	if (line_number == 0) return true;//The first line is not interesting
	if (line_number == MAX_ELEMENTS - 1) return false;//The first line is not interesting
	if (9 > sscanf_s((char*)line, "%d%f%f%f%f%f%f%f%f%f", &nr, &f[0], &f[1], &f[2], &f[3], &f[4], &f[5], &f[6], &f[7], &f[8])){
		swprintf(ErrorMsg, 100, L"Error in *.fat file in line %d", line_number);
		MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
		return false;
	}

	for (int i = 0; i < 9; ++i)
		if (f[i] != 0)
			co[line_number - 1][i] = (double)f[i];

	return true;
}


//----------------------------------------------------------------------------------------------------------------------------
bool ReadFilesFromMacro(int FileType, char* FileName){
	HANDLE			hFile;
	WCHAR WFileName[MAX_PATH] = TEXT("\0");
	DWORD dwSave;
	//LPSTR szTemporary;


	AtoT(WFileName, FileName);

	hFile = CreateFile(WFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		MessageBox(NULL, TEXT("I can't open the file!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		return false;
	}


	DWORD dwsize = GetFileSize(hFile, 0);
	//szTemporary = (LPSTR)HeapAlloc(GetProcessHeap(), 0, (dwsize + 1) * sizeof(CHAR));       // bufor uzywany do zapisu strimu 
	char* szTemporary = new char[dwsize+1];

	if (!ReadFile(hFile, szTemporary, dwsize, &dwSave, NULL)){
		MessageBox(NULL, TEXT("Read file failed!"), NULL, MB_ICONERROR);
		CloseHandle(hFile);
		delete[] szTemporary;
		return false;
	}

	if (FileType < 6 || FileType == 8)
		if (!ReadDataFromBuffer(szTemporary, FileType, dwsize))
		{
		//MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
		}


	//Read scattering factors file
	if (FileType == 7)
		ReadScatteringFactorsFromBuffer(szTemporary, dwsize);

	//HeapFree(GetProcessHeap(), 0, szTemporary);
	CloseHandle(hFile);
	delete[] szTemporary;
	return true;
}





//--------------------------------------------------------------------------------------------------------------------------------
void    calc_rlat(double dlat[6], double rlat[6]){
	//--------------------------------------------------------------------------------------------------------------------------------
	/*
		Oryginally writen by Prof. Elias Vlieg
		Compute reciprocal lattice parameters. The convention used is that
		a[i]*b[j] = d[ij], i.e. no 2PI's in reciprocal lattice.
		*/

	double  volume;

	/* compute volume of real lattice cell */

	volume = dlat[0] * dlat[1] * dlat[2] *
		sqrt(1 + 2 * cos(dlat[3])*cos(dlat[4])*cos(dlat[5])
		- cos(dlat[3])*cos(dlat[3])
		- cos(dlat[4])*cos(dlat[4])
		- cos(dlat[5])*cos(dlat[5]));

	/* compute reciprocal lattice parameters */

	rlat[0] = dlat[1] * dlat[2] * sin(dlat[3]) / volume;
	rlat[1] = dlat[0] * dlat[2] * sin(dlat[4]) / volume;
	rlat[2] = dlat[0] * dlat[1] * sin(dlat[5]) / volume;
	rlat[3] = acos((cos(dlat[4])*cos(dlat[5]) - cos(dlat[3]))
		/ (sin(dlat[4])*sin(dlat[5])));
	rlat[4] = acos((cos(dlat[3])*cos(dlat[5]) - cos(dlat[4]))
		/ (sin(dlat[3])*sin(dlat[5])));
	rlat[5] = acos((cos(dlat[3])*cos(dlat[4]) - cos(dlat[5]))
		/ (sin(dlat[3])*sin(dlat[4])));

}



////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function converts char characters to WCHAR
//
////////////////////////////////////////////////////////////////////////////////////////////////
void AtoT(WCHAR *lptsz, LPSTR lpsz)
{

	int len = (int) strlen(lpsz);

	// Converts the path to wide characters
	int needed = MultiByteToWideChar(0, 0, lpsz, len + 1, lptsz, len + 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function converts WCHAR to char
//
////////////////////////////////////////////////////////////////////////////////////////////////
void TtoA(LPSTR lpsz, LPTSTR lptsz)
{
	int len = (int)wcslen(lptsz);

	// Converts the path to wide characters
	int needed = WideCharToMultiByte(CP_UTF8, 0, lptsz, len + 1, lpsz, len + 1, NULL, NULL);

}


void CalculateAcceleratores()
{

	//Accelerators needed for calc rutines
	thread.Rlat2cosRlat4 = thread.RLAT[2] * cos(thread.RLAT[4]);
	thread.Rlat1cosRlat5 = thread.RLAT[1] * cos(thread.RLAT[5]);
	thread.Rlat1sinRlat5 = thread.RLAT[1] * sin(thread.RLAT[5]);
	thread.Rlat2sinRlat4cosDlat3 = thread.RLAT[2] * sin(thread.RLAT[4])*cos(thread.DLAT[3]);

	variable.a_sin_alp= thread.DLAT[0] * sin(thread.DLAT[3]);
	variable.b_cos_gam= thread.DLAT[1] * cos(thread.DLAT[5]);
	variable.c_cos_bet__sin_alp = thread.DLAT[2] * cos(thread.DLAT[4])* sin(thread.DLAT[3]);

	variable.b_sin_Gam= thread.DLAT[1] * sin(thread.DLAT[5]);
	variable.c_cos_alp = thread.DLAT[2] * cos(thread.DLAT[3]);
	variable.c_sin_bet__sin_alp = thread.DLAT[2] * sin(thread.DLAT[4]) * sin(thread.DLAT[3]);


}

//UnitCellFrame[i].position.X = cx * a * Cell.Sin_alp + cy * b * Cell.Cos_gam + cz * c * Cell.Cos_bet * Cell.Sin_alp;
//UnitCellFrame[i].position.Y = cy * b * Cell.Sin_gam + cz * c * Cell.Cos_alp;
//UnitCellFrame[i].position.Z = cz * c * Cell.Sin_bet * Cell.Sin_alp;