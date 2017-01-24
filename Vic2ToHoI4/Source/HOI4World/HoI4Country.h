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
class HoI4Minister;
class HoI4Leader;
class HoI4Faction;

typedef struct
{
	string			name;
	string			localisationString;
	string			ideology;
	string			economic_pol;
	string			trade_pol;
	string			rel_pol;
	string			citizen_pol;
	string			war_pol;
	unsigned int	popularity;
	unsigned int	organization;
} HoI4Party;

class HoI4Country
{
public:
	HoI4Country(string _tag, string _commonCountryFile, HoI4World* _theWorld, bool _newCountry = false);
	void		output(const map<int, HoI4State*>& states, const vector<HoI4Faction*>& Factions) const;
	void		outputCommonCountryFile() const;
	void		outputColors(ofstream& out) const;
	void outputToCommonCountriesFile(ofstream& countriesFile) const;

	void		initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _vic2ideology, map<int, int>& leaderMap, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const map<int, int>& stateMap, map<int, HoI4State*> states);
	void		initFromHistory();
	void		generateLeaders(leaderTraitsMap leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap);
	void		convertNavy(map<int, HoI4State*> states);
	void		convertAirforce();
	void		convertArmyDivisions();
	void		setTechnology(string tech, int level);
	void		addProvince(int _province);
	void addState(HoI4State* _state);
	void calculateIndustry();
	void reportIndustry(ofstream& out);
	void addVPsToCapital(int VPs);

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

	const map<string, HoI4Relations*>&	getRelations() const { return relations; }
	set<int>									getProvinces() const { return provinces; }
	string										getTag() const { return tag; }
	const V2Country*							getSourceCountry() const { return srcCountry; }
	string										getGovernment() const { return government; }
	bool											isNewCountry() const { return newCountry; }
	HoI4Faction*								getFaction() const { return faction; }
	HoI4Alignment*								getAlignment() { return &alignment; }
	string										getIdeology() const { return ideology; }
	string										getRulingIdeology() const { return rulingHoI4Ideology; }
	const set<string>&						getAllies() const { return allies; }
	set<string>&								editAllies() { return allies; }
	map<string, double>&						getPracticals() { return practicals; }
	int											getCapitalNum() { return capital; }
	vector<int>									getBrigs() const { return brigs; }
	int											getCapitalProv() const { return capital; }
	const string									getSphereLeader() const { return sphereLeader; }
	HoI4Party									getRulingParty() const { return RulingPartyModel; }
	map<int, HoI4State*> getStates() const { return states; }
	bool isInFaction() const { return faction != nullptr; }
	bool isCivilized() const { return civilized; }

	vector<HoI4Party> getParties() const { return parties; }
	int getTotalFactories() const { return totalfactories; }
	int getTechnologyCount() const { return technologies.size(); }
	int getProvinceCount() const { return provinceCount; }
	void setProvinceCount(int count) { provinceCount = count; }
	bool isGreatPower() const { return greatPower; }

private:
	void			outputOOB()						const;
	void			outputPracticals(FILE*)		const;
	void			outputTech(FILE*)				const;
	void			outputParties(FILE*)			const;
	void			outputLeaders()				const;
	void outputRelations(ofstream& output) const;

	void determineCapitalFromVic2(const map<int, int>& provinceToStateIDMap, const map<int, HoI4State*>& states);
	bool isStateValidForCapital(map<int, int>::const_iterator capitalState, const map<int, HoI4State*>& states);
	bool isThisStateOwnedByUs(const HoI4State* state) const;
	bool isThisStateACoreWhileWeOwnNoStates(const HoI4State* state) const;
	void setCapitalInCapitalState(int capitalProvince, const map<int, HoI4State*>& states);
	void findBestCapital();

	vector<int>	getPortProvinces(vector<int> locationCandidates, map<int, HoI4Province*> allProvinces);
	void			convertParties(const V2Country* srcCountry, vector<V2Party*> V2Parties, V2Party* rulingParty, string& rulingIdeology);

	void setPartyPopularity();

	HoI4World*							theWorld;
	const V2Country*					srcCountry;
	string								filename;
	bool								newCountry;	// true if this country is being added by the converter, i.e. doesn't already exist in HoI4
	const string						sphereLeader = "";
	string								tag;
	set<int>							provinces;
	map<int, HoI4State*> states;
	int									capital;
	string								commonCountryFile;
	map<string, int>					technologies;
	string								government;
	HoI4Alignment						alignment;
	string								ideology;
	int									totalfactories;
	map<string, HoI4Relations*>	relations;
	Color									color;
	double								nationalUnity;
	HoI4Faction*						faction;
	bool									factionLeader;
	set<string>							allies;
	map<string, double>				practicals;
	vector<HoI4Party>					parties;
	vector<HoI4Minister>				ministers;
	vector<HoI4Minister>				rulingMinisters;
	vector<HoI4Leader>				leaders;
	string								graphicalCulture;
	string								rulingHoI4Ideology;
	bool									majorNation;
	bool									civilized;
	vector<int>							brigs;
	int									convoys;
	HoI4Party							RulingPartyModel;
	map<std::pair<int, int>, string>	NFT;
	map<string, vector<HoI4Country*>>	CountryTargets;
	int provinceCount;
	long armyStrength;
	double militaryFactories;
	double civilianFactories;
	double dockyards;

	// laws
	string				civil_law;
	string				conscription_law;
	string				economic_law;
	string				educational_investment_law;
	string				industrial_policy_laws;
	string				press_laws;
	string				training_laws;

	// faction popularity
	int communismPopularity;
	int democraticPopularity;
	int facismPopularity;
	int liberalPopularity;
	int socialistPopularity;
	int syndicalistPopularity;
	int ancapPopularity;
	int autocraticPopularity;
	int neutralityPopularity;

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
