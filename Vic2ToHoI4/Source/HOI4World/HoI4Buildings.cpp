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
#include "HoI4States.h"
#include "HoI4State.h"
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


HoI4::Buildings::Buildings(const HoI4States& theStates, const coastalProvinces& theCoastalProvinces, MapData& theMapData)
{
	LOG(LogLevel::Info) << "Creating buildings";

	importDefaultBuildings(theMapData);
	placeBuildings(theStates, theCoastalProvinces, theMapData);
}


void HoI4::Buildings::importDefaultBuildings(MapData& theMapData)
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
		processLine(line, theMapData);
	}
}


void HoI4::Buildings::processLine(const std::string& line, MapData& theMapData)
{
	std::regex pattern("(.+);(.+);(.+);(.+);(.+);(.+);(.+)");
	std::smatch matches;
	if (regex_match(line, matches, pattern))
	{
		if (matches[2] == "arms_factory")
		{
			importDefaultBuilding(matches, defaultArmsFactories, theMapData);
		}
		else if (matches[2] == "industrial_complex")
		{
			importDefaultBuilding(matches, defaultIndustrialComplexes, theMapData);
		}
		else if (matches[2] == "air_base")
		{
			importDefaultBuilding(matches, defaultAirBases, theMapData);
		}
		else if (matches[2] == "naval_base")
		{
			importDefaultBuilding(matches, defaultNavalBases, theMapData);
		}
		else if (matches[2] == "bunker")
		{
			importDefaultBuilding(matches, defaultBunkers, theMapData);
		}
		else if (matches[2] == "coastal_bunker")
		{
			importDefaultBuilding(matches, defaultCoastalBunkers, theMapData);
		}
		else if (matches[2] == "dockyard")
		{
			importDefaultBuilding(matches, defaultDockyards, theMapData);
		}
		else if (matches[2] == "anti_air_building")
		{
			importDefaultBuilding(matches, defaultAntiAirs, theMapData);
		}
		else if (matches[2] == "synthetic_refinery")
		{
			importDefaultBuilding(matches, defaultSyntheticRefineries, theMapData);
		}
		else if (matches[2] == "nuclear_reactor")
		{
			importDefaultBuilding(matches, defaultNuclearReactors, theMapData);
		}
	}
}


void HoI4::Buildings::importDefaultBuilding(const std::smatch& matches, defaultPositions& positions, MapData& theMapData)
{
	buildingPosition position;
	position.xCoordinate = stof(matches[3].str());
	position.yCoordinate = stof(matches[4].str());
	position.zCoordinate = stof(matches[5].str());
	position.rotation = stof(matches[6].str());

	int connectingSeaProvince = stoi(matches[7].str());

	auto province = theMapData.getProvinceNumber(position.xCoordinate, position.zCoordinate);
	if (province)
	{
		auto key = std::make_pair(*province, connectingSeaProvince);
		positions[key] = position;
	}
}


void HoI4::Buildings::placeBuildings(const HoI4States& theStates, const coastalProvinces& theCoastalProvinces, const MapData& theMapData)
{
	auto provinceToStateIDMap = theStates.getProvinceToStateIDMap();
	auto actualCoastalProvinces = theCoastalProvinces.getCoastalProvinces();

	placeAirports(theStates, theMapData);
	placeArmsFactories(theStates, theMapData);
	placeIndustrialComplexes(theStates, theMapData);
	placeNavalBases(provinceToStateIDMap, actualCoastalProvinces, theMapData);
	placeBunkers(provinceToStateIDMap, theMapData);
	placeCoastalBunkers(provinceToStateIDMap, actualCoastalProvinces, theMapData);
	placeDockyards(theStates, theCoastalProvinces, actualCoastalProvinces, theMapData);
	placeAntiAir(theStates, theMapData);
	placeSyntheticRefineries(theStates, theMapData);
	placeNuclearReactors(theStates, theMapData);
}


