#pragma once

#define MAX_SAMPLES 16

class GaussBlur
{
public:

	//ID3DXEffect            *g_pEffect;

	GaussBlur()
	{
	}
	virtual ~GaussBlur()
	{
	}

	void Set_g_pEffect(ID3DXEffect *effect){
	g_pEffect = effect;
	}

	// Texture coordinate rectangle
	struct CoordRect
	{
		float fLeftU, fTopV;
		float fRightU, fBottomV;
	};

	// Screen quad vertex format
	struct ScreenVertex
	{
		D3DXVECTOR4 p; // position
		D3DXVECTOR2 t; // texture coordinate

		DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
	};
	ScreenVertex ScrVer;

	LPDIRECT3DTEXTURE9           g_pShadowMap = NULL;    // Texture to which the shadow map is rendered
	LPDIRECT3DTEXTURE9           g_pShadowMapGauss = NULL;    // Texture to which the shadow map is rendered and Gaussed
	

	HRESULT GaussBlurMap(IDirect3DDevice9* g_pd3dDevice, LPDIRECT3DTEXTURE9 InMap, LPDIRECT3DTEXTURE9 OutMap);

private:
	ID3DXEffect *g_pEffect;

	HRESULT GetSampleOffsets_GaussBlur5x5(DWORD dwD3DTexWidth, DWORD dwD3DTexHeight, D3DXVECTOR2* avTexCoordOffset, D3DXVECTOR4* avSampleWeights, FLOAT fMultiplier = 1.0f);
	HRESULT GetTextureCoords(PDIRECT3DTEXTURE9 pTexSrc, RECT* pRectSrc, PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, CoordRect* pCoords);
	HRESULT GetTextureRect(PDIRECT3DTEXTURE9 pTexture, RECT* pRect);
	VOID DrawFullScreenQuad(IDirect3DDevice9* g_pd3dDevice, float fLeftU, float fTopV, float fRightU, float fBottomV);
	VOID DrawFullScreenQuad(IDirect3DDevice9* pd3dDevice, CoordRect c)
	{
		DrawFullScreenQuad(pd3dDevice, c.fLeftU, c.fTopV, c.fRightU, c.fBottomV);
	}
	float GaussianDistribution(float x, float y, float _rho);
};