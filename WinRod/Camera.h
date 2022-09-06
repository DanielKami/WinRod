

#define MOUSE_TRACK_LIGHT 5.5f

class Camera{


public:
	Camera()
	{
		TotalQuad = D3DXQUATERNION(1, 0, 0, 0);
	}

	//State camera flags
	bool					    mouse_rotate;
	bool					    mouse_select;
	bool					    mouse_lasso;
	bool						mouse_translate;
	bool						mouse_Zcut;
	bool						scene_rotate;
	bool						LeftButtonUp;
	bool						g_autorotate;


   // Used to select atoms from the screen
    POINT						ptMouseSelect;  

	//All important 3D vectors
	D3DXVECTOR3                 g_cameraPos;
	D3DXVECTOR3                 g_targetPos;
	D3DXVECTOR3                 g_system_translate;
	D3DXVECTOR3					g_LightPosition;

	//All the important camera matrices
	D3DXMATRIX					rotate;
	D3DXMATRIX					view;
	D3DXMATRIX					proj;

	//All the important variables for perspective matrice
	float						Fovy;
	float						Znear;
	float						Zfar;

	//functions prototypes
	void UpdateCamera();
	bool ProcessMouseInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void AbsoluteQuaternionRotation(float pitch, float yow, float roll);
	void Init();

private:

	int							g_yow;
	int							g_pitch;
	int							g_arcX, g_arcY;

	//for camera
	D3DXVECTOR3					CameraRotateAroundX;			// When the orientation of camera change the z direction for arcball too
	D3DXVECTOR3					CameraRotateAroundY;			// to follow these changes we have to change 
	D3DXVECTOR3					CameraRotateAroundZ;			// the rotation directions to be z always pointing inside the screen											

	//The main quaternion for the orientation matrix calculation
	D3DXQUATERNION TotalQuad;

    POINT						ptMousePrev;

	int							WindowsWidth;
	int							WindowsHight;

    //functions prototypes
	void ApplyEffects(int g_windowWidth, int g_windowHeight );
	void UpdateQuadRotation();
	void UpdateCameraOrientationPichYow(void);
	void UpdateCamera(HWND hWnd, int &W, int &H);

};

