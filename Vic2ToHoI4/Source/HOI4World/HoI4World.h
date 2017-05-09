/*Copyright (c) 2017 The Paradox Game Converters Project

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



#ifndef HoI4WORLD_H_
#define HoI4WORLD_H_



#include "HoI4States.h"
#include "../Mappers/Mapper.h"
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;



class HoI4Buildings;
class HoI4Country;
class HoI4Diplomacy;
class HoI4Events;
class HoI4Faction;
class HoI4Province;
class HoI4State;
class HoI4States;
class HoI4StrategicRegion;
class HoI4SupplyZones;
class V2Country;
class V2World;



class HoI4World
{
	public:
		HoI4World(const V2World* sourceWorld);

		void output() const;

		map<string, HoI4Country*> getCountries()	const { return countries; }
		vector<HoI4Country*> getGreatPowers() const { return greatPowers; }
		map<int, HoI4State*> getStates() const { return states->getStates(); }
		vector<HoI4Faction*> getFactions() const { return factions; }
		HoI4Events* getEvents() const { return events; }

		HoI4Country* findCountry(string countryTag);

	private:
		void convertNavalBases();

		void convertCountries();
		void convertCountry(pair<string, V2Country*> country, map<int, int>& leaderMap, governmentJobsMap governmentJobs, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);
		void identifyMajorIdeologies();
		void addNeutrality();
		void convertIdeologySupport();

		void convertIndustry();
		void addStatesToCountries();
		map<string, double> calculateFactoryWorkerRatios();
		map<string, double> getIndustrialWorkersPerCountry();
		double getTotalWorldWorkers(map<string, double> industrialWorkersPerCountry);
		map<string, double> adjustWorkers(map<string, double> industrialWorkersPerCountry, double totalWorldWorkers);
		double getWorldwideWorkerFactoryRatio(map<string, double> workersInCountries, double totalWorldWorkers);
		void putIndustryInStates(map<string, double> factoryWorkerRatios);
		void calculateIndustryInCountries();
		void reportIndustryLevels();
		void reportCountryIndustry();
		void reportDefaultIndustry();
		pair<string, array<int, 3>> getDefaultStateIndustry(string stateFilename);
		void reportDefaultIndustry(const map<string, array<int, 3>>& countryIndustry);

		void convertResources();
		map<int, map<string, double>> importResourceMap() const;

		void convertStrategicRegions();
		map<int, int> importStrategicRegions();
		map<int, int> determineUsedRegions(const HoI4State* state, map<int, int>& provinceToStrategicRegionMap);
		int determineMostUsedRegion(const map<int, int>& usedRegions) const;
		void addProvincesToRegion(const HoI4State* state, int regionNum);
		void addLeftoverProvincesToRegions(const map<int, int>& provinceToStrategicRegionMap);

		void convertDiplomacy();
		void convertAgreements();
		void convertRelations();

		void convertTechs();
		map<string, vector<pair<string, int>>> importTechMap() const;
		void addTechs(HoI4Country* countryaddTechs, const string& oldTech, const map<string, vector<pair<string, int>>>& techMap);

		void generateLeaders();

		void convertArmies();
		void convertNavies();
		void convertAirforces();

		void determineGreatPowers();

		void convertCapitalVPs();
		void addBasicCapitalVPs();
		void addGreatPowerVPs();
		void addStrengthVPs();
		double getStrongestCountryStrength();
		int calculateStrengthVPs(HoI4Country* country, double greatestStrength);

		void convertAirBases();
		void addBasicAirBases();
		void addCapitalAirBases();
		void addGreatPowerAirBases();

		void createFactions();
		void logFactionMember(ofstream& factionsLog, const HoI4Country* member);
		string returnSphereLeader(HoI4Country* possibleSphereling);
		bool governmentsAllowFaction(string leaderGovernment, string allyGovernment);

		void outputCommonCountries() const;
		void outputColorsfile() const;
		void outputNames() const;
		void outputMap() const;
		void outputCountries() const;
		void outputRelations() const;

		/*vector<int> getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		vector<int> getPortProvinces(const vector<int>& locationCandidates);
		int getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner);*/


		const V2World* sourceWorld;

		HoI4States* states;
		//map<int, HoI4Province*> provinces;

		HoI4SupplyZones* supplyZones;
		map<int, HoI4StrategicRegion*> strategicRegions;
		HoI4Buildings* buildings;

		map<string, HoI4Country*> countries;
		map<string, HoI4Country*> landedCountries;
		vector<HoI4Country*> greatPowers;

		set<string> majorIdeologies;
		
		vector<HoI4Faction*> factions;
		HoI4Diplomacy* diplomacy;
		HoI4Events* events;

		leaderTraitsMap leaderTraits;
		portraitMapping portraitMap;
};



#endif // HoI4WORLD_H_
