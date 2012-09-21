#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include <fstream>
#include "..\Mappers.h"
#include "..\Date.h"



class CK2Province;
class CK2World;
class EU3Country;
class EU3Province;
class EU3Advisor;



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
		EU3World(CK2World*);

		void						output(FILE*);
		
		void						addPotentialCountries();
		void						setupProvinces(provinceMapping& provinceMap);

		void						convertCountries(countryMapping&, const religionMapping& religionMap, const cultureMapping& cultureMap, const provinceMapping provinceMap);
		void						convertProvinces(provinceMapping&, map<int, CK2Province*>&, countryMapping&, cultureMapping& cultureMap, religionMapping& religionMap, continentMapping& continentMap, adjacencyMapping& adjacencyMap, const tradeGoodMapping& tradeGoodMap, const religionGroupMapping& EU3ReligionGroup);
		void						addAcceptedCultures();
		void						convertAdvisors(inverseProvinceMapping&, provinceMapping&, CK2World&);
		void						convertTech(countryMapping& countryMap, const religionGroupMapping& religionGroupMap);
		void						convertGovernments(const religionGroupMapping& religionGroupMap);
		void						convertEconomies(const cultureGroupMapping& cultureGroups, const tradeGoodMapping& tradeGoodMap);

		void						setJapaneseEmperor(string tag)	{ japaneseEmperor = tag; };
		void						addDamiyo(string tag)				{ daimyos.push_back(tag); };
		void						setShogun(string tag)				{ shogun = tag; };
		void						setShogunPower(double power)		{ shogunPower = power; };
		
		map<string, EU3Country*>	getCountries() const { return countries; };
	private:
		int									options[OPTIONS_END];
		date									startDate;
		map<int, EU3Province*>			provinces;
		map<string, EU3Country*>		countries;
		vector<string>						europeanCountries;
		vector<EU3Advisor*>				advisors;
		map< string, vector<string> >	mapSpreadStrings;

		string			japaneseEmperor;
		vector<string>	daimyos;
		string			shogun;
		double			shogunPower;
};



#endif	// EU3WORLD_H_