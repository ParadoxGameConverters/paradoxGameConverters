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



#include "HoI4FocusTree.h"
#include "HoI4Country.h"
#include "HoI4Focus.h"
#include "HoI4Localisation.h"
#include "../Configuration.h"
#include "../V2World/Party.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "ParserHelpers.h"
#include <fstream>
using namespace std;



//this is used to cache focuses that can be just loaded from a file
static std::map<std::string, HoI4Focus> loadedFocuses;



HoI4FocusTree::HoI4FocusTree(const HoI4Country& country):
	srcCountryTag(country.getSourceCountry()->getTag()),
	dstCountryTag(country.getTag())
{
}


void HoI4FocusTree::addGenericFocusTree(const set<string>& majorIdeologies)
{
	confirmLoadedFocuses();

	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>(loadedFocuses.find("army_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("equipment_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("motorization_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("doctrine_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("equipment_effort_2")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("mechanization_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("doctrine_effort_2")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("equipment_effort_3")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("armor_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("special_forces")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("aviation_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("fighter_focus")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("bomber_focus")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("aviation_effort_2")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("CAS_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("rocket_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("NAV_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("naval_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("flexible_navy")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("large_navy")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("submarine_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("cruiser_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("destroyer_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("capital_ships_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("industrial_effort")->second);
	newFocus->aiWillDo  = "= {\n";
	newFocus->aiWillDo += "			factor = 3\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				date < 1939.1.1\n";
	newFocus->aiWillDo += "				OR = {\n";
	newFocus->aiWillDo += "					# we also dont want tiny nations to go crazy with slots right away\n";
	newFocus->aiWillDo += "					num_of_controlled_states < 2\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("construction_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("production_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("construction_effort_2")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("production_effort_2")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("infrastructure_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("production_effort_3")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("infrastructure_effort_2")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("construction_effort_3")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("nuclear_effort")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("extra_tech_slot")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("extra_tech_slot_2")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("secret_weapons")->second);
	focuses.push_back(newFocus);

	int numCollectovistIdeologies = calculateNumCollectovistIdeologies(majorIdeologies);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("political_effort")->second);
	newFocus->xPos = static_cast<int>((numCollectovistIdeologies * 1.5) + 16);
	focuses.push_back(newFocus);

	if (numCollectovistIdeologies > 0)
	{
		newFocus = make_shared<HoI4Focus>(loadedFocuses.find("collectivist_ethos")->second);
		newFocus->available  = "= {\n";
		newFocus->available += "			OR = {\n";
		for (auto majorIdeology: majorIdeologies)
		{
			if (majorIdeology == "democratic")
			{
				continue;
			}
			newFocus->available += "				has_government = " + majorIdeology + "\n";
		}
		newFocus->available += "			}\n";
		newFocus->available += "		}";
		newFocus->xPos = -(numCollectovistIdeologies / 2) - 1;
		newFocus->completionReward = "= {\n";
		if (majorIdeologies.count("democratic") > 0)
		{
			newFocus->completionReward += "			add_ideas = collectivist_ethos_focus_democratic\n";
		}
		else
		{
			newFocus->completionReward += "			add_ideas = collectivist_ethos_focus_neutral\n";
		}
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);

		determineMutualExclusions(majorIdeologies);

		string ideolgicalFanaticsmPrereqs;
		int relativePosition = 1 - numCollectovistIdeologies;
		if (majorIdeologies.count("fascism") > 0)
		{
			addFascistGenericFocuses(relativePosition, majorIdeologies);
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = paramilitarism";
			relativePosition += 2;
		}
		if (majorIdeologies.count("communism") > 0)
		{
			addCommunistGenericFocuses(relativePosition);
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = political_commissars";
			relativePosition += 2;
		}
		if (majorIdeologies.count("absolutist") > 0)
		{
			addAbsolutistGenericFocuses(relativePosition);
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = historical_claims_focus";
			relativePosition += 2;
		}
		if (majorIdeologies.count("radical") > 0)
		{
			addRadicalGenericFocuses(relativePosition);
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = army_provides_focus";
		}

		newFocus = make_shared<HoI4Focus>(loadedFocuses.find("ideological_fanaticism")->second);
		newFocus->prerequisites.clear();
		newFocus->prerequisites.push_back("= { " + ideolgicalFanaticsmPrereqs + " }");
		newFocus->xPos = 0;
		newFocus->yPos = 6;
		newFocus->relativePositionId = "collectivist_ethos";
		focuses.push_back(newFocus);
	}

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("liberty_ethos")->second);
	if (numCollectovistIdeologies == 0)
	{
		newFocus->mutuallyExclusive.clear();
	}
	if (majorIdeologies.count("democratic") == 0)
	{
		newFocus->available = "= {\n";
		newFocus->available += "			has_government = neutrality\n";
		newFocus->available += "		}";
	}
	newFocus->xPos = (numCollectovistIdeologies + 1) / 2;
	newFocus->aiWillDo  = "= {\n";
	newFocus->aiWillDo += "			factor = 95\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0.1\n";
	newFocus->aiWillDo += "				any_neighbor_country = {\n";
	newFocus->aiWillDo += "					is_major = yes\n";
	newFocus->aiWillDo += "					OR = {\n";
	for (auto majorIdeology: majorIdeologies)
	{
		newFocus->aiWillDo += "						has_government = " + majorIdeology + "\n";
	}
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	if (majorIdeologies.count("democratic") > 0)
	{
		newFocus->aiWillDo += "				NOT = {\n";
		newFocus->aiWillDo += "					any_neighbor_country = {\n";
		newFocus->aiWillDo += "						is_major = yes\n";
		newFocus->aiWillDo += "						has_government = democratic\n";
		newFocus->aiWillDo += "					}\n";
		newFocus->aiWillDo += "				}\n";
	}
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("neutrality_focus")->second);
	if (majorIdeologies.count("democratic") == 0)
	{
		newFocus->mutuallyExclusive.clear();
	}
	newFocus->aiWillDo  = "= {\n";
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "		}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("deterrence")->second);
	focuses.push_back(newFocus);

	if (majorIdeologies.count("democratic") != 0)
	{
		newFocus = make_shared<HoI4Focus>(loadedFocuses.find("interventionism_focus")->second);
		focuses.push_back(newFocus);

		newFocus = make_shared<HoI4Focus>(loadedFocuses.find("volunteer_corps")->second);
		focuses.push_back(newFocus);

		newFocus = make_shared<HoI4Focus>(loadedFocuses.find("foreign_expeditions")->second);
		focuses.push_back(newFocus);
	}

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("why_we_fight")->second);
	if (majorIdeologies.count("democratic") == 0)
	{
		newFocus->prerequisites.clear();
		newFocus->prerequisites.push_back("= { focus = deterrence }");
	}
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("technology_sharing")->second);
	if (numCollectovistIdeologies == 0)
	{
		newFocus->prerequisites.clear();
		newFocus->prerequisites.push_back("= { focus = why_we_fight }");
	}
	newFocus->xPos = numCollectovistIdeologies;
	focuses.push_back(newFocus);

	nextFreeColumn = static_cast<int>(numCollectovistIdeologies * 1.5) + ((numCollectovistIdeologies + 1) / 2) + 20;
}


void HoI4FocusTree::confirmLoadedFocuses()
{
	if (loadedFocuses.size() == 0)
	{
		registerKeyword(std::regex("focus_tree"), [this](const std::string& unused, std::istream& theStream) {});
		registerKeyword(std::regex("id"), commonItems::ignoreString);
		registerKeyword(std::regex("country"), commonItems::ignoreObject);
		registerKeyword(std::regex("default"), commonItems::ignoreString);
		registerKeyword(std::regex("reset_on_civilwar"), commonItems::ignoreString);
		registerKeyword(std::regex("focus"), [this](const std::string& unused, std::istream& theStream)
			{
				HoI4Focus newFocus(theStream);
				loadedFocuses.insert(make_pair(newFocus.id, newFocus));
			}
		);

		parseFile(Configuration::getHoI4Path() + "/common/national_focus/generic.txt");
		parseFile("converterFocuses.txt");
	}
}


int HoI4FocusTree::calculateNumCollectovistIdeologies(const set<string>& majorIdeologies)
{
	int numCollectovistIdeologies = 0;
	numCollectovistIdeologies += majorIdeologies.count("radical");
	numCollectovistIdeologies += majorIdeologies.count("absolutist");
	numCollectovistIdeologies += majorIdeologies.count("communism");
	numCollectovistIdeologies += majorIdeologies.count("fascism");
	return numCollectovistIdeologies;
}


void HoI4FocusTree::determineMutualExclusions(const set<string>& majorIdeologies)
{
	if (majorIdeologies.count("fascism") > 0)
	{
		communistMutualExclusions += "focus = nationalism_focus";
		absolutistMutualExlusions += "focus = nationalism_focus";
		radicalMutualExclusions += "focus = nationalism_focus";
	}
	if (majorIdeologies.count("communism") > 0)
	{
		if (fascistMutualExlusions.size() > 0)
		{
			fascistMutualExlusions += " ";
		}
		if (absolutistMutualExlusions.size() > 0)
		{
			absolutistMutualExlusions += " ";
		}
		if (radicalMutualExclusions.size() > 0)
		{
			radicalMutualExclusions += " ";
		}
		fascistMutualExlusions += "focus = internationalism_focus";
		absolutistMutualExlusions += "focus = internationalism_focus";
		radicalMutualExclusions += "focus = internationalism_focus";
	}
	if (majorIdeologies.count("absolutist") > 0)
	{
		if (fascistMutualExlusions.size() > 0)
		{
			fascistMutualExlusions += " ";
		}
		if (communistMutualExclusions.size() > 0)
		{
			communistMutualExclusions += " ";
		}
		if (radicalMutualExclusions.size() > 0)
		{
			radicalMutualExclusions += " ";
		}
		fascistMutualExlusions += "focus = absolutism_focus";
		communistMutualExclusions += "focus = absolutism_focus";
		radicalMutualExclusions += "focus = absolutism_focus";
	}
	if (majorIdeologies.count("radical") > 0)
	{
		if (fascistMutualExlusions.size() > 0)
		{
			fascistMutualExlusions += " ";
		}
		if (communistMutualExclusions.size() > 0)
		{
			communistMutualExclusions += " ";
		}
		if (absolutistMutualExlusions.size() > 0)
		{
			absolutistMutualExlusions += " ";
		}
		fascistMutualExlusions += "focus = radical_focus";
		communistMutualExclusions += "focus = radical_focus";
		absolutistMutualExlusions += "focus = radical_focus";
	}
}


void HoI4FocusTree::addFascistGenericFocuses(int relativePosition, const std::set<std::string>& majorIdeologies)
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>(loadedFocuses.find("nationalism_focus")->second);
	newFocus->mutuallyExclusive = "= { " + fascistMutualExlusions + " }";
	newFocus->xPos = relativePosition;
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("militarism")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("military_youth")->second);
	newFocus->completionReward  = "= {\n";
	newFocus->completionReward += "			add_ideas = military_youth_focus\n";
	for (auto ideology: majorIdeologies)
	{
		newFocus->completionReward += "			if = {\n";
		newFocus->completionReward += "				limit = { has_government = " + ideology + " }\n";
		newFocus->completionReward += "				add_popularity = {\n";
		newFocus->completionReward += "					ideology = " + ideology + "\n";
		newFocus->completionReward += "					popularity = 0.2\n";
		newFocus->completionReward += "				}\n";
		newFocus->completionReward += "			}\n";
	}
	newFocus->completionReward  += "		}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("paramilitarism")->second);
	focuses.push_back(newFocus);
}


void HoI4FocusTree::addCommunistGenericFocuses(int relativePosition)
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>(loadedFocuses.find("internationalism_focus")->second);
	newFocus->mutuallyExclusive = "= { " + communistMutualExclusions + " }";
	newFocus->available  = "= {\n";
	newFocus->available += "			OR = {\n";
	newFocus->available += "				has_government = communism\n";
	newFocus->available += "				has_government = neutrality\n";
	newFocus->available += "			}\n";
	newFocus->available += "		}";
	newFocus->xPos = relativePosition;
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("political_correctness")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("indoctrination_focus")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("political_commissars")->second);
	newFocus->completionReward  = "= {\n";
	newFocus->completionReward += "			add_ideas = political_commissars_focus\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = { has_government = communism }\n";
	newFocus->completionReward += "				add_popularity = {\n";
	newFocus->completionReward += "					ideology = communism\n";
	newFocus->completionReward += "					popularity = 0.2\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			add_political_power = 200\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);
}


