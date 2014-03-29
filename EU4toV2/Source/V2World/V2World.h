#ifndef V2WORLD_H_
#define V2WORLD_H_



#include "V2Country.h"
#include "../EU4World/EU4Army.h"
#include "V2Diplomacy.h"
#include "V2Factory.h"
#include "V2TechSchools.h"
#include "V2Party.h"
#include "../Mapper.h"
#include <set>

class V2Country;
class V2Province;
class V2Army;
class V2LeaderTraits;



class V2World {
	public:
		V2World();
		void output() const;
		void createProvinceFiles(const EU4World& sourceWorld, const provinceMapping& provinceMap);
		
		void convertCountries(const EU4World& sourceWorld, const countryMapping& countryMap, const cultureMapping& cultureMap, const unionCulturesMap& unionCultures, const religionMapping& religionMap, const governmentMapping& governmentMap, const inverseProvinceMapping& inverseProvinceMap, const vector<techSchool>& techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt);
		void convertDiplomacy(const EU4World& sourceWorld, const countryMapping& countryMap);
		void convertProvinces(const EU4World& sourceWorld, const provinceMapping& provinceMap, const resettableMap& resettableProvinces, const countryMapping& countryMap, const cultureMapping& cultureMap, const religionMapping& religionMap, const stateIndexMapping& stateIndexMap);
		void setupColonies(const adjacencyMapping& adjacencyMap, const continentMapping& continentMap);
		void setupStates(const stateMapping&);
		void convertUncivReforms();
		void setupPops(EU4World& sourceWorld);
		void addUnions(const unionMapping& unionMap);
		void convertArmies(const EU4World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const map<int,int>& leaderIDMap);
		void convertTechs(const EU4World& sourceWorld);
		void allocateFactories(const EU4World& sourceWorld, const V2FactoryFactory& factoryBuilder);

		map<string, V2Country*>	getPotentialCountries()	const;
		map<string, V2Country*>	getDynamicCountries()	const;
	private:
		void				outputHeader(FILE*) const;
		void				getProvinceLocalizations(string file);
		vector<int>		getPortProvinces(vector<int> provinces);
		V2Country*		getCountry(string tag);

		map<int, V2Province*>		provinces;
		map<string, V2Country*>		countries;
		vector<V2Country*>			potentialCountries;
		vector<V2Country*>			dynamicCountries;
		map<int, V2Party*>			parties;
		V2Diplomacy						diplomacy;
		map< int, set<string> >		colonies;
};



#endif // V2WORLD_H_