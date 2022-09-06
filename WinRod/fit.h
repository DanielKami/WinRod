//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: The header file contains all functions prototypes defined in fit.cpp
// Fit is responsible for all optymalisations used in WinRod, it is a bonding place between WinRod and Levenberg and ASA rutine.
// Fit contains all the necessary thread functions use by ASA
//
// Oryginally part of ROD writen by Prof. Elias Vlieg
// Created by Daniel Kaminski 
// Asa contains parts of ROD program writen by Prof. Elias Vlieg  but these are here hardly rebuild.
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include"definitions.h"
#include "Thread.h"	//function prototypes used by WinRod

#define EXIT_CODE_LENGHT 256
#define LARGE_PENALTY 1e4	/* useful penalty factor */
#define NO_PENALTY    0

#define LSQ_ASA		0
#define LSQ_MRQ		1

typedef void(*FitFunction)(double, double *, double **, double *, int, int);
typedef double (*FitFunctionASA)(double *, double **, int *, int *);// 
typedef double (*RandomASA)(void);

double   ffit_asa(double *a, double **, int*, int *cost_flag);//
void    ffit(double x, double a[], double **ptr_a, double *y, int na, int derivatives);

double   calc_error( double *x, double *y, double *wgt, int ndata,
	double *a, double **ptr_a, int ma, int par,
					 double *amin, double *amax, double *apen,
					 double chi2_inc, FitFunction funcs, HWND hwnd);

bool	fit(int FitMethod, HWND *hProgressBar, bool ShowResults = true);
void	FinishAsa (HWND hwnd);

void CopyFittingResultsToThreadStructure(double *a, double **ptr_a, double *error, double **ptr_error, int na);
bool	ConvertMsecToData(int *Days, int *hours, int *min, int *sec, int *msec); //msec is In/ Out

void	update_model(void);

double  Chi_sqr(Thread_t* data, int i_start, int i_end);
float SizeDistribution(int i);

void  ComputeWeights();


typedef  DWORD (WINAPI *ThreadFunction)(LPVOID);
//Thread functions used by ASA
DWORD WINAPI ThreadProcX(LPVOID);
DWORD WINAPI ThreadProc0(LPVOID lpThread_);
DWORD WINAPI ThreadProc1(LPVOID lpThread_);
DWORD WINAPI ThreadProc2(LPVOID lpThread_);
DWORD WINAPI ThreadProc3(LPVOID lpThread_);
DWORD WINAPI ThreadProc4(LPVOID lpThread_);
DWORD WINAPI ThreadProc5(LPVOID lpThread_);
DWORD WINAPI ThreadProc6(LPVOID lpThread_);
DWORD WINAPI ThreadProc7(LPVOID lpThread_);


void StartThread(int ThreadNumber, Thread_t* lpThread);
void StopThreads();






