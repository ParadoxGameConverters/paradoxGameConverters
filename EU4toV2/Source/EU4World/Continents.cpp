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



#include "Continents.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <algorithm>



EU4::continents* EU4::continents::instance = nullptr;



EU4::continents::continents()
{
	LOG(LogLevel::Info) << "Finding Continents";
	for (auto mod: Configuration::getEU4Mods())
	{
		string continentFile = mod + "/map/continent.txt";
		if (Utils::DoesFileExist(continentFile))
		{
			initContinentMap(continentFile);
		}
	}

	if (continentMap.empty())
	{
		initContinentMap(Configuration::getEU4Path() + "/map/continent.txt");
	}

	if (continentMap.empty())
	{
		LOG(LogLevel::Warning) << "No continent mappings found - may lead to problems later";
	}
}



void EU4::continents::initContinentMap(const std::string& filename)
{
	continentMap.clear();

	registerKeyword(std::regex("\\w+"), [this](const std::string& continentName, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			commonItems::intList newContinent(theStream);
			auto provinces = newContinent.getInts();
			std::for_each(provinces.begin(), provinces.end(), [this, continentName](const int& province)
				{
					continentMap.insert(make_pair(province, continentName));
				}
			);
		}
	);

	parseFile(filename);
}


std::optional<std::string> EU4::continents::GetEU4Continent(int EU4Province)
{
	auto mapping = continentMap.find(EU4Province);
	if (mapping != continentMap.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}