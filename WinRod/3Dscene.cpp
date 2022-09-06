////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Plot scene functions
//by Daniel Kaminski
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdafx.h>				//Header necessary for Visual Studio
#include <commctrl.h>

#include "lasso.h"
#include "thread.h"
#include "variables.h"
#include "stdio.h"
#include "3Ddef.h"				//Definition for 3D graphisc
#include "DX_Ui.h"				//Set the environment for 3D graphisc DirectX9.0c + HSLS 2.0 shaders
#include "Measure3D.h"
#include "Camera.h"				//Interpret the mouse movments and calculate all necesary matrtices for position and rotation of word and camera for 3D
#include "3Dscene.h"
#include "Scene3D.h"

Lasso						lasso;						//All functions for lasso selection method
extern Camera						MyCamera;					//Main camera class
extern DX_Ui						myDX_Ui;					//Main system class

//Register 3D objects
extern ID3DXEffect                 *g_pEffect;
LPD3DXLINE					_line;						//For measurements
ID3DXFont                   *g_pFont;
extern COrbiter					*g_Orbiter;	// Orbiter meshes

extern float shift_x, shift_y;
extern MEASUREMENT measurement[MAX_MEASUREMENTS];
extern int IndexMeasure;
extern FLAG flag;
extern AtomParam *D3DbulkAtom = NULL;
extern AtomParam *D3DsurfAtom = NULL;
extern ATOM_PROPERITIES AtProp;

//Read section
extern ModelStruct Bul;
extern ModelStruct Sur;
extern ModelStruct Fit;
extern DataStruct  Dat;

AtomParam *D3DcompAtom = NULL;			//for comparison after fitting
BondParam *D3DbulkBonds = NULL;
BondParam *D3DsurfBonds = NULL;
BondParam *D3DcompBonds = NULL;


int nr_bulk_atoms_toplot;				//3D
int nr_surf_atoms_toplot;				//3D

inline void DrawCell(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix)
{
	WCHAR str[10];
	RECT rcClient;
	D3DXVECTOR3 VertexList[16];


	//1
	VertexList[0].x = -shift_x;
	VertexList[0].y = -shift_y;
	VertexList[0].z = 0;
	//2
	VertexList[1].x = (float)thread.DLAT[0] - shift_x;
	VertexList[1].y = -shift_y;
	VertexList[1].z = 0;
	//3
	VertexList[2].x = (float)(thread.DLAT[0] + variable.Dlat1cosDlat5) - shift_x;
	VertexList[2].y = (float)(variable.Dlat1sinDlat5) - shift_y;
	VertexList[2].z = 0;
	//4
	VertexList[3].x = (float)(variable.Dlat1cosDlat5) - shift_x;
	VertexList[3].y = (float)(variable.Dlat1sinDlat5) - shift_y;
	VertexList[3].z = 0;
	//1
	VertexList[4].x = -shift_x;
	VertexList[4].y = -shift_y;
	VertexList[4].z = 0;

	//1
	VertexList[5].x = (float)(variable.Dlat2cosDlat3) - shift_x;
	VertexList[5].y = (float)(variable.Dlat2cosDlat4) - shift_y;
	VertexList[5].z = (float)thread.DLAT[2];
	//2
	VertexList[6].x = (float)(thread.DLAT[0] + variable.Dlat2cosDlat3) - shift_x;
	VertexList[6].y = (float)(variable.Dlat2cosDlat4) - shift_y;
	VertexList[6].z = (float)thread.DLAT[2];
	//3
	VertexList[7].x = (float)(thread.DLAT[0] + variable.Dlat1cosDlat5 + variable.Dlat2cosDlat3) - shift_x;
	VertexList[7].y = (float)(variable.Dlat1sinDlat5 + variable.Dlat2cosDlat4) - shift_y;
	VertexList[7].z = (float)thread.DLAT[2];
	//4
	VertexList[8].x = (float)(variable.Dlat1cosDlat5 + variable.Dlat2cosDlat3) - shift_x;
	VertexList[8].y = (float)(variable.Dlat1sinDlat5 + variable.Dlat2cosDlat4) - shift_y;
	VertexList[8].z = (float)thread.DLAT[2];
	//1
	VertexList[9].x = (float)(variable.Dlat2cosDlat3) - shift_x;
	VertexList[9].y = (float)(variable.Dlat2cosDlat4) - shift_y;
	VertexList[9].z = (float)thread.DLAT[2];

	//2 up
	VertexList[10].x = VertexList[6].x;
	VertexList[10].y = VertexList[6].y;
	VertexList[10].z = VertexList[6].z;
	//2 down
	VertexList[11].x = VertexList[1].x;
	VertexList[11].y = -shift_y;
	VertexList[11].z = 0;
	//3 down
	VertexList[12].x = VertexList[2].x;
	VertexList[12].y = VertexList[2].y;
	VertexList[12].z = VertexList[2].z;
	//3 up
	VertexList[13].x = VertexList[7].x;
	VertexList[13].y = VertexList[7].y;
	VertexList[13].z = VertexList[7].z;
	//4 up
	VertexList[14].x = VertexList[8].x;
	VertexList[14].y = VertexList[8].y;
	VertexList[14].z = VertexList[8].z;
	//4 down
	VertexList[15].x = VertexList[3].x;
	VertexList[15].y = VertexList[3].y;
	VertexList[15].z = VertexList[3].z;

	// Draw the line.
	_line->SetWidth(2.0f);
	_line->Begin();
	_line->DrawTransform(VertexList, 16, RotateViewProjectionMatrix, D3DXCOLOR(0.5f, 0.5f, 0.1f, 1.0f));
	_line->End();


	//////////////////////////////////////// Draw x vector////////////////////////////////////////////////////////
	//1
	VertexList[0].x = -shift_x;
	VertexList[0].y = -shift_y;
	VertexList[0].z = 0;
	//2
	VertexList[1].x = (float)thread.DLAT[0] - shift_x;
	VertexList[1].y = -shift_y;
	VertexList[1].z = 0;


	_line->SetWidth(2.0f);
	_line->Begin();
	_line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.9f, 0.1f, 0.1f, 1.0f));
	_line->End();

	swprintf_s(str, 20, L"x");

	D3DXMatrixTranslation(world, VertexList[1].x, VertexList[1].y, VertexList[1].z);
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

	rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
	rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
	rcClient.left -= 12;
	rcClient.top -= 20;
	rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
	g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 0, 0));


	//////////////////////////////////////// Draw y vector////////////////////////////////////////////////////////
	//1
	VertexList[0].x = -shift_x;
	VertexList[0].y = -shift_y;
	VertexList[0].z = 0;
	//2
	VertexList[1].x = (float)(variable.Dlat1cosDlat5) - shift_x;
	VertexList[1].y = (float)(variable.Dlat1sinDlat5) - shift_y;
	VertexList[1].z = 0;


	_line->SetWidth(2.0f);
	_line->Begin();
	_line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.9f, 0.9f, 0.1f, 1.0f));
	_line->End();

	swprintf_s(str, 20, L"y");

	D3DXMatrixTranslation(world, VertexList[1].x, VertexList[1].y, VertexList[1].z);
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

	rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
	rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
	rcClient.left -= 12;
	rcClient.top -= 20;
	rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
	g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 255, 0));


	//////////////////////////////////////// Draw z vector////////////////////////////////////////////////////////
	//1
	VertexList[0].x = -shift_x;
	VertexList[0].y = -shift_y;
	VertexList[0].z = 0;
	//2
	VertexList[1].x = (float)(variable.Dlat2cosDlat3) - shift_x;
	VertexList[1].y = (float)(variable.Dlat2cosDlat4) - shift_y;
	VertexList[1].z = (float)thread.DLAT[2];


	_line->SetWidth(2.0f);
	_line->Begin();
	_line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.9f, 0.1f, 1.0f));
	_line->End();

	swprintf_s(str, 20, L"z");

	D3DXMatrixTranslation(world, VertexList[1].x, VertexList[1].y, VertexList[1].z);
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

	rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
	rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
	rcClient.left -= 12;
	rcClient.top -= 20;
	rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
	g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));

}

