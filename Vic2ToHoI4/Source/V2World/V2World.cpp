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
#include "ParadoxParser8859_15.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "V2Country.h"
#include "V2Diplomacy.h"
#include "V2Party.h"
#include "V2Province.h"
#include "Vic2State.h"
#include "../Mappers/ProvinceMapper.h"



V2World::V2World(Object* obj, const inventionNumToName& iNumToName, const stateMapping& stateMap)
{
	provinces.clear();
	countries.clear();
	parties.clear();
	greatPowers.clear();

	map<int, int> countryIndexToGPRank = extractGreatNationIndices(obj);
	unsigned int countryIndex = 1; // Starts from 1 at REB

	vector<Object*> leaves = obj->getLeaves();
	for (auto leaf: leaves)
	{
		string key = leaf->getKey();

		if (isProvinceKey(key))
		{
			provinces[atoi(key.c_str())] = new V2Province(leaf);
		}
		else if (isCountryKey(key))
		{
			countries[key] = new V2Country(leaf, iNumToName);
			setGreatPowerStatus(key, countryIndexToGPRank, countryIndex);
			countryIndex++;
		}
	}

	setProvinceOwners();
	addProvinceCoreInfoToCountries();
	applyWorkersToProvinces();
	removeEmptyNations();
	determinePartialStates(stateMap);
	inputDiplomacy(obj->getValue("diplomacy"));
	readCountryFiles();
}


map<int, int> V2World::extractGreatNationIndices(const Object* obj)
{
	map<int, int> countryIndexToGPRank;

	vector<Object*> greatNationsObj = obj->getValue("great_nations");
	if (greatNationsObj.size() > 0)
	{
		vector<string> greatNations = greatNationsObj[0]->getTokens();
		for (unsigned int i = 0; i < greatNations.size(); i++)
		{
			countryIndexToGPRank.insert(make_pair(atoi(greatNations[i].c_str()), i));
		}

		greatPowers.resize(greatNations.size());
	}

	return countryIndexToGPRank;
}


bool V2World::isProvinceKey(string key) const
{
	return ((!key.empty()) && (key.find_first_not_of("0123456789") == string::npos));
}


bool V2World::isCountryKey(string key) const
{
	return
	(
		(key.size() == 3) &&
		(isNormalCountryKey(key) || isDominionCountryKey(key) || isConvertedCountryKey(key))
	);
}


bool V2World::isNormalCountryKey(string key) const
{
	return (isupper(key.c_str()[0]) && isupper(key.c_str()[1]) && isupper(key.c_str()[2]));
}


bool V2World::isDominionCountryKey(string key) const
{
	return ((key.c_str()[0] == 'D') && isdigit(key.c_str()[1]) && isdigit(key.c_str()[2]));
}


bool V2World::isConvertedCountryKey(string key) const
{
	return (isupper(key.c_str()[0]) && isdigit(key.c_str()[1]) && isdigit(key.c_str()[2]));
}


void V2World::setGreatPowerStatus(string tag, const map<int, int>& countryIndexToGPRank, const unsigned int& countryIndex)
{
	auto rankingItr = countryIndexToGPRank.find(countryIndex);
	if (rankingItr != countryIndexToGPRank.end())
	{
		countries[tag]->setAsGreatNation();
		greatPowers[rankingItr->second] = tag;
	}
}


void V2World::setProvinceOwners()
{
	for (auto province: provinces)
	{
		auto country = countries.find(province.second->getOwnerString());
		if (country != countries.end())
		{
			country->second->addProvince(province);
			province.second->setOwner(country->second);
		}
	}
	for (auto country: countries)
	{
		country.second->putProvincesInStates();
	}
}


void V2World::addProvinceCoreInfoToCountries()
{
	for (auto province: provinces)
	{
		province.second->setCores(countries);
		auto provinceCores = province.second->getCores();
		for (auto coreCountry: provinceCores)
		{
			coreCountry->addCore(province.second);
		}
	}
}


void V2World::applyWorkersToProvinces()
{
	for (auto country: countries)
	{
		country.second->putWorkersInProvinces();
	}
}


void V2World::removeEmptyNations()
{
	map<string, V2Country*> newCountries;

	for (auto country: countries)
	{
		if (!country.second->isEmpty())
		{
			newCountries.insert(country);
		}
	}

	countries.swap(newCountries);
}


void V2World::determinePartialStates(const stateMapping& stateMap)
{
	for (auto country: countries)
	{
		for (auto state: country.second->getStates())
		{
			state->determinePartialState(stateMap);
		}
	}
}


