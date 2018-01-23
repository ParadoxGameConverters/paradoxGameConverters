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



#include "MapAreas.h"
#include "Log.h"




EU4World::mapAreas::mapAreas(const std::string& filename):
	areaToProvincesMapping()
{
	std::ifstream areasFile(filename);
	if (!areasFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << filename;
		exit(-1);
	}

	while (!areasFile.eof())
	{
		importArea(areasFile);
	}

	areasFile.close();
}



void EU4World::mapAreas::importArea(std::ifstream& areasFile)
{
	auto possibleAreaName = getNextToken(areasFile);
	if (possibleAreaName && (possibleAreaName->size() >= 4) && (possibleAreaName->substr(possibleAreaName->size() - 4, 4) == "area"))
	{
		mapArea newArea(areasFile);
		areaToProvincesMapping.insert(make_pair(*possibleAreaName, newArea.getProvinces()));
	}
}


const std::vector<int> EU4World::mapAreas::getProvincesInArea(const std::string& area) const
{
	auto areaItr(areaToProvincesMapping.find(area));
	if (areaItr != areaToProvincesMapping.end())
	{
		return areaItr->second;
	}
	else
	{
		return {};
	}
}


EU4World::mapArea::mapArea(std::ifstream& theStream)
{
	registerKeyword(std::regex("color"), commonItems::ignoreObject);

	auto token = getNextToken(theStream);
	while (token && (*token != "}"))
	{
		if ((token->find('=') == std::string::npos) && (token->find('{') == std::string::npos))
		{
			provinces.push_back(std::stoi(*token));
		}
		token = getNextToken(theStream);
	}
}