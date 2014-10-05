#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include <istream>
#include "EU3Army.h"

class EU3Country;
class EU3Province;
class EU3Diplomacy;
class EU3Localisation;
struct EU3Agreement;
typedef map< int, vector<int> > inverseProvinceMapping; // < sourceProvince, destProvinces >



enum WorldType
{
	unknown = 0,
	VeryOld,
	InNomine,
	HeirToTheThrone,
	DivineWind
};


class EU3World {
	public:
		EU3World(EU3Localisation&, Object* obj);

		void	readCommonCountries(istream&, const std::string& rootPath);
		void	readCountryLocalisation(istream&);

		EU3Country*						getCountry(string tag) const;
		EU3Province*					getProvince(int provNum) const;
		void								removeCountry(string tag);
		void								resolveRegimentTypes(const RegimentTypeMap& map);
		WorldType						getWorldType();
		void								checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const;

		map<string, EU3Country*>	getCountries()	const { return countries; };
		EU3Diplomacy*					getDiplomacy()	const { return diplomacy; };
	private:
		WorldType						cachedWorldType;
		map<int, EU3Province*>		provinces;
		map<string, EU3Country*>	countries;
		EU3Diplomacy*					diplomacy;
};



#endif // EU3WORLD_H_
