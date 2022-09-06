
void TooltipMain(LPARAM lParam){

	LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
	switch (lpttt->hdr.idFrom)
	{
	case ID_NEW_MAIN:
		lpttt->lpszText = L"Reset all settings";
		break;
	case ID_FILE_OPEN_MAIN:
		lpttt->lpszText = L"Open files";
		break;
	case ID_FILE_SAVE_MAIN:
		lpttt->lpszText = L"Save files";
		break;
	case IDM_FIT:
		lpttt->lpszText = L"Fit the model to data and edit parameters";
		break;
	case IDM_Z_Ell_PROFILE:
		lpttt->lpszText = L"Electron density profile settings";
		break;
	case IDM_KEATING:
		lpttt->lpszText = L"Keating, this option helps to keap atoms distances and angles within physical limits";
		break;
	case IDM_EDIT_FIT:
		lpttt->lpszText = L"Edit fit file";
		break;
	case IDM_EDIT_BULK:
		lpttt->lpszText = L"Edit bulk file";
		break;
		//-----------------------Windows-------------------------------------------------
	case ID_3D:
		lpttt->lpszText = L"3D scene window";
		break;
	case ID_2D_ROD:
		lpttt->lpszText = L"2D window with rod data and model";
		break;
	case ID_2D_IN_PLANEFILE:
		lpttt->lpszText = L"2D window with in-plande data and model";
		break;
	case  ID_CML:
		lpttt->lpszText = L"Command line window";
		break;
	case  ID_FOURIER:
		lpttt->lpszText = L"Fourier 2D window";
		break;
	case  ID_SLIDERS:
		lpttt->lpszText = L"Sliders for manual setting of parameters";
		break;

	case  ID_CHI2:
		lpttt->lpszText = L"Shows the normalised Chi square value";
		break;

	case  ID_RESIDUALS_BUTTON:
		lpttt->lpszText = L"Shows the residuals normalised to number of data points";
		break;

	case  ID_MULTIPLE:
		lpttt->lpszText = L"Show multiplot window";
		break;

		//-----------------------------------------------------------------------------------
		
	}
}
