 
/***************************************************************************/
/*      include files                                                      */
/***************************************************************************/
#include "Thread.h"	//function prototypes used by WinRod


class Calc
{
	public:

		Calc(){
			MaxData = 0;
			MemoryAllocateCalc(1);
		}

		//destructor
		~Calc(){
			delete[] Q_PAR_SQR_PIPI16;
			delete[] Q_PERP_SQR_PIPI16;
			delete[] Q_PAR_SQR_PLUS_Q_PERP_SQR_PIPI16;
			delete[] FBULK_SQR;
			delete[] ApproxBetaTableCoef;

			for (int i = 0; i < MaxData; ++i)
			{
				delete[] FAT_RE[i];
				delete[] FAT_IM[i];
				delete[] RE_BULK[i];
				delete[] IM_BULK[i];
				delete[] CostermTable[i];
				delete[] SintermTable[i];
			}	

			delete[] FAT_RE;
			delete[] FAT_IM;
			delete[] RE_BULK;
			delete[] IM_BULK;
			delete[] CostermTable;
			delete[] SintermTable;
		};

		void MemoryAllocateCalc(int max_data = MAXDATA);

		//Function prototypes
		void    f_calc(double h, double k, double l, double atten, int lbragg, bool frac,
			           int nsubsc, int nenergy, double *fbulk, double *fsurf, double *fsum, double *phase, Thread_t* Tdata);
		double  occupancy_profile(double z, double zmean, double sigma, double start_occ);
		void	f_calc_fit_init(Thread_t* Tdata);


		double	 f_calc_fit( Thread_t* Tdata, int index);
		double   rms_calc(double *sum, double *mean, double *rms, Thread_t* Tdata);
		double   q_length(double h, double k, double l, Thread_t* Tdata);
		double   f_atomic(double q_half, int natom, Thread_t* Tdata);
		void     FindLBraggs(Thread_t* Tdata); //Finds the LBragg parameters form bulk unit cell

		double   sqr(double x);


	private:
		//Funcvtion prototypes
		double	 gammln(double xx);
		double	 factrl(int n);
		double   occ_calc(int k, Thread_t* Tdata);
		void     f_bulk(double h, double k, double l, double fat_re[], double fat_im[], double dw[], double atten, double re_bulk[], double im_bulk[], Thread_t* Tdata);

		void	 f_surf(double h, double k, double l, double fat_re[], double fat_im[], double dw_par[], double dw_perp[], double dw[], int cell, double re_surf[], double im_surf[], Thread_t* Tdata);
		void     f_surf(double *dw_par, double *dw_perp, double *dw, int cell, double *re_surf, double *im_surf, Thread_t* Tdata, int index);		
		void	 InitializeApproxBeta(Thread_t* Tdata);
		void     InitializeNumBeta(Thread_t* Tdata);
		
		//LIQUID PROFILE
		void     f_surf_liquid(double l, double fat_re[], double fat_im[], double dw_perp[], double*, double*, Thread_t* Tdata);

		void	 f_bulk_beta(double h, double k, double l, double fat_re[], double fat_im[], double dw[], double re_bulk[], double im_bulk[], Thread_t* Tdata);
		void	 f_bulk_beta(double dw[], double re_bulk[], double im_bulk[], Thread_t* Tdata, int index);
		void     calc_roughness(double *roughness, double h, double k, double l, double lbragg, bool frac, double *fat_re, double *fat_im, double *dw, Thread_t* Tdata);
		void     calc_roughness(double *roughness, Thread_t* Tdata, int index);
		void     next_domain(double hin, double kin, double *hout, double *kout, int ndomain, Thread_t* Tdata);
		void     next_domain( double *hout, double *kout, int ndomain, Thread_t* Tdata, int index);
		double	 fact(double x);


		/* Storage of fixed values to speed up fitting */
		int		 MaxData;								//Number of data points necesary for memory cleaning
		double   *Q_PAR_SQR_PIPI16;						/* Square of length of q_parallel vector of i-th	data point */
		double   *Q_PERP_SQR_PIPI16;					/* Square of length of q_perpendicular vector of	i-th data point */
		double	 *Q_PAR_SQR_PLUS_Q_PERP_SQR_PIPI16;		// (Q_PAR_SQR+Q_PERP_SQR)/PIPI16
		double   *FBULK_SQR;							/* Square of total bulk str. factor */
		double	 *ApproxBetaTableCoef;					//Accelerator for fitting when beta is used

		double   **FAT_RE;								/* Atomic scattering factor Re part for    all atom types of i-th data point */
		double   **FAT_IM;								/* Atomic scattering factor Im part for    all atom types of i-th data point */
		double   **RE_BULK;								/* Real part of bulk structure factor */
		double   **IM_BULK;								/* Imaginary part of bulk str. factor */

		double  ONE_MINUS_BETA;							//Helper to speed up roughness calculations
		double  TablePhaseCoef[MAX_ROUGH_LAYERS];       //Table with phase coeficient for numerical roughnes calculations
		int		Ntermination[MAX_ROUGH_LAYERS];         //Table with phase coeficient for numerical roughnes calculations
		double  **CostermTable;							//Table with phase coeficient for numerical roughnes calculations
		double  **SintermTable;							//Table with phase coeficient for numerical roughnes calculations

};

#ifndef CALC
	extern Calc calc;
#else
	Calc calc;
#undef CALC
#endif
 