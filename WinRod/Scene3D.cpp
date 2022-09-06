//-----------------------------------------------------------------------------
// All functions needed by 3D scene modeling
// 06.07.2014 by Daniel Kaminski
//-----------------------------------------------------------------------------

#include <stdafx.h>				//Header necessary for Visual Studio
#include <commctrl.h>
#include "Scene3D.h"
#include "variables.h"			//Variables defined in structure not critical for thread calculations
#include "Save.h"				//Save functions
#include "CallBack.h"			//Call back functions for dialog boxes 
#include "fit.h"				//Fitting procedures
#include "ExMath.h"				//Additional mathematical functions
#include "ReadFile.h"
#include "elements.h"
#include "Exmath.h"
#include "resource.h"
#include "toolbar3D.h"
#include "Tooltip3D.h"


//Register class for 3D
Scene3D						myScene;


LRESULT CALLBACK WindowScene3DProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case RBN_AUTOSIZE:
			myScene.Rbn_autosize();
			break;

		case TTN_GETDISPINFO:
			Tooltip3D(lParam);
			break;
		}
		break;

	case WM_ACTIVATE:
		switch (wParam)
		{
		case WA_ACTIVE:
			myScene.Update3DModel();
			break;
		}
		break;

	case WM_3D:
		switch (static_cast<int>(wParam))
		{
		case UPDATE:
			if (static_cast<int>(lParam) == NO_CHANGES)
				myScene.Update3DModel(false);
			else
				myScene.Update3DModel();
			break;

		case UPDATE_COMPARE:
			myScene.Update3Dcompare(); //Compare atoms before fitting
			break;

		case CLEAR_MODEL:
			myScene.ClearModel();
			break;

		case DELETE_OBJECTS:
			myScene.DeleteObiects();
			break;

		case UPDATE_WINDOWS_SIZE:
			OnThreadExit(hWnd);
			break;
		}
		break;

	case WM_CREATE:
	{
		myScene.MenuHeight = 33;
		// Get the horizontal and vertical screen sizes in pixel
		RECT desktop;
		// Get a handle to the desktop window
		HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		myScene.hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
		myScene.hwndRB = myScene.CreateToolbar3D(hWnd);	//myScene.g_params.BackBufferWidth

		myScene.ResetMeasurement();

		for (int i = 0; i < myScene.nr_bulk_atoms_toplot; i++)
			myScene.D3DbulkAtom[i].display_mode = 1;

		for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
			myScene.D3DsurfAtom[i].display_mode = 1;

		//set the item in list box 2
		if (myScene.flag.display_mode >= 0 && myScene.flag.display_mode < 5)
			
		ComboBox_SetCurSel(myScene.g_hCombo2, myScene.flag.display_mode);
	}
		break;

	case WM_KEYDOWN:
		switch (static_cast<int>(wParam))
		{
		case VK_DELETE:

			for (int i = 0; i < myScene.nr_bulk_atoms_toplot; i++)
				if (myScene.D3DbulkAtom[i].Selected){
				myScene.D3DbulkAtom[i].AtomicNumber = 0;
				myScene.D3DbulkAtom[i].Selected = 0;
				}
			for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
				if (myScene.D3DsurfAtom[i].Selected){
				myScene.D3DsurfAtom[i].AtomicNumber = 0;
				myScene.D3DsurfAtom[i].Selected = 0;
				}
			for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
				if (myScene.D3DcompAtom[i].Selected){
				myScene.D3DcompAtom[i].AtomicNumber = 0;
				myScene.D3DcompAtom[i].Selected = 0;
				}
			break;

		case VK_F7:
			myScene.flag.g_specular_intensty -= 0.05f;
			if (myScene.flag.g_specular_intensty <= 0) myScene.flag.g_specular_intensty = 0;
			break;

		case VK_F8:
			myScene.flag.g_specular_intensty += 0.05f;
			if (myScene.flag.g_specular_intensty >= 1) myScene.flag.g_specular_intensty = 1;
			break;

		case VK_F5:
			myScene.flag.g_specular -= 1.0f;
			if (myScene.flag.g_specular <= 0.0) myScene.flag.g_specular = 0.0;
			break;

		case VK_F6:
			myScene.flag.g_specular += 1.0f;
			if (myScene.flag.g_specular >= 100) myScene.flag.g_specular = 100;
			break;
		}
		break;

	case WM_COMMAND:
		// Parse the menu selections:
		switch LOWORD(wParam)
		{
		case ID_EXPORT_3D:
			Save3DFunction(hWnd);// the 3D scene				//Thread safe
			break;

		case ID_PRINT_3D:
			Print3DFunction(hWnd);// the 3D scene				//Thread safe
			break;

		case ID_FULSCREAN_3D:
			myScene.ToggleFullScreen(hWnd, OnLostDevice, OnResetDevice);//Thread safe
			break;

		case ID_PROPERITIES_3D:
			DialogBox(myScene.hInstance, MAKEINTRESOURCE(IDD_3D_SETTINGS), hWnd, Settings3D);//Thread safe
			myScene.g_params.MultiSampleType = myScene.g_msaaSamples[myScene.g_msaaCurentModes];
			myScene.MSAAModeSupported(&myScene.g_params);
			if (myScene.g_pDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				OnLostDevice();  //delete all things
				myScene.g_pDevice->Reset(&myScene.g_params); 
				OnResetDevice();
			}

			myScene.Update3DModel();
			myScene.ApplyColors();
			break;

		case ID_PLATE_3D:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_PLATE_3D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
			{
				myScene.flag.g_planeOnOff = true;				//Thread safe
			}
			else
			{
				myScene.flag.g_planeOnOff = false;				//Thread safe
			}
			break;

		case ID_COMPARE_3D:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_COMPARE_3D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
			{
				myScene.flag.g_compare = true;					//Thread safe
			}
			else
			{
				myScene.flag.g_compare = false;					//Thread safe
			}
			break;

		case ID_TERMALRINGS_3D:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_TERMALRINGS_3D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
			{
				myScene.flag.g_termal_rings = true;				//Thread safe
			}
			else
			{
				myScene.flag.g_termal_rings = false;			//Thread safe
			}
			break;

		case ID_CELL:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_CELL, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
			{
				myScene.flag.g_cell = true;						//Thread safe
			}
			else
			{
				myScene.flag.g_cell = false;					//Thread safe
			}
			break;

		case ID_LABELS_3D:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_LABELS_3D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
			{
				myScene.flag.plot_surf_labels = true;			//Thread safe
				myScene.flag.plot_bulk_labels = true;			//Thread safe
			}
			else
			{
				myScene.flag.plot_surf_labels = false;			//Thread safe
				myScene.flag.plot_bulk_labels = false;			//Thread safe
			}
			break;

		case ID_SELECT_3D:
			myScene.g_autorotate = false;						//Thread safe
			myScene.mouse_select = true;						//Thread safe
			myScene.mouse_lasso = false;						//Thread safe
			myScene.mouse_rotate = false;						//Thread safe
			myScene.mouse_translate = false;					//Thread safe
			myScene.mouse_Zcut = false;							//Thread safe
			break;

		case ID_LASSO_3D:
			myScene.g_autorotate = false;						//Thread safe
			myScene.mouse_lasso = true;							//Thread safe
			myScene.mouse_rotate = false;						//Thread safe
			myScene.mouse_select = false;						//Thread safe
			myScene.mouse_translate = false;					//Thread safe
			myScene.mouse_Zcut = false;							//Thread safe
			break;

		case ID_ROTATE_3D:
			myScene.mouse_rotate = true;						//Thread safe	
			myScene.mouse_select = false;						//Thread safe
			myScene.mouse_translate = false;					//Thread safe
			myScene.mouse_Zcut = false;							//Thread safe
			myScene.mouse_lasso = false;						//Thread safe
			break;

		case ID_MOVE_3D:
			myScene.mouse_rotate = false;						//Thread safe
			myScene.mouse_select = false;						//Thread safe
			myScene.mouse_translate = true;						//Thread safe
			myScene.mouse_Zcut = false;							//Thread safe
			myScene.mouse_lasso = false;						//Thread safe
			break;

		case ID_ZOOM_3D:
			myScene.mouse_rotate = false;						//Thread safe
			myScene.mouse_select = false;						//Thread safe
			myScene.mouse_translate = false;					//Thread safe
			myScene.mouse_Zcut = true;							//Thread safe
			myScene.mouse_lasso = false;						//Thread safe
			break;

		case ID_BUL_3D:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_BUL_3D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
			{
				myScene.flag.g_bulk = true;						//Thread safe
			}
			else
			{
				myScene.flag.g_bulk = false;					//Thread safe
			}

			break;

		case ID_SUR_3D:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_SUR_3D, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				myScene.flag.g_surface = true;					//Thread safe
			else
				myScene.flag.g_surface = false;					//Thread safe
			break;

		case ID_PERSPECTIVE:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_PERSPECTIVE, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				myScene.flag.g_perspective = true;				//Thread safe
			else
				myScene.flag.g_perspective = false;				//Thread safe

			//Set on/off perspective
			myScene.TuneCamera();
			break;

		case ID_SURFACEI_II:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_SURFACEI_II, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				myScene.flag.display_surface1 = true;			//Thread safe
			else
				myScene.flag.display_surface1 = false;			//Thread safe

			myScene.Update3DModel();							//Thread safe
			break;

		case ID_CHARGES:
			if ((SendMessage(myScene.hwndTB, TB_GETSTATE, ID_CHARGES, 0) & TBSTATE_CHECKED) == TBSTATE_CHECKED)
				myScene.flag.g_individual_color = true;			//Thread safe
			else
				myScene.flag.g_individual_color = false;			//Thread safe

			myScene.Update3DModel();							//Thread safe
			break;


		case ID_RESET_MEASUREMENT://Reset all measurements
			myScene.ResetAllMeasurements();						//Thread safe
			break;

		case ID_LOOK_X:
			myScene.LookX();									//Thread safe
			break;

		case ID_LOOK_Y:
			myScene.LookY();									//Thread safe
			break;

		case ID_LOOK_Z:
			myScene.LookZ();									//Thread safe
			break;

		case IDC_COMBOBOX:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELENDOK:
			{
				int number = ComboBox_GetCurSel(myScene.g_hCombo);
				SendMessage(myScene.g_hCombo, CB_SETCURSEL, 0, 0);
				ReleaseCapture();								//Thread safe
				myScene.Measure(number);						//Thread safe

				break;
				SetFocus(hWnd);									//Thread safe
			}
			}
			break;
		}
		case IDC_COMBOBOX2:

			switch (HIWORD(wParam))
			{
			case CBN_SELENDOK:
			{
				static int display_mode;
				int temp = ComboBox_GetCurSel(myScene.g_hCombo2);
				if (temp >= 0 && temp < 5)
					display_mode = temp;						//Thread safe
				//Check if something is selected
				int selected = 0;
				for (int i = 0; i < myScene.nr_bulk_atoms_toplot; i++)
				{
					if (myScene.D3DbulkAtom[i].Selected)
						selected++;
				}
				for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
				{
					if (myScene.D3DsurfAtom[i].Selected)
						selected++;
				}

				if (selected == 0)
				{
					myScene.flag.display_mode = display_mode;
					//Set the same style to all atoms
					for (int i = 0; i < myScene.nr_bulk_atoms_toplot; i++)
					{
						myScene.D3DbulkAtom[i].display_mode = display_mode;	//Thread safe
					}
					for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
					{
						myScene.D3DsurfAtom[i].display_mode = display_mode;	//Thread safe
					}
					for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
					{
						myScene.D3DcompAtom[i].display_mode = display_mode;	//Thread safe
					}
				}
				else
				{
					//Set the style to the selected atoms
					for (int i = 0; i < myScene.nr_bulk_atoms_toplot; i++)
					{
						if (myScene.D3DbulkAtom[i].Selected)
							myScene.D3DbulkAtom[i].display_mode = display_mode;	//Thread safe
					}
					for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
					{
						if (myScene.D3DsurfAtom[i].Selected)
							myScene.D3DsurfAtom[i].display_mode = display_mode;	//Thread safe
					}
					for (int i = 0; i < myScene.nr_surf_atoms_toplot; i++)
					{
						if (myScene.D3DcompAtom[i].Selected)
							myScene.D3DcompAtom[i].display_mode = display_mode;	//Thread safe
					}
				}
			}
				SetFocus(hWnd);								//Thread safe
				break;
			}
			break;
		}
		break;
	}
	myScene.ProcessMouseInput(hWnd, msg, wParam, lParam);	//Thread safe
	return  myScene.WindowProc(hWnd, myScene.hInstance, msg, wParam, lParam, OnLostDevice, OnResetDevice);	//Thread safe
}

													//3DX9 functions//
