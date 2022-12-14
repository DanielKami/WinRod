//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header with definitions for 3D camera movements, macros for 3D objectsand atom informations like atomic CPK and color 
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include <d3dx9.h>



//Camera parameters for 3D
#define ROTATION_STEP 0.004
#define ROTATION_STEP_Z 0.008

#define CAMERA_ZNEAR_PERSPECTIVE 1
#define CAMERA_ZFAR_PERSPECTIVE 4000
#define CAMERA_ZNEAR_NO_PERSPECTIVE 1000
#define CAMERA_ZFAR_NO_PERSPECTIVE 45000




#define CAMERA_FOVY_PERSPECTIVE D3DXToRadian(55.0f)
#define CAMERA_FOVY_NO_PERSPECTIVE D3DXToRadian(0.5f)

//todo
const float DOLLY_MIN = -2.5;
const float DOLLY_MAX = -1000.0;

#define MOUSE_DOLLY_SPEED  0.2f
#define MOUSE_TRACK_SPEED   0.2f
#define MOUSE_WHEEL_DOLLY_SPEED_3D   0.0007f

//--------------------------------------------------------------------------------------------------------------------------------
// Macros for 3D.
//--------------------------------------------------------------------------------------------------------------------------------

#define SAFE_RELEASE(x) if ((x) != 0) { (x)->Release(); (x) = 0; }

#ifndef V
#define V(x)           { hr = (x); }
#endif

#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif


//--------------------------------------------------------------------------------------------------------------------------------
// Encapsulate an object in the scene with its world transformation
// matrix.
// created by: Daniel Kaminski
// 01/12/2012 Lublin
//--------------------------------------------------------------------------------------------------------------------------------
struct COrbiter
{

private:

public:

	D3DXMATRIXA16 m_mWorld;		// World transformation
	ID3DXMesh * m_Mesh;

	IDirect3DTexture9**         g_pMeshTexture;
	IDirect3DTexture9**         g_pnullTexture;

	D3DMATERIAL9*				m_pMeshMaterials;
	IDirect3DTexture9*          g_pMeshBump;		//bump map texture for material not used in WinRod but possible
	D3DXMATRIXA16				mCenter;
	DWORD						m_dwNumMaterials;


	void OnDestroyDevice()
	{
		SAFE_RELEASE(m_Mesh);
		SAFE_RELEASE(g_pMeshBump);
		if (!m_pMeshMaterials) delete[] m_pMeshMaterials;

		for (unsigned int j = 0; j < m_dwNumMaterials; j++){
			SAFE_RELEASE(g_pMeshTexture[j]);
		}
	}

	//Make a local copy of all elements and delete them
	void OnLostDevice(){
		SAFE_RELEASE(m_Mesh);
		SAFE_RELEASE(g_pMeshBump);
		if (!m_pMeshMaterials) delete[] m_pMeshMaterials;

		for (unsigned int j = 0; j < m_dwNumMaterials; j++){
			SAFE_RELEASE(g_pMeshTexture[j]);
		}
	}
};

