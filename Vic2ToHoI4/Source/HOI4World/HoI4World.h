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

#include <string>
#include "HoI4Buildings.h"
#include "HoI4Country.h"
#include "HoI4Province.h"
#include "HoI4Diplomacy.h"
#include "HoI4Events.h"
#include "HoI4Localisation.h"
#include "HoI4States.h"
#include "HoI4StrategicRegion.h"



typedef const map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMapping;



class HoI4Faction;
class HoI4SupplyZones;



class HoI4World
{
	public:
		HoI4World(const V2World* sourceWorld);

		void	output() const;

		void convertIndustry();
		void convertResources();
		void convertStrategicRegions();
		void convertTechs();
		void generateLeaders();
		void convertArmies();
		void convertNavies();
		void convertAirforces();
		void convertCapitalVPs();

		void outputRelations() const;

		map<string, HoI4Country*> getCountries()	const { return countries; }
		vector<HoI4Country*> getGreatPowers() const { return greatPowers; }
		map<int, HoI4State*> getStates() const { return states->getStates(); }
		vector<HoI4Faction*> getFactions() const { return factions; }
		HoI4Events* getEvents() const { return events; }

	private:
		void importStrategicRegions();
		void convertNavalBases();

		void convertCountries();
		void convertCountry(pair<string, V2Country*> country, map<int, int>& leaderMap, governmentJobsMap governmentJobs, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);

		void	getProvinceLocalizations(const string& file);

		void determineGreatPowers();

		void convertAirBases();
		void addBasicAirBases();
		void addCapitalAirBases();
		void addGreatPowerAirBases();

		void addBasicCapitalVPs();
		void addGreatPowerVPs();
		void addStrengthVPs();
		double getStrongestCountryStrength();
		int calculateStrengthVPs(HoI4Country* country, double greatestStrength);

		void convertDiplomacy();
		void convertAgreements();
		void convertRelations();

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

		void createFactions();
		void logFactionMember(ofstream& factionsLog, const HoI4Country* member);
		string returnSphereLeader(HoI4Country* possibleSphereling);
		bool governmentsAllowFaction(string leaderGovernment, string allyGovernment);

		void outputCommonCountries() const;
		void outputColorsfile() const;
		void outputAutoexecLua() const;
		void outputLocalisations() const;
		void outputMap() const;
		void outputHistory() const;
		void outputCountries() const;


		const V2World* sourceWorld;

		HoI4SupplyZones* supplyZones;

		vector<HoI4Country*> greatPowers;

		HoI4States* states;
		
		map<int, HoI4Province*>		provinces;
		map<string, HoI4Country*>	countries;
		map<string, HoI4Country*> landedCountries;
		HoI4Diplomacy					diplomacy;
		map<int, int>					stateMap;

		// map items
		map<int, string>						continents;  // < province, continent >
		map<int, HoI4StrategicRegion*>	strategicRegions;
		map<int, int>							provinceToStratRegionMap;

		HoI4Localisation				localisation;

		HoI4Events* events;

		leaderTraitsMap leaderTraits;
		namesMapping namesMap;
		portraitMapping portraitMap;

		vector<HoI4Faction*> factions;

		HoI4Buildings* buildings;
};



#endif // HoI4WORLD_H_
