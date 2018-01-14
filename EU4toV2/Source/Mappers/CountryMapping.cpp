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



#include "CountryMapping.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "../Configuration.h"
#include "../EU4World/EU4World.h"
#include "../EU4World/EU4Country.h"
#include "../EU4World/EU4Province.h"
#include "../Mappers/CK2TitleMapper.h"
#include "../Mappers/EU4CultureGroupMapper.h"
#include "../Mappers/ProvinceMapper.h"
#include "../V2World/V2Country.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "ParadoxParser8859_15.h"



CountryMapping* CountryMapping::instance = NULL;


CountryMapping::CountryMapping()
{
	LOG(LogLevel::Info) << "Getting country mappings";
	readRules();
	readEU4Regions();
	readVic2Regions();
	getAvailableFlags();
}


void CountryMapping::readRules()
{
	LOG(LogLevel::Info) << "Reading country mapping rules";
	vector<shared_ptr<Object>> ruleNodes = getRules();
	for (auto rule: ruleNodes)
	{
		importRule(rule);
	}
}


vector<shared_ptr<Object>> CountryMapping::getRules()
{
	shared_ptr<Object> countryMappingsFile = parser_UTF8::doParseFile("country_mappings.txt");
	if (!countryMappingsFile)
	{
		LOG(LogLevel::Error) << "Failed to parse country_mappings.txt";
		exit(-1);
	}
	vector<shared_ptr<Object>> nodes = countryMappingsFile->getLeaves();
	if (nodes.empty())
	{
		LOG(LogLevel::Error) << "country_mappings.txt does not contain a mapping";
		exit(-1);
	}

	return nodes[0]->getLeaves();
}


void CountryMapping::importRule(shared_ptr<Object> rule)
{
	vector<shared_ptr<Object>> ruleItems = rule->getLeaves();

	string newEU4Tag;
	vector<string>	V2Tags;
	for (auto item: ruleItems)
	{
		string key = item->getKey();
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);
		if (key == "EU4")
		{
			newEU4Tag = item->getLeaf();
			std::transform(newEU4Tag.begin(), newEU4Tag.end(), newEU4Tag.begin(), ::toupper);
		}
		else if (key == "V2")
		{
			string V2Tag = item->getLeaf();
			std::transform(V2Tag.begin(), V2Tag.end(), V2Tag.begin(), ::toupper);
			V2Tags.push_back(V2Tag);
		}
		else
		{
			LOG(LogLevel::Warning) << "Ignoring unknown key '" << key << "' while mapping countries";
		}
	}
	EU4TagToV2TagsRules.insert(make_pair(newEU4Tag, V2Tags));
}


void CountryMapping::readEU4Regions()
{
	LOG(LogLevel::Info) << "Parsing EU4 colonial regions";

	auto EU4RegionsObjs = parseEU4RegionsFiles();
	for (auto EU4RegionsObj: EU4RegionsObjs)
	{
		for (auto regionsObj: EU4RegionsObj->getLeaves())
		{
			auto provincesObj = regionsObj->getValue("provinces");
			if (provincesObj.size() > 0)
			{
				vector<string> provinceStrings = provincesObj[0]->getTokens();
				set<int> provinces;
				for (auto provinceString: provinceStrings)
				{
					provinces.insert(stoi(provinceString));
				}
				EU4ColonialRegions.insert(make_pair(regionsObj->getKey(), provinces));
			}
		}
	}
}


vector<shared_ptr<Object>> CountryMapping::parseEU4RegionsFiles()
{
	vector<shared_ptr<Object>> colonialRegionsObjs;

	shared_ptr<Object> colonialRegionsObj = parser_UTF8::doParseFile((Configuration::getEU4Path() + "/common/colonial_regions/00_colonial_regions.txt").c_str());
	if (colonialRegionsObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << Configuration::getEU4Path() << "/common/colonial_regions/00_colonial_regions.txt";
		exit(-1);
	}
	if (colonialRegionsObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse 00_colonial_regions.txt";
		exit(-1);
	}
	colonialRegionsObjs.push_back(colonialRegionsObj);

	for (auto itr: Configuration::getEU4Mods())
	{
		set<string> filenames;
		Utils::GetAllFilesInFolder(itr + "/common/colonial_regions/", filenames);
		for (auto filename: filenames)
		{
			string modRegionsFile(itr + "/common/colonial_regions/" + filename);
			if (Utils::DoesFileExist(modRegionsFile))
			{
				colonialRegionsObj = parser_UTF8::doParseFile(modRegionsFile.c_str());
				if (colonialRegionsObj == NULL)
				{
					LOG(LogLevel::Error) << "Could not parse file " << modRegionsFile;
					exit(-1);
				}
				colonialRegionsObjs.push_back(colonialRegionsObj);
			}
		}
	}

	return colonialRegionsObjs;
}