//--------------------------------------------------------------------------------------------------------------------------------
// Atomic parameters for 3D drawing
//--------------------------------------------------------------------------------------------------------------------------------
static int CONSTANT_PROPERITIES[98][6]=
{/*
bond   color    VDM    d  Hyb1  Hyb2         */
0,		 0,	    00/2,  0, 0, 0,//nothing
75,	 1,    80/2, 13, 1, 0,//H

140,	 2,    220/2, 25, 0, 0,//He
130,	 8,    122/2, 25, 1, 0,//Li
90,		 5,     62/2, 25, 2, 0,//Be
190,	 5,    175/2, 25, 3, 0,//B
100,	 6,    155/2, 25, 2, 4,//C
110,	 7,    140/2, 25,-3, 5,//N
110,	 8,    135/2, 25,-1,-2,//O
120,	10,    130/2, 25,-1, 0,//F

170,	 2,    202/2, 25, 0, 0,//Ne
170,	 3,    202/2, 25, 1, 0,//Na
130,	 6,    150/2, 25, 2, 0,//Mg
130,	 6,    150/2, 25, 3, 0,//Al
190,	10,    200/2, 25, 4, 0,//Si
120,	 4,    188/2, 29,-3, 5,//P
125,	 3,    181/2, 29,-2, 6,//S
180, 	10,    175/2, 25,-1, 7,//Cl

150,	 2,    277/2, 25, 0, 0,//Ar
290,	 4,    239/2, 25, 1, 0,//K
220,	 2,    195/2, 25 ,2, 0,//Ca
190,	 6,    134/2, 25, 3, 4,//Sc
190,	 6,    195/2, 25, 2, 3,//Ti
190,	 6,    106/2, 25, 3, 4,//V
190,	 6,    103/2, 25, 2, 3,//Cr
190,	 6,    119/2, 25, 2, 3,//Mn
210,	 9,    126/2, 25, 2, 3,//Fe
210,	 8,    113/2, 25, 2, 3,//Co
210,	 9,    124/2, 25, 2, 3,//Ni
180,	 4,    124/2, 25, 1, 2,//Cu
190,	 4,    115/2, 25, 2, 0,//Zn
190,	 4,    155/2, 25, 3, 0,//Ga
190,	 4,    148/2, 25, 2, 4,//Ge
190,	 4,     83/2, 25,-3, 3,//As
190,	 4,     90/2, 25,-2, 4,//Se
190,	 4,    195/2, 25,-1, 1,//Br

180,	 4,    190/2, 30, 0, 0,//Kr
180,	 1,    265/2, 30, 1, 0,//Rb
220,	 9,    202/2, 30, 2, 0,//Sr
220,	 6,    161/2, 30, 3, 0,//Y
220,	 6,    142/2, 30, 4 ,0,//Zr
220,	 4,    133/2, 30, 3, 5,//Nb
220,	 2,    175/2, 30, 2, 3,//Mo
220,	 6,    180/2, 30, 2, 4,//Tc
220,	 6,    120/2, 30, 2, 3,//Ru
220,	 1,    122/2, 30, 2, 3,//Rh
220,	 1,    144/2, 30, 2, 4,//Pd
160,	 1,    155/2, 30, 1, 2,//Ag
220,	 9,    175/2, 30, 2, 0,//Cd
220,	 6,    146/2, 30, 1, 3,//In
220,	 9,    167/2, 30, 2, 4,//Sn
150,	 6,    112/2, 30,-3, 3,//Sb
220,	 6,    126/2, 30,-2, 4,//Te
220,	 4,    215/2, 33,-1, 1,//I

240,	 6,    210/2, 30, 0, 0,//Xe
240,	 1,    310/2, 30, 1, 0,//Cs
240,	 4,    241/2, 30, 2, 0,//Ba
240,	 6,    183/2, 30, 3, 0,//La
240,	 5,    186/2, 30, 3, 0,//Ce
240,	 5,    162/2, 30, 3, 4,//Pr
240,	 5,    179/2, 33, 3, 0,//Nd
240,	 5,    176/2, 33, 3, 0,//Pm
240,	 5,    174/2, 33, 2, 3,//Sm
240,	 5,    196/2, 33, 2, 3,//Eu
240,	 5,    169/2, 33, 3, 9,//Gd
240,	 5,    166/2, 33, 2, 3,//Tb
240,	 5,    163/2, 33, 3, 0,//Dy
240,	 5,    161/2, 33, 3, 0,//Ho
240,	 5,    159/2, 33, 3, 0,//Er
240,	 5,    157/2, 33, 3, 0,//Tm
240,	 5,    154/2, 33, 2, 3,//Yb 
240,	 5,    153/2, 33, 3, 0,//Lu

240,	 6,    140/2, 33, 4, 0,//Hf
240,	 4,    122/2, 33, 3, 4,//Ta
240,	 2,    126/2, 33, 2, 3,//W
240,	 6,    130/2, 33, 2, 3,//Re
240,	 1,    156/2, 33, 3, 4,//Os
240,	 1,    122/2, 33, 2, 3,//Ir
300,	 1,    155/2, 38, 2, 4,//Pt
310,	 3,    145/2, 38, 1, 3,//Au
240,	10,    198/2, 33, 1, 2,//Hg
240,	 6,    171/2, 33, 1, 3,//Tl
240,	10,    216/2, 33, 2, 4,//Pb
240,	 9,    173/2, 33, 3, 5,//Bi
240,	 4,    221/2, 33, 2, 4,//Po 
240,	 6,    112/2, 33,-1, 1,//At
150,	 6,    230/2, 33, 0, 0,//Rn

150,	 1,    324/2, 25, 1, 0,//Fr
150,	 6,    257/2, 25, 2, 0,//Ra
150,	 6,    212/2, 25, 3, 0,//Ac

190,	 7,    184/2, 25, 4, 0,//Th
190,	 7,    160/2, 25, 4, 5,//Pa
190,	 3,    175/2, 25, 3, 4,//U
190,	 7,    171/2, 25, 3, 4,//Np
190,	 7,    167/2, 25, 3, 4,//Pu
190,	 7,    166/2, 25, 3, 4//Am
};

