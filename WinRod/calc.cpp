/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*$$$$$                         calc.c                                $$$$$*/
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/****************************************************************************

  CVS information:

  $Id: calc.c,v 1.3 2000/12/21 10:34:21 svensson Exp $

****************************************************************************/

/***************************************************************************

changes:

Update 05/07/2001 E. Vlieg (vlieg@sci.kun.nl)
                  Changed menus to follow Rainer's scheme.
                  Replaced "Miller index" by the correct term 
                  "diffraction index"

    30/10/91:	Structure factors of bulk are averaged over domains
				if necessary.
    11/03/93:   Poisson bulk roughness model
    15/03/93:   Gauss bulk roughness model
    20/12/94:	Also coherent addition of rotational domains possible.
    09/02/95:	All roughness models applicable to both bulk and surface
				unit cells. Approxiamated beta and beta model still as they
				were.
    05/10/00:   E. Vlieg
				Correct ZEROFRAC flag (name should be changed as well).

	01.09.2014	 All the changes are implemented like in Prof. Elias Vlieg version except organic molecule support which is in Import file in case of WinRod.	

	12/02/2013	D. Kaminski
				- All calc functions wrapped in class Calc.
				- All variables are divided on thread important and variables (not used directly by calc and thus by threads). This is important for multithreading calculations. In this way we avoid processor collisions.
				- All functions have added Thread_t* Tdata. In this way functions have acces to the data by the structure Thread_t (important for parallel calculations) and additionally the variables are nicely closed in structure not in the scope of the whole program (more object oriented).
				- Added overloaded function double	f_calc_fit( Thread_t* Tdata, int index) with thread data and enumerator to speed up the fitting process a bit.
				- New memory menagement
				-All functions optymalization


***************************************************************************/

/***************************************************************************/
/*      include files                                                      */
/***************************************************************************/
#include <stdafx.h>

#define CALC
#include <math.h>
#include "definitions.h"
#include "calc.h"
#include "structures.h"
#include "WinBase.h"


#pragma optimize("t", on)

void Calc::MemoryAllocateCalc(int max_data){

	if (MaxData == max_data) return;

	//Clean if allocated
	if (Q_PAR_SQR_PIPI16) delete[] Q_PAR_SQR_PIPI16;
	if (Q_PERP_SQR_PIPI16) delete[] Q_PERP_SQR_PIPI16;
	if (Q_PAR_SQR_PLUS_Q_PERP_SQR_PIPI16) delete[] Q_PAR_SQR_PLUS_Q_PERP_SQR_PIPI16;
	if (FBULK_SQR) delete[] FBULK_SQR;
	if (ApproxBetaTableCoef) delete[] ApproxBetaTableCoef;

	if (FAT_RE)
	{
		for (int i = 0; i < MaxData; ++i)
			if (FAT_RE[i])
			{
			delete[] FAT_RE[i];
			FAT_RE[i] = NULL;
			}
		delete[] FAT_RE;
		FAT_RE = NULL;
	}

	if (FAT_IM)
	{
		for (int i = 0; i < MaxData; ++i)
			if (FAT_IM[i])
			{
			delete[] FAT_IM[i];
			FAT_IM[i] = NULL;
			}
		delete[] FAT_IM;
		FAT_IM = NULL;
	}

	if (RE_BULK)
	{
		for (int i = 0; i < MaxData; ++i)
			if (RE_BULK[i])
			{
			delete[] RE_BULK[i];
			RE_BULK[i] = NULL;
			}
		delete[] RE_BULK;
		RE_BULK = NULL;
	}

	if (IM_BULK)
	{
		for (int i = 0; i < MaxData; ++i)
			if (IM_BULK[i])
			{
			delete[] IM_BULK[i];
			IM_BULK[i] = NULL;
			}
		delete[] IM_BULK;
		IM_BULK = NULL;
	}

	if (CostermTable)
	{
		for (int i = 0; i < MaxData; ++i)
			if (CostermTable[i])
			{
			delete[] CostermTable[i];
			CostermTable[i] = NULL;
			}
		delete[] CostermTable;
		CostermTable = NULL;
	}

	if (SintermTable)
	{
		for (int i = 0; i < MaxData; ++i)
			if (SintermTable[i])
			{
			delete[] SintermTable[i];
			SintermTable[i] = NULL;
			}
		delete[] SintermTable;
		SintermTable = NULL;
	}

	//Alocate
	Q_PAR_SQR_PIPI16 = new double[max_data + 1];
	Q_PERP_SQR_PIPI16 = new double[max_data + 1];
	Q_PAR_SQR_PLUS_Q_PERP_SQR_PIPI16 = new double[max_data + 1];
	FBULK_SQR = new double[max_data + 1];
	ApproxBetaTableCoef = new double[max_data + 1];

	FAT_RE = new double*[max_data + 1];
	FAT_IM = new double*[max_data + 1];
	RE_BULK = new double*[max_data + 1];
	IM_BULK = new double*[max_data + 1];
	CostermTable = new double*[max_data + 1];
	SintermTable = new double*[max_data + 1];
	for (int i = 0; i < max_data + 1; ++i)
	{
		FAT_RE[i] = new double[MAXTYPES];
		FAT_IM[i] = new double[MAXTYPES];
		RE_BULK[i] = new double[MAXDOM];
		IM_BULK[i] = new double[MAXDOM];
		CostermTable[i] = new double[MAX_ROUGH_LAYERS];
		SintermTable[i] = new double[MAX_ROUGH_LAYERS];
	}

	MaxData = max_data;
}

/***************************************************************************/
double Calc::f_atomic(double q_half, int natom, Thread_t* Tdata)
/***************************************************************************/

/*
Compute atomic scattering factor for atom 'natom'.
*/

{

	double coef = sqr(0.5 / PI * q_half );
	double f = Tdata->F_COEFF[natom][8];

	for (int i = 0; i < 4; ++i)
		f += Tdata->F_COEFF[natom][i * 2] *
			exp(-Tdata->F_COEFF[natom][i * 2 + 1] * coef);
	
	return(f);

}



/***************************************************************************/
void    Calc::f_bulk(double h, double k, double l, double fat_re[], double fat_im[],
					 double dw[], double atten, double re_bulk[], double im_bulk[], Thread_t* Tdata)
	/***************************************************************************/

	/*
	Calculate real and imaginary parts of the structure factor of the
	bulk unit cell for all domains present.
	*/

