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



#include "Areas.h"
#include "Log.h"
#include <fstream>
#include <functional>



EU4::areas::areas(const std::string& filename):
	theAreas()
{
	registerKeyword(std::regex("[\\w_]+"), [this](const std::string& areaName, std::istream& areasFile)
		{
			area newArea(areasFile);
			theAreas.insert(make_pair(areaName, newArea));
		}
	);

	parseFile(filename);
}


const std::set<int> EU4::areas::getProvincesInArea(const std::string& area) const
{
	auto areaItr(theAreas.find(area));
	if (areaItr != theAreas.end())
	{
		return areaItr->second.getProvinces();
	}
	else
	{
		return {};
	}
}


EU4::area::area(std::istream& theStream)
{
	registerKeyword(std::regex("color"), [this](const std::string& colorToken, std::istream& areaFile)
		{
			commonItems::Color newColor(areaFile);
			color = newColor;
		}
	);

	auto token = getNextToken(theStream);
	while (token && (*token != "}"))
	{
		if ((token->find('=') == std::string::npos) && (token->find('{') == std::string::npos))
		{
			try
			{
				provinces.insert(std::stoi(*token));
			}
			catch (const std::exception& e)
			{
				if (!token)
				{
					LOG(LogLevel::Warning) << "Missing token when parsing areas";
				}
				else
				{
					LOG(LogLevel::Warning) << "Unexpected token when parsing areas: " << *token;
				}
			}
		}
		token = getNextToken(theStream);
	}
}