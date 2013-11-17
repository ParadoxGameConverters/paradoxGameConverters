#ifndef V2WORLD_H_
#define V2WORLD_H_



//#include "V2Country.h"
#include "EU3Army.h"
#include "V2Diplomacy.h"
#include "V2Factory.h"
#include "V2TechSchools.h"
#include "V2Party.h"
#include "Mapper.h"
#include <set>

class V2Country;
class V2Province;
class V2Army;
class V2LeaderTraits;



class V2World {
	public:
		V2World(string V2Loc);
		void output(FILE*) const;
		
		void convertCountries(const EU3World& sourceWorld, const countryMapping& countryMap, const cultureMapping& cultureMap, const unionCulturesMap& unionCultures, const religionMapping& religionMap, const governmentMapping& governmentMap, const inverseProvinceMapping& inverseProvinceMap, const vector<techSchool>& techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt);
		void convertDiplomacy(const EU3World& sourceWorld, const countryMapping& countryMap);
		void convertProvinces(const EU3World& sourceWorld, const provinceMapping& provinceMap, const countryMapping& countryMap, const cultureMapping& cultureMap, const religionMapping& religionMap, const stateIndexMapping& stateIndexMap);
		void setupStates(const stateMapping&);
		void convertUncivReforms();
		void setupPops(EU3World& sourceWorld);
		void addUnions(const unionMapping& unionMap);
		void convertArmies(const EU3World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const map<int,int>& leaderIDMap);
		void convertTechs(const EU3World& sourceWorld);
		void allocateFactories(const EU3World& sourceWorld, const V2FactoryFactory& factoryBuilder);

		map<string, V2Country*>	getPotentialCountries()	const;
	private:
		void			outputHeader(FILE*) const;
		void			getProvinceLocalizations(string file);
		vector<int>	getPortProvinces(vector<int> provinces);
		V2Country*	getCountry(string tag);

		vector<V2Province*>		provinces;
		vector<V2Country*>		countries;
		vector<V2Country*>		potentialCountries;
		map<int, V2Party*>		parties;
		V2Diplomacy					diplomacy;
		int							equalityLeft;
		int							libertyLeft;
		map< int, set<string> >	colonies;
};



#endif // V2WORLD_H_