//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: levenberg.cpp optymallisation rutine.
//
// Writen oryginally by Prof. Elias Vlieg                                                                                                    
// Modified by Daniel Kaminski
//		-Added mesage controls for windows programs
//		-Change for from *.c to *.cpp add header <stdafx.h>
//		-Windows communication
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
//     include files                                                     
//--------------------------------------------------------------------------------------------------------------------------------
#include <stdafx.h>
#include "math.h"
#include "fit.h" 
#include "levenberg.h"
#include "variables.h"


int		MAXIT  =		200;	// max. number of iterations 
int		MAXCHI =        3;		// max. number of iterations while converging in chi-square (lsqfit() only) 
int     MAXLAMDA  =		10;		// max. number of iterations while converging or diverging in lamda with no change in chi-square (lsqfit() only)
int     DERIV;					// Flag to denote whether fitted function has analytical derivatives or not
double	*ASTEP;					// Stepsize in fitting parameters used in numerical evaluation of derivative
double	LSQ_DCHICONV =	0.01;	// norm. chi^2 convergence criterion




//--------------------------------------------------------------------------------------------------------------------------------
int levenberg(double *x, double *y, double *wgt, int ndata,
	double *a, double **ptr_a, int ma, int *lista, int mfit, double *asig,
	double *amin, double *amax, double *apen, double *chisqr,
	FitFunction funcs,
	int derivatives, HWND hwnd, WCHAR *Wstr){
	//--------------------------------------------------------------------------------------------------------------------------------

	/*
	Subroutine for Levenberg Marquardt fitting. Meaning of all variables:
	x[]             array with x-values of data
	y[]             array with y-values of data
	wgt[]           array with weight factors of data
	ndata           number of data points
	a[]             array with parameter values. The subroutine starts
	with the initial values and returns the optimal ones.
	ma              total number of parameters
	lista[]         array with serial numbers of free parameters = 1,2,3,...
	mfit            total number of free parameters
	asig[]          standard statistical error in optimum parameters
	amin[]          lower limit on allowed variable range
	amax[]          upper limit on allowed variable range
	apen[]		penalty coeff's for the fit parameters:
	penalties are introduced by simply multiplying
	chi-square with a factor which is given for
	example for parameter a being larger than amax:

	penalty = ((a-amax) * apen/(amax-amin))^2

	e.g. for apen = 10:

	chi-square doubles if a is off by 0.1*range
	chi-square quadruples if a is off by 0.2*range

	apen = 0   means no range checking
	apen = 10  means very soft range boundaries
	apen = 100 means modest sharp boundaries
	apen = 1e4 means sharp boundaries

	*chisqr         pointer to minimum chi-square value
	*(funcs)        pointer to function that calculates y and dy/da
	derivatives     flag denoting whether *(funcs) is able to provide
	analytical derivatives. If not, the subroutines
	will do (the much slower) numerical differentiation.

	The function *(funcs) has as arguments:
	double           x-value
	double *         array of parameter values
	double *         pointer to computed y-value
	double *         pointer to computed analytical derivatives dy/da
	int             total number of parameters
	int             flag denoting whether analytical derivatives should
	be computed.
	*/

	double   **covar, **alpha;
	double   alamda, oldchi, oldalamda, chi2_inc;
	double   *acoeff;
	int      i, chistop, iter, lamdastop = 0, res, nfree;

	/* enable crash protection */
	lsq_breakcontrol(LSQ_BREAK_INIT, funcs, a, ptr_a, ma, hwnd);

	/* Set global flag denoting whether function has analytical derivatives
	or not */
	DERIV = derivatives;

	covar = matrix(1, ma, 1, ma, hwnd);
	alpha = matrix(1, ma, 1, ma, hwnd);
	ASTEP = vector(1, mfit, hwnd);
	acoeff = vector(0, ma - 1, hwnd);

	nfree = ndata - mfit;
	if (nfree < 1) nfree = 1;

	/* Check the lower and upper bounds on variable ranges */
#define NDX lista[i]-1

	for (i = 0; i < mfit; i++)
	{
		if (amin[NDX] > amax[NDX])
		{
			apen[NDX] = 0;
			amin[NDX] = amax[NDX] = 0;
		}
		if ((amin[NDX] != amax[NDX]) &&
			((a[NDX] < amin[NDX]) || (a[NDX] > amax[NDX])))
			a[NDX] = 0.5*(amin[NDX] + amax[NDX]);
	}

	/* Initialize penalty coefficients */
	for (i = 0; i < mfit; i++)
		if (apen[NDX] && amax[NDX] > amin[NDX])
			acoeff[NDX] = apen[NDX] /
			(amax[NDX] - amin[NDX]);
		else
			acoeff[NDX] = 0;

//#undef NDX

	/* Initialize Marquardt algorithm */
	alamda = -1.;
	if (!mrqmin(x - 1, y - 1, wgt - 1, ndata, a - 1, ptr_a, ma, lista - 1, mfit, amin - 1, amax - 1,
		acoeff - 1, covar, alpha, chisqr, funcs, &alamda, hwnd)) return(0);


	/* Iterate to find minimum in chisqr */
	iter = 1;
	chistop = 0;
	if (mfit == 0)
		iter = -1; /* Don't iterate if no free parameters */

	while (!((chistop >= MAXCHI)
		&& (abs(lamdastop) >= MAXLAMDA)
		)
		&& (alamda < CRITLAMDA)
		&& (iter <= MAXIT)
		&& (iter != -1)
		)
	{
#ifdef DEBUG
		printf("-------------------------------------------------------------------\n");
		printf("ITERATION %d\n\n", iter);
#else
		//sprintf(string,"Iteration %2d\r",iter);
		//type_line(string);
#endif
		iter++;
		oldchi = *chisqr;
		oldalamda = alamda;
		if (!mrqmin(x - 1, y - 1, wgt - 1, ndata, a - 1, ptr_a, ma, lista - 1, mfit, amin - 1, amax - 1,
			acoeff - 1, covar, alpha, chisqr, funcs, &alamda, hwnd)) return(0);
#ifdef DEBUG
		printf("alamda = %.1e\n",alamda);
#endif
		if (alamda < oldalamda)		/* are things getting better ? */
		{
			// update good parameters
			lsq_breakcontrol(LSQ_BREAK_UPDATE, NULL, a, ptr_a, 0, hwnd);

			if (lamdastop < 0)
			{
				lamdastop = 2;
			}
			else
			{
				lamdastop += 2;
			}
		}
		else                        /* No, this is not convergence! */
		{
			if (lamdastop > 0)
			{
				lamdastop = -1;
			}
			else
			{
				lamdastop -= 1;
			}
		}
#ifdef DEBUG
		printf("lamdastop = %+2d\n",lamdastop);
#endif
		if (fabs((oldchi - *chisqr) / nfree) < LSQ_DCHICONV)
		{
			chistop++;
		}
		else
		{
			chistop = 0;
		}

	} /* while loop */

	if (iter != -1)	/* no free parameters */
	{
		if (chistop >= MAXCHI)
		{
			if (lamdastop > 0)
				swprintf_s(Wstr, EXIT_CODE_LENGHT, L"Iteration stopped after converging.");
			else
				swprintf_s(Wstr, EXIT_CODE_LENGHT, L"Iteration stopped with diverging lambda.");
		}
		if (oldalamda >= CRITLAMDA*0.1)
		{
			swprintf_s(Wstr, EXIT_CODE_LENGHT, L"Iteration stopped with critical lambda.");
		}
		if (iter >= MAXIT)
		{
			swprintf_s(Wstr, EXIT_CODE_LENGHT, L"Too many iterations in Levenberg opt.");
		}
		/*		printf("Covariance matrix: \n\n");
				for (i=1; i<=ma; i++)
				{
				for (j=1; j<=i; j++)
				{
				printf("%10.2e  ",covar[i][j]);
				}
				printf("\n");
				}
				printf("\n");
				*/

	} /* iter != -1 */

	// update good parameters
	lsq_breakcontrol(LSQ_BREAK_UPDATE, NULL, a, ptr_a, 0, hwnd);

	/* Compute standard errors of fitted parameters */
	if (mfit)
	{
		/*
			chi2_inc = get_real(0., "Chi2 increase for error estimate (0 = use covariance matrix): [0.0] ");
			if (chi2_inc < 0.) chi2_inc = 0.;


			if (chi2_inc == 0.) {
			*/
		if (variable.LSQ_ERRORCALC == COVARIANCE)
		{
			alamda = 0.;
			res = mrqmin(x - 1, y - 1, wgt - 1, ndata, a - 1, ptr_a, ma, lista - 1, mfit, amin - 1, amax - 1,
				acoeff - 1, covar, alpha, chisqr, funcs, &alamda, hwnd);

			for (i = 0; i < ma; i++)
			{
				if (covar[i + 1][i + 1] < 0. || !res)
					asig[i] = -1.;	/* negative error indicates error */
				else
					asig[i] = sqrt(covar[i + 1][i + 1]);
			}
		}
		if (variable.LSQ_ERRORCALC == CHISQR)
		{
			//chi2_inc = get_real(1., "Chi2 increase for error estimate: [1.0] ");
			//MessageBox(hwnd, TEXT("Chi2 increase for error estimate: [1.0] "), NULL, MB_OK);
			chi2_inc = 1.; //todo insert more extended dialog box for taking value option
			if (chi2_inc < 0.01) chi2_inc = 1.;
			for (i = 0; i < ma; i++)
				asig[i] = -1.;


			for (i = 0; i < mfit; i++)
				asig[lista[i] - 1] = calc_error(x - 1, y - 1, wgt - 1, ndata,
				a - 1, ptr_a, ma, lista[i],
				amin - 1, amax - 1, apen - 1,
				chi2_inc,
				funcs, hwnd);
		}

	} /* if mfit */
	free_vector(ASTEP, 1, mfit);
	free_matrix(alpha, 1, ma, 1, ma);
	free_matrix(covar, 1, ma, 1, ma);
	free_vector(acoeff, 0, ma - 1);

	// restore saved parameters
	// this is necessary if anything went wrong during the error computation

	lsq_breakcontrol(LSQ_BREAK_RESTORE, NULL, a, ptr_a, 0, hwnd);

	return(1);

}

