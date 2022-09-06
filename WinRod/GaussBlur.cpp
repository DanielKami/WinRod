//--------------------------------------------------------------------------------------------------------------------------------
//This file contains special functions necesarry for bluring the scene. It is of minor importance for the WinRod program. 
//It is necessary for shadows in 3D scene
//--------------------------------------------------------------------------------------------------------------------------------

#include <stdafx.h>				//Header necessary for Visual Studio
#include "WinRod.h"				//The main header of WinRod
#include "3Ddef.h"				//Definition for 3D graphisc
#include <math.h>
#include "definitions.h"
#include "GaussBlur.h"


//extern ID3DXEffect                 *g_pEffect;

//-----------------------------------------------------------------------------
// Name: GaussianDistribution
// Desc: Helper function for GetSampleOffsets function to compute the 
//       2 parameter Gaussian distrubution using the given standard deviation
//       rho
//-----------------------------------------------------------------------------
float GaussBlur::GaussianDistribution(float x, float y, float _rho)
{
	float g = 1.0f / sqrtf(2.0f * D3DX_PI * _rho * _rho);
	g *= expf(-(x * x + y * y) / (2.f * _rho * _rho));

	return (g);
}

//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_GaussBlur5x5
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.
//-----------------------------------------------------------------------------
HRESULT GaussBlur::GetSampleOffsets_GaussBlur5x5(DWORD dwD3DTexWidth, DWORD dwD3DTexHeight,	D3DXVECTOR2* avTexCoordOffset,	D3DXVECTOR4* avSampleWeight, FLOAT fMultiplier)
{
	float tu = 1.0f / (float)dwD3DTexWidth;
	float tv = 1.0f / (float)dwD3DTexHeight;

	D3DXVECTOR4 vWhite(1.0f, 1.0f, 1.0f, 1.0f);

	float totalWeight = 0.0f;
	int index = 0;
	for (int x = -2; x <= 2; x++)
	{
		for (int y = -2; y <= 2; y++)
		{
			// Exclude pixels with a block distance greater than 2. This will
			// create a kernel which approximates a 5x5 kernel using only 13
			// sample points instead of 25; this is necessary since 2.0 shaders
			// only support 16 texture grabs.
			if (abs(x) + abs(y) > 2)
				continue;

			// Get the unscaled Gaussian intensity for this offset
			avTexCoordOffset[index] = D3DXVECTOR2(x * tu, y * tv);
			avSampleWeight[index] = vWhite * GaussianDistribution((float)x, (float)y, 1.0f);
			totalWeight += avSampleWeight[index].x;

			index++;
		}
	}

	// Divide the current weight by the total weight of all the samples; Gaussian
	// blur kernels add to 1.0f to ensure that the intensity of the image isn't
	// changed when the blur occurs. An optional multiplier variable is used to
	// add or remove image intensity during the blur.
	for (int i = 0; i < index; i++)
	{
		avSampleWeight[i] /= totalWeight;
		avSampleWeight[i] *= fMultiplier;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void GaussBlur::DrawFullScreenQuad(IDirect3DDevice9* g_pd3dDevice, float fLeftU, float fTopV, float fRightU, float fBottomV)
{
	D3DSURFACE_DESC dtdsdRT;
	PDIRECT3DSURFACE9 pSurfRT;

	// Acquire render target width and height
	g_pd3dDevice->GetRenderTarget(0, &pSurfRT);
	pSurfRT->GetDesc(&dtdsdRT);
	pSurfRT->Release();

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	FLOAT fWidth5 = (FLOAT)dtdsdRT.Width - 0.5f;
	FLOAT fHeight5 = (FLOAT)dtdsdRT.Height - 0.5f;

	// Draw the quad
	ScreenVertex svQuad[4];

	svQuad[0].p = D3DXVECTOR4(-0.5f, -0.5f, 0.5f, 1.0f);
	svQuad[0].t = D3DXVECTOR2(fLeftU, fTopV);

	svQuad[1].p = D3DXVECTOR4(fWidth5, -0.5f, 0.5f, 1.0f);
	svQuad[1].t = D3DXVECTOR2(fRightU, fTopV);

	svQuad[2].p = D3DXVECTOR4(-0.5f, fHeight5, 0.5f, 1.0f);
	svQuad[2].t = D3DXVECTOR2(fLeftU, fBottomV);

	svQuad[3].p = D3DXVECTOR4(fWidth5, fHeight5, 0.5f, 1.0f);
	svQuad[3].t = D3DXVECTOR2(fRightU, fBottomV);

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	g_pd3dDevice->SetFVF(ScrVer.FVF);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertex));
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

//-----------------------------------------------------------------------------
// Name: BrightPass_To_StarSource
// Desc: Perform a 5x5 gaussian blur on g_pTexBrightPass and place the result
//       in g_pTexStarSource. The bright-pass filtered image is blurred before
//       being used for star operations to avoid aliasing artifacts.
//-----------------------------------------------------------------------------
HRESULT GaussBlur::GaussBlurMap(IDirect3DDevice9* g_pd3dDevice, LPDIRECT3DTEXTURE9 InMap, LPDIRECT3DTEXTURE9 OutMap)
{
	HRESULT hr = S_OK;

	D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];
	D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];

	// Get the new render target surface
	PDIRECT3DSURFACE9 pSurfStarSource;
	hr = OutMap->GetSurfaceLevel(0, &pSurfStarSource);
	if (FAILED(hr))
		goto LCleanReturn;


	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
	GetTextureRect(OutMap, &rectDest);
	InflateRect(&rectDest, -1, -1);

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	CoordRect coords;
	GetTextureCoords(InMap, NULL, OutMap, &rectDest, &coords);//g_pShadowMap

	// Get the sample offsets used within the pixel shader
	D3DSURFACE_DESC desc;
	hr = InMap->GetLevelDesc(0, &desc);
	if (FAILED(hr))
		return hr;


	GetSampleOffsets_GaussBlur5x5(desc.Width, desc.Height, avSampleOffsets, avSampleWeights);
	g_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
	g_pEffect->SetValue("g_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

	// The gaussian blur smooths out rough edges to avoid aliasing effects
	// when the star effect is run
	g_pEffect->SetTechnique("GaussBlur5x5");

	g_pd3dDevice->SetRenderTarget(0, pSurfStarSource);
	g_pd3dDevice->SetTexture(0, InMap);//g_pShadowMap
	g_pd3dDevice->SetScissorRect(&rectDest);
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	UINT uiPassCount, uiPass;
	hr = g_pEffect->Begin(&uiPassCount, 0);
	if (FAILED(hr))
		goto LCleanReturn;

	for (uiPass = 0; uiPass < uiPassCount; uiPass++)
	{
		g_pEffect->BeginPass(uiPass);

		// Draw a fullscreen quad
		DrawFullScreenQuad(g_pd3dDevice, coords);

		g_pEffect->EndPass();
	}

	g_pEffect->End();
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);


	hr = S_OK;
