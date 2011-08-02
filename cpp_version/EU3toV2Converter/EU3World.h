#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include "EU3Country.h"
#include "EU3Diplomacy.h"


class EU3World {
	public:
		void						init(Object* obj);
		vector<EU3Country>	getCountries();
		EU3Country*				getCountry(string);
		EU3Province*			getProvince(int provNum);
		void						removeCountry(string tag);
		EU3Diplomacy&			getDiplomacy();
		void					resolveRegimentTypes(const RegimentTypeMap& map);
	private:
		vector<EU3Province>	provinces;
		vector<EU3Country>	countries;
		EU3Diplomacy	diplomacy;
};


#endif // EU3WORLD_H_