void HoI4FocusTree::addAbsolutistGenericFocuses(int relativePosition)
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>(loadedFocuses.find("absolutism_focus")->second);
	newFocus->mutuallyExclusive = "= { " + absolutistMutualExlusions + " }";
	newFocus->xPos = relativePosition;
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("royal_dictatorship_focus")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("royal_army_tradition_focus")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("historical_claims_focus")->second);
	focuses.push_back(newFocus);
}


void HoI4FocusTree::addRadicalGenericFocuses(int relativePosition)
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>(loadedFocuses.find("radical_focus")->second);
	newFocus->mutuallyExclusive = "= { " + radicalMutualExclusions + " }";
	newFocus->xPos = relativePosition;
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("private_channels_focus")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("hardfought_market_focus")->second);
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>(loadedFocuses.find("army_provides_focus")->second);
	focuses.push_back(newFocus);
}


shared_ptr<HoI4FocusTree> HoI4FocusTree::makeCustomizedCopy(const HoI4Country& country) const
{
	auto newFocusTree = make_shared<HoI4FocusTree>(country);

	for (auto focus: focuses)
	{
		auto newFocus = focus->makeCustomizedCopy(newFocusTree->dstCountryTag);
		newFocusTree->addFocus(newFocus);
	}
	newFocusTree->setNextFreeColumn(nextFreeColumn);

	return newFocusTree;
}


