
#include <ddraw.h>
#include "structures.h"
bool	SaveMacroFile();					//Save WinRod macro file
char*	yesnostr(bool value);
int		Save3DFunction(HWND hwnd);			//Export 3D scene to one of the picture formats
int		SaveFileFunction(HWND hwnd);		//Save the WINROD files

bool	SaveParameterFile(char *FileName);
bool	SaveFitFile(char *FileName);
bool	SaveXYZFile(char *FileName);
bool	SaveBulFile(char *FileName);
bool	SaveDatFile(char *FileName);
bool	SaveMolecularFitFile(char *FileName);

//Save fitting results
void	SaveFittingResults(WCHAR *ExitCode, int TimeEnd, int TimeStart, double chisqr);

//Save bmp
void	CopySurface(HWND hwnd);
bool	SaveAtomProperitiesFile(ATOM_PROPERITIES AtProp );

//Print
int		Print3DFunction(HWND hwnd);
HDC		GetPrinterDC(HWND Hwnd);

