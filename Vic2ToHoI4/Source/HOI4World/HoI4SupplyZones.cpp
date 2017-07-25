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



#include "HoI4SupplyZones.h"
#include "../Configuration.h"
#include "HoI4State.h"
#include "HoI4States.h"
#include "HoI4SupplyZone.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include <set>
using namespace std;



HoI4SupplyZones::HoI4SupplyZones()
{
	LOG(LogLevel::Info) << "Importing supply zones";
	importStates();

	set<string> supplyZonesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/map/supplyareas", supplyZonesFiles);
	for (auto supplyZonesFile: supplyZonesFiles)
	{
		importSupplyZone(supplyZonesFile);
	}
}


void HoI4SupplyZones::importStates()
{
	set<string> statesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/history/states", statesFiles);
	for (auto stateFile: statesFiles)
	{
		int num = stoi(stateFile.substr(0, stateFile.find_first_of('-')));

		shared_ptr<Object> fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/history/states/" + stateFile);
		if (fileObj == nullptr)
		{
			LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/history/states/" << stateFile;
			exit(-1);
		}
		auto stateObj = fileObj->getValue("state");
		auto provincesObj = stateObj[0]->getValue("provinces");
		auto tokens = provincesObj[0]->getTokens();
		vector<int> provinces;
		for (auto provinceNumString: tokens)
		{
			provinces.push_back(stoi(provinceNumString));
		}

		defaultStateToProvinceMap.insert(make_pair(num, provinces));
	}
}


void HoI4SupplyZones::importSupplyZone(const string& supplyZonesFile)
{
	int num = stoi(supplyZonesFile.substr(0, supplyZonesFile.find_first_of('-')));
	supplyZonesFilenames.insert(make_pair(num, supplyZonesFile));

	shared_ptr<Object> fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/map/supplyareas/" + supplyZonesFile);
	if (fileObj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/map/supplyareas/" << supplyZonesFile;
		exit(-1);
	}
	auto supplyAreaObj = fileObj->getValue("supply_area");
	int ID = stoi(supplyAreaObj[0]->getLeaf("id"));
	int value = stoi(supplyAreaObj[0]->getLeaf("value"));

	HoI4SupplyZone* newSupplyZone = new HoI4SupplyZone(ID, value);
	supplyZones.insert(make_pair(ID, newSupplyZone));

	mapProvincesToSupplyZone(ID, supplyAreaObj[0]);
}


void HoI4SupplyZones::mapProvincesToSupplyZone(int ID, shared_ptr<Object> supplyAreaObj)
{
	auto statesObj = supplyAreaObj->getValue("states");
	for (auto idString: statesObj[0]->getTokens())
	{
		auto mapping = defaultStateToProvinceMap.find(stoi(idString));
		for (auto province : mapping->second)
		{
			provinceToSupplyZoneMap.insert(make_pair(province, ID));
		}
	}
}


void HoI4SupplyZones::output()
{
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/map/supplyareas"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/map/supplyareas";
		exit(-1);
	}

	for (auto zone: supplyZones)
	{
		auto filenameMap = supplyZonesFilenames.find(zone.first);
		if (filenameMap != supplyZonesFilenames.end())
		{
			zone.second->output(filenameMap->second);
		}
	}
}


void HoI4SupplyZones::convertSupplyZones(const HoI4States* states)
{
	for (auto state: states->getStates())
	{
		for (auto province : state.second->getProvinces())
		{
			auto mapping = provinceToSupplyZoneMap.find(province);
			if (mapping != provinceToSupplyZoneMap.end())
			{
				auto supplyZone = supplyZones.find(mapping->second);
				if (supplyZone != supplyZones.end())
				{
					supplyZone->second->addState(state.first);
					break;
				}
			}
		}
	}
}