{

	int	i, j;
	double	hnew, knew, re_bulk1, im_bulk1, factor, phase, sinterm, costerm, temp;
	double temp_cos, temp_sin;
	double exp_temp;

	factor = sqr(1. - exp(-atten)) + 4.*exp(-atten)*sqr(sin(PI*l));
	if (factor < 1e-20) factor = 1e-20;

	exp_temp = exp(-atten);
	temp_cos = exp_temp*cos(2.*PI*l);
	temp_sin = exp_temp*sin(2.*PI*l);

	for (j = 0; j < Tdata->NDOMAIN; ++j)
	{
		re_bulk[j] = im_bulk[j] = 0.;
		next_domain(h, k, &hnew, &knew, j, Tdata);	/* Next equivalent refl. */
		if (!(!Tdata->BULK_ZEROFRACT | !Tdata->ZEROFRACT && ((fmod(hnew, 1.) != 0) || (fmod(knew, 1.) != 0))))
		{
			re_bulk1 = im_bulk1 = 0.;
			for (i = 0; i < Tdata->NBULK; ++i)
			{
				/* the exp is added to fatom in order to avoid numerical
				   problems (suggestion by Yves Garreau */
				phase = 2.*PI*(hnew*Tdata->XB[i] + knew*Tdata->YB[i] + l*Tdata->ZB[i]);
				temp = dw[Tdata->NDWB[i]] * exp(atten*Tdata->ZB[i]);

				costerm = cos(phase)*temp;
				sinterm = sin(phase)*temp;
				re_bulk1 += (fat_re[Tdata->TB[i]] * costerm - fat_im[Tdata->TB[i]] * sinterm);
				im_bulk1 += (fat_re[Tdata->TB[i]] * sinterm + fat_im[Tdata->TB[i]] * costerm);

			}

			re_bulk[j] = ((1. - temp_cos) * re_bulk1 + temp_sin * im_bulk1) / factor;
			im_bulk[j] = ((1. - temp_cos) * im_bulk1 - temp_sin * re_bulk1) / factor;
		}
		else
		{
			re_bulk[j] = 0;
			im_bulk[j] = 0;
		}
	}
}



/***************************************************************************/
void    Calc::calc_roughness(double *roughness, double h, double k, double l, double lbragg, bool frac, double *fat_re, double *fat_im, double *dw, Thread_t* Tdata)
/***************************************************************************/

/*
Calculate reduction in intensity due to roughness. Various models are
available, whose effects can always be expressed as a multiplication
factor of the calculated intensity. All models use a column
approximation, i.e., each level terminates in an identical fashion.
The programme does not deal with phase factors too carefully in these
calculations, so be warned!

The 'approximate beta' is very quick, but requires as input the
diffraction index of the nearest Bragg peak.
The other models are numerical and thus slower. They calculate the
phase factor between layers from the bulk unit cell. The atoms in the
bulk unit cell need to be in the correct order: ranging from top to
bottom.
*/
{
	int    i, j, m, layer;
	double occ, occ_lower;

	double im_total, re_total, phase;
	double rough, sinterm, costerm, zshift;
	double re_bulk[MAXDOM], im_bulk[MAXDOM];
	double hnew, knew;
	double temp;
	
	ONE_MINUS_BETA = 1. - Tdata->BETA;  //accelerator


	if (Tdata->ROUGHMODEL == APPROXBETA)
	{
		if (!frac)
			rough = ONE_MINUS_BETA /sqrt(sqr(ONE_MINUS_BETA) + 4. * Tdata->BETA*sqr(sin(PI*(l - lbragg) / Tdata->NLAYERS)));
		else /* for fractional-orders no l-dependence */
			rough = sqrt((ONE_MINUS_BETA) / (1. + Tdata->BETA));
		for (m = 0; m < Tdata->NDOMAIN; ++m)
			roughness[m] = rough;
	}
	else
	{
		if (Tdata->NBULK == 0)
			for (m = 0; m < Tdata->NDOMAIN; ++m)
				roughness[m] = 1.;
		else
		{
			if (!frac)
			{

				/* lots of things to do to make sure we include the option that the
				amplitude varies per termination... */

				/* calculation has to be done for each separate domain */
				temp = Tdata->NLAYERS / Tdata->NBULK;
				InitializeNumBeta(Tdata);

				for (m = 0; m < Tdata->NDOMAIN; ++m)
				{
					next_domain(h, k, &hnew, &knew, m, Tdata);	/* Next equivalent refl. */

					/* calculate bulk structure factor for each possible
					termination */

					/* Not clear of all details, like dw[] and fat_im[], are needed:
					worth checking later */

					for (j = 0; j < Tdata->NLAYERS; ++j)
					{
						re_bulk[j] = im_bulk[j] = 0.;
						for (i = 0; i < Tdata->NBULK; ++i)
						{
							if (j >= (Tdata->NLAYERS - i * temp))
								zshift = 1.;
							else
								zshift = 0.;

							phase = 2. * PI * (hnew * Tdata->XB[i] + knew * Tdata->YB[i] + l * (Tdata->ZB[i] + zshift));
							costerm = cos(phase);// *dw[Tdata->NDWB[i]];
							sinterm = sin(phase);// *dw[Tdata->NDWB[i]];
							re_bulk[j] += (fat_re[Tdata->TB[i]] * costerm - fat_im[Tdata->TB[i]] * sinterm);
							im_bulk[j] += (fat_re[Tdata->TB[i]] * sinterm + fat_im[Tdata->TB[i]] * costerm);

						}
					}

					/* sum over all rough layers */
					int ntermination;
					re_total = im_total = 0.;
					occ = 1.;
					for (layer = 0; (layer < MAX_ROUGH_LAYERS) && (occ > 1e-5); ++layer)
					{
						occ = occ_calc(layer, Tdata);									
						phase = TablePhaseCoef[layer] * l;
				
						ntermination = (int)fmod((double)layer, Tdata->NLAYERS);
						costerm = cos(phase);
						sinterm = sin(phase);
						
						re_total += occ*(re_bulk[Ntermination[layer]] * costerm - im_bulk[Ntermination[layer]] * sinterm);
						im_total += occ*(re_bulk[Ntermination[layer]] * sinterm + im_bulk[Ntermination[layer]] * costerm);
					}

					/* normalize roughness on amplitude of flat surface with normal
					termination */

					roughness[m] = sqrt((sqr(re_total) + sqr(im_total)) / (sqr(re_bulk[0]) + sqr(im_bulk[0]) + 1e-10));
				}
			}
			else /* fractional order */
			{
				/* inhorent sum over all visible surface layers */

				re_total = 0.;
				occ = 1.;
				occ_lower = occ_calc(0, Tdata);
				for (layer = 1; (layer < 100) && (occ > 1e-5); ++layer)
				{
					occ = occ_calc(layer, Tdata);
					re_total += sqr(occ_lower - occ);
					occ_lower = occ;
				}
				for (m = 0; m < Tdata->NDOMAIN; ++m)
					roughness[m] = sqrt(re_total);
			}
		}
	}
}

