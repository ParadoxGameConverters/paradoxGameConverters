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



#ifndef CK2BUILDING_H_
#define CK2BUILDING_H_



#include "..\Mappers.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class Object;
class CK2Character;


class CK2Building
{
	public:
		CK2Building(Object* obj);

		double			getFortLevel()						const { return fortLevel; };
		double			getLevyMultiplier()				const { return levyMultiplier; };
		double			getNumSoldiers()					const { return numSoldiers; };
		double			getTaxIncome()						const { return taxIncome; };
		double			getTechBonus()						const { return techBonus; };
		string			getForbiddenReligion()			const { return forbiddenReligion; };
		string			getRequiredReligion()			const { return requiredReligion; };
		vector<string>	getAcceptableCultures()			const { return acceptableCultures; };
		vector<string>	getAcceptableCultureGroups()	const { return acceptableCultureGroups; };

	private:
		double	fortLevel;
		double	levyMultiplier;
		double	numSoldiers;
		double	taxIncome;
		double	techBonus;
		
		string			forbiddenReligion;
		string			requiredReligion;
		vector<string>	acceptableCultures;
		vector<string>	acceptableCultureGroups;
};


class CK2BuildingFactory
{
	public:
		CK2BuildingFactory(const cultureGroupMapping* _cultureGroupMap);
		
		static void						addBuildingTypes(Object* obj);
		const CK2Building*	getBuilding(string type, const CK2Character* baronyHolder) const;

	private:
		const cultureGroupMapping* cultureGroupMap;
		static map<string, const CK2Building*> buildings;
};


#endif // CK2BUILDING_H_