//--------------------------------------------------------------------------------------------------------------------------------
int     mrqcof(double x_[], double y_[], double wgt_[], int ndata_,
	double a_[], double **ptr_a, int ma_, int lista_[], int mfit_,
	    double amin_[], double amax_[], double apen_[],
	    double **alpha_, double beta_[], double *chisq_,
            FitFunction funcs_, HWND hwnd)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    Calculate first (beta) and second (alpha) partial derivatives of
    chi-square. Modified version of NR subroutine.
    */

    {
    int k,j,i;
    double ymod,wt,dy,*dyda, d;
    double ystep, penalty;

    dyda=vector(1,ma_, hwnd);
    for (j=1;j<=mfit_;j++)
	{
	for (k=1;k<=j;k++)
		alpha_[j][k]=0.0;
		beta_[j]=0.0;
	}
    *chisq_=0.0;
    for (i=1;i<=ndata_;i++)
	{
		(*funcs_)(x_[i], a_ + 1,ptr_a, &ymod,  ma_, DERIV);
	if (!DERIV)
	    {
	    for (j = 1; j <= mfit_; j++)
		{
		a_[lista_[j]] += ASTEP[j];
		(*funcs_)(x_[i], a_ + 1,ptr_a, &ystep,  ma_, DERIV);
		dyda[lista_[j]] = (ystep-ymod)/ASTEP[j];
		a_[lista_[j]] -= ASTEP[j];
		}
	    }
	dy=y_[i]-ymod;
	for (j=1;j<=mfit_;j++)
	    {
	    wt=dyda[lista_[j]]*wgt_[i];
	    for (k=1;k<=j;k++)
		alpha_[j][k] += wt*dyda[lista_[k]];
	    beta_[j] += dy*wt;
	    }
	(*chisq_) += dy*dy*wgt_[i];
	}
    for (j=2;j<=mfit_;j++)
	for (k=1;k<=j-1;k++) alpha_[k][j]=alpha_[j][k];

    /* Calculate penalty: */

    penalty = 1.0;
    for (j=1;j<=mfit_;j++)
      {
		d = amin_[lista_[j]] - a_[lista_[j]];
		if (d > 0)
		  penalty += PENALTY(d,apen_[lista_[j]]);
		d = a_[lista_[j]] - amax_[lista_[j]];
		if (d > 0)
		  penalty += PENALTY(d,apen_[lista_[j]]);
      }

	*chisq_ *= penalty;
#ifdef DEBUG
	printf("\nPar. Values = ");
	for (i=1;i<=ma;i++)
	  {
		printf("%10.4g  ",a[i]);
	  }
	printf("\nPenalty = %8.2g, ChiSqr = %8.2g",penalty,*chisq);
#endif
    free_vector(dyda,1,ma_);
    return(1);
    }

