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



#include "StateMapper.h"
#include "Log.h"
#include "../Configuration.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParser8859_15.h"



stateMapper* stateMapper::instance = NULL;



stateMapper::stateMapper():
	stateMap(),
	stateIdMap(),
	stateToCapitalMap()
{
	LOG(LogLevel::Info) << "Importing Vic2 states";
	bool stateMapInitialized = false;

	for (auto itr: Configuration::getVic2Mods())
	{
		if (Utils::DoesFileExist(Configuration::getV2Path() + "/mod/" + itr + "/map/region.txt"))
		{
			auto parsedMappingsFile = parser_8859_15::doParseFile((Configuration::getV2Path() + "/mod/" + itr + "/map/region.txt"));
			if (parsedMappingsFile)
			{
				initStateMap(parsedMappingsFile);
				stateMapInitialized = true;
				break;
			}
		}
	}
	if (!stateMapInitialized)
	{
		auto parsedMappingsFile = parser_8859_15::doParseFile((Configuration::getV2Path() + "/map/region.txt"));
		if (parsedMappingsFile)
		{
			initStateMap(parsedMappingsFile);
		}
		else
		{
			LOG(LogLevel::Error) << "Could not import " << Configuration::getV2Path() << "/map/region.txt";
			exit(-1);
		}
	}
}



void stateMapper::initStateMap(shared_ptr<Object> parsedMappingsFile)
{
	vector<shared_ptr<Object>> leafObjs = parsedMappingsFile->getLeaves();

	for (auto leafObj: leafObjs)
	{
		string ID = leafObj->getKey();
		vector<string> provinceNums = leafObj->getTokens();
		unordered_set<int> neighbors;

		for (auto provNum: provinceNums)
		{
			neighbors.insert(stoi(provNum));
			stateIdMap.insert(make_pair(stoi(provNum), ID));
		}

		for (auto neighbor: neighbors)
		{
			stateMap.insert(make_pair(neighbor, neighbors));
		}

		if (provinceNums.size() > 0)
		{
			stateToCapitalMap.insert(make_pair(ID, stoi(provinceNums.front())));
		}
	}
}


optional<int> stateMapper::GetCapitalProvince(const string& stateID) const
{
	auto mapping = stateToCapitalMap.find(stateID);
	if (mapping != stateToCapitalMap.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}