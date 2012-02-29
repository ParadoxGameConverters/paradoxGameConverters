#ifndef CK2WORLD
#define CK2WORLD


#include "..\Parsers\Object.h"
#include "..\Date.h"
#include "CK2Title.h"


class CK2World
{
	public:
		void	init(Object*);
		date	getEndDate();
		~CK2World();
	private:
		date					endDate;
		vector<CK2Title*>	independentTitles;
};


#endif // CK2WORLD