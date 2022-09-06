#define MENU_HEIGHT			19
#define SIDE_BAR_WIDTH      28
#define NUMIMAGES2D_HOR		19		//Number of buttons in the 3D toolbar
#define NUMIMAGES2D_VERT	11		//Number of buttons in the 3D toolbar
#define IMAGEWIDTH			16		// Width of the buttons in the toolbar  
#define IMAGEHEIGHT			16		// Height of the buttons in the toolbar  
#define BUTTONWIDTH			0		// Width of the button images in the toolbar
#define BUTTONHEIGHT		0		// Height of the button images in the toolbar

 // Identifier of the main tool bar
#define dwTBFontStyle TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_GROUP
                          

#define IDM_FILE_OPEN                   620
#define IDM_FILE_SAVE                   621
#define IDM_PRINT		                622
#define IDM_2D_SETTINGS                 623
#define IDM_GRID2D	                    624
#define IDM_MOVI                        625
#define IDM_EXPAND                      626
#define IDM_EXPAND_DATA					627
#define IDM_EXPAND_SIMPLE				628
#define IDM_POINTS                      629
#define IDM_SHOW_RECIPROCAL             630
#define IDM_LIN			                631
#define IDM_LOG				            632
#define IDM_ZOOM						633
#define IDM_ARROW						634
#define IDM_LINE						635
#define IDM_SUR                         640
#define IDM_BUL                         641
#define IDM_BOT                         642
#define IDM_PHASE                       643
#define IDM_DATA                        645
#define IDM_ERROR                       646
#define IDM_OPEN_DATA					647
#define IDM_OPEN_FIT					648
#define IDM_ADD_DATA					649
#define IDM_ADD_FIT						650
#define IDM_SCALE_MOVE					651
#define IDM_SHOW_CURVE					652
#define IDM_SHOW_FI						653
#define IDM_SHOW_FII					654
#define IDM_COPY_TO_CLIPOBARD			660
#define IDM_LBRAGG						662




// The TBBUTTON structure contains information about the toolbar buttons.
static TBBUTTON tbButton[] =      
{
  {	0,  IDM_FILE_OPEN,			TBSTATE_ENABLED,										TBSTYLE_BUTTON,   (BYTE)0, (BYTE)-1},
  { 1, IDM_FILE_SAVE,			TBSTATE_ENABLED,										TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 },
  {	2,  IDM_PRINT,				TBSTATE_ENABLED,										TBSTYLE_BUTTON,	  (BYTE)0, (BYTE)-1},
  { 26, IDM_COPY_TO_CLIPOBARD,	TBSTATE_ENABLED,										TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 },
  {	3,  IDM_2D_SETTINGS,		TBSTATE_ENABLED,										TBSTYLE_BUTTON,   (BYTE)0, (BYTE)-1},
  {	0, 0,						TBSTATE_ENABLED,										TBSTYLE_SEP,      (BYTE)0, (BYTE)-1},
  {	13, IDM_SUR,				TBSTATE_ENABLED,										TBSTYLE_CHECK,    (BYTE)0, (BYTE)-1},
  {	14, IDM_BUL,				TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    (BYTE)0, (BYTE)-1},
  {	15, IDM_BOT,				TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    (BYTE)0, (BYTE)-1},
  { 27, IDM_PHASE,				TBSTATE_ENABLED,										TBSTYLE_CHECK, (BYTE)0, (BYTE)-1 },
  { 22, IDM_SHOW_CURVE,			TBSTATE_HIDDEN | TBSTATE_PRESSED | TBSTATE_CHECKED,		TBSTYLE_CHECK,	  (BYTE)0, (BYTE)-1 },
  { 23, IDM_SHOW_FI,			TBSTATE_HIDDEN | TBSTATE_PRESSED | TBSTATE_CHECKED,		TBSTYLE_CHECK, (BYTE)0, (BYTE)-1 },
  { 24, IDM_SHOW_FII,			TBSTATE_HIDDEN | TBSTATE_PRESSED | TBSTATE_CHECKED,		TBSTYLE_CHECK, (BYTE)0, (BYTE)-1 },
  {	0, 0,						TBSTATE_ENABLED,										TBSTYLE_SEP,      (BYTE)0, (BYTE)-1},
  {	16, IDM_DATA,				TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    (BYTE)0, (BYTE)-1},
  {	17, IDM_ERROR,				TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED,	TBSTYLE_CHECK,    (BYTE)0, (BYTE)-1},
  { 28,	IDM_LBRAGG,				TBSTATE_HIDDEN | TBSTATE_PRESSED | TBSTATE_CHECKED,		TBSTYLE_CHECK, (BYTE)0, (BYTE)-1 },
  {	0, 0,						TBSTATE_ENABLED,										TBSTYLE_SEP,      (BYTE)0, (BYTE)-1},
  { 18, IDM_OPEN_DATA,			TBSTATE_ENABLED, 										TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 },
  { 19, IDM_OPEN_FIT,			TBSTATE_ENABLED,										TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 },
  { 20, IDM_ADD_DATA,			TBSTATE_ENABLED,										TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 },
  { 21, IDM_ADD_FIT,			TBSTATE_ENABLED,										TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 },
  { 0, 0,						TBSTATE_ENABLED,										TBSTYLE_SEP,		(BYTE)0, (BYTE)-1 }
 
};

