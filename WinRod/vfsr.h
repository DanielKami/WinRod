//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /* essential DEFINES and MACROS */
#define MAXIMUM_EXPONENT ((double) 2.302585/((double) SMALL_double))

 /* macros */

 /* VFOR
    is a simple macro to iterate on each parameter index. */
#define VFOR(index_v) \
for (index_v = 0; index_v < number_all_parameters; ++index_v)

 /* EXPONENT_CHECK
    checks that an exponent x is within a valid range and,
    if not, reduces its magnitude to fit in the range. */
#define EXPONENT_CHECK(x) \
 (fabs(x) < MAXIMUM_EXPONENT ? x : ((x * MAXIMUM_EXPONENT)/fabs(x)))

 /* PARAMETER_RANGE_TOO_SMALL(x)
    checks if the range of parameter x is too small to work with
    If user_cost_function were modified to pass parameter ranges,
    this test could be (and has been) used to adaptively bypass
    some parameters, e.g., depending on constraints. */
#define PARAMETER_RANGE_TOO_SMALL(x) \
 (fabs(parameter_minimum[x] - parameter_maximum[x]) < (double) SMALL_double)

 /* ROW_COL_INDEX (i, j)
    converts from row i, column j to an index. */
#define ROW_COL_INDEX(i, j) (i + number_all_parameters * j)

 /* The State of the system, its parameters and their resulting function
    value */
typedef struct
    {
    double cost;
    double *parameter;
    }
    STATE;

 /* The 3 states that are kept track of during the annealing process */
STATE current_generated_state, last_saved_state, best_generated_state;

 /* The array of parameter bounds */
double *parameter_minimum, *parameter_maximum;

 /* The array of tangents (absolute value of the numerical derivatives),
    and the maximum |tangent| of the array */
double *tangents, maximum_tangent;

 /* The parameter curvature/covariance about the best state */
double *curvature;

 /* ratio of acceptances to generated points - determines when to
    test/reanneal */
double accepted_to_generated_ratio;

 /* temperature parameters */
double temperature_scale, temperature_scale_parameters;
 /* relative scalings of cost and parameters to temperature_scale */
double temperature_scale_cost;
double *current_parameter_temperature, *initial_parameter_temperature;
double current_cost_temperature, initial_cost_temperature;
double temperature_rescale_power;	/* used when applying REANNEAL_RESCALE */
double log_new_temperature_ratio;	/* current temp = initial temp *
					   exp(log_new_temperature_ratio) */
double one_over_number_parameters;	/* 1/number_parameters */
int index_exit_v;		/* information for vfsr_exit */

 /* flag to determine if curvatures should be calculated */
int curvature_flag, number_parameters, number_all_parameters;

 /* counts of generated states and acceptances */
long int *index_parameter_generations;
long int number_generated, best_number_generated_saved;
long int recent_number_generated, number_accepted;
long int recent_number_acceptances, index_cost_acceptances;
long int number_acceptances_saved, best_number_accepted_saved;

/* Flag indicates that the parameters generated were
   invalid according to the cost function validity criteria. */
int valid_state_generated_flag;
long int number_invalid_generated_states;
//long int steps_print;
/* parameter type is real or integer */
int *parameter_type;




