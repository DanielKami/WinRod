

/***************************************************************************/
/*      This file contains all the functions necessary for camera, light
		calculations in a 3D space.
		For rotation a quaternions are used for the best 3D quality operations.
		This class interprete the mouse movments and calculate all necessary
		3D matrtices.

		Creared by: Daniel Kaminski based partly on DirectX documentation
*/
/***************************************************************************/
#include <stdafx.h>
#include <Windowsx.h>
#include <d3dx9.h>
#include "3Ddef.h"
#include "Camera.h"


#if !defined(WHEEL_DELTA)
#define WHEEL_DELTA 120
#endif

#if !defined(WM_MOUSEWHEEL)
#define WM_MOUSEWHEEL 0x020A
#endif

//-----------------------------------------------------------------------------
// Process all events related to camera.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
bool Camera::ProcessMouseInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	static POINT ptMouseCurrent;  //Is also used to determine atom position from the screen for selection, therefore must be public

    static float wheelDelta = 0.0f;
	static RECT rect;

    switch (msg)
    {

	case WM_CREATE:
		mouse_rotate=true;
		Fovy = CAMERA_FOVY_PERSPECTIVE;
		Znear = CAMERA_ZNEAR_PERSPECTIVE;
		Zfar = CAMERA_ZFAR_PERSPECTIVE;
		break;

    case WM_ACTIVATE:

		ptMouseSelect.x = -1;
		LeftButtonUp = false;
		UpdateCamera(hWnd, WindowsWidth, WindowsHight);

		break;

    case WM_LBUTTONDOWN:
		scene_rotate = false;
		LeftButtonUp = false;
		g_autorotate = false;

        ptMousePrev.x = ptMouseCurrent.x;
        ptMousePrev.y = ptMouseCurrent.y;

		if(mouse_lasso || mouse_select)
		{
			ptMouseSelect.x = ptMouseCurrent.x;
			ptMouseSelect.y = ptMouseCurrent.y;
		}

        break;

    case WM_RBUTTONDOWN:
		scene_rotate = false;
		LeftButtonUp = false;
		g_autorotate = false;

        ptMousePrev.x = ptMouseCurrent.x;
        ptMousePrev.y = ptMouseCurrent.y;

		//For deselection
		if(mouse_select || mouse_lasso)
		{
			ptMouseSelect.x = -3;
			ptMouseSelect.y = -3;
		}
        break;

    case WM_MBUTTONDOWN:
		LeftButtonUp = false;
		g_autorotate = false;

        ptMousePrev.x = GET_X_LPARAM(lParam);
        ptMousePrev.y = GET_Y_LPARAM(lParam);
         break;

    case WM_MOUSEMOVE:
		LeftButtonUp = false;
		
        ptMouseCurrent.x = GET_X_LPARAM(lParam);
        ptMouseCurrent.y = GET_Y_LPARAM(lParam);

//------------------------------------------------------------------------
		switch (wParam)
		{
		case MK_LBUTTON:
			scene_rotate = true;

			if (mouse_lasso)
			{
				ptMouseSelect.x = ptMouseCurrent.x;
				ptMouseSelect.y = ptMouseCurrent.y;
			}

			if (mouse_rotate)
			{
				g_yow = ptMousePrev.x - ptMouseCurrent.x;
				g_pitch = ptMousePrev.y - ptMouseCurrent.y;

				g_arcX = (WindowsWidth) / 2 - ptMouseCurrent.x;
				g_arcY = (WindowsHight) / 2 - ptMouseCurrent.y;
			}
			else
			{
				g_yow = 0;
				g_pitch = 0;
			}

			if (mouse_translate)
			{
				int dx = (ptMouseCurrent.x - ptMousePrev.x);
				int dy = (ptMouseCurrent.y - ptMousePrev.y);

				g_system_translate.x += dx * MOUSE_TRACK_SPEED;
				g_system_translate.y -= dy * MOUSE_TRACK_SPEED;
			}

			if (mouse_Zcut)
			{
				Znear += (ptMousePrev.y - ptMouseCurrent.y)*MOUSE_TRACK_SPEED;
			}
			ptMousePrev.x = ptMouseCurrent.x;
			ptMousePrev.y = ptMouseCurrent.y;

			ApplyEffects(WindowsWidth, WindowsHight);
			break; //Lbutton

		case MK_RBUTTON:

			if (mouse_rotate)
			{
				int dx = (ptMouseCurrent.x - ptMousePrev.x);
				int dy = (ptMouseCurrent.y - ptMousePrev.y);

				g_LightPosition.x += dx * MOUSE_TRACK_LIGHT;
				g_LightPosition.y += -dy * MOUSE_TRACK_LIGHT;

				if (g_LightPosition.x > 5000) g_LightPosition.x = 5000;
				if (g_LightPosition.x < -5000) g_LightPosition.x = -5000;
				if (g_LightPosition.y > 5000) g_LightPosition.y = 5000;
				if (g_LightPosition.y < -5000) g_LightPosition.y = -5000;
			}

			if (mouse_Zcut)
			{
				Zfar += (ptMousePrev.y - ptMouseCurrent.y)*MOUSE_TRACK_SPEED;
			}
			ptMousePrev.x = ptMouseCurrent.x;
			ptMousePrev.y = ptMouseCurrent.y;

			break;
		}
        break;
//------------------------------------------------------------------------

    case WM_RBUTTONUP:
		{
			scene_rotate = false;
			g_autorotate = false;
		}
		break;

    case WM_LBUTTONUP:
		{
			LeftButtonUp = true;
			scene_rotate = false;

			if (abs(g_yow) > 20 || abs(g_pitch) > 20)
			{
				g_autorotate = true;
				g_yow /= 10;
				g_pitch /= 10;
			}
			else
			{
				g_autorotate = false;
				g_yow = 0;
				g_pitch = 0;
			}

		}
		break;

    case WM_MBUTTONUP:
		scene_rotate = false;
		ApplyEffects(WindowsWidth, WindowsHight );
        return 0;

    case WM_MOUSEWHEEL:
		scene_rotate = false;
        wheelDelta = static_cast<float>(static_cast<int>(wParam) >> 16);
		if ((1.-wheelDelta * MOUSE_WHEEL_DOLLY_SPEED_3D)>0.)
        g_cameraPos *= (float)(1.-wheelDelta * MOUSE_WHEEL_DOLLY_SPEED_3D);
		
    case WM_SIZE:
			UpdateCamera(hWnd, WindowsWidth, WindowsHight);	
	    return 0;
 		
	case WM_TIMER:
		if (g_autorotate)
			ApplyEffects(WindowsWidth, WindowsHight );
		break;

    default:
        return 1;
    }
	return 1;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////Camera///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// Calculate all camera matrices. The rotation matrics is recovery every time from quaternion which stores the rotation state.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Camera::ApplyEffects(int g_windowWidth, int g_windowHeight)
{

	D3DXMatrixPerspectiveFovLH(&proj,
		Fovy,
		static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight),
		Znear, Zfar);

	D3DXVECTOR3 LookUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	if (g_cameraPos.x == 0 && g_cameraPos.z == 0)
		LookUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	D3DXMatrixLookAtLH(&view,
		&g_cameraPos,
		&g_targetPos,
		&LookUp);

	UpdateCameraOrientationPichYow();
	UpdateQuadRotation();

	//build rotation matrix from quaternion
	D3DXMatrixRotationQuaternion(&rotate, &TotalQuad);

	D3DXMATRIX m_temp;
	D3DXMatrixTranslation(&m_temp, g_system_translate.x, g_system_translate.y, g_system_translate.z);
	view *= m_temp;
}

