
//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: This file contain all the variable necessary in fitting rutine. These one are separated from the rest because they are
//			heavely used by calc and threads. This trick is neccesary to avoid any processor colisions and gives an elegant way to 
//			keep data in one place. Data separation is important because  Window gives only 1MB for one thread of memory for variable
//			and functions for local storage. This can be changed but large memory blocks can slow down the processor calls.

// Writen by Daniel Kaminski 
//
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#pragma once 


 typedef struct tag_thread{
	//
	int NumberOfCores;
	HWND hwnd;
	double Chi_result0, Chi_result1, Chi_result2, Chi_result3;


	//copy of all variables important in fitting rutine
	double  DLAT[6];							// Direct lattice parameters
	double  RLAT[6];							// Reciprocal lattice parameters

	double SCALE;
	double SCALE_ERR;

	double SCALE2;
	double SCALE2_ERR;

	int     NDOMAIN;							// Number of domains 
	double  DOMMAT11[MAXDOM];					// Element 11 of domain transform. matrix 
	double  DOMMAT12[MAXDOM];					// Element 12 of domain transform. matrix
	double  DOMMAT21[MAXDOM];					// Element 21 of domain transform. matrix 
	double  DOMMAT22[MAXDOM];					// Element 22 of domain transform. matrix
	double  DOMOCCUP[MAXDOM];					// Fractional occupation of each domain
	bool     COHERENTDOMAINS;					// Flag denotes whether domains are added coherently or not
	bool     ZEROFRACT;							// Flag denoting whether fractional diffraction indices (created by the transformation)

	int		NSURF;								// Number of atoms in surface unit cell
	double  XSFIT[MAXATOMS];					// Fitted x-positions of surface atoms 
	double  YSFIT[MAXATOMS];					// Fitted y-positions of surface atoms 
	double  ZSFIT[MAXATOMS];					// Fitted z-positions of surface atoms
	int     TS[MAXATOMS];						// Type number of surface atom 
	int     NDWS[MAXATOMS];						// Serial number of surface Debye-Waller parameter along in-plane direction */
	int     NDWS2[MAXATOMS];					// Serial number of surface Debye-Waller parameter along perpendicular direction */

	int     NDWTOT;							    // Total no. of paral. Debye-Waller params
	double	DEBWAL[MAXPAR];
	double	DEBWAL_ERR[MAXPAR];
	int		NDWTOT2;
	double	DEBWAL2[MAXPAR];
	double	DEBWAL2_ERR[MAXPAR];

	int     NBULK;					            // Number of atoms in bulk unit cell
	double  XB[MAXATOMS];						// Normalized x coordinate of bulk atom 
	double  YB[MAXATOMS];						// Normalized y coordinate of bulk atom 
	double  ZB[MAXATOMS];						// Normalized z coordinate of bulk atom
	int     NDWB[MAXATOMS];						// Serial number of bulk Debye-Waller parameter
	int     TB[MAXATOMS];						// Type number of bulk atom

	double  BETA_ERR;
	double  BETA;
	int		ROUGHMODEL;							// Flat to denote roughness model used
	int     NLAYERS;					        // Total number of layers in bulk unit cell	(used to describe surface roughness)

	double  SURFFRAC;
	double  SURFFRAC_ERR;
	double  SURF2FRAC;							// Fraction of reconstructed surface occupied by 2-nd surface unit cell
	int     NSURF2;								// Number of atoms in 2nd surface unit cell


	int     NOCCUP[MAXATOMS];					// Serial number of occupation fraction 
	double	OCCUP[MAXPAR];
	double	OCCUP_ERR[MAXPAR];

	double  F_COEFF[MAXTYPES][9];				// Coefficients for atomic scattering factor of the atom types used
	bool	STRUCFAC;							// Flag denoting whether structure factors or intensities are used 

	double  ATTEN;              /* Attenuation factor in truncation rod */
	int     NTYPES;
	int		NDAT;               /* Number of data points */

#ifdef PROFILE
	bool	LIQUID_PROFILE;     //Calcultae the liquid profile yes/no
	double  OCCFIT[MAXATOMS];	/* Occupancy of surface atoms */
#endif


	double  HDAT[MAXDATA];      /* h-value of data */
	double  KDAT[MAXDATA];      /* k-value of data */
	double  LDAT[MAXDATA];      /* l-values of data */
	double  FDAT[MAXDATA];      /* Structure factors of data */
	double  ERRDAT[MAXDATA];    /* Error in structure factors */
	double  LBR[MAXDATA];


	/* Storage of fixed values to speed up fitting */
	double   Q_PAR[MAXDATA];     /* Length of q_parallel vector of i-th	data point */
	double   Q_PERP[MAXDATA];    /* Length of q_perpendicular vector of	i-th data point */
	double   FAT[MAXDATA][MAXTYPES]; /* Atomic scattering factor for    all atom types of i-th data point */
	double   RE_BULK[MAXDATA][MAXDOM]; /* Real part of bulk structure factor */
	double   IM_BULK[MAXDATA][MAXDOM]; /* Imaginary part of bulk str. factor */
	double   FBULK_SQR[MAXDATA];   /* Square of total bulk str. factor */
}
Thread_t;


#ifndef THREAD
extern Thread_t thread; //Variables not used by threads the registration is in WinRod.cpp
#else
Thread_t thread;
#undef WINROD
#endif