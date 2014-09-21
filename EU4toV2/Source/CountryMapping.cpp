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



#include "CountryMapping.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <utility>

#include <boost/algorithm/string.hpp>

#include "EU4World\EU4World.h"
#include "EU4World\EU4Country.h"
#include "EU4World\EU4Province.h"
#include "Parsers\Object.h"
#include "Parsers\Parser.h"
#include "V2World\V2World.h"
#include "V2World\V2Country.h"
#include "V2World\V2Province.h"
#include "Log.h"

bool CountryMapping::ReadRules(const std::string& fileName)
{
	LOG(LogLevel::Info) << "Reading country mapping rules";

	// Read the rule nodes from file.
	LOG(LogLevel::Debug) << "Parsing rules from file " << fileName;
	initParser();
	Object* countryMappingsFile = doParseFile(fileName.c_str());	// the parsed country mappings file
	if (!countryMappingsFile)
	{
		LOG(LogLevel::Error) << "Failed to parse " << fileName;
		return false;
	}
	vector<Object*> nodes = countryMappingsFile->getLeaves();	// the country mapping rules in the file
	if (nodes.empty())
	{
		LOG(LogLevel::Error) << fileName << " does not contain a mapping";
		return false;
	}
	vector<Object*> ruleNodes = nodes[0]->getLeaves();

	// Convert rule nodes into our map data structure.
	LOG(LogLevel::Debug) << "Building rules map";
	map<string, vector<string>> newEU4TagToV2TagsRules;	// the mapping rules
	for (vector<Object*>::iterator i = ruleNodes.begin(); i != ruleNodes.end(); ++i)
	{
		vector<Object*> rule = (*i)->getLeaves();	// an individual rule
		string newEU4Tag;									// the EU4 tag in the rule
		vector<string>	V2Tags;							// the V2 tags in the rule
		for (vector<Object*>::iterator j = rule.begin(); j != rule.end(); ++j)
		{
			std::string key = boost::to_upper_copy((*j)->getKey());	// the key for this part of the rule
			if (key == "EU4")
			{
				newEU4Tag = boost::to_upper_copy((*j)->getLeaf());
			}
			else if (key == "V2")
			{
				V2Tags.push_back(boost::to_upper_copy((*j)->getLeaf()));
			}
			else
			{
				LOG(LogLevel::Warning) << "Ignoring unknown key '" << key << "' while mapping countries";
			}
		}
		newEU4TagToV2TagsRules.insert(std::make_pair(newEU4Tag, V2Tags));
	}

	LOG(LogLevel::Debug) << "Finished reading country mapping rules";
	std::swap(EU4TagToV2TagsRules, newEU4TagToV2TagsRules);
	return true;
}


void CountryMapping::readEU4Regions(Object* obj)
{
	vector<Object*> regionsObj = obj->getLeaves();	// the regions themselves
	for (vector<Object*>::iterator regionsItr = regionsObj.begin(); regionsItr != regionsObj.end(); regionsItr++)
	{
		vector<Object*> provincesObj = (*regionsItr)->getValue("provinces");	// the section containing the provinces in the regions
		vector<string> provinceStrings = provincesObj[0]->getTokens();				// the province numbers
		std::set<int> provinces;
		for (vector<string>::iterator provinceItr = provinceStrings.begin(); provinceItr != provinceStrings.end(); provinceItr++)
		{
			provinces.insert(atoi(provinceItr->c_str()));
		}
		EU4ColonialRegions.insert(make_pair((*regionsItr)->getKey(), provinces));
	}
}


void CountryMapping::readV2Regions(Object* obj)
{
	vector<Object*> regionsObj = obj->getLeaves();	// the regions themselves
	for (vector<Object*>::iterator regionsItr = regionsObj.begin(); regionsItr != regionsObj.end(); regionsItr++)
	{
		vector<string> provinceStrings = (*regionsItr)->getTokens();				// the province numbers
		std::set<int> provinces;
		for (vector<string>::iterator provinceItr = provinceStrings.begin(); provinceItr != provinceStrings.end(); provinceItr++)
		{
			provinces.insert(atoi(provinceItr->c_str()));
		}
		V2Regions.insert(make_pair((*regionsItr)->getKey(), provinces));
	}
}


