

void	InitKeating(float *AtomRadius);
void	InitialiseKeating();
double  energy_calc(Thread_t* Tdata);
void    get_cartesian(double cart[3], double x, double y, double z, Thread_t* Tdata);
void    fenergy(float a[], float *y, Thread_t* Tdata 	);
void    bond_search(Thread_t* Tdata);
void    angle_search( Thread_t* Tdata );

#define MAXBOND 24
#define MAXANGLE 24


typedef struct tag_keating
{
	#define		VKEATING		1					/* Keating potential */
	#define		VLENNARDJONES	2					/* Lennard-Jones potential */

	bool		KEAT_PLUS_CHI;						/* Flag to denote whether energy is combined with chi^2 minimization */
	int			POTENTIAL;							/* Flag to denote the potential used */
	double		ALPHA_KEAT;							/* Bond-length deformation parameter */
	double		BETA_KEAT;							/* Bond-angle deformation parameter */
	double		ATOM_RAD[MAXTYPES];					/* Atomic radii of the elements involved */
	double		EQU_ANGLE;							/* Equilibrium bond angle */
	bool		IN_KEAT[MAXATOMS];					/* Flag to denote whether atom is part of lattice energy minimization */
	int			NBOND[MAXATOMS];					/* Number of bonds of atoms i */
	int			BOND_LIST[MAXATOMS][MAXBOND];		/* List of bonds */
	int			XBSHIFT[MAXATOMS][MAXBOND];			/* Shift along a1 to get correct bond */
	int			YBSHIFT[MAXATOMS][MAXBOND];			/* Shift along a2 to get correct bond */
	int			NANGLE[MAXATOMS];					/* Number of angles of atom i */
	int			ANGLE_LIST[MAXATOMS][MAXANGLE][2];	/* List of bond angles */
	int			XASHIFT[MAXATOMS][MAXANGLE][2];		/* Shift along a1 to get correct bond angle */
	int			YASHIFT[MAXATOMS][MAXANGLE][2];		/* Shift along a2 to get correct bond angle */
}
Ket;

//Register variables
#ifndef KEATING
	extern Ket ket; 
#else
	Ket ket;
	#undef KEATING
#endif