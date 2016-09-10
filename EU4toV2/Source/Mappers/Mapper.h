/*Copyright (c) 2016 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#ifndef MAPPER_H
#define MAPPER_H



#include "Object.h"
#include "../EU4World/EU4Version.h"
#include "../FlagUtils.h"
#include <map>
#include <vector>
#include <unordered_set>
#include <set>
#include <memory>
class EU4World;
class V2World;

struct CustomFlag;




typedef vector< vector<int> > adjacencyMapping;
adjacencyMapping initAdjacencyMap();


typedef map<int, string>	continentMapping;	// <province, continent>
void initContinentMap(Object* obj, continentMapping& continentMap);


void mergeNations(EU4World&, Object* mergeObj);
void uniteJapan(EU4World&);
void removeEmptyNations(EU4World&);
void removeDeadLandlessNations(EU4World&);
void removeLandlessNations(EU4World&);


// State Mappings
typedef map< int, vector<int> >	stateMapping;	// < province, all other provinces in state >
typedef map< int, int >				stateIndexMapping; // < province, state index >
void initStateMap(Object* obj, stateMapping& stateMap, stateIndexMapping& stateIndexMap);


// EU4 regions
typedef map<int, set<string>>	EU4RegionsMapping;		// the regions in EU4
void initEU4RegionMapOldVersion(Object *obj, EU4RegionsMapping& regions);
void initEU4RegionMap(Object* areaObj, Object* regionObj, EU4RegionsMapping& regions);


// Distinguishers for mappings
enum distinguisherType
{
	DTOwner,
	DTReligion,
	DTRegion,
	DTProvince
};

// Culture Mappings
typedef pair<distinguisherType, string> distinguisher;
typedef struct {
	string srcCulture;							// the EU4 culture
	string dstCulture;							// the V2 culture
	vector<distinguisher> distinguishers;	// additional rules to match the culture
} cultureStruct;
typedef vector<cultureStruct> cultureMapping;
cultureMapping initCultureMap(Object* obj);
bool cultureMatch(const cultureMapping& cultureMap, const EU4RegionsMapping& regionsMap, string srcCulture, string& dstCulture, string religion, int EU4Province, string ownerTag);


// Religion Mappings
typedef map<string, string> religionMapping;		// <srcReligion, destReligion>
religionMapping initReligionMap(Object* obj);


// Minority cultures/religion
typedef vector<pair<string, string>> minorityPopMapping;
minorityPopMapping initMinorityPopMap(Object* obj);


// Union Mappings
typedef vector< pair<string, string> > unionMapping;	// <cultures, tag>
unionMapping initUnionMap(Object* obj);


// Government Mappings
typedef map<string, string> governmentMapping;	// <srcGov, dstGov>
governmentMapping initGovernmentMap(Object* obj);


// Cultural Union Nation mappings
typedef map< string, vector<string> > unionCulturesMap;	// <culture group, cultures>
typedef map< string, string > inverseUnionCulturesMap;	// <culture, culture group>
void initUnionCultures(Object* obj, unionCulturesMap& unionCultures, inverseUnionCulturesMap& inverseUnionCultures);

// idea effects
void initIdeaEffects(Object* obj, map<string, int>& armyInvIdeas, map<string, int>& commerceInvIdeas, map<string, int>& cultureInvIdeas, map<string, int>& industryInvIdeas, map<string, int>& navyInvIdeas, map<string, double>& armyTechIdeas, map<string, double>& commerceTechIdeas, map<string, double>& cultureTechIdeas, map<string, double>& industryTechIdeas, map<string, double>& navyTechIdeas, map<string, double>& UHLiberalIdeas, map<string, double>& UHReactionaryIdeas, vector< pair<string, int> >& literacyIdeas, map<string, int>& orderIdeas, map<string, int>& libertyIdeas, map<string, int>& equalityIdeas);

// colonial nations
typedef struct {
	string tag;
	string EU4Region;
	string V2Region;
	string cultureGroup;
} colonyStruct;
typedef vector<colonyStruct> colonyMapping;
colonyMapping initColonyMap(Object* obj);

// colonial nation flags
typedef struct {
	std::string name;
	string region;
	bool unique;
	std::string overlord;
} colonyFlag;
typedef map<string, shared_ptr<colonyFlag> > colonyFlagset; // <name, flag>
colonyFlagset initColonyFlagset(Object* obj);

typedef map<string, shared_ptr<CustomFlag> > customFlagset; // <name, flag>


// utility functions
string CardinalToOrdinal(int cardinal);

#endif // MAPPER_H
