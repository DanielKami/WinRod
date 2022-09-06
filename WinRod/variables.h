
//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header with structure variables which contains all the variables not used by threads
// Notes:   Variables not used by threads to make the thread memory useage a bit smaller.
//			The defoult threads memory in Windows is 1MB (this can be changed but there is no reason).
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

#include "definitions.h"

//--------------------------------------------------------------------------------------------------------------------------------
typedef struct tag_variable
{

	int MODE;								//Working mode of WinRod  0-atomic standard, 1-molecular;
	//New variables
	//Atom displacement parameters
	double		 h, k, l;					//For WinRod sharing values
	double		 h_start, k_start, l_start;	//For WinRod sharing values
	double		 h_end, k_end, l_end;		//For WinRod sharing values
	int			 N_points;
	int			 Energy;

	RECT WindowsRect[MAX_WINDOWS_NUMBER+1];
	bool WindowsVisible[MAX_WINDOWS_NUMBER+1];

	bool PrintTestFitFile;
	int DetailsInFile;
	char NameTestFitFile[MAX_FILE_NAME];

	//Headers
	char FitHeader[HEADER];
	char BulHeader[HEADER];


	char		 ELEMENT[MAXTYPES][3];	// Element symbol of the different atoms specified by TB and TS
	double		 XS[MAXATOMS];			/* Normalized x coordinate of surface atom */
	double		 YS[MAXATOMS];			/* Normalized y coordinate of surface atom */
	double		 ZS[MAXATOMS];			/* Normalized z coordinate of surface atom */
	int			 NXDIS[MAXATOMS];		/* Serial number of x-displacement par. */
	double		 XCONST[MAXATOMS];		/* Multiplication factor of x-displacement	parameter */
	double		 X2CONST[MAXATOMS];		/* Multiplication factor of 2nd x-displacement parameter */
	int			 NX2DIS[MAXATOMS];		/* Serial number of 2nd x-displacement par. */
	double		 YCONST[MAXATOMS];		/* Multiplication factor of y-displacement parameter */
	int			 NYDIS[MAXATOMS];		/* Serial number of y-displacement par. */
	double		 Y2CONST[MAXATOMS];		/* Multiplication factor of 2nd y-displacement parameter */
	int			 NY2DIS[MAXATOMS];		/* Serial number of 2nd y-displacement par. */
	int			 NZDIS[MAXATOMS];		/* Serial number of z-displacement par. */
	double		 ZCONST[MAXATOMS];		/* Multiplication factor of z-displacement	parameter */
	double		 Z2CONST[MAXATOMS];		/* Multiplication factor of 2nd x-displacement parameter */
	int			 NZ2DIS[MAXATOMS];		/* Serial number of 2nd x-displacement par. */

	int			 NDISTOT;				/* Total number of displacement parameters */
	int			 NSURFTOT;				/* Total # of surface atoms NSURF+NSURF2 */
	int			 NOCCTOT;				/* Total number of occ. parameters */
	int			 NOCCTOT2;				/* Total number of occ.2 parameters */
	int			 NFACTOR;				/* Total number of FACTOR parameters */
	int			 ENERGYTOT;				/* Total number of X-ray energy values used */

	double		SCALELIM[2];
	bool		SCALEPEN;

	double		SUBSCALELIM[MAXPAR][2];					/* Fit limits for subscale parameters */
	double		SUBSCALEFIT[MAXPAR];					/* Flag to denote whether parameter is fitted */
	int			NSUBSCALETOT;							/* Total number of subscale parameters */
	bool		SUBSCALEPEN[MAXPAR];

	double		BETALIM[2];
	bool		BETAPEN;

	double		SURFFRACLIM[2];
	bool		SURFFRACPEN;
	double		SURF2FRACLIM[2];
	bool		SURF2FRACPEN;


	double		DISPL[MAXPAR];
	double		DISPL_ERR[MAXPAR];
	double		DISPLLIM[MAXPAR][2];
	bool		DISPLPEN[MAXPAR];

	double		DEBWALLIM[MAXPAR][2];
	bool		DEBWALPEN[MAXPAR];

	double		DEBWAL2LIM[MAXPAR][2];
	bool		DEBWAL2PEN[MAXPAR];

	double		OCCUPLIM[MAXPAR][2];
	bool		OCCUPPEN[MAXPAR];
	double		OCCUP2LIM[MAXPAR][2];
	bool		OCCUP2PEN[MAXPAR];

	double		FACTOR_LIM[MAXPAR][2];
	bool		FACTOR_PEN[MAXPAR];

	int			NROTATIONX[MAXATOMS];						/* Serial number of rotation parameters x */
	double		ROTATIONX[MAXPAR];							/* Rotation parameters x */
	double		ROTATIONLIMX[MAXPAR][2];					/* Fit limits for rotation parameters x*/
	int			ROTATIONTOTX;								/* Total number of rotation parameters x*/
	bool		ROTATIONPENX[MAXPAR];						/* Flag to denote whether parameter is fitted */
	double		ROTATION_ERRX[MAXPAR];

	int			NROTATIONY[MAXATOMS];						/* Serial number of rotation parameters y */
	double		ROTATIONY[MAXPAR];
	double		ROTATIONLIMY[MAXPAR][2];					/* Fit limits for rotation parameters y*/
	int			ROTATIONTOTY;								/* Total number of rotation parameters y*/
	bool		ROTATIONPENY[MAXPAR];						/* Flag to denote whether parameter is fitted */
	double		ROTATION_ERRY[MAXPAR];

	int			NROTATIONZ[MAXATOMS];						/* Serial number of rotation parameters z */
	double		ROTATIONZ[MAXPAR];
	double		ROTATIONLIMZ[MAXPAR][2];					/* Fit limits for rotation parameters z*/
	int			ROTATIONTOTZ;								/* Total number of rotation parameters z*/
	bool		ROTATIONPENZ[MAXPAR];						/* Flag to denote whether parameter is fitted */
	double		ROTATION_ERRZ[MAXPAR];

	bool		DOMOCCUPAUTO;								//Flag to denote if domain occupancy have to be fitted automatically
	double		DOMOCCUPLIM[MAXDOM][2];					    /* Fit limits for domain occupancy parameters*/
	bool		DOMOCCUPPEN[MAXDOM];						/* Flag to denote whether parameter is fitted */
	double		DOMOCCUP_ERR[MAXDOM];						//Error of domain occupancy
	bool		DOMOCCUP2PEN[MAXDOM];						/* Flag to denote whether parameter is fitted */
	double		DOMOCCUP2_ERR[MAXDOM];						//Error of domain occupancy

	double		z_mean;
	double		z_meanLIM[2];
	bool		z_meanPEN;

	double		sigma;
	double		sigmaLIM[2];
	bool		sigmaPEN;

	double		ERRDAT[MAXDATA];    /* Error in structure factors */
	double		COPYERRDAT[MAXDATA];    /* Error in structure factors */

	bool		DOMEQUAL;		//Flag denoting whether all domains have
	bool		FRACTIONAL;
	int			NTH;                /* Number of theoretical points */

//Variables important for fourier maps
	char		PLANEGROUP[5];			//Plane group name
	double	    TRANS[MAXEQU+1][2][2];    /* The transformation matrices between symmetry-equivalent reflections */
    int         NTRANS;                 /* Number of matrices for the space group */

	struct
    {
    int     H[MAXEQU+1];          /* Diffraction index h of symmetry set */
    int     K[MAXEQU+1];          /* Diffraction index k of symmetry set */
    int     N;                  /* # of symmetry equivalents */
    }
    SYMSET[MAXDATA];            /* List of symmetry-equivalent reflections
				associated with input structure factor set */

	//Variables for electron density profile
	double ell_dens_h, ell_dens_k;

	int		LSQ_ERRORCALC;//  CHISQR/COVARIANCE	- Flag to denote type of error calculation


	bool BRAGG_WEIGHT_DONE;

	//Multiplot
	int NumberOfMultiplots;
	int Multi_N_column;
	int Multi_L_points;
	bool Multi_SelectPlot[MAXMULTIPLOT+1];


	//Speed up constants
	//double Dlat1cosDlat5;
	//double Dlat2cosDlat3;
	//double Dlat1sinDlat5;
	//double Dlat2cosDlat4;

	double a_sin_alp;
	double b_cos_gam;
	double c_cos_bet__sin_alp;

	double b_sin_Gam;
	double c_cos_alp;
	double c_sin_bet__sin_alp;

	//Data reduction
	int reduction_step;
	double reduction_minF;
	double reduction_MaxL;
}
Variable_t;


//Register variables
#ifndef VARIABLES
	extern Variable_t variable; 
#else
	Variable_t variable;
	#undef WINROD
#endif