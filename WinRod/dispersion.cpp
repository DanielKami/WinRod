//The code was translated from FORTRAN by Daniel M. Kaminski. Code is checked, optymised and wraped in the class. All constants from oryginal work
//are transformed to constatn arrays.
//The oryginal version was provided by Sean Brennan and the fortran support is from:
//http://www.ccp14.ac.uk/ccp/ccp14/ftp-mirror/cross-section-sfac/pub/cross-section_codes/



#include "stdafx.h"
#include "Dispersion.h"
#include <math.h>
#include "definitions.h"
#include "bdnrgin.h"	//constans
#include "relinc.h"		//constans
#include "xnrg_b_d.h"	//constans
#include "xsec_com.h"	//constans	



Dispersion::Dispersion()
{
}


Dispersion::~Dispersion()
{
}


// This routine reads data for f' and f" according to an
// algorithm by cromer and lieberman, given to fuoss.
// converted to direct access file by brennan
// converted to internal data 3-may-1993 smb

void Dispersion::cromer(int zed, double energy, double *fp, double*fpp)
{
	int irb = 0, ipr = 0, iopen = 0, jj, i_zero = 0, i_nxsect = 0;

	double		d_fp_orb = 0,
		d_fpp_orb = 0,
		d_nrg_s[12],
		d_xsect_s[12],
		d_log_nrg[12],
		d_log_xsect[12],
		d_nrg_int[6],
		d_fp_corr,
		d_energy,
		d_log_energy,
		d_var,
		d_sum_fp,
		d_sum_fpp;

	ZeroMemory(&d_log_xsect, sizeof(double) * 12);

	// relcor is the relativistic correction term
	// executable code
	if (zed < 1 || zed > 92)
	{
		//write(*,'('' Atomic number out of range'')')
		zed = 0;
		return;
	}
	else if (energy <= 0.)
	{
		//write(*,'('' Energy undefined'')')
		zed = 0;
		return;
	}
	else
	{
		if (zed < 3)
		{
			mcm(zed, energy, fp, fpp);
			iopen = zed;
			return;
		}
	}

	//       Change to keV
	d_energy = energy / 1000.;
	d_log_energy = log(d_energy);
	d_energy_au = d_energy / KEV_PER_RYD;

	d_sum_fp = 0.;
	d_sum_fpp = 0.;

	// MAIN LOOP THROUGH THE ORBITALS
	for (irb = 1; irb <= n_orb[zed]; ++irb)
	{
		icount = 6;
		d_fp_orb = 0.;
		d_fp_corr = 0.;
		d_fpp_orb = 0.;
		d_xsect_barns = 0.;
		d_bind_nrg_au = bind_nrg[zed][irb] / KEV_PER_RYD;

		if (nparams[zed][irb] == 11)
		{
			d_xsect_edge_au = xsc[zed][irb][11] / AU;
		}

		// also copy subset into second array
		for (ipr = 6; ipr <= 10; ++ipr)
		{
			jj = ipr - 5;
			d_xsect_int[jj] = xsc[zed][irb][ipr] / AU;
			d_nrg_int[jj] = nrg[zed][irb][ipr];
		}

		// the sorting routine messes up subsequent calls with same energy
		// so copy to second array before sorting.
		for (ipr = 1; ipr <= nparams[zed][irb]; ++ipr)
		{
			d_xsect_s[ipr] = xsc[zed][irb][ipr];
			d_nrg_s[ipr] = nrg[zed][irb][ipr];
		}


		sort(nparams[zed][irb], d_nrg_s, d_xsect_s);
		sort(5, d_nrg_int, d_xsect_int);

		//           convert to log of energy,xsect
		for (ipr = 1; ipr <= nparams[zed][irb]; ++ipr)
		{
			d_log_nrg[ipr] = log(d_nrg_s[ipr]);
			if (d_xsect_s[ipr] == 0.0)
				d_log_xsect[ipr] = 0.0;
			else
				d_log_xsect[ipr] = log(d_xsect_s[ipr]);

		}

		if (bind_nrg[zed][irb] <= d_energy)
		{
			i_zero = 0;
			ipr = 1;
			while (d_log_xsect[ipr] == 0.0)
			{
				i_zero = i_zero + 1;
				ipr = ipr + 1;
			}
			i_nxsect = nparams[zed][irb] - i_zero;
			i_zero = i_zero + 1;
			d_xsect_barns = aknint(d_log_energy, i_nxsect, &d_log_nrg[i_zero], &d_log_xsect[i_zero]);

			d_xsect_barns = exp(d_xsect_barns) / AU;

			d_fpp_orb = INV_FINE_STRUCT*d_xsect_barns * d_energy_au / (4.0*PI);

			d_var = d_energy_au - d_bind_nrg_au;
			if (d_var == 0.)
				d_var = 1.;
			d_fp_corr = -0.5*d_xsect_barns * d_energy_au * FINE_PI * log((d_energy_au + d_bind_nrg_au) / d_var);
		}

		if (bind_nrg[zed][irb] > d_energy && funtype[zed][irb] == 0)
		{
			d_fp_orb = gauss(this, &Dispersion::sigma3) * FINE_PI;

			d_fp_corr = 0.5*d_xsect_edge_au*sqr(d_bind_nrg_au) * log((-d_bind_nrg_au + d_energy_au) / (-d_bind_nrg_au - d_energy_au)) / d_energy_au * FINE_PI;
		}
		else
		{
			if (funtype[zed][irb] == 0)
				d_fp_orb = gauss(this, &Dispersion::sigma0) * FINE_PI;
			if (funtype[zed][irb] == 1)
				d_fp_orb = gauss(this, &Dispersion::sigma1) * FINE_PI;
			if (funtype[zed][irb] == 2)
				d_fp_orb = gauss(this, &Dispersion::sigma2) * FINE_PI;
		}

		d_sum_fp = d_sum_fp + d_fp_orb + d_fp_corr;
		d_sum_fpp = d_sum_fpp + d_fpp_orb;

	}
	// this is the end of the loop over orbits

	*fpp = d_sum_fpp;


	// Note: the Jensen correction to f' was subsequently shown to be incorrect
	// (see L. Kissel and R.H. Pratt, Acta Cryst. A46, 170 (1990))
	// and that the relativistic correction that Ludwig used is also
	// wrong.  This section retained as comments for historical reasons.
	//
	//		jensen_cor = -0.5*float(zed)
	//     1			*(d_energy_au/INV_FINE_STRUCT**2)**2
	//
	//       Subtract relcor ala ludwig and change back to real*4
	//
	//		fp = d_sum_fp+jensen_cor-relcor(zed)
	//
	// Kissel and Pratt give better corrections.  The relativistic correction
	// that Ludwig used is (5/3)(E_tot/mc^2).  Kissel and Pratt say that this
	// should be simply (E_tot/mc^2), but their correction (kpcor) apparently
	// takes this into account.  So we can use the old relcor and simply add
	// the (energy independent) kpcor term:
	//
	*fp = d_sum_fp - relcor[zed] + kpcor[zed];

	return;
}