void HoI4::Buildings::placeArmsFactories(const HoI4States& theStates, const MapData& theMapData)
{
	for (auto state: theStates.getStates())
	{
		int numPlaced = 0;
		for (auto theProvince: state.second->getProvinces())
		{
			auto possibleArmsFactory = defaultArmsFactories.find(make_pair(theProvince, 0));
			if (possibleArmsFactory != defaultArmsFactories.end())
			{
				auto position = possibleArmsFactory->second;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "arms_factory", position, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
				numPlaced++;

				if (numPlaced > 3)
				{
					break;
				}
			}
		}
		while (numPlaced < 6)
		{
			for (auto theProvince: state.second->getProvinces())
			{
				auto theProvincePoints = theMapData.getProvincePoints(theProvince);
				if (theProvincePoints)
				{
					auto centermostPoint = theProvincePoints->getCentermostPoint();
					HoI4::buildingPosition thePosition;
					thePosition.xCoordinate = centermostPoint.first;
					thePosition.yCoordinate = 11.0;
					thePosition.zCoordinate = centermostPoint.second;
					thePosition.rotation = 0;
					HoI4::Building* newBuilding = new HoI4::Building(state.first, "arms_factory", thePosition, 0);
					buildings.insert(std::make_pair(state.first, newBuilding));
					numPlaced++;
				}
				else
				{
					LOG(LogLevel::Warning) << "Province " << theProvince << " did not have any points. Arms factories not fully set in state " << state.first << ".";
					break;
				}

				if (numPlaced >=6)
				{
					break;
				}
			}
		}
	}
}


void HoI4::Buildings::placeIndustrialComplexes(const HoI4States& theStates, const MapData& theMapData)
{
	for (auto state: theStates.getStates())
	{
		int numPlaced = 0;
		for (auto theProvince: state.second->getProvinces())
		{
			auto possibleIndustrialComplex = defaultIndustrialComplexes.find(make_pair(theProvince, 0));
			if (possibleIndustrialComplex != defaultIndustrialComplexes.end())
			{
				auto position = possibleIndustrialComplex->second;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "industrial_complex", position, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
				numPlaced++;

				if (numPlaced > 3)
				{
					break;
				}
			}
		}
		while (numPlaced < 6)
		{
			for (auto theProvince: state.second->getProvinces())
			{
				auto theProvincePoints = theMapData.getProvincePoints(theProvince);
				if (theProvincePoints)
				{
					auto centermostPoint = theProvincePoints->getCentermostPoint();
					HoI4::buildingPosition thePosition;
					thePosition.xCoordinate = centermostPoint.first;
					thePosition.yCoordinate = 11.0;
					thePosition.zCoordinate = centermostPoint.second;
					thePosition.rotation = 0;
					HoI4::Building* newBuilding = new HoI4::Building(state.first, "industrial_complex", thePosition, 0);
					buildings.insert(std::make_pair(state.first, newBuilding));
					numPlaced++;
				}
				else
				{
					LOG(LogLevel::Warning) << "Province " << theProvince << " did not have any points. Industrial complexes not fully set in state " << state.first << ".";
					break;
				}

				if (numPlaced >=6)
				{
					break;
				}
			}
		}
	}
}


void HoI4::Buildings::placeAirports(const HoI4States& theStates, const MapData& theMapData)
{
	for (auto state: theStates.getStates())
	{
		bool airportPlaced = false;
		for (auto theProvince: state.second->getProvinces())
		{
			auto possibleAirbase = defaultAirBases.find(make_pair(theProvince, 0));
			if (possibleAirbase != defaultAirBases.end())
			{
				auto position = possibleAirbase->second;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "air_base", position, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
				airportLocations.insert(make_pair(state.first, theProvince));
				airportPlaced = true;
				break;
			}
		}
		if (!airportPlaced)
		{
			auto theProvince = *state.second->getProvinces().begin();
			bool pause = false;
			airportLocations.insert(make_pair(state.first, theProvince));

			auto theProvincePoints = theMapData.getProvincePoints(theProvince);
			if (theProvincePoints)
			{
				auto centermostPoint = theProvincePoints->getCentermostPoint();
				HoI4::buildingPosition thePosition;
				thePosition.xCoordinate = centermostPoint.first;
				thePosition.yCoordinate = 11.0;
				thePosition.zCoordinate = centermostPoint.second;
				thePosition.rotation = 0;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "air_base", thePosition, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
			}
			else
			{
				LOG(LogLevel::Warning) << "Province " << theProvince << " did not have any points. Airport not set for state " << state.first << ".";
			}
		}
	}
}


