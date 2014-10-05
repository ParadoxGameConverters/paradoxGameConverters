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

#include "EU3World\EU3World.h"
#include "Parsers\Object.h"
#include "Parsers\Parser.h"
#include "V2World\V2World.h"
#include "Log.h"

bool CountryMapping::ReadRules(const std::string& fileName)
{
	LOG(LogLevel::Info) << "Reading country mapping rules";

	// Read the rule nodes from file.
	LOG(LogLevel::Debug) << "Parsing rules from file " << fileName;
	initParser();
	Object* countryMappingsFile = doParseFile(fileName.c_str());
	if (!countryMappingsFile)
	{
		LOG(LogLevel::Error) << "Failed to parse " << fileName;
		return false;
	}
	vector<Object*> nodes = countryMappingsFile->getLeaves();
	if (nodes.empty())
	{
		LOG(LogLevel::Error) << fileName << " does not contain a mapping";
		return false;
	}
	vector<Object*> ruleNodes = nodes[0]->getLeaves();

	// Convert rule nodes into our map data structure.
	LOG(LogLevel::Debug) << "Building rules map";
	map<string, vector<string>> newEU3TagToV2TagsRules;
	for (vector<Object*>::iterator i = ruleNodes.begin(); i != ruleNodes.end(); ++i)
	{
		vector<Object*> rule = (*i)->getLeaves();
		string newEU3Tag;
		vector<string>	V2Tags;
		for (vector<Object*>::iterator j = rule.begin(); j != rule.end(); ++j)
		{
			std::string key = boost::to_upper_copy((*j)->getKey());
			if (key == "EU3")
			{
				newEU3Tag = boost::to_upper_copy((*j)->getLeaf());
			}
			else if (key == "VIC")
			{
				V2Tags.push_back(boost::to_upper_copy((*j)->getLeaf()));
			}
			else
			{
				LOG(LogLevel::Warning) << "Ignoring unknown key '" << key << "' while mapping countries";
			}
		}
		newEU3TagToV2TagsRules.insert(std::make_pair(newEU3Tag, V2Tags));
	}

	LOG(LogLevel::Debug) << "Finished reading country mapping rules";
	std::swap(EU3TagToV2TagsRules, newEU3TagToV2TagsRules);
	return true;
}

void CountryMapping::CreateMapping(const EU3World& srcWorld, const V2World& destWorld)
{
	LOG(LogLevel::Info) << "Creating country mapping";
	EU3TagToV2TagMap.clear();

	char generatedV2TagPrefix = 'X'; // single letter prefix
	int generatedV2TagSuffix = 0; // two digit suffix

	// Get the EU3 tags for all countries we want to map.
	std::set<std::string> EU3TagsToMap;
	const std::map<std::string, EU3Country*> EU3Countries = srcWorld.getCountries();
	for (std::map<std::string, EU3Country*>::const_iterator i = EU3Countries.begin(); i != EU3Countries.end(); ++i)
	{
		EU3TagsToMap.insert(i->first);
	}

	// Find a V2 tag from the rules for each EU3 tag.
	const map<string, V2Country*> V2Countries = destWorld.getPotentialCountries();
	for (std::set<std::string>::iterator i = EU3TagsToMap.begin(); i != EU3TagsToMap.end(); ++i)
	{
		const std::string& EU3Tag = *i;
		bool mapped = false;
		// Find a V2 tag from our rule if possible.
		std::map<std::string, std::vector<std::string>>::iterator findIter = EU3TagToV2TagsRules.find(EU3Tag);
		if (findIter != EU3TagToV2TagsRules.end())
		{
			const std::vector<std::string>& possibleV2Tags = findIter->second;
			// We want to use a V2 tag that corresponds to an actual V2 country if possible.
			for (vector<string>::const_iterator j = possibleV2Tags.begin(); j != possibleV2Tags.end() && !mapped; ++j)
			{
				const std::string& V2Tag = *j;
				if (V2Countries.find(V2Tag) != V2Countries.end() && EU3TagToV2TagMap.right.find(V2Tag) == EU3TagToV2TagMap.right.end())
				{
					mapped = true;
					EU3TagToV2TagMap.left.insert(make_pair(EU3Tag, V2Tag));
					LogMapping(EU3Tag, V2Tag, "default V2 country");
				}
			}
			if (!mapped)
			{	// None of the V2 tags in our rule correspond to an actual V2 country, so we just use the first unused V2 tag.
				for (vector<string>::const_iterator j = possibleV2Tags.begin(); j != possibleV2Tags.end() && !mapped; ++j)
				{
					const std::string& V2Tag = *j;
					if (EU3TagToV2TagMap.right.find(V2Tag) == EU3TagToV2TagMap.right.end())
					{
						mapped = true;
						EU3TagToV2TagMap.left.insert(make_pair(EU3Tag, V2Tag));
						LogMapping(EU3Tag, V2Tag, "mapping rule, not a V2 country");
					}
				}
				// It's possible to get here if all the V2 tags for this EU3 tag have already been used - we fallback
				// on the same case as EU3 tags without rules.
			}
		}
		if (!mapped)
		{	// Either the EU3 tag had no mapping rule or no V2 tag from its mapping rule could be used. 
			// We generate a new V2 tag for it.
			ostringstream generatedV2TagStream;
			generatedV2TagStream << generatedV2TagPrefix << setfill('0') << setw(2) << generatedV2TagSuffix;
			string V2Tag = generatedV2TagStream.str();
			EU3TagToV2TagMap.left.insert(make_pair(EU3Tag, V2Tag));
			LogMapping(EU3Tag, V2Tag, "generated tag");
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

const std::string& CountryMapping::GetV2Tag(const std::string& EU3Tag) const
{
	// The following EU3 tags always map to the V2 rebel tag.
	const std::vector<string> EU3RebelTags = { "REB", "PIR", "NAT" };
	static const std::string V2RebelTag = "REB";
	if (std::find(EU3RebelTags.begin(), EU3RebelTags.end(), EU3Tag) != EU3RebelTags.end())
	{
		return V2RebelTag;
	}

	boost::bimap<std::string, std::string>::left_const_iterator findIter = EU3TagToV2TagMap.left.find(EU3Tag);
	if (findIter != EU3TagToV2TagMap.left.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string EU3TagNotFound = "";
		return EU3TagNotFound;
	}
}

const std::string& CountryMapping::GetEU3Tag(const std::string& V2Tag) const
{
	boost::bimap<std::string, std::string>::right_const_iterator findIter = EU3TagToV2TagMap.right.find(V2Tag);
	if (findIter != EU3TagToV2TagMap.right.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string V2TagNotFound = "";
		return V2TagNotFound;
	}
}

void CountryMapping::LogMapping(const std::string& EU3Tag, const std::string& V2Tag, const std::string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << EU3Tag << " -> " << V2Tag << " (" << reason << ')';
}