inline void DrawMeasurement(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix)
{
	WCHAR str[10];
	RECT rcClient;
	D3DXVECTOR3 VertexList[6];

	for (int i = 0; i < IndexMeasure; ++i)
	{
		if (measurement[i].measurement == 1){ //distance

			VertexList[0].x = (float)measurement[i].Atom1.x;
			VertexList[0].y = (float)measurement[i].Atom1.y;
			VertexList[0].z = (float)measurement[i].Atom1.z;

			VertexList[1].x = (float)measurement[i].Atom2.x;
			VertexList[1].y = (float)measurement[i].Atom2.y;
			VertexList[1].z = (float)measurement[i].Atom2.z;

			// Draw the line.
			_line->SetWidth(3.0f);
			_line->Begin();
			_line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.5f, 0.1f, 1.0f));
			_line->End();
			//draw measurement
			swprintf_s(str, 20, L"%4.3f\305", measurement[i].value);

			D3DXMatrixTranslation(world, (float)(measurement[i].Atom1.x + measurement[i].Atom2.x) / 2, (float)(measurement[i].Atom1.y + measurement[i].Atom2.y) / 2, (float)(measurement[i].Atom1.z + measurement[i].Atom2.z) / 2);
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

			rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
			rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
			rcClient.left -= 12;
			rcClient.top -= 20;
			rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
			g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));
		}

		if (measurement[i].measurement == 2){ //angle

			VertexList[0].x = (float)measurement[i].Atom1.x;
			VertexList[0].y = (float)measurement[i].Atom1.y;
			VertexList[0].z = (float)measurement[i].Atom1.z;

			VertexList[1].x = (float)measurement[i].Atom2.x;
			VertexList[1].y = (float)measurement[i].Atom2.y;
			VertexList[1].z = (float)measurement[i].Atom2.z;

			VertexList[2].x = (float)measurement[i].Atom3.x;
			VertexList[2].y = (float)measurement[i].Atom3.y;
			VertexList[2].z = (float)measurement[i].Atom3.z;

			// Draw the line.
			_line->SetWidth(3.0f);
			_line->Begin();
			_line->DrawTransform(VertexList, 3, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.5f, 0.1f, 1.0f));
			_line->End();

			//draw measurement
			swprintf_s(str, 20, L"%4.2f\272", measurement[i].value);

			D3DXMatrixTranslation(world, (float)(measurement[i].Atom1.x + measurement[i].Atom2.x + measurement[i].Atom3.x) / 3, (float)(measurement[i].Atom1.y + measurement[i].Atom2.y + measurement[i].Atom3.y) / 3, (float)(measurement[i].Atom1.z + measurement[i].Atom2.z + measurement[i].Atom3.z) / 3);

			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

			rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
			rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
			rcClient.left -= 12;
			rcClient.top -= 20;
			rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
			g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));
		}

		if (measurement[i].measurement == 3){ //dihedral

			VertexList[0].x = (float)measurement[i].Atom3.x;
			VertexList[0].y = (float)measurement[i].Atom3.y;
			VertexList[0].z = (float)measurement[i].Atom3.z;

			VertexList[1].x = (float)measurement[i].Atom2.x;
			VertexList[1].y = (float)measurement[i].Atom2.y;
			VertexList[1].z = (float)measurement[i].Atom2.z;

			VertexList[2].x = (float)measurement[i].Atom1.x;
			VertexList[2].y = (float)measurement[i].Atom1.y;
			VertexList[2].z = (float)measurement[i].Atom1.z;

			VertexList[3].x = (float)measurement[i].Atom3.x;
			VertexList[3].y = (float)measurement[i].Atom3.y;
			VertexList[3].z = (float)measurement[i].Atom3.z;

			VertexList[4].x = (float)measurement[i].Atom4.x;
			VertexList[4].y = (float)measurement[i].Atom4.y;
			VertexList[4].z = (float)measurement[i].Atom4.z;

			VertexList[5].x = (float)measurement[i].Atom2.x;
			VertexList[5].y = (float)measurement[i].Atom2.y;
			VertexList[5].z = (float)measurement[i].Atom2.z;

			// Draw the line.
			_line->SetWidth(3.0f);
			_line->Begin();
			_line->DrawTransform(VertexList, 6, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.5f, 0.1f, 1.0f));
			_line->End();

			//draw measurement
			swprintf_s(str, 20, L"%4.2f\272", measurement[i].value);

			D3DXMatrixTranslation(world, (float)(measurement[i].Atom1.x + measurement[i].Atom2.x + measurement[i].Atom3.x + measurement[i].Atom4.x) / 4, (float)(measurement[i].Atom1.y + measurement[i].Atom2.y + measurement[i].Atom3.y + measurement[i].Atom4.y) / 4, (float)(measurement[i].Atom1.z + measurement[i].Atom2.z + measurement[i].Atom3.z + measurement[i].Atom4.z) / 4);
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

			rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
			rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
			rcClient.left -= 12;
			rcClient.top -= 20;
			rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
			g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));
		}
	}
}

