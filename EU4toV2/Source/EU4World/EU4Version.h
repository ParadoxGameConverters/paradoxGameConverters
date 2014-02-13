#ifndef EU4VERSION_H_
#define EU4VERSION_H_


#include "..\Parsers\Object.h"
#include <string>
using namespace std;



class EU4Version
{
	public:
		EU4Version();
		EU4Version(Object* obj);
		EU4Version(string version);
		bool operator >= (EU4Version& rhs) const;
	private:
		int	first;
		int	second;
		int	third;
		int	fourth;
};



#endif EU4VERSION_H_