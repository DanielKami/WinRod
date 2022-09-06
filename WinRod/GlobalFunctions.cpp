
//--------------------------------------------------------------------------------------------------------------------------------
//This file provide support for version information of the WinROd bases on the WinRod.exe file properties.
//--------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include <winver.h>
#include "GlobalFunctions.h"
#include <stdio.h>


CGlobalFunctions::CGlobalFunctions()
{

}

CGlobalFunctions::~CGlobalFunctions()
{

}
// This is the key method
void CGlobalFunctions::GetVersionInfo(WCHAR *csEntry)
{

	// = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
	HMODULE hLib = NULL;
	HRSRC hVersion = FindResource(hLib,
		MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (hVersion != NULL)
	{
		HGLOBAL hGlobal = LoadResource(hLib, hVersion);
		if (hGlobal != NULL)
		{

			LPVOID versionInfo = LockResource(hGlobal);
			if (versionInfo != NULL)
			{
				DWORD vLen, langD;
				BOOL retVal;

				LPVOID retbuf = NULL;

				static WCHAR fileEntry[256];

				wsprintf(fileEntry, L"\\VarFileInfo\\Translation");
				retVal = VerQueryValue(versionInfo, fileEntry, &retbuf, (UINT *)&vLen);
				if (retVal && vLen == 4)
				{
					memcpy(&langD, retbuf, 4);
					wsprintf(fileEntry, L"\\StringFileInfo\\%02X%02X%02X%02X\\%s",
						(langD & 0xff00) >> 8, langD & 0xff, (langD & 0xff000000) >> 24,
						(langD & 0xff0000) >> 16, csEntry);
				}
				else
					wsprintf(fileEntry, L"\\StringFileInfo\\%04X04B0\\%s",
					GetUserDefaultLangID(), csEntry);

				if (VerQueryValue(versionInfo, fileEntry, &retbuf, (UINT *)&vLen))
					if (vLen > 999) vLen = 999;//protection
					memcpy(csRet, retbuf, vLen*sizeof(WCHAR));
			}
		}
		if (hGlobal != NULL)
		{
			UnlockResource(hGlobal);
			FreeResource(hGlobal);
		}
	}

}

	WCHAR*	CGlobalFunctions::GetVersionNumber()
	{
		GetVersionInfo(L"ProductVersion");

		WCHAR *t = wcsrchr(csRet, '0');
		if (t != NULL)
		{
			wcscpy_s(t-3, 10, L"   ");
		}

		return csRet;
	}