//--------------------------------------------------------------------------------------------------------------------------------
int     mrqmin(double x__[], double y__[], double wgt__[], int ndata__,
	double a__[], double **ptr_a, int ma__, int lista__[], int mfit__,
	    double amin__[], double amax__[], double apen__[], 
	    double **covar__, double **alpha__, double *chisq__,
            FitFunction funcs__,
	    double *alamda__, HWND hwnd)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    Modified version of NR subroutine.
    */

    {
    int k,kk,j,ihit,i;
    static double *da,*atry,**oneda,*beta,ochisq;

    if (*alamda__ < 0.0)
	{
	oneda=matrix(1,mfit__,1,1, hwnd);
	atry=vector(1,ma__, hwnd);
	da=vector(1,ma__, hwnd);
	beta=vector(1,ma__, hwnd);
	kk=mfit__+1;
	for (j=1;j<=ma__;j++)
	    {
	    ihit=0;
	    for (k=1;k<=mfit__;k++)
		if (lista__[k] == j) ihit++;
	    if (ihit == 0)
		lista__[kk++]=j;
	    else if (ihit > 1)
		{
			MessageBox(hwnd, TEXT("Bad LISTA permutation in MRQMIN-1"), NULL, MB_ICONERROR);
		return(0);
		}
	    }
	if (kk != ma__+1)
	    {
			MessageBox(hwnd, TEXT("Bad LISTA permutation in MRQMIN-2"), NULL, MB_ICONERROR);
	    return(0);
	    }
	if (!DERIV) /* estimate stepsize for numerical differentiation */
	    {
	    for (i = 1; i <= mfit__; i++)
		{
		ASTEP[i] = fabs(DELTA*a__[lista__[i]]);
		if (ASTEP[i] < 1e-20) ASTEP[i] = DELTA;
		}
	    }
	*alamda__=0.001;
	mrqcof(x__, y__, wgt__, ndata__, a__, ptr_a, ma__, lista__, mfit__,
	       amin__,amax__,apen__,alpha__,beta,chisq__,funcs__, hwnd);
	ochisq=(*chisq__);
	return(1);
	}

    for (j=1;j<=mfit__;j++)
	{
	for (k=1;k<=mfit__;k++)
		covar__[j][k]=alpha__[j][k];
		covar__[j][j]=alpha__[j][j]*(1.0+(*alamda__));
		oneda[j][1]=beta[j];
	}
    if (!gaussj(covar__,mfit__,oneda,1, hwnd)) return(0);
#ifdef DEBUG
    printf("da(GJ) = ");
#endif
    for (j=1;j<=mfit__;j++)
	  {
		da[j]=oneda[j][1];
#ifdef DEBUG
		printf("%10.4g  ",da[j]);
#endif
	  }
#ifdef DEBUG
	printf("\n");
#endif
    if (*alamda__ == 0.0)
	{
	covsrt(covar__,ma__,lista__,mfit__);
	free_vector(beta,1,ma__);
	free_vector(da,1,ma__);
	free_vector(atry,1,ma__);
	free_matrix(oneda,1,mfit__,1,1);
	return(1);
	}
    for (j=1;j<=ma__;j++) atry[j]=a__[j];
    for (j=1;j<=mfit__;j++)
	{
	atry[lista__[j]] = a__[lista__[j]]+da[j];
	}

    /* Use covariance matrix to get new estimate on the appropriate 
    stepsize for numerical evaluation of the derivatives */

    if (!DERIV)
	{
	for (i = 1; i <= mfit__; i++)
	    {
	    ASTEP[i] = sqrt(fabs(covar__[lista__[i]][lista__[i]]));
	    if ((ASTEP[i] < 1e-10) || (ASTEP[i] > 10*fabs(a__[lista__[i]])))
		ASTEP[i] = DELTA;
	    }
	}

	mrqcof(x__, y__, wgt__, ndata__, atry, ptr_a, ma__, lista__, mfit__,
	   amin__,amax__,apen__,covar__,da,chisq__,funcs__, hwnd);

    if (*chisq__ < ochisq)
	{
	*alamda__ *= 0.1;
	ochisq=(*chisq__);
	for (j=1;j<=mfit__;j++)
	    {
	    for (k=1;k<=mfit__;k++) alpha__[j][k]=covar__[j][k];
	    beta[j]=da[j];
	    a__[lista__[j]]=atry[lista__[j]];
	    }
	}
    else
	{
	*alamda__ *= 10.0;
	*chisq__=ochisq;
	}
#ifdef DEBUG
	printf("\nda(MRQ) = ");
	for (j=1;j<=mfit;j++)
	  {
		printf("%10.4g",da[j]);
	  }
	printf("\n");
#endif
    return(1);
    }



