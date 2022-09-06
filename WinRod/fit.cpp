//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: The fit.cpp file contains all functions necessary for model fitting.
// Fit is responsible for all optymalisations used in WinRod, it is a bonding place between WinRod and (Levenberg and ASA rutine).
// Fit contains all the necessary thread functions use by ASA
//                                                  WinRod
//                                                    |
//                                                    |_
//														Fit
//	 												   _/\_
//									struct thread     /	   \
//											   \	 /	    \
//                                              \  ASA       LEVENBERG
//                                               \ /
//                                             Threads
//
// Based on LSQ file which is a part of ROD writen by Prof. Elias Vlieg
// Created  by Daniel Kaminski based partly on  Prof. Elias Vlieg code.
//		-The function is naw in c++ standars
//		-Fit is the main plase for optymalisation not like in ROD LSQ according to this the whole phylosopy is changed
//		-All functions are change to follow the new phylosophy with fit as an only place for bonding main program with fitting rutines
//		-The ASA uses threeds to do all the heavy calculations for chi^2 calculations. The task is divided on threads, 
//		 every thread calculates part of chi^2 data points. This is very efective for more than one core processors.
//
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

#include <stdafx.h>
#include <math.h>
#include "variables.h"
#include "set.h"
#include "calc.h"
#include "fit.h"
#include "asa.h"	//function prototypes for asa, rnd and so on
#include "levenberg.h"
#include "keating.h"
#include "Import.h"
#include "Save.h"


extern Import myImport;


//Threads variable
static HANDLE Thread[MAXTHREADS];
DWORD threadID[MAXTHREADS];
HANDLE StartEgzecution[MAXTHREADS];
HANDLE EndEgzecution[MAXTHREADS];
HANDLE EndThread[MAXTHREADS];
ThreadFunction ThreadProc[MAXTHREADS];


