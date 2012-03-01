#include <vector>
#include <map>
#include "Parsers\Object.h"
#include "CK2World\CK2World.h"
using namespace std;


typedef map< int, vector<int> > provinceMapping;			// < destProvince,	sourceProvinces	>
typedef map< int, vector<int> > inverseProvinceMapping;	// < sourceProvince,	destProvinces		>
provinceMapping initProvinceMap(Object* obj);
inverseProvinceMapping invertProvinceMap(provinceMapping provMap);

vector<string> processBlockedNations(Object* obj);

typedef map< string, string > countryMapping;	// < sourceTitle, destCountry>
int initCountryMap(countryMapping& mapping, vector<string> CK2Titles, vector<string> EU3Tags, vector<string> blockedNations, Object* rulesObj);
vector<string>	getCK2Titles(CK2World& srcWorld);
