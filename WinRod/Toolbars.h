

#include "resource.h"
#include "definitions.h"
#include "TollbarWindows.h"		//Tollbar for restoring closed windowses
#include "TolbarMain.h"
#include "ToolbarEdit.h"

#define MENU_HEIGHT   17
#define NUMIMAGES     17   // Number of buttons in the toolbar 
#define NUMIMAGES3D	   6	//Number of buttons in the 3D toolbar
#define IMAGEWIDTH    16   // Width of the buttons in the toolbar  
#define IMAGEHEIGHT   16   // Height of the buttons in the toolbar  
#define BUTTONWIDTH   16    // Width of the button images in the toolbar
#define BUTTONHEIGHT  16    // Height of the button images in the toolbar


HWND WINAPI CreateToolbar(HWND hwnd, HWND *hwndTB4, HINSTANCE hInst, HWND *ComboBox_h, HWND *ComboBox_k, HWND *ComboBox_l, HWND *hComboRODs, int Width);


