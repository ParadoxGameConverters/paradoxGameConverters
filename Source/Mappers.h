#ifndef MAPPERS_H_
#define MAPPERS_H_


#include <vector>
#include <map>
#include "Parsers\Object.h"
#include "CK2World\CK2World.h"
#include "EU3World\EU3Country.h"
using namespace std;


typedef map< int, vector<int> > provinceMapping;			// < destProvince,	sourceProvinces	>
typedef map< int, vector<int> > inverseProvinceMapping;	// < sourceProvince,	destProvinces		>
provinceMapping initProvinceMap(Object* obj);
inverseProvinceMapping invertProvinceMap(provinceMapping provMap);

vector<string> processBlockedNations(Object* obj);

typedef map< CK2Title*, EU3Country* > countryMapping;
int initCountryMap(countryMapping& mapping, vector<CK2Title*> CK2Titles, vector<EU3Country*> EU3Countries, vector<string> blockedNations, Object* rulesObj);



#endif	// MAPPERS_H_