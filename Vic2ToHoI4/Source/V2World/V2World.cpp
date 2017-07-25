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



V2World::V2World(const string& filename)
{
	LOG(LogLevel::Info) << "* Importing V2 save *";

	LOG(LogLevel::Info) << "Parsing save";
	shared_ptr<Object> obj = parser_8859_15::doParseFile(filename);
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << filename << ". File is likely missing.";
		exit(-1);
	}

	LOG(LogLevel::Info) << "Building world";

	provinces.clear();
	countries.clear();
	parties.clear();
	greatPowers.clear();

	map<int, int> countryIndexToGPRank = extractGreatNationIndices(obj);
	unsigned int countryIndex = 1; // Starts from 1 at REB

	vector<shared_ptr<Object>> leaves = obj->getLeaves();
	for (auto leaf: leaves)
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
	removeSimpleLandlessNations();
	determineEmployedWorkers();
	removeEmptyNations();
	determinePartialStates();
	inputDiplomacy(obj->getValue("diplomacy"));
	readCountryFiles();
	setLocalisations();

	CountryMapper::createMappings(this);

	overallMergeNations();
	checkAllProvincesMapped();
}


map<int, int> V2World::extractGreatNationIndices(const shared_ptr<Object> obj)
{
	map<int, int> countryIndexToGPRank;

	vector<shared_ptr<Object>> greatNationsObj = obj->getValue("great_nations");
	if (greatNationsObj.size() > 0)
	{
		vector<string> greatNations = greatNationsObj[0]->getTokens();
		for (unsigned int i = 0; i < greatNations.size(); i++)
		{
			countryIndexToGPRank.insert(make_pair(stoi(greatNations[i]), i));
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
	return (isupper(key[0]) && isupper(key[1]) && isupper(key[2]));
}


bool V2World::isDominionCountryKey(string key) const
{
	return ((key[0] == 'D') && isdigit(key[1]) && isdigit(key[2]));
}


bool V2World::isConvertedCountryKey(string key) const
{
	return (isupper(key[0]) && isdigit(key[1]) && isdigit(key[2]));
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


bool V2World::shouldCoreBeRemoved(const V2Province* core, const V2Country* country)
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


void V2World::inputDiplomacy(const vector<shared_ptr<Object>> diplomacyObj)
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


shared_ptr<Object> V2World::readCountryFile(string countryFileName, string mod) const
{
	shared_ptr<Object> countryData = NULL;
	if (mod != "")
	{
		string file = Configuration::getV2Path() + "/mod/" + mod + "/common/countries/" + countryFileName;
		if (Utils::DoesFileExist(file))
		{
			countryData = parser_8859_15::doParseFile(file);
			if (countryData == NULL)
			{
				LOG(LogLevel::Warning) << "Could not parse file " << file;
			}
		}
	}
	if (countryData == NULL)
	{
		string file = Configuration::getV2Path() +  "/common/countries/" + countryFileName;
		if (Utils::DoesFileExist(file))
		{
			countryData = parser_8859_15::doParseFile(file);
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


void V2World::readCountryColor(const shared_ptr<Object> countryData, string line)
{
	string tag = line.substr(0, 3);
	vector<shared_ptr<Object>> colorObj = countryData->getValue("color");
	if (colorObj.size() > 0)
	{
		vector<string> rgb = colorObj[0]->getTokens();
		if (rgb.size() == 3)
		{
			if (countries.find(tag) != countries.end())
			{
				countries[tag]->setColor(Color(stoi(rgb[0]), stoi(rgb[1]), stoi(rgb[2])));
			}
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
	shared_ptr<Object> mergeObj = parser_UTF8::doParseFile("merge_nations.txt");
	if (mergeObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file merge_nations.txt";
		exit(-1);
	}

	vector<shared_ptr<Object>> rules = mergeObj->getValue("merge_nations");	// all merging rules
	if (rules.size() < 0)
	{
		LOG(LogLevel::Debug) << "No nations have merging requested (skipping)";
		return;
	}

	rules = rules[0]->getLeaves();	// the rules themselves
	for (auto rule: rules)
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


void V2World::setLocalisations()
{
	for (auto country: countries)
	{
		country.second->setLocalisationNames();
		country.second->setLocalisationAdjectives();
	}
}


const V2Province* V2World::getProvince(int provNum) const
{
	auto provinceItr = provinces.find(provNum);
	return (provinceItr != provinces.end()) ? provinceItr->second : nullptr;
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