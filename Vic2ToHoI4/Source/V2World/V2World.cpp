/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include "ParadoxParserUTF8.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "V2Country.h"
#include "V2Diplomacy.h"
#include "V2Party.h"
#include "V2Province.h"
#include "Vic2State.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/ProvinceMapper.h"



V2World::V2World(const string& filename):
	provinces(),
	countries(),
	diplomacy(nullptr),
	parties(),
	greatPowers()
{
	LOG(LogLevel::Info) << "* Importing V2 save *";

	LOG(LogLevel::Info) << "Parsing save";
	auto obj = parser_8859_15::doParseFile(filename);
	if (obj)
	{
		LOG(LogLevel::Info) << "Building world";

		auto countryIndexToGPRank = extractGreatNationIndices(obj);
		greatPowers.resize(countryIndexToGPRank.size());
		unsigned int countryIndex = 1; // Starts from 1 at REB

		for (auto leaf: obj->getLeaves())
		{
			string key = leaf->getKey();
			if (isProvinceKey(key))
			{
				provinces[stoi(key)] = new V2Province(leaf);
			}
			else if (isCountryKey(key))
			{
				countries[key] = new V2Country(leaf);
				setGreatPowerStatus(key, countryIndexToGPRank, countryIndex);
				countryIndex++;
			}
		}

		setProvinceOwners();
		addProvinceCoreInfoToCountries();
		if (Configuration::getRemoveCores())
		{
			removeSimpleLandlessNations();
		}
		determineEmployedWorkers();
		removeEmptyNations();
		determinePartialStates();
		inputDiplomacy(obj->safeGetObject("diplomacy"));
		readCountryFiles();
		setLocalisations();
		handleMissingCountryCultures();

		CountryMapper::createMappings(this);

		overallMergeNations();
		checkAllProvincesMapped();
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse file " << filename << ". File is likely missing.";
		exit(-1);
	}
}


map<int, int> V2World::extractGreatNationIndices(const shared_ptr<Object> obj) const
{
	map<int, int> countryIndexToGPRank;

	auto greatNations = obj->safeGetTokens("great_nations");
	for (unsigned int i = 0; i < greatNations.size(); i++)
	{
		countryIndexToGPRank.insert(make_pair(stoi(greatNations[i]), i));
	}

	return countryIndexToGPRank;
}


bool V2World::isProvinceKey(const string& key) const
{
	return ((!key.empty()) && (key.find_first_not_of("0123456789") == string::npos));
}


bool V2World::isCountryKey(const string& key) const
{
	return
	(
		(key.size() == 3) &&
		(isNormalCountryKey(key) || isDominionCountryKey(key) || isConvertedCountryKey(key))
	);
}


bool V2World::isNormalCountryKey(const string& key) const
{
	return (isupper(key[0]) && isupper(key[1]) && isupper(key[2]));
}


bool V2World::isDominionCountryKey(const string& key) const
{
	return ((key[0] == 'D') && isdigit(key[1]) && isdigit(key[2]));
}


bool V2World::isConvertedCountryKey(const string& key) const
{
	return (isupper(key[0]) && isdigit(key[1]) && isdigit(key[2]));
}


void V2World::setGreatPowerStatus(const string& tag, const map<int, int>& countryIndexToGPRank, unsigned int countryIndex)
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


void V2World::removeSimpleLandlessNations()
{
	for (auto country: countries)
	{
		if (country.second->getProvinces().size() > 0)
		{
			continue;
		}

		vector<V2Province*> coresToKeep;
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
			vector<V2Province*> emptyCores;
			country.second->replaceCores(emptyCores);
		}
	}
}


bool V2World::shouldCoreBeRemoved(const V2Province* core, const V2Country* country) const
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