//*****************************************************************************
double Dispersion::sigma0(double x)
{
	double d_prod;
	double temp;


	icount = icount - 1;
	d_prod = sqr(d_energy_au)* sqr(x) - sqr(d_bind_nrg_au);

	if (fabs(d_prod) < 1.e-30)
	{
		return d_xsect_int[icount] * d_bind_nrg_au / sqr(x);
	}
	else
	{
		temp = d_bind_nrg_au * d_bind_nrg_au * d_bind_nrg_au;
		return (d_xsect_int[icount] * temp / sqr(x) - d_bind_nrg_au* d_xsect_barns* sqr(d_energy_au)) / d_prod;
	}
	return 0;
}

//*****************************************************************************
double  Dispersion::sigma1(double x)
{

	icount = icount - 1;
	double temp = sqr(d_bind_nrg_au) * d_bind_nrg_au;
	return 0.5 * temp * d_xsect_int[icount] / (sqrt(x) * (sqr(d_energy_au) * sqr(x) - sqr(d_bind_nrg_au) * x));

}

//*****************************************************************************
double  Dispersion::sigma2(double x)
{
	double  denom;
	double x_pow3 = x*x*x;
	double temp;
	icount = icount - 1;

	// Code modes by Chris T. Chantler, May 12-1992

	if (fabs(x) < 1.0e-31)
	{
		//			WRITE(5,*) 'Denom. overflow'
		return 0.0;
	}
	else
		if (d_energy_au < 1.0e-31)
		{
		return 0.0;
		//				WRITE (5,*) 'e simeq 0';
		}
		else
			if (fabs(d_xsect_int[icount] - d_xsect_barns) < 1.0e-30)
			{
		//					WRITE(5,*) 'Factor cancels'
		return -2.0 *d_xsect_int[icount] * d_bind_nrg_au / x_pow3;
			}
			else
			{
				denom = x_pow3 * sqr(d_energy_au) - sqr(d_bind_nrg_au) / x;
				if (fabs(denom) < 1.0e-31)
				{
					//						WRITE(5,*) 'Denom. zero'
					return -2.0*d_xsect_int[icount] * d_bind_nrg_au / x_pow3;
				}
				else
				{
					temp = d_bind_nrg_au / x;
					temp = temp*temp*temp;
					temp = 2.0*(d_xsect_int[icount] * temp / x - d_bind_nrg_au* d_xsect_barns* sqr(d_energy_au)) / denom;
					return temp;
				}
			}
	return 0.;
}