//--------------------------------------------------------------------------------------------------------------------------------
double   *vector(int nl, int nh, HWND hwnd)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    From NR.
    */

{

 double *v;


	int size;
	size=nh-nl>0 ? nh-nl+1 : 1;
	//v = new double[size];
	v=(double *)malloc((unsigned) (size)*sizeof(double));
	if (!v) MessageBox(hwnd, TEXT("Allocation failure in vector() v"), NULL, MB_ICONERROR);
	return v-nl;
}

//--------------------------------------------------------------------------------------------------------------------------------
int     *ivector(int nl, int nh, HWND hwnd)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    From NR.
    */

{
	int *v;

	int size;
	size=nh-nl>0 ? nh-nl+1 : 1;

	//v = new int[size];
	v=(int *)malloc((unsigned) (size+1)*sizeof(int));
	if (!v) MessageBox(hwnd, TEXT("Allocation failure in vector() v"), NULL, MB_ICONERROR);
	return v-nl;
}


//--------------------------------------------------------------------------------------------------------------------------------
double   **matrix(int nrl, int nrh, int ncl,int nch, HWND hwnd)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    From NR.
    */

{
 int i;

	int sizer, sizec;
	sizer=nrh-nrl>0 ? nrh-nrl+1 : 1;

 double **m = new double*[sizer+2];;

	//m=(double **) malloc((unsigned) (sizer)*sizeof(double*));
	if (!m) MessageBox(hwnd, TEXT("Allocation failure 1 in vector() m"), NULL, MB_ICONERROR);
	m -= nrl;

	sizec=nch-ncl>0 ? nch-ncl+1 : 1;

	for (i = nrl; i <= nrh; i++)
	{
		//m[i] = new double[sizec];
		m[i]=(double *) malloc((unsigned) (sizec)*sizeof(double));
		if (!m[i]) MessageBox(hwnd, TEXT("Allocation failure 2 in vector() m"), NULL, MB_ICONERROR);
		m[i] -= ncl;
	}
	return m;
}


