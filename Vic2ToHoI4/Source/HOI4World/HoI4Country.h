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



#ifndef HoI4COUNTRY_H_
#define HoI4COUNTRY_H_



#include "HoI4Airforce.h"
#include "HoI4Army.h"
#include "HoI4FocusTree.h"
#include "HoI4Leader.h"
#include "HoI4Navy.h"
#include "HoI4Province.h"
#include "HoI4Relations.h"
#include "HoI4State.h"
#include "../Color.h"
#include "Date.h"
#include "../V2World/V2Army.h"
#include <optional>
#include <set>
#include <vector>
using namespace std;



class V2Country;
class V2Party;
class HoI4Advisor;
class HoI4Faction;
class HoI4World;
struct advisorCompare;



class HoI4Country
{
	public:
		HoI4Country(const string& _tag, const string& _commonCountryFile, const HoI4World* _theWorld);

		void initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const map<int, int>& stateMap, const map<int, HoI4State*>& states);
		void initFromHistory();
		void setGovernmentToExistingIdeology(const set<string>& majorIdeologies, const map<string, HoI4Ideology*>& ideologies);
		void convertGovernment(const V2World& _srcWorld);
		void convertParties(const set<string>& majorIdeologies);
		void convertIdeologySupport(const set<string>& majorIdeologies);
		void		convertNavy(const map<string, HoI4UnitMap>& unitMap);
		void		convertConvoys(const map<string, HoI4UnitMap>& unitMap);
		void		convertAirforce(const map<string, HoI4UnitMap>& unitMap);
		void		convertArmyDivisions(const map<string, HoI4UnitMap>& unitMap, const vector<HoI4DivisionTemplateType>& divisionTemplates);		
		void		setTechnology(const string& tech, int level);
		void		setResearchBonus(const string& tech, int bonus);
		void addState(HoI4State* _state);
		void calculateIndustry();
		void reportIndustry(ofstream& out);
		void addVPsToCapital(int VPs);
		void outputToCommonCountriesFile(ofstream& countriesFile) const;
		void outputColors(ofstream& out) const;
		void outputToNamesFiles(ofstream& namesFile) const;
		void output(const set<const HoI4Advisor*, advisorCompare>& ideologicalMinisters, const vector<HoI4DivisionTemplateType>& divisionTemplates) const;
		void outputIdeaGraphics(ofstream& ideasFile) const;

		void		setSphereLeader(const string& SphereLeader) { sphereLeader == SphereLeader; }
		void		setFaction(shared_ptr<const HoI4Faction> newFaction) { faction = newFaction; }
		void		setFactionLeader() { factionLeader = true; }
		void addNationalFocus(shared_ptr<const HoI4FocusTree> NF) { nationalFocus = NF; }
		void setGreatPower() { greatPower = true; }

		optional<const HoI4Relations*> getRelations(string withWhom) const;
		double getStrengthOverTime(double years) const;
		double getMilitaryStrength() const;
		double getEconomicStrength(double years) const;

		bool isHuman() const { return human; }
		const map<string, HoI4Relations*>&	getRelations() const { return relations; }
		set<int>									getProvinces() const { return provinces; }
		string										getTag() const { return tag; }
		const V2Country*							getSourceCountry() const { return srcCountry; }
		shared_ptr<const HoI4Faction> getFaction() const { return faction; }
		string getGovernmentIdeology() const { return governmentIdeology; }
		map<string, int> getIdeologySupport() const { return ideologySupport; }
		const set<string>&						getAllies() const { return allies; }
		const set<string>&							getPuppets() const { return puppets; }
		const string&								getPuppetmaster() const { return puppetMaster; }
		const map<string, double>&						getPracticals() const { return practicals; }
		vector<int>									getBrigs() const { return brigs; }
		HoI4State* getCapitalState() const { return capitalState; }
		int											getCapitalStateNum() const { return capitalStateNum; }
		const string									getSphereLeader() const { return sphereLeader; }
		const V2Party* getRulingParty() const { return rulingParty; }
		set<const V2Party*, function<bool (const V2Party*, const V2Party*)>> getParties() const { return parties; }
		map<int, HoI4State*> getStates() const { return states; }
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
		void initIdeas();
		//void convertLaws();
		//void convertLeaders(portraitMapping& portraitMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);
		void convertRelations();
		void determineCapitalFromVic2(const map<int, int>& provinceToStateIDMap, const map<int, HoI4State*>& states);
		bool isStateValidForCapital(int capitalState, const map<int, HoI4State*>& states);
		bool isThisStateOwnedByUs(const HoI4State* state) const;
		bool isThisStateACoreWhileWeOwnNoStates(const HoI4State* state) const;
		void setCapitalInCapitalState(int capitalProvince);
		void findBestCapital();

		void addProvince(int _province);

		void outputNamesSet(ofstream& namesFile, const optional<vector<string>>& names, const string& tabs) const;
		void outputHistory() const;
		void outputCapital(ofstream& output) const;
		void outputResearchSlots(ofstream& output) const;
		void outputThreat(ofstream& output) const;
		void outputOOB(const vector<HoI4DivisionTemplateType>& divisionTemplates) const;
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
		void outputNationalUnity(ofstream& output) const;
		void outputCountryLeader(ofstream& output) const;
		void outputOOBLine(ofstream& output) const;
		void outputCommonCountryFile() const;
		void outputIdeas(const set<const HoI4Advisor*, advisorCompare>& ideologicalAdvisors) const;


		const HoI4World* theWorld;
		const V2Country* srcCountry;
		string filename;

		bool human;

		string governmentIdeology;
		string leaderIdeology;
		const V2Party* rulingParty;
		set<const V2Party*, function<bool (const V2Party*, const V2Party*)>> parties;
		map<string, int> ideologySupport;
		date lastElection;

		const string						sphereLeader = "";
		string								tag;
		set<int>							provinces;
		map<int, HoI4State*> states;
		int									capitalStateNum;
		HoI4State*							capitalState;
		string								commonCountryFile;
		map<string, int>					technologies;
		map<string, int>					researchBonuses;
		map<string, HoI4Relations*>	relations;
		ConverterColor::Color color;
		double								nationalUnity;
		shared_ptr<const HoI4Faction>	faction;
		bool									factionLeader;
		set<string>							allies;
		set<string> puppets;
		string puppetMaster;
		map<string, double>				practicals;
		vector<HoI4Leader>				leaders;
		string graphicalCulture;
		string graphicalCulture2d;
		bool									majorNation;
		bool									civilized;
		vector<int>							brigs;
		int									convoys;
		
		int provinceCount;
		long armyStrength;
		double militaryFactories;
		double civilianFactories;
		double dockyards;
		double threat;

		// laws
		string				civil_law;
		string				conscription_law;
		string				economic_law;
		string				educational_investment_law;
		string				industrial_policy_laws;
		string				press_laws;
		string				training_laws;

		bool greatPower;

		// military stuff
		vector<HoI4DivisionType>				divisions;
		vector<HoI4Ship>							ships;
		vector<HoI4Airplane>						planes;
		int											navalLocation;
		map<string, int>							equipmentStockpile;

		shared_ptr<const HoI4FocusTree> nationalFocus;
};

#endif	// HoI4COUNTRY_H_

