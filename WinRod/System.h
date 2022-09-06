//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of System.cpp file containing all function prototypes used in  it.
//			These prototypes are only important for WinRod.cpp. System provides all the necessary functions for
//			DirectX 9.0c +HLSL 2.0.
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------


class System
{


//Types
typedef void    (*ON_INIT_DEVICE  )( HWND hWnd );
typedef bool    (*ON_RESET_DEVICE )( HWND hWnd );
typedef HRESULT (*ON_LOST_DEVICE  )( void );



public:

	//constructor
	System(){}
	//destructor
	~System(){
	    SAFE_RELEASE(g_pDevice);
		SAFE_RELEASE(g_pDirect3D);
	}

	int                          g_framesPerSecond;
	bool                         g_isFullScreen;
	bool                         g_hasFocus;


	bool                         g_enableVerticalSync;
	DWORD                        g_maxAnisotrophy;
	DWORD						 g_msaaSamples;
	bool                         g_displayHelp;

	D3DPRESENT_PARAMETERS        g_params;

	IDirect3DDevice9            *g_pDevice;
	IDirect3D9                  *g_pDirect3D;

	int							MenuHeight;
	HWND						hwndRB;						//The 3D rebar for tools 
	HWND						hwndTB;						//The 3b band for tools 
    HWND						g_hCombo;					//Combo box with what to measure
    HWND						g_hCombo2;					//Combo box with what to measure

	//Function prototypes
	HWND	CreateAppWindow(const WNDCLASSEX &wcl, TCHAR *pszTitle, HWND hwnd_parent, int left = 0, int top = 0, int halfScreenWidth = 0, int halfScreenHeight = 0);
	LRESULT CALLBACK WindowProc(HWND hWnd, HINSTANCE hinst, UINT msg, WPARAM wParam, LPARAM lParam, ON_LOST_DEVICE OnLostDevice, ON_RESET_DEVICE OnResetDevice);
	void	SetProcessorAffinity(void);
	void	UpdateFrameRate();
	bool	FramePerSec(DWORD);
	bool	DeviceIsValid(HWND hWnd, ON_LOST_DEVICE, ON_RESET_DEVICE  );
	void	ToggleFullScreen(HWND hWnd, ON_LOST_DEVICE, ON_RESET_DEVICE );
	bool	Init(HWND  g_hWnd, ON_INIT_DEVICE);
	bool	LoadShader(HWND hwnd, TCHAR *pszFilename, LPD3DXEFFECT &pEffect);
	bool	InitFont(TCHAR *pszFont, int ptSize, LPD3DXFONT &pFont);
	HRESULT LoadMesh(HWND hWnd, DWORD mesh_nr, WCHAR* strFileName, COrbiter *g_Container, IDirect3DTexture9 *g_pNullTexture);
	bool	CreateNullTexture(int width, int height, LPDIRECT3DTEXTURE9 &pTexture);

private:
	bool    g_is_sizable;
	void	Log(TCHAR *pszMessage);
	float	GetElapsedTimeInSeconds();
	bool	InitD3D(HWND  g_hWnd );
	void	ChooseBestMSAAMode( D3DPRESENT_PARAMETERS *g_Params, DWORD &samplesPerPixel );
	bool	MSAAModeSupported( D3DPRESENT_PARAMETERS *g_Params );
	bool	FindDepthStencillFormat(); 

};