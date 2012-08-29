#ifndef MAPPERS_H_
#define MAPPERS_H_


#include <vector>
#include <map>
using namespace std;



class Object;
class CK2World;
class CK2Version;
class CK2Title;
class	CK2Province;
class EU3Country;

typedef map< int, vector<int> > provinceMapping;			// < destProvince,	sourceProvinces	>
typedef map< int, vector<int> > inverseProvinceMapping;	// < sourceProvince,	destProvinces		>
provinceMapping initProvinceMap(Object* obj, CK2Version* version);
inverseProvinceMapping invertProvinceMap(provinceMapping& provMap);

vector<string> processBlockedNations(Object* obj);

typedef map< const CK2Title*, EU3Country* > countryMapping;
int initCountryMap(countryMapping& mapping, map<string, CK2Title*>& CK2Titles, map<string, EU3Country*>& EU3Countries, vector<string>& blockedNations, Object* rulesObj);


// Culture Mappings
enum distinguisherType
{
	DTDeJure,
	DTKingdomCulture,
	DTReligion,
	DTHREMember
};
typedef pair<distinguisherType, string> distinguisher;
typedef struct {
	string srcCulture;
	string dstCulture;
	vector<distinguisher> distinguishers;
} cultureStruct;
typedef vector<cultureStruct> cultureMapping;
cultureMapping initCultureMap(Object* obj);
string determineEU3Culture(string CK2Culture, const cultureMapping& cultureMap, const CK2Province* srcProvince);

typedef map<string, string> cultureGroupMapping; // CK2 culture, CK2 culture group
void addCultureGroupMappings(Object* obj, cultureGroupMapping& map);

typedef map<string, string> religionMapping; // CK2 religion, EU3 religion
religionMapping initReligionMap(Object* obj);

typedef map<string, string> religionGroupMapping; // CK2 religion, CK2 religion group
void addReligionGroupMappings(Object* obj, religionGroupMapping& map);



#endif	// MAPPERS_H_