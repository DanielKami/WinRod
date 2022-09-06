//--------------------------------------------------------------------------------------------------------------------------------
//Almost oryginal part of Prof. Elias code with  minor changes
//Still needs some work for better compatybility with Windows
//--------------------------------------------------------------------------------------------------------------------------------


#include <stdafx.h>
#define KEATING			//register keating variable

#include "variables.h"
#include "thread.h"
#include "keating.h"
#include "plot.h"
#include "3Ddef.h"
#include "elements.h"
#include "ExMath.h"				//Additional mathematical functions

WCHAR tstr[100];



void InitKeating(float *AtomRadius){

	for(int i=0; i<thread.NTYPES; i++){
		ket.ATOM_RAD[thread.TS[i]] = AtomRadius[get_element_number(variable.ELEMENT[thread.TS[i]])];
	}
}


void InitialiseKeating(){

	ket.ALPHA_KEAT = 1.;
	ket.BETA_KEAT = 1.;
	ket.KEAT_PLUS_CHI = false;
	ket.EQU_ANGLE = 180.*deg2rad;
	ket.POTENTIAL = VKEATING;

}

/***************************************************************************/
double    energy_calc(Thread_t* Tdata)
/***************************************************************************/

    /*
    Calculate lattice energy.
    */

    {

    double	pos1[3],pos2[3],pos3[3];
    int	i,j;
    double	bond_energy,angle_energy;
    double	ratiosqr,ratio6;


    bond_energy = 0;
    angle_energy = 0;

    if (ket.POTENTIAL == VKEATING)  /* Use Keating potential */
	{
	for (i = 0; i < Tdata->NSURF; ++i)
	    {
	    if (ket.IN_KEAT[i])
		{
		get_cartesian(pos1,Tdata->XSFIT[i],Tdata->YSFIT[i],Tdata->ZSFIT[i], Tdata);

		/* calculate bond-length deformation energy */

		for (j = 0; j < ket.NBOND[i]; ++j)
		    {
		    get_cartesian(pos2,Tdata->XSFIT[ket.BOND_LIST[i][j]]+ket.XBSHIFT[i][j],
				  Tdata->YSFIT[ket.BOND_LIST[i][j]]+ket.YBSHIFT[i][j],
				  Tdata->ZSFIT[ket.BOND_LIST[i][j]], Tdata);
		    bond_energy += sqr(sqr(pos1[0]-pos2[0])+
				       sqr(pos1[1]-pos2[1])+sqr(pos1[2]-pos2[2])-
				       sqr(ket.ATOM_RAD[Tdata->TS[i]]+ket.ATOM_RAD[Tdata->TS[ket.BOND_LIST[i][j]]]));
		    }

		/* calculate bond-angle deformatin energy */

		for (j = 0; j < ket.NANGLE[i]; ++j)
		    {
		    get_cartesian(pos2,
				  Tdata->XSFIT[ket.ANGLE_LIST[i][j][0]]+ket.XASHIFT[i][j][0],
				  Tdata->YSFIT[ket.ANGLE_LIST[i][j][0]]+ket.YASHIFT[i][j][0],
				  Tdata->ZSFIT[ket.ANGLE_LIST[i][j][0]], Tdata);
		    get_cartesian(pos3,
				  Tdata->XSFIT[ket.ANGLE_LIST[i][j][1]]+ket.XASHIFT[i][j][1],
				  Tdata->YSFIT[ket.ANGLE_LIST[i][j][1]]+ket.YASHIFT[i][j][1],
				  Tdata->ZSFIT[ket.ANGLE_LIST[i][j][1]], Tdata);
		    angle_energy += sqr(
			(pos2[0]-pos1[0])*(pos3[0]-pos1[0])+
			(pos2[1]-pos1[1])*(pos3[1]-pos1[1])+
			(pos2[2]-pos1[2])*(pos3[2]-pos1[2])-cos(ket.EQU_ANGLE)*
			(ket.ATOM_RAD[Tdata->TS[i]]+ket.ATOM_RAD[Tdata->TS[ket.ANGLE_LIST[i][j][0]]])*
			(ket.ATOM_RAD[Tdata->TS[i]]+ket.ATOM_RAD[Tdata->TS[ket.ANGLE_LIST[i][j][1]]]));
		    }
		}
	    }

	return(ket.ALPHA_KEAT*bond_energy+ket.BETA_KEAT*angle_energy);
	}

    if (ket.POTENTIAL == VLENNARDJONES) /* Use Lennard-Jones potential */
	{
	for (i = 0; i < Tdata->NSURF; ++i)
	    {
	    if (ket.IN_KEAT[i])
		{
		get_cartesian(pos1,Tdata->XSFIT[i],Tdata->YSFIT[i],Tdata->ZSFIT[i], Tdata);

		/* calculate bond-length deformation energy */

		for (j = 0; j < ket.NBOND[i]; ++j)
		    {
		    get_cartesian(pos2,Tdata->XSFIT[ket.BOND_LIST[i][j]]+ket.XBSHIFT[i][j],
				  Tdata->YSFIT[ket.BOND_LIST[i][j]]+ket.YBSHIFT[i][j],
				  Tdata->ZSFIT[ket.BOND_LIST[i][j]], Tdata);
		    ratiosqr = sqr(ket.ATOM_RAD[Tdata->TS[i]]+ket.ATOM_RAD[Tdata->TS[ket.BOND_LIST[i][j]]])/
			(1.259*(sqr(pos1[0]-pos2[0])+sqr(pos1[1]-pos2[1])+
				sqr(pos1[2]-pos2[2])));
		    ratio6 = ratiosqr*ratiosqr*ratiosqr;
		    bond_energy += 1+4*ratio6*(ratio6-1);
		    }
		}
	    }

	return(ket.ALPHA_KEAT*bond_energy);
	}
    return(0.); /* to avoid compiler warning */
    }