void OnThreadExit(HWND hWnd)
{
	if (IsWindow(hWnd))
	{
		ShowWindow(hWnd, SW_RESTORE);
		RECT rect;
		GetWindowRect(hWnd, &rect);

		//Thread safe
		EnterCriticalSection(&myScene.CriticalSection);
		if (rect.bottom - rect.top > 0 && rect.right - rect.left > 0)
		{
			variable.WindowsRect[10].right = rect.right;
			variable.WindowsRect[10].left = rect.left;
			variable.WindowsRect[10].top = rect.top;
			variable.WindowsRect[10].bottom = rect.bottom;
		}
		if (IsWindow(hWnd) == 0)
			variable.WindowsVisible[10] = false;
		else
			variable.WindowsVisible[10] = true;
		LeaveCriticalSection(&myScene.CriticalSection);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------
void OnDestroyDevice()
{
	SAFE_RELEASE(myScene.g_pEffect);
	SAFE_RELEASE(myScene.g_pFont);
	SAFE_RELEASE(myScene.g_pNullTexture);
	SAFE_RELEASE(myScene._line);
	SAFE_RELEASE(myScene.g_pDirect3D);
	SAFE_RELEASE(myScene.g_pDevice);

	for (int i = 0; i<OBJECT_NUMBER; ++i){
		myScene.g_Orbiter[i].OnDestroyDevice();
	}

}

void OnInitDevice()
{

	// initiate colors
	if (myScene.flag.rgbCurrent==0) myScene.flag.rgbCurrent = RGB(54, 0, 64);
	if (myScene.flag.rgbLight == 0) myScene.flag.rgbLight = RGB(250, 250, 250);
	if (myScene.flag.rgbText == 0) myScene.flag.rgbText = RGB(20, 20, 20);

	myScene.ApplyColors();
	myScene.flag.Init();

	//declare only ones a line here 
	if(D3DXCreateLine(myScene.g_pDevice, &myScene._line) != D3D_OK)
		MessageBox(NULL, TEXT("Failed to create line object DX9"), NULL, MB_ICONERROR);

	if (!myScene.LoadShader(L"HLSL_light.fx", myScene.g_pEffect))
		MessageBox(NULL, TEXT("Failed to load shader: HLSL_light.fx"), NULL, MB_ICONERROR);

	if (!myScene.InitFont(L"Arial", myScene.flag.g_text_size, myScene.g_pFont))
		MessageBox(NULL, TEXT("Failed to create font."), NULL, MB_ICONERROR);

	if (!myScene.CreateNullTexture(2, 2, myScene.g_pNullTexture))
		MessageBox(NULL, TEXT("Failed to create null texture."), NULL, MB_ICONERROR);


	myScene.Init();
	myScene.TuneCamera();
	
	// Load meshes
	WCHAR Wchar[MAX_PATH];
	swprintf(Wchar, MAX_PATH, L"ball.x");//test
	FAILED(myScene.LoadMesh(0, Wchar, &myScene.g_Orbiter[0], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"plane.x");
	FAILED(myScene.LoadMesh(1, Wchar, &myScene.g_Orbiter[1], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"rod.x");
	FAILED(myScene.LoadMesh(2, Wchar, &myScene.g_Orbiter[2], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"ballsmall.x");
	FAILED(myScene.LoadMesh(3, Wchar, &myScene.g_Orbiter[3], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"rodsmall.x");
	FAILED(myScene.LoadMesh(4, Wchar, &myScene.g_Orbiter[4], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"spring.x");
	FAILED(myScene.LoadMesh(5, Wchar, &myScene.g_Orbiter[5], myScene.g_pNullTexture));
	
}

//-----------------------------------------------------------------------------
// Functions: clear memory important for DX9
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
HRESULT OnLostDevice()
{
	HRESULT hr;
	if (FAILED(hr = myScene.g_pEffect->OnLostDevice()))
		return  hr;

	if (FAILED(hr = myScene.g_pFont->OnLostDevice()))
		return  hr;

	if (FAILED(hr = myScene._line->OnLostDevice()))
		return  hr;

	SAFE_RELEASE(myScene.g_pNullTexture);

	for (int i = 0; i < OBJECT_NUMBER; i++)
	{
		myScene.g_Orbiter[i].OnLostDevice();
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Functions: restore memory important for DX9
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
bool OnResetDevice()
{
	if (FAILED(myScene.g_pEffect->OnResetDevice()))
		return false;

	if (FAILED(myScene.g_pFont->OnResetDevice()))
		return false;

	if (FAILED(myScene._line->OnResetDevice()))
		return false;

	if (!myScene.CreateNullTexture(2, 2, myScene.g_pNullTexture))
		MessageBox(NULL, TEXT("Failed to create null texture."), NULL, MB_ICONERROR);

	// Create the depth-stencil buffer to be used with the shadow map
	// We do this to ensure that the depth-stencil buffer is large
	// enough and has correct multisample type/quality when rendering
	// the shadow map.  The default depth-stencil buffer created during
	// device creation will not be large enough if the user resizes the
	// window to a very small size.  Furthermore, if the device is created
	// with multisampling, the default depth-stencil buffer will not
	// work with the shadow map texture because texture render targets
	// do not support multisample.	


	
	// Load the mesh
	WCHAR Wchar[MAX_PATH];
	swprintf(Wchar, MAX_PATH, L"ball.x");
	FAILED(myScene.LoadMesh(0, Wchar, &myScene.g_Orbiter[0], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"plane.x");
	FAILED(myScene.LoadMesh(1, Wchar, &myScene.g_Orbiter[1], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"rod.x");
	FAILED(myScene.LoadMesh(2, Wchar, &myScene.g_Orbiter[2], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"ballsmall.x");
	FAILED(myScene.LoadMesh(3, Wchar, &myScene.g_Orbiter[3], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"rodsmall.x");
	FAILED(myScene.LoadMesh(4, Wchar, &myScene.g_Orbiter[4], myScene.g_pNullTexture));
	swprintf(Wchar, MAX_PATH, L"spring.x");
	FAILED(myScene.LoadMesh(5, Wchar, &myScene.g_Orbiter[5], myScene.g_pNullTexture));
	
	return true;
}

//-----------------------------------------------------------------------------
// Functions: Prepare the frame for render
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void OnFrameRender(HWND hWnd, Thread_t* var_t)
{
	HRESULT hr;
	int frames;

	if (myScene.g_hasFocus)
		frames = 120;
	else
		frames = var_t->Refresh3DIdle;//to save the power

	if (myScene.FramePerSec(frames))
	{
		myScene.UpdateFrameRate();

		if (myScene.DeviceIsValid(hWnd, OnLostDevice, OnResetDevice))
		{

			// Render the shadow map
			LPDIRECT3DSURFACE9 pOldRT = NULL;
			V(myScene.g_pDevice->GetRenderTarget(0, &pOldRT));
			SAFE_RELEASE(pOldRT);

			myScene.g_pDevice->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(GetRValue(myScene.flag.rgbCurrent), GetGValue(myScene.flag.rgbCurrent), GetBValue(myScene.flag.rgbCurrent)), 1.0f, 0);

			if (FAILED(myScene.g_pDevice->BeginScene()))
				return;

			//Draw the scene
			myScene.Scene();

			myScene.g_pDevice->EndScene();

			//Get the viewport dimensions
			RECT rect;
			rect.left = 0;
			rect.right = myScene.g_params.BackBufferWidth;
			rect.top = myScene.MenuHeight;
			rect.bottom = myScene.g_params.BackBufferHeight + myScene.MenuHeight;
			//Copy the scent to the window
			myScene.g_pDevice->Present(0, &rect, hWnd, 0);

		}
	}
}





void   Scene3D::Measure(int number)
{
	switch (number)
	{
	case 1:
		myScene.MeasureDistance(D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
		break;
	case 2:
		myScene.MeasureAngle(D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
		break;
	case 3:
		myScene.MeasureTorsian(D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
		break;
	}
}
void   Scene3D::LookX()
{
	TuneCamera();
	AbsoluteQuaternionRotation((float)(90 * deg2rad), (float)(270 * deg2rad), (float)(0 * deg2rad));
}

void   Scene3D::LookY()
{
	float arb3 = (float)((dlat[5] - 90 * deg2rad));
	TuneCamera();
	AbsoluteQuaternionRotation((float)(90 * deg2rad), (float)(180 * deg2rad)-arb3, (float)(0 * deg2rad));
}

void   Scene3D::LookZ()
{
	TuneCamera();
	double y_t = (float)(dlat[3] - 90.0 * deg2rad);
	double  arb1 = -22.0f * y_t * y_t + 62.f * y_t;

	double x_t = (float)(dlat[4] - 90.0 * deg2rad);
	double arb2 = -22.0f * x_t * x_t + 62.0f * x_t;
	double arb3 = -2.22f * x_t * x_t - 4.6 * x_t;

	//rotation axis x              rotation axis y                 rotation axis z
	AbsoluteQuaternionRotation((float)((0 - arb2) * deg2rad), (float)((180 - arb1- arb3) * deg2rad), (float)(0 * deg2rad));
}

void   Scene3D::Rbn_autosize()
{
	RECT rect;
	GetWindowRect(hwndRB, &rect);
	MenuHeight = rect.bottom - rect.top + 2;
}

void Scene3D::TuneCamera()
{
	//Initialize light

	myScene.g_targetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//Set on/off perspective
	if (myScene.flag.g_perspective)
	{
		myScene.Fovy = CAMERA_FOVY_PERSPECTIVE;
		myScene.Znear = CAMERA_ZNEAR_PERSPECTIVE;
		myScene.Zfar = CAMERA_ZFAR_PERSPECTIVE;
		//Initialize light
		myScene.g_LightPosition = D3DXVECTOR3(0.f, 0, 800.0f);
		//initialize camera
		myScene.g_cameraPos = D3DXVECTOR3(0.0f, 0.0f, 40.0f);
		AbsoluteQuaternionRotation((float)(55 * deg2rad), (float)(180 * deg2rad), (float)(0 * deg2rad));
		myScene.UpdateCamera();
	}
	else
	{
		myScene.Fovy = CAMERA_FOVY_NO_PERSPECTIVE;
		myScene.Znear = CAMERA_ZNEAR_NO_PERSPECTIVE;
		myScene.Zfar = CAMERA_ZFAR_NO_PERSPECTIVE;
		//Initialize light
		myScene.g_LightPosition = D3DXVECTOR3(0.f, 0, 10000);
		//initialize camera
		myScene.g_cameraPos = D3DXVECTOR3(0.0f, 0.0f, 900.0f * 4);
		AbsoluteQuaternionRotation((float)(55 * deg2rad), (float)(180 * deg2rad), (float)(0 * deg2rad));

		myScene.UpdateCamera();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------
void Scene3D::Update3Datoms(void)
//--------------------------------------------------------------------------------------------------------------------------------
{
	int i;
	int SizeOf = sizeof(char) * 3;
	WCHAR WName[5];

	//Make the copy thread safe
	EnterCriticalSection(&CriticalSection);

	for (i = 0; i < 6; ++i)
		dlat[i] = (float)thread.DLAT[i];

	//Dlat1cosDlat5 = (float)variable.Dlat1cosDlat5;
	//Dlat2cosDlat3 = (float)variable.Dlat2cosDlat3;
	//Dlat1sinDlat5 = (float)variable.Dlat1sinDlat5;
	//Dlat2cosDlat4 = (float)variable.Dlat2cosDlat4;

	 a_sin_alp = (float)variable.a_sin_alp;
	 b_cos_gam = (float)variable.b_cos_gam;
	 c_cos_bet__sin_alp = (float)variable.c_cos_bet__sin_alp;
	 b_sin_Gam = (float)variable.b_sin_Gam;
	 c_cos_alp = (float)variable.c_cos_alp;
	 c_sin_bet__sin_alp = (float)variable.c_sin_bet__sin_alp;

	Bul.nr_of_atoms = thread.NBULK;
	for (i = 0; i < thread.NBULK; ++i)
	{
		Bul.Atom[i].AtomicNumber = get_element_number(variable.ELEMENT[thread.TB[i]]);
		Bul.Atom[i].x = (float)thread.XB[i];
		Bul.Atom[i].y = (float)thread.YB[i];
		Bul.Atom[i].z = (float)thread.ZB[i];
		Bul.Atom[i].DW_in_plane = (float)thread.DEBWAL[thread.NDWB[i] - 1];
		Bul.Atom[i].charge = (float)thread.ChargeB[i];

		AtoT(WName, thread.Name[i]);
		memcpy(Bul.Atom[i].Name, WName, sizeof(WName));
	}

//liquid
	flag.g_liquid_profile = thread.LIQUID_PROFILE;

	if (flag.g_liquid_profile)
	{
		nr_liquid_layers_toplot = thread.RealNumberLiquidLayers;
		if (nr_liquid_layers_toplot < 1) nr_liquid_layers_toplot = 1;

		if (D3DLiquid)
			delete[] D3DLiquid;
		D3DLiquid = new AtomParam[nr_liquid_layers_toplot + 2];

		D3DLiquid[0].AtomicNumber = get_element_number(variable.ELEMENT[thread.TS[0]]);
		D3DLiquid[0].x = 0;
		D3DLiquid[0].y = 0;
		D3DLiquid[0].z = (float)(thread.ZSFIT[0] * dlat[2]);
		D3DLiquid[0].DW_out_of_plane = (float)(thread.DEBWAL2[thread.NDWS2[0] - 1]+0.05) * 800;
		D3DLiquid[0].AtomInTheFile = 0;
		D3DLiquid[0].Selected = false;
		if (thread.NOCCUP[0] == 0)
			D3DLiquid[0].occupancy = 1.;
		else
			D3DLiquid[0].occupancy = (float)(thread.OCCUP[thread.NOCCUP[0] - 1]* thread.OCCUP2[thread.NOCCUP2[0] - 1]);


		for (i = 0; i < nr_liquid_layers_toplot + 1; ++i)
		{
			D3DLiquid[i + 1].AtomicNumber = D3DLiquid[0].AtomicNumber;
			D3DLiquid[i + 1].x = 0;
			D3DLiquid[i + 1].y = 0;
			D3DLiquid[i + 1].z = (float)(thread.LIQUID_ZSFIT[i] * dlat[2]);
			D3DLiquid[i + 1].DW_out_of_plane = D3DLiquid[0].DW_out_of_plane;
			if (thread.NOCCUP[0] == 0)
				D3DLiquid[i + 1].occupancy = 1.;
			else
				D3DLiquid[i + 1].occupancy = (float)thread.LIQUID_OCCFIT[i];

			D3DLiquid[i + 1].AtomInTheFile = 0;
			D3DLiquid[i + 1].Selected = false;
			
		}
	}


	if (myScene.flag.display_surface1)
	{
		Fit.nr_of_atoms = Sur.nr_of_atoms = variable.NSURFTOT - thread.NSURF2;
		for (i = 0; i < Fit.nr_of_atoms; ++i){

			Fit.Atom[i].AtomicNumber = get_element_number(variable.ELEMENT[thread.TS[i]]);

			Fit.Atom[i].x = (float)thread.XSFIT[i];
			Fit.Atom[i].y = (float)thread.YSFIT[i];
			Fit.Atom[i].z = (float)thread.ZSFIT[i];

			Fit.Atom[i].DW_in_plane = (float)thread.DEBWAL[thread.NDWS[i] - 1];
			if (thread.NDWS2[i] == 0)
				Fit.Atom[i].DW_out_of_plane = (float)Fit.Atom[i].DW_in_plane;
			else
				Fit.Atom[i].DW_out_of_plane = (float)thread.DEBWAL2[thread.NDWS2[i] - 1];

			Fit.Atom[i].occupancy = (float)(1.0 - thread.SURF2FRAC);
			if (thread.NOCCUP[i] == 0)
				Fit.Atom[i].occupancy *= 1.0f;
			else
				if (!thread.OCCUP_FLAG[i])
					Fit.Atom[i].occupancy *= (float)thread.OCCUP[thread.NOCCUP[i] - 1];
				else
					Fit.Atom[i].occupancy *= (float)(1.0 - thread.OCCUP[thread.NOCCUP[i] - 1]);

			if (variable.NFACTOR > 0)
				Fit.Atom[i].occupancy *= (float)SizeDistribution(thread.NOCCUP2[i] - 1);
			else
			if (thread.NOCCUP2[i] == 0)
				Fit.Atom[i].occupancy *= 1.;
			else
				Fit.Atom[i].occupancy *= (float)thread.OCCUP2[thread.NOCCUP2[i] - 1];

		}

	}
	//Second surface unit cell
	else
	{
		Fit.nr_of_atoms = Sur.nr_of_atoms = thread.NSURF2;
		int j;
		for (i = 0; i < thread.NSURF2; ++i)
		{
			j = i + variable.NSURFTOT - thread.NSURF2;
			Fit.Atom[i].AtomicNumber = get_element_number(variable.ELEMENT[thread.TS[j]]);

			Fit.Atom[i].x = (float)thread.XSFIT[j];
			Fit.Atom[i].y = (float)thread.YSFIT[j];
			Fit.Atom[i].z = (float)thread.ZSFIT[j];

			Fit.Atom[i].DW_in_plane = (float)thread.DEBWAL[thread.NDWS[j] - 1];
			if (thread.NDWS2[j] == 0)
				Fit.Atom[i].DW_out_of_plane = (float)Fit.Atom[j].DW_in_plane;
			else
				Fit.Atom[i].DW_out_of_plane = (float)thread.DEBWAL2[thread.NDWS2[j] - 1];
		  
			Fit.Atom[i].occupancy = (float)thread.SURF2FRAC;
			if (thread.NOCCUP[j] == 0)
				Fit.Atom[i].occupancy *=  1.0f;
			else
				if (!thread.OCCUP_FLAG[j])
					Fit.Atom[i].occupancy *= (float)thread.OCCUP[thread.NOCCUP[j] - 1];
				else
					Fit.Atom[i].occupancy *= (float)(1.0 - thread.OCCUP[thread.NOCCUP[j] - 1]);

			if (variable.NFACTOR > 0)
				Fit.Atom[i].occupancy *= (float)SizeDistribution(thread.NOCCUP2[j] - 1);
			else
				if (thread.NOCCUP2[j] == 0)
					Fit.Atom[i].occupancy *= 1.;
				else
					Fit.Atom[i].occupancy *= (float)thread.OCCUP2[thread.NOCCUP2[j] - 1];


		}
	}
	// Release ownership of the critical section.
	LeaveCriticalSection(&CriticalSection);
}

//--------------------------------------------------------------------------------------------------------------------------------
void Scene3D::Update3Dcompare(void)
//--------------------------------------------------------------------------------------------------------------------------------

{
	int SizeOf = sizeof(char) * 3;

	EnterCriticalSection(&CriticalSection);

	if (myScene.flag.display_surface1)
		Sur.nr_of_atoms = variable.NSURFTOT - thread.NSURF2;
	else
		Sur.nr_of_atoms = thread.NSURF2;
	for (int i = 0; i < Sur.nr_of_atoms; ++i)
	{
		Sur.Atom[i].x = (float)thread.XSFIT[i];
		Sur.Atom[i].y = (float)thread.YSFIT[i];
		Sur.Atom[i].z = (float)thread.ZSFIT[i];

		Sur.Atom[i].DW_in_plane = (float)thread.DEBWAL[thread.NDWS[i] - 1];
		if (thread.NDWS2[i] == 0)
			Sur.Atom[i].DW_out_of_plane = (float)Sur.Atom[i].DW_in_plane;
		else
			Sur.Atom[i].DW_out_of_plane = (float)thread.DEBWAL2[thread.NDWS2[i] - 1];

		if (thread.NOCCUP[i] == 0)
			Sur.Atom[i].occupancy = 1.;
		else
			Sur.Atom[i].occupancy = (float)thread.OCCUP[thread.NOCCUP[i] - 1];

		if (thread.NOCCUP2[i] == 0)
			Sur.Atom[i].occupancy *= 1.;
		else
			Sur.Atom[i].occupancy *= (float)thread.OCCUP2[thread.NOCCUP2[i] - 1];

		Sur.Atom[i].AtomicNumber = get_element_number(variable.ELEMENT[thread.TS[i]]);
	}

	// Release ownership of the critical section.
	LeaveCriticalSection(&CriticalSection);
}

void Scene3D::ResetBonds(BondParam *D3DAtoms, int nr)
{

	for (int i = 0; i < nr; ++i){
		D3DAtoms[i].NrBondedAtoms = 0;
		ZeroMemory(D3DAtoms[i].BondedAtom, sizeof(int)*MAX_BONDED_ATOMS);
		ZeroMemory(D3DAtoms[i].AngleZ, sizeof(float)*MAX_BONDED_ATOMS);
		ZeroMemory(D3DAtoms[i].AngleXY, sizeof(float)*MAX_BONDED_ATOMS);
		ZeroMemory(D3DAtoms[i].Distance, sizeof(float)*MAX_BONDED_ATOMS);
	}
}

void Scene3D::ResetAtomParam(AtomParam *D3DAtoms, int nr){

	for (int i = 0; i < nr; ++i){
		D3DAtoms[i].x = D3DAtoms[i].y = D3DAtoms[i].z = 0.f;
		D3DAtoms[i].DW_in_plane = D3DAtoms[i].DW_out_of_plane = D3DAtoms[i].occupancy = 0.f;
		D3DAtoms[i].AtomicNumber = D3DAtoms[i].Selected = D3DAtoms[i].Z_Depth = D3DAtoms[i].AtomInTheFile = D3DAtoms[i].display_mode = 0;
	}
}

void Scene3D::Update3DModel(bool reset)
{

	//Update 3D model
	Update3Datoms();	//thread safe

	if (!reset)
		CopyAtomsStyle();
	else
	{
		nr_bulk_atoms_toplot = abs(g_BRepetitionX) * abs(g_BRepetitionY) * (1 + abs(g_BRepetitionZ))*Bul.nr_of_atoms;
		nr_surf_atoms_toplot = abs(g_SRepetitionX) * abs(g_SRepetitionY) * Fit.nr_of_atoms;

		if (reset)
		{
			if (D3DbulkAtom) delete[] D3DbulkAtom;
			D3DbulkAtom = new AtomParam[nr_bulk_atoms_toplot];
			ResetAtomParam(D3DbulkAtom, nr_bulk_atoms_toplot);

			if (D3DsurfAtom) delete[] D3DsurfAtom;
			D3DsurfAtom = new AtomParam[nr_surf_atoms_toplot];
			ResetAtomParam(D3DsurfAtom, nr_surf_atoms_toplot);

			if (D3DcompAtom) delete[] D3DcompAtom;
			D3DcompAtom = new AtomParam[nr_surf_atoms_toplot];
			ResetAtomParam(D3DcompAtom, nr_surf_atoms_toplot);

			if (D3DbulkBonds) delete[] D3DbulkBonds;
			D3DbulkBonds = new BondParam[nr_bulk_atoms_toplot];
			ResetBonds(D3DbulkBonds, nr_bulk_atoms_toplot);

			if (D3DsurfBonds) delete[] D3DsurfBonds;
			D3DsurfBonds = new BondParam[nr_surf_atoms_toplot];
			ResetBonds(D3DsurfBonds, nr_surf_atoms_toplot);

			if (D3DcompBonds) delete[] D3DcompBonds;
			D3DcompBonds = new BondParam[nr_surf_atoms_toplot];
			ResetBonds(D3DcompBonds, nr_surf_atoms_toplot);
		}
	}

	nr_bulk_atoms_toplot = Calculate_Atoms3D(D3DbulkAtom, Bul, flag, g_BRepetitionX, g_BRepetitionY, -g_BRepetitionZ);
	nr_surf_atoms_toplot = Calculate_Atoms3D(D3DsurfAtom, Fit, flag, g_SRepetitionX, g_SRepetitionY, 1);
	Calculate_Atoms3D(D3DcompAtom, Sur, flag, g_SRepetitionX, g_SRepetitionY, 1);

	Calculate_Bonds3D(D3DbulkBonds, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
	Calculate_Bonds3D(D3DsurfBonds, D3DsurfAtom, nr_surf_atoms_toplot, D3DbulkAtom, nr_bulk_atoms_toplot);
	Calculate_Bonds3D(D3DcompBonds, D3DcompAtom, nr_surf_atoms_toplot, D3DbulkAtom, nr_bulk_atoms_toplot);

	if (!reset)
		RestoreAtomsStyle();

	CenterAtoms3D(D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, D3DcompAtom, nr_surf_atoms_toplot);
	myScene.UpdateCamera();

}

void Scene3D::CopyAtomsStyle()
{

	if (display_modeBulk != NULL) delete[] display_modeBulk;
	if (display_modeSur != NULL) delete[] display_modeSur;
	if (display_modeComp != NULL) delete[] display_modeComp;

	display_modeBulk = new int[nr_bulk_atoms_toplot];
	display_modeSur = new int[nr_surf_atoms_toplot];
	display_modeComp = new int[nr_surf_atoms_toplot];

	for (int i = 0; i<nr_bulk_atoms_toplot; ++i){
		display_modeBulk[i] = D3DbulkAtom[i].display_mode;
	}
	for (int i = 0; i<nr_surf_atoms_toplot; ++i){
		display_modeSur[i] = D3DsurfAtom[i].display_mode;
		display_modeComp[i] = D3DcompAtom[i].display_mode;
	}


}

void Scene3D::RestoreAtomsStyle()
{
	for (int i = 0; i<nr_bulk_atoms_toplot; ++i){
		D3DbulkAtom[i].display_mode = display_modeBulk[i];
	}
	for (int i = 0; i<nr_surf_atoms_toplot; ++i)
	{
			D3DsurfAtom[i].display_mode = display_modeSur[i];
			D3DcompAtom[i].display_mode = display_modeComp[i];
	}
}

void Scene3D::ClearModel()
{
	Bul.nr_of_atoms = 0;
	Sur.nr_of_atoms = 0;
	Fit.nr_of_atoms = 0;
}

void Scene3D::DeleteObiects(){
	if (D3DbulkAtom) delete[] D3DbulkAtom;
	if (D3DsurfAtom) delete[] D3DsurfAtom;
	if (D3DcompAtom) delete[] D3DcompAtom;
	if (D3DbulkBonds) delete[] D3DbulkBonds;
	if (D3DsurfBonds) delete[] D3DsurfBonds;
	if (D3DcompBonds) delete[] D3DcompBonds;

	if (display_modeBulk != NULL) delete[] display_modeBulk;
	if (display_modeSur != NULL) delete[] display_modeSur;
	if (display_modeComp != NULL) delete[] display_modeComp;
}

//--------------------------------------------------------------------------------------------------------------------------------
//
//  "Drawing bulk" calculate cartesian coordinates of atoms and write them to structure  
//
//--------------------------------------------------------------------------------------------------------------------------------
int Scene3D::Calculate_Atoms3D(AtomParam *OutputData, ModelStruct InputData, FLAG flag, int repeatx, int repeaty, int repeatz)  // Drowing the xy plane cartesian coord.
{
	int n;
	int repeatx_, repeaty_, repeatz_;
	int repeatz_start, repeatz_end;
	int repeatx_start, repeatx_end;
	int repeaty_start, repeaty_end;

	if (repeatx>0) {
		repeatx_start = 0;
		repeatx_end = repeatx;
	}
	else{
		repeatx_start = repeatx + 1;
		repeatx_end = 1;
	}

	if (repeaty>0) {
		repeaty_start = 0;
		repeaty_end = repeaty;
	}
	else{
		repeaty_start = repeaty + 1;
		repeaty_end = 1;
	}

	if (repeatz>0) {
		repeatz_start = 0;
		repeatz_end = repeatz;
	}
	else{
		repeatz_start = repeatz;
		repeatz_end = 1;
	}


	int nr_atoms_toplot = 0;
	for (repeatz_ = repeatz_start; repeatz_ < repeatz_end; ++repeatz_)
	{
		for (repeaty_ = repeaty_start; repeaty_ < repeaty_end; ++repeaty_)
		{
			for (repeatx_ = repeatx_start; repeatx_ < repeatx_end; ++repeatx_)
			{
				for (n = 0; n < InputData.nr_of_atoms; ++n)
				{ // Ploting all atoms in the cell

					if (flag.g_liquid_profile && n == 0)
					{
						OutputData[nr_atoms_toplot].x = 0;
						OutputData[nr_atoms_toplot].y = 0;
						OutputData[nr_atoms_toplot].z = 10000;
						OutputData[nr_atoms_toplot].AtomicNumber = 0;//do not show
						OutputData[nr_atoms_toplot].DW_in_plane = 0;
						OutputData[nr_atoms_toplot].DW_out_of_plane = 0;
						OutputData[nr_atoms_toplot].occupancy = 0;
						OutputData[nr_atoms_toplot].display_mode = 100; //it is a liquid
						OutputData[nr_atoms_toplot].charge = 0;
					}
					else
					{
						OutputData[nr_atoms_toplot].x = (float)((InputData.Atom[n].x + repeatx_) * variable.a_sin_alp + (InputData.Atom[n].y + repeaty_) * variable.b_cos_gam + (InputData.Atom[n].z + repeatz_) * variable.c_cos_bet__sin_alp);
						OutputData[nr_atoms_toplot].y = (float)((InputData.Atom[n].y + repeaty_) *  variable.b_sin_Gam + (InputData.Atom[n].z + repeatz_) * variable.c_cos_alp);
						OutputData[nr_atoms_toplot].z = (float)((InputData.Atom[n].z + repeatz_) * variable.c_sin_bet__sin_alp);




						memcpy(OutputData[nr_atoms_toplot].Name, InputData.Atom[n].Name, sizeof(InputData.Atom[n].Name));
						OutputData[nr_atoms_toplot].charge = InputData.Atom[n].charge;
						OutputData[nr_atoms_toplot].AtomicNumber = InputData.Atom[n].AtomicNumber;
						OutputData[nr_atoms_toplot].DW_in_plane = (float)InputData.Atom[n].DW_in_plane;
						OutputData[nr_atoms_toplot].DW_out_of_plane = (float)InputData.Atom[n].DW_out_of_plane;						
						OutputData[nr_atoms_toplot].occupancy = (float)InputData.Atom[n].occupancy;
						OutputData[nr_atoms_toplot].AtomInTheFile = n;
						OutputData[nr_atoms_toplot].display_mode = flag.display_mode;
					}

					nr_atoms_toplot++;
					//if (nr_atoms_toplot >= MAX_ATOMS_3D) return MAX_ATOMS_3D;
				}
			}
		}
	}

	return nr_atoms_toplot;
}

//--------------------------------------------------------------------------------------------------------------------------------
//
//  //Find bonds and write results to structure  
//
//--------------------------------------------------------------------------------------------------------------------------------
void Scene3D::Calculate_Bonds3D(BondParam *OutputData1, AtomParam *InputData1, int nr_atoms_toplot1, AtomParam *InputData2, int nr_atoms_toplot2) { // Drowing the xy plane cartesian coord.

	int n;
	float distance, xixj, yiyj, zizj;
	D3DXMATRIX m_Temp;

	for (int i = 0; i < nr_atoms_toplot1; ++i)
	{
		n = 0;
		for (int j = 0; j < nr_atoms_toplot1; ++j)
		{
			if (i != j)
			{
				xixj = InputData1[i].x - InputData1[j].x;
				yiyj = InputData1[i].y - InputData1[j].y;
				zizj = InputData1[i].z - InputData1[j].z;
				distance = sqrtf(sqr(xixj) + sqr(yiyj) + sqr(zizj));
				if (distance < (AtProp.AtomRadius[InputData1[i].AtomicNumber] + AtProp.AtomRadius[InputData1[j].AtomicNumber]) * BOND_FACTOR)
				{
					OutputData1[i].Distance[n] = distance * STICK_CORRECTION;
					OutputData1[i].BondedAtom[n] = j;
					if (zizj == 0)
						OutputData1[i].AngleZ[n] = 0;
					else
						OutputData1[i].AngleZ[n] = (float)(asin(zizj / distance));

					if (xixj == 0.)
					{
						OutputData1[i].AngleXY[n] = -(float)(90.*deg2rad)*sng(yiyj);
					}
					else
						OutputData1[i].AngleXY[n] = -(float)(180.*deg2rad - atan2(yiyj, xixj));

					//------------------
					D3DXMatrixRotationY(&OutputData1[i].mRotate[n], OutputData1[i].AngleZ[n]);					
					D3DXMatrixRotationZ(&m_Temp, OutputData1[i].AngleXY[n]);
					OutputData1[i].mRotate[n] *= m_Temp;

					n++;
					if (n >= MAX_BONDED_ATOMS) break;
				}
			}
		}
		OutputData1[i].NrBondedAtoms = n;
	}

	// find bonding between 1 and 2 
	for (int i = 0; i < nr_atoms_toplot1; ++i)
	{
		n = OutputData1[i].NrBondedAtoms;
		for (int j = 0; j < nr_atoms_toplot2; ++j)
		{
			if (i != j)
			{
				xixj = InputData1[i].x - InputData2[j].x;
				yiyj = InputData1[i].y - InputData2[j].y;
				zizj = InputData1[i].z - InputData2[j].z;
				distance = sqrtf(sqr(xixj) + sqr(yiyj) + sqr(zizj));
				if (distance < (AtProp.AtomRadius[InputData1[i].AtomicNumber] + AtProp.AtomRadius[InputData2[j].AtomicNumber])*BOND_FACTOR)
				{
					OutputData1[i].Distance[n] = distance * STICK_CORRECTION;
					OutputData1[i].BondedAtom[n] = 0;//it is a different atom set
					OutputData1[i].AngleZ[n] = (float)(asin(zizj / (distance + 1e-9)));

					if (xixj == 0.)
					{
						OutputData1[i].AngleXY[n] = -(float)(90.*deg2rad)*sng(yiyj);
					}
					else
						OutputData1[i].AngleXY[n] = -(float)(180.*deg2rad - atan2(yiyj, xixj));

					//------------------
					D3DXMatrixRotationY(&OutputData1[i].mRotate[n], OutputData1[i].AngleZ[n]);
					D3DXMatrixRotationZ(&m_Temp, OutputData1[i].AngleXY[n]);
					OutputData1[i].mRotate[n] *= m_Temp;

					n++;
					if (n >= MAX_BONDED_ATOMS) break;
				}
			}
		}
		OutputData1[i].NrBondedAtoms = n;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------
bool Scene3D::CenterAtoms3D(AtomParam *D3DbulkAtom, int nr_bulk_atoms_toplot, AtomParam *D3DsurfAtom, AtomParam *D3DcompAtom, int nr_surf_atoms_toplot){
	//--------------------------------------------------------------------------------------------------------------------------------
	int i;
	

	if (nr_surf_atoms_toplot == 0 && nr_bulk_atoms_toplot == 0) return false;

	if (nr_bulk_atoms_toplot != 0){
		//Center bulk x y coordinate
		for (i = 0; i < nr_bulk_atoms_toplot; ++i)
		{
			shift_x += (float)D3DbulkAtom[i].x;
			shift_y += (float)D3DbulkAtom[i].y;
		}
	}

	if (nr_surf_atoms_toplot != 0){
		//Center surface x coordinate
		for (i = 0; i < nr_surf_atoms_toplot; ++i)
		{
			shift_x += (float)D3DsurfAtom[i].x;
			shift_y += (float)D3DsurfAtom[i].y;
		}
	}

	shift_x /= (nr_bulk_atoms_toplot + nr_surf_atoms_toplot);
	shift_y /= (nr_bulk_atoms_toplot + nr_surf_atoms_toplot);

	//center bulk x y
	for (i = 0; i < nr_bulk_atoms_toplot; ++i)
	{
		D3DbulkAtom[i].x -= shift_x;
		D3DbulkAtom[i].y -= shift_y;
		D3DXMatrixTranslation(&D3DbulkAtom[i].mShift, D3DbulkAtom[i].x, D3DbulkAtom[i].y, D3DbulkAtom[i].z);
	}
	//center surface x y
	for (i = 0; i < nr_surf_atoms_toplot; ++i)
	{
		D3DsurfAtom[i].x -= shift_x;
		D3DsurfAtom[i].y -= shift_y;
		D3DXMatrixTranslation(&D3DsurfAtom[i].mShift, D3DsurfAtom[i].x, D3DsurfAtom[i].y, D3DsurfAtom[i].z);
	}
	//...........................................................

	//Center comp (compare) atoms
	//center surface x
	for (i = 0; i < nr_surf_atoms_toplot; ++i)
	{
		D3DcompAtom[i].x -= shift_x;
		D3DcompAtom[i].y -= shift_y;
		D3DXMatrixTranslation(&D3DcompAtom[i].mShift, D3DcompAtom[i].x, D3DcompAtom[i].y, D3DcompAtom[i].z);
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
//Purpose: This rutine is important in transparecny mode in 3D
//Created by: Daniel Kaminski
//01.02.02013 Lublin
//--------------------------------------------------------------------------------------------------------------------------------
void Scene3D::Order_Atoms3D(D3DXMATRIX RotateViewnProjMatrix, AtomParam *D3DAtom, int nr_atoms_toplot) {
	//--------------------------------------------------------------------------------------------------------------------------------
	D3DXVECTOR3 v_local;
	D3DXVECTOR3 v_real;

	float last_z;
	int iterate = 0;

	bool *atomCheck = new bool[nr_atoms_toplot];

	ZeroMemory(atomCheck, sizeof(bool)*nr_atoms_toplot);

	for (int m = 0; m < nr_atoms_toplot; ++m)
	{
		last_z = -10000000.0;
		for (int n = 0; n < nr_atoms_toplot; ++n)
		{
			v_local.x = D3DAtom[n].x;
			v_local.y = D3DAtom[n].y;
			v_local.z = D3DAtom[n].z;
			D3DXVec3TransformCoord(&v_real, &v_local, &RotateViewnProjMatrix);

			if (!atomCheck[n] && v_real.z >= last_z)
			{
				last_z = v_real.z;
				iterate = n;
			}
		}
		D3DAtom[m].Z_Depth = iterate;
		atomCheck[iterate] = true;
	}

	delete[] atomCheck;
}

void Scene3D::ApplyColors()
{
	g_LightColor[0] = (float)(1.0 / 255 * GetRValue(myScene.flag.rgbLight));
	g_LightColor[1] = (float)(1.0 / 255 * GetGValue(myScene.flag.rgbLight));
	g_LightColor[2] = (float)(1.0 / 255 * GetBValue(myScene.flag.rgbLight));
	g_LightColor[3] = 1.0f;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Plot scene functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Functions: Prepare 3D scene
//21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Scene3D::Scene()
{

	static UINT totalPasses;

	D3DXMATRIX worldViewProjectionMatrix;
	D3DXMATRIX RotateViewProjectionMatrix;

	D3DXMATRIX world, m_temp;
	float ObjectScale, ObjectScale_z;
	D3DXVECTOR3 vPos;

	if (myScene.flag.g_light_follows_cam)
	{
		vPos = myScene.g_LightPosition;
	}
	else
	{
		D3DXVec3TransformCoord(&vPos, &myScene.g_LightPosition, &myScene.rotate);
	}

	float v_Tmp[4] = { -vPos.x, vPos.y, vPos.z, 30 };
	

	myScene.g_pEffect->SetValue("g_LightColor", &g_LightColor, sizeof(float) * 4);
	myScene.g_pEffect->SetValue("SpecularColor", &g_LightColor, sizeof(float) * 4);
	
	myScene.g_pEffect->SetValue("g_Shininess", &flag.g_specular, sizeof(float));
	myScene.g_pEffect->SetValue("g_SpecularIntensity", &flag.g_specular_intensty, sizeof(float));

	// Set the scene global ambient term.
	float g_sceneAmbient[4] = { 0.24f, 0.24f, 0.24f, 1.0f };
	myScene.g_pEffect->SetValue("g_LightAmbient", &g_sceneAmbient, sizeof(g_sceneAmbient));

	// Set the camera position.
	RotateViewProjectionMatrix = myScene.rotate * myScene.view * myScene.proj;
	myScene.g_pEffect->SetValue("ObsPosition", myScene.g_cameraPos, sizeof(myScene.g_cameraPos));
	myScene.g_pEffect->SetTechnique("NoBlend");

	// Set the lighting parameters for the shader.;
	myScene.g_pEffect->SetValue("LightPosition", &v_Tmp, sizeof(float) * 4);
	myScene.g_pEffect->SetMatrix("g_mRotate", &myScene.rotate);
	
	//-------------------------------------------------------------------------------------------------------------
	//Cell
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_cell)
	{
		if (Bul.nr_of_atoms != 0 || Sur.nr_of_atoms != 0)
			DrawCell(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix);
		else
		{
			RECT rcClient;
			rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2);
			rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2);
			rcClient.left -= 80;
			rcClient.top -= 70;
			rcClient.right = rcClient.left + 180 + myScene.flag.g_text_size; rcClient.bottom = rcClient.top + myScene.flag.g_text_size + 10;
			g_pFont->DrawText(0, L"No coordinates available", -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 255, 0));
		}
	}
	//-------------------------------------------------------------------------------------------------------------
	//Measurements
	//-------------------------------------------------------------------------------------------------------------

	DrawMeasurement(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix);

	//-------------------------------------------------------------------------------------------------------------
	//Bulk bonds
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_bulk)
		DrawBulkBonds(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale, &ObjectScale_z);

	//-------------------------------------------------------------------------------------------------------------
	//Bulk
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_bulk)
		DrawBulkAtoms(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale);
	//-------------------------------------------------------------------------------------------------------------

	//Select technique
	myScene.g_pEffect->SetTechnique("Blend");
	//-------------------------------------------------------------------------------------------------------------
	//Surface bonds
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_surface)
		DrawSurfaceBonds(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale, &ObjectScale_z);
	
	//-------------------------------------------------------------------------------------------------------------
	//Surface bonds compare
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_surface & myScene.flag.g_compare)//
		DrawSurfaceBondsCompare(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale, &ObjectScale_z);


	//Select technique
	if(myScene.flag.g_texture)
		myScene.g_pEffect->SetTechnique("BlendWithTexture");
	else
		myScene.g_pEffect->SetTechnique("Blend");
	//-------------------------------------------------------------------------------------------------------------
	//Surface atoms
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_surface)
		DrawSurfaceAtoms(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale, &ObjectScale_z);
	
	myScene.g_pEffect->SetTechnique("Blend");
	//-------------------------------------------------------------------------------------------------------------
	//Compare atomic positions before and after fitting
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_surface & myScene.flag.g_compare)
		DrawSurfaceAtomsCompare(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale, &ObjectScale_z);

	//-------------------------------------------------------------------------------------------------------------
	//Liquid layers
	//-------------------------------------------------------------------------------------------------------------
	if (flag.g_liquid_profile)
		DrawLiquidLayers(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale, &ObjectScale_z);

	myScene.g_pEffect->SetTechnique("NoBlend");

	//-------------------------------------------------------------------------------------------------------------
	//Thermal Elipsoids for surface
	//-------------------------------------------------------------------------------------------------------------
	if (!myScene.scene_rotate & myScene.flag.g_termal_rings & myScene.flag.g_surface)
		DrawThermalElipsoids(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses, &ObjectScale, &ObjectScale_z);


	myScene.g_pEffect->SetTechnique("Blend");
	//-------------------------------------------------------------------------------------------------------------
	//Plane
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_planeOnOff)
		DrawPlane(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses);


	//-------------------------------------------------------------------------------------------------------------
	//Lasso
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.mouse_lasso)
		DrawLasso(&RotateViewProjectionMatrix);

	//-------------------------------------------------------------------------------------------------------------
	//Charges scale
	//-------------------------------------------------------------------------------------------------------------
	if (myScene.flag.g_individual_color)
	{
		//Select technique
		myScene.g_pEffect->SetTechnique("Simple");
		DrawCharges(&world, &RotateViewProjectionMatrix, &worldViewProjectionMatrix, &totalPasses);


	}
	myScene.RenderText();

	//end draw 3D
}


D3DXVECTOR3 Scene3D::CalculatePoint3D(float x, float y, float z)
{
	D3DXVECTOR3 V;

	V.x = x * a_sin_alp+y* b_cos_gam+z* c_cos_bet__sin_alp;
	V.y = y * b_sin_Gam + z * c_cos_alp;
	V.z = z * c_sin_bet__sin_alp;
	return V;
}

inline void Scene3D::DrawCell(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix)
{
	WCHAR str[10];
	RECT rcClient;
	D3DXVECTOR3 VertexList[16];


	//1
	VertexList[0] = CalculatePoint3D(0, 0, 0);
	//2
	VertexList[1] = CalculatePoint3D(1, 0, 0);
	//3
	VertexList[2] = CalculatePoint3D(1, 1, 0);
	//4
	VertexList[3] = CalculatePoint3D(0, 1, 0);
	//1
	VertexList[4] = CalculatePoint3D(0, 0, 0);

	//1
	VertexList[5] = CalculatePoint3D(0, 0, 1);
	//2
	VertexList[6] = CalculatePoint3D(1, 0, 1);
	//3
	VertexList[7] = CalculatePoint3D(1, 1, 1);
	//4
	VertexList[8] = CalculatePoint3D(0, 1, 1);
	//1
	VertexList[9] = CalculatePoint3D(0, 0, 1);

	for (int i = 0; i < 10; i++)
	{
		VertexList[i].x -= shift_x;
		VertexList[i].y -= shift_y;
	}

	//2 up
	VertexList[10] = VertexList[6];
	//2 down
	VertexList[11] = VertexList[1];
	//3 down
	VertexList[12] = VertexList[2];
	//3 up
	VertexList[13] = VertexList[7];
	//4 up
	VertexList[14] = VertexList[8];
	//4 down
	VertexList[15] = VertexList[3];


	// Draw the line.
	myScene._line->SetWidth(2.0f);
	myScene._line->Begin();
	myScene._line->DrawTransform(VertexList, 16, RotateViewProjectionMatrix, D3DXCOLOR(0.5f, 0.5f, 0.1f, 1.0f));
	myScene._line->End();


	//////////////////////////////////////// Draw x vector////////////////////////////////////////////////////////


	myScene._line->SetWidth(2.0f);
	myScene._line->Begin();
	myScene._line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.9f, 0.1f, 0.1f, 1.0f));
	myScene._line->End();

	swprintf_s(str, 20, L"x");

	D3DXMatrixTranslation(world, VertexList[1].x, VertexList[1].y, VertexList[1].z);
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

	rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
	rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
	rcClient.left -= 12;
	rcClient.top -= 20;
	rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
	g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 0, 0));


	//////////////////////////////////////// Draw y vector////////////////////////////////////////////////////////
	VertexList[1] = VertexList[3];



	myScene._line->SetWidth(2.0f);
	myScene._line->Begin();
	myScene._line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.9f, 0.9f, 0.1f, 1.0f));
	myScene._line->End();

	swprintf_s(str, 20, L"y");

	D3DXMatrixTranslation(world, VertexList[1].x, VertexList[1].y, VertexList[1].z);
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

	rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
	rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
	rcClient.left -= 12;
	rcClient.top -= 20;
	rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
	g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 255, 0));


	//////////////////////////////////////// Draw z vector////////////////////////////////////////////////////////

	//2
	VertexList[1] = VertexList[5];


	myScene._line->SetWidth(2.0f);
	myScene._line->Begin();
	myScene._line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.9f, 0.1f, 1.0f));
	myScene._line->End();

	swprintf_s(str, 20, L"z");

	D3DXMatrixTranslation(world, VertexList[1].x, VertexList[1].y, VertexList[1].z);
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

	rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
	rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
	rcClient.left -= 12;
	rcClient.top -= 20;
	rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
	g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));

}

