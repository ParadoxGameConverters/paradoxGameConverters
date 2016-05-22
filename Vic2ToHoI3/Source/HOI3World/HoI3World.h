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



#ifndef HOI3WORLD_H_
#define HOI3WORLD_H_

#include <string>
#include "HoI3Country.h"
#include "HoI3Province.h"
#include "HoI3Diplomacy.h"
#include "../Mapper.h"



typedef const map<string, multimap<HoI3RegimentType, unsigned> > unitTypeMapping;


class HoI3World
{
	public:
		HoI3World() {}

		void	output() const;

		void	importProvinces(const provinceMapping& provinceMap);
		void	checkCoastalProvinces();
		void	importPotentialCountries();
		void	convertCountries(const V2World &sourceWorld, const CountryMapping& countryMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);
		void	convertProvinceOwners(const V2World &sourceWorld, const provinceMapping& provinceMap, const CountryMapping& countryMap);
		void	convertNavalBases(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap);
		void	convertProvinceItems(const V2World& sourceWorld, const provinceMapping& provinceMap, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, const HoI3AdjacencyMapping& HoI3AdjacencyMap);
		void	convertTechs(const V2World& sourceWorld);
		void	convertDiplomacy(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	convertArmies(const V2World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const HoI3AdjacencyMapping& HoI3AdjacencyMap);
		void	configureFactions(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	generateLeaders(const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap);
		void	consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap);
		void	convertVictoryPoints(const V2World& sourceWorld, const CountryMapping& countryMap);
		void	setAIFocuses(const AIFocusModifiers& focusModifiers);
		void	copyFlags(const V2World &sourceWorld, const CountryMapping& countryMap);
		void	addMinimalItems(const inverseProvinceMapping& inverseProvinceMap);

		map<string, HoI3Country*>	getPotentialCountries()	const { return potentialCountries; }

	private:
		void	getProvinceLocalizations(const string& file);
		void	checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string leader, const string& factionName);
		void	factionSatellites();
		void	checkAllProvincesMapped(const provinceMapping& provinceMap);
		void	setFactionMembers(const V2World &sourceWorld, const CountryMapping& countryMap);
		void	setAlignments();
		void	convertRegiments(const unitTypeMapping& unitTypeMap, vector<V2Regiment*>& sourceRegiments, map<string, unsigned>& typeCount, const pair<string, HoI3Country*>& country, HoI3RegGroup& destArmy, HoI3RegGroup& destAirForce);

		vector<int>				getPortProvinces(const vector<int>& locationCandidates);
		unitTypeMapping		getUnitMappings();
		vector<int>				getPortLocationCandidates(const vector<int>& locationCandidates, const HoI3AdjacencyMapping& HoI3AdjacencyMap);
		int						getAirLocation(HoI3Province* locationProvince, const HoI3AdjacencyMapping& HoI3AdjacencyMap, string owner);
		vector<V2Regiment*>	reorderRegiments(const vector<V2Regiment*>& sourceRegiments, const string& tag, const string& armyName);

		void	outputCommonCountries() const;
		void	outputAutoexecLua() const;
		void	outputLocalisations() const;
		void	outputHistory() const;

		map<int, HoI3Province*>		provinces;
		map<string, HoI3Country*>	countries;
		map<string,HoI3Country*>	potentialCountries;
		HoI3Diplomacy					diplomacy;
		map<int, string>				continents;  // < province, continent >
		vector<string>					countryOrder; // Order of countries in common\countries.txt. Used for determining faction leader. Also, REB should be first.

		string axisLeader;
		string alliesLeader;
		string cominternLeader;
};



#endif // HOI3WORLD_H_