/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "Parsers\Object.h"
#include "EU4World\EU4Version.h"
#include <map>
#include <vector>
#include <unordered_set>
class EU4World;
class V2World;



// Province Mappings
typedef map< int, vector<int> >	provinceMapping;			// < destProvince, sourceProvinces >
typedef map< int, vector<int> >	inverseProvinceMapping;	// < sourceProvince, destProvinces >
typedef unordered_set<int>			resettableMap;

void initProvinceMap(Object* obj, const EU4Version* version, provinceMapping& provMap, provinceMapping& inverseProvMap, resettableMap& resettableProvinces);
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, int eu4ProvinceNum);


typedef struct {
	int type;
	int to;
	int via;
	int unknown1;
	int unknown2;
	int pathX;
	int pathY;
	int unknown3;
	int unknown4;
} adjacency;
typedef vector< vector<adjacency> > adjacencyMapping;
adjacencyMapping initAdjacencyMap();


typedef map<int, string>	continentMapping;	// <province, continent>
void initContinentMap(Object* obj, continentMapping& continentMap);


void mergeNations(EU4World&, Object* mergeObj);
void uniteJapan(EU4World&);
void removeEmptyNations(EU4World&);
void removeDeadLandlessNations(EU4World&);
void removeOlderLandlessNations(EU4World&, int excess);
void removeLandlessNations(EU4World&);


// State Mappings
typedef map< int, vector<int> >	stateMapping;	// < province, all other provinces in state >
typedef map< int, int >				stateIndexMapping; // < province, state index >
void initStateMap(Object* obj, stateMapping& stateMap, stateIndexMapping& stateIndexMap);


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
typedef map< string, vector<string> > unionCulturesMap; // <culture group, cultures>
void initUnionCultures(Object* obj, unionCulturesMap& unionCultures);



#endif // MAPPER_H