//Preapre the table with data for approximate beta model to calculate it before the main fitting loop
void Calc::InitializeApproxBeta(Thread_t* Tdata){

	for (int i = 0; i < Tdata->NDAT; ++i)
		ApproxBetaTableCoef[i] = 4.0 * sqr(sin(PI * (Tdata->LDAT[i] - Tdata->LBR[i]) / Tdata->NLAYERS));
}

//Preapre the table with data for numerical beta model to calculate it before the main fitting loop
void Calc::InitializeNumBeta(Thread_t* Tdata){

	double phase;
	
	for (int layer = 0; layer < MAX_ROUGH_LAYERS; ++layer)
	{
		TablePhaseCoef[layer] = 2 * PI * (layer / Tdata->NLAYERS);
		Ntermination[layer] = (int)fmod((double)layer, Tdata->NLAYERS);

		for (int index = 0; index < Tdata->NDAT; ++index)
		{

			phase = TablePhaseCoef[layer] * Tdata->LDAT[index];
			CostermTable[index][layer] = cos(phase);
			SintermTable[index][layer] = sin(phase);
		}
	}
}

//The fast version of the overloaded calc_roughness function
/***************************************************************************/
void   Calc::calc_roughness(double *roughness, Thread_t* Tdata, int index){
	/***************************************************************************/

	/*
	Calculate reduction in intensity due to roughness. Various models are
	available, whose effects can always be expressed as a multiplication
	factor of the calculated intensity. All models use a column
	approximation, i.e., each level terminates in an identical fashion.
	The programme does not deal with phase factors too carefully in these
	calculations, so be warned!

	The 'approximate beta' is very quick, but requires as input the
	diffraction index of the nearest Bragg peak.
	The other models are numerical and thus slower. They calculate the
	phase factor between layers from the bulk unit cell. The atoms in the
	bulk unit cell need to be in the correct order: ranging from top to
	bottom.

	*/
	int     i, j, m, layer;
	double	occ, occ_lower;

	double  im_total, re_total, phase;
	double rough, sinterm, costerm, zshift;
	double re_bulk[MAXDOM], im_bulk[MAXDOM];
	double hnew, knew;
	double temp;

	ONE_MINUS_BETA = 1. - Tdata->BETA;

	if (Tdata->ROUGHMODEL == APPROXBETA)
	{

		if (!Tdata->FRAC[index])
		{		
			//roughness = temp / sqrt(sqr(temp) + 4.0 * Tdata->BETA * sqr(sin(PI * (Tdata->LDAT[index] - Tdata->LBR[index]) / Tdata->NLAYERS)));
			rough = ONE_MINUS_BETA / sqrt(sqr(ONE_MINUS_BETA) + Tdata->BETA * ApproxBetaTableCoef[index]);
		}
		else /* for fractional-orders no l-dependence */
			rough = sqrt((ONE_MINUS_BETA) / (1. + Tdata->BETA));

		for (m = 0; m < Tdata->NDOMAIN; ++m)
			roughness[m] = rough;
	}
	else
		if (!Tdata->FRAC[index])
		{

		/* lots of things to do to make sure we include the option that the
		amplitude varies per termination... */

		/* calculation has to be done for each separate domain */
		temp = Tdata->NLAYERS / Tdata->NBULK;

		for (m = 0; m < Tdata->NDOMAIN; ++m)
		{
			next_domain(Tdata->HDAT[index], Tdata->KDAT[index], &hnew, &knew, m, Tdata);	/* Next equivalent refl. */

			/* calculate bulk structure factor for each possible
			termination */

			/* Not clear of all details, like dw[] and fat_im[], are needed:
			worth checking later */

			for (j = 0; j < Tdata->NLAYERS; ++j)
			{
				re_bulk[j] = im_bulk[j] = 0.;
				for (i = 0; i < Tdata->NBULK; ++i)
				{
					if (j >= (Tdata->NLAYERS - i * temp))
						zshift = 1.;
					else
						zshift = 0.;

					phase = 2. * PI*(hnew*Tdata->XB[i] + knew*Tdata->YB[i] + Tdata->LDAT[index] * (Tdata->ZB[i] + zshift));
					costerm = cos(phase);// *dw[Tdata->NDWB[i]];
					sinterm = sin(phase);// *dw[Tdata->NDWB[i]];
					re_bulk[j] += (FAT_RE[index][Tdata->TB[i]] * costerm - FAT_IM[index][Tdata->TB[i]] * sinterm);
					im_bulk[j] += (FAT_RE[index][Tdata->TB[i]] * sinterm + FAT_IM[index][Tdata->TB[i]] * costerm);
				}
			}

			/* sum over all rough layers */

			re_total = im_total = 0.;
			occ = 1.;
			for (layer = 0; (layer < MAX_ROUGH_LAYERS) && (occ > 1e-5); ++layer)
			{
				occ = occ_calc(layer, Tdata);
				re_total += occ*(re_bulk[Ntermination[layer]] * CostermTable[index][layer] - im_bulk[Ntermination[layer]] * SintermTable[index][layer]);
				im_total += occ*(re_bulk[Ntermination[layer]] * SintermTable[index][layer] + im_bulk[Ntermination[layer]] * CostermTable[index][layer]);
			}

			/* normalize roughness on amplitude of flat surface with normal
			termination */

			roughness[m] = sqrt((sqr(re_total) + sqr(im_total)) / (sqr(re_bulk[0]) + sqr(im_bulk[0]) + 1e-10));
		}
		}

		else
		{ /* fractional order */
			/* inhorent sum over all visible surface layers */

			re_total = 0.;
			occ = 1.;
			occ_lower = occ_calc(0, Tdata);
			for (layer = 1; (layer < 100) && (occ > 1e-5); ++layer)
			{
				occ = occ_calc(layer, Tdata);
				re_total += sqr(occ_lower - occ);
				occ_lower = occ;
			}
			for (m = 0; m < Tdata->NDOMAIN; ++m)
				roughness[m] = sqrt(re_total);
		}
}



/***************************************************************************/
void    Calc::f_calc(double h, double k, double l, double atten, int lbragg, bool frac,
	int nsubsc, int nenergy,
	double *fbulk, double *fsurf, double *fsum, double *phase, Thread_t* Tdata)
	/***************************************************************************/

	/*
	Compute bulk and surface contributions to rod profile, plus the
	interference sum, including the phase, of these two.
	*/