inline void DrawPlane(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses)
{
	HRESULT hr;

	g_pEffect->SetTechnique("Blend");
	g_pEffect->SetBool("Drawshadows", false);

	float g_plane[4] = { 0.5f, 0.1f, 0.9f, 0.1f };
	g_pEffect->SetValue("g_ObiectColor", g_plane, sizeof(float) * 4);

	float ObjectScale = 1.f;
	D3DXMATRIXA16 mTemp;
	D3DXMatrixIdentity(&mTemp);
	D3DXMatrixScaling(world, ObjectScale, ObjectScale, ObjectScale);
	D3DXMatrixRotationX(&mTemp, (float)(90.*deg2rad));
	*world *= mTemp;
	// Set the matrices for the shader.
	g_pEffect->SetMatrix("g_mWorld", world);


	// Calculate combined world-view-projection matrix.
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
	g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

	// Render.
	if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0))){
		for (UINT pass = 0; pass < *totalPasses; ++pass){
			if (SUCCEEDED(g_pEffect->BeginPass(pass))){
				for (DWORD l = 0; l < g_Orbiter[1].m_dwNumMaterials; ++l){
					//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
					// Initialize required parameter
					V(g_pEffect->CommitChanges());
					// Render the mesh with the applied technique
					V(g_Orbiter[1].m_Mesh->DrawSubset(l));
				}
				g_pEffect->EndPass();
			}
		}
		g_pEffect->End();
	}
}

inline void DrawBulkBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;

	for (int i = 0; i < nr_bulk_atoms_toplot; ++i)
	{
		int Ordered_z = i;//D3DbulkAtom[i].Z_Depth;
		*ObjectScale = flag.RodThickness;
		if (*shadow)
		{
			*ObjectScale *= SHADOW_SIZE; //lets make the balls a bit bigger for nicer shadows
		}

		//Show selected atoms atoms
		if (D3DbulkAtom[Ordered_z].Selected > 0){
			g_color[0] = 1.f; g_color[1] = 1.f; g_color[2] = .1f; g_color[3] = 1.f;
		}
		else
		{
			CopyMemory(g_color, AtProp.AtomColor[D3DbulkAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);
			if (flag.g_transparency)
				g_color[3] = 1;//D3DsurfAtom[Ordered_z].occupancy;
		}
		g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

		//for bonds
		for (int j = 0; j < D3DbulkBonds[Ordered_z].NrBondedAtoms; ++j)
		{

			if (D3DbulkAtom[Ordered_z].AtomicNumber != 0 && (D3DbulkAtom[Ordered_z].display_mode == 2 || D3DbulkAtom[Ordered_z].display_mode == 3 || D3DbulkAtom[Ordered_z].display_mode == 4)){

				*ObjectScale_z = (float)(D3DbulkBonds[Ordered_z].Distance[j] * .0127);

				D3DXMatrixScaling(world, *ObjectScale_z, *ObjectScale, *ObjectScale);

				//First rotate the bond
				D3DXMatrixRotationY(&m_temp, D3DbulkBonds[Ordered_z].AngleZ[j]);//
				*world *= m_temp;
				D3DXMatrixRotationZ(&m_temp, D3DbulkBonds[Ordered_z].AngleXY[j]);//
				*world *= m_temp;
				//Now translate
				D3DXMatrixTranslation(&m_temp, (float)D3DbulkAtom[Ordered_z].x, (float)D3DbulkAtom[Ordered_z].y, (float)D3DbulkAtom[Ordered_z].z);
				*world *= m_temp;
				// Set the matrices for the shader.
				g_pEffect->SetMatrix("g_mWorld", world);

				// Calculate combined world-view-projection matrix.
				*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
				g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

				//-------------------------------------------------------------------------------------------------------------------------------
				int mesh;
				// Render.
				if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0)))
				{
					if (MyCamera.scene_rotate)
						mesh = 4;
					else
					{
						if (flag.g_spring)
							mesh = 5;
						else
							mesh = 2;
					}

					for (UINT pass = 0; pass < *totalPasses; ++pass){

						if (SUCCEEDED(g_pEffect->BeginPass(pass))){
							for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l){
								//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
								// Initialize required parameter
								V(g_pEffect->CommitChanges());
								// Render the mesh with the applied technique
								V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
							}
							g_pEffect->EndPass();
						}
					}
					g_pEffect->End();
				}
			}//for bonds
		}//for if mode
	}//for elements
}

