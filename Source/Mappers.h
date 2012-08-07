#ifndef MAPPERS_H_
#define MAPPERS_H_


#include <vector>
#include <map>
using namespace std;



class Object;
class CK2World;
class CK2Version;
class CK2Title;
class EU3Country;

typedef map< int, vector<int> > provinceMapping;			// < destProvince,	sourceProvinces	>
typedef map< int, vector<int> > inverseProvinceMapping;	// < sourceProvince,	destProvinces		>
provinceMapping initProvinceMap(Object* obj, CK2Version* version);
inverseProvinceMapping invertProvinceMap(provinceMapping& provMap);

vector<string> processBlockedNations(Object* obj);

typedef map< const CK2Title*, EU3Country* > countryMapping;
int initCountryMap(countryMapping& mapping, vector<CK2Title*>& CK2Titles, vector<EU3Country*>& EU3Countries, vector<string>& blockedNations, Object* rulesObj);


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



#endif	// MAPPERS_H_