#define MENU_HEIGHT   23
#define NUMIMAGES2D	  17	//Number of buttons in the 3D toolbar
#define IMAGEWIDTH    20   // Width of the buttons in the toolbar  
#define IMAGEHEIGHT   18   // Height of the buttons in the toolbar  
#define BUTTONWIDTH   0    // Width of the button images in the toolbar
#define BUTTONHEIGHT  0    // Height of the button images in the toolbar
 // Identifier of the main tool bar
#define dwTBFontStyle TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_GROUP
                           // Style of the Font toolbar

#define IDM_FILE_OPEN                   120
#define IDM_FILE_SAVE                   121
#define IDM_PRINT		                122
#define IDM_2D_SETTINGS                 123
#define IDM_GRID2D	                    124
#define IDM_MOVI                        125

#define IDM_POINTS                      127
#define IDM_SHOW_RECIPROCAL             128
#define IDM_LIN			                129
#define IDM_LOG				            130
#define IDM_ZOOM						131
#define IDM_ARROW						132
#define IDM_LINE						133
#define IDM_SUR                         140
#define IDM_BUL                         141
#define IDM_BOT                         142
#define IDM_DATA                        143
#define IDM_ERROR                       144

#define IDM_SCALE_MOVE					200



// The TBBUTTON structure contains information about the toolbar buttons.
static TBBUTTON tbButton[] =      
{
  {0,  IDM_FILE_OPEN,		TBSTATE_ENABLED,										TBSTYLE_BUTTON,   0, -1},
  {1,  IDM_FILE_SAVE,		TBSTATE_ENABLED,										TBSTYLE_BUTTON,   0, -1},
  {2,  IDM_PRINT,			TBSTATE_ENABLED,										TBSTYLE_BUTTON,	  0, -1},
  {3,  IDM_2D_SETTINGS,		TBSTATE_ENABLED,										TBSTYLE_BUTTON,   0, -1},
  {0,  0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1},
  {4,  IDM_GRID2D,			TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, -1},
  {0,  0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1},
  {5,  IDM_MOVI,				TBSTATE_ENABLED,										TBSTYLE_BUTTON,   0, -1},
  {0,  0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1},
  {6,  IDM_ARROW,			TBSTATE_ENABLED | TBSTATE_CHECKED,						BTNS_CHECKGROUP , 1, -1},
  {7,  IDM_ZOOM,				TBSTATE_ENABLED,										BTNS_CHECKGROUP,  0, -1},
  {8,  IDM_SCALE_MOVE,		TBSTATE_ENABLED,										BTNS_CHECKGROUP,  0, -1},
  {0,  0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1},
  {9,  IDM_LIN,				TBSTATE_ENABLED,										BTNS_CHECKGROUP,  0, -1},
  {10, IDM_LOG,				TBSTATE_ENABLED | TBSTATE_CHECKED,						BTNS_CHECKGROUP,  0, -1},
  {0, 0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1},
  {11, IDM_LINE,			TBSTATE_ENABLED | TBSTATE_CHECKED,						BTNS_CHECKGROUP,  0, -1},
  {12, IDM_POINTS,			TBSTATE_ENABLED,										BTNS_CHECKGROUP,  0, -1},
  {0, 0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1},
  {13, IDM_SUR,				TBSTATE_ENABLED,										TBSTYLE_CHECK,    0, -1},
  {14, IDM_BUL,				TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, -1},
  {15, IDM_BOT,				TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, -1},
  {0, 0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1},
  {16, IDM_DATA,			TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, -1},
  {17, IDM_ERROR,			TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    0, -1},
  {0, 0,					TBSTATE_ENABLED,										TBSTYLE_SEP,      0, -1}

};



void Tooltip2D(LPARAM lParam){

	LPTOOLTIPTEXT lpttt =( LPTOOLTIPTEXT ) lParam;
	switch( lpttt->hdr.idFrom )
	{
 		case IDM_FILE_OPEN:
			lpttt->lpszText = L"Open files";
			break;
		case IDM_FILE_SAVE:
			lpttt->lpszText = L"Save files";
			break;
		case IDM_PRINT:
			   lpttt->lpszText = L"Print the graph";
			   break;
		case IDM_2D_SETTINGS:
			   lpttt->lpszText = L"2D window settings";
			   break;
		case IDM_GRID2D:
			   lpttt->lpszText = L"Add remove grid from graph";
			   break;
		case IDM_POINTS:
			   lpttt->lpszText = L"Show points";
			   break;
		case IDM_LIN:
			   lpttt->lpszText = L"Set linear scale";
			   break;
		case IDM_LOG:
			   lpttt->lpszText = L"Set log scale";
			   break;
//-----------------------Windows-------------------------------------------------
		case IDM_ZOOM:
			   lpttt->lpszText = L"Zoom selected area";
			   break;
		case IDM_ARROW:
			   lpttt->lpszText = L"Investigate selected point";
			   break;
		case IDM_LINE:
			   lpttt->lpszText = L"Connect points by line";
			   break;
		case  IDM_SUR:
			   lpttt->lpszText = L"Shows surface model";
			   break;
		case  IDM_BOT:
			   lpttt->lpszText = L"Shows both bulk and surface interference sum of model";
			   break;
		case  IDM_BUL:
			   lpttt->lpszText = L"Shows bulk model";
			   break;
		case  IDM_DATA:
			   lpttt->lpszText = L"Shows data";
			   break;
		case  IDM_ERROR:
			   lpttt->lpszText = L"Shows error bars";
			   break;
		case  IDM_SCALE_MOVE:
			   lpttt->lpszText = L"Move scale";
			   break;
	}
}
