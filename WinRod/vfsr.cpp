

/* Quite drastically modified version for ROD of the Lester Ingberg
adaptive simulated annealing (asa) code. An early version of the code is
used (version 6.38 for 29 Dec 1992), when the name "very fast simulated
reannealing (vfsr)" was still used. */

/*
Update 28/10/2001 E. Vlieg (vlieg@sci.kun.nl)
	Use USER_INITIAL_PARAMETERS as flag set by user.
Update 21/10/2001 E. Vlieg (vlieg@sci.kun.nl)
	Add test in reanneal() when calulating new index_cost_acceptances:
	the rounding sometimes yields a zero, which gives a very high
	annealing temperature.
	Asign correct memory for all parameters (thus remove the extra bytes
        and correct curvatures in particular.
Update 14/10/2001 E. Vlieg (vlieg@sci.kun.nl)
	Remove HAVE_ANSI flags, because we always use ANSI.
	Add (temporary) asatest file.
	Changed accept_new_state to int in order to use it as flag.
Update 07.07.2014 D. Kaminski (daniel_kaminski3@wp.pl)
	Added array of pointers of fitting parameters. With this trick it is much faster to copy parameters between vfsr and fit_asa function. 
	Now it is done with one simple loop.
*/

/* vfsr.h for Very Fast Simulated Reannealing */

#include "stdafx.h"
#include "vfsr_funct.h"
#include "vfsr.h"

//for comunication with WinRod during fitting
#include "Resource.h"
#include <commctrl.h>
#include <windows.h>

#define VFSR_PRINT1
#include <stdio.h>
#ifdef VFSR_PRINT1
 FILE*   ptr_vfsr_out = NULL;
#endif

 VFSR_par vfsr_par;

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: vfsr
* Parameters:
* Inputs:
*     double (*user_cost_function) ()	- user's cost function
*     double (*user_random_generator) ()	-user's rng
*     int g_number_parameters		- number of parameters
*     double *parameter_initial_final	- initial and final parameters
*     double **parameter_pointer	- array of parameters pointers
*     double *g_parameter_minimum	- minimum parameter values
*     double *g_parameter_maximum	- maximum parameter values
*     double *g_tangents		- holds 1st derivatives
*     double *g_curvature		- holds 2nd derivatives
*     int *exit_status                  - reason for exiting
* Outputs:
*     double *parameter_initial_final	- holds final parameters
* Global Variables:
* Returns:
*     double final_cost		        - best cost found
* Calls:
* Description:
*	This procedure implements the full VFSR function optimization.
* Local Variables:
*	int index_cost_constraint	- index initial cost functions
*					- to set initial temperature
*	int temp_var_int		- integer temporary value
*	int index_v			- iteration index
*	double index_cost_repeat	- test when MAXIMUM_COST_REPEAT
 *      double sampled_cost_sum	        - temporary initial costs
*	double tmp_var_db1, tmp_var_db2	- temporary doubles
* Portability:
* Other:
***********************************************************************/

