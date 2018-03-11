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
	registerKeyword(std::regex("government"), [this](const std::string& unused, std::istream& theStream)
		{
			auto governmentObj = commonItems::convert8859Object(unused, theStream);
			vector<shared_ptr<Object>> subGovernmentObj = governmentObj->getLeaves()[0]->getValue("government");
			(subGovernmentObj.size() > 0) ? government = subGovernmentObj[0]->getLeaf() : government = "";
		}
	);
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

	// ignored items
	registerKeyword(std::regex("human"), commonItems::ignoreString);
	registerKeyword(std::regex("was_player"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_age_objectives"), commonItems::ignoreString);
	registerKeyword(std::regex("technology_cost"), commonItems::ignoreString);
	registerKeyword(std::regex("is_primitive"), commonItems::ignoreString);
	registerKeyword(std::regex("fixed_capital"), commonItems::ignoreString);
	registerKeyword(std::regex("original_capital"), commonItems::ignoreString);
	registerKeyword(std::regex("trade_port"), commonItems::ignoreString);
	registerKeyword(std::regex("base_tax"), commonItems::ignoreString);
	registerKeyword(std::regex("development"), commonItems::ignoreString);
	registerKeyword(std::regex("raw_development"), commonItems::ignoreString);
	registerKeyword(std::regex("capped_development"), commonItems::ignoreString);
	registerKeyword(std::regex("treasure_fleet_gold"), commonItems::ignoreString);
	registerKeyword(std::regex("last_month_treasure_fleet_gold"), commonItems::ignoreString);
	registerKeyword(std::regex("potential_incidents"), commonItems::ignoreObject);
	registerKeyword(std::regex("incident_variables"), commonItems::ignoreObject);
	registerKeyword(std::regex("harmonized_religions"), commonItems::ignoreObject);
	registerKeyword(std::regex("initialized_rivals"), commonItems::ignoreString);
	registerKeyword(std::regex("recalculate_strategy"), commonItems::ignoreString);
	registerKeyword(std::regex("dirty_colony"), commonItems::ignoreString);
	registerKeyword(std::regex("national_focus"), commonItems::ignoreString);
	registerKeyword(std::regex("dominant_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("graphical_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("religious_school"), commonItems::ignoreString);
	registerKeyword(std::regex("secondary_religion"), commonItems::ignoreString);
	registerKeyword(std::regex("dominant_religion"), commonItems::ignoreString);
	registerKeyword(std::regex("fervor"), commonItems::ignoreObject);
	registerKeyword(std::regex("unit_type"), commonItems::ignoreString);
	registerKeyword(std::regex("estate"), commonItems::ignoreObject);
	registerKeyword(std::regex("rival"), commonItems::ignoreObject);
	registerKeyword(std::regex("faction"), commonItems::ignoreObject);
	registerKeyword(std::regex("top_faction"), commonItems::ignoreString);
	registerKeyword(std::regex("highest_possible_fort"), commonItems::ignoreString);
	registerKeyword(std::regex("highest_possible_fort_building"), commonItems::ignoreString);
	registerKeyword(std::regex("transfer_home_bonus"), commonItems::ignoreString);
	registerKeyword(std::regex("enemy"), commonItems::ignoreString);
	registerKeyword(std::regex("rebel_threat"), commonItems::ignoreString);
	registerKeyword(std::regex("goldtype"), commonItems::ignoreString);
	registerKeyword(std::regex("luck"), commonItems::ignoreString);
	registerKeyword(std::regex("is_at_war"), commonItems::ignoreString);
	registerKeyword(std::regex("new_monarch"), commonItems::ignoreString);
	registerKeyword(std::regex("last_election"), commonItems::ignoreString);
	registerKeyword(std::regex("current_power_projection"), commonItems::ignoreString);
	registerKeyword(std::regex("great_power_score"), commonItems::ignoreString);
	registerKeyword(std::regex("power_projection"), commonItems::ignoreObject);
	registerKeyword(std::regex("navy_strength"), commonItems::ignoreString);
	registerKeyword(std::regex("tariff"), commonItems::ignoreString);
	registerKeyword(std::regex("parliament"), commonItems::ignoreObject);
	registerKeyword(std::regex("total_war_worth"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_rebel_controlled_provinces"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_rebel_armies"), commonItems::ignoreString);
	registerKeyword(std::regex("num_owned_home_cores"), commonItems::ignoreString);
	registerKeyword(std::regex("government_name"), commonItems::ignoreString);
	registerKeyword(std::regex("subject_focus"), commonItems::ignoreString);
	registerKeyword(std::regex("trade_mission"), commonItems::ignoreString);
	registerKeyword(std::regex("blockade_mission"), commonItems::ignoreString);
	registerKeyword(std::regex("continent"), commonItems::ignoreObject);
	registerKeyword(std::regex("non_overseas_development"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_controlled_cities"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_ports"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_non_cores"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_core_ports"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_total_ports"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_cardinals"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_regulars"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_cities"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_provinces_in_states"), commonItems::ignoreString);
	registerKeyword(std::regex("forts"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_overseas"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_allies"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_royal_marriages"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_heathen_provs"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_heretic_provs"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_subjects"), commonItems::ignoreString);
	registerKeyword(std::regex("inland_sea_ratio"), commonItems::ignoreString);
	registerKeyword(std::regex("has_friendly_reformation_center"), commonItems::ignoreString);
	registerKeyword(std::regex("average_unrest"), commonItems::ignoreString);
	registerKeyword(std::regex("average_effective_unrest"), commonItems::ignoreString);
	registerKeyword(std::regex("average_autonomy"), commonItems::ignoreString);
	registerKeyword(std::regex("average_autonomy_above_min"), commonItems::ignoreString);
	registerKeyword(std::regex("average_home_autonomy"), commonItems::ignoreString);
	registerKeyword(std::regex("update_supply_range"), commonItems::ignoreString);
	registerKeyword(std::regex("friend_bools"), commonItems::ignoreObject);
	registerKeyword(std::regex("num_of_buildings_indexed"), commonItems::ignoreObject);
	registerKeyword(std::regex("produced_goods_value"), commonItems::ignoreObject);
	registerKeyword(std::regex("num_of_goods_produced"), commonItems::ignoreObject);
	registerKeyword(std::regex("traded"), commonItems::ignoreObject);
	registerKeyword(std::regex("num_of_religions_indexed"), commonItems::ignoreObject);
	registerKeyword(std::regex("num_of_leaders"), commonItems::ignoreObject);
	registerKeyword(std::regex("num_of_leaders_with_traits"), commonItems::ignoreObject);
	registerKeyword(std::regex("num_of_free_leaders"), commonItems::ignoreObject);
	registerKeyword(std::regex("num_of_provinces_in_territories"), commonItems::ignoreString);
	registerKeyword(std::regex("tribute_type"), commonItems::ignoreString);
	registerKeyword(std::regex("num_of_subject_count_indexed"), commonItems::ignoreObject);
	registerKeyword(std::regex("border_pct"), commonItems::ignoreObject);
	registerKeyword(std::regex("border_sit"), commonItems::ignoreObject);
	registerKeyword(std::regex("border_provinces"), commonItems::ignoreObject);
	registerKeyword(std::regex("range_cache"), commonItems::ignoreObject);
	registerKeyword(std::regex("neighbours"), commonItems::ignoreObject);
	registerKeyword(std::regex("home_neighbours"), commonItems::ignoreObject);
	registerKeyword(std::regex("core_neighbours"), commonItems::ignoreObject);
	registerKeyword(std::regex("transfer_trade_power_to"), commonItems::ignoreObject);
	registerKeyword(std::regex("current_at_war_with"), commonItems::ignoreObject);
	registerKeyword(std::regex("current_war_allies"), commonItems::ignoreObject);
	registerKeyword(std::regex("friends"), commonItems::ignoreObject);
	registerKeyword(std::regex("allies"), commonItems::ignoreObject);
	registerKeyword(std::regex("extended_allies"), commonItems::ignoreObject);
	registerKeyword(std::regex("subjects"), commonItems::ignoreObject);
	registerKeyword(std::regex("transfer_trade_power_from"), commonItems::ignoreObject);
	registerKeyword(std::regex("score_rating"), commonItems::ignoreObject);
	registerKeyword(std::regex("score_rank"), commonItems::ignoreObject);
	registerKeyword(std::regex("age_score"), commonItems::ignoreObject);
	registerKeyword(std::regex("score_place"), commonItems::ignoreString);
	registerKeyword(std::regex("prestige"), commonItems::ignoreString);
	registerKeyword(std::regex("treasury"), commonItems::ignoreString);
	registerKeyword(std::regex("estimated_monthly_income"), commonItems::ignoreString);
	registerKeyword(std::regex("inflation"), commonItems::ignoreString);
	registerKeyword(std::regex("inflation_history"), commonItems::ignoreObject);
	registerKeyword(std::regex("opinion_cache"), commonItems::ignoreObject);
	registerKeyword(std::regex("under_construction"), commonItems::ignoreObject);
	registerKeyword(std::regex("under_construction_queued"), commonItems::ignoreObject);
	registerKeyword(std::regex("total_count"), commonItems::ignoreObject);
	registerKeyword(std::regex("state"), commonItems::ignoreObject);
	registerKeyword(std::regex("owned_provinces"), commonItems::ignoreObject);
	registerKeyword(std::regex("controlled_provinces"), commonItems::ignoreObject);
	registerKeyword(std::regex("core_provinces"), commonItems::ignoreObject);
	registerKeyword(std::regex("claim_provinces"), commonItems::ignoreObject);
	registerKeyword(std::regex("idea_may_cache"), commonItems::ignoreObject);
	registerKeyword(std::regex("update_opinion_cache"), commonItems::ignoreString);
	registerKeyword(std::regex("needs_refresh"), commonItems::ignoreString);
	registerKeyword(std::regex("casus_bellis_refresh"), commonItems::ignoreString);
	registerKeyword(std::regex("needs_rebel_unit_refresh"), commonItems::ignoreString);
	registerKeyword(std::regex("rebels_in_country"), commonItems::ignoreObject);
	registerKeyword(std::regex("war_exhaustion"), commonItems::ignoreString);
	registerKeyword(std::regex("can_take_wartaxes"), commonItems::ignoreString);
	registerKeyword(std::regex("land_maintenance"), commonItems::ignoreString);
	registerKeyword(std::regex("naval_maintenance"), commonItems::ignoreString);
	registerKeyword(std::regex("colonial_maintenance"), commonItems::ignoreString);
	registerKeyword(std::regex("missionary_maintenance"), commonItems::ignoreString);
	registerKeyword(std::regex("army_tradition"), commonItems::ignoreString);
	registerKeyword(std::regex("navy_tradition"), commonItems::ignoreString);
	registerKeyword(std::regex("last_war_ended"), commonItems::ignoreString);
	registerKeyword(std::regex("num_uncontested_cores"), commonItems::ignoreString);
	registerKeyword(std::regex("ledger"), commonItems::ignoreObject);
	registerKeyword(std::regex("loan_size"), commonItems::ignoreString);
	registerKeyword(std::regex("estimated_loan"), commonItems::ignoreString);
	registerKeyword(std::regex("religious_unity"), commonItems::ignoreString);
	registerKeyword(std::regex("republican_tradition"), commonItems::ignoreString);
	registerKeyword(std::regex("devotion"), commonItems::ignoreString);
	registerKeyword(std::regex("meritocracy"), commonItems::ignoreString);
	registerKeyword(std::regex("papal_influence"), commonItems::ignoreString);
	registerKeyword(std::regex("buffer"), commonItems::ignoreString);
	registerKeyword(std::regex("piety"), commonItems::ignoreString);
	registerKeyword(std::regex("root_out_corruption_slider"), commonItems::ignoreString);
	registerKeyword(std::regex("unlock_cult"), commonItems::ignoreObject);
	registerKeyword(std::regex("horde_unity"), commonItems::ignoreString);
	registerKeyword(std::regex("mercantilism"), commonItems::ignoreString);
	registerKeyword(std::regex("splendor"), commonItems::ignoreString);
	registerKeyword(std::regex("absolutism"), commonItems::ignoreString);
	registerKeyword(std::regex("army_professionalism"), commonItems::ignoreString);
	registerKeyword(std::regex("max_historic_army_professionalism"), commonItems::ignoreString);
	registerKeyword(std::regex("church"), commonItems::ignoreObject);
	registerKeyword(std::regex("disaster_progress"), commonItems::ignoreObject);
	registerKeyword(std::regex("disaster_started"), commonItems::ignoreObject);
	registerKeyword(std::regex("colonists"), commonItems::ignoreObject);
	registerKeyword(std::regex("merchants"), commonItems::ignoreObject);
	registerKeyword(std::regex("missionaries"), commonItems::ignoreObject);
	registerKeyword(std::regex("diplomats"), commonItems::ignoreObject);
	registerKeyword(std::regex("manpower"), commonItems::ignoreString);
	registerKeyword(std::regex("active_religious_reform"), commonItems::ignoreObject);
	registerKeyword(std::regex("active_native_advancement"), commonItems::ignoreObject);
	registerKeyword(std::regex("max_manpower"), commonItems::ignoreString);
	registerKeyword(std::regex("sailors"), commonItems::ignoreString);
	registerKeyword(std::regex("max_sailors"), commonItems::ignoreString);
	registerKeyword(std::regex("sub_unit"), commonItems::ignoreObject);
	registerKeyword(std::regex("overextension_percentage"), commonItems::ignoreString);
	registerKeyword(std::regex("leader"), commonItems::ignoreObject);
	registerKeyword(std::regex("monarch"), commonItems::ignoreObject);
	registerKeyword(std::regex("queen"), commonItems::ignoreObject);
	registerKeyword(std::regex("heir"), commonItems::ignoreObject);
	registerKeyword(std::regex("decision_seed"), commonItems::ignoreString);
	registerKeyword(std::regex("original_dynasty"), commonItems::ignoreString);
	registerKeyword(std::regex("is_great_power"), commonItems::ignoreString);
	registerKeyword(std::regex("wants_to_be_great_power"), commonItems::ignoreString);
	registerKeyword(std::regex("wants_to_be_great_power_next"), commonItems::ignoreString);
	registerKeyword(std::regex("inauguration"), commonItems::ignoreString);
	registerKeyword(std::regex("previous_monarch"), commonItems::ignoreObject);
	registerKeyword(std::regex("last_major_mission_pick"), commonItems::ignoreString);
	registerKeyword(std::regex("last_major_mission_cancel"), commonItems::ignoreString);
	registerKeyword(std::regex("last_major_mission"), commonItems::ignoreString);
	registerKeyword(std::regex("ai"), commonItems::ignoreObject);
	registerKeyword(std::regex("assigned_estates"), commonItems::ignoreString);
	registerKeyword(std::regex("traded_bonus"), commonItems::ignoreObject);
	registerKeyword(std::regex("historical_friends"), commonItems::ignoreObject);
	registerKeyword(std::regex("historical_rivals"), commonItems::ignoreObject);
	registerKeyword(std::regex("powers"), commonItems::ignoreObject);
	registerKeyword(std::regex("delayed_event"), commonItems::ignoreObject);
	registerKeyword(std::regex("interesting_countries"), commonItems::ignoreObject);
	registerKeyword(std::regex("blockaded_percent"), commonItems::ignoreString);
	registerKeyword(std::regex("colonial_name_source"), commonItems::ignoreString);
	registerKeyword(std::regex("losses"), commonItems::ignoreObject);

	parseStream(theStream);

	determineJapaneseRelations();
	determineInvestments();
	determineLibertyDesire();
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
	if (colony)
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