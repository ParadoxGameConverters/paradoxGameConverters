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
#include "HoI4State.h"
#include "HoI4SupplyZone.h"
#include "../Mapper.h"



typedef const map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMapping;



class HoI4World
{
	public:
		HoI4World() {}

		void	output() const;

		void	importStates(map<int, vector<int>>& defaultStateToProvinceMap);
		void	importSuppplyZones(const map<int, vector<int>>& defaultStateToProvinceMap, map<int, int>& provinceToSupplyZoneMap);
		void	checkCoastalProvinces();
		void	importPotentialCountries();

		void	convertCountries(const V2World &sourceWorld, const CountryMapping& countryMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const HoI4StateMapping& stateMap);
		void	setStateInfo(const V2World & sourceWorld, const inverseProvinceMapping & inverseProvinceMap, const CountryMapping & countryMap, HoI4StateMapping & stateMap, V2Localisation & localisation);
		void	convertProvinceOwners(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, HoI4StateMapping& stateMap, V2Localisation& Vic2Localisations);
		void	convertNavalBases(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap);
		void	convertIndustry(const V2World& sourceWorld);
		void	convertResources();
		void	convertSupplyZones(const map<int, int>& provinceToSupplyZoneMap);
		void	convertTechs(const V2World& sourceWorld);
		void	convertDiplomacy(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	convertArmies(const V2World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		void	configureFactions(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	generateLeaders(const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap);
		void	convertArmies(const inverseProvinceMapping& inverseProvinceMap);
		void	consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap);
		void	convertVictoryPoints(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	setAIFocuses(const AIFocusModifiers& focusModifiers);
		void	copyFlags(const V2World &sourceWorld, const CountryMapping& countryMap);
		void	addMinimalItems(const inverseProvinceMapping& inverseProvinceMap);
		void	recordAllLandProvinces();
		void	checkAllProvincesMapped(const provinceMapping& provinceMap);

		map<string, HoI4Country*>	getPotentialCountries()	const { return potentialCountries; }

	private:
		void	getProvinceLocalizations(const string& file);
		void	checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string leader, const string& factionName);
		void	factionSatellites();
		void	setFactionMembers(const V2World &sourceWorld, const CountryMapping& countryMap);
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

		map<int, HoI4State*>			states;
		map<int, string>				stateFilenames;
		map<int, HoI4SupplyZone*>	supplyZones;
		map<int, string>				supplyZonesFilenames;
		map<int, HoI4Province*>		provinces;
		set<int>							landProvinces;
		map<string, HoI4Country*>	countries;
		map<string,HoI4Country*>	potentialCountries;
		HoI4Diplomacy					diplomacy;
		map<int, string>				continents;  // < province, continent >

		HoI4Localisation				localisation;

		string axisLeader;
		string alliesLeader;
		string cominternLeader;
};



#endif // HoI4WORLD_H_