double vfsr(							/* return final_cost */
	FitFunctionASA user_cost_function,
	RandomASA user_random_generator,
	int g_number_parameters,			/* number of parameters */
	int g_number_param,					/*parameters to calculations */
	double *parameter_initial_final,	/* initial and final parameters */
	double**parameter_pointers,		/* parameter pointers*/
	double *g_parameter_minimum,		/* minimum parameter values */
	double *g_parameter_maximum,		/* maximum parameter values */
	int *exit_status,					/* exit code */
	HWND *hProgressBar
	)
{
    int index_cost_constraint,	/* index initial cost functions averaged
									to set initial temperature */
    temp_var_int,				/* integer temporary value */
    index_v;					/* iteration index */
    double index_cost_repeat,	/* test COST_PRECISION when this reaches
									MAXIMUM_COST_REPEAT */
    sampled_cost_sum,			/* temporary initial costs */
    final_cost,					/* best cost to return to user */
    tmp_var_db1, tmp_var_db2;	/* temporary doubles */

    /* initializations */

	/* g_... are defined to make local information passed to vfsr() by
	the user into global variables useful for internal vfsr() routines.
	This was considered a reasonable tradeoff to passing several
	parameters between several routines. */

	number_parameters = g_number_param;
	number_all_parameters = g_number_parameters; /*number all parameters */

	/* set indices and counts to 0 */
	best_number_generated_saved = number_generated = recent_number_generated = recent_number_acceptances = 0;
	index_cost_acceptances = best_number_accepted_saved = number_accepted = number_acceptances_saved = 1;
	index_cost_repeat = 0;

	/* do not calculate curvatures initially */
	curvature_flag = FALSE;


    /* allocate storage for all parameters */
	current_generated_state.parameter	 = new double[g_number_parameters + 1];
	last_saved_state.parameter			 = new double[g_number_parameters + 1];
	best_generated_state.parameter		 = new double[g_number_parameters + 1];
	current_parameter_temperature		 = new double[g_number_parameters + 1];
	initial_parameter_temperature		 = new double[g_number_parameters + 1];
	index_parameter_generations			 = new long int[g_number_parameters + 1];
	tangents							 = new double[g_number_parameters + 1];
	curvature							 = new double[g_number_parameters * g_number_parameters + 1];

	parameter_minimum					 = new double[g_number_parameters + 1];
	parameter_maximum					 = new double[g_number_parameters + 1];

	/* set the user's initial parameters to the initial system state */
    VFOR(index_v) {
	current_generated_state.parameter[index_v] = parameter_initial_final[index_v];
    }




    VFOR(index_v) {
	parameter_minimum[index_v] =  g_parameter_minimum[index_v];
    }

	VFOR(index_v) {
	parameter_maximum[index_v] =  g_parameter_maximum[index_v];
    }


#ifdef VFSR_PRINT1

 char str [MAX_PATH]={};

 errno_t err = fopen_s(&ptr_vfsr_out, "Asa.res", "wt" );
	if(err != NULL){
		MessageBox( NULL, L"Unable to create WinRod macro file", L"Error", MB_OK|MB_ICONERROR);
		return false;
	} 

   fprintf(ptr_vfsr_out, "number_parameters = %d\n",number_parameters);
   fprintf(ptr_vfsr_out,"n = %4d; c_cost = %6.3f; c_par = %6.3f\n",number_parameters,
		-log((double)vfsr_par.TEMPERATURE_RATIO_SCALE)*exp(-log((double)vfsr_par.TEMPERATURE_ANNEAL_SCALE)/number_parameters)*vfsr_par.COST_PARAMETER_SCALE,
		-log((double)vfsr_par.TEMPERATURE_RATIO_SCALE)*exp(-log((double)vfsr_par.TEMPERATURE_ANNEAL_SCALE)/number_parameters));
  fprintf(ptr_vfsr_out,"%10s %10s %10s %10s %10s %10s %10s\n","T_anneal","T_par_min", "T_par_max","N_gener.","N_accep.","curr. cost","best cost");
#endif

    VFOR(index_v) {
		/* set all starting and current temperatures */
		current_parameter_temperature[index_v] = initial_parameter_temperature[index_v] = (double) vfsr_par.INITIAL_PARAMETER_TEMPERATURE;

		/* set all states to the initial parameter values */
		best_generated_state.parameter[index_v] = last_saved_state.parameter[index_v] = current_generated_state.parameter[index_v];

#ifdef VFSR_PRINT1
		fprintf(ptr_vfsr_out, " %-8d %-18.5g %-17.5g %-15.5g \n",
			index_v,
			parameter_minimum[index_v],
			parameter_maximum[index_v],
			current_generated_state.parameter[index_v]);

#endif
    }
#ifdef VFSR_PRINT1
    fprintf(ptr_vfsr_out, "\n\n");
#endif

    /* set the initial index of parameter generations to 1 */
    VFOR(index_v) {
		/* ignore parameters that have too small a range */
		if (PARAMETER_RANGE_TOO_SMALL(index_v))
			continue;
		index_parameter_generations[index_v] = 1;
    }

    /* calculate the average cost over samplings of the cost function */
    sampled_cost_sum = 0.0;
    for (index_cost_constraint = 0; index_cost_constraint < ((int) NUMBER_COST_SAMPLES); ++index_cost_constraint) {
		number_invalid_generated_states = 0;
		do {
			++number_invalid_generated_states;
			valid_state_generated_flag = TRUE;
			generate_new_state(user_random_generator);



			current_generated_state.cost =
				(*user_cost_function)(current_generated_state.parameter, parameter_pointers, &g_number_parameters, &valid_state_generated_flag);
	} while (valid_state_generated_flag == FALSE);
	--number_invalid_generated_states;
	sampled_cost_sum += fabs(current_generated_state.cost);
    }

    /* set all cost temperatures to the average cost */
    initial_cost_temperature = current_cost_temperature = sampled_cost_sum / ((double) NUMBER_COST_SAMPLES);

    /* establish an initial current state */
    /* user's initial guess of parameters */
    VFOR(index_v) {
		current_generated_state.parameter[index_v] = parameter_initial_final[index_v];
    }

//#if USER_INITIAL_PARAMETERS	/* if using user's initial parameters */
    if (USER_INITIAL_PARAMETERS == TRUE){
		valid_state_generated_flag = TRUE;
		current_generated_state.cost = (*user_cost_function)(current_generated_state.parameter, parameter_pointers, &g_number_parameters, &valid_state_generated_flag);
	}

#ifdef VFSR_PRINT1
    if (valid_state_generated_flag == FALSE)
		fprintf(ptr_vfsr_out, "user's initial parameters generated FALSE valid_state_generated_flag\n");
#endif

	/* let vfsr generate valid initial parameters */
    else
	{
	do {
	    ++number_invalid_generated_states;
	    valid_state_generated_flag = TRUE;
	    generate_new_state(user_random_generator);
		current_generated_state.cost = (*user_cost_function)(current_generated_state.parameter, parameter_pointers, &g_number_parameters, &valid_state_generated_flag);
	} while (valid_state_generated_flag == FALSE);
	--number_invalid_generated_states;
	}

	/* USER_INITIAL_PARAMETERS */
    /* set all states to the last one generated */
    VFOR(index_v) {
		/* ignore parameters that have too small a range */
		if (PARAMETER_RANGE_TOO_SMALL(index_v))
			continue;
		best_generated_state.parameter[index_v] = last_saved_state.parameter[index_v] = current_generated_state.parameter[index_v];
    }

    /* set all costs to the last one generated */
    best_generated_state.cost = last_saved_state.cost =	current_generated_state.cost;
    accepted_to_generated_ratio = ONE;

    /* compute 1/number_parameters for efficiency */
    one_over_number_parameters = ONE / number_parameters;
    temperature_rescale_power = ONE / pow((double) REANNEAL_RESCALE, one_over_number_parameters);
    tmp_var_db1 = -log(((double) vfsr_par.TEMPERATURE_RATIO_SCALE));

    /* compute temperature scales */
    tmp_var_db2 = log((double) vfsr_par.TEMPERATURE_ANNEAL_SCALE);
    temperature_scale = tmp_var_db1 * exp(-tmp_var_db2 / (double) number_parameters);

    temperature_scale_parameters = temperature_scale;
    temperature_scale_cost = temperature_scale * vfsr_par.COST_PARAMETER_SCALE;

    /* do not calculate curvatures initially */
    curvature_flag = FALSE;

#ifdef VFSR_PRINT1
    fprintf(ptr_vfsr_out,  "temperature_scale = %10.5g\n", temperature_scale);
    fprintf(ptr_vfsr_out, "T_scale_parameters = %10.5g\nT_scale_cost = %10.5g\n", temperature_scale_parameters, temperature_scale_cost);
    fprintf(ptr_vfsr_out, "\n");
    //print_state();
    fprintf (ptr_vfsr_out,"%10s %10s %10s %10s %10s\n","T_anneal", "N_gener.","N_accep.","curr. cost","best cost");
#endif

    /* reset the current cost and the number of generations performed */
    number_invalid_generated_states = 0;
    best_number_generated_saved = number_generated = recent_number_generated = 0;
    VFOR(index_v) {
		/* ignore parameters that have too small a range */
		if (PARAMETER_RANGE_TOO_SMALL(index_v))
			continue;
		index_parameter_generations[index_v] = 1;
		}

    /* MAIN ANNEALING LOOP */

    while (number_accepted <= ((long int) vfsr_par.LIMIT_ACCEPTANCES)) {

//////////////* CALCULATE NEW TEMPERATURES *///////////////////////////////

		/* calculate new parameter temperatures */
		VFOR(index_v) {
			/* skip parameters with too small a range */
			if (PARAMETER_RANGE_TOO_SMALL(index_v))
				continue;

			log_new_temperature_ratio = -temperature_scale_parameters * pow((double) index_parameter_generations[index_v], one_over_number_parameters);
			/* check (and correct) for too large an exponent */
			log_new_temperature_ratio =	EXPONENT_CHECK(log_new_temperature_ratio);
			current_parameter_temperature[index_v] = initial_parameter_temperature[index_v]	* exp(log_new_temperature_ratio);

			/* check for too small a parameter temperature */
			if (exp(log_new_temperature_ratio) < (double) SMALL_double) {
				*exit_status = (int) P_TEMP_TOO_SMALL;
				index_exit_v = index_v;

				/* Note that this abridged exit can hold and print old values of some variables, such as current_cost_temperature. */
				vfsr_exit(user_cost_function,
					  parameter_initial_final,
					  &final_cost,
					  exit_status);
				return (final_cost);
			}
		}

		/* calculate new cost temperature */
		log_new_temperature_ratio = -temperature_scale_cost * pow((double) index_cost_acceptances, one_over_number_parameters);
		log_new_temperature_ratio =  EXPONENT_CHECK(log_new_temperature_ratio);
		current_cost_temperature = initial_cost_temperature * exp(log_new_temperature_ratio);

		/* check for too small a cost temperature */
		if (exp(log_new_temperature_ratio) < (double) SMALL_double) {
			*exit_status = (int) C_TEMP_TOO_SMALL;
			/* Note that this abridged exit can hold and print old values of some variables. */
			vfsr_exit(user_cost_function,
						parameter_initial_final,
						&final_cost,
						exit_status);
			return (final_cost);
		}

////////////////////////////////////////////////////////////////////////////
//////////////////* GENERATE NEW PARAMETERS *///////////////////////////////
////////////////////////////////////////////////////////////////////////////

		/* generate a new valid set of parameters */
		do {
			++number_invalid_generated_states;
			valid_state_generated_flag = TRUE;
			generate_new_state(user_random_generator);
			current_generated_state.cost = (*user_cost_function)(current_generated_state.parameter, parameter_pointers, &g_number_parameters, &valid_state_generated_flag);
		} while (valid_state_generated_flag == FALSE);
		--number_invalid_generated_states;

		/* ACCEPT/REJECT NEW PARAMETERS */
		/* decide to accept/reject the new state */
		if (accept_new_state(user_random_generator)){
			if (number_accepted%ASA_NPRINT == 0) print_state(hProgressBar); /* EV 19/9/01 */
		}

		/* calculate the ratio of acceptances to generated states */
		accepted_to_generated_ratio = (double)(recent_number_acceptances + 1) / (recent_number_generated + 1);


		/* CHECK FOR NEW MINIMUM */

		if (current_generated_state.cost < best_generated_state.cost) {
			/* NEW MINIMUM FOUND */

			/* reset the recent acceptances and generated counts */
			recent_number_acceptances = recent_number_generated = 0;
			best_number_generated_saved = number_generated;
			best_number_accepted_saved = number_accepted;

			/* copy the current state into the best_generated state */
			best_generated_state.cost = current_generated_state.cost;
			VFOR(index_v) {
				/* ignore parameters that have too small a range */
				if (PARAMETER_RANGE_TOO_SMALL(index_v))
					continue;
				best_generated_state.parameter[index_v] =  current_generated_state.parameter[index_v];
			}

	    /* printout the new minimum state and value */
#ifdef VFSR_PRINT1
			fprintf(ptr_vfsr_out, "best_generated_state = %-12.5g\nnumber_accepted=%ld\n number_generated=%ld\n", best_generated_state.cost, number_accepted, number_generated);/* EV */
#endif
		}


		temp_var_int = (int) (number_accepted % ((long int) vfsr_par.TESTING_FREQUENCY_MODULUS));
		if ((temp_var_int == 0 && number_acceptances_saved == number_accepted) || accepted_to_generated_ratio < ((double) vfsr_par.ACCEPTED_TO_GENERATED_RATIO)) {
			if (accepted_to_generated_ratio < ((double) vfsr_par.ACCEPTED_TO_GENERATED_RATIO))
				recent_number_acceptances = recent_number_generated = 0;

			/* if best.cost repeats MAXIMUM_COST_REPEAT then exit */
			if (fabs(last_saved_state.cost - best_generated_state.cost) < (double) COST_PRECISION) {
				++index_cost_repeat;
				if (index_cost_repeat == ((int) MAXIMUM_COST_REPEAT)) {
					*exit_status = (int) COST_REPEATING;
					/* Note that this abridged exit can hold and print old values of some variables. */
					vfsr_exit(user_cost_function,
							parameter_initial_final,
							&final_cost,
							exit_status);
					return (final_cost);
				}
			}
			else
				index_cost_repeat = 0;

#if ACTIVATE_REANNEAL		/* set to FALSE to skip reannealing */
			/* calculate tangents, not curvatures, to reanneal */
			curvature_flag = FALSE;
			cost_derivatives(user_cost_function, parameter_pointers, &g_number_parameters);
			reanneal();
#endif
		}
    }
    /* FINISHED ANNEALING and MINIMIZATION */

    *exit_status = (int) NORMAL_EXIT;
    vfsr_exit(user_cost_function,
				parameter_initial_final,
				&final_cost,
				exit_status);
    return (final_cost);
}

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: vfsr_exit
*	This procedures cleans up the system and frees storage, and
*	copies the best parameters and cost into final_cost and
*	parameter_initial_final
* Parameters:
* Inputs:
*	double user_cost_function - user's cost function
	int *exit_status           - exit code
