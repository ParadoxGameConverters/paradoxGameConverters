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
#include <iostream>
#include <set>
#include <sstream>
#include <utility>
#include <boost/algorithm/string.hpp>
#include "ParadoxParserUTF8.h"
#include "../V2World/V2World.h"
#include "../HOI4World/HoI4World.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



CountryMapper* CountryMapper::instance = NULL;


CountryMapper::CountryMapper()
{
	LOG(LogLevel::Info) << "Getting country mappings";
	readRules();
}


void CountryMapper::readRules()
{
	LOG(LogLevel::Info) << "Reading country mapping rules";
	vector<Object*> ruleNodes = getRules();
	for (auto rule: ruleNodes)
	{
		importRule(rule);
	}
}


vector<Object*> CountryMapper::getRules()
{
	Object* countryMappingsFile = parser_UTF8::doParseFile("country_mappings.txt");
	if (!countryMappingsFile)
	{
		LOG(LogLevel::Error) << "Failed to parse country_mappings.txt";
		exit(-1);
	}
	vector<Object*> nodes = countryMappingsFile->getLeaves();
	if (nodes.empty())
	{
		LOG(LogLevel::Error) << "country_mappings.txt does not contain a mapping";
		exit(-1);
	}

	return nodes[0]->getLeaves();
}


void CountryMapper::importRule(Object* rule)
{
	vector<Object*> ruleItems = rule->getLeaves();

	string newVic2Tag;
	vector<string>	HoI4Tags;
	for (auto item: ruleItems)
	{
		string key = boost::to_upper_copy(item->getKey());
		if (key == "VIC")
		{
			newVic2Tag = boost::to_upper_copy(item->getLeaf());
		}
		else if (key == "HOI")
		{
			HoI4Tags.push_back(boost::to_upper_copy(item->getLeaf()));
		}
		else
		{
			LOG(LogLevel::Warning) << "Ignoring unknown key '" << key << "' while mapping countries";
		}
	}
	Vic2TagToHoI4TagsRules.insert(make_pair(newVic2Tag, HoI4Tags));
}


void CountryMapper::CreateMappings(const V2World* srcWorld)
{
	LOG(LogLevel::Info) << "Creating country mapping";
	resetMappingData();

	for (auto Vic2Country: srcWorld->getCountries())
	{
		makeOneMapping(Vic2Country.first);
	}
}


void CountryMapper::resetMappingData()
{
	V2TagToHoI4TagMap.clear();

	generatedHoI4TagPrefix = 'X';
	generatedHoI4TagSuffix = 0;
}


void CountryMapper::makeOneMapping(const string& Vic2Tag)
{
	auto mappingRule = Vic2TagToHoI4TagsRules.find(Vic2Tag);

	bool mapped = false;
	if (mappingRule != Vic2TagToHoI4TagsRules.end())
	{
		auto possibleHoI4Tags = mappingRule->second;
		mapped = mapToFirstUnusedVic2Tag(possibleHoI4Tags, Vic2Tag);
	}

	if (!mapped)
	{
		string HoI4Tag = generateNewHoI4Tag(Vic2Tag);
		mapToNewTag(Vic2Tag, HoI4Tag);
	}
}


bool CountryMapper::mapToFirstUnusedVic2Tag(const vector<string>& possibleHoI4Tags, const string& Vic2Tag)
{
	for (auto possibleHoI4Tag: possibleHoI4Tags)
	{
		if (!tagIsAlreadyAssigned(possibleHoI4Tag))
		{
			V2TagToHoI4TagMap.left.insert(make_pair(Vic2Tag, possibleHoI4Tag));
			LogMapping(Vic2Tag, possibleHoI4Tag, "mapping rule");

			return true;
		}
	}

	return false;
}


string CountryMapper::generateNewHoI4Tag(const string& Vic2Tag)
{
	ostringstream generatedHoI4TagStream;
	generatedHoI4TagStream << generatedHoI4TagPrefix << setfill('0') << setw(2) << generatedHoI4TagSuffix;
	string newTag = generatedHoI4TagStream.str();

	++generatedHoI4TagSuffix;
	if (generatedHoI4TagSuffix > 99)
	{
		generatedHoI4TagSuffix = 0;
		--generatedHoI4TagPrefix;
	}

	return newTag;
}


void CountryMapper::mapToNewTag(const string& Vic2Tag, const string& HoI4Tag)
{
	V2TagToHoI4TagMap.left.insert(make_pair(Vic2Tag, HoI4Tag));
	LogMapping(Vic2Tag, HoI4Tag, "generated tag");
}


void CountryMapper::LogMapping(const string& sourceTag, const string& targetTag, const string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << sourceTag << " -> " << targetTag << " (" << reason << ')';
}


bool CountryMapper::tagIsAlreadyAssigned(const string& HoI4Tag)
{
	return (V2TagToHoI4TagMap.right.find(HoI4Tag) != V2TagToHoI4TagMap.right.end());
}


const string CountryMapper::GetHoI4Tag(const string& V2Tag) const
{
	boost::bimap<string, string>::left_const_iterator findIter = V2TagToHoI4TagMap.left.find(V2Tag);
	if (findIter != V2TagToHoI4TagMap.left.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}


const string CountryMapper::GetVic2Tag(const string& HoI4Tag) const
{
	boost::bimap<string, string>::right_const_iterator findIter = V2TagToHoI4TagMap.right.find(HoI4Tag);
	if (findIter != V2TagToHoI4TagMap.right.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}