#ifndef CK2WORLD
#define CK2WORLD


#include <vector>
#include <map>
#include "..\Date.h"
using namespace std;



class Object;
class CK2Version;
class CK2Title;
class CK2Province;
class CK2Barony;
class CK2Dynasty;
class CK2Character;
class CK2Trait;



class CK2World
{
	public:
		CK2World();
		void							init(Object*);
		void							addDynasties(Object*);
		void							addTraits(Object*);

		CK2Version*					getVersion()				const	{ return version; };
		date							getEndDate()				const { return endDate; };
		vector<CK2Title*>			getIndependentTitles()	const { return independentTitles; };
		map<string, CK2Title*>	getAllTitles()				const { return titles; };
		map<int, CK2Province*>	getProvinces()				const { return provinces; };
	private:
		CK2Version*					version;
		date							endDate;
		vector<CK2Title*>			independentTitles;
		vector<CK2Title*>			hreMembers;
		map<int, CK2Dynasty*>	dynasties;
		map<int, CK2Character*>	characters;
		map<int, CK2Trait*>		traits;
		map<string, CK2Title*>	titles;
		map<int, CK2Province*>	provinces;
		map<string, CK2Barony*>	baronies;
};


#endif // CK2WORLD