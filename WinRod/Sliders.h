
	//The mainslider class
	//----------------------------------------------------------------------------------------------------------------------
class Sliders {

public:

	Sliders()
	{//at every restart the number of parameters have to be calculated
		hSlider = NULL;
		hEdit = NULL;
		hEditValue = NULL;
		hEditState = NULL;
	}

	//destructor
	~Sliders()
	{

	};

	typedef void(*Update)(bool);
	Update	updata_windows;

	HWND	CreateSliderWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect);
	void	CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void	UpdateSliders(HWND);
	void	UpdateValuesFromSliders(void);
	void	SliderInitialisation(HWND hWnd);

private:
	int		MaxParameters; //number of all parameters, this is also number of sliders
	int		WindowWidth;
	HFONT	MyFont_Small;

	HWND*	hSlider;
	HWND*	hEdit;
	HWND*	hEditValue;
	HWND*	hEditState;


	HWND	CreateSlider(int SliderNumber, HWND hWnd, int PositionX, int PositionY, WCHAR *title, int item, double Min, double Max, double value, bool  state);
	void	SetSlider(int SliderNumber, double Min, double Max, double value);
	double	GetSlider(int SliderNumber, double Min, double Max);
	void	AddTextToEdit(int SliderNumber, double value);
	void	AddStateToEdit(int SliderNumber, bool state);
	void	MoveElements(int YPosition, int width);
	int		CalculateNumberOfParameters();

};