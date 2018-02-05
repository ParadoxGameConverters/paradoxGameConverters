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



#include "ColonyFlagsetMapper.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



mappers::colonyFlagsetMapper* mappers::colonyFlagsetMapper::instance = nullptr;


mappers::colonyFlag::colonyFlag(std::istream& theStream, const std::string& region):
	name(),
	region(region),
	unique(false),
	overlord()
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto possibleName = getNextToken(theStream);
			if (possibleName)
			{
				name = Utils::convertUTF8To8859_15(*possibleName);
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			}
		}
	);
	registerKeyword(std::regex("unique"), [this](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto uniqueness = getNextToken(theStream);
			if (uniqueness && (*uniqueness == "true"))
			{
				unique = true;
			}
		}
	);

	parseStream(theStream);
}


mappers::colonyFlagsetRegion::colonyFlagsetRegion(std::istream& theStream, const std::string& region, std::map<std::string, std::shared_ptr<colonyFlag>>& colonyFlagset)
{
	registerKeyword(std::regex("flag"), [region, &colonyFlagset](const std::string& unused, std::istream& theStream)
		{
			std::shared_ptr<colonyFlag> flag = std::make_shared<colonyFlag>(theStream, region);
			colonyFlagset[flag->getName()] =  flag;
		}
	);

	parseStream(theStream);
}



mappers::colonyFlagsetMapper::colonyFlagsetMapper()
{
	LOG(LogLevel::Info) << "Parsing colony naming rules.";

	registerKeyword(std::regex("[\\w_]+"), [this](const std::string& region, std::istream& theStream)
		{
			colonyFlagsetRegion newRegion(theStream, region, colonyFlagset);
		}
	);

	parseFile("colonial_flags.txt");
}


std::shared_ptr<mappers::colonyFlag> mappers::colonyFlagsetMapper::GetFlag(const std::string& name)
{
	auto possibleFlag = colonyFlagset.find(name);
	if (possibleFlag != colonyFlagset.end())
	{
		return possibleFlag->second;
	}
	else
	{
		for (auto flag: colonyFlagset)
		{
			if (name.find(flag.first) != std::string::npos)
			{
				return flag.second;
			}
		}
		return {};
	}
}


std::vector<std::string> mappers::colonyFlagsetMapper::GetNames()
{
	std::vector<std::string> names;

	for (auto flag: colonyFlagset)
	{
		if (!flag.second->isUnique())
		{
			names.push_back(flag.first);
		}
	}

	return names;
}


void mappers::colonyFlagsetMapper::RemoveFlag(const std::string& name)
{
	colonyFlagset.erase(name);
}