{
	int i, j;
	double  q, q_par, q_perp;
	double	dw[MAXPAR + 1];   /* Debye-Waller factors. dw[0] contains DW for atoms without serial DW number, i.e. having serial number 0. */
	double	dw_par[MAXPAR + 1];
	double  dw_perp[MAXPAR + 1];
	double	fat_re[MAXTYPES], fat_im[MAXTYPES];  /* Atomic scattering factors */
	double	scale;
	double	im_bulk[MAXDOM], re_bulk[MAXDOM];
	double	im_surf[MAXDOM], re_surf[MAXDOM];
	double	im_surf2[MAXDOM], re_surf2[MAXDOM], fsurf2, fsum1, fsum2;
	double	im_bul, re_bul, im_sur1, re_sur1, im_sur2, re_sur2;
	double  re_dom, im_dom;
	double	fbulk_sqr_save;
	double  roughness[MAXDOM];
	double  qq_temp, q_half;
	double	im_liq, re_liq;

	im_liq = re_liq = 0;

	/* Compute length of in-plane and out-of-plane momentum transfer */
	q_par = q_length(h, k, 0, Tdata);
	q_perp = q_length(0, 0, l, Tdata);
	q = q_length(h, k, l, Tdata);
	q_half = q / 2;

	/* Compute in-plane Debye-Waller factors */
	if (Tdata->NDWTOT2 != 0)
	{
		dw_par[0] = 1.;         /* Unit factor if no parameter given */
		qq_temp = q_par * q_par / PIPI16;

		for (i = 1; i < Tdata->NDWTOT + 1; ++i)
			dw_par[i] = exp(-Tdata->DEBWAL[i - 1] * qq_temp);

	}

	/* Compute out-of-plane Debye-Waller factors */

	dw_perp[0] = 1.;         /* Unit factor if no parameter given */
	qq_temp = q_perp * q_perp / PIPI16;
	for (i = 1; i < Tdata->NDWTOT2 + 1; ++i)
		dw_perp[i] = exp(-Tdata->DEBWAL2[i - 1] * qq_temp);


	/* In case NDWS2[i] = 0, isotropic Debye-Waller parameters are assumed.
	   Then the total momentum transfer is used together with the value of
	   DEBWAL. */

	dw[0] = 1.;            /* Unit factor if no parameter given */
	qq_temp = q * q / PIPI16;

	for (i = 1; i < Tdata->NDWTOT + 1; ++i)
		dw[i] = exp(-Tdata->DEBWAL[i - 1] * qq_temp);


	/* Compute atomic scattering factors */

	for (i = 0; i < Tdata->NTYPES; ++i)
	{
		fat_re[i] = f_atomic(q_half, i, Tdata) + Tdata->F1[i][nenergy];
		fat_im[i] = Tdata->F2[i][nenergy];
	}
	/* Compute intensity reduction due to roughness */

	calc_roughness(roughness, h, k, l, lbragg, frac, fat_re, fat_im, dw, Tdata);

	/* Compute scale factor, including possible subscale */

	if (nsubsc != 0)
		scale = Tdata->SCALE*Tdata->SUBSCALE[nsubsc - 1];
	else
		scale = Tdata->SCALE;


	/* Compute bulk structure factor */

	if (Tdata->NBULK > 0 || (Tdata->INCOMENSURATE && h!=0 && k!=0))//do not compute other rods with bulk contribution than (00) in incommensurate mode
	{
		f_bulk(h, k, l, fat_re, fat_im, dw, atten, re_bulk, im_bulk, Tdata);
		*fbulk = 0;
		for (j = 0; j < Tdata->NDOMAIN; ++j)
			*fbulk += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (sqr(re_bulk[j]) + sqr(im_bulk[j]));
		fbulk_sqr_save = *fbulk;
		*fbulk = scale*sqrt(*fbulk);
	}
	else
	{
		fbulk_sqr_save = *fbulk = 0;
		for (j = 0; j < Tdata->NDOMAIN; ++j)
			re_bulk[j] = im_bulk[j] = 0;
	}

	/* Compute surface structure factor of 1st unit cell for all
	   present domains */

	f_surf(h, k, l, fat_re, fat_im, dw_par, dw_perp, dw, 1, re_surf, im_surf, Tdata);
	*fsurf = 0;
	for (j = 0; j < Tdata->NDOMAIN; ++j)
		*fsurf += Tdata->DOMOCCUP[j] * sqr(roughness[j])* (sqr(re_surf[j]) + sqr(im_surf[j]));
	*fsurf = scale*sqrt(*fsurf*Tdata->SURFFRAC*(1. - Tdata->SURF2FRAC));

	/* Calculate surface structure factor for 2nd surface unit cell */

	if (Tdata->NSURF2 != 0)
	{
		f_surf(h, k, l, fat_re, fat_im, dw_par, dw_perp, dw, 2, re_surf2, im_surf2, Tdata);
		fsurf2 = 0;
		for (j = 0; j < Tdata->NDOMAIN; ++j)
			fsurf2 += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (sqr(re_surf2[j]) + sqr(im_surf2[j]));
		fsurf2 = scale*sqrt(fsurf2*Tdata->SURFFRAC*Tdata->SURF2FRAC);
		*fsurf = sqrt(sqr(*fsurf) + sqr(fsurf2));
	}

	/* Calculate liquid structure factor for 2nd surface unit cell */
	if (Tdata->LIQUID_PROFILE)
		if (h == 0 && k == 0)
			f_surf_liquid(l, fat_re, fat_im, dw_perp, &re_liq, &im_liq, Tdata);

	/* Compute interference sum of bulk and surface. */

	fsum1 = re_dom = im_dom = 0.;

	if (!Tdata->COHERENTDOMAINS)
	{
		for (j = 0; j < Tdata->NDOMAIN; ++j)
			fsum1 += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (sqr(re_bulk[j] + re_surf[j] + re_liq) + sqr(im_bulk[j] + im_surf[j] + im_liq));
	}
	else
	{
		for (j = 0; j < Tdata->NDOMAIN; ++j)
		{
			re_dom += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (re_bulk[j] + re_surf[j] + re_liq);
			im_dom += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (im_bulk[j] + im_surf[j] + im_liq);
		}
		fsum1 = sqr(re_dom) + sqr(im_dom);
	}


	*fsum = scale*sqrt(fsum1*Tdata->SURFFRAC*(1. - Tdata->SURF2FRAC) +
		(1. - Tdata->SURFFRAC)*fbulk_sqr_save);

	/* Add corresponding fraction of 2nd surface unit cell to the result
	   if necessary */

	if (Tdata->NSURF2 != 0)
	{
		fsum2 = re_dom = im_dom = 0.;

			if (!Tdata->COHERENTDOMAINS)
			{
				for (j = 0; j < Tdata->NDOMAIN; ++j)
					fsum2 += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (sqr(re_bulk[j] + re_surf2[j] + re_liq) + sqr(im_bulk[j] + im_surf2[j] + im_liq));
			}
			else
			{
				for (j = 0; j < Tdata->NDOMAIN; ++j)
				{
					re_dom += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (re_bulk[j] + re_surf2[j] + re_liq);
					im_dom += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (im_bulk[j] + im_surf2[j] + im_liq);
				}
				fsum2 = sqr(re_dom) + sqr(im_dom);				
			}

		fsum2 = scale*sqrt(fsum2*Tdata->SURFFRAC*Tdata->SURF2FRAC);
		*fsum = sqrt(sqr(*fsum) + sqr(fsum2));
	}

	/* Calculate the phase factor of the interference sum */

	im_bul = re_bul = 0;
	for (i = 0; i < Tdata->NDOMAIN; ++i)
	{
		im_bul += Tdata->DOMOCCUP[i] * im_bulk[i];
		re_bul += Tdata->DOMOCCUP[i] * re_bulk[i];
	}

	im_sur1 = re_sur1 = 0;
	for (i = 0; i < Tdata->NDOMAIN; ++i)
	{
		im_sur1 += Tdata->DOMOCCUP[i] * im_surf[i];
		re_sur1 += Tdata->DOMOCCUP[i] * re_surf[i];
	}

	im_sur2 = re_sur2 = 0;
	if (Tdata->NSURF2 != 0)
	{
		for (i = 0; i < Tdata->NDOMAIN; ++i)
		{
			im_sur2 += Tdata->DOMOCCUP[i] * im_surf2[i];
			re_sur2 += Tdata->DOMOCCUP[i] * re_surf2[i];
		}
	}

	double temp1 = Tdata->SURFFRAC * (1. - Tdata->SURF2FRAC);
	double temp2 = Tdata->SURFFRAC * Tdata->SURF2FRAC;
	*phase = atan2(
		im_bul + temp1 * im_sur1 + temp2 * im_sur2,
		re_bul + temp1 * re_sur1 + temp2 * re_sur2
		+ 1e-10);

	if (!Tdata->STRUCFAC)
	{
		*fbulk *= *fbulk;
		*fsurf *= *fsurf;
		*fsum *= *fsum;
	}

}