void V2World::inputDiplomacy(const vector<Object*> diplomacyObj)
{
	if (diplomacyObj.size() > 0)
	{
		diplomacy = new V2Diplomacy(diplomacyObj[0]);
	}
	else
	{
		diplomacy = new V2Diplomacy();
	}
}


void V2World::readCountryFiles()
{
	bool countriesDotTxtRead = false;

	for (auto vic2Mod: Configuration::getVic2Mods())
	{
		if (processCountriesDotTxt(Configuration::getV2Path() + "/mod/" + vic2Mod + "/common/countries.txt", vic2Mod))
		{
			countriesDotTxtRead = true;
		}
	}
	if (!countriesDotTxtRead)
	{
		if (!processCountriesDotTxt(Configuration::getV2Path() + "/common/countries.txt", ""))
		{
			LOG(LogLevel::Error) << "Could not open " << Configuration::getV2Path() + "/common/countries.txt";
			exit(1);
		}
	}
}


bool V2World::processCountriesDotTxt(string countryListFile, string mod)
{
	ifstream V2CountriesInput(countryListFile.c_str());
	if (!V2CountriesInput.is_open())
	{
		return false;
	}

	while (!V2CountriesInput.eof())
	{
		string line;
		getline(V2CountriesInput, line);
		if (shouldLineBeSkipped(line))
		{
			continue;
		}

		string countryFileName = extractCountryFileName(line);
		Object* countryData = readCountryFile(countryFileName, mod);
		if (countryData == NULL)
		{
			continue;
		}
		readCountryColor(countryData, line);
		inputPartyInformation(countryData->getLeaves());
	}

	V2CountriesInput.close();
	return true;
}


bool V2World::shouldLineBeSkipped(string line) const
{
	return ((line[0] == '#') || (line.size() < 3) || (line.substr(0, 12) == "dynamic_tags"));
}


string V2World::extractCountryFileName(string countryFileLine) const
{
	string countryFileName;
	int start = countryFileLine.find_first_of('/');
	int size = countryFileLine.find_last_of('\"') - start;
	countryFileName = countryFileLine.substr(start, size);

	return countryFileName;
}


Object* V2World::readCountryFile(string countryFileName, string mod) const
{
	Object* countryData = NULL;
	if (mod != "")
	{
		string file = Configuration::getV2Path() + "/mod/" + mod + "/common/countries/" + countryFileName;
		if (Utils::DoesFileExist(file.c_str()))
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
		string file = Configuration::getV2Path() +  "/common/countries/" + countryFileName;
		if (Utils::DoesFileExist(file.c_str()))
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
		}
	}

	return countryData;
}


void V2World::readCountryColor(const Object* countryData, string line)
{
	string tag = line.substr(0, 3);
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
}


void V2World::inputPartyInformation(const vector<Object*>& leaves)
{
	for (auto leaf: leaves)
	{
		string key = leaf->getKey();
		if (key == "party")
		{
			parties.push_back(new V2Party(leaf));
		}
	}
}


void V2World::mergeNations(string masterTag, const vector<string>& slaveTags)
{
	V2Country* master = getCountry(masterTag);
	if (master != NULL)
	{
		for (auto slaveTag: slaveTags)
		{
			V2Country* slave = getCountry(slaveTag);
			if (slave != NULL)
			{
				master->eatCountry(slave);
			}
		}
	}
}


V2Country* V2World::getCountry(string tag) const
{
	auto countryItr = countries.find(tag);
	return (countryItr != countries.end()) ? countryItr->second : NULL;
}


void V2World::setLocalisations(const V2Localisation& localisation, const stateIdMapping& stateIdMap)
{
	for (auto country: countries)
	{
		country.second->setLocalisationNames(localisation);
		country.second->setLocalisationAdjectives(localisation);
		country.second->setStateIDs(stateIdMap);
	}
}


const V2Province* V2World::getProvince(int provNum) const
{
	auto provinceItr = provinces.find(provNum);
	return (provinceItr != provinces.end()) ? provinceItr->second : NULL;
}


void V2World::checkAllProvincesMapped() const
{
	auto Vic2ToHoI4ProvinceMapping = provinceMapper::getVic2ToHoI4ProvinceMapping();
	for (auto province: provinces)
	{
		if (Vic2ToHoI4ProvinceMapping.find(province.first) == Vic2ToHoI4ProvinceMapping.end())
		{
			LOG(LogLevel::Warning) << "No mapping for Vic2 province " << province.first;
		}
	}
}