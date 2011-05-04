#ifndef V2WORLD_H_
#define V2WORLD_H_


#include "V2Country.h"
#include "Mapper.h"


class V2World {
	public:
		void				init(Object* obj, string V2Loc);
		void				addPotentialCountries(ifstream &countriesMapping, string V2Loc);
		vector<string>	getPotentialTags(); 
		void				convertCountries(EU3World sourceWorld, countryMapping countryMap, cultureMapping cultureMap, religionMapping religionMap);
		void				convertProvinces(EU3World sourceWorld, provinceMapping provinceMap, countryMapping countryMap, cultureMapping cultureMap, religionMapping religionMap);
		void				convertCapitals(EU3World sourceWorld, provinceMapping provinceMap);
		void				setupStates(stateMapping);
		void				convertTechs(EU3World sourceWorld);
		void				output(FILE*);
	private:
		vector<V2Province>	provinces;
		vector<V2Country>		countries;
		vector<V2Country>		potentialCountries;
};


#endif // V2WORLD_H_