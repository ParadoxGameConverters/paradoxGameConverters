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



#ifndef HoI4COUNTRY_H_
#define HoI4COUNTRY_H_



#include "HoI4Airforce.h"
#include "HoI4Army.h"
#include "Division.h"
#include "DivisionTemplate.h"
#include "HoI4FocusTree.h"
#include "HOI4Ideology.h"
#include "HoI4Leader.h"
#include "HoI4Navy.h"
#include "HoI4Province.h"
#include "HoI4Relations.h"
#include "HoI4State.h"
#include "../Color.h"
#include "Date.h"
#include "../V2World/Army.h"
#include "../V2World/Party.h"
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>
using namespace std;



namespace Vic2
{
class Country;
class World;
}


class HoI4Faction;
class HoI4Ideology;


namespace HoI4
{
class Advisor;
class coastalProvinces;
class namesMapper;
class State;
class UnitMap;
class World;
struct advisorCompare;
}


class CountryMapper;
class governmentMapper;
class graphicsMapper;



class HoI4Country
{
	public:
		HoI4Country(const string& _tag, const string& _commonCountryFile, const HoI4::World* _theWorld);

		void initFromV2Country(const Vic2::World& _srcWorld, const Vic2::Country* _srcCountry, const std::map<int, int>& stateMap, const std::map<int, HoI4::State*>& states, HoI4::namesMapper& theNames, const graphicsMapper& theGraphics, const CountryMapper& countryMap);
		void setGovernmentToExistingIdeology(const set<string>& majorIdeologies, const map<string, HoI4Ideology*>& ideologies, const governmentMapper& governmentMap);
		void convertGovernment(const Vic2::World& _srcWorld, const governmentMapper& governmentMap);
		void convertParties(const set<string>& majorIdeologies, const governmentMapper& governmentMap);
		void convertIdeologySupport(const set<string>& majorIdeologies, const governmentMapper& governmentMap);
		void convertNavies(const map<string, HoI4::UnitMap>& unitMap, const HoI4::coastalProvinces& theCoastalProvinces, const std::map<int, int>& provinceToStateIDMap);
		void convertConvoys(const map<string, HoI4::UnitMap>& unitMap);
		void convertAirforce(const map<string, HoI4::UnitMap>& unitMap);
		void convertArmies(const HoI4::militaryMappings& theMilitaryMappings);
		void		setTechnology(const string& tech, int level);
		void		setResearchBonus(const string& tech, int bonus);
		void addState(HoI4::State* _state);
		void calculateIndustry();
		void reportIndustry(ofstream& out);
		void addVPsToCapital(int VPs);
		void addGenericFocusTree(const set<string>& majorIdeologies);
		void adjustResearchFocuses();
		void outputToCommonCountriesFile(ofstream& countriesFile) const;
		void outputColors(ofstream& out) const;
		void outputToNamesFiles(ofstream& namesFile, const HoI4::namesMapper& theNames) const;
		void outputToUnitNamesFiles(ofstream& unitNamesFile) const;
		void output(const set<const HoI4::Advisor*, HoI4::advisorCompare>& ideologicalMinisters, const vector<HoI4::DivisionTemplateType>& divisionTemplates, HoI4::namesMapper& theNames, graphicsMapper& theGraphics) const;
		void outputIdeaGraphics(ofstream& ideasFile, graphicsMapper& graphics) const;

		void setSphereLeader(const string& SphereLeader) { sphereLeader = SphereLeader; }
		void		setFaction(shared_ptr<const HoI4Faction> newFaction) { faction = newFaction; }
		void		setFactionLeader() { factionLeader = true; }
		void addNationalFocus(shared_ptr<HoI4FocusTree> NF) { nationalFocus = NF; }
		void setGreatPower() { greatPower = true; }

		optional<const HoI4Relations*> getRelations(string withWhom) const;
		double getStrengthOverTime(double years) const;
		double getMilitaryStrength() const;
		double getEconomicStrength(double years) const;

		bool isHuman() const { return human; }
		const map<string, HoI4Relations*>&	getRelations() const { return relations; }
		set<int>									getProvinces() const { return provinces; }
		string										getTag() const { return tag; }
		const Vic2::Country*							getSourceCountry() const { return srcCountry; }
		shared_ptr<const HoI4Faction> getFaction() const { return faction; }
		string getGovernmentIdeology() const { return governmentIdeology; }
		map<string, int> getIdeologySupport() const { return ideologySupport; }
		const set<string>&						getAllies() const { return allies; }
		const set<string>&							getPuppets() const { return puppets; }
		const string&								getPuppetmaster() const { return puppetMaster; }
		const map<string, double>&						getPracticals() const { return practicals; }
		vector<int>									getBrigs() const { return brigs; }
		HoI4::State* getCapitalState() const { return capitalState; }
		int											getCapitalStateNum() const { return capitalStateNum; }
		const string									getSphereLeader() const { return sphereLeader; }
		const Vic2::Party getRulingParty() const { return rulingParty; }
		set<Vic2::Party, function<bool (const Vic2::Party&, const Vic2::Party&)>> getParties() const { return parties; }
		map<int, HoI4::State*> getStates() const { return states; }
		bool isInFaction() const { return faction != nullptr; }
		bool isCivilized() const { return civilized; }

