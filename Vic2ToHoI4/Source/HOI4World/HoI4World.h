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
#include <optional>
#include <set>
#include <string>
#include <vector>
using namespace std;



class HoI4Advisor;
class HoI4Buildings;
class HoI4Country;
class HoI4Diplomacy;
class HoI4DivisionTemplateType;
class HoI4Events;
class HoI4Faction;
class HoI4Ideology;
class HoI4OnActions;
class HoI4Province;
class HoI4State;
class HoI4States;
class HoI4StrategicRegion;
class HoI4SupplyZones;
class HoI4UnitMap;
class V2Country;
class V2World;


struct advisorCompare;



class HoI4World
{
	public:
		explicit HoI4World(const V2World* sourceWorld);

		void output() const;

		map<string, shared_ptr<HoI4Country>> getCountries()	const { return countries; }
		vector<shared_ptr<HoI4Country>> getGreatPowers() const { return greatPowers; }
		map<int, HoI4State*> getStates() const { return states->getStates(); }
		const map<int, int>& getProvinceToStateIDMap() const { return states->getProvinceToStateIDMap(); }
		vector<shared_ptr<HoI4Faction>> getFactions() const { return factions; }
		HoI4Events* getEvents() const { return events; }
		set<string> getMajorIdeologies() const { return majorIdeologies; }

		shared_ptr<HoI4Country> findCountry(const string& countryTag);

	private:
		HoI4World(const HoI4World&) = delete;
		HoI4World& operator=(const HoI4World&) = delete;

		void convertNavalBases();

		void convertCountries();
		void convertCountry(pair<string, V2Country*> country, map<int, int>& leaderMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);

		void importIdeologies();
		void importIdeologyFile(const string& filename);

		void importLeaderTraits();

		void importIdeologicalMinisters();

		void convertGovernments();

		void convertParties();

		void importIdeologicalIdeas();

		void identifyMajorIdeologies();
		void addNeutrality();
		void convertIdeologySupport();

		void convertIndustry();
		void addStatesToCountries();
		map<string, double> calculateFactoryWorkerRatios();
		map<string, double> getIndustrialWorkersPerCountry();
		double getTotalWorldWorkers(const map<string, double>& industrialWorkersPerCountry);
		map<string, double> adjustWorkers(const map<string, double>& industrialWorkersPerCountry, double totalWorldWorkers);
		double getWorldwideWorkerFactoryRatio(const map<string, double>& workersInCountries, double totalWorldWorkers);
		void putIndustryInStates(const map<string, double>& factoryWorkerRatios);
		void calculateIndustryInCountries();
		void reportIndustryLevels();
		void reportCountryIndustry();
		void reportDefaultIndustry();
		pair<string, array<int, 3>> getDefaultStateIndustry(const string& stateFilename);
		void reportDefaultIndustry(const map<string, array<int, 3>>& countryIndustry);

		void convertResources();
		map<int, map<string, double>> importResourceMap() const;

		void convertStrategicRegions();
		map<int, int> importStrategicRegions();
		map<int, int> determineUsedRegions(const HoI4State* state, map<int, int>& provinceToStrategicRegionMap);
		optional<int> determineMostUsedRegion(const map<int, int>& usedRegions) const;
		void addProvincesToRegion(const HoI4State* state, int regionNum);
		void addLeftoverProvincesToRegions(const map<int, int>& provinceToStrategicRegionMap);

		void convertDiplomacy();
		void convertAgreements();
		void convertRelations();

		void convertTechs();
		map<string, vector<pair<string, int>>> importTechMap() const;
		map<string, vector<pair<string, int>>> importResearchBonusMap() const;
		void addTechs(shared_ptr<HoI4Country> countryaddTechs, const string& oldTech, const map<string, vector<pair<string, int>>>& techMap);
		void addResearchBonuses(shared_ptr<HoI4Country> countryaddBonuses, const string& oldTech, const map<string, vector<pair<string, int>>>& researchBonusMap);

		map<string, HoI4UnitMap> importUnitMap() const;
		vector<HoI4DivisionTemplateType> importDivisionTemplates() const;
		void convertMilitaries();
		void convertArmies(const map<string, HoI4UnitMap>& unitMap, const vector<HoI4DivisionTemplateType>& divisionTemplates);
		void convertNavies(const map<string, HoI4UnitMap>& unitMap);
		void convertAirforces(const map<string, HoI4UnitMap>& unitMap);

		void determineGreatPowers();

		void convertCapitalVPs();
		void addBasicCapitalVPs();
		void addGreatPowerVPs();
		void addStrengthVPs();
		double getStrongestCountryStrength() const;
		int calculateStrengthVPs(shared_ptr<HoI4Country> country, double greatestStrength) const;

		void convertAirBases();
		void addBasicAirBases();
		void addCapitalAirBases();
		void addGreatPowerAirBases();

		void createFactions();
		void logFactionMember(ofstream& factionsLog, shared_ptr<HoI4Country> member) const;
		optional<string> returnSphereLeader(shared_ptr<HoI4Country> possibleSphereling) const;
		bool governmentsAllowFaction(const string& leaderGovernment, const string& allyGovernment) const;

		void addCountryElectionEvents(const set<string>& majorIdeologies);

		void outputCommonCountries() const;
		void outputColorsfile() const;
		void outputNames() const;
		void outputMap() const;
		void outputGenericFocusTree() const;
		void outputCountries() const;
		set<const HoI4Advisor*, advisorCompare> getActiveIdeologicalAdvisors() const;
		void outputRelations() const;
		void outputIdeologies() const;
		void outputLeaderTraits() const;
		void outputIdeologicalIdeas() const;
		void outputScriptedTriggers() const;
		void outputOnActions() const;

		/*vector<int> getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		vector<int> getPortProvinces(const vector<int>& locationCandidates);
		int getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner);*/


		const V2World* sourceWorld;

		HoI4States* states;
		//map<int, HoI4Province*> provinces;

		HoI4SupplyZones* supplyZones;
		map<int, HoI4StrategicRegion*> strategicRegions;
		HoI4Buildings* buildings;

		map<string, shared_ptr<HoI4Country>> countries;
		map<string, shared_ptr<HoI4Country>> landedCountries;
		vector<shared_ptr<HoI4Country>> greatPowers;

		map<string, HoI4Ideology*> ideologies;
		set<string> majorIdeologies;
		map<string, vector<shared_ptr<Object>>> ideologicalLeaderTraits;
		map<string, HoI4Advisor*> ideologicalAdvisors;
		map<string, vector<shared_ptr<Object>>> ideologicalIdeas;
		
		vector<shared_ptr<HoI4Faction>> factions;
		HoI4Diplomacy* diplomacy;
		HoI4Events* events;
		HoI4OnActions* onActions;

		vector<HoI4DivisionTemplateType> divisionTemplates;

		leaderTraitsMap leaderTraits;
};



#endif // HoI4WORLD_H_