/***************************************************************************/
_inline void    get_cartesian(double cart[3], double x, double y, double z, Thread_t* Tdata)
/***************************************************************************/

    /*
    Calculate cartesian coordinates in Angstroms for the atom at position 
    (x,y,z) in reduced coordinates.
    */

    {

    cart[0] = Tdata->DLAT[0]*x+Tdata->DLAT[1]*cos(Tdata->DLAT[5])*y+Tdata->DLAT[2]*cos(Tdata->DLAT[4])*z;
    cart[1] = Tdata->DLAT[1]*sin(Tdata->DLAT[5])*y-Tdata->DLAT[2]*sin(Tdata->DLAT[4])*cos(Tdata->RLAT[3])*z;
    cart[2] = z/Tdata->RLAT[2];

    }


/***************************************************************************/
void    fenergy(double a[], double *y, Thread_t* Tdata 	){
/***************************************************************************/

    /*
    Function called by fitting routines to compute lattice energy for
    given displacement settings.
    */

    /* Copy values in array 'a' to corresponding model parameters */
    for (int i = 0; i < variable.NDISTOT; i++){
		variable.DISPL[i] = a[i];
	}

   // update_model();
    *y = 100*energy_calc(Tdata);
}


/***************************************************************************/
void    angle_search( Thread_t* Tdata )
/***************************************************************************/

    /*
    Find all bond angles between atoms that are part of the energy
    minimization.
    */

    {

    int	i,j,k,l;
    int	xas[MAXBOND],yas[MAXBOND],bondlist[MAXBOND],nbonds;
    double	pos1[3],pos2[3],pos3[3],bond1[3],bond2[3],angle;

    for (i = 0; i < thread.NSURF; i++)
	{
	ket.NANGLE[i] = 0;
	if (ket.IN_KEAT[i])
	    {

	    /* Find all bonds of atom i */

	    nbonds = 0;

	    /* See if i appears as a bond to an earlier atom */

	    for (j = 0; j < i; j++)
		{
		if (ket.IN_KEAT[j])
		    {
		    for (k = 0; k < ket.NBOND[j]; k++)
			{
			if (ket.BOND_LIST[j][k] == i)
			    {
			    bondlist[nbonds] = j;
			    xas[nbonds] = -ket.XBSHIFT[j][k];
			    yas[nbonds] = -ket.YBSHIFT[j][k];
			    nbonds++;
			    }
			}
		    }
		}

	    /* Add to the bond list the bonds of the atom itself */

	    if (nbonds+ket.NBOND[i] > MAXBOND)
		swprintf_s(tstr, 100, L"Maximum number of bonds exceeded");
	    else
		{
		for (j = 0; j < ket.NBOND[i]; j++)
		    {
		    bondlist[nbonds] = ket.BOND_LIST[i][j];
		    xas[nbonds] = ket.XBSHIFT[i][j];
		    yas[nbonds] = ket.YBSHIFT[i][j];
		    nbonds++;
		    }
		}

	    /* Generate all bond angles out of the bondlist */

	    get_cartesian(pos1,Tdata->XSFIT[i],Tdata->YSFIT[i],Tdata->ZSFIT[i], Tdata);
	    for (j = 0; j < nbonds; j++)
		{
		get_cartesian(pos2,Tdata->XSFIT[bondlist[j]]+xas[j],
			      Tdata->YSFIT[bondlist[j]]+yas[j],Tdata->ZSFIT[bondlist[j]], Tdata);
		for (k = j+1; k < nbonds; k++)
		    {
		    if (ket.NANGLE[i] == MAXANGLE)
			{
			swprintf_s(tstr, 100, L"Maximum number of bond angles exceeded");
			}
		    else
			{
			get_cartesian(pos3,Tdata->XSFIT[bondlist[k]]+xas[k],
				      thread.YSFIT[bondlist[k]]+yas[k],Tdata->ZSFIT[bondlist[k]], Tdata);
			for (l = 0; l < 3; l++)
			    {
			    bond1[l] = pos2[l]-pos1[l];
			    bond2[l] = pos3[l]-pos1[l];
			    }
			angle = acos((bond1[0]*bond2[0]+
				      bond1[1]*bond2[1]+bond1[2]*bond2[2])
				     /sqrt(sqr(bond1[0])+sqr(bond1[1])+sqr(bond1[2]))
				     /sqrt(sqr(bond2[0])+sqr(bond2[1])+sqr(bond2[2])));
			if (angle < 1.1*ket.EQU_ANGLE)
			    {
			    ket.ANGLE_LIST[i][ket.NANGLE[i]][0] = bondlist[j];
			    ket.ANGLE_LIST[i][ket.NANGLE[i]][1] = bondlist[k];
			    ket.XASHIFT[i][ket.NANGLE[i]][0] = xas[j];
			    ket.YASHIFT[i][ket.NANGLE[i]][0] = yas[j];
			    ket.XASHIFT[i][ket.NANGLE[i]][1] = xas[k];
			    ket.YASHIFT[i][ket.NANGLE[i]][1] = yas[k];
			    ket.NANGLE[i]++;
			    }
			else
			    {
			    }
			}
		    }
		}
	    }
	}
    }


