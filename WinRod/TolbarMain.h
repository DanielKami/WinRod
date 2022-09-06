
 // Identifier of the main tool bar
#define dwTBFontStyle TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_GROUP
                           // Style of the Font toolbar

#define ID_NEW_MAIN              1000
#define ID_FILE_OPEN_MAIN        1001
#define ID_FILE_SAVE_MAIN        1002
#define ID_PRINT_MAIN            1003
#define IDM_FIT			         1006
#define IDM_Z_Ell_PROFILE        1007
#define IDM_KEATING				 1014
#define IDM_EDIT_FIT			 1015
#define IDM_EDIT_BULK			 1016
#define IDM_SLIDERS				 1017

// The TBBUTTON structure contains information the toolbar buttons.
static TBBUTTON tbButtonMain[] =      
{
  {0, ID_NEW_MAIN,			TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {1, ID_FILE_OPEN_MAIN,	TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {2, ID_FILE_SAVE_MAIN,	TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {3, ID_PRINT_MAIN,		TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {0, 0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {6, IDM_FIT,				TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {7, IDM_Z_Ell_PROFILE,	TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {21,IDM_KEATING,			TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1},
  {0, 0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1},
  {22,IDM_EDIT_FIT,			TBSTATE_ENABLED,					 TBSTYLE_BUTTON,  0, (BYTE)-1}

};

static TBBUTTON tbButton3[] =      
{
  {0, 0,					TBSTATE_ENABLED,					 TBSTYLE_SEP,     0, (BYTE)-1}
};


//Combo RODs
static TBBUTTON tbButton4[] =
{
	{ 0, 0, (BYTE)TBSTATE_ENABLED, (BYTE)TBSTYLE_SEP, (BYTE)0, (BYTE)-1 }
};