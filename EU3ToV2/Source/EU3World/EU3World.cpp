#include "EU3World.h"
#include <algorithm>
#include "../Log.h"
#include "../Configuration.h"
#include "../Mapper.h"
#include "../Parsers/Object.h"
#include "../Parsers/Parser.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3Diplomacy.h"
#include "EU3Localisation.h"



EU3World::EU3World(EU3Localisation& localisation, Object* obj)
{
	cachedWorldType = unknown;

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
			provinces.insert(make_pair(province->getNum(), province));
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) && 
					(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
					(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
					(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				  )
		{
			if ((key == "---") || (key == "REB") || (key == "PIR") || (key == "NAT"))
			{
				continue;
			}
			else
			{
				EU3Country* country = new EU3Country(leaves[i]);
				const auto& nameLocalisations = localisation.GetTextInEachLanguage(country->getTag());
				for (const auto& nameLocalisation : nameLocalisations)
				{
					const std::string& language = nameLocalisation.first;
					const std::string& name = nameLocalisation.second;
					country->setLocalisationName(language, name);
				}
				const auto& adjectiveLocalisations = localisation.GetTextInEachLanguage(country->getTag() + "_ADJ");
				for (const auto& adjectiveLocalisation : adjectiveLocalisations)
				{
					const std::string& language = adjectiveLocalisation.first;
					const std::string& adjective = adjectiveLocalisation.second;
					country->setLocalisationAdjective(language, adjective);
				}
				countries.insert(make_pair(country->getTag(), country));
			}
		}
	}

	// add province owner info to countries
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		map<string, EU3Country*>::iterator j = countries.find( i->second->getOwnerString() );
		if (j != countries.end())
		{
			j->second->addProvince(i->second);
			i->second->setOwner(j->second);
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


void EU3World::readCommonCountries(istream& in, const std::string& rootPath)
{
	// Add any info from common\countries
	const int maxLineLength = 10000;
	char line[maxLineLength];

	while (true)
	{
		in.getline(line, maxLineLength);
		if (in.eof())
		{
			return;
		}
		std::string countryLine = line;
		if (countryLine.size() >= 6 && countryLine[0] != '#')
		{
			// First three characters must be the tag.
			std::string tag = countryLine.substr(0, 3);
			map<string, EU3Country*>::iterator findIter = countries.find(tag);
			if (findIter != countries.end())
			{
				EU3Country* country = findIter->second;

				// The country file name is all the text after the equals sign (possibly in quotes).
				size_t equalPos = countryLine.find('=', 3);
				size_t beginPos = countryLine.find_first_not_of(' ', equalPos + 1);
				size_t endPos = countryLine.find_last_not_of(' ') + 1;
				std::string fileName = countryLine.substr(beginPos, endPos - beginPos);
				if (fileName.front() == '"' && fileName.back() == '"')
				{
					fileName = fileName.substr(1, fileName.size() - 2);
				}
				std::replace(fileName.begin(), fileName.end(), '/', '\\');

				// Parse the country file.
				std::string path = rootPath + "\\common\\" + fileName;
				size_t lastPathSeparatorPos = path.find_last_of('\\');
				std::string localFileName = path.substr(lastPathSeparatorPos + 1, string::npos);
				country->readFromCommonCountry(localFileName, doParseFile(path.c_str()));
			}
		}
	}
}


EU3Country* EU3World::getCountry(string tag) const
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


EU3Province* EU3World::getProvince(int provNum) const
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


void EU3World::removeCountry(string tag)
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
	if (cachedWorldType != unknown)
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
		Log(LogLevel::Warning) << "Unrecognized max province ID: " << maxProvinceID;
		if (maxProvinceID < 1774)
		{
			cachedWorldType = VeryOld; // pre-IN
		}
		else
		{
			cachedWorldType = unknown; // possibly modded
		}
		break;
	}

	// Allow the configuration file to override the game type
	string configWorldType = Configuration::getEU3Gametype();
	WorldType forcedWorldType = unknown;
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

	if ((cachedWorldType != forcedWorldType) && (cachedWorldType != unknown))
	{
		Log(LogLevel::Warning) << "World type was detected successfuly, but a different type was specified in the configuration file!";
	}

	if (cachedWorldType == unknown)
	{
		Log(LogLevel::Warning) << "World type unknown!";
	}

	if (forcedWorldType != unknown)
	{
		cachedWorldType = forcedWorldType;
	}

	return cachedWorldType;
}


void EU3World::checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const
{
	for (map<int, EU3Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		inverseProvinceMapping::const_iterator j = inverseProvinceMap.find(i->first);
		if (j == inverseProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for province " << i->first;
		}
	}
}