void HoI4FocusTree::addDemocracyNationalFocuses(shared_ptr<HoI4Country> Home, vector<shared_ptr<HoI4Country>>& CountriesToContain)
{
	double WTModifier = 1;
	if (Home->getGovernmentIdeology() == "democratic")
	{
		string warPol = Home->getRulingParty().getWarPolicy();
		if (warPol == "jingoism")
		{
			WTModifier = 0;
		}
		if (warPol == "pro_military")
		{
			WTModifier = 0.25;
		}
		if (warPol == "anti_military")
		{
			WTModifier = 0.5;
		}
		if (warPol == "pacifism" || warPol == "pacifist")
		{
			WTModifier = 0.5;
		}
	}

	shared_ptr<HoI4Focus> newFocus = loadedFocuses.find("WarProp")->second.makeCustomizedCopy(Home->getTag());
	newFocus->available = "= {\n";
	newFocus->available += "			threat > " + to_string(0.20 * WTModifier / 1000) + "\n";
	newFocus->available += "		}";
	newFocus->xPos = nextFreeColumn + (CountriesToContain.size() * 2) - 1;
	focuses.push_back(newFocus);

	newFocus = loadedFocuses.find("PrepInter")->second.makeCustomizedCopy(Home->getTag());
	newFocus->available = "= {\n";
	newFocus->available += "			threat > " + to_string(0.30 * WTModifier / 1000) + "\n";
	newFocus->available += "		}";
	focuses.push_back(newFocus);

	newFocus = loadedFocuses.find("Lim")->second.makeCustomizedCopy(Home->getTag());
	newFocus->available = "= {\n";
	newFocus->available += "			threat > " + to_string(0.50 * WTModifier / 1000) + "\n";
	newFocus->available += "		}";
	focuses.push_back(newFocus);

	int relavtivePos = 1 - (CountriesToContain.size() * 2);
	for (auto country: CountriesToContain)
	{
		auto possibleContainedCountryName = country->getSourceCountry()->getName("english");
		string containedCountryName;
		if (possibleContainedCountryName)
		{
			containedCountryName = *possibleContainedCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not determine contained country name for democratic focuses";
			containedCountryName = "";
		}

		newFocus = loadedFocuses.find("WarPlan")->second.makeCustomizedCopy(Home->getTag());
		newFocus->id += country->getTag();
		newFocus->text = "War Plan " + containedCountryName;
		newFocus->bypass = "= {\n";
		newFocus->bypass += "			has_war_with = " + country->getTag() + "\n";
		newFocus->bypass += "		}";
		newFocus->xPos = relavtivePos;
		newFocus->available = "= {\n";
		newFocus->available += "			any_other_country = {\n";
		newFocus->available += "				original_tag = " + country->getTag() + "\n";
		newFocus->available += "				exists = yes\n";
		newFocus->available += "				NOT = { is_in_faction_with = " + Home->getTag() + " }\n";
		newFocus->available += "				OR = {\n";
		newFocus->available += "					has_offensive_war = yes\n";
		newFocus->available += "					has_added_tension_amount > 30\n";
		newFocus->available += "				}\n";
		newFocus->available += "			}\n";
		newFocus->available += "		}";
		focuses.push_back(newFocus);
		HoI4Localisation::copyFocusLocalisations("WarPlan", newFocus->id);
		HoI4Localisation::updateLocalisationWithCountry(newFocus->id, "$TARGET", country->getTag() + "_neutrality");

		newFocus = loadedFocuses.find("Embargo")->second.makeCustomizedCopy(Home->getTag());
		newFocus->id += country->getTag();
		newFocus->text = "Embargo " + containedCountryName;
		newFocus->prerequisites.clear();
		newFocus->prerequisites.push_back("= { focus =  WarPlan" + Home->getTag() + country->getTag() + " }");
		newFocus->bypass = "= {\n";
		newFocus->bypass += "			has_war_with = " + country->getTag() + "\n";
		newFocus->bypass += "		}";
		newFocus->relativePositionId += country->getTag();
		newFocus->available = "= {\n";
		newFocus->available += "			any_other_country = {\n";
		newFocus->available += "				original_tag = " + country->getTag() + "\n";
		newFocus->available += "				exists = yes\n";
		newFocus->available += "				NOT = { is_in_faction_with = " + Home->getTag() + " }\n";
		newFocus->available += "				OR = {\n";
		newFocus->available += "					has_offensive_war = yes\n";
		newFocus->available += "					has_added_tension_amount > 30\n";
		newFocus->available += "					threat > 0.6\n";
		newFocus->available += "				}\n";
		newFocus->available += "			}\n";
		newFocus->available += "		}";
		newFocus->completionReward = "= {\n";
		newFocus->completionReward += "			" + country->getTag() + " = {\n";
		newFocus->completionReward += "				add_opinion_modifier = { target = " + Home->getTag() + " modifier = embargo }\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
		HoI4Localisation::copyFocusLocalisations("Embargo", newFocus->id);
		HoI4Localisation::updateLocalisationWithCountry(newFocus->id, "$TARGET", country->getTag() + "_neutrality");

		newFocus = loadedFocuses.find("WAR")->second.makeCustomizedCopy(Home->getTag());
		newFocus->id += country->getTag();
		newFocus->text = "Enact War Plan " + containedCountryName;
		newFocus->prerequisites.clear();
		newFocus->prerequisites.push_back("= { focus =  Embargo" + Home->getTag() + country->getTag() + " }");
		newFocus->prerequisites.push_back("= { focus =  Lim" + Home->getTag() + " }");
		newFocus->bypass = "= {\n";
		newFocus->bypass += "			has_war_with = " + country->getTag() + "\n";
		newFocus->bypass += "		}";
		newFocus->relativePositionId += country->getTag();
		newFocus->available  = "= {\n";
		newFocus->available += "			has_war = no\n";
		newFocus->available += "			any_other_country = {\n";
		newFocus->available += "				original_tag = " + country->getTag() + "\n";
		newFocus->available += "				exists = yes\n";
		newFocus->available += "				NOT = { is_in_faction_with = " + Home->getTag() + " }\n";
		newFocus->available += "				OR = {\n";
		newFocus->available += "					has_offensive_war = yes\n";
		newFocus->available += "					has_added_tension_amount > 30\n";
		newFocus->available += "					threat > 0.6\n";
		newFocus->available += "				}\n";
		newFocus->available += "			}\n";
		newFocus->available += "		}";
		newFocus->aiWillDo  = "= {\n";
		newFocus->aiWillDo += "			factor = 10\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward  = "= {\n";
		newFocus->completionReward += "			declare_war_on = {\n";
		newFocus->completionReward += "				type = puppet_wargoal_focus\n";
		newFocus->completionReward += "				target = " + country->getTag() + "\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
		HoI4Localisation::copyFocusLocalisations("WAR", newFocus->id);
		HoI4Localisation::updateLocalisationWithCountry(newFocus->id, "$TARGET", country->getTag() + "_neutrality");

		relavtivePos += 4;
	}
}


void HoI4FocusTree::addAbsolutistEmpireNationalFocuses(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& targetColonies, const vector<shared_ptr<HoI4Country>>& annexationTargets)
{
	auto possibleHomeCountryAdjective = Home->getSourceCountry()->getAdjective("english");
	string homeCountryAdjective;
	if (possibleHomeCountryAdjective)
	{
		homeCountryAdjective = *possibleHomeCountryAdjective;
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not determine home country adjective for absolutist focuses";
		homeCountryAdjective = "";
	}

	auto newFocus = loadedFocuses.find("EmpireGlory")->second.makeCustomizedCopy(Home->getTag());
	newFocus->xPos = nextFreeColumn + 5;
	focuses.push_back(newFocus);

	newFocus = loadedFocuses.find("StrengthenColonies")->second.makeCustomizedCopy(Home->getTag());
	newFocus->xPos = nextFreeColumn + 4;
	focuses.push_back(newFocus);

	newFocus = loadedFocuses.find("StrengthenHome")->second.makeCustomizedCopy(Home->getTag());
	newFocus->xPos = nextFreeColumn + 6;
	focuses.push_back(newFocus);

	newFocus = loadedFocuses.find("ColonialInd")->second.makeCustomizedCopy(Home->getTag());
	newFocus->xPos = nextFreeColumn + 2;
	focuses.push_back(newFocus);

	newFocus = loadedFocuses.find("ColonialHwy")->second.makeCustomizedCopy(Home->getTag());
	newFocus->xPos = nextFreeColumn;
	focuses.push_back(newFocus);

	newFocus = loadedFocuses.find("ResourceFac")->second.makeCustomizedCopy(Home->getTag());
	newFocus->xPos = nextFreeColumn + 2;
	focuses.push_back(newFocus);

	//establish colonial army
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "ColonialArmy" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_allies_build_infantry";
	newFocus->text += "Establish Colonial Army";
	newFocus->prerequisites.push_back("= { focus = StrengthenColonies" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 4;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			add_ideas = militarism_focus\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//establish protectorate
	if (targetColonies.size() >= 1)
	{
		auto target = targetColonies.front();

		auto possibleProtectorateCountryName = target->getSourceCountry()->getName("english");
		string protectorateCountryName;
		if (possibleProtectorateCountryName)
		{
			protectorateCountryName = *possibleProtectorateCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not determine protectorate country name for absolutist focuses";
			protectorateCountryName = "";
		}

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Protectorate" + Home->getTag() + target->getTag();
		newFocus->icon = "GFX_goal_generic_major_war";
		newFocus->text += "Establish Protectorate over " + protectorateCountryName;
		newFocus->available += "= {\n";
		newFocus->available += "			" + target->getTag() + " = { is_in_faction = no }\n";
		newFocus->available += "		}";
		newFocus->prerequisites.push_back("= { focus = ColonialArmy" + Home->getTag() + " }");
		newFocus->xPos = nextFreeColumn + 4;
		newFocus->yPos = 3;
		newFocus->cost = 10;
		newFocus->bypass += "= {\n";
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() +"\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}\n";
		newFocus->bypass += "		}";
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 10\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
	}
	if (targetColonies.size() >= 2)
	{
		auto target = targetColonies.back();

		auto possibleProtectorateCountryName = target->getSourceCountry()->getName("english");
		string protectorateCountryName;
		if (possibleProtectorateCountryName)
		{
			protectorateCountryName = *possibleProtectorateCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not determine protectorate country name for absolutist focuses";
			protectorateCountryName = "";
		}

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Protectorate" + Home->getTag() + target->getTag();
		newFocus->icon = "GFX_goal_generic_major_war";
		newFocus->text += "Establish Protectorate over " + protectorateCountryName;
		newFocus->available += "= {\n";
		newFocus->available += "			" + target->getTag() + " = { is_in_faction = no }\n";
		newFocus->available += "		}";
		newFocus->prerequisites.push_back("= { focus = Protectorate" + Home->getTag() + targetColonies.front()->getTag() + " }");
		newFocus->xPos = nextFreeColumn + 4;
		newFocus->yPos = 4;
		newFocus->cost = 10;
		newFocus->bypass += "= {\n";
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() +"\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}\n";
		newFocus->bypass += "		}";
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
	}

	//Trade Empire
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "TradeEmpire" + Home->getTag();
	newFocus->icon = "GFX_goal_anschluss";
	newFocus->text += "Fund the " + homeCountryAdjective + " Colonial Trade Corporation";
	newFocus->prerequisites.push_back("= { focus = ColonialHwy" + Home->getTag() + " focus = ResourceFac" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 1;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			add_ideas = established_traders\n";
	newFocus->completionReward += "			set_country_flag = established_traders\n";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "						is_coastal = yes\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = no\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = no\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 2\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = dockyard\n";
	newFocus->completionReward += "					level = 2\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//Home Industry Buildup
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "IndHome" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_production";
	newFocus->text += "Fund Industrial Improvement";
	newFocus->prerequisites.push_back("= { focus = StrengthenHome" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 7;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = industrial_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = industry\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//National Highway
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "NationalHwy" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_construct_infrastructure";
	newFocus->text += "National Highway";
	newFocus->prerequisites.push_back("= { focus = IndHome" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 6;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 1\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 1\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 1\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//National College
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "NatCollege" + Home->getTag();
	newFocus->icon = "GFX_goal_anschluss";
	newFocus->text += "Establish National College";
	newFocus->prerequisites.push_back("= { focus = IndHome" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 8;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			add_ideas = national_college\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//Improve Factories
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "MilitaryBuildup" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_construct_mil_factory";
	newFocus->text += "Military Buildup";
	newFocus->prerequisites.push_back("= { focus = NatCollege" + Home->getTag() + " }");
	newFocus->prerequisites.push_back("= { focus = NationalHwy" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 9;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = arms_factory\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = arms_factory\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 1\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = arms_factory\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = arms_factory\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = arms_factory\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 1\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = arms_factory\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = arms_factory\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = arms_factory\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 1\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = arms_factory\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//PrepBorder
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "PrepTheBorder" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_defence";
	newFocus->text += "Prepare the Border";
	newFocus->prerequisites.push_back("= { focus = StrengthenHome" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 10;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			add_ideas = border_buildup\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//Promote Nationalistic Spirit
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "NatSpirit" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_political_pressure";
	newFocus->text += "Promote Nationalistic Spirit";
	newFocus->prerequisites.push_back("= { focus = PrepTheBorder" + Home->getTag() + " }");
	newFocus->xPos = nextFreeColumn + 10;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "= {\n";
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "		}";
	newFocus->completionReward += "= {\n";
	newFocus->completionReward += "			add_ideas = paramilitarism_focus\n";
	newFocus->completionReward += "		}";
	focuses.push_back(newFocus);

	//ANNEX
	if (annexationTargets.size() >= 1)
	{
		auto target = annexationTargets.front();

		auto possibleTargetCountryName = target->getSourceCountry()->getName("english");
		string targetCountryName;
		if (possibleTargetCountryName)
		{
			targetCountryName = *possibleTargetCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not determine target country name for absolutist focuses";
			targetCountryName = "";
		}

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Annex" + Home->getTag() + target->getTag();
		newFocus->icon = "GFX_goal_generic_major_war";
		newFocus->text += "Conquer " + targetCountryName;
		newFocus->available += "= {\n";
		newFocus->available += "			" + target->getTag() + " = { is_in_faction = no }\n";
		newFocus->available += "		}";
		newFocus->prerequisites.push_back("= { focus = PrepTheBorder" + Home->getTag() + " }");
		newFocus->xPos = nextFreeColumn + 12;
		newFocus->yPos = 3;
		newFocus->cost = 10;
		newFocus->bypass += "= {\n";
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + "= {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() + "\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}\n";
		newFocus->bypass += "		}";
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
	}
	if (annexationTargets.size() >= 2)
	{
		auto target = annexationTargets.back();

		auto possibleTargetCountryName = target->getSourceCountry()->getName("english");
		string targetCountryName;
		if (possibleTargetCountryName)
		{
			targetCountryName = *possibleTargetCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not determine target country name for absolutist focuses";
			targetCountryName = "";
		}

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Annex" + Home->getTag() + target->getTag();
		newFocus->icon = "GFX_goal_generic_major_war";
		newFocus->text += "Conquer " + targetCountryName;
		newFocus->available += "= {\n";
		newFocus->available += "			\"" + target->getTag() + "\" = { is_in_faction = no }\n";
		newFocus->available += "		}";
		newFocus->prerequisites.push_back("= { focus = NatSpirit" + Home->getTag() + " }");
		newFocus->xPos = nextFreeColumn + 10;
		newFocus->yPos = 4;
		newFocus->cost = 10;
		newFocus->bypass += "= {\n";
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() + "\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}\n";
		newFocus->bypass += "		}";
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
	}
}

void HoI4FocusTree::addCommunistCoupBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& coupTargets, const std::set<std::string>& majorIdeologies)
{
	if (coupTargets.size() > 0)
	{
		//Focus to increase Comm support and prereq for coupTargets
		shared_ptr<HoI4Focus> newFocus = loadedFocuses.find("Home_of_Revolution")->second.makeCustomizedCopy(Home->getTag());
		focuses.push_back(newFocus);

		for (unsigned int i = 0; i < 2; i++)
		{
			if (i < coupTargets.size())
			{
				auto possibleCoupCountryName = coupTargets[i]->getSourceCountry()->getName("english");
				string coupCountryName;
				if (possibleCoupCountryName)
				{
					coupCountryName = *possibleCoupCountryName;
				}
				else
				{
					LOG(LogLevel::Warning) << "Could not determine coup country name for communist coup focuses";
					coupCountryName = "";
				}

				newFocus = loadedFocuses.find("Influence_")->second.makeCustomizedCopy(Home->getTag());
				newFocus->id = "Influence_" + coupTargets[i]->getTag() + "_" + Home->getTag();
				newFocus->text = "Influence " + coupCountryName;
				newFocus->completionReward += "= {\n";
				newFocus->completionReward += "			" + coupTargets[i]->getTag() + " = {\n";
				if (majorIdeologies.count("fascism") > 0)
				{
					newFocus->completionReward += "				if = {\n";
					newFocus->completionReward += "					limit = {\n";
					newFocus->completionReward += "						" + Home->getTag() + " = {\n";
					newFocus->completionReward += "							has_government = fascism\n";
					newFocus->completionReward += "						}\n";
					newFocus->completionReward += "					}\n";
					newFocus->completionReward += "					add_ideas = fascist_influence\n";
					newFocus->completionReward += "				}\n";
				}
				if (majorIdeologies.count("communism") > 0)
				{
					newFocus->completionReward += "				if = {\n";
					newFocus->completionReward += "					limit = {\n";
					newFocus->completionReward += "						" + Home->getTag() + " = {\n";
					newFocus->completionReward += "							has_government = communism\n";
					newFocus->completionReward += "						}\n";
					newFocus->completionReward += "					}\n";
					newFocus->completionReward += "					add_ideas = communist_influence\n";
					newFocus->completionReward += "				}\n";
				}
				if (majorIdeologies.count("democratic") > 0)
				{
					newFocus->completionReward += "				if = {\n";
					newFocus->completionReward += "					limit = {\n";
					newFocus->completionReward += "						" + Home->getTag() + " = {\n";
					newFocus->completionReward += "							has_government = democratic\n";
					newFocus->completionReward += "						}\n";
					newFocus->completionReward += "					}\n";
					newFocus->completionReward += "					add_ideas = democratic_influence\n";
					newFocus->completionReward += "				}\n";
				}
				newFocus->completionReward += "				country_event = { id = generic.1 }\n";
				newFocus->completionReward += "			}\n";
				newFocus->completionReward += "		}";
				focuses.push_back(newFocus);

				//Civil War
				newFocus = loadedFocuses.find("Coup_")->second.makeCustomizedCopy(Home->getTag());
				newFocus->id = "Coup_" + coupTargets[i]->getTag() + "_" + Home->getTag();
				newFocus->text = "Civil War in " + coupCountryName;
				newFocus->prerequisites.push_back("= { focus = Influence_" + coupTargets[i]->getTag() + "_" + Home->getTag() + " }");
				newFocus->relativePositionId = "Influence_" + coupTargets[i]->getTag() + "_" + Home->getTag();
				newFocus->available = "= {\n";
				newFocus->available += "			" + coupTargets[i]->getTag() + " = { communism > 0.5 }\n";
				newFocus->available += "		}";
				newFocus->completionReward += "= {\n";
				newFocus->completionReward += "			" + coupTargets[i]->getTag() + " = {\n";
				newFocus->completionReward += "				start_civil_war = {\n";
				newFocus->completionReward += "					ideology = communism\n";
				newFocus->completionReward += "					size = 0.5\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}\n";
				newFocus->completionReward += "		}";
				focuses.push_back(newFocus);
			}
		}
		nextFreeColumn += 2 * coupTargets.size();
	}
	return;
}

void HoI4FocusTree::addCommunistWarBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& warTargets, HoI4::Events* events)
{
	if (warTargets.size() > 0)
	{

		//Strengthen Commintern
		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		newFocus->id = "StrengthCom" + Home->getTag();
		newFocus->icon = "GFX_goal_support_communism";
		newFocus->text = "Strengthen The Comintern";//change to faction name later
		newFocus->xPos = nextFreeColumn + warTargets.size() - 1;
		newFocus->yPos = 0;
		newFocus->cost = 10;
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			army_experience = 20\n";
		newFocus->completionReward += "			add_tech_bonus = {\n";
		newFocus->completionReward += "				bonus = 0.5\n";
		newFocus->completionReward += "				uses = 2\n";
		newFocus->completionReward += "				category = land_doctrine\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Inter_Com_Pres" + Home->getTag();
		newFocus->icon = "GFX_goal_generic_dangerous_deal";
		newFocus->text = "International Communist Pressure";//change to faction name later
		newFocus->prerequisites.push_back("= { focus = StrengthCom" + Home->getTag() + " }");
		newFocus->available = "= {\n";
		newFocus->available += "			date > 1937.1.1\n";
		newFocus->available += "		}";
		newFocus->xPos = nextFreeColumn + warTargets.size() - 1;
		newFocus->yPos = 1;
		newFocus->cost = 10;
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
		newFocus->completionReward += "		}";
		//FIXME
		//maybe add some claims?
		focuses.push_back(newFocus);

		for (unsigned int i = 0; i < 3; i++)
		{
			if (i < warTargets.size())
			{
				auto possibleWarTargetCountryName = warTargets[i]->getSourceCountry()->getName("english");
				string warTargetCountryName;
				if (possibleWarTargetCountryName)
				{
					warTargetCountryName = *possibleWarTargetCountryName;
				}
				else
				{
					LOG(LogLevel::Warning) << "Could not determine war target country name for communist war focuses";
					warTargetCountryName = "";
				}

				int v1 = rand() % 12 + 1;
				int v2 = rand() % 12 + 1;
				newFocus = make_shared<HoI4Focus>();
				newFocus->id = "War" + warTargets[i]->getTag() + Home->getTag();
				newFocus->icon = "GFX_goal_generic_major_war";
				newFocus->text = "War with " + warTargetCountryName;//change to faction name later
				newFocus->prerequisites.push_back("= { focus = Inter_Com_Pres" + Home->getTag() + " }");
				newFocus->available = "= {\n";
				newFocus->available += "			date > 1938." + to_string(v1) + "." + to_string(v2) + "\n";
				newFocus->available += "		}";
				newFocus->xPos = nextFreeColumn + i * 2;
				newFocus->yPos = 2;
				newFocus->cost = 10;
				newFocus->bypass = "= {\n";
				newFocus->bypass += "					has_war_with = " + warTargets[i]->getTag() + "\n";
				newFocus->bypass += "				}";
				newFocus->aiWillDo = "= {\n";
				newFocus->aiWillDo += "			factor = 5\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				strength_ratio = { tag = " + warTargets[i]->getTag() + " ratio < 1 }\n";
				newFocus->aiWillDo += "			}";
				if (warTargets.size() > 1)
				{
					newFocus->aiWillDo += "\n";
					newFocus->aiWillDo += "			modifier = {\n";
					newFocus->aiWillDo += "				factor = 0\n";
					newFocus->aiWillDo += "				OR = {\n";
					for (unsigned int i2 = 0; i2 < warTargets.size(); i2++)
					{
						if (i != i2)
							newFocus->aiWillDo += "					has_war_with = " + warTargets[i]->getTag() + "\n";
					}
					newFocus->aiWillDo += "				}\n";
					newFocus->aiWillDo += "			}";
				}
				newFocus->aiWillDo += "\n";
				newFocus->aiWillDo += "		}";

				newFocus->completionReward += "= {\n";
				newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = puppet_wargoal_focus\n";
				newFocus->completionReward += "				target = " + warTargets[i]->getTag() + "\n";
				newFocus->completionReward += "			}\n";
				newFocus->completionReward += "		}";
				focuses.push_back(newFocus);
			}
		}
		nextFreeColumn += warTargets.size() * 2;
	}
}

void HoI4FocusTree::addFascistAnnexationBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& annexationTargets, HoI4::Events* events)
{
	if (annexationTargets.size() >= 1)
	{
		//Focus to increase fascist support and prereq for anschluss
		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		newFocus->id = "The_third_way" + Home->getTag();
		newFocus->icon = "GFX_goal_support_fascism";
		newFocus->text = "The Third Way!";
		newFocus->xPos = nextFreeColumn + annexationTargets.size() - 1;
		newFocus->yPos = 0;
		newFocus->cost = 10;
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "		}";
		//FIXME
		//Need to get Drift Defense to work
		//in modified generic focus? (tk)
		//newFocus->completionReward += "			drift_defence_factor = 0.5\n";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
		newFocus->completionReward += "			add_ideas = fascist_influence\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);

		//Focus to increase army support
		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "mil_march" + Home->getTag();
		newFocus->icon = "GFX_goal_generic_allies_build_infantry";
		newFocus->text = "Establish Military March Day";
		newFocus->prerequisites.push_back("= { focus = The_third_way" + Home->getTag() + " }");
		newFocus->xPos = nextFreeColumn + annexationTargets.size() - 1;
		newFocus->yPos = 1;
		newFocus->cost = 10;
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			army_experience = 20\n";
		newFocus->completionReward += "			add_tech_bonus = {\n";
		newFocus->completionReward += "				bonus = 0.5\n";
		newFocus->completionReward += "				uses = 2\n";
		newFocus->completionReward += "				category = land_doctrine\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);

		for (unsigned int i = 0; i < annexationTargets.size(); i++)
		{
			if (i < annexationTargets.size())
			{
				auto possibleAnnexationTargetCountryName = annexationTargets[i]->getSourceCountry()->getName("english");
				string annexationTargetCountryName;
				if (possibleAnnexationTargetCountryName)
				{
					annexationTargetCountryName = *possibleAnnexationTargetCountryName;
				}
				else
				{
					LOG(LogLevel::Warning) << "Could not determine annexation target country name for fascist annexation focuses";
					annexationTargetCountryName = "";
				}

				//int x = i * 3;
				//for random date
				int v1 = rand() % 5 + 1;
				int v2 = rand() % 5 + 1;
				//focus for anschluss
				newFocus = make_shared<HoI4Focus>();
				newFocus->id = Home->getTag() + "_anschluss_" + annexationTargets[i]->getTag();
				newFocus->icon = "GFX_goal_anschluss";
				newFocus->text = "Union with " + annexationTargetCountryName;
				newFocus->available += "= {\n";
				newFocus->available += "			" + annexationTargets[i]->getTag() + " = {\n";
				newFocus->available += "				is_in_faction = no\n";
				newFocus->available += "			}\n";
				newFocus->available += "			is_puppet = no\n";
				newFocus->available += "			date > 1937." + to_string(v1 + 5) + "." + to_string(v2 + 5) + "\n";
				newFocus->available += "		}";
				newFocus->prerequisites.push_back("= { focus = mil_march" + Home->getTag() + " }");
				newFocus->xPos = nextFreeColumn + i * 2;
				newFocus->yPos = 2;
				newFocus->cost = 10;
				newFocus->aiWillDo += "= {\n";
				newFocus->aiWillDo += "			factor = 10\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				date < 1937.6.6\n";
				newFocus->aiWillDo += "			}\n";
				newFocus->aiWillDo += "		}";
				newFocus->completionReward += "= {\n";
				newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + annexationTargets[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + annexationTargets[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(events->getCurrentNationFocusEventNum()) + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}\n";
				newFocus->completionReward += "		}";
				focuses.push_back(newFocus);

				events->createAnnexEvent(Home, annexationTargets[i]);
			}
		}
		nextFreeColumn += annexationTargets.size() * 2;
	}
}

void HoI4FocusTree::addFascistSudetenBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& sudetenTargets, const vector<vector<int>>& demandedStates, const HoI4World* world)
{
	HoI4::Events* events = world->getEvents();

	if (sudetenTargets.size() >= 1)
	{
		//if it can easily take these targets as they are not in an alliance, you can get annexation event

		//Focus to increase empire size more
		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		newFocus->id = "expand_the_reich" + Home->getTag();
		newFocus->icon = "GFX_goal_generic_political_pressure";  //something about claiming land
		newFocus->text = "Expand the Reich";
		if (sudetenTargets.size() == 1 || sudetenTargets.size() >= 2)
		{
			//if there are anschlusses, make this event require at least 1 anschluss, else, its the start of a tree
			for (unsigned int i = 0; i < 2; i++)
			{
				if (i < sudetenTargets.size())
				{
//					newFocus->prerequisites.push_back("= { focus = " + Home->getTag() + "_anschluss_" + sudetenTargets[i]->getTag() + " }");
				}
			}
			newFocus->xPos = nextFreeColumn + sudetenTargets.size() - 1;
			newFocus->yPos = 0;
		}
		newFocus->cost = 10;
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "	factor = 5\n";
		newFocus->aiWillDo += "}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";//give some claims or cores
		newFocus->completionReward += "		}";
		addFocus(newFocus);

		for (unsigned int i = 0; i < sudetenTargets.size(); i++)
		{
			if (i < sudetenTargets.size())
			{
				auto possibleSudetenTargetCountryName = sudetenTargets[i]->getSourceCountry()->getName("english");
				string sudetenTargetCountryName;
				if (possibleSudetenTargetCountryName)
				{
					sudetenTargetCountryName = *possibleSudetenTargetCountryName;
				}
				else
				{
					LOG(LogLevel::Warning) << "Could not determine annexation target country name for fascist sudeten focuses";
					sudetenTargetCountryName = "";
				}

				int v1 = rand() % 8 + 1;
				int v2 = rand() % 8 + 1;
				//focus for sudaten
				newFocus = make_shared<HoI4Focus>();
				newFocus->id = Home->getTag() + "_sudeten_" + sudetenTargets[i]->getTag();
				newFocus->icon = "GFX_goal_anschluss";
				newFocus->text = "Demand Territory from " + sudetenTargetCountryName;
				newFocus->prerequisites.push_back("= { focus = expand_the_reich" + Home->getTag() + " }");
				newFocus->available = "= {\n";
				newFocus->available += "			is_puppet = no\n";
				newFocus->available += "			date > 1938." + to_string(v1) + "." + to_string(v2) + "\n";
				newFocus->available += "		}";
				newFocus->xPos = nextFreeColumn + 2 * i;
				newFocus->yPos = 1;
				newFocus->cost = 10;
				newFocus->bypass += "= {\n";
				newFocus->bypass += "  has_war_with = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->bypass += "}";
				newFocus->aiWillDo += "= {\n";
				newFocus->aiWillDo += "	factor = 10\n";
				newFocus->aiWillDo += "	modifier = {\n";
				newFocus->aiWillDo += "		factor = 0\n";
				newFocus->aiWillDo += "		date < 1937.6.6\n";
				newFocus->aiWillDo += "	}\n";
				newFocus->aiWillDo += "}";
				newFocus->completionReward += "= {\n";
				newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + sudetenTargets[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(events->getCurrentNationFocusEventNum()) + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}\n";
				newFocus->completionReward += "		}";
				addFocus(newFocus);

				//FINISH HIM
				newFocus = make_shared<HoI4Focus>();
				newFocus->id = Home->getTag() + "_finish_" + sudetenTargets[i]->getTag();
				newFocus->icon = "GFX_goal_generic_territory_or_war";
				newFocus->text = "Fate of " + sudetenTargetCountryName;
				newFocus->available = sudetenTargets[i]->getTag() + " = { is_in_faction = no }";
				newFocus->prerequisites.push_back("= { focus =  " + Home->getTag() + "_sudeten_" + sudetenTargets[i]->getTag() + " }");
				newFocus->available = "			is_puppet = no";
				newFocus->xPos = nextFreeColumn + 2 * i;
				newFocus->yPos = 2;
				newFocus->cost = 10;
				newFocus->aiWillDo += "= {\n";
				newFocus->aiWillDo += "	factor = 10\n";
				newFocus->aiWillDo += "	modifier = {\n";
				newFocus->aiWillDo += "		factor = 0\n";
				newFocus->aiWillDo += "		date < 1937.6.6\n";
				newFocus->aiWillDo += "	}\n";
				newFocus->aiWillDo += "}";
				newFocus->bypass += "= {\n";
				newFocus->bypass += "		has_war_with = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->bypass += "	}";
				newFocus->completionReward += "= {\n";
				newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = annex_everything\n";
				newFocus->completionReward += "				target = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->completionReward += "			}\n";
				newFocus->completionReward += "		}";
				addFocus(newFocus);

				//events
				events->createSudetenEvent(Home, sudetenTargets[i], demandedStates[i]);
			}
		}
		nextFreeColumn += 2 * sudetenTargets.size();
	}
}


void HoI4FocusTree::addGPWarBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& newAllies, const vector<shared_ptr<HoI4Country>>& GCTargets, const string& ideology, HoI4::Events* events)
{
	string ideologyShort = ideology.substr(0, 3);
	if (newAllies.size() > 0)
	{
		//Focus to call summit, maybe have events from summit
		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		newFocus->id = ideologyShort + "_Summit" + Home->getTag();
		newFocus->icon = "GFX_goal_generic_allies_build_infantry";
		newFocus->text = "Call for the " + ideology + " Summit";
		newFocus->xPos = nextFreeColumn + newAllies.size() - 1;
		newFocus->yPos = 0;
		newFocus->cost = 10;
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 2\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "			factor = 10\n";
		newFocus->aiWillDo += "			date > 1938.1.1\n";
		newFocus->aiWillDo += "			}\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
	}

	unsigned int i = 0;
	for (auto newAlly : newAllies)
	{
		auto possibleAllyCountryName = newAlly->getSourceCountry()->getName("english");
		string allyCountryName;
		if (possibleAllyCountryName)
		{
			allyCountryName = *possibleAllyCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not determine ally country name for GP alliance focuses";
			allyCountryName = "";
		}

		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Alliance_" + newAlly->getTag() + Home->getTag();
		newFocus->icon = "GFX_goal_generic_allies_build_infantry";
		newFocus->text = "Alliance with " + allyCountryName;
		newFocus->prerequisites.push_back("= { focus = " + ideologyShort + "_Summit" + Home->getTag() + " }");
		newFocus->xPos = nextFreeColumn + i * 2;
		newFocus->yPos = 1;
		newFocus->cost = 10;
		newFocus->aiWillDo += "= {\n";
		newFocus->aiWillDo += "			factor = 10\n";
		newFocus->aiWillDo += "		}";
		newFocus->bypass += "= {\n";
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = { is_in_faction_with = " + newAlly->getTag() + "}\n";
		newFocus->bypass += "				has_war_with = " + newAlly->getTag() + "\n";
		newFocus->bypass += "				NOT = { country_exists = " + newAlly->getTag() + " }\n";
		newFocus->bypass += "			}\n";
		newFocus->bypass += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			" + newAlly->getTag() + " = {\n";
		newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(events->getCurrentNationFocusEventNum()) + " }\n";
		newFocus->completionReward += "				add_opinion_modifier = { target = " + Home->getTag() + " modifier = positive_50 }\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);

		events->createFactionEvents(Home, newAlly);
		i++;
	}

	i = 0;
	for (auto GC : GCTargets)
	{
		auto possibleWarTargetCountryName = GC->getSourceCountry()->getName("english");
		string warTargetCountryName;
		if (possibleWarTargetCountryName)
		{
			warTargetCountryName = *possibleWarTargetCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not determine war target country name for GP war focuses";
			warTargetCountryName = "";
		}

		string prereq = "";
		int y2 = 1;
		//figuring out location of WG
		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		if (newAllies.size() > 0)
		{
			y2 = 2;
			for (unsigned int i2 = 0; i2 < newAllies.size(); i2++)
			{
				newFocus->prerequisites.push_back("= { focus = Alliance_" + newAllies[i2]->getTag() + Home->getTag() + " }");
			}
		}
		int v1 = rand() % 12 + 1;
		int v2 = rand() % 12 + 1;
		newFocus->id = "War" + GC->getTag() + Home->getTag();
		newFocus->icon = "GFX_goal_generic_major_war";
		newFocus->text = "War with " + warTargetCountryName;//change to faction name later
		newFocus->available = "= {\n";
		newFocus->available += "			has_war = no\n";
		newFocus->available += "			date > 1939." + to_string(v1) + "." + to_string(v2) + "\n";
		newFocus->available += "		}";
		newFocus->xPos = nextFreeColumn + i * 2;
		newFocus->yPos = y2;
		newFocus->cost = 10;
		newFocus->bypass = "= {\n";
		newFocus->bypass += "		   has_war_with = " + GC->getTag() + "\n";
		newFocus->bypass += "		}";
		newFocus->aiWillDo = "= {\n";
		newFocus->aiWillDo += "			factor = " + to_string(10 - GCTargets.size() * 5) + "\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "					factor = 0\n";
		newFocus->aiWillDo += "					strength_ratio = { tag = " + GC->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}";
		if (GCTargets.size() > 1)
		{
			newFocus->aiWillDo = "\n";

			//make ai have this as a 0 modifier if they are at war
			newFocus->aiWillDo += "			modifier = {\n";
			newFocus->aiWillDo += "				factor = 0\n";
			newFocus->aiWillDo += "				OR = {\n";
			for (unsigned int i2 = 0; i2 < GCTargets.size(); i2++)
			{
				if (GC != GCTargets[i2])
				{
					newFocus->aiWillDo += "					has_war_with = " + GCTargets[i2]->getTag() + "\n";
				}
			}
			newFocus->aiWillDo += "				}\n";
			newFocus->aiWillDo += "			}";
		}
		newFocus->aiWillDo += "\n";
		newFocus->aiWillDo += "		}";
		newFocus->completionReward += "= {\n";
		newFocus->completionReward += "			add_named_threat = { threat = 5 name = " + newFocus->id + " }\n";
		newFocus->completionReward += "			declare_war_on = {\n";
		newFocus->completionReward += "				type = puppet_wargoal_focus\n";
		newFocus->completionReward += "				target = " + GC->getTag() + "\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "		}";
		focuses.push_back(newFocus);
		i++;
	}
	nextFreeColumn += 2 * max(newAllies.size(), GCTargets.size());
}


