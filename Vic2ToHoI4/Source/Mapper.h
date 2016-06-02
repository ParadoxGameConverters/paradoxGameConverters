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
#include "GovernmentMapper.h"
#include <map>
#include <vector>
#include <unordered_set>
#include <set>
class V2World;
class HoI4World;



// Province Mappings
typedef map< int, vector<int> >	provinceMapping;			// < destProvince, sourceProvinces >
typedef map< int, vector<int> >	inverseProvinceMapping;	// < sourceProvince, destProvinces >
typedef unordered_set<int>			resettableMap;

void initProvinceMap(Object* obj, provinceMapping& provMap, provinceMapping& inverseProvMap, resettableMap& resettableProvinces);
vector<int> getHoI4ProvinceNums(inverseProvinceMapping invProvMap, int v2ProvinceNum);


typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
} adjacency;			// an entry in the adjacencies.bin format
typedef vector< vector<adjacency> > HoI4AdjacencyMapping;
HoI4AdjacencyMapping initHoI4AdjacencyMap();


typedef map<int, wstring> continentMapping;	// <province, continent>
void initContinentMap(Object* obj, continentMapping& continentMap);


void mergeNations(V2World&, Object* mergeObj);
void removeEmptyNations(V2World&);
void removeOlderLandlessNations(V2World&, int excess);
void removeLandlessNations(V2World&);

void mergeNations(HoI4World&, Object* mergeObj);
void removeDeadLandlessNations(HoI4World&);
void removeOlderLandlessNations(HoI4World&, int excess);
void removeLandlessNations(HoI4World&);


// State Mappings
typedef map< int, vector<int> >	stateMapping;	// < province, all other provinces in state >
typedef map< int, int >				stateIndexMapping; // < province, state index >
void initStateMap(Object* obj, stateMapping& stateMap, stateIndexMapping& stateIndexMap);


// Union Mappings
typedef vector< pair<wstring, wstring> > unionMapping;	// <cultures, tag>
unionMapping initUnionMap(Object* obj);


// Cultural Union Nation mappings
typedef map< wstring, vector<wstring> > unionCulturesMap; // <culture group, cultures>
void initUnionCultures(Object* obj, unionCulturesMap& unionCultures);

// Culture Mappings
typedef map<wstring, wstring> cultureMapping; // <srcCulture, dstCulture>
cultureMapping initCultureMap(Object* obj);

// idea effects
void initIdeaEffects(Object* obj, map<wstring, int>& armyInvIdeas, map<wstring, int>& commerceInvIdeas, map<wstring, int>& cultureInvIdeas, map<wstring, int>& industryInvIdeas, map<wstring, int>& navyInvIdeas, map<wstring, double>& UHLiberalIdeas, map<wstring, double>& UHReactionaryIdeas, vector< pair<wstring, int> >& literacyIdeas, map<wstring, int>& orderIdeas, map<wstring, int>& libertyIdeas, map<wstring, int>& equalityIdeas);


// government jobs
typedef pair<wstring, vector<wstring>> governmentJob; // <job name, possible traits>
typedef map<wstring, vector<wstring>> governmentJobsMap;
void initGovernmentJobTypes(Object* obj, governmentJobsMap& governmentJobs);


// leaderTraits
typedef map<wstring, vector<wstring>> leaderTraitsMap; // <leader type, possible traits>
void initLeaderTraitsMap(Object* obj, leaderTraitsMap& leaderTraits);
typedef map<wstring, vector<wstring>> personalityMap;	// <V2 personality, possible HoI4 traits>
void initLeaderPersonalityMap(Object* obj, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap);
typedef map<wstring, vector<wstring>> backgroundMap;		// <V2 background, possible HoI4 traits>
void initLeaderBackgroundMap(Object* obj, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);


// names
typedef map<wstring, pair<vector<wstring>, vector<wstring>>> namesMapping;
void initNamesMapping(Object* obj, namesMapping& namesMap);

// portraits
typedef map<wstring, vector<wstring>> portraitMapping; // <graphical culture, valid portraits>
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
	wstring	modifierType;
	wstring	modifierRequirement;
} AIFocusModifier;
typedef map<AIFocusType, vector<AIFocusModifier>> AIFocusModifiers;
void initAIFocusModifiers(Object* obj, AIFocusModifiers& modifiers);


#endif // MAPPER_H
