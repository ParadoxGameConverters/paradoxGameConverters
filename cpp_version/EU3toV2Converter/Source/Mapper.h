#ifndef MAPPER_H
#define MAPPER_H



#include "Parsers\Object.h"
class EU3World;
class V2World;



// Province Mappings
typedef map< int, vector<int> > provinceMapping;			// < destProvince, sourceProvinces >
typedef map< int, vector<int> > inverseProvinceMapping;	// < sourceProvince, destProvinces >

provinceMapping initProvinceMap(Object* obj);
inverseProvinceMapping invertProvinceMap(const provinceMapping& provMap);
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, int eu3ProvinceNum);


// Country Mappings
typedef map< string, string > countryMapping;	// < sourceCountry, destCountry>

vector<string> processBlockedNations(Object* obj);
int initCountryMap(countryMapping& mapping, const EU3World& srcWorld, const V2World& destWorld, const vector<string>& blockedNations, Object* rulesObj);
void mergeNations(EU3World&, Object* mergeObj);
void uniteJapan(EU3World&);
void removeEmptyNations(EU3World&);
void removeDeadLandlessNations(EU3World&);
void removeOlderLandlessNations(EU3World&, int excess);
void removeLandlessNations(EU3World&);


// State Mappings
typedef map< int, vector<int> > stateMapping;	// < province, all other provinces in state >
stateMapping initStateMap(Object* obj);


// Culture Mappings
enum distinguisherType
{
	DTOwner,
	DTReligion
};
typedef pair<distinguisherType, string> distinguisher;
typedef struct {
	string srcCulture;
	string dstCulture;
	vector<distinguisher> distinguishers;
} cultureStruct;
typedef vector<cultureStruct> cultureMapping;
cultureMapping initCultureMap(Object* obj);


// Religion Mappings
typedef map<string, string> religionMapping;		// <srcReligion, destReligion>
religionMapping initReligionMap(Object* obj);


// Union Mappings
typedef vector< pair<string, string> > unionMapping;	// <cultures, tag>
unionMapping initUnionMap(Object* obj);


// Government Mappings
typedef map<string, string> governmentMapping;	// <srcGov, dstGov>
governmentMapping initGovernmentMap(Object* obj);


// Cultural Union Nation mappings
typedef map< string, vector<string> > unionCulturesMap; // <tag, cultures>
unionCulturesMap initUnionCultures(Object* obj);



#endif // MAPPER_H
