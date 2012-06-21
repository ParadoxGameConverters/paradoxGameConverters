#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include "EU3Army.h"

class EU3Country;
class EU3Province;
class EU3Diplomacy;
struct EU3Agreement;
typedef map< int, vector<int> > provinceMapping; // < destProvince, sourceProvinces >



enum WorldType
{
	Unknown = 0,
	VeryOld,
	InNomine,
	HeirToTheThrone,
	DivineWind
};


class EU3World {
	public:
		EU3World();
		void								init(Object* obj);
		map<string, EU3Country*>	getCountries();
		EU3Country*						getCountry(string);
		EU3Province*					getProvince(int provNum);
		void								removeCountry(string tag);
		void								removeCountries(vector<string>& tags);
		EU3Diplomacy*					getDiplomacy();
		void								resolveRegimentTypes(const RegimentTypeMap& map);
		WorldType						getWorldType();
		void								checkAllProvincesMapped(provinceMapping provinceMap);
	private:
		WorldType						cachedWorldType;
		vector<EU3Province*>			provinces;
		map<string, EU3Country*>	countries;
		//vector<EU3Country*>	countries;
		EU3Diplomacy*					diplomacy;
};



#endif // EU3WORLD_H_
