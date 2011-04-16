#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_

#include <string>
#include <vector>
#include "Parsers\Object.h"
#include "EU3Province.h"
#include "log.h"
using namespace std;


class EU3Country
{
	public:
		void						init(Object* obj);
		string					getTag();
		void						addProvince(EU3Province*);
		vector<EU3Province*>	getProvinces();
		int						getCapital();
		string					getTechGroup();
		string					getPrimaryCulture();
		vector<string>			getAcceptedCultures();
		double					getLandTech();
		double					getNavalTech();
		double					getTradeTech();
		double					getProductionTech();
		double					getGovernmentTech();
	private:
		string					tag;
		vector<EU3Province*>	provinces;
		int						capital;
		string					techGroup;
		string					primaryCulture;
		vector<string>			acceptedCultures;
		double					landTech;
		double					navalTech;
		double					tradeTech;
		double					productionTech;
		double					governmentTech;
};


#endif // EU3COUNTRY_H_