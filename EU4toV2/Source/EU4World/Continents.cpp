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



#include "ContinentMapper.h"
#include "../Configuration.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"



continentMapper* continentMapper::instance = NULL;



continentMapper::continentMapper()
{
	LOG(LogLevel::Info) << "Finding Continents";
	for (auto mod: Configuration::getEU4Mods())
	{
		string continentFile = mod + "/map/continent.txt";
		if (Utils::DoesFileExist(continentFile))
		{
			shared_ptr<Object> continentObject = parser_UTF8::doParseFile(continentFile);
			if ((continentObject != NULL) && (continentObject->getLeaves().size() > 0))
			{
				initContinentMap(continentObject);
			}
		}
	}

	if (continentMap.empty())
	{
		shared_ptr<Object> continentObject = parser_UTF8::doParseFile(Configuration::getEU4Path() + "/map/continent.txt");
		if (continentObject == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file " << Configuration::getEU4Path() << "/map/continent.txt";
			exit(-1);
		}
		if (continentObject->getLeaves().size() < 1)
		{
			LOG(LogLevel::Error) << "Failed to parse continent.txt";
			exit(-1);
		}
		initContinentMap(continentObject);
	}

	if (continentMap.empty())
	{
		LOG(LogLevel::Warning) << "No continent mappings found - may lead to problems later";
	}
}



void continentMapper::initContinentMap(shared_ptr<Object> obj)
{
	continentMap.clear();

	vector<shared_ptr<Object>> continentObjs = obj->getLeaves();
	for (auto continentObj: continentObjs)
	{
		string continent = continentObj->getKey();
		vector<string> provinceNums = continentObj->getTokens();
		for (auto provinceNum: provinceNums)
		{
			const int province = stoi(provinceNum);
			continentMap.insert(make_pair(province, continent));
		}
	}
}


string continentMapper::GetEU4Continent(int EU4Province)
{
	auto mapping = continentMap.find(EU4Province);
	if (mapping != continentMap.end())
	{
		return mapping->second;
	}
	else
	{
		return "";
	}
}