inline void DrawBulkAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale)
{
	HRESULT hr;
	WCHAR str[10];
	RECT rcClient;
	float g_color[4];
	D3DXMATRIX m_temp;

	for (int i = 0; i < nr_bulk_atoms_toplot; ++i)
	{
		if (D3DbulkAtom[i].AtomicNumber != 0)
		{ //Don't draw not existing atoms simple delete trick

			//Show selected atoms atoms
			if (D3DbulkAtom[i].Selected > 0)
			{
				g_color[0] = 1.f; g_color[1] = 1.f; g_color[2] = .1f; g_color[3] = 1.f;
			}
			else
			{
				CopyMemory(g_color, AtProp.AtomColor[D3DbulkAtom[i].AtomicNumber], sizeof(float) * 4);
				if (flag.g_transparency)
					g_color[3] = 1;
			}

			g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

			//Elipsoids
			if (D3DbulkAtom[i].display_mode == 1 || D3DbulkAtom[i].display_mode == 4)
			{
				*ObjectScale = (float)(sqrt(D3DbulkAtom[i].DW_in_plane) * flag.DWscale);
			}

			//VDM
			if (D3DbulkAtom[i].display_mode == 0 || *ObjectScale == 0.)
			{
				*ObjectScale = (float)(AtProp.AtomRadius[D3DbulkAtom[i].AtomicNumber] * PLOT_SCALE_FACTOR);
			}

			//Sticks
			if (D3DbulkAtom[i].display_mode == 2)
			{
				*ObjectScale = 0.01f;
			}

			//Balls
			if (D3DbulkAtom[i].display_mode == 3)
			{
				*ObjectScale = flag.BallSize;
			}

			if (*shadow) *ObjectScale *= SHADOW_SIZE; //lets make the balls a bit bigger for nicer shadows
			D3DXMatrixScaling(world, *ObjectScale, *ObjectScale, *ObjectScale);

			D3DXMatrixTranslation(&m_temp, (float)D3DbulkAtom[i].x, (float)D3DbulkAtom[i].y, (float)D3DbulkAtom[i].z);
			*world *= m_temp;

			// Set the matrices for the shader.
			g_pEffect->SetMatrix("g_mWorld", world);
			// Calculate combined world-view-projection matrix.
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
			g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

			//-------------------------------------------------------------------------------------------------------------
			//Bulk atoms labels  && D3DbulkAtom[i].z>CAMERA_ZNEAR
			//-------------------------------------------------------------------------------------------------------------
			if (!*shadow && flag.plot_bulk_labels && (worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44 < MyCamera.Zfar && worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44>MyCamera.Znear || !MyCamera.mouse_Zcut))
			{
				swprintf_s(str, 60, L"%s%d", D3DbulkAtom[i].TSymbol, D3DbulkAtom[i].AtomInTheFile);
				rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
				rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));

				rcClient.left -= 10;
				rcClient.top -= 10;
				rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
				g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(GetRValue(flag.rgbText), GetGValue(flag.rgbText), GetBValue(flag.rgbText)));
			}
			//-------------------------------------------------------------------------------------------------------------------------------
			//Pickup atom position and select
			//-------------------------------------------------------------------------------------------------------------------------------
			if (!*shadow && MyCamera.mouse_select)
			{
				lasso.ResetSelection(MyCamera.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
				MouseSelect(&MyCamera.ptMouseSelect, myDX_Ui.g_params.BackBufferWidth, myDX_Ui.g_params.BackBufferHeight, worldViewProjectionMatrix, D3DbulkAtom, i, myDX_Ui.MenuHeight, flag.g_perspective);
			}

			//-------------------------------------------------------------------------------------------------------------------------------
			int mesh;
			// Render.
			if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0))){
				if (MyCamera.scene_rotate) mesh = 3;
				else mesh = 0;
				for (UINT pass = 0; pass < *totalPasses; ++pass){
					if (SUCCEEDED(g_pEffect->BeginPass(pass))){
						for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l){
							//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[0] ) ;
							// Initialize required parameter
							V(g_pEffect->CommitChanges());
							// Render the mesh with the applied technique
							V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
						}
						g_pEffect->EndPass();
					}
				}
				g_pEffect->End();
			}
		}//for elements
	}
}

