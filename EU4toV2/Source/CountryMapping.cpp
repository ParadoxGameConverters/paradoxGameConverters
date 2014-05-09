#include "CountryMapping.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <utility>

#include <boost/algorithm/string.hpp>

#include "EU4World\EU4World.h"
#include "Parsers\Object.h"
#include "Parsers\Parser.h"
#include "V2World\V2World.h"
#include "Log.h"

bool CountryMapping::ReadRules(const std::string& fileName)
{
	LogUpdate("Reading country mapping rules");

	// Read the rule nodes from file.
	LogUpdate("\tParsing rules from file " + fileName, false);
	initParser();
	Object* countryMappingsFile = doParseFile(fileName.c_str());
	if (!countryMappingsFile)
	{
		LogUpdate("Error: Failed to parse " + fileName);
		return false;
	}
	vector<Object*> nodes = countryMappingsFile->getLeaves();
	if (nodes.empty())
	{
		LogUpdate("Error: " + fileName + " does not contain a mapping");
		return false;
	}
	vector<Object*> ruleNodes = nodes[0]->getLeaves();

	// Convert rule nodes into our map data structure.
	LogUpdate("\tBuilding rules map", false);
	map<string, vector<string>> newEU4TagToV2TagsRules;
	for (vector<Object*>::iterator i = ruleNodes.begin(); i != ruleNodes.end(); ++i)
	{
		vector<Object*> rule = (*i)->getLeaves();
		string newEU4Tag;
		vector<string>	V2Tags;
		for (vector<Object*>::iterator j = rule.begin(); j != rule.end(); ++j)
		{
			std::string key = boost::to_upper_copy((*j)->getKey());
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
				LogUpdate("Warning: Ignoring unknown key '" + key + "' while mapping countries");
			}
		}
		newEU4TagToV2TagsRules.insert(std::make_pair(newEU4Tag, V2Tags));
	}

	LogUpdate("\tFinished reading country mapping rules", false);
	std::swap(EU4TagToV2TagsRules, newEU4TagToV2TagsRules);
	return true;
}

void CountryMapping::CreateMapping(const EU4World& srcWorld, const V2World& destWorld)
{
	LogUpdate("Creating country mapping");
	EU4TagToV2TagMap.clear();

	char generatedV2TagPrefix = 'X'; // single letter prefix
	int generatedV2TagSuffix = 0; // two digit suffix

	// Get the EU4 tags for all countries we want to map.
	std::set<std::string> EU4TagsToMap;
	const std::map<std::string, EU4Country*> EU4Countries = srcWorld.getCountries();
	for (std::map<std::string, EU4Country*>::const_iterator i = EU4Countries.begin(); i != EU4Countries.end(); ++i)
	{
		EU4TagsToMap.insert(i->first);
	}

	// Find a V2 tag from the rules for each EU4 tag.
	const map<string, V2Country*> V2Countries = destWorld.getPotentialCountries();
	for (std::set<std::string>::iterator i = EU4TagsToMap.begin(); i != EU4TagsToMap.end(); ++i)
	{
		const std::string& EU4Tag = *i;
		bool mapped = false;
		// Find a V2 tag from our rule if possible.
		std::map<std::string, std::vector<std::string>>::iterator findIter = EU4TagToV2TagsRules.find(EU4Tag);
		if (findIter != EU4TagToV2TagsRules.end())
		{
			const std::vector<std::string>& possibleV2Tags = findIter->second;
			// We want to use a V2 tag that corresponds to an actual V2 country if possible.
			for (vector<string>::const_iterator j = possibleV2Tags.begin(); j != possibleV2Tags.end() && !mapped; ++j)
			{
				const std::string& V2Tag = *j;
				if (V2Countries.find(V2Tag) != V2Countries.end() && EU4TagToV2TagMap.right.find(V2Tag) == EU4TagToV2TagMap.right.end())
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
		if (!mapped)
		{	// Either the EU4 tag had no mapping rule or no V2 tag from its mapping rule could be used. 
			// We generate a new V2 tag for it.
			ostringstream generatedV2TagStream;
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
}

const std::string& CountryMapping::GetV2Tag(const std::string& EU4Tag) const
{
	// The following EU4 tags always map to the V2 rebel tag.
	const std::vector<string> EU4RebelTags = { "REB", "PIR", "NAT" };
	static const std::string V2RebelTag = "REB";
	if (std::find(EU4RebelTags.begin(), EU4RebelTags.end(), EU4Tag) != EU4RebelTags.end())
	{
		return V2RebelTag;
	}

	boost::bimap<std::string, std::string>::left_const_iterator findIter = EU4TagToV2TagMap.left.find(EU4Tag);
	if (findIter != EU4TagToV2TagMap.left.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string EU4TagNotFound = "";
		return EU4TagNotFound;
	}
}

const std::string& CountryMapping::GetEU4Tag(const std::string& V2Tag) const
{
	boost::bimap<std::string, std::string>::right_const_iterator findIter = EU4TagToV2TagMap.right.find(V2Tag);
	if (findIter != EU4TagToV2TagMap.right.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string V2TagNotFound = "";
		return V2TagNotFound;
	}
}

void CountryMapping::LogUpdate(const std::string& text, bool includeConsole)
{
	log("%s\n", text.c_str());
	if (includeConsole)
	{
		std::cout << text << '\n';
	}
}

void CountryMapping::LogMapping(const std::string& EU4Tag, const std::string& V2Tag, const std::string& reason)
{
	LogUpdate("\tMapping " + EU4Tag + " -> " + V2Tag + " (" + reason + ')', false);
}
