#pragma once

//http://www.codeproject.com/Articles/8628/Retrieving-version-information-from-your-local-app
class CGlobalFunctions
{
public:
	CGlobalFunctions();
	virtual ~CGlobalFunctions();

public:
	WCHAR*	GetVersionNumber();


private:
	WCHAR csRet[1000];
	void GetVersionInfo(WCHAR *csEntry);

};

