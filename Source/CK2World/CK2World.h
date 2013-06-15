#ifndef CK2WORLD
#define CK2WORLD


#include <vector>
#include <map>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;



class Object;
class CK2BuildingFactory;
class CK2Version;
class CK2Title;
class CK2Province;
class CK2Barony;
class CK2Dynasty;
class CK2Character;
class CK2Trait;
class CK2War;
class CK2Version;



class CK2World
{
	public:
		CK2World();
		void							init(Object*, const cultureGroupMapping& cultureGroupMap);
		void							addBuildingTypes(Object*);
		void							addDynasties(Object*);
		void							addTraits(Object*);
		void							addPotentialTitles(Object*);
		void							mergeTitles();

		CK2Version*					getVersion()				const	{ return version; };
		date							getEndDate()				const { return endDate; };
		map<string, CK2Title*>	getIndependentTitles()	const { return independentTitles; };
		map<string, CK2Title*>	getAllTitles()				const { return titles; };
		CK2Title*					getHRETitle()				const { return hreTitle; };
		map<int, CK2Province*>	getProvinces()				const { return provinces; };
		map<string, CK2Title*>	getHREMembers()			const { return hreMembers; };

		vector<double>				getAverageTechLevels(CK2Version& version) const;
	private:
		CK2BuildingFactory*		buildingFactory;

		CK2Version*					version;
		date							endDate;
		map<string, CK2Title*>	independentTitles;
		map<string, CK2Title*>	hreMembers;
		map<int, CK2Dynasty*>	dynasties;
		map<int, CK2Character*>	characters;
		map<int, CK2Trait*>		traits;
		map<string, CK2Title*>	potentialTitles;
		map<string, CK2Title*>	titles;
		CK2Title*					hreTitle;
		map<int, CK2Province*>	provinces;
		map<string, CK2Barony*>	baronies;
		vector<CK2War*>			wars;
};


#endif // CK2WORLD