inline void Scene3D::DrawMeasurement(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix)
{
	WCHAR str[10];
	RECT rcClient;
	D3DXVECTOR3 VertexList[6];

	for (int i = 0; i < myScene.IndexMeasure; ++i)
	{
		if (myScene.measurement[i].measurement == 1){ //distance

			VertexList[0].x = (float)myScene.measurement[i].Atom1.x;
			VertexList[0].y = (float)myScene.measurement[i].Atom1.y;
			VertexList[0].z = (float)myScene.measurement[i].Atom1.z;

			VertexList[1].x = (float)myScene.measurement[i].Atom2.x;
			VertexList[1].y = (float)myScene.measurement[i].Atom2.y;
			VertexList[1].z = (float)myScene.measurement[i].Atom2.z;

			// Draw the line.
			myScene._line->SetWidth(3.0f);
			myScene._line->Begin();
			myScene._line->DrawTransform(VertexList, 2, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.5f, 0.1f, 1.0f));
			myScene._line->End();
			//draw myScene.measurement
			swprintf_s(str, 20, L"%4.3f\305", myScene.measurement[i].value);

			D3DXMatrixTranslation(world, (float)(myScene.measurement[i].Atom1.x + myScene.measurement[i].Atom2.x) / 2, (float)(myScene.measurement[i].Atom1.y + myScene.measurement[i].Atom2.y) / 2, (float)(myScene.measurement[i].Atom1.z + myScene.measurement[i].Atom2.z) / 2);
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

			rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
			rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
			rcClient.left -= 12;
			rcClient.top -= 20;
			rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
			g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));
		}

		if (myScene.measurement[i].measurement == 2){ //angle

			VertexList[0].x = (float)myScene.measurement[i].Atom1.x;
			VertexList[0].y = (float)myScene.measurement[i].Atom1.y;
			VertexList[0].z = (float)myScene.measurement[i].Atom1.z;

			VertexList[1].x = (float)myScene.measurement[i].Atom2.x;
			VertexList[1].y = (float)myScene.measurement[i].Atom2.y;
			VertexList[1].z = (float)myScene.measurement[i].Atom2.z;

			VertexList[2].x = (float)myScene.measurement[i].Atom3.x;
			VertexList[2].y = (float)myScene.measurement[i].Atom3.y;
			VertexList[2].z = (float)myScene.measurement[i].Atom3.z;

			// Draw the line.
			myScene._line->SetWidth(3.0f);
			myScene._line->Begin();
			myScene._line->DrawTransform(VertexList, 3, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.5f, 0.1f, 1.0f));
			myScene._line->End();

			//draw myScene.measurement
			swprintf_s(str, 20, L"%4.2f\272", myScene.measurement[i].value);

			D3DXMatrixTranslation(world, (float)(myScene.measurement[i].Atom1.x + myScene.measurement[i].Atom2.x + myScene.measurement[i].Atom3.x) / 3, (float)(myScene.measurement[i].Atom1.y + myScene.measurement[i].Atom2.y + myScene.measurement[i].Atom3.y) / 3, (float)(myScene.measurement[i].Atom1.z + myScene.measurement[i].Atom2.z + myScene.measurement[i].Atom3.z) / 3);

			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

			rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
			rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
			rcClient.left -= 12;
			rcClient.top -= 20;
			rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
			g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));
		}

		if (myScene.measurement[i].measurement == 3){ //dihedral

			VertexList[0].x = (float)myScene.measurement[i].Atom3.x;
			VertexList[0].y = (float)myScene.measurement[i].Atom3.y;
			VertexList[0].z = (float)myScene.measurement[i].Atom3.z;

			VertexList[1].x = (float)myScene.measurement[i].Atom2.x;
			VertexList[1].y = (float)myScene.measurement[i].Atom2.y;
			VertexList[1].z = (float)myScene.measurement[i].Atom2.z;

			VertexList[2].x = (float)myScene.measurement[i].Atom1.x;
			VertexList[2].y = (float)myScene.measurement[i].Atom1.y;
			VertexList[2].z = (float)myScene.measurement[i].Atom1.z;

			VertexList[3].x = (float)myScene.measurement[i].Atom3.x;
			VertexList[3].y = (float)myScene.measurement[i].Atom3.y;
			VertexList[3].z = (float)myScene.measurement[i].Atom3.z;

			VertexList[4].x = (float)myScene.measurement[i].Atom4.x;
			VertexList[4].y = (float)myScene.measurement[i].Atom4.y;
			VertexList[4].z = (float)myScene.measurement[i].Atom4.z;

			VertexList[5].x = (float)myScene.measurement[i].Atom2.x;
			VertexList[5].y = (float)myScene.measurement[i].Atom2.y;
			VertexList[5].z = (float)myScene.measurement[i].Atom2.z;

			// Draw the line.
			myScene._line->SetWidth(3.0f);
			myScene._line->Begin();
			myScene._line->DrawTransform(VertexList, 6, RotateViewProjectionMatrix, D3DXCOLOR(0.1f, 0.5f, 0.1f, 1.0f));
			myScene._line->End();

			//draw myScene.measurement
			swprintf_s(str, 20, L"%4.2f\272", myScene.measurement[i].value);

			D3DXMatrixTranslation(world, (float)(myScene.measurement[i].Atom1.x + myScene.measurement[i].Atom2.x + myScene.measurement[i].Atom3.x + myScene.measurement[i].Atom4.x) / 4, (float)(myScene.measurement[i].Atom1.y + myScene.measurement[i].Atom2.y + myScene.measurement[i].Atom3.y + myScene.measurement[i].Atom4.y) / 4, (float)(myScene.measurement[i].Atom1.z + myScene.measurement[i].Atom2.z + myScene.measurement[i].Atom3.z + myScene.measurement[i].Atom4.z) / 4);
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;

			rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
			rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));
			rcClient.left -= 12;
			rcClient.top -= 20;
			rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + 20;
			g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));
		}
	}
}

