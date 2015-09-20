/*Copyright (c) 2015 The Paradox Game Converters Project

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



#include "V2World.h"
#include <fstream>
#include <sys/stat.h>
#include "../Parsers/Parser.h"
#include "../Log.h"
#include "../Configuration.h"
#include "../WinUtils.h"
#include "V2Province.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Pop.h"
#include "V2Country.h"
#include "V2Party.h"




V2World::V2World(Object* obj)
{
	provinces.clear();
	countries.clear();
	string key;
	vector<Object*> leaves = obj->getLeaves();

	// Get great nation indices
	unsigned countriesIndex = 1; // Starts from 1 at REB
	vector<Object*> greatNationsObj = obj->getValue("great_nations");
	map<int, int> greatNationIndices; // Map greatNation index to its ranking (i.e. 0 - 7)
	if (greatNationsObj.size() > 0)
	{
		vector<string> greatNations = greatNationsObj[0]->getTokens();
		for (unsigned int i = 0; i < greatNations.size(); i++)
		{
			greatNationIndices.insert(make_pair(atoi(greatNations[i].c_str()), i));
		}

		greatCountries.resize(greatNations.size());
	}

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
		{
			provinces[atoi(key.c_str())] = new V2Province(leaves[i]);
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) &&
			(
				(
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') &&
				(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				)
				||
				( // Dominions
				(key.c_str()[0] == 'D') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
				||
				( // Others (From previous conversion)
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
			)
		)
		{
			countries[key] = new V2Country(leaves[i]);

			map<int, int>::iterator rankingItr = greatNationIndices.find(countriesIndex++);
			if (rankingItr != greatNationIndices.end())
			{
				LOG(LogLevel::Debug) << "Set " << key << " as Great Power #" << rankingItr->second + 1;
				countries[key]->setGreatNationRanking(rankingItr->second);
				greatCountries[rankingItr->second] = key;
			}
		}
	}

	// add province owner info to countries
	for (map<int, V2Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		map<string, V2Country*>::iterator j = countries.find(i->second->getOwnerString());
		if (j != countries.end())
		{
			j->second->addProvince(i->first, i->second);
			i->second->setOwner(j->second);
		}
	}

	// add province core info to countries
	for (map<int, V2Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		vector<V2Country*> cores = i->second->getCores(countries);	// the cores held on this province
		for (vector<V2Country*>::iterator j = cores.begin(); j != cores.end(); j++)
		{
			(*j)->addCore(i->second);
		}
	}

	// BE: Cull countries with neither cores nor owned provinces (i.e. dead countries and uncreated dominions)
	removeEmptyNations();

	// Diplomacy
	vector<Object*> diploObj = obj->getValue("diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy = V2Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = V2Diplomacy();
	}

	// get country colours and parties
	vector<string> vic2Mods = Configuration::getVic2Mods();
	for (auto itr: vic2Mods)
	{
		readCountryFiles(Configuration::getV2Path() + "\\mod\\" + itr + "\\common\\countries.txt", itr);
	}
	if (vic2Mods.size() == 0)
	{
		readCountryFiles(Configuration::getV2Path() + "\\common\\countries.txt", "");
	}
}


V2Country* V2World::getCountry(string tag) const
{
	map<string, V2Country*>::const_iterator itr = countries.find(tag);
	if (itr != countries.end())
	{
		return itr->second;
	}
	else
	{
		return NULL;
	}
}


void V2World::removeCountry(string tag)
{
	countries.erase(tag);
}


V2Province* V2World::getProvince(int provNum) const
{
	map<int, V2Province*>::const_iterator i = provinces.find(provNum);
	return (i != provinces.end()) ? i->second : NULL;
}


void V2World::checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const
{
	for (map<int, V2Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		inverseProvinceMapping::const_iterator j = inverseProvinceMap.find(i->first);
		if (j == inverseProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for province " << i->first;
		}
	}
}


void V2World::setLocalisations(V2Localisation& localisation)
{
	for (map<string, V2Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
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


V2Party* V2World::getRulingParty(const V2Country* country) const
{
	if (country->getRulingPartyId() > parties.size() || country->getRulingPartyId() < 1)
	{
		return NULL;
	}

	return parties[country->getRulingPartyId() - 1]; // Subtract 1, because party ID starts from index of 1
}


vector<V2Party*> V2World::getActiveParties(const V2Country* country) const
{
	vector<V2Party*> activeParties;

	vector<unsigned int> partyIDs = country->getActiveParties();
	for (auto i: partyIDs)
	{
		if (i < parties.size())
		{
			activeParties.push_back(parties[i - 1]);  // Subtract 1, because party ID starts from index of 1
		}
		else
		{
			LOG(LogLevel::Warning) << "Party ID mismatch! Did some Vic2 country files not get read?";
		}
	}

	return activeParties;
}


void V2World::removeEmptyNations()
{
	for (map<string, V2Country*>::iterator i = countries.begin(); i != countries.end();)
	{
		if (i->second->getCores().size() < 1 && i->second->getProvinces().size() < 1)
		{
			i = countries.erase(i); // i points to the next element
		}
		else
		{
			++i;
		}
	}
}


void V2World::readCountryFiles(string countryListFile, string mod)
{
	struct _stat st;
	ifstream V2CountriesInput;
	V2CountriesInput.open(countryListFile.c_str());
	if (!V2CountriesInput.is_open())
	{
		if (mod == "")
		{
			LOG(LogLevel::Error) << "Could not open " << countryListFile;
			exit(1);
		}
		else
		{
			return;
		}
	}
	while (!V2CountriesInput.eof())
	{
		string line;
		getline(V2CountriesInput, line);

		if ((line[0] == '#') || (line.size() < 3))
		{
			continue;
		}
		else if (line.substr(0, 12) == "dynamic_tags")
		{
			continue;
		}

		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start = line.find_first_of('/');
		int size = line.find_last_of('\"') - start;
		countryFileName = line.substr(start, size);

		Object* countryData = NULL;
		string file;
		if (mod != "")
		{
			file = Configuration::getV2Path() + "\\mod\\" + mod + "\\common\\countries\\" + countryFileName;
			if (_stat(file.c_str(), &st) == 0)
			{
				countryData = doParseFile(file.c_str());
				if (countryData == NULL)
				{
					LOG(LogLevel::Warning) << "Could not parse file " << file;
				}
			}
		}
		if (countryData == NULL)
		{
			file = Configuration::getV2Path() +  "\\common\\countries\\" + countryFileName;
			if (_stat(file.c_str(), &st) == 0)
			{
				countryData = doParseFile(file.c_str());
				if (countryData == NULL)
				{
					LOG(LogLevel::Warning) << "Could not parse file " << file;
				}
			}
			else
			{
				LOG(LogLevel::Debug) << "Could not find file V2 " << file << " - skipping";
				continue;
			}
		}

		vector<Object*> colorObj = countryData->getValue("color");
		if (colorObj.size() > 0)
		{
			vector<string> rgb = colorObj[0]->getTokens();
			if (rgb.size() == 3)
			{
				if (countries.find(tag) != countries.end())
				{
					countries[tag]->setColor(Color(atoi(rgb[0].c_str()), atoi(rgb[1].c_str()), atoi(rgb[2].c_str())));
				}
			}
		}

		// Get party information
		vector<Object*> leaves = countryData->getLeaves();

		for (unsigned int i = 0; i < leaves.size(); i++)
		{
			string key = leaves[i]->getKey();
			if (key == "party")
			{
				parties.push_back(new V2Party(leaves[i]));
			}
		}
	}
	V2CountriesInput.close();
}