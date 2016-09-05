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
#include "../V2World/V2World.h"
#include "../HoI4World/HoI4World.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



CountryMapper* CountryMapper::instance = NULL;


CountryMapper::CountryMapper()
{
}


void CountryMapper::CreateMappings(const V2World* srcWorld)
{
	LOG(LogLevel::Info) << "Creating country mapping";
	resetMappingData();

	for (auto Vic2Tag: srcWorld->getCountries())
	{
		string HoI4Tag = generateNewTag();
		V2TagToHoI4TagMap.left.insert(make_pair(Vic2Tag.first, HoI4Tag));
		LogMapping(Vic2Tag.first, HoI4Tag, "generated tag");
	}
}


void CountryMapper::resetMappingData()
{
	V2TagToHoI4TagMap.clear();

	generatedHoI4TagPrefix = 'X';
	generatedHoI4TagSuffix = 0;
}


string CountryMapper::generateNewTag()
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


void CountryMapper::LogMapping(const string& sourceTag, const string& targetTag, const string& reason)
{
	LOG(LogLevel::Debug) << "Mapping " << sourceTag << " -> " << targetTag << " (" << reason << ')';
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