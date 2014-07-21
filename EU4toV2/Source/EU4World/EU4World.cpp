/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "EU4World.h"
#include <algorithm>
#include <fstream>
#include "../Log.h"
#include "../Configuration.h"
#include "../Mapper.h"
#include "../Parsers/Object.h"
#include "../Parsers/Parser.h"
#include "EU4Province.h"
#include "EU4Country.h"
#include "EU4Diplomacy.h"
#include "EU4Version.h"
#include "EU4Localisation.h"



EU4World::EU4World(Object* obj, map<string, int> armyInvIdeas, map<string, int> commerceInvIdeas, map<string, int> cultureInvIdeas, map<string, int> industryInvIdeas, map<string, int> navyInvIdeas)
{
	vector<Object*> versionObj = obj->getValue("savegame_version");	// the version of the save
	(versionObj.size() > 0) ? version = new EU4Version(versionObj[0]) : version = new EU4Version();

	// Get Provinces and then get Countries
	provinces.clear();
	vector<Object*> provincesObj = obj->getValue("provinces");					// the object holding the provinces
	if (provincesObj.size() > 0)
	{
		vector<Object*> provincesLeaves = provincesObj[0]->getLeaves();		// the objects holding the individual provinces
		for (unsigned int j = 0; j < provincesLeaves.size(); j++)
		{
			string keyProv = (provincesLeaves[j])->getKey();						// the key for the province

			if (atoi(keyProv.c_str()) < 0)												// Check if key is a negative value (EU4 style)
			{
				EU4Province* province = new EU4Province((provincesLeaves[j]));	// the province in our format
				provinces.insert(make_pair(province->getNum(), province));
			}
		}
	}

	countries.clear();
	vector<Object*> countriesObj = obj->getValue("countries");				// the object holding the countries
	if (countriesObj.size() > 0)
	{
		vector<Object*> countriesLeaves = countriesObj[0]->getLeaves();	// the objects holding the countries themselves
		for (unsigned int j = 0; j < countriesLeaves.size(); j++)
		{
			string keyCoun = countriesLeaves[j]->getKey();						// the key for this country

			if ((keyCoun == "---") || (keyCoun == "REB") || (keyCoun == "PIR") || (keyCoun == "NAT"))
			{
				continue;
			}
			else
			{
				EU4Country* country = new EU4Country(countriesLeaves[j], armyInvIdeas, commerceInvIdeas, cultureInvIdeas, industryInvIdeas, navyInvIdeas);	// the country in our format
				if (country->isUnusedCountry())
				{
					LOG(LogLevel::Debug) << "Discarding unused EU4 tag " << country->getTag();
					delete country;
					continue;
				}
				countries.insert(make_pair(country->getTag(), country));
			}
		}
	}

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
		vector<EU4Country*> cores = i->second->getCores(countries);	// the cores held on this province
		for (vector<EU4Country*>::iterator j = cores.begin(); j != cores.end(); j++)
		{
			(*j)->addCore(i->second);
		}
	}

	vector<Object*> diploObj = obj->getValue("diplomacy");	// the object holding the world's diplomacy
	if (diploObj.size() > 0)
	{
		diplomacy = new EU4Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = new EU4Diplomacy;
	}
}


void EU4World::readCommonCountries(istream& in, const std::string& rootPath)
{
	// Add any info from common\countries
	const int maxLineLength = 10000;	// the maximum line length
	char line[maxLineLength];			// the line being processed

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
			map<string, EU4Country*>::iterator findIter = countries.find(tag);
			if (findIter != countries.end())
			{
				EU4Country* country = findIter->second;
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


EU4Country* EU4World::getCountry(string tag) const
{
	map<string, EU4Country*>::const_iterator i = countries.find(tag);
	return (i != countries.end()) ? i->second : NULL;
}


EU4Province* EU4World::getProvince(const int provNum) const
{
	map<int, EU4Province*>::const_iterator i = provinces.find(provNum);
	return (i != provinces.end()) ? i->second : NULL;
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


void EU4World::checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const
{
	for (map<int, EU4Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		inverseProvinceMapping::const_iterator j = inverseProvinceMap.find(i->first);
		if (j == inverseProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for province " << i->first;
		}
	}
}


void EU4World::setLocalisations(EU4Localisation& localisation)
{
	for (map<string, EU4Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		const auto& nameLocalisations = localisation.GetTextInEachLanguage(countryItr->second->getTag());	// the names in all languages
		for (const auto& nameLocalisation : nameLocalisations)	// the name under consideration
		{
			const std::string& language = nameLocalisation.first;	// the language
			const std::string& name = nameLocalisation.second;		// the name of the country in this language
			countryItr->second->setLocalisationName(language, name);
		}
		const auto& adjectiveLocalisations = localisation.GetTextInEachLanguage(countryItr->second->getTag() + "_ADJ");	// the adjectives in all languages
		for (const auto& adjectiveLocalisation : adjectiveLocalisations)	// the adjective under consideration
		{
			const std::string& language = adjectiveLocalisation.first;		// the language
			const std::string& adjective = adjectiveLocalisation.second;	// the adjective for the country in this language
			countryItr->second->setLocalisationAdjective(language, adjective);
		}
	}
}