inline void DrawSurfaceBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;
	int Ordered_z;

	for (int i = 0; i < nr_surf_atoms_toplot; ++i)
	{
		Ordered_z = D3DsurfAtom[i].Z_Depth;
		if (D3DsurfAtom[Ordered_z].AtomicNumber != 0 && (D3DsurfAtom[Ordered_z].display_mode == 2 || D3DsurfAtom[Ordered_z].display_mode == 3 || D3DsurfAtom[Ordered_z].display_mode == 4))
		{

			*ObjectScale = flag.RodThickness;
			if (*shadow)
			{
				//	*ObjectScale *= SHADOW_SIZE; //lets make the balls a bit bigger for nicer shadows
			}

			if (flag.g_compare)
			{
				g_color[0] = 0.9f; g_color[1] = 0.1f; g_color[2] = 0.1f; g_color[3] = 1.0f;
			}
			else
				CopyMemory(g_color, AtProp.AtomColor[D3DsurfAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);

			if (flag.g_transparency)
				g_color[3] = D3DsurfAtom[Ordered_z].occupancy;
			else
				g_color[3] = 1.f;

			g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);


			for (int j = 0; j < D3DsurfBonds[Ordered_z].NrBondedAtoms; ++j)
			{ //for bonds

				*ObjectScale_z = D3DsurfBonds[Ordered_z].Distance[j] * .0127f;

				D3DXMatrixScaling(world, *ObjectScale_z, *ObjectScale, *ObjectScale);

				//First rotate the bond
				D3DXMatrixRotationY(&m_temp, D3DsurfBonds[Ordered_z].AngleZ[j]);
				*world *= m_temp;
				D3DXMatrixRotationZ(&m_temp, D3DsurfBonds[Ordered_z].AngleXY[j]);
				*world *= m_temp;
				//Now translate
				D3DXMatrixTranslation(&m_temp, D3DsurfAtom[Ordered_z].x, D3DsurfAtom[Ordered_z].y, D3DsurfAtom[Ordered_z].z);
				*world *= m_temp;
				// Set the matrices for the shader.
				g_pEffect->SetMatrix("g_mWorld", world);

				// Calculate combined world-view-projection matrix.
				*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
				g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

				//Show selected atoms
				if (D3DsurfAtom[Ordered_z].Selected > 0)
				{
					float g_ColorSelected[4] = { 1., 1.0, 0.0, 1.0f };
					g_pEffect->SetValue("g_ObiectColor", g_ColorSelected, sizeof(float) * 4);
				}

				//-------------------------------------------------------------------------------------------------------------------------------
				int mesh;
				// Render.
				if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0)))
				{
					if (MyCamera.scene_rotate)
						mesh = 4;
					else
					{
						if (flag.g_spring)
							mesh = 5;
						else
							mesh = 2;
					}
					for (UINT pass = 0; pass < *totalPasses; ++pass)
					{
						if (SUCCEEDED(g_pEffect->BeginPass(pass)))
						{
							for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
							{
								//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
								// Initialize required parameter
								V(g_pEffect->CommitChanges());
								// Render the mesh with the applied technique
								V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
							}
							g_pEffect->EndPass();
						}
					}
					g_pEffect->End();
				}
			}//for bonds
		}//if mode
	}//for elements
}

inline void DrawSurfaceBondsCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;


	for (int i = 0; i < nr_surf_atoms_toplot; ++i)
	{
		int Ordered_z = D3DcompAtom[i].Z_Depth;
		if (D3DcompAtom[Ordered_z].AtomicNumber != 0 && (D3DcompAtom[Ordered_z].display_mode == 2 || D3DcompAtom[Ordered_z].display_mode == 3 || D3DcompAtom[Ordered_z].display_mode == 4))
		{
			for (int j = 0; j < D3DcompBonds[Ordered_z].NrBondedAtoms; j++){ //for bonds

				*ObjectScale = (float)(.05);
				*ObjectScale_z = (float)(D3DcompBonds[Ordered_z].Distance[j] * .0127);

				if (*shadow){
					*ObjectScale *= SHADOW_SIZE; //lets make the balls a bit bigger for nicer shadows
				}
				*ObjectScale *= flag.RodThickness;
				*ObjectScale_z *= 1.02f;
				D3DXMatrixScaling(world, *ObjectScale_z, *ObjectScale, *ObjectScale);


				//Show selected atoms atoms
				if (D3DsurfAtom[Ordered_z].Selected > 0)
				{
					g_color[0] = 1.0f; g_color[1] = 1.0f; g_color[2] = 0.1f; g_color[3] = 1.0f;
				}
				else
				{
					g_color[0] = 0.1f; g_color[1] = 0.1f; g_color[2] = 0.9f; g_color[3] = 1.0f;
					//CopyMemory(color, COLOR[D3DcompAtom[Ordered_z].AtomicNumber], sizeof(float)*4);
					if (flag.g_transparency)
						g_color[3] = 1;//D3DsurfAtom[Ordered_z].occupancy;
				}
				g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);
				//First rotate the bond
				D3DXMatrixRotationY(&m_temp, D3DcompBonds[Ordered_z].AngleZ[j]);
				*world *= m_temp;
				D3DXMatrixRotationZ(&m_temp, D3DcompBonds[Ordered_z].AngleXY[j]);
				*world *= m_temp;
				//Now translate
				D3DXMatrixTranslation(&m_temp, (float)D3DcompAtom[Ordered_z].x, (float)D3DcompAtom[Ordered_z].y, (float)D3DcompAtom[Ordered_z].z);
				*world *= m_temp;
				// Set the matrices for the shader.
				g_pEffect->SetMatrix("g_mWorld", world);

				// Calculate combined world-view-projection matrix.
				*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
				g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);



				//-------------------------------------------------------------------------------------------------------------------------------
				int mesh;
				// Render.
				if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0)))
				{
					if (MyCamera.scene_rotate) mesh = 4;
					else
					{
						if (flag.g_spring)
							mesh = 5;
						else
							mesh = 2;
					}
					for (UINT pass = 0; pass < *totalPasses; ++pass)
					{
						if (SUCCEEDED(g_pEffect->BeginPass(pass)))
						{
							for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
							{
								//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
								// Initialize required parameter
								V(g_pEffect->CommitChanges());
								// Render the mesh with the applied technique
								V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
							}
							g_pEffect->EndPass();
						}
					}
					g_pEffect->End();
				}
			}//for bonds
		}//if mode
	}//for elements
}

