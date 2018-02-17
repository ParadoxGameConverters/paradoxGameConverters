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



#include "CultureGroups.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <set>
using namespace std;



EU4::culture::culture(std::istream& theStream):
	primaryTag(),
	graphicalCulture(),
	maleNames(),
	femaleNames(),
	dynastyNames()
{
	registerKeyword(std::regex("country"), commonItems::ignoreObject);
	registerKeyword(std::regex("province"), commonItems::ignoreObject);
	registerKeyword(std::regex("primary"), [this](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			primaryTag = *getNextToken(theStream);
		}
	);
	registerKeyword(std::regex("graphical_culture"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString token(theStream);
			graphicalCulture = token.getString();
		}
	);
	registerKeyword(std::regex("male_names"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringList names(theStream);
			maleNames = names.getStrings();
		}
	);
	registerKeyword(std::regex("female_names"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringList names(theStream);
			femaleNames = names.getStrings();
		}
	);
	registerKeyword(std::regex("dynasty_names"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringList names(theStream);
			dynastyNames = names.getStrings();
		}
	);

	parseStream(theStream);
}


EU4::cultureGroup::cultureGroup(const std::string& name_, std::istream& theStream):
	name(name_),
	graphicalCulture(),
	maleNames(),
	femaleNames(),
	dynastyNames(),
	cultures()
{
	registerKeyword(std::regex("graphical_culture"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString token(theStream);
			graphicalCulture = token.getString();
		}
	);
	registerKeyword(std::regex("male_names"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringList names(theStream);
			maleNames = names.getStrings();
		}
	);
	registerKeyword(std::regex("female_names"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringList names(theStream);
			femaleNames = names.getStrings();
		}
	);
	registerKeyword(std::regex("dynasty_names"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringList names(theStream);
			dynastyNames = names.getStrings();
		}
	);
	registerKeyword(std::regex("\\w+"), [this](const std::string& cultureName, std::istream& theStream)
		{
			culture newCulture(theStream);
			cultures.insert(make_pair(cultureName, newCulture));
		}
	);

	parseStream(theStream);
}



EU4::cultureGroups* EU4::cultureGroups::instance = nullptr;



EU4::cultureGroups::cultureGroups():
	groupToCulturesMap(),
	cultureToGroupMap()
{
	addCulturesFromFile(Configuration::getEU4Path() + "/common/cultures/00_cultures.txt");

	for (auto itr: Configuration::getEU4Mods())
	{
		std::set<std::string> cultureFiles;
		Utils::GetAllFilesInFolder(itr + "/common/cultures/", cultureFiles);
		for (auto cultureFile: cultureFiles)
		{
			addCulturesFromFile(itr + "/common/cultures/" + cultureFile);
		}
	}
}


void EU4::cultureGroups::addCulturesFromFile(const std::string& filename)
{
	registerKeyword(std::regex("\\w+"), [this](const std::string& cultureGroupName, std::istream& theStream)
		{
			std::vector<culture> cultures;
			cultureGroup newGroup(cultureGroupName, theStream);
			for (auto culture: newGroup.getCultures())
			{
				cultureToGroupMap.insert(make_pair(culture.first, newGroup));
				cultures.push_back(culture.second);
			}

			auto itr = groupToCulturesMap.find(cultureGroupName);
			if (itr != groupToCulturesMap.end())
			{
				auto oldCultures = itr->second;
				for (auto oldCulture: oldCultures)
				{
					cultures.push_back(oldCulture);
				}
			}
			groupToCulturesMap[cultureGroupName] = cultures;
		}
	);

	parseFile(filename);
}


std::optional<EU4::cultureGroup> EU4::cultureGroups::GetCulturalGroup(const std::string& culture)
{
	auto mapping = cultureToGroupMap.find(culture);
	if (mapping != cultureToGroupMap.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}


vector<EU4::culture> EU4::cultureGroups::GetCulturesInGroup(const std::string& group)
{
	auto mapping = groupToCulturesMap.find(group);
	if (mapping != groupToCulturesMap.end())
	{
		return mapping->second;
	}
	else
	{
		std::vector<culture> empty;
		return empty;
	}
}