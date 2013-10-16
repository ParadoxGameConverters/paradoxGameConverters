/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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


// Province Mappings
typedef map< int, vector<int> > provinceMapping;			// < destProvince,	sourceProvinces	>
typedef map< int, vector<int> > inverseProvinceMapping;	// < sourceProvince,	destProvinces		>
provinceMapping initProvinceMap(Object* obj, CK2Version* version);
inverseProvinceMapping invertProvinceMap(provinceMapping& provMap);

typedef map<int, string> continentMapping;
continentMapping initContinentMap(Object* obj);

typedef struct {
	int type;
	int to;
	int via;
	int unknown1;
	int pathX;
	int pathY;
} adjacency;
typedef vector< vector<adjacency> > adjacencyMapping;
adjacencyMapping initAdjacencyMap();
	


// Country Mappings
vector<string> processBlockedNations(Object* obj);



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

typedef map<string, string> cultureGroupMapping; // culture, culture group
void addCultureGroupMappings(Object* obj, cultureGroupMapping& map);


// Religion Mappings
typedef map<string, string> religionMapping; // CK2 religion, EU3 religion
religionMapping initReligionMap(Object* obj);

typedef map<string, string> religionGroupMapping; // EU3 religion, EU3 religion group
void addReligionGroupMappings(Object* obj, religionGroupMapping& map);


// Trade Goods
typedef struct {
	int														basePrice;
	vector<  pair<string, double>  >					supplyModifiers;
	vector<  pair< vector<string>, double >  >	demandModifiers;
} tradeGood;
typedef map<string, tradeGood> tradeGoodMapping;
tradeGoodMapping initTradeGoodMapping(Object* obj);


#endif	// MAPPERS_H_