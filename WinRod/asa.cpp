//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: The fit.cpp file containing all function important for fitting this is different than in ROD program.
//			In WinRod fit bonds the WinRod with Levenberg and ASA rutines
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------



/***************************************************************************/
/*      include files                                                      */
/***************************************************************************/
#include <stdafx.h>
#include "time.h"
#include "math.h"
#include "asa.h"
#include "vfsr_funct.h"
#include "variables.h"


extern VFSR_par vfsr_par;

/***************************************************************************/
int asa(	
			double *a,  //array of parameters a
			double **ptr_a, // array of pointers which are from thread structure
			bool *lista,
			int ma,
		    double *amin,
			double *amax,
		    double *chisqr,
		    FitFunctionASA funcsasa,
			WCHAR *wchar,
			HWND *hProgressBar){
/***************************************************************************/
    /*
    Find global minimum in Chi^2 using asa.
    Parameters are:

	ma      -       number of fit parameters
	lista[] -       indices of free (unfixed) fit parameters = 1,2,3,...
	amin[]  -       lower boundaries of fit parameters
	amax[]  -       upper boundaries of fit parameters
	chisqr  -       pointer to (non normalized) chi^2.
	funcsasa        pointer to fit function of asa type
	hProgressBar -  acces to progres bars
    The refined values of a[] as well as the errors asig[] and chi^2 are returned.
    */

   int    i, nfree;
   int	  exit_code;
   double *bmin, *bmax;


	//Set VFSR
	if(vfsr_par.TEMPERATURE_RATIO_SCALE==0.  ) vfsr_par.TEMPERATURE_RATIO_SCALE  = _TEMPERATURE_RATIO_SCALE;
	if(vfsr_par.TEMPERATURE_ANNEAL_SCALE==0. ) vfsr_par.TEMPERATURE_ANNEAL_SCALE = _TEMPERATURE_ANNEAL_SCALE;
	if(vfsr_par.COST_PARAMETER_SCALE==0.     ) vfsr_par.COST_PARAMETER_SCALE     = _COST_PARAMETER_SCALE;

	if(vfsr_par.ACCEPTED_TO_GENERATED_RATIO==0.  ) vfsr_par.ACCEPTED_TO_GENERATED_RATIO  = _ACCEPTED_TO_GENERATED_RATIO;
	if(vfsr_par.INITIAL_PARAMETER_TEMPERATURE==0. ) vfsr_par.INITIAL_PARAMETER_TEMPERATURE = _INITIAL_PARAMETER_TEMPERATURE;
	if(vfsr_par.DELTA_X==0.     ) vfsr_par.DELTA_X     = _DELTA_X;

	if(vfsr_par.LIMIT_ACCEPTANCES==0 ) vfsr_par.LIMIT_ACCEPTANCES = _LIMIT_ACCEPTANCES;
	if(vfsr_par.TESTING_FREQUENCY_MODULUS ==0.     ) vfsr_par.TESTING_FREQUENCY_MODULUS    = _TESTING_FREQUENCY_MODULUS;

   //Warm up rnd. generator
	initialize_rng();

	bmin = new double[ma+1];
	bmax = new double[ma+1];


	/* The range of all parameters is rewritten to bmin[i] and bmax[i] the range must be 0 for fixed parameters in vfsr*/
	/* Setting range of parameters, fixed ones have 0 range */
    for (i=0; i<ma; ++i){
		if (!lista[i]) bmax[i] = bmin[i] = *(double*) ptr_a[i]; //can be just a[i] at this stage, it is the same
		else{
			bmin[i] = amin[i];
			bmax[i] = amax[i];
	    }
	}

	//Find the number of free parameters (not fixed)
	nfree = 0;
    for (i = 0; i < ma; i++){
		if (lista[i]) nfree++;
	}

	/* begining of ASA*/
	exit_code=0;

	//optimize the cost_function, returning the results in cost_value and cost_parameters 
    *chisqr = vfsr(funcsasa,			//pointer to the fit function 
					  randflt,			//pointer to the random generator
					  ma,				//number of fit parameters
					  nfree,			//number of free fit parameters not fixed
					  a,				//pointer to parameter values array
					  ptr_a,	//pointer to parameter pointers values array
					  bmin,				//pointer to minimum array
					  bmax,				//pointer to max array
					  &exit_code,		//exit code status (reason)
					  hProgressBar		//Pointers to the progress bars
					  );


	delete[] bmin; 
	delete[] bmax; 

	if (exit_code==0) wsprintf(wchar, L"Normal exit"); 
	if (exit_code==1) wsprintf(wchar, L"Parameter temperature too small."); 
	if (exit_code==2) wsprintf(wchar, L"C temperature too small."); 
	if (exit_code==3) wsprintf(wchar, L"Chi^2 with the same value repeated maximum times." ); 

 return 1;
}




/* ***********************************************************************/
/* Here is a good random number generator                                */
#define SHUFFLE 256		/* size of random array							 */
/*                                                                       */
double random_array[SHUFFLE];	/* random variables                      */
/*                                                                       */
/*************************************************************************
* Author: A.P.B.
* Date	 28 May 93
* Procedure:
*	double myrand(void) - returns a random number between 0 and 1
* Parameters: None
* Inputs:
* Outputs:
* Global Variables: None
* Returns: None
* Calls: None
* Description:
*	This routine returns the random number generator between 0 and 1
* Local Variables: None
* Portability:
* Other:
***********************************************************************/

inline double myrand(void)

/* returns random number in {0,1} */
/* requires one call to initialize_rng to truly randomize */
{
    return ((double) rand()/RAND_MAX);//32767
}

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure:
*	double randflt(void) - Shuffles random numbers in random_array[]
* Parameters: None
* Inputs:
* Outputs:
* Global Variables: None
* Returns: None
* Calls:
*	myrand()
* Description:
*	This routine initializes the random number generator
* Local Variables: None
* Portability:
* Other: Function changed by Daniel Kaminski to reduce number of calculations
* 25.10.2014
***********************************************************************/

inline double randflt(void)

/* shuffles random numbers in random_array[SHUFFLE] array */
{
    double rranf;
    double *kranf;

    kranf = (random_array + (int) (myrand() * SHUFFLE) % SHUFFLE);
    rranf = *kranf;
    *kranf = myrand(); //Used value replease with new one
	return (rranf);
	
}

/***********************************************************************
* Author: A.P.B.
* Date	 28 May 93
* Procedure:
*	initialize_rng() - to initialize the random number generator
* Parameters: None
* Inputs:
* Outputs:
* Global Variables: None
* Returns: None
* Calls:
*	myrand()
*	randflt()
* Description:
*	This routine initializes the random number generator
* Local Variables: None
* Portability:
* Other:
***********************************************************************/
void initialize_rng(void)

{   int n, seed;
    time_t now;

    /*  first initialize the seed for random number generation  */
    /*  time() returns time (in sec?), on IBM clock() returns msec since program began  */

	time(&now);

	seed = ((unsigned int) now + (unsigned int) clock());

	srand(seed);

	for (n = 0; n < SHUFFLE; ++n)
	random_array[n] = myrand();
}

