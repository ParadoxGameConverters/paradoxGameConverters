/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "World.h"
#include <fstream>
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include "ParserHelpers.h"
#include "CommonCountryData.h"
#include "Country.h"
#include "Diplomacy.h"
#include "Issues.h"
#include "Party.h"
#include "Province.h"
#include "State.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/MergeRules.h"
#include "../Mappers/ProvinceMapper.h"



Vic2::World::World(const string& filename)
{
	Vic2::issuesInstance.instantiate();

	std::vector<int> GPIndexes;
	registerKeyword(std::regex("great_nations"), [&GPIndexes, this](const std::string& unused, std::istream& theStream)
	{
		commonItems::intList indexList(theStream);
		GPIndexes = indexList.getInts();
	});

	registerKeyword(std::regex("\\d+"), [this](const std::string& provinceID, std::istream& theStream)
	{
		provinces[stoi(provinceID)] = new Vic2::Province(provinceID, theStream);
	});

	std::vector<std::string> tagsInOrder;
	tagsInOrder.push_back(""); // REB (first country is index 1
	registerKeyword(std::regex("[A-Z]{3}"), [&tagsInOrder, this](const std::string& countryTag, std::istream& theStream)
	{
		countries[countryTag] = new Country(countryTag, theStream);
		tagsInOrder.push_back(countryTag);
	});
	registerKeyword(std::regex("[A-Z][0-9]{2}"), [&tagsInOrder, this](const std::string& countryTag, std::istream& theStream)
	{
		countries[countryTag] = new Country(countryTag, theStream);
		tagsInOrder.push_back(countryTag);
	});

	registerKeyword(std::regex("diplomacy"), [this](const std::string& top, std::istream& theStream)
	{
		diplomacy = new Vic2::Diplomacy(theStream);
	});

	registerKeyword(std::regex("[A-Za-z0-9_]+"), commonItems::ignoreItem);


	LOG(LogLevel::Info) << "* Importing V2 save *";
	parseFile(filename);


	LOG(LogLevel::Info) << "Building world";
	setGreatPowerStatus(GPIndexes, tagsInOrder);
	setProvinceOwners();
	addProvinceCoreInfoToCountries();
	if (Configuration::getRemoveCores())
	{
		removeSimpleLandlessNations();
	}
	determineEmployedWorkers();
	removeEmptyNations();
	determinePartialStates();
	if (diplomacy == nullptr)
	{
		diplomacy = new Vic2::Diplomacy();
	}
	readCountryFiles();
	setLocalisations();
	handleMissingCountryCultures();

	CountryMapper::createMappings(this);

	overallMergeNations();
	checkAllProvincesMapped();
}


void Vic2::World::setGreatPowerStatus(const std::vector<int>& GPIndexes, const std::vector<std::string>& tagsInOrder)
{
	for (auto index: GPIndexes)
	{
		auto tag = tagsInOrder.at(index);
		greatPowers.push_back(tag);
		if (auto country = countries.find(tag); country != countries.end())
		{
			country->second->setAsGreatNation();
		}
	}
}


void Vic2::World::setProvinceOwners()
{
	for (auto province: provinces)
	{
		if (province.second->getOwnerString() == "")
		{
			continue;
		}

		if (auto country = countries.find(province.second->getOwnerString()); country != countries.end())
		{
			country->second->addProvince(province);
			province.second->setOwner(country->second);
		}
		else
		{
			LOG(LogLevel::Warning) << "Trying to set " << province.second->getOwnerString() << " as owner of " << province.first << ", but country does not exist.";
		}
	}
	for (auto country: countries)
	{
		country.second->putProvincesInStates();
	}
}


void Vic2::World::addProvinceCoreInfoToCountries()
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


void Vic2::World::removeSimpleLandlessNations()
{
	for (auto country: countries)
	{
		if (country.second->getProvinces().size() > 0)
		{
			continue;
		}

		std::vector<Vic2::Province*> coresToKeep;
		for (auto core: country.second->getCores())
		{
			if (shouldCoreBeRemoved(core, country.second))
			{
				core->removeCoreString(country.first);
				core->removeCore(country.second);
			}
			else
			{
				coresToKeep.push_back(core);
			}
		}
		country.second->replaceCores(coresToKeep);

		if (!country.second->hasCoreOnCapital())
		{
			std::vector<Province*> emptyCores;
			country.second->replaceCores(emptyCores);
		}
	}
}


