//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header for WinRod.cpp. 
//
// Writen by Daniel Kaminski 
//
// 01/01/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#pragma once


#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#define THREAD
#define VARIABLES

//--------------------------------------------------------------------------------------------------------------------------------
#include <windows.h>
#include <Windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <commctrl.h>
#include <Commdlg.h >
#include "resource.h"
#include "structures.h"
#include "thread.h"		

#if defined(_DEBUG)
#include <crtdbg.h>
#endif

typedef LRESULT    (CALLBACK *WIN_PROC )(HWND,UINT,WPARAM,LPARAM);
//-----------------------------------------------------------------------------
// Function Prototypes.
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window0Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window1Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window2Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window3Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window4Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window5Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window6Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window7Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window8Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Window9Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



bool RegisterMyClass(WNDCLASSEX *wcl, HINSTANCE hInstance, WCHAR *ClassTitle, WIN_PROC WindowProc, int IDC_MENU = 0 );

void DrawStatusBar(HWND, HDC, POINT, RECT);
void GetWindowsSize(bool correction = true);
void AddRodsToComboMain();
void UpdateAllWindows(bool upate_sliders_flag=true);
void ReopenSlidersWindow();
void WinRod_init(HWND hWnd);
bool ReadCommandLineArguments();
void RestartWindow(int i);
void CopyRect(int i, RECT rect);
void InitStatusBar();
void CascadeWindows();
void CafleWindows();
void MiniaturesWindows();
void DefaultWindows(int Style, int windows_nr=0);
void SizeWindow(int nr, int St[][4], double CoorX, double CorrY);
void StatusBarsUpdate(void);
void Shutdown();
void UpdateStatusBar();
void test(void);

//3D window thread functions
void StartThread3D(Thread_t* lpThread);
void StopThreads3D();
DWORD WINAPI  ThreadProc3D(LPVOID lpThread_);
void RestartDX_Window(bool store_size = false);