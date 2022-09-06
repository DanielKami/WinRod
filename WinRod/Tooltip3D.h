
#pragma once

void Tooltip3D(LPARAM lParam)
{
	LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
	switch (lpttt->hdr.idFrom)
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
	case ID_CELL:
		lpttt->lpszText = L"Add surface cell frame";
		break;
	case ID_PERSPECTIVE:
		lpttt->lpszText = L"Apply perspective";
		break;
	case ID_SURFACEI_II:
		lpttt->lpszText = L"Select surface 1 or 2. Prest is surface 1.";
		break;
	case ID_CHARGES:
		lpttt->lpszText = L"Change between atom colors and charges colors.";
		break;
	}
}
