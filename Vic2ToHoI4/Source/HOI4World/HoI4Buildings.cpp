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
#include <string>
using namespace std;



HoI4Building::HoI4Building(int _stateID, double _xCoordinate, double _zCoordinate)
{
	stateID = _stateID;
	xCoordinate = _xCoordinate;
	yCoordinate = 10.0;
	zCoordinate = _zCoordinate;
	rotation = 0.0;
}


ostream& operator << (ostream& out, HoI4Building& building)
{
	return building.print(out);
}


ostream& HoI4Building::print(ostream& out) const
{
	return out;
}


HoI4NavalBase::HoI4NavalBase(int _stateID, double _xCoordinate, double _zCoordinate, int _connectingSeaProvince): HoI4Building(_stateID, _xCoordinate, _zCoordinate)
{
	connectingSeaProvince = _connectingSeaProvince;
}


ostream& HoI4NavalBase::print(ostream& out) const
{
	out << stateID << ";naval_base;" << xCoordinate << ';' << 11/*yCoordinate*/ << ';' << zCoordinate << ';';
	out << rotation << ';' << connectingSeaProvince << endl;

	return out;
}


HoI4Buildings::HoI4Buildings(const map<int, int>& provinceToStateIDMap)
{
	LOG(LogLevel::Info) << "Creating buildings";
	placeNavalBases(provinceToStateIDMap);
}


void HoI4Buildings::placeNavalBases(const map<int, int>& provinceToStateIDMap)
{
	map<int, int> coastalProvinces = coastalHoI4ProvincesMapper::getCoastalProvinces();
	for (auto province: coastalProvinces)
	{
		auto position = provinceNeighborMapper::getBorderCenter(province.first, province.second);
		if (position.first == -1)
		{
			LOG(LogLevel::Warning) << "Could not find position for province " << province.first << ". Naval base not set.";
			continue;
		}

		auto provinceToStateMapping = provinceToStateIDMap.find(province.first);
		if (provinceToStateMapping == provinceToStateIDMap.end())
		{
			LOG(LogLevel::Warning) << "Could not find state for province " << province.first << ". Naval base not set.";
			continue;
		}

		HoI4NavalBase* newNavalBase = new HoI4NavalBase(provinceToStateMapping->second, position.first, position.second, province.second);
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