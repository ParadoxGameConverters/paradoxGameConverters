#include "ModCultureRule.h"
#include <boost/tokenizer.hpp>
#include <algorithm>
#include <string>
#include <sstream>
#include "..\Log.h"
#include "..\Parsers\Object.h"

using namespace boost;

#define GROUPING_SYMBOL '*'
vector<string> ModCultureRule::processRawNames(vector<Object*> nameListObj)
{
	vector<string> nameList;
	string rawNames, name, temp;
	stringstream ss;
	typedef tokenizer<char_separator<char> > 
		tokenizer;
	char_separator<char> sep(" ");
	char first,last;
	bool group = false;
	if (nameListObj.size() > 0)
	{
		rawNames	= nameListObj[0]->getLeaf();
		tokenizer tokens(rawNames, sep);
		for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
		{
			name=tok_iter->c_str();
			first = name.front();
			last = name.back();
			//printf("<%c%c> ",first,last);
			if( (first==GROUPING_SYMBOL) && (last==GROUPING_SYMBOL) ) // "name"
			{
				//printf("Case <**>\n");
				name.erase(remove(name.begin(), name.end(), GROUPING_SYMBOL), name.end()); // strip grouping symbol
				nameList.push_back(name);
			}
			else if( (first==GROUPING_SYMBOL) && (last!=GROUPING_SYMBOL) ) // "First...
			{
				//printf("Case <* >\n");
				ss << name;
				group = true;
			}
			else if(last==GROUPING_SYMBOL) // ...third"
			{
				//printf("Case < *>\n");
				ss << " " << name;
				temp = ss.str();
				ss.str(string());
				temp.erase(remove(temp.begin(), temp.end(), GROUPING_SYMBOL), temp.end());
				nameList.push_back(temp);
				//printf("<%s>\n",temp.c_str());
				temp = "";
				group = false;
			}
			else if(group==true) // ...second...
			{
				//printf("Case *< >*\n");
				ss << " " << name;
			}
			else // name
			{
				//printf("Case < >\n");
				nameList.push_back(name);
			}
			//printf("%s ",nameList.back().c_str());

		}
	}
	return nameList;
}

ModCultureRule::ModCultureRule(string _key, Object* obj)
{
	key			= _key;

	#define DEFAULT_GFX "latingfx"
	vector<Object*> graphicalCultureObj = obj->getValue("graphical_culture");
	if (graphicalCultureObj.size() > 0)
	{
		graphicalCulture	= graphicalCultureObj[0]->getLeaf();
	}
	else
	{
		graphicalCulture	= DEFAULT_GFX;
	}

	#define DEFAULT_GC "Generic"
	vector<Object*> graphicalCultureV2Obj = obj->getValue("graphical_culture_V2");
	if (graphicalCultureV2Obj.size() > 0)
	{
		graphicalCultureV2	= graphicalCultureV2Obj[0]->getLeaf();
	}
	else
	{
		graphicalCultureV2	= DEFAULT_GC;
	}

	#define DEFAULT_BASE_V2 "CAN"
	vector<Object*> baseCountryV2Obj = obj->getValue("base_country_V2");
	if (graphicalCultureV2Obj.size() > 0)
	{
		baseCountryV2	= baseCountryV2Obj[0]->getLeaf();
	}
	else
	{
		baseCountryV2	= DEFAULT_BASE_V2;
	}

	maleNames.clear();
	vector<Object*> maleNameObj = obj->getValue("male_names");
	if (maleNameObj.size() > 0)
	{
		maleNames = processRawNames(maleNameObj);
	}
	

	femaleNames.clear();
	vector<Object*> femaleNameObj = obj->getValue("female_names");
	if (femaleNameObj.size() > 0)
	{
		femaleNames = processRawNames(femaleNameObj);
	}

	leaderNames.clear();
	vector<Object*> leaderNameObj = obj->getValue("leader_names");
	if (leaderNameObj.size() > 0)
	{
		leaderNames = processRawNames(leaderNameObj);
	}

	shipNames.clear();
	vector<Object*> shipNameObj = obj->getValue("ship_names");
	if (shipNameObj.size() > 0)
	{
		shipNames = processRawNames(shipNameObj);
	}

	armyNames.clear();
	vector<Object*> armyNameObj = obj->getValue("army_names");
	if (armyNameObj.size() > 0)
	{
		armyNames = processRawNames(armyNameObj);
	}

	fleetNames.clear();
	vector<Object*> fleetNameObj = obj->getValue("fleet_names");
	if (fleetNameObj.size() > 0)
	{
		fleetNames = processRawNames(fleetNameObj);
	}
	
}