//--------------------------------------------------------------------------------------------------------------------------------
//Atom colors and transparecny 
//--------------------------------------------------------------------------------------------------------------------------------
const float CONSTANT_COLOR[98][4]=
{
 0.0f, 0.0f, 0.0f, 0.0f,//nothing
 0.9f, 0.9f, 0.9f, 0.9f,//H

 0.9f, 0.9f, 0.9f, 0.9f,//He
 0.9f, 0.6f, 0.4f, 0.9f,//Li
 0.5f, 0.9f, 0.9f, 0.9f,//Be
 0.8f, 0.9f, 0.8f, 0.9f,//B
 0.1f, 0.1f, 0.1f, 0.4f,//C
 0.0f, 0.0f, 0.8f, 0.4f,//N
 0.7f, 0.0f, 0.0f, 0.4f,//O
 0.1f, 0.9f, 0.9f, 0.9f,//F

 0.9f, 0.9f, 0.9f, 0.9f,//Ne
 0.9f, 0.9f, 0.1f, 0.9f,//Na
 0.6f, 0.6f, 0.3f, 0.9f,//Mg
 0.7f, 0.7f, 0.7f, 0.9f,//Al
 0.3f, 0.3f, 0.3f, 0.9f,//Si
 0.9f, 0.8f, 0.4f, 0.2f,//P
 1.0f, 1.0f, 0.0f, 0.9f,//S
 0.9f, 0.9f, 0.9f, 0.9f,//Cl

 0.9f, 0.9f, 0.9f, 0.9f,//Ar
 0.1f, 0.1f, 0.9f, 0.9f,//K
 0.9f, 0.3f, 0.3f, 0.9f,//Ca
 0.9f, 0.7f, 0.9f, 0.9f,//Sc
 0.3f, 0.6f, 0.3f, 0.9f,//Ti
 0.9f, 0.6f, 0.8f, 0.9f,//V
 0.3f, 0.4f, 0.9f, 0.9f,//Cr
 0.5f, 0.0f, 0.5f, 0.9f,//Mn
 0.6f, 0.6f, 0.6f, 0.9f,//Fe
 0.6f, 0.2f, 0.2f, 0.9f,//Co
 0.9f, 0.3f, 0.9f, 0.9f,//Ni
 0.9f, 0.5f, 0.1f, 0.9f,//Cu
 0.4f, 0.4f, 0.4f, 0.9f,//Zn
 0.4f, 0.6f, 0.4f, 0.9f,//Ga
 0.4f, 0.7f, 0.3f, 0.9f,//Ge
 0.3f, 0.5f, 0.3f, 0.9f,//As
 0.6f, 0.6f, 0.2f, 0.9f,//Se
 0.8f, 0.2f, 0.0f, 0.9f,//Br

 0.9f, 0.9f, 0.9f, 0.9f,//Kr
 0.9f, 0.9f, 0.9f, 0.9f,//Rb
 0.7f, 0.7f, 0.9f, 0.9f,//Sr
 0.9f, 0.9f, 0.9f, 0.9f,//Y
 0.5f, 0.5f, 0.5f, 0.9f,//Zr
 0.5f, 0.6f, 0.5f, 0.9f,//Nb
 0.8f, 0.8f, 0.4f, 0.9f,//Mo
 0.9f, 0.9f, 0.9f, 0.9f,//Tc
 0.9f, 0.9f, 0.6f, 0.9f,//Ru
 0.8f, 0.8f, 0.8f, 0.9f,//Rh
 0.8f, 0.8f, 0.8f, 0.9f,//Pd
 0.9f, 0.9f, 0.9f, 0.9f,//Ag
 0.9f, 0.9f, 0.9f, 0.9f,//Cd
 0.8f, 0.9f, 0.8f, 0.9f,//In
 0.5f, 0.5f, 0.6f, 0.9f,//Sn
 0.7f, 0.5f, 0.3f, 0.5f,//Sb
 0.8f, 0.9f, 0.9f, 0.9f,//Te
 0.5f, 0.1f, 0.3f, 0.9f,//I

 0.9f, 0.9f, 0.9f, 0.9f,//Xe
 0.9f, 0.7f, 0.7f, 0.9f,//Cs
 0.9f, 0.2f, 0.2f, 0.9f,//Ba
 0.9f, 0.9f, 0.9f, 0.9f,//La
 0.9f, 0.9f, 0.9f, 0.9f,//Ce
 0.9f, 0.9f, 0.9f, 0.9f,//Pr
 0.9f, 0.9f, 0.9f, 0.9f,//Nd
 0.9f, 0.9f, 0.9f, 0.9f,//Pm
 0.9f, 0.9f, 0.9f, 0.9f,//Sm
 0.7f, 0.7f, 0.7f, 0.9f,//Eu
 0.7f, 0.7f, 0.7f, 0.9f,//Gd
 0.7f, 0.7f, 0.7f, 0.9f,//Tb
 0.7f, 0.7f, 0.7f, 0.9f,//Dy
 0.7f, 0.7f, 0.7f, 0.9f,//Ho
 0.7f, 0.7f, 0.7f, 0.9f,//Er
 0.7f, 0.7f, 0.7f, 0.9f,//Tm
 0.7f, 0.7f, 0.7f, 0.9f,//Yb 
 0.7f, 0.7f, 0.7f, 0.9f,//Lu

 0.7f, 0.7f, 0.7f, 0.9f,//Hf
 0.7f, 0.7f, 0.7f, 0.9f,//Ta
 0.7f, 0.7f, 0.7f, 0.9f,//W
 0.7f, 0.7f, 0.7f, 0.9f,//Re
 0.7f, 0.7f, 0.7f, 0.9f,//Os
 0.7f, 0.7f, 0.7f, 0.9f,//Ir
 0.8f, 0.8f, 0.8f, 0.9f,//Pt
 0.8f, 0.8f, 0.1f, 0.9f,//Au
 0.7f, 0.7f, 0.7f, 0.9f,//Hg
 0.8f, 0.8f, 0.8f, 0.9f,//Tl
 0.5f, 0.5f, 0.6f, 0.9f,//Pb
 0.6f, 0.6f, 0.5f, 0.9f,//Bi
 0.6f, 0.6f, 0.6f, 0.9f,//Po 
 0.6f, 0.6f, 0.6f, 0.9f,//At
 0.6f, 0.6f, 0.6f, 0.9f,//Rn

 0.6f, 0.6f, 0.6f, 0.9f,//Fr
 0.6f, 0.6f, 0.6f, 0.9f,//Ra
 0.6f, 0.6f, 0.6f, 0.9f,//Ac

 0.6f, 0.6f, 0.6f, 0.9f,//Th
 0.8f, 0.8f, 0.8f, 0.9f,//Pa
 0.6f, 0.6f, 0.6f, 0.9f,//U
 0.6f, 0.6f, 0.6f, 0.9f,//Np
 0.6f, 0.6f, 0.6f, 0.9f,//Pu
 0.6f, 0.6f, 0.6f, 0.9f //Am
};
