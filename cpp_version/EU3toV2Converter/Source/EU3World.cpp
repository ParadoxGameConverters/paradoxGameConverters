#include "EU3World.h"
#include "Log.h"
#include "Configuration.h"
#include "Mapper.h"
#include "Parsers\Object.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3Diplomacy.h"



EU3World::EU3World(Object* obj)
{
	cachedWorldType = Unknown;

	string key;	
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
			provinces.insert( make_pair(province->getNum(), province) );
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

	// add province owner info to countries
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		map<string, EU3Country*>::iterator j = countries.find( i->second->getOwner() );
		if (j != countries.end())
		{
			j->second->addProvince(i->second);
		}
	}

	// add province core info to countries
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		vector<EU3Country*> cores = i->second->getCores(countries);
		for (vector<EU3Country*>::iterator j = cores.begin(); j != cores.end(); j++)
		{
			(*j)->addCore(i->second);
		}
	}

	vector<Object*> diploObj = obj->getValue("diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy = new EU3Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = new EU3Diplomacy;
	}

	vector<Object*> tradeObj = obj->getValue("trade");
	if (tradeObj.size() > 0)
	{
		vector<Object*> COTsObj = tradeObj[0]->getValue("cot");
		for (vector<Object*>::iterator i = COTsObj.begin(); i != COTsObj.end(); i++)
		{
			int location = atoi( (*i)->getValue("location")[0]->getLeaf().c_str() );
			map<int, EU3Province*>::iterator j = provinces.find(location);
			if ( j != provinces.end() )
			{
				j->second->setCOT(true);
			}
		}
	}
}


EU3Country* EU3World::getCountry(const string tag) const
{
	map<string, EU3Country*>::const_iterator i = countries.find(tag);
	if (i != countries.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


EU3Province* EU3World::getProvince(const int provNum) const
{
	map<int, EU3Province*>::const_iterator i = provinces.find(provNum);
	if (i != provinces.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


void EU3World::removeCountry(const string tag)
{
	countries.erase(tag);
}


void EU3World::resolveRegimentTypes(const RegimentTypeMap& rtMap)
{
	for (map<string, EU3Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		itr->second->resolveRegimentTypes(rtMap);
	}
}


WorldType EU3World::getWorldType()
{
	if (cachedWorldType != Unknown)
	{
		return cachedWorldType;
	}

	int maxProvinceID = 0;
	for (map<int, EU3Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ( itr->first > maxProvinceID )
		{
			maxProvinceID = itr->first;
		}
	}

	switch (maxProvinceID)
	{
	case 1774:
	case 1775:
		cachedWorldType = InNomine;
		break;
	case 1814:
		cachedWorldType = HeirToTheThrone;
		break;
	case 1882:
		cachedWorldType = DivineWind;
		break;
	default:
		log("	Unrecognized max province ID: %d\n", maxProvinceID);
		if (maxProvinceID < 1774)
		{
			cachedWorldType = VeryOld; // pre-IN
		}
		else
		{
			cachedWorldType = Unknown; // possibly modded
		}
		break;
	}

	// Allow the configuration file to override the game type
	string configWorldType = Configuration::getEU3Gametype();
	WorldType forcedWorldType = Unknown;
	if (configWorldType == "dw")
	{
		forcedWorldType = DivineWind;
	}
	else if (configWorldType == "httt")
	{
		forcedWorldType = HeirToTheThrone;
	}
	else if (configWorldType == "in")
	{
		forcedWorldType = InNomine;
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


void EU3World::checkAllProvincesMapped(const inverseProvinceMapping inverseProvinceMap) const
{
	for (map<int, EU3Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		inverseProvinceMapping::const_iterator j = inverseProvinceMap.find(i->first);
		if ( (j != inverseProvinceMap.end()) && (j->second.size() == 0) )
		{
			log("	Error: no destination for province #%d\n", i->first);
		}
	}
}