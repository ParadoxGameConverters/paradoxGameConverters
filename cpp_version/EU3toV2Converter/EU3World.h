#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include "EU3Country.h"
#include "EU3Diplomacy.h"


enum WorldType
{
	Unknown = 0,
	VeryOld,
	InNomine,
	HeirToTheThrone,
	DivineWind
};

typedef map< int, vector<int> > provinceMapping; // prevents circular dependency


class EU3World {
	public:
		EU3World() : cachedWorldType(Unknown) {};
		void						init(Object* obj);
		vector<EU3Country>	getCountries();
		EU3Country*				getCountry(string);
		EU3Province*			getProvince(int provNum);
		void						removeCountry(string tag);
		void						removeCountries(vector<string>& tags);
		EU3Diplomacy&			getDiplomacy();
		void					resolveRegimentTypes(const RegimentTypeMap& map);
		WorldType				getWorldType();
		void						checkAllProvincesMapped(provinceMapping provinceMap);
	private:
		WorldType				cachedWorldType;
		vector<EU3Province>	provinces;
		vector<EU3Country>	countries;
		EU3Diplomacy	diplomacy;
};


#endif // EU3WORLD_H_
