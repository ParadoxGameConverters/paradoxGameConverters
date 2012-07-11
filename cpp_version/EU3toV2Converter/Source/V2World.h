#ifndef V2WORLD_H_
#define V2WORLD_H_



//#include "V2Country.h"
#include "EU3Army.h"
#include "V2Diplomacy.h"
#include "V2Factory.h"
#include "V2TechSchools.h"
#include "V2Party.h"
#include "Mapper.h"

class V2Country;
class V2Province;
class V2Army;
class V2LeaderTraits;



class V2World {
	public:
		void				init(string V2Loc);
		void				addPotentialCountries(ifstream &countriesMapping, string V2Loc);
		vector<string>	getPotentialTags(); 
		void				sortCountries(const vector<string>& order);
		void				convertCountries(EU3World sourceWorld, countryMapping countryMap, cultureMapping cultureMap, unionCulturesMap unionCultures, religionMapping religionMap, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, vector<techSchool> techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt);
		void				convertProvinces(EU3World sourceWorld, provinceMapping provinceMap, countryMapping countryMap, cultureMapping cultureMap, religionMapping religionMap);
		void				addUnions(unionMapping unionMap);
		void				setupStates(stateMapping);
		void				setupPops(EU3World& sourceWorld);
		void				convertTechs(EU3World sourceWorld);
		void				output(FILE*);
		void				convertDiplomacy(EU3World sourceWorld, countryMapping countryMap);
		void				convertArmies(EU3World sourceWorld, inverseProvinceMapping inverseProvinceMap, const map<int,int>& leaderIDMap);
		void				allocateFactories(EU3World sourceWorld, V2FactoryFactory& factoryBuilder);
		V2Party*			getParty(int index);

		map<string, V2Country*>	getPotentialCountries()	const;
	private:
		void						buildParties();
		void						outputHeader(FILE*);
		void						getProvinceLocalizations(string file);
		vector<int>				getPortProvinces(vector<int> provinces);
		V2Country*				getCountry(string tag);

		vector<V2Province*>	provinces;
		vector<V2Country*>	countries;
		vector<V2Country*>	potentialCountries;
		vector<V2Party*>		parties;
		V2Diplomacy				diplomacy;
		int						equalityLeft;
		int						libertyLeft;
};



#endif // V2WORLD_H_