void V2World::determineEmployedWorkers()
{
	for (auto country: countries)
	{
		country.second->determineEmployedWorkers();
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


void V2World::determinePartialStates()
{
	for (auto country: countries)
	{
		for (auto state: country.second->getStates())
		{
			state->determineIfPartialState();
		}
	}
}


void V2World::inputDiplomacy(const shared_ptr<Object>& diplomacyObj)
{
	if (diplomacyObj != nullptr)
	{
		diplomacy = new V2Diplomacy(diplomacyObj);
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
			exit(-1);
		}
	}
}


bool V2World::processCountriesDotTxt(const string& countryListFile, const string& mod)
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

		string countryFileName = extractCountryFileName(line);
		shared_ptr<Object> countryData = readCountryFile(countryFileName, mod);
		if (countryData == nullptr)
		{
			continue;
		}
		readCountryColor(countryData, line);
		inputPartyInformation(countryData->getLeaves());
	}

	V2CountriesInput.close();
	return true;
}


bool V2World::shouldLineBeSkipped(const string& line) const
{
	return ((line[0] == '#') || (line.size() < 3) || (line.substr(0, 12) == "dynamic_tags"));
}


string V2World::extractCountryFileName(const string& countryFileLine) const
{
	string countryFileName;
	int start = countryFileLine.find_first_of('/');
	int size = countryFileLine.find_last_of('\"') - start;
	countryFileName = countryFileLine.substr(start, size);

	return countryFileName;
}


shared_ptr<Object> V2World::readCountryFile(const string& countryFileName, const string& mod) const
{
	shared_ptr<Object> countryData = nullptr;
	if (mod != "")
	{
		string file = Configuration::getV2Path() + "/mod/" + mod + "/common/countries/" + countryFileName;
		if (Utils::DoesFileExist(file))
		{
			countryData = parser_8859_15::doParseFile(file);
			if (!countryData)
			{
				LOG(LogLevel::Warning) << "Could not parse file " << file;
			}
		}
	}
	if (!countryData)
	{
		string file = Configuration::getV2Path() +  "/common/countries/" + countryFileName;
		if (Utils::DoesFileExist(file))
		{
			countryData = parser_8859_15::doParseFile(file);
			if (!countryData)
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


void V2World::readCountryColor(shared_ptr<Object> countryData, const string& line)
{
	string tag = line.substr(0, 3);
	auto rgb = countryData->safeGetTokens("color");
	if (rgb.size() == 3)
	{
		if (countries.find(tag) != countries.end())
		{
			countries[tag]->setColor(ConverterColor::Color(stoi(rgb[0]), stoi(rgb[1]), stoi(rgb[2])));
		}
	}
}


void V2World::inputPartyInformation(const vector<shared_ptr<Object>>& leaves)
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


void V2World::overallMergeNations()
{
	LOG(LogLevel::Info) << "Merging nations";
	auto mergeObj = parser_UTF8::doParseFile("merge_nations.txt");
	if (mergeObj)
	{
		auto rules = mergeObj->safeGetObject("merge_nations");	// all merging rules
		if (rules == nullptr)
		{
			LOG(LogLevel::Debug) << "No nations have merging requested (skipping)";
			return;
		}

		for (auto rule: rules->getLeaves())
		{
			vector<shared_ptr<Object>> thisMerge = rule->getLeaves();	// the current merge rule
			string masterTag;										// the nation to merge into
			vector<string> slaveTags;								// the nations that will be merged into the master
			bool enabled = false;									// whether or not this rule is enabled
			for (auto item: thisMerge)
			{
				if (item->getKey() == "merge" && item->getLeaf() == "yes")
				{
					enabled = true;
				}
				else if (item->getKey() == "master")
				{
					masterTag = item->getLeaf();
				}
				else if (item->getKey() == "slave")
				{
					slaveTags.push_back(item->getLeaf());
				}
			}
			if (enabled)
			{
				mergeNations(masterTag, slaveTags);
			}
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse file merge_nations.txt";
		exit(-1);
	}
}


void V2World::mergeNations(const string& masterTag, const vector<string>& slaveTags)
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
		}
	}
}


optional<V2Country*> V2World::getCountry(const string& tag) const
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


void V2World::setLocalisations()
{
	for (auto country: countries)
	{
		country.second->setLocalisationNames();
		country.second->setLocalisationAdjectives();
	}
}


void V2World::handleMissingCountryCultures()
{
	for (auto country: countries)
	{
		country.second->handleMissingCulture();
	}
}


optional<const V2Province*> V2World::getProvince(int provNum) const
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