//*****************************************************************************
double  Dispersion::sigma3(double x)
{
	icount = icount - 1;
	double xx = sqr(x);
	return (sqr(d_bind_nrg_au)*d_bind_nrg_au * (d_xsect_int[icount] - d_xsect_edge_au * xx) / (xx * (xx * sqr(d_energy_au) - sqr(d_bind_nrg_au))));
}

//*****************************************************************************
inline double  Dispersion::sqr(double x)
{
	return (x*x);
}

//*****************************************************************************
void Dispersion::lgndr(int index_, double *d_bb, double *d_cc)
{
	int ip;
	double  d_const, d_x[3], d_a[4];

	d_x[1] = .04691007703067e0;
	d_x[2] = .23076534494716e0;

	d_a[1] = .11846344252810e0;
	d_a[2] = .23931433524968e0;
	d_a[3] = .28444444444444e0;


	//  WARNING! THIS ROUTINE HAS BEEN STRIPPED SO IT IS ONLY USEFUL
	//  WITH ABS$CROMER IN THIS SET OF ROUTINES.

	*d_cc = 0.5;
	d_const = 0.0;
	ip = index_;
	//      ip limited to 1,2,3

	if (ip > 3)
	{
		ip = 6 - ip;
		d_const = -1.0;
	}
	*d_bb = d_a[ip];
	if (ip == 3)
		return;
	*d_cc = -d_const + sign(d_x[ip], d_const);
	return;

}

//*****************************************************************************
double Dispersion::sign(double x, double y)
{

	if (y>=0)
		return fabs(x);
	else
		return -fabs(x);

}

//*****************************************************************************
double  Dispersion::gauss(Dispersion *obiect, Sigma sigma)
{
	int i, index;
	double aa, bb, cc, dd;
	// sigma ! added 01-NOV-1994 RJD

	aa = bb = cc = 0.0;
	for (i = 1; i <= 5; ++i)
	{
		index = i;
		lgndr(index, &bb, &cc);
		dd = ((obiect)->*(sigma))(cc); // sigma(cc);
		aa = aa + bb * dd;
	}

	return aa;
}

