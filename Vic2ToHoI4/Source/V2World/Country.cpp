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



#include "Country.h"
#include "Log.h"
#include "../Mappers/CultureGroupMapper.h"
#include "../Mappers/InventionsMapper.h"
#include "../Mappers/ReformMapper.h"
#include "../Mappers/V2Localisations.h"
#include "Army.h"
#include "Leader.h"
#include "Party.h"
#include "Pop.h"
#include "Province.h"
#include "Relations.h"
#include "State.h"
#include "ParserHelpers.h"
#include <functional>



Vic2::Country::Country(const std::string& theTag, std::istream& theStream):
	tag(theTag)
{
	registerKeyword(std::regex("capital"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt capitalInt(theStream);
		capital = capitalInt.getInt();
	});
	registerKeyword(std::regex("civilized"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString civilizedString(theStream);
		if (civilizedString.getString() == "yes")
		{
			civilized = true;
		}
	});
	registerKeyword(std::regex("revanchism"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble revanchismDouble(theStream);
		revanchism = revanchismDouble.getDouble();
	});
	registerKeyword(std::regex("war_exhaustion"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble warExhaustionDouble(theStream);
		warExhaustion = warExhaustionDouble.getDouble();
	});
	registerKeyword(std::regex("badboy"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble badboyDouble(theStream);
		badboy = badboyDouble.getDouble();
	});
	registerKeyword(std::regex("government"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString governmentString(theStream);
		government = governmentString.getString();
	});
	registerKeyword(std::regex("last_election"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString lastElectionString(theStream);
		lastElection = date(lastElectionString.getString());
	});
	registerKeyword(std::regex("domain_region"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString regionString(theStream);
		domainName = regionString.getString();
		domainAdjective = regionString.getString();
	});
	registerKeyword(std::regex("human"), [this](const std::string& unused, std::istream& theStream){
		commonItems::ignoreItem(unused, theStream);
		human = true;
	});
	registerKeyword(std::regex("primary_culture"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString cultureString(theStream);
		primaryCulture = cultureString.getString();
		if (primaryCulture.substr(0,1) == "\"")
		{
			primaryCulture = primaryCulture.substr(1, primaryCulture.size() - 2);
		}
		acceptedCultures.insert(primaryCulture);

		auto cultureGroupOption = cultureGroupMapper::getCultureGroup(primaryCulture);
		if (cultureGroupOption)
		{
			primaryCultureGroup = *cultureGroupOption;
		}
		else
		{
			primaryCultureGroup = "";
		}
	});
	registerKeyword(std::regex("culture"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringList cultureString(theStream);
		for (auto culture: cultureString.getStrings())
		{
			if (culture.substr(0,1) == "\"")
			{
				culture = culture.substr(1, culture.size() - 2);
			}
			acceptedCultures.insert(culture);
		}
	});
	registerKeyword(std::regex("technology"), [this](const std::string& unused, std::istream& theStream){
		auto equals = getNextTokenWithoutMatching(theStream);
		auto openBrace = getNextTokenWithoutMatching(theStream);
		auto token = getNextTokenWithoutMatching(theStream);
		while (token != "}")
		{
			techs.insert(*token);
			commonItems::ignoreItem(*token, theStream);
			token = getNextTokenWithoutMatching(theStream);
		}
	});
	registerKeyword(std::regex("active_inventions"), [this](const std::string& unused, std::istream& theStream){
		commonItems::intList inventionNums(theStream);
		for (auto inventionNum: inventionNums.getInts())
		{
			auto inventionName = inventionsMapper::getInventionName(inventionNum);
			if (inventionName)
			{
				inventions.insert(*inventionName);
			}
		}
	});
	registerKeyword(std::regex("active_party"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt partyNum(theStream);
		activePartyIDs.push_back(partyNum.getInt());
		if (rulingPartyID == 0)
		{
			rulingPartyID = partyNum.getInt();
		}
	});
	registerKeyword(std::regex("ruling_party"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt partyNum(theStream);
		rulingPartyID = partyNum.getInt();
	});
	for (auto reform: reformMapper::getReformTypes())
	{
		registerKeyword(std::regex(reform.first), [this](const std::string& reformType, std::istream& theStream){
			commonItems::singleString reformValue(theStream);
			reformsArray[reformType] = reformValue.getString();
		});
	}
	registerKeyword(std::regex("upper_house"), [this](const std::string& unused, std::istream& theStream){
		auto equals = getNextTokenWithoutMatching(theStream);
		auto openBrace = getNextTokenWithoutMatching(theStream);
		auto token = getNextTokenWithoutMatching(theStream);
		while (token != "}")
		{
			commonItems::singleDouble ideologyAmount(theStream);
			upperHouseComposition.insert(make_pair(*token, ideologyAmount.getDouble()));
			token = getNextTokenWithoutMatching(theStream);
		}
	});
	registerKeyword(std::regex("[A-Z]{3}"), [this](const std::string& countryTag, std::istream& theStream)
	{
		Relations* rel = new Relations(countryTag, theStream);
		relations.insert(make_pair(rel->getTag(), rel));
	});
	registerKeyword(std::regex("[A-Z][0-9]{2}"), [this](const std::string& countryTag, std::istream& theStream)
	{
		Relations* rel = new Relations(countryTag, theStream);
		relations.insert(make_pair(rel->getTag(), rel));
	});
	registerKeyword(std::regex("army"), [this](const std::string& type, std::istream& theStream)
	{
		Army* army = new Army(type, theStream);
		armies.push_back(army);
	});
	registerKeyword(std::regex("navy"), [this](const std::string& type, std::istream& theStream)
	{
		Army* navy = new Army(type, theStream);
		armies.push_back(navy);

		for (auto transportedArmy: navy->getTransportedArmies())
		{
			armies.push_back(transportedArmy);
		}
	});
	registerKeyword(std::regex("leader"), [this](const std::string& unused, std::istream& theStream)
	{
		Leader* leader = new Leader(theStream);
		leaders.push_back(leader);
	});
	registerKeyword(std::regex("state"), [this](const std::string& unused, std::istream& theStream)
	{
		State* newState = new State(theStream, tag);
		states.push_back(newState);
	});
	registerKeyword(std::regex("[A-Za-z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}


void Vic2::Country::eatCountry(Vic2::Country* target)
{
	if (target->tag == tag)
	{
		return;
	}

	for (auto core : target->cores)
	{
		addCore(core);
		core->addCoreString(tag);
		core->removeCoreString(target->tag);
	}

	for (auto provinceItr : target->provinces)
	{
		addProvince(provinceItr);
		provinceItr.second->setOwner(this);
	}

	for (auto tech: target->techs)
	{
		techs.insert(tech);
	}

	for (auto itr : target->inventions)
	{
		inventions.insert(itr);
	}

	armies.insert(armies.end(), target->armies.begin(), target->armies.end());

	// coreless, landless countries will be cleaned up automatically
	target->provinces.clear();
	target->cores.clear();

	LOG(LogLevel::Debug) << "Merged " << target->tag << " into " << tag;
}


void Vic2::Country::putProvincesInStates()
{
	for (auto state : states)
	{
		for (auto provinceNum : state->getProvinceNums())
		{
			auto province = provinces.find(provinceNum);
			if (province == provinces.end())
			{
				LOG(LogLevel::Warning) << "State owned by " << tag << " had province that " << tag << " did not";
				continue;
			}

			state->addProvince(province->second);
		}
	}
}


void Vic2::Country::determineEmployedWorkers()
{
	for (auto state : states)
	{
		state->determineEmployedWorkers();
	}
}


void Vic2::Country::setLocalisationNames()
{
	if (domainName != "")
	{
		V2Localisations::UpdateDomainCountry(tag, domainName);
	}
	auto nameInAllLanguages = V2Localisations::GetTextInEachLanguage(tag);
	for (auto nameInLanguage : nameInAllLanguages)
	{
		setLocalisationName(nameInLanguage.first, nameInLanguage.second);
	}
}


void Vic2::Country::setLocalisationName(const string& language, const string& name)
{
	if (domainName != "")
	{
		namesByLanguage[language] = domainName;
	}
	else if (name != "")
	{
		namesByLanguage[language] = name;
	}
}


void Vic2::Country::setLocalisationAdjectives()
{
	auto adjectiveInAllLanguages = V2Localisations::GetTextInEachLanguage(tag + "_ADJ");
	for (auto adjectiveinLanguage : adjectiveInAllLanguages)
	{
		setLocalisationAdjective(adjectiveinLanguage.first, adjectiveinLanguage.second);
	}
}


void Vic2::Country::handleMissingCulture()
{
	if (primaryCulture == "")
	{
		auto cultureSizes = determineCultureSizes();
		primaryCulture = selectLargestCulture(cultureSizes);
		auto cultureGroupOption = cultureGroupMapper::getCultureGroup(primaryCulture);
		if (cultureGroupOption)
		{
			primaryCultureGroup = *cultureGroupOption;
		}
		else
		{
			primaryCultureGroup = "no_culture";
		}
	}
}


map<string, int> Vic2::Country::determineCultureSizes()
{
	map<string, int> cultureSizes;

	for (auto province: provinces)
	{
		for (auto pop: province.second->getPops())
		{
			string popCulture = pop->getCulture();
			auto cultureSize = cultureSizes.find(popCulture);
			if (cultureSize == cultureSizes.end())
			{
				cultureSizes.insert(make_pair(popCulture, 0));
				cultureSize = cultureSizes.find(popCulture);
			}
			cultureSize->second += pop->getSize();
		}
	}

	return cultureSizes;
}


string Vic2::Country::selectLargestCulture(const map<string, int>& cultureSizes)
{
	string largestCulture;
	int largestCultureSize = 0;
	for (auto cultureSize: cultureSizes)
	{
		if (cultureSize.second > largestCultureSize)
		{
			largestCulture = cultureSize.first;
			largestCultureSize = cultureSize.second;
		}
	}

	return largestCulture;
}


void Vic2::Country::setLocalisationAdjective(const string& language, const string& adjective)
{
	if (domainAdjective != "") // Domains have their adjective set from domain_region
	{
		adjectivesByLanguage[language] = domainAdjective;
	}
	else if (adjective != "")
	{
		adjectivesByLanguage[language] = adjective;
	}
}


optional<string> Vic2::Country::getReform(const string& reform) const
{
	map<string, string>::const_iterator itr = reformsArray.find(reform);
	if (itr == reformsArray.end())
	{
		return {};
	}

	return itr->second;
}


optional<string> Vic2::Country::getName(const string& language) const
{
	map<string, string>::const_iterator findIter = namesByLanguage.find(language);
	if (findIter != namesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return {};
	}
}


optional<string> Vic2::Country::getAdjective(const string& language) const
{
	map<string, string>::const_iterator findIter = adjectivesByLanguage.find(language);
	if (findIter != adjectivesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return {};
	}
}


double Vic2::Country::getUpperHousePercentage(const string& ideology) const
{
	map<string, double>::const_iterator itr = upperHouseComposition.find(ideology);
	if (itr == upperHouseComposition.end())
	{
		return 0.0;
	}

	return itr->second;
}


long Vic2::Country::getEmployedWorkers() const
{
	long employedWorkers = 0;
	for (auto state : states)
	{
		employedWorkers += state->getEmployedWorkers();
	}

	return employedWorkers;
}


optional<const Vic2::Party> Vic2::Country::getRulingParty(const vector<Vic2::Party>& allParties) const
{
	if ((rulingPartyID <= allParties.size()) && (rulingPartyID > 0))
	{
		return allParties.at(rulingPartyID - 1); // Subtract 1, because party ID starts from index of 1
	}
	else
	{
		return {};
	}
}


set<Vic2::Party, function<bool (const Vic2::Party&, const Vic2::Party&)>> Vic2::Country::getActiveParties(const vector<Vic2::Party>& allParties) const
{
	set<Vic2::Party, function<bool (const Vic2::Party&, const Vic2::Party&)>> activeParties([](const Vic2::Party& first, const Vic2::Party& second)
		{ return first.getName() < second.getName(); }
	);

	for (auto ID : activePartyIDs)
	{
		if (ID < allParties.size())
		{
			activeParties.insert(allParties[ID - 1]);  // Subtract 1, because party ID starts from index of 1
		}
		else
		{
			LOG(LogLevel::Warning) << "Party ID mismatch! Did some Vic2 country files not get read?";
		}
	}

	return activeParties;
}


bool Vic2::Country::hasCoreOnCapital() const
{
	for (auto core: cores)
	{
		if (core->getNumber() == capital)
		{
			return true;
		}
	}

	return false;
}

vector<string> Vic2::Country::getShipNames(string category) const
{
	auto foundShipNames = shipNames.find(category);
	if (foundShipNames == shipNames.end())
	{
		return vector<string>();
	}
	return foundShipNames->second;
}


double Vic2::Country::getAverageMilitancy() const
{
	int totalPopulation = 0;
	double totalMilitancy = 0.0;
	for (auto province: provinces)
	{
		auto pops = province.second->getPops();
		for (auto pop: pops)
		{
			int size = pop->getSize();
			totalMilitancy += pop->getMilitancy() * size;
			totalPopulation += size;
		}
	}

	return totalMilitancy / totalPopulation;
}


float Vic2::Country::getAverageIssueSupport(const std::string& issueName) const
{
	int totalPopulation = 0;
	float totalSupport = 0.0;
	for (auto province: provinces)
	{
		auto pops = province.second->getPops();
		for (auto pop: pops)
		{
			int size = pop->getSize();
			totalSupport += pop->getIssue(issueName) * size;
			totalPopulation += size;
		}
	}

	return totalSupport / totalPopulation;
}