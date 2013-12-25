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



#include "CK2Army.h"
#include "..\log.h"



CK2Subunit::CK2Subunit(const Object* obj)
{
	home = atoi( obj->getLeaf("home").c_str() );


	currentInfantryPSE	= 0.0F;
	maxInfantryPSE			= 0.0F;
	currentCavalryPSE		= 0.0F;
	maxCavalryPSE			= 0.0F;
	currentShips			= 0.0F;
	maxShips					= 0.0F;

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
		else if (troopsObjs[i]->getKey() == "galleys")
		{
			vector<string> tokens = troopsObjs[i]->getTokens();
			currentShips			+= atoi(tokens[0].c_str());
			maxShips					+= atoi(tokens[1].c_str());
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

	transportedArmies.clear();
	vector<Object*> armyObjs = obj->getValue("army");
	for (unsigned int i = 0; i < armyObjs.size(); i++)
	{
		CK2Army* newArmy = new CK2Army(armyObjs[i]);
		transportedArmies.push_back(newArmy);
	}

	currentInfantryPSE	= 0.0F;
	maxInfantryPSE			= 0.0F;
	currentCavalryPSE		= 0.0F;
	maxCavalryPSE			= 0.0F;
	currentShips			= 0.0F;
	maxShips					= 0.0F;
	for (unsigned int i = 0; i < subunits.size(); i++)
	{
		currentInfantryPSE	+= subunits[i]->getCurrentInfantryPSE();
		maxInfantryPSE			+= subunits[i]->getMaxInfantryPSE();
		currentCavalryPSE		+= subunits[i]->getCurrentCavalryPSE();
		maxCavalryPSE			+= subunits[i]->getMaxCavalryPSE();
		currentShips			+= subunits[i]->getCurrentShips();
		maxShips					+= subunits[i]->getMaxShips();
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