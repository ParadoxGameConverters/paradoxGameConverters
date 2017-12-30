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



#include "Mapper.h"
#include "Log.h"
#include "../Configuration.h"
#include "Object.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include "../V2World/V2World.h"
#include <algorithm>



void initContinentMap(shared_ptr<Object> obj, continentMapping& continentMap)
{
	continentMap.clear();
	for (auto continentObj: obj->getLeaves())
	{
		string continent = continentObj->getKey();
		for (auto provinceStr: continentObj->safeGetTokens("provinces"))
		{
			const int province = stoi(provinceStr);
			continentMap.insert( make_pair(province, continent) );
		}
	}
}


void initLeaderTraitsMap(leaderTraitsMap& leaderTraits)
{
	LOG(LogLevel::Info) << "Parsing government jobs";

	auto obj = parser_UTF8::doParseFile("leader_traits.txt");
	if (obj)
	{
		vector<shared_ptr<Object>> typesObj = obj->getLeaves();
		for (auto typeItr: typesObj)
		{
			string type = typeItr->getKey();
			vector<string> traits;
			auto traitsObj = typeItr->getLeaves();
			for (auto trait: traitsObj)
			{
				traits.push_back(trait->getLeaf());
			}
			leaderTraits.insert(make_pair(type, traits));
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse file leader_traits.txt";
		exit(-1);
	}
}


void initLeaderPersonalityMap(personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap)
{
	LOG(LogLevel::Info) << "Parsing personality mappings";

	auto obj = parser_UTF8::doParseFile("personality_map.txt");
	if (obj)
	{
		vector<shared_ptr<Object>> personalitiesObj = obj->getLeaves();
		for (auto personalityItr: personalitiesObj)
		{
			string personality = personalityItr->getKey();
			vector<string> landTraits;
			vector<string> seaTraits;
			auto traitsObj = personalityItr->getLeaves();
			for (auto trait: traitsObj)
			{
				if (trait->getKey() == "land")
				{
					landTraits.push_back(trait->getLeaf());
				}
				else if (trait->getKey() == "sea")
				{
					seaTraits.push_back(trait->getLeaf());
				}
			}
			landPersonalityMap.insert(make_pair(personality, landTraits));
			seaPersonalityMap.insert(make_pair(personality, seaTraits));
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse file personality_map.txt";
		exit(-1);
	}
}


void initLeaderBackgroundMap(backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	LOG(LogLevel::Info) << "Parsing background mappings";

	auto obj = parser_UTF8::doParseFile("background_map.txt");
	if (obj)
	{
		vector<shared_ptr<Object>> backgroundObj = obj->getLeaves();
		for (auto backgroundItr: backgroundObj)
		{
			string background = backgroundItr->getKey();
			vector<string> landTraits;
			vector<string> seaTraits;
			auto traitsObj = backgroundItr->getLeaves();
			for (auto trait: traitsObj)
			{
				if (trait->getKey() == "land")
				{
					landTraits.push_back(trait->getLeaf());
				}
				else if (trait->getKey() == "sea")
				{
					seaTraits.push_back(trait->getLeaf());
				}
			}
			landBackgroundMap.insert(make_pair(background, landTraits));
			seaBackgroundMap.insert(make_pair(background, seaTraits));
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse file background_map.txt";
		exit(-1);
	}
}