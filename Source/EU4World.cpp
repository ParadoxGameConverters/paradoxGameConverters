#include "EU4World.h"
#include "Log.h"
#include "Configuration.h"
#include "Mapper.h"
#include "Parsers\Object.h"
#include "EU4Province.h"
#include "EU4Country.h"
#include "EU4Diplomacy.h"

EU4World::EU4World(Object* obj)
{
	cachedWorldType = Unknown;

	string key;

	// Get Provinces
	provinces.clear();
	countries.clear();

	vector<Object*> leavesProvinces = obj->getValue("provinces");
	for (unsigned int i = 0; i < leavesProvinces.size(); i++) // loop through all the sections marked provinces
//	if (leavesProvinces.size() > 0)
	{
		vector<Object*> lowerProvinces = obj->getLeaves();
		for (unsigned int j = 0; j < lowerProvinces.size(); j++)
		{
			key = lowerProvinces[j]->getKey();

			// Is this a negative value? If so, must be a province
//			if (key.substr(0, 1) == "-")
			if (atoi(key.c_str()) < 0)
			{
				EU4Province* province = new EU4Province(lowerProvinces[j]);
				provinces.insert(make_pair(province->getNum(), province));
			}
		}
	}

	vector<Object*> leavesCountries = obj->getValue("countries");
	for (unsigned int i = 0; i < leavesCountries.size(); i++) // loop through all the sections marked countries
	{
		vector<Object*> lowerCountries = obj->getLeaves();
		for (unsigned int j = 0; j < lowerCountries.size(); j++)
		{
			key = lowerCountries[j]->getKey();

			// Countries are three uppercase characters
			if ((key.size() == 3) && 
					(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
					(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
					(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				  )
			{
				EU4Country* country = new EU4Country(lowerCountries[j]);
				countries.insert(make_pair(country->getTag(), country));
			}
		}
	}

/*	// Old Code
	vector<Object*> leaves = obj->getLeaves();

	provinces.clear();
	countries.clear();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
		{
			EU3Province* province = new EU3Province(leaves[i]);
			provinces.insert(make_pair(province->getNum(), province));
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) && 
					(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
					(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
					(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				  )
		{
			EU3Country* country = new EU3Country(leaves[i]);
			countries.insert(make_pair(country->getTag(), country));
		}
	}
*/

	// add province owner info to countries
	for (map<int, EU4Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		map<string, EU4Country*>::iterator j = countries.find( i->second->getOwnerString() );
		if (j != countries.end())
		{
			j->second->addProvince(i->second);
			i->second->setOwner(j->second);
		}
	}

	// add province core info to countries
	for (map<int, EU4Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		vector<EU4Country*> cores = i->second->getCores(countries);
		for (vector<EU4Country*>::iterator j = cores.begin(); j != cores.end(); j++)
		{
			(*j)->addCore(i->second);
		}
	}

	vector<Object*> diploObj = obj->getValue("diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy = new EU4Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = new EU4Diplomacy;
	}

	vector<Object*> tradeObj = obj->getValue("trade");
	if (tradeObj.size() > 0)
	{
		vector<Object*> COTsObj = tradeObj[0]->getValue("cot");
		for (vector<Object*>::iterator i = COTsObj.begin(); i != COTsObj.end(); i++)
		{
			int location = atoi( (*i)->getValue("location")[0]->getLeaf().c_str() );
			map<int, EU4Province*>::iterator j = provinces.find(location);
			if ( j != provinces.end() )
			{
				j->second->setCOT(true);
			}
		}
	}
}

EU4Country* EU4World::getCountry(string tag) const
{
	map<string, EU4Country*>::const_iterator i = countries.find(tag);
	if (i != countries.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


EU4Province* EU4World::getProvince(int provNum) const
{
	map<int, EU4Province*>::const_iterator i = provinces.find(provNum);
	if (i != provinces.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


void EU4World::removeCountry(string tag)
{
	countries.erase(tag);
}


void EU4World::resolveRegimentTypes(const RegimentTypeMap& rtMap)
{
	for (map<string, EU4Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		itr->second->resolveRegimentTypes(rtMap);
	}
}


WorldType EU4World::getWorldType()
{
	if (cachedWorldType != Unknown)
	{
		return cachedWorldType;
	}

	int maxProvinceID = 2002;
	for (map<int, EU4Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ( itr->first > maxProvinceID )
		{
			maxProvinceID = itr->first;
		}
	}

	switch (maxProvinceID)
	{
	case 2002:
		cachedWorldType = Base;
		break;
	default:
		log("	Unrecognized max province ID: %d\n", maxProvinceID);
		if (maxProvinceID < 2002)
		{
			cachedWorldType = Unknown; // possibly modded
		}
		break;
	}

	// Allow the configuration file to override the game type
	string configWorldType = Configuration::getEU4Gametype();
	WorldType forcedWorldType = Unknown;
	if (configWorldType == "base")
	{
		forcedWorldType = Base;
	}
	else if (configWorldType == "auto")
	{
		forcedWorldType = cachedWorldType;
	}

	if ((cachedWorldType != forcedWorldType) && (cachedWorldType != Unknown))
	{
		log("	Warning: world type was detected successfuly, but a different type was specified in the configuration file!\n");
	}

	if (cachedWorldType == Unknown)
	{
		log("	Error: world type unknown!\n");
	}

	if (forcedWorldType != Unknown)
	{
		cachedWorldType = forcedWorldType;
	}

	return cachedWorldType;
}


void EU4World::checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const
{
	for (map<int, EU4Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		inverseProvinceMapping::const_iterator j = inverseProvinceMap.find(i->first);
		if ( (j == inverseProvinceMap.end()) || (j->second.size() == 0) )
		{
			log("	Error: no destination for province #%d\n", i->first);
		}
	}
}