void HoI4::Buildings::placeAntiAir(const HoI4States& theStates, const MapData& theMapData)
{
	for (auto state: theStates.getStates())
	{
		int numPlaced = 0;
		for (auto theProvince: state.second->getProvinces())
		{
			auto possibleAntiAir = defaultAntiAirs.find(make_pair(theProvince, 0));
			if (possibleAntiAir != defaultAntiAirs.end())
			{
				auto position = possibleAntiAir->second;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "anti_air_building", position, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
				numPlaced++;

				if (numPlaced > 3)
				{
					break;
				}
			}
		}
		while (numPlaced < 3)
		{
			for (auto theProvince: state.second->getProvinces())
			{
				auto theProvincePoints = theMapData.getProvincePoints(theProvince);
				if (theProvincePoints)
				{
					auto centermostPoint = theProvincePoints->getCentermostPoint();
					HoI4::buildingPosition thePosition;
					thePosition.xCoordinate = centermostPoint.first;
					thePosition.yCoordinate = 11.0;
					thePosition.zCoordinate = centermostPoint.second;
					thePosition.rotation = 0;
					HoI4::Building* newBuilding = new HoI4::Building(state.first, "anti_air_building", thePosition, 0);
					buildings.insert(std::make_pair(state.first, newBuilding));
					numPlaced++;
				}
				else
				{
					LOG(LogLevel::Warning) << "Province " << theProvince << " did not have any points. Anti-air not fully set in state " << state.first << ".";
					break;
				}

				if (numPlaced >=3)
				{
					break;
				}
			}
		}
	}
}


void HoI4::Buildings::placeNavalBases(const std::map<int, int>& provinceToStateIDMap, std::map<int, std::vector<int>> actualCoastalProvinces, const MapData& theMapData)
{
	for (auto province: actualCoastalProvinces)
	{
		auto provinceToStateMapping = provinceToStateIDMap.find(province.first);
		if (provinceToStateMapping == provinceToStateIDMap.end())
		{
			LOG(LogLevel::Warning) << "Could not find state for province " << province.first << ". Naval base not set.";
			continue;
		}

		addNavalBase(provinceToStateMapping->second, province, theMapData);
	}
}


void HoI4::Buildings::placeBunkers(const std::map<int, int>& provinceToStateIDMap, const MapData& theMapData)
{
	for (auto provinceAndStateID: provinceToStateIDMap)
	{
		addBunker(provinceAndStateID.second, provinceAndStateID.first, theMapData);
	}
}


void HoI4::Buildings::addNavalBase(int stateID, const std::pair<int, std::vector<int>>& province, const MapData& theMapData)
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
		auto possiblePosition = theMapData.getSpecifiedBorderCenter(province.first, province.second[0]);
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


void HoI4::Buildings::addBunker(int stateID, int province, const MapData& theMapData)
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
		auto possiblePosition = theMapData.getAnyBorderCenter(province);
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


void HoI4::Buildings::placeCoastalBunkers(const std::map<int, int>& provinceToStateIDMap, std::map<int, std::vector<int>> actualCoastalProvinces, const MapData& theMapData)
{
	for (auto province: actualCoastalProvinces)
	{
		auto provinceToStateMapping = provinceToStateIDMap.find(province.first);
		if (provinceToStateMapping == provinceToStateIDMap.end())
		{
			LOG(LogLevel::Warning) << "Could not find state for province " << province.first << ". Coastal bunker not set.";
			continue;
		}

		addCoastalBunker(provinceToStateMapping->second, province, theMapData);
	}
}


void HoI4::Buildings::addCoastalBunker(int stateID, const std::pair<int, std::vector<int>>& province, const MapData& theMapData)
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
		auto possiblePosition = theMapData.getSpecifiedBorderCenter(province.first, province.second[0]);
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


