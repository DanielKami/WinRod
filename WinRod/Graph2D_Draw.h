#pragma once
#include "2DGraph.h"

class Graph2D_Draw :
	public Graph2D
{
public:
	Graph2D_Draw();
	~Graph2D_Draw();


	static void FillProperitiesList(HWND  hItemList, Data_struct*);
};

