#ifndef CK2WORLD
#define CK2WORLD


#include "..\Parsers\Object.h"
#include "..\Date.h"
#include "CK2Title.h"
#include "CK2Province.h"
#include "CK2Character.h"
#include "CK2Dynasty.h"


class CK2World
{
	public:
		void							init(Object*);
		void							addDynasties(Object*);
		date							getEndDate();
		vector<CK2Title*>			getIndependentTitles();
		map<int, CK2Province*>	getProvinces();
		~CK2World();
	private:
		date							endDate;
		vector<CK2Title*>			independentTitles;
		vector<CK2Title*>			hreMembers;
		map<int, CK2Dynasty*>	dynasties;
		map<int, CK2Character*>	characters;
		map<string, CK2Title*>	titles;
		map<int, CK2Province*>	provinces;
};


#endif // CK2WORLD