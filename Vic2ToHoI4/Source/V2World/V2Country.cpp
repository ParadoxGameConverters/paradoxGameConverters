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



V2Country::V2Country(Object* countryObj)
{
	tag = countryObj->getKey();
	provinces.clear();
	cores.clear();
	greatNation = false;

	readInDomainNameAndAdjective(countryObj);
	readInCapital(countryObj);
	readInCultures(countryObj);
	readInCivilized(countryObj);
	readInTechnology(countryObj);
	readInInventions(countryObj);
	readInPoliticalParties(countryObj);
	readInSpending(countryObj);
	readInRevanchism(countryObj);
	readInWarExhaustion(countryObj);
	readInBadBoy(countryObj);
	readInReforms(countryObj);
	readInGovernment(countryObj);
	readInUpperHouse(countryObj);
	readInRelations(countryObj);
	readInMilitary(countryObj);
	readInLeaders(countryObj);
	readInStates(countryObj);
	detectIfHuman(countryObj);
}


void V2Country::readInDomainNameAndAdjective(const Object* countryObj)
{
	vector<Object*> nameObj = countryObj->getValue("domain_region");	// the region name for dynamically generated dominions
	if (!nameObj.empty())
	{
		dominionName = nameObj[0]->getLeaf();
		dominionAdjective = dominionName;
	}
	else
	{
		dominionName = "";
		dominionAdjective = "";
	}
}


void V2Country::readInCapital(const Object* countryObj)
{
	vector<Object*> capitalObjs = countryObj->getValue("capital");
	if (capitalObjs.size() > 0)
	{
		capital = stoi(capitalObjs[0]->getLeaf());
	}
	else
	{
		capital = 0;
	}
}


void V2Country::readInCultures(const Object* countryObj)
{
	vector<Object*> primaryCultureObjs = countryObj->getValue("primary_culture");
	if (primaryCultureObjs.size() > 0)
	{
		primaryCulture = primaryCultureObjs[0]->getLeaf();
		acceptedCultures.insert(primaryCulture);
	}
	else
	{
		primaryCulture = "";
	}

	primaryCultureGroup = cultureGroupMapper::getCultureGroup(primaryCulture);

	vector<Object*> cultureSectionObjs = countryObj->getValue("culture");
	if (cultureSectionObjs.size() > 0)
	{
		auto cultures = cultureSectionObjs[0]->getTokens();
		for (auto culture: cultures)
		{
			acceptedCultures.insert(culture);
		}
	}
}


void V2Country::readInCivilized(const Object* countryObj)
{
	vector<Object*> civilizedObjs = countryObj->getValue("civilized");
	if (civilizedObjs.size() > 0)
	{
		civilized = (civilizedObjs[0]->getLeaf() == "yes");
	}
	else
	{
		civilized = false;
	}
}


void V2Country::readInTechnology(const Object* countryObj)
{
	vector<Object*> techsObjs = countryObj->getValue("technology");	// the object holding the technology levels
	if (techsObjs.size() > 0)
	{
		for (auto tech: techsObjs[0]->getKeys())
		{
			techs.insert(tech);
		}
	}
}


