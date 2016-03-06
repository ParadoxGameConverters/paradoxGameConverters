/*Copyright (c) 2015 The Paradox Game Converters Project

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

class HoI3World {
	public:
		HoI3World(const provinceMapping& provinceMap);
		void				createProvinceFiles(const V2World& sourceWorld, const provinceMapping& provinceMap);
		void				convertCountries(const V2World &sourceWorld, CountryMapping countryMap, const governmentMapping& governmentMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, governmentJobsMap governmentJobs, leaderTraitsMap leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);
		void				convertProvinces(const V2World &sourceWorld, provinceMapping provinceMap, CountryMapping countryMap, const adjacencyMapping &adjacencyMap);
		void				convertTechs(V2World& sourceWorld);
		void				output() const;
		void				convertDiplomacy(V2World& sourceWorld, CountryMapping countryMap);
		void				convertArmies(V2World& sourceWorld, inverseProvinceMapping inverseProvinceMap);
		void				configureFactions(const V2World &sourceWorld, const CountryMapping& countryMap);
		void				generateLeaders(leaderTraitsMap leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap);
		void				consolidateProvinceItems(inverseProvinceMapping& inverseProvinceMap);
		void				convertVictoryPoints(const V2World& sourceWorld, CountryMapping countryMap);

		map<string, HoI3Country*>	getPotentialCountries()	const;

		void copyFlags(const V2World &sourceWorld, CountryMapping countryMap);
		string convertIdeology(V2Party* v2Party) const;
private:
		void					getProvinceLocalizations(string file);
		void					checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string& leader, string factionName);
		void					factionSatellites();
		vector<int>			getPortProvinces(vector<int> locationCandidates);
		void					checkAllProvincesMapped(const provinceMapping& provinceMap);

		map<int, HoI3Province*>	provinces;
		map<string, HoI3Country*>		countries;
		vector<HoI3Country*>		potentialCountries;
		HoI3Diplomacy			diplomacy;
		map<int, string>		continents;  // < province, continent >

		vector<string> countryOrder; // Order of countries in common\countries.txt. Used for determining faction leader. Also, REB should be first.

		string axisLeader;
		string alliesLeader;
		string cominternLeader;
};


#endif // HOI3WORLD_H_