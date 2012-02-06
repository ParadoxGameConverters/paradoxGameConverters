#ifndef V2WORLD_H_
#define V2WORLD_H_

#include "V2Country.h"
#include "V2Province.h"
#include "V2Diplomacy.h"


typedef map< int, vector<int> > provinceMapping; // prevents circular dependency


class V2World {
	public:
		void					init(Object* obj);
		vector<V2Country>		getCountries();
		V2Country*				getCountry(string);
		V2Province*				getProvince(int provNum);
		void					removeCountry(string tag);
		void					removeCountries(vector<string>& tags);
		V2Diplomacy&			getDiplomacy() { return diplomacy; };
		void					checkAllProvincesMapped(provinceMapping provinceMap);

	private:
		vector<V2Province>	provinces;
		vector<V2Country>	countries;
		V2Diplomacy	diplomacy;
};


#endif // V2WORLD_H_