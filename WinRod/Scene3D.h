#pragma once
#define STICK_CORRECTION .0127f


#include <Windowsx.h>
#include <sstream>
#include "structures.h"
#include "3Ddef.h"		
#include "thread.h"
#include "Camera.h"
#include "Measure3D.h"
#include "lasso.h"
#include "DX_Ui.h"			


LRESULT CALLBACK WindowScene3DProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//3D functions used by DirectX
//--------------------------------------------------------------------------------------------------------------------------------
HRESULT OnLostDevice();
void    OnDestroyDevice();
void    OnInitDevice();
bool    OnResetDevice();
void	OnFrameRender(HWND hWnd, Thread_t* var_t);
void	OnThreadExit(HWND hWnd);



class Scene3D :	public Camera,
				public Measure, 
				public Lasso, 
				public DX_Ui
{

public:

	Scene3D()
	{
		// Initialize the critical section one time only.
		if (!InitializeCriticalSectionAndSpinCount(&CriticalSection, 0x00000400))
			return;

		//Initialisation here for backward compatybility
		D3DbulkAtom = NULL;
		D3DsurfAtom = NULL;
		D3DcompAtom = NULL;
		D3DLiquid = NULL;
		D3DbulkBonds = NULL;
		D3DsurfBonds = NULL;
		D3DcompBonds = NULL;

		g_pFont = NULL;
		g_pEffect = NULL;
		g_pNullTexture = NULL;
		_line = NULL;

		display_modeBulk = NULL;
		display_modeSur = NULL;
		display_modeComp = NULL;

		shift_x = 0.f;
		shift_y = 0.f;
	}
	~Scene3D()
	{
		// Release resources used by the critical section object.
		DeleteCriticalSection(&CriticalSection);
		DeleteObiects();

	}

	CRITICAL_SECTION CriticalSection;


	void	Scene();
	void	Update3DModel(bool reset = true);
	void	Update3Dcompare(void);
	void	TuneCamera();

	void	DeleteObiects();
	void	CopyAtomsStyle();
	void	RestoreAtomsStyle();
	void	ClearModel();
	void	ApplyColors();
	void    Rbn_autosize();
	void	LookX();
	void	LookY();
	void	LookZ();
	void    Measure(int number);


	FLAG				flag;							//All necessary flags to comunicate between callback windows and WinRod 

	AtomParam	*D3DbulkAtom;
	AtomParam	*D3DsurfAtom;
	AtomParam	*D3DcompAtom;			//for comparison after fitting
	AtomParam	*D3DLiquid;
	BondParam	*D3DbulkBonds;
	BondParam	*D3DsurfBonds;
	BondParam	*D3DcompBonds;


	int		nr_bulk_atoms_toplot;				//3D
	int		nr_surf_atoms_toplot;				//3D
	int		nr_liquid_layers_toplot;			//3D
	ATOM_PROPERITIES AtProp;


	//Read section
	ModelStruct Bul;
	ModelStruct Sur;
	ModelStruct Fit;

	//Register 3D objects
	ID3DXFont                   *g_pFont;
	ID3DXEffect                 *g_pEffect;
	IDirect3DTexture9           *g_pNullTexture;
	LPD3DXLINE					_line;						//For measurements

#define						OBJECT_NUMBER 6				//defines number of objects
	COrbiter					g_Orbiter[OBJECT_NUMBER];	// Orbiter meshes


	int g_SRepetitionX, g_SRepetitionY;
	int g_BRepetitionX, g_BRepetitionY, g_BRepetitionZ;


	float dlat[6];
	//float Dlat1cosDlat5;
	//float Dlat2cosDlat3;
	//float Dlat1sinDlat5;
	//float Dlat2cosDlat4;

	float	 a_sin_alp;
	float	 b_cos_gam;
	float	 c_cos_bet__sin_alp;
	float	 b_sin_Gam;
	float	 c_cos_alp;
	float	 c_sin_bet__sin_alp;



private:
	float g_LightColor[4];

	//Position center
	float	shift_x, shift_y;

	//Style copy
	int		*display_modeBulk;
	int		*display_modeSur;
	int		*display_modeComp;

	//3d plot functions
	//--------------------------------------------------------------------------------------------------------------------------------

	bool	CenterAtoms3D(AtomParam*, int, AtomParam*, AtomParam*, int);
	void	Order_Atoms3D(D3DXMATRIX, AtomParam*, int);
	void	ResetBonds(BondParam *D3DAtoms, int nr);
	void	ResetAtomParam(AtomParam *D3DAtoms, int nr);
	int		Calculate_Atoms3D(AtomParam*, ModelStruct, FLAG, int, int, int);
	void    Calculate_Bonds3D(BondParam*, AtomParam*, int, AtomParam*, int);
	void	Update3Datoms(void);


	//Plot scene functions
	D3DXVECTOR3 CalculatePoint3D(float x, float y, float z);
	void	DrawCell(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix);
	void	DrawMeasurement(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix);
	void	DrawPlane(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses);
	void	DrawBulkBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z);
	void	DrawBulkAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale);
	void	DrawSurfaceBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z);
	void	DrawSurfaceBondsCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z);
	void	DrawSurfaceAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z);
	void	DrawSurfaceAtomsCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z);
	void	DrawThermalElipsoids(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z);
	void    DrawLiquidLayers(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z);

	void	DrawLasso(D3DXMATRIX *RotateViewProjectionMatrix);
	void	RenderText();
	void    DrawCharges(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses);

};