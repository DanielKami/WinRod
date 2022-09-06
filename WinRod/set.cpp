//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: set.cpp is described in its header file.
//--------------------------------------------------------------------------------------------------------------------------------
// Writen by Daniel Kaminski 
//
// 12/02/2013 Lublin																											 
//                                                      
//--------------------------------------------------------------------------------------------------------------------------------

#include <stdafx.h>
#include <stdio.h>
#include <d3dx9.h>
#include "set.h"
#include "variables.h"
#include "structures.h"
#include "calc.h"
#include "Thread.h"
#include "definitions.h"
#include "vfsr_funct.h"
#include "elements.h"
#include "ReadFile.h"
#include "UndoRedo.h"
#include "import.h"
#include "ExMath.h"
#include "2DGraph.h"
#include "Scene3D.h"

extern Graph2D						my2D[];
extern Scene3D						myScene;

extern ModelStruct Bul;
extern ModelStruct Sur;
extern ModelStruct Fit;
extern DataStruct  Dat;
extern Import	   myImport;
extern UndoRedo	   myUndoRedo;
extern VFSR_par    vfsr_par;
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
bool FindTheValues(char *String) {
	char Command[COMMLENGHT];
	char Str[COMMLENGHT];

	for (int i = 0; i < COMMLENGHT; i++) {
		Command[i] = 0;
		Str[i] = 0;
	}

	sscanf_s(String, "%s", &Command, (unsigned int)sizeof(Command));

	if (CompareStr(Command, "scale")) {
		set_parameters(0, String);
		return true;
	}

	if (CompareStr(Command, "subscale")) {
		set_parameters(1, String);
		return true;
	}

	if (CompareStr(Command, "beta")) {
		set_parameters(2, String);
		return true;
	}

	if (CompareStr(Command, "sfraction")) {
		set_parameters(3, String);
		return true;
	}

	if (CompareStr(Command, "s2fraction")) {
		set_parameters(62, String);
		return true;
	}
	if (CompareStr(Command, "displace")) {
		set_parameters(4, String);
		return true;
	}

	if (CompareStr(Command, "b1")) {
		set_parameters(5, String);
		return true;
	}

	if (CompareStr(Command, "b2")) {
		set_parameters(6, String);
		return true;
	}

	if (CompareStr(Command, "occupancy")) {
		set_parameters(7, String);
		return true;
	}

	if (CompareStr(Command, "z_mean")) {
		set_parameters(8, String);
		return true;
	}

	if (CompareStr(Command, "sigma")) {
		set_parameters(9, String);
		return true;
	}

	if (CompareStr(Command, "atten")) {
		set_parameters(10, String);
		return true;
	}

	if (CompareStr(Command, "beta")) {
		set_parameters(11, String);
		return true;
	}

	if (CompareStr(Command, "lbragg")) {
		set_parameters(12, String);
		return true;
	}

	if (CompareStr(Command, "nlayers")) {
		set_parameters(13, String);
		return true;
	}

	if (CompareStr(Command, "rough")) {
		set_parameters(14, String);
		return true;
	}

	if (CompareStr(Command, "ndomain")) {
		set_parameters(15, String);
		return true;
	}

	if (CompareStr(Command, "matrix")) {
		set_parameters(16, String);
		return true;
	}

	if (CompareStr(Command, "fractional")) {
		set_parameters(17, String);
		return true;
	}

	if (CompareStr(Command, "equal")) {
		set_parameters(18, String);
		return true;
	}

	if (CompareStr(Command, "coherent")) {
		set_parameters(19, String);
		return true;
	}

	if (CompareStr(Command, "set")) {
		set_parameters(20, String);
		return true;
	}

	if (CompareStr(Command, "indexh")) {
		set_parameters(21, String);
		return true;
	}

	if (CompareStr(Command, "indexk")) {
		set_parameters(22, String);
		return true;
	}

	if (CompareStr(Command, "indexl")) {
		set_parameters(23, String);
		return true;
	}
	if (CompareStr(Command, "occupancy2")) {
		set_parameters(60, String);
		return true;
	}
	if (CompareStr(Command, "factor")) {
		set_parameters(61, String);
		return true;
	}

	//ASA
	if (CompareStr(Command, "TEMPERATURE_RATIO_SCALE")) {
		set_parameters(24, String);
		return true;
	}
	if (CompareStr(Command, "TEMPERATURE_ANNEAL_SCALE")) {
		set_parameters(25, String);
		return true;
	}
	if (CompareStr(Command, "COST_PARAMETER_SCALE")) {
		set_parameters(26, String);
		return true;
	}

	if (CompareStr(Command, "ACCEPTED_TO_GENERATED_RATIO")) {
		set_parameters(27, String);
		return true;
	}

	if (CompareStr(Command, "INITIAL_PARAMETER_TEMPERATURE")) {
		set_parameters(28, String);
		return true;
	}

	if (CompareStr(Command, "DELTA_X")) {
		set_parameters(29, String);
		return true;
	}

	if (CompareStr(Command, "LIMIT_ACCEPTANCES")) {
		set_parameters(30, String);
		return true;
	}

	if (CompareStr(Command, "TESTING_FREQUENCY_MODULUS")) {
		set_parameters(31, String);
		return true;
	}

	if (CompareStr(Command, "bul_fractional")) {
		set_parameters(33, String);
		return true;
	}

	//Which error metod to use
	if (CompareStr(Command, "LSQ_ERRORCALC")) {
		set_parameters(35, String);
		return true;
	}
	//Rotations
	if (CompareStr(Command, "rotation")) {
		set_parameters(40, String);
		return true;
	}
	//Domain occ.
	if (CompareStr(Command, "domain_occ")) {
		set_parameters(41, String);
		return true;
	}
	//Rotations
	if (CompareStr(Command, "dispersion")) {
		set_parameters(45, String);
		return true;
	}

	//hstart
	if (CompareStr(Command, "hstart")) {
		set_parameters(50, String);
		return true;
	}

	//hend
	if (CompareStr(Command, "hend")) {
		set_parameters(51, String);
		return true;
	}
	//kstart
	if (CompareStr(Command, "kstart")) {
		set_parameters(52, String);
		return true;
	}

	//kend
	if (CompareStr(Command, "kend")) {
		set_parameters(53, String);
		return true;
	}
	//lstart
	if (CompareStr(Command, "lstart")) {
		set_parameters(54, String);
		return true;
	}

	//lend
	if (CompareStr(Command, "lend")) {
		set_parameters(55, String);
		return true;
	}

	//lend
	if (CompareStr(Command, "nsurf2")) {
		set_parameters(56, String);
		return true;
	}

	//LIQUID_PROFILE
	if (CompareStr(Command, "liquid_profile")) {
		set_parameters(59, String);
		return true;
	}
	//NSurf2
	if (CompareStr(Command, "nsurf2")) {
		set_parameters(62, String);
		return true;
	}
	/////Specyfic WinRod commands///////

	  //Priority
	if (CompareStr(Command, "Priority")) {
		set_parametersWinRod(1, String);
		return true;
	}

	//Multiplot
	if (CompareStr(Command, "Multiplot")) {
		set_parametersWinRod(3, String);
		return true;
	}
	if (CompareStr(Command, "Multiplot_select")) {
		set_parametersWinRod(4, String);
		return true;
	}

	if (CompareStr(Command, "windows_set")) {
		set_parametersWinRod(5, String);
		return true;
	}

	if (CompareStr(Command, "Procesor_cores")) {
		set_parametersWinRod(7, String);
		return true;
	}

	if (CompareStr(Command, "Fit_file")) {
		set_parametersWinRod(10, String);
		return true;
	}

	if (CompareStr(Command, "Window_colors")) {
		set_parametersWinRod(15, String);
		return true;
	}

	if (CompareStr(Command, "3Drepetitions")) {
		set_parametersWinRod(16, String);
		return true;
	}
	/*
	  //keating
	  sprintf_s(Str, COMMLENGHT, "alpha"); //for domains symmetry
	  if(CompareStr(Command, Str)){
		 set_parameters(20, String);
		 return true;
	  }

	  sprintf_s(Str, COMMLENGHT, "alpha"); //for domains symmetry
	  if(CompareStr(Command, Str)){
		 set_parameters(21, String);
		 return true;
	  }

	  sprintf_s(Str, COMMLENGHT, "equangle"); //for domains symmetry
	  if(CompareStr(Command, Str)){
		 set_parameters(22, String);
		 return true;
	*/


	return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
bool CompareStr(char *InString, char *ConstString){
//--------------------------------------------------------------------------------------------------------------------------------
	int Count = 0;
	size_t lenght = strlen(ConstString);
	size_t lenght2 = strlen(InString);
	if(lenght!=lenght2) return false;

	if (lenght == 0) return false;
	if (lenght > COMMLENGHT) return false;

	for(int i = 0; i < (int)lenght; ++i){
		if (InString[i] == ConstString[i]) Count++;
		else return false;
	}

	if(Count == lenght)return true;
	//We could not find the match
	return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
bool CompareStr(WCHAR *Comm, WCHAR *String){
//--------------------------------------------------------------------------------------------------------------------------------

	int Count = 0;
	size_t lenght = wcslen(String);
	if (lenght == 0) return false;
	if (lenght > COMMLENGHT) return false;

	for(int i = 0; i < (int)lenght; i++){
		if (Comm[i] == String[i]) Count++;
		else return false;
	}

	if(Count == lenght)return true;
	//We could not find the match
	return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
void    set_parameters(int Interpret, char *Line){
	//--------------------------------------------------------------------------------------------------------------------------------

	char Command[100];
	char Command2[100];
	float tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
	int tmpi = 0, tmpi2 = 0, tmpi3 = 0;
	char yesnostr[4];
	int npar;

	switch (Interpret)
	{

		//Scale factor of theory 
	case 0:				//command, value, up_limit, low_limit, yesno
		sscanf_s((char*)Line, "%s %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		thread.SCALE = (double)tmp1; variable.SCALELIM[0] = (double)tmp2; variable.SCALELIM[1] = (double)tmp3; variable.SCALEPEN = TestYesNo(yesnostr);
		break;

		//subscale factor of theory
	case 1:
		sscanf_s((char*)Line, "%s %d %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		npar = tmpi;
		if ((npar < 1) || (npar > variable.NSUBSCALETOT)){
			//errtype("ERROR, no such serial number in model");
			break;
		}
		thread.SUBSCALE[npar - 1] = (double)tmp1; variable.SUBSCALELIM[npar - 1][0] = (double)tmp2; variable.SUBSCALELIM[npar - 1][1] = (double)tmp3; variable.SUBSCALEPEN[npar - 1] = TestYesNo(yesnostr);
		break;

		//Roughness parameter beta
	case 2:
		sscanf_s((char*)Line, "%s %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		thread.BETA = (double)tmp1; variable.BETALIM[0] = (double)tmp2; variable.BETALIM[1] = (double)tmp3; variable.BETAPEN = TestYesNo(yesnostr);
		break;

		//Reconstructed-surface fraction
	case 3:
		sscanf_s((char*)Line, "%s %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		thread.SURFFRAC = (double)tmp1; variable.SURFFRACLIM[0] = (double)tmp2; variable.SURFFRACLIM[1] = (double)tmp3; variable.SURFFRACPEN = TestYesNo(yesnostr);
		break;


		//Serial number of displacement parameter:
	case 4:
		sscanf_s((char*)Line, "%s %d %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		npar = tmpi;

		if ((npar < 1) || (npar > variable.NDISTOT)){
			//errtype("ERROR, no such serial number in model"); //todo
			break;
		}
		variable.DISPL[npar - 1] = (double)tmp1; variable.DISPLLIM[npar - 1][0] = (double)tmp2; variable.DISPLLIM[npar - 1][1] = (double)tmp3; variable.DISPLPEN[npar - 1] = TestYesNo(yesnostr);
		break;


		//Serial number of parallel Debye-Waller parameter:
	case 5:
		sscanf_s((char*)Line, "%s %d %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		npar = tmpi;
		if ((npar < 1) || (npar > thread.NDWTOT)){
			//errtype("ERROR, no such serial number in model");
			break;
		}
		thread.DEBWAL[npar - 1] = (double)tmp1; variable.DEBWALLIM[npar - 1][0] = (double)tmp2; variable.DEBWALLIM[npar - 1][1] = (double)tmp3; variable.DEBWALPEN[npar - 1] = TestYesNo(yesnostr);
		break;


		//Serial number of perpendicular Debye-Waller parameter:
	case 6:
		sscanf_s((char*)Line, "%s %d %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		npar = tmpi;
		if ((npar < 1) || (npar > thread.NDWTOT2)){
			//errtype("ERROR, no such serial number in model");
			break;
		}
		thread.DEBWAL2[npar - 1] = (double)tmp1; variable.DEBWAL2LIM[npar - 1][0] = (double)tmp2; variable.DEBWAL2LIM[npar - 1][1] = (double)tmp3; variable.DEBWAL2PEN[npar - 1] = TestYesNo(yesnostr);
		break;

		//Serial number of occupancy parameter:
	case 7:
		sscanf_s((char*)Line, "%s %d %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		npar = tmpi;
		if ((npar < 1) || (npar > variable.NOCCTOT)){
			//errtype("ERROR, no such serial number in model");
			break;
		}
		thread.OCCUP[npar - 1] = (double)tmp1; variable.OCCUPLIM[npar - 1][0] = (double)tmp2; variable.OCCUPLIM[npar - 1][1] = (double)tmp3; variable.OCCUPPEN[npar - 1] = TestYesNo(yesnostr);
		break;

		//z_mean for liquid corection
	case 8:
		sscanf_s((char*)Line, "%s %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		variable.z_mean = (double)tmp1; variable.z_meanLIM[0] = (double)tmp2; variable.z_meanLIM[1] = (double)tmp3; variable.z_meanPEN = TestYesNo(yesnostr);
		break;

		//sigma for liquid contribution
	case 9:
		sscanf_s((char*)Line, "%s %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		variable.sigma = (double)tmp1; variable.sigmaLIM[0] = (double)tmp2; variable.sigmaLIM[1] = (double)tmp3; variable.z_meanPEN = TestYesNo(yesnostr);
		break;

		//atenuation factor
	case 10:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		thread.ATTEN = (double)tmp1;
		break;

		//beta roughness
	case 11:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		thread.BETA = (double)tmp1;
		break;

		//l-bragg
	case 12:
		sscanf_s((char*)Line, "%s %d", &Command, (unsigned int)sizeof(Command), &tmpi);
		thread.LBR[0] = tmpi;
		break;

		//n layers for roughness calculations
	case 13:
		sscanf_s((char*)Line, "%s %d", &Command, (unsigned int)sizeof(Command), &tmpi);
		thread.NLAYERS = tmpi;
		break;

		//roughness model
	case 14:
		sscanf_s((char*)Line, "%s %20s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command));

		if (CompareStr(Command, "approx")){
			thread.ROUGHMODEL = APPROXBETA;
			break;
		}


		if (CompareStr(Command, "poisson")){
			thread.ROUGHMODEL = POISSONROUGH;
			break;
		}

		if (CompareStr(Command, "gaussian")){
			thread.ROUGHMODEL = GAUSSROUGH;
			break;
		}

		if (CompareStr(Command, "linear")){
			thread.ROUGHMODEL = LINEARROUGH;
			break;
		}

		if (CompareStr(Command, "cosine")){
			thread.ROUGHMODEL = COSINEROUGH;
			break;
		}

		if (CompareStr(Command, "twolevel")){
			thread.ROUGHMODEL = TWOLEVEL;
			break;
		}
		break;

		//n domains
	case 15:
		sscanf_s((char*)Line, "%s %d", &Command, (unsigned int)sizeof(Command), &tmpi);
		thread.NDOMAIN = tmpi;
		break;

		//n matrix settinigs
	case 16:
		sscanf_s((char*)Line, "%s %d %f %f %f %f", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &tmp4);
		if (tmpi > 0 && tmpi < MAXDOM){
			tmpi -= 1;
			thread.DOMMAT11[tmpi] = (double)tmp1; thread.DOMMAT12[tmpi] = (double)tmp2; thread.DOMMAT21[tmpi] = (double)tmp3; thread.DOMMAT22[tmpi] = (double)tmp4;
		}
		break;


		//fractional reflections settinigs for surface
	case 17:
		sscanf_s((char*)Line, "%s %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command));
		if (CompareStr(Command, "yes")){
			thread.ZEROFRACT = true;
			variable.FRACTIONAL = true;
		}
		else
		{
			variable.FRACTIONAL = false;
			thread.ZEROFRACT = false;
		}
		break;

		//domains equal
	case 18:
		sscanf_s((char*)Line, "%s %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command));
		if (CompareStr(Command, "yes"))
			variable.DOMEQUAL = true;
		else
			variable.DOMEQUAL = false;
		break;

		//domains coherently added
	case 19:
		sscanf_s((char*)Line, "%s %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command));
		if (CompareStr(Command, "yes"))
			thread.COHERENTDOMAINS = true;
		else
			thread.COHERENTDOMAINS = false;
		break;

		//set symmetry
	case 20:
		sscanf_s((char*)Line, "%s %s %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command), &Command2, (unsigned int)sizeof(Command2));
		char Str[5];
		if (CompareStr(Command, "symmetry")){
			for (int i = 0; i < 18; i++){
				sprintf_s(Str, 5, "%s", symmetry[i]); //for domains symmetry
				if (CompareStr(Command2, Str)){
					CopyMemory(variable.PLANEGROUP, Str, (unsigned int)sizeof(char) * 5);
					set_symmetry(0, i);
					break;
				}
			}

		}
		break;

	case 21:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.h = tmp1;
		break;

	case 22:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.k = tmp1;
		break;

	case 23:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.l = tmp1;
		break;

	case 24:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.TEMPERATURE_RATIO_SCALE = tmp1;
		break;

	case 25:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.TEMPERATURE_ANNEAL_SCALE = tmp1;
		break;

	case 26:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.COST_PARAMETER_SCALE = tmp1;
		break;

	case 27:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.ACCEPTED_TO_GENERATED_RATIO = tmp1;
		break;

	case 28:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.INITIAL_PARAMETER_TEMPERATURE = tmp1;
		break;

	case 29:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.DELTA_X = tmp1;
		break;

	case 30:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.LIMIT_ACCEPTANCES = (int)tmp1;
		break;

	case 31:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		vfsr_par.TESTING_FREQUENCY_MODULUS = (int)tmp1;
		break;


		//fractional reflections settinigs
	case 33:
		sscanf_s((char*)Line, "%s %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command));
		if (CompareStr(Command, "yes")){
			thread.BULK_ZEROFRACT = true;
		}
		else
			thread.BULK_ZEROFRACT = false;

		break;

		//Error calculations method
	case 35:
		sscanf_s((char*)Line, "%s %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command));
		if (CompareStr(Command, "CHISQR"))
			variable.LSQ_ERRORCALC = CHISQR;
		if (CompareStr(Command, "COVARIANCE"))
			variable.LSQ_ERRORCALC = COVARIANCE;
		break;

		//Rotations
	case 40:
		sscanf_s((char*)Line, "%s %s %d    %f %f %f %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		tmpi--;
		if (tmpi >= 0 && tmpi < MAXPAR)
		{
			if (CompareStr(Command, "x"))
			{
				variable.ROTATIONX[tmpi] = tmp1;
				variable.ROTATIONLIMX[tmpi][0] = tmp2;
				variable.ROTATIONLIMX[tmpi][1] = tmp3;
				variable.ROTATIONPENX[tmpi] = TestYesNo(yesnostr);
				break;
			}
			if (CompareStr(Command, "y"))
			{
				variable.ROTATIONY[tmpi] = tmp1;
				variable.ROTATIONLIMY[tmpi][0] = tmp2;
				variable.ROTATIONLIMY[tmpi][1] = tmp3;
				variable.ROTATIONPENY[tmpi] = TestYesNo(yesnostr);
			}
			if (CompareStr(Command, "z"))
			{
				variable.ROTATIONZ[tmpi] = tmp1;
				variable.ROTATIONLIMZ[tmpi][0] = tmp2;
				variable.ROTATIONLIMZ[tmpi][1] = tmp3;
				variable.ROTATIONPENZ[tmpi] = TestYesNo(yesnostr);
			}
		}
		break;

		//Occ. domains
	case 41:
		sscanf_s((char*)Line, "%s %d    %f %f %f %s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		tmpi--;
		if (tmpi >= 0 && tmpi < MAXPAR)
		{
			variable.DOMOCCUPAUTO = true;
			thread.DOMOCCUP[tmpi] = tmp1;
			variable.DOMOCCUPLIM[tmpi][1] = tmp3;
			variable.DOMOCCUPPEN[tmpi] = TestYesNo(yesnostr);
			break;

		}
		break;

		//Dispersion "dispersion %2s %2d %8.4f %8.4f
	case 45:
		sscanf_s((char*)Line, "%s %s  %d  %f %f", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2);

		int i;
		for (i = 0; i < thread.NTYPES; ++i)
		{
			if (CompareStr(Command, variable.ELEMENT[i]))
				break;
		}
		if (tmpi >= 0 && tmpi < MAXENERGIES)
		{
			thread.F1[i][tmpi] = tmp1;
			thread.F2[i][tmpi] = tmp2;
		}
		break;
	//h,k,l -start ,-end
	case 50:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.h_start = (int)tmp1;
		break;
	case 51:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.h_end = (int)tmp1;
		break;
	case 52:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.k_start = (int)tmp1;
		break;
	case 53:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.k_end = (int)tmp1;
		break;
	case 54:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.l_start = (int)tmp1;
		break;
	case 55:
		sscanf_s((char*)Line, "%s %f", &Command, (unsigned int)sizeof(Command), &tmp1);
		variable.l_end = (int)tmp1;
		break;


	//NSURF2 have to be read after fit file to take effect
	case 56:
		sscanf_s((char*)Line, "%s %d", &Command, (unsigned int)sizeof(Command), &tmpi);
		
		if (tmpi > 0 && tmpi < thread.NSURF)
		{
			thread.NSURF = variable.NSURFTOT - tmpi;
			thread.NSURF2 = tmpi;
		}
		break;

		//NSURF2 have to be read after fit file to take effect
	case 59:
		sscanf_s((char*)Line, "%s %s", &Command, (unsigned int)sizeof(Command), &yesnostr[0], (unsigned int)sizeof(yesnostr));

		if (CompareStr(yesnostr, "yes") || CompareStr(yesnostr, "YES"))
			thread.LIQUID_PROFILE = true;
		else
			thread.LIQUID_PROFILE = false;
		break;

		//Serial number of occupancy2 parameter:
	case 60:
		sscanf_s((char*)Line, "%s %d %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		npar = tmpi;
		if ((npar < 1) || (npar > variable.NOCCTOT2)) {
			//errtype("ERROR, no such serial number in model");
			break;
		}
		thread.OCCUP2[npar - 1] = (double)tmp1; variable.OCCUP2LIM[npar - 1][0] = (double)tmp2; variable.OCCUP2LIM[npar - 1][1] = (double)tmp3; variable.OCCUP2PEN[npar - 1] = TestYesNo(yesnostr);
		break;

	case 61:
		sscanf_s((char*)Line, "%s %d %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		npar = tmpi;
		if ((npar < 1) || (npar > 3)) {
			//errtype("ERROR, no such serial number in model");
			break;
		}
		thread.FACTOR[npar - 1] = (double)tmp1; variable.FACTOR_LIM[npar - 1][0] = (double)tmp2; variable.FACTOR_LIM[npar - 1][1] = (double)tmp3; variable.FACTOR_PEN[npar - 1] = TestYesNo(yesnostr);
		//Increase the number of FACTOR parameters
		if (npar >variable.NFACTOR)
			variable.NFACTOR = npar;
		break;


		//Reconstructed-surface fraction
	case 62:
		sscanf_s((char*)Line, "%s %f %f %f %4s", &Command, (unsigned int)sizeof(Command), &tmp1, &tmp2, &tmp3, &yesnostr[0], (unsigned int)sizeof(yesnostr));
		thread.SURF2FRAC = (double)tmp1; variable.SURF2FRACLIM[0] = (double)tmp2; variable.SURF2FRACLIM[1] = (double)tmp3; variable.SURF2FRACPEN = TestYesNo(yesnostr);
		break;
	}
}


void    set_parametersWinRod(int Interpret, char *Line)
{
	char Command[100];
	char Command2[100];
	float tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
	int tmpi = 0, tmpi2 = 0, tmpi3 = 0, tmpi4 = 0, tmpi5 = 0;
	char yesnostr[4];
	int npar;

	switch (Interpret)
	{

	case 1:
		sscanf_s((char*)Line, "%s %s", &Command, (unsigned int)sizeof(Command), &Command, (unsigned int)sizeof(Command));

		if (CompareStr(Command, "THREAD_PRIORITY_LOWEST"))
			thread.priority = THREAD_PRIORITY_LOWEST;

		if (CompareStr(Command, "THREAD_PRIORITY_BELOW_NORMAL"))
			thread.priority = THREAD_PRIORITY_BELOW_NORMAL;

		if (CompareStr(Command, "THREAD_PRIORITY_NORMAL"))
			thread.priority = THREAD_PRIORITY_NORMAL;

		if (CompareStr(Command, "THREAD_PRIORITY_ABOVE_NORMAL"))
			thread.priority = THREAD_PRIORITY_ABOVE_NORMAL;

		if (CompareStr(Command, "THREAD_PRIORITY_HIGHEST"))
			thread.priority = THREAD_PRIORITY_HIGHEST;

		
		break;

		//Multiplot
	case 3:
		sscanf_s((char*)Line, "%s  %d %d %d", &Command, (unsigned int)sizeof(Command), &tmpi, &tmpi2, &tmpi3);

		if (tmpi >= 0 && tmpi < MAXMULTIPLOT)
			variable.NumberOfMultiplots = tmpi;
		else
			variable.NumberOfMultiplots = 0;
		if (tmpi2 > 0 && tmpi2 < 10)
			variable.Multi_N_column = tmpi2;
		else
			variable.Multi_N_column = 1;
		if (tmpi3 >= 0 && tmpi3 < MAXTHEO)
		{
			variable.Multi_L_points = tmpi3;
		}
		else
			variable.Multi_L_points = 0;

		break;

	case 4:
		sscanf_s((char*)Line, "%s %d %s", &Command, (unsigned int)sizeof(Command), &tmpi, &yesnostr[0], (unsigned int)sizeof(yesnostr));

		if (tmpi >= 0 && tmpi < MAXMULTIPLOT)
		{
			variable.Multi_SelectPlot[tmpi] = TestYesNo(yesnostr);
		}
		break;

	//Windows settings
	case 5:
		sscanf_s((char*)Line, "%s %d %f %f %f %f %s", &Command, (unsigned int)sizeof(Command), &tmpi, &tmp1, &tmp2, &tmp3, &tmp4, &Command2, (unsigned int)sizeof(Command2));

		if ((int)tmpi == 0) break;
		npar = (int)tmpi - 1;

		if (npar > MAX_WINDOWS_NUMBER) break;
		if (tmp1 < 1.)tmp1 = 1.;
		if (tmp2 < 1.)tmp2 = 1.;
		if (tmp3 - tmp1 < 1)tmp3 = tmp1 + 1;
		if (tmp4 - tmp2 < 1)tmp4 = tmp2 + 1;

		variable.WindowsRect[npar].left = (int)tmp1;
		variable.WindowsRect[npar].top = (int)tmp2;
		variable.WindowsRect[npar].right = (int)tmp3;
		variable.WindowsRect[npar].bottom = (int)tmp4;

		if (CompareStr(Command2, "yes"))
			variable.WindowsVisible[npar] = true;
		else
			variable.WindowsVisible[npar] = false;
		break;

	//Procesor cores
	case 7:
		sscanf_s((char*)Line, "%s %d", &Command, (unsigned int)sizeof(Command), &tmpi);

		if (tmpi > 0 && tmp1 < 8)
			thread.NumberOfCores = tmpi;
		break;

	//Fit file details
	case 10:
		sscanf_s((char*)Line, "%s %d %s %s", &Command, (unsigned int)sizeof(Command), &tmpi, variable.NameTestFitFile, (unsigned int)sizeof(variable.NameTestFitFile), &Command2, (unsigned int)sizeof(Command2));
		if (CompareStr(Command2, "yes"))
			variable.PrintTestFitFile = true;
		else
			variable.PrintTestFitFile = false;

		if (tmpi > 0 && tmp1 < 6)
			variable.DetailsInFile = tmpi;
		break;
	
		//Windows colors
	case 15:
		sscanf_s((char*)Line, "%s %d %d %d %d", &Command, (unsigned int)sizeof(Command), &tmpi, &tmpi2, &tmpi3, &tmpi4);

		if (tmpi == 0)
		{
			myScene.flag.rgbCurrent = tmpi2; myScene.flag.rgbLight = tmpi3; myScene.flag.rgbText = tmpi4;
		}
		else
		{
			if (tmpi>0 && tmpi < TOTAL_2D_WINDOWS+1)
			{
				my2D[tmpi - 1].rgbColors.background = tmpi2; my2D[tmpi - 1].rgbColors.framecolor = tmpi3; my2D[tmpi-1].rgbColors.text = tmpi4;
			}
		}

		break;
	
	case 16:
		sscanf_s((char*)Line, "%s %d %d %d %d %d", &Command, (unsigned int)sizeof(Command), &tmpi, &tmpi2, &tmpi3, &tmpi4, &tmpi5);

		
		if (tmpi != 0)
			myScene.g_BRepetitionX = tmpi;
		if (tmpi2 != 0)
			myScene.g_BRepetitionY = tmpi2;
		if (tmpi3 != 0)
			myScene.g_BRepetitionZ = tmpi3;
		if (tmpi4 != 0)
			myScene.g_SRepetitionX = tmpi4;
		if (tmpi5 != 0)
			myScene.g_SRepetitionY = tmpi5;

		break;
	}

}

//--------------------------------------------------------------------------------------------------------------------------------
bool TestYesNo(char *YesNoStr){
//--------------------------------------------------------------------------------------------------------------------------------
	if(YesNoStr[0] == 'Y' && YesNoStr[1] == 'E' && YesNoStr[2] == 'S' ||
		YesNoStr[0] == 'y' && YesNoStr[1] == 'e' && YesNoStr[2] == 's') return true;

return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
void Reset_Rod(void)
{

	/* Reset some global parameters */
	thread.priority = THREAD_PRIORITY_NORMAL; //THREAD_PRIORITY_LOWEST  THREAD_PRIORITY_BELOW_NORMAL
	thread.NumberOfCores = 1;

	ZeroMemory(&thread.DLAT, sizeof(thread.DLAT));
	ZeroMemory(&thread.RLAT, sizeof(thread.RLAT));

	thread.NDOMAIN = 1;
	thread.SCALE = 1;
	thread.ATTEN = 0.001;
	thread.COHERENTDOMAINS = false;
	thread.SCALE_ERR = 0;
	thread.ZEROFRACT = true;
	thread.BULK_ZEROFRACT = true;
	thread.NSURF = 0;
	thread.NDWTOT = 0;
	thread.NDWTOT2 = 0;
	thread.NBULK = 0;
	thread.BETA_ERR = 0;
	thread.BETA = 0;
	thread.ROUGHMODEL = APPROXBETA;
	thread.NLAYERS = 1;
	thread.SURFFRAC = 1;
	thread.SURF2FRAC = 0;
	thread.NSURF2 = 0;
	thread.STRUCFAC = true;
	thread.NTYPES = 0;
	thread.NDAT = 0;


	for (int i = 0; i < MAXDOM; i++)
	{
		thread.DOMMAT11[i] = 1.;
		thread.DOMMAT12[i] = 0.;
		thread.DOMMAT21[i] = 0.;
		thread.DOMMAT22[i] = 1.;
	}

	for (int i = 0; i < 6; i++)
	{
		thread.DLAT[i] = 0.;
		thread.RLAT[i] = 0.;
	}

	thread.DOMOCCUP[0] = 1.;
	thread.ZEROFRACT = false;
	variable.DOMEQUAL = true;


	for (int i = 0; i < MAXPAR; ++i)
	{
		thread.OCCUP_FLAG[i] = false;
		thread.DEBWAL[i] = 0;
		thread.DEBWAL2[i] = 0;
	}

	for (int i = 0; i < MAXATOMS; ++i)
	{
		thread.NDWB[i] = 0;
		thread.NDWS[i] = 0;
		thread.NDWS2[i] = 0;
	}

#ifdef PROFILE
	thread.LIQUID_PROFILE = false;     //Calcultae the liquid profile yes/no
#endif

	InitializeScateringFactors();

	myScene.g_SRepetitionX = 3;
	myScene.g_SRepetitionY = 3;
	myScene.g_BRepetitionX = 3;
	myScene.g_BRepetitionY = 3;
	myScene.g_BRepetitionZ = 0;

	//Variables
	variable.MODE = 0;
	variable.NDISTOT = 0;
	variable.NSURFTOT = 0;
	variable.NOCCTOT = 0;
	variable.ENERGYTOT = 0;
	variable.NSUBSCALETOT = 0;
	variable.ROTATIONTOTX = 0;
	variable.ROTATIONTOTY = 0;
	variable.ROTATIONTOTZ = 0;
	variable.DOMEQUAL = true;
	variable.NTH = 800;
	variable.LSQ_ERRORCALC = CHISQR;//  CHISQR/COVARIANCE
	variable.N_points = 800;
	variable.NFACTOR = 0;

	//Reset rotations
	for (int i = 0; i < MAXPAR; ++i)
	{
		variable.ROTATIONX[i] = variable.ROTATIONY[i] = variable.ROTATIONZ[i] = 0;
	}

	//Starting values for calculations
	variable.h = 0.0;		variable.k = 0.0;		variable.l = 0.3;
	variable.h_start = -5.; variable.k_start = -5.; variable.l_start = -5.;
	variable.h_end = 5.;	variable.k_end = 5.;	variable.l_end = 5.;


	variable.DOMOCCUPAUTO = false;
	//Domain occupancy 1 is never fitted
	variable.DOMOCCUPLIM[0][0] = variable.DOMOCCUPLIM[0][1] = 0;
	variable.DOMOCCUPPEN[0] = false;

	variable.BRAGG_WEIGHT_DONE = false;

	for (int i = 0; i < MAXTYPES; ++i)
	{
		for (int j = 0; j < MAXENERGIES; ++j)
		{
			thread.F1[i][j] = thread.F2[i][j] = 0.;
		}
	}

	//Multiplot
	variable.Multi_N_column = 3;
	variable.Multi_L_points = MAXTHEO / 16;
	variable.Multi_SelectPlot[MAXMULTIPLOT];
	for (int i = 0; i < MAXMULTIPLOT; ++i)
	{
		variable.Multi_SelectPlot[i] = false;
	}

	variable.PrintTestFitFile = true;
	variable.DetailsInFile = 1;
	
	sprintf_s(variable.NameTestFitFile, MAX_FILE_NAME, "FitResults.txt");
	ResetErrors();

	//Molecules
	myUndoRedo.MaximalPosition = myUndoRedo.CurentPosition = 0;
	myImport.TotalMolecules = 0;

	//3D rotaation speed
	thread.Refresh3DIdle = 3;

	for (int i = 0; i < MAX_WINDOWS_NUMBER; i++)
	{
		variable.WindowsRect[i].bottom = 0;
		variable.WindowsRect[i].top = 0;
		variable.WindowsRect[i].left = 0;
		variable.WindowsRect[i].right = 0;
		variable.WindowsVisible[i] = false;
	}

	//Data reduction
	variable.reduction_step=3;
	variable.reduction_minF=0.2;
	variable.reduction_MaxL=0.4;
}

void ResetErrors()
{
	int i;
	//zero errors
	thread.SCALE_ERR = 0;
	thread.BETA_ERR = 0;
	thread.SURFFRAC_ERR = 0;

	for (i = 0; i < MAXPAR; ++i){
		variable.DISPL_ERR[i] = 0;
	}
	for (i = 0; i < MAXPAR; ++i){
		thread.DEBWAL_ERR[i] = 0;
	}
	for (i = 0; i < MAXPAR; ++i){
		thread.DEBWAL2_ERR[i] = 0;
	}
	for (i = 0; i < MAXPAR; ++i){
		thread.OCCUP_ERR[i] = 0;
	}

	for (i = 0; i < MAXPAR; ++i) {
		thread.OCCUP2_ERR[i] = 0;
	}

	for (i = 0; i < MAXPAR; ++i) {
		thread.FACTOR_ERR[i] = 0;
	}

	for (i = 0; i < MAXPAR; ++i){
		thread.SUBSCALE_ERR[i] = 0;
	}

	for (i = 0; i < MAXPAR; ++i){
		variable.ROTATION_ERRX[i] = 0;
	}

	for (i = 0; i < MAXPAR; ++i){
		variable.ROTATION_ERRY[i] = 0;
	}

	for (i = 0; i < MAXPAR; ++i){
		variable.ROTATION_ERRZ[i] = 0;
	}

	for (i = 0; i < MAXDOM; ++i){
		variable.DOMOCCUP_ERR[i + 1] = 0;
	}

}

//--------------------------------------------------------------------------------------------------------------------------------
// Domain rotation matrices by using quaternions.
// Notes: Under construction
// 09.02.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void CalculateDomains(double angle){

//quaternions for unit rotations
D3DXQUATERNION LocalRotQuadZ;
D3DXVECTOR3 RotateAroundZ=D3DXVECTOR3(1,0,0);
D3DXMATRIX rotate;
//Rotate around Z axis
D3DXQuaternionRotationAxis(  &LocalRotQuadZ, 
							 &RotateAroundZ,
							 (float)angle);



//build rotation matrix from quaternion
D3DXMatrixRotationQuaternion(&rotate,
							 &LocalRotQuadZ);

}

/***************************************************************************/
void    set_symmetry(HWND hwnd, int nplanegroup){
/***************************************************************************/

    /*
    Set plane group symmetry of crystal structure data menu.
    When generating symmetry-equivalent reflections, the program will
    in addition look for Friedel pairs (most important for l = 0).
    */

    

    /* Define transformation matrices that generate equivalent reflections
       for the various plane groups */

    static int  p1[4] = {1,0,0,1};                                   /*  1 */
    static int  p2[8] = {1,0,0,1, -1,0,0,-1};                        /*  2 */
    static int  pm[8] = {1,0,0,1, 1,0,0,-1};                         /*  3 */
    int			*pg = pm;                                            /*  4 */
    int			*cm = pm;                                            /*  5 */
    static int  p2mm[16] = {1,0,0,1, -1,0,0,-1, -1,0,0,1, 1,0,0,-1}; /*  6 */
    int			*p2mg = p2mm;                                        /*  7 */
    int			*p2gg = p2mm;                                        /*  8 */
    int			*c2mm = p2mm;                                        /*  9 */
    static int  p4[16] = {1,0,0,1, -1,0,0,-1, 0,-1,1,0, 0,1,-1,0};   /* 10 */
    static int  p4mm[32] = {1,0,0,1, -1,0,0,-1, -1,0,0,1, 1,0,0,-1,  /* 11 */
			    0,1,1,0, 0,-1,-1,0, 0,1,-1,0, 0,-1,1,0};
    int			*p4gm = p4mm;                                        /* 12 */
    static int  p3[12] = {1,0,0,1, 0,1,-1,-1, -1,-1,1,0};            /* 13 */
    static int  p3m1[24] = {1,0,0,1, 0,1,-1,-1, -1,-1,1,0, 0,-1,-1,0,/* 14 */
			    1,1,0,-1, -1,0,1,1};
    static int  p31m[24] = {1,0,0,1, 0,1,-1,-1, -1,-1,1,0, 0,1,1,0,  /* 15 */
			    -1,-1,0,1, 1,0,-1,-1};
    static int  p6[24] = {1,0,0,1, 0,1,-1,-1, -1,-1,1,0, -1,0,0,-1,  /* 16 */
			    0,-1,1,1, 1,1,-1,0};
    static int  p6mm[48] = {1,0,0,1, 0,1,-1,-1, -1,-1,1,0, -1,0,0,-1,/* 17 */
			    0,-1,1,1, 1,1,-1,0, 0,1,1,0, -1,-1,0,1,
			    1,0,-1,-1, 0,-1,-1,0, 1,1,0,-1, -1,0,1,1};

    static int  n_matrices[17] = {1,2,2,2,2,4,4,4,4,4,8,8,3,6,6,6,12};

    int     i;
    static int  *matrix;

 	//nplanegroup = ncom; -take the number from the list position
	//sprintf(PLANEGROUP,"%s",set_sym_menu[nplanegroup-1].COMMAND);
	variable.NTRANS = n_matrices[nplanegroup-1];//number of equivalent positions (matrices) for given symmetry
		
	//Copy the matrices to the temporary
	if (nplanegroup == 1) matrix = p1;
	if (nplanegroup == 2) matrix = p2;
	if (nplanegroup == 3) matrix = pm;
	if (nplanegroup == 4) matrix = pg;
	if (nplanegroup == 5) matrix = cm;
	if (nplanegroup == 6) matrix = p2mm;
	if (nplanegroup == 7) matrix = p2mg;
	if (nplanegroup == 8) matrix = p2gg;
	if (nplanegroup == 9) matrix = c2mm;
	if (nplanegroup == 10) matrix = p4;
	if (nplanegroup == 11) matrix = p4mm;
	if (nplanegroup == 12) matrix = p4gm;
	if (nplanegroup == 13) matrix = p3;
	if (nplanegroup == 14) matrix = p3m1;
	if (nplanegroup == 15) matrix = p31m;
	if (nplanegroup == 16) matrix = p6;
	if (nplanegroup == 17) matrix = p6mm;

	//fill all the important matrices with 4 elements
	for (i = 0; i < n_matrices[nplanegroup-1]; i++){
		variable.TRANS[i][0][0] = matrix[i*4];
		variable.TRANS[i][0][1] = matrix[i*4+1];
		variable.TRANS[i][1][0] = matrix[i*4+2];
		variable.TRANS[i][1][1] = matrix[i*4+3];
	}



		//If no symetry chosen then make a warning
		if (nplanegroup == 0){
				MessageBox(NULL, TEXT("No plane group set!"), NULL, MB_ICONERROR);
		}
	
}

void SetCall(){
	int i;
	int ndx;

	/* Copy all fitting parameters to one array FITPAR and get starting  values */
	if (variable.SCALEPEN) thread.SCALE = (variable.SCALELIM[1] + variable.SCALELIM[0]) / 2;
	
	if (variable.BETAPEN) thread.BETA = (variable.BETALIM[1] + variable.BETALIM[0]) / 2;

	if (variable.SURFFRACPEN) thread.SURFFRAC = (variable.SURFFRACLIM[1] + variable.SURFFRACLIM[0]) / 2;

	if (thread.NSURF2>0)
		if (variable.SURF2FRACPEN) thread.SURF2FRAC = (variable.SURF2FRACLIM[1] + variable.SURF2FRACLIM[0]) / 2;

	for (i = 0; i < variable.NDISTOT; ++i){
		ndx = i + NSF;
		if (variable.DISPLPEN[i]) variable.DISPL[i] = (variable.DISPLLIM[i][1] + variable.DISPLLIM[i][0]) / 2;
	}
	for (i = 0; i < thread.NDWTOT; ++i){
		ndx = i + NSF + variable.NDISTOT;
		if (variable.DEBWALPEN[i]) thread.DEBWAL[i] = (variable.DEBWALLIM[i][1] + variable.DEBWALLIM[i][0]) / 2;
	}
	for (i = 0; i < thread.NDWTOT2; ++i){
		ndx = i + NSF + variable.NDISTOT + thread.NDWTOT;
		if (variable.DEBWAL2PEN[i]) thread.DEBWAL2[i] = (variable.DEBWAL2LIM[i][1] + variable.DEBWAL2LIM[i][0]) / 2;
	}

	for (i = 0; i < variable.NOCCTOT; ++i){
		ndx = i + NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2;
		if (variable.OCCUPPEN[i]) thread.OCCUP[i] = (variable.OCCUPLIM[i][1] + variable.OCCUPLIM[i][0]) / 2;
	}

	for (i = 0; i < variable.NSUBSCALETOT; ++i){
		ndx = i + NSF + variable.NDISTOT + thread.NDWTOT + thread.NDWTOT2 + variable.NOCCTOT;
		if (variable.SUBSCALEPEN[i]) thread.SUBSCALE[i] = (variable.SUBSCALELIM[i][1] + variable.SUBSCALELIM[i][0]) / 2;
	}
}

/***************************************************************************/
void decrease_bragg_weight(double L_INTERVAL, double ERROR_FRACTION)
/***************************************************************************/

/*
Increase error bar near Bragg peaks in order to give these data points
less weigth. At the Bragg peak the new error is the square sum of existing
data error with error_fraction of structure factor value. The fraction
decreases linearly over l_interval.
Adopted of Prof. Elias function
*/

{

	int i;
	double fsum, dummy, l_bragg;

	

	for (i = 0; i < thread.NDAT; ++i)
	{
		variable.COPYERRDAT[i] = variable.ERRDAT[i];
		/* check if data point is near Bragg peak */
		l_bragg = floor(thread.LDAT[i] + 0.5);
		if (fabs(l_bragg - thread.LDAT[i]) < L_INTERVAL)
		{
			calc.f_calc(thread.HDAT[i], thread.KDAT[i], l_bragg + 0.001, thread.ATTEN, thread.LBR[i], 0,
				variable.NSUBSCALETOT, thread.ENERGY[i], &dummy, &dummy,  &fsum, &dummy, &thread);
			if (fsum / thread.SCALE > 1000) /* indeed a Bragg peak */
			{
				variable.COPYERRDAT[i] = sqrt(sqr((1. - fabs(l_bragg - thread.LDAT[i]) / L_INTERVAL)*
					ERROR_FRACTION*thread.FDAT[i]) + sqr(variable.ERRDAT[i]));
			}
		}
	}
}

//Thread_t* Tdata, double* errors, int range, double MinF, double MaxL)
/***************************************************************************/
void  ReduceData()
/***************************************************************************/
{

	int i, j, l, p;
	int l_i;
	double Ftmp, Ltmp, Etmp;
	j = 0;
	p = 0;

	for (i = 0; i < thread.NDAT; i += p)
	{
		Ftmp = Ltmp = 0;
		p = 1;

		Ftmp = thread.FDAT[i];
		Ltmp = thread.LDAT[i];
		Etmp = variable.ERRDAT[i] * variable.ERRDAT[i];

		for (l = 1; l < variable.reduction_step; ++l)
		{
			l_i = i + l;
			if (thread.HDAT[i] == thread.HDAT[l_i] && thread.KDAT[i] == thread.KDAT[l_i] && thread.ENERGY[i] == thread.ENERGY[l_i] && l_i < thread.NDAT)
			{
				if (fabs(thread.LDAT[i] - thread.LDAT[l_i]) < variable.reduction_MaxL && fabs(thread.FDAT[i] - thread.FDAT[l_i]) < variable.reduction_minF)
				{
					p++; //divider for the group
					Ftmp += thread.FDAT[l_i];
					Ltmp += thread.LDAT[l_i];
					Etmp += variable.ERRDAT[l_i] * variable.ERRDAT[l_i];
				}
				else
				{
					break;//if the value of the point is off the linit just skip threst of the points
				}
			}
			else
			{
				break;//if the value of the point is off the linit just skip threst of the points
			}
		}

		thread.FDAT[j] = Ftmp / p;//average the points
		if (p > 1)
			variable.ERRDAT[j] = sqrt(Etmp / (p - 1.0)) / sqrt(p);
		//Tdata->FACTOR_ERR[j] = sqrt(Etmp) / p;
		else
			variable.ERRDAT[j] = sqrt(Etmp);

		thread.LDAT[j] = Ltmp / p;        //average the points
		thread.HDAT[j] = thread.HDAT[i];
		thread.KDAT[j] = thread.KDAT[i];
		thread.LBR[j] = thread.LBR[i];
		thread.FRAC[j] = thread.FRAC[i];
		thread.LBR[j] = p;

		j++; //new data index
	}

	//set the new size for data
	thread.NDAT = j;

}


