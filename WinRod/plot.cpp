//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: File plot.cpp contains all the necessary functions to prepare graphs in 2D.
// The code is strongly changed comparing to Prof. Elias Vlieg version
//
// Writen by Daniel Kamisnki                                                                                                   
//
// 12/02/2013 Lublin
//--------------------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------------------
//      include files                                                       
//--------------------------------------------------------------------------------------------------------------------------------
#include <stdafx.h>
#include <math.h>
#include <ppl.h>
#include <commctrl.h>	//For progress bar in Fourier map
#include "ExMath.h"
#include "plot.h"
#include "variables.h"
#include "calc.h"
#include "2DGraph.h"
#include "resource.h"
#include "fit.h"
#include "elements.h"
#include "dispersion.h"
#include "ReadFile.h"

extern Graph2D						my2D[];

extern Dispersion					myDispersion;
extern ATOM_PROPERITIES AtProp;

//--------------------------------------------------------------------------------------------------------------------------------
void CalculateRod(double hrod, double krod, int energy, double lStart, double lEnd, int L_points, bool AutoScale){
//--------------------------------------------------------------------------------------------------------------------------------
	int i;

	double   Fth[3][MAXTHEO];    /* Structure factors for bulk, surface and   interference sum */
	double   Lth[MAXTHEO];       /* l-values of theoretical point */
	double   Phase[MAXTHEO];     /* The phase of the interference sum f's */
	int   LBRAGG = 0;
	int SUBSCALE = 0;
	bool FRAC = false;
	//int ENERGY = 0;

	if (hrod == 0 && krod == 0)
		lStart = 0;
	/* Compute range of l-values */
	if (L_points < 2)
		L_points = 2;
	double lstep = (lEnd - lStart) / (L_points - 1);

	for (i = 0; i < L_points; ++i)
	{
		Lth[i] = (lStart + lstep * i);
	}

	//Find LBragg
	for (int i = 0; i < thread.NDAT; ++i)
	{
		if (thread.HDAT[i] == hrod && thread.KDAT[i] == krod)
		{
			LBRAGG = thread.LBR[i];
			SUBSCALE = thread.SUBSC[i];
			FRAC = thread.FRAC[i];
			//ENERGY = thread.ENERGY[i];
			break;
		}
	}

	/* Compute the three structure factors */
	update_model();
	using namespace Concurrency;
	parallel_for(0, L_points, [&](int i)
	//for (i = 0; i < L_points; ++i)
	{
		calc.f_calc(hrod, krod, Lth[i], thread.ATTEN, LBRAGG, FRAC, SUBSCALE, energy,
			&Fth[0][i], &Fth[1][i], &Fth[2][i], &Phase[i], &thread);
	});

	my2D[0].fit[0].NrOfPoints = L_points;  //Calculate both
	my2D[0].fit[1].NrOfPoints = L_points;  //Calculate bulk
	my2D[0].fit[2].NrOfPoints = L_points;  //Calculate surface
	my2D[0].fit[3].NrOfPoints = L_points;  //Calculate phase

	for (i = 0; i < L_points; ++i)
	{
		my2D[0].fit[0].x[i] = Lth[i];
		my2D[0].fit[0].y[i] = Fth[2][i];

		my2D[0].fit[1].x[i] = Lth[i];
		my2D[0].fit[1].y[i] = Fth[0][i];

		my2D[0].fit[2].x[i] = Lth[i];
		my2D[0].fit[2].y[i] = Fth[1][i];

		my2D[0].fit[3].x[i] = Lth[i];
		my2D[0].fit[3].y[i] = Phase[i];

		
	}

	my2D[0].fit[0].LBragg = LBRAGG;

	if (energy == 0)
	{
		if (thread.ZEROFRACT){
			swprintf_s(my2D[0].data.header, HEADER, L"Rod (%2.5f %2.5f)", hrod, krod);
			swprintf_s(my2D[0].fit[0].header, HEADER, L"Rod (%2.5f %2.5f)", hrod, krod);
		}
		else{
			swprintf_s(my2D[0].data.header, HEADER, L"Rod (%2.0f %2.0f)", hrod, krod);
			swprintf_s(my2D[0].fit[0].header, HEADER, L"Rod (%2.0f %2.0f)", hrod, krod);
		}
	}
	else
	{
		if (thread.ZEROFRACT){
			swprintf_s(my2D[0].data.header, HEADER, L"Rod (%2.5f %2.5f) Energy %d", hrod, krod, energy);
			swprintf_s(my2D[0].fit[0].header, HEADER, L"Rod (%2.5f %2.5f) Energy %d", hrod, krod, energy);
		}
		else{
			swprintf_s(my2D[0].data.header, HEADER, L"Rod (%2.0f %2.0f) Energy %d", hrod, krod, energy);
			swprintf_s(my2D[0].fit[0].header, HEADER, L"Rod (%2.0f %2.0f) Energy %d", hrod, krod, energy);
		}
	}

	swprintf_s(my2D[0].data.TitleX, AXIS_TITLE_LENGHT, L"l");
	if (!thread.STRUCFAC)
		swprintf_s(my2D[0].data.TitleY, AXIS_TITLE_LENGHT, L"F²");
	else
		swprintf_s(my2D[0].data.TitleY, AXIS_TITLE_LENGHT, L"F");

	my2D[0].SELECT_GRAPH = WINDOW_ROD;   //0 - select the rod graph 
	my2D[0].lin_log = false;
	my2D[0].STRUCFAC = thread.STRUCFAC;
	if (AutoScale)
		my2D[0].AutomaticSetTheScale();

}


