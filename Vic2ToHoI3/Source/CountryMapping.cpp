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



#include "CountryMapping.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <utility>

#include <boost/algorithm/string.hpp>

#include "Object.h"
#include "paradoxParser.h"
#include "V2World\V2World.h"
#include "HoI3World\HoI3World.h"
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
	map<string, vector<string>> newV2TagToHoI3TagsRules;	// the mapping rules
	for (vector<Object*>::iterator i = ruleNodes.begin(); i != ruleNodes.end(); ++i)
	{
		vector<Object*> rule = (*i)->getLeaves();	// an individual rule
		string newV2Tag;									// the V2 tag in the rule
		vector<string>	HoI3Tags;							// the HoI3 tags in the rule
		for (vector<Object*>::iterator j = rule.begin(); j != rule.end(); ++j)
		{
			std::string key = boost::to_upper_copy((*j)->getKey());	// the key for this part of the rule
			if (key == "VIC")
			{
				newV2Tag = boost::to_upper_copy((*j)->getLeaf());
			}
			else if (key == "HOI")
			{
				HoI3Tags.push_back(boost::to_upper_copy((*j)->getLeaf()));
			}
			else
			{
				LOG(LogLevel::Warning) << "Ignoring unknown key '" << key << "' while mapping countries";
			}
		}
		newV2TagToHoI3TagsRules.insert(std::make_pair(newV2Tag, HoI3Tags));
	}

	LOG(LogLevel::Debug) << "Finished reading country mapping rules";
	std::swap(V2TagToHoI3TagsRules, newV2TagToHoI3TagsRules);
	return true;
}


void CountryMapping::CreateMapping(const V2World& srcWorld, const HoI3World& destWorld)
{
	LOG(LogLevel::Info) << "Creating country mapping";
	V2TagToHoI3TagMap.clear();

	char generatedHoI3TagPrefix = 'X'; // single letter prefix
	int generatedHoI3TagSuffix = 0; // two digit suffix

	// Get the V2 tags for all countries we want to map.
	std::set<std::string> V2TagsToMap;		// the V2 tags that still need mapping
	const std::map<std::string, V2Country*> V2Countries = srcWorld.getCountries();	// all the EU4 countries
	for (std::map<std::string, V2Country*>::const_iterator i = V2Countries.begin(); i != V2Countries.end(); ++i)
	{
		V2TagsToMap.insert(i->first);
	}

	// Find a HoI3 tag from the rules for each V2 tag.
	const map<string, HoI3Country*> HoI3Countries = destWorld.getPotentialCountries();
	for (std::set<std::string>::iterator i = V2TagsToMap.begin(); i != V2TagsToMap.end(); ++i)
	{
		const std::string& V2Tag = *i;	// the V2 tag being considered
		bool mapped = false;					// whether or not the V2 tag has been mapped
		// Find a HoI3 tag from our rule if possible.
		std::map<std::string, std::vector<std::string>>::iterator findIter = V2TagToHoI3TagsRules.find(V2Tag);	// the rule (if any) with this V2 tag
		if (findIter != V2TagToHoI3TagsRules.end())
		{
			const std::vector<std::string>& possibleHoI3Tags = findIter->second;
			// We want to use a HoI3 tag that corresponds to an actual HoI3 country if possible.
			for (vector<string>::const_iterator j = possibleHoI3Tags.begin(); j != possibleHoI3Tags.end() && !mapped; ++j)
			{
				const std::string& HoI3Tag = *j;
				if (HoI3Countries.find(HoI3Tag) != HoI3Countries.end() && V2TagToHoI3TagMap.right.find(HoI3Tag) == V2TagToHoI3TagMap.right.end())
				{
					mapped = true;
					V2TagToHoI3TagMap.left.insert(make_pair(V2Tag, HoI3Tag));
					LogMapping(V2Tag, HoI3Tag, "default HoI3 country");
				}
			}
		}
		if (!mapped)
		{	// Either the V2 tag had no mapping rule or no HoI3 tag from its mapping rule could be used. 
			// We generate a new HoI3 tag for it.
			ostringstream generatedHoI3TagStream;	// a stream for the new tag to be constructed in
			generatedHoI3TagStream << generatedHoI3TagPrefix << setfill('0') << setw(2) << generatedHoI3TagSuffix;
			string HoI3Tag = generatedHoI3TagStream.str();
			V2TagToHoI3TagMap.left.insert(make_pair(V2Tag, HoI3Tag));
			LogMapping(V2Tag, HoI3Tag, "generated tag");
			// Prepare the next generated tag.
			++generatedHoI3TagSuffix;
			if (generatedHoI3TagSuffix > 99)
			{
				generatedHoI3TagSuffix = 0;
				--generatedHoI3TagPrefix;
			}
		}
	}
}

const std::string& CountryMapping::GetHoI3Tag(const std::string& V2Tag) const
{
	static const std::string V2RebelTag = "REB";								// the V2 tag for rebels

	boost::bimap<std::string, std::string>::left_const_iterator findIter = V2TagToHoI3TagMap.left.find(V2Tag);	// the mapping with this V2 tag
	if (findIter != V2TagToHoI3TagMap.left.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string V2TagNotFound = "";	// an empty string for unfound tags
		return V2TagNotFound;
	}
}

const std::string& CountryMapping::GetVic2Tag(const std::string& HoI3Tag) const
{
	boost::bimap<std::string, std::string>::right_const_iterator findIter = V2TagToHoI3TagMap.right.find(HoI3Tag);	// the mapping with this HoI3 tag
	if (findIter != V2TagToHoI3TagMap.right.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string HoI3TagNotFound = "";	// an empty string for unfound tags
		return HoI3TagNotFound;
	}
}

void CountryMapping::LogMapping(const std::string& sourceTag, const std::string& targetTag, const std::string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << sourceTag << " -> " << targetTag << " (" << reason << ')';
}