LCleanReturn:
	SAFE_RELEASE(pSurfStarSource);
	return hr;
}

//-----------------------------------------------------------------------------
// Name: GetTextureCoords()
// Desc: Get the texture coordinates to use when rendering into the destination
//       texture, given the source and destination rectangles
//-----------------------------------------------------------------------------
HRESULT GaussBlur::GetTextureCoords(PDIRECT3DTEXTURE9 pTexSrc, RECT* pRectSrc,
	PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, CoordRect* pCoords)
{
	HRESULT hr = S_OK;
	D3DSURFACE_DESC desc;
	float tU, tV;

	// Validate arguments
	if (pTexSrc == NULL || pTexDest == NULL || pCoords == NULL)
		return E_INVALIDARG;

	// Start with a default mapping of the complete source surface to complete 
	// destination surface
	pCoords->fLeftU = 0.0f;
	pCoords->fTopV = 0.0f;
	pCoords->fRightU = 1.0f;
	pCoords->fBottomV = 1.0f;

	// If not using the complete source surface, adjust the coordinates
	if (pRectSrc != NULL)
	{
		// Get destination texture description
		hr = pTexSrc->GetLevelDesc(0, &desc);
		if (FAILED(hr))
			return hr;

		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / desc.Width;
		tV = 1.0f / desc.Height;

		pCoords->fLeftU += pRectSrc->left * tU;
		pCoords->fTopV += pRectSrc->top * tV;
		pCoords->fRightU -= (desc.Width - pRectSrc->right) * tU;
		pCoords->fBottomV -= (desc.Height - pRectSrc->bottom) * tV;
	}

	// If not drawing to the complete destination surface, adjust the coordinates
	if (pRectDest != NULL)
	{
		// Get source texture description
		hr = pTexDest->GetLevelDesc(0, &desc);
		if (FAILED(hr))
			return hr;

		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / desc.Width;
		tV = 1.0f / desc.Height;

		pCoords->fLeftU -= pRectDest->left * tU;
		pCoords->fTopV -= pRectDest->top * tV;
		pCoords->fRightU += (desc.Width - pRectDest->right) * tU;
		pCoords->fBottomV += (desc.Height - pRectDest->bottom) * tV;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetTextureRect()
// Desc: Get the dimensions of the texture
//-----------------------------------------------------------------------------
HRESULT GaussBlur::GetTextureRect(PDIRECT3DTEXTURE9 pTexture, RECT* pRect)
{
	HRESULT hr = S_OK;

	if (pTexture == NULL || pRect == NULL)
		return E_INVALIDARG;

	D3DSURFACE_DESC desc;
	hr = pTexture->GetLevelDesc(0, &desc);
	if (FAILED(hr))
		return hr;

	pRect->left = 0;
	pRect->top = 0;
	pRect->right = desc.Width;
	pRect->bottom = desc.Height;

	return S_OK;
}