// The TBBUTTON structure contains information about the toolbar buttons.
static TBBUTTON tbButtonVert[] =
{
	{ 0, 0,										TBSTATE_ENABLED | TBSTATE_WRAP,									TBSTYLE_SEP,					0, (BYTE)-1 },
	{ 6,	IDM_ARROW,							TBSTATE_ENABLED | TBSTATE_CHECKED | TBSTATE_WRAP,				BTNS_CHECKGROUP, 1, (BYTE)-1 },
	{ 7,	IDM_ZOOM,							TBSTATE_ENABLED | TBSTATE_WRAP,									BTNS_CHECKGROUP, 0, (BYTE)-1 },
	{ 8,	IDM_SCALE_MOVE,						TBSTATE_ENABLED | TBSTATE_WRAP,									BTNS_CHECKGROUP, 0, (BYTE)-1 },
	{ 0, 0,										TBSTATE_ENABLED | TBSTATE_WRAP,									TBSTYLE_SEP, 0, (BYTE)-1 },
	{ 5,	IDM_EXPAND,							TBSTATE_ENABLED | TBSTATE_CHECKED | TBSTATE_WRAP,				BTNS_CHECKGROUP, 0, (BYTE)-1 },
	{ 25,	IDM_EXPAND_DATA,					TBSTATE_ENABLED | TBSTATE_WRAP,									BTNS_CHECKGROUP, 0,(BYTE)-1 },
	{ 5,	IDM_EXPAND_SIMPLE,					TBSTATE_ENABLED | TBSTATE_WRAP,									TBSTYLE_BUTTON, 0,(BYTE)-1 },
	{ 0, 0,										TBSTATE_ENABLED | TBSTATE_WRAP,									TBSTYLE_SEP, 0, (BYTE)-1 },
	{ 9,	IDM_LIN,							TBSTATE_ENABLED | TBSTATE_WRAP,									BTNS_CHECKGROUP, 0,(BYTE)-1 },
	{ 10,	IDM_LOG,							TBSTATE_ENABLED | TBSTATE_CHECKED | TBSTATE_WRAP,				BTNS_CHECKGROUP, 0, (BYTE)-1 },
	{ 0, 0,										TBSTATE_ENABLED | TBSTATE_WRAP,									TBSTYLE_SEP, 0,(BYTE)-1 },
	{ 4,	IDM_GRID2D,							TBSTATE_ENABLED | TBSTATE_PRESSED | TBSTATE_CHECKED | TBSTATE_WRAP, TBSTYLE_CHECK, 0, (BYTE)-1 },
	{ 0, 0,										TBSTATE_ENABLED | TBSTATE_WRAP,									TBSTYLE_SEP, 0, (BYTE)-1 },
	{ 11,	IDM_LINE,							TBSTATE_ENABLED | TBSTATE_WRAP,									BTNS_CHECKGROUP, 0, (BYTE)-1 },
	{ 12,	IDM_POINTS,							TBSTATE_ENABLED | TBSTATE_CHECKED |TBSTATE_WRAP,				BTNS_CHECKGROUP, 0, (BYTE)-1 }
};



//Combo open
static TBBUTTON tbButton_open[] =
{
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP,0, (BYTE)-1 }
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
		case  IDM_PHASE:
			   lpttt->lpszText = L"Shows phase";
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
		case  IDM_EXPAND:
			lpttt->lpszText = L"Fit scale to data and the curve";
			break;
		case  IDM_EXPAND_DATA:
			lpttt->lpszText = L"Fit scale only to data";
			break;
			
		case  IDM_OPEN_DATA:
			lpttt->lpszText = L"Added points dialog box";
			break;
		case  IDM_OPEN_FIT:
			lpttt->lpszText = L"Added curve dialog box";
			break;
		case  IDM_ADD_DATA:
			lpttt->lpszText = L"Add points to stored points";
			break;
		case  IDM_ADD_FIT:
			lpttt->lpszText = L"Add fit curve to stored curves";
			break;

		case  IDM_SHOW_CURVE:
			lpttt->lpszText = L"Show electron density profile";
			break;

		case  IDM_SHOW_FI:
			lpttt->lpszText = L"Show f' coefficient";
			break;

		case  IDM_SHOW_FII:
			lpttt->lpszText = L"Show f'' coefficient";
			break;

		case  IDM_LBRAGG:
			lpttt->lpszText = L"Indicate by arrow position of LBragg from data file";
			break;
			
	}
}
