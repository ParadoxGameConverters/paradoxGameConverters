#include "EU3World.h"
#include "Log.h"
#include "Configuration.h"
#include "Mapper.h"
#include "Parsers\Object.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3Diplomacy.h"



EU3World::EU3World()
{
	cachedWorldType	= Unknown;
	provinces.clear();
	countries.clear();
	diplomacy			= NULL;
}


void EU3World::init(Object* obj) {
	string key;	
	vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
		{
			EU3Province* province = new EU3Province(leaves[i]);
			provinces.push_back(province);
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) && 
					(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
					(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
					(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				  )
		{
			EU3Country* country = new EU3Country;
			country->init(leaves[i]);
			countries.insert(make_pair(country->getTag(), country));
		}
	}

	// add province owner info to countries
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		for (map<string, EU3Country*>::iterator j = countries.begin(); j != countries.end(); j++)
		{
			if (provinces[i]->getOwner() == j->second->getTag())
			{
				j->second->addProvince(provinces[i]);
				break;
			}
		}
	}

	// add province core info to countries
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		vector<EU3Country*> cores = provinces[i]->getCores(countries);
		for (unsigned int j = 0; j < cores.size(); j++)
		{
			cores[j]->addCore(provinces[i]);
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
		for (unsigned int i = 0; i < COTsObj.size(); i++)
		{
			int location = atoi( COTsObj[i]->getValue("location")[0]->getLeaf().c_str() );
			for (unsigned int j = 0; j < provinces.size(); j++)
			{
				if (provinces[j]->getNum() == location)
				{
					provinces[j]->setCOT(true);
					break;
				}
			}
		}
	}
}


map<string, EU3Country*> EU3World::getCountries()
{
	return countries;
}


EU3Country* EU3World::getCountry(string tag)
{
	map<string, EU3Country*>::iterator i = countries.find(tag);
	if (i != countries.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


EU3Province* EU3World::getProvince(int provNum)
{
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		if (provinces[i]->getNum() == provNum)
		{
			return provinces[i];
		}
	}
	return NULL;
}


// calling vector::erase in a loop is really slow.  batch removals to avoid it.
void EU3World::removeCountries(vector<string>& tags)
{
	for (vector<string>::iterator i = tags.begin(); i != tags.end(); i++)
	{
		countries.erase(*i);
	}
}


void EU3World::removeCountry(string tag)
{
	countries.erase(tag);
}


EU3Diplomacy* EU3World::getDiplomacy()
{
	return diplomacy;
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
		return cachedWorldType;

	int maxProvinceID = 0;
	for (vector<EU3Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ( (*itr)->getNum() > maxProvinceID )
			maxProvinceID = (*itr)->getNum();
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
		forcedWorldType = DivineWind;
	else if (configWorldType == "httt")
		forcedWorldType = HeirToTheThrone;
	else if (configWorldType == "in")
		forcedWorldType = InNomine;
	else if (configWorldType == "auto")
		forcedWorldType = cachedWorldType;

	if ((cachedWorldType != forcedWorldType) && (cachedWorldType != Unknown))
		log("	Warning: world type was detected successfuly, but a different type was specified in the configuration file!\n");

	if (cachedWorldType == Unknown)
		log("	Error: world type unknown!\n");

	if (forcedWorldType != Unknown)
		cachedWorldType = forcedWorldType;

	return cachedWorldType;
}


void EU3World::checkAllProvincesMapped(provinceMapping provinceMap)
{
	inverseProvinceMapping inverseMap = invertProvinceMap(provinceMap);
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		int srcNum					= provinces[i]->getNum();
		vector<int> destNums		= inverseMap[srcNum];
		if (destNums.size() == 0)
		{
			log("	Error: no destination for province #%d\n", srcNum);
		}
	}
}