void CountryMapping::CreateMapping(const EU4World& srcWorld, const V2World& destWorld, const colonyMapping& colonyMap, const inverseProvinceMapping& inverseProvinceMap, const provinceMapping& provinceMap, const inverseUnionCulturesMap& inverseUnionCultures)
{
	EU4TagToV2TagMap.clear();

	char generatedV2TagPrefix = 'X'; // single letter prefix
	int generatedV2TagSuffix = 0; // two digit suffix

	const std::map<std::string, EU4Country*> EU4Countries = srcWorld.getCountries();	// all the EU4 countries
	std::map<std::string, EU4Country*> ColonialCountries;										// colonial nations that are valid for replacement
	const map<string, V2Country*> V2Countries = destWorld.getPotentialCountries();
	for (std::map<std::string, EU4Country*>::const_iterator i = EU4Countries.begin(); i != EU4Countries.end(); ++i)
	{
		const std::string& EU4Tag = i->first;	// the EU4 tag being considered
		char charTag[4];
		strcpy_s(charTag, EU4Tag.c_str());

		// colonial replacements
		if (i->second->isColony() && isalpha(charTag[0]) && isdigit(charTag[1]) && isdigit(charTag[2]))
		{
			ColonialCountries.insert(*i);
			continue;
		}

		// rules-based
		oneMapping(EU4Tag, V2Countries,generatedV2TagPrefix, generatedV2TagSuffix);
	}

	for (std::map<std::string, EU4Country*>::const_iterator i = ColonialCountries.begin(); i != ColonialCountries.end(); ++i)
	{
		bool success = attemptColonialReplacement(i->second, srcWorld, V2Countries, colonyMap, inverseProvinceMap, provinceMap, inverseUnionCultures);
		if (!success)
		{
			oneMapping(i->first, V2Countries, generatedV2TagPrefix, generatedV2TagSuffix);
		}
	}
}


bool CountryMapping::attemptColonialReplacement(EU4Country* country, const EU4World& srcWorld, const map<string, V2Country*> V2Countries, const colonyMapping& colonyMap, const inverseProvinceMapping& inverseProvinceMap, const provinceMapping& provinceMap, const inverseUnionCulturesMap& inverseUnionCultures)
{
	bool mapped = false;
	int EU4Capital = country->getCapital();
	int V2Capital = inverseProvinceMap.find(country->getCapital())->second[0];
	for (colonyMapping::const_iterator j = colonyMap.begin(); j != colonyMap.end(); j++)
	{
		// the capital must be in the specified EU4 colonial region
		if (j->EU4Region != "")
		{
			std::map<std::string, std::set<int>>::iterator EU4Region = EU4ColonialRegions.find(j->EU4Region);
			if (EU4Region == EU4ColonialRegions.end())
			{
				LOG(LogLevel::Warning) << "Unknown EU4 Colonial Region " << j->EU4Region;
				continue;
			}
			else if (EU4Region->second.find(EU4Capital) == EU4Region->second.end())
			{
				continue;
			}
		}

		// the capital must be in the specified EU4 colonial region, or the entire region must be owned
		std::map<std::string, std::set<int>>::iterator V2Region = V2Regions.find(j->V2Region);
		if (j->V2Region != "")
		{
			if (V2Region == V2Regions.end())
			{
				LOG(LogLevel::Warning) << "Unknown V2 Region " << j->V2Region;
				continue;
			}
			else if (V2Region->second.find(V2Capital) == V2Region->second.end())
			{
				bool allOwned = true;
				for (set<int>::iterator V2Provs = V2Region->second.begin(); V2Provs != V2Region->second.end(); V2Provs++)
				{
					provinceMapping::const_iterator map = provinceMap.find(*V2Provs);
					if (map == provinceMap.end())
					{
						allOwned = false;
						break;
					}
					for (vector<int>::const_iterator EU4Provs = map->second.begin(); EU4Provs != map->second.end(); EU4Provs++)
					{
						const EU4Province* province = srcWorld.getProvince(*EU4Provs);
						if ((province == NULL) || (province->getOwnerString() != country->getTag()))
						{
							allOwned = false;
							break;
						}
					}
				}
				if (!allOwned)
				{
					continue;
				}
			}
		}

		// it must be in the correct culture group
		if (j->cultureGroup != "")
		{
			if (inverseUnionCultures.find(srcWorld.getCountry(country->getTag())->getPrimaryCulture())->second != j->cultureGroup)
			{
				continue;
			}
		}

		// the tag must be available
		if ((V2Countries.find(j->tag) != V2Countries.end()) &&									// the V2 country exists
			(EU4TagToV2TagMap.right.find(j->tag) == EU4TagToV2TagMap.right.end())		// the V2 country isn't used
			)
		{
			mapped = true;
			EU4TagToV2TagMap.left.insert(make_pair(country->getTag(), j->tag));
			LogMapping(country->getTag(), j->tag, "colonial replacement");
			break;
		}
	}
	return mapped;
}


