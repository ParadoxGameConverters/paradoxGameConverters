/*Copyright (c) 2018 The Paradox Game Converters Project

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
#include "../EU4World/Areas.h"
#include "../EU4World/Region.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "../Configuration.h"
#include <algorithm>
#include <fstream>
#include <string>
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
	EU4World::areas installedAreas(Configuration::getEU4Path() + "/map/area.txt");
	initEU4RegionMap(installedAreas, (Configuration::getEU4Path() + "/map/region.txt"));

	for (auto itr: Configuration::getEU4Mods())
	{
		if (!Utils::DoesFileExist(itr + "/map/area.txt") || !Utils::DoesFileExist(itr + "/map/region.txt"))
		{
			continue;
		}

		EU4World::areas modAreas(itr + "/map/area.txt");
		initEU4RegionMap(modAreas, (itr + "/map/region.txt"));
	}
}


void EU4RegionMapper::initEU4RegionMap(const EU4World::areas& areas, const std::string& regionsFilename)
{
	regions.clear();

	registerKeyword(std::regex("\\w+_region"), [this, areas](const std::string& areaName, std::istream& areasFile)
		{
			EU4World::region newRegion(areasFile);
			newRegion.addProvinces(areas);
			regions.insert(make_pair(areaName, newRegion));
		}
	);

	parseFile(regionsFilename);

	auto theAreas = areas.getAreas();
	std::for_each(theAreas.begin(), theAreas.end(), [this](const std::pair<std::string, EU4World::area>& theArea)
		{
			regions.insert(make_pair(theArea.first, EU4World::region(theArea.second.getProvinces())));
		}
	);
}


bool EU4RegionMapper::ProvinceInRegion(int province, const string& regionName)
{
	auto candidateRegions = getRegionsForProvince(province);
	if (candidateRegions.count(regionName) > 0)
	{
		return true;
	}

	auto region = regions.find(regionName);
	if (region != regions.end())
	{
		return region->second.containsProvince(province);
	}
	else
	{
		return false;
	}
}


set<string> EU4RegionMapper::getRegionsForProvince(int province)
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