//-----------------------------------------------------------------------------
// ArcBall rotation by quaternions.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Camera::UpdateQuadRotation()
{

	//quaternions for unit rotations
	D3DXQUATERNION LocalRotQuadX;
	D3DXQUATERNION LocalRotQuadY;
	D3DXQUATERNION LocalRotQuadZ;

	//Rotate around X,Y,Z of arcball
	float pitch = (float)ROTATION_STEP * g_pitch;	//g_pitch added to rotate with different speed
	float roll_X = pitch * g_arcX;

	float yow = (float)ROTATION_STEP * g_yow;	    //g_yowto added to rotate with different speed
	float roll_Y = yow  * g_arcY;

	float roll = (float)ROTATION_STEP_Z * (roll_Y - roll_X);


	//Rotate around X axis
	D3DXQuaternionRotationAxis(&LocalRotQuadX,
		&CameraRotateAroundX,
		pitch);

	//Rotate around Y axis
	D3DXQuaternionRotationAxis(&LocalRotQuadY,
		&CameraRotateAroundY,
		yow);

	//Rotate around Z axis
	D3DXQuaternionRotationAxis(&LocalRotQuadZ,
		&CameraRotateAroundZ,
		roll);

	//Combine all rotations and update the main rotation quaternion
	TotalQuad *= LocalRotQuadX * LocalRotQuadY * LocalRotQuadZ;

	//Check for Quaternion normalisation after N rotation events to recover it
#define NORMALIZATION_CHECK 100 
	static int N = 0; N++;

	if (N > NORMALIZATION_CHECK)
	{
		N = 0;

		D3DXQuaternionNormalize(
			&TotalQuad,
			&TotalQuad
			);
	}
}


