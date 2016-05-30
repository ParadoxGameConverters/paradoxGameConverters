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

#ifndef MODCULTURERULE_H_
#define MODCULTURERULE_H_

#include <string>
#include <vector>
#include <Parsers\IObject.h>

using namespace std;

class Object;

#define DEFAULT_GFX "latingfx"

// Culture Rules
class ModCultureRule
{
	public:
		ModCultureRule(string _key, Object* obj);
		
		string						getKey() const					{ return key; };
		string						getGraphicalCulture() const		{ return graphicalCulture; };
		string						getGraphicalCultureV2() const	{ return graphicalCultureV2; };
		string						getBaseCountryV2() const		{ return baseCountryV2; };
		vector<string>				getMaleNames() const			{ return maleNames; };
		vector<string>				getFemaleNames() const			{ return femaleNames; };
		vector<string>				getLeaderNames() const			{ return leaderNames; };
		vector<string>				getShipNames() const			{ return shipNames; };
		vector<string>				getArmyNames() const			{ return armyNames; };
		vector<string>				getFleetNames() const			{ return fleetNames; };
	private:
		vector<string>						processRawNames(vector<IObject*> nameListObj);

		string						key;
		string						graphicalCulture;
		string						graphicalCultureV2;
		string						baseCountryV2;
		vector<string>				maleNames;
		vector<string>				femaleNames;
		vector<string>				leaderNames;
		vector<string>				shipNames;
		vector<string>				armyNames;
		vector<string>				fleetNames;
};



#endif	// MODCULTURERULE_H_