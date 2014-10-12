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
#include "EU3World\EU3World.h"
#include <map>
#include <vector>
#include <unordered_set>
class V2World;



// Province Mappings
typedef map< int, vector<int> >	provinceMapping;			// < destProvince, sourceProvinces >
typedef map< int, vector<int> >	inverseProvinceMapping;	// < sourceProvince, destProvinces >
typedef unordered_set<int>			resettableMap;

void initProvinceMap(Object* obj, WorldType worldType, provinceMapping& provinceMap, inverseProvinceMapping& inverseProvinceMap, resettableMap& resettableProvinces);
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, int eu3ProvinceNum);


typedef vector< vector<int> > adjacencyMapping;
adjacencyMapping initAdjacencyMap();


typedef map<int, string>	continentMapping;	// <province, continent>
void initContinentMap(Object* obj, continentMapping& continentMap);


void mergeNations(EU3World&, Object* mergeObj);
void uniteJapan(EU3World&);
void removeEmptyNations(EU3World&);
void removeDeadLandlessNations(EU3World&);
void removeOlderLandlessNations(EU3World&, int excess);
void removeLandlessNations(EU3World&);


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
typedef map< string, vector<string> > unionCulturesMap; // <tag, cultures>
void initUnionCultures(Object* obj, unionCulturesMap& unionCultures);



#endif // MAPPER_H
