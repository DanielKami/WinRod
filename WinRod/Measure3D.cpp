//------------------------------------------------------------------------------------------------
//Functions used for measurements in 3D window
//------------------------------------------------------------------------------------------------

#include <stdafx.h>				//Header necessary for Visual Studio
#include <d3dx9.h>
#include "structures.h"
#include "Measure3D.h"
#include "ExMath.h"				//Additional mathematical functions
#include "Elements.h"	


void Measure::ResetMeasurement()
{
	measurement[IndexMeasure].measurement = 0; //reset measurement
}

void Measure::ResetAllMeasurements()
{
	IndexMeasure = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------
//    Distance between atoms
//--------------------------------------------------------------------------------------------------------------------------------
bool Measure::MeasureDistance(AtomParam *BulkAtom, int nr_bulk_atoms_toplot, AtomParam *SurfaceAtom, int nr_surf_atoms_toplot)
{

	int j, max_selected;
	double x1 = 0, y1 = 0, z1 = 0;
	double x2 = 0, y2 = 0, z2 = 0;
	WCHAR Symbol1[30];
	WCHAR Symbol2[30];
	double distans;

	max_selected = 0;
	//Check bulk atoms
	for (j = 0; j < nr_bulk_atoms_toplot; ++j) 
	{
		if (BulkAtom[j].Selected > 2)
		{
			MessageBox(NULL, TEXT("Too many atoms selected for distance calculation!"), NULL, MB_ICONERROR);
			return false;
		}
		if (BulkAtom[j].Selected > 0) max_selected++;
	}
	//Check surface atoms
	for (j = 0; j < nr_surf_atoms_toplot; ++j)
	{
		if (SurfaceAtom[j].Selected > 2){
			MessageBox(NULL, TEXT("Too many atoms selected for distance calculation!"), NULL, MB_ICONERROR);
			return false;
		}
		if (SurfaceAtom[j].Selected > 0) max_selected++;
	}

	if (max_selected < 2) 
	{
		MessageBox(NULL, TEXT("Not enought atoms selected for distance calculations!"), NULL, MB_ICONERROR);
		return false;
	}

	//If the number of atoms is 2 we can find the atoms coordinates

	//Copy from bulk if they are there
	for (j = 0; j < nr_bulk_atoms_toplot; ++j)
	{
		if (BulkAtom[j].Selected == 1)
		{
			x1 = BulkAtom[j].x;
			y1 = BulkAtom[j].y;
			z1 = BulkAtom[j].z;
			swprintf_s(Symbol1, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}
		if (BulkAtom[j].Selected == 2)
		{
			x2 = BulkAtom[j].x;
			y2 = BulkAtom[j].y;
			z2 = BulkAtom[j].z;
			swprintf_s(Symbol2, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom2.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}
	}
	//Copy from surface if they are there
	for (j = 0; j < nr_surf_atoms_toplot; ++j)
	{
		if (SurfaceAtom[j].Selected == 1){
			x1 = SurfaceAtom[j].x;
			y1 = SurfaceAtom[j].y;
			z1 = SurfaceAtom[j].z;
			swprintf_s(Symbol1, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
		if (SurfaceAtom[j].Selected == 2)
		{
			x2 = SurfaceAtom[j].x;
			y2 = SurfaceAtom[j].y;
			z2 = SurfaceAtom[j].z;
			swprintf_s(Symbol2, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
	}

	distans = sqrt((x1 - x2)*(x1 - x2) +
		(y1 - y2)*(y1 - y2) +
		(z1 - z2)*(z1 - z2));
	WCHAR TStr[100];
	swprintf_s(TStr, 100, L"Distance between %s and %s is %4.3f\305.", Symbol1, Symbol2, distans);
	MessageBox(NULL, TStr, L"Distance", MB_OK);

	//copy the measurement for presentation
	measurement[IndexMeasure].measurement = 1; //distance
	measurement[IndexMeasure].value = distans;
	measurement[IndexMeasure].Atom1.x = (float)x1;
	measurement[IndexMeasure].Atom1.y = (float)y1;
	measurement[IndexMeasure].Atom1.z = (float)z1;
	measurement[IndexMeasure].Atom2.x = (float)x2;
	measurement[IndexMeasure].Atom2.y = (float)y2;
	measurement[IndexMeasure].Atom2.z = (float)z2;
	if (IndexMeasure < MAX_MEASUREMENTS)
		IndexMeasure++;

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------
//Measure angle between 3 selected atoms
//
//--------------------------------------------------------------------------------------------------------------------------------
bool Measure::MeasureAngle(AtomParam *BulkAtom, int nr_bulk_atoms_toplot, AtomParam *SurfaceAtom, int nr_surf_atoms_toplot)
{

	int j;
	double distance1, distance2, distance3;
	double x1 = 0, y1 = 0, z1 = 0;
	double x2 = 0, y2 = 0, z2 = 0;
	double x3 = 0, y3 = 0, z3 = 0;
	double angle;
	WCHAR Symbol1[100], Symbol2[100], Symbol3[100];

	int max_selected = 0;
	//Check bulk atoms
	for (j = 0; j < nr_bulk_atoms_toplot; j++)
	{
		if (BulkAtom[j].Selected > 3)
		{
			MessageBox(NULL, TEXT("Too many atoms in bulk selected for angle calculation!"), NULL, MB_ICONERROR);
			return false;
		}
		if (BulkAtom[j].Selected > 0) max_selected++;
	}
	//Check surface atoms
	for (j = 0; j < nr_surf_atoms_toplot; j++)
	{
		if (SurfaceAtom[j].Selected > 3)
		{
			MessageBox(NULL, TEXT("Too many atoms selected for angle calculation!"), NULL, MB_ICONERROR);
			return false;
		}
		if (SurfaceAtom[j].Selected > 0) max_selected++;
	}

	if (max_selected < 3)
	{
		MessageBox(NULL, TEXT("Not enought atoms selected for angle calculations!"), NULL, MB_ICONERROR);
		return false;
	}

	//If the number of atoms is 2 we can find the atoms coordinates

	//Copy from bulk if they are there
	for (j = 0; j < nr_bulk_atoms_toplot; j++)
	{
		if (BulkAtom[j].Selected == 1)
		{
			x1 = BulkAtom[j].x;
			y1 = BulkAtom[j].y;
			z1 = BulkAtom[j].z;
			swprintf_s(Symbol1, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}
		if (BulkAtom[j].Selected == 2)
		{
			x2 = BulkAtom[j].x;
			y2 = BulkAtom[j].y;
			z2 = BulkAtom[j].z;
			swprintf_s(Symbol2, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}

		if (BulkAtom[j].Selected == 3)
		{
			x3 = BulkAtom[j].x;
			y3 = BulkAtom[j].y;
			z3 = BulkAtom[j].z;
			swprintf_s(Symbol3, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}


	}
	//Copy from surface if they are there
	for (j = 0; j < nr_surf_atoms_toplot; ++j)
	{
		if (SurfaceAtom[j].Selected == 1)
		{
			x1 = SurfaceAtom[j].x;
			y1 = SurfaceAtom[j].y;
			z1 = SurfaceAtom[j].z;
			swprintf_s(Symbol1, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 3, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
		if (SurfaceAtom[j].Selected == 2)
		{
			x2 = SurfaceAtom[j].x;
			y2 = SurfaceAtom[j].y;
			z2 = SurfaceAtom[j].z;
			swprintf_s(Symbol2, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 3, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
		if (SurfaceAtom[j].Selected == 3)
		{
			x3 = SurfaceAtom[j].x;
			y3 = SurfaceAtom[j].y;
			z3 = SurfaceAtom[j].z;
			swprintf_s(Symbol3, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 3, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
	}

	//---------------------------
	distance1 = sqrt((x1 - x2)*(x1 - x2) +
		(y1 - y2)*(y1 - y2) +
		(z1 - z2)*(z1 - z2));

	distance2 = sqrt((x2 - x3)*(x2 - x3) +
		(y2 - y3)*(y2 - y3) +
		(z2 - z3)*(z2 - z3));

	distance3 = sqrt((x1 - x3)*(x1 - x3) +
		(y1 - y3)*(y1 - y3) +
		(z1 - z3)*(z1 - z3));

	//ABC = arc cos (AB*AB + BC*BC - AC*AC) / (2*AB*AC)

	double temp_calc = distance1*distance1 + distance2*distance2 - distance3*distance3;
	temp_calc /= 2 * distance1 * distance2;
	angle = 180.0 / PI*acos(temp_calc);
	WCHAR TStr[100];
	swprintf_s(TStr, 100, L" Angle between %s, %s and %s is %3.3f\272.", Symbol1, Symbol2, Symbol3, angle);
	MessageBox(NULL, TStr, L"Angle", MB_OK);


	//copy the measurement for presentation
	measurement[IndexMeasure].measurement = 2; //angle
	measurement[IndexMeasure].value = angle;
	measurement[IndexMeasure].Atom1.x = (float)x1;
	measurement[IndexMeasure].Atom1.y = (float)y1;
	measurement[IndexMeasure].Atom1.z = (float)z1;
	measurement[IndexMeasure].Atom2.x = (float)x2;
	measurement[IndexMeasure].Atom2.y = (float)y2;
	measurement[IndexMeasure].Atom2.z = (float)z2;
	measurement[IndexMeasure].Atom3.x = (float)x3;
	measurement[IndexMeasure].Atom3.y = (float)y3;
	measurement[IndexMeasure].Atom3.z = (float)z3;
	if (IndexMeasure < MAX_MEASUREMENTS)
		IndexMeasure++;

	return true;
}


bool Measure::MeasureTorsian(AtomParam *BulkAtom, int nr_bulk_atoms_toplot, AtomParam *SurfaceAtom, int nr_surf_atoms_toplot)
{

	int j;
	double b_c[3], b_a[3], c_d[3];


	double x1 = 0, y1 = 0, z1 = 0;
	double x2 = 0, y2 = 0, z2 = 0;
	double x3 = 0, y3 = 0, z3 = 0;
	double x4 = 0, y4 = 0, z4 = 0;

	double angle;
	WCHAR Symbol1[100], Symbol2[100], Symbol3[100], Symbol4[100];

	int max_selected = 0;
	//Check bulk atoms
	for (j = 0; j < nr_bulk_atoms_toplot; j++)
	{
		if (BulkAtom[j].Selected > 4)
		{
			MessageBox(NULL, TEXT("Too many atoms selected for torsian angle calculation!"), NULL, MB_ICONERROR);
			return false;
		}
		if (BulkAtom[j].Selected > 0) max_selected++;
	}
	//Check surface atoms
	for (j = 0; j < nr_surf_atoms_toplot; j++)
	{
		if (SurfaceAtom[j].Selected > 4){
			MessageBox(NULL, TEXT("Too many atoms selected for torsian angle calculation!"), NULL, MB_ICONERROR);
			return false;
		}
		if (SurfaceAtom[j].Selected > 0) max_selected++;
	}

	if (max_selected < 4)
	{
		MessageBox(NULL, TEXT("Not enought atoms selected for torsian angle calculations, select 4 atoms!"), NULL, MB_ICONERROR);
		return false;
	}

	//If the number of atoms is 2 we can find the atoms coordinates

	//Copy from bulk if they are there
	for (j = 0; j < nr_bulk_atoms_toplot; j++)
	{
		if (BulkAtom[j].Selected == 1)
		{
			x1 = BulkAtom[j].x;
			y1 = BulkAtom[j].y;
			z1 = BulkAtom[j].z;
			swprintf_s(Symbol1, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}
		if (BulkAtom[j].Selected == 2)
		{
			x2 = BulkAtom[j].x;
			y2 = BulkAtom[j].y;
			z2 = BulkAtom[j].z;
			swprintf_s(Symbol2, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}

		if (BulkAtom[j].Selected == 3)
		{
			x3 = BulkAtom[j].x;
			y3 = BulkAtom[j].y;
			z3 = BulkAtom[j].z;
			swprintf_s(Symbol3, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}
		if (BulkAtom[j].Selected == 4)
		{
			x4 = BulkAtom[j].x;
			y4 = BulkAtom[j].y;
			z4 = BulkAtom[j].z;
			swprintf_s(Symbol4, 30, L"bulk atom %s%d", W_elements[BulkAtom[j].AtomicNumber], BulkAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[BulkAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = BulkAtom[j].AtomInTheFile;
		}
	}
	//Copy from surface if they are there
	for (j = 0; j < nr_surf_atoms_toplot; j++)
	{
		if (SurfaceAtom[j].Selected == 1){
			x1 = SurfaceAtom[j].x;
			y1 = SurfaceAtom[j].y;
			z1 = SurfaceAtom[j].z;
			swprintf_s(Symbol1, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
		if (SurfaceAtom[j].Selected == 2)
		{
			x2 = SurfaceAtom[j].x;
			y2 = SurfaceAtom[j].y;
			z2 = SurfaceAtom[j].z;
			swprintf_s(Symbol2, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
		if (SurfaceAtom[j].Selected == 3)
		{
			x3 = SurfaceAtom[j].x;
			y3 = SurfaceAtom[j].y;
			z3 = SurfaceAtom[j].z;
			swprintf_s(Symbol3, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
		if (SurfaceAtom[j].Selected == 4)
		{
			x4 = SurfaceAtom[j].x;
			y4 = SurfaceAtom[j].y;
			z4 = SurfaceAtom[j].z;
			swprintf_s(Symbol4, 30, L"surface atom %s%d", W_elements[SurfaceAtom[j].AtomicNumber], SurfaceAtom[j].AtomInTheFile);
			swprintf_s(W_elements[measurement[IndexMeasure].Atom1.AtomicNumber], 4, L"%s", W_elements[SurfaceAtom[j].AtomicNumber]);
			measurement[IndexMeasure].Atom1.AtomInTheFile = SurfaceAtom[j].AtomInTheFile;
		}
	}

	//b1
	b_a[0] = -(x1 - x2);
	b_a[1] = -(y1 - y2);
	b_a[2] = -(z1 - z2);

	//b2
	b_c[0] = x2 - x3;
	b_c[1] = y2 - y3;
	b_c[2] = z2 - z3;

	//b3
	c_d[0] = x4 - x3;
	c_d[1] = y4 - y3;
	c_d[2] = z4 - z3;

	/*
	//Method 2
	double b_a_ort[3];
	double c_d_ort[3];
	//b1
	b_a[0] = (x1 - x2);
	b_a[1] = (y1 - y2);
	b_a[2] = (z1 - z2);

	VectroNormalisation(b_c);
	OrtNormVector(b_a, b_c, b_a_ort);
	OrtNormVector(c_d, b_c, c_d_ort);
	double angle2;
	double sign;
	double cross[3];

	CrossProduct(b_a_ort, c_d_ort, cross);
	sign = DotProduct(cross, b_c);
	if (sign>0)
	angle2 = 180.0 / PI * acos(DotProduct(b_a_ort, c_d_ort));
	else
	angle2 = -180.0 / PI * acos(DotProduct(b_a_ort, c_d_ort));
	*/

	//Method 3
	double n1[3];
	double n2[3];
	double m[3];
	double x, y;
	VectorNormalisation(b_c);
	VectorNormalisation(b_a);
	VectorNormalisation(c_d);

	CrossProduct(b_a, b_c, n1);
	CrossProduct(b_c, c_d, n2);
	CrossProduct(n1, b_c, m);

	x = DotProduct(n1, n2);
	y = DotProduct(m, n2);

	angle = 180.0 / PI * atan2(y, x);

	WCHAR TStr[300];
	swprintf_s(TStr, 300, L" Torsian angle between %s, %s, %s and %s is %3.3f\272.", Symbol1, Symbol2, Symbol3, Symbol4, angle);
	MessageBox(NULL, TStr, L"Torsian angle", MB_OK);

	//copy the measurement for presentation
	measurement[IndexMeasure].measurement = 3; //duhedral
	measurement[IndexMeasure].value = angle;
	measurement[IndexMeasure].Atom1.x = (float)x1;
	measurement[IndexMeasure].Atom1.y = (float)y1;
	measurement[IndexMeasure].Atom1.z = (float)z1;
	measurement[IndexMeasure].Atom2.x = (float)x2;
	measurement[IndexMeasure].Atom2.y = (float)y2;
	measurement[IndexMeasure].Atom2.z = (float)z2;
	measurement[IndexMeasure].Atom3.x = (float)x3;
	measurement[IndexMeasure].Atom3.y = (float)y3;
	measurement[IndexMeasure].Atom3.z = (float)z3;
	measurement[IndexMeasure].Atom4.x = (float)x4;
	measurement[IndexMeasure].Atom4.y = (float)y4;
	measurement[IndexMeasure].Atom4.z = (float)z4;
	if (IndexMeasure < MAX_MEASUREMENTS)
		IndexMeasure++;

	return true;
}


void Measure::MouseSelect(POINT *pt, int Width, int Height, D3DXMATRIX *TransformationMatrix, AtomParam *Atom, int i, int MenuHeight, bool flag_perspective)
{

	static int NrSelectedAtoms = 0; //Number of all selected atoms

	if (pt->x == -3)
		NrSelectedAtoms = 0;

	double factor = TOLERANCE;
	if (!flag_perspective)
		factor = TOLERANCE * 1.5;

	int modelX = (int)(Width / 2 * (1.0 + TransformationMatrix->_41 / TransformationMatrix->_44));
	int modelY = (int)(Height / 2 * (1.0 - TransformationMatrix->_42 / TransformationMatrix->_44));

	//Corection for viewport size related to menu height
	int temp_pt_y = (int)(pt->y - (Height - pt->y) / Height - MenuHeight);

	if (modelX < pt->x + factor  && modelX > pt->x - factor  &&  modelY < temp_pt_y + factor  && modelY > temp_pt_y - factor)
	{
		if (Atom[i].Selected > 0)
		{
			Atom[i].Selected = 0; //if 0 is not selected 
			NrSelectedAtoms--;
		}
		else
		{
			NrSelectedAtoms++;
			Atom[i].Selected = NrSelectedAtoms;
		}
		pt->x = -1;
		pt->y = -1;
	}
}