inline void DrawSurfaceAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	WCHAR str[10];
	RECT rcClient;
	float g_color[4];
	D3DXMATRIX m_temp;
	int Ordered_z;

	Order_Atoms3D(*RotateViewProjectionMatrix, D3DsurfAtom, nr_surf_atoms_toplot);
	for (int i = 0; i<nr_surf_atoms_toplot; ++i)
	{
		Ordered_z = D3DsurfAtom[i].Z_Depth;
		if (D3DsurfAtom[Ordered_z].AtomicNumber != 0)
		{
			if (D3DsurfAtom[Ordered_z].display_mode == 1 || D3DsurfAtom[Ordered_z].display_mode == 4)
			{
				*ObjectScale_z = *ObjectScale = (float)(AtProp.AtomRadius[D3DsurfAtom[Ordered_z].AtomicNumber] * PLOT_SCALE_FACTOR);
				if (D3DsurfAtom[Ordered_z].DW_in_plane>0.)
					*ObjectScale = (float)(sqrt(D3DsurfAtom[Ordered_z].DW_in_plane) * flag.DWscale);
				*ObjectScale_z = (float)(sqrt(D3DsurfAtom[Ordered_z].DW_out_of_plane) * flag.DWscale);
			}

			if (D3DsurfAtom[Ordered_z].display_mode == 0 || *ObjectScale == 0.)
			{
				*ObjectScale_z = *ObjectScale = (float)(AtProp.AtomRadius[D3DsurfAtom[Ordered_z].AtomicNumber] * PLOT_SCALE_FACTOR);
			}

			if (D3DsurfAtom[Ordered_z].display_mode == 2)
			{
				*ObjectScale = 0.001f;
				*ObjectScale_z = *ObjectScale;
			}

			if (D3DsurfAtom[Ordered_z].display_mode == 3)
			{
				*ObjectScale = flag.BallSize;
				*ObjectScale_z = flag.BallSize;
			}

			if (*ObjectScale_z == 0.)
			{
				*ObjectScale_z = *ObjectScale;
			}

			if (*shadow)
			{
				*ObjectScale *= SHADOW_SIZE; //lets make the balls a bit bigger for nicer shadows
				*ObjectScale_z *= SHADOW_SIZE;
			}
			D3DXMatrixScaling(world, *ObjectScale, *ObjectScale, *ObjectScale_z);


			//Show selected atoms atoms
			if (D3DsurfAtom[Ordered_z].Selected > 0)
			{
				g_color[0] = 1.0f; g_color[1] = 1.0f; g_color[2] = 0.1f; g_color[1] = 1.0f;
			}
			else
			{
				if (flag.g_compare)
				{
					g_color[0] = 0.9f; g_color[1] = 0.1f; g_color[2] = 0.1f; g_color[3] = 1.0f;
				}
				else
					CopyMemory(g_color, AtProp.AtomColor[D3DsurfAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);

				if (flag.g_transparency)
					g_color[3] = D3DsurfAtom[Ordered_z].occupancy;
				else
					g_color[3] = 1.f;
			}
			g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

			D3DXMatrixTranslation(&m_temp, (float)D3DsurfAtom[Ordered_z].x, (float)D3DsurfAtom[Ordered_z].y, (float)D3DsurfAtom[Ordered_z].z);
			*world *= m_temp;
			// Set the matrices for the shader.
			g_pEffect->SetMatrix("g_mWorld", world);

			// Calculate combined world-view-projection matrix.
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
			g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);


			//-------------------------------------------------------------------------------------------------------------
			//Surface labels
			//-------------------------------------------------------------------------------------------------------------
			if (flag.plot_surf_labels && !*shadow && (worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44 < MyCamera.Zfar && worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44>MyCamera.Znear || !MyCamera.mouse_Zcut))
			{
				swprintf_s(str, 20, L"%s%d", D3DsurfAtom[Ordered_z].TSymbol, D3DsurfAtom[Ordered_z].AtomInTheFile);
				rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
				rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));

				rcClient.left -= 10;
				rcClient.top -= 10;
				rcClient.right = rcClient.left + 40; rcClient.bottom = rcClient.top + 20;
				g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(GetRValue(flag.rgbText), GetGValue(flag.rgbText), GetBValue(flag.rgbText)));
			}
			//-------------------------------------------------------------------------------------------------------------------------------
			//Pickup atom position and select
			//-------------------------------------------------------------------------------------------------------------------------------
			if (MyCamera.mouse_select)
			{
				lasso.ResetSelection(MyCamera.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
				MouseSelect(&MyCamera.ptMouseSelect, myDX_Ui.g_params.BackBufferWidth, myDX_Ui.g_params.BackBufferHeight, worldViewProjectionMatrix, D3DsurfAtom, Ordered_z, myDX_Ui.MenuHeight, flag.g_perspective);
			}

			//-------------------------------------------------------------------------------------------------------------------------------
			int mesh;
			// Render.
			if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0)))
			{
				if (MyCamera.scene_rotate) mesh = 3;
				else mesh = 0;
				for (UINT pass = 0; pass < *totalPasses; ++pass)
				{
					if (SUCCEEDED(g_pEffect->BeginPass(pass)))
					{
						for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
						{
							//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
							// Initialize required parameter
							V(g_pEffect->CommitChanges());
							// Render the mesh with the applied technique
							V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
						}
						g_pEffect->EndPass();
					}
				}
				g_pEffect->End();
			}
		}//for elements
	}
}