/***************************************************************************/
void    bond_search(Thread_t* Tdata)
/***************************************************************************/

    /*
    Find the bonds between atoms that are part of the energy minimization.
    The subroutine stores the values of the displacements along the lattice
    vectors that are necessary to calculate the correct bond lengths.

    Bonds of an atom to itself (on an equivalent position) are ignored.
    These only occur for small unit cells that are normally not used in
    experiments.
    */

    {

    int	i,j,k,l;
    double	pos1[3],pos2[3],length;

    /* Allow the 2nd atom to be shifted over one lattice parameter in order
       to find the correct (= minimum) bond distance. */

    for (i = 0; i < Tdata->NSURF; i++)
	{
	ket.NBOND[i] = 0;
	if (ket.IN_KEAT[i])
	    {
	    get_cartesian(pos1,Tdata->XSFIT[i],Tdata->YSFIT[i],Tdata->ZSFIT[i], Tdata);
	    for (j = i+1; j < Tdata->NSURF; j++)
		{
		if (ket.IN_KEAT[j])
		    {
		    for (k = -1; k <= 1; k++)
			{
			for (l = -1; l <= 1; l++)
			    {
			    get_cartesian(pos2,Tdata->XSFIT[j]+k,Tdata->YSFIT[j]+l,
					  Tdata->ZSFIT[j], Tdata);
			    length = sqrt(sqr(pos1[0]-pos2[0])+
					  sqr(pos1[1]-pos2[1])+sqr(pos1[2]-pos2[2]));
			    if (length < 1.1*(ket.ATOM_RAD[Tdata->TS[i]]
					      +ket.ATOM_RAD[Tdata->TS[j]]))
				{
				if (ket.NBOND[i] == MAXBOND)
				    {
				    swprintf_s(tstr, 100, L"Error, maximum # bonds exceeded");
					
				    }
				else
				    {
				    ket.BOND_LIST[i][ket.NBOND[i]] = j;
				    ket.XBSHIFT[i][ket.NBOND[i]] = k;
				    ket.YBSHIFT[i][ket.NBOND[i]] = l;
				    ket.NBOND[i]++;
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }