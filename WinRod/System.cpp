//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Sets the 3D environment provided by DIRECTX 9.0c. Also gives the support for HLSL 2.0 shader use.

// Writen by Daniel Kaminski 
//
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
//      include files                                                    
//--------------------------------------------------------------------------------------------------------------------------------
#include <stdafx.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "3Ddef.h"
#include <sstream>
#include <stdexcept>
#include <string>
#include <Mmsystem.h>
#include "Camera.h" 
#include "System.h"

#include "Resource.h"




//--------------------------------------------------------------------------------------------------------------------------------
// Functions: Main call back function for the first window with DX9
//21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK System::WindowProc(HWND hWnd, HINSTANCE hinst, UINT msg, WPARAM wParam, LPARAM lParam, ON_LOST_DEVICE OnLostDevice, ON_RESET_DEVICE OnResetDevice)
{
	RECT rect;

    switch (msg)
    {
	case WM_CREATE:

		//Copy position
        GetClientRect(hWnd, &rect);
        g_params.BackBufferWidth = rect.right - rect.left;
        g_params.BackBufferHeight = rect.bottom - rect.top;
		g_is_sizable = true;
		g_enableVerticalSync = true;
		break;

    case WM_ACTIVATE:
        switch (wParam)
        {


        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            g_hasFocus = true; //to avoid problems after restoring the window
            break;

        case WA_INACTIVE:
    //        if (g_isFullScreen)
    //            ShowWindow(hWnd, SW_MINIMIZE);
            g_hasFocus = false; //now device is set so we can resize
            break;

        default:
            break;

        }
        break;

/////////////////////////////////////////////////////////////////////////////////////////////////

    case WM_CHAR:
        switch (static_cast<int>(wParam))
        {
        case VK_ESCAPE:
			ToggleFullScreen(hWnd, OnLostDevice, OnResetDevice);
            break;

        case VK_SPACE:
            break;

        case VK_BACK:
            break;

        case 'h':
        case 'H':
          g_displayHelp = !g_displayHelp;  
            break;

        default:
            break;
        }
        break;

    case WM_DESTROY:
		g_is_sizable = false;
        return 0;


	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint (hWnd, &ps);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, (HBRUSH) (COLOR_WINDOW+1) );	
		EndPaint (hWnd, &ps);

		}
		break;

    case WM_SIZE:
		SendMessage( hwndRB, WM_SIZE, 0, 0 );

		//InvalidateRect(hWnd, 0, 0);
		GetWindowRect(hwndRB, &rect);
		
		MenuHeight = rect.bottom - rect.top+2;
		if(g_is_sizable){ //We have to omit the resize rutine to avoid problems after restoring the 3D window
		// If the device is not NULL and the WM_SIZE message is not a 
		// SIZE_MINIMIZED event, resize the device's swap buffers to match
		// the new window size.
			if( g_pDevice != NULL && wParam != SIZE_MINIMIZED ){
				OnLostDevice();  //delete all things
				g_params.BackBufferWidth  = LOWORD(lParam);
				g_params.BackBufferHeight = HIWORD(lParam);

				HRESULT hr = g_pDevice->Reset( &g_params );

                if( hr == D3DERR_INVALIDCALL )
                    {
                        MessageBox( NULL, L"Call to Reset D3 device() failed with D3DERR_INVALIDCALL! ",
                            L"ERROR", MB_OK | MB_ICONEXCLAMATION );
                    }
				OnResetDevice(hWnd);
			}

		}
		else{
			g_is_sizable = true;
		}
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN)
            ToggleFullScreen(hWnd, OnLostDevice, OnResetDevice);
        break;

    default:
		//ProcessMouseInput(hWnd, msg, wParam, lParam);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


//--------------------------------------------------------------------------------------------------------------------------------
    // Assign the current thread to one processor. This ensures that timing
    // code runs on only one processor, and will not suffer any ill effects
    // from power management.
    //
    // Based on the DXUTSetProcessorAffinity() function in the DXUT framework.
	// 21.01.2013 by DXUT rutine
//--------------------------------------------------------------------------------------------------------------------------------
void System::SetProcessorAffinity()
{

    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;
    HANDLE hCurrentProcess = GetCurrentProcess();

    if (!GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask))
        return;

    if (dwProcessAffinityMask)
    {
        // Find the lowest processor that our process is allowed to run against.

        DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

        // Set this as the processor that our thread must always run against.
        // This must be a subset of the process affinity mask.

        HANDLE hCurrentThread = GetCurrentThread();

        if (hCurrentThread != INVALID_HANDLE_VALUE)
        {
            SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
            CloseHandle(hCurrentThread);
        }
    }

    CloseHandle(hCurrentProcess);
}


//--------------------------------------------------------------------------------------------------------------------------------
// Update framerate.
// 21.01.2013 by DXUT framework
//--------------------------------------------------------------------------------------------------------------------------------
void System::UpdateFrameRate()
{
    static float accumTimeSec = 0.;
    static int frames = 0;

    accumTimeSec += GetElapsedTimeInSeconds();

    if (accumTimeSec > 1.0f)
    {
        g_framesPerSecond = frames;

        frames = 0;
        accumTimeSec = 0.0f;
    }
    else
    {
        ++frames;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// Limiter of frame per sec to save the procesor time. the GetTickCount() works on milisec this is not enought
// for good tuning of higher framerate in this case is better to use QueryPerformanceCounter(&dwCurrentTime) and LARGE_INTEGER
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool System::FramePerSec(DWORD dwFPSLimit){

 static DWORD dwLastFrameTime;
 DWORD dwCurrentTime= GetTickCount();

	if ((dwCurrentTime - dwLastFrameTime)  < (DWORD)(1000 / dwFPSLimit) ){ // 1000 miliseconds in a second.	
		return false;
	}	
	dwLastFrameTime = dwCurrentTime;
 return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Print mesage
// 21.01.2013 by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void System::Log( TCHAR *pszMessage)
{
    MessageBox(0, pszMessage, L"Error", MB_ICONSTOP);
}


//--------------------------------------------------------------------------------------------------------------------------------
// Find the best MSAA Mode. 
// 21.01.2013 modified by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool System::MSAAModeSupported( D3DPRESENT_PARAMETERS *g_Params)
{
    DWORD backBufferQualityLevels = 0;
    DWORD depthStencilQualityLevels = 0;

    HRESULT hr = g_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL, g_Params->BackBufferFormat, g_Params->Windowed, g_Params->MultiSampleType,
        &backBufferQualityLevels);

    if (SUCCEEDED(hr))
    {
        hr = g_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL, g_Params->AutoDepthStencilFormat, g_Params->Windowed, g_Params->MultiSampleType,
            &depthStencilQualityLevels);

        if (SUCCEEDED(hr))
        {
            if (backBufferQualityLevels == depthStencilQualityLevels)
            {
                // The valid range is between zero and one less than the level
                // returned by IDirect3D9::CheckDeviceMultiSampleType().

                if (backBufferQualityLevels > 0)
                    g_Params->MultiSampleQuality = backBufferQualityLevels - 1;
                else
                    g_Params->MultiSampleQuality = backBufferQualityLevels;

                return true;
            }
        }
    }

    return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Set the best MSAA Mode. 
// 21.01.2013 modification of DXUT framework by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void System::ChooseBestMSAAMode(D3DPRESENT_PARAMETERS *g_Params, DWORD &samplesPerPixel)
{
    bool supported = false;

    struct MSAAMode
    {
        D3DMULTISAMPLE_TYPE type;
        DWORD samples;
    }
    multsamplingTypes[15] =
    {
        { D3DMULTISAMPLE_16_SAMPLES,  16 },
        { D3DMULTISAMPLE_15_SAMPLES,  15 },
        { D3DMULTISAMPLE_14_SAMPLES,  14 },
        { D3DMULTISAMPLE_13_SAMPLES,  13 },
        { D3DMULTISAMPLE_12_SAMPLES,  12 },
        { D3DMULTISAMPLE_11_SAMPLES,  11 },
        { D3DMULTISAMPLE_10_SAMPLES,  10 },
        { D3DMULTISAMPLE_9_SAMPLES,   9  },
        { D3DMULTISAMPLE_8_SAMPLES,   8  },
        { D3DMULTISAMPLE_7_SAMPLES,   7  },
        { D3DMULTISAMPLE_6_SAMPLES,   6  },
        { D3DMULTISAMPLE_5_SAMPLES,   5  },
        { D3DMULTISAMPLE_4_SAMPLES,   4  },
        { D3DMULTISAMPLE_3_SAMPLES,   3  },
        { D3DMULTISAMPLE_2_SAMPLES,   2  }
    };

    for (int i = 0; i < 16; ++i)
    {
        g_Params->MultiSampleType = multsamplingTypes[i].type;

        supported = MSAAModeSupported( g_Params);

        if (supported)
        {
            samplesPerPixel = multsamplingTypes[i].samples;
            return;
        }
    }

    g_Params->MultiSampleType = D3DMULTISAMPLE_NONE;
    g_Params->MultiSampleQuality = 0;
    samplesPerPixel = 1;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Validat the device. 
// 21.01.2013 modification of DXUT framework by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool System::DeviceIsValid(HWND hWnd, ON_LOST_DEVICE g_LostDevice, ON_RESET_DEVICE g_ResetDevice)
{
    HRESULT hr = g_pDevice->TestCooperativeLevel();

    if (FAILED(hr))
    {
		if (hr == D3DERR_DEVICENOTRESET){
			g_LostDevice();

			if (FAILED(g_pDevice->Reset(&g_params)))
		     return false;

            return g_ResetDevice(hWnd);
		}
    }

    return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Set the full screen.  
// 21.01.2013 modification of DXUT framework by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
void System::ToggleFullScreen(HWND hWnd, ON_LOST_DEVICE OnLostDev, ON_RESET_DEVICE OnResetDev)
{
    static DWORD savedExStyle;
    static DWORD savedStyle;
    static RECT rcSaved;

    g_isFullScreen = !g_isFullScreen;

    if (g_isFullScreen)
    {
        // Moving to full screen mode.
        savedExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
        savedStyle = GetWindowLong(hWnd, GWL_STYLE);
        GetWindowRect(hWnd, &rcSaved);

        SetWindowLong(hWnd, GWL_EXSTYLE, 0);
        SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        g_params.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
        g_params.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);

		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
            g_params.BackBufferWidth , g_params.BackBufferHeight, SWP_SHOWWINDOW);

        // Update presentation parameters.
        g_params.Windowed = FALSE;
        //g_params.BackBufferWidth = g_windowWidth;
        //g_params.BackBufferHeight = g_windowHeight;

        if (g_enableVerticalSync)
        {
            g_params.FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        }
        else
        {
            g_params.FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_IMMEDIATE;
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        }
    }
    else
    {
        // Moving back to windowed mode.
		OnLostDev();
		
        SetWindowLong(hWnd, GWL_EXSTYLE, savedExStyle);
        SetWindowLong(hWnd, GWL_STYLE, savedStyle);

        g_params.BackBufferWidth = rcSaved.right - rcSaved.left;
        g_params.BackBufferHeight = rcSaved.bottom - rcSaved.top;

        // Update presentation parameters.
        g_params.Windowed = TRUE;
//        g_params.BackBufferWidth =  g_windowWidth;
//        g_params.BackBufferHeight = g_windowHeight;
        g_params.FullScreen_RefreshRateInHz = 0;

        if (g_enableVerticalSync)
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        else
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//
	OnResetDev(hWnd);
    //   SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
    //        SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        SetWindowPos(hWnd, HWND_NOTOPMOST, rcSaved.left, rcSaved.top,
            g_params.BackBufferWidth, g_params.BackBufferHeight, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    }
 
}


//--------------------------------------------------------------------------------------------------------------------------------
// Init DirectX.   
// 21.01.2013 modification of DXUT framework by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool System::InitD3D(HWND  g_hWnd )
{
    HRESULT hr = 0;
    D3DDISPLAYMODE desktop = {0};

    g_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!g_pDirect3D)
        return false;

    // Just use the current desktop display mode.
    hr = g_pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &desktop);

    if (FAILED(hr))
    {
        g_pDirect3D->Release();
        g_pDirect3D = 0;
        return false;
    }

	//Find the best stencil format 
	FindDepthStencillFormat();

    // Setup Direct3D for windowed rendering.
    g_params.BackBufferWidth = 0;
    g_params.BackBufferHeight = 0;
    g_params.BackBufferFormat = desktop.Format;
    g_params.BackBufferCount = 1;
    g_params.hDeviceWindow = g_hWnd;
    g_params.Windowed = TRUE;
    g_params.EnableAutoDepthStencil = TRUE;
    //g_params.AutoDepthStencilFormat = g_D3Dpp.AutoDepthStencilFormat;//D3DFMT_D24S8;//already set by FindDepthStencillFormat

    g_params.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    g_params.FullScreen_RefreshRateInHz = 0;

    if (g_enableVerticalSync)
        g_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    else
        g_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Swap effect must be D3DSWAPEFFECT_DISCARD for multi-sampling support.
    g_params.SwapEffect = D3DSWAPEFFECT_DISCARD;

    // Select the highest quality multi-sample anti-aliasing (MSAA) mode.
    ChooseBestMSAAMode( &g_params, g_msaaSamples);

    // Most modern video cards should have no problems creating pure devices.
    // Note that by creating a pure device we lose the ability to debug vertex
    // and pixel shaders.
    hr = g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
        &g_params, &g_pDevice);

    if (FAILED(hr))
    {
        // Fall back to software vertex processing for less capable hardware.
        // Note that in order to debug vertex shaders we must use a software
        // vertex processing device.
        hr = g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_params, &g_pDevice);
    }

    if (FAILED(hr))
    {
        g_pDirect3D->Release();
        g_pDirect3D = 0;
        return false;
    }

    D3DCAPS9 caps;

    // Prefer anisotropic texture filtering if it's supported.
    if (SUCCEEDED(g_pDevice->GetDeviceCaps(&caps)))
    {
        if (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY)
            g_maxAnisotrophy = caps.MaxAnisotropy;
        else
            g_maxAnisotrophy = 1;
    }

    return true;
}


//--------------------------------------------------------------------------------------------------------------------------------
    // Returns the elapsed time (in seconds) since the last time this function
    // was called. This elaborate setup is to guard against large spikes in
    // the time returned by QueryPerformanceCounter().
	// 21.01.2013 
//--------------------------------------------------------------------------------------------------------------------------------
float System::GetElapsedTimeInSeconds()
{


    static const int MAX_SAMPLE_COUNT = 50;

    static float frameTimes[MAX_SAMPLE_COUNT];
    static float timeScale = 0.0f;
    static float actualElapsedTimeSec = 0.0f;
    static INT64 freq = 0;
    static INT64 lastTime = 0;
    static int sampleCount = 0;
    static bool initialized = false;

    INT64 time = 0;
    float elapsedTimeSec = 0.0f;

    if (!initialized)
    {
        initialized = true;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&lastTime));
        timeScale = 1.0f / freq;
    }

    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
    elapsedTimeSec = (time - lastTime) * timeScale;
    lastTime = time;

    if (fabsf(elapsedTimeSec - actualElapsedTimeSec) < 1.0f)
    {
        memmove(&frameTimes[1], frameTimes, sizeof(frameTimes) - sizeof(frameTimes[0]));
        frameTimes[0] = elapsedTimeSec;

        if (sampleCount < MAX_SAMPLE_COUNT)
            ++sampleCount;
    }

    actualElapsedTimeSec = 0.0f;

    for (int i = 0; i < sampleCount; ++i)
        actualElapsedTimeSec += frameTimes[i];

    if (sampleCount > 0)
        actualElapsedTimeSec /= sampleCount;

    return actualElapsedTimeSec;
}



//--------------------------------------------------------------------------------------------------------------------------------
// Create the window used by DirectX
// 21/01/2013 created by Daniel Kamisnki
//--------------------------------------------------------------------------------------------------------------------------------
HWND System::CreateAppWindow(const WNDCLASSEX &wcl, TCHAR *pszTitle, HWND hwnd_parent, int left, int top, int halfScreenWidth, int halfScreenHeight ){

    // Create a window that is centered on the desktop. It's exactly 1/4 the
    // size of the desktop.

    DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;//
 //   DWORD wndStyle =  WS_SYSMENU |WS_DLGFRAME|// WS_CAPTION WS_OVERLAPPED |
 //       WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS   WS_SIZEBOX  ;//  WS_CLIPCHILDREN|

    DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
					 WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
					 WS_CLIPSIBLINGS |  WS_SIZEBOX;// // WS_CLIPCHILDREN

    HWND hWnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
        wndStyle, 0, 0, 0, 0, hwnd_parent, 0, wcl.hInstance, 0);

    if (hWnd)
    {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        if(halfScreenWidth  == 0) halfScreenWidth = screenWidth / 3;
        if(halfScreenHeight == 0) halfScreenHeight = screenHeight / 3;
        if(left == 0)  left = (screenWidth - halfScreenWidth) / 2;
        if(top  == 0)  top = (screenHeight - halfScreenHeight) / 2;
        RECT rc = {0};

        SetRect(&rc, left, top, left + halfScreenWidth, top + halfScreenHeight);
        AdjustWindowRectEx(&rc, wndStyle, FALSE, wndExStyle);
        MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

        GetClientRect(hWnd, &rc);
        g_params.BackBufferWidth = rc.right - rc.left;
        g_params.BackBufferHeight = rc.bottom - rc.top;
    }

    return hWnd;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Init DirectX part of aplication