void V2Country::readInInventions(const Object* countryObj)
{
	inventionNumToName inventionNumsToNames = getInventionNums();

	inventions.clear();
	vector<Object*> inventionsObjs = countryObj->getValue("active_inventions");
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


void V2Country::readInPoliticalParties(const Object* countryObj)
{
	activePartyIDs.clear();
	vector<Object*> partyObjs = countryObj->getValue("active_party");
	for (auto partyObj : partyObjs)
	{
		activePartyIDs.push_back(stoi(partyObj->getLeaf()));
	}

	partyObjs = countryObj->getValue("ruling_party");
	if (partyObjs.size() > 0)
	{
		rulingPartyID = stoi(partyObjs[0]->getLeaf());
	}
	else if (activePartyIDs.size() > 0)
	{
		rulingPartyID = activePartyIDs[0];
	}
	else
	{
		rulingPartyID = 0; // Bad value, but normal for Rebel faction.
	}
}


void V2Country::readInSpending(const Object* countryObj)
{
	vector<Object*> spendingObjs = countryObj->getValue("education_spending");
	if (spendingObjs.size() > 0)
	{
		vector<Object*> settingsObjs = spendingObjs[0]->getValue("settings");
		if (settingsObjs.size() > 0)
		{
			educationSpending = stof(settingsObjs[0]->getLeaf());
		}
		else
		{
			educationSpending = 0.0;
		}
	}

	spendingObjs = countryObj->getValue("military_spending");
	if (spendingObjs.size() > 0)
	{
		vector<Object*> settingsObjs = spendingObjs[0]->getValue("settings");
		if (settingsObjs.size() > 0)
		{
			militarySpending = stof(settingsObjs[0]->getLeaf());
		}
		else
		{
			militarySpending = 0.0;
		}
	}
}


void V2Country::readInRevanchism(const Object* countryObj)
{
	vector<Object*> revanchismObjs = countryObj->getValue("revanchism");
	if (revanchismObjs.size() > 0)
	{
		revanchism = stof(revanchismObjs[0]->getLeaf());
	}
	else
	{
		revanchism = 0.0;
	}
}


void V2Country::readInWarExhaustion(const Object* countryObj)
{
	vector<Object*> warExhaustionObjs = countryObj->getValue("war_exhaustion");
	if (warExhaustionObjs.size() > 0)
	{
		warExhaustion = stof(warExhaustionObjs[0]->getLeaf());
	}
	else
	{
		warExhaustion = 0.0;
	}
}

void V2Country::readInBadBoy(const Object* countryObj)
{
	vector<Object*> badBoyObjs = countryObj->getValue("badboy");
	if (badBoyObjs.size() > 0)
	{
		badboy = stof(badBoyObjs[0]->getLeaf());
	}
	else
	{
		badboy = 0.0;
	}
}


void V2Country::readInReforms(Object* countryObj)
{
	map<string, string> reformTypes = reformMapper::getReformTypes();

	for (auto leaf : countryObj->getLeaves())
	{
		string key = leaf->getKey();
		if (reformTypes.find(key) != reformTypes.end())
		{
			reformsArray[key] = leaf->getLeaf();
		}
	}
}


void V2Country::readInGovernment(const Object* countryObj)
{
	vector<Object*> governmentObjs = countryObj->getValue("government");
	if (governmentObjs.size() > 0)
	{
		government = governmentObjs[0]->getLeaf();
	}
	else
	{
		government = "";
	}
}


void V2Country::readInUpperHouse(const Object* countryObj)
{
	auto upperHouseObjs = countryObj->getValue("upper_house");
	auto ideologyObjs = upperHouseObjs[0]->getLeaves();
	for (auto ideologyObj : ideologyObjs)
	{
		upperHouseComposition.insert(make_pair(ideologyObj[0].getKey(), stof(ideologyObj[0].getLeaf())));
	}
}


void V2Country::readInRelations(Object* countryObj)
{
	for (auto leaf : countryObj->getLeaves())
	{
		string key = leaf->getKey();

		if (isCountryTag(key))
		{
			V2Relations* rel = new V2Relations(leaf);
			relations.insert(make_pair(rel->getTag(), rel));
		}
	}
}


bool V2Country::isCountryTag(string potentialTag)
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


void V2Country::readInMilitary(const Object* countryObj)
{
	armies.clear();
	vector<Object*> armyObjs = countryObj->getValue("army");
	for (auto armyObj : armyObjs)
	{
		V2Army* army = new V2Army(armyObj);
		armies.push_back(army);
	}

	vector<Object*> navyObjs = countryObj->getValue("navy");
	for (auto navyObj : navyObjs)
	{
		V2Army* navy = new V2Army(navyObj);
		armies.push_back(navy);

		// get transported armies
		vector<Object*> armyObjs = navyObj->getValue("army");
		for (auto armyObj : armyObjs)
		{
			V2Army* army = new V2Army(armyObj);
			armies.push_back(army);
		}
	}
}


void V2Country::readInLeaders(const Object* countryObj)
{
	leaders.clear();
	vector<Object*> leaderObjs = countryObj->getValue("leader");
	for (auto leaderObj : leaderObjs)
	{
		V2Leader* leader = new V2Leader(leaderObj);
		leaders.push_back(leader);
	}
}


void V2Country::readInStates(const Object* countryObj)
{
	vector<Object*> stateObjs = countryObj->getValue("state");
	for (auto stateObj : stateObjs)
	{
		createNewState(stateObj);
	}
}


void V2Country::createNewState(const Object* stateObj)
{
	Vic2State* newState = new Vic2State(stateObj, tag);
	states.push_back(newState);
}


void V2Country::detectIfHuman(const Object* countryObj)
{
	if (countryObj->getValue("human").size() > 0)
	{
		human = true;
	}
	else
	{
		human = false;
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
	auto nameInAllLanguages = V2Localisations::GetTextInEachLanguage(tag);
	for (auto nameInLanguage : nameInAllLanguages)
	{
		setLocalisationName(nameInLanguage.first, nameInLanguage.second);
	}
}


void V2Country::setLocalisationName(const string& language, const string& name)
{
	if (dominionName != "")
	{
		namesByLanguage[language] = dominionName;
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
	if (dominionAdjective != "") // Domains have their adjective set from domain_region
	{
		adjectivesByLanguage[language] = dominionAdjective;
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


V2Party* V2Country::getRulingParty(const vector<V2Party*>& allParties) const
{
	if ((rulingPartyID <= allParties.size()) && (rulingPartyID > 0))
	{
		return allParties[rulingPartyID - 1]; // Subtract 1, because party ID starts from index of 1
	}
	else
	{
		return nullptr;
	}
}


set<V2Party*> V2Country::getActiveParties(const vector<V2Party*>& allParties) const
{
	set<V2Party*> activeParties;

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