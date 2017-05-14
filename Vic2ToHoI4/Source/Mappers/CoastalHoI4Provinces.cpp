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



#include "CoastalHoI4Provinces.h"
#include <fstream>
#include "Log.h"
#include "../Configuration.h"
#include "ProvinceNeighborMapper.h"




coastalHoI4ProvincesMapper* coastalHoI4ProvincesMapper::instance = nullptr;


coastalHoI4ProvincesMapper::coastalHoI4ProvincesMapper()
{
	map<int, province> provinces = getProvinces();

	for (auto province: provinces)
	{
		if (!province.second.isLand)
		{
			continue;
		}

		auto neighbors = provinceNeighborMapper::getNeighbors(province.first);
		for (auto adjProvinceNum: neighbors)
		{
			auto adjProvince = provinces.find(adjProvinceNum);
			if ((adjProvince != provinces.end()) && (adjProvince->second.type == "ocean"))
			{
				coastalProvinces.insert(make_pair(province.first, adjProvince->first));
				break;
			}
		}
	}
}


map<int, province> coastalHoI4ProvincesMapper::getProvinces()
{
	ifstream provinceDefinitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!provinceDefinitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	map<int, province> provinces;
	while (!provinceDefinitions.eof())
	{
		string line;
		getline(provinceDefinitions, line);
		if (line.length() == 0)
		{
			break;
		}

		int IDSeparator = line.find_first_of(';');
		int ID = stoi(line.substr(0, IDSeparator));
		if (ID == 0)
		{
			continue;
		}
		line = line.substr(IDSeparator + 1, line.size());

		int redSeparator = line.find_first_of(';');
		line = line.substr(redSeparator + 1, line.size());

		int greenSeparator = line.find_first_of(';');
		line = line.substr(greenSeparator + 1, line.size());

		int blueSeparator = line.find_first_of(';');
		line = line.substr(blueSeparator + 1, line.size());

		int landSeaSeparator = line.find_first_of(';');
		string landOrSea = line.substr(0, landSeaSeparator);
		bool isLand = (landOrSea == "land");
		line = line.substr(landSeaSeparator + 1, line.size());

		int boolSeparator = line.find_first_of(';');
		line = line.substr(boolSeparator + 1, line.size());

		int typeSeparator = line.find_first_of(';');
		string type = line.substr(0, typeSeparator);

		province newProvince;
		newProvince.isLand = isLand;
		newProvince.type = type;
		provinces.insert(make_pair(ID, newProvince));
	}

	return provinces;
}


bool coastalHoI4ProvincesMapper::IsProvinceCoastal(int provinceNum)
{
	auto province = coastalProvinces.find(provinceNum);
	return (province != coastalProvinces.end());
}