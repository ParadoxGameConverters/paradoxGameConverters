#ifndef EU4WORLD_H_
#define EU4WORLD_H_


#include "EU4Army.h"

class EU4Country;
class EU4Province;
class EU4Diplomacy;
class EU4Version;
struct EU4Agreement;
typedef map< int, vector<int> > inverseProvinceMapping; // < sourceProvince, destProvinces >



class EU4World {
	public:
		EU4World(Object* obj);
		EU4Country*						getCountry(string tag) const;
		EU4Province*					getProvince(int provNum) const;
		void								removeCountry(string tag);
		void								resolveRegimentTypes(const RegimentTypeMap& map);
		void								checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const;

		map<string, EU4Country*>	getCountries()	const { return countries; };
		EU4Diplomacy*					getDiplomacy()	const { return diplomacy; };
	private:
		map<int, EU4Province*>		provinces;
		map<string, EU4Country*>	countries;
		EU4Diplomacy*					diplomacy;
		EU4Version*						version;
};



#endif // EU4WORLD_H_
