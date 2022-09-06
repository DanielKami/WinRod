//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: This file contain all the important definitions used by WinRod.
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
//Number of 2D windows
#define TOTAL_2D_WINDOWS 5

// Constant strings
#define WINDOW_2D_INPLANE	L"In-plane"
#define APP_TITLE 			L"3D"
#define WINDOW_TITLE 		L"WinRod"
#define WINDOW_2D_ROD 		L"Rod"
#define WINDOW_CML			L"Command line"
#define WINDOW_ELL_DENS		L"Electron density profile"
#define WINDOW_FOURIER_MAP  L"Contur map"
#define WINDOW_FAST_CHECK   L"Parameter test"
#define WINDOW_CHI2			L"Fitting results"
#define WINROD_INIT_MACRO   L"WinRod_Init.mac"			//Definition of the WinRod macro init
#define ATOMS_INIT_FILE		L"AtomsProperities.txt"		//Definition of the WinRod macro init
#define WINDOW_RESIDUALS	L"Residuals"
#define WINDOW_MuLTiPLOT    L"Multiplot"

#define MAX_WINDOWS_NUMBER  11					//Number of windows which positions on the screen are remembered
#define FIT_TEST_PRINT							//Print intermediate results of asa fitting to the file
#define MAX_MACRO_LINES		1000				//Max number fo readed macro lines
#define LENGHT_STR			1000000				//Buffer for the read files
#define COMMLENGHT			1000				//Maximum command line size
#define COMMAND				100					//Lenght of command instruction
#define MAX_SAVED_STRING	20					//Number of saved strings from command line
#define	HEADER				512					//Maximum lenght of header string
#define	MOLECULENAME		10					//Maximum lenght of molecule name
#define	MAX_STORAGE_FOR_COMBO_RODS 1000			//Max entries in Rods combo from main toolbar
#define	MAX_FILE_NAME		64					//Maximum lenght of file name string

//3D scene
#define PLOT_SCALE_FACTOR	0.48f  				//scale ball size for atoms from the used mesh
#define TOLERANCE			12					//Tolerance in pickup position of atoms in selected mode
#define MAX_MEASUREMENTS	30					//Storage maximum number of measurements
//--------------------------------------------------------------------------------------------------------------------------------
#define MAX_STRING			512
#define MAX_DATA_POINTS		10000
#define MAX_ATOMS			2000				// Maximum number of atoms for Bul and Sur structures
#define MAXMOLECULES		5					// Number of molecules
#define SHADOW_SCENE		1					// Rander scene for shadows
#define NORMAL_SCENE		0					// Rander normal scene 
#define MAX_BONDED_ATOMS    36					// Max number of bonded atoms for bond reconstruction
#define BOND_FACTOR			1.0					//Detect bonds between atoms
#define SHADOW_SIZE			1.8f				//Shadows have to a bit bigger to avoid artefacts of self shadowing objects
#define MAX_LASSO_POINTS    2000				//Number of points to make a lasso in selection method
#define MAXMULTIPLOT        300					//Number of rods in multiplot
#define MAXTHREADS			8					//Maximum number of threads /don't chane it
//--------------------------------------------------------------------------------------------------------------------------------
#define PI					3.1415926536
#define RAD					57.29578
#define PIPI16				157.91367			// 16*PI*PI, used in Debye-Waller
#define deg2rad				1.745329251994330E-2// Degrees to radians
#define rad2deg				1./deg2rad
//--------------------------------------------------------------------------------------------------------------------------------
#define MAXTYPES			50					// Maximum number of element types
#define MAXATOMS			1000//170           // Maximum number of atoms in model for single unit cell
#define MAXLIUID			50					// Total number of atoms in liquid
#define MAXPAR				50		            // Maximum number of theoretical
#define MAXTHEO				5000				// Maximum number of theor. data points
#define MAXDOM				8					// Maximum number of rotational
#define MAXDATA				20000				// Maximum number of data to read
#define MAXFIT				50					// Maximum number of fit parameters
#define NSF					3					// Number of 'single' fitting parameters: SCALE, SCALE2, BETA and SURFFRAC
#define PROFILE									//Add the liquid profile or not
#define MAXENERGIES         10                  //Maximum number of different energies from dat file

//Roughness model
//--------------------------------------------------------------------------------------------------------------------------------
#define APPROXBETA			0					// use approximation for beta roughness
#define NUMBETA				1					// use numberical beta model
#define POISSONROUGH		2					// use poisson roughness
#define GAUSSROUGH			3					// use Gaussian roughness 
#define LINEARROUGH			4					// use linear decay roughness 
#define COSINEROUGH			5					// use cosine decay roughness 
#define TWOLEVEL			6					// use two-level roughness 
#define MAX_ROUGH_LAYERS	100					// It is number of rough layers in numerical roughnes calculations


#define MAXEQU              12					// Maximum # of symmetry-equivalent	reflections

#define CHISQR				1 					// Use chi^2 increase for error estimate
#define COVARIANCE			0					// Use covariance matrix for error estimate



//Dispersion
#define DEGREES_TO_RADIANS		0.0174533e0
#define SECONDS_TO_MICRORADS	4.8481368e0
#define TWO_PI					6.283185e0
#define PLANCK					6.6260755e-34			// Joule-sec
#define H_BAR_MEV				6.5821220e-22			// MeV-sec
#define HC						12398.4244e0			// ev-A
#define BOLTZMANN				1.380658e-23			// Joules/K
#define LIGHT_SPEED				2.99792458e8			// meters/sec
#define ELECTRON_CHARGE			1.60217733e-19			// coulombs
#define ELECTRON_MASS			9.1093897e-31			// kg
#define ELECTRON_ENERGY			0.51099906e0			// MeV
#define ELECTRON_RADIUS			2.81794092e-5
#define FINE_STRUCTURE			7.29735308e0			// 4pi*e_0*e^2/h_bar*c
#define AVOGADRO				6.0221367e23			// atoms/mole
#define AVOGADRO_CC_PER_AA		0.60221367e0 			// atoms-cc/mole-A^3
#define GAS_CONSTANT			1.987216e0				// calories/mole
#define F_TO_MU					4208.031548e0			// Mu in microns
#define ELECTRON_EV_TO_K		0.512316590e0			// k in A^-1
#define BARNS_TO_ELECTRONS		1.43110541e-8 			// barns to f"
#define AU						2.80022e+7
#define KEV_PER_RYD				0.02721e0
#define INV_FINE_STRUCT			1.37036e2               
#define FINE_PI					6.942325				//FINE_PI= 1/(alpha*2*pi**2)


//3D thread section
#define WM_3D					 WM_APP+1
#define UPDATE					100
#define NO_CHANGES				101
#define UPDATE_COMPARE			200
#define CLEAR_MODEL				300
#define DELETE_OBJECTS			400
#define RESIZE					500
#define UPDATE_WINDOWS_SIZE		550