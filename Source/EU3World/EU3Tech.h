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

		double	getGovernmentBaseCost(date startDate, int level) const;
		double	getProductionBaseCost(date startDate, int level) const;
		double	getTradeBaseCost(date startDate, int level) const;
		double	getNavalBaseCost(date startDate, int level) const;
		double	getLandBaseCost(date startDate, int level) const;

		double	getGroupModifier(string group) const;
	private:
		map<string, pair<int, double> >	groups; // name, start level, modifier
		map<string, vector<double> >		startingTechLevels; // tech group, tech levels
		vector<int>								governmentYears;
		vector<int>								productionYears;
		vector<int>								tradeYears;
		vector<int>								navalYears;
		vector<int>								landYears;
};



#endif // EU3TECH_H_