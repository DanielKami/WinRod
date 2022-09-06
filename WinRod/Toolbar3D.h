#pragma once 
#define MENU_HEIGHT   17
#define NUMIMAGES     17   // Number of buttons in the toolbar 
#define NUMIMAGES3D	   6	//Number of buttons in the 3D toolbar
#define IMAGEWIDTH    16   // Width of the buttons in the toolbar  
#define IMAGEHEIGHT   16    // Width of the button images in the toolbar
#define BUTTONWIDTH   16    // Width of the button images in the toolbar
#define BUTTONHEIGHT  16    // Height of the button images in the toolbar



// Identifier of the 3D tool bar
#define dwTBFontStyle TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_GROUP
                           // Style of the Font toolbar

#define ID_EXPORT_3D				 3001
#define ID_PRINT_3D					 3003
#define ID_FULSCREAN_3D				 3004
#define ID_PROPERITIES_3D			 3005
#define ID_LASSO_3D		 			 3007
#define ID_SELECT_3D	 			 3008
#define ID_ROTATE_3D				 3009
#define ID_MOVE_3D					 3010
#define ID_ZOOM_3D					 3011
#define ID_LABELS_3D				 3013
#define ID_RESET_MEASUREMENT		 3014
#define ID_BUL_3D					 3015
#define ID_SUR_3D					 3016
#define ID_PLATE_3D					 3017
#define ID_COMPARE_3D				 3018
#define ID_TERMALRINGS_3D			 3019
#define ID_CELL						 3020
#define ID_PERSPECTIVE				 3021
#define ID_LOOK_X					 3022
#define ID_LOOK_Y					 3023
#define ID_LOOK_Z					 3024
#define ID_SURFACEI_II				 3025
#define ID_CHARGES  				 3026

// The TBBUTTON structure contains information the toolbar buttons.
static TBBUTTON tbButton3D[] =      
{
  {2,  ID_EXPORT_3D,		TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {3,  ID_PRINT_3D,			TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {0,  0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {4,  ID_FULSCREAN_3D,		TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {5,  ID_PROPERITIES_3D,	TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {18, ID_PLATE_3D,			TBSTATE_ENABLED | TBSTATE_PRESSED |  TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, (BYTE)-1},
  {20, ID_TERMALRINGS_3D,	TBSTATE_ENABLED | TBSTATE_PRESSED |  TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, (BYTE)-1},
  {23, ID_CELL, TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, TBSTYLE_CHECK, 0, (BYTE)-1 },
  {0,  0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {9,  ID_SELECT_3D,		TBSTATE_ENABLED,					 BTNS_CHECKGROUP, 0, (BYTE)-1},
  {8,  ID_LASSO_3D,			TBSTATE_ENABLED,					 BTNS_CHECKGROUP, 0, (BYTE)-1},
  {10, ID_ROTATE_3D,		TBSTATE_ENABLED  | TBSTATE_CHECKED,	 BTNS_CHECKGROUP, 0, (BYTE)-1},
  {11, ID_MOVE_3D,			TBSTATE_ENABLED,					 BTNS_CHECKGROUP, 0, (BYTE)-1},
  {12, ID_ZOOM_3D,			TBSTATE_ENABLED,					 BTNS_CHECKGROUP, 0, (BYTE)-1},
  {0,  0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {14, ID_LABELS_3D,		TBSTATE_ENABLED,					 TBSTYLE_CHECK,   0, (BYTE)-1},
  {0,  0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {17, ID_BUL_3D,			TBSTATE_ENABLED | TBSTATE_PRESSED |  TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, (BYTE)-1},
  {16, ID_SUR_3D,			TBSTATE_ENABLED | TBSTATE_PRESSED |  TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, (BYTE)-1},
  {0,  0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {19, ID_COMPARE_3D,		TBSTATE_ENABLED,					 TBSTYLE_CHECK,    0, (BYTE)-1},
  { 0, 0, TBSTATE_ENABLED,	TBSTYLE_SEP,										  0, (BYTE)-1 },
  { 24, ID_PERSPECTIVE,		TBSTATE_ENABLED | TBSTATE_PRESSED |  TBSTATE_CHECKED, TBSTYLE_CHECK, 0, (BYTE)-1 },
  { 0, 0, TBSTATE_ENABLED,  TBSTYLE_SEP, 0, (BYTE)-1 },
  { 28, ID_SURFACEI_II,		TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED, TBSTYLE_CHECK, 0, (BYTE)-1 },
  { 29, ID_CHARGES,		    TBSTATE_ENABLED,                     TBSTYLE_CHECK, 0, (BYTE)-1 }
};


static TBBUTTON tbButton3D1[] =      
{  
  {0, 0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {0, 0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {15, ID_RESET_MEASUREMENT,TBSTATE_ENABLED,					 TBSTYLE_BUTTON,   0, (BYTE)-1}

};

static TBBUTTON tbButton3D2[] =      
{
  {0, 0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1}
};

//For look in certain direction
static TBBUTTON tbButton3D3[] =
{
	{ 25, ID_LOOK_X, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 },
	{ 26, ID_LOOK_Y, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 },
	{ 27, ID_LOOK_Z, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 },
};

