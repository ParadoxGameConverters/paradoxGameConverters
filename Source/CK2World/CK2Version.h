#ifndef CK2VERSION_H_
#define CK2VERSION_H_


#include <string>
using namespace std;


class CK2Version
{
	public:
		CK2Version(string versionString);

		bool operator > (CK2Version& rhs) const { return ( (major > rhs.major) || (minor > rhs.minor) || (revision > rhs.revision) ); };
	private:
		int	major;
		int	minor;
		int	revision;
};



#endif // CK2VERSION_H_