/*Copyright(c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


#include "EU4Country.h"
#include "../Configuration.h"
#include "Log.h"
#include "Object.h"
#include "CultureGroups.h"
#include "EU4Province.h"
#include "EU4Relations.h"
#include "EU4Leader.h"
#include "EU4Version.h"
#include "CountryHistory.h"
#include "../Mappers/IdeaEffectMapper.h"
#include "../V2World/V2Localisation.h"
#include <algorithm>



EU4::Country::Country(const std::string& countryTag, std::istream& theStream):
	tag(countryTag),
	provinces(),
	cores(),
	inHRE(false),
	holyRomanEmperor(false),
	celestialEmperor(false),
	capital(0),
	techGroup(),
	embracedInstitutions(),
	isolationism(1),
	primaryCulture(),
	acceptedCultures(),
	culturalUnion({}),
	religion(),
	score(0.0),
	stability(-3.0),
	admTech(0.0),
	dipTech(0.0),
	milTech(0.0),
	armyInvestment(32.0),
	navyInvestment(32.0),
	commerceInvestment(32.0),
	industryInvestment(32.0),
	cultureInvestment(32.0),
	flags(),
	modifiers(),
	possibleDaimyo(false),
	possibleShogun(false),
	militaryLeaders(),
	government(),
	relations(),
	armies(),
	nationalIdeas(),
	legitimacy(1.0),
	customNation(false),
	colony(false),
	colonialRegion(),
	libertyDesire(0.0),
	randomName(),
	customFlag({ "-1", 0, { 0, 0, 0}}),
	revolutionary(false),
	revolutionaryTricolour({0,0,0}),
	name(),
	adjective(),
	color(),
	namesByLanguage(),
	adjectivesByLanguage()
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString theName(theStream);
			name = theName.getString();
			if (name.substr(0,1) == "\"")
			{
				name = name.substr(1, name.size() - 2);
			}
		}
	);
	registerKeyword(std::regex("custom_name"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString theName(theStream);
			randomName = V2Localisation::Convert(theName.getString());
			customNation = true;
		}
	);
	registerKeyword(std::regex("adjective"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString theAdjective(theStream);
			adjective = theAdjective.getString();
		}
	);
	registerKeyword(std::regex("map_color"), [this](const std::string& unused, std::istream& theStream)
		{
			color = commonItems::Color(theStream);
			// Countries whose colors are included in the object here tend to be generated countries,
			// i.e. colonial nations which take on the color of their parent. To help distinguish 
			// these countries from their parent's other colonies we randomly adjust the color.
			color.RandomlyFlunctuate(30);
		}
	);
	registerKeyword(std::regex("colors"), [this](const std::string& colorsString, std::istream& theStream)
		{
			auto colorObj = commonItems::convert8859Object(colorsString, theStream);
			vector<shared_ptr<Object>> countryColorObjs = colorObj->getLeaves()[0]->getValue("country_color");
			if (countryColorObjs.size() > 0)
			{
				color = commonItems::Color(countryColorObjs[0]);
				// Countries whose colors are included in the object here tend to be generated countries,
				// i.e. colonial nations which take on the color of their parent. To help distinguish 
				// these countries from their parent's other colonies we randomly adjust the color.
				//color.RandomlyFlunctuate(30);
			}
		}
	);
	registerKeyword(std::regex("capital"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt theCapital(theStream);
			capital = theCapital.getInt();
		}
	);
	registerKeyword(std::regex("technology_group"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString theTechGroup(theStream);
			techGroup = theTechGroup.getString();
		}
	);
	registerKeyword(std::regex("liberty_desire"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleDouble theLibertyDesire(theStream);
			libertyDesire = theLibertyDesire.getDouble();
		}
	);
	registerKeyword(std::regex("institutions"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::intList theInstitutions(theStream);
			for (auto institution: theInstitutions.getInts())
			{
				if (institution == 1)
				{
					embracedInstitutions.push_back(true);
				}
				else
				{
					embracedInstitutions.push_back(false);
				}
			}
		}
	);
	registerKeyword(std::regex("isolationism"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt isolationismValue(theStream);
			isolationism = isolationismValue.getInt();
		}
	);
	registerKeyword(std::regex("primary_culture"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString thePrimaryCulture(theStream);
			primaryCulture = thePrimaryCulture.getString();
		}
	);
	registerKeyword(std::regex("accepted_culture"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString theAcceptedCulture(theStream);
			acceptedCultures.push_back(theAcceptedCulture.getString());
		}
	);
	registerKeyword(std::regex("government_rank"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt theGovernmentRank(theStream);
			if ((theGovernmentRank.getInt() > 2) && (Configuration::wasDLCActive("The Cossacks")))
			{
				culturalUnion = EU4::cultureGroups::getCulturalGroup(primaryCulture);
			}
		}
	);
	registerKeyword(std::regex("realm_development"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt theDevelopment(theStream);
			if ((theDevelopment.getInt() >= 1000) && (!Configuration::wasDLCActive("The Cossacks")))
			{
				culturalUnion = EU4::cultureGroups::getCulturalGroup(primaryCulture);
			}
		}
	);
	registerKeyword(std::regex("culture_group_union"), [this](const std::string& unused, std::istream& theStream)
		{
			EU4::cultureGroup newUnion(tag + "_union", theStream);
			culturalUnion = newUnion;
		}
	);
	registerKeyword(std::regex("religion"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString theReligion(theStream);
			religion = theReligion.getString();
		}
	);
	registerKeyword(std::regex("score"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleDouble theScore(theStream);
			score = theScore.getDouble();
		}
	);
	registerKeyword(std::regex("stability"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleDouble theStability(theStream);
			stability = theStability.getDouble();
		}
	);
	registerKeyword(std::regex("technology"), [this](const std::string& unused, std::istream& theStream)
		{
			auto topObj = commonItems::convert8859Object(unused, theStream);
			auto techsObj = topObj->getLeaves();
			auto techObj = techsObj[0]->getValue("adm_tech");
			admTech = stof(techObj[0]->getLeaf());

			techObj = techsObj[0]->getValue("dip_tech");
			dipTech = stof(techObj[0]->getLeaf());

			techObj = techsObj[0]->getValue("mil_tech");
			milTech = stof(techObj[0]->getLeaf());
		}
	);
	registerKeyword(std::regex("flags"), [this](const std::string& unused, std::istream& theStream)
		{
			auto flagsObj = commonItems::convert8859Object(unused, theStream);
			for (auto flagObject: flagsObj->getLeaves()[0]->getLeaves())
			{
				flags[flagObject->getKey()] = true;
			}
		}
	);
	registerKeyword(std::regex("hidden_flags"), [this](const std::string& unused, std::istream& theStream)
		{
			auto flagsObj = commonItems::convert8859Object(unused, theStream);
			for (auto flagObject: flagsObj->getLeaves()[0]->getLeaves())
			{
				flags[flagObject->getKey()] = true;
			}
		}
	);
	registerKeyword(std::regex("modifier"), [this](const std::string& unused, std::istream& theStream)
		{
			auto modifierObj = commonItems::convert8859Object(unused, theStream);
			vector<shared_ptr<Object>> subModifierObj = modifierObj->getLeaves()[0]->getValue("modifier");
			if (subModifierObj.size() > 0)
			{
				modifiers[subModifierObj[0]->getLeaf()] = true;
			}
		}
	);
	registerKeyword(std::regex("variables"), [this](const std::string& unused, std::istream& theStream)
		{
			auto variablesObj = commonItems::convert8859Object(unused, theStream);
			for (auto variableObject: variablesObj->getLeaves()[0]->getLeaves())
			{
				flags[variableObject->getKey()] = true;
			}
		}
	);
	commonItems::parsingFunction governmentFunction = std::bind(&EU4::Country::getGovernmentFromStream, this, std::placeholders::_1, std::placeholders::_2);
	registerKeyword(std::regex("government"), governmentFunction);
	registerKeyword(std::regex("active_relations"), [this](const std::string& unused, std::istream& theStream)
		{
			auto relationLeaves = commonItems::convert8859Object(unused, theStream);
			for (auto relationLeaf: relationLeaves->getLeaves()[0]->getLeaves())
			{
				string key = relationLeaf->getKey();
				EU4Relations* rel = new EU4Relations(relationLeaf);
				relations.insert(make_pair(key, rel));
			}
		}
	);
	registerKeyword(std::regex("army"), [this](const std::string& unused, std::istream& theStream)
		{
			auto armyObj = commonItems::convert8859Object(unused, theStream);
			EU4Army* army = new EU4Army(armyObj->getLeaves()[0]);
			armies.push_back(army);
		}
	);
	registerKeyword(std::regex("navy"), [this](const std::string& unused, std::istream& theStream)
		{
			auto armyObj = commonItems::convert8859Object(unused, theStream);
			EU4Army* navy = new EU4Army(armyObj->getLeaves()[0]);
			armies.push_back(navy);
		}
	);
	registerKeyword(std::regex("active_idea_groups"), [this](const std::string& unused, std::istream& theStream)
		{
			auto topObject = commonItems::convert8859Object(unused, theStream);
			auto activeIdeasObjs = topObject->getLeaves();
			for (auto ideasObj: activeIdeasObjs[0]->getLeaves())
			{
				nationalIdeas.insert(make_pair(ideasObj->getKey(), stoi(ideasObj->getLeaf())));
			}
		}
	);
	registerKeyword(std::regex("legitimacy"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleDouble theLegitimacy(theStream);
			legitimacy = theLegitimacy.getDouble();
		}
	);
	registerKeyword(std::regex("parent"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString alsoUnused(theStream);
			colony = true;
		}
	);
	registerKeyword(std::regex("colonial_parent"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString alsoUnused(theStream);
			colony = true;
		}
	);
	registerKeyword(std::regex("overlord"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString theOverlord(theStream);
			overlord = theOverlord.getString();
		}
	);
	registerKeyword(std::regex("country_colors"), [this](const std::string& unused, std::istream& theStream)
		{
			auto customFlagObj = commonItems::convert8859Object(unused, theStream);
			vector<shared_ptr<Object>> flag = customFlagObj->getValue("flag");
			vector<shared_ptr<Object>> emblem = customFlagObj->getValue("subject_symbol_index");
			vector<shared_ptr<Object>> colours = customFlagObj->getValue("flag_colors");

			if (flag.size() > 0 && emblem.size() > 0 && colours.size() > 0)
			{
				customFlag.flag = to_string(1+stoi(flag[0]->getLeaf()));
				customFlag.emblem = stoi(emblem[0]->getLeaf())+1;

				vector<string> colourtokens = colours[0]->getTokens();
				customFlag.colours = std::make_tuple(stoi(colourtokens[0]), stoi(colourtokens[1]), stoi(colourtokens[2]));
			}
		}
	);
	registerKeyword(std::regex("revolutionary_colors"), [this](const std::string& unused, std::istream& theStream)
		{
			auto colorTokens = commonItems::intList(theStream).getInts();
			revolutionaryTricolour = std::make_tuple(colorTokens[0], colorTokens[1], colorTokens[2]);
		}
	);
	registerKeyword(std::regex("history"), [this](const std::string& unused, std::istream& theStream)
		{
			EU4::countryHistory theCountryHistory(theStream);

			for (auto& leader: theCountryHistory.getItemsOfType("leader"))
			{
				auto actualLeader = std::static_pointer_cast<EU4::historyLeader>(leader)->getTheLeader();
				if (actualLeader->isAlive())
				{
					militaryLeaders.push_back(actualLeader);
				}
			}
			/*vector<shared_ptr<Object>> daimyoObj = historyObj[0]->getValue("daimyo");	// the object holding the daimyo information for this country
			if (daimyoObj.size() > 0)
			{
				possibleDaimyo = true;
			}*/
		}
	);

	registerKeyword(std::regex("[a-z0-9\\_]+"), commonItems::ignoreItem);

	parseStream(theStream);

	determineJapaneseRelations();
	determineInvestments();
	determineLibertyDesire();
}


