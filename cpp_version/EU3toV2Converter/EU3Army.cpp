#include "EU3Army.h"
#include "Log.h"


void EU3Regiment::init(Object *obj)
{
	std::vector<Object*> objName = obj->getValue("name");
	if (objName.size() > 0)
	{
		name = objName[0]->getLeaf();
	}

	std::vector<Object*> objType = obj->getValue("type");
	if (objType.size() > 0)
	{
		type = objType[0]->getLeaf();
	}
	else
	{
		log("Error: Regiment or Ship \"%s\" has no type.\n", name.c_str());
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
}

void EU3Army::init(Object *obj)
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

	std::vector<Object*> objRegs = obj->getValue("regiment");
	for (vector<Object*>::iterator itr = objRegs.begin(); itr != objRegs.end(); ++itr)
	{
		EU3Regiment reg;
		reg.init(*itr);
		regiments.push_back(reg);
	}

	std::vector<Object*> objShips = obj->getValue("ship");
	for (vector<Object*>::iterator itr = objShips.begin(); itr != objShips.end(); ++itr)
	{
		EU3Regiment reg;
		reg.init(*itr);
		regiments.push_back(reg);
	}
}

void EU3Army::resolveRegimentTypes(const RegimentTypeMap& regimentTypeMap)
{
	for (vector<EU3Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		RegimentTypeMap::const_iterator ritr = regimentTypeMap.find(itr->getType());
		if (ritr != regimentTypeMap.end())
		{
			itr->setCategory(ritr->second.first);
			itr->setTypeStrength(ritr->second.second);
		}
		else
		{
			log("Unknown unit type %s for regiment \"%s\"\n", itr->getType().c_str(), itr->getName().c_str());
		}
	}
}

double EU3Army::getAverageStrength(RegimentCategory category)
{
	int count = 0;
	double total = 0.0;
	for (vector<EU3Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if (itr->getCategory() == category)
		{
			total += itr->getStrength();
			++count;
		}
	}
	return (total / count);
}

int EU3Army::getTotalTypeStrength(RegimentCategory category)
{
	int total = 0;
	for (vector<EU3Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if (itr->getCategory() == category)
		{
			total += itr->getTypeStrength();
		}
	}
	return total;
}

int EU3Army::getProbabilisticHomeProvince(RegimentCategory category)
{
	vector<int> homeProvinces;
	for (vector<EU3Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if (itr->getCategory() == category)
		{
			int home = itr->getHome();
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