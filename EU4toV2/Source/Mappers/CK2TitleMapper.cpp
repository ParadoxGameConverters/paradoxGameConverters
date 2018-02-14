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



#include "CK2TitleMapper.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



mappers::CK2TitleMapper* mappers::CK2TitleMapper::instance = nullptr;



class titleMapping: commonItems::parser
{
	public:
		titleMapping(std::istream& theSteam);

		bool hasIslamicRegion() const;
		bool hasIndianRegion() const;

		std::string getName() const { return name; }
		std::string getID() const { return ID; }

	private:
		std::string name;
		std::string ID;
		std::string region = "";
};


titleMapping::titleMapping(std::istream& theStream):
	name(),
	ID(),
	region()
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto temp = getNextToken(theStream);
			name = Utils::convertUTF8To8859_15(*temp);
		}
	);
	registerKeyword(std::regex("title"), [this](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto temp = getNextToken(theStream);
			ID = *temp;
		}
	);
	registerKeyword(std::regex("region"), [this](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto temp = getNextToken(theStream);
			region = *temp;
		}
	);

	parseStream(theStream);
}


bool titleMapping::hasIslamicRegion() const
{
	return ((region == "e_persia") || (region == "e_arabia"));
}


bool titleMapping::hasIndianRegion() const
{
	return ((region == "e_rajastan") || (region == "e_bengal") || (region == "e_deccan"));
}


mappers::CK2TitleMapper::CK2TitleMapper():
	titleMap(),
	titles(),
	islamicFlags(),
	indianFlags(),
	generator()
{
	LOG(LogLevel::Info) << "Getting CK2 titles";

	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream& theStream)
		{
			titleMapping newMapping(theStream);
			if (newMapping.hasIslamicRegion())
			{
				islamicFlags.push_back(newMapping.getID());
			}
			else if (newMapping.hasIndianRegion())
			{
				indianFlags.push_back(newMapping.getID());
			}

			titleMap[newMapping.getName()] = newMapping.getID();
			titles.insert(newMapping.getID());
		}
	);

	parseFile("ck2titlemap.txt");
}


std::optional<std::string> mappers::CK2TitleMapper::GetTitle(std::string name)
{
	auto mapping = titleMap.find(name);
	if (mapping != titleMap.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}


bool mappers::CK2TitleMapper::DoesTitleExist(std::string title)
{
	return (titles.count(title) > 0);
}


std::optional<std::string> mappers::CK2TitleMapper::GetRandomIslamicFlag()
{
	if (islamicFlags.size() > 0)
	{
		size_t randomTagIndex = std::uniform_int_distribution<size_t>(0, islamicFlags.size() - 1)(generator);
		return islamicFlags[randomTagIndex];
	}
	else
	{
		return {};
	}
}


std::optional<std::string> mappers::CK2TitleMapper::GetRandomIndianFlag()
{
	if (islamicFlags.size() > 0)
	{
		size_t randomTagIndex = std::uniform_int_distribution<size_t>(0, indianFlags.size() - 1)(generator);
		return indianFlags[randomTagIndex];
	}
	else
	{
		return {};
	}
}