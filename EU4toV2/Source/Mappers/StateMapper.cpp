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
#include "../Configuration.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParser8859_15.h"



stateMapper* stateMapper::instance = NULL;



stateMapper::stateMapper()
{
	LOG(LogLevel::Info) << "Parsing region structure";

	string filename;
	if (Utils::DoesFileExist("./blankMod/output/map/region.txt"))
	{
		filename = "./blankMod/output/map/region.txt";
	}
	else
	{
		filename = Configuration::getV2Path() + "/map/region.txt";
	}

	shared_ptr<Object> Vic2RegionsObj = parser_8859_15::doParseFile(filename);
	if (Vic2RegionsObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << filename;
		exit(-1);
	}
	if (Vic2RegionsObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Could not parse region.txt";
		exit(-1);
	}
	initStateMap(Vic2RegionsObj);
}

void stateMapper::initStateMap(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> states = obj->getLeaves();

	for (unsigned int stateIndex = 0; stateIndex < states.size(); stateIndex++)
	{
		vector<string> provinces = states[stateIndex]->getTokens();
		vector<int> neighbors;

		for (auto province : provinces)
		{
			neighbors.push_back(stoi(province));
			stateIndexMap.insert(make_pair(stoi(province), stateIndex));
		}

		for (auto neighbor : neighbors)
		{
			stateProvincesMap.insert(make_pair(neighbor, neighbors));
		}
	}
}

vector<int> stateMapper::GetOtherProvincesInState(int province)
{
	auto mapping = stateProvincesMap.find(province);
	if (mapping != stateProvincesMap.end())
	{
		return mapping->second;
	}
	else
	{
		vector<int> empty;
		return empty;
	}
}

int stateMapper::GetStateIndex(int province)
{
	auto mapping = stateIndexMap.find(province);
	if (mapping != stateIndexMap.end())
	{
		return mapping->second;
	}
	else
	{
		return -1;
	}
}