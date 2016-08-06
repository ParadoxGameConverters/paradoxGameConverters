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



#ifndef HoI4WORLD_H_
#define HoI4WORLD_H_

#include <string>
#include "HoI4Country.h"
#include "HoI4Province.h"
#include "HoI4Diplomacy.h"
#include "HoI4Localisation.h"
#include "HoI4States.h"
#include "HoI4StrategicRegion.h"
#include "HoI4SupplyZone.h"
#include "../Mapper.h"



typedef const map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMapping;


class HoI4Faction
{
	
public:
	HoI4Faction(HoI4Country* leader, vector<HoI4Country*> members) { Factionleader = leader; Factionmembers = members; }

	HoI4Country* getLeader() { return Factionleader; }
	vector<HoI4Country*> getMembers() { return Factionmembers; }
	void addMember(HoI4Country* addedCon) { Factionmembers.push_back(addedCon); }
private:
	HoI4Country* Factionleader;
	vector<HoI4Country*> Factionmembers;
	
};
class HoI4World
{
	public:
		HoI4World(const V2World* sourceWorld) { this->sourceWorld = sourceWorld; }

		void	output() const;

		void addStates(const HoI4States* theStates) { states = theStates; }

		void	importSuppplyZones(const map<int, vector<int>>& defaultStateToProvinceMap, map<int, int>& provinceToSupplyZoneMap);
		void	importStrategicRegions();
		void	checkCoastalProvinces();
		void	convertCountries(const CountryMapping& countryMap, const Vic2ToHoI4ProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);
		void	convertNavalBases(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap);
		void	convertIndustry();
		void	convertResources();
		void	convertSupplyZones(const map<int, int>& provinceToSupplyZoneMap);
		void	convertStrategicRegions();
		void	convertTechs();
		void	convertDiplomacy(const CountryMapping& countryMap);
		void	convertArmies(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		void	configureFactions(const CountryMapping& countryMap);
		void	generateLeaders(const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap);
		void	convertArmies(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap);
		void	convertNavies();
		void	convertAirforces();
		void convertCapitalVPs(const CountryMapping& countryMap);
		void	setAIFocuses(const AIFocusModifiers& focusModifiers);
		void fillCountryProvinces();
		void    setSphereLeaders(const V2World & sourceWorld, const CountryMapping & countryMap);
		void    thatsgermanWarCreator(const V2World & sourceWorld, const CountryMapping& countryMap);
		HoI4Country* FindProvOwner(int prov);
		vector<int> getCountryProvinces(HoI4Country * Country);
		vector<HoI4Faction*> CreateFactions(const V2World & sourceWorld, const CountryMapping & countryMap);
		HoI4Country *    GetFactionLeader(vector<HoI4Country*> Faction);
		double    GetFactionStrength(HoI4Faction* Faction, int years);
		vector<HoI4Country*>    returnGreatCountries(const V2World & sourceWorld, const CountryMapping & countryMap);
		string returnIfSphere(HoI4Country * leadercountry, HoI4Country * posLeaderCountry, const V2World & sourceWorld, const CountryMapping & countryMap);
		vector<HoI4Faction*> FascistWarMaker(HoI4Country * Leader, V2World sourceWorld, CountryMapping countryMap);
		vector<HoI4Faction*> CommunistWarCreator(HoI4Country * Leader, V2World sourceWorld, CountryMapping countryMap);
		vector<HoI4Faction*> DemocracyWarCreator(HoI4Country * Leader, V2World sourceWorld, CountryMapping countryMap);
		vector<HoI4Faction*> MonarchyWarCreator(HoI4Country * Leader, V2World sourceWorld, CountryMapping countryMap);
		void CreateFactionEvents(HoI4Country * Leader, HoI4Country * newAlly);
		string HowToTakeLand(HoI4Country * TargetCountry, HoI4Country * AttackingCountry, double time);
		vector<HoI4Country*> GetMorePossibleAllies(HoI4Country * CountryThatWantsAllies);
		double getDistanceBetweenCountries(const HoI4Country* Country1, const HoI4Country* Country2);
		bool bothCountriesHaveCapitals(const HoI4Country* Country1, const HoI4Country* Country2);
		pair<int, int> getCapitalPosition(const HoI4Country* country);
		pair<int, int> getProvincePosition(int provinceNum);
		void establishProvincePositions();
		void processPositionLine(const string& line);
		vector<string> tokenizeLine(const string& line);
		void addProvincePosition(const vector<string>& tokenizedLine);
		double getDistanceBetweenPoints(pair<int, int> point1, pair<int, int> point2);
		double GetFactionStrengthWithDistance(HoI4Country * HomeCountry, vector<HoI4Country*> Faction, double time);
		HoI4Faction* findFaction(HoI4Country * CheckingCountry);
		bool checkIfGreatCountry(HoI4Country * checkingCountry, const V2World & sourceWorld, const CountryMapping & countryMap);
		map<string, HoI4Country*> findNeighbors(vector<int> CountryProvs, HoI4Country * CheckingCountry);
		void fillProvinces();
		string createAnnexEvent(HoI4Country * Annexer, HoI4Country * Annexed, int eventnumber);
		string createSudatenEvent(HoI4Country * Annexer, HoI4Country * Annexed, int eventnumber, vector<int> claimedStates);
		string createDemocracyNF(HoI4Country * Home, vector<HoI4Country*> CountriesToContain, int XStart);
		string createMonarchyEmpireNF(HoI4Country * Home, HoI4Country * Annexed1, HoI4Country * Annexed2, HoI4Country * Annexed3, HoI4Country * Annexed4, int ProtectorateNumber, int AnnexNumber, int x);
		void fillProvinceNeighbors();
		string genericFocusTreeCreator(HoI4Country * CreatingCountry);
		void addStatesToCountries();
		void fillCountryIC();
		void outputRelations();
		void	checkAllProvincesMapped(const HoI4ToVic2ProvinceMapping& provinceMap);

		map<string, HoI4Country*>	getCountries()	const { return countries; }

	private:
		void	getProvinceLocalizations(const string& file);
		void	checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string leader, const string& factionName);
		void	factionSatellites();
		void	setAlignments();