* Outputs:
*	double *parameter_initial_final   - final parameters state
*	double *final_cost - final cost at state parameter_initial_final
* Global Variables:
*	curvature_flag - reset to compute curvature
*	best_generated_state.parameter    - array of best parameters
*	current_generated_state.parameter - free this storage
*	last_saved_state.parameter        - free this storage
*	best_generated_state.parameter    - free this storage
*	current_parameter_temperature     - free this storage
*	initial_parameter_temperature     - free this storage
*	index_parameter_generations       - free this storage
*	ptr_vfsr_out - output file
* Returns: None
* Calls:
*	cost_derivatives()
*	print_state()
*	free()
*	fclose()
* Description:
* Local Variables:
*	int index_v - iteration index
* Portability:
* Other:
***********************************************************************/
//#if HAVE_ANSI
void vfsr_exit(
		  FitFunctionASA user_cost_function,	/* user's cost function */
		  double *parameter_initial_final,	/* initial and final
							   parameters */
		  double *final_cost,	/* best cost found to return */
		  int *exit_status /* exit code */ )

{
    int index_v;		/* iteration index */

    /* calculate curvatures and tangents at best point */
    /* why would you want to do this? */
//    curvature_flag = TRUE;
//    cost_derivatives(user_cost_function);

    /* return final function minimum and associated parameters */
    *final_cost = best_generated_state.cost;
    VFOR(index_v) {
		parameter_initial_final[index_v] = best_generated_state.parameter[index_v];
    }


#ifdef VFSR_PRINT1
 //   print_state();

    switch (*exit_status) {
    case 0:
	fprintf(ptr_vfsr_out,
		"\n\n NORMAL_EXIT exit_status \n "
		);
	break;
    case 1:
	fprintf(ptr_vfsr_out,
		"\n\n P_TEMP_TOO_SMALL exit_status \n "
		);
	fprintf(ptr_vfsr_out,
			"\n current_parameter_temperature[%d] too small = %10.5g\n",
	      index_exit_v, current_parameter_temperature[index_exit_v]);
	break;
    case 2:
	fprintf(ptr_vfsr_out,
		"\n\n C_TEMP_TOO_SMALL exit_status = %d\n",
		*exit_status);
	fprintf(ptr_vfsr_out,
		"\n current_cost_temperature too small = %10.5g\n",
		current_cost_temperature);
	break;
    case 3:
	fprintf(ptr_vfsr_out,
		"\n\n COST_REPEATING exit_status = %d\n",
		*exit_status);
	break;
    default:
	fprintf(ptr_vfsr_out, "\n\n ERR: no exit code available = %d\n",
		*exit_status);
    }

	fprintf(ptr_vfsr_out,
	    "final_cost = best_generated_state.cost = %-15.5g\n",
	    *final_cost);
	fprintf(ptr_vfsr_out,
	    "number_accepted at best_generated_state.cost = %ld\n",
	    best_number_accepted_saved);
	fprintf(ptr_vfsr_out,
	    "number_generated at best_generated_state.cost = %ld\n",
	    best_number_generated_saved);
#endif

    /* return unused space */
    delete[] current_generated_state.parameter;
    delete[] last_saved_state.parameter;
    delete[] best_generated_state.parameter;
    delete[] current_parameter_temperature;
    delete[] initial_parameter_temperature;
    delete[] index_parameter_generations;
    delete[] tangents;
    delete[] curvature;

	delete[] parameter_minimum;
	delete[] parameter_maximum;

#ifdef VFSR_PRINT1
    fclose(ptr_vfsr_out);
#endif
}

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: generate_new_state
* Parameters:
* Inputs:
*	double (*user_random_generator) () - the random number generator
* Outputs: None
* Global Variables:
* Returns: None
* Calls:
*	fabs                     - absolute value function
*	generate_vfsr_state      - to get a vfsr distributed state
* Description:
*	Generates a valid new state from the old state
* Local Variables:
*	int index_v		 - iteration index v
*	double x		 - the new parameter value
*	double parameter_v	 - the vth parameter value
*	double min_parameter_v 	 - the vth parameter lower bound
*	double max_parameter_v 	 - the vth parameter upper bound
*	double temperature_v	 - the vth parameter temperature
*	double parameter_range_v - the vth parameter range
* Portability:
* Other:
***********************************************************************/
 /* generate a new state from the old state that lies between
    the min and max parameter values */
