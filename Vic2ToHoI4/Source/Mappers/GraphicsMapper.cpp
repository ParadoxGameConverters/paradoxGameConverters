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



#include "GraphicsMapper.h"
#include "Log.h"
#include "ParserHelpers.h"



class ideologyToPortraitsMapping: commonItems::parser
{
	public:
		ideologyToPortraitsMapping(std::istream& theStream);

		auto getMap() const { return theMap; }

	private:
		std::map<std::string, std::vector<std::string>> theMap;
};


ideologyToPortraitsMapping::ideologyToPortraitsMapping(std::istream& theStream)
{
	registerKeyword(std::regex("[a-zA-Z0-9_]+"), [this](const std::string& ideology, std::istream& theStream){
		commonItems::stringList portraits(theStream);
		theMap.insert(make_pair(ideology, portraits.getStrings()));
	});

	parseStream(theStream);
}



class graphicsCultureGroup: commonItems::parser
{
	public:
	graphicsCultureGroup(std::istream& theStream);

		auto getLeaderPortraits() const { return leaderPortraits; }
		auto getIdeologyMinisterPortraits() const { return ideologyMinisterPortraits; }
		auto getGraphicalCulture() const { return graphicalCulture; }
		auto getGraphicalCulture2D() const { return graphicalCulture2D; }

	private:
		std::map<std::string, std::vector<std::string>> leaderPortraits;
		std::map<std::string, std::vector<std::string>> ideologyMinisterPortraits;
		std::string graphicalCulture;
		std::string graphicalCulture2D;
};


graphicsCultureGroup::graphicsCultureGroup(std::istream& theStream)
{
	registerKeyword(std::regex("leader_portraits"), [this](const std::string& unused, std::istream& theStream){
		ideologyToPortraitsMapping mappings(theStream);
		leaderPortraits = mappings.getMap();
	});
	registerKeyword(std::regex("ideology_minister_portraits"), [this](const std::string& unused, std::istream& theStream){
		ideologyToPortraitsMapping mappings(theStream);
		ideologyMinisterPortraits = mappings.getMap();
	});
	registerKeyword(std::regex("graphical_culture"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringList graphicsString(theStream);
		graphicalCulture = graphicsString.getStrings()[0];
	});
	registerKeyword(std::regex("graphical_culture_2d"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringList graphicsString(theStream);
		graphicalCulture2D = graphicsString.getStrings()[0];
	});

	parseStream(theStream);
}


graphicsMapper::graphicsMapper() noexcept:
	rng()
{
	LOG(LogLevel::Info) << "Reading graphics mappings";

	registerKeyword(std::regex("[a-zA-Z0-9_]+"), [this](const std::string& cultureGroupName, std::istream& theStream){
		graphicsCultureGroup newCultureGroup(theStream);
		graphicalCultureMap[cultureGroupName] = newCultureGroup.getGraphicalCulture();
		graphicalCulture2dMap[cultureGroupName] = newCultureGroup.getGraphicalCulture2D();
		loadLeaderPortraitMappings(cultureGroupName, newCultureGroup.getLeaderPortraits());
		loadIdeologyMinisterPortraitMappings(cultureGroupName, newCultureGroup.getIdeologyMinisterPortraits());
	});

	parseFile("cultureGroupToGraphics.txt");
}


void graphicsMapper::loadLeaderPortraitMappings(const std::string& cultureGroup, std::map<std::string, std::vector<std::string>> portraitMappings)
{
	auto cultureGroupMappings = leaderPortraitMappings.find(cultureGroup);
	if (cultureGroupMappings == leaderPortraitMappings.end())
	{
		cultureGroupToPortraitsMap newCultureGroupMappings;
		leaderPortraitMappings.insert(make_pair(cultureGroup, newCultureGroupMappings));
		cultureGroupMappings = leaderPortraitMappings.find(cultureGroup);
	}

	for (auto portraitMapping: portraitMappings)
	{
		cultureGroupMappings->second.insert(portraitMapping);
	}
}


void graphicsMapper::loadIdeologyMinisterPortraitMappings(const std::string& cultureGroup, std::map<std::string, std::vector<std::string>> portraitMappings)
{
	auto cultureGroupMappings = ideologyMinisterMappings.find(cultureGroup);
	if (cultureGroupMappings == ideologyMinisterMappings.end())
	{
		cultureGroupToPortraitsMap newCultureGroupMappings;
		ideologyMinisterMappings.insert(make_pair(cultureGroup, newCultureGroupMappings));
		cultureGroupMappings = ideologyMinisterMappings.find(cultureGroup);
	}

	for (auto portraitMapping: portraitMappings)
	{
		cultureGroupMappings->second.insert(portraitMapping);
	}
}


std::string graphicsMapper::getLeaderPortrait(const std::string& cultureGroup, const std::string& ideology)
{
	if (auto portraits = getLeaderPortraits(cultureGroup, ideology))
	{
		std::uniform_int_distribution<int> firstNameGen(0, portraits->size() - 1);
		return (*portraits)[firstNameGen(rng)];
	}
	else
	{
		return "gfx/leaders/leader_unknown.dds";
	}
}


std::optional<std::vector<std::string>> graphicsMapper::getLeaderPortraits(const std::string& cultureGroup, const std::string& ideology) const
{
	if (auto mapping = leaderPortraitMappings.find(cultureGroup); mapping != leaderPortraitMappings.end())
	{
		if (auto portraits = mapping->second.find(ideology); portraits != mapping->second.end())
		{
			return portraits->second;
		}
	}

	return {};
}


std::string graphicsMapper::getIdeologyMinisterPortrait(const std::string& cultureGroup, const std::string& ideology)
{
	auto portraits = getIdeologyMinisterPortraits(cultureGroup, ideology);

	if (portraits)
	{
		std::uniform_int_distribution<int> firstNameGen(0, portraits->size() - 1);
		return (*portraits)[firstNameGen(rng)];
	}
	else
	{
		return "gfx/interface/ideas/idea_unknown.dds";
	}
}


std::string graphicsMapper::getGeneralPortrait(const std::string& cultureGroup) const
{
	return "";
}


std::optional<std::vector<std::string>> graphicsMapper::getIdeologyMinisterPortraits(const std::string& cultureGroup, const std::string& ideology) const
{
	if (auto mapping = ideologyMinisterMappings.find(cultureGroup); mapping != ideologyMinisterMappings.end())
	{
		if (auto portraits = mapping->second.find(ideology); portraits != mapping->second.end())
		{
			return portraits->second;
		}
	}

	return {};
}


std::optional<std::string> graphicsMapper::getGraphicalCulture(const std::string& cultureGroup) const
{
	if (auto itr = graphicalCultureMap.find(cultureGroup); itr != graphicalCultureMap.end())
	{
		return itr->second;
	}
	else
	{
		return {};
	}
}


std::optional<std::string> graphicsMapper::get2dGraphicalCulture(const std::string& cultureGroup) const
{
	if (auto itr = graphicalCulture2dMap.find(cultureGroup); itr != graphicalCulture2dMap.end())
	{
		return itr->second;
	}
	else
	{
		return {};
	}
}