void Camera::AbsoluteQuaternionRotation(float pitch, float yow, float roll)
{
	//quaternions for unit rotations
	D3DXQUATERNION LocalRotQuadX;
	D3DXQUATERNION LocalRotQuadY;
	D3DXQUATERNION LocalRotQuadZ;

	const D3DXVECTOR3 LocalRotateAroundX = D3DXVECTOR3(1, 0, 0);
	const D3DXVECTOR3 LocalRotateAroundY = D3DXVECTOR3(0, 1, 0);
	const D3DXVECTOR3 LocalRotateAroundZ = D3DXVECTOR3(0, 0, 1);


	TotalQuad = D3DXQUATERNION(1, 0, 0, 0);
	//Rotate around X axis
	D3DXQuaternionRotationAxis(&LocalRotQuadX,
		&LocalRotateAroundX,
		pitch);

	//Rotate around Y axis
	D3DXQuaternionRotationAxis(&LocalRotQuadY,
		&LocalRotateAroundY,
		yow);

	//Rotate around Z axis
	D3DXQuaternionRotationAxis(&LocalRotQuadZ,
		&LocalRotateAroundZ,
		roll);

	//Combine all rotations and update the main rotation quaternion
	TotalQuad *= LocalRotQuadX * LocalRotQuadY * LocalRotQuadZ;

	//build rotation matrix from quaternion
	D3DXMatrixRotationQuaternion(&rotate,
		&TotalQuad);
}


//-----------------------------------------------------------------------------
// ArcBall rotation by quaternions.
// 21.01.2013 by Daniel Kaminski
//-----------------------------------------------------------------------------
void Camera::UpdateCameraOrientationPichYow(void)
{

	static float m_fCameraYawAngle = 0.;		//Yaw to follow the position of camera
	static float m_fCameraPitchAngle = 0.;		//Pich to follow the position of camera

	D3DXMATRIX mCameraRot;
	D3DXMatrixRotationYawPitchRoll(&mCameraRot, m_fCameraYawAngle, m_fCameraPitchAngle, 0);
	//axis vectors 
	const D3DXVECTOR3 LocalRotateAroundX = D3DXVECTOR3(1, 0, 0);
	const D3DXVECTOR3 LocalRotateAroundY = D3DXVECTOR3(0, 1, 0);
	const D3DXVECTOR3 LocalRotateAroundZ = D3DXVECTOR3(0, 0, 1);

	D3DXMATRIX InvView;
	D3DXMatrixInverse(&InvView, NULL, &view);

	// The axis basis vectors and camera position are stored inside the 
	// position matrix in the 4 rows of the camera's world matrix.
	// To figure out the yaw/pitch of the camera, we just need the Z basis vector
	// this is neccesary to follow by the arc ball the camera position in the word,
	// the basic rotators of quaternions are changed
	D3DXVECTOR3* pZBasis = (D3DXVECTOR3*)&InvView._31;

	m_fCameraYawAngle = atan2f(pZBasis->x, pZBasis->z);
	float fLen = sqrtf(pZBasis->z * pZBasis->z + pZBasis->x * pZBasis->x);
	m_fCameraPitchAngle = -atan2f(pZBasis->y, fLen);
	D3DXVec3TransformCoord(&CameraRotateAroundX, &LocalRotateAroundX, &mCameraRot);
	D3DXVec3TransformCoord(&CameraRotateAroundY, &LocalRotateAroundY, &mCameraRot);
	D3DXVec3TransformCoord(&CameraRotateAroundZ, &LocalRotateAroundZ, &mCameraRot);

}

void Camera::UpdateCamera(HWND hWnd, int &W, int &H)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	W = rect.right - rect.left;
	H = rect.bottom - rect.top;
	ApplyEffects(W, H);
};

void Camera::UpdateCamera()
{
	if (!g_autorotate)
		ApplyEffects(WindowsWidth, WindowsHight);
};

void Camera::Init()
{
	//Init camera flags
	mouse_rotate = true;
	mouse_select = false;
	mouse_lasso = false;
	mouse_translate = false;
	mouse_Zcut = false;
	scene_rotate = false;
	LeftButtonUp = false;
}