// Overloaded version with only Tdata and enumerator i to save even more calculation time
/***************************************************************************/
double	Calc::f_calc_fit(Thread_t* Tdata, int index){
	/***************************************************************************/
	/*
	Even more special version with inplace variable iteration. modyfication Daniel Kaminski
	Special version of f_calc that stores constant values (in particular
	the bulk structure factors) and is therefore faster when fitting.
	*/

	double	dw[MAXPAR + 1];   /* Debye-Waller factors. dw[0] contains DW
				   for atoms without serial DW number, i.e.  having serial number 0. */
	double	dw_par[MAXPAR + 1], dw_perp[MAXPAR + 1];

	double	scale;
	int 	i, j;
	double  re_dom = 0., im_dom = 0.;
	double	im_surf[MAXDOM], re_surf[MAXDOM];
	double	im_surf2[MAXDOM], re_surf2[MAXDOM];
	double  fsum = 0., fsum1 = 0., fsum2 = 0.;
	double  roughness[MAXDOM];
	double	im_liq, re_liq;

	im_liq = re_liq = 0;

	if (Tdata->NDWTOT2 != 0)
	{
		/* Compute in-plane Debye-Waller factors */
		dw_par[0] = 1.;         /* Unit factor if no parameter given */
		for (i = 1; i < Tdata->NDWTOT + 1; ++i)
		{
			dw_par[i] = exp(-Tdata->DEBWAL[i - 1] * Q_PAR_SQR_PIPI16[index]);   //		dw_par[i] = exp(-Tdata->DEBWAL[i-1]*Q_PAR[index]*Q_PAR[index]/PIPI16);
		}


		/* Compute out-of-plane Debye-Waller factors */
		dw_perp[0] = 1.;         /* Unit factor if no parameter given */
		for (i = 1; i < Tdata->NDWTOT2 + 1; ++i)
		{
			if (Tdata->NDWS2[i - 1] != 0)
				dw_perp[i] = exp(-Tdata->DEBWAL2[i - 1] * Q_PERP_SQR_PIPI16[index]);   //		dw_perp[i] = exp(-Tdata->DEBWAL2[i-1] * Q_PERP[index] * Q_PERP[index] / PIPI16);
		}
	}
	/* In case NDWS2[i] = 0, isotropic Debye-Waller parameters are assumed.
	   Then the total momentum transfer is used together with the value of
	   DEBWAL. */

	dw[0] = 1.;            /* Unit factor if no parameter given */
	for (i = 1; i < Tdata->NDWTOT + 1; ++i)
	{
		dw[i] = exp(Tdata->DEBWAL[i - 1] * Q_PAR_SQR_PLUS_Q_PERP_SQR_PIPI16[index]);  //		dw[i] = exp((Tdata->DEBWAL[i-1])*(Q_PAR[index] * Q_PAR[index] + Q_PERP[index] * Q_PERP[index])/PIPI16 );
	}


	/* Compute intensity reduction due to roughness */

	calc_roughness(roughness, Tdata, index);

	/* Compute scale factor, a negative lbragg means a separate scale  factor */
	if (Tdata->SUBSC[index] != 0)
		scale = Tdata->SCALE*Tdata->SUBSCALE[Tdata->SUBSC[index] - 1];
	else
		scale = Tdata->SCALE;

	/* Compute surface structure factor of 1st unit cell for all
	   present domains */
	f_surf( dw_par, dw_perp, dw, 1, re_surf, im_surf, Tdata, index);

	/* Calculate surface structure factor for 2nd surface unit cell */
	if (Tdata->NSURF2 != 0)
		f_surf( dw_par, dw_perp, dw, 2, re_surf2, im_surf2, Tdata, index);

	/* Calculate liquid structure factor for 2nd surface unit cell */
	if (Tdata->LIQUID_PROFILE)
		if(Tdata->HDAT[index] == 0 && Tdata->KDAT[index] == 0 )
			f_surf_liquid(Tdata->LDAT[index], FAT_RE[index], FAT_IM[index], dw_perp, &re_liq, &im_liq, Tdata);

	/* Compute interference sum of bulk and surface. */
	if (!Tdata->COHERENTDOMAINS)
		for (j = 0; j < Tdata->NDOMAIN; ++j)
			fsum1 += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (sqr(RE_BULK[index][j] + re_surf[j] + re_liq) + sqr(IM_BULK[index][j] + im_surf[j] + im_liq));

	else
	{
		for (j = 0; j < Tdata->NDOMAIN; ++j)
		{
			re_dom += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (RE_BULK[index][j] + re_surf[j] + re_liq);
			im_dom += Tdata->DOMOCCUP[j] * sqr(roughness[j]) * (IM_BULK[index][j] + im_surf[j] + im_liq);
		}
		fsum1 = sqr(re_dom) + sqr(im_dom);
	}

	fsum = scale * sqrt(fsum1 * Tdata->SURFFRAC * (1. - Tdata->SURF2FRAC) +
		(1. - Tdata->SURFFRAC) * FBULK_SQR[index]);

	/* Add corresponding fraction of 2nd surface unit cell to the result
	   if necessary */

	if (Tdata->NSURF2 != 0)
	{
		fsum2 = re_dom = im_dom = 0.;
		if (!Tdata->COHERENTDOMAINS)
			for (j = 0; j < Tdata->NDOMAIN; ++j)
				fsum2 += Tdata->DOMOCCUP[j] * (sqr(RE_BULK[index][j] + re_surf2[j] + re_liq) + sqr(IM_BULK[index][j] + im_surf2[j] + im_liq));

		else
		{
			for (j = 0; j < Tdata->NDOMAIN; ++j)
			{
				re_dom += Tdata->DOMOCCUP[j] * (RE_BULK[index][j] + re_surf2[j] + re_liq);
				im_dom += Tdata->DOMOCCUP[j] * (IM_BULK[index][j] + im_surf2[j] + im_liq);
			}
			fsum2 = sqr(re_dom) + sqr(im_dom);
		}
		fsum2 = scale*sqrt(fsum2*Tdata->SURFFRAC*Tdata->SURF2FRAC);
		fsum = sqrt(sqr(fsum) + sqr(fsum2));
	}

	if (Tdata->STRUCFAC)
		return(fsum);
	else
		return(fsum*fsum);

}