inline void Scene3D::DrawPlane(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses)
{
	HRESULT hr;

	float g_plane[4] = { 0.5f, 0.1f, 0.9f, 0.1f };
	myScene.g_pEffect->SetValue("g_ObiectColor", g_plane, sizeof(float) * 4);

	float ObjectScale = 1.f;
	D3DXMATRIXA16 mTemp;
	D3DXMatrixScaling(world, ObjectScale, ObjectScale, ObjectScale);
	D3DXMatrixRotationX(&mTemp, (float)(90.*deg2rad));
	*world *= mTemp;
	// Set the matrices for the shader.
	myScene.g_pEffect->SetMatrix("g_mWorld", world);

	// Calculate combined world-view-projection matrix.
	*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
	myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

	// Render.
	DWORD l;
	if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0))){
		for (UINT pass = 0; pass < *totalPasses; ++pass){
			if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass))){
				for (l = 0; l < g_Orbiter[1].m_dwNumMaterials; ++l){
					//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
					// Initialize required parameter
					V(myScene.g_pEffect->CommitChanges());
					// Render the mesh with the applied technique
					V(g_Orbiter[1].m_Mesh->DrawSubset(l));
				}
				myScene.g_pEffect->EndPass();
			}
		}
		myScene.g_pEffect->End();
	}
}