void CountryMapping::oneMapping(string EU4Tag, const map<string, V2Country*> V2Countries, char& generatedV2TagPrefix, int& generatedV2TagSuffix)
{
	bool mapped = false;	// whether or not the EU4 tag has been mapped

	// Find a V2 tag from our rule if possible.
	std::map<std::string, std::vector<std::string>>::iterator findIter = EU4TagToV2TagsRules.find(EU4Tag);	// the rule (if any) with this EU4 tag
	if (findIter != EU4TagToV2TagsRules.end())
	{
		const std::vector<std::string>& possibleV2Tags = findIter->second;
		// We want to use a V2 tag that corresponds to an actual V2 country if possible.
		for (vector<string>::const_iterator j = possibleV2Tags.begin(); j != possibleV2Tags.end() && !mapped; ++j)
		{
			const std::string& V2Tag = *j;
			if ((V2Countries.find(V2Tag) != V2Countries.end()) && (EU4TagToV2TagMap.right.find(V2Tag) == EU4TagToV2TagMap.right.end()))
			{
				mapped = true;
				EU4TagToV2TagMap.left.insert(make_pair(EU4Tag, V2Tag));
				LogMapping(EU4Tag, V2Tag, "default V2 country");
			}
		}
		if (!mapped)
		{	// None of the V2 tags in our rule correspond to an actual V2 country, so we just use the first unused V2 tag.
			for (vector<string>::const_iterator j = possibleV2Tags.begin(); j != possibleV2Tags.end() && !mapped; ++j)
			{
				const std::string& V2Tag = *j;
				if (EU4TagToV2TagMap.right.find(V2Tag) == EU4TagToV2TagMap.right.end())
				{
					mapped = true;
					EU4TagToV2TagMap.left.insert(make_pair(EU4Tag, V2Tag));
					LogMapping(EU4Tag, V2Tag, "mapping rule, not a V2 country");
				}
			}
			// It's possible to get here if all the V2 tags for this EU4 tag have already been used - we fallback
			// on the same case as EU4 tags without rules.
		}
	}

	// Either the EU4 tag had no mapping rule or no V2 tag from its mapping rule could be used. 
	// We generate a new V2 tag for it.
	if (!mapped)
	{
		ostringstream generatedV2TagStream;	// a stream fr the new tag to be constructed in
		generatedV2TagStream << generatedV2TagPrefix << setfill('0') << setw(2) << generatedV2TagSuffix;
		string V2Tag = generatedV2TagStream.str();
		EU4TagToV2TagMap.left.insert(make_pair(EU4Tag, V2Tag));
		LogMapping(EU4Tag, V2Tag, "generated tag");

		// Prepare the next generated tag.
		++generatedV2TagSuffix;
		if (generatedV2TagSuffix > 99)
		{
			generatedV2TagSuffix = 0;
			--generatedV2TagPrefix;
		}
	}
}


const std::string& CountryMapping::GetV2Tag(const std::string& EU4Tag) const
{
	// The following EU4 tags always map to the V2 rebel tag.
	const std::vector<string> EU4RebelTags = { "REB", "PIR", "NAT" };	// the EU4 tags for rebels, pirates, and natives
	static const std::string V2RebelTag = "REB";								// the V2 tag for rebels
	if (std::find(EU4RebelTags.begin(), EU4RebelTags.end(), EU4Tag) != EU4RebelTags.end())
	{
		return V2RebelTag;
	}

	boost::bimap<std::string, std::string>::left_const_iterator findIter = EU4TagToV2TagMap.left.find(EU4Tag);	// the mapping with this EU4 tag
	if (findIter != EU4TagToV2TagMap.left.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string EU4TagNotFound = "";	// an empty string for unfound tags
		return EU4TagNotFound;
	}
}

const std::string& CountryMapping::GetEU4Tag(const std::string& V2Tag) const
{
	boost::bimap<std::string, std::string>::right_const_iterator findIter = EU4TagToV2TagMap.right.find(V2Tag);	// the mapping with this V2 tag
	if (findIter != EU4TagToV2TagMap.right.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string V2TagNotFound = "";	// an empty string for unfound tags
		return V2TagNotFound;
	}
}

void CountryMapping::LogMapping(const std::string& EU4Tag, const std::string& V2Tag, const std::string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << EU4Tag << " -> " << V2Tag << " (" << reason << ')';
}