//--------------------------------------------------------------------------------------------------------------------------------
void    free_vector(double *v, int nl, int nh)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    From NR.
    */

{
	nh*=1;     
	free((char*) (v+nl));
	//delete[] v;
}

//--------------------------------------------------------------------------------------------------------------------------------
void    free_ivector(int *v, int nl,int nh)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    From NR.
    */

{
	nh*=1;
	free((char*) (v+nl));
	//delete[] v;
}


//--------------------------------------------------------------------------------------------------------------------------------
void    free_matrix(double **m, int nrl, int nrh, int ncl, int nch)
//--------------------------------------------------------------------------------------------------------------------------------
{
	int i;

    nch*=1;

	for(i=nrh;i>=nrl;i--) 
		free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
	//delete[] m[i]

	//delete[] m;

}

//--------------------------------------------------------------------------------------------------------------------------------
int     gaussj(double **a, int n, double **b, int m, HWND hwnd)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    From NR.
    */

{
	int *indxc,*indxr,*ipiv;
	int i,icol=0,irow=0,j,k,l,ll;
	double big,dum,pivinv;

	indxc=ivector(1,n, hwnd);
	indxr=ivector(1,n, hwnd);
	ipiv=ivector(1,n, hwnd);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv[k] > 1)
						{
						MessageBox(hwnd, TEXT("GAUSSJ: Singular Matrix-1"), NULL, MB_ICONERROR);
						return(0);
						}
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=1;l<=n;++l) SWAP(a[irow][l],a[icol][l])
			for (l=1;l<=m;++l) SWAP(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] < 1e-10)
			{
			MessageBox(hwnd, TEXT("GAUSSJ: Singular Matrix-2"), NULL, MB_ICONERROR);
			return(0);
			}
		pivinv=1.0/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++)
			if (ll != icol) {
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}
	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP(a[k][indxr[l]],a[k][indxc[l]]);
	}
	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
	return(1);
}