void EU4::Country::getGovernmentFromStream(const std::string& unused, std::istream& theStream)
{
	auto equals = getNextTokenWithoutMatching(theStream);
	auto next = getNextTokenWithoutMatching(theStream);
	if (next == "{")
	{
		while (next != "government")
		{
			next = getNextTokenWithoutMatching(theStream);
		}
		equals = getNextTokenWithoutMatching(theStream);
		government = *getNextTokenWithoutMatching(theStream);
		auto closingBracket = getNextTokenWithoutMatching(theStream);
	}
	else
	{
		government = *next;
	}
}


void EU4::Country::determineJapaneseRelations()
{
	if (government == "daimyo") 
	{
		possibleDaimyo = true;
	}

	if (government == "shogunate")
	{
		possibleShogun = true;
	}
}


void EU4::Country::determineInvestments()
{
	for (auto idea: nationalIdeas)
	{
		armyInvestment += ideaEffectMapper::getArmyInvestmentFromIdea(idea.first, idea.second);
		commerceInvestment += ideaEffectMapper::getCommerceInvestmentFromIdea(idea.first, idea.second);
		cultureInvestment += ideaEffectMapper::getCultureInvestmentFromIdea(idea.first, idea.second);
		industryInvestment += ideaEffectMapper::getIndustryInvestmentFromIdea(idea.first, idea.second);
		navyInvestment += ideaEffectMapper::getNavyInvestmentFromIdea(idea.first, idea.second);
	}
}


