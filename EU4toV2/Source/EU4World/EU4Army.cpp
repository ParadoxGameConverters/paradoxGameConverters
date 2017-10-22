/*Copyright(c) 2017 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


#include "EU4Army.h"
#include "Log.h"
#include "ParadoxParserUTF8.h"



EU4Regiment::EU4Regiment(shared_ptr<Object> obj)
{
	std::vector<shared_ptr<Object>> objName = obj->getValue("name");
	(objName.size() > 0) ? name = objName[0]->getLeaf() : name = "";

	std::vector<shared_ptr<Object>> objType = obj->getValue("type");
	if (objType.size() > 0)
	{
		type = objType[0]->getLeaf();
	}
	else
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no type";
		type = "";
	}

	std::vector<shared_ptr<Object>> objHome = obj->getValue("home");
	if (objHome.size() > 0)
	{
		home = atoi(objHome[0]->getLeaf().c_str());
	}
	else
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no home";
		home = -1;
	}

	std::vector<shared_ptr<Object>> objStr = obj->getValue("strength");
	(objStr.size() > 0) ? strength = atof(objStr[0]->getLeaf().c_str()) : strength = 0.0;

	category			= num_reg_categories;
	type_strength	= 0;
}


EU4Army::EU4Army(shared_ptr<Object> obj)
{
	std::vector<shared_ptr<Object>> objName = obj->getValue("name");
	(objName.size() > 0) ? name = objName[0]->getLeaf() : name = "";

	std::vector<shared_ptr<Object>> objLoc = obj->getValue("location");
	if (objLoc.size() > 0)
	{
		location = atoi(objLoc[0]->getLeaf().c_str());
	}
	else
	{
		LOG(LogLevel::Warning) << "Army or Navy " << name << " has no location";
		location = -1;
	}

	std::vector<shared_ptr<Object>> objAtSea = obj->getValue("at_sea");
	(objAtSea.size() > 0) ? at_sea = atoi(objAtSea[0]->getLeaf().c_str()) : at_sea = 0;

	regiments.clear();
	std::vector<shared_ptr<Object>> objRegs = obj->getValue("regiment");
	for (vector<shared_ptr<Object>>::iterator itr = objRegs.begin(); itr != objRegs.end(); ++itr)
	{
		EU4Regiment* reg = new EU4Regiment(*itr);
		regiments.push_back(reg);
	}
	std::vector<shared_ptr<Object>> objShips = obj->getValue("ship");
	for (vector<shared_ptr<Object>>::iterator itr = objShips.begin(); itr != objShips.end(); ++itr)
	{
		EU4Regiment* reg = new EU4Regiment(*itr);
		regiments.push_back(reg);
	}

	blocked_homes.clear();

	std::vector<shared_ptr<Object>> objLeader = obj->getValue("leader");
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


void EU4Army::resolveRegimentTypes(const RegimentTypeMap& regimentTypeMap)
{
	for (vector<EU4Regiment*>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		RegimentTypeMap::const_iterator ritr = regimentTypeMap.find( (*itr)->getType() );
		if (ritr != regimentTypeMap.end())
		{
			(*itr)->setCategory(ritr->second.first);
			(*itr)->setTypeStrength(ritr->second.second);
		}
		else
		{
			LOG(LogLevel::Warning) << "Unknown unit type " << (*itr)->getType() << " for regiment \"" << (*itr)->getName() << "\"";
		}
	}
}


double EU4Army::getAverageStrength(RegimentCategory category) const
{
	int count = 0;			// the total number of regiments
	double total = 0.0;	// the total strength
	for (vector<EU4Regiment*>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			total += (*itr)->getStrength();
			++count;
		}
	}
	return (total / count);
}


int EU4Army::getTotalTypeStrength(RegimentCategory category) const
{
	int total = 0;	//the total strength
	for (vector<EU4Regiment*>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			total += (*itr)->getTypeStrength();
		}
	}
	return total;
}


int EU4Army::getProbabilisticHomeProvince(RegimentCategory category) const
{
	vector<int> homeProvinces;	// the possible home provinces
	for (vector<EU4Regiment*>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		if ( (*itr)->getCategory() == category )
		{
			const int home = (*itr)->getHome();	// the home of this regiment
			bool blocked = false;					// whether or not this home is blocked
			for (vector<int>::const_iterator bitr = blocked_homes.begin(); bitr != blocked_homes.end(); ++bitr)
			{
				if (home == *bitr)
				{
					blocked = true;
					break;
				}
			}
			if (!blocked)
			{
				homeProvinces.push_back(home);
			}
		}
	}
	if (homeProvinces.size() == 0)
	{
		return -1;
	}

	return homeProvinces[int(homeProvinces.size() * ((double)rand() / RAND_MAX))];
}


void EU4Army::blockHomeProvince(const int home)
{
	blocked_homes.push_back(home);
}


void AddCategoryToRegimentTypeMap(shared_ptr<Object> obj, RegimentCategory category, string categoryName, RegimentTypeMap& rtm)
{
	vector<shared_ptr<Object>> top = obj->getValue(categoryName);	// the regiment type mapping category
	if (top.size() != 1)
	{
		LOG(LogLevel::Error) << "Could not get regiment type map for " << categoryName;
		exit(1);
	}
	vector<shared_ptr<Object>> types = top[0]->getLeaves();	// the mappings themselves
	if (types.size() == 0)
	{
		LOG(LogLevel::Error) << "No regiment types to map for " << categoryName;
		exit(1);
	}
	for (vector<shared_ptr<Object>>::iterator itr = types.begin(); itr != types.end(); ++itr)
	{
		string type = (*itr)->getKey();			// the regiment category
		string strength = (*itr)->getLeaf();	// the regiment cost
		rtm[type] = pair<RegimentCategory, int>(category, atoi(strength.c_str()));
	}
}


void AddUnitFileToRegimentTypeMap(string directory, string filename, RegimentTypeMap& rtm)
{
	shared_ptr<Object> obj = parser_UTF8::doParseFile((directory + "/" + filename).c_str());	// the parsed regiment costs file
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << directory << '/' << filename;
		exit(-1);
	}

	int period = filename.find_last_of('.');
	string name = filename.substr(0, period);

	int rc = -1;	// the regiment cost
	vector<shared_ptr<Object>> typeObj = obj->getValue("type");	// the unit type as an object
	if (typeObj.size() < 1)
	{
		LOG(LogLevel::Warning) << "Unit file for " << name << " has no type";
		return;
	}
	string type = typeObj[0]->getLeaf();	// the unit type as a string
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

	int unitStrength = 0;	// the unit strenth
	vector<shared_ptr<Object>> strObj;
	strObj = obj->getValue("maneuver");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("offensive_morale");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("defensive_morale");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("offensive_fire");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("defensive_fire");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("offensive_shock");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("defensive_shock");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());
	strObj = obj->getValue("hull_size");
	if (strObj.size() > 0)	unitStrength += atoi(strObj[0]->getLeaf().c_str());

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