bool Vic2::World::shouldCoreBeRemoved(const Province* core, const Country* country) const
{
	if (core->getOwner() == nullptr)
	{
		return true;
	}
	else if (country->getPrimaryCulture() == core->getOwner()->getPrimaryCulture())
	{
		return true;
	}
	else if (core->getOwner()->isAnAcceptedCulture(country->getPrimaryCulture()))
	{
		return true;
	}
	else if (core->getPercentageWithCultures(country->getAcceptedCultures()) < 0.25)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void Vic2::World::determineEmployedWorkers()
{
	for (auto country: countries)
	{
		country.second->determineEmployedWorkers();
	}
}


void Vic2::World::removeEmptyNations()
{
	map<string, Country*> newCountries;

	for (auto country: countries)
	{
		if (!country.second->isEmpty())
		{
			newCountries.insert(country);
		}
	}

	countries.swap(newCountries);
}


void Vic2::World::determinePartialStates()
{
	for (auto country: countries)
	{
		for (auto state: country.second->getStates())
		{
			state->determineIfPartialState();
		}
	}
}


void Vic2::World::readCountryFiles()
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
			exit(-1);
		}
	}
}


bool Vic2::World::processCountriesDotTxt(const string& countryListFile, const string& mod)
{
	ifstream V2CountriesInput(countryListFile);
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

		string tag = line.substr(0, 3);
		string countryFileName = extractCountryFileName(line);
		commonCountryData countryData(countryFileName, mod);
		if (countries.find(tag) != countries.end())
		{
			countries[tag]->setColor(countryData.getColor());
			countries[tag]->setShipNames(countryData.getUnitNames());
		}
		for (auto party: countryData.getParties())
		{
			parties.emplace_back(party);
		}
	}

	V2CountriesInput.close();
	return true;
}


bool Vic2::World::shouldLineBeSkipped(const string& line) const
{
	return ((line[0] == '#') || (line.size() < 3) || (line.substr(0, 12) == "dynamic_tags"));
}


string Vic2::World::extractCountryFileName(const string& countryFileLine) const
{
	string countryFileName;
	int start = countryFileLine.find_first_of('/');
	int size = countryFileLine.find_last_of('\"') - start;
	countryFileName = countryFileLine.substr(start, size);

	return countryFileName;
}


void Vic2::World::overallMergeNations()
{
	LOG(LogLevel::Info) << "Merging nations";
	MergeRules theMergeRules;
	for (auto rule: theMergeRules.getRules())
	{
		mergeNations(rule.first, rule.second);
	}
}


void Vic2::World::mergeNations(const string& masterTag, const vector<string>& slaveTags)
{
	auto master = getCountry(masterTag);
	if (master)
	{
		for (auto slaveTag: slaveTags)
		{
			auto slave = getCountry(slaveTag);
			if (slave)
			{
				(*master)->eatCountry(*slave);
			}
			countries.erase(slaveTag);
		}
	}
}


optional<Vic2::Country*> Vic2::World::getCountry(const string& tag) const
{
	auto countryItr = countries.find(tag);
	if (countryItr != countries.end())
	{
		return countryItr->second;
	}
	else
	{
		return {};
	}
}


void Vic2::World::setLocalisations()
{
	for (auto country: countries)
	{
		country.second->setLocalisationNames();
		country.second->setLocalisationAdjectives();
	}
}


void Vic2::World::handleMissingCountryCultures()
{
	for (auto country: countries)
	{
		country.second->handleMissingCulture();
	}
}


std::optional<const Vic2::Province*> Vic2::World::getProvince(int provNum) const
{
	auto provinceItr = provinces.find(provNum);
	if (provinceItr != provinces.end())
	{
		return provinceItr->second;
	}
	else
	{
		return {};
	}
}


void Vic2::World::checkAllProvincesMapped() const
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