void CountryMapping::readVic2Regions()
{
	LOG(LogLevel::Info) << "Parsing Vic2 regions";

	auto Vic2RegionsObj = parseVic2RegionsFile();
	for (auto regionsObj: Vic2RegionsObj->getLeaves())
	{
		vector<string> provinceStrings = regionsObj->getTokens();
		set<int> provinces;
		for (auto provinceString: provinceStrings)
		{
			provinces.insert(stoi(provinceString));
		}
		Vic2Regions.insert(make_pair(regionsObj->getKey(), provinces));
	}
}


shared_ptr<Object> CountryMapping::parseVic2RegionsFile()
{
	shared_ptr<Object> Vic2RegionsObj;
	if (Utils::DoesFileExist("./blankMod/output/map/region.txt"))
	{
		Vic2RegionsObj = parser_8859_15::doParseFile("./blankMod/output/map/region.txt");
		if (Vic2RegionsObj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file ./blankMod/output/map/region.txt";
			exit(-1);
		}
	}
	else
	{
		Vic2RegionsObj = parser_8859_15::doParseFile( (Configuration::getV2Path() + "/map/region.txt").c_str() );
		if (Vic2RegionsObj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file " << Configuration::getV2Path() << "/map/region.txt";
			exit(-1);
		}
	}

	return Vic2RegionsObj;
}


void CountryMapping::getAvailableFlags()
{
	const vector<string> availableFlagFolders = { "blankMod/output/gfx/flags", Configuration::getV2Path() + "/gfx/flags" };

	set<string> availableFlagFiles;
	for (auto availableFlagFolder: availableFlagFolders)
	{
		Utils::GetAllFilesInFolder(availableFlagFolder, availableFlagFiles);
	}

	for (auto file: availableFlagFiles)
	{
		size_t lastdot = file.find_last_of(".");
		if (lastdot != string::npos)
		{
			availableFlags.insert(file.substr(0, lastdot)); 
		}
	}
}


void CountryMapping::CreateMappings(const EU4World& srcWorld, const map<string, V2Country*>& Vic2Countries)
{
	LOG(LogLevel::Info) << "Creating country mappings";

	generatedV2TagPrefix = 'X';
	generatedV2TagSuffix = 0;

	set<EU4Country*> colonialCountries;
	for (auto EU4Country: srcWorld.getCountries())
	{
		if (isPotentialColonialReplacement(EU4Country))
		{
			colonialCountries.insert(EU4Country.second);
		}
		else
		{
			makeOneMapping(EU4Country.second, Vic2Countries);
		}
	}

	for (auto colonialCountry: colonialCountries)
	{
		bool success = attemptColonialReplacement(colonialCountry, srcWorld, Vic2Countries);
		if (!success)
		{
			makeOneMapping(colonialCountry, Vic2Countries);
		}
	}
}