//--------------------------------------------------------------------------------------------------------------------------------
int     covsrt(double **covar, int ma, int lista[], int mfit)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    From NR.
    */

{
	int i,j;
	double swap;

	for (j=1;j<ma;j++)
		for (i=j+1;i<=ma;i++) covar[i][j]=0.0;
	for (i=1;i<mfit;i++)
		for (j=i+1;j<=mfit;j++) {
			if (lista[j] > lista[i])
				covar[lista[j]][lista[i]]=covar[i][j];
			else
				covar[lista[i]][lista[j]]=covar[i][j];
		}
	swap=covar[1][1];
	for (j=1;j<=ma;j++) {
		covar[1][j]=covar[j][j];
		covar[j][j]=0.0;
	}
	covar[lista[1]][lista[1]]=swap;
	for (j=2;j<=mfit;j++) covar[lista[j]][lista[j]]=covar[1][j];
	for (j=2;j<=ma;j++)
		for (i=1;i<=j-1;i++) covar[i][j]=covar[j][i];
	return(1);
}


//--------------------------------------------------------------------------------------------------------------------------------
double calc_chisq(double *x, double *y, double *wgt, int ndata_,
	double *a, double **ptr_a, int ma, FitFunction funcs)
//--------------------------------------------------------------------------------------------------------------------------------

    /*
    Evaluate chi^2 for given data set x[],y[],wgt[] by comparison with
    model function funcs, with ma parameters in array a[].

    Note:

    During fitting, chi2 is not computed by this function but by
    mrqcof(), which simultaneously computes derivatives and penalties.
    Calc_chisq() is called by lsqmin() for the computation of errors of
    fit parameters.
    */

{
    int i;
    double chi2=0.;
    double ymod;

    for (i=1; i<=ndata_; i++)
	{

		(*funcs)(x[i], a + 1, ptr_a, &ymod, ma, 0);

//        if (LSQ_USELOG && ymod > 1e-20)  /* Ignore this log mode here */
//            ymod = log10(ymod);
		chi2 += (ymod-y[i])*(ymod-y[i])*wgt[i];
	}

    return chi2;
}


//--------------------------------------------------------------------------------------------------------------------------------
void lsq_breakcontrol(int command, FitFunction funcs, double *a, double **ptr_a, int na, HWND hwnd)
//--------------------------------------------------------------------------------------------------------------------------------

/*
crash protection: restore old parameters if crashed or Ctrl-Break pressed
*/
{
	static double *a_saved;
	static int na_saved;
	static int status;
	static FitFunction funcs_saved;
	int    i;
	double  y;

	switch (command)
	{
	case LSQ_BREAK_INIT:
		a_saved = vector(0, na - 1, hwnd);
		if (a_saved != NULL)
		{
			for (i = 0; i < na; ++i) a_saved[i] = a[i];
			status = LSQ_BREAK_FITTING;
			na_saved = na;
			funcs_saved = funcs;
		}
		break;

	case LSQ_BREAK_UPDATE:
		if (status == LSQ_BREAK_FITTING && a_saved != NULL)
			for (i = 0; i < na_saved; ++i) a_saved[i] = a[i];
		break;

	case LSQ_BREAK_RESTORE:
		if (status == LSQ_BREAK_FITTING && a_saved != NULL)
		{
			if (a != NULL) for (i = 0; i < na_saved; ++i) a[i] = a_saved[i];
			status = LSQ_BREAK_INACTIVE;
			funcs_saved(0., a_saved, ptr_a, &y, na_saved, 0);
			free_vector(a_saved, 0, na_saved - 1);
			a_saved = NULL;
		}
		break;
	}
}