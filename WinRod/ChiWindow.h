//The mainslider class
//----------------------------------------------------------------------------------------------------------------------
class ChiWin {

public:

	//destructor
	~ChiWin(){
		DeleteObject(MyFont_Medium);
	};

public:
	HWND CreateChiWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect);
	LRESULT CALLBACK ChiWin::WinProc(HWND hWnd, UINT msg, HINSTANCE hinst, WPARAM wParam, LPARAM lParam);

	double CalculateChi_sqr(void);

private:
	HWND hRebar;
	HWND hwndTB;

	void Scene(HWND hwnd, HDC hdc_scene, int size);
						// Handle to the toolbar
	HWND WINAPI CreateToolbar(HWND hwnd, HINSTANCE hInst);
	HDC GetPrinterDC(HWND Hwnd);
	void PrintFunction(HWND hwnd);
	void SaveFileFunction(HWND hwnd);
	void CopyToClipobard(HWND hwnd);
	void TextToCopy(char *str);

	HFONT MyFont_Medium;
	double R;
	double Chi_Sqr;
	double sqrt_chi_Sqr;


};