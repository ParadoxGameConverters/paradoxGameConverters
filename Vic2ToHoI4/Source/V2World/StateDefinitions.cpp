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



#include "StateDefinitions.h"
#include "Log.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"




Vic2::StateDefinitions Vic2::theStateDefinitions;


void Vic2::StateDefinitions::initialize()
{
	registerKeyword(std::regex("[A-Z0-9\\_]+"), [this](const std::string& stateID, std::istream& theStream){
		commonItems::intList provinceNumbers(theStream);

		std::set<int> neighbors;
		for (auto provinceNumber: provinceNumbers.getInts())
		{
			neighbors.insert(provinceNumber);
			provinceToIDMap.insert(std::make_pair(provinceNumber, stateID));
		}

		for (auto neighbor: neighbors)
		{
			stateMap.insert(make_pair(neighbor, neighbors));
		}

		if (provinceNumbers.getInts().size() > 0)
		{
			stateToCapitalMap.insert(std::make_pair(stateID, provinceNumbers.getInts().front()));
		}
	});

	LOG(LogLevel::Info) << "Importing Vic2 states";
	bool stateMapInitialized = false;
	for (auto itr: theConfiguration.getVic2Mods())
	{
		std::string regionFileName = theConfiguration.getVic2Path() + "/mod/" + itr + "/map/region.txt";
		if (Utils::DoesFileExist(regionFileName))
		{
			parseFile(regionFileName);
			stateMapInitialized = true;
			break;
		}
	}

	if (!stateMapInitialized)
	{
		parseFile(theConfiguration.getVic2Path() + "/map/region.txt");
	}
}


std::set<int> Vic2::StateDefinitions::getAllProvinces(int provinceNumber) const
{
	if (auto mapping = stateMap.find(provinceNumber); mapping != stateMap.end())
	{
		return mapping->second;
	}
	else
	{
		std::set<int> empty;
		return empty;
	}
}


std::optional<std::string> Vic2::StateDefinitions::getStateID(int provinceNumber) const
{
	if (auto mapping = provinceToIDMap.find(provinceNumber); mapping != provinceToIDMap.end())
	{
		return mapping->second;
	}
	else
	{
		return std::nullopt;
	}
}


std::optional<int> Vic2::StateDefinitions::getCapitalProvince(const std::string& stateID) const
{
	if (auto mapping = stateToCapitalMap.find(stateID); mapping != stateToCapitalMap.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}