inline void DrawSurfacFitCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z)
{
	//-------------------------------------------------------------------------------------------------------------
	//Surface atoms no fit
	//-------------------------------------------------------------------------------------------------------------

	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;

	Order_Atoms3D(*RotateViewProjectionMatrix, D3DcompAtom, nr_surf_atoms_toplot);
	for (int i = 0; i<nr_surf_atoms_toplot; ++i)
	{
		int Ordered_z = D3DcompAtom[i].Z_Depth;
		if (D3DcompAtom[Ordered_z].AtomicNumber != 0){
			if (D3DcompAtom[Ordered_z].display_mode == 1 || D3DcompAtom[Ordered_z].display_mode == 4){
				*ObjectScale_z = *ObjectScale = AtProp.AtomRadius[D3DcompAtom[Ordered_z].AtomicNumber] * PLOT_SCALE_FACTOR;
				if (D3DcompAtom[Ordered_z].DW_in_plane>0.)
					*ObjectScale = (float)(sqrt(D3DcompAtom[Ordered_z].DW_in_plane) * flag.DWscale);
				*ObjectScale_z = (float)(sqrt(D3DcompAtom[Ordered_z].DW_out_of_plane) * flag.DWscale);
			}
			if (D3DcompAtom[Ordered_z].display_mode == 0 || *ObjectScale == 0.)
			{
				*ObjectScale_z = *ObjectScale = (float)(AtProp.AtomRadius[D3DcompAtom[Ordered_z].AtomicNumber] * PLOT_SCALE_FACTOR);
			}

			if (D3DcompAtom[Ordered_z].display_mode == 2)
			{
				*ObjectScale = 0.001f;
				*ObjectScale_z = *ObjectScale;
			}
			if (D3DcompAtom[Ordered_z].display_mode == 3)
			{
				*ObjectScale = flag.BallSize;
				*ObjectScale_z = flag.BallSize;
			}

			if (*ObjectScale_z == 0.)
			{
				*ObjectScale_z = *ObjectScale;
			}

			if (*shadow)
			{
				*ObjectScale *= SHADOW_SIZE; //lets make the balls a bit bigger for nicer shadows
				*ObjectScale_z *= SHADOW_SIZE;
			}

			//To avoid interference with real model
			*ObjectScale *= 1.02f;
			*ObjectScale_z *= 1.02f;

			D3DXMatrixScaling(world, *ObjectScale, *ObjectScale, *ObjectScale_z);


			//Show selected atoms  
			if (D3DcompAtom[Ordered_z].Selected > 0)
			{
				g_color[0] = 1.0f; g_color[1] = 1.0f; g_color[2] = 0.1f; g_color[3] = 1.0f;
				g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);
			}
			else
			{
				if (flag.g_compare)
				{
					g_color[0] = 0.1f; g_color[1] = 0.1f; g_color[2] = 0.9f; g_color[3] = 1.f;
				}
				else
					CopyMemory(g_color, AtProp.AtomColor[D3DcompAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);
			}
			g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

			D3DXMatrixTranslation(&m_temp, (float)D3DcompAtom[Ordered_z].x, (float)D3DcompAtom[Ordered_z].y, (float)D3DcompAtom[Ordered_z].z);
			*world *= m_temp;
			// Set the matrices for the shader.
			g_pEffect->SetMatrix("g_mWorld", world);

			// Calculate combined world-view-projection matrix.
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
			g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);


			//-------------------------------------------------------------------------------------------------------------
			//Surface labels
			//-------------------------------------------------------------------------------------------------------------
			if (flag.plot_surf_labels && !*shadow && (worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44 < MyCamera.Zfar && worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44>MyCamera.Znear || !MyCamera.mouse_Zcut))
			{
				WCHAR str[200];
				RECT rcClient;
				swprintf_s(str, 20, L"%s%d", D3DcompAtom[Ordered_z].TSymbol, D3DcompAtom[Ordered_z].AtomInTheFile);
				rcClient.left = (int)(myDX_Ui.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
				rcClient.top = (int)(myDX_Ui.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));

				rcClient.left -= 10;
				rcClient.top -= 10;
				rcClient.right = rcClient.left + 40; rcClient.bottom = rcClient.top + 20;
				g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(GetRValue(flag.rgbText), GetGValue(flag.rgbText), GetBValue(flag.rgbText)));
			}
			//-------------------------------------------------------------------------------------------------------------------------------
			//Pickup atom position and select
			//-------------------------------------------------------------------------------------------------------------------------------
			if (MyCamera.mouse_select){
				lasso.ResetSelection(MyCamera.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DcompAtom, nr_surf_atoms_toplot);
				MouseSelect(&MyCamera.ptMouseSelect, myDX_Ui.g_params.BackBufferWidth, myDX_Ui.g_params.BackBufferHeight, worldViewProjectionMatrix, D3DcompAtom, Ordered_z, myDX_Ui.MenuHeight, flag.g_perspective);
			}


			//-------------------------------------------------------------------------------------------------------------------------------
			int mesh;
			// Render.
			if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0)))
			{
				if (MyCamera.scene_rotate) mesh = 3;
				else mesh = 0;
				for (UINT pass = 0; pass < *totalPasses; ++pass)
				{
					if (SUCCEEDED(g_pEffect->BeginPass(pass)))
					{
						for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
						{
							//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
							// Initialize required parameter
							V(g_pEffect->CommitChanges());
							// Render the mesh with the applied technique
							V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
						}
						g_pEffect->EndPass();
					}
				}
				g_pEffect->End();
			}
		}//for elements
	}
}