void generate_new_state(RandomASA user_random_generator)

{
    int index_v;
    double x;
    double parameter_v, min_parameter_v, max_parameter_v, temperature_v, parameter_range_v;

    /* generate a new value for each parameter */
    VFOR(index_v) {
		min_parameter_v = parameter_minimum[index_v];
		max_parameter_v = parameter_maximum[index_v];
		parameter_range_v = max_parameter_v - min_parameter_v;

		/* ignore parameters that have too small a range */
		if (fabs(parameter_range_v) < (double) SMALL_double)
			continue;

		temperature_v = current_parameter_temperature[index_v];
		parameter_v = last_saved_state.parameter[index_v];

		/* generate a new state x until within the parameter bounds */
		for (;;) {
			x = parameter_v + generate_vfsr_state(temperature_v, user_random_generator) * parameter_range_v;
			/* exit the loop if within its valid parameter range */
			if (x <= max_parameter_v - (double) SMALL_double && x >= min_parameter_v + (double) SMALL_double)
				break;
		}

		/* save the newly generated value */

        //	 printf( "x = %ld   parameter_range_v = %g \n ", x,parameter_range_v);
		current_generated_state.parameter[index_v] = x;
    }
}

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: generate_vfsr_state
* Parameters:
* Inputs: double Temperature
*	double user_random_generator() returns random number between 0,1
* Outputs: None
* Global Variables: None
* Returns: double: A random variable VFSR distributed
* Calls:
*	double pow() power function
*	double fabs() absolute value function
* Description:
*	This function generates a single value according to the
*	VFSR generating function and the passed temperature
* Local Variables:
*	double x - temporary variable
*	double y - temporary variable
*	double z - temporary variable
* Portability:
* Fully portable
* Other:
***********************************************************************/
double generate_vfsr_state(double temp, RandomASA user_random_generator){
 double x, y, z;

    x = (*user_random_generator) ();
    y = x < HALF ? -ONE : ONE;
    z = y * temp * (pow((ONE + ONE / temp), fabs(TWO * x - ONE)) - ONE);

 return (z);

}

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: accept_new_state
*	This procedure accepts or rejects a newly generated state,
*	depending on whether the difference between new and old
*	cost functions passes a statistical test. If accepted,
*	the current state is updated.
* Parameters:
* Inputs: None
* double user_random_generator() returning a random number between 0,1
* Outputs: None
* Global Variables:
* Returns: None
* Calls:
* Description:
* Local Variables:
* Portability:
* Other:
***********************************************************************/
 /* determine whether to accept or reject the new state */
