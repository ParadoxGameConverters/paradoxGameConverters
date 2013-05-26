#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include <fstream>
#include <tuple>
#include <set>
#include "..\Mappers.h"
#include "..\Date.h"



class CK2Province;
class CK2World;
class EU3Country;
class EU3Province;
class EU3Advisor;
class EU3Tech;
class EU3Diplomacy;



enum options
{
	ADVISORS	= 0,
	LEADERS,
	COLONISTS,
	MERCHANTS,
	MISSIONARIES,
	INFLATION,
	COLONIST_SIZE,
	DIFFICULTY,
	AI_AGGRESSIVENESS,
	LAND_SPREAD,
	SEA_SPREAD,
	SPIES,
	LUCKY_NATIONS,
	OPTIONS_END
};



class EU3World
{
	public:
		EU3World(CK2World*, EU3Tech*);

		void	output(FILE*);
		
		void	addHistoricalCountries();
		void	setupProvinces(provinceMapping& provinceMap);

		void	convertCountries(map<string, CK2Title*> CK2Titles, const religionMapping& religionMap, const cultureMapping& cultureMap, const provinceMapping provinceMap);
		void	convertProvinces(provinceMapping&, map<int, CK2Province*>&, cultureMapping& cultureMap, religionMapping& religionMap, continentMapping& continentMap, const adjacencyMapping& adjacencyMap, const tradeGoodMapping& tradeGoodMap, const religionGroupMapping& EU3ReligionGroup, Object* positionObj);
		void	addAcceptedCultures();
		void	convertAdvisors(inverseProvinceMapping&, provinceMapping&, CK2World&);
		void	convertTech(const CK2World& srcWorld);
		void	convertGovernments();
		void	convertEconomies(const cultureGroupMapping& cultureGroups, const tradeGoodMapping& tradeGoodMap);	
		void	assignTags(Object* rulesObj, vector<string>& blockedNations, const provinceMapping& provinceMap, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping& inverseProvinceMap);
		void	convertDiplomacy();
		void	convertArmies(const inverseProvinceMapping inverseProvinceMap);
		void	convertCoTs();
		
		void	setJapaneseEmperor(EU3Country* emperor)	{ japaneseEmperor = emperor; };
		void	addDamiyo(EU3Country* daimyo)					{ daimyos.push_back(daimyo); };
		void	setShogun(EU3Country* _shogun)				{ shogun = _shogun; };
		void	setShogunPower(double power)					{ shogunPower = power; };
		
		map<string, EU3Country*>	getCountries() const { return countries; };
	private:
		void	removeUnusedCountries();
		int	matchTags(Object* rulesObj, vector<string>& blockedNations, const provinceMapping& provinceMap, vector< tuple<EU3Country*, EU3Country*, string, string, int> >& mappings);
		void	determineMapSpread();
		void	convertHRE();
		void	addModCountries(const vector<EU3Country*>& countries, set<string> mappedTags, vector< tuple<EU3Country*, EU3Country*, string, string, int> >& mappings, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping& inverseProvinceMap);
		void	outputCountryFile(FILE* countryFile, EU3Country* country);

		CK2World*							srcWorld;
		int									options[OPTIONS_END];
		date									startDate;
		EU3Tech*								techData;

		vector<int>							centersOfTrade;
		map<int, EU3Province*>			provinces;
		map<string, EU3Country*>		countries;
		vector<EU3Country*>				convertedCountries;
		vector<EU3Advisor*>				advisors;
		map< string, vector<string> >	mapSpreadStrings;
		EU3Diplomacy*						diplomacy;
		EU3Country*							hreEmperor;

		EU3Country*				japaneseEmperor;
		vector<EU3Country*>	daimyos;
		EU3Country*				shogun;
		double					shogunPower;
};



#endif	// EU3WORLD_H_