		void addBasicCapitalVPs(const CountryMapping& countryMap);
		void addGreatPowerVPs(const CountryMapping& countryMap);
		void addStrengthVPs();
		double getStrongestCountryStrength();
		int calculateStrengthVPs(HoI4Country* country, double greatestStrength);

		vector<int>					getPortProvinces(const vector<int>& locationCandidates);
		vector<int>					getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		int							getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner);

		void	outputCommonCountries() const;
		void	outputColorsfile() const;
		void	outputAutoexecLua() const;
		void	outputLocalisations() const;
		void	outputMap() const;
		void	outputHistory() const;
		void	outputSupply() const;

		map<int, vector<int>>			provinceNeighbors;
		const V2World* sourceWorld;

		const HoI4States* states;
		
		map<int, HoI4Province*>		provinces;
		map<string, HoI4Country*>	countries;
		HoI4Diplomacy					diplomacy;
		map<int, int>					stateMap;
		vector<HoI4Country*> AggressorFactions;
		map<HoI4Country*, vector<HoI4Country*>> WorldTargetMap;
		string aiOutputLog;

		// map items
		map<int, string>						continents;  // < province, continent >
		map<int, HoI4SupplyZone*>			supplyZones;
		map<int, string>						supplyZonesFilenames;
		map<int, HoI4StrategicRegion*>	strategicRegions;
		map<int, int>							provinceToStratRegionMap;
		map<int, pair<int, int>> provincePositions;

		HoI4Localisation				localisation;
		vector<HoI4Faction*> Factions;
		string axisLeader;
		string alliesLeader;
		string cominternLeader;
		map<int, vector<string>> provincemap;
		bool fascismIsRelevant;
		bool communismIsRelevant;
		string NewsEvents;
		int NewsEventNumber;
		string nfEvents;
		int nfEventNumber;
};



#endif // HoI4WORLD_H_
