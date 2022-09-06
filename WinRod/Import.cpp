//--------------------------------------------------------------------------------------------------------------------------------
//This file is for reading and operating on organic molecules. It is different approach than in Prof. Elias Vlieg ROD version but the 
//results are the same.
//Created by: Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Import.h"
#include <commdlg.h>
#include <math.h>
#include "variables.h"
#include "thread.h"
#include "elements.h"
#include "ReadFile.h"
#include "Scene3D.h"


extern Scene3D		myScene;


Import::Import()
{
	Reset();
}


Import::~Import()
{
}

void Import::Reset()
{

	thread.NSURF = 0;
	variable.NSURFTOT = 0;
	TotalMolecules = -1;

	for (int i = 0; i < MAXATOMS; ++i)
	{
		NewMoleculePositionInData[i] = false;
	}
	//First position in the list is ready for molecule input
	NewMoleculePositionInData[0] = true;

	for (int i = 0; i < MAXMOLECULES; ++i)
	{
		myMolecule[i].MolecularPosition_x = 0;
		myMolecule[i].MolecularPosition_y = 0;
		myMolecule[i].MolecularPosition_z = 0;

		myMolecule[i].NROTATIONX = 1;
		myMolecule[i].NROTATIONY = 1;
		myMolecule[i].NROTATIONZ = 1;

		myMolecule[i].start = myMolecule[i].end = 0;

	}
	


}



//--------------------------------------------------------------------------------------------------------------------------------
int Import::OpenFileFunction(HWND hwnd){
	//--------------------------------------------------------------------------------------------------------------------------------

	HANDLE			hFile;
	WCHAR szFile[MAX_PATH] = TEXT("\0");
	DWORD			dwSave;
	dwsize = 0;
	LPSTR szTemporary;
	OPENFILENAME	ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("All (*.xyz)\0*.xyz\0All (*.pdb *.ent)\0*.pdb;*.ent\0All (*.mol *.sdf *.sd *.mdl)\0*.mol;*.sdf;*.sd;*.mdl\0All (*.ml2)\0*.ml2\0\0");	//TEXT("All (*.bul)\0*.bul\0")
	ofn.lpstrTitle = TEXT("Import molecular file");
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = TEXT("*.xyz");
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.nFilterIndex = 1;

	if (GetOpenFileName(&ofn)){
		hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			MessageBox(hwnd, TEXT("I can't import the file!"), NULL, MB_ICONERROR);
			CloseHandle(hFile);
			return 0;
		}
	}
	else return 0;

	dwsize = GetFileSize(hFile, 0);
	szTemporary = new char[dwsize+1]; 


	if (!ReadFile(hFile, szTemporary, dwsize, &dwSave, NULL))
	{
		MessageBox(hwnd, TEXT("Read file failed!"), NULL, MB_ICONERROR);
		delete[] szTemporary;
		CloseHandle(hFile);
		return 0;
	}

	variable.MODE = 1;//Switch to molecular mode
	variable.ROTATIONTOTX = 1;
	variable.ROTATIONTOTY = 1;
	variable.ROTATIONTOTZ = 1;
	myMolecule[0].NXDIS = 1;
	myMolecule[0].NYDIS = 1;
	myMolecule[0].NZDIS = 1;

	ReadDataFromBuffer(szTemporary, (short)ofn.nFilterIndex);

	//Convert and copy to variables
	SetMolecule();

	delete[] szTemporary;
	CloseHandle(hFile);

	return ofn.nFilterIndex;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Reading files with atoms data
//
/////////////////////////////////////////////////////////////////////////////////////////////////
bool Import::ReadDataFromBuffer(char *input_str, short Type_of_file){
	
	unsigned int LinePositionInLine = 0;
	int i, j;
	char command1[21], command2[7], command3[4], command4[7];
	char out_str[MAX_LINE_OUT];
	unsigned int PositionInLine = 0;
	int *bonded0 = new int[MAXATOMS];
	int *bonded1 = new int[MAXATOMS];
	int *bonded2 = new int[MAXATOMS];
	int *bonded3 = new int[MAXATOMS];
	int *bonded4 = new int[MAXATOMS];
	int *bonded5 = new int[MAXATOMS];
	int *bonded6 = new int[MAXATOMS];
	int *bonded7 = new int[MAXATOMS];
	int *bonded8 = new int[MAXATOMS];



	////////////////// Read *.xyz file //////////////////////////////////////////////////////////////

	if (Type_of_file == 1){

		Take_line(input_str, out_str, &PositionInLine); //1-st line
		Take_line(input_str, out_str, &PositionInLine); //2-nd line

		
		for (i = 0; i < MAXATOMS; ++i){
			if (!Take_line(input_str, out_str, &PositionInLine))
				break;
				if (out_str != ""){
					sscanf_s(out_str, "%s%f%f%f%f", &element_from_file[thread.NSURF], (unsigned int)sizeof(char) * 3, &x_cartesian[thread.NSURF], &y_cartesian[thread.NSURF], &z_cartesian[thread.NSURF], &nd[thread.NSURF]);
				thread.NSURF += 1;
			}
				else				
					break;
				
		}
	}


	/////////////////// Read *.PDB, *.ENT files /////////////////////////////////////////////////////
	if (Type_of_file == 2){

	sprintf_s(command2, 10, "CONECT");
	sprintf_s(command3, 10, "END");
	sprintf_s(command4, 10, "HETATM");

	for (i = 0; i < MAXATOMS; ++i){
		bonded0[i] = bonded1[i] = bonded2[i] = bonded3[i] = bonded4[i] = bonded5[i] = bonded6[i] = bonded7[i] = bonded8[i] = 0;
	}



		thread.NSURF = 0;
		for (i = 0; i < MAXATOMS; ++i){
			if (Take_line(input_str, out_str, &PositionInLine) == false) break;
			if (out_str != ""){
				sscanf_s(out_str, "%s%d%s%d%f%f%f", &command1, (unsigned int)sizeof(command1), &index[thread.NSURF], &element_from_file[thread.NSURF], (unsigned int)(3*sizeof(char)), &index[thread.NSURF], &x_cartesian[thread.NSURF], &y_cartesian[thread.NSURF], &z_cartesian[thread.NSURF]);
				thread.NSURF += 1;
			}
			if (command1[0] != command4[0]) break;
		}

		el_nr_old = thread.NSURF - 1; //just remember the true number of atoms
		thread.NSURF = 0;

		for (i = 0; i < MAXATOMS; ++i){
			if (i != 0)//the first line already read
				if (!Take_line(input_str, out_str, &PositionInLine))
					break;

			if (out_str != "")
			{
				if (LinePositionInLine < 57)  sscanf_s(out_str, "%9s%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF], &bonded2[thread.NSURF], &bonded3[thread.NSURF], &bonded4[thread.NSURF], &bonded5[thread.NSURF], &bonded6[thread.NSURF], &bonded7[thread.NSURF], &bonded8[thread.NSURF]);
				if (LinePositionInLine < 52){ sscanf_s(out_str, "%9s%3d%3d%3d%3d%3d%3d%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF], &bonded2[thread.NSURF], &bonded3[thread.NSURF], &bonded4[thread.NSURF], &bonded5[thread.NSURF], &bonded6[thread.NSURF], &bonded7[thread.NSURF]); bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 47){ sscanf_s(out_str, "%9s%3d%3d%3d%3d%3d%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF], &bonded2[thread.NSURF], &bonded3[thread.NSURF], &bonded4[thread.NSURF], &bonded5[thread.NSURF], &bonded6[thread.NSURF]);      bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 42){ sscanf_s(out_str, "%9s%3d%3d%3d%3d%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF], &bonded2[thread.NSURF], &bonded3[thread.NSURF], &bonded4[thread.NSURF], &bonded5[thread.NSURF]);           bonded6[thread.NSURF] = bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 37){ sscanf_s(out_str, "%9s%3d%3d%3d%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF], &bonded2[thread.NSURF], &bonded3[thread.NSURF], &bonded4[thread.NSURF]);                bonded5[thread.NSURF] = bonded6[thread.NSURF] = bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 32){ sscanf_s(out_str, "%9s%3d%3d%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF], &bonded2[thread.NSURF], &bonded3[thread.NSURF]);                     bonded4[thread.NSURF] = bonded5[thread.NSURF] = bonded6[thread.NSURF] = bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 27){ sscanf_s(out_str, "%9s%3d%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF], &bonded2[thread.NSURF]);                          bonded3[thread.NSURF] = bonded4[thread.NSURF] = bonded5[thread.NSURF] = bonded6[thread.NSURF] = bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 22){ sscanf_s(out_str, "%9s%3d%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF], &bonded1[thread.NSURF]);                              bonded2[thread.NSURF] = bonded3[thread.NSURF] = bonded4[thread.NSURF] = bonded5[thread.NSURF] = bonded6[thread.NSURF] = bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 17){ sscanf_s(out_str, "%9s%3d%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF], &bonded0[thread.NSURF]);                                    bonded1[thread.NSURF] = bonded2[thread.NSURF] = bonded3[thread.NSURF] = bonded4[thread.NSURF] = bonded5[thread.NSURF] = bonded6[thread.NSURF] = bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
				if (LinePositionInLine < 13){ sscanf_s(out_str, "%9s%3d", &command1, (unsigned int)(21 * sizeof(char)), &index[thread.NSURF]);                 bonded0[thread.NSURF] = bonded1[thread.NSURF] = bonded2[thread.NSURF] = bonded3[thread.NSURF] = bonded4[thread.NSURF] = bonded5[thread.NSURF] = bonded6[thread.NSURF] = bonded7[thread.NSURF] = bonded8[thread.NSURF] = 0; }
			}
			if (command1[0] != command2[0]) break;
			thread.NSURF += 1;
		}
		thread.NSURF = el_nr_old;



		// clean the conncetion matrix
		for (i = 0; i < MAXATOMS; ++i){
			for (j = 0; j < 9; ++j){
				b_o_p[i][j] = -1; //-1  atom not bonded
			}
		}

		// write the connection matrix and for PDB files corect the bond matrix -1 to count from 0
		for (i = 0; i < thread.NSURF; ++i){
			if (index[i]>0){
				if (bonded0[i]>0) b_o_p[index[i] - 1][0] = bonded0[i] - 1;
				if (bonded1[i]>0) b_o_p[index[i] - 1][1] = bonded1[i] - 1;
				if (bonded2[i] > 0) b_o_p[index[i] - 1][2] = bonded2[i] - 1;
				if (bonded3[i] > 0) b_o_p[index[i] - 1][3] = bonded3[i] - 1;
				if (bonded4[i] > 0) b_o_p[index[i] - 1][4] = bonded4[i] - 1;
				if (bonded5[i] > 0) b_o_p[index[i] - 1][5] = bonded5[i] - 1;
				if (bonded6[i] > 0) b_o_p[index[i] - 1][6] = bonded6[i] - 1;
				if (bonded7[i] > 0) b_o_p[index[i] - 1][7] = bonded7[i] - 1;
				if (bonded8[i] > 0) b_o_p[index[i] - 1][8] = bonded8[i] - 1;
			}
		}
	}


	/////////////////// Read *.mol, *. files /////////////////////////////////////////////////////


	if (Type_of_file == 3){

		Take_line(input_str, out_str, &PositionInLine);  //line 0
		Take_line(input_str, out_str, &PositionInLine);  //line 1
		Take_line(input_str, out_str, &PositionInLine);  //line 2
		Take_line(input_str, out_str, &PositionInLine);  //line 3

		sscanf_s(out_str, "%4d%4d", &thread.NSURF, &el_nr_old);
		if (thread.NSURF >= MAXATOMS) thread.NSURF = MAXATOMS;

		for (i = 0; i < thread.NSURF; ++i){
			if (!Take_line(input_str, out_str, &PositionInLine))
				break;
			if (out_str != ""){
				sscanf_s(out_str, "%f%f%f%s", &x_cartesian[i], &y_cartesian[i], &z_cartesian[i], &element_from_file[i], 3);
			}
		}

		//connection matrix
		for (i = 0; i < el_nr_old; ++i){
			if (!Take_line(input_str, out_str, &PositionInLine))
				break;
			if (out_str != ""){
				sscanf_s(out_str, "%d%d%d", &mol_index_atom[i], &mol_bonded_atom[i], &mol_typ_atom[i]);
			}
		}

		// clean the conncetion matrix
		for (i = 0; i < MAXATOMS; i++){
			for (j = 0; j < 9; j++){
				b_o_p[i][j] = -1; //-1  atom not bonded
			}
		}

		for (i = el_nr_old; i < el_nr_old * 2; i++){// extend matrix for swaPing the connection matrix the elements from right side are not on the left
			mol_index_atom[i] = mol_bonded_atom[i - el_nr_old];
			mol_bonded_atom[i] = mol_index_atom[i - el_nr_old];
			mol_typ_atom[i] = mol_typ_atom[i - el_nr_old];
		}

		for (i = 0; i < el_nr_old * 2; ++i){
			if (mol_index_atom[i]>0) putTo_connect_matrix(mol_index_atom[i], mol_bonded_atom[i], mol_typ_atom[i]);
		}
	}

	/////////////////// Read *.ml2, *. files /////////////////////////////////////////////////////
	if (Type_of_file == 4){
		int tmp_mol;

		for (i = 0; i < 10; ++i)
			Take_line(input_str, out_str, &PositionInLine);  //9 -line 

		sscanf_s(out_str, "%d%d", &thread.NSURF, &el_nr_old);

		for (i = 0; i < 5; ++i) //15 line
			Take_line(input_str, out_str, &PositionInLine);  //9 -line 

		for (i = 0; i < thread.NSURF; ++i){
			if (!Take_line(input_str, out_str, &PositionInLine))
				break;
			if (out_str != ""){
				sscanf_s(out_str, "%d%s%f%f%f%s%d", &index[i], &command1, (unsigned int)(21 * sizeof(char)),&x_cartesian[i], &y_cartesian[i], &z_cartesian[i], &element_from_file[i], (unsigned int)(3*sizeof(char)), &tmp_mol);
			}
		}

		//separator
		Take_line(input_str, out_str, &PositionInLine);

		//connection matrix
		for (i = 0; i < el_nr_old; i++){
			if (Take_line(input_str, out_str, &PositionInLine) == false) break;
			if (out_str != ""){
				sscanf_s(out_str, "%d%d%d%d", &index[i], &mol_index_atom[i], &mol_bonded_atom[i], &mol_typ_atom[i]);
			}
		}

		//clean the connection matrix
		for (i = 0; i < MAXATOMS; ++i){
			for (j = 0; j < 9; ++j){
				b_o_p[i][j] = -1; //-1  atom not bonded
			}
		}

		for (i = el_nr_old; i < el_nr_old * 2; ++i){// extend matrix for swaPing the connection matrix the elements from right side are not on the left
			mol_index_atom[i] = mol_bonded_atom[i - el_nr_old];
			mol_bonded_atom[i] = mol_index_atom[i - el_nr_old];
			mol_typ_atom[i] = mol_typ_atom[i - el_nr_old];
		}

		for (i = 0; i < el_nr_old * 2; ++i){
			if (mol_index_atom[i]>0) putTo_connect_matrix(mol_index_atom[i], mol_bonded_atom[i], mol_typ_atom[i]);
		}

	}

	thread.NSURF -= 1;
	variable.NSURFTOT = thread.NSURF;
	NewMoleculePositionInData[thread.NSURF] = true;
	TotalMolecules++;
	


	delete[] bonded0;
	delete[] bonded1;
	delete[] bonded2;
	delete[] bonded3;
	delete[] bonded4;
	delete[] bonded5;
	delete[] bonded6;
	delete[] bonded7;
	delete[] bonded8;
	return true;
}


//if(	Take_lines(instring    - pointer to the input string
//				  ,outstring   - pointer to the extracted line
//				  ,Position	   - actual position in the instring 
//				  )
//--------------------------------------------------------------------------------------------------------------------------------
bool Import::Take_line(char *instring, char *outstring, unsigned int *Position){
	//--------------------------------------------------------------------------------------------------------------------------------


	unsigned int x, y;

	unsigned int InpStringSize = (int)dwsize;// (unsigned int)strlen(instring);

	ZeroMemory(outstring, sizeof(char)*MAX_LINE_OUT);
	y = 0;
	for (x = *Position; x<InpStringSize; ++x)
	{
		if (instring[x] != 13 && instring[x + 1] != 10)
		{
			outstring[y] = instring[x];
			y++;
			if (y >= MAX_LINE_OUT - 1) break;
		}
		else break;
	}
	outstring[y] = 0;

	*Position = x + 2;//move to the beginning of the new line
	if (*Position>InpStringSize + 2)
	{
		*Position = 0;
		return false;
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Put the bonded atom to bonded matrix in correct place
//
////////////////////////////////////////////////////////////////////////////////////////////////

void Import::putTo_connect_matrix(int atom, int bonded_atom, int typ_of_bond){
	int i;

	for (i = 0; i < 9; ++i){
		if (b_o_p[atom - 1][i] == -1)
		{
			b_o_p[atom - 1][i] = bonded_atom - 1;
			b_o_p[atom - 1][i] = typ_of_bond;
			break;
		}
	}
}


void Import::SetMolecule(bool auto_center)
{

	if (thread.NBULK == 0)
	{
		thread.DLAT[0] = thread.DLAT[1] = thread.DLAT[2] = 1;
		thread.DLAT[3] = thread.DLAT[4] = thread.DLAT[5] = PI / 2;
				

		myScene.g_SRepetitionX = myScene.g_SRepetitionY = 1;
	}

	//Initialize accelerators
	CalculateAcceleratores();


	//Find first and last atom of molecule in data
	FindStartEnd();

	//Center molecule
	if (auto_center)
		CenterMolecule();

	//rewrite it to fractional WinRod coordinates
	Rotate();

	//Copy parameters from the first molecule atom  the rest is in molecule
	CopyParametersToAllAtoms();

	bool newtype;
	//Find in surface
	for (int i = 0; i < thread.NSURF; ++i)
	{
		//Find new types
		newtype = true;
		for (int j = 0; j < thread.NTYPES; ++j)
		{
			if (element_from_file[i][0] == variable.ELEMENT[j][0] && element_from_file[i][1] == variable.ELEMENT[j][1] && element_from_file[i][2] == variable.ELEMENT[j][2]) newtype = false;
		}

		if (newtype){
			if (thread.NTYPES == MAXTYPES)
			{
				WCHAR ErrorMsg[100];
				swprintf_s(ErrorMsg, 50, L"ERROR, too many atom types in unit cell the maximum is %d", MAXTYPES);
				MessageBox(NULL, ErrorMsg, NULL, MB_ICONERROR);
				break;
			}

			CopyMemory(variable.ELEMENT[thread.NTYPES], element_from_file[i], sizeof(element_from_file[i]));
			/* Store the coefficients for the computation of the atomic scattering
			factors for all the different elements in F_COEFF */
			get_coeff(variable.ELEMENT[thread.NTYPES], thread.F_COEFF[thread.NTYPES]);

			thread.NTYPES++;
		}
	}

	/* Assign a type number to atoms in the model */
	for (int j = 0; j < thread.NSURF; ++j)
	{
		for (int i = 0; i < thread.NTYPES; ++i)
		{
			if (element_from_file[j][0] == variable.ELEMENT[i][0] && element_from_file[j][1] == variable.ELEMENT[i][1]) thread.TS[j] = i;
		}
	}

}

inline void Import::Rotate()
{
	double rotated_x, rotated_y, rotated_z;

	for (int j = 0; j < TotalMolecules; ++j)
	{
		//Prepare the rotation matrix for next molecule
		RotationMatrix(&variable.ROTATIONX[myMolecule[j].NROTATIONX - 1], &variable.ROTATIONY[myMolecule[j].NROTATIONY - 1], &variable.ROTATIONZ[myMolecule[j].NROTATIONZ - 1]);

		for (int i = myMolecule[j].start; i < myMolecule[j].end; ++i)
		{
			//rotation around z
			rotated_x = x_cartesian[i] * a[0][0] + y_cartesian[i] * a[0][1] + z_cartesian[i] * a[0][2];
			rotated_y = x_cartesian[i] * a[1][0] + y_cartesian[i] * a[1][1] + z_cartesian[i] * a[1][2];
			rotated_z = x_cartesian[i] * a[2][0] + y_cartesian[i] * a[2][1] + z_cartesian[i] * a[2][2];

			//rewrite it to fractional WinRod coordinates
			variable.ZS[i] = rotated_z / variable.c_sin_bet__sin_alp;
			variable.YS[i] = (rotated_y - variable.ZS[i] * variable.c_cos_alp) / variable.b_sin_Gam;
			variable.XS[i] = (rotated_x - variable.YS[i] * variable.b_cos_gam - variable.ZS[i] * variable.c_cos_bet__sin_alp) / variable.a_sin_alp;
			


			//Shift molecule to the correct position
			variable.XS[i] += myMolecule[j].MolecularPosition_x;
			variable.YS[i] += myMolecule[j].MolecularPosition_y;
			variable.ZS[i] += myMolecule[j].MolecularPosition_z;
		}
	}
}



inline void Import::RotationMatrix(double *alpha, double *beta, double *gamma)
{

	double b[3][3];

	double cos_alpha = cos(*alpha);
	double sin_alpha = sin(*alpha);

	double cos_beta = cos(*beta);
	double sin_beta = sin(*beta);

	double cos_gamma = cos(*gamma);
	double sin_gamma = sin(*gamma);

	//Matrix gamma rotated
	a[0][0] = cos_gamma; a[0][1] = -sin_gamma; a[0][2] = 0;
	a[1][0] = sin_gamma; a[1][1] = cos_gamma;  a[1][2] = 0;
	a[2][0] = 0.0;       a[2][1] = 0.0;        a[2][2] = 1;

	//Matrix beta rotated
	//b[0][0] = cos_beta * a[0][0] - sin_beta * a[2][0]; b[0][1] = cos_beta * a[0][1] - sin_beta * a[2][1]; b[0][2] = cos_beta * a[0][2] - sin_beta * a[2][2];
	//b[1][0] = a[1][0];                                 b[1][1] = a[1][1];                                 b[1][2] = a[1][2];
	//b[2][0] = sin_beta * a[0][0] + cos_beta * a[2][0]; b[2][1] = sin_beta * a[0][1] + cos_beta * a[2][1]; b[2][2] = sin_beta * a[0][2] + cos_beta * a[2][2];

	//Matrix alpha rotated
	//a[0][0] = b[0][0];                                   a[0][1] = b[0][1];                                   a[0][2] = b[0][2];
	//a[1][0] = cos_alpha * b[1][0] - sin_alpha * b[2][0]; a[1][1] = cos_alpha * b[1][1] - sin_alpha * b[2][1]; a[1][2] = cos_alpha * b[1][2] - sin_alpha * b[2][2];
	//a[2][0] = sin_alpha * b[1][0] + cos_alpha * b[2][0]; a[2][1] = sin_alpha * b[1][1] + cos_alpha * b[2][1]; a[2][2] = sin_alpha * b[1][2] + cos_alpha * b[2][2];



	//Matrix beta rotated reduced
	b[0][0] = cos_beta * a[0][0]; b[0][1] = cos_beta * a[0][1]; b[0][2] = - sin_beta * a[2][2];
	b[1][0] = a[1][0];            b[1][1] = a[1][1];            b[1][2] = 0;
	b[2][0] = sin_beta * a[0][0]; b[2][1] = sin_beta * a[0][1]; b[2][2] =   cos_beta * a[2][2];

	//Matrix alpha rotated
	a[0][0] = b[0][0];                                   a[0][1] = b[0][1];                                   a[0][2] =   b[0][2];
	a[1][0] = cos_alpha * b[1][0] - sin_alpha * b[2][0]; a[1][1] = cos_alpha * b[1][1] - sin_alpha * b[2][1]; a[1][2] = - sin_alpha * b[2][2];
	a[2][0] = sin_alpha * b[1][0] + cos_alpha * b[2][0]; a[2][1] = sin_alpha * b[1][1] + cos_alpha * b[2][1]; a[2][2] =   cos_alpha * b[2][2];

}

inline void Import::CenterMolecule()
{
	//This function center molecules to they centers
	int i, j;
	int number_of_atoms;
	float temp = 0.;

	if (thread.NSURF == 0) return;

	for (j = 0; j < TotalMolecules; ++j)
	{

		number_of_atoms = myMolecule[j].end - myMolecule[j].start;

		//Center molecule x coordinate
		temp = 0.;
		for (i = myMolecule[j].start; i < myMolecule[j].end; ++i)
			temp += x_cartesian[i];

		temp /= number_of_atoms;

		for (i = myMolecule[j].start; i < myMolecule[j].end; ++i)
			x_cartesian[i] -= temp;

		//Center molecule  y coordinate
		temp = 0;
		for (i = myMolecule[j].start; i < myMolecule[j].end; ++i)
			temp += y_cartesian[i];

		temp /= number_of_atoms;

		for (i = myMolecule[j].start; i < myMolecule[j].end; ++i)
			y_cartesian[i] -= temp;

		//Center molecule  z coordinate
		temp = 0;
		for (i = myMolecule[j].start; i < myMolecule[j].end; ++i)
			temp += z_cartesian[i];

		temp /= number_of_atoms;

		for (i = myMolecule[j].start; i < myMolecule[j].end; ++i)
			z_cartesian[i] -= temp;
	}
}

inline void Import::FindStartEnd()
{
	//This function center molecules to they centers
	int j, k;
	int molecule_nr = -1;



	if (thread.NSURF == 0) return;

	//Find first and last atom of molecule j
	for (j = 0; j < thread.NSURF; ++j)
	{
		if (NewMoleculePositionInData[j])
		{
			molecule_nr++;
			if (molecule_nr >= MAXMOLECULES) break;
			myMolecule[molecule_nr].start = j;

			for (k = j + 1; k < thread.NSURF; ++k)
			{
				if (NewMoleculePositionInData[k])
				{
					break;
				}
				myMolecule[molecule_nr].end = k+1;
			}
		}
	}


}


inline void Import::CopyParametersToAllAtoms()
{
	int i, j;

	//Copy values from molecule to its atoms	
	for (j = 0; j < TotalMolecules; ++j)
	{
		for (i = myMolecule[j].start; i < myMolecule[j].end; ++i)
		{
			variable.XCONST[i] = 1;
			variable.NXDIS[i] = myMolecule[j].NXDIS;
			variable.YCONST[i] = 1;
			variable.NYDIS[i] = myMolecule[j].NYDIS;
			variable.ZCONST[i] = 1;
			variable.NZDIS[i] = myMolecule[j].NZDIS;
			thread.NDWS[i] = myMolecule[j].NDWS;
			thread.NDWS2[i] = myMolecule[j].NDWS2;
			thread.NOCCUP[i] = myMolecule[j].NOCCUP;
			variable.NROTATIONX[i] = myMolecule[j].NROTATIONX;
			variable.NROTATIONY[i] = myMolecule[j].NROTATIONY;
			variable.NROTATIONZ[i] = myMolecule[j].NROTATIONZ;
		}
	}
}