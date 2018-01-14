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



#include "EU4RegionMapper.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "../Configuration.h"
#include <fstream>
#include <vector>
using namespace std;




EU4RegionMapper* EU4RegionMapper::instance = nullptr;



EU4RegionMapper::EU4RegionMapper()
{
	LOG(LogLevel::Info) << "Parsing EU4 regions";
	attemptOldVersion();

	if (EU4RegionsMap.empty()) // if it failed, we're using the new regions format
	{
		doNewVersion();
	}
}


void EU4RegionMapper::attemptOldVersion()
{
	shared_ptr<Object> regionsObj = parser_UTF8::doParseFile((Configuration::getEU4Path() + "/map/region.txt").c_str());
	if (regionsObj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not parse file " << Configuration::getEU4Path() << "/map/region.txt";
		exit(-1);
	}
	if (regionsObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse region.txt";
		exit (-1);
	}
	initEU4RegionMapOldVersion(regionsObj);

	for (auto itr: Configuration::getEU4Mods())
	{
		string modRegionFile(itr + "/map/region.txt");
		if (Utils::DoesFileExist(modRegionFile))
		{
			regionsObj = parser_UTF8::doParseFile(modRegionFile.c_str());
			if (regionsObj == nullptr)
			{
				LOG(LogLevel::Error) << "Could not parse file " << modRegionFile;
				exit(-1);
			}
			initEU4RegionMapOldVersion(regionsObj);
		}
	}
}


void EU4RegionMapper::initEU4RegionMapOldVersion(shared_ptr<Object> obj)
{
	EU4RegionsMap.clear();

	vector<shared_ptr<Object>> regionsObjs = obj->getLeaves();
	for (auto regionObj: regionsObjs)
	{
		string regionName = regionObj->getKey();
		vector<string> provinceStrings = regionObj->getTokens();
		for (auto provinceString: provinceStrings)
		{
			int provinceNum = stoi(provinceString);
			insertMapping(provinceNum, regionName);
		}
	}
}


void EU4RegionMapper::insertMapping(int provinceNumber, string regionName)
{
	auto mapping = EU4RegionsMap.find(provinceNumber);
	if (mapping == EU4RegionsMap.end())
	{
		set<string> newRegions;
		newRegions.insert(regionName);
		EU4RegionsMap.insert(make_pair(provinceNumber, newRegions));
	}
	else
	{
		mapping->second.insert(regionName);
	}
}


void EU4RegionMapper::doNewVersion()
{
	makeWorkingAreaTxt(Configuration::getEU4Path());
	shared_ptr<Object> areaObj = parser_UTF8::doParseFile("area.txt");
	if (areaObj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not parse file " << Configuration::getEU4Path() << "/map/area.txt";
		exit(-1);
	}
	if (areaObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse area.txt";
		exit (-1);
	}

	shared_ptr<Object> regionsObj = parser_UTF8::doParseFile((Configuration::getEU4Path() + "/map/region.txt").c_str());
	if (regionsObj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not parse file " << Configuration::getEU4Path() << "/map/region.txt";
		exit(-1);
	}
	if (regionsObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse region.txt";
		exit (-1);
	}
	initEU4RegionMap(regionsObj, areaObj);

	for (auto itr: Configuration::getEU4Mods())
	{
		if (!Utils::DoesFileExist(itr + "/map/area.txt") && !Utils::DoesFileExist(itr + "/map/region.txt"))
		{
			continue;
		}

		makeWorkingAreaTxt(itr);
		shared_ptr<Object> areaObj = parser_UTF8::doParseFile("area.txt");
		if (areaObj == nullptr)
		{
			LOG(LogLevel::Error) << "Could not parse file " << Configuration::getEU4Path() << "/map/area.txt";
			exit(-1);
		}
		if (areaObj->getLeaves().size() < 1)
		{
			LOG(LogLevel::Error) << "Failed to parse area.txt";
			exit (-1);
		}

		regionsObj = parser_UTF8::doParseFile(itr + "/map/region.txt");
		if (regionsObj == nullptr)
		{
			LOG(LogLevel::Error) << "Could not parse file " << itr << "/map/region.txt";
			exit(-1);
		}
		initEU4RegionMap(regionsObj, areaObj);
	}
}


void EU4RegionMapper::makeWorkingAreaTxt(const string& path)
{
	ifstream original(path + "/map/area.txt");
	ofstream copy("area.txt");

	char buffer[256];
	while (!original.eof())
	{
		original.getline(buffer, sizeof(buffer));
		string line(buffer);

		if (line.find("color =") == string::npos)
		{
			copy << line << "\n";
		}
	}

	original.close();
	copy.close();
}


void EU4RegionMapper::initEU4RegionMap(shared_ptr<Object> regionsObj, shared_ptr<Object> areasObj)
{
	auto areaToProvincesMapping = getAreaToProvincesMapping(areasObj);

	EU4RegionsMap.clear();
	vector<shared_ptr<Object>> regionObjs = regionsObj->getLeaves();
	for (auto regionObj: regionObjs)
	{
		string regionName = regionObj->getKey();
		vector<shared_ptr<Object>> areasObj = regionObj->getValue("areas");
		if (areasObj.size() > 0)
		{
			vector<string> areas = areasObj[0]->getTokens();
			for (auto area: areas)
			{
				auto areaMapping = areaToProvincesMapping.find(area);
				for (auto provinceNum: areaMapping->second)
				{
					insertMapping(provinceNum, areaMapping->first);
					insertMapping(provinceNum, regionName);
				}
			}
		}
	}
}


map<string, vector<int>> EU4RegionMapper::getAreaToProvincesMapping(shared_ptr<Object> areasObj)
{
	map<string, vector<int>> areaToProvincesMapping;

	for (auto areaObj: areasObj->getLeaves())
	{
		string areaName = areaObj->getKey();

		vector<int> provinces;
		for (auto provinceString: areaObj->getTokens())
		{
			int provinceNum = stoi(provinceString);
			provinces.push_back(provinceNum);
		}

		areaToProvincesMapping.insert(make_pair(areaName, provinces));
	}

	return areaToProvincesMapping;
}


bool EU4RegionMapper::ProvinceInRegion(int province, const string& region)
{
	auto regions = getRegions(province);
	return (regions.count(region) > 0);
}


set<string> EU4RegionMapper::getRegions(int province)
{
	auto mapping = EU4RegionsMap.find(province);
	if (mapping != EU4RegionsMap.end())
	{
		return mapping->second;
	}
	else
	{
		set<string> empty;
		return empty;
	}
}