//--------------------------------------------------------------------------------------------------------------------------------
void CalculateInPlane(double hStart, double hEnd, double kStart, double kEnd, double lPlane, bool AutoScale){
//--------------------------------------------------------------------------------------------------------------------------------
	int index = 0;
	double   h, k;
	double   Fth[3][MAXTHEO];    /* Structure factors for bulk, surface and   interference sum */
	double   Hth[MAXTHEO];       /* h-values of theoretical point */
	double   Kth[MAXTHEO];       /* k-values of theoretical point */
	double   Phase[MAXTHEO];     /* The phase of the interference sum f's */

	double cosRlat5 = cos(thread.RLAT[5]);

	update_model();

	for (h = hStart - 1; h < hEnd + 1; ++h)
	{
		for (k = kStart - 1; k < kEnd + 1; ++k)
		{
			Hth[index] = h + k*cosRlat5;
			Kth[index] = k;

			/* Compute the three structure factors */
			calc.f_calc(h, k, lPlane, thread.ATTEN, thread.LBR[index], thread.FRAC[index], thread.SUBSC[index], thread.ENERGY[index], &Fth[0][index], &Fth[1][index], &Fth[2][index], &Phase[index], &thread);
			index++;
		}
	}

	//After the rec space have been calculated copy the reciprocal lattice param. to graph structure
	for (int i = 0; i < 6; ++i)
	{
		my2D[1].fit[0].RLAT[i] = thread.RLAT[i];
		my2D[1].fit[1].RLAT[i] = thread.RLAT[i];
		my2D[1].fit[2].RLAT[i] = thread.RLAT[i];
	}

	my2D[1].fit[0].NrOfPoints = index;
	my2D[1].fit[1].NrOfPoints = index;
	my2D[1].fit[2].NrOfPoints = index;

	for (int i = 0; i < index; ++i){ //calculate both
		my2D[1].fit[0].x[i] = Hth[i];
		my2D[1].fit[0].y[i] = Kth[i];
		my2D[1].fit[0].z[i] = Fth[2][i];
		//calculate bulk
		my2D[1].fit[1].x[i] = Hth[i];
		my2D[1].fit[1].y[i] = Kth[i];
		my2D[1].fit[1].z[i] = Fth[0][i];
		//calculate surface
		my2D[1].fit[2].x[i] = Hth[i];
		my2D[1].fit[2].y[i] = Kth[i];
		my2D[1].fit[2].z[i] = Fth[1][i];
	}

	my2D[1].data.InPlaneL = lPlane;
	swprintf_s(my2D[1].data.header, HEADER, L"Plane l=%4.3f", lPlane);
	swprintf_s(my2D[1].data.TitleX, AXIS_TITLE_LENGHT, L"h");
	swprintf_s(my2D[1].data.TitleY, AXIS_TITLE_LENGHT, L"k");
	my2D[1].SELECT_GRAPH = WINDOW_IN_PLANE;   //1 - select the in-plane graph  
	my2D[1].lin_log = false;

	if (AutoScale)
		my2D[1].AutomaticSetTheScale(true);//true only from data

}


//--------------------------------------------------------------------------------------------------------------------------------
void FindInPlaneData(double lPlane, bool AutoScale)
//--------------------------------------------------------------------------------------------------------------------------------
{

#define L_ACCEPTANCE 0.1//data points in range of +-L_ACCEPTANCE will be accepted 

	//find the same in-plane data and count the points
	my2D[1].data.NrOfPoints = 0;
	for(int i = 0; i<thread.NDAT; ++i){
		if(thread.LDAT[i] < lPlane + L_ACCEPTANCE && thread.LDAT[i] > lPlane - L_ACCEPTANCE)
		{
			my2D[1].data.x[my2D[1].data.NrOfPoints] = thread.HDAT[i] + thread.KDAT[i]*cos(thread.RLAT[5]);
			my2D[1].data.y[my2D[1].data.NrOfPoints] = thread.KDAT[i];
			my2D[1].data.z[my2D[1].data.NrOfPoints] = thread.FDAT[i];
			my2D[1].data.sigma[my2D[1].data.NrOfPoints] = variable.ERRDAT[i];
			my2D[1].data.NrOfPoints++;
		}
	}
	swprintf_s( my2D[1].data.header, HEADER, L"Plane l=%4.3f", lPlane);
	swprintf_s(my2D[1].data.TitleX, AXIS_TITLE_LENGHT, L"h");
	swprintf_s(my2D[1].data.TitleY, AXIS_TITLE_LENGHT, L"k");
	my2D[1].SELECT_GRAPH = WINDOW_IN_PLANE;   //1 - select the in-plane graph  
	my2D[1].RefreshWindow();
	my2D[1].errors_plot = true;
	my2D[1].data_plot = true;
	if (AutoScale)
		my2D[1].AutomaticSetTheScale();
}


//--------------------------------------------------------------------------------------------------------------------------------
void FindRodData(double hrod, double krod, int energy, bool AutoScale   ){
//--------------------------------------------------------------------------------------------------------------------------------
	//find the same rods and count the points
	my2D[0].data.NrOfPoints = 0;

	if (variable.ENERGYTOT == 0)
	{
		for (int i = 0; i < thread.NDAT; ++i)
		{
			if (thread.HDAT[i] == hrod && thread.KDAT[i] == krod){
				my2D[0].data.x[my2D[0].data.NrOfPoints] = thread.LDAT[i];
				my2D[0].data.y[my2D[0].data.NrOfPoints] = thread.FDAT[i];
				my2D[0].data.sigma[my2D[0].data.NrOfPoints] = variable.ERRDAT[i];
				my2D[0].data.NrOfPoints++;
			}
		}
	}
	else
	{
		using namespace Concurrency;
		parallel_for(0, thread.NDAT, [&](int i)
		//for (int i = 0; i < thread.NDAT; ++i)
		{
			if (thread.HDAT[i] == hrod && thread.KDAT[i] == krod  && thread.ENERGY[i] == energy){
				my2D[0].data.x[my2D[0].data.NrOfPoints] = thread.LDAT[i];
				my2D[0].data.y[my2D[0].data.NrOfPoints] = thread.FDAT[i];
				my2D[0].data.sigma[my2D[0].data.NrOfPoints] = variable.ERRDAT[i];
				my2D[0].data.NrOfPoints++;
			}
		});
	}


	if (thread.ZEROFRACT)
		swprintf_s(my2D[0].data.header, HEADER, L"Rod (%2.5f %2.5f)", hrod, krod);
	else
		swprintf_s(my2D[0].data.header, HEADER, L"Rod (%2.0f %2.0f)", hrod, krod);

	swprintf_s(my2D[0].data.TitleX, AXIS_TITLE_LENGHT, L"l");
	if (!thread.STRUCFAC)
		swprintf_s(my2D[0].data.TitleY, AXIS_TITLE_LENGHT, L"F²");
	else
		swprintf_s(my2D[0].data.TitleY, AXIS_TITLE_LENGHT, L"F");
	my2D[0].SELECT_GRAPH = WINDOW_ROD;   //0 - select the Rod graph 
	my2D[0].RefreshWindow();
	my2D[0].errors_plot = true;
	my2D[0].data_plot = true;
	if (AutoScale)
		my2D[0].AutomaticSetTheScale();

}