// 21/01/2013 created by Daniel Kamisnki
//--------------------------------------------------------------------------------------------------------------------------------
bool System::Init(HWND  g_hWnd, ON_INIT_DEVICE OnInitDev)
{   
	WCHAR Wstr[200];
    if (!InitD3D( g_hWnd ))
    {
        Log(L"Direct3D initialization failed!");
        return false;
    }

    try
    {
        OnInitDev(g_hWnd);
        return true;
    }
   catch (TCHAR * str)
    {
		swprintf_s( Wstr, 100, L"Application initialization failed! %s", str);
        Log(Wstr);
        return false;
    }    
}


//--------------------------------------------------------------------------------------------------------------------------------
// Load shader from file
// 21/01/2013 modified by Daniel Kamisnki
//--------------------------------------------------------------------------------------------------------------------------------
bool System::LoadShader(HWND hwnd, TCHAR *pszFilename, LPD3DXEFFECT &pEffect)
{
    ID3DXBuffer *pCompilationErrors = 0;
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;

    // Both vertex and pixel shaders can be debugged. To enable shader
    // debugging add the following flag to the dwShaderFlags variable:
    //      dwShaderFlags |= D3DXSHADER_DEBUG;
    //
    // Vertex shaders can be debugged with either the REF device or a device
    // created for software vertex processing (i.e., the IDirect3DDevice9
    // object must be created with the D3DCREATE_SOFTWARE_VERTEXPROCESSING
    // behavior). Pixel shaders can be debugged only using the REF device.
    //
    // To enable vertex shader debugging add the following flag to the
    // dwShaderFlags variable:
    //     dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    //
    // To enable pixel shader debugging add the following flag to the
    // dwShaderFlags variable:
    //     dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;

	//The path can be changed . To avoid problems with the path ans fx file we have to set again the aplication directory
       // Load the texture
	TCHAR str[MAX_PATH];
		if(GetModuleFileName(NULL, str, MAX_PATH))
		{
		TCHAR* t = wcsrchr(str,'\\');
		if(t!=NULL)
			{
			wcscpy_s(t, MAX_PATH, L"\\\\");
			t = wcsrchr(str,'\\');
			wcscpy_s(t, MAX_PATH, pszFilename); //Vlad
			}

		else MessageBox(hwnd, TEXT("Incorrect texture file  path!"), NULL, MB_ICONERROR); //DK
	}

    HRESULT hr = D3DXCreateEffectFromFile(g_pDevice, str, 0, 0,
        dwShaderFlags, 0, &pEffect, &pCompilationErrors);

    if (FAILED(hr))
    {
        if (pCompilationErrors)
        {
            std::string compilationErrors(static_cast<const char *>(
                pCompilationErrors->GetBufferPointer()));

            pCompilationErrors->Release();
            throw std::runtime_error(compilationErrors);
        }
    }

    if (pCompilationErrors)
        pCompilationErrors->Release();

    return pEffect != 0;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Init Font.
// 21/01/2013 modified by Daniel Kamisnki
//--------------------------------------------------------------------------------------------------------------------------------
bool System::InitFont(TCHAR *pszFont, int ptSize, LPD3DXFONT &pFont)
{
    static DWORD dwQuality = 0;

    // Prefer ClearType font quality if available.

    if (!dwQuality)
    {
		DWORD dwVersion = GetVersion();
        DWORD dwMajorVersion = static_cast<DWORD>((LOBYTE(LOWORD((DWORD)dwVersion))));
		DWORD dwMinorVersion = static_cast<DWORD>((HIBYTE(LOWORD((DWORD)dwVersion))));

        // Windows XP and higher will support ClearType quality fonts.
        if (dwMajorVersion >= 6 || (dwMajorVersion == 5 && dwMinorVersion == 1))
            dwQuality = CLEARTYPE_QUALITY;
        else
            dwQuality = ANTIALIASED_QUALITY;
    }

    int logPixelsY = 0;

    // Convert from font point size to pixel size.

    if (HDC hDC = GetDC((0)))
    {
        logPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
        ReleaseDC(0, hDC);
    }

    int fontCharHeight = -ptSize * logPixelsY / 72;

    // Now create the font. Prefer anti-aliased text.

    HRESULT hr = D3DXCreateFont(
        g_pDevice,
        fontCharHeight,                 // height
        0,                              // width
        FW_BOLD,                        // weight
        1,                              // mipmap levels
        FALSE,                          // italic
        DEFAULT_CHARSET,                // char set
        OUT_DEFAULT_PRECIS,             // output precision
        dwQuality,                      // quality
        DEFAULT_PITCH | FF_DONTCARE,    // pitch and family
        pszFont,                        // font name
        &pFont);

    return SUCCEEDED(hr) ? true : false;
}


//--------------------------------------------------------------------------------------------------------------------------------
// This function loads the mesh.
//21.01.2013 created by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
HRESULT System::LoadMesh(HWND hWnd, DWORD mesh_nr, WCHAR* strFileName, COrbiter *g_Container, IDirect3DTexture9 *g_pNullTexture)//
{

    WCHAR str[MAX_PATH];
    HRESULT hr;
	LPD3DXBUFFER ppAdjacencyBuffer;
	LPD3DXBUFFER pD3DXMtrlBuffer;

	//find the patch
	if(GetModuleFileName(NULL, str, MAX_PATH))
		{
		TCHAR* t = wcsrchr(str,'\\');
		if(t!=NULL)
			{
			wcscpy_s(t, MAX_PATH, L"\\\\");
			t = wcsrchr(str,'\\');
			wcscpy_s(t, MAX_PATH,strFileName); //Vlad
			}
		else MessageBox(hWnd, TEXT("Incorrect mesh file  path!"), NULL, MB_ICONERROR); //DK
	}	

	// Load the mesh from the disk 
	DWORD m_dwNumMaterialsTemp;
	V_RETURN(D3DXLoadMeshFromX (str, D3DXMESH_MANAGED, g_pDevice, &ppAdjacencyBuffer, &pD3DXMtrlBuffer, NULL, &m_dwNumMaterialsTemp, &g_Container->m_Mesh)); 

	//copy the number of materials to container
	g_Container->m_dwNumMaterials = m_dwNumMaterialsTemp;

	D3DXMATERIAL* matMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	
	// Declare an array of materials
	g_Container->m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterialsTemp+1];
    g_Container->g_pMeshTexture = new LPDIRECT3DTEXTURE9[m_dwNumMaterialsTemp+1];

	// Loop through the materials in the mesh and create a D3DMATERIAL for each one
	for(DWORD i = 0; i <m_dwNumMaterialsTemp ; i++)//1
	{
		//Copy the material
		g_Container->m_pMeshMaterials[i] = matMaterials[i].MatD3D;
 
		//Set the ambient color for the material (D3DX does not do this)
		g_Container->m_pMeshMaterials[i].Ambient = g_Container->m_pMeshMaterials[i].Diffuse;

		g_Container->g_pMeshTexture[i] = NULL;

        // Get a path to the texture
        if( matMaterials[i].pTextureFilename != NULL )
        {
            WCHAR wszBuf[MAX_PATH];
            MultiByteToWideChar( CP_ACP, 0, matMaterials[i].pTextureFilename, -1, wszBuf, MAX_PATH );
            wszBuf[MAX_PATH - 1] = L'\0';

        // Load the texture
		if(GetModuleFileName(NULL, str, MAX_PATH))
		{
		TCHAR* t = wcsrchr(str,'\\');
		if(t!=NULL)
			{
			wcscpy_s(t, MAX_PATH, L"\\textures\\\\");
			t = wcsrchr(str,'\\');
			wcscpy_s(t, MAX_PATH, wszBuf); //Vlad
			}

		else MessageBox(hWnd, TEXT("Incorrect texture file  path!"), NULL, MB_ICONERROR); //DK
	}

			V_RETURN( D3DXCreateTextureFromFileEx( g_pDevice, str, D3DX_DEFAULT, D3DX_DEFAULT,
						                       D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
											   D3DX_DEFAULT, D3DX_DEFAULT, 0,
											   NULL, NULL, &(g_Container->g_pMeshTexture[i]) ) );	
		}
       else
        {
            // Use the default texture
            g_Container->g_pMeshTexture[i] = g_pNullTexture;
            g_Container->g_pMeshTexture[i]->AddRef();
       }

	}
 return hr;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Create empty texture.
//21.01.2013 modified by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool System::CreateNullTexture(int width, int height, LPDIRECT3DTEXTURE9 &pTexture)
{
    // Create an empty white texture. This texture is applied to geometry
    // that doesn't have any texture maps. This trick allows the same shader to
    // be used to draw the geometry with and without textures applied.

    HRESULT hr = D3DXCreateTexture(g_pDevice, width, height, 0, 0,
                    D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pTexture);

    if (FAILED(hr))
        return false;

    LPDIRECT3DSURFACE9 pSurface = 0;

    if (SUCCEEDED(pTexture->GetSurfaceLevel(0, &pSurface)))
    {
        D3DLOCKED_RECT rcLock = {0};

        if (SUCCEEDED(pSurface->LockRect(&rcLock, 0, 0)))
        {
            BYTE *pPixels = static_cast<BYTE*>(rcLock.pBits);
            int widthInBytes = width * 4;

            if (widthInBytes == rcLock.Pitch)
            {
                memset(pPixels, 0xff, widthInBytes * height);
            }
            else
            {
                for (int y = 0; y < height; ++y)
                    memset(&pPixels[y * rcLock.Pitch], 0xff, rcLock.Pitch);
            }

            pSurface->UnlockRect();
            pSurface->Release();
            return true;
        }

        pSurface->Release();
    }

    pTexture->Release();
    return false;
}


//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: 
// Builds the D3DPRESENT_PARAMETERS structure using the current window size. 
// Returns: TRUE on success. FALSE if a valid depth format cannot be found.
// 30/01/2013 created by Daniel Kaminski
//--------------------------------------------------------------------------------------------------------------------------------
bool System::FindDepthStencillFormat() 
{

//ZeroMemory( &g_params, sizeof(g_D3Dpp) );

 D3DDISPLAYMODE g_displayMode;
 g_pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &g_displayMode );

 #define WINDOWED TRUE
 D3DFORMAT adapterFormat = (WINDOWED) ? g_displayMode.Format : D3DFMT_A16B16G16R16; //D3DFMT_X8R8G8B8 

 if ( SUCCEEDED( g_pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) ) 
	{
	g_params.AutoDepthStencilFormat = D3DFMT_D24S8; 
	} 
	else if ( SUCCEEDED( g_pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 ) ) ) 
		{
			g_params.AutoDepthStencilFormat = D3DFMT_D24X8; 
		} 
		else if ( SUCCEEDED( g_pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) ) 
			{ 
			g_params.AutoDepthStencilFormat = D3DFMT_D16; 
			} 
			else 
				{
				return false; 
				} 

 return true; 
} 

