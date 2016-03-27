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



#include "Parsers\Object.h"
#include "GovernmentMapper.h"
#include <map>
#include <vector>
#include <unordered_set>
#include <set>
class V2World;
class HoI3World;



// Province Mappings
typedef map< int, vector<int> >	provinceMapping;			// < destProvince, sourceProvinces >
typedef map< int, vector<int> >	inverseProvinceMapping;	// < sourceProvince, destProvinces >
typedef unordered_set<int>			resettableMap;

void initProvinceMap(Object* obj, provinceMapping& provMap, provinceMapping& inverseProvMap, resettableMap& resettableProvinces);
vector<int> getHoI3ProvinceNums(inverseProvinceMapping invProvMap, int v2ProvinceNum);


typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
} adjacency;			// an entry in the adjacencies.bin format
typedef vector< vector<adjacency> > HoI3AdjacencyMapping;
HoI3AdjacencyMapping initHoI3AdjacencyMap();


typedef map<int, string> continentMapping;	// <province, continent>
void initContinentMap(Object* obj, continentMapping& continentMap);


void mergeNations(V2World&, Object* mergeObj);
void removeEmptyNations(V2World&);
void removeOlderLandlessNations(V2World&, int excess);
void removeLandlessNations(V2World&);

void mergeNations(HoI3World&, Object* mergeObj);
void removeDeadLandlessNations(HoI3World&);
void removeOlderLandlessNations(HoI3World&, int excess);
void removeLandlessNations(HoI3World&);


// State Mappings
typedef map< int, vector<int> >	stateMapping;	// < province, all other provinces in state >
typedef map< int, int >				stateIndexMapping; // < province, state index >
void initStateMap(Object* obj, stateMapping& stateMap, stateIndexMapping& stateIndexMap);


// Union Mappings
typedef vector< pair<string, string> > unionMapping;	// <cultures, tag>
unionMapping initUnionMap(Object* obj);


// Government Mappings
governmentMapping initGovernmentMap(Object* obj);


// Cultural Union Nation mappings
typedef map< string, vector<string> > unionCulturesMap; // <culture group, cultures>
void initUnionCultures(Object* obj, unionCulturesMap& unionCultures);

// Culture Mappings
typedef map<string, string> cultureMapping; // <srcCulture, dstCulture>
cultureMapping initCultureMap(Object* obj);

// idea effects
void initIdeaEffects(Object* obj, map<string, int>& armyInvIdeas, map<string, int>& commerceInvIdeas, map<string, int>& cultureInvIdeas, map<string, int>& industryInvIdeas, map<string, int>& navyInvIdeas, map<string, double>& UHLiberalIdeas, map<string, double>& UHReactionaryIdeas, vector< pair<string, int> >& literacyIdeas, map<string, int>& orderIdeas, map<string, int>& libertyIdeas, map<string, int>& equalityIdeas);


// government jobs
typedef pair<string, vector<string>> governmentJob; // <job name, possible traits>
typedef map<string, vector<string>> governmentJobsMap;
void initGovernmentJobTypes(Object* obj, governmentJobsMap& governmentJobs);


// leaderTraits
typedef map<string, vector<string>> leaderTraitsMap; // <leader type, possible traits>
void initLeaderTraitsMap(Object* obj, leaderTraitsMap& leaderTraits);
typedef map<string, vector<string>> personalityMap;	// <V2 personality, possible HoI3 traits>
void initLeaderPersonalityMap(Object* obj, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap);
typedef map<string, vector<string>> backgroundMap;		// <V2 background, possible HoI3 traits>
void initLeaderBackgroundMap(Object* obj, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);


// names
typedef map<string, pair<vector<string>, vector<string>>> namesMapping;
void initNamesMapping(Object* obj, namesMapping& namesMap);

// portraits
typedef map<string, vector<string>> portraitMapping; // <graphical culture, valid portraits>
void initPortraitMapping(Object* obj, portraitMapping& portraitMap);

// AI focus
typedef enum {
	SEA_FOCUS,
	TANK_FOCUS,
	AIR_FOCUS,
	INF_FOCUS
} AIFocusType;
typedef struct {
	double	modifierAmount;
	string	modifierType;
	string	modifierRequirement;
} AIFocusModifier;
typedef map<AIFocusType, vector<AIFocusModifier>> AIFocusModifiers;
void initAIFocusModifiers(Object* obj, AIFocusModifiers& modifiers);


#endif // MAPPER_H
