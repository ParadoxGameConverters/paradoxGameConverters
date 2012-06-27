#ifndef MAPPER_H
#define MAPPER_H



#include "Parsers\Object.h"
class EU3World;
class V2World;



typedef map< int, vector<int> > provinceMapping; // < destProvince, sourceProvinces >
typedef map< int, vector<int> > inverseProvinceMapping; // < sourceProvince, destProvinces >
provinceMapping initProvinceMap(Object* obj);
inverseProvinceMapping invertProvinceMap(const provinceMapping& provMap);
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, int eu3ProvinceNum);


vector<string> processBlockedNations(Object* obj);


typedef map< string, string > countryMapping;	// < sourceCountry, destCountry>
int initCountryMap(countryMapping& mapping, const EU3World& srcWorld, const V2World& destWorld, const vector<string>& blockedNations, Object* rulesObj);
void mergeNations(const EU3World&, Object* mergeObj);
void uniteJapan(const EU3World&);
void removeEmptyNations(EU3World&);
void removeDeadLandlessNations(EU3World&);
void removeOlderLandlessNations(EU3World&, int excess);
void removeLandlessNations(EU3World&);


typedef map< int, vector<int> > stateMapping;	// < province, all other provinces in state >
stateMapping initStateMap(Object* obj);


enum distinguisherType
{
	owner,
	religion
};
typedef pair<distinguisherType, string> distinguisher;
typedef struct {
	string srcCulture;
	string dstCulture;
	vector<distinguisher> distinguishers;
} cultureStruct;
typedef vector<cultureStruct> cultureMapping;
cultureMapping initCultureMap(Object* obj);


typedef map<string, string> religionMapping;		// <srcReligion, destReligion>
religionMapping initReligionMap(Object* obj);


typedef vector< pair<string, string> > unionMapping;	// <cultures, tag>
unionMapping initUnionMap(Object* obj);


typedef map<string, string> governmentMapping;	// <srcGov, dstGov>
governmentMapping initGovernmentMap(Object* obj);


typedef struct {
	string tag;
	vector<string> cultures;
} unionCultureStruct;
typedef vector<unionCultureStruct> unionCulturesList;
unionCulturesList initUnionCultures(Object* obj);



#endif // MAPPER_H