		int getTechnologyCount() const { return technologies.size(); }
		int getResearchBonusesCount() const { return researchBonuses.size(); }
		int getProvinceCount() const { return provinceCount; }
		bool isGreatPower() const { return greatPower; }

		void setProvinceCount(int count) { provinceCount = count; }
		void setPuppetmaster(const string& _master) { puppetMaster = _master; }
		void addPuppet(const string& countryTag) { puppets.insert(countryTag); }

		set<string>&								editAllies() { return allies; }

	private:
		HoI4Country(const HoI4Country&) = delete;
		HoI4Country& operator=(const HoI4Country&) = delete;

		void determineFilename();
		void initIdeas(HoI4::namesMapper& theNames);
		void convertLaws();
		void convertLeaders(const graphicsMapper& theGraphics);
		void convertRelations(const CountryMapper& countryMap);
		void determineCapitalFromVic2(const map<int, int>& provinceToStateIDMap, const map<int, HoI4::State*>& states);
		bool isStateValidForCapital(int capitalState, const map<int, HoI4::State*>& states);
		bool isThisStateOwnedByUs(const HoI4::State* state) const;
		bool isThisStateACoreWhileWeOwnNoStates(const HoI4::State* state) const;
		void setCapitalInCapitalState(int capitalProvince);
		void findBestCapital();

		void addProvince(int _province);

		void outputNamesSet(ofstream& namesFile, const optional<vector<string>>& names, const string& tabs) const;
		void outputHistory(HoI4::namesMapper& theNames, graphicsMapper& theGraphics) const;
		void outputCapital(ofstream& output) const;
		void outputResearchSlots(ofstream& output) const;
		void outputThreat(ofstream& output) const;
		void outputOOB(const vector<HoI4::DivisionTemplateType>& divisionTemplates) const;
		void outputTechnology(ofstream& output) const;
		void outputResearchBonuses(ofstream& output) const;
		void outputConvoys(ofstream& output) const;
		void outputEquipmentStockpile(ofstream& output) const;
		void outputPuppets(ofstream& output) const;
		void outputPolitics(ofstream& output) const;
		void outputRelations(ofstream& output) const;
		bool areElectionsAllowed(void) const;
		void outputFactions(ofstream& output) const;
		void outputIdeas(ofstream& output) const;
		void outputStability(ofstream& output) const;
		void outputWarSupport(ofstream& output) const;
		void outputCountryLeader(ofstream& output, HoI4::namesMapper& theNames, graphicsMapper& theGraphics) const;
		void outputCommanders(ofstream& output) const;
		void outputOOBLine(ofstream& output) const;
		void outputCommonCountryFile() const;
		void outputAdvisorIdeas(const set<const HoI4::Advisor*, HoI4::advisorCompare>& ideologicalAdvisors) const;
		void outputUnitType(ofstream& unitNamesFile, const string& sourceUnitType, const string& destUnitType, const string& defaultName) const;


		const HoI4::World* theWorld;
		const Vic2::Country* srcCountry;
		string filename;

		bool human;

		string governmentIdeology;
		string leaderIdeology;
		Vic2::Party rulingParty;
		set<Vic2::Party, function<bool (const Vic2::Party&, const Vic2::Party&)>> parties;
		map<string, int> ideologySupport;
		date lastElection;

		string sphereLeader = "";
		string								tag;
		set<int>							provinces;
		map<int, HoI4::State*> states;
		int									capitalStateNum;
		HoI4::State*							capitalState;
		string								commonCountryFile;
		map<string, int>					technologies;
		map<string, int>					researchBonuses;
		map<string, HoI4Relations*>	relations;
		ConverterColor::Color color;
		double stability = 0.50;
		double warSupport = 0.50;
		shared_ptr<const HoI4Faction>	faction;
		bool									factionLeader;
		set<string>							allies;
		set<string> puppets;
		string puppetMaster;
		map<string, double>				practicals;
		string graphicalCulture;
		string graphicalCulture2d;
		bool									majorNation;
		bool									civilized;
		vector<int>							brigs;
		int									convoys;
		
		int provinceCount = 0;
		long armyStrength;
		double militaryFactories;
		double civilianFactories;
		double dockyards;
		double threat;

		// laws
		string mobilizationLaw;
		string economicLaw;
		string tradeLaw;

		bool greatPower;

		// military stuff
		HoI4::Army theArmy;
		vector<HoI4::Navy> navies;
		vector<HoI4Airplane> planes;
		map<string, int> equipmentStockpile;
		std::vector<HoI4::General> generals;
		std::vector<HoI4::Admiral> admirals;

		shared_ptr<HoI4FocusTree> nationalFocus;
};

#endif	// HoI4COUNTRY_H_