inline void DrawThermalElipsoids(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	D3DXMATRIX m_temp;

	float g_elipsoids[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	g_pEffect->SetValue("g_ObiectColor", g_elipsoids, sizeof(float) * 4);
	g_pEffect->SetMatrix("g_mRotate", &MyCamera.rotate);
	if (!*shadow)
		g_pEffect->SetTechnique("NoBlend");

	for (int j = 0; j < 3; ++j){ //3 elipsoids
		for (int i = 0; i < nr_surf_atoms_toplot; ++i)
		{
			if (D3DsurfAtom[i].AtomicNumber != 0)
			{
				if (D3DsurfAtom[i].display_mode == 1 || D3DsurfAtom[i].display_mode == 4)
				{
					*ObjectScale_z = *ObjectScale = AtProp.AtomRadius[D3DsurfAtom[i].AtomicNumber] * PLOT_SCALE_FACTOR;
					if (D3DsurfAtom[i].DW_in_plane > 0.)
						*ObjectScale = (float)(sqrt(D3DsurfAtom[i].DW_in_plane) * flag.DWscale);
					*ObjectScale_z = (float)(sqrt(D3DsurfAtom[i].DW_out_of_plane) * flag.DWscale);

					if (D3DsurfAtom[i].DW_out_of_plane == 0)
						*ObjectScale_z = *ObjectScale;

					if (*ObjectScale > 0.f){
						float reduce_x = 1.05f, reduce_y = 1.05f, reduce_z = 1.05f;

						if (j == 0) reduce_x = 0.1f;
						if (j == 1) reduce_y = 0.1f;
						if (j == 2) reduce_z = 0.1f;
						D3DXMatrixScaling(world, *ObjectScale*reduce_x, *ObjectScale*reduce_y, *ObjectScale_z*reduce_z);

						D3DXMatrixTranslation(&m_temp, (float)D3DsurfAtom[i].x, (float)D3DsurfAtom[i].y, (float)D3DsurfAtom[i].z);
						*world *= m_temp;
						// Set the matrices for the shader.
						g_pEffect->SetMatrix("g_mWorld", world);
						// Calculate combined world-view-projection matrix.
						*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
						g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);


						// Render.//for shadows is only one pass
						if (SUCCEEDED(g_pEffect->Begin(totalPasses, 0)))
						{
							for (UINT pass = 0; pass < *totalPasses; ++pass)
							{
								if (SUCCEEDED(g_pEffect->BeginPass(pass)))
								{
									for (DWORD l = 0; l < g_Orbiter[0].m_dwNumMaterials; l++)
									{
										//g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
										// Initialize required parameter
										V(g_pEffect->CommitChanges());
										// Render the mesh with the applied technique
										V(g_Orbiter[0].m_Mesh->DrawSubset(l));
									}
									g_pEffect->EndPass();
								}
							}
							g_pEffect->End();
						}
					}//for mode
				}//for i
			}//0 atomic number
		}//for j
	}//if
}

inline void DrawLasso(D3DXMATRIX *RotateViewProjectionMatrix)
{

	static D3DXVECTOR2 VertexList[MAX_LASSO_POINTS];
	static D3DXVECTOR2 VertexList2[2];
	static int nr = 0;
	static int nr2 = 0;
	//Delete all selections
	lasso.ResetSelection(MyCamera.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);

	if (MyCamera.ptMouseSelect.x == -1)
	{
		nr = 0;
		nr2 = 0;
	}

	if (MyCamera.ptMouseSelect.x > 0)
	{
		if (nr == 0)
		{
			VertexList[0].x = (float)MyCamera.ptMouseSelect.x;
			VertexList[0].y = (float)MyCamera.ptMouseSelect.y - myDX_Ui.MenuHeight;

			VertexList2[0].x = VertexList[0].x;
			VertexList2[0].y = VertexList[0].y;
			nr++;
		}


		if (nr > 0)//&& (nr < 2 || (MyCamera.ptMouseSelect.x != VertexList[nr - 1].x && MyCamera.ptMouseSelect.y != VertexList[nr - 1].y))
		{
			if (nr >= MAX_LASSO_POINTS) nr = 0;
			VertexList[nr].x = (float)MyCamera.ptMouseSelect.x;
			VertexList[nr].y = (float)MyCamera.ptMouseSelect.y - myDX_Ui.MenuHeight;

			nr2 = 2;
			VertexList2[1].x = VertexList[nr].x;
			VertexList2[1].y = VertexList[nr].y;
			nr++;
		}
	}

	//Select atoms from lasso points
	if (MyCamera.LeftButtonUp)
	{
		lasso.MouseLasso(VertexList, nr, myDX_Ui.g_params.BackBufferWidth, myDX_Ui.g_params.BackBufferHeight, RotateViewProjectionMatrix, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot, myDX_Ui.MenuHeight);
		nr = 0;
		nr2 = 0;
		MyCamera.ptMouseSelect.x = -1;
	}

	// Draw the line.
	_line->SetWidth(2.0f);
	_line->Begin();
	_line->Draw(VertexList, nr, D3DXCOLOR(0.9f, 0.9f, 0.1f, 1.0f));
	_line->End();

	//Draw closing line
	_line->SetWidth(2.0f);
	_line->Begin();
	_line->Draw(VertexList2, nr2, D3DXCOLOR(0.9f, 0.9f, 0.1f, 1.0f));
	_line->End();


}

//-----------------------------------------------------------------------------
// Functions: Render text in 3D window
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void RenderText(HWND hWnd)
{
	static RECT rcClient;
	WCHAR str[1000];
	std::ostringstream output;

	if (myDX_Ui.g_displayHelp)
	{
		wsprintf(str, L"Left mouse click and drag to track camera \n\rMiddle mouse click and drag to dolly camera \n\rRight mouse click and drag to orbit camera \n\rMouse wheel to dolly camera\n\rAlt+Enter full screen\n\rFPS: %d \n\rMultisample anti-aliasing: %d \n\rAnisotropic filtering: %d\n\rResolution %dx%d\n\rAuto depth stencil format %d", myDX_Ui.g_framesPerSecond, myDX_Ui.g_msaaSamples, myDX_Ui.g_maxAnisotrophy, myDX_Ui.g_params.BackBufferWidth, myDX_Ui.g_params.BackBufferHeight, myDX_Ui.g_params.AutoDepthStencilFormat);
	}
	else
	{
		wsprintf(str, L" \n\rPress H to display help");
	}

	GetClientRect(hWnd, &rcClient);
	rcClient.left += 4;
	rcClient.top -= 17;

	g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 255, 0));
}