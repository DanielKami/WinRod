         

 // Identifier of the main tool bar
#define dwTBFontStyle TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_GROUP
                           // Style of the Font toolbar

#define ID_3D		            2000
#define ID_2D_ROD				2001
#define ID_2D_IN_PLANEFILE      2002
#define ID_CML			        2003
#define ID_FOURIER				2004
#define ID_SLIDERS			    2005
#define ID_CHI2				    2006
#define ID_RESIDUALS_BUTTON	    2007
#define ID_MULTIPLE			    2008

// The TBBUTTON structure contains information the toolbar buttons.
static TBBUTTON tbButton2[] =      
{
  {0, ID_3D,				TBSTATE_ENABLED, TBSTYLE_BUTTON,  0, (BYTE)-1},
  {1, ID_2D_ROD,			TBSTATE_ENABLED, TBSTYLE_BUTTON,  0, (BYTE)-1},
  {2, ID_2D_IN_PLANEFILE,	TBSTATE_ENABLED, TBSTYLE_BUTTON,  0, (BYTE)-1},
  {3, ID_CML,				TBSTATE_ENABLED, TBSTYLE_BUTTON,  0, (BYTE)-1},
  {4, ID_FOURIER,			TBSTATE_ENABLED, TBSTYLE_BUTTON,  0, (BYTE)-1},
  {8, ID_MULTIPLE,			TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 },
  {5, ID_SLIDERS,			TBSTATE_ENABLED, TBSTYLE_BUTTON,  0, (BYTE)-1},
  {6, ID_CHI2,				TBSTATE_ENABLED, TBSTYLE_BUTTON,  0, (BYTE)-1},
  {7, ID_RESIDUALS_BUTTON, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, (BYTE)-1 }
};