inline void Scene3D::DrawCharges(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses)
{
	HRESULT hr;

	float g_color[4];
	float charge = -1.5f;
	for (int i = 0; i < 14; i++)
	{
		charge += 0.2f;
		g_color[3] = 1;	g_color[2] = 0;	g_color[1] = 1;	g_color[0] = 0;
		if (charge >= 0)
		{
			g_color[0] = charge;//Czerwony
			g_color[1] = 1 - charge * myScene.flag.g_scale_color;//Czerwony
		}
		else
		{
			g_color[2] = -charge;//Niebieski
			g_color[1] = 1 + charge * myScene.flag.g_scale_color;//Czerwony
		}


		myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);


		D3DXMATRIXA16 mTemp;
		D3DXMatrixScaling(world, .3f, .3f, .3f);

		D3DXMATRIX mShift;
		D3DXMatrixTranslation(&mShift, -18, 2.f*i-10, 0);

		// Set the matrices for the shader.
		myScene.g_pEffect->SetMatrix("g_mWorld", world);
		*world = *world*mShift;

		// Calculate combined world-view-projection matrix.
		D3DXMATRIX l_view;
		D3DXVECTOR3 LookUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 cameraPos = D3DXVECTOR3(0.0f, 0.0f, 40.0f);
		D3DXVECTOR3 targetPos = D3DXVECTOR3(0.0f, .0f, 0.0f);
		D3DXMatrixLookAtLH(&l_view, &cameraPos,	&targetPos, &LookUp);

		*worldViewProjectionMatrix = *world * l_view * myScene.proj;
		myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

		// Render.
		DWORD l;
		if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0))) {
			for (UINT pass = 0; pass < *totalPasses; ++pass) {
				if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass))) {
					for (l = 0; l < g_Orbiter[0].m_dwNumMaterials; ++l) {
						//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
						// Initialize required parameter
						V(myScene.g_pEffect->CommitChanges());
						// Render the mesh with the applied technique
						V(g_Orbiter[0].m_Mesh->DrawSubset(l));
					}
					myScene.g_pEffect->EndPass();
				}
			}
			myScene.g_pEffect->End();
		}

		WCHAR str[11];
		static RECT rcClient;
		if(charge<1.2)
			swprintf_s(str, 10, L"%4.2f", charge);
		else
			swprintf_s(str, 10, L"%4.2f<", charge);
		rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
		rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));

		rcClient.left -= 10;
		rcClient.top -= 12;
		rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + myScene.flag.g_text_size + 10;
		g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(GetRValue(myScene.flag.rgbText), GetGValue(myScene.flag.rgbText), GetBValue(myScene.flag.rgbText)));


	}
}


