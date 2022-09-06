

// Identifier of the main tool bar
#define dwTBFontStyle TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_GROUP
// Style of the Font toolbar

#define ID_UNDO		        2500
#define ID_REDO				2501




// The TBBUTTON structure contains information about the toolbar buttons.
static TBBUTTON tbButtonsEdit[] =
{
	{ 0, ID_UNDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 },
	{ 1, ID_REDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, (BYTE)0, (BYTE)-1 }
};