

void Tooltip3D(LPARAM lParam){

	LPTOOLTIPTEXT lpttt =( LPTOOLTIPTEXT ) lParam;
	switch( lpttt->hdr.idFrom )
	{
		case ID_EXPORT_3D:
			lpttt->lpszText = L"Export the 3D scene to graphisc formats";
			break;
 		case ID_PRINT_3D:
			lpttt->lpszText = L"Print the 3D scene";
			break;

		case ID_FULSCREAN_3D:
			   lpttt->lpszText = L"Shows 3D scene in full screan mode";
			   break;

		case ID_PROPERITIES_3D:
			   lpttt->lpszText = L"3D scene settings";
			   break;
		case ID_LASSO_3D:
			   lpttt->lpszText = L"Lasso selection method";
			   break;
		case ID_SELECT_3D:
			   lpttt->lpszText = L"Single atom selection method";
			   break;
		case ID_ROTATE_3D:
			   lpttt->lpszText = L"Scene rotation with arc ball";
			   break;
		case ID_MOVE_3D:
			   lpttt->lpszText = L"Translate 3D scene in x and y direction";
			   break;
		case ID_ZOOM_3D:
			   lpttt->lpszText = L"Cut off the z buffer (with left mouse button the front and with right mouse button the back";
			   break;
		case ID_LABELS_3D:
			   lpttt->lpszText = L"Add labels to atoms";
			   break;
		case ID_RESET_MEASUREMENT:
			   lpttt->lpszText = L"Delete all measurements";
			   break;
		case ID_BUL_3D:
			   lpttt->lpszText = L"Show bulk atom";
			   break;
		case ID_SUR_3D:
			   lpttt->lpszText = L"Show surface atom";
			   break;
		case ID_PLATE_3D:
			   lpttt->lpszText = L"Show z=0 plane";
			   break;
		case ID_COMPARE_3D:
			   lpttt->lpszText = L"Compare atomic positions before and after fitting";
			   break;
		case ID_TERMALRINGS_3D:
			   lpttt->lpszText = L"Add Debye-Waller rings";
			   break;


	}

}



////////////////////////////////////////////////////////////////////////////////////////////////////////
void TooltipMain(LPARAM lParam){

	LPTOOLTIPTEXT lpttt =( LPTOOLTIPTEXT ) lParam;
	switch( lpttt->hdr.idFrom )
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
			
	}
}