inline void Scene3D::DrawBulkBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;
	UINT pass;
	DWORD l;
	int mesh;
	int Ordered_z;
	int i, j;

	for (i = 0; i < nr_bulk_atoms_toplot; ++i)
	{
		Ordered_z = i;//D3DbulkAtom[i].Z_Depth;
		*ObjectScale = myScene.flag.RodThickness;

		//Show selected atoms atoms
		if (D3DbulkAtom[Ordered_z].Selected > 0){
			g_color[0] = 1.f; g_color[1] = 1.f; g_color[2] = .1f; g_color[3] = 1.f;
		}
		else
		{
			if(!myScene.flag.g_individual_color)
			CopyMemory(g_color, AtProp.AtomColor[D3DbulkAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);
			else
			{
				g_color[3] = 1;	g_color[2] = 0;	g_color[1] = 1;	g_color[0] = 0;
				if (D3DbulkAtom[Ordered_z].charge >= 0)
				{
					g_color[0] = D3DbulkAtom[Ordered_z].charge;//Czerwony
					g_color[1] = 1 - D3DbulkAtom[Ordered_z].charge*myScene.flag.g_scale_color;//Czerwony
				}
				else
				{
					g_color[2] = -D3DbulkAtom[Ordered_z].charge;//Niebieski
					g_color[1] = 1 + D3DbulkAtom[Ordered_z].charge*myScene.flag.g_scale_color;//Czerwony
				}
			}
				if (myScene.flag.g_transparency)
				g_color[3] = 1;//D3DsurfAtom[Ordered_z].occupancy;
		}
		myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

		//for bonds
		for (j = 0; j < D3DbulkBonds[Ordered_z].NrBondedAtoms; ++j)
		{ 

			if (D3DbulkAtom[Ordered_z].AtomicNumber != 0 && (D3DbulkAtom[Ordered_z].display_mode == 2 || D3DbulkAtom[Ordered_z].display_mode == 3 || D3DbulkAtom[Ordered_z].display_mode == 4)){

				*ObjectScale_z = (float)(D3DbulkBonds[Ordered_z].Distance[j]);

				D3DXMatrixScaling(world, *ObjectScale_z, *ObjectScale, *ObjectScale);
				
				//First rotate the bond after that translate
				*world *= D3DbulkBonds[Ordered_z].mRotate[j] * D3DbulkAtom[Ordered_z].mShift;

				myScene.g_pEffect->SetMatrix("g_mWorld", world);
				
				// Calculate combined world-view-projection matrix.
				*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
				myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

				//-------------------------------------------------------------------------------------------------------------------------------
				
				// Render.
				if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0)))
				{
					if (myScene.scene_rotate)
						mesh = 4;
					else
					{
						if (myScene.flag.g_spring)
							mesh = 5;
						else
							mesh = 2;
					}

					for (pass = 0; pass < *totalPasses; ++pass){

						if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass))){
							for (l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l){
								//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
								// Initialize required parameter
								V(myScene.g_pEffect->CommitChanges());
								// Render the mesh with the applied technique
								V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
							}
							myScene.g_pEffect->EndPass();
						}
					}
					myScene.g_pEffect->End();
				}
			}//for bonds
		}//for if mode
	}//for elements
}

