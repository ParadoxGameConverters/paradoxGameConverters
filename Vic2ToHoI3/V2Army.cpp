#include "V2Army.h"
#include "Log.h"


void V2Regiment::init(Object* obj)
{
	string key = obj->getKey();
	isShip = (key == "ship");

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

	std::vector<Object*> objStr = obj->getValue("strength");
	if (objStr.size() > 0)
	{
		strength = atof(objStr[0]->getLeaf().c_str());
	}
	else
	{
		strength = 0.0;
	}

	objStr = obj->getValue("organisation");
	if (objStr.size() > 0)
	{
		organization = atof(objStr[0]->getLeaf().c_str());
	}
	else
	{
		organization = 0.0;
	}
	
	objStr = obj->getValue("experience");
	if (objStr.size() > 0)
	{
		experience = atof(objStr[0]->getLeaf().c_str());
	}
	else
	{
		experience = 0.0;
	}

/* -- not needed?
	std::vector<Object*> objPop = obj->getValue("pop");
	if (objPop.size() > 0)
	{
		objPop = objPop[0]->getValue("id");
		popID = atoi(objPop[0]->getLeaf().c_str());
	}
	else
	{
		popID = 0;
	}
*/
}


void V2Army::init(Object* obj)
{
	string key = obj->getKey();
	isNavy = (key == "navy");

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

	std::vector<Object*> objSupp = obj->getValue("supplies");
	if (objSupp.size() > 0)
	{
		supplies = atof(objSupp[0]->getLeaf().c_str());
	}
	else
	{
		supplies = 0.0;
	}

	std::vector<Object*> objRegs = obj->getValue("regiment");
	for (vector<Object*>::iterator itr = objRegs.begin(); itr != objRegs.end(); ++itr)
	{
		V2Regiment reg;
		reg.init(*itr);
		regiments.push_back(reg);
	}

	std::vector<Object*> objShips = obj->getValue("ship");
	for (vector<Object*>::iterator itr = objShips.begin(); itr != objShips.end(); ++itr)
	{
		V2Regiment reg;
		reg.init(*itr);
		regiments.push_back(reg);
	}
}