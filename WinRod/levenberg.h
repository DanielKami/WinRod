//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of levenberg.cpp file containing all function prototypes used in it.
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#pragma once

#define LSQ_BREAK_INACTIVE		0
#define LSQ_BREAK_FITTING		1
#define LSQ_BREAK_INIT			1
#define LSQ_BREAK_UPDATE		2
#define LSQ_BREAK_RESTORE		3


#define	CRITLAMDA		1E30	// critical lamda value (lsqfit() only) 
#define DELTA			0.001	// Relative stepsize used in numerical

#define PENALTY(dx,sc) ((dx)*(dx)*(sc)*(sc))	/* the penalty function */
#define SWAP(a,b) {double temp=(a);(a)=(b);(b)=temp;}


int		levenberg(double*, double*, double*, int ndata, double*, double **ptr_a, int ma, int*, int mfit, double*,
	              double*, double*, double*, double*, FitFunction funcs, int derivatives, HWND hwnd, WCHAR *wchar);

int     mrqcof(double x[], double y[], double wgt[], int ndata, double a[], double **ptr_a, int ma, int lista[], int mfit,
	           double amin[], double amax[], double apen[], double **alpha, double beta[], double *chisq, FitFunction funcs, HWND hwnd);

int     mrqmin(double x[], double y[], double wgt[], int ndata, double a[], double **ptr_a, int ma, int lista[], int mfit,
	           double amin[], double amax[], double apen[], double **covar, double **alpha, double *chisq, FitFunction funcs, double *alamda, HWND hwnd);

double	*vector(int nl, int nh, HWND hwnd);
int     *ivector(int nl, int nh, HWND hwnd);
double  **matrix(int nrl, int nrh, int ncl,int nch, HWND hwnd);
void    free_vector(double *v, int nl, int nh);
void    free_ivector(int *v, int nl,int nh, HWND hwnd);
void    free_matrix(double **m, int nrl, int nrh, int ncl, int nch);
int     gaussj(double **a, int n, double **b, int m, HWND hwnd);
int     covsrt(double **covar, int ma, int lista[], int mfit);
double	calc_chisq(double*, double*, double*, int ndata, double*, double **ptr_a, int ma, FitFunction funcs);
void	lsq_breakcontrol(int command, FitFunction funcs, double *a, double **ptr_a, int na, HWND hwnd);