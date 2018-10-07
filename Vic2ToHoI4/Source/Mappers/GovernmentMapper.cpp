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



#include "GovernmentMapper.h"
#include "../V2World/Country.h"
#include "Log.h"
#include "ParserHelpers.h"



class aGovernmentMapping: commonItems::parser
{
	public:
		explicit aGovernmentMapping(std::istream& theStream);

		auto getMapping() const { return mapping; }

	private:
		governmentMapping mapping;
};


aGovernmentMapping::aGovernmentMapping(std::istream& theStream)
{
	registerKeyword(std::regex("vic"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString vic2Government(theStream);
		mapping.vic2Government = vic2Government.getString();
	});
	registerKeyword(std::regex("ruling_party"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString rulingParty(theStream);
		mapping.rulingPartyRequired = rulingParty.getString();
	});
	registerKeyword(std::regex("hoi_gov"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString hoi4Government(theStream);
		mapping.HoI4GovernmentIdeology = hoi4Government.getString();
	});
	registerKeyword(std::regex("hoi_leader"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString hoi4Leader(theStream);
		mapping.HoI4LeaderIdeology = hoi4Leader.getString();
	});

	parseStream(theStream);
}


class governmentMappings: commonItems::parser
{
	public:
		explicit governmentMappings(std::istream& theStream);

		auto getGovernmentMap() const { return governmentMap; }

	private:
		std::vector<governmentMapping> governmentMap;
};


governmentMappings::governmentMappings(std::istream& theStream)
{
	registerKeyword(std::regex("mapping"), [this](const std::string& unused, std::istream& theStream){
		aGovernmentMapping mapping(theStream);
		governmentMap.push_back(mapping.getMapping());
	});

	parseStream(theStream);
}


class aPartyMapping: commonItems::parser
{
	public:
		explicit aPartyMapping(std::istream& theStream);

		auto getMapping() const { return mapping; }

	private:
		partyMapping mapping;
};


aPartyMapping::aPartyMapping(std::istream& theStream)
{
	registerKeyword(std::regex("ruling_ideology"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString ideologyString(theStream);
		mapping.rulingIdeology = ideologyString.getString();
	});
	registerKeyword(std::regex("vic2_ideology"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString ideologyString(theStream);
		mapping.vic2Ideology = ideologyString.getString();
	});
	registerKeyword(std::regex("supported_ideology"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString ideologyString(theStream);
		mapping.supportedIdeology = ideologyString.getString();
	});

	parseStream(theStream);
}


class partyMappings: commonItems::parser
{
	public:
		explicit partyMappings(std::istream& theStream);

		auto getPartyMap() const { return partyMap; }

	private:
		std::vector<partyMapping> partyMap;
};


partyMappings::partyMappings(std::istream& theStream)
{
	registerKeyword(std::regex("mapping"), [this](const std::string& unused, std::istream& theStream){
		aPartyMapping mapping(theStream);
		partyMap.push_back(mapping.getMapping());
	});

	parseStream(theStream);
}


governmentMapper::governmentMapper() noexcept
{
	registerKeyword(std::regex("government_mappings"), [this](const std::string& unused, std::istream& theStream){
		governmentMappings mappings(theStream);
		governmentMap = mappings.getGovernmentMap();
	});
	registerKeyword(std::regex("party_mappings"), [this](const std::string& unused, std::istream& theStream){
		partyMappings mappings(theStream);
		partyMap = mappings.getPartyMap();
	});

	LOG(LogLevel::Info) << "Parsing governments mappings";
	parseFile("governmentMapping.txt");
}


std::string governmentMapper::getIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology) const
{
	std::string ideology = "neutrality";
	for (auto mapping: governmentMap)
	{
		if (governmentMatches(mapping, country->getGovernment()) &&	rulingIdeologyMatches(mapping, Vic2RulingIdeology))
		{
			ideology = mapping.HoI4GovernmentIdeology;
			break;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " government " << country->getGovernment() << " to " << ideology;
	return ideology;
}


std::string governmentMapper::getLeaderIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology) const
{
	std::string ideology = "neutrality";
	for (auto mapping: governmentMap)
	{
		if (governmentMatches(mapping, country->getGovernment()) &&	rulingIdeologyMatches(mapping, Vic2RulingIdeology))
		{
			ideology = mapping.HoI4LeaderIdeology;
			break;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " leader " << country->getGovernment() << " to " << ideology;
	return ideology;
}


std::string governmentMapper::getExistingIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology, const std::set<std::string>& majorIdeologies, const std::map<std::string, HoI4Ideology*>& ideologies) const
{
	std::string ideology = "neutrality";
	for (auto mapping: governmentMap)
	{
		if (governmentMatches(mapping, country->getGovernment()) &&	rulingIdeologyMatches(mapping, Vic2RulingIdeology) && ideologyIsValid(mapping, majorIdeologies, ideologies))
		{
			ideology = mapping.HoI4GovernmentIdeology;
			break;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " government " << country->getGovernment() << " to " << ideology;
	return ideology;
}


std::string governmentMapper::getExistingLeaderIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology, const std::set<std::string>& majorIdeologies, const std::map<std::string, HoI4Ideology*>& ideologies) const
{
	std::string ideology = "neutrality";
	for (auto mapping: governmentMap)
	{
		if (governmentMatches(mapping, country->getGovernment()) &&	rulingIdeologyMatches(mapping, Vic2RulingIdeology) && ideologyIsValid(mapping, majorIdeologies, ideologies))
		{
			ideology = mapping.HoI4LeaderIdeology;
			break;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " leader " << country->getGovernment() << " to " << ideology;
	return ideology;
}


bool governmentMapper::governmentMatches(const governmentMapping& mapping, const std::string& government) const
{
	return ((mapping.vic2Government == "") || (mapping.vic2Government == government));
}


bool governmentMapper::rulingIdeologyMatches(const governmentMapping& mapping, const std::string& rulingIdeology) const
{
	return ((mapping.rulingPartyRequired == "") || (mapping.rulingPartyRequired == rulingIdeology));
}


bool governmentMapper::ideologyIsValid(const governmentMapping& mapping, const std::set<std::string>& majorIdeologies, const std::map<std::string, HoI4Ideology*>& ideologies) const
{
	if (majorIdeologies.count(mapping.HoI4GovernmentIdeology) > 0)
	{
		auto ideology = ideologies.find(mapping.HoI4GovernmentIdeology);
		if (ideology != ideologies.end())
		{
			for (auto type: ideology->second->getTypes())
			{
				if (mapping.HoI4LeaderIdeology == type)
				{
					return true;
				}
			}
		}
	}

	return false;
}


std::string governmentMapper::getSupportedIdeology(const std::string& rulingIdeology, const std::string& Vic2Ideology, const std::set<std::string>& majorIdeologies) const
{
	std::string ideology = "neutrality";
	for (auto mapping: partyMap)
	{
		if ((rulingIdeology == mapping.rulingIdeology) && (Vic2Ideology == mapping.vic2Ideology) && (majorIdeologies.count(mapping.supportedIdeology) > 0))
		{
			ideology = mapping.supportedIdeology;
			break;
		}
	}

	return ideology;
}