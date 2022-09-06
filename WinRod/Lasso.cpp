//--------------------------------------------------------------------------------------------------------------------------------
//The file contain all the necessary support for lasso selection method in 3D scene.
//Created by: Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------

#include <stdafx.h>
#include "lasso.h"

#define MAX_COMPLETE_LASSO_POINTS 2*MAX_LASSO_POINTS

void	Lasso::ResetSelection(POINT pt, AtomParam* BulkAtoms, int NrBulkAtoms, AtomParam* SurfaceAtoms, int NrSurfAtoms)
{
		//Delete all selections
		if(pt.x == -3){
			for(int i=0; i<NrSurfAtoms; ++i)
			{
				SurfaceAtoms[i].Selected = 0;
			}
			for(int i=0; i<NrBulkAtoms; ++i)
			{
				BulkAtoms[i].Selected = 0;
			}
		}
}

void	Lasso::MouseLasso(D3DXVECTOR2 *VertexList, int NrOfPoint, int Width, int Height, D3DXMATRIX *RotateViewProjectionMatrix, AtomParam* BulkAtoms, int NrBulkAtoms, AtomParam* SurfaceAtoms, int NrSurfAtoms, int MenuHeight)
{

	static int NrSelectedAtoms = 0; //Number of all selected atoms
	static POINT CompleteLassoPoints[MAX_COMPLETE_LASSO_POINTS + 2];
	int i, j;

	int point_lasso = 0;
	//First close the shape eliminate all gaps
	CompleteLassoPoints[point_lasso].x = (int)VertexList[0].x; CompleteLassoPoints[point_lasso].y = (int)VertexList[0].y;
	point_lasso++;
	for (i = 1; i < NrOfPoint; ++i)
	{
		if (point_lasso == MAX_COMPLETE_LASSO_POINTS) point_lasso = 0;
		LineLasso(VertexList[i - 1], VertexList[i], CompleteLassoPoints, &point_lasso);
	}

	LineLasso(VertexList[NrOfPoint - 1], VertexList[0], CompleteLassoPoints, &point_lasso);

	Mask = new bool[(Height*Width + Width) +1];
	ZeroMemory(Mask, sizeof(bool)*(Height*Width+Width));


	for (i = 0; i <= point_lasso; ++i)
	{
		if (CompleteLassoPoints[i].x < 0)
			CompleteLassoPoints[i].x = 0;
		Mask[CompleteLassoPoints[i].y*Width + CompleteLassoPoints[i].x] = true;
	}

	//Fill the outside area and prepare the mask
	BoundaryFill(1, 1, Width, Height, 1, 1);

	//Reverse selection
	int Temp;
	for (i = 0; i < Width; ++i)
	{
		for (j = 0; j < Height; ++j)
		{
			Temp = i + j * Width;
			if (Mask[Temp])
				Mask[Temp] = false;
			else Mask[Temp] = true;
		}
	}

	//__________________________________________________________________________________________________________________
	//	Find atoms
	D3DXMATRIXA16 world;
	int matrix_size = Width * Height;
	int iterator;
	for (i = 0; i < NrBulkAtoms; ++i)
	{
		D3DXMatrixTranslation(&world, (float)BulkAtoms[i].x, (float)BulkAtoms[i].y, (float)BulkAtoms[i].z);
		D3DXMATRIXA16 worldViewProjectionMatrix = world * *RotateViewProjectionMatrix;

		int modelX = (int)(Width / 2 * (1.0 + worldViewProjectionMatrix._41 / worldViewProjectionMatrix._44));
		int modelY = (int)(Height / 2 * (1.0 - worldViewProjectionMatrix._42 / worldViewProjectionMatrix._44));

		//Corection for viewport size related to menu height
		modelY = (int)(modelY - MenuHeight / 4 + 10);
		iterator = modelX + modelY * Width;
		if (iterator < matrix_size && iterator>0)
			if (Mask[iterator])
			{
				NrSelectedAtoms++;
				BulkAtoms[i].Selected = NrSelectedAtoms;
			}
	}

	//Surf
	for (i = 0; i < NrSurfAtoms; ++i)
	{
		D3DXMatrixTranslation(&world, (float)SurfaceAtoms[i].x, (float)SurfaceAtoms[i].y, (float)SurfaceAtoms[i].z);
		D3DXMATRIXA16 worldViewProjectionMatrix = world * *RotateViewProjectionMatrix;

		int modelX = (int)(Width / 2 * (1.0 + worldViewProjectionMatrix._41 / worldViewProjectionMatrix._44));
		int modelY = (int)(Height / 2 * (1.0 - worldViewProjectionMatrix._42 / worldViewProjectionMatrix._44));

		//Corection for viewport size related to the menu height
		modelY = (int)(modelY - MenuHeight / 4 + 10);

		//Atoms can be out of screen
		iterator = modelX + modelY * Width;
		if (iterator < matrix_size && iterator>0)
			if (Mask[iterator])
			{
				NrSelectedAtoms++;
				SurfaceAtoms[i].Selected = NrSelectedAtoms;
			}
	}

	delete[] Mask;
}


