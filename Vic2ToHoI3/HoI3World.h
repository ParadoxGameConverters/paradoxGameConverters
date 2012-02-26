#ifndef HOI3WORLD_H_
#define HOI3WORLD_H_

#include <string>
#include "V2World.h"
#include "HoI3Country.h"
#include "HoI3Province.h"
#include "HoI3Diplomacy.h"
#include "Mapper.h"

class HoI3World {
	public:
		void				init(string HoI3Loc);
		void				addPotentialCountries(ifstream &countriesMapping, string HoI3Loc);
		vector<string>		getPotentialTags(); 
		void				sortCountries(const vector<string>& order);
		void				convertCountries(V2World sourceWorld, countryMapping countryMap);
		void				convertProvinces(V2World sourceWorld, provinceMapping provinceMap, countryMapping countryMap);
		void				convertCapitals(V2World sourceWorld, provinceMapping provinceMap);
		void				convertTechs(V2World& sourceWorld);
		void				output(FILE*);
		void				convertDiplomacy(V2World sourceWorld, countryMapping countryMap);
		//XXX: void				convertLeaders(V2World sourceWorld, map<int,int>& leaderIDMap);
		void				convertArmies(V2World sourceWorld, provinceMapping provinceMap, const map<int,int>& leaderIDMap);
		void				configureFactions(const countryMapping& countryMap);

private:
		void					getProvinceLocalizations(string file);
		void					checkManualFaction(const countryMapping& countryMap, const vector<string>& candidateTags, vector<string>& destination, string logName);
		void					factionSatellites(vector<string>& faction);
		vector<int>				getPortProvinces(vector<int> locationCandidates);

		vector<HoI3Province>	provinces;
		vector<HoI3Country>		countries;
		vector<HoI3Country>		potentialCountries;
		HoI3Diplomacy			diplomacy;
		vector<string>			axisFaction;
		vector<string>			alliesFaction;
		vector<string>			cominternFaction;
};


#endif // HOI3WORLD_H_