int accept_new_state(RandomASA user_random_generator)   /* EV */
{
    double delta_cost;
    int index_v;

    /* update accepted and generated count */
    ++number_acceptances_saved;
    ++recent_number_generated;
    ++number_generated;

    /* increment the parameter index generation for each parameter */
    VFOR(index_v) {
	/* ignore parameters with too small a range */
	if (PARAMETER_RANGE_TOO_SMALL(index_v))
	    continue;
	++(index_parameter_generations[index_v]);
    }

    /* effective cost function for testing acceptance criteria,
       calculate the cost difference and divide by the temperature */
    delta_cost = (current_generated_state.cost - last_saved_state.cost)
	/ current_cost_temperature;

    /* decide to accept/reject the new state */
    if (exp(-EXPONENT_CHECK(delta_cost)) > (*user_random_generator) ()) {
	/* copy the current generated parameters to the last saved state */
	last_saved_state.cost = current_generated_state.cost;
	VFOR(index_v) {
	    /* ignore parameters with too small a range */
	    if (PARAMETER_RANGE_TOO_SMALL(index_v))
		continue;
	    last_saved_state.parameter[index_v] =
		current_generated_state.parameter[index_v];
	}
	/* update acceptance counts */
	++recent_number_acceptances;
	++number_accepted;
	++index_cost_acceptances;
	number_acceptances_saved = number_accepted;
	return(1); /* EV */
    }
    return(0); /* EV */
}

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: reanneal
* Parameters:
* Inputs: None
* Outputs: None
* Global Variables:
* Returns: None
* Calls:
* Description:
*	Readjust temperatures of generating and acceptance functions
* Local Variables:
*	int index_v				- vth iteration index
*	double tmp_var_db2			-
*	double new_temperature			- the new temperature
*	double log_initial_current_temperature_ratio - for efficiency
* Portability:
* Other:
***********************************************************************/
 /* REANNEALING - readjust the temperatures */
void reanneal(void)

