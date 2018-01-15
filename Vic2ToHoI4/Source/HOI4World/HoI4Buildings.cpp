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



#include "HoI4Buildings.h"
#include "../Configuration.h"
#include "../Mappers/CoastalHoI4Provinces.h"
#include "../Mappers/ProvinceNeighborMapper.h"
#include "Log.h"
#include "HoI4Province.h"
#include <fstream>
#include <iomanip>
#include <regex>
#include <string>
using namespace std;



HoI4Building::HoI4Building(int _stateID, const buildingPosition& _position):
	stateID(_stateID),
	position(_position)
{
}


ostream& operator << (ostream& out, const HoI4Building& building)
{
	return building.print(out);
}


ostream& HoI4Building::print(ostream& out) const
{
	return out;
}


HoI4NavalBase::HoI4NavalBase(int _stateID, const buildingPosition& _position, int _connectingSeaProvince):
	HoI4Building(_stateID, _position),
	connectingSeaProvince(_connectingSeaProvince)
{
}


ostream& HoI4NavalBase::print(ostream& out) const
{
	out << stateID << ";naval_base;";
	out << fixed << setprecision(2) << position.xCoordinate << ';' << position.yCoordinate << ';' << position.zCoordinate << ';' << position.rotation << ';';
	out << connectingSeaProvince << '\n';

	return out;
}


HoI4Buildings::HoI4Buildings(const map<int, int>& provinceToStateIDMap):
	buildings()
{
	LOG(LogLevel::Info) << "Creating buildings";

	importDefaultBuildings();
	placeNavalBases(provinceToStateIDMap);
}


void HoI4Buildings::importDefaultBuildings()
{
	ifstream buildingsFile(Configuration::getHoI4Path() + "/map/buildings.txt");
	if (!buildingsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/buildings.txt";
		exit(-1);
	}

	while (!buildingsFile.eof())
	{
		string line;
		getline(buildingsFile, line);
		importDefaultBuilding(line);
	}
}


void HoI4Buildings::importDefaultBuilding(const string& line)
{
	regex pattern("(.+);(.+);(.+);(.+);(.+);(.+);(.+)");
	smatch matches;
	if (regex_match(line, matches, pattern))
	{
		buildingPosition position;
		position.xCoordinate = stof(matches[3].str());
		position.yCoordinate = stof(matches[4].str());
		position.zCoordinate = stof(matches[5].str());
		position.rotation = stof(matches[6].str());

		if (matches[2] == "naval_base")
		{
			importDefaultNavalBase(position, matches);
		}
	}
}


void HoI4Buildings::importDefaultNavalBase(const buildingPosition& position, const smatch& matches)
{
	int connectingSeaProvince = stoi(matches[7].str());

	auto province = provinceNeighborMapper::getProvinceNumber(position.xCoordinate, position.zCoordinate);
	if (province)
	{
		auto key = make_pair(*province, connectingSeaProvince);
		defaultNavalBases[key] = position;
	}
}


void HoI4Buildings::placeNavalBases(const map<int, int>& provinceToStateIDMap)
{
	auto coastalProvinces = coastalHoI4ProvincesMapper::getCoastalProvinces();
	for (auto province: coastalProvinces)
	{
		auto provinceToStateMapping = provinceToStateIDMap.find(province.first);
		if (provinceToStateMapping == provinceToStateIDMap.end())
		{
			LOG(LogLevel::Warning) << "Could not find state for province " << province.first << ". Naval base not set.";
			continue;
		}

		buildingPosition position;
		bool positionUnset = true;
		int connectingSeaProvince = 0;
		for (auto seaProvince: province.second)
		{
			auto defaultNavalBase = defaultNavalBases.find(make_pair(province.first, seaProvince));
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
			auto possiblePosition = provinceNeighborMapper::getBorderCenter(province.first, province.second[0]);
			if (!possiblePosition)
			{
				LOG(LogLevel::Warning) << "Could not find position for province " << province.first << ". Naval base not set.";
				continue;
			}

			position.xCoordinate = possiblePosition->first;
			position.yCoordinate = 11.0;
			position.zCoordinate = possiblePosition->second;
			position.rotation = 0.0;

			if (Configuration::getDebug())
			{
				LOG(LogLevel::Warning) << "The naval base from " << province.first << " to " << connectingSeaProvince << " at (" << position.xCoordinate << ", " << position.zCoordinate << ") did not have a location in default HoI4.";
			}
		}

		HoI4NavalBase* newNavalBase = new HoI4NavalBase(provinceToStateMapping->second, position, connectingSeaProvince);
		buildings.insert(make_pair(provinceToStateMapping->second, newNavalBase));
	}
}


void HoI4Buildings::output() const
{
	ofstream out("output/" + Configuration::getOutputName() + "/map/buildings.txt");
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open output/" << Configuration::getOutputName() << "/map/buildings.txt";
		exit(-1);
	}

	for (auto building: buildings)
	{
		out << *(building.second);
	}
}