void HoI4FocusTree::removeFocus(const string& id)
{
	for (auto itr = focuses.begin(); itr != focuses.end(); itr++)
	{
		if ((*itr)->id == id)
		{
			focuses.erase(itr);
			break;
		}
	}
}


void HoI4FocusTree::output(const string& filename) const
{
	ofstream out(filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not create " << filename;
		exit(-1);
	}

	out << "focus_tree = {\n";
	if (dstCountryTag != "")
	{
		out << "	id = " << dstCountryTag + "_focus\n";
		out << "	\n";
		out << "	country = {\n";
		out << "		factor = 0\n";
		out << "		\n";
		out << "		modifier = {\n";
		out << "			add = 10\n";
		out << "			tag = " << dstCountryTag << "\n";
		out << "		}\n";
		out << "	}\n";
		out << "	\n";
		out << "	default = no\n";
		out << "\n";
	}
	else
	{
		out << "	id = generic_focus\n";
		out << "	\n";
		out << "	country = {\n";
		out << "		factor = 1\n";
		out << "	}\n";
		out << "	\n";
		out << "	default = yes\n";
		out << "	reset_on_civilwar = no\n";
		out << "\n";
	}

	for (auto focus: focuses)
	{
		out << *focus;
		out <<"\n";
	}

	out << "}";

	out.close();
}