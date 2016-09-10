/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include "V2Diplomacy.h"
#include "V2Factory.h"
#include "V2TechSchools.h"
#include "V2Party.h"
#include "V2Province.h"
#include "../EU4World/EU4Army.h"
#include "../EU4World/EU4Province.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/Mapper.h"
#include <set>

class V2Country;
class V2Army;
class V2LeaderTraits;



class V2World {
	public:
		V2World(const minorityPopMapping& minorities);
		void output() const;
		void createProvinceFiles(const EU4World& sourceWorld);
		
		void convertCountries(const EU4World& sourceWorld, const CountryMapping& countryMap, const cultureMapping& cultureMap, const unionCulturesMap& unionCultures, const religionMapping& religionMap, const governmentMapping& governmentMap, const vector<techSchool>& techSchools, map<int, int>& leaderMap, const V2LeaderTraits& lt, colonyFlagset& colonyFlags, const map<string, double>& UHLiberalIdeas, const map<string, double>& UHReactionaryIdeas, const vector< pair<string, int> >& literacyIdeas, const map<string, int>& orderIdeas, const map<string, int>& libertyIdeas, const map<string, int>& equalityIdeas, const EU4RegionsMapping& regionsMap);
		void convertDiplomacy(const EU4World& sourceWorld, const CountryMapping& countryMap);
		void convertProvinces(const EU4World& sourceWorld, const CountryMapping& countryMap, const cultureMapping& cultureMap, const cultureMapping& slaveCultureMap, const religionMapping& religionMap, const stateIndexMapping& stateIndexMap, const EU4RegionsMapping& regionsMap);
		void setupColonies(const adjacencyMapping& adjacencyMap, const continentMapping& continentMap);
		void setupStates(const stateMapping&);
		void convertUncivReforms();
		void setupPops(EU4World& sourceWorld);
		void addUnions(const unionMapping& unionMap);
		void convertArmies(const EU4World& sourceWorld, const map<int,int>& leaderIDMap, adjacencyMapping adjacencyMap);
		void convertTechs(const EU4World& sourceWorld, map<string, double>& armyTechIdeas, map<string, double>& commerceTechIdeas, map<string, double>& cultureTechIdeas, map<string, double>& industryTechIdeas, map<string, double>& navyTechIdeas);
		void allocateFactories(const EU4World& sourceWorld, const V2FactoryFactory& factoryBuilder);

		map<string, V2Country*>	getPotentialCountries()	const;
		map<string, V2Country*>	getDynamicCountries()	const;

	private:
		vector<V2Demographic>	determineDemographics(vector<EU4PopRatio>& popRatios, EU4Province* eProv, V2Province* vProv, EU4Country* oldOwner, const cultureMapping& cultureMap, const cultureMapping& slaveCultureMap, const religionMapping& religionMap, const EU4RegionsMapping& regionsMap, int destNum, double provPopRatio);

		void				outputPops() const;
		void				getProvinceLocalizations(string file);
		V2Country*		getCountry(string tag);

		map<int, V2Province*>		provinces;
		map<string, V2Country*>		countries;
		vector<V2Country*>			potentialCountries;
		map<string, V2Country*>		dynamicCountries;
		V2Diplomacy						diplomacy;
		map< int, set<string> >		colonies;

		colonyFlagset					colonyFlagMap;
		map<string, list<int>* >	popRegions;

		long								totalWorldPopulation;

		bool						isRandomWorld;
};



#endif // V2WORLD_H_