void EU4::Country::determineLibertyDesire()
{
	if ((colony) && (libertyDesire != 0.0))
	{
		auto relationship = relations.find(overlord);
		if (relationship != relations.end())
		{
			string attitude = relationship->second->getAttitude();
			if (attitude == "attitude_rebellious")
			{
				libertyDesire = 95.0;
			}
			else if (attitude == "attitude_disloyal")
			{
				libertyDesire = 90.0;
			}
			else if (attitude == "attitude_disloyal_vassal")	// for pre-1.14 games
			{
				libertyDesire = 90.0;
			}
			else if (attitude == "attitude_outraged")
			{
				libertyDesire = 85.0;
			}
			else if (attitude == "atittude_rivalry")
			{
				libertyDesire = 80.0;
			}
			else if (attitude == "attitude_hostile")
			{
				libertyDesire = 75.0;
			}
			else if (attitude == "attitude_threatened")
			{
				libertyDesire = 65.0;
			}
			else if (attitude == "attitude_neutral")
			{
				libertyDesire = 50.0;
			}
			else if (attitude == "attitude_defensive")
			{
				libertyDesire = 35.0;
			}
			else if (attitude == "attitude_domineering")
			{
				libertyDesire = 20.0;
			}
			else if (attitude == "attitude_protective")
			{
				libertyDesire = 15.0;
			}
			else if (attitude == "attitude_allied")
			{
				libertyDesire = 10.0;
			}
			else if (attitude == "attitude_friendly")
			{
				libertyDesire = 10.0;
			}
			else if (attitude == "attitude_loyal")
			{
				libertyDesire = 5.0;
			}
			else if (attitude == "attitude_overlord")
			{
				libertyDesire = 5.0;
			}
			else if (attitude == "attitude_vassal")	// for pre-1.14 games
			{
				libertyDesire = 5.0;
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown attitude type " << attitude << " while setting liberty desire for " << tag;
				libertyDesire = 95.0;
			}
		}
	}
}


