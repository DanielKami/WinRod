//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header for ReadFile.cpp. ReadFile.cpp is responsible for data transfer read/write
// Writen by Daniel Kaminski 
//
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
//#include <atlbase.h>		 //to convert between char and WCHAR     
//#include <atlconv.h>		 //to convert between char and WCHAR
#include "structures.h"


bool ReadMacroInit();

int		OpenFileFunction(HWND hwnd);
bool	ReadDataFromBuffer(LPSTR, int, DWORD);
bool	ReadMacroFromBuffer(LPSTR TStr, DWORD size);
int		ReadFileFromPatch(WCHAR *szFile, WCHAR* Extension);
int		ApplyExtension(char *szTemporary, WCHAR *Extension, int dwsize);
bool	ReadFilesFromMacro(int FileType, char* FileName);
bool	ReadAtomInit(HWND hwnd, ATOM_PROPERITIES *AtProp);
bool	ReadScatteringFactorsFromBuffer(LPSTR TStr, DWORD size);
bool	Read_ScatteringFactors(char *line, int line_number, unsigned int* PositionInFile);
bool	FindAtomTypes(char *AtomSymbol);
WCHAR *FindLastDot(WCHAR *Input);

bool	Take_line(char *instring, DWORD InpStringSize, char *outstring, unsigned int OutStringSize, unsigned int *Position);
bool	Read_Bul(char*, int);
bool	Read_Sur(char*, int);
bool	Read_Dat(char*, int);
bool	Read_Fit(char*, int);
bool	Read_Par(char*, int);
bool	Read_Mft(char*, int);
bool	Read_Macro(char*, int, unsigned int*);

void	calc_rlat(double dlat[6], double rlat[6]);
void	CalculateAcceleratores();

#ifdef FIT_TEST_PRINT
void	OpenPrintFitFile();
void	ClosePrintFitFile();
#endif

//////////////////////////////////////////////
//convertion between char and WCHAR
void	AtoT(LPTSTR, LPSTR);
void	TtoA(LPSTR, LPTSTR);