{
	int index_v;
	double tmp_var_db2;
	double new_temperature;	/* the temperature */
	double log_initial_current_temperature_ratio;

	VFOR(index_v) {
		/* ignore parameters with too small a range */
		if (PARAMETER_RANGE_TOO_SMALL(index_v))
			continue;
		/* ignore parameters with too small tangents */
		if (fabs(tangents[index_v]) < (double)SMALL_double)
			continue;

		/* reset the index of parameter generations appropriately */
		new_temperature =
			current_parameter_temperature[index_v] *
			(maximum_tangent / fabs(tangents[index_v]));
		if (new_temperature < initial_parameter_temperature[index_v]) {
			log_initial_current_temperature_ratio =
				log(((double)SMALL_double + initial_parameter_temperature[index_v])
				/ ((double)SMALL_double + new_temperature));
			index_parameter_generations[index_v] =
				(long int)((double)SMALL_double
				+ pow(log_initial_current_temperature_ratio
				/ temperature_scale_parameters,
				number_parameters));
		}
		else
			index_parameter_generations[index_v] = 1;

		/* Reset index_parameter_generations if index reset too large,
			   and also reset the initial_parameter_temperature, to achieve
			   the same new temperature. */
		while (index_parameter_generations[index_v]
		> ((long int)MAXIMUM_REANNEAL_INDEX)) {
			log_new_temperature_ratio = -temperature_scale_parameters *
				pow((double)index_parameter_generations[index_v],
				one_over_number_parameters);
			log_new_temperature_ratio =
				EXPONENT_CHECK(log_new_temperature_ratio);
			new_temperature =
				initial_parameter_temperature[index_v]
				* exp(log_new_temperature_ratio);
			index_parameter_generations[index_v] /=
				(long int)REANNEAL_RESCALE;
			initial_parameter_temperature[index_v] =
				new_temperature * pow(
				initial_parameter_temperature[index_v] / new_temperature,
				temperature_rescale_power);
		}
	}

	/* reanneal : Reset the index of cost acceptances to take
	   into account finer detail in cost terrain. */

	/* set the starting cost_temperature to last cost found so far */
	if (initial_cost_temperature > fabs(last_saved_state.cost))
		initial_cost_temperature = fabs(last_saved_state.cost);

	if (current_cost_temperature > fabs(best_generated_state.cost)) {
		tmp_var_db2 =
			log(((double)SMALL_double + initial_cost_temperature) /
			((double)SMALL_double + fabs(best_generated_state.cost)));
		index_cost_acceptances = (long int)((double)SMALL_double
			+ pow(tmp_var_db2
			/ temperature_scale_cost,
			number_parameters));
	}
	else {
		log_initial_current_temperature_ratio =
			log(((double)SMALL_double + initial_cost_temperature) /
			((double)SMALL_double + current_cost_temperature));
		index_cost_acceptances = (long int)((double)SMALL_double
			+ pow(log_initial_current_temperature_ratio
			/ temperature_scale_cost,
			number_parameters));
		/* Due to rounding errors, the index may become 0: recalculate the
		initial temperature in that case */
		if (index_cost_acceptances < 1)
		{
			index_cost_acceptances = 1;
			initial_cost_temperature = current_cost_temperature /
				exp(-temperature_scale_cost);
		}
	}

	/* reset index_cost_temperature if index reset too large */
	while (index_cost_acceptances > ((long int)MAXIMUM_REANNEAL_INDEX)) {
		log_new_temperature_ratio = -temperature_scale_cost *
			pow((double)index_cost_acceptances,
			one_over_number_parameters);
		log_new_temperature_ratio =
			EXPONENT_CHECK(log_new_temperature_ratio);
		new_temperature = initial_cost_temperature
			* exp(log_new_temperature_ratio);
		index_cost_acceptances /= (int)REANNEAL_RESCALE;
		/*	index_cost_acceptances /= (double) REANNEAL_RESCALE;  original */
		initial_cost_temperature =
			new_temperature * pow(
			initial_cost_temperature / new_temperature,
			temperature_rescale_power);
	}
}

