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
		if (type[0] == '\"')
			type = type.substr(1, type.length() - 2);
	}
	else
	{
		log("Error: Regiment or Ship \"%s\" has no type.", name.c_str());
	}

	std::vector<Object*> objHome = obj->getValue("home");
	if (objHome.size() > 0)
	{
		home = atoi(objHome[0]->getLeaf().c_str());
	}
	else
	{
		log("Error: Regiment or Ship \"%s\" has no home.", name.c_str());
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
		name = "\"\"";
	}

	std::vector<Object*> objLoc = obj->getValue("location");
	if (objLoc.size() > 0)
	{
		location = atoi(objLoc[0]->getLeaf().c_str());
	}
	else
	{
		log("Error: Army or Navy \"%s\" has no location.", name.c_str());
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
			log("Unknown unit type %s for regiment \"%s\"", itr->getType(), itr->getName());
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
			homeProvinces.push_back(itr->getHome());
		}
	}
	return homeProvinces[int(homeProvinces.size() * ((double)rand() / RAND_MAX))];
}