//*****************************************************************************
// aitken repeated interpolation
//   d_log_energy = abscissa at which interpolation is desired
//   d_log_nrg    = vector of n values of abscissa
//   d_log_xsect    = vector of n values of ordinate
//   t    = temporary storage vector of 4*(m+1) locations)
//*****************************************************************************
double  Dispersion::aknint(double d_log_energy, int n, double d_log_nrg[], double d_log_xsect[])
{
	int i, index, mend, kk, jj;
	double t[20], del;

	ZeroMemory(&t, sizeof(double) * 20);

	index = 0;
	if (n <= 2)
	{
		//(*,'('' Too few points, funct=y(1)'')');
		return d_log_xsect[1];
	}
	del = d_log_nrg[2] - d_log_nrg[1];
	if (del > 0.)
	{
		i = 1;
		while (d_log_nrg[i] < d_log_energy && i <= n)
		{
			index = i;
			i = i + 1;
		}
	}
	else
	{
		i = 1;
		while (d_log_nrg[i] > d_log_energy && i <= n)
		{
			index = i;
			i = i + 1;
		}
	}

	index = index - 1;
	index = max0(index, 1);
	index = min0(index, n - 2);
	mend = index + 2;
	for (i = index; i <= mend; ++i)
	{
		kk = i - index + 1;
		t[kk] = d_log_xsect[i];
		t[kk + 3] = d_log_nrg[i] - d_log_energy;
	}

	for (i = 1; i <= 2; ++i)
	{
		kk = i + 1;
		for (jj = kk; jj <= 3; ++jj)
		{
			t[jj] = (t[i] * t[jj + 3] - t[jj] * t[i + 3]) / (d_log_nrg[jj + index - 1] - d_log_nrg[i + index - 1]);
		}
	}

	return t[3];
}

//*****************************************************************************
int  Dispersion::max0(int x, int y)
{
	if (x < y)
		return y;
	return x;
}
//*****************************************************************************
int  Dispersion::min0(int x, int y)
{
	if (x >= y)
		return y;
	return x;
}

//*****************************************************************************
// bubble sort.  largest becomes last
//*****************************************************************************
void Dispersion::sort(int n, double a[], double b[])
{
	int i, j;
	double  x, y;

	for (i = 1; i <= n - 1; ++i)
	{
		for (j = i + 1; j <= n; ++j)
		{
			if (a[j] < a[i])
			{
				x = a[j];
				y = a[i];
				a[i] = x;
				a[j] = y;
				x = b[j];
				y = b[i];
				b[i] = x;
				b[j] = y;
			}
		}
	}
	return;

}

//*****************************************************************************
void Dispersion::mcm(int zed, double energy, double *fp, double *fpp)
{
	double p1, p2, p3;

	// barns_to_electrons is a constant that converts the cross section in 
	// barns/atom C into a cross-section in electrons/atom which is the same 
	// as f''.  C it is calculated as
	//  1/( 10^8 [b/A^2]*2* rsube=2.8179^-5* hc=12398.52)
	// executable code

	*fp = 0.;
	p1 = log(energy / 1000.);
	p2 = p1*p1;
	p3 = p2*p1;


	if (zed == 1)
	{
		//       Hydrogen
		if (energy >= 14.e-3)
		{
			*fpp = BARNS_TO_ELECTRONS * energy * exp(2.44964 - 3.34953 * p1 - 0.047137*p2 + 0.0070996 * p3);
		}
		else
			*fpp = 0.;
	}
	else
	{
		//       Helium
		if (energy >= 25.e-3)
		{
			*fpp = BARNS_TO_ELECTRONS * energy * exp(6.06488 - 3.2905 * p1 - 0.107256 * p2 + 0.0144465 * p3);
		}
		else
			*fpp = 0.;
	}
	return;
}