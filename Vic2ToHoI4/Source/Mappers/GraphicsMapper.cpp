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



#include "GraphicsMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"



graphicsMapper* graphicsMapper::instance = nullptr;



graphicsMapper::graphicsMapper()
{
	LOG(LogLevel::Info) << "Reading graphics mappings";

	auto fileObj = parser_UTF8::doParseFile("cultureGroupToGraphics.txt");
	if (fileObj == nullptr)
	{
		return;
	}

	auto cultureGroupObjs = fileObj->getLeaves();
	for (auto cultureGroupObj: cultureGroupObjs)
	{
		string cultureGroup = cultureGroupObj->getKey();

		auto leaderPortraitObjs = cultureGroupObj->getValue("leader_portraits");
		if (leaderPortraitObjs.size() > 0)
		{
			loadLeaderPortraitMappings(cultureGroup, leaderPortraitObjs[0]);
		}

		auto ideologyMinisterPortraitObjs = cultureGroupObj->getValue("ideology_minister_portraits");
		if (leaderPortraitObjs.size() > 0)
		{
			loadIdeologyMinisterPortraitMappings(cultureGroup, ideologyMinisterPortraitObjs[0]);
		}

		auto graphicalCultureObjs = cultureGroupObj->getValue("graphical_culture");
		if (graphicalCultureObjs.size() > 0)
		{
			loadGraphicalCultureMappings(cultureGroup, graphicalCultureObjs[0]);
		}

		auto graphicalCulture2dObjs = cultureGroupObj->getValue("graphical_culture_2d");
		if (graphicalCulture2dObjs.size() > 0)
		{
			loadGraphicalCulture2dMappings(cultureGroup, graphicalCulture2dObjs[0]);
		}
	}
}


void graphicsMapper::loadLeaderPortraitMappings(const string& cultureGroup, shared_ptr<Object> portraitMappings)
{
	auto cultureGroupMappings = leaderPortraitMappings.element.find(cultureGroup);
	if (cultureGroupMappings == leaderPortraitMappings.element.end())
	{
		cultureGroupToPortraitsMap newCultureGroupMappings;
		leaderPortraitMappings.element.insert(make_pair(cultureGroup, newCultureGroupMappings));
		cultureGroupMappings = leaderPortraitMappings.element.find(cultureGroup);
	}

	auto ideologyObjs = portraitMappings->getLeaves();
	for (auto ideologyObj: ideologyObjs)
	{
		string ideology = ideologyObj->getKey();
		auto ideologyMapping = cultureGroupMappings->second.element.find(ideology);
		if (ideologyMapping == cultureGroupMappings->second.element.end())
		{
			vector<string> newPortaits;
			cultureGroupMappings->second.element.insert(make_pair(ideology, newPortaits));
			ideologyMapping = cultureGroupMappings->second.element.find(ideology);
		}

		for (auto portraitStr: ideologyObj->getTokens())
		{
			ideologyMapping->second.push_back(portraitStr);
		}
	}
}


void graphicsMapper::loadIdeologyMinisterPortraitMappings(const string& cultureGroup, shared_ptr<Object> portraitMappings)
{
	auto cultureGroupMappings = ideologyMinisterMappings.element.find(cultureGroup);
	if (cultureGroupMappings == ideologyMinisterMappings.element.end())
	{
		cultureGroupToPortraitsMap newCultureGroupMappings;
		ideologyMinisterMappings.element.insert(make_pair(cultureGroup, newCultureGroupMappings));
		cultureGroupMappings = ideologyMinisterMappings.element.find(cultureGroup);
	}

	auto ideologyObjs = portraitMappings->getLeaves();
	for (auto ideologyObj: ideologyObjs)
	{
		string ideology = ideologyObj->getKey();
		auto ideologyMapping = cultureGroupMappings->second.element.find(ideology);
		if (ideologyMapping == cultureGroupMappings->second.element.end())
		{
			vector<string> newPortaits;
			cultureGroupMappings->second.element.insert(make_pair(ideology, newPortaits));
			ideologyMapping = cultureGroupMappings->second.element.find(ideology);
		}

		for (auto portraitStr: ideologyObj->getTokens())
		{
			ideologyMapping->second.push_back(portraitStr);
		}
	}
}


void graphicsMapper::loadGraphicalCultureMappings(const string& cultureGroup, shared_ptr<Object> graphicalCultureMappings)
{
	graphicalCultureMap[cultureGroup] = graphicalCultureMappings->getLeaf();
}


void graphicsMapper::loadGraphicalCulture2dMappings(const string& cultureGroup, shared_ptr<Object> graphicalCulture2dMappings)
{
	graphicalCulture2dMap[cultureGroup] = graphicalCulture2dMappings->getLeaf();
}


string graphicsMapper::GetLeaderPortrait(string cultureGroup, string ideology)
{
	vector<string> portraits = GetLeaderPortraits(cultureGroup, ideology);

	std::uniform_int_distribution<int> firstNameGen(0, portraits.size() - 1);
	return portraits[firstNameGen(rng)];
}


vector<string> graphicsMapper::GetLeaderPortraits(string cultureGroup, string ideology)
{
	auto mapping = leaderPortraitMappings.element.find(cultureGroup);
	if (mapping != leaderPortraitMappings.element.end())
	{
		auto portraits = mapping->second.element.find(ideology);
		if (portraits != mapping->second.element.end())
		{
			return portraits->second;
		}
	}

	vector<string> genericPortait;
	genericPortait.push_back("gfx/leaders/leader_unknown.dds");
	return genericPortait;
}


string graphicsMapper::GetIdeologyMinisterPortrait(string cultureGroup, string ideology)
{
	vector<string> portraits = GetIdeologyMinisterPortraits(cultureGroup, ideology);

	std::uniform_int_distribution<int> firstNameGen(0, portraits.size() - 1);
	return portraits[firstNameGen(rng)];
}


vector<string> graphicsMapper::GetIdeologyMinisterPortraits(string cultureGroup, string ideology)
{
	auto mapping = ideologyMinisterMappings.element.find(cultureGroup);
	if (mapping != ideologyMinisterMappings.element.end())
	{
		auto portraits = mapping->second.element.find(ideology);
		if (portraits != mapping->second.element.end())
		{
			return portraits->second;
		}
	}

	vector<string> genericPortait;
	genericPortait.push_back("gfx/interface/ideas/idea_unknown.dds");
	return genericPortait;
}


string graphicsMapper::GetGraphicalCulture(const string& cultureGroup)
{
	auto itr = graphicalCultureMap.find(cultureGroup);
	if (itr != graphicalCultureMap.end())
	{
		return itr->second;
	}
	else
	{
		return "";
	}
}


string graphicsMapper::Get2dGraphicalCulture(const string& cultureGroup)
{
	auto itr = graphicalCulture2dMap.find(cultureGroup);
	if (itr != graphicalCulture2dMap.end())
	{
		return itr->second;
	}
	else
	{
		return "";
	}
}