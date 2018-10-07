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



#include "TechMapper.h"
#include "ParserHelpers.h"


class techMapping: commonItems::parser
{
	public:
		explicit techMapping(std::istream& theStream);

		auto getKey() const { return key; }
		auto getValues() const { return values; }

	private:
		std::string key;
		std::vector<std::pair<std::string, int>> values;
};


techMapping::techMapping(std::istream& theStream)
{
	registerKeyword(std::regex("vic2"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString theKey(theStream);
		key = theKey.getString();
	});
	registerKeyword(std::regex("[a-z0-9_]+"), [this](const std::string& valueName, std::istream& theStream){
		commonItems::singleInt valueValue(theStream);
		values.push_back(make_pair(valueName, valueValue.getInt()));
	});

	parseStream(theStream);
}



class map: commonItems::parser
{
	public:
		explicit map(std::istream& theStream);

		auto getMappings() const { return mappings; }

	private:
		std::map<std::string, std::vector<std::pair<std::string, int>>> mappings;
};


map::map(std::istream& theStream)
{
	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream& theStream)
	{
		techMapping theMapping(theStream);
		mappings.insert(make_pair(theMapping.getKey(), theMapping.getValues()));
	});

	parseStream(theStream);
}


techMapper::techMapper() noexcept
{
	registerKeyword(std::regex("tech_map"), [this](const std::string& unused, std::istream& theStream){
		map theTechMap(theStream);
		techMap = theTechMap.getMappings();
	});
	registerKeyword(std::regex("bonus_map"), [this](const std::string& unused, std::istream& theStream){
		map theBonusMap(theStream);
		researchBonusMap = theBonusMap.getMappings();
	});

	parseFile("tech_mapping.txt");
}


std::vector<std::pair<std::string, int>> techMapper::getHoI4Techs(const std::string& oldTech) const
{
	auto mapItr = techMap.find(oldTech);
	if (mapItr == techMap.end())
	{
		return {};
	}
	else
	{
		return mapItr->second;
	}
}


std::vector<std::pair<std::string, int>> techMapper::getResearchBonuses(const std::string& oldTech) const
{
	auto mapItr = researchBonusMap.find(oldTech);
	if (mapItr == researchBonusMap.end())
	{
		return {};
	}
	else
	{
		return mapItr->second;
	}
}