#include <vector>
#include <map>
#include "Parsers\Object.h"
using namespace std;


typedef map< int, vector<int> > provinceMapping;			// < destProvince,	sourceProvinces	>
typedef map< int, vector<int> > inverseProvinceMapping;	// < sourceProvince,	destProvinces		>
provinceMapping initProvinceMap(Object* obj);
inverseProvinceMapping invertProvinceMap(provinceMapping provMap);
