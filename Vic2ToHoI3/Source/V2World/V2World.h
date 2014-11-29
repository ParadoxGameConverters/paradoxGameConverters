/*Copyright (c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#ifndef V2WORLD_H_
#define V2WORLD_H_



#include "V2Country.h"
#include "../EU4World/EU4Army.h"
#include "V2Diplomacy.h"
#include "V2Factory.h"
#include "V2TechSchools.h"
#include "V2Party.h"
#include "../CountryMapping.h"
#include "../Mapper.h"
#include <set>

class V2Country;
class V2Province;
class V2Army;
class V2LeaderTraits;
class V2Version;



class V2World {
	public:
		V2World();
		V2World(Object* obj);
		void output() const;
		void createProvinceFiles(const EU4World& sourceWorld, const provinceMapping& provinceMap);
		
		void convertCountries(const EU4World& sourceWorld, const CountryMapping& countryMap, const cultureMapping& cultureMap, const unionCulturesMap& unionCultures, const religionMapping& religionMap, const governmentMapping& governmentMap, const inverseProvinceMapping& inverseProvinceMap, const vector<techSchool>& techSchools, map<int, int>& leaderMap, const V2LeaderTraits& lt, const map<string, double>& UHLiberalIdeas, const map<string, double>& UHReactionaryIdeas, const vector< pair<string, int> >& literacyIdeas, const map<string, int>& orderIdeas, const map<string, int>& libertyIdeas, const map<string, int>& equalityIdeas);
		void convertDiplomacy(const EU4World& sourceWorld, const CountryMapping& countryMap);
		void convertProvinces(const EU4World& sourceWorld, const provinceMapping& provinceMap, const resettableMap& resettableProvinces, const CountryMapping& countryMap, const cultureMapping& cultureMap, const religionMapping& religionMap, const stateIndexMapping& stateIndexMap);
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

		V2Country*		getCountry(string tag) const;
		map<string, V2Country*>	getCountries()	const { return countries; };
		void								removeCountry(string tag);
		V2Province* getProvince(int provNum) const;
		void								checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const;
		void								setLocalisations(V2Localisation& localisation);
		V2Party* getRulingParty(const V2Country* country) const;
		
		void removeEmptyNations();
		V2Diplomacy* getDiplomacy() { return &diplomacy; };
		const vector<string> &getGreatCountries() const { return greatCountries; };
	private:
		void				getProvinceLocalizations(string file);
		vector<int>		getPortProvinces(vector<int> provinces);

		map<int, V2Province*>		provinces;
		map<string, V2Country*>		countries;
		vector<V2Country*>			potentialCountries;
		vector<V2Country*>			dynamicCountries;
		V2Diplomacy						diplomacy;
		map< int, set<string> >		colonies;
		vector<V2Party*>		parties;
		vector<string> greatCountries; // Tags of great nations in order of ranking
};



#endif // V2WORLD_H_