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
#include "paradoxParserUTF8.h"
#include "V2World/V2World.h"
#include "HoI4World/HoI4World.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



bool CountryMapping::ReadRules(const std::string& fileName)
{
	LOG(LogLevel::Info) << "Reading country mapping rules";

	// Read the rule nodes from file.
	LOG(LogLevel::Debug) << "Parsing rules from file " << fileName;
	parser_UTF8::initParser();
	Object* countryMappingsFile = parser_UTF8::doParseFile(fileName.c_str());	// the parsed country mappings file
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

	// Convert rule nodes into our map data structure.
	LOG(LogLevel::Debug) << "Building rules map";
	map<string, vector<string>> newV2TagToHoI4TagsRules;	// the mapping rules
	for (auto ruleNode: nodes[0]->getLeaves())
	{
		vector<Object*> rule = ruleNode->getLeaves();	// an individual rule
		string newV2Tag;											// the V2 tag in the rule
		vector<string>	HoI4Tags;							// the HoI4 tags in the rule
		for (auto part: rule)
		{
			std::string key = boost::to_upper_copy(part->getKey());	// the key for this part of the rule
			if (key == "VIC")
			{
				newV2Tag = boost::to_upper_copy(part->getLeaf());
			}
			else if (key == "HOI")
			{
				HoI4Tags.push_back(boost::to_upper_copy(part->getLeaf()));
			}
			else
			{
				LOG(LogLevel::Warning) << "Ignoring unknown key '" << key << "' while mapping countries";
			}
		}
		newV2TagToHoI4TagsRules.insert(std::make_pair(newV2Tag, HoI4Tags));
	}

	LOG(LogLevel::Debug) << "Finished reading country mapping rules";
	std::swap(V2TagToHoI4TagsRules, newV2TagToHoI4TagsRules);
	return true;
}


void CountryMapping::CreateMapping(const V2World& srcWorld, const HoI4World& destWorld)
{
	LOG(LogLevel::Info) << "Creating country mapping";
	V2TagToHoI4TagMap.clear();

	char generatedHoI4TagPrefix = 'X'; // single letter prefix
	int generatedHoI4TagSuffix = 0; // two digit suffix

	// Find a HoI4 tag from the rules for each Vic2 country
	for (auto Vic2Tag: srcWorld.getCountries())
	{
		// Either the V2 tag had no mapping rule or no HoI4 tag from its mapping rule could be used. 
		// We generate a new HoI4 tag for it.
		ostringstream generatedHoI4TagStream;	// a stream for the new tag to be constructed in
		generatedHoI4TagStream << generatedHoI4TagPrefix << setfill('0') << setw(2) << generatedHoI4TagSuffix;
		string HoI4Tag = generatedHoI4TagStream.str();
		V2TagToHoI4TagMap.left.insert(make_pair(Vic2Tag.first, HoI4Tag));
		LogMapping(Vic2Tag.first, HoI4Tag, "generated tag");
		// Prepare the next generated tag.
		++generatedHoI4TagSuffix;
		if (generatedHoI4TagSuffix > 99)
		{
			generatedHoI4TagSuffix = 0;
			--generatedHoI4TagPrefix;
		}
	}
}


const std::string& CountryMapping::GetHoI4Tag(const std::string& V2Tag) const
{
	static const std::string V2RebelTag = "REB";								// the V2 tag for rebels

	boost::bimap<std::string, std::string>::left_const_iterator findIter = V2TagToHoI4TagMap.left.find(V2Tag);	// the mapping with this V2 tag
	if (findIter != V2TagToHoI4TagMap.left.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string V2TagNotFound = "";	// an empty string for unfound tags
		return V2TagNotFound;
	}
}

const std::string& CountryMapping::GetVic2Tag(const std::string& HoI4Tag) const
{
	boost::bimap<std::string, std::string>::right_const_iterator findIter = V2TagToHoI4TagMap.right.find(HoI4Tag);	// the mapping with this HoI4 tag
	if (findIter != V2TagToHoI4TagMap.right.end())
	{
		return findIter->second;
	}
	else
	{
		static const std::string HoI4TagNotFound = "";	// an empty string for unfound tags
		return HoI4TagNotFound;
	}
}

void CountryMapping::LogMapping(const std::string& sourceTag, const std::string& targetTag, const std::string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << sourceTag << " -> " << targetTag << " (" << reason << ')';
}
