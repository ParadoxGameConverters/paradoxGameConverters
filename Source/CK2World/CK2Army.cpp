#include "CK2Army.h"
#include "..\log.h"



CK2Subunit::CK2Subunit(const Object* obj)
{
	home = atoi( obj->getLeaf("home").c_str() );


	currentInfantryPSE	= 0.0F;
	maxInfantryPSE			= 0.0F;
	currentCavalryPSE		= 0.0F;
	maxCavalryPSE			= 0.0F;

	vector<Object*> troopsObjs = obj->getValue("troops")[0]->getLeaves();
	for (unsigned int i = 0; i < troopsObjs.size(); i++)
	{
		if (troopsObjs[i]->getKey() == "light_infantry")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentInfantryPSE	+= 0.305 * atoi(tokens[0].c_str());
			maxInfantryPSE			+= 0.305 * atoi(tokens[1].c_str());
		}
		else if (troopsObjs[i]->getKey() == "archers")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentInfantryPSE	+= 0.384 * atoi(tokens[0].c_str());
			maxInfantryPSE			+= 0.384 * atoi(tokens[1].c_str());
		}
		else if (troopsObjs[i]->getKey() == "heavy_infantry")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentInfantryPSE	+= 0.610 * atoi(tokens[0].c_str());
			maxInfantryPSE			+= 0.610 * atoi(tokens[1].c_str());
		}
		else if (troopsObjs[i]->getKey() == "pikemen")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentInfantryPSE	+= 1.00 * atoi(tokens[0].c_str());
			maxInfantryPSE			+= 1.00 * atoi(tokens[1].c_str());
		}
		else if (troopsObjs[i]->getKey() == "light_cavalry")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentCavalryPSE		+= 0.23 * atoi(tokens[0].c_str());
			maxCavalryPSE			+= 0.23 * atoi(tokens[1].c_str());
		}
		else if (troopsObjs[i]->getKey() == "knights")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentCavalryPSE		+= 1.00 * atoi(tokens[0].c_str());
			maxCavalryPSE			+= 1.00 * atoi(tokens[1].c_str());
		}
		else if (troopsObjs[i]->getKey() == "horse_archers")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentCavalryPSE		+= 0.42 * atoi(tokens[0].c_str());
			maxCavalryPSE			+= 0.42 * atoi(tokens[1].c_str());
		}
	}
}


CK2Army::CK2Army(const Object* obj)
{
	name = obj->getLeaf("name");
	vector<Object*> subunitObjs = obj->getValue("sub_unit");
	for (unsigned int i = 0; i < subunitObjs.size(); i++)
	{
		CK2Subunit* newSubunit = new CK2Subunit(subunitObjs[i]);
		subunits.push_back(newSubunit);
	}
	vector<Object*> movementObj = obj->getValue("movement_progress");
	if (movementObj.size() > 0)
	{
		movementProgress = atof( movementObj[0]->getLeaf().c_str() );
	}
	else
	{
		movementProgress = 0.0F;
	}
	
	vector<Object*> pathObj = obj->getValue("path");
	if (pathObj.size() > 0)
	{
		vector<string> pathStrs = pathObj[0]->getTokens();
		for (unsigned int i = 0; i < pathStrs.size(); i++)
		{
			path.push_back( atoi(pathStrs[i].c_str()) );
		}
	}

	location = atoi( obj->getLeaf("location").c_str() );

	currentInfantryPSE	= 0.0F;
	maxInfantryPSE			= 0.0F;
	currentCavalryPSE		= 0.0F;
	maxCavalryPSE			= 0.0F;
	for (unsigned int i = 0; i < subunits.size(); i++)
	{
		currentInfantryPSE	+= subunits[i]->getCurrentInfantryPSE();
		maxInfantryPSE			+= subunits[i]->getMaxInfantryPSE();
		currentCavalryPSE		+= subunits[i]->getCurrentCavalryPSE();
		maxCavalryPSE			+= subunits[i]->getMaxCavalryPSE();
	}
}


vector<int> CK2Army::getHomeProvinces() const
{
	vector<int> homeProvinces;
	for (unsigned int i = 0; i < subunits.size(); i++)
	{
		homeProvinces.push_back(subunits[i]->getHome());
	}

	return homeProvinces;
}