/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
using namespace std;



class V2World;
class HoI4World;



typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
} adjacency;			// an entry in the adjacencies.bin format
typedef vector< vector<adjacency> > HoI4AdjacencyMapping;
HoI4AdjacencyMapping initHoI4AdjacencyMap();


typedef map<int, string> continentMapping;	// <province, continent>
void initContinentMap(shared_ptr<Object> obj, continentMapping& continentMap);


void removeOlderLandlessNations(V2World&, int excess);
void removeLandlessNations(V2World&);

void mergeNations(HoI4World&, shared_ptr<Object> mergeObj);
void removeDeadLandlessNations(HoI4World&);
void removeOlderLandlessNations(HoI4World&, int excess);
void removeLandlessNations(HoI4World&);

// Union Mappings
typedef vector< pair<string, string> > unionMapping;	// <cultures, tag>
unionMapping initUnionMap(shared_ptr<Object> obj);


// Cultural Union Nation mappings
typedef map< string, vector<string> > unionCulturesMap; // <culture group, cultures>
void initUnionCultures(shared_ptr<Object> obj, unionCulturesMap& unionCultures);

// idea effects
void initIdeaEffects(shared_ptr<Object> obj, map<string, int>& armyInvIdeas, map<string, int>& commerceInvIdeas, map<string, int>& cultureInvIdeas, map<string, int>& industryInvIdeas, map<string, int>& navyInvIdeas, map<string, double>& UHLiberalIdeas, map<string, double>& UHReactionaryIdeas, vector< pair<string, int> >& literacyIdeas, map<string, int>& orderIdeas, map<string, int>& libertyIdeas, map<string, int>& equalityIdeas);


// government jobs
typedef pair<string, vector<string>> governmentJob; // <job name, possible traits>
typedef map<string, vector<string>> governmentJobsMap;
void initGovernmentJobTypes(governmentJobsMap& governmentJobs);


// leaderTraits
typedef map<string, vector<string>> leaderTraitsMap; // <leader type, possible traits>
void initLeaderTraitsMap(leaderTraitsMap& leaderTraits);
typedef map<string, vector<string>> personalityMap;	// <V2 personality, possible HoI4 traits>
void initLeaderPersonalityMap(personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap);
typedef map<string, vector<string>> backgroundMap;		// <V2 background, possible HoI4 traits>
void initLeaderBackgroundMap(backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);


// portraits
typedef map<string, vector<string>> portraitMapping; // <graphical culture, valid portraits>
void initPortraitMapping(portraitMapping& portraitMap);



#endif // MAPPER_H
