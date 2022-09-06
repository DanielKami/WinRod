//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header file for plot.cpp.
//
// Writen by Daniel Kamisnki                                                                                                   
//
// 12/02/2013 Lublin
//--------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "definitions.h"


//2D plot functions
//--------------------------------------------------------------------------------------------------------------------------------
//Curves points generation
void	CalculateRod(double, double, int, double lStart = -6, double lEnd = 6, int L_points = MAXTHEO, bool AutoScale = false);
void	CalculateInPlane(double, double, double, double, double lPlane = .1, bool AutoScale = false);

//Data search and preparation for plot
void	FindInPlaneData(double lPlane = .1, bool AutoScale = false);
void	FindRodData(double hrod = 0, double krod = 0, int energy = 0 , bool AutoScale = false);

//Preparation data for ploting
void	plot_zdensity(double, double, int curve = 0, int Npoints = MAXTHEO);
bool    plot_fourier(HWND, int, unsigned int, unsigned int, double, double, double, double, unsigned int);
void	Multiplot(bool *SelectPlot, int, int L_points);
bool    plot_chi_map(HWND hwnd, int NxCon, int NyCon, int Nlevel, double *Parameter1, double LimitsParameter1Low, double LimitsParameter1Up, WCHAR *x_title, double *Parameter2, double LimitsParameter2Low, double LimitsParameter2Up, WCHAR *y_title);
//Coefficients f' and f'' plot
void	PlotCoeficients(int Ze, double energy_Start, double energy_End, int L_points);

//Functions for Fourier maps
void    generate_equivalents(int, int, int);
void    make_symmetry_set(int);
double  fourier(double, double, double*, double*, int);


void	SmallRodUpdate();