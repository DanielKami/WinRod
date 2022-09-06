#pragma once
#include "definitions.h"

#define MAX_BONDED 8	//Number of bonds
#define MAX_LINE_OUT	100//Maximum lenght of readed line


class Import
{
public:
	Import();
	virtual ~Import();


	struct Molecule {
		char	MoleculeName[MOLECULENAME];

		//Start and end in the  thread.NSURF;
		int		start;
		int		end;

		int		NXDIS;
		int		NYDIS;
		int		NZDIS;

		int		NDWS;
		int		NDWS2;
		int		NOCCUP;

		float	MolecularPosition_x, MolecularPosition_y, MolecularPosition_z;

		int		NROTATIONX;
		int		NROTATIONY;
		int		NROTATIONZ;
	};
	Molecule	myMolecule[MAXMOLECULES];

	float	x_cartesian[MAXATOMS];
	float	y_cartesian[MAXATOMS];
	float	z_cartesian[MAXATOMS];
	float	nd[MAXATOMS];

	//Copy
	char	copy_element_from_file[MAXATOMS][3];
	float	copy_x_cartesian[MAXATOMS];
	float	copy_y_cartesian[MAXATOMS];
	float	copy_z_cartesian[MAXATOMS];

	void	Rotate();
	int		OpenFileFunction(HWND hwnd);
	void	SetMolecule(bool auto_center = true);
	void	Reset();

	int		TotalMolecules;
	bool	NewMoleculePositionInData[MAXATOMS];
	char	element_from_file[MAXATOMS][3];

private:
	bool	ReadDataFromBuffer(char *input_str, short Type_of_file);
	bool	Take_line(char *instring, char *outstring, unsigned int *Position);
	void	putTo_connect_matrix(int atom, int bonded_atom, int typ_of_bond);
	void	RotationMatrix(double *alpha, double *beta, double *gamma);
	void	FindStartEnd();
	void	CenterMolecule();
	void	CopyParametersToAllAtoms();

	DWORD	dwsize;
	int		el_nr_old;

	int		index[MAXATOMS];
	int		tb[MAXATOMS];

	int		bonded[MAX_BONDED];
	int		b_o_p[MAXATOMS][MAX_BONDED];  //Connection matrix
	int		mol_index_atom[MAXATOMS];
	int		mol_bonded_atom[MAXATOMS];
	int		mol_typ_atom[MAXATOMS];

	//For fast rotation calculations
	double one_over_D0;
	double one_over_D2;
	double one_over_temp2;

	//Molecule rotation matrix
	double a[3][3];
};