//Fill intermediate points
void Lasso::LineLasso( D3DXVECTOR2 Pos1, D3DXVECTOR2 Pos2, POINT *Lasso, int *point_lasso)
{

 float delta_x;
 float delta_y;
 int i;
 int delta_i;
 int count= *point_lasso;

	delta_x=(Pos2.x-Pos1.x);
	delta_y=(Pos2.y-Pos1.y);
	//take longer vector
	if (abs((int)delta_x)>abs((int)delta_y))
		delta_i=abs((int)delta_x);
	 else
		delta_i=abs((int)delta_y);

	delta_x/=delta_i;
	delta_y/=delta_i;

	for( i=0; i<delta_i; ++i)
	{
		Lasso[count].x=(int)(Pos1.x+delta_x*i);
		Lasso[count].y=(int)(Pos1.y+delta_y*i);
		count++;
		if (count>MAX_COMPLETE_LASSO_POINTS) count = 0;
	}
	*point_lasso=count;
}



//----------------------------  FloodFill( )  ---------------------------//

 void Lasso::insert_pixel(const int x,const int y)
    {
       Entry=new Pixel;

       Entry->x=x;
       Entry->y=y;

       Entry->Next=NULL;
       Last->Next=Entry;
       Last=Entry;
    }

 void Lasso::BoundaryFill(const int _x, const int _y, int Width, int Height, const bool fill_color, const bool boundary_color)
 {

	 if (Mask[_x + _y*Width] == boundary_color || Mask[_x + _y*Width] == fill_color)
		 return;

	 int x = _x;
	 int y = _y;

	 Start = new Pixel;

	 Start->x = x;
	 Start->y = y;

	 Start->Next = NULL;
	 Last = Start;

	 while (Start != NULL)
	 {
		 Mask[_x + _y*Width] = fill_color;

		 if ((x - 1) >= 0 && Mask[x - 1 + y*Width] != boundary_color && Mask[x - 1 + y*Width] != fill_color)
		 {
			 Mask[x - 1 + y*Width] = fill_color;
			 insert_pixel((x - 1), y);
		 }

		 if ((y - 1) >= 0 && Mask[x + (y - 1)*Width] != boundary_color && Mask[x + (y - 1)*Width] != fill_color)
		 {
			 Mask[x + (y - 1)*Width] = fill_color;
			 insert_pixel(x, (y - 1));
		 }

		 if ((x + 1) <= Width && Mask[x + 1 + y*Width] != boundary_color && Mask[x + 1 + y*Width] != fill_color)
		 {
			 Mask[x + 1 + y*Width] = fill_color;
			 insert_pixel((x + 1), y);
		 }

		 if ((y + 1) <= Height && Mask[x + (y + 1)*Width] != boundary_color && Mask[x + (y + 1)*Width] != fill_color)
		 {
			 Mask[x + (y + 1)*Width] = fill_color;
			 insert_pixel(x, (y + 1));
		 }

		 Entry = Start;
		 Start = Start->Next;

		 if (Start)x = Start->x;
		 if (Start)y = Start->y;

		 delete Entry;
	 }
 }