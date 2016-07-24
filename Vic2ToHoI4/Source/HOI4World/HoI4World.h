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



class HoI4World
{
	public:
		HoI4World(const V2World* sourceWorld) { this->sourceWorld = sourceWorld; }

		void	output() const;

		void addStates(const HoI4States* theStates) { states = theStates; }

		void	importSuppplyZones(const map<int, vector<int>>& defaultStateToProvinceMap, map<int, int>& provinceToSupplyZoneMap);
		void	importStrategicRegions();
		void	checkCoastalProvinces();
		void	importPotentialCountries();

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
		void	convertVictoryPoints(const CountryMapping& countryMap);
		void	setAIFocuses(const AIFocusModifiers& focusModifiers);
		void	addMinimalItems(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap);
		void	checkAllProvincesMapped(const HoI4ToVic2ProvinceMapping& provinceMap);

		map<string, HoI4Country*>	getPotentialCountries()	const { return potentialCountries; }
		map<string, HoI4Country*>	getCountries()	const { return countries; }

	private:
		void	getProvinceLocalizations(const string& file);
		void	checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string leader, const string& factionName);
		void	factionSatellites();
		void	setAlignments();

		vector<int>					getPortProvinces(const vector<int>& locationCandidates);
		unitTypeMapping			getUnitMappings();
		vector<int>					getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		int							getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner);

		void	outputCommonCountries() const;
		void	outputColorsfile() const;
		void	outputAutoexecLua() const;
		void	outputLocalisations() const;
		void	outputMap() const;
		void	outputHistory() const;
		void	outputSupply() const;

		const V2World* sourceWorld;

		const HoI4States* states;
		
		map<int, HoI4Province*>		provinces;
		map<string, HoI4Country*>	countries;
		map<string,HoI4Country*>	potentialCountries;
		HoI4Diplomacy					diplomacy;

		// map items
		map<int, string>						continents;  // < province, continent >
		map<int, HoI4SupplyZone*>			supplyZones;
		map<int, string>						supplyZonesFilenames;
		map<int, HoI4StrategicRegion*>	strategicRegions;
		map<int, int>							provinceToStratRegionMap;

		HoI4Localisation				localisation;

		string axisLeader;
		string alliesLeader;
		string cominternLeader;
};



#endif // HoI4WORLD_H_
