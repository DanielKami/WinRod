//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: Header for command line. Provides all the necessary function declarations located in cml.cpp
// Writen by Daniel Kaminski    
//
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------
#pragma once

#define MAX_LEVEL_MENU 10
#define MAX_KEY_WORDS  20

#define LEVEL0 0
#define LEVEL1 1
#define LEVEL2 2
#define LEVEL3 3
#define LEVEL4 4
#define LEVEL5 5

#define KEYWORD_EMPTY   0       //Menu is empty we can show the welcome screen

// Menu       LEVEL1              LEVEL2                 LEVEL3 
#define		KEYWORD_SET 1
#define		KEYWORD_FIT 2
#define		KEYWORD_PLOT 3
#define		KEYWORD_CALC 4
#define		KEYWORD_READ 5
#define		KEYWORD_SAVE 6
#define		KEYWORD_LIST 7
#define		KEYWORD_HELP  8

//Menu calc
#define		KEYWORD_CALC_RO		41
#define		KEYWORD_CALC_INP	42
#define		KEYWORD_CALC_ELL	43
#define		KEYWORD_CALC_PATT	44

//Menu list
#define		KEYWORD_LIST_SURF 71
#define		KEYWORD_LIST_BULK 72
#define		KEYWORD_LIST_HELP  73

//Fit
#define		KEYWORD_FIT_HELP	21
#define		KEYWORD_FIT_LIST    22

struct Menu
{
int At[MAX_LEVEL_MENU];
int level;
};






//------------------------------------------------------------------------------------------------------------------------------------
class Cml
{

private:

	
		//Remove the first command in e qiue after interpretation
		bool RemoveCommand(WCHAR*);

		//Main command draw window. In this window all list windows
		void CmlDraw(HWND, HDC, bool, WCHAR* );
		// Command line
		void  CmlLine(HWND hwnd, HDC hdcBack, bool carret, WCHAR *command_linetext  );
		//Shows lists
		void WelcomeCMD(HWND hwnd, HDC);
		void ListParametersFit(HDC);
		void ListParametersSet(HDC);
		void ListParametersListBulk(HDC);
		void ListParametersListSurf(HDC);
		void ListParametersAsk(HDC);

		//List menu
		void ListParametersListAsk(HDC);

		//List menu
		void ListParametersFitHelp(HDC);


		//Used in list windows to make them more atractive
		bool Change_parameters(bool, WCHAR*, HDC );

		//just create the string readable for humans from numbers included in menu structure.
		bool ComposeMenuTreeText(WCHAR *MenuText);


	//Level 1
	bool interpretate_empty(HWND, WCHAR*);
	bool interpretate_set  (HWND, WCHAR*);
	bool interpretate_fit  (HWND, WCHAR*);
	bool interpretate_plot (HWND, WCHAR*);
	bool interpretate_calc (HWND, WCHAR*);
	bool interpretate_read (HWND, WCHAR*);
	bool interpretate_save (HWND, WCHAR*);
	bool interpretate_list (HWND, WCHAR*);

	//Level 2 of calculate
	bool interpretate_rod      (HWND, WCHAR*);
	bool interpretate_in_plane (HWND, WCHAR*);

	//level 2 list
	bool interpretate_list_surface(HWND, WCHAR*);
	bool interpretate_list_bulk(HWND, WCHAR*);





public:
		WCHAR FeedbackStr[500];

		HWND CreateAppWindow(const WNDCLASSEX &wcl, WCHAR *pszTitle, int left, int top, int halfScreenWidth, int halfScreenHeight );
		//Call back function to interact with command line window
		LRESULT CALLBACK ProcessCmlInput(HWND, UINT, WPARAM, LPARAM);
		//Interprete the rught keyboard line (later this can be extended to file lines
		bool Interprete_line(HWND, WCHAR* );
		//Interprete command according to the defined operation. The KEYWORD defines the shortcut, menu level and interpretation function
		bool CommandInterprete(HWND hwnd, WCHAR* String, WCHAR* Command, WCHAR* ConstString, int LEVEL, int KEYWORD);
};

#ifndef CML
	extern Cml cml; //Variables not used by threads the registration is in WinRod.cpp
#else
	Cml cml;
#undef CML
#endif




