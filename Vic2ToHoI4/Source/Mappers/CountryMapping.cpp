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



#include "CountryMapping.h"
#include <iomanip>
#include "../V2World/World.h"
#include "Log.h"
#include "ParserHelpers.h"



class countryMappingRule: commonItems::parser
{
	public:
		explicit countryMappingRule(std::istream& theStream);

		std::string getVic2Tag() const { return Vic2Tag; }
		std::vector<std::string> getHoI4Tags() const { return HoI4Tags; }

	private:
		std::string Vic2Tag;
		std::vector<std::string> HoI4Tags;
};


countryMappingRule::countryMappingRule(std::istream& theStream)
{
	registerKeyword(std::regex("vic"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString mapping(theStream);
		Vic2Tag = mapping.getString();
	});
	registerKeyword(std::regex("hoi"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString mapping(theStream);
		HoI4Tags.push_back(mapping.getString());
	});

	parseStream(theStream);
}


CountryMapper::CountryMapper(const Vic2::World* srcWorld):
	Vic2TagToHoI4TagsRules(),
	V2TagToHoI4TagMap(),
	generatedHoI4TagPrefix('X'),
	generatedHoI4TagSuffix(0)
{
	LOG(LogLevel::Info) << "Getting country mappings";
	readRules();
	createMappings(srcWorld);
}


void CountryMapper::readRules()
{
	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream& theStream){
		countryMappingRule rule(theStream);
		Vic2TagToHoI4TagsRules.insert(make_pair(rule.getVic2Tag(), rule.getHoI4Tags()));
	});

	LOG(LogLevel::Info) << "Reading country mapping rules";
	parseFile("country_mappings.txt");
}


void CountryMapper::createMappings(const Vic2::World* srcWorld)
{
	LOG(LogLevel::Info) << "Creating country mapping";
	resetMappingData();

	for (auto Vic2Country: srcWorld->getCountries())
	{
		makeOneMapping(Vic2Country.first);
	}
}


void CountryMapper::resetMappingData()
{
	generatedHoI4TagPrefix = 'X';
	generatedHoI4TagSuffix = 0;
}


void CountryMapper::makeOneMapping(const std::string& Vic2Tag)
{
	auto mappingRule = Vic2TagToHoI4TagsRules.find(Vic2Tag);

	bool mapped = false;
	if (mappingRule != Vic2TagToHoI4TagsRules.end())
	{
		auto possibleHoI4Tags = mappingRule->second;
		mapped = mapToFirstUnusedVic2Tag(possibleHoI4Tags, Vic2Tag);
	}

	if (!mapped)
	{
		std::string HoI4Tag = generateNewHoI4Tag(Vic2Tag);
		mapToNewTag(Vic2Tag, HoI4Tag);
	}
}


bool CountryMapper::mapToFirstUnusedVic2Tag(const std::vector<std::string>& possibleHoI4Tags, const std::string& Vic2Tag)
{
	for (auto possibleHoI4Tag: possibleHoI4Tags)
	{
		if (!tagIsAlreadyAssigned(possibleHoI4Tag))
		{
			V2TagToHoI4TagMap.insert(make_pair(Vic2Tag, possibleHoI4Tag));
			HoI4TagToV2TagMap.insert(make_pair(possibleHoI4Tag, Vic2Tag));
			logMapping(Vic2Tag, possibleHoI4Tag, "mapping rule");

			return true;
		}
	}

	return false;
}


std::string CountryMapper::generateNewHoI4Tag(const std::string& Vic2Tag)
{
	std::ostringstream generatedHoI4TagStream;
	generatedHoI4TagStream << generatedHoI4TagPrefix << std::setfill('0') << std::setw(2) << generatedHoI4TagSuffix;
	std::string newTag = generatedHoI4TagStream.str();

	++generatedHoI4TagSuffix;
	if (generatedHoI4TagSuffix > 99)
	{
		generatedHoI4TagSuffix = 0;
		--generatedHoI4TagPrefix;
	}

	return newTag;
}


void CountryMapper::mapToNewTag(const std::string& Vic2Tag, const std::string& HoI4Tag)
{
	V2TagToHoI4TagMap.insert(make_pair(Vic2Tag, HoI4Tag));
	HoI4TagToV2TagMap.insert(make_pair(HoI4Tag, Vic2Tag));
	logMapping(Vic2Tag, HoI4Tag, "generated tag");
}


void CountryMapper::logMapping(const std::string& sourceTag, const std::string& targetTag, const std::string& reason) const
{
	LOG(LogLevel::Debug) << "Mapping " << sourceTag << " -> " << targetTag << " (" << reason << ')';
}


bool CountryMapper::tagIsAlreadyAssigned(const std::string& HoI4Tag) const
{
	return (HoI4TagToV2TagMap.find(HoI4Tag) != HoI4TagToV2TagMap.end());
}


std::optional<std::string> CountryMapper::getHoI4Tag(const std::string& V2Tag) const
{
	auto findIter = V2TagToHoI4TagMap.find(V2Tag);
	if (findIter != V2TagToHoI4TagMap.end())
	{
		return findIter->second;
	}
	else
	{
		return {};
	}
}


std::optional<std::string> CountryMapper::getVic2Tag(const std::string& HoI4Tag) const
{
	auto findIter = HoI4TagToV2TagMap.find(HoI4Tag);
	if (findIter != HoI4TagToV2TagMap.end())
	{
		return findIter->second;
	}
	else
	{
		return {};
	}
}