/***************************************************************************/
void	Calc::f_calc_fit_init(Thread_t* Tdata)
/***************************************************************************/

/*
Store constant parameter values to speedup fitting.
*/

{
	int	i, j;
	double	q, dw[MAXPAR + 1], fbulk;
	double qq_temp;

	MemoryAllocateCalc(Tdata->NDAT);


	for (i = 0; i < Tdata->NDAT; ++i)
	{
		Q_PAR_SQR_PIPI16[i] = sqr(q_length(Tdata->HDAT[i], Tdata->KDAT[i], 0, Tdata)) / PIPI16;
		Q_PERP_SQR_PIPI16[i] = sqr(q_length(0, 0, Tdata->LDAT[i], Tdata)) / PIPI16;
		Q_PAR_SQR_PLUS_Q_PERP_SQR_PIPI16[i] = Q_PAR_SQR_PIPI16[i] + Q_PERP_SQR_PIPI16[i];
		/* q = sqrt(Q_PAR[i]*Q_PAR[i]+Q_PERP[i]*Q_PERP[i]); */
		q = q_length(Tdata->HDAT[i], Tdata->KDAT[i], Tdata->LDAT[i], Tdata);

		dw[0] = 1.;            /* Unit factor if no parameter given */
		qq_temp = q*q / PIPI16;
		for (j = 1; j < Tdata->NDWTOT + 1; ++j)
		{
			dw[j] = exp(-Tdata->DEBWAL[j - 1] * qq_temp);
		}

		/* Compute atomic scattering factors */

		for (j = 0; j < Tdata->NTYPES; ++j)
		{
			FAT_RE[i][j] = f_atomic(q / 2., j, Tdata) + Tdata->F1[j][Tdata->ENERGY[i]];
			FAT_IM[i][j] = Tdata->F2[j][Tdata->ENERGY[i]];
		}

		/* Compute bulk structure factor */

		if (Tdata->NBULK > 0)
		{
			f_bulk(Tdata->HDAT[i], Tdata->KDAT[i], Tdata->LDAT[i], FAT_RE[i], FAT_IM[i],
				dw, Tdata->ATTEN, RE_BULK[i], IM_BULK[i], Tdata);
			fbulk = 0;
			for (j = 0; j < Tdata->NDOMAIN; ++j)
				fbulk += sqr(RE_BULK[i][j]) + sqr(IM_BULK[i][j]);
			FBULK_SQR[i] = fbulk / Tdata->NDOMAIN;
		}
		else
		{
			FBULK_SQR[i] = 0;
			for (j = 0; j < Tdata->NDOMAIN; ++j)
				RE_BULK[i][j] = IM_BULK[i][j] = 0;
		}
	}

	if (Tdata->ROUGHMODEL == APPROXBETA)
		InitializeApproxBeta(Tdata);	//storage the roughness coeficients for approxymate beta model
	else
		InitializeNumBeta(Tdata);		//storage the roughness coeficients for num. roughness model
}


/***************************************************************************/
void    Calc::f_surf(double h, double k, double l, double fat_re[], double fat_im[],
	double dw_par[], double dw_perp[], double dw[], int cell,
	double re_surf[], double im_surf[], Thread_t* Tdata)
	/***************************************************************************/

	/*
	Calculate real and imaginary parts of the structure factor of the
	surface unit cell for all domains present. Depending on the value
	of 'cell', this is done for the first or second surface unit cell.
	*/

{

	int	i, j, istart=0, iend=0;
	double	hnew, knew;
	double prefactor, phase, costerm, sinterm;

	if (cell == 1)
	{
		istart = 0;
		iend = Tdata->NSURF;
	}
	else if (cell == 2)
	{
		istart = Tdata->NSURF;
		iend = Tdata->NSURF + Tdata->NSURF2;
	}

	for (j = 0; j < Tdata->NDOMAIN; ++j)
	{
		re_surf[j] = im_surf[j] = 0.;
		next_domain(h, k, &hnew, &knew, j, Tdata); /* Next equivalent diffraction indices */

		/* Don't compute structure factor if the transformed diffraction indices
		   are fractional and the corresponding flag is set */

		if (!(!Tdata->ZEROFRACT && ((fmod(hnew, 1.) != 0) || (fmod(knew, 1.) != 0))))
		{
			for (i = istart; i < iend; ++i)
			{
				if (Tdata->NDWS2[i] != 0)
				{
					prefactor = dw_par[Tdata->NDWS[i]] * dw_perp[Tdata->NDWS2[i]];
				}
				else
				{
					prefactor = dw[Tdata->NDWS[i]];
				}

				prefactor *= Tdata->OCCFIT[i];
				phase = 2. * PI*(hnew*Tdata->XSFIT[i] + knew*Tdata->YSFIT[i] + l*Tdata->ZSFIT[i]);
				costerm = cos(phase);
				sinterm = sin(phase);

				re_surf[j] += prefactor*(fat_re[Tdata->TS[i]] * costerm - fat_im[Tdata->TS[i]] * sinterm);
				im_surf[j] += prefactor*(fat_re[Tdata->TS[i]] * sinterm + fat_im[Tdata->TS[i]] * costerm);
			}
		}
	}
}

/***************************************************************************/
void    Calc::f_surf_liquid(double l, double fat_re[], double fat_im[],
	double dw_perp[], double *re_liq, double *im_liq, Thread_t* Tdata)
	/***************************************************************************/

	/*
	Calculate real and imaginary parts of the structure factor of the
	liquid contribution to the rod 0 0.
	It is true for h and k equal 0
	*/

{
	int	i;
	double prefactor, phase, costerm, sinterm;

	for (i = 0; i < Tdata->RealNumberLiquidLayers; ++i)
	{
		prefactor = dw_perp[Tdata->NDWS2[0]] * Tdata->LIQUID_OCCFIT[i];

		phase = 2. * PI*(l*Tdata->LIQUID_ZSFIT[i]);
		costerm = cos(phase);
		sinterm = sin(phase);

		*re_liq += prefactor*(fat_re[Tdata->TS[0]] * costerm - fat_im[Tdata->TS[0]] * sinterm);
		*im_liq += prefactor*(fat_re[Tdata->TS[0]] * sinterm + fat_im[Tdata->TS[0]] * costerm);
	}
}
	



 /***************************************************************************/
inline void    Calc::f_surf(
	double *dw_par, double *dw_perp, double *dw, int cell,
	double *re_surf, double *im_surf, Thread_t* Tdata, int index) {
	/***************************************************************************/

	/*
	Calculate real and imaginary parts of the structure factor of the
	surface unit cell for all domains present. Depending on the value
	of 'cell', this is done for the first or second surface unit cell.
	*/

	int	i, j, istart, iend;
	double	hnew, knew, phase, costerm, sinterm, prefactor;

	switch (cell)
	{
	case 1:
		istart = 0;
		iend = Tdata->NSURF;
		break;

	case 2:
		istart = Tdata->NSURF;
		iend = Tdata->NSURF + Tdata->NSURF2;
		break;


	default:
		iend = Tdata->NSURF;
	}

	for (j = 0; j < Tdata->NDOMAIN; ++j){
		re_surf[j] = im_surf[j] = 0.;

		/* Next equivalent diffraction indices */
		next_domain(&hnew, &knew, j, Tdata, index); 

		/* Don't compute structure factor if the transformed diffraction indices
			are fractional and the corresponding flag is set */

		if (!(!Tdata->ZEROFRACT && ((fmod(hnew, 1.) != 0) || (fmod(knew, 1.) != 0))))
		{
			//This is the hot place in the code
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
			for (i = istart; i < iend; ++i)
			{
				if (Tdata->NDWS2[i] != 0)
					prefactor = dw_par[Tdata->NDWS[i]] * dw_perp[Tdata->NDWS2[i]];
				else
					prefactor = dw[Tdata->NDWS[i]];

				prefactor *= Tdata->OCCFIT[i];
				phase = 2.0 * PI * (hnew * Tdata->XSFIT[i] + knew * Tdata->YSFIT[i] + Tdata->LDAT[index] * Tdata->ZSFIT[i]);
				costerm = cos(phase);
				sinterm = sin(phase);
								
				re_surf[j] += prefactor*(FAT_RE[index][Tdata->TS[i]] * costerm - FAT_IM[index][Tdata->TS[i]] * sinterm);
				im_surf[j] += prefactor*(FAT_RE[index][Tdata->TS[i]] * sinterm + FAT_IM[index][Tdata->TS[i]] * costerm);
			}
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		}
	}
}



/***************************************************************************/
inline void    Calc::next_domain(double hin, double kin, double *hout, double *kout,
	int ndomain, Thread_t* Tdata)
/***************************************************************************/

	/*
	Calculate the equivalent diffraction indices (hout,kout) corresponding
	to the n-th domain.
	*/

{
	*hout = hin*Tdata->DOMMAT11[ndomain] + kin*Tdata->DOMMAT12[ndomain];
	*kout = hin*Tdata->DOMMAT21[ndomain] + kin*Tdata->DOMMAT22[ndomain];
}

/***************************************************************************/
inline void    Calc::next_domain(double *hout, double *kout,
	int ndomain, Thread_t* Tdata, int index)
/***************************************************************************/

	/*
	Calculate the equivalent diffraction indices (hout,kout) corresponding
	to the n-th domain.
	*/

{
	*hout = Tdata->HDAT[index] * Tdata->DOMMAT11[ndomain] + Tdata->KDAT[index] * Tdata->DOMMAT12[ndomain];
	*kout = Tdata->HDAT[index] * Tdata->DOMMAT21[ndomain] + Tdata->KDAT[index] * Tdata->DOMMAT22[ndomain];
}

/***************************************************************************/
inline double   Calc::occ_calc(int k, Thread_t* Tdata)
/***************************************************************************/

/*
Calculate occupancy of layer k (= fraction of surface that is terminated
by layer k) for the various roughness models
*/

{
	double 	occ;
	//It is always higher than 0
	//if (k < 0) /* this should never happen! */
	//{
	//	//errtype("Error, called occ_calc with negative layer number");
	//	return(1.);
	//}

	//if (Tdata->BETA < 1e-5) Tdata->BETA = 1e-5;   /* avoid numerical problems */

	switch (Tdata->ROUGHMODEL)
	{

	case NUMBETA:
	case APPROXBETA:
		occ = ONE_MINUS_BETA * pow(Tdata->BETA, k);
		break;

	case POISSONROUGH:
		occ = pow(Tdata->BETA + 1e-10, k)*exp(-Tdata->BETA) / factrl(k);
		break;

	case GAUSSROUGH:
		occ = exp(-sqr(k / Tdata->BETA)) - exp(-sqr((k + 1) / Tdata->BETA));
		break;

	case LINEARROUGH:
		if (k < Tdata->BETA)
			occ = 1. / Tdata->BETA;
		else
			occ = 0.;
		break;

	case COSINEROUGH:
		if (k + 1 < Tdata->BETA)
			occ = 0.5*(cos(PI*k / Tdata->BETA) - cos(PI*(k + 1) / Tdata->BETA));
		else
			occ = 0.;
		break;

	case TWOLEVEL:
		if (k == 0)
			occ = ONE_MINUS_BETA;
		else if
			(k == 1) occ = Tdata->BETA;
		else
			occ = 0.;
		break;

	default:
		occ = 0.;
		break;

	} /* endswitch */
	return (occ);
}



/***************************************************************************/
double Calc::occupancy_profile(double z, double zmean, double sigma, double start_occ)
/***************************************************************************/

/*
Calculate occupancy profile.  Start with simple Gaussian
*/

{
	if (z < 0.)
		return(0.);
	else if (z < zmean)
		return(start_occ);
	else
		return(start_occ*exp(-sqr((z - zmean) / sigma)));
}




/***************************************************************************/
_inline double Calc::factrl(int n)
/***************************************************************************/

/*
Calculate n!.
*/

{
	static int ntop = 4;
	static double a[33] = { 1.0, 1.0, 2.0, 6.0, 24.0 };
	int j;


	if (n > 32)
		return exp(gammln(1.0 + n));
	while (ntop < n)
	{
		j = ntop++;
		a[ntop] = a[j] * ntop;
	}
	return a[n];
}



/* Control function - computes x! */
double Calc::fact(double x)
{
    if (x == 1.0)
	{
        return (x);
    }
    return (x * fact(x - 1.0));
}


// Optimizations to gammln as implemented in the
// first edition of Numerical Recipes in C 
// http://www.nr.com/forum/showthread.php?t=606
__inline double Calc::gammln(double xx){
    double tmp, ser;

    tmp  = xx + 4.5;
    tmp -= (xx - 0.5) * log(tmp);

    ser = 1.000000000190015
        + (76.18009172947146     / xx)
        - (86.50532032941677     / (xx + 1.0))
        + (24.01409824083091     / (xx + 2.0))
        - (1.231739572450155     / (xx + 3.0))
        + (0.1208650973866179e-2 / (xx + 4.0))
        - (0.5395239384953e-5    / (xx + 5.0));

    return (log(2.5066282746310005 * ser) - tmp);
}



/***************************************************************************/
 double   Calc::q_length(double h, double k, double l, Thread_t* Tdata)
/***************************************************************************/

/*
Calculate length of momentum transfer vector q = (h,k,l).
Optymized version by Daniel
*/

{
		return(2.*PI*sqrt(
			sqr(h*Tdata->RLAT[0] + k*Tdata->Rlat1cosRlat5 + l*Tdata->Rlat2cosRlat4)
			+ sqr(k*Tdata->Rlat1sinRlat5 - l*Tdata->Rlat2sinRlat4cosDlat3)
			+ sqr(l / Tdata->DLAT[2])
			));
}



/***************************************************************************/
double   Calc::rms_calc(double *sum, double *mean, double *rms, Thread_t* Tdata)
/***************************************************************************/

/*
Calculate sum, root mean square and mean value of roughness distribution
*/

{
	double       p2, norm, fm, sm;
	double one_minus_beta;
	int          i;

	switch (Tdata->ROUGHMODEL)
	{
	case APPROXBETA:
		one_minus_beta = 1. - Tdata->BETA;
		if (Tdata->BETA > 0 && one_minus_beta > 1e-8) {
			*mean = Tdata->BETA / one_minus_beta;
			*rms = sqrt(Tdata->BETA) / one_minus_beta;
			*sum = 1. / one_minus_beta;
		}
		else {
			*mean = *rms = -1.;
		} /* endif */
		break;

	default: /* numerical models: */

		/* Calculate normalization factor
		   Note this routine has changed compared to Martin's old routine.
		   Occupancy now denotes the difference between successive layers.*/

		*sum = 0.;
		for (i = 0; true; ++i) {
			p2 = occ_calc(i, Tdata); /* occupancy layer 'i' */
			if (p2 < 1e-5) {
				break;
			}
			*sum += p2;
		} /* endfor */
		if (*sum < 1e-10) {
*mean = *rms = -1.;
break;
		}
		else {
			norm = 1. / (*sum);
		} /* endif */

		/* Calculate first momentum = mean value : */

		fm = 0.;
		for (i = 0; true; ++i) {
			p2 = occ_calc(i, Tdata);
			if (p2 < 1e-5) {
				break;
			} /* endif */
			fm += p2 * (double)i;
		} /* endfor */
		fm *= norm;       /* Normalization */

		/* Calculate second momentum = root mean square : */

		sm = 0.;
		for (i = 0; true; ++i) {
			p2 = occ_calc(i, Tdata);
			if (p2 < 1e-5) {
				break;
			} /* endif */
			sm += p2 * sqr((double)i - fm);
		} /* endfor */
		sm = sqrt(norm*sm);    /* Normalization */
		*mean = fm;
		*rms = sm;
		break;

	} /* endswitch */
	return (*rms);
}

/***************************************************************************/
void Calc::FindLBraggs(Thread_t* Tdata)
/***************************************************************************/
{
	int i, j, l;
	double phase_before_L, phase_after_L;
	double re_bulk[MAXDOM], im_bulk[MAXDOM];
	double	fat_re[MAXTYPES], fat_im[MAXTYPES];  /* Atomic scattering factors */
	double q_half;
	double dw[MAXPAR + 1];

	/* Compute atomic scattering factors */
	for (i = 0; i < MAXPAR; ++i)
		dw[i] = 1;

	for (i = 0; i < Tdata->NDAT; ++i)
	{
		for (l = 1; l < 30; ++l)
		{
			q_half = q_length(Tdata->HDAT[i], Tdata->KDAT[i], l, Tdata) / 2;
			for (j = 0; j < Tdata->NTYPES; ++j)
			{
				fat_re[j] = f_atomic(q_half, j, Tdata) + Tdata->F1[j][Tdata->ENERGY[i]];
				fat_im[j] = Tdata->F2[j][Tdata->ENERGY[i]];
			}

			f_bulk(Tdata->HDAT[i], Tdata->KDAT[i], 1.0*l - .1, fat_re, fat_im, dw, Tdata->ATTEN, re_bulk, im_bulk, Tdata);
			phase_before_L = atan2(im_bulk[0], re_bulk[0] + 1e-10);

			f_bulk(Tdata->HDAT[i], Tdata->KDAT[i], 1.0*l + .1, fat_re, fat_im, dw, Tdata->ATTEN, re_bulk, im_bulk, Tdata);
			phase_after_L = atan2(im_bulk[0], re_bulk[0] + 1e-10);

			Tdata->LBR[i] = 0;
			Tdata->FRAC[i] = true;
			if (phase_before_L > 0.5 && phase_after_L < 0.5 && (sqr(re_bulk[0]) + sqr(im_bulk[0]))>.1)
			{
				Tdata->LBR[i] = l;
				Tdata->FRAC[i] = false;

				break;
			}

		}

	}

}



/***************************************************************************/
inline double  Calc::sqr(double x)
/***************************************************************************/
/*
Return square of x.
*/
{
	return(x*x);
}


