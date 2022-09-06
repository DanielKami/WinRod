//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of fit.cpp file containing all function prototypes used in fit.cpp
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------


#include "fit.h"

int asa(
			double*, //Array of fit parameters
			double**,//Aray of pointers of fit parameters to spead up.
			bool*,
			int,
			double*,
			double*,
			double*,
			FitFunctionASA,
			WCHAR*,
			HWND *hProgressBar);

double	myrand(void);
double	randflt(void);
void	initialize_rng(void);


#ifdef FITTHREAD
DWORD WINAPI ThreadProcAsa(LPVOID lpThread_);
void StartThreadAsa(FitThread_t* lpThread);
void StopThreadAsa();
#endif