//--------------------------------------------------------------------------------------------------------------------------------
bool   fit(int FitMethod, HWND *hProgressBar, bool ShowResults){
//--------------------------------------------------------------------------------------------------------------------------------
/* Fit the experimental rod data.  */

int 	i,ndx, ndx_const;
int 	ntot;
double  *index = new double[thread.NDAT+1];
int     rc;
double  chisqr = 0.;
int 	derivatives = 0;
int		nfree = 0;
int     TimeStart, TimeEnd;

ndx = NSF  + variable.NOCCTOT2 + variable.NOCCTOT + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NSUBSCALETOT;
if (variable.NFACTOR > 0)
ndx += variable.NFACTOR;
if (variable.MODE == 1)
ndx += variable.ROTATIONTOTX + variable.ROTATIONTOTY + variable.ROTATIONTOTZ;
if (variable.DOMOCCUPAUTO)
ndx += thread.NDOMAIN;
if (thread.NSURF2 > 0)
ndx++;

ntot = ndx;

double	*penalties = new double[ndx+1];
int 	*freepar = new int[ndx + 1];

double   *FITPAR =	new double	[ndx + 1];		/* Values of fit parameters */
double   *FITERR =	new double	[ndx + 1];		/* Errors in fit parameters */
int      *FIXPAR =	new int		[ndx + 1];		/* Flag to denote a fixed parameter */
double   *FITMIN =	new double	[ndx + 1];		/* Lower bound on value of fit parameters */
double   *FITMAX =	new double	[ndx + 1];		/* Upper bound on value of fit parameters */
bool	 *FITPEN =	new bool	[ndx + 1];		/* Penalty factors for fit parameters */
double   **PTR_FITPAR = new double*[ndx + 1];	//New array of pointers for fitting parameters
double   **PTR_FITERR = new double*[ndx + 1];	//New array of pointers for fitting parameters

//Exit code
WCHAR ExitCode[EXIT_CODE_LENGHT];
WCHAR ErrorMsg[512];

	ResetErrors();

    /* Check whether data has been read in */
    if (thread.NDAT < 1){
		MessageBox(NULL, TEXT("ERROR, no data read in!"), NULL, MB_ICONERROR); 
		return 0;
	}

    /* Check whether model has been read in */
    if (thread.NSURF+thread.NBULK < 1){
		MessageBox(NULL, TEXT("ERROR, no model read in!"), NULL, MB_ICONERROR); 
		return 0;
	}

    /* Check whether there are not too many fit parameters */
    if (NSF+variable.NDISTOT+thread.NDWTOT+thread.NDWTOT2+variable.NOCCTOT + variable.NOCCTOT2+variable.NFACTOR> MAXFIT){
		MessageBox(NULL, TEXT("ERROR, number of fitting parameters exceeds MAXFIT!"), NULL, MB_ICONERROR); 	
		return 0;
	}


    /* Copy all fitting parameters to one array FITPAR and get starting  values */
	PTR_FITPAR[0] = &thread.SCALE;
	PTR_FITERR[0] = &thread.SCALE_ERR;
    FITPAR[0] = thread.SCALE;
    FITMIN[0] = variable.SCALELIM[0];
    FITMAX[0] = variable.SCALELIM[1];
    FITPEN[0] = variable.SCALEPEN;

	PTR_FITPAR[1] = &thread.BETA;
	PTR_FITERR[1] = &thread.BETA_ERR;
    FITPAR[1] = thread.BETA;
    FITMIN[1] = variable.BETALIM[0];
    FITMAX[1] = variable.BETALIM[1];
    FITPEN[1] = variable.BETAPEN;


	PTR_FITPAR[2] = &thread.SURFFRAC;
	PTR_FITERR[2] = &thread.SURFFRAC_ERR;
    FITPAR[2] = thread.SURFFRAC;
    FITMIN[2] = variable.SURFFRACLIM[0];
    FITMAX[2] = variable.SURFFRACLIM[1];
    FITPEN[2] = variable.SURFFRACPEN;

	ndx_const = NSF;


	if (thread.NSURF2 > 0 )
	{
		PTR_FITPAR[ndx_const] = &thread.SURF2FRAC;
		PTR_FITERR[ndx_const] = &thread.SURF2FRAC_ERR;
		FITPAR[ndx_const] = thread.SURF2FRAC;
		FITMIN[ndx_const] = variable.SURF2FRACLIM[0];
		FITMAX[ndx_const] = variable.SURF2FRACLIM[1];
		FITPEN[ndx_const] = variable.SURF2FRACPEN;
		ndx_const = NSF + 1;
	}

    for (i = 0; i < variable.NDISTOT; ++i){
		ndx = i+ ndx_const;

		PTR_FITPAR[ndx] = &variable.DISPL[i];
		PTR_FITERR[ndx] = &variable.DISPL_ERR[i];
		FITPAR[ndx] = variable.DISPL[i];
		FITMIN[ndx] = variable.DISPLLIM[i][0];
		FITMAX[ndx] = variable.DISPLLIM[i][1];
		FITPEN[ndx] = variable.DISPLPEN[i];
	}
	ndx_const += variable.NDISTOT;

    for (i = 0; i < thread.NDWTOT; ++i){
		ndx = i + ndx_const;

		PTR_FITPAR[ndx] = &thread.DEBWAL[i];
		PTR_FITERR[ndx] = &thread.DEBWAL_ERR[i];
		FITPAR[ndx] = thread.DEBWAL[i];
		FITMIN[ndx] = variable.DEBWALLIM[i][0];
		FITMAX[ndx] = variable.DEBWALLIM[i][1];
		FITPEN[ndx] = variable.DEBWALPEN[i];
	}
	ndx_const +=  thread.NDWTOT;

    for (i = 0; i < thread.NDWTOT2; ++i){
		ndx = i + ndx_const;

		PTR_FITPAR[ndx] = &thread.DEBWAL2[i];
		PTR_FITERR[ndx] = &thread.DEBWAL2_ERR[i];
		FITPAR[ndx] = thread.DEBWAL2[i];
		FITMIN[ndx] = variable.DEBWAL2LIM[i][0];
		FITMAX[ndx] = variable.DEBWAL2LIM[i][1];
		FITPEN[ndx] = variable.DEBWAL2PEN[i];
	}
	ndx_const += thread.NDWTOT2;

    for (i = 0; i < variable.NOCCTOT; ++i){
		ndx = i + ndx_const;

		PTR_FITPAR[ndx] = &thread.OCCUP[i];
		PTR_FITERR[ndx] = &thread.OCCUP_ERR[i];
		FITPAR[ndx] = thread.OCCUP[i];
		FITMIN[ndx] = variable.OCCUPLIM[i][0];
		FITMAX[ndx] = variable.OCCUPLIM[i][1];
		FITPEN[ndx] = variable.OCCUPPEN[i];
	}
	ndx_const += variable.NOCCTOT;

	for (i = 0; i < variable.NOCCTOT2; ++i) {
		ndx = i + ndx_const;

		PTR_FITPAR[ndx] = &thread.OCCUP2[i];
		PTR_FITERR[ndx] = &thread.OCCUP2_ERR[i];
		FITPAR[ndx] = thread.OCCUP2[i];
		FITMIN[ndx] = variable.OCCUP2LIM[i][0];
		FITMAX[ndx] = variable.OCCUP2LIM[i][1];
		FITPEN[ndx] = variable.OCCUP2PEN[i];
	}
	ndx_const += variable.NOCCTOT2;

	if (variable.NFACTOR > 0)
	{
		for (i = 0; i < variable.NFACTOR; ++i) {
			ndx = i + ndx_const;

			PTR_FITPAR[ndx] = &thread.FACTOR[i];
			PTR_FITERR[ndx] = &thread.FACTOR_ERR[i];
			FITPAR[ndx] = thread.FACTOR[i];
			FITMIN[ndx] = variable.FACTOR_LIM[i][0];
			FITMAX[ndx] = variable.FACTOR_LIM[i][1];
			FITPEN[ndx] = variable.FACTOR_PEN[i];
		}
		ndx_const += variable.NFACTOR;
	}
	for (i = 0; i < variable.NSUBSCALETOT; ++i){
		ndx = i + ndx_const;

		PTR_FITPAR[ndx] = &thread.SUBSCALE[i];
		PTR_FITERR[ndx] = &thread.SUBSCALE_ERR[i];
		FITPAR[ndx] = thread.SUBSCALE[i];
		FITMIN[ndx] = variable.SUBSCALELIM[i][0];
		FITMAX[ndx] = variable.SUBSCALELIM[i][1];
		FITPEN[ndx] = variable.SUBSCALEPEN[i];
	}
	ndx_const += variable.NSUBSCALETOT;	

	if (variable.MODE == 1)
	{
		for (i = 0; i < variable.ROTATIONTOTX; ++i) {
			ndx = i + ndx_const;

			PTR_FITPAR[ndx] = &variable.ROTATIONX[i];
			PTR_FITERR[ndx] = &variable.ROTATION_ERRX[i];
			FITPAR[ndx] = variable.ROTATIONX[i];
			FITMIN[ndx] = variable.ROTATIONLIMX[i][0];
			FITMAX[ndx] = variable.ROTATIONLIMX[i][1];
			FITPEN[ndx] = variable.ROTATIONPENX[i];
		}

		ndx_const += variable.ROTATIONTOTX;
		for (i = 0; i < variable.ROTATIONTOTY; ++i) {
			ndx = i + ndx_const;

			PTR_FITPAR[ndx] = &variable.ROTATIONY[i];
			PTR_FITERR[ndx] = &variable.ROTATION_ERRY[i];
			FITPAR[ndx] = variable.ROTATIONY[i];
			FITMIN[ndx] = variable.ROTATIONLIMY[i][0];
			FITMAX[ndx] = variable.ROTATIONLIMY[i][1];
			FITPEN[ndx] = variable.ROTATIONPENY[i];
		}

		ndx_const += variable.ROTATIONTOTY;
		for (i = 0; i < variable.ROTATIONTOTZ; ++i) {
			ndx = i + ndx_const;
			PTR_FITPAR[ndx] = &variable.ROTATIONZ[i];
			PTR_FITERR[ndx] = &variable.ROTATION_ERRZ[i];
			FITPAR[ndx] = variable.ROTATIONZ[i];
			FITMIN[ndx] = variable.ROTATIONLIMZ[i][0];
			FITMAX[ndx] = variable.ROTATIONLIMZ[i][1];
			FITPEN[ndx] = variable.ROTATIONPENZ[i];
		}
		ndx_const += variable.ROTATIONTOTZ;
	}



	//Domain occ.
	if (variable.DOMOCCUPAUTO)
	{
		for (i = 0; i < thread.NDOMAIN; ++i)
		{
			ndx = i + ndx_const;
			PTR_FITPAR[ndx] = &thread.DOMOCCUP[i];
			PTR_FITERR[ndx] = &variable.DOMOCCUP_ERR[i];
			FITPAR[ndx] = thread.DOMOCCUP[i];
			FITMIN[ndx] = variable.DOMOCCUPLIM[i][0];
			FITMAX[ndx] = variable.DOMOCCUPLIM[i][1];
			FITPEN[ndx] = variable.DOMOCCUPPEN[i];
		}
	}
	ndx_const += thread.NDOMAIN;

    /* Add fake data point when Keating energy is added in the least-squares minimization */
    if (ket.KEAT_PLUS_CHI)
	{
	if (thread.NDAT == MAXDATA){
		MessageBox(NULL, TEXT("ERROR, too many data points to add Keating!"), NULL, MB_ICONERROR); 
	    ket.KEAT_PLUS_CHI = false;
	    }
	else
	    {
	    thread.NDAT++;
	    thread.HDAT[thread.NDAT-1] = 100.;
	    thread.KDAT[thread.NDAT-1] = 0.;
	    thread.LDAT[thread.NDAT-1] = 0.1;
	    thread.FDAT[thread.NDAT-1] = 0.;
	    variable.ERRDAT[thread.NDAT-1] = 1.;
	    }
	}

    /* Compute weight of data points */
	ComputeWeights();

    /* Generate array with index numbers of (hkl)-points to be used as x-values of fit */
    for (i = 0; i < thread.NDAT; ++i)
		index[i] = (double) i;

    /* Calculate and store constant values, in order to speed up fitting */
	calc.f_calc_fit_init(&thread);	//Storage values for bulk speedup are stored in structure Rod (it is important for multithreading)

	//copy fixed parameters to the FIXPAR[MAXFIT] array
	for(i=0; i<ntot; ++i)
	{
		if(FITPEN[i]) FIXPAR[i] = 0;
		else FIXPAR[i] = 1;
	}

	//See how long it takes
	TimeStart = (int)GetTickCount();

	//chosse the fitting method
	switch (FitMethod)
	{
	case LSQ_ASA:
	{
		//Just in case
		if (thread.NumberOfCores < 1)
			thread.NumberOfCores = 1;

		//Asign function pointers to arrays of function pointers for simplicity
		ThreadProc[0] = ThreadProc0;
		ThreadProc[1] = ThreadProc1;
		ThreadProc[2] = ThreadProc2;
		ThreadProc[3] = ThreadProc3;
		ThreadProc[4] = ThreadProc4;
		ThreadProc[5] = ThreadProc5;
		ThreadProc[6] = ThreadProc6;
		ThreadProc[7] = ThreadProc7;


		//Divide the task between threeds
		thread.Task[0] = 0;
		thread.Task[thread.NumberOfCores] = thread.NDAT;
		for (i = 0; i < thread.NumberOfCores; ++i)
			thread.Task[i] = (int)floor((double)thread.NDAT / thread.NumberOfCores * i);

		// Create two event objects to  synchronise threads
		for (i = 0; i < thread.NumberOfCores; ++i)
		{
			StartEgzecution[i] = CreateEvent(
				NULL, // default security attributes
				FALSE, // auto-reset event object
				FALSE, // initial state is nonsignaled
				NULL); // unnamed object
			if (StartEgzecution[i] == NULL)
			{
				wprintf_s(ErrorMsg, EXIT_CODE_LENGHT, L"CreateEvent() #%d error: %d\n", i, GetLastError());
				MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
				ExitProcess(0);
			}

			EndEgzecution[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (EndEgzecution[i] == NULL)
			{
				wprintf_s(ErrorMsg, EXIT_CODE_LENGHT, L"CreateEvent() #%d error: %d\n", i, GetLastError());
				MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
				ExitProcess(0);
			}

			EndThread[i] = CreateEvent(	NULL, FALSE, FALSE, NULL);
			if (EndThread[i] == NULL)
			{
				wprintf_s(ErrorMsg, EXIT_CODE_LENGHT, L"CreateEvent(EndThread) #%d error: %d\n", i, GetLastError());
				MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
				ExitProcess(0);
			}	
		}

		//Start all threads in idle mode when flags are signaled the threads start to calculate
		for (i = 0; i < thread.NumberOfCores; ++i)
			StartThread(i, &thread);


		rc = asa(
			FITPAR,
			PTR_FITPAR,		//Array of poiters of fit par.	
			FITPEN,
			ntot,
			FITMIN,
			FITMAX,
			&chisqr,
			ffit_asa,
			ExitCode,
			hProgressBar);

		//The threads are in idle mode so stop them, they are not used after fitting
		StopThreads();

		if (rc == 0)
			goto fit_end;
	}
		break;

		// 	return 0; //Make the processor free to do another tasks. We will check the progres form the timer.
		//We would like to continue fitting with levenberg to obtain reliable errors. The errors should be calculated for the minimum.

	case LSQ_MRQ:
	{
		for (i = 0; i < ntot; ++i)
		{
			if (!FIXPAR[i]){
				freepar[nfree] = i + 1;
				nfree++;
			}
		}

		// Prepare penalty factors for lsqfit: 
		for (i = 0; i < ntot; i++)
			penalties[i] = (FITPEN[i] == TRUE ? LARGE_PENALTY : NO_PENALTY);

		rc = levenberg(index,				//x
			thread.FDAT,		//y
			thread.FWEIGHT,		//wgt
			thread.NDAT,		//ndata
			FITPAR,				//a
			PTR_FITPAR,			//ptr a
			ntot,				//ma
			freepar,			//lista
			nfree,				//mfit
			FITERR,				//asig
			FITMIN,				//amin
			FITMAX,				//amax
			penalties,			//apen
			&chisqr,			//&
			ffit,				//funcs
			derivatives,
			NULL,
			ExitCode);			//Exit message

		if (rc == 0)
			goto fit_end;
	}
		break;
	}

    /* Remove fake data point used for Keating energy */
    if (ket.KEAT_PLUS_CHI)
		thread.NDAT--;
    variable.NTH = thread.NDAT;

	//Time of calculation
	TimeEnd = (int) GetTickCount();
	int days, hours, min, sec, msec;
	msec = TimeEnd-TimeStart;
	ConvertMsecToData(&days, &hours, &min, &sec, &msec);

	//Normalize chsqrt to nr of all data points
	chisqr/=thread.NDAT; 



	if (ShowResults)
	{
		if (FitMethod == LSQ_ASA)
			swprintf_s(ErrorMsg, 2 * EXIT_CODE_LENGHT, L"Fitting finished after: %ddays %dhours, %dmin, %dsec, %dmsec\n\rVFSR exit code: %s\n\rChisqr = %4.3f normalised to %d reflections", //
			days, hours, min, sec, msec, ExitCode, chisqr, thread.NDAT);


		if (FitMethod == LSQ_MRQ)
			swprintf_s(ErrorMsg, 2 * EXIT_CODE_LENGHT, L"Fitting finished after: %ddays %dhours, %dmin, %dsec, %dmsec\n\rLevenberg exit code: %s\n\rChisqr = %4.3f normalised to %d reflections", //
			days, hours, min, sec, msec, ExitCode, chisqr, thread.NDAT);

		MessageBox(NULL, ErrorMsg, L"Fitting result", MB_OK);
	}
	else
		Sleep(100);

	//Copy fittng results to thread structure	
	CopyFittingResultsToThreadStructure(FITPAR, PTR_FITPAR, FITERR, PTR_FITERR, ntot);

	//To avoid problems with crazy values in errors when ASA
	if (FitMethod == LSQ_ASA)
		ResetErrors();

#ifdef FIT_TEST_PRINT
	if (variable.PrintTestFitFile)
		SaveFittingResults(ExitCode, TimeEnd, TimeStart, chisqr);
#endif

fit_end:

	delete[] penalties;
	delete[] freepar;
	delete[] index;

	delete[] FITPAR;
	delete[] FITERR;
	delete[] FIXPAR;
	delete[] FITMIN;
	delete[] FITMAX;
	delete[] FITPEN;

	delete[] PTR_FITPAR;
	delete[] PTR_FITERR;
	
	if (rc == 0)
		return false;
return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
bool ConvertMsecToData(int *Days, int *Hours, int *Min, int *Sec, int *Msec){
//--------------------------------------------------------------------------------------------------------------------------------
 int x;
 double time = *Msec;

	x = (int) time / 1000;
	*Sec =   x % 60;
	x /= 60;
	*Min =  x % 60;
	x /= 60;
	*Hours = x % 24;
	x /= 24;
	*Days = x; 
	*Msec = (int)time - (*Sec * 1000 + *Min * 60 * 1000 + *Hours *60 * 60 *1000 + *Days * 24 * 60 *60 *1000);

return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
void CopyFittingResultsToThreadStructure(double *a, double **ptr_a,  double *error, double **ptr_error, int na)
{

	//Added Function which copy parameters (a) from vfsr to poiners which are related to values in thread parameters. In this way it is a bit faster
	//Created by Daniel Kaminski
	//16.01.2015

	//Copy valuer
	for (int i = 0; i < na; ++i)
		*(double*)ptr_a[i] = a[i];

	//Copy errors
	for (int i = 0; i < na; ++i)
		*(double*)ptr_error[i] = error[i];

	/*
	int i, ndx;
    // Copy values in array 'FITPAR' to corresponding model parameters and update structure model 
    thread.SCALE = FITPAR[0];
	thread.SCALE_ERR = FITERR[0];
 

    thread.BETA = FITPAR[1];
    thread.BETA_ERR = FITPAR[1];

    thread.SURFFRAC = FITPAR[2];
	thread.SURFFRAC_ERR = FITERR[2];

    for (i = 0; i < variable.NDISTOT; ++i){
		ndx = i+NSF;
		variable.DISPL[i] = FITPAR[ndx];
		variable.DISPL_ERR[i] = FITERR[ndx];
	}
    for (i = 0; i < thread.NDWTOT; ++i){
		ndx = i+NSF+variable.NDISTOT;
		thread.DEBWAL[i] = FITPAR[ndx];
		thread.DEBWAL_ERR[i] = FITERR[ndx];
	}
    for (i = 0; i < thread.NDWTOT2; ++i){
		ndx = i+NSF+variable.NDISTOT+thread.NDWTOT;
		thread.DEBWAL2[i] = FITPAR[ndx];
		thread.DEBWAL2_ERR[i] = FITERR[ndx];
	}
    for (i = 0; i < variable.NOCCTOT; ++i){
		ndx = i+NSF+variable.NDISTOT+thread.NDWTOT+thread.NDWTOT2;
		thread.OCCUP[i] = FITPAR[ndx];
		thread.OCCUP_ERR[i] = FITERR[ndx];
	}
	for (i = 0; i < variable.NSUBSCALETOT; ++i){
		ndx = i + NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT;
		thread.SUBSCALE[i] = FITPAR[ndx];
		thread.SUBSCALE_ERR[i] = FITERR[ndx];
	}
	
	if (variable.MODE == 1)
	{
		for (i = 0; i < variable.ROTATIONTOTX; ++i){
			ndx = i + NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT + variable.NSUBSCALETOT;
			variable.ROTATIONX[i] = FITPAR[ndx];
			variable.ROTATION_ERRX[i] = FITERR[ndx];
		}

		for (i = 0; i < variable.ROTATIONTOTY; ++i){
			ndx = i + NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT + variable.NSUBSCALETOT + variable.ROTATIONTOTX;
			variable.ROTATIONY[i] = FITPAR[ndx];
			variable.ROTATION_ERRY[i] = FITERR[ndx];
		}

		for (i = 0; i < variable.ROTATIONTOTZ; ++i){
			ndx = i + NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT + variable.NSUBSCALETOT + variable.ROTATIONTOTX + variable.ROTATIONTOTY;
			variable.ROTATIONZ[i] = FITPAR[ndx];
			variable.ROTATION_ERRZ[i] = FITERR[ndx];
		}
	}
	if (variable.DOMOCCUPAUTO)
	{
		for (i = 0; i < thread.NDOMAIN-1; ++i){
			ndx = i + NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT + variable.NSUBSCALETOT + variable.ROTATIONTOTX + variable.ROTATIONTOTY + variable.ROTATIONTOTZ;
			thread.DOMOCCUP[i+1] = FITPAR[ndx];
			variable.DOMOCCUP_ERR[i+1] = FITERR[ndx];
		}
	}
	*/
}

//--------------------------------------------------------------------------------------------------------------------------------
//Orginally code created by Prof. Elias Vlieg in this version is after minor changes
//--------------------------------------------------------------------------------------------------------------------------------
double   calc_error(double x[], double y[], double wgt[], int ndata,
	double a[], double **ptr_a, int ma, int par,
	double amin[], double amax[], double apen[],
	double chi2_inc, FitFunction funcs, HWND hwnd)
	//--------------------------------------------------------------------------------------------------------------------------------

	/*
	Oryginally created by Prof. Elias Vlieg

	Compute the error of the fitparameter a[par].
	Do this by finding the value of the parameter
	for which chi^2 increases by chi2_inc.

	This function should only be called after optimizing chi2
	for the parameter of which the error is to be computed !

	*/

{

	double chi0, chi1, p0, pinc, pinc0, root, sigma;
	double fl = 0, fh = 0, pl = 0, ph = 0, del = 0, f = 0;
	int   i;

	/* get starting value of chi2 */

	chi1 = chi0 = calc_chisq(x, y, wgt, ndata, a, ptr_a, ma, funcs);


	p0 = a[par];

	if (apen[par] > 0. && amax[par] != amin[par])
	{
		pinc = pinc0 = fabs((amax[par] - amin[par]))*0.01;
	}
	else
	{
		pinc = pinc0 = 0.01;
	}


	/* find upper value of parameter */

	while (chi1 - chi0 - chi2_inc <= 0)
	{
		ph = p0 + pinc;
		pinc *= 1.6;
		a[par] = ph;
		chi1 = calc_chisq(x, y, wgt, ndata, a, ptr_a, ma, funcs);
	}

	fh = chi1 - chi0 - chi2_inc;
	pinc = pinc0;
	chi1 = chi0;
	a[par] = p0;

	pl = 0;
	while (chi1 - chi0 - chi2_inc <= 0)
	{
		pl = p0 - pinc;
		pinc *= 1.6;
		a[par] = pl;
		chi1 = calc_chisq(x, y, wgt, ndata, a, ptr_a, ma, funcs);
	}

	if (fabs(p0 - pl) > fabs(p0 - ph))
	{
		ph = pl;
		pl = p0;
		fh = chi1 - chi0 - chi2_inc;
	}
	else
	{
		pl = p0;
	}

	fl = -chi2_inc;


	pinc = ph - pl;

	for (i = 1; i <= 200; i++) {
		root = pl + pinc*fl / (fl - fh);
		a[par] = root;
		f = calc_chisq(x, y, wgt, ndata, a, ptr_a, ma, funcs) - chi0 - chi2_inc;
		if (f < 0.0) {
			del = pl - root;
			pl = root;
			fl = f;
		}
		else {
			del = ph - root;
			ph = root;
			fh = f;
		}
		pinc = ph - pl;
		a[par] = p0;
		sigma = fabs(root - p0);
		if (fabs(del) < 0.001*sigma || f == 0.0) {
			/*		printf("CALC_ERROR: Converged after %d iterations.\n", i); */
			return (sigma);
		}
	}
	MessageBox(hwnd, TEXT("ERROR, Maximum number of iterations exceeded in CALC_ERROR!"), NULL, MB_ICONERROR);
	return (-1.);

}


    /*
	Oryginally created by Prof. Elias Vlieg

    Function called by fitting routines to compute y-values and, optionally,
    derivatives with respect to fitting parameters.
    */
//--------------------------------------------------------------------------------------------------------------------------------
void    ffit(double x, double a[], double **ptr_a, double *y, int na, int derivatives)
{
	//Added Function which copy parameters (a) from vfsr to poiners which are related to values in thread parameters. In this way it is a bit faster
	//Created by Daniel Kaminski
	//07.07.2014
	for (int i = 0; i < na; ++i)
		*(double*)ptr_a[i] = a[i];

	/* Calculate structure model with new parameters */
	update_model();

	/* Calculate theoretical structure factor */
	*y = calc.f_calc_fit(&thread, (int)x);  /* (int) x;  this is the index of the current data point */
	
	/* Try to add lattice energy */
	if (thread.HDAT[(int)x] > 99.)
		*y += sqrt(energy_calc(&thread));

	na = na; derivatives = derivatives; /* prevent compiler warning */
}

//--------------------------------------------------------------------------------------------------------------------------------
//Orginally code created by Prof. Elias Vlieg in this version it is after minor changes
//--------------------------------------------------------------------------------------------------------------------------------
void update_model(void)
/*
Update the parameters of the model
*/
{
	int i;

	//Rotate molecule
	if (variable.MODE == 1)
		myImport.Rotate();

	//Occ. fitting
	/*
	if (variable.DOMOCCUPAUTO)
	{
		thread.DOMOCCUP[0] = 1;
		for (i = 1; i < thread.NDOMAIN; ++i)
		{
			thread.DOMOCCUP[0] -= thread.DOMOCCUP[i];
			if (thread.DOMOCCUP[0] < 0)
				thread.DOMOCCUP[0] = 0;
		}
	}
	*/

	for (i = 0; i < thread.NSURF + thread.NSURF2; ++i)
	{
		thread.XSFIT[i] = variable.XS[i];
		if (variable.NXDIS[i] > 0) thread.XSFIT[i] += variable.XCONST[i] * variable.DISPL[variable.NXDIS[i] - 1];
		if (variable.NX2DIS[i] > 0) thread.XSFIT[i] += variable.X2CONST[i] * variable.DISPL[variable.NX2DIS[i] - 1];

		thread.YSFIT[i] = variable.YS[i];
		if (variable.NYDIS[i] > 0) thread.YSFIT[i] += variable.YCONST[i] * variable.DISPL[variable.NYDIS[i] - 1];
		if (variable.NY2DIS[i] > 0) thread.YSFIT[i] += variable.Y2CONST[i] * variable.DISPL[variable.NY2DIS[i] - 1];

		thread.ZSFIT[i] = variable.ZS[i];
		if (variable.NZDIS[i] > 0) thread.ZSFIT[i] += variable.ZCONST[i] * variable.DISPL[variable.NZDIS[i] - 1];
		if (variable.NZ2DIS[i] > 0) thread.ZSFIT[i] += variable.Z2CONST[i] * variable.DISPL[variable.NZ2DIS[i] - 1];
	}

	/* Set occupancy for all normal surface atoms */
	for (i = 0; i < thread.NSURF + thread.NSURF2; ++i)
	{

		if (thread.NOCCUP[i] == 0)
			thread.OCCFIT[i] = 1.;
		else
			//new implementation of occ and 1-occ
			if (!thread.OCCUP_FLAG[i])
				thread.OCCFIT[i] = thread.OCCUP[thread.NOCCUP[i] - 1];
			else
				thread.OCCFIT[i] = 1. - thread.OCCUP[thread.NOCCUP[i] - 1];

		if (variable.NFACTOR > 0)
			thread.OCCFIT[i] *= SizeDistribution(thread.NOCCUP2[i] - 1);
		else
		{
			if (thread.NOCCUP2[i] > 0)
			{
				thread.OCCFIT[i] = thread.OCCUP2[thread.NOCCUP2[i] - 1];
			}
		}
		 

	}

#ifdef PROFILE

	if (thread.LIQUID_PROFILE)
	{
		thread.RealNumberLiquidLayers = 0;
		//extend model with liquid top layers according to specific occupancy profile 
		for (i = 0; i < MAXLIUID; ++i)
		{
			thread.LIQUID_ZSFIT[i] = thread.ZSFIT[0] + variable.DISPL[variable.NXDIS[0] - 1] * (i + 1);
			thread.LIQUID_OCCFIT[i] = calc.occupancy_profile(thread.LIQUID_ZSFIT[i] - thread.ZSFIT[0], variable.DISPL[variable.NX2DIS[0] - 1],
				                      variable.DISPL[variable.NYDIS[0] - 1], thread.OCCUP[thread.NOCCUP[0] - 1]);
			if (thread.LIQUID_OCCFIT[i] < 0.005)
				break;

			thread.RealNumberLiquidLayers++;
		}
	}

#endif PROFILE

}

//--------------------------------------------------------------------------------------------------------------------------------
float SizeDistribution(int i)
//--------------------------------------------------------------------------------------------------------------------------------
{
	static float b = 0;
	static int ii = -10000;
	static  float norm;
	if (i == ii) return b;
	ii = i;
	double c = thread.FACTOR[1] - i;
	if (c < 0) c = 0;
	double a = pow(c, thread.FACTOR[2]);
	b = 1.f - (float)exp(- a*thread.FACTOR[0]);
	if (i == 1)
		norm = b;
	b /= norm; //normalisation to 1
	return b;
}


//--------------------------------------------------------------------------------------------------------------------------------
double    ffit_asa(double *a, double **ptr_a, int *max_par, int *cost_flag){//
//--------------------------------------------------------------------------------------------------------------------------------

/*  
	Oryginally created by Daniel Kaminski in 2003 than modified by Prof. Elias Vlieg 2003-2004
	Function called by adaptive simulated annealing fitting routine.
	
    a[] array with parameter values
	ptr_a array of pointers of parameters (a)
    *cost_flag some sort of flag
    function returns chi^2 value

	//10.07.2014 mdified by Daniel Kaminski
*/

int i;
double	chisqr;

	//Added Function which copy parameters (a) from vfsr to poiners which are related to values in thread parameters. In this way it is a bit faster
	//Created by Daniel Kaminski
	//07.07.2014
	for (i = 0; i < *max_par; ++i)
		*(double*)ptr_a[i] = a[i];

    /* Calculate structure model with new parameters */
    update_model(); 

	//Start stop threads
	for (i = 0; i <thread.NumberOfCores; ++i)
		SetEvent(StartEgzecution[i]);

	WaitForMultipleObjects(thread.NumberOfCores, EndEgzecution, TRUE, INFINITE); //ghEvents 

	for (i = 0; i < thread.NumberOfCores; ++i)
	{
		ResetEvent(StartEgzecution[i]);
		ResetEvent(EndEgzecution[i]);
	}
		
	chisqr = 0.;
	for (i = 0; i < thread.NumberOfCores; ++i)
		chisqr += thread.ChiThreadResult[i];

    /* Add lattice energy to chi^2 if requested (easy in asa!) */
    if (ket.KEAT_PLUS_CHI)
	chisqr += sqrt(energy_calc(&thread));

    *cost_flag = TRUE;
    return(chisqr);
    }


//The hottest point in the code
 double Chi_sqr(Thread_t* data, int i_start, int i_end)
 {

	double chisqr = 0.;
	double y;
    for (int i = i_start; i < i_end; ++i)
	{
		y = calc.f_calc_fit(data, i) - data->FDAT[i];  //only the fast version here thx to Elias

		chisqr += y * y * data->FWEIGHT[i];
	}
 return (chisqr);
}



 void  ComputeWeights()
 {
	 /* Compute weight of data points */
	 for (int i = 0; i < thread.NDAT; ++i) {
		 if (variable.ERRDAT[i] > 1e-5)
			 thread.FWEIGHT[i] = 1. / (variable.ERRDAT[i] * variable.ERRDAT[i]);
		 else
			 thread.FWEIGHT[i] = 0.;  //Why 0, it make problem with lsqfit
	 }
 }


//--------------------------------------------------------------------------------------------------------------------------------
//Threads created by Daniel Kaminski
//notes:	Do not put anything before while loops in ThreadProc 1,2,3,4.... this can seriously slow down the machin. Started threads
//			are waiting for flags to start caclulations. It is faster than start/stop threads every time. The ASA rutine takes
//			a some computational time therefore the threads are not working with 100% of processor power. ASA is one processor
//			rutine at the moment and threads are waiting for it always.
//--------------------------------------------------------------------------------------------------------------------------------

 void StartThread(int ThreadNumber, Thread_t* lpThread)
 {
	 if (Thread[ThreadNumber] == NULL)
	 {
		 Thread[ThreadNumber] = CreateThread(NULL, 0, ThreadProc[ThreadNumber], lpThread, 0, &threadID[ThreadNumber]);
		 SetThreadPriority(Thread[ThreadNumber], thread.priority);
	 }
	 else
	 {
		 CloseHandle(Thread[ThreadNumber]);
		 Thread[ThreadNumber] = CreateThread(NULL, 0, ThreadProc[ThreadNumber], lpThread, 0, &threadID[ThreadNumber]);
		 SetThreadPriority(Thread[ThreadNumber], thread.priority);
	 }
 }


void StopThreads()
{
	//Forse to normal thread exit
	for (int i = 0; i <MAXTHREADS; ++i)
	{
		if (Thread[i] != NULL)
		{
			SetEvent(EndThread[i]);		
		}
	}
	//Wait a bit to end all threads
	Sleep(200);

	//Close handles and set them to 0
	for (int i = 0; i < MAXTHREADS; ++i)
	{
		if (Thread[i] != NULL)
		{
			CloseHandle(Thread[i]);
			Thread[i] = NULL;
		}
	}
}

/***************************************************
Starts thread proces #0 for Chisqr calculations
***************************************************/
void ThreadProcX(int ThreadNr, LPVOID lpThread_)
{
	Thread_t* lpParam = (Thread_t*)lpThread_;

	while (Thread[ThreadNr] != NULL)
	{
		//Safe stop thread
		if (WaitForSingleObject(EndThread[ThreadNr], 0) == WAIT_OBJECT_0)
			break;

		if (WaitForSingleObject(StartEgzecution[ThreadNr], INFINITE) == WAIT_OBJECT_0)
		{
			lpParam->ChiThreadResult[ThreadNr] = Chi_sqr(lpParam, lpParam->Task[ThreadNr], lpParam->Task[ThreadNr+1]);
			ResetEvent(StartEgzecution[ThreadNr]);
			SetEvent(EndEgzecution[ThreadNr]);
		}
	}
}

/***************************************************
Starts thread proces #0 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc0(LPVOID lpThread_)
{
	ThreadProcX(0, lpThread_);
	return 0;
}

/***************************************************
Starts thread process #1 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc1(LPVOID lpThread_)
{
	ThreadProcX(1, lpThread_);
	return 0;
}

/***************************************************
Starts thread process #2 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc2(LPVOID lpThread_)
{
	ThreadProcX(2, lpThread_);
	return 0;
}

/***************************************************
Starts thread process #3 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc3(LPVOID lpThread_)
{
	ThreadProcX(3, lpThread_);
	return 0;
}


/***************************************************
Starts thread process #4 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc4(LPVOID lpThread_)
{
	ThreadProcX(4, lpThread_);
	return 0;
}

/***************************************************
Starts thread process #5 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc5(LPVOID lpThread_)
{
	ThreadProcX(5, lpThread_);
	return 0;
}

/***************************************************
Starts thread process #6 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc6(LPVOID lpThread_)
{
	ThreadProcX(6, lpThread_);
	return 0;
}

/***************************************************
Starts thread process #7 for Chisqr calculations
***************************************************/
DWORD WINAPI ThreadProc7(LPVOID lpThread_)
{
	ThreadProcX(7, lpThread_);
	return 0;
}