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



#include "HoI4Buildings.h"
#include "HoI4Province.h"
#include "CoastalProvinces.h"
#include "MapData.h"
#include "../Configuration.h"
#include "Log.h"
#include <iomanip>



HoI4::Building::Building(int _stateID, const std::string& _type, buildingPosition& _position, std::optional<int> _connectingSeaProvince):
	stateID(_stateID),
	type(_type),
	position(_position)
{
	if (_connectingSeaProvince)
	{
		connectingSeaProvince = *_connectingSeaProvince;
	}
}


std::ostream& HoI4::operator << (std::ostream& out, const HoI4::Building& building)
{
	out << building.stateID << ";" << building.type << ";";
	out << std::fixed << std::setprecision(2) << building.position.xCoordinate << ';' << building.position.yCoordinate << ';' << building.position.zCoordinate << ';' << building.position.rotation << ';';
	out << building.connectingSeaProvince << '\n';

	return out;
}


HoI4::Buildings::Buildings(const std::map<int, int>& provinceToStateIDMap, const coastalProvinces& theCoastalProvinces)
{
	LOG(LogLevel::Info) << "Creating buildings";

	importDefaultBuildings();
	placeBuildings(provinceToStateIDMap, theCoastalProvinces);
}


void HoI4::Buildings::importDefaultBuildings()
{
	std::ifstream buildingsFile(theConfiguration.getHoI4Path() + "/map/buildings.txt");
	if (!buildingsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << theConfiguration.getHoI4Path() << "/map/buildings.txt";
		exit(-1);
	}

	while (!buildingsFile.eof())
	{
		std::string line;
		getline(buildingsFile, line);
		processLine(line);
	}
}


void HoI4::Buildings::processLine(const std::string& line)
{
	std::regex pattern("(.+);(.+);(.+);(.+);(.+);(.+);(.+)");
	std::smatch matches;
	if (regex_match(line, matches, pattern))
	{
		if (matches[2] == "arms_factory")
		{
			importDefaultBuilding(matches, defaultArmsFactories);
		}
		else if (matches[2] == "industrial_complex")
		{
			importDefaultBuilding(matches, defaultIndustrialComplexes);
		}
		else if (matches[2] == "air_base")
		{
			importDefaultBuilding(matches, defaultAirBase);
		}
		else if (matches[2] == "naval_base")
		{
			importDefaultBuilding(matches, defaultNavalBases);
		}
		else if (matches[2] == "bunker")
		{
			importDefaultBuilding(matches, defaultBunkers);
		}
		else if (matches[2] == "coastal_bunker")
		{
			importDefaultBuilding(matches, defaultCoastalBunkers);
		}
		else if (matches[2] == "dockyard")
		{
			importDefaultBuilding(matches, defaultDockyards);
		}
		else if (matches[2] == "anti_air_building")
		{
			importDefaultBuilding(matches, defaultAntiAirs);
		}
		else if (matches[2] == "synthetic_refinery")
		{
			importDefaultBuilding(matches, defaultSyntheticRefineries);
		}
		else if (matches[2] == "nuclear_reactor")
		{
			importDefaultBuilding(matches, defaultNuclearReactors);
		}
	}
}


void HoI4::Buildings::importDefaultBuilding(const std::smatch& matches, defaultPositions& positions)
{
	buildingPosition position;
	position.xCoordinate = stof(matches[3].str());
	position.yCoordinate = stof(matches[4].str());
	position.zCoordinate = stof(matches[5].str());
	position.rotation = stof(matches[6].str());

	int connectingSeaProvince = stoi(matches[7].str());

	auto province = MapData::getProvinceNumber(position.xCoordinate, position.zCoordinate);
	if (province)
	{
		auto key = std::make_pair(*province, connectingSeaProvince);
		positions[key] = position;
	}
}


void HoI4::Buildings::placeBuildings(const std::map<int, int>& provinceToStateIDMap, const coastalProvinces& theCoastalProvinces)
{
	auto coastalProvinces = theCoastalProvinces.getCoastalProvinces();
	for (auto province: coastalProvinces)
	{
		auto provinceToStateMapping = provinceToStateIDMap.find(province.first);
		if (provinceToStateMapping == provinceToStateIDMap.end())
		{
			LOG(LogLevel::Warning) << "Could not find state for province " << province.first << ". Naval base not set.";
			continue;
		}

		addNavalBase(provinceToStateMapping->second, province);
	}

	for (auto provinceAndStateID: provinceToStateIDMap)
	{
		addBunker(provinceAndStateID.second, provinceAndStateID.first);
	}

	for (auto province: coastalProvinces)
	{
		auto provinceToStateMapping = provinceToStateIDMap.find(province.first);
		if (provinceToStateMapping == provinceToStateIDMap.end())
		{
			LOG(LogLevel::Warning) << "Could not find state for province " << province.first << ". Coastal bunker not set.";
			continue;
		}

		addCoastalBunker(provinceToStateMapping->second, province);
	}
}


