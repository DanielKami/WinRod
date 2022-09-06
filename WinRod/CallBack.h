//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header of CallBack.cpp file containing all function prototypes used in in it
//			These prototypes are only important for WinRod dialog boxes
//
// Writen by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK CalculatePatterson(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Calculate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Sort_Bulk(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Domains(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ElectronDensity(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ElectronDensity(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Keating(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Fitting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Settings3D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FourierConturMap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SetSymmetry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditFitFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditBulkFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditDataFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditEnergyF1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditEnergyF2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AtomProperities(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK subEditProc2(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ChiConturMap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ElectronDensityProfileSettings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditMolecularFitFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ErrorIncrease(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Data_reduction(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK CalculateCoefficients(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK PlotCoefficients(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MultiplePlot(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SetAsa(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Option(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Option3D(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void	SetSlider(HWND hSlider, double Min, double Max, double value);
double	GetSlider(HWND hSlider, double Min, double Max);
void	SetDlgItemReal(HWND, int, double, WCHAR FORMAT[10] = L"%5.7f");
double	GetDlgItemReal(HWND, int);

double	GetItemReal(HWND hWnd, int nItem, int nSubItem);
void	GetItemText(HWND hWnd, WCHAR *Wstr, int nLen, int nItem, int nSubItem);
void	SetCell(HWND hWnd1, double value, int nRow, int nCol, bool Int = false);
void	SetCell(HWND hWnd1, WCHAR* szString, int nRow, int nCol);
void	CopyValuesFromListBox(HWND hItemList, bool *ValueFlag, bool *IsSomthingSelected);
void	SetAutoLimits(HWND hItemList);
void	TestBorders(int CellNumber, HWND hItemList, bool*);
bool	CheckRange(HWND hItemList);
bool	PerformRangeTest(HWND hItemList, int row);

void	KeyMoveEdit(HWND hEdit, HWND, int nIt, int nSub);
void	OnClickList(HWND, HWND, NMHDR*, int*, int*, int, int);
void	OnClickListColor(HWND, HWND, NMHDR*, int*, int*, COLORREF *);

void	ColorSet(HWND hDlg, int IDC_PictureBox, COLORREF *color);

void	SetCALL(HWND hItemList);
bool	CheckFitEdit(HWND hItemList);
void	CheckBulEdit(HWND hItemList);
void	MoveButton(HWND hwnd, int iDDl, RECT rect, int right, int bottom);

void    ArrangeAtomPositions(HWND hItemList);
void	CopyValuesFromFitEditListBox(HWND);
void	CopyValuesFromBulkEditListBox(HWND);
void	CopyValuesFromKeatingListBox(HWND);
void	CopyValuesFromDataEditListBox(HWND);
void	CopyValuesFromAtomProperitiesListBox(HWND hItemList);
void	CopyValuesFromMoleculeFitEditListBox(HWND hItemList, int Molecule);
void	CheckFitMoleculeEdit(HWND hItemList);
void	FillSecondTable(HWND hItemList, int Molecule);
void	FillAtomProperitiesList(HWND  hItemList);
void    FunctionSort(void);
double  distance(double x, double rx, double y, double ry, double z, double rz);
void    Find_closest(double distance, bool * fix);


LRESULT ProcessCustomDraw (LPARAM lParam);
LRESULT ProcessCustomDrawFit (LPARAM lParam);
LRESULT ProcessCustomDrawBulk (LPARAM lParam );
LRESULT ProcessCustomDrawFitting (LPARAM lParam, bool* );
LRESULT ProcessCustomDrawFitMolecule(LPARAM lParam);