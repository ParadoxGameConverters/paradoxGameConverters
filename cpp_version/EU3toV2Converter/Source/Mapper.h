// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************


#ifndef MAPPER_H
#define MAPPER_H


#include "EU3World.h"


typedef map< int, vector<int> > provinceMapping; // < destProvince, sourceProvinces >
typedef map< int, vector<int> > inverseProvinceMapping; // < sourceProvince, destProvinces >
provinceMapping initProvinceMap(Object* obj);
inverseProvinceMapping invertProvinceMap(provinceMapping provMap);
//vector<int> getV2ProvinceNums(provinceMapping provinceMap, int eu3ProvinceNum);
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, int eu3ProvinceNum);

vector<string> processBlockedNations(Object* obj);

typedef map< string, string > countryMapping;	// < sourceCountry, destCountry>
int initCountryMap(countryMapping& mapping, vector<string> EU3Tags, vector<string> V2Tags, vector<string> blockedNations, Object* rulesObj);
vector<string>	getEU3Tags(EU3World& srcWorld);
void mergeNations(EU3World&, Object* mergeObj);
void uniteJapan(EU3World&);
void removeEmptyNations(EU3World&);
void removeDeadLandlessNations(EU3World&);
void removeOlderLandlessNations(EU3World&, int);
void removeLandlessNations(EU3World&);

typedef map< int, vector<int> > stateMapping;	// < province, all other provinces in state >
stateMapping initStateMap(Object* obj);

enum distinguisherType
{
	owner,
	religion
};
typedef pair< distinguisherType, string > distinguisher;
typedef struct {
	string srcCulture;
	string dstCulture;
	vector< distinguisher > distinguishers;
} cultureStruct;
typedef vector < cultureStruct > cultureMapping;
cultureMapping initCultureMap(Object* obj);

typedef map< string, string> religionMapping;		// <srcReligion, destReligion>
religionMapping initReligionMap(Object* obj);

typedef vector< pair<string, string> > unionMapping;	// <cultures, tag>
unionMapping initUnionMap(Object* obj);

typedef map< string, string > governmentMapping;	// <srcGov, dstGov>
governmentMapping initGovernmentMap(Object* obj);

typedef struct {
	string tag;
	vector<string> cultures;
} unionCultureStruct;
typedef vector<unionCultureStruct> unionCulturesList;
unionCulturesList initUnionCultures(Object* obj);


#endif // MAPPER_H
