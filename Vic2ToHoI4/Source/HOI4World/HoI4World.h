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
#include "HoI4Buildings.h"
#include "HoI4Country.h"
#include "HoI4Province.h"
#include "HoI4Diplomacy.h"
#include "HoI4Events.h"
#include "HoI4Localisation.h"
#include "HoI4States.h"
#include "HoI4StrategicRegion.h"
#include "HoI4SupplyZone.h"



typedef const map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMapping;



class HoI4Faction;



class HoI4World
{
	public:
		HoI4World(const V2World* sourceWorld);

		void	output() const;

		void	importSuppplyZones(const map<int, vector<int>>& defaultStateToProvinceMap);
		void	importStrategicRegions();
		void	checkCoastalProvinces();
		void	convertCountries();
		void	convertNavalBases();
		void	convertIndustry();
		void	convertResources();
		void	convertSupplyZones();
		void	convertStrategicRegions();
		void	convertTechs();
		void	convertDiplomacy();
		void	convertArmies(const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		void	generateLeaders();
		void	convertArmies();
		void	convertNavies();
		void	convertAirforces();
		void convertCapitalVPs();
		void fillCountryProvinces();
		void    setSphereLeaders(const V2World* sourceWorld);
		void    thatsgermanWarCreator();
		HoI4Country* FindProvOwner(int prov);
		vector<int> getCountryProvinces(HoI4Country * Country);
		vector<HoI4Faction*> CreateFactions(const V2World* sourceWorld);
		HoI4Country *    GetFactionLeader(vector<HoI4Country*> Faction);
		double    GetFactionStrength(HoI4Faction* Faction, int years);
		vector<HoI4Country*>    returnGreatCountries(const V2World* sourceWorld);
		string returnIfSphere(HoI4Country * leadercountry, HoI4Country * posLeaderCountry, const V2World* sourceWorld);
		vector<HoI4Faction*> FascistWarMaker(HoI4Country * Leader, const V2World* sourceWorld);
		vector<HoI4Faction*> CommunistWarCreator(HoI4Country * Leader, const V2World* sourceWorld);
		vector<HoI4Faction*> DemocracyWarCreator(HoI4Country * Leader, const V2World* sourceWorld);
		vector<HoI4Faction*> MonarchyWarCreator(HoI4Country * Leader, const V2World* sourceWorld);
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
		bool checkIfGreatCountry(HoI4Country * checkingCountry, const V2World* sourceWorld);
		map<string, HoI4Country*> findNeighbors(vector<int> CountryProvs, HoI4Country * CheckingCountry);
		void fillProvinces();
		void fillProvinceNeighbors();
		void outputRelations() const;
		void	checkAllProvincesMapped();

		map<string, HoI4Country*>	getCountries()	const { return countries; }

	private:
		void	getProvinceLocalizations(const string& file);

		void addBasicCapitalVPs();
		void addGreatPowerVPs();
		void addStrengthVPs();
		double getStrongestCountryStrength();
		int calculateStrengthVPs(HoI4Country* country, double greatestStrength);

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
		void outputDefaultIndustry(const map<string, array<int, 3>>& countryIndustry);

		vector<int>					getPortProvinces(const vector<int>& locationCandidates);
		vector<int>					getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		int							getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner);

		void outputCommonCountries() const;
		void outputColorsfile() const;
		void outputAutoexecLua() const;
		void outputLocalisations() const;
		void outputMap() const;
		void outputHistory() const;
		void outputSupply() const;
		void outputCountries() const;

		map<int, vector<int>>			provinceNeighbors;
		const V2World* sourceWorld;

		HoI4States* states;
		
		map<int, HoI4Province*>		provinces;
		map<string, HoI4Country*>	countries;
		map<string, HoI4Country*> landedCountries;
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
		map<int, vector<string>> stateToProvincesMap;

		HoI4Events events;

		leaderTraitsMap leaderTraits;
		namesMapping namesMap;
		portraitMapping portraitMap;

		map<int, int> provinceToSupplyZoneMap;

		HoI4Buildings* buildings;
};



#endif // HoI4WORLD_H_
