// ****************************************************************************
// *                                                                          *
// *		     Victoria 2 to HoI3 conversion project                        *
// *                                                                          *
// ****************************************************************************


#ifndef MAPPER_H
#define MAPPER_H


#include "V2World.h"


typedef map< int, vector<int> > provinceMapping; // < destProvince, sourceProvinces >
typedef map< int, vector<int> > inverseProvinceMapping; // < sourceProvince, destProvinces >
provinceMapping initProvinceMap(Object* obj);
inverseProvinceMapping invertProvinceMap(provinceMapping provMap);
const vector<int>& getHoI3ProvinceNums(const inverseProvinceMapping& invProvMap, int V2ProvinceNum);

vector<string> processBlockedNations(Object* obj);

typedef vector< pair<string, string> > unionMapping;	// <culture, tag>
unionMapping initUnionMap(Object* obj);

typedef map< string, string > countryMapping;	// < sourceCountry, destCountry >
int initCountryMap(countryMapping& mapping, vector<string> V2Tags, vector<string> HoI3Tags, vector<string> blockedNations, Object* rulesObj);
vector<string>	getV2Tags(V2World& srcWorld);
void mergeNations(V2World&, Object* mergeObj);
void removeEmptyNations(V2World&);
void removeCulturallyAbsorbedNations(V2World&, unionMapping);
void removeLandlessNations(V2World&);

#endif // MAPPER_H
