#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include "EU3Army.h"

class EU3Country;
class EU3Province;
class EU3Diplomacy;
struct EU3Agreement;
typedef map< int, vector<int> > inverseProvinceMapping; // < sourceProvince, destProvinces >



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
		EU3World(Object* obj);
		EU3Country*						getCountry(const string tag);
		EU3Province*					getProvince(const int provNum);
		void								removeCountry(const string tag);
		void								resolveRegimentTypes(const RegimentTypeMap& map);
		WorldType						getWorldType();
		void								checkAllProvincesMapped(const inverseProvinceMapping inverseProvinceMap);

		map<string, EU3Country*>	getCountries()	const { return countries; };
		EU3Diplomacy*					getDiplomacy()	const { return diplomacy; };
	private:
		WorldType						cachedWorldType;
		map<int, EU3Province*>		provinces;
		map<string, EU3Country*>	countries;
		EU3Diplomacy*					diplomacy;
};



#endif // EU3WORLD_H_