inline void Scene3D::DrawBulkAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale)
{
	HRESULT hr;
	WCHAR str[10];
	RECT rcClient;
	float g_color[4];
	D3DXMATRIX m_temp;
	int Ordered_z;
	UINT pass;
	DWORD l;

	Order_Atoms3D(*RotateViewProjectionMatrix, D3DbulkAtom, nr_bulk_atoms_toplot);
	for (int i = 0; i < nr_bulk_atoms_toplot; ++i)
	{
		Ordered_z = D3DbulkAtom[i].Z_Depth;
		if (D3DbulkAtom[Ordered_z].AtomicNumber != 0)
		{ //Don't draw not existing atoms simple delete trick

			//Show selected atoms atoms
			if (D3DbulkAtom[Ordered_z].Selected > 0)
			{
				g_color[0] = 1.f; g_color[1] = 1.f; g_color[2] = .1f; g_color[3] = 1.f;
			}
			else
			{
			if (!myScene.flag.g_individual_color)
					CopyMemory(g_color, AtProp.AtomColor[D3DbulkAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);
				else
				{
					g_color[3] = 1;	g_color[2] = 0;	g_color[1] = 0;	g_color[0] = 0;
					if (D3DbulkAtom[Ordered_z].charge >= 0)
					{
						g_color[0] = D3DbulkAtom[Ordered_z].charge;//Czerwony
						g_color[1] = 1 - D3DbulkAtom[Ordered_z].charge*myScene.flag.g_scale_color;//Czerwony
						g_color[2] = 0;
					}
					else
					{
						g_color[2] = -D3DbulkAtom[Ordered_z].charge;//Niebieski
						g_color[1] = 1 + D3DbulkAtom[Ordered_z].charge*myScene.flag.g_scale_color;//Czerwony
						g_color[0] = 0;
					}
				}


				if (myScene.flag.g_transparency)
					g_color[3] = 1;
			}

			myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

			//Elipsoids
			if (D3DbulkAtom[Ordered_z].display_mode == 1 || D3DbulkAtom[Ordered_z].display_mode == 4)
			{
				*ObjectScale = (float)(sqrt(D3DbulkAtom[Ordered_z].DW_in_plane) * myScene.flag.DWscale);
			}

			//VDM
			if (D3DbulkAtom[Ordered_z].display_mode == 0 || *ObjectScale == 0.)
			{
				*ObjectScale = (float)(AtProp.AtomRadius[D3DbulkAtom[Ordered_z].AtomicNumber] * PLOT_SCALE_FACTOR);
			}

			//Sticks
			if (D3DbulkAtom[Ordered_z].display_mode == 2)
			{
				*ObjectScale = 0.01f;
			}

			//Balls
			if (D3DbulkAtom[Ordered_z].display_mode == 3)
			{
				*ObjectScale = myScene.flag.BallSize;
			}

			D3DXMatrixScaling(world, *ObjectScale, *ObjectScale, *ObjectScale);

			*world *= D3DbulkAtom[Ordered_z].mShift;

			// Set the matrices for the shader.
			myScene.g_pEffect->SetMatrix("g_mWorld", world);
			// Calculate combined world-view-projection matrix.
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
			myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

			//-------------------------------------------------------------------------------------------------------------
			//Bulk atoms labels  && D3DbulkAtom[i].z>CAMERA_ZNEAR
			//-------------------------------------------------------------------------------------------------------------
			if (myScene.flag.plot_bulk_labels && (worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44 < myScene.Zfar && worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44>myScene.Znear || !myScene.mouse_Zcut))
			{
				if (!myScene.flag.g_individual_color)
					swprintf_s(str, 60, L"%s%d", W_elements[D3DbulkAtom[Ordered_z].AtomicNumber], D3DbulkAtom[Ordered_z].AtomInTheFile);
				else
					swprintf_s(str, 60, L"%s %4.2f",  D3DbulkAtom[Ordered_z].Name, D3DbulkAtom[Ordered_z].charge);//W_elements[D3DbulkAtom[Ordered_z].AtomicNumber]

				rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
				rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));

				rcClient.left -= 10;
				rcClient.top -= 10;
				rcClient.right = rcClient.left + 80; rcClient.bottom = rcClient.top + myScene.flag.g_text_size+10;
				g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(GetRValue(myScene.flag.rgbText), GetGValue(myScene.flag.rgbText), GetBValue(myScene.flag.rgbText)));
			}
			//-------------------------------------------------------------------------------------------------------------------------------
			//Pickup atom position and select
			//-------------------------------------------------------------------------------------------------------------------------------
			if (myScene.mouse_select)
			{
				myScene.ResetSelection(myScene.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
				myScene.MouseSelect(&myScene.ptMouseSelect, myScene.g_params.BackBufferWidth, myScene.g_params.BackBufferHeight, worldViewProjectionMatrix, D3DbulkAtom, Ordered_z, myScene.MenuHeight, myScene.flag.g_perspective);
			}

			//-------------------------------------------------------------------------------------------------------------------------------
			int mesh;
			// Render.
			if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0))){
				if (myScene.scene_rotate) mesh = 3;
				else mesh = 0;
				for (pass = 0; pass < *totalPasses; ++pass){
					if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass))){
						for (l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l){
							myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[0] ) ;
							// Initialize required parameter
							V(myScene.g_pEffect->CommitChanges());
							// Render the mesh with the applied technique
							V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
						}
						myScene.g_pEffect->EndPass();
					}
				}
				myScene.g_pEffect->End();
			}
		}//for elements
	}
}

inline void Scene3D::DrawSurfaceBonds(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;
	int Ordered_z;
	DWORD l;
	int mesh;
	UINT pass;

	*ObjectScale = myScene.flag.RodThickness;

	for (int i = 0; i < nr_surf_atoms_toplot; ++i)
	{
		Ordered_z = D3DsurfAtom[i].Z_Depth;
		if (D3DsurfAtom[Ordered_z].AtomicNumber != 0 && (D3DsurfAtom[Ordered_z].display_mode == 2 || D3DsurfAtom[Ordered_z].display_mode == 3 || D3DsurfAtom[Ordered_z].display_mode == 4))
		{
	
			if (myScene.flag.g_compare)
			{
				g_color[0] = 0.9f; g_color[1] = 0.1f; g_color[2] = 0.1f; g_color[3] = 1.0f;
			}
			else
				CopyMemory(g_color, AtProp.AtomColor[D3DsurfAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);

			if (myScene.flag.g_transparency)
				g_color[3] = D3DsurfAtom[Ordered_z].occupancy;
			else
				g_color[3] = 1.f;

			myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);


			for (int j = 0; j < D3DsurfBonds[Ordered_z].NrBondedAtoms; ++j)
			{ //for bonds
			
				*ObjectScale_z = D3DsurfBonds[Ordered_z].Distance[j];

				D3DXMatrixScaling(world, *ObjectScale_z, *ObjectScale, *ObjectScale);

				//First rotate the bond after that translate
				*world *= D3DsurfBonds[Ordered_z].mRotate[j] * D3DsurfAtom[Ordered_z].mShift;

				// Set the matrices for the shader.
				myScene.g_pEffect->SetMatrix("g_mWorld", world);

				// Calculate combined world-view-projection matrix.
				*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
				myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

				//Show selected atoms
				if (D3DsurfAtom[Ordered_z].Selected > 0)
				{
					float g_ColorSelected[4] = { 1., 1.0, 0.0, 1.0f };
					myScene.g_pEffect->SetValue("g_ObiectColor", g_ColorSelected, sizeof(float) * 4);
				}

				//-------------------------------------------------------------------------------------------------------------------------------
				
				// Render.
				if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0)))
				{
					if (myScene.scene_rotate)
						mesh = 4;
					else
					{
						if (myScene.flag.g_spring)
							mesh = 5;
						else
							mesh = 2;
					}
					for (pass = 0; pass < *totalPasses; ++pass)
					{
						if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass)))
						{
							for (l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
							{
								//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
								// Initialize required parameter
								V(myScene.g_pEffect->CommitChanges());
								// Render the mesh with the applied technique
								V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
							}
							myScene.g_pEffect->EndPass();
						}
					}
					myScene.g_pEffect->End();
				}
			}//for bonds
		}//if mode
	}//for elements
}

inline void Scene3D::DrawSurfaceBondsCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;

	*ObjectScale = myScene.flag.RodThickness;
	for (int i = 0; i < nr_surf_atoms_toplot; ++i)
	{
		int Ordered_z = D3DcompAtom[i].Z_Depth;
		if (D3DcompAtom[Ordered_z].AtomicNumber != 0 && (D3DcompAtom[Ordered_z].display_mode == 2 || D3DcompAtom[Ordered_z].display_mode == 3 || D3DcompAtom[Ordered_z].display_mode == 4))
		{
			for (int j = 0; j < D3DcompBonds[Ordered_z].NrBondedAtoms; j++){ //for bonds

				
				*ObjectScale_z = D3DsurfBonds[Ordered_z].Distance[j];

				D3DXMatrixScaling(world, *ObjectScale_z, *ObjectScale, *ObjectScale);

				//Show selected atoms atoms
				if (D3DcompAtom[Ordered_z].Selected > 0)
				{
					g_color[0] = 1.0f; g_color[1] = 1.0f; g_color[2] = 0.1f; g_color[3] = 1.0f;
				}
				else
				{
					g_color[0] = 0.1f; g_color[1] = 0.1f; g_color[2] = 0.9f; g_color[3] = 1.0f;
				
					if (myScene.flag.g_transparency)
						g_color[3] = 1;//D3DsurfAtom[Ordered_z].occupancy;
				}
				myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

				//First rotate the bond after that translate
				*world *= D3DcompBonds[Ordered_z].mRotate[j] * D3DcompAtom[Ordered_z].mShift;

				// Set the matrices for the shader.
				myScene.g_pEffect->SetMatrix("g_mWorld", world);

				// Calculate combined world-view-projection matrix.
				*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
				myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);



				//-------------------------------------------------------------------------------------------------------------------------------
				int mesh;
				// Render.
				if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0)))
				{
					if (myScene.scene_rotate) mesh = 4;
					else
					{
						if (myScene.flag.g_spring)
							mesh = 5;
						else
							mesh = 2;
					}
					for (UINT pass = 0; pass < *totalPasses; ++pass)
					{
						if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass)))
						{
							for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
							{
								//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
								// Initialize required parameter
								V(myScene.g_pEffect->CommitChanges());
								// Render the mesh with the applied technique
								V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
							}
							myScene.g_pEffect->EndPass();
						}
					}
					myScene.g_pEffect->End();
				}
			}//for bonds
		}//if mode
	}//for elements
}

inline void Scene3D::DrawSurfaceAtoms(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	WCHAR str[10];
	RECT rcClient;
	float g_color[4];
	D3DXMATRIX m_temp;
	int Ordered_z;
	DWORD l;
	UINT pass;
	int mesh;

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
					*ObjectScale = (float)(sqrt(D3DsurfAtom[Ordered_z].DW_in_plane) * myScene.flag.DWscale);
				*ObjectScale_z = (float)(sqrt(D3DsurfAtom[Ordered_z].DW_out_of_plane) * myScene.flag.DWscale);
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
				*ObjectScale = myScene.flag.BallSize;
				*ObjectScale_z = myScene.flag.BallSize;
			}

			if (*ObjectScale_z == 0.)
			{
				*ObjectScale_z = *ObjectScale;
			}

			D3DXMatrixScaling(world, *ObjectScale, *ObjectScale, *ObjectScale_z);


			//Show selected atoms atoms
			if (D3DsurfAtom[Ordered_z].Selected > 0)
			{
				g_color[0] = 1.0f; g_color[1] = 1.0f; g_color[2] = 0.1f; g_color[3] = 1.0f;
			}
			else
			{
				if (myScene.flag.g_compare)
				{
					g_color[0] = 0.9f; g_color[1] = 0.1f; g_color[2] = 0.1f; g_color[3] = 1.0f;
				}
				else
					CopyMemory(g_color, AtProp.AtomColor[D3DsurfAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);

				if (myScene.flag.g_transparency)
					g_color[3] = D3DsurfAtom[Ordered_z].occupancy;
				else
					g_color[3] = 1.f;
			}
			myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

			*world *= D3DsurfAtom[Ordered_z].mShift;

			// Set the matrices for the shader.
			myScene.g_pEffect->SetMatrix("g_mWorld", world);

			// Calculate combined world-view-projection matrix.
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
			myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);


			//-------------------------------------------------------------------------------------------------------------
			//Surface labels
			//-------------------------------------------------------------------------------------------------------------
			if (myScene.flag.plot_surf_labels && (worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44 < myScene.Zfar && worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44>myScene.Znear || !myScene.mouse_Zcut))
			{
				swprintf_s(str, 20, L"%s%d", W_elements[D3DsurfAtom[Ordered_z].AtomicNumber], D3DsurfAtom[Ordered_z].AtomInTheFile);
				rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
				rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));

				rcClient.left -= 10;
				rcClient.top -= 10;
				rcClient.right = rcClient.left + 40; rcClient.bottom = rcClient.top + myScene.flag.g_text_size + 10;
				g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(GetRValue(myScene.flag.rgbText), GetGValue(myScene.flag.rgbText), GetBValue(myScene.flag.rgbText)));
			}
			//-------------------------------------------------------------------------------------------------------------------------------
			//Pickup atom position and select
			//-------------------------------------------------------------------------------------------------------------------------------
			if (myScene.mouse_select)
			{
				myScene.ResetSelection(myScene.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);
				myScene.MouseSelect(&myScene.ptMouseSelect, myScene.g_params.BackBufferWidth, myScene.g_params.BackBufferHeight, worldViewProjectionMatrix, D3DsurfAtom, Ordered_z, myScene.MenuHeight, myScene.flag.g_perspective);
			}

			//-------------------------------------------------------------------------------------------------------------------------------
			
			// Render.
			if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0)))
			{
				if (myScene.scene_rotate)
					mesh = 3;
				else 
					mesh = 0;

				for (pass = 0; pass < *totalPasses; ++pass)
				{
					if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass)))
					{
						for (l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
						{
							myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
							// Initialize required parameter
							V(myScene.g_pEffect->CommitChanges());
							// Render the mesh with the applied technique
							V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
						}
						myScene.g_pEffect->EndPass();
					}
				}
				myScene.g_pEffect->End();
			}
		}//for elements
	}
}

inline void Scene3D::DrawSurfaceAtomsCompare(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z)
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
					*ObjectScale = (float)(sqrt(D3DcompAtom[Ordered_z].DW_in_plane) * myScene.flag.DWscale);
				*ObjectScale_z = (float)(sqrt(D3DcompAtom[Ordered_z].DW_out_of_plane) * myScene.flag.DWscale);
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
				*ObjectScale = myScene.flag.BallSize;
				*ObjectScale_z = myScene.flag.BallSize;
			}

			if (*ObjectScale_z == 0.)
			{
				*ObjectScale_z = *ObjectScale;
			}

			//To avoid interference with real model
			*ObjectScale *= 1.02f;
			*ObjectScale_z *= 1.02f;

			D3DXMatrixScaling(world, *ObjectScale, *ObjectScale, *ObjectScale_z);


			//Show selected atoms  
			if (D3DcompAtom[Ordered_z].Selected > 0)
			{
				g_color[0] = 1.0f; g_color[1] = 1.0f; g_color[2] = 0.1f; g_color[3] = 1.0f;
				myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);
			}
			else
			{
				if (myScene.flag.g_compare)
				{
					g_color[0] = 0.1f; g_color[1] = 0.1f; g_color[2] = 0.9f; g_color[3] = 1.f;
				}
				else
					CopyMemory(g_color, AtProp.AtomColor[D3DcompAtom[Ordered_z].AtomicNumber], sizeof(float) * 4);
			}
			myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

			*world *= D3DcompAtom[Ordered_z].mShift;

			// Set the matrices for the shader.
			myScene.g_pEffect->SetMatrix("g_mWorld", world);

			// Calculate combined world-view-projection matrix.
			*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
			myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);


			//-------------------------------------------------------------------------------------------------------------
			//Surface labels
			//-------------------------------------------------------------------------------------------------------------
			if (myScene.flag.plot_surf_labels && (worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44 < myScene.Zfar && worldViewProjectionMatrix->_43*worldViewProjectionMatrix->_44>myScene.Znear || !myScene.mouse_Zcut))
			{
				WCHAR str[200];
				RECT rcClient;
				swprintf_s(str, 20, L"%s%d", W_elements[D3DcompAtom[Ordered_z].AtomicNumber], D3DcompAtom[Ordered_z].AtomInTheFile);
				rcClient.left = (int)(myScene.g_params.BackBufferWidth / 2 * (1.0 + worldViewProjectionMatrix->_41 / worldViewProjectionMatrix->_44));
				rcClient.top = (int)(myScene.g_params.BackBufferHeight / 2 * (1.0 - worldViewProjectionMatrix->_42 / worldViewProjectionMatrix->_44));

				rcClient.left -= 10;
				rcClient.top -= 10;
				rcClient.right = rcClient.left + 40; rcClient.bottom = rcClient.top + myScene.flag.g_text_size + 10;
				g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(GetRValue(myScene.flag.rgbText), GetGValue(myScene.flag.rgbText), GetBValue(myScene.flag.rgbText)));
			}
			//-------------------------------------------------------------------------------------------------------------------------------
			//Pickup atom position and select
			//-------------------------------------------------------------------------------------------------------------------------------
			if (myScene.mouse_select){
				myScene.ResetSelection(myScene.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DcompAtom, nr_surf_atoms_toplot);
				myScene.MouseSelect(&myScene.ptMouseSelect, myScene.g_params.BackBufferWidth, myScene.g_params.BackBufferHeight, worldViewProjectionMatrix, D3DcompAtom, Ordered_z, myScene.MenuHeight, myScene.flag.g_perspective);
			}


			//-------------------------------------------------------------------------------------------------------------------------------
			int mesh;
			// Render.
			if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0)))
			{
				if (myScene.scene_rotate) mesh = 3;
				else mesh = 0;
				for (UINT pass = 0; pass < *totalPasses; ++pass)
				{
					if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass)))
					{
						for (DWORD l = 0; l < g_Orbiter[mesh].m_dwNumMaterials; ++l)
						{
							//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
							// Initialize required parameter
							V(myScene.g_pEffect->CommitChanges());
							// Render the mesh with the applied technique
							V(g_Orbiter[mesh].m_Mesh->DrawSubset(l));
						}
						myScene.g_pEffect->EndPass();
					}
				}
				myScene.g_pEffect->End();
			}
		}//for elements
	}
}

inline void Scene3D::DrawThermalElipsoids(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	D3DXMATRIX m_temp;

	float g_elipsoids[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	myScene.g_pEffect->SetValue("g_ObiectColor", g_elipsoids, sizeof(float) * 4);
	myScene.g_pEffect->SetMatrix("g_mRotate", &myScene.rotate);

	for (int j = 0; j < 3; ++j){ //3 elipsoids
		for (int i = 0; i < nr_surf_atoms_toplot; ++i)
		{
			if (D3DsurfAtom[i].AtomicNumber != 0)
			{
				if (D3DsurfAtom[i].display_mode == 1 || D3DsurfAtom[i].display_mode == 4)
				{
					*ObjectScale_z = *ObjectScale = AtProp.AtomRadius[D3DsurfAtom[i].AtomicNumber] * PLOT_SCALE_FACTOR;
					if (D3DsurfAtom[i].DW_in_plane > 0.)
						*ObjectScale = (float)(sqrt(D3DsurfAtom[i].DW_in_plane) * myScene.flag.DWscale);
					*ObjectScale_z = (float)(sqrt(D3DsurfAtom[i].DW_out_of_plane) * myScene.flag.DWscale);

					if (D3DsurfAtom[i].DW_out_of_plane == 0)
						*ObjectScale_z = *ObjectScale;

					if (*ObjectScale > 0.f){
						float reduce_x = 1.05f, reduce_y = 1.05f, reduce_z = 1.05f;

						if (j == 0) reduce_x = 0.1f;
						if (j == 1) reduce_y = 0.1f;
						if (j == 2) reduce_z = 0.1f;
						D3DXMatrixScaling(world, *ObjectScale*reduce_x, *ObjectScale*reduce_y, *ObjectScale_z*reduce_z);

						*world *= D3DsurfAtom[i].mShift;

						// Set the matrices for the shader.
						myScene.g_pEffect->SetMatrix("g_mWorld", world);
						// Calculate combined world-view-projection matrix.
						*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
						myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);


						// Render.//for shadows is only one pass
						if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0)))
						{
							for (UINT pass = 0; pass < *totalPasses; ++pass)
							{
								if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass)))
								{
									for (DWORD l = 0; l < g_Orbiter[0].m_dwNumMaterials; l++)
									{
										//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
										// Initialize required parameter
										V(myScene.g_pEffect->CommitChanges());
										// Render the mesh with the applied technique
										V(g_Orbiter[0].m_Mesh->DrawSubset(l));
									}
									myScene.g_pEffect->EndPass();
								}
							}
							myScene.g_pEffect->End();
						}
					}//for mode
				}//for i
			}//0 atomic number
		}//for j
	}//if
}

inline void Scene3D::DrawLiquidLayers(D3DXMATRIX *world, D3DXMATRIX *RotateViewProjectionMatrix, D3DXMATRIX *worldViewProjectionMatrix, UINT *totalPasses, float *ObjectScale, float *ObjectScale_z)
{
	HRESULT hr;
	float g_color[4];
	D3DXMATRIX m_temp;
	int Ordered_z;

	myScene.g_pEffect->SetTechnique("Blend");
	myScene.g_pEffect->SetBool("Drawshadows", false);

	Order_Atoms3D(*RotateViewProjectionMatrix, D3DLiquid, nr_liquid_layers_toplot);
	for (int i = 0; i < nr_liquid_layers_toplot; ++i)
	{
		Ordered_z = D3DLiquid[i].Z_Depth;
		if (D3DLiquid[Ordered_z].AtomicNumber != 0)
		{
			double scale = sqrt(sqr((double)g_SRepetitionX) + sqr((double)g_SRepetitionY)) / 3;
			*ObjectScale = (float)((1.+scale) * myScene.flag.DWscale);
			*ObjectScale_z = (float)(sqrt(D3DLiquid[Ordered_z].DW_out_of_plane) * myScene.flag.DWscale);
		}

		CopyMemory(g_color, AtProp.AtomColor[D3DLiquid[Ordered_z].AtomicNumber], sizeof(float) * 4);
		if (myScene.flag.g_transparency)
			g_color[3] = D3DLiquid[Ordered_z].occupancy;
		else
			g_color[3] = 1.f;

		myScene.g_pEffect->SetValue("g_ObiectColor", g_color, sizeof(float) * 4);

		D3DXMatrixScaling(world, *ObjectScale, *ObjectScale_z, *ObjectScale);

		D3DXMatrixRotationX(&m_temp, (float)(90.*deg2rad));
		*world *= m_temp;

		D3DXMatrixTranslation(&m_temp, (float)D3DLiquid[Ordered_z].x, (float)D3DLiquid[Ordered_z].y, (float)D3DLiquid[Ordered_z].z);
		*world *= m_temp;



		// Set the matrices for the shader.
		myScene.g_pEffect->SetMatrix("g_mWorld", world);

		// Calculate combined world-view-projection matrix.
		*worldViewProjectionMatrix = *world * *RotateViewProjectionMatrix;
		myScene.g_pEffect->SetMatrix("g_mWorldViewProjection", worldViewProjectionMatrix);

		//-------------------------------------------------------------------------------------------------------------------------------
		DWORD l;
		// Render.
		if (SUCCEEDED(myScene.g_pEffect->Begin(totalPasses, 0)))
		{
			for (UINT pass = 0; pass < *totalPasses; ++pass)
			{
				if (SUCCEEDED(myScene.g_pEffect->BeginPass(pass)))
				{
					for (l = 0; l < g_Orbiter[1].m_dwNumMaterials; ++l)
					{
						//myScene.g_pEffect->SetTexture( "colorMapTexture", g_Orbiter[0].g_pMeshTexture[l] ) ;
						// Initialize required parameter
						V(myScene.g_pEffect->CommitChanges());
						// Render the mesh with the applied technique
						V(g_Orbiter[1].m_Mesh->DrawSubset(l));
					}
					myScene.g_pEffect->EndPass();
				}
			}
			myScene.g_pEffect->End();
		}
	}//for elements

}

inline void Scene3D::DrawLasso(D3DXMATRIX *RotateViewProjectionMatrix)
{

	static D3DXVECTOR2 VertexList[MAX_LASSO_POINTS];
	static D3DXVECTOR2 VertexList2[2];
	static int nr = 0;
	static int nr2 = 0;
	//Delete all selections
	myScene.ResetSelection(myScene.ptMouseSelect, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot);

	if (myScene.ptMouseSelect.x == -1)
	{
		nr = 0;
		nr2 = 0;
	}

	if (myScene.ptMouseSelect.x > 0)
	{
		if (nr == 0)
		{
			VertexList[0].x = (float)myScene.ptMouseSelect.x;
			VertexList[0].y = (float)myScene.ptMouseSelect.y - myScene.MenuHeight;

			VertexList2[0].x = VertexList[0].x;
			VertexList2[0].y = VertexList[0].y;
			nr++;
		}


		if (nr > 0)//&& (nr < 2 || (myScene.ptMouseSelect.x != VertexList[nr - 1].x && myScene.ptMouseSelect.y != VertexList[nr - 1].y))
		{
			if (nr >= MAX_LASSO_POINTS) nr = 0;
			VertexList[nr].x = (float)myScene.ptMouseSelect.x;
			VertexList[nr].y = (float)myScene.ptMouseSelect.y - myScene.MenuHeight;

			nr2 = 2;
			VertexList2[1].x = VertexList[nr].x;
			VertexList2[1].y = VertexList[nr].y;
			nr++;
		}
	}

	//Select atoms from myScene points
	if (myScene.LeftButtonUp)
	{
		myScene.MouseLasso(VertexList, nr, myScene.g_params.BackBufferWidth, myScene.g_params.BackBufferHeight, RotateViewProjectionMatrix, D3DbulkAtom, nr_bulk_atoms_toplot, D3DsurfAtom, nr_surf_atoms_toplot, myScene.MenuHeight);
		nr = 0;
		nr2 = 0;
		myScene.ptMouseSelect.x = -1;
	}

	// Draw the line.
	myScene._line->SetWidth(2.0f);
	myScene._line->Begin();
	myScene._line->Draw(VertexList, nr, D3DXCOLOR(0.9f, 0.9f, 0.1f, 1.0f));
	myScene._line->End();

	//Draw closing line
	myScene._line->SetWidth(2.0f);
	myScene._line->Begin();
	myScene._line->Draw(VertexList2, nr2, D3DXCOLOR(0.9f, 0.9f, 0.1f, 1.0f));
	myScene._line->End();


}

void Scene3D::RenderText()
{
	static RECT rcClient;
	WCHAR str[500];


	if (myScene.g_displayHelp)
	{
		wsprintf(str, L"Left mouse click and drag to track camera \n\rMiddle mouse click and drag to dolly camera \n\rRight mouse click and drag to orbit camera \n\rMouse wheel to dolly camera\n\rAlt+Enter full screen\n\rFPS: %d \n\rMultisample anti-aliasing: %d \n\rAnisotropic filtering: %d\n\rResolution %dx%d\n\rAuto depth stencil format %d", myScene.g_framesPerSecond, myScene.g_msaaSamples[myScene.g_msaaCurentModes], myScene.g_maxAnisotrophy, myScene.g_params.BackBufferWidth, myScene.g_params.BackBufferHeight, myScene.g_params.AutoDepthStencilFormat);
		rcClient.left = 4; rcClient.right = 300;
		rcClient.top = 0; rcClient.bottom = 400;

		g_pFont->DrawText(0, str, -1, &rcClient, DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 255, 0));
	}

}