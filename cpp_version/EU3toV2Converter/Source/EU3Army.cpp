#include "EU3Army.h"
#include "Log.h"
#include "Parsers/Parser.h"



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
		log("Error: Regiment or Ship \"%s\" has no type.\n", name.c_str());
		type = "";
	}

	std::vector<Object*> objHome = obj->getValue("home");
	if (objHome.size() > 0)
	{
		home = atoi(objHome[0]->getLeaf().c_str());
	}
	else
	{
		log("Error: Regiment or Ship \"%s\" has no home.\n", name.c_str());
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
		log("Error: Army or Navy \"%s\" has no location.\n", name.c_str());
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
			log("Unknown unit type %s for regiment \"%s\"\n", (*itr)->getType().c_str(), (*itr)->getName().c_str());
		}
	}
}


double EU3Army::getAverageStrength(RegimentCategory category)
{
	int count = 0;
	double total = 0.0;
	for (vector<EU3Regiment*>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			total += (*itr)->getStrength();
			++count;
		}
	}
	return (total / count);
}


int EU3Army::getTotalTypeStrength(RegimentCategory category)
{
	int total = 0;
	for (vector<EU3Regiment*>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			total += (*itr)->getTypeStrength();
		}
	}
	return total;
}


int EU3Army::getProbabilisticHomeProvince(RegimentCategory category)
{
	vector<int> homeProvinces;
	for (vector<EU3Regiment*>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			int home = (*itr)->getHome();
			bool blocked = false;
			for (vector<int>::iterator bitr = blocked_homes.begin(); bitr != blocked_homes.end(); ++bitr)
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
		log("Error: could not get regiment type map for %s", categoryName);
		printf("Error: could not get regiment type map for %s", categoryName);
		exit(1);
	}
	vector<Object*> types = top[0]->getLeaves();
	if (types.size() == 0)
	{
		log("Error: no regiment types to map for %s", categoryName);
		printf("Error: no regiment types to map for %s", categoryName);
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

	int rc = -1;
	vector<Object*> typeObj = obj->getValue("type");
	if (typeObj.size() < 1)
	{
		log("Error: unit file for %s has no type!\n", name.c_str());
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
		log("Error: unit file for %s has unrecognized type %s!\n", name.c_str(), type.c_str());
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
		log("Error: unit %s has no strength!\n", name.c_str());
		return;
	}

	rtm[name] = pair<RegimentCategory, int>((RegimentCategory)rc, unitStrength);
}