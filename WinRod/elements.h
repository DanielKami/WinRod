//--------------------------------------------------------------------------------------------------------------------------------
// Purpose: The header file contains all functions prototypes defined in elements.cpp
//
// Oryginally part of ROD writen by Prof. Elias Vlieg
// Modified by Daniel Kaminski                                                                                                     
// 12/02/2013 Lublin																											 
//--------------------------------------------------------------------------------------------------------------------------------

#define MAX_ELEMENTS 105

void InitializeScateringFactors();
double *get_coeff(char el[3], double coeff[9]);
int    get_element_number(char el[3]);



    static char elements[][3] =
	{
	"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg",
	"Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr",
	"Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br",
	"Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd",
	"Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La",
	"Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er",
	"Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au",
	"Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th",
	"Pa","U","Np","Pu","Am","Cm","Bk","Cf",
	"E1", "E2", "E3", "E4", "E5" /* fake elements for general purposes */
	};

	//For acceleration
	static WCHAR W_elements[][3] =
	{ L"",
		L"H", L"He", L"Li", L"Be", L"B", L"C", L"N", L"O", L"F", L"Ne", L"Na", L"Mg",
		L"Al", L"Si", L"P", L"S", L"Cl", L"Ar", L"K", L"Ca", L"Sc", L"Ti", L"V", L"Cr",
		L"Mn", L"Fe", L"Co", L"Ni", L"Cu", L"Zn", L"Ga", L"Ge", L"As", L"Se", L"Br",
		L"Kr", L"Rb", L"Sr", L"Y", L"Zr", L"Nb", L"Mo", L"Tc", L"Ru", L"Rh", L"Pd",
		L"Ag", L"Cd", L"In", L"Sn", L"Sb", L"Te", L"I", L"Xe", L"Cs", L"Ba", L"La",
		L"Ce", L"Pr", L"Nd", L"Pm", L"Sm", L"Eu", L"Gd", L"Tb", L"Dy", L"Ho", L"Er",
		L"Tm", L"Yb", L"Lu", L"Hf", L"Ta", L"W", L"Re", L"Os", L"Ir", L"Pt", L"Au",
		L"Hg", L"Tl", L"Pb", L"Bi", L"Po", L"At", L"Rn", L"Fr", L"Ra", L"Ac", L"Th",
		L"Pa", L"U", L"Np", L"Pu", L"Am", L"Cm", L"Bk", L"Cf",
		L"E1", L"E2", L"E3", L"E4", L"E5" /* fake elements for general purposes */
	};