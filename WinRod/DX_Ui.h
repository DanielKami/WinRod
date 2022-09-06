//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of System.cpp file containing all function prototypes used in  it.
//			These prototypes are only important for WinRod.cpp. System provides all the necessary functions for
//			DirectX 9.0c +HLSL 2.0.
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------


class DX_Ui
{
	
//Types
typedef void    (*ON_INIT_DEVICE  )();
typedef bool    (*ON_RESET_DEVICE )();
typedef HRESULT (*ON_LOST_DEVICE  )();


public:

	//constructor
	DX_Ui()
	{

		hFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tachoma");

		g_msaaTotalModes = 0;
		g_msaaCurentModes = -1;

		g_pDevice = NULL;
		g_pDirect3D = NULL;
		
	}

	//destructor
	~DX_Ui()
	{
	    SAFE_RELEASE(g_pDevice);
		SAFE_RELEASE(g_pDirect3D);
		DeleteObject(hFont);
	}

	int                          g_framesPerSecond;
	bool                         g_isFullScreen;
	bool                         g_hasFocus;

	bool                         g_displayHelp;
	bool                         g_enableVerticalSync;
	DWORD                        g_maxAnisotrophy;

	D3DMULTISAMPLE_TYPE			 g_msaaSamples[16];
	int							 g_msaaTotalModes;
	int							 g_msaaCurentModes;

	D3DPRESENT_PARAMETERS        g_params;
	IDirect3DDevice9            *g_pDevice;
	IDirect3D9                  *g_pDirect3D;

	int							MenuHeight;
	HWND						hwndRB;						//The 3D rebar for tools 
	HWND						hwndTB;						//The 3b band for tools 
    HWND						g_hCombo;					//Combo box with what to measure
    HWND						g_hCombo2;					//Combo box with what to measure

	HINSTANCE hInstance;
	HFONT hFont;

	//Function prototypes
	HWND	WINAPI CreateToolbar3D(HWND);
	HWND	CreateAppWindow(WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect);
	LRESULT CALLBACK WindowProc(HWND hWnd, HINSTANCE hinst, UINT msg, WPARAM wParam, LPARAM lParam, ON_LOST_DEVICE OnLostDevice, ON_RESET_DEVICE OnResetDevice);

	void	UpdateFrameRate();
	bool	FramePerSec(DWORD);
	bool	DeviceIsValid(HWND hWnd, ON_LOST_DEVICE, ON_RESET_DEVICE  );
	void	ToggleFullScreen(HWND hWnd, ON_LOST_DEVICE, ON_RESET_DEVICE );
	bool	InitDX(HWND  g_hWnd, ON_INIT_DEVICE);
	bool	LoadShader(WCHAR *pszFilename, LPD3DXEFFECT &pEffect);
	bool	InitFont(WCHAR *pszFont, int ptSize, LPD3DXFONT &pFont);
	HRESULT LoadMesh(DWORD mesh_nr, WCHAR* strFileName, COrbiter *g_Container, IDirect3DTexture9 *g_pNullTexture);
	bool	CreateNullTexture(int width, int height, LPDIRECT3DTEXTURE9 &pTexture);
	bool	MSAAModeSupported( D3DPRESENT_PARAMETERS *g_Params );

private:
	HDC		hdc;
	bool    g_is_sizable;
	void	Log(WCHAR *pszMessage);
	float	GetElapsedTimeInSeconds();
	bool	InitD3D(HWND  g_hWnd );
	void	ChooseBestMSAAMode( D3DPRESENT_PARAMETERS *g_Params );

	bool	FindDepthStencillFormat(); 
	void	SetProcessorAffinity(void);
};