void EU4::Country::readFromCommonCountry(const std::string& fileName, const std::string& fullFilename)
{
	if (name.empty())
	{
		// For this country's name we will use the stem of the file name.
		size_t extPos = fileName.find_last_of('.');
		name = fileName.substr(0, extPos);
	}

	if (!color)
	{
		registerKeyword(std::regex("graphical_culture"), commonItems::ignoreString);
		registerKeyword(std::regex("color"), [this](const std::string& unused, std::istream& theStream)
			{
				color = commonItems::Color(theStream);
			}
		);
		registerKeyword(std::regex("revolutionary_colors"), commonItems::ignoreObject);
		registerKeyword(std::regex("historical_score"), commonItems::ignoreString);
		registerKeyword(std::regex("preferred_religion"), commonItems::ignoreString);
		registerKeyword(std::regex("historical_idea_groups"), commonItems::ignoreObject);
		registerKeyword(std::regex("historical_units"), commonItems::ignoreObject);
		registerKeyword(std::regex("monarch_names"), commonItems::ignoreObject);
		registerKeyword(std::regex("leader_names"), commonItems::ignoreObject);
		registerKeyword(std::regex("ship_names"), commonItems::ignoreObject);
		registerKeyword(std::regex("army_names"), commonItems::ignoreObject);
		registerKeyword(std::regex("fleet_names"), commonItems::ignoreObject);
		registerKeyword(std::regex("colonial_parent"), commonItems::ignoreString);
		registerKeyword(std::regex("random_nation_chance"), commonItems::ignoreString);
		registerKeyword(std::regex("random_nation_extra_size"), commonItems::ignoreString);
		registerKeyword(std::regex("right_to_bear_arms"), commonItems::ignoreString);
		parseFile(fullFilename);
	}
}


void EU4::Country::setLocalisationName(const string& language, const string& name)
{
	namesByLanguage[language] = name;
}


void EU4::Country::setLocalisationAdjective(const string& language, const string& adjective)
{
	adjectivesByLanguage[language] = adjective;
}


void EU4::Country::addProvince(EU4Province* province)
{
	provinces.push_back(province);
}


void EU4::Country::addCore(EU4Province* core)
{
	cores.push_back(core);
}


bool EU4::Country::hasModifier(string modifier) const
{
	map<string, bool>::const_iterator itr = modifiers.find(modifier);
	return (itr != modifiers.end());
}


int EU4::Country::hasNationalIdea(string ni) const
{
	map<string, int>::const_iterator itr = nationalIdeas.find(ni);
	if (itr != nationalIdeas.end())
	{
		return itr->second;
	}
	else
	{
		return -1;
	}
}