/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: cost_derivatives
*	This procedure calculates the derivatives of the cost function
*	with respect to its parameters.  The first derivatives are
*	used as a sensitivity measure for reannealing.  The second
*	derivatives are calculated only if curvature_flag=TRUE;
*	these are a measure of the covariance of the fit when a
*	minimum is found.
* Parameters:
* Inputs: (double (*user_cost_function) () - the user's cost function
* Outputs: None
* Global Variables:
* Returns: None
* Calls:
* Description:
* Local Variables:
*	int index_v, index_vv, index_v_vv - iteration indices
*	long int saved_number_invalid_generated_states - temporary variable
*	double parameter_v, parameter_vv
*		- values of the v_th and vv_th parameters
*	double recent_best_cost - the most recently found best cost
*	double new_cost_state_1, new_cost_state_2, new_cost_state_3;
*		- cost values taken at sample points 1,2 and 3
* Portability:
* Other:
***********************************************************************/
 /* Calculate the numerical derivatives of the best
    generated state found so far */

/* In this implementation of VFSR, no checks are made for
   valid_state_generated_flag=FALSE for differential neighbors
   to the current best state. */

/* Assuming no information is given about the metric of the parameter
   space, use simple Cartesian space to calculate curvatures. */


void cost_derivatives(FitFunctionASA user_cost_function, double** parameter_pointers, int *g_number_parameters)
{
    int index_v, index_vv, index_v_vv, index_vv_v;
    long int saved_number_invalid_generated_states;
    double parameter_v, parameter_vv, recent_best_cost;
    double new_cost_state_1, new_cost_state_2, new_cost_state_3;

    /* save the best cost */
    recent_best_cost = best_generated_state.cost;

    /* copy the best state into the current state */
    VFOR(index_v) {
	/* ignore parameters with too small ranges */
	if (PARAMETER_RANGE_TOO_SMALL(index_v))
	    continue;
	current_generated_state.parameter[index_v] =
	    best_generated_state.parameter[index_v];
    }

    /* set parameters (& possibly constraints) to best state */
    saved_number_invalid_generated_states =
	number_invalid_generated_states;
    valid_state_generated_flag = TRUE;
    current_generated_state.cost =
	(*user_cost_function)(current_generated_state.parameter, parameter_pointers, g_number_parameters, &valid_state_generated_flag);

#ifdef VFSR_PRINT1
    if (valid_state_generated_flag == FALSE)	/* extra check */
	fprintf(ptr_vfsr_out,
		"Generated an invalid best state when calculating \
 the derivatives\n");
#endif

    number_invalid_generated_states =
	saved_number_invalid_generated_states;
    valid_state_generated_flag = TRUE;

    VFOR(index_v) {
	/* skip parameters with too small range or integer parameters */
#if INCLUDE_INTEGER_PARAMETERS
	if (PARAMETER_RANGE_TOO_SMALL(index_v))
	    continue;
#else
	if (PARAMETER_RANGE_TOO_SMALL(index_v))
	    continue;
#endif

	/* save parameter_v */
	parameter_v = best_generated_state.parameter[index_v];

	/* generate the first sample point and
           calculate cost at current point + DELTA_X */
	current_generated_state.parameter[index_v] =
	    (ONE + (double) vfsr_par.DELTA_X) * parameter_v;
	/* generate the first sample point */
	current_generated_state.cost =
		(*user_cost_function)(current_generated_state.parameter, parameter_pointers, g_number_parameters, &valid_state_generated_flag);
	new_cost_state_1 = current_generated_state.cost;
	valid_state_generated_flag = TRUE;

	/* restore the parameter state */
	current_generated_state.parameter[index_v] = parameter_v;

	/* calculate the numerical derivative */
	tangents[index_v] = (new_cost_state_1 - recent_best_cost)
	    / (((double) vfsr_par.DELTA_X) * parameter_v + (double) SMALL_double);

	/* calculate the diagonal curvatures */
	if (curvature_flag == TRUE) {

	    /* generate the second sample point and
               calculate cost at current point - DELTA_X */
	    current_generated_state.parameter[index_v] =
		(ONE - (double) vfsr_par.DELTA_X) * parameter_v;
	    current_generated_state.cost =
			(*user_cost_function)(current_generated_state.parameter, parameter_pointers, g_number_parameters, &valid_state_generated_flag);
	    new_cost_state_2 = current_generated_state.cost;
	    valid_state_generated_flag = TRUE;

	    /* restore the parameter state */
	    current_generated_state.parameter[index_v] =
		parameter_v;

	    /* index_v_vv: row index_v, column index_v */
	    index_v_vv = ROW_COL_INDEX(index_v, index_v);

	    /* calculate and store the curvature */
	    curvature[index_v_vv] =
		(new_cost_state_2 - TWO * recent_best_cost
		 + new_cost_state_1) / (((double) vfsr_par.DELTA_X)
		     * parameter_v * parameter_v + (double) SMALL_double);
	}
    }

    /* calculate off-diagonal curvatures */
    if (curvature_flag == TRUE) {
	VFOR(index_v) {
	    /* skip parms with too small a range or integer parameters
	       index_v */
#if INCLUDE_INTEGER_PARAMETERS
	    if (PARAMETER_RANGE_TOO_SMALL(index_v))
		continue;
#else
	    if (PARAMETER_RANGE_TOO_SMALL(index_v))
		continue;
#endif

	    VFOR(index_vv) {
		/* skip parms with too small range or integer parameters
		   index_vv */
#if INCLUDE_INTEGER_PARAMETERS
		if (PARAMETER_RANGE_TOO_SMALL(index_v))
		    continue;
#else
		if (PARAMETER_RANGE_TOO_SMALL(index_v))
		    continue;
#endif
		/* calculate only the upper diagonal */
		if (index_v <= index_vv)
		    continue;

		/* index_v_vv: row index_v, column index_vv */
		index_v_vv = ROW_COL_INDEX(index_v, index_vv);
		index_vv_v = ROW_COL_INDEX(index_vv, index_v);

		/* save the v_th parameter */
		parameter_v =
		    current_generated_state.parameter[index_v];

		/* save the vv_th parameter */
		parameter_vv =
		    current_generated_state.parameter[index_vv];

		/* generate first sample point */
		current_generated_state.parameter[index_v] =
		    (ONE + (double) vfsr_par.DELTA_X) * parameter_v;

		current_generated_state.parameter[index_vv] =
		    (ONE + (double) vfsr_par.DELTA_X) * parameter_vv;
		current_generated_state.cost =
			(*user_cost_function)(current_generated_state.parameter, parameter_pointers, g_number_parameters, &valid_state_generated_flag);
		new_cost_state_1 = current_generated_state.cost;
		valid_state_generated_flag = TRUE;

		/* restore the v_th parameter */
		current_generated_state.parameter[index_v] =
		    parameter_v;

		/* generate second sample point */
		current_generated_state.cost =
			(*user_cost_function)(current_generated_state.parameter, parameter_pointers, g_number_parameters, &valid_state_generated_flag);
		new_cost_state_2 = current_generated_state.cost;
		valid_state_generated_flag = TRUE;

		/* restore the vv_th parameter */
		current_generated_state.parameter[index_vv] =
		    parameter_vv;

		/* generate third sample point */
		current_generated_state.parameter[index_v] =
		    (ONE + (double) vfsr_par.DELTA_X)
		    * best_generated_state.parameter[index_v];
		current_generated_state.cost =
			(*user_cost_function)(current_generated_state.parameter, parameter_pointers, g_number_parameters, &valid_state_generated_flag);
		new_cost_state_3 = current_generated_state.cost;
		valid_state_generated_flag = TRUE;

		/* restore the v_th parameter */
		current_generated_state.parameter[index_v] =
		    parameter_v;

		/* calculate and store the curvature */
		curvature[index_vv_v] =
		    curvature[index_v_vv] =
		    (new_cost_state_1 - new_cost_state_2
		     - new_cost_state_3 + recent_best_cost)
		    / (((double) vfsr_par.DELTA_X) * ((double) vfsr_par.DELTA_X)
		       * parameter_v * parameter_vv
		       + (double) SMALL_double);
	    }
	}
    }
    /* restore the best cost function value */
    current_generated_state.cost = recent_best_cost;

    /* find the maximum |tangent| from all tangents */
    maximum_tangent = 0;
    VFOR(index_v) {
	/* ignore too small ranges and integer parameters types */

	if (PARAMETER_RANGE_TOO_SMALL(index_v))
	    continue;


	/* find the maximum |tangent| (from all tangents) */
	if (fabs(tangents[index_v]) > maximum_tangent) {
	    maximum_tangent = fabs(tangents[index_v]);
	}
    }

}

#ifdef VFSR_PRINT1
/***********************************************************************
* Author: Lester Ingber, Bruce Rosen (copyright) (c)
* Date	 5 Nov 92
* Procedure: print_state
* Parameters:
* Inputs: None
* Outputs: None
* Global Variables:
* Returns: None
* Calls:
* Description:
*	Prints a description of the current state of the system
* Local Variables:
*	int index variables index_v, index_vv, index_v_vv
* Portability:
* Other:
***********************************************************************/
 /* print the state of the system to the user */

void print_state(HWND *hProgressBar)
{
	int index_v, index_vv, index_v_vv;



	if (hProgressBar)
	{
		//progres bar forward
		SendMessage(hProgressBar[number_all_parameters*2+1], PBM_SETPOS, (WPARAM)number_accepted, 0);
		//set the whole banch of progress bars
		VFOR(index_v)
		{
			//I
			int value = (int)(fabs(best_generated_state.parameter[index_v] / (parameter_maximum[index_v] - parameter_minimum[index_v])) * 100);
			SendMessage(hProgressBar[index_v * 2], PBM_SETPOS, (WPARAM)value, 0);// progress
			//II
			double curent_parameter_temp = log(current_parameter_temperature[index_v]);
			if (curent_parameter_temp < -33.) curent_parameter_temp = -33.;
			value = (int)(100. + curent_parameter_temp * 3);
			SendMessage(hProgressBar[index_v * 2 + 1], PBM_SETPOS, (WPARAM)value, 0);// progress
		}
	}
	//for tests
	//Sleep(50);

	fprintf(ptr_vfsr_out, "\n");

	fprintf(ptr_vfsr_out,
		"index_cost_acceptances = %ld, current_cost_temperature = %10.5g\n",
		index_cost_acceptances, current_cost_temperature);

	fprintf(ptr_vfsr_out,
		"accepted_to_generated_ratio = %10.5g,\
		 number_invalid_generated_states = %ld\n",
		 accepted_to_generated_ratio,
		 number_invalid_generated_states);

	fprintf(ptr_vfsr_out,
		"number_generated = %ld, number_accepted = %ld\n",
		number_generated, number_accepted);

	fprintf(ptr_vfsr_out,
		"best_generated_state.cost = %10.5g,\
		 last_saved_state.cost = %10.5g\n",
		 best_generated_state.cost, last_saved_state.cost);

	/* Note that tangents will not be calculated until reanneal is called,
	   and therefore their listing in the printout only is relevant then */

	VFOR(index_v) {
		/* ignore too small ranges */
		if (PARAMETER_RANGE_TOO_SMALL(index_v))
			continue;
		fprintf(ptr_vfsr_out,
			"best_generated_state.parameter[%d] = %10.5g\n\
					current_parameter_temperature[%d] = %10.5g\n\
							tangents[%d]: %10.5g\n",
							index_v, best_generated_state.parameter[index_v],
							index_v, current_parameter_temperature[index_v],
							index_v, tangents[index_v]);
	}

	if (curvature_flag == TRUE) {
		/* print curvatures */
		VFOR(index_v) {
			/* ignore too small ranges */
			if (PARAMETER_RANGE_TOO_SMALL(index_v))
				continue;
			VFOR(index_vv) {
				/* ignore too small ranges (index_vv) */
				if (PARAMETER_RANGE_TOO_SMALL(index_vv))
					continue;
				/* only print upper diagonal of matrix */
				if (index_v < index_vv)
					continue;

				/* index_v_vv: row index_v, column index_vv */
				index_v_vv = ROW_COL_INDEX(index_v, index_vv);

				fprintf(ptr_vfsr_out, "curvature[%d][%d] = %10.5g\n",
					index_v, index_vv, curvature[index_v_vv]);
			}
		}
	}
	fprintf(ptr_vfsr_out, "\n");
	fflush(ptr_vfsr_out);

}

#endif

