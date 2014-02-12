#ifndef EU4VERSION_H_
#define EU4VERSION_H_


#include "..\Parsers\Object.h"



class EU4Version
{
	public:
		EU4Version();
		EU4Version(Object* obj);
		bool operator < (EU4Version& rhs);
	private:
		int	first;
		int	second;
		int	third;
		int	fourth;
};



#endif EU4VERSION_H_