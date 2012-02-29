#ifndef CK2WORLD
#define CK2WORLD


#include "..\Parsers\Object.h"
#include "..\Date.h"


class CK2World
{
	public:
		void	init(Object*);
		date	getEndDate();
	private:
		date	endDate;
};


#endif // CK2WORLD