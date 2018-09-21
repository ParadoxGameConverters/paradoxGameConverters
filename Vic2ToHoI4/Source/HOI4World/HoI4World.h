/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "AIPeaces.h"
#include "CoastalProvinces.h"
#include "Decisions.h"
#include "Ideas.h"
#include "OnActions.h"
#include "Names.h"
#include "HoI4States.h"
#include "MapData.h"
#include "AllMilitaryMappings.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/GovernmentMapper.h"
#include "../Mappers/GraphicsMapper.h"
#include "newParser.h"
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>



class HoI4Country;
class HoI4Diplomacy;
class HoI4Faction;
class HoI4Ideology;
class HoI4Province;
class HoI4States;
class HoI4StrategicRegion;
class techMapper;


namespace Vic2
{

class Country;
class World;

}



namespace HoI4
{

class Advisor;
class Buildings;
class DivisionTemplateType;
class Events;
class State;
class SupplyZones;
class UnitMap;
struct advisorCompare;

class World: commonItems::parser
{
	public:
		explicit World(const Vic2::World* sourceWorld);
		~World() = default;

		void output();

		map<string, shared_ptr<HoI4Country>> getCountries()	const { return countries; }
		vector<shared_ptr<HoI4Country>> getGreatPowers() const { return greatPowers; }
		map<int, HoI4::State*> getStates() const { return states->getStates(); }
		const map<int, int>& getProvinceToStateIDMap() const { return states->getProvinceToStateIDMap(); }
		vector<shared_ptr<HoI4Faction>> getFactions() const { return factions; }
		HoI4::Events* getEvents() const { return events; }
		set<string> getMajorIdeologies() const { return majorIdeologies; }

		shared_ptr<HoI4Country> findCountry(const string& countryTag);

	private:
		World(const World&) = delete;
		World& operator=(const World&) = delete;

		void convertNavalBases();

		void convertCountries();
		void convertCountry(pair<string, Vic2::Country*> country);

		void importIdeologies();
		void importLeaderTraits();
		void importIdeologicalMinisters();

		void convertGovernments();

		void convertParties();

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
		pair<string, array<int, 3>> getDefaultStateIndustry(const HoI4::State* state);
		void reportDefaultIndustry(const map<string, array<int, 3>>& countryIndustry);

		void convertResources();

		void convertStrategicRegions();
		map<int, int> importStrategicRegions();
		map<int, int> determineUsedRegions(const HoI4::State* state, map<int, int>& provinceToStrategicRegionMap);
		optional<int> determineMostUsedRegion(const map<int, int>& usedRegions) const;
		void addProvincesToRegion(const HoI4::State* state, int regionNum);
		void addLeftoverProvincesToRegions(const map<int, int>& provinceToStrategicRegionMap);

		void convertDiplomacy();
		void convertAgreements();
		void convertRelations();

		void convertTechs();
		void addTechs(shared_ptr<HoI4Country> countryaddTechs, const string& oldTech, const techMapper& techMap);
		void addResearchBonuses(shared_ptr<HoI4Country> countryaddBonuses, const string& oldTech, const techMapper& researchBonusMap);

		void convertMilitaries();
		void convertArmies(const militaryMappings& theMilitaryMappings);
		void convertNavies(const map<string, HoI4::UnitMap>& unitMap);
		void convertAirforces(const map<string, HoI4::UnitMap>& unitMap);

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

		void addFocusTrees();
		void adjustResearchFocuses();

		void addCountryElectionEvents(const set<string>& majorIdeologies);

		void outputCommonCountries() const;
		void outputColorsfile() const;
		void outputNames() const;
		void outputUnitNames() const;
		void outputMap() const;
		void outputGenericFocusTree() const;
		void outputCountries();
		set<const HoI4::Advisor*, HoI4::advisorCompare> getActiveIdeologicalAdvisors() const;
		void outputRelations() const;
		void outputIdeologies() const;
		void outputLeaderTraits() const;
		void outputIdeas() const;
		void outputScriptedTriggers() const;
		void outputBookmarks() const;

		/*vector<int> getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		vector<int> getPortProvinces(const vector<int>& locationCandidates);
		int getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner);*/

		HoI4::namesMapper theNames;
		graphicsMapper theGraphics;
		governmentMapper governmentMap;
		CountryMapper countryMap;

		const Vic2::World* sourceWorld = nullptr;

		HoI4States* states = nullptr;
		//map<int, HoI4Province*> provinces;

		HoI4::SupplyZones* supplyZones = nullptr;
		map<int, HoI4StrategicRegion*> strategicRegions;
		Buildings* buildings = nullptr;

		map<string, shared_ptr<HoI4Country>> countries;
		map<string, shared_ptr<HoI4Country>> landedCountries;
		vector<shared_ptr<HoI4Country>> greatPowers;

		map<string, HoI4Ideology*> ideologies;
		std::set<std::string> majorIdeologies;
		std::map<std::string, vector<std::string>> ideologicalLeaderTraits;
		map<std::string, HoI4::Advisor*> ideologicalAdvisors;
		std::unique_ptr<HoI4::Ideas> theIdeas;
		
		vector<shared_ptr<HoI4Faction>> factions;
		HoI4Diplomacy* diplomacy = nullptr;
		std::unique_ptr<HoI4::decisions> decisions;
		std::unique_ptr<HoI4::AIPeaces> peaces;
		HoI4::Events* events = nullptr;
		std::unique_ptr<HoI4::OnActions> onActions;

		allMilitaryMappings theMilitaryMappings;

		coastalProvinces theCoastalProvinces;
		MapData theMapData;
};

}



#endif // HoI4WORLD_H_