void HoI4::Buildings::placeDockyards(const HoI4States& theStates, const coastalProvinces& theCoastalProvinces, std::map<int, std::vector<int>> actualCoastalProvinces, const MapData& theMapData)
{
	for (auto state: theStates.getStates())
	{
		bool dockyardPlaced = false;
		for (auto theProvince: state.second->getProvinces())
		{
			auto possibleDockyard = defaultDockyards.find(make_pair(theProvince, 0));
			if (possibleDockyard != defaultDockyards.end())
			{
				auto position = possibleDockyard->second;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "dockyard", position, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
				dockyardPlaced = true;
				break;
			}
		}
		if (!dockyardPlaced)
		{
			std::optional<int> theProvince;
			for (auto possibleProvince: state.second->getProvinces())
			{
				if (theCoastalProvinces.isProvinceCoastal(possibleProvince))
				{
					theProvince = possibleProvince;
					break;
				}
			}
			if (theProvince)
			{
				auto connectingSeaProvinces = actualCoastalProvinces.find(*theProvince);
				if (connectingSeaProvinces != actualCoastalProvinces.end())
				{
					auto centermostPoint = theMapData.getSpecifiedBorderCenter(*theProvince, connectingSeaProvinces->second[0]);
					if (centermostPoint)
					{
						HoI4::buildingPosition thePosition;
						thePosition.xCoordinate = centermostPoint->first;
						thePosition.yCoordinate = 11.0;
						thePosition.zCoordinate = centermostPoint->second;
						thePosition.rotation = 0;
						HoI4::Building* newBuilding = new HoI4::Building(state.first, "dockyard", thePosition, 0);
						buildings.insert(std::make_pair(state.first, newBuilding));
					}
					else
					{
						LOG(LogLevel::Warning) << "Province " << *theProvince << " did not have any points. Dockyard not set for state " << state.first << ".";
					}
				}
			}
		}
	}
}


void HoI4::Buildings::placeSyntheticRefineries(const HoI4States& theStates, const MapData& theMapData)
{
	for (auto state: theStates.getStates())
	{
		bool refineryPlaced = false;
		for (auto theProvince: state.second->getProvinces())
		{
			auto possibleRefinery = defaultSyntheticRefineries.find(make_pair(theProvince, 0));
			if (possibleRefinery != defaultSyntheticRefineries.end())
			{
				auto position = possibleRefinery->second;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "synthetic_refinery", position, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
				refineryPlaced = true;
				break;
			}
		}
		if (!refineryPlaced)
		{
			auto theProvince = *state.second->getProvinces().begin();
			auto theProvincePoints = theMapData.getProvincePoints(theProvince);
			if (theProvincePoints)
			{
				auto centermostPoint = theProvincePoints->getCentermostPoint();
				HoI4::buildingPosition thePosition;
				thePosition.xCoordinate = centermostPoint.first;
				thePosition.yCoordinate = 11.0;
				thePosition.zCoordinate = centermostPoint.second;
				thePosition.rotation = 0;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "synthetic_refinery", thePosition, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
			}
			else
			{
				LOG(LogLevel::Warning) << "Province " << theProvince << " did not have any points. Synthetic refinery not set for state " << state.first << ".";
			}
		}
	}
}


void HoI4::Buildings::placeNuclearReactors(const HoI4States& theStates, const MapData& theMapData)
{
	for (auto state: theStates.getStates())
	{
		bool reactorPlaced = false;
		for (auto theProvince: state.second->getProvinces())
		{
			auto possibleReactor = defaultNuclearReactors.find(make_pair(theProvince, 0));
			if (possibleReactor != defaultNuclearReactors.end())
			{
				auto position = possibleReactor->second;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "nuclear_reactor", position, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
				reactorPlaced = true;
				break;
			}
		}
		if (!reactorPlaced)
		{
			auto theProvince = *state.second->getProvinces().begin();
			auto theProvincePoints = theMapData.getProvincePoints(theProvince);
			if (theProvincePoints)
			{
				auto centermostPoint = theProvincePoints->getCentermostPoint();
				HoI4::buildingPosition thePosition;
				thePosition.xCoordinate = centermostPoint.first;
				thePosition.yCoordinate = 11.0;
				thePosition.zCoordinate = centermostPoint.second;
				thePosition.rotation = 0;
				HoI4::Building* newBuilding = new HoI4::Building(state.first, "nuclear_reactor", thePosition, 0);
				buildings.insert(std::make_pair(state.first, newBuilding));
			}
			else
			{
				LOG(LogLevel::Warning) << "Province " << theProvince << " did not have any points. Nuclear reactor not set for state " << state.first << ".";
			}
		}
	}
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
	out.close();

	ofstream airportsFile("output/" + theConfiguration.getOutputName() + "/map/airports.txt");
	if (!airportsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create output/" << theConfiguration.getOutputName() << "/map/airports.txt";
		exit(-1);
	}
	for (auto airportLocation: airportLocations)
	{
		airportsFile << airportLocation.first << " = { " << airportLocation.second << " }\n";
	}
	airportsFile.close();
}