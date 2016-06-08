/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include <codecvt>
#include "ParadoxParser.h"
#include "Log.h"
#include "../Configuration.h"
#include "../../../common_items/WinUtils.h"
#include "V2Province.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Pop.h"
#include "V2Country.h"
#include "V2Party.h"




V2World::V2World(Object* obj, const inventionNumToName& iNumToName, map<wstring, wstring>& armyTechs, map<wstring, wstring>& navyTechs, const continentMapping& continentMap)
{
	provinces.clear();
	countries.clear();
	vector<Object*> leaves = obj->getLeaves();

	// Get great nation indices
	unsigned countriesIndex = 1; // Starts from 1 at REB
	vector<Object*> greatNationsObj = obj->getValue(L"great_nations");
	map<int, int> greatNationIndices; // Map greatNation index to its ranking (i.e. 0 - 7)
	if (greatNationsObj.size() > 0)
	{
		vector<wstring> greatNations = greatNationsObj[0]->getTokens();
		for (unsigned int i = 0; i < greatNations.size(); i++)
		{
			greatNationIndices.insert(make_pair(_wtoi(greatNations[i].c_str()), i));
		}

		greatCountries.resize(greatNations.size());
	}

	for (auto leaf: leaves)
	{
		wstring key = leaf->getKey();

		// Is this a numeric value? If so, must be a province
		if (_wtoi(key.c_str()) > 0)
		{
			provinces[_wtoi(key.c_str())] = new V2Province(leaf);
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
			countries[key] = new V2Country(leaf, iNumToName, armyTechs, navyTechs, continentMap);

			map<int, int>::iterator rankingItr = greatNationIndices.find(countriesIndex++);
			if (rankingItr != greatNationIndices.end())
			{
				LOG(LogLevel::Debug) << "Set " << key << " as Great Power #" << rankingItr->second + 1;
				countries[key]->setGreatNation(true);
				greatCountries[rankingItr->second] = key;
			}
		}
	}

	// add province owner info to countries
	for (auto province: provinces)
	{
		auto country = countries.find(province.second->getOwnerString());
		if (country != countries.end())
		{
			country->second->addProvince(province.first, province.second);
			province.second->setOwner(country->second);
		}
	}

	// add province core info to countries
	for (auto province: provinces)
	{
		vector<V2Country*> coreCountries = province.second->getCores(countries);	// the cores held on this province
		for (auto coreCountry: coreCountries)
		{
			coreCountry->addCore(province.second);
		}
	}

	// apply workers to provinces
	for (auto country : countries)
	{
		country.second->putWorkersInProvinces();
	}

	// Cull countries with neither cores nor owned provinces (i.e. dead countries and uncreated dominions)
	removeEmptyNations();

	// Diplomacy
	vector<Object*> diploObj = obj->getValue(L"diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy = V2Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = V2Diplomacy();
	}

	// get country colours and parties
	vector<wstring> vic2Mods = Configuration::getVic2Mods();
	for (auto itr: vic2Mods)
	{
		readCountryFiles(Configuration::getV2Path() + L"\\mod\\" + itr + L"\\common\\countries.txt", itr);
	}
	if (vic2Mods.size() == 0)
	{
		readCountryFiles(Configuration::getV2Path() + L"\\common\\countries.txt", L"");
	}
}


V2Country* V2World::getCountry(wstring tag) const
{
	map<wstring, V2Country*>::const_iterator itr = countries.find(tag);
	if (itr != countries.end())
	{
		return itr->second;
	}
	else
	{
		return NULL;
	}
}


void V2World::removeCountry(wstring tag)
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
	for (auto province: provinces)
	{
		inverseProvinceMapping::const_iterator mapping = inverseProvinceMap.find(province.first);
		if (mapping == inverseProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for Vic2 province " << province.first;
		}
	}
}


void V2World::setLocalisations(V2Localisation& localisation)
{
	for (auto countryItr: countries)
	{
		const auto& nameLocalisations = localisation.GetTextInEachLanguage(countryItr.second->getTag());	// the names in all languages
		for (const auto& nameLocalisation : nameLocalisations)	// the name under consideration
		{
			const std::wstring& language = nameLocalisation.first;	// the language
			const std::wstring& name = nameLocalisation.second;		// the name of the country in this language
			countryItr.second->setLocalisationName(language, name);
		}
		const auto& adjectiveLocalisations = localisation.GetTextInEachLanguage(countryItr.second->getTag() + L"_ADJ");	// the adjectives in all languages
		for (const auto& adjectiveLocalisation : adjectiveLocalisations)	// the adjective under consideration
		{
			const std::wstring& language = adjectiveLocalisation.first;		// the language
			const std::wstring& adjective = adjectiveLocalisation.second;	// the adjective for the country in this language
			countryItr.second->setLocalisationAdjective(language, adjective);
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
	for (auto country = countries.begin(); country != countries.end();)
	{
		if ((country->second->getCores().size() < 1) && (country->second->getProvinces().size() < 1))
		{
			country = countries.erase(country); // i points to the next element
		}
		else
		{
			++country;
		}
	}
}


void V2World::readCountryFiles(wstring countryListFile, wstring mod)
{
	struct _stat st;
	wifstream V2CountriesInput;
	V2CountriesInput.open(countryListFile.c_str());
	if (!V2CountriesInput.is_open())
	{
		if (mod == L"")
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
		wstring line;
		getline(V2CountriesInput, line);

		if ((line[0] == '#') || (line.size() < 3))
		{
			continue;
		}
		else if (line.substr(0, 12) == L"dynamic_tags")
		{
			continue;
		}

		wstring tag;
		tag = line.substr(0, 3);

		wstring countryFileName;
		int start = line.find_first_of('/');
		int size = line.find_last_of('\"') - start;
		countryFileName = line.substr(start, size);

		Object* countryData = NULL;
		wstring file;
		if (mod != L"")
		{
			file = Configuration::getV2Path() + L"\\mod\\" + mod + L"\\common\\countries\\" + countryFileName;
			if (_wstat(file.c_str(), &st) == 0)
			{
				countryData = parser_8859_15::doParseFile(file.c_str());
				if (countryData == NULL)
				{
					LOG(LogLevel::Warning) << "Could not parse file " << file;
				}
			}
		}
		if (countryData == NULL)
		{
			file = Configuration::getV2Path() +  L"\\common\\countries\\" + countryFileName;
			if (_wstat(file.c_str(), &st) == 0)
			{
				countryData = parser_8859_15::doParseFile(file.c_str());
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

		vector<Object*> colorObj = countryData->getValue(L"color");
		if (colorObj.size() > 0)
		{
			vector<wstring> rgb = colorObj[0]->getTokens();
			if (rgb.size() == 3)
			{
				if (countries.find(tag) != countries.end())
				{
					countries[tag]->setColor(Color(_wtoi(rgb[0].c_str()), _wtoi(rgb[1].c_str()), _wtoi(rgb[2].c_str())));
				}
			}
		}

		// Get party information
		vector<Object*> leaves = countryData->getLeaves();

		for (auto leaf: leaves)
		{
			wstring key = leaf->getKey();
			if (key == L"party")
			{
				parties.push_back(new V2Party(leaf));
			}
		}
	}
	V2CountriesInput.close();
}