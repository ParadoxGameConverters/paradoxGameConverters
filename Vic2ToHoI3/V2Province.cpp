#include "V2Province.h"
#include "Log.h"


void V2Province::init(Object* obj)
{
	num = atoi(obj->getKey().c_str());

	vector<Object*> ownerObjs;
	ownerObjs = obj->getValue("owner");
	if (ownerObjs.size() == 0)
	{
		owner = "";
	}
	else
	{
		owner = ownerObjs[0]->getLeaf();
	}

	vector<Object*> coreObjs;
	coreObjs = obj->getValue("core");
	for (unsigned int i = 0; i < coreObjs.size(); i++)
	{
		cores.push_back(coreObjs[i]->getLeaf());
	}

	vector<Object*> buildingObjs;
	fortLevel = 0;
	buildingObjs = obj->getValue("fort");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			fortLevel = atoi(tokens[0].c_str());
		}
	}
	navalBaseLevel = 0;
	buildingObjs = obj->getValue("naval_base");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			navalBaseLevel = atoi(tokens[0].c_str());
		}
	}
	infrastructureLevel = 0;
	buildingObjs = obj->getValue("railroad");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			infrastructureLevel = atoi(tokens[0].c_str());
		}
	}
	
	// read pops
	vector<Object*> children = obj->getLeaves();
	for (vector<Object*>::iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		string key = (*itr)->getKey();
		if (key == "aristocrats" || key == "artisans" || key == "bureaucrats" || key == "capitalists" || key == "clergymen" 
			|| key == "craftsmen" || key == "clerks" || key == "farmers" || key == "soldiers" || key == "officers"
			|| key == "labourers" || key == "slaves")
		{
			V2Pop pop;
			pop.init(*itr);
			pops.push_back(pop);
		}
	}
}


int V2Province::getNum()
{
	return num;
}


void V2Province::setOwner(string newOwner)
{
	owner = newOwner;
}


string V2Province::getOwner()
{
	return owner;
}


vector<string> V2Province::getCores()
{
	return cores;
}


void V2Province::addCore(string tag)
{
	cores.push_back(tag);
}


void V2Province::removeCore(string tag)
{
	for (vector<string>::iterator i = cores.begin(); i != cores.end(); i++)
	{
		if (*i == tag)
		{
			cores.erase(i);
			if (cores.size() == 0)
			{
				break;
			}
			i = cores.begin();
		}
	}
}


int V2Province::getPopulation(string type) const
{
	int totalPop = 0;
	for (vector<V2Pop>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		// empty string for type gets total population
		if (type == "" || type == itr->getType())
		{
			totalPop += itr->getSize();
		}
	}
	return totalPop;
}

bool V2Province::containsPop(int id) const
{
	for (vector<V2Pop>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		if (itr->getId() == id)
			return true;
	}
	return false;
}
