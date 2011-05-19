#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_


#include "EU3Province.h"


class EU3Country
{
	public:
		void						init(Object* obj);
		string					getTag();
		void						addProvince(EU3Province*);
		void						addCore(EU3Province*);
		vector<EU3Province*>	getProvinces();
		vector<EU3Province*>	getCores();
		int						getCapital();
		string					getTechGroup();
		string					getPrimaryCulture();
		vector<string>			getAcceptedCultures();
		string					getReligion();
		double					getPrestige();
		double					getLandTech();
		double					getNavalTech();
		double					getTradeTech();
		double					getProductionTech();
		double					getGovernmentTech();
	private:
		string					tag;
		vector<EU3Province*>	provinces;
		vector<EU3Province*>	cores;
		int						capital;
		string					techGroup;
		string					primaryCulture;
		vector<string>			acceptedCultures;
		string					religion;
		double					prestige;
		double					landTech;
		double					navalTech;
		double					tradeTech;
		double					productionTech;
		double					governmentTech;
};


#endif // EU3COUNTRY_H_