void HoI4::Buildings::addNavalBase(int stateID, const std::pair<int, std::vector<int>>& province)
{
	buildingPosition position;
	bool positionUnset = true;
	int connectingSeaProvince = 0;
	for (auto seaProvince: province.second)
	{
		auto defaultNavalBase = defaultNavalBases.find(std::make_pair(province.first, seaProvince));
		if (defaultNavalBase != defaultNavalBases.end())
		{
			position = defaultNavalBase->second;
			connectingSeaProvince = seaProvince;
			positionUnset = false;
		}
	}

	if (positionUnset)
	{
		connectingSeaProvince = province.second[0];
		auto possiblePosition = MapData::getBorderCenter(province.first, province.second[0]);
		if (!possiblePosition)
		{
			LOG(LogLevel::Warning) << "Could not find position for province " << province.first << ". Naval base not set.";
			return;
		}

		position.xCoordinate = possiblePosition->first;
		position.yCoordinate = 11.0;
		position.zCoordinate = possiblePosition->second;
		position.rotation = 0.0;

		if (theConfiguration.getDebug())
		{
			LOG(LogLevel::Warning) << "The naval base from " << province.first << " to " << connectingSeaProvince << " at (" << position.xCoordinate << ", " << position.zCoordinate << ") did not have a location in default HoI4.";
		}
	}

	HoI4::Building* newBuilding = new HoI4::Building(stateID, "naval_base", position, connectingSeaProvince);
	buildings.insert(std::make_pair(stateID, newBuilding));
}


void HoI4::Buildings::addBunker(int stateID, int province)
{
	buildingPosition position;
	bool positionUnset = true;

	auto defaultBunker = defaultBunkers.find(std::make_pair(province, 0));
	if (defaultBunker != defaultBunkers.end())
	{
		position = defaultBunker->second;
		positionUnset = false;
	}

	if (positionUnset)
	{
		auto possiblePosition = MapData::getBorderCenter(province, 0);
		if (!possiblePosition)
		{
			LOG(LogLevel::Warning) << "Could not find position for province " << province << ". Bunker not set.";
			return;
		}

		position.xCoordinate = possiblePosition->first;
		position.yCoordinate = 11.0;
		position.zCoordinate = possiblePosition->second;
		position.rotation = 0.0;

		if (theConfiguration.getDebug())
		{
			LOG(LogLevel::Warning) << "The bunker in " << province << " at (" << position.xCoordinate << ", " << position.zCoordinate << ") did not have a location in default HoI4.";
		}
	}

	HoI4::Building* newBuilding = new HoI4::Building(stateID, "bunker", position, 0);
	buildings.insert(std::make_pair(stateID, newBuilding));
}


void HoI4::Buildings::addCoastalBunker(int stateID, const std::pair<int, std::vector<int>>& province)
{
	buildingPosition position;
	bool positionUnset = true;

	auto defaultBunker = defaultCoastalBunkers.find(std::make_pair(province.first, 0));
	if (defaultBunker != defaultCoastalBunkers.end())
	{
		position = defaultBunker->second;
		positionUnset = false;
	}

	if (positionUnset)
	{
		auto possiblePosition = MapData::getBorderCenter(province.first, province.second[0]);
		if (!possiblePosition)
		{
			LOG(LogLevel::Warning) << "Could not find position for province " << province.first << ". Coastal bunker not set.";
			return;
		}

		position.xCoordinate = possiblePosition->first;
		position.yCoordinate = 11.0;
		position.zCoordinate = possiblePosition->second;
		position.rotation = 0.0;

		if (theConfiguration.getDebug())
		{
			LOG(LogLevel::Warning) << "The coastal bunker in " << province.first << " at (" << position.xCoordinate << ", " << position.zCoordinate << ") did not have a location in default HoI4.";
		}
	}

	HoI4::Building* newBuilding = new HoI4::Building(stateID, "coastal_bunker", position, 0);
	buildings.insert(std::make_pair(stateID, newBuilding));
}


void HoI4::Buildings::output() const
{
	std::ofstream out("output/" + theConfiguration.getOutputName() + "/map/buildings.txt");
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open output/" << theConfiguration.getOutputName() << "/map/buildings.txt";
		exit(-1);
	}

	for (auto building: buildings)
	{
		out << *(building.second);
	}
}