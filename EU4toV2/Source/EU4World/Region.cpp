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



#include "Region.h"
#include "Areas.h"
#include <algorithm>



EU4::region::region(std::istream& theStream)
{
	commonItems::parsingFunction areasFunction = std::bind(&EU4::region::importAreas, this, std::placeholders::_1, std::placeholders::_2);
	registerKeyword(std::regex("areas"), areasFunction);
	registerKeyword(std::regex("discover_if"), commonItems::ignoreObject);
	parseStream(theStream);
}


EU4::region::region(std::set<int> _provinces):
	provinces(_provinces)
{
}


void EU4::region::importAreas(const std::string& unused, std::istream& theStream)
{
	registerKeyword(std::regex("\\w+"), [this](const std::string& areaName, std::istream& areasFile)
		{
			areaNames.insert(areaName);
		}
	);
	parseStream(theStream);
}


bool EU4::region::containsProvince(unsigned int province) const
{
	return (provinces.count(province) > 0);
}


void EU4::region::addProvinces(const EU4::areas& areas)
{
	for (auto areaName: areaNames)
	{
		auto newProvinces = areas.getProvincesInArea(areaName);
		provinces.insert(newProvinces.begin(), newProvinces.end());
	}
}