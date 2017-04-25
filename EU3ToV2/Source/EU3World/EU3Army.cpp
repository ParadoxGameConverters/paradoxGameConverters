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


#include "EU3Army.h"
#include "../Log.h"
#include "../Parsers/Parser.h"



EU3Regiment::EU3Regiment(Object *obj)
{
	std::vector<Object*> objName = obj->getValue("name");
	if (objName.size() > 0)
	{
		name = objName[0]->getLeaf();
	}
	else
	{
		name = "";
	}

	std::vector<Object*> objType = obj->getValue("type");
	if (objType.size() > 0)
	{
		type = objType[0]->getLeaf();
	}
	else
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no type";
		type = "";
	}

	std::vector<Object*> objHome = obj->getValue("home");
	if (objHome.size() > 0)
	{
		home = atoi(objHome[0]->getLeaf().c_str());
	}
	else
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no home";
		home = -1;
	}

	std::vector<Object*> objStr = obj->getValue("strength");
	if (objStr.size() > 0)
	{
		strength = atof(objStr[0]->getLeaf().c_str());
	}
	else
	{
		strength = 0.0;
	}

	category			= num_reg_categories;
	type_strength	= 0;
}


EU3Army::EU3Army(Object *obj)
{
	std::vector<Object*> objName = obj->getValue("name");
	if (objName.size() > 0)
	{
		name = objName[0]->getLeaf();
	}
	else
	{
		name = "";
	}

	std::vector<Object*> objLoc = obj->getValue("location");
	if (objLoc.size() > 0)
	{
		location = atoi(objLoc[0]->getLeaf().c_str());
	}
	else
	{
		LOG(LogLevel::Warning) << "Army or Navy " << name << " has no location";
		location = -1;
	}

	std::vector<Object*> objAtSea = obj->getValue("at_sea");
	if (objAtSea.size() > 0)
	{
		at_sea = atoi(objAtSea[0]->getLeaf().c_str());
	}
	else
	{
		at_sea = 0;
	}

	regiments.clear();
	std::vector<Object*> objRegs = obj->getValue("regiment");
	for (vector<Object*>::iterator itr = objRegs.begin(); itr != objRegs.end(); ++itr)
	{
		EU3Regiment* reg = new EU3Regiment(*itr);
		regiments.push_back(reg);
	}
	std::vector<Object*> objShips = obj->getValue("ship");
	for (vector<Object*>::iterator itr = objShips.begin(); itr != objShips.end(); ++itr)
	{
		EU3Regiment* reg = new EU3Regiment(*itr);
		regiments.push_back(reg);
	}

	blocked_homes.clear();

	std::vector<Object*> objLeader = obj->getValue("leader");
	if (objLeader.size() > 0)
	{
		objLeader = objLeader[0]->getValue("id");
		leaderID = atoi(objLeader[0]->getLeaf().c_str());
	}
	else
	{
		leaderID = 0;
	}
}


void EU3Army::resolveRegimentTypes(const RegimentTypeMap& regimentTypeMap)
{
	for (vector<EU3Regiment*>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		RegimentTypeMap::const_iterator ritr = regimentTypeMap.find( (*itr)->getType() );
		if (ritr != regimentTypeMap.end())
		{
			(*itr)->setCategory(ritr->second.first);
			(*itr)->setTypeStrength(ritr->second.second);
		}
		else
		{
			LOG(LogLevel::Warning) << "Unknown unit type " << (*itr)->getType() << " for regiment \"" << (*itr)->getName() << "\"\n";
		}
	}
}


double EU3Army::getAverageStrength(RegimentCategory category) const
{
	int count = 0;
	double total = 0.0;
	for (vector<EU3Regiment*>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			total += (*itr)->getStrength();
			++count;
		}
	}
	return (total / count);
}


int EU3Army::getTotalTypeStrength(RegimentCategory category) const
{
	int total = 0;
	for (vector<EU3Regiment*>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			total += (*itr)->getTypeStrength();
		}
	}
	return total;
}


int EU3Army::getProbabilisticHomeProvince(RegimentCategory category) const
{
	vector<int> homeProvinces;
	for (vector<EU3Regiment*>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			int home = (*itr)->getHome();
			bool blocked = false;
			for (vector<int>::const_iterator bitr = blocked_homes.begin(); bitr != blocked_homes.end(); ++bitr)
			{
				if (home == *bitr)
				{
					blocked = true;
					break;
				}
			}
			if (!blocked)
				homeProvinces.push_back(home);
		}
	}
	if (homeProvinces.size() == 0)
		return -1;

	return homeProvinces[int(homeProvinces.size() * ((double)rand() / RAND_MAX))];
}


void EU3Army::blockHomeProvince(int home)
{
	blocked_homes.push_back(home);
}


void AddCategoryToRegimentTypeMap(Object* obj, RegimentCategory category, string categoryName, RegimentTypeMap& rtm)
{
	vector<Object*> top = obj->getValue(categoryName);
	if (top.size() != 1)
	{
		LOG(LogLevel::Error) << "Could not get regiment type map for " << categoryName;
		exit(1);
	}
	vector<Object*> types = top[0]->getLeaves();
	if (types.size() == 0)
	{
		LOG(LogLevel::Error) << "No regiment types to map for " << categoryName;
		exit(1);
	}
	for (vector<Object*>::iterator itr = types.begin(); itr != types.end(); ++itr)
	{
		string type = (*itr)->getKey();
		string strength = (*itr)->getLeaf();
		rtm[type] = pair<RegimentCategory, int>(category, atoi(strength.c_str()));
	}
}


void AddUnitFileToRegimentTypeMap(string directory, string name, RegimentTypeMap& rtm)
{
	Object* obj = doParseFile((directory + "\\" + name + ".txt").c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << directory << '\\' << name << ".txt";
		exit(-1);
	}

	int rc = -1;
	vector<Object*> typeObj = obj->getValue("type");
	if (typeObj.size() < 1)
	{
		LOG(LogLevel::Warning) << "Unit file for " << name << " has no type";
		return;
	}
	string type = typeObj[0]->getLeaf();
	for (int i = 0; i < num_reg_categories; ++i)
	{
		if (type == RegimentCategoryNames[i])
			rc = i;
	}
	if (rc == -1)
	{
		LOG(LogLevel::Warning) << "Unit file for " << name << " has unrecognized type " << type;
		return;
	}

	int unitStrength = 0;
	vector<Object*> strObj;
	strObj = obj->getValue("maneuver");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("offensive_morale");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("defensive_morale");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("offensive_fire");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("defensive_fire");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("offensive_shock");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("defensive_shock");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("hull_size");
	if (strObj.size() > 0)
		unitStrength += atoi(strObj[0]->getLeaf().c_str());

	// give all transports equal weight for 1-to-1 conversion
	if (rc == transport)
	{
		unitStrength = 24;
	}

	if (unitStrength == 0)
	{
		LOG(LogLevel::Warning) << "Unit " << name << " has no strength";
		return;
	}

	rtm[name] = pair<RegimentCategory, int>((RegimentCategory)rc, unitStrength);
}