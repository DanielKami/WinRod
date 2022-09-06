
#include "structures.h"

//Plot scene functions
void	Scene(bool);
void	DrawCell(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix);
void	DrawMeasurement(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix);
void	DrawPlane(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses);
void	DrawBulkBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z);
void	DrawBulkAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale);
void	DrawSurfaceBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z);
void	DrawSurfaceBondsCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z);
void	DrawSurfaceAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z);
void	DrawSurfacFitCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z);
void	DrawThermalElipsoids(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, bool *shadow, float *ObjectScale, float *ObjectScale_z);
void	DrawLasso(D3DXMATRIX *RotateViewProjectionMatrix);
void	RenderText(HWND hWnd);