bool CountryMapping::isPotentialColonialReplacement(const pair<string, EU4Country*>& country)
{
	if (country.second->isColony() && tagIsAlphaDigitDigit(country.first))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CountryMapping::tagIsAlphaDigitDigit(const string& tag)
{
	return (isalpha(tag[0]) && isdigit(tag[1]) && isdigit(tag[2]));
}


void CountryMapping::makeOneMapping(EU4Country* country, const map<string, V2Country*>& Vic2Countries)
{
	string EU4Tag = country->getTag();

	auto mappingRule = EU4TagToV2TagsRules.find(EU4Tag);
	mappingRule = ifValidGetCK2MappingRule(country, mappingRule);

	bool mapped = false;
	if ((mappingRule != EU4TagToV2TagsRules.end()) && (!country->isCustom()))
	{
		auto possibleVic2Tags = mappingRule->second;

		mapped = mapToExistingVic2Country(possibleVic2Tags, Vic2Countries, EU4Tag);
		if (!mapped)
		{
			mapped = mapToFirstUnusedVic2Tag(possibleVic2Tags, EU4Tag);
		}
	}

	if (!mapped)
	{
		string newVic2Tag = generateNewTag();
		mapToNewTag(EU4Tag, newVic2Tag);
	}
}


bool CountryMapping::mapToExistingVic2Country(const vector<string>& possibleVic2Tags, const map<string, V2Country*>& Vic2Countries, const string& EU4Tag)
{
	for (auto possibleVic2Tag: possibleVic2Tags)
	{
		if ((Vic2Countries.find(possibleVic2Tag) != Vic2Countries.end()) && (!tagIsAlreadyAssigned(possibleVic2Tag)))
		{
			EU4TagToV2TagMap.insert(make_pair(EU4Tag, possibleVic2Tag));
			V2TagToEU4TagMap.insert(make_pair(possibleVic2Tag, EU4Tag));
			logMapping(EU4Tag, possibleVic2Tag, "default V2 country");

			return true;
		}
	}

	return false;
}


bool CountryMapping::mapToFirstUnusedVic2Tag(const vector<string>& possibleVic2Tags, const string& EU4Tag)
{
	for (auto possibleVic2Tag: possibleVic2Tags)
	{
		if (!tagIsAlreadyAssigned(possibleVic2Tag))
		{
			EU4TagToV2TagMap.insert(make_pair(EU4Tag, possibleVic2Tag));
			V2TagToEU4TagMap.insert(make_pair(possibleVic2Tag, EU4Tag));
			logMapping(EU4Tag, possibleVic2Tag, "mapping rule, not a V2 country");

			return true;
		}
	}

	return false;
}


string CountryMapping::generateNewTag()
{
	ostringstream generatedV2TagStream;
	generatedV2TagStream << generatedV2TagPrefix << setfill('0') << setw(2) << generatedV2TagSuffix;
	string Vic2Tag = generatedV2TagStream.str();

	++generatedV2TagSuffix;
	if (generatedV2TagSuffix > 99)
	{
		generatedV2TagSuffix = 0;
		--generatedV2TagPrefix;
	}

	return Vic2Tag;
}


void CountryMapping::mapToNewTag(const string& EU4Tag, const string& Vic2Tag)
{
	EU4TagToV2TagMap.insert(make_pair(EU4Tag, Vic2Tag));
	V2TagToEU4TagMap.insert(make_pair(Vic2Tag, EU4Tag));
	logMapping(EU4Tag, Vic2Tag, "generated tag");
}


map<string, vector<string>>::iterator CountryMapping::ifValidGetCK2MappingRule(const EU4Country* country, map<string, vector<string>>::iterator mappingRule)
{
	if ((mappingRule == EU4TagToV2TagsRules.end()) || (country->isCustom()))
	{
		string CK2Title = GetCK2Title(country->getTag(), country->getName("english"), availableFlags);
		if (CK2Title != "")
		{
			std::transform(CK2Title.begin(), CK2Title.end(), CK2Title.begin(), ::toupper);
			mappingRule = EU4TagToV2TagsRules.find(CK2Title);
		}
	}

	return mappingRule;
}


bool CountryMapping::attemptColonialReplacement(EU4Country* country, const EU4World& srcWorld, const map<string, V2Country*>& Vic2Countries)
{
	bool mapped = false;

	int Vic2Capital;
	int EU4Capital = country->getCapital();
	auto potentialVic2Capitals = provinceMapper::getVic2ProvinceNumbers(EU4Capital);
	if (potentialVic2Capitals.size() > 0)
	{
		Vic2Capital = potentialVic2Capitals[0];
	}

	for (auto colony: colonialTagMapper::getColonyMap())
	{
		if (!capitalInRightEU4Region(colony, EU4Capital))
		{
			continue;
		}
		country->setColonialRegion(colony.EU4Region);

		if (!capitalInRightVic2Region(colony, Vic2Capital, srcWorld, country->getTag()))
		{
			continue;
		}

		if (!inCorrectCultureGroup(colony, country->getPrimaryCulture()))
		{
			continue;
		}

		if (tagIsAvailable(colony, Vic2Countries))
		{
			EU4TagToV2TagMap.insert(make_pair(country->getTag(), colony.tag));
			V2TagToEU4TagMap.insert(make_pair(colony.tag, country->getTag()));
			logMapping(country->getTag(), colony.tag, "colonial replacement");
			return true;
		}
	}

	return false;
}


bool CountryMapping::capitalInRightEU4Region(const colonyStruct& colony, int EU4Capital)
{
	if (colony.EU4Region != "")
	{
		auto EU4Region = EU4ColonialRegions.find(colony.EU4Region);
		if (EU4Region == EU4ColonialRegions.end())
		{
			LOG(LogLevel::Warning) << "Unknown EU4 Colonial Region " << colony.EU4Region;
			return false;
		}
		else if (EU4Region->second.find(EU4Capital) == EU4Region->second.end())
		{
			return false;
		}
	}

	return true;
}


bool CountryMapping::capitalInRightVic2Region(const colonyStruct& colony, int Vic2Capital, const EU4World& srcWorld, const string& EU4Tag)
{
	if (colony.V2Region != "")
	{
		auto Vic2Region = Vic2Regions.find(colony.V2Region);
		if (Vic2Region == Vic2Regions.end())
		{
			LOG(LogLevel::Warning) << "Unknown V2 Region " << colony.V2Region;
			return false;
		}
		else if (Vic2Region->second.find(Vic2Capital) == Vic2Region->second.end())
		{
			for (auto Vic2ProvinceNumber: Vic2Region->second)
			{
				auto EU4ProvinceNumbers = provinceMapper::getEU4ProvinceNumbers(Vic2ProvinceNumber);
				if (EU4ProvinceNumbers.size() > 0)
				{
					return false;
				}
				for (auto EU4ProvinceNumber: EU4ProvinceNumbers)
				{
					const EU4Province* province = srcWorld.getProvince(EU4ProvinceNumber);
					if ((province == NULL) || (province->getOwnerString() != EU4Tag))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}


bool CountryMapping::inCorrectCultureGroup(const colonyStruct& colony, const string& primaryCulture)
{
	if (colony.cultureGroup != "")
	{
		if (EU4CultureGroupMapper::getCulturalGroup(primaryCulture) != colony.cultureGroup)
		{
			return false;
		}
	}

	return true;
}


bool CountryMapping::tagIsAvailable(const colonyStruct& colony, const map<string, V2Country*>& Vic2Countries)
{
	if (Vic2Countries.find(colony.tag) == Vic2Countries.end())
	{
		return false;
	}
	if (tagIsAlreadyAssigned(colony.tag))
	{
		return false;
	}

	return true;
}


void CountryMapping::logMapping(const string& EU4Tag, const string& V2Tag, const string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << EU4Tag << " -> " << V2Tag << " (" << reason << ')';
}


bool CountryMapping::tagIsAlreadyAssigned(const string& Vic2Tag)
{
	return (V2TagToEU4TagMap.find(Vic2Tag) != V2TagToEU4TagMap.end());
}


const string CountryMapping::GetV2Tag(const string& EU4Tag) const
{
	const vector<string> EU4RebelTags = { "REB", "PIR", "NAT" };
	static const string V2RebelTag = "REB";
	if (find(EU4RebelTags.begin(), EU4RebelTags.end(), EU4Tag) != EU4RebelTags.end())
	{
		return V2RebelTag;
	}

	auto findIter = EU4TagToV2TagMap.find(EU4Tag);
	if (findIter != EU4TagToV2TagMap.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}


string CountryMapping::GetCK2Title(const string& EU4Tag, const string& countryName, const set<string>& availableFlags)
{
	if (!tagIsAlphaDigitDigit(EU4Tag))
	{
		return "";
	}

	string name = V2Localisation::Convert(countryName);
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	auto ck2title = CK2TitleMapper::getTitle(name);
	if (ck2title == "")
	{
		string titlename = V2Localisation::StripAccents(name);
		string c_name = "c_" + titlename;
		string d_name = "d_" + titlename;
		string k_name = "k_" + titlename;

		if (CK2TitleMapper::doesTitleExist(c_name))
		{
			ck2title = c_name;
		}
		else if (CK2TitleMapper::doesTitleExist(d_name))
		{
			ck2title = d_name;
		}
		else if (CK2TitleMapper::doesTitleExist(k_name))
		{
			ck2title = k_name;
		}
		else
		{
			// I've found titles that don't exist in the ck2 name mapping, but do exist in the flagset (c_znojmo).
			if (availableFlags.find("k_" + titlename) != availableFlags.end())
			{
				LOG(LogLevel::Debug) << "Country " << EU4Tag << " (" << name << ") has the CK2 title k_" << titlename;
				return k_name;
			}
			else if (availableFlags.find(d_name) != availableFlags.end())
			{
				LOG(LogLevel::Debug) << "Country " << EU4Tag << " (" << name << ") has the CK2 title " << d_name;
				return d_name;
			}
			else if (availableFlags.find(c_name) != availableFlags.end())
			{
				LOG(LogLevel::Debug) << "Country " << EU4Tag << " (" << name << ") has the CK2 title " << c_name;
				return c_name;
			}
		}
	}

	if (ck2title != "")
	{
		LOG(LogLevel::Debug) << "Country " << EU4Tag << " (" << name << ") has the CK2 title " << ck2title;
		return ck2title;
	}

	return ""; 
}