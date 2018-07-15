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



#include "SupplyZones.h"
#include "../Configuration.h"
#include "HoI4State.h"
#include "HoI4States.h"
#include "HoI4SupplyZone.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"


class supplyArea: commonItems::parser
{
	public:
		explicit supplyArea(std::istream& theStream);

		auto getID() const { return ID; }
		auto getValue() const { return value; }
		auto getStates() const { return states; }

	private:
		int ID = 0;
		int value = 0;
		std::vector<int> states;
};


supplyArea::supplyArea(std::istream& theStream)
{
	registerKeyword(std::regex("id"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt idInt(theStream);
		ID = idInt.getInt();
	});
	registerKeyword(std::regex("value"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt valueInt(theStream);
		value = valueInt.getInt();
	});
	registerKeyword(std::regex("states"), [this](const std::string& unused, std::istream& theStream){
		commonItems::intList stateInts(theStream);
		states = stateInts.getInts();
	});
	registerKeyword(std::regex("[a-zA-Z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}



HoI4::SupplyZones::SupplyZones(const std::map<int, HoI4::State*>& defaultStates):
	defaultStateToProvinceMap(),
	supplyZonesFilenames(),
	supplyZones(),
	provinceToSupplyZoneMap()
{
	LOG(LogLevel::Info) << "Importing supply zones";
	importStates(defaultStates);

	registerKeyword(std::regex("supply_area"), [this](const std::string& unused, std::istream& theStream){
		supplyArea area(theStream);
		auto ID = area.getID();

		HoI4SupplyZone* newSupplyZone = new HoI4SupplyZone(ID, area.getValue());
		supplyZones.insert(make_pair(ID, newSupplyZone));

		for (auto state: area.getStates())
		{
			auto mapping = defaultStateToProvinceMap.find(state);
			for (auto province : mapping->second)
			{
				provinceToSupplyZoneMap.insert(make_pair(province, ID));
			}
		}
	});

	std::set<std::string> supplyZonesFiles;
	Utils::GetAllFilesInFolder(theConfiguration.getHoI4Path() + "/map/supplyareas", supplyZonesFiles);
	for (auto supplyZonesFile: supplyZonesFiles)
	{
		int num = stoi(supplyZonesFile.substr(0, supplyZonesFile.find_first_of('-')));
		supplyZonesFilenames.insert(make_pair(num, supplyZonesFile));
		parseFile(theConfiguration.getHoI4Path() + "/map/supplyareas/" + supplyZonesFile);
	}
}


void HoI4::SupplyZones::importStates(const std::map<int, HoI4::State*>& defaultStates)
{
	for (auto state: defaultStates)
	{
		defaultStateToProvinceMap.insert(make_pair(state.first, state.second->getProvinces()));
	}
}


void HoI4::SupplyZones::output()
{
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/map/supplyareas"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/map/supplyareas";
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


void HoI4::SupplyZones::convertSupplyZones(const HoI4States* states)
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