//--------------------------------------------------------------------------------------------------------------------------------
//Created by D. Kaminski
// 08.07.2014
//Purpose: Undo/Redo action
//--------------------------------------------------------------------------------------------------------------------------------

#include <stdafx.h>				//Header necessary for Visual Studio
#include <windows.h>
#include <commctrl.h>
#include "definitions.h"
#include "Thread.h"
#include "variables.h"
#include "UndoRedo.h"
#include "ToolbarEdit.h"



void UndoRedo::AddNewRecord(WCHAR* add_text)
{
	CurentPosition++;
	if (CurentPosition >= MEMORY_STEPS) CurentPosition = 0;
	if (CurentPosition >MaximalPosition) MaximalPosition = CurentPosition;

	CopyMemory(&storeage_thread[CurentPosition], ptr_thread, sizeof(Thread_t));
	CopyMemory(&storage_variable[CurentPosition], ptr_variable, sizeof(Variable_t));
	//CopyMemory(&(WCHAR*)storage_text[CurentPosition], &(WCHAR*)add_text, sizeof(WCHAR)*50);
	swprintf_s(storage_text[CurentPosition], MEMORT_TEXT_SIZE, L"%s", add_text);
	Buttons();
}


void UndoRedo::Undo()
{
	CurentPosition--;
	if (CurentPosition < 0) CurentPosition = 0;

	CopyMemory(ptr_thread, &storeage_thread[CurentPosition], sizeof(Thread_t));
	CopyMemory(ptr_variable, &storage_variable[CurentPosition], sizeof(Variable_t));
	Buttons();
}

void UndoRedo::Redo()
{
	CurentPosition++;
	if (CurentPosition > MEMORY_STEPS) CurentPosition = MEMORY_STEPS;
	if (CurentPosition > MaximalPosition) CurentPosition = MaximalPosition;
	if (MaximalPosition == 0) return;

	CopyMemory(ptr_thread, &storeage_thread[CurentPosition], sizeof(Thread_t));
	CopyMemory(ptr_variable, &storage_variable[CurentPosition], sizeof(Variable_t));
	Buttons();
}


void UndoRedo::Buttons()
{
	if (CurentPosition > 0)
		SendMessage(hWndTB4, TB_ENABLEBUTTON, ID_UNDO, TRUE);
	else
		SendMessage(hWndTB4, TB_ENABLEBUTTON, ID_UNDO, FALSE);

	if (CurentPosition < MaximalPosition)
		SendMessage(hWndTB4, TB_ENABLEBUTTON, ID_REDO, TRUE);
	else
		SendMessage(hWndTB4, TB_ENABLEBUTTON, ID_REDO, FALSE);

}

void UndoRedo::Initialize()
{
	WCHAR add_text[] = L"Beginning";
	CurentPosition = 0;
	CopyMemory(&storeage_thread[CurentPosition], ptr_thread, sizeof(Thread_t));
	CopyMemory(&storage_variable[CurentPosition], ptr_variable, sizeof(Variable_t));
//	CopyMemory((WCHAR*)storage_text[CurentPosition], add_text, sizeof(WCHAR));
	swprintf_s(storage_text[CurentPosition], MEMORT_TEXT_SIZE, L"%s", add_text);
}

void UndoRedo::Tooltip(LPARAM lParam)
{
	static WCHAR Wstr[MEMORT_TEXT_SIZE];

	LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
	switch (lpttt->hdr.idFrom)
	{
	case ID_UNDO:
		if (CurentPosition >= 0)
		{
			swprintf_s(Wstr, MEMORT_TEXT_SIZE, L"Undo %s", storage_text[CurentPosition]);
			lpttt->lpszText = Wstr;
		}
		break;
	case ID_REDO:
		if (CurentPosition + 1 <= MaximalPosition)
		{
			swprintf_s(Wstr, MEMORT_TEXT_SIZE, L"Redo %s", storage_text[CurentPosition + 1]);
			lpttt->lpszText = Wstr;
		}
		break;
	}
}