//--------------------------------------------------------------------------------------------------------------------------------
void Multiplot(bool *SelectPlot, int N_columns, int L_points)
//--------------------------------------------------------------------------------------------------------------------------------
{
	int i, k, nr, NumberSelectedRods;
	int NumberOfRods;

	double   Fth[3][MAXTHEO];    /* Structure factors for bulk, surface and   interference sum */
	double   Hth[MAXTHEO];       /* h-values of theoretical point */
	double   Kth[MAXTHEO];       /* k-values of theoretical point */
	double   Lth[MAXTHEO];       /* l-values of theoretical point */
	double   Phase[MAXTHEO];     /* The phase of the interference sum f's */
	int   LBRAGG = 0;
	int SUBSCALE = 0;
	bool FRAC = false;
	double hrod, krod, l_start[MAXMULTIPLOT];
	double Storage_h[MAXMULTIPLOT];
	double Storage_k[MAXMULTIPLOT];
	int Storage_E[MAXMULTIPLOT];
	int energy = 0;
	double lstep[MAXMULTIPLOT];

	/* Compute range of l-values */
	if (L_points < 2)
		L_points = 2;

//----------------------------------------------------------------------------------------------------------------
//find rods
//----------------------------------------------------------------------------------------------------------------
	krod = hrod = 9999; //set it to arbitrary large number
	NumberOfRods = 0;

	if (variable.ENERGYTOT == 0)
	{
		for (i = 0; i < thread.NDAT; ++i)
		{
			if (thread.HDAT[i] != hrod || thread.KDAT[i] != krod)
			{
				hrod = thread.HDAT[i];
				krod = thread.KDAT[i];
			
				if (hrod == 0 && krod == 0)
					l_start[NumberOfRods] = 0.;
				else
					l_start[NumberOfRods] = variable.l_start;
				lstep[NumberOfRods] = (variable.l_end - l_start[NumberOfRods]) / (L_points - 1);

				Storage_h[NumberOfRods] = hrod;
				Storage_k[NumberOfRods] = krod;
				Storage_E[NumberOfRods] = 0;

				NumberOfRods++;
				if (NumberOfRods >= MAXMULTIPLOT){
					MessageBox(NULL, TEXT("Maximum number of ROD in multiplot rich limit."), NULL, MB_ICONERROR);
					break;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < thread.NDAT; ++i)
		{
			if (thread.HDAT[i] != hrod || thread.KDAT[i] != krod || thread.ENERGY[i] != energy)
			{
				hrod = thread.HDAT[i];
				krod = thread.KDAT[i];
				energy = thread.ENERGY[i];

				if (hrod == 0 && krod == 0)
					l_start[NumberOfRods] = 0.;
				else
					l_start[NumberOfRods] = variable.l_start;
				lstep[NumberOfRods] = (variable.l_end - l_start[NumberOfRods]) / (L_points - 1);

				Storage_h[NumberOfRods] = hrod;
				Storage_k[NumberOfRods] = krod;
				Storage_E[NumberOfRods] = energy;

				NumberOfRods++;
				if (NumberOfRods >= MAXMULTIPLOT){
					MessageBox(NULL, TEXT("Maximum number of ROD in multiplot rich limit."), NULL, MB_ICONERROR);
					
					break;
				}
			}
		}
	}

//We have number of different rods NumberOfRods, and segregated rods in Storage. Lets fill the 2DPlot tables with different rods

	//Find number of selected rods
	NumberSelectedRods = 0;
	for (i = 0; i < NumberOfRods; ++i)
	{
		if (SelectPlot[i])
			NumberSelectedRods++;
	}


	my2D[4].NumberOfRods = NumberSelectedRods;
	if (!my2D[4].InitializeMultiDraw())
	{
		MessageBox(NULL, TEXT("Maximum number of RODs in multiplot rich limit."), NULL, MB_ICONERROR);
		return;
	}
//----------------------------------------------------------------------------------------------------------------
	
	nr = 0;
	for (k = 0; k <NumberOfRods; ++k)
	{
		if (SelectPlot[k])	//Plot rods only selected in CallBack window
		{
			for (i = 0; i < L_points; ++i)
			{
				Hth[i] = Storage_h[k];
				Kth[i] = Storage_k[k];
				Lth[i] = (l_start[k] + lstep[k] * i);
			}

			//Find LBragg
			for (i = 0; i < thread.NDAT; ++i)
			{
				if (thread.HDAT[i] == Storage_h[k] && thread.KDAT[i] == Storage_k[k])
				{
					LBRAGG = thread.LBR[i];
					SUBSCALE = thread.SUBSC[i];
					FRAC = thread.FRAC[i];
					break;
				}
			}

			/* Compute the three structure factors */
			using namespace Concurrency;
			parallel_for(0, L_points, [&](int i)
			//for (i = 0; i < L_points; ++i)
			{
				calc.f_calc(Storage_h[k], Storage_k[k], Lth[i], thread.ATTEN, LBRAGG, FRAC, SUBSCALE, Storage_E[k],
					&Fth[0][i], &Fth[1][i], &Fth[2][i], &Phase[i], &thread);
			});

			my2D[4].Multiplot[nr].fit[0].NrOfPoints = L_points;  //Calculate both
			my2D[4].Multiplot[nr].fit[1].NrOfPoints = L_points;  //Calculate bulk
			my2D[4].Multiplot[nr].fit[2].NrOfPoints = L_points;  //Calculate surface
			my2D[4].Multiplot[nr].fit[3].NrOfPoints = L_points;  //Calculate phase

			for (i = 0; i < L_points; ++i)
			{
				my2D[4].Multiplot[nr].fit[0].x[i] = Lth[i];
				my2D[4].Multiplot[nr].fit[0].y[i] = Fth[2][i];

				my2D[4].Multiplot[nr].fit[1].x[i] = Lth[i];
				my2D[4].Multiplot[nr].fit[1].y[i] = Fth[0][i];

				my2D[4].Multiplot[nr].fit[2].x[i] = Lth[i];
				my2D[4].Multiplot[nr].fit[2].y[i] = Fth[1][i];

				my2D[4].Multiplot[nr].fit[3].x[i] = Lth[i];
				my2D[4].Multiplot[nr].fit[3].y[i] = Phase[i];
			}

			my2D[4].Multiplot[nr].fit[0].LBragg = LBRAGG;

			if (energy == 0)
			{
				if (thread.ZEROFRACT){
					swprintf_s(my2D[4].Multiplot[nr].fit[0].header, HEADER, L"Rod (%2.5f %2.5f)", Storage_h[k], Storage_k[k]);
				}
				else{
					swprintf_s(my2D[4].Multiplot[nr].fit[0].header, HEADER, L"Rod (%2.0f %2.0f)", Storage_h[k], Storage_k[k]);
				}
			}
			else
			{
				if (thread.ZEROFRACT){
					swprintf_s(my2D[4].Multiplot[nr].fit[0].header, HEADER, L"Rod (%2.5f %2.5f) Energy %d", Storage_h[k], Storage_k[k], energy);
				}
				else{
					swprintf_s(my2D[4].Multiplot[nr].fit[0].header, HEADER, L"Rod (%2.0f %2.0f) Energy %d", Storage_h[k], Storage_k[k], energy);
				}
			}
			swprintf_s(my2D[4].data.header, HEADER, L"");

			swprintf_s(my2D[4].data.TitleX, AXIS_TITLE_LENGHT, L"l");
			if (!thread.STRUCFAC)
				swprintf_s(my2D[4].data.TitleY, AXIS_TITLE_LENGHT, L"F²");
			else
				swprintf_s(my2D[4].data.TitleY, AXIS_TITLE_LENGHT, L"F");
			my2D[4].SELECT_GRAPH = WINDOW_MULTIPLOT;

			my2D[4].lin_log = false;


			//--------------------------------------------
			//Add data points
			//find the same rods and count the points
			my2D[4].Multiplot[nr].data.NrOfPoints = 0;

			if (variable.ENERGYTOT == 0)
			{
				for (i = 0; i < thread.NDAT; ++i)
				{
					if (thread.HDAT[i] == Storage_h[k] && thread.KDAT[i] == Storage_k[k])
					{
						my2D[4].Multiplot[nr].data.x[my2D[4].Multiplot[nr].data.NrOfPoints] = thread.LDAT[i];
						my2D[4].Multiplot[nr].data.y[my2D[4].Multiplot[nr].data.NrOfPoints] = thread.FDAT[i];
						my2D[4].Multiplot[nr].data.sigma[my2D[4].Multiplot[nr].data.NrOfPoints] = variable.ERRDAT[i];
						my2D[4].Multiplot[nr].data.NrOfPoints++;
					}
				}
			}
			else
			{
				for (i = 0; i < thread.NDAT; ++i)
				{
					if (thread.HDAT[i] == Storage_h[k] && thread.KDAT[i] == Storage_k[k] && thread.ENERGY[i] == Storage_E[k])
					{
						my2D[4].Multiplot[nr].data.x[my2D[4].Multiplot[nr].data.NrOfPoints] = thread.LDAT[i];
						my2D[4].Multiplot[nr].data.y[my2D[4].Multiplot[nr].data.NrOfPoints] = thread.FDAT[i];
						my2D[4].Multiplot[nr].data.sigma[my2D[4].Multiplot[nr].data.NrOfPoints] = variable.ERRDAT[i];
						my2D[4].Multiplot[nr].data.NrOfPoints++;
					}
				}
			}

			my2D[4].errors_plot = true;
			my2D[4].data_plot = true;

			my2D[4].N_columns = N_columns;
		
			nr++;
		}
	}
}


//-----------------------------------------------------------------------------------------------------------------
void    plot_zdensity(double h, double k, int curve, int Npoints)
//-----------------------------------------------------------------------------------------------------------------
{
	/*
	Plot z-projected electron density, with strength depending on the
	in-plane momentum transfer.
	Created by E. Vlieg
	modified by Daniel Kaminski
	*/
	double zmin, qpar, zmax, zstep;
	double	usqr_perp, weight=0, dw_perp, dw_par, occupancy;
	int	i, j;

	double *DENS = new double[Npoints];				//function value
	double *ZDENS = new double[Npoints];

	/* Get in-plane diffraction indices */
	qpar = calc.q_length(h, k, 0, &thread);

	/* Determine calculation range */
	zmin = zmax = 0.;
	for (i = 0; i < thread.NBULK; ++i){
		if (thread.ZB[i] < zmin) zmin = thread.ZB[i];
		if (thread.ZB[i] > zmax) zmax = thread.ZB[i];
	}
	for (i = 0; i < thread.NSURF; ++i){
		if (thread.ZSFIT[i] < zmin) zmin = thread.ZSFIT[i];
		if (thread.ZSFIT[i] > zmax) zmax = thread.ZSFIT[i];
	}
	if (thread.LIQUID_PROFILE)
		for (i = 0; i < thread.RealNumberLiquidLayers; ++i)
		{
		if (thread.LIQUID_ZSFIT[i] < zmin) zmin = thread.LIQUID_ZSFIT[i];
		if (thread.LIQUID_ZSFIT[i] > zmax) zmax = thread.LIQUID_ZSFIT[i];
		}

	zmin -= 0.1;
	zmax += 1.1;
	zstep = (zmax - zmin) / (Npoints - 1);

	/* Initialize density array */
	for (i = 0; i < Npoints; ++i)
	{
		DENS[i] = 0.;				//function value
		ZDENS[i] = zmin + i*zstep;	//z coordinate
	}

	/* Sum the contributions of all the individual atoms */
	/* The vibration amplitude is expressed in fractional coordinates */


	if (thread.LIQUID_PROFILE)
	{
		for (i = 0; i < thread.RealNumberLiquidLayers; ++i)//
		{
			usqr_perp = thread.DEBWAL2[thread.NDWS2[0] - 1] / (8. * PI*PI*thread.DLAT[2] * thread.DLAT[2]);
	
			if (usqr_perp < 0.0001)
				usqr_perp = 0.0001;

			occupancy = thread.LIQUID_OCCFIT[i];
			//very high DW par. reduce te expresion
			weight = calc.f_atomic(0., thread.TS[0], &thread) * occupancy;

			for (j = 0; j < Npoints; ++j)
			{
				DENS[j] += weight*exp(-sqr(thread.LIQUID_ZSFIT[i] - ZDENS[j]) / (2.*usqr_perp));  //Removed SCALE2 somehow it should not be here???
			}
		}
	}

	//Bulk contribution
	for (i = 0; i < thread.NBULK; ++i){
		if (thread.NDWB[i] == 0)
			usqr_perp = 0.0001;
		else
			usqr_perp = thread.DEBWAL[thread.NDWB[i] - 1] / (8. * PI*PI*thread.DLAT[2] * thread.DLAT[2]);

		if (usqr_perp < 0.0001)
			usqr_perp = 0.0001;
		if (thread.NDWB[i] == 0)
			dw_par = 0.;
		else
			dw_par = thread.DEBWAL[thread.NDWB[i] - 1];

		weight = calc.f_atomic(0., thread.TB[i], &thread) * exp(-qpar*qpar*dw_par / PIPI16) / sqrt(usqr_perp * 2 * PI);

		for (j = 0; j < Npoints; ++j)
		{
			DENS[j] += weight*exp(-sqr(thread.ZB[i] - ZDENS[j]) / (2. * usqr_perp));
		}
	}


	//I surface contribution
	for (i = 0; i < thread.NSURF; ++i){
		if (thread.NDWS2[i] == 0)
		{ /* isotropic DW parameter */
			if (thread.NDWS[i] == 0)
				dw_perp = 0.;
			else
				dw_perp = thread.DEBWAL[thread.NDWS[i] - 1];
		}
		else
			dw_perp = thread.DEBWAL2[thread.NDWS2[i] - 1];

		usqr_perp = dw_perp / (8. * PI*PI*thread.DLAT[2] * thread.DLAT[2]);

		if (usqr_perp < 0.0001)
			usqr_perp = 0.0001;

		//Implementation of two surface unit cels
		occupancy = 1.0-thread.SURF2FRAC;

		if (thread.NOCCUP[i] == 0)
			occupancy *= 1.;
		else
			occupancy *= thread.OCCFIT[i];


		if (variable.NFACTOR > 0)
			occupancy *= (float)SizeDistribution(thread.NOCCUP2[i] - 1);
		else
			if (thread.NOCCUP2[i] == 0)
				occupancy *= 1.;
			else
				occupancy *= thread.OCCUP2[thread.NOCCUP2[i] - 1];


		if (thread.NDWS[i] == 0)
			dw_par = 0.;
		else
			dw_par = thread.DEBWAL[thread.NDWS[i] - 1];

		weight = calc.f_atomic(0., thread.TS[i], &thread)	* occupancy * exp(-qpar*qpar*dw_par / PIPI16) / sqrt(usqr_perp * 2. * PI);

		for (j = 0; j < Npoints; ++j)
		{
			//DENS[j] += weight*exp(-sqr(thread.ZSFIT[i]*thread.SCALE2-ZDENS[j])/(2*usqr_perp));
			DENS[j] += weight*exp(-sqr(thread.ZSFIT[i] - ZDENS[j]) / (2.* usqr_perp));  //Removed SCALE2 somehow it should not be here???
		}
	}


	//II surface contribution
	for (i = thread.NSURF; i < thread.NSURF+ thread.NSURF2; ++i) {
		if (thread.NDWS2[i] == 0)
		{ /* isotropic DW parameter */
			if (thread.NDWS[i] == 0)
				dw_perp = 0.;
			else
				dw_perp = thread.DEBWAL[thread.NDWS[i] - 1];
		}
		else
			dw_perp = thread.DEBWAL2[thread.NDWS2[i] - 1];

		usqr_perp = dw_perp / (8. * PI*PI*thread.DLAT[2] * thread.DLAT[2]);

		if (usqr_perp < 0.0001)
			usqr_perp = 0.0001;

		//Implementation of two surface unit cels
		occupancy = thread.SURF2FRAC;

		if (thread.NOCCUP[i] == 0)
			occupancy *= 1.;
		else
			occupancy *= thread.OCCFIT[i];


		if (variable.NFACTOR > 0)
			occupancy *= (float)SizeDistribution(thread.NOCCUP2[i] - 1);
		else
			if (thread.NOCCUP2[i] == 0)
				occupancy *= 1.;
			else
				occupancy *= thread.OCCUP2[thread.NOCCUP2[i] - 1];


		if (thread.NDWS[i] == 0)
			dw_par = 0.;
		else
			dw_par = thread.DEBWAL[thread.NDWS[i] - 1];

		weight = calc.f_atomic(0., thread.TS[i], &thread)	* occupancy * exp(-qpar * qpar*dw_par / PIPI16) / sqrt(usqr_perp * 2. * PI);

		for (j = 0; j < Npoints; ++j)
		{
			//DENS[j] += weight*exp(-sqr(thread.ZSFIT[i]*thread.SCALE2-ZDENS[j])/(2*usqr_perp));
			DENS[j] += weight * exp(-sqr(thread.ZSFIT[i] - ZDENS[j]) / (2.* usqr_perp));  //Removed SCALE2 somehow it should not be here???
		}
	}




	/* Convert the fractional z-coordinates to Angstroms */
	for (i = 0; i < Npoints; ++i)
		ZDENS[i] *= thread.DLAT[2];

	/* And now plot the results */

	my2D[2].fit[curve].NrOfPoints = Npoints; //Nr of points
	for (i = 0; i < Npoints; ++i){
		my2D[2].fit[curve].x[i] = ZDENS[i];
		my2D[2].fit[curve].y[i] = DENS[i];
	}

	my2D[2].errors_plot = false;
	my2D[2].data_plot = false;
	my2D[2].lin_log = true;
	//my2D[2].fit[curve].status = true;
	my2D[2].SELECT_GRAPH = WINDOW_DENSITY;
	my2D[2].AutomaticSetTheScale();
	swprintf_s(my2D[2].data.header, HEADER, L"Electron density profile");
	swprintf_s(my2D[2].fit[curve].header, HEADER, L"(%2.0f %2.0f)", h, k);
	swprintf_s(my2D[2].data.TitleX, AXIS_TITLE_LENGHT, L"z [\305]");
	swprintf_s(my2D[2].data.TitleY, AXIS_TITLE_LENGHT, L"E.d.");

	delete[] DENS;
	delete[] ZDENS;
}


//--------------------------------------------------------------------------------------------------------------------------------
bool    plot_chi_map(HWND hwnd, int NxCon, int NyCon, int Nlevel, double *Parameter1, double LimitsParameter1Low, double LimitsParameter1Up, WCHAR *x_title, double *Parameter2, double LimitsParameter2Low, double LimitsParameter2Up, WCHAR *y_title)
//--------------------------------------------------------------------------------------------------------------------------------
{

	double		 zmin, zmax, zstep;
	double       xcmin, xcmax, ycmin, ycmax, xstep, ystep;

	int i, j, k;

	if ((thread.NSURF == 0) && (thread.NBULK == 0)){
		MessageBox(hwnd, L"No model read in!", L"Error", NULL);
		return false;
	}

	/* Calculate and store constant values, in order to speed up fitting */
	calc.f_calc_fit_init(&thread);	//Storage values for bulk speedup are stored in structure Rod (it is important for multithreading)


	/* Compute weight of data points */
	ComputeWeights();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Check number of steps 
	if (NxCon < 3) NxCon = 3;
	if (NyCon < 3) NyCon = 3;

	//make it even
	if (NxCon % 2 == 0) NxCon++;
	if (NyCon % 2 == 0) NyCon++;

	double        *x, *y, *z;
	// Allocate memory for the arrays neccesary for the Fourier map 
	x = new double[NxCon + 1];
	y = new double[NyCon + 1];
	z = new double[NxCon*NyCon + 1];


	////////////////////////////////////////////////////////////////////////////////

	xcmin = LimitsParameter1Low;
	xcmax = LimitsParameter1Up;

	ycmin = LimitsParameter2Low;
	ycmax = LimitsParameter2Up;

	// Compute arrays with x and y values 
	xstep = (xcmax - xcmin) / (NxCon - 1);
	ystep = (ycmax - ycmin) / (NyCon - 1);
	for (i = 0; i < NxCon; ++i) 
		x[i] = xcmin + i*xstep;
	for (i = 0; i < NyCon; ++i)
		y[i] = ycmin + i*ystep;


	zmin = 1e100;
	zmax = 0;

	double *chi = new double[thread.NDAT];
	double chisqr=0;
	double temp_chi=0;
	for (i = 0; i < NyCon; ++i)
	{
		SendMessage(GetDlgItem(hwnd, IDC_PROGRESS1), PBM_SETPOS, (WPARAM)i, 0);// progress bar

		*Parameter2 = ycmin + i*ystep;
		for (j = 0; j < NxCon; ++j){
			*Parameter1 = xcmin + j*xstep;
			////////////////////////////////////////////////////////////////

			update_model();

			using namespace Concurrency;
			parallel_for(0, thread.NDAT, [&](int k)
			//for (k = 0; k < thread.NDAT; ++k)
			{
				temp_chi = calc.f_calc_fit(&thread, k) - thread.FDAT[k];
				chi[k] = temp_chi * temp_chi * thread.FWEIGHT[k];
			});

			chisqr = 0.;
			for (k = 0; k < thread.NDAT; ++k)
				chisqr += chi[k];
			////////////////////////////////////////////////////////////////
			if (chisqr < zmin)  zmin = chisqr; //minimal contur level value
			if (chisqr > zmax)  zmax = chisqr; //maximal contur level value
			z[NxCon*i + j] = chisqr;
		}	// end j loop 
	} // end i loop

	delete[] chi;

	if (Nlevel < 2) Nlevel = 2;
	zstep = (zmax - zmin) / (Nlevel - 1);


	// Make the actual contour plot 
	my2D[3].SELECT_GRAPH = WINDOW_CONTUR; // 2 - contur plot
	my2D[3].lin_log = true;
	my2D[3].Contur.NxCon = NxCon;
	my2D[3].Contur.NyCon = NyCon;
	my2D[3].Contur.AllocateMemory(); //Alocate memory according to given dimensions NxCon  and NyCon. Dont copy any values to x, y, z without memory alocation
	my2D[3].Contur.zmin = zmin;
	my2D[3].Contur.zmax = zmax;
	my2D[3].Contur.zstep = zstep;
	my2D[3].Contur.Nlevel = Nlevel;
	CopyMemory(my2D[3].Contur.x, x, sizeof(double)*NxCon);
	CopyMemory(my2D[3].Contur.y, y, sizeof(double)*NyCon);
	CopyMemory(my2D[3].Contur.z, z, sizeof(double)*(NxCon*NyCon + 1));
	my2D[3].AutomaticSetTheScale();

	//Text
	WCHAR Str[HEADER];
	swprintf_s(Str, HEADER, L" zmin %4.4g, zmax %4.4g midle %4.4g", zmin, zmax, (zmax + zmin) / 2);
	CopyMemory(my2D[3].Contur.Title, Str, sizeof(Str));
	CopyMemory(my2D[3].Contur.TitleX, x_title, sizeof(WCHAR) * 20);
	CopyMemory(my2D[3].Contur.TitleY, y_title, sizeof(WCHAR) * 20);

	swprintf_s(my2D[3].data.Title, HEADER, L"Chi square map in function of parameters ");

	// Free the previously allocated memory
	delete[] x;
	delete[] y;
	delete[] z;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
void PlotCoeficients(int Ze, double energy_Start, double energy_End, int L_points)
//--------------------------------------------------------------------------------------------------------------------------------

{
	double lstep = (energy_End - energy_Start) / (L_points - 1);

	double fi, fii;
	int i;

	double en;
	for (i = 0; i < L_points; ++i)
	{
		en = energy_Start + lstep * i;
		myDispersion.cromer(Ze, en * 1000, &fi, &fii);
		my2D[0].fit[0].x[i] = en;
		my2D[0].fit[0].y[i] = fi;

		my2D[0].fit[1].x[i] = en;
		my2D[0].fit[1].y[i] = fii;
	}
	//____________________________________________________________________________________________________

	my2D[0].fit[0].NrOfPoints = L_points;  //Calculate both
	my2D[0].fit[1].NrOfPoints = L_points;  //Calculate bulk
	WCHAR Tstr[100];
	AtoT(Tstr, elements[Ze - 1]);
	swprintf_s(my2D[0].data.header, HEADER, L"Dispersion coefficients f' and f'' for element %s", Tstr);

	swprintf_s(my2D[0].data.TitleY, AXIS_TITLE_LENGHT, L"f'/f''");
	swprintf_s(my2D[0].data.TitleX, AXIS_TITLE_LENGHT, L"Energy [keV]");
	swprintf_s(my2D[0].fit[0].header, AXIS_TITLE_LENGHT, L"");
	my2D[0].SELECT_GRAPH = WINDOW_DISPERSION;   // select the dispersion graph 

	my2D[0].lin_log = true;
	my2D[0].data_plot = false;
	my2D[0].errors_plot = false;
	my2D[0].AutomaticSetTheScale();
	my2D[0].RefreshWindow();

}


//--------------------------------------------------------------------------------------------------------------------------------
bool    plot_fourier(HWND hwnd, int mode, unsigned int NxCon, unsigned int NyCon, double XminCon, double XmaxCon, double YminCon, double YmaxCon, unsigned int Nlevel)
//--------------------------------------------------------------------------------------------------------------------------------
{

	/*
	Plot Fourier contour plot of structure factors.
	mode 1  Patterson of experimental structure factors
	mode 2  Patterson of calculated structure factors
	mode 3  Electron density difference map
	mode 4  Electron density of model
	*/
	//this are our gra bondaries
	//XminCon           Lower bound on x range for contour plot
	//XmaxCon           Upper bound on x range for contour plot
	//YminCon           Lower bound on y range for contour plot
	//YmaxCon           Upper bound on y range for contour plot

	//_____________________________________________________________________________________________________________________________________________________________
	double   Fth[3][MAXTHEO];    /* Structure factors for bulk, surface and   interference sum */
	double   Phase[MAXTHEO];     /* The phase of the interference sum f's */


	//We need theoretical structure factors and phases so calculate it first
	for (int i = 0; i < thread.NDAT; ++i)
		calc.f_calc(thread.HDAT[i], thread.KDAT[i], thread.LDAT[i], thread.ATTEN, thread.LBR[i], 0, thread.SUBSC[i], 0, &Fth[0][i], &Fth[1][i], &Fth[2][i], &Phase[i], &thread);

	//_____________________________________________________________________________________________________________________________________________________________


	double		 zmin, zmax, zstep;
	double       cart, xcmin, xcmax, ycmin, ycmax, xstep, ystep, temp;

	double	tan5, sin5;
	long unsigned  i;


	if ((thread.NSURF == 0) && (thread.NBULK == 0)){
		MessageBox(hwnd, L"No model read in!", L"Error", NULL);
		return false;
	}

	// Check number of steps 
	if (NxCon < 3) NxCon = 3;
	if (NyCon < 3) NyCon = 3;

	//make it even
	if (NxCon % 2 == 0) NxCon++;
	if (NyCon % 2 == 0) NyCon++;

	// Genetrate a set of symmetry-equivalent reflections from the  data set 
	if (variable.NTRANS == 0){
		MessageBox(hwnd, L"First set the symmetry group!", L"Symmetry error", NULL);
		return false;
	}

	// Generate the set of symmetry-equivalent reflections 
	make_symmetry_set(mode);

	double        *x, *y, *z;
	// Allocate memory for the arrays neccesary for the Fourier map 
	x = new double[NxCon + 1];
	y = new double[NyCon + 1];
	z = new double[NxCon*NyCon + 1];

	// Find the cartesian coordinates of the boundaries, such that the required unit cell fits in 
	xcmin = xcmax = XminCon*thread.DLAT[0] + YminCon*thread.DLAT[1] * cos(thread.DLAT[5]);
	cart = XmaxCon*thread.DLAT[0] + YminCon*thread.DLAT[1] * cos(thread.DLAT[5]);
	if (cart < xcmin) xcmin = cart;
	if (cart > xcmax) xcmax = cart;
	cart = XmaxCon*thread.DLAT[0] + YmaxCon*thread.DLAT[1] * cos(thread.DLAT[5]);
	if (cart < xcmin) xcmin = cart;
	if (cart > xcmax) xcmax = cart;
	cart = XminCon*thread.DLAT[0] + YmaxCon*thread.DLAT[1] * cos(thread.DLAT[5]);
	if (cart < xcmin) xcmin = cart;
	if (cart > xcmax) xcmax = cart;

	ycmin = ycmax = YminCon*thread.DLAT[1] * sin(thread.DLAT[5]);
	cart = YmaxCon*thread.DLAT[1] * sin(thread.DLAT[5]);
	if (cart < ycmin) ycmin = cart;
	if (cart > ycmax) ycmax = cart;

	// Compute arrays with x and y values 
	xstep = (xcmax - xcmin) / (NxCon - 1);
	ystep = (ycmax - ycmin) / (NyCon - 1);
	for (i = 0; i < NxCon; i++) x[i] = xcmin + i*xstep;
	for (i = 0; i < NyCon; i++) y[i] = ycmin + i*ystep;

	// Compute the Patterson function 
	tan5 = tan(thread.DLAT[5]);
	sin5 = sin(thread.DLAT[5]);
	zmin = zmax = 0;

	double temp_z;
	double yi_tan5;
	double yi_sin5;
	for (i = 0; i < NyCon; ++i){
		SendMessage(GetDlgItem(hwnd, IDC_PROGRESS1), PBM_SETPOS, (WPARAM)i, 0);// progress bar
		yi_tan5 = y[i] / tan5;
		yi_sin5 = y[i] / sin5 / thread.DLAT[1];

		using namespace Concurrency;
		parallel_for((UINT)0, NxCon, [&](UINT j)
		//for (j = 0; j < NxCon; ++j)
		{
			temp_z = fourier((x[j] - yi_tan5) / thread.DLAT[0], yi_sin5, Fth[2], Phase, mode);
			if (temp_z < zmin)  zmin = temp_z; //minimal contur level value
			if (temp_z > zmax)  zmax = temp_z; //maximal contur level value
			z[NxCon*i + j] = temp_z;
		});	// end j loop 
	} // end i loop


	if (zmax < zmin){  //corect for growing order
		temp = zmax;
		zmax = zmin;
		zmin = temp;
	}

	if (Nlevel < 2) Nlevel = 2;
	zstep = (zmax - zmin) / (Nlevel - 1);

	// Make the actual contour plot 
	my2D[3].SELECT_GRAPH = WINDOW_CONTUR; // 2 - contur plot
	my2D[3].RefreshWindow();
	my2D[3].lin_log = true;
	my2D[3].Contur.NxCon = NxCon;
	my2D[3].Contur.NyCon = NyCon;
	my2D[3].Contur.AllocateMemory(); //Alocate memory according to given dimensions NxCon  and NyCon. Dont copy any values to x, y, z without memory alocation
	my2D[3].Contur.zmin = zmin;
	my2D[3].Contur.zmax = zmax;
	my2D[3].Contur.zstep = zstep;
	my2D[3].Contur.Nlevel = Nlevel;
	CopyMemory(my2D[3].Contur.x, x, sizeof(double)*NxCon);
	CopyMemory(my2D[3].Contur.y, y, sizeof(double)*NyCon);
	CopyMemory(my2D[3].Contur.z, z, sizeof(double)*(NxCon*NyCon + 1));
	my2D[3].AutomaticSetTheScale();

	//unit cell contur
	my2D[3].Contur.CellVertices[0].x = 0;
	my2D[3].Contur.CellVertices[0].y = 0;
	my2D[3].Contur.CellVertices[1].x = thread.DLAT[0];
	my2D[3].Contur.CellVertices[1].y = 0;
	my2D[3].Contur.CellVertices[2].x = thread.DLAT[0] + thread.DLAT[1] * cos(thread.DLAT[5]);
	my2D[3].Contur.CellVertices[2].y = thread.DLAT[1] * sin(thread.DLAT[5]);
	my2D[3].Contur.CellVertices[3].x = thread.DLAT[1] * cos(thread.DLAT[5]);
	my2D[3].Contur.CellVertices[3].y = thread.DLAT[1] * sin(thread.DLAT[5]);
	my2D[3].Contur.CellVertices[4].x = 0;
	my2D[3].Contur.CellVertices[4].y = 0;

	//Text
	WCHAR Str[HEADER], SymStr[6];
	AtoT(SymStr, variable.PLANEGROUP);
	swprintf_s(Str, HEADER, L" zmin %4.4g, zmax %4.4g  Symmetry %s", zmin, zmax, SymStr);
	CopyMemory(my2D[3].Contur.Title, Str, sizeof(Str));
	CopyMemory(my2D[3].Contur.TitleX, L"x", sizeof(WCHAR) * 2);
	CopyMemory(my2D[3].Contur.TitleY, L"y", sizeof(WCHAR) * 2);

	if (mode == 1) swprintf_s(my2D[3].data.Title, HEADER, L"Patterson of experimental structure factors");
	if (mode == 2) swprintf_s(my2D[3].data.Title, HEADER, L"Patterson of calculated structure factors");
	if (mode == 3) swprintf_s(my2D[3].data.Title, HEADER, L"Electron density difference map");
	if (mode == 4) swprintf_s(my2D[3].data.Title, HEADER, L"Electron density of model");


	// Free the previously allocated memory
	delete[] x;
	delete[] y;
	delete[] z;

	return true;
}


/***************************************************************************/
_inline void    make_symmetry_set(int mode)
/***************************************************************************/

/*
Generate all symmetry equivalent reflections belonging to the input
structure factors.
mode 1  experimental structure factors (for Patterson)
mode 2  theoretical structure factors (for Patterson)
mode 3  use experimental structure factors for difference map
mode 4  theoretical structure factors (for electron density map)
*/
{
	int i;

	switch (mode)
	{
	case 1:
	case 3:
		for (i = 0; i < thread.NDAT; i++){
			generate_equivalents((int)thread.HDAT[i], (int)thread.KDAT[i], i);
		}
		break;

	case 2:
	case 4:
		for (i = 0; i < variable.NTH; i++){
			//generate_equivalents(variable.HTH[i],variable.KTH[i],i); //orginal 
			generate_equivalents((int)thread.HDAT[i], (int)thread.KDAT[i], i);
		}
		break;
	}
}


/***************************************************************************/
void    generate_equivalents(int h, int k, int n)
/***************************************************************************/

/*
Generate symmetry-equivalent reflections of (hk) and store these
as the n-th item in SYMSET.
*/

{

	int i, j, hnew, knew;
	int in_set;

	variable.SYMSET[n].N = 0;
	for (i = 0; i < variable.NTRANS; i++)
	{
		hnew = (int)(variable.TRANS[i][0][0] * h + variable.TRANS[i][0][1] * k);
		knew = (int)(variable.TRANS[i][1][0] * h + variable.TRANS[i][1][1] * k);

		/* Check whether this reflection, or its Friedel sister, is not allready in the set */

		in_set = FALSE;
		for (j = 0; j < variable.SYMSET[n].N; ++j)
		{
			if (((hnew == variable.SYMSET[n].H[j]) && (knew == variable.SYMSET[n].K[j])) ||
				((hnew == -variable.SYMSET[n].H[j]) && (knew == -variable.SYMSET[n].K[j])))
				in_set = TRUE;
		}
		if (!in_set)
		{
			variable.SYMSET[n].H[variable.SYMSET[n].N] = hnew;
			variable.SYMSET[n].K[variable.SYMSET[n].N] = knew;
			variable.SYMSET[n].N++;
		}
	}
}



/***************************************************************************/
double   fourier(double x, double y, double *Fthe, double *phase, int mode)
/***************************************************************************/

/*
Compute a Fourier transform function for point (x,y) in reduced
coordinates.
mode 1  Patterson of experimental structure factors
mode 2  Patterson function of theoretical structure factors
mode 3  Electron density difference map
mode 4  Electron density map of structure model

For each structrure factor, a sum is made over all symmetry-equivalent
reflections given in SYMSET
*/

{

	int i, j;
	double four = 0, temp;

	//For a time being we dont have better solution now
	variable.NTH = thread.NDAT;


	switch (mode)
	{
	case 1:
		for (i = 0; i < thread.NDAT; ++i)
		{
			if (thread.STRUCFAC) temp = thread.FDAT[i] * thread.FDAT[i];
			else	temp = thread.FDAT[i];

			for (j = 0; j < variable.SYMSET[i].N; ++j)
			{
				four += temp*cos(2.*PI*(variable.SYMSET[i].H[j] * x + variable.SYMSET[i].K[j] * y));
			}
		}
		break;
	case 2:
		for (i = 0; i < variable.NTH; ++i)//
		{
			if (thread.STRUCFAC) temp = Fthe[i] * Fthe[i];
			else	temp = Fthe[i];
			for (j = 0; j < variable.SYMSET[i].N; ++j)
			{
				four += temp*cos(2.*PI*(variable.SYMSET[i].H[j] * x + variable.SYMSET[i].K[j] * y));
			}
		}
		break;
	case 3:
		for (i = 0; i < thread.NDAT; ++i)
		{
			if (thread.STRUCFAC) temp = thread.FDAT[i] - Fthe[i];
			else	temp = sqrt(thread.FDAT[i]) - sqrt(Fthe[i]);
			for (j = 0; j < variable.SYMSET[i].N; ++j)
			{
				four += temp*cos(2.*PI*(variable.SYMSET[i].H[j] * x + variable.SYMSET[i].K[j] * y) - phase[i]);
			}
		}
		break;
	case 4:
		for (i = 0; i < variable.NTH; ++i)
		{
			if (thread.STRUCFAC) temp = Fthe[i];
			else	temp = sqrt(Fthe[i]);
			for (j = 0; j < variable.SYMSET[i].N; ++j)
			{
				four += temp * cos(2.*PI*(variable.SYMSET[i].H[j] * x + variable.SYMSET[i].K[j] * y) - phase[i]);
			}
		}
		break;
	}

	return(four);

}


void SmallRodUpdate()
{
	update_model();
	CalculateRod(variable.h, variable.k, variable.Energy, variable.l_start, variable.l_end, variable.N_points);
	CalculateInPlane(variable.h_start, variable.h_end, variable.k_start, variable.k_end, variable.l);
	FindInPlaneData(variable.l);
	FindRodData(variable.h, variable.k, variable.Energy);
	plot_zdensity(variable.ell_dens_h, variable.ell_dens_k, 0, variable.N_points);
	Multiplot(variable.Multi_SelectPlot, variable.Multi_N_column, variable.Multi_L_points);
}