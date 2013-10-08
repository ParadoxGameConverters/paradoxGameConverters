#ifndef MODCULTURERULE_H_
#define MODCULTURERULE_H_

#include <string>
#include <vector>

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
		vector<string>						processRawNames(vector<Object*> nameListObj);

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