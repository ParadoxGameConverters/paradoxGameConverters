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
#include "HoI4Alignment.h"
#include "HoI4Army.h"
#include "HoI4FocusTree.h"
#include "HoI4Navy.h"
#include "HoI4Province.h"
#include "HoI4Relations.h"
#include "HoI4State.h"
#include "../Color.h"
#include "Date.h"
#include "../V2World/V2Army.h"
#include <vector>
#include <set>
using namespace std;

class HoI4World;
class V2Country;
struct V2Party;
class HoI4Minister;
class HoI4Leader;
class HoI4Faction;


class HoI4Country
{
	public:
		HoI4Country(string _tag, string _commonCountryFile, HoI4World* _theWorld);

		void initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _vic2ideology, map<int, int>& leaderMap, governmentJobsMap governmentJobs, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const map<int, int>& stateMap, map<int, HoI4State*> states);
		void initFromHistory();
		void setGovernmentToNeutral();
		void convertIdeologySupport(const set<string>& majorIdeologies);
		void generateLeaders(leaderTraitsMap leaderTraits, portraitMapping& portraitMap);
		void		convertNavy(map<int, HoI4State*> states);
		void		convertAirforce();
		void		convertArmyDivisions();
		void		setTechnology(string tech, int level);
		void		addProvince(int _province);
		void addState(HoI4State* _state);
		void calculateIndustry();
		void reportIndustry(ofstream& out);
		void addVPsToCapital(int VPs);
		void outputToCommonCountriesFile(ofstream& countriesFile) const;
		void outputColors(ofstream& out) const;
		void outputToNamesFiles(ofstream& namesFile) const;
		void output(const map<int, HoI4State*>& states, const vector<HoI4Faction*>& Factions) const;
		void outputIdeaGraphics(ofstream& ideasFile) const;

		void		setSphereLeader(string SphereLeader) { sphereLeader == SphereLeader; }
		void		setFaction(HoI4Faction* newFaction) { faction = newFaction; }
		void		setFactionLeader() { factionLeader = true; }
		void addNationalFocus(HoI4FocusTree* NF) { nationalFocus = NF; }
		void setGreatPower() { greatPower = true; }

		HoI4Relations*								getRelations(string withWhom) const;
		HoI4State* getCapital();
		double getStrengthOverTime(double years) const;
		double getMilitaryStrength() const;
		double getEconomicStrength(double years) const;

		bool isHuman() const { return human; }
		const map<string, HoI4Relations*>&	getRelations() const { return relations; }
		set<int>									getProvinces() const { return provinces; }
		string										getTag() const { return tag; }
		const V2Country*							getSourceCountry() const { return srcCountry; }
		HoI4Faction*								getFaction() const { return faction; }
		HoI4Alignment*								getAlignment() { return &alignment; }
		string getGovernmentIdeology() const { return governmentIdeology; }
		map<string, int> getIdeologySupport() const { return ideologySupport; }
		const set<string>&						getAllies() const { return allies; }
		set<string>&								editAllies() { return allies; }
		const set<string>&							getPuppets() const { return puppets; }
		void										addPuppet(string countryTag);
		const string&								getPuppetmaster() { return puppetMaster; }
		void										setPuppetmaster(string _master) { puppetMaster = _master; }
		map<string, double>&						getPracticals() { return practicals; }
		int											getCapitalNum() { return capital; }
		vector<int>									getBrigs() const { return brigs; }
		int											getCapitalProv() const { return capital; }
		const string									getSphereLeader() const { return sphereLeader; }
		V2Party* getRulingParty() const { return rulingParty; }
		set<V2Party*> getParties() const { return parties; }
		map<int, HoI4State*> getStates() const { return states; }
		bool isInFaction() const { return faction != nullptr; }
		bool isCivilized() const { return civilized; }

		int getTotalFactories() const { return totalfactories; }
		int getTechnologyCount() const { return technologies.size(); }
		int getProvinceCount() const { return provinceCount; }
		void setProvinceCount(int count) { provinceCount = count; }
		bool isGreatPower() const { return greatPower; }

	private:
		void determineCapitalFromVic2(const map<int, int>& provinceToStateIDMap, const map<int, HoI4State*>& states);
		bool isStateValidForCapital(map<int, int>::const_iterator capitalState, const map<int, HoI4State*>& states);
		bool isThisStateOwnedByUs(const HoI4State* state) const;
		bool isThisStateACoreWhileWeOwnNoStates(const HoI4State* state) const;
		void setCapitalInCapitalState(int capitalProvince, const map<int, HoI4State*>& states);
		void findBestCapital();

		vector<int>	getPortProvinces(vector<int> locationCandidates, map<int, HoI4Province*> allProvinces);

		void outputNamesSet(ofstream& namesFile, const vector<string>& names, const string& tabs) const;
		void outputHistory(const map<int, HoI4State*>& states, const vector<HoI4Faction*>& Factions) const;
		void outputRelations(ofstream& output) const;
		void outputCountryLeader(ofstream& output) const;
		void outputOOB() const;
		void outputCommonCountryFile() const;
		void outputIdeas() const;


		HoI4World* theWorld;
		const V2Country* srcCountry;
		string filename;

		bool human;

		string governmentIdeology;
		string leaderIdeology;
		V2Party* rulingParty;
		set<V2Party*> parties;
		map<string, int> ideologySupport;

		const string						sphereLeader = "";
		string								tag;
		set<int>							provinces;
		map<int, HoI4State*> states;
		int									capital;
		string								commonCountryFile;
		map<string, int>					technologies;
		HoI4Alignment						alignment;
		int									totalfactories;
		map<string, HoI4Relations*>	relations;
		Color									color;
		double								nationalUnity;
		HoI4Faction*						faction;
		bool									factionLeader;
		set<string>							allies;
		set<string>							puppets;
		string								puppetMaster;
		map<string, double>				practicals;
		vector<HoI4Minister>				ministers;
		vector<HoI4Minister>				rulingMinisters;
		vector<HoI4Leader>				leaders;
		string								graphicalCulture;
		bool									majorNation;
		bool									civilized;
		vector<int>							brigs;
		int									convoys;
		
		map<std::pair<int, int>, string>	NFT;
		map<string, vector<HoI4Country*>>	CountryTargets;
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
		vector<HoI4DivisionTemplateType>		divisionTemplates;
		vector<HoI4DivisionType>				divisions;
		vector<HoI4Ship>							ships;
		vector<HoI4Airplane>						planes;
		int											navalLocation;

		HoI4FocusTree* nationalFocus;
};

#endif	// HoI4COUNTRY_H_
