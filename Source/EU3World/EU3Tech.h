#ifndef EU3TECH_H_
#define EU3TECH_H_



#include "..\Parsers\Object.h"
#include "..\Date.h"



class EU3Tech
{
	public:
		EU3Tech(date startDate, Object* mainObj, Object* governmentObj, Object* productionObj, Object* tradeObj, Object* navalObj, Object* landObj);

		double	getGovernmentTech(string techGroup) const;
		double	getProductionTech(string techGroup) const;
		double	getTradeTech(string techGroup) const;
		double	getNavalTech(string techGroup) const;
		double	getLandTech(string techGroup) const;
	private:
		map<string, vector<double> > startingTechLevels; // tech group, tech levels
};



#endif // EU3TECH_H_