bool EU4::Country::hasFlag(string flag) const
{
	map<string, bool>::const_iterator itr = flags.find(flag);
	return (itr != flags.end());
}


void EU4::Country::resolveRegimentTypes(const RegimentTypeMap& map)
{
	for (vector<EU4Army*>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		(*itr)->resolveRegimentTypes(map);
	}
}


int EU4::Country::getManufactoryCount() const
{
	int retval = 0;	// the number of manus
	for (vector<EU4Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->hasBuilding("weapons"))		++retval;
		if ((*itr)->hasBuilding("wharf"))		++retval;
		if ((*itr)->hasBuilding("textile"))		++retval;
		if ((*itr)->hasBuilding("refinery"))	++retval;
	}
	return retval;
}


void EU4::Country::eatCountry(std::shared_ptr<EU4::Country> target, std::shared_ptr<Country> self)
{
	// autocannibalism is forbidden
	if (target->getTag() == tag)
	{
		return;
	}

	// for calculation of weighted averages
	int totalProvinces = target->provinces.size() + provinces.size();
	if (totalProvinces == 0)
	{
		totalProvinces = 1;
	}
	const double myWeight = (double)provinces.size() / (double)totalProvinces;						// the amount of influence from the main country
	const double targetWeight = (double)target->provinces.size() / (double)totalProvinces;		// the amount of influence from the target country

	// acquire target's cores (always)
	for (unsigned int j = 0; j < target->cores.size(); j++)
	{
		addCore(target->cores[j]);
		target->cores[j]->addCore(tag);
		target->cores[j]->removeCore(target->tag);
	}

	// everything else, do only if this country actually currently exists
	if (target->provinces.size() > 0)
	{
		// acquire target's provinces
		for (unsigned int j = 0; j < target->provinces.size(); j++)
		{
			addProvince(target->provinces[j]);
			target->provinces[j]->setOwner(self);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());
		militaryLeaders.insert(militaryLeaders.end(), target->militaryLeaders.begin(), target->militaryLeaders.end());

		// rebalance prestige, badboy, inflation and techs from weighted average
		score						= myWeight * score						+ targetWeight * target->score;
		admTech					= myWeight * admTech						+ targetWeight * target->admTech;
		dipTech					= myWeight * dipTech						+ targetWeight * target->dipTech;
		milTech					= myWeight * milTech						+ targetWeight * target->milTech;
		armyInvestment			= myWeight * armyInvestment			+ targetWeight * target->armyInvestment;
		navyInvestment			= myWeight * navyInvestment			+ targetWeight * target->navyInvestment;
		commerceInvestment	= myWeight * commerceInvestment		+ targetWeight * target->commerceInvestment;
		industryInvestment	= myWeight * industryInvestment		+ targetWeight * target->industryInvestment;
		cultureInvestment		= myWeight * cultureInvestment		+ targetWeight * target->cultureInvestment;
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();

	LOG(LogLevel::Debug) << "Merged " << target->tag << " into " << tag;
}


void EU4::Country::takeArmies(std::shared_ptr<Country> target)
{
	// acquire target's armies, navies, admirals, and generals
	armies.insert(armies.end(), target->armies.begin(), target->armies.end());
	militaryLeaders.insert(militaryLeaders.end(), target->militaryLeaders.begin(), target->militaryLeaders.end());
	target->clearArmies();
}


void EU4::Country::clearArmies()
{
	armies.clear();
	militaryLeaders.clear();
}


bool EU4::Country::cultureSurvivesInCores()
{
	for (auto core: cores)
	{
		if (core->getOwner() == NULL)
		{
			continue;
		}
		if (core->getOwner()->getPrimaryCulture() == primaryCulture)
		{
			continue;
		}

		if (core->getCulturePercent(primaryCulture) >= 0.5)
		{
			return true;
		}
	}

	return false;
}


string EU4::Country::getName(const string& language) const
{
	if (!randomName.empty())
	{
		return randomName;
	}

	if (namesByLanguage.empty() && language == "english")
	{
		return name;
	}

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


string EU4::Country::getAdjective(const string& language) const
{
	if (!randomName.empty())
	{
		return randomName;
	}

	if (adjectivesByLanguage.empty() && language == "english")
	{
		return adjective;
	}

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

int EU4::Country::numEmbracedInstitutions() const {
	int total = 0;
	for (unsigned int i = 0; i < embracedInstitutions.size(); i++) {
		if (embracedInstitutions[i]) total++;
	}
	return total;
}


void EU4::Country::clearProvinces()
{
	provinces.clear();
}


void EU4::Country::clearCores()
{
	cores.clear();
}