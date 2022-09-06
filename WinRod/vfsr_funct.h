//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of VFSR optymallisation rutine.
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------


#pragma once
#include "fit.h"
#include "math.h"

//#include <time.h>


#define TRUE			1
#define FALSE			0

#define ONE ((double) 1.0)
#define TWO ((double) 2.0)
#define TEN ((double) 10.0)
#define HALF ((double) 0.5)

#define NORMAL_EXIT			0
#define P_TEMP_TOO_SMALL	1
#define C_TEMP_TOO_SMALL	2
#define COST_REPEATING		3


struct VFSR_par{
double ACCEPTED_TO_GENERATED_RATIO;
double TEMPERATURE_RATIO_SCALE;
double TEMPERATURE_ANNEAL_SCALE;
double COST_PARAMETER_SCALE;
double INITIAL_PARAMETER_TEMPERATURE;
double DELTA_X;
int LIMIT_ACCEPTANCES;
int TESTING_FREQUENCY_MODULUS;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ASA_NPRINT 20
#define USER_INITIAL_PARAMETERS TRUE

#define _ACCEPTED_TO_GENERATED_RATIO 1.0E-11		// determines when to test for reaneling to tune the delta values and the spred of jumps smaller better

#define _LIMIT_ACCEPTANCES 6000						//How many cycles is in a loop, the loop can finisch elier if temperature parameters or temperature acceptance is too small

#define _TEMPERATURE_RATIO_SCALE 9.0E-8				//1e-6 gives better results than 1e-8 but increases the calculation time


#define _TEMPERATURE_ANNEAL_SCALE  _LIMIT_ACCEPTANCES/10				// _LIMIT_ACCEPTANCES/TEN /*ACCEPTED_LIMIT*/

#define _COST_PARAMETER_SCALE 0.9					//cost of acceptance of new state low cost more accepted states less precise higher ->more states per accepted better precision 0.8 is good

#define _TESTING_FREQUENCY_MODULUS 146				//50-300 is good, to often is not good

#define MAXIMUM_REANNEAL_INDEX 1.0E-6				// Reset index_parameter_generations if index reset too large, and also reset the initial_parameter_temperature, to achieve the same new temperature.

#define REANNEAL_RESCALE 3.0

#define _INITIAL_PARAMETER_TEMPERATURE 2.0

#define NUMBER_COST_SAMPLES 6						//Initial average start parameter(i) value from  the given number of samples

#define MAXIMUM_COST_REPEAT 3
	
#define _DELTA_X 0.000001	/* delta of derivatives in cost_derivatives */

#define SMALL_double 1.0E-20

#define COST_PRECISION 1.0E-4  /* EV: enough for chi^2 in ROD precision at MAXIMUM_COST_REPEAT  */

#define IO_PROTOTYPES FALSE

#define ACTIVATE_REANNEAL TRUE


//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header with function prototypes used by WinRod and VFSR.
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

int		accept_new_state(RandomASA user_random_generator);
void	generate_new_state(RandomASA user_random_generator);
void	reanneal(void);
void	cost_derivatives(FitFunctionASA user_cost_function, double** parameter_pointers, int *g_number_parameters);//
double	generate_vfsr_state(double temp, RandomASA user_random_generator);


//--------------------------------------------------------------------------------------------------------------------------------
double	vfsr(							/* return final_cost */
				FitFunctionASA,			/* optymalised function */
				RandomASA,				/* random generator */
				int,					/* number of parameters */
				int,					/* parameters to calculations */
				double*,				/* initial and final parameters */
				double**,				/* array of  parameter pointers for spead up */
				double*,				/* minimum parameter values */
				double*,				/* maximum parameter values */
				int*,					/* exit code */
				HWND *hProgressBar
			);

//--------------------------------------------------------------------------------------------------------------------------------
void vfsr_exit(
				FitFunctionASA,			/* user's cost function */
				double*,				/* initial and final parameters */
				double*,				/* best cost found to return */
				int*					/* exit code */ 
			  );

//--------------------------------------------------------------------------------------------------------------------------------
void print_state(HWND *hProgressBar);