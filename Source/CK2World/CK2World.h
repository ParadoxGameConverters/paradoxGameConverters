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



#ifndef CK2WORLD
#define CK2WORLD


#include <vector>
#include <map>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;



class Object;
class CK2BuildingFactory;
class CK2Version;
class CK2Title;
class CK2Province;
class CK2Barony;
class CK2Dynasty;
class CK2Character;
class CK2Trait;
class CK2War;
class CK2Version;



class CK2World
{
	public:
		CK2World();
		void							init(Object*, const cultureGroupMapping& cultureGroupMap);
		void							addBuildingTypes(Object*);
		void							addDynasties(Object*);
		void							addTraits(Object*);
		void							addPotentialTitles(Object*);
		void							mergeTitles();

		CK2Version*					getVersion()				const	{ return version; };
		date							getEndDate()				const { return endDate; };
		map<string, CK2Title*>	getIndependentTitles()	const { return independentTitles; };
		map<string, CK2Title*>	getAllTitles()				const { return titles; };
		CK2Title*					getHRETitle()				const { return hreTitle; };
		map<int, CK2Province*>	getProvinces()				const { return provinces; };
		map<string, CK2Title*>	getHREMembers()			const { return hreMembers; };

		vector<double>				getAverageTechLevels(CK2Version& version) const;
	private:
		CK2BuildingFactory*		buildingFactory;

		CK2Version*					version;
		date							endDate;
		map<string, CK2Title*>	independentTitles;
		map<string, CK2Title*>	hreMembers;
		map<int, CK2Dynasty*>	dynasties;
		map<int, CK2Character*>	characters;
		map<int, CK2Trait*>		traits;
		map<string, CK2Title*>	potentialTitles;
		map<string, CK2Title*>	titles;
		CK2Title*					hreTitle;
		map<int, CK2Province*>	provinces;
		map<string, CK2Barony*>	baronies;
		vector<CK2War*>			wars;
};


#endif // CK2WORLD