#pragma once
class ResidualsWindow
{
public:
	ResidualsWindow();
	virtual ~ResidualsWindow();


	HWND CreateWin(const WNDCLASSEX &wcl, WCHAR *pszTitle, RECT WinRect);
	LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void UpdateResiduals(HWND hWnd);
	void Resize(HWND hWnd, int);

private:
	HDC hdc, hdcBack;
	HPEN hPen;
	HBRUSH hBru;
	HBRUSH hBruWhite;
	int yCurrentScroll;   // current vertical scroll value
	int Max;

	// These variables are required for vertical scrolling. 
	int yMinScroll;       // minimum vertical scroll value  
	int yMaxScroll;       // maximum vertical scroll value 

	bool fScroll;
	SCROLLINFO si;

	double *ResidualsBank;
	int *ColorBank;
	double MaxResidual;
	double MinResidual;

	HFONT MyFont_Residuals;
	void CalculateResiduals();
	void InitCalculateResiduals();
	void DrawScene(HDC hdcBack, RECT rect);
	void DrawHeader(HDC hdc_in, RECT rect);

};

