#ifndef CK2VERSION_H_
#define CK2VERSION_H_


#include <string>
using namespace std;


class CK2Version
{
	public:
		CK2Version(string versionString);
		bool operator == (CK2Version& rhs);
		bool operator > (CK2Version& rhs);
	private:
		int	major;
		int	minor;
		char	revision;
};



#endif // CK2VERSION_H_