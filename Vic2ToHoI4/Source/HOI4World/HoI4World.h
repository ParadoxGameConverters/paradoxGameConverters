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
#include "HoI4State.h"
#include "../Mapper.h"



typedef const map<wstring, multimap<HoI4RegimentType, unsigned> > unitTypeMapping;



class HoI4World
{
	public:
		HoI4World() {}

		void	output() const;

		void	importProvinces(const provinceMapping& provinceMap);
		void	checkCoastalProvinces();
		void	importPotentialCountries();
		void	convertCountries(const V2World &sourceWorld, const CountryMapping& countryMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const HoI4StateMapping& stateMap);
		void	convertProvinceOwners(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, HoI4StateMapping& stateMap);
		void	convertNavalBases(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap);
		void	convertProvinceItems(const V2World& sourceWorld, const provinceMapping& provinceMap, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		void	convertTechs(const V2World& sourceWorld);
		void	convertDiplomacy(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	convertArmies(const V2World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		void	configureFactions(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	generateLeaders(const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap);
		void	consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap);
		void	convertVictoryPoints(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	setAIFocuses(const AIFocusModifiers& focusModifiers);
		void	copyFlags(const V2World &sourceWorld, const CountryMapping& countryMap);
		void	addMinimalItems(const inverseProvinceMapping& inverseProvinceMap);
		int		getStates() const;
		map<string, HoI4Country*>	getPotentialCountries()	const { return potentialCountries; }
	private:
		void	getProvinceLocalizations(const wstring& file);
		void	checkManualFaction(const CountryMapping& countryMap, const vector<wstring>& candidateTags, string leader, const wstring& factionName);
		void	factionSatellites();
		void	checkAllProvincesMapped(const provinceMapping& provinceMap);
		void	setFactionMembers(const V2World &sourceWorld, const CountryMapping& countryMap);
		void	setAlignments();

		vector<int>					getPortProvinces(const vector<int>& locationCandidates);
		unitTypeMapping			getUnitMappings();
		vector<int>					getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap);
		int							getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, wstring owner);
		vector<HoI4Regiment*>	convertRegiments(const unitTypeMapping& unitTypeMap, vector<V2Regiment*>& sourceRegiments, map<wstring, unsigned>& typeCount, const pair<wstring, HoI4Country*>& country);
		HoI4RegGroup*				createArmy(const inverseProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap, wstring tag, const V2Army* oldArmy, vector<HoI4Regiment*>& sourceRegiments, int& airForceIndex);

		void	outputCommonCountries() const;
		void	outputAutoexecLua() const;
		void	outputLocalisations() const;
		void	outputHistory() const;

		map<int, HoI4State*>			states;
		map<int, HoI4Province*>		provinces;
		map<string, HoI4Country*>	countries;
		map<string,HoI4Country*>	potentialCountries;
		HoI4Diplomacy					diplomacy;
		map<int, wstring>				continents;  // < province, continent >

		string axisLeader;
		string alliesLeader;
		string cominternLeader;
};



#endif // HoI4WORLD_H_
