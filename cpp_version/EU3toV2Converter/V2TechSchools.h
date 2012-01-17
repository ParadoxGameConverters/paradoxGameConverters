#ifndef V2_TECH_SCHOOLS
#define V2_TECH_SCHOOLS



#include <string>
#include "Parsers\Object.h"



typedef struct techSchool
{
	string name;
	double armyInvestment;
	double commerceInvestment;
	double cultureInvestment;
	double industryInvestment;
	double navyInvestment;
} techSchool;


vector<techSchool> initTechSchools(Object*);



#endif // V2_TECH_SCHOOLS