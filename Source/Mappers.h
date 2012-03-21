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

typedef map< string, EU3Country* > countryMapping;	// < sourceTitle, destCountry>
int initCountryMap(countryMapping& mapping, vector<string> CK2Titles, vector<EU3Country*> EU3Countries, vector<string> blockedNations, Object* rulesObj);
vector<string>	getCK2Titles(CK2World& srcWorld);



#endif	// MAPPERS_H_