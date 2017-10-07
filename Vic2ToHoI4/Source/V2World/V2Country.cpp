/*Copyright (c) 2017 The Paradox Game Converters Project

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



#include "V2Country.h"
#include "Log.h"
#include "Object.h"
#include "../Mappers/CultureGroupMapper.h"
#include "../Mappers/ReformMapper.h"
#include "../Mappers/V2Localisations.h"
#include "V2Army.h"
#include "V2Leader.h"
#include "V2Province.h"
#include "V2Relations.h"
#include "Vic2State.h"



V2Country::V2Country(shared_ptr<Object> countryObj):
	tag(countryObj->getKey()),
	color(),
	states(),
	provinces(),
	cores(),
	capital(countryObj->safeGetInt("capital")),
	primaryCulture(""),
	primaryCultureGroup(""),
	acceptedCultures(),
	techs(),
	inventions(),
	relations(),
	greatNation(false),
	civilized(countryObj->safeGetString("civilized", "no") == "yes"),
	armies(),
	leaders(),
	educationSpending(0.0),
	militarySpending(0.0),
	revanchism(countryObj->safeGetFloat("revanchism")),
	warExhaustion(countryObj->safeGetFloat("war_exhaustion")),
	badboy(countryObj->safeGetFloat("badboy")),
	government(countryObj->safeGetString("government")),
	reformsArray(),
	upperHouseComposition(),
	rulingPartyID(0),	// Bad value, but normal for Rebel faction.
	activePartyIDs(),
	domainName(countryObj->safeGetString("domain_region")),
	domainAdjective(countryObj->safeGetString("domain_region")),
	namesByLanguage(),
	adjectivesByLanguage(),
	human(false)
{
	readInCultures(countryObj);
	readInTechnology(countryObj);
	readInInventions(countryObj);
	readInPoliticalParties(countryObj);
	readInSpending(countryObj);
	readInReforms(countryObj);
	readInUpperHouse(countryObj);
	readInRelations(countryObj);
	readInMilitary(countryObj);
	readInLeaders(countryObj);
	readInStates(countryObj);
	detectIfHuman(countryObj);
}


void V2Country::readInCultures(shared_ptr<Object> countryObj)
{
	vector<shared_ptr<Object>> primaryCultureObjs = countryObj->getValue("primary_culture");
	if (primaryCultureObjs.size() > 0)
	{
		primaryCulture = primaryCultureObjs[0]->getLeaf();
		acceptedCultures.insert(primaryCulture);
	}

	primaryCultureGroup = cultureGroupMapper::getCultureGroup(primaryCulture);

	vector<shared_ptr<Object>> cultureSectionObjs = countryObj->getValue("culture");
	if (cultureSectionObjs.size() > 0)
	{
		auto cultures = cultureSectionObjs[0]->getTokens();
		for (auto culture: cultures)
		{
			acceptedCultures.insert(culture);
		}
	}
}


void V2Country::readInTechnology(shared_ptr<Object> countryObj)
{
	vector<shared_ptr<Object>> techsObjs = countryObj->getValue("technology");	// the object holding the technology levels
	if (techsObjs.size() > 0)
	{
		for (auto tech: techsObjs[0]->getKeys())
		{
			techs.insert(tech);
		}
	}
}


void V2Country::readInInventions(shared_ptr<Object> countryObj)
{
	inventionNumToName inventionNumsToNames = getInventionNums();

	vector<shared_ptr<Object>> inventionsObjs = countryObj->getValue("active_inventions");
	if (inventionsObjs.size() > 0)
	{
		vector<string> activeInventionsNumbers = inventionsObjs[0]->getTokens();
		for (auto activeInventionNumber : activeInventionsNumbers)
		{
			auto inventionName = inventionNumsToNames.find(stoi(activeInventionNumber));
			if (inventionName == inventionNumsToNames.end())
			{
				LOG(LogLevel::Warning) << tag << " has an invalid invention. Is this using a mod that changed inventions?";
			}
			else
			{
				inventions.insert(inventionName->second);
			}
		}
	}
}


void V2Country::readInPoliticalParties(shared_ptr<Object> countryObj)
{
	for (auto partyObj: countryObj->getValue("active_party"))
	{
		activePartyIDs.push_back(stoi(partyObj->getLeaf()));
	}

	if (activePartyIDs.size() > 0)
	{
		rulingPartyID = countryObj->safeGetInt("ruling_party", activePartyIDs[0]);
	}
}


void V2Country::readInSpending(shared_ptr<Object> countryObj)
{
	vector<shared_ptr<Object>> spendingObjs = countryObj->getValue("education_spending");
	if (spendingObjs.size() > 0)
	{
		vector<shared_ptr<Object>> settingsObjs = spendingObjs[0]->getValue("settings");
		if (settingsObjs.size() > 0)
		{
			educationSpending = stof(settingsObjs[0]->getLeaf());
		}
	}

	spendingObjs = countryObj->getValue("military_spending");
	if (spendingObjs.size() > 0)
	{
		vector<shared_ptr<Object>> settingsObjs = spendingObjs[0]->getValue("settings");
		if (settingsObjs.size() > 0)
		{
			militarySpending = stof(settingsObjs[0]->getLeaf());
		}
	}
}


void V2Country::readInReforms(shared_ptr<Object> countryObj)
{
	map<string, string> reformTypes = reformMapper::getReformTypes();

	for (auto leaf: countryObj->getLeaves())
	{
		string key = leaf->getKey();
		if (reformTypes.find(key) != reformTypes.end())
		{
			reformsArray[key] = leaf->getLeaf();
		}
	}
}


void V2Country::readInUpperHouse(shared_ptr<Object> countryObj)
{
	auto upperHouseObjs = countryObj->getValue("upper_house");
	auto ideologyObjs = upperHouseObjs[0]->getLeaves();
	for (auto ideologyObj : ideologyObjs)
	{
		upperHouseComposition.insert(make_pair(ideologyObj->getKey(), stof(ideologyObj->getLeaf())));
	}
}


void V2Country::readInRelations(shared_ptr<Object> countryObj)
{
	for (auto leaf: countryObj->getLeaves())
	{
		string key = leaf->getKey();

		if (isCountryTag(key))
		{
			V2Relations* rel = new V2Relations(leaf);
			relations.insert(make_pair(rel->getTag(), rel));
		}
	}
}


bool V2Country::isCountryTag(const string& potentialTag)
{
	if (potentialTag.size() != 3)
	{
		return false;
	}

	// normal Vic2 countries
	if (isupper(potentialTag[0]) && isupper(potentialTag[1]) && isupper(potentialTag[2]))
	{
		return true;
	}

	// dominions
	if ((potentialTag[0] == 'D') && isdigit(potentialTag[1]) && isdigit(potentialTag[2]))
	{
		return true;
	}

	// countries converted from EU4 or EU3
	if (isupper(potentialTag[0]) && isdigit(potentialTag[1]) && isdigit(potentialTag[2]))
	{
		return true;
	}

	return false;
}


void V2Country::readInMilitary(shared_ptr<Object> countryObj)
{
	for (auto armyObj: countryObj->getValue("army"))
	{
		V2Army* army = new V2Army(armyObj);
		armies.push_back(army);
	}

	for (auto navyObj: countryObj->getValue("navy"))
	{
		V2Army* navy = new V2Army(navyObj);
		armies.push_back(navy);

		// get transported armies
		for (auto armyObj: navyObj->getValue("army"))
		{
			V2Army* army = new V2Army(armyObj);
			armies.push_back(army);
		}
	}
}


void V2Country::readInLeaders(shared_ptr<Object> countryObj)
{
	for (auto leaderObj: countryObj->getValue("leader"))
	{
		V2Leader* leader = new V2Leader(leaderObj);
		leaders.push_back(leader);
	}
}


void V2Country::readInStates(shared_ptr<Object> countryObj)
{
	for (auto stateObj: countryObj->getValue("state"))
	{
		createNewState(stateObj);
	}
}


void V2Country::createNewState(shared_ptr<Object> stateObj)
{
	Vic2State* newState = new Vic2State(stateObj, tag);
	states.push_back(newState);
}


void V2Country::detectIfHuman(shared_ptr<Object> countryObj)
{
	if (countryObj->getValue("human").size() > 0)
	{
		human = true;
	}
}


void V2Country::eatCountry(V2Country* target)
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


void V2Country::putProvincesInStates()
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


void V2Country::determineEmployedWorkers()
{
	for (auto state : states)
	{
		state->determineEmployedWorkers();
	}
}


void V2Country::setLocalisationNames()
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


void V2Country::setLocalisationName(const string& language, const string& name)
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


void V2Country::setLocalisationAdjectives()
{
	auto adjectiveInAllLanguages = V2Localisations::GetTextInEachLanguage(tag + "_ADJ");
	for (auto adjectiveinLanguage : adjectiveInAllLanguages)
	{
		setLocalisationAdjective(adjectiveinLanguage.first, adjectiveinLanguage.second);
	}
}


void V2Country::setLocalisationAdjective(const string& language, const string& adjective)
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


std::string V2Country::getReform(const string& reform) const
{
	map<string, string>::const_iterator itr = reformsArray.find(reform);
	if (itr == reformsArray.end())
	{
		return "";
	}

	return itr->second;
}


string V2Country::getName(const string& language) const
{
	map<string, string>::const_iterator findIter = namesByLanguage.find(language);
	if (findIter != namesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}


string V2Country::getAdjective(const string& language) const
{
	map<string, string>::const_iterator findIter = adjectivesByLanguage.find(language);
	if (findIter != adjectivesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}


double V2Country::getUpperHousePercentage(const string& ideology) const
{
	map<string, double>::const_iterator itr = upperHouseComposition.find(ideology);
	if (itr == upperHouseComposition.end())
		return 0.0;

	return itr->second;
}


long V2Country::getEmployedWorkers() const
{
	long employedWorkers = 0;
	for (auto state : states)
	{
		employedWorkers += state->getEmployedWorkers();
	}

	return employedWorkers;
}


const V2Party* V2Country::getRulingParty(const vector<const V2Party*>& allParties) const
{
	if ((rulingPartyID <= allParties.size()) && (rulingPartyID > 0))
	{
		return allParties.at(rulingPartyID - 1); // Subtract 1, because party ID starts from index of 1
	}
	else
	{
		return nullptr;
	}
}


set<const V2Party*> V2Country::getActiveParties(const vector<const V2Party*>& allParties) const
{
	set<const V2Party*> activeParties;

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


bool V2Country::hasCoreOnCapital() const
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