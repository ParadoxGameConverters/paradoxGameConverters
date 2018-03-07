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



#include "CountryMapping.h"
#include <algorithm>
#include <iomanip>
#include "../Configuration.h"
#include "../EU4World/ColonialRegions.h"
#include "../EU4World/CultureGroups.h"
#include "../EU4World/EU4World.h"
#include "../EU4World/EU4Country.h"
#include "../EU4World/EU4Province.h"
#include "CK2TitleMapper.h"
#include "ProvinceMapper.h"
#include "../V2World/Vic2Regions.h"
#include "../V2World/V2Country.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



mappers::CountryMappings* mappers::CountryMappings::instance = nullptr;



mappers::CountryMapping::CountryMapping(std::istream& theStream)
{
	registerKeyword(std::regex("EU4"), [this](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto possibleEU4Tag = getNextToken(theStream);
			if (possibleEU4Tag)
			{
				EU4Tag = *possibleEU4Tag;
				std::transform(EU4Tag.begin(), EU4Tag.end(), EU4Tag.begin(), ::toupper);
			}
		}
	);
	registerKeyword(std::regex("Vic2"), [this](const std::string& unused, std::istream& theStream)
	{
		auto equals = getNextToken(theStream);
		auto possibleVic2Tag = getNextToken(theStream);
		if (possibleVic2Tag)
		{
			Vic2Tag = *possibleVic2Tag;
			std::transform(Vic2Tag.begin(), Vic2Tag.end(), Vic2Tag.begin(), ::toupper);
		}
	}
	);
	
	parseStream(theStream);
}


mappers::CountryMappings::CountryMappings()
{
	LOG(LogLevel::Info) << "Getting country mappings";
	readRules();
	getAvailableFlags();
}


void mappers::CountryMappings::readRules()
{
	LOG(LogLevel::Info) << "Reading country mapping rules";

	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream& theStream)
		{
			CountryMapping newMapping(theStream);
			EU4TagToV2TagsRules.insert(make_pair(newMapping.getEU4Tag(), newMapping.getVic2Tag()));
		}
	);

	parseFile("country_mappings.txt");
}


void mappers::CountryMappings::getAvailableFlags()
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


void mappers::CountryMappings::CreateMappings(const EU4::world& srcWorld, const map<string, V2Country*>& Vic2Countries)
{
	LOG(LogLevel::Info) << "Creating country mappings";

	generatedV2TagPrefix = 'X';
	generatedV2TagSuffix = 0;

	set<std::shared_ptr<EU4::Country>> colonialCountries;
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


bool mappers::CountryMappings::isPotentialColonialReplacement(const pair<string, std::shared_ptr<EU4::Country>>& country)
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


bool mappers::CountryMappings::tagIsAlphaDigitDigit(const string& tag)
{
	return (isalpha(tag[0]) && isdigit(tag[1]) && isdigit(tag[2]));
}


void mappers::CountryMappings::makeOneMapping(std::shared_ptr<EU4::Country> country, const map<string, V2Country*>& Vic2Countries)
{
	string EU4Tag = country->getTag();

	auto mappingRule = EU4TagToV2TagsRules.find(EU4Tag);
	mappingRule = ifValidGetCK2MappingRule(country, mappingRule);

	bool mapped = false;
	if ((mappingRule != EU4TagToV2TagsRules.end()) && (!country->isCustom()))
	{
		mapped = mapToExistingVic2Country(mappingRule->second, Vic2Countries, EU4Tag);
		if (!mapped)
		{
			mapped = mapToFirstUnusedVic2Tag(mappingRule->second, EU4Tag);
		}
	}

	if (!mapped)
	{
		string newVic2Tag = generateNewTag();
		mapToNewTag(EU4Tag, newVic2Tag);
	}
}


bool mappers::CountryMappings::mapToExistingVic2Country(const string& possibleVic2Tag, const map<string, V2Country*>& Vic2Countries, const string& EU4Tag)
{
	if ((Vic2Countries.find(possibleVic2Tag) != Vic2Countries.end()) && (!tagIsAlreadyAssigned(possibleVic2Tag)))
	{
		EU4TagToV2TagMap.insert(make_pair(EU4Tag, possibleVic2Tag));
		V2TagToEU4TagMap.insert(make_pair(possibleVic2Tag, EU4Tag));
		logMapping(EU4Tag, possibleVic2Tag, "default V2 country");

		return true;
	}

	return false;
}


bool mappers::CountryMappings::mapToFirstUnusedVic2Tag(const string& possibleVic2Tag, const string& EU4Tag)
{
	if (!tagIsAlreadyAssigned(possibleVic2Tag))
	{
		EU4TagToV2TagMap.insert(make_pair(EU4Tag, possibleVic2Tag));
		V2TagToEU4TagMap.insert(make_pair(possibleVic2Tag, EU4Tag));
		logMapping(EU4Tag, possibleVic2Tag, "mapping rule, not a V2 country");

		return true;
	}

	return false;
}


string mappers::CountryMappings::generateNewTag()
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


void mappers::CountryMappings::mapToNewTag(const string& EU4Tag, const string& Vic2Tag)
{
	EU4TagToV2TagMap.insert(make_pair(EU4Tag, Vic2Tag));
	V2TagToEU4TagMap.insert(make_pair(Vic2Tag, EU4Tag));
	logMapping(EU4Tag, Vic2Tag, "generated tag");
}


map<string, string>::iterator mappers::CountryMappings::ifValidGetCK2MappingRule(const std::shared_ptr<EU4::Country> country, map<string, string>::iterator mappingRule)
{
	if ((mappingRule == EU4TagToV2TagsRules.end()) || (country->isCustom()))
	{
		auto CK2Title = GetCK2Title(country->getTag(), country->getName("english"), availableFlags);
		if (CK2Title)
		{
			std::transform(CK2Title->begin(), CK2Title->end(), CK2Title->begin(), ::toupper);
			mappingRule = EU4TagToV2TagsRules.find(*CK2Title);
		}
	}

	return mappingRule;
}


bool mappers::CountryMappings::attemptColonialReplacement(std::shared_ptr<EU4::Country> country, const EU4::world& srcWorld, const map<string, V2Country*>& Vic2Countries)
{
	bool mapped = false;

	int Vic2Capital;
	int EU4Capital = country->getCapital();
	auto potentialVic2Capitals = provinceMapper::getVic2ProvinceNumbers(EU4Capital);
	if (potentialVic2Capitals.size() > 0)
	{
		Vic2Capital = potentialVic2Capitals[0];
	}

	for (auto colony: mappers::colonialTagMapper::getColonyMap())
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


bool mappers::CountryMappings::capitalInRightEU4Region(const mappers::colonyStruct& colony, int EU4Capital)
{
	if (colony.EU4Region != "")
	{
		return EU4::colonialRegions::provinceIsInRegion(EU4Capital, colony.EU4Region);
	}
	else
	{
		return true;
	}
}


bool mappers::CountryMappings::capitalInRightVic2Region(const mappers::colonyStruct& colony, int Vic2Capital, const EU4::world& srcWorld, const string& EU4Tag)
{
	if (colony.V2Region != "")
	{
		if (Vic2::regions::provinceIsInRegion(Vic2Capital, colony.V2Region))
		{
			return true;
		}
		else
		{
			for (auto Vic2ProvinceNumber: Vic2::regions::getProvincesInRegion(colony.V2Region))
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


bool mappers::CountryMappings::inCorrectCultureGroup(const mappers::colonyStruct& colony, const string& primaryCulture)
{
	if (colony.cultureGroup != "")
	{
		auto culturalGroup = EU4::cultureGroups::getCulturalGroup(primaryCulture);
		if ((culturalGroup) && (culturalGroup->getName() != colony.cultureGroup))
		{
			return false;
		}
	}

	return true;
}


bool mappers::CountryMappings::tagIsAvailable(const mappers::colonyStruct& colony, const map<string, V2Country*>& Vic2Countries)
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


void mappers::CountryMappings::logMapping(const string& EU4Tag, const string& V2Tag, const string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << EU4Tag << " -> " << V2Tag << " (" << reason << ')';
}


bool mappers::CountryMappings::tagIsAlreadyAssigned(const string& Vic2Tag)
{
	return (V2TagToEU4TagMap.find(Vic2Tag) != V2TagToEU4TagMap.end());
}


const string mappers::CountryMappings::GetV2Tag(const string& EU4Tag) const
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


std::optional<std::string> mappers::CountryMappings::GetCK2Title(const string& EU4Tag, const string& countryName, const set<string>& availableFlags)
{
	if (!tagIsAlphaDigitDigit(EU4Tag))
	{
		return {};
	}

	string name = V2Localisation::Convert(countryName);
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	auto ck2title = mappers::CK2TitleMapper::getTitle(name);
	if (!ck2title)
	{
		string titlename = V2Localisation::StripAccents(name);
		string c_name = "c_" + titlename;
		string d_name = "d_" + titlename;
		string k_name = "k_" + titlename;

		if (mappers::CK2TitleMapper::doesTitleExist(c_name))
		{
			ck2title = c_name;
		}
		else if (mappers::CK2TitleMapper::doesTitleExist(d_name))
		{
			ck2title = d_name;
		}
		else if (mappers::CK2TitleMapper::doesTitleExist(k_name))
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

	if (ck2title)
	{
		LOG(LogLevel::Debug) << "Country " << EU4Tag << " (" << name << ") has the CK2 title " << *ck2title;
	}

	return ck2title;
}