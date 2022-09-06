//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header for set.cpp. Set is important for data heandling. Copy data from strings to proper variables.

// Writen by Daniel Kaminski 
//
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#include "definitions.h"

void	set_parameters(int Interpret, char *Line);
void    set_parametersWinRod(int Interpret, char *Line);//Set specyfic WinRod parameters related only to WinRod

bool	TestYesNo(char *YesNoStr);
bool	CompareStr(char *Comm, char *String);
bool	CompareStr(WCHAR *Comm, WCHAR *String);
bool	FindTheValues(char *String);

void	Reset_Rod(void);
void	ResetErrors();

void    set_symmetry(HWND hwnd, int nplanegroup);
void	SetCall();

void	decrease_bragg_weight(double L_INTERVAL, double ERROR_FRACTION);
void     ReduceData();
//Symmetry table
const char symmetry[18][5]=
{
"None",
"p1",
"p2",
"pm",
"pg",
"cm",			
"p2mm",
"p2mg",
"p2gg",
"c2mm",
"p4",
"p4mm",
"p4gm",			
"p3",
"p3m1",
"p31m",
"p6",			
"p6mm",
};

