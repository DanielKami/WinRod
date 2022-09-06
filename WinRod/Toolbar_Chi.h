#define MENU_HEIGHT			17
#define NUMIMAGES			4		//Number of buttons in the 3D toolbar

#define IMAGEWIDTH			16		// Width of the buttons in the toolbar  
#define IMAGEHEIGHT			16		// Height of the buttons in the toolbar  
#define BUTTONWIDTH			16		// Width of the button images in the toolbar
#define BUTTONHEIGHT		16		// Height of the button images in the toolbar

// Identifier of the main tool bar
#define dwTBFontStyle TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_GROUP


#define IDM_FILE_SAVE                   621
#define IDM_PRINT		                622
#define IDM_COPY_TO_CLIPOBARD			624

// The TBBUTTON structure contains information about the toolbar buttons.
static TBBUTTON tbButton[] =
{
	{ 1,  IDM_FILE_SAVE,         TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 },
	{ 2,  IDM_PRINT,             TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 },
	{ 26, IDM_COPY_TO_CLIPOBARD, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 },
};





void TooltipChi(LPARAM lParam)
{
	LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
	switch (lpttt->hdr.idFrom)
	{

	case IDM_FILE_SAVE:
		lpttt->lpszText = L"Save files";
		break;
	case IDM_PRINT:
		lpttt->lpszText = L"Print the graph";
		break;
	case IDM_COPY_TO_CLIPOBARD:
		lpttt->lpszText = L"Copy to clipobard";
		break;
	}
}
