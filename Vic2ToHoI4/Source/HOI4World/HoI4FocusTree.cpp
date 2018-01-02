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



#include "HoI4FocusTree.h"
#include "HoI4Country.h"
#include "HoI4Focus.h"
#include "../Configuration.h"
#include "../V2World/V2Party.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include <fstream>
using namespace std;



HoI4FocusTree::HoI4FocusTree():
	srcCountryTag(""),
	dstCountryTag(""),
	focuses(),
	nextFreeColumn(0),
	fascistMutualExlusions(""),
	communistMutualExclusions(""),
	absolutistMutualExlusions(""),
	radicalMutualExclusions("")
{
}


HoI4FocusTree::HoI4FocusTree(shared_ptr<HoI4Country> country):
	srcCountryTag(country->getSourceCountry()->getTag()),
	dstCountryTag(country->getTag()),
	focuses(),
	nextFreeColumn(0),
	fascistMutualExlusions(""),
	communistMutualExclusions(""),
	absolutistMutualExlusions(""),
	radicalMutualExclusions("")
{
}


void HoI4FocusTree::addGenericFocusTree(const set<string>& majorIdeologies)
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
	newFocus->id = "army_effort";
	newFocus->icon = "GFX_goal_generic_allies_build_infantry";
	newFocus->xPos = 1;
	newFocus->yPos = 0;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward =  "			army_experience = 5\n";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = land_doc_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = land_doctrine\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "equipment_effort";
	newFocus->icon = "GFX_goal_generic_small_arms";
	newFocus->prerequisites.push_back("focus = army_effort");
	newFocus->xPos = 0;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward =  "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = infantry_weapons_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = infantry_weapons\n";
	newFocus->completionReward += "				category = artillery\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "motorization_effort";
	newFocus->icon = "GFX_goal_generic_army_motorized";
	newFocus->prerequisites.push_back("focus = army_effort");
	newFocus->bypass = "			has_tech = motorised_infantry";
	newFocus->xPos = 2;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward =  "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = motorized_bonus\n";
	newFocus->completionReward += "				bonus = 0.75\n";
	newFocus->completionReward += "				technology = motorised_infantry\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "doctrine_effort";
	newFocus->icon = "GFX_goal_generic_army_doctrines";
	newFocus->prerequisites.push_back("focus = army_effort");
	newFocus->xPos = 1;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward  = "			army_experience = 5\n";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = land_doc_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = land_doctrine\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "equipment_effort_2";
	newFocus->icon = "GFX_goal_generic_army_artillery";
	newFocus->prerequisites.push_back("focus = equipment_effort");
	newFocus->xPos = 0;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward  = "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = infantry_artillery_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = infantry_weapons\n";
	newFocus->completionReward += "				category = artillery\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "mechanization_effort";
	newFocus->icon = "GFX_goal_generic_build_tank";
	newFocus->prerequisites.push_back("focus = motorization_effort");
	newFocus->xPos = 2;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward  = "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = motorized_bonus\n";
	newFocus->completionReward += "				ahead_reduction = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = motorized_equipment\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "doctrine_effort_2";
	newFocus->icon = "GFX_goal_generic_army_doctrines";
	newFocus->prerequisites.push_back("focus = doctrine_effort");
	newFocus->xPos = 1;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward  = "			army_experience = 5\n";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = land_doc_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = land_doctrine\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "equipment_effort_3";
	newFocus->icon = "GFX_goal_generic_army_artillery2";
	newFocus->prerequisites.push_back("focus = equipment_effort_2");
	newFocus->xPos = 0;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward  = "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = infantry_artillery_bonus\n";
	newFocus->completionReward += "				ahead_reduction = 1\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = infantry_weapons\n";
	newFocus->completionReward += "				category = artillery\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "armor_effort";
	newFocus->icon = "GFX_goal_generic_army_tanks";
	newFocus->prerequisites.push_back("focus = mechanization_effort");
	newFocus->xPos = 2;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward  = "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = armor_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 2\n";
	newFocus->completionReward += "				category = armor\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "special_forces";
	newFocus->icon = "GFX_goal_generic_special_forces";
	newFocus->prerequisites.push_back("focus = equipment_effort_3");
	newFocus->prerequisites.push_back("focus = doctrine_effort_2");
	newFocus->prerequisites.push_back("focus = armor_effort");
	newFocus->xPos = 1;
	newFocus->yPos = 6;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward  = "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = special_forces_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				technology = paratroopers\n";
	newFocus->completionReward += "				technology = paratroopers2\n";
	newFocus->completionReward += "				technology = marines\n";
	newFocus->completionReward += "				technology = marines2\n";
	newFocus->completionReward += "				technology = tech_mountaineers\n";
	newFocus->completionReward += "				technology = tech_mountaineers2\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "aviation_effort";
	newFocus->icon = "GFX_goal_generic_build_airforce";
	newFocus->xPos = 5;
	newFocus->yPos = 0;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completeTooltip  = "			air_experience = 25\n";
	newFocus->completeTooltip += "			if = { limit = { has_country_flag = aviation_effort_AB }\n";
	newFocus->completeTooltip += "				add_building_construction = {\n";
	newFocus->completeTooltip += "					type = air_base\n";
	newFocus->completeTooltip += "					level = 2\n";
	newFocus->completeTooltip += "					instant_build = yes\n";
	newFocus->completeTooltip += "				}\n";
	newFocus->completeTooltip += "			}			\n";
	newFocus->completeTooltip += "			add_tech_bonus = {\n";
	newFocus->completeTooltip += "				name = air_doc_bonus\n";
	newFocus->completeTooltip += "				bonus = 0.5\n";
	newFocus->completeTooltip += "				uses = 1\n";
	newFocus->completeTooltip += "				category = air_doctrine\n";
	newFocus->completeTooltip += "			}			";
	newFocus->completionReward += "			air_experience = 25\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					capital_scope = {\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							free_building_slots = {\n";
	newFocus->completionReward += "								building = air_base\n";
	newFocus->completionReward += "								size > 1\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				random_owned_controlled_state = {\n";
	newFocus->completionReward += "					limit = {\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = air_base\n";
	newFocus->completionReward += "							size > 1\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					add_building_construction = {\n";
	newFocus->completionReward += "						type = air_base\n";
	newFocus->completionReward += "						level = 2\n";
	newFocus->completionReward += "						instant_build = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					ROOT = { set_country_flag = aviation_effort_AB }\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					capital_scope = {\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = air_base\n";
	newFocus->completionReward += "							size > 1\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				capital_scope = {\n";
	newFocus->completionReward += "					add_building_construction = {\n";
	newFocus->completionReward += "						type = air_base\n";
	newFocus->completionReward += "						level = 2\n";
	newFocus->completionReward += "						instant_build = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					ROOT = { set_country_flag = aviation_effort_AB }\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = air_doc_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = air_doctrine\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "fighter_focus";
	newFocus->icon = "GFX_goal_generic_air_fighter";
	newFocus->prerequisites.push_back("focus = aviation_effort");
	newFocus->mutuallyExclusive = "focus = bomber_focus";
	newFocus->xPos = 4;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = fighter_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 2\n";
	newFocus->completionReward += "				technology = early_fighter\n";
	newFocus->completionReward += "				technology = fighter1\n";
	newFocus->completionReward += "				technology = fighter2\n";
	newFocus->completionReward += "				technology = fighter3\n";
	newFocus->completionReward += "				technology = heavy_fighter1\n";
	newFocus->completionReward += "				technology = heavy_fighter2\n";
	newFocus->completionReward += "				technology = heavy_fighter3\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "bomber_focus";
	newFocus->icon = "GFX_goal_generic_air_bomber";
	newFocus->prerequisites.push_back("focus = aviation_effort");
	newFocus->mutuallyExclusive = "focus = fighter_focus";
	newFocus->xPos = 6;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = bomber_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 2\n";
	newFocus->completionReward += "				technology = strategic_bomber1\n";
	newFocus->completionReward += "				technology = strategic_bomber2\n";
	newFocus->completionReward += "				technology = strategic_bomber3\n";
	newFocus->completionReward += "				category = tactical_bomber\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "aviation_effort_2";
	newFocus->icon = "GFX_goal_generic_air_doctrine";
	newFocus->prerequisites.push_back("focus = bomber_focus focus = fighter_focus");
	newFocus->xPos = 5;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completeTooltip += "			air_experience = 25\n";
	newFocus->completeTooltip += "			if = { limit = { has_country_flag = aviation_effort_2_AB }\n";
	newFocus->completeTooltip += "				add_building_construction = {\n";
	newFocus->completeTooltip += "					type = air_base\n";
	newFocus->completeTooltip += "					level = 2\n";
	newFocus->completeTooltip += "					instant_build = yes\n";
	newFocus->completeTooltip += "				}\n";
	newFocus->completeTooltip += "			}\n";
	newFocus->completeTooltip += "			add_tech_bonus = {\n";
	newFocus->completeTooltip += "				name =  air_doc_bonus\n";
	newFocus->completeTooltip += "				bonus = 0.5\n";
	newFocus->completeTooltip += "				uses = 1\n";
	newFocus->completeTooltip += "				category = air_doctrine\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			air_experience = 25\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					capital_scope = {\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "							NOT = { is_controlled_by = ROOT }\n";
	newFocus->completionReward += "							NOT = {\n";
	newFocus->completionReward += "								free_building_slots = {\n";
	newFocus->completionReward += "									building = air_base\n";
	newFocus->completionReward += "									size > 1\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				random_owned_controlled_state = {\n";
	newFocus->completionReward += "					limit = {\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = air_base\n";
	newFocus->completionReward += "							size > 1\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					add_building_construction = {\n";
	newFocus->completionReward += "						type = air_base\n";
	newFocus->completionReward += "						level = 2\n";
	newFocus->completionReward += "						instant_build = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					ROOT = { set_country_flag = aviation_effort_2_AB }\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					capital_scope = {\n";
	newFocus->completionReward += "						is_controlled_by = ROOT\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = air_base\n";
	newFocus->completionReward += "							size > 1\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				capital_scope = {\n";
	newFocus->completionReward += "					add_building_construction = {\n";
	newFocus->completionReward += "						type = air_base\n";
	newFocus->completionReward += "						level = 2\n";
	newFocus->completionReward += "						instant_build = yes\n";
	newFocus->completionReward += "					}				\n";
	newFocus->completionReward += "					ROOT = { set_country_flag = aviation_effort_2_AB }\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name =  air_doc_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = air_doctrine\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "CAS_effort";
	newFocus->icon = "GFX_goal_generic_CAS";
	newFocus->prerequisites.push_back("focus = aviation_effort_2");
	newFocus->prerequisites.push_back("focus = motorization_effort");
	newFocus->xPos = 4;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = CAS_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				ahead_reduction = 1\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = cas_bomber\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "rocket_effort";
	newFocus->icon = "GFX_focus_rocketry";
	newFocus->prerequisites.push_back("focus = aviation_effort_2");
	newFocus->prerequisites.push_back("focus = infrastructure_effort");
	newFocus->xPos = 5;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = jet_rocket_bonus\n";
	newFocus->completionReward += "				ahead_reduction = 0.5\n";
	newFocus->completionReward += "				uses = 2\n";
	newFocus->completionReward += "				category = rocketry\n";
	newFocus->completionReward += "				category = jet_technology\n";
	newFocus->completionReward += "			}";
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0.25\n";
	newFocus->aiWillDo += "				always = yes\n";
	newFocus->aiWillDo += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "NAV_effort";
	newFocus->icon = "GFX_goal_generic_air_naval_bomber";
	newFocus->prerequisites.push_back("focus = aviation_effort_2");
	newFocus->prerequisites.push_back("focus = flexible_navy");
	newFocus->xPos = 6;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = nav_bomber_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				ahead_reduction = 1\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = naval_bomber\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "naval_effort";
	newFocus->icon = "GFX_goal_generic_construct_naval_dockyard";
	newFocus->xPos = 9;
	newFocus->yPos = 0;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->available += "			any_state = {\n";
	newFocus->available += "				is_coastal = yes\n";
	newFocus->available += "				is_controlled_by = ROOT\n";
	newFocus->available += "			}";
	newFocus->completeTooltip += "			navy_experience = 25\n";
	newFocus->completeTooltip += "			add_extra_state_shared_building_slots = 3\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = dockyard\n";
	newFocus->completeTooltip += "				level = 3\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			navy_experience = 25\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					NOT = {\n";
	newFocus->completionReward += "						any_owned_state = {\n";
	newFocus->completionReward += "							dockyard > 0\n";
	newFocus->completionReward += "							free_building_slots = {\n";
	newFocus->completionReward += "								building = dockyard\n";
	newFocus->completionReward += "								size > 2\n";
	newFocus->completionReward += "								include_locked = yes\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				random_owned_controlled_state = {\n";
	newFocus->completionReward += "					limit = {\n";
	newFocus->completionReward += "						is_coastal = yes\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = dockyard\n";
	newFocus->completionReward += "							size > 2\n";
	newFocus->completionReward += "							include_locked = yes\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					add_extra_state_shared_building_slots = 3\n";
	newFocus->completionReward += "					add_building_construction = {\n";
	newFocus->completionReward += "						type = dockyard\n";
	newFocus->completionReward += "						level = 3\n";
	newFocus->completionReward += "						instant_build = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				set_country_flag = naval_effort_built\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					NOT = { has_country_flag = naval_effort_built }\n";
	newFocus->completionReward += "					any_owned_state = {\n";
	newFocus->completionReward += "						dockyard > 0\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = dockyard\n";
	newFocus->completionReward += "							size > 2\n";
	newFocus->completionReward += "							include_locked = yes\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				random_owned_controlled_state = {\n";
	newFocus->completionReward += "					limit = {\n";
	newFocus->completionReward += "						dockyard > 0\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = dockyard\n";
	newFocus->completionReward += "							size > 2\n";
	newFocus->completionReward += "							include_locked = yes\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					add_extra_state_shared_building_slots = 3\n";
	newFocus->completionReward += "					add_building_construction = {\n";
	newFocus->completionReward += "						type = dockyard\n";
	newFocus->completionReward += "						level = 3\n";
	newFocus->completionReward += "						instant_build = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				set_country_flag = naval_effort_built\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					NOT = { has_country_flag = naval_effort_built }\n";
	newFocus->completionReward += "					NOT = {\n";
	newFocus->completionReward += "						any_owned_state = {\n";
	newFocus->completionReward += "							free_building_slots = {\n";
	newFocus->completionReward += "								building = dockyard\n";
	newFocus->completionReward += "								size > 2\n";
	newFocus->completionReward += "								include_locked = yes\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				random_state = {\n";
	newFocus->completionReward += "					limit = {\n";
	newFocus->completionReward += "						controller = { tag = ROOT }\n";
	newFocus->completionReward += "						free_building_slots = {\n";
	newFocus->completionReward += "							building = dockyard\n";
	newFocus->completionReward += "							size > 2\n";
	newFocus->completionReward += "							include_locked = yes\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					add_extra_state_shared_building_slots = 3\n";
	newFocus->completionReward += "					add_building_construction = {\n";
	newFocus->completionReward += "						type = dockyard\n";
	newFocus->completionReward += "						level = 3\n";
	newFocus->completionReward += "						instant_build = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "flexible_navy";
	newFocus->icon = "GFX_goal_generic_build_navy";
	newFocus->prerequisites.push_back("focus = naval_effort");
	newFocus->mutuallyExclusive = "focus = large_navy";
	newFocus->xPos = 8;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				all_owned_state = {\n";
	newFocus->aiWillDo += "					OR = {\n";
	newFocus->aiWillDo += "						is_coastal = no\n";
	newFocus->aiWillDo += "						dockyard < 1\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = sub_op_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 2\n";
	newFocus->completionReward += "				technology = convoy_interdiction_ti\n";
	newFocus->completionReward += "				technology = unrestricted_submarine_warfare\n";
	newFocus->completionReward += "				technology = wolfpacks\n";
	newFocus->completionReward += "				technology = advanced_submarine_warfare\n";
	newFocus->completionReward += "				technology = combined_operations_raiding\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "large_navy";
	newFocus->icon = "GFX_goal_generic_navy_doctrines_tactics";
	newFocus->prerequisites.push_back("focus = naval_effort");
	newFocus->mutuallyExclusive = "focus = flexible_navy";
	newFocus->xPos = 10;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				all_owned_state = {\n";
	newFocus->aiWillDo += "					OR = {\n";
	newFocus->aiWillDo += "						is_coastal = no\n";
	newFocus->aiWillDo += "						dockyard < 1\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = fleet_in_being_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 2\n";
	newFocus->completionReward += "				category = fleet_in_being_tree\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "submarine_effort";
	newFocus->icon = "GFX_goal_generic_navy_submarine";
	newFocus->prerequisites.push_back("focus = flexible_navy focus = large_navy");
	newFocus->xPos = 8;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				all_owned_state = {\n";
	newFocus->aiWillDo += "					OR = {\n";
	newFocus->aiWillDo += "						is_coastal = no\n";
	newFocus->aiWillDo += "						dockyard < 1\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = ss_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				ahead_reduction = 1\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				technology = early_submarine\n";
	newFocus->completionReward += "				technology = basic_submarine\n";
	newFocus->completionReward += "				technology = improved_submarine\n";
	newFocus->completionReward += "				technology = advanced_submarine\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "cruiser_effort";
	newFocus->icon = "GFX_goal_generic_navy_cruiser";
	newFocus->prerequisites.push_back("focus = large_navy focus = flexible_navy");
	newFocus->xPos = 10;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				all_owned_state = {\n";
	newFocus->aiWillDo += "					OR = {\n";
	newFocus->aiWillDo += "						is_coastal = no\n";
	newFocus->aiWillDo += "						dockyard < 1\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = cr_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				ahead_reduction = 1\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				technology = improved_light_cruiser\n";
	newFocus->completionReward += "				technology = advanced_light_cruiser\n";
	newFocus->completionReward += "				technology = improved_heavy_cruiser\n";
	newFocus->completionReward += "				technology = advanced_heavy_cruiser\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "destroyer_effort";
	newFocus->icon = "GFX_goal_generic_wolf_pack";
	newFocus->prerequisites.push_back("focus = submarine_effort");
	newFocus->xPos = 8;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				all_owned_state = {\n";
	newFocus->aiWillDo += "					OR = {\n";
	newFocus->aiWillDo += "						is_coastal = no\n";
	newFocus->aiWillDo += "						dockyard < 1\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = dd_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				ahead_reduction = 1\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				technology = early_destroyer\n";
	newFocus->completionReward += "				technology = basic_destroyer\n";
	newFocus->completionReward += "				technology = improved_destroyer\n";
	newFocus->completionReward += "				technology = advanced_destroyer\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "capital_ships_effort";
	newFocus->icon = "GFX_goal_generic_navy_battleship";
	newFocus->prerequisites.push_back("focus = cruiser_effort");
	newFocus->xPos = 10;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				all_owned_state = {\n";
	newFocus->aiWillDo += "					OR = {\n";
	newFocus->aiWillDo += "						is_coastal = no\n";
	newFocus->aiWillDo += "						dockyard < 1\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			navy_experience = 25\n";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = capital_ships_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				ahead_reduction = 1\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				technology = basic_battlecruiser\n";
	newFocus->completionReward += "				technology = basic_battleship\n";
	newFocus->completionReward += "				technology = improved_battleship\n";
	newFocus->completionReward += "				technology = advanced_battleship\n";
	newFocus->completionReward += "				technology = heavy_battleship\n";
	newFocus->completionReward += "				technology = heavy_battleship2\n";
	newFocus->completionReward += "				technology = early_carrier\n";
	newFocus->completionReward += "				technology = basic_carrier\n";
	newFocus->completionReward += "				technology = improved_carrier\n";
	newFocus->completionReward += "				technology = advanced_carrier\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "industrial_effort";
	newFocus->icon = "GFX_goal_generic_production";
	newFocus->xPos = 13;
	newFocus->yPos = 0;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = industrial_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = industry\n";
	newFocus->completionReward += "			}";
	newFocus->aiWillDo += "			factor = 3\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				date < 1939.1.1\n";
	newFocus->aiWillDo += "				OR = {\n";
	newFocus->aiWillDo += "					# we also dont want tiny nations to go crazy with slots right away\n";
	newFocus->aiWillDo += "					num_of_controlled_states < 2\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "construction_effort";
	newFocus->icon = "GFX_goal_generic_construct_civ_factory";
	newFocus->prerequisites.push_back("focus = industrial_effort");
	newFocus->xPos = 12;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 2";
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = construction_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = industrial_complex\n";
	newFocus->bypass += "						size < 1\n";
	newFocus->bypass += "						include_locked = yes\n";
	newFocus->bypass += "					}					\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_extra_state_shared_building_slots = 1\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = industrial_complex\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = industrial_complex\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = industrial_complex\n";
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
	newFocus->completionReward += "					type = industrial_complex\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "production_effort";
	newFocus->icon = "GFX_goal_generic_construct_mil_factory";
	newFocus->prerequisites.push_back("focus = industrial_effort");
	newFocus->xPos = 14;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 2";
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = production_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = arms_factory\n";
	newFocus->bypass += "						size < 1\n";
	newFocus->bypass += "						include_locked = yes\n";
	newFocus->bypass += "					}\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_extra_state_shared_building_slots = 1\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = arms_factory\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
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
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "construction_effort_2";
	newFocus->icon = "GFX_goal_generic_construct_civ_factory";
	newFocus->prerequisites.push_back("focus = construction_effort");
	newFocus->xPos = 12;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 2";
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = construction_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = industrial_complex\n";
	newFocus->bypass += "						size < 1\n";
	newFocus->bypass += "						include_locked = yes\n";
	newFocus->bypass += "					}\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_extra_state_shared_building_slots = 1\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = industrial_complex\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = industrial_complex\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = industrial_complex\n";
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
	newFocus->completionReward += "					type = industrial_complex\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "production_effort_2";
	newFocus->icon = "GFX_goal_generic_construct_mil_factory";
	newFocus->prerequisites.push_back("focus = production_effort");
	newFocus->xPos = 14;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 2";
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = production_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = arms_factory\n";
	newFocus->bypass += "						size < 1\n";
	newFocus->bypass += "						include_locked = yes\n";
	newFocus->bypass += "					}\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_extra_state_shared_building_slots = 1\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = arms_factory\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
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
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "infrastructure_effort";
	newFocus->icon = "GFX_goal_generic_construct_infrastructure";
	newFocus->prerequisites.push_back("focus = construction_effort_2");
	newFocus->xPos = 12;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = infrastructure_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {			\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = infrastructure\n";
	newFocus->bypass += "						size < 1\n";
	newFocus->bypass += "					}\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = infrastructure\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = infrastructure\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 1\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 1\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 2\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "production_effort_3";
	newFocus->icon = "GFX_goal_generic_construct_mil_factory";
	newFocus->prerequisites.push_back("focus = production_effort_2");
	newFocus->xPos = 14;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 2";
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = production_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = arms_factory\n";
	newFocus->bypass += "						size < 1\n";
	newFocus->bypass += "						include_locked = yes\n";
	newFocus->bypass += "					}\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_extra_state_shared_building_slots = 1\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = arms_factory\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
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
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "infrastructure_effort_2";
	newFocus->icon = "GFX_goal_generic_construct_infrastructure";
	newFocus->prerequisites.push_back("focus = infrastructure_effort");
	newFocus->xPos = 12;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = infrastructure_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {			\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = infrastructure\n";
	newFocus->bypass += "						size < 1\n";
	newFocus->bypass += "					}\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = infrastructure\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = infrastructure\n";
	newFocus->completeTooltip += "				level = 1\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 1\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = infrastructure\n";
	newFocus->completionReward += "										size > 1\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 2\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "construction_effort_3";
	newFocus->icon = "GFX_goal_generic_construct_civ_factory";
	newFocus->prerequisites.push_back("focus = infrastructure_effort");
	newFocus->xPos = 14;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 2";
	newFocus->bypass += "			custom_trigger_tooltip = {\n";
	newFocus->bypass += "				tooltip = construction_effort_tt\n";
	newFocus->bypass += "				all_owned_state = {\n";
	newFocus->bypass += "					free_building_slots = {\n";
	newFocus->bypass += "						building = industrial_complex\n";
	newFocus->bypass += "						size < 2\n";
	newFocus->bypass += "						include_locked = yes\n";
	newFocus->bypass += "					}\n";
	newFocus->bypass += "				}\n";
	newFocus->bypass += "			}";
	newFocus->completeTooltip += "			add_extra_state_shared_building_slots = 2\n";
	newFocus->completeTooltip += "			add_building_construction = {\n";
	newFocus->completeTooltip += "				type = industrial_complex\n";
	newFocus->completeTooltip += "				level = 2\n";
	newFocus->completeTooltip += "				instant_build = yes\n";
	newFocus->completeTooltip += "			}";
	newFocus->completionReward += "			random_owned_controlled_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = industrial_complex\n";
	newFocus->completionReward += "						size > 1\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = yes\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = industrial_complex\n";
	newFocus->completionReward += "										size > 1\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = yes\n";
	newFocus->completionReward += "								}\n";
	newFocus->completionReward += "							}\n";
	newFocus->completionReward += "						}\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 2\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = industrial_complex\n";
	newFocus->completionReward += "					level = 2\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "nuclear_effort";
	newFocus->icon = "GFX_focus_wonderweapons";
	newFocus->prerequisites.push_back("focus = infrastructure_effort_2");
	newFocus->xPos = 10;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = nuclear_bonus\n";
	newFocus->completionReward += "				ahead_reduction = 0.5\n";
	newFocus->completionReward += "				category = nuclear\n";
	newFocus->completionReward += "			}";
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0.25\n";
	newFocus->aiWillDo += "				always = yes\n";
	newFocus->aiWillDo += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "extra_tech_slot";
	newFocus->icon = "GFX_focus_research";
	newFocus->prerequisites.push_back("focus = infrastructure_effort_2");
	newFocus->xPos = 12;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_research_slot = 1";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "extra_tech_slot_2";
	newFocus->icon = "GFX_focus_research";
	newFocus->prerequisites.push_back("focus = extra_tech_slot");
	newFocus->available += "			num_of_factories > 50";
	newFocus->cancelIfInvalid += "no";
	newFocus->continueIfInvalid += "yes";
	newFocus->xPos = 12;
	newFocus->yPos = 6;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_research_slot = 1";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "secret_weapons";
	newFocus->icon = "GFX_goal_generic_secret_weapon";
	newFocus->prerequisites.push_back("focus = infrastructure_effort_2");
	newFocus->xPos = 14;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = secret_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 4\n";
	newFocus->completionReward += "				category = electronics\n";
	newFocus->completionReward += "				category = nuclear\n";
	newFocus->completionReward += "				category = rocketry\n";
	newFocus->completionReward += "			}";
	newFocus->aiWillDo += "			factor = 1\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0.25\n";
	newFocus->aiWillDo += "				always = yes\n";
	newFocus->aiWillDo += "			}";
	focuses.push_back(newFocus);

	nextFreeColumn += 16;

	int numCollectovistIdeologies = calculateNumCollectovistIdeologies(majorIdeologies);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "political_effort";
	newFocus->icon = "GFX_goal_generic_demand_territory";
	newFocus->xPos = nextFreeColumn + numCollectovistIdeologies + 1;
	newFocus->yPos = 0;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_political_power = 120";
	focuses.push_back(newFocus);

	if (numCollectovistIdeologies > 0)
	{
		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "collectivist_ethos";
		newFocus->icon = "GFX_goal_generic_national_unity #icon = GFX_goal_tripartite_pact";
		newFocus->prerequisites.push_back("focus = political_effort");
		newFocus->mutuallyExclusive = "focus = liberty_ethos";
		newFocus->available += "			OR = {\n";
		newFocus->available += "				has_government = fascism\n";
		newFocus->available += "				has_government = communism\n";
		newFocus->available += "				has_government = absolutist\n";
		newFocus->available += "				has_government = radical\n";
		newFocus->available += "				has_government = neutrality\n";
		newFocus->available += "			}";
		newFocus->xPos = nextFreeColumn + numCollectovistIdeologies - 1;
		newFocus->yPos = 1;
		newFocus->cost = 10;
		newFocus->availableIfCapitulated = true;
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				OR = {\n";
		newFocus->aiWillDo += "					AND = {\n";
		newFocus->aiWillDo += "						is_historical_focus_on = yes\n";
		newFocus->aiWillDo += "						has_government = neutrality\n";
		newFocus->aiWillDo += "					}\n";
		newFocus->aiWillDo += "					has_idea = neutrality_idea\n";
		newFocus->aiWillDo += "				}\n";
		newFocus->aiWillDo += "			}";
		if (majorIdeologies.count("democratic") > 0)
		{
			newFocus->completionReward += "			add_ideas = collectivist_ethos_focus_democratic";
		}
		else
		{
			newFocus->completionReward += "			add_ideas = collectivist_ethos_focus_neutral";
		}
		focuses.push_back(newFocus);

		determineMutualExclusions(majorIdeologies);

		string ideolgicalFanaticsmPrereqs;
		if (majorIdeologies.count("fascism") > 0)
		{
			addFascistGenericFocuses();
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = paramilitarism";
			nextFreeColumn += 2;
		}
		if (majorIdeologies.count("communism") > 0)
		{
			addCommunistGenericFocuses();
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = political_commissars";
			nextFreeColumn += 2;
		}
		if (majorIdeologies.count("absolutist") > 0)
		{
			addAbsolutistGenericFocuses();
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = historical_claims_focus";
			nextFreeColumn += 2;
		}
		if (majorIdeologies.count("radical") > 0)
		{
			addRadicalGenericFocuses();
			if (ideolgicalFanaticsmPrereqs.size() > 0)
			{
				ideolgicalFanaticsmPrereqs += " ";
			}
			ideolgicalFanaticsmPrereqs += "focus = army_provides_focus";
			nextFreeColumn += 2;
		}

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "ideological_fanaticism";
		newFocus->icon = "GFX_goal_generic_demand_territory";
		newFocus->prerequisites.push_back(ideolgicalFanaticsmPrereqs);
		newFocus->xPos = nextFreeColumn - numCollectovistIdeologies - 1;
		newFocus->yPos = 6;
		newFocus->cost = 10;
		newFocus->availableIfCapitulated = true;
		newFocus->completionReward += "			add_ideas = ideological_fanaticism_focus\n";
		newFocus->completionReward += "			set_rule = {\n";
		newFocus->completionReward += "				can_create_factions = yes\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "			hidden_effect = {\n";
		newFocus->completionReward += "				set_rule = { can_use_kamikaze_pilots = yes }\n";
		newFocus->completionReward += "			}\n";
		newFocus->completionReward += "			custom_effect_tooltip = kamikaze_focus_tooltip";
		focuses.push_back(newFocus);
	}

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "liberty_ethos";
	newFocus->icon = "GFX_goal_support_democracy";
	newFocus->prerequisites.push_back("focus = political_effort");
	if (numCollectovistIdeologies > 0)
	{
		newFocus->mutuallyExclusive = "focus = collectivist_ethos";
	}
	if (majorIdeologies.count("democratic") != 0)
	{
		newFocus->available += "			OR = {\n";
		newFocus->available += "				has_government = democratic\n";
		newFocus->available += "				has_government = neutrality\n";
		newFocus->available += "			}";
	}
	else
	{
		newFocus->available += "			has_government = neutrality\n";
	}
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 95\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0.1\n";
	newFocus->aiWillDo += "				any_neighbor_country = {\n";
	newFocus->aiWillDo += "					is_major = yes\n";
	newFocus->aiWillDo += "					OR = {\n";
	newFocus->aiWillDo += "						has_government = communism\n";
	newFocus->aiWillDo += "						has_government = fascism\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "				NOT = {\n";
	newFocus->aiWillDo += "					any_neighbor_country = {\n";
	newFocus->aiWillDo += "						is_major = yes\n";
	newFocus->aiWillDo += "						has_government = democratic\n";
	newFocus->aiWillDo += "					}\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = liberty_ethos_focus";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "neutrality_focus";
	newFocus->icon = "GFX_goal_generic_neutrality_focus";
	newFocus->prerequisites.push_back("focus = liberty_ethos");
	if (majorIdeologies.count("democratic") != 0)
	{
		newFocus->mutuallyExclusive = "focus = interventionism_focus";
	}
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = { NOT = { has_idea = neutrality_idea } }\n";
	newFocus->completionReward += "				add_ideas = neutrality_idea\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			add_political_power = 150";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "deterrence";
	newFocus->icon = "GFX_goal_generic_defence";
	newFocus->prerequisites.push_back("focus = neutrality_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = deterrence";
	focuses.push_back(newFocus);

	nextFreeColumn += 2;

	if (majorIdeologies.count("democratic") != 0)
	{
		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "interventionism_focus";
		newFocus->icon = "GFX_goal_generic_political_pressure";
		newFocus->prerequisites.push_back("focus = liberty_ethos");
		newFocus->mutuallyExclusive = "focus = neutrality_focus";
		newFocus->xPos = nextFreeColumn;
		newFocus->yPos = 2;
		newFocus->cost = 10;
		newFocus->availableIfCapitulated = true;
		newFocus->aiWillDo += "			factor = 1\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				has_idea = neutrality_idea\n";
		newFocus->aiWillDo += "			}";
		newFocus->completionReward += "			if = {\n";
		newFocus->completionReward += "				limit = { has_idea = neutrality_idea }\n";
		newFocus->completionReward += "				remove_ideas = neutrality_idea\n";
		newFocus->completionReward += "			}	\n";
		newFocus->completionReward += "			set_rule = { can_send_volunteers = yes }\n";
		newFocus->completionReward += "			add_political_power = 150";
		focuses.push_back(newFocus);

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "volunteer_corps";
		newFocus->icon = "GFX_goal_generic_allies_build_infantry";
		newFocus->prerequisites.push_back("focus = interventionism_focus");
		newFocus->xPos = nextFreeColumn;
		newFocus->yPos = 3;
		newFocus->cost = 10;
		newFocus->availableIfCapitulated = true;
		newFocus->completionReward += "			add_ideas = volunteer_corps_focus";
		focuses.push_back(newFocus);

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "foreign_expeditions";
		newFocus->icon = "GFX_goal_generic_more_territorial_claims";
		newFocus->prerequisites.push_back("focus = volunteer_corps");
		newFocus->xPos = nextFreeColumn;
		newFocus->yPos = 4;
		newFocus->cost = 10;
		newFocus->availableIfCapitulated = true;
		newFocus->completionReward += "			add_ideas = foreign_expeditions_focus";
		focuses.push_back(newFocus);

		nextFreeColumn += 2;
	}

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "why_we_fight";
	newFocus->icon = "GFX_goal_generic_propaganda";
	if (majorIdeologies.count("democratic") != 0)
	{
		newFocus->prerequisites.push_back("focus = foreign_expeditions focus = deterrence");
	}
	else
	{
		newFocus->prerequisites.push_back("focus = deterrence");
	}
	newFocus->available += "			OR = {\n";
	newFocus->available += "				threat > 0.75\n";
	newFocus->available += "				has_defensive_war = yes\n";
	newFocus->available += "			}";
	newFocus->continueIfInvalid += "yes";
	newFocus->xPos = nextFreeColumn - 4;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = why_we_fight_focus\n";
	newFocus->completionReward += "			custom_effect_tooltip = why_we_fight_effect\n";
	newFocus->completionReward += "			hidden_effect = {\n";
	newFocus->completionReward += "				if = {\n";
	newFocus->completionReward += "					limit = { NOT = { has_idea = neutrality_idea } }\n";
	newFocus->completionReward += "					set_rule = { can_create_factions = yes }\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "technology_sharing";
	newFocus->icon = "GFX_goal_generic_scientific_exchange";
	if (numCollectovistIdeologies > 0)
	{
		newFocus->prerequisites.push_back("focus = ideological_fanaticism focus = why_we_fight");
	}
	else
	{
		newFocus->prerequisites.push_back("focus = why_we_fight");
	}
	newFocus->available += "			has_war = yes\n";
	newFocus->available += "			is_in_faction = yes\n";
	newFocus->available += "			OR = {\n";
	newFocus->available += "				num_of_factories > 50\n";
	newFocus->available += "				any_other_country = {\n";
	newFocus->available += "					is_in_faction_with = ROOT\n";
	newFocus->available += "					num_of_factories > 50\n";
	newFocus->available += "				}\n";
	newFocus->available += "			}";
	newFocus->xPos = nextFreeColumn - 4;
	newFocus->yPos = 7;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					original_research_slots < 3\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_research_slot = 1\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					original_research_slots > 2\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_tech_bonus = {\n";
	newFocus->completionReward += "					name = electronics_bonus\n";
	newFocus->completionReward += "					bonus = 0.5\n";
	newFocus->completionReward += "					uses = 1\n";
	newFocus->completionReward += "					category = electronics\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_tech_bonus = {\n";
	newFocus->completionReward += "					name = industrial_bonus\n";
	newFocus->completionReward += "					bonus = 0.5\n";
	newFocus->completionReward += "					uses = 1\n";
	newFocus->completionReward += "					category = industry\n";
	newFocus->completionReward += "				}	\n";
	newFocus->completionReward += "				add_tech_bonus = {\n";
	newFocus->completionReward += "					name = infantry_weapons_bonus\n";
	newFocus->completionReward += "					bonus = 0.5\n";
	newFocus->completionReward += "					uses = 1\n";
	newFocus->completionReward += "					category = infantry_weapons\n";
	newFocus->completionReward += "					category = artillery\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);
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


void HoI4FocusTree::addFascistGenericFocuses()
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
	newFocus->id = "nationalism_focus";
	newFocus->icon = "GFX_goal_support_fascism #icon = GFX_goal_tripartite_pact";
	newFocus->prerequisites.push_back("focus = collectivist_ethos");
	newFocus->mutuallyExclusive = fascistMutualExlusions;
	newFocus->available += "			OR = {\n";
	newFocus->available += "				has_government = fascism\n";
	newFocus->available += "				has_government = neutrality\n";
	newFocus->available += "			}";
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 5\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 2\n";
	newFocus->aiWillDo += "				any_neighbor_country = {\n";
	newFocus->aiWillDo += "					is_major = yes\n";
	newFocus->aiWillDo += "					has_government = fascism\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				AND = {\n";
	newFocus->aiWillDo += "					is_historical_focus_on = yes\n";
	newFocus->aiWillDo += "					NOT = { has_government = fascism }\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = nationalism";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "militarism";
	newFocus->icon = "GFX_goal_generic_political_pressure";
	newFocus->prerequisites.push_back("focus = nationalism_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = { has_idea = neutrality_idea }\n";
	newFocus->completionReward += "				remove_ideas = neutrality_idea\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			add_ideas = militarism_focus\n";
	newFocus->completionReward += "			army_experience = 20\n";
	newFocus->completionReward += "			set_rule = { can_send_volunteers = yes }";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "military_youth";
	newFocus->icon = "GFX_goal_generic_more_territorial_claims";
	newFocus->prerequisites.push_back("focus = militarism");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = military_youth_focus\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = { has_government = fascism }\n";
	newFocus->completionReward += "				add_popularity = {\n";
	newFocus->completionReward += "					ideology = fascism\n";
	newFocus->completionReward += "					popularity = 0.2\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "paramilitarism";
	newFocus->icon = "GFX_goal_generic_military_sphere";
	newFocus->prerequisites.push_back("focus = military_youth");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = paramilitarism_focus";
	focuses.push_back(newFocus);
}


void HoI4FocusTree::addCommunistGenericFocuses()
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
	newFocus->id = "internationalism_focus";
	newFocus->icon = "GFX_goal_support_communism #icon = GFX_goal_tripartite_pact";
	newFocus->prerequisites.push_back("focus = collectivist_ethos");
	newFocus->mutuallyExclusive = communistMutualExclusions;
	newFocus->available += "			OR = {\n";
	newFocus->available += "				has_government = communism\n";
	newFocus->available += "				has_government = neutrality\n";
	newFocus->available += "			}";
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 5\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 2\n";
	newFocus->aiWillDo += "				any_neighbor_country = {\n";
	newFocus->aiWillDo += "					is_major = yes\n";
	newFocus->aiWillDo += "					has_government = communism\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				AND = {\n";
	newFocus->aiWillDo += "					is_historical_focus_on = yes\n";
	newFocus->aiWillDo += "					NOT = { has_government = communism }\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = internationalism";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "political_correctness";
	newFocus->icon = "GFX_goal_generic_dangerous_deal";
	newFocus->prerequisites.push_back("focus = internationalism_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = { has_idea = neutrality_idea }\n";
	newFocus->completionReward += "				remove_ideas = neutrality_idea\n";
	newFocus->completionReward += "			}		\n";
	newFocus->completionReward += "			add_political_power = 200\n";
	newFocus->completionReward += "			add_ideas = idea_political_correctness";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "indoctrination_focus";
	newFocus->icon = "GFX_goal_generic_propaganda";
	newFocus->prerequisites.push_back("focus = political_correctness");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = indoctrination_focus\n";
	newFocus->completionReward += "			add_political_power = 150";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "political_commissars";
	newFocus->icon = "GFX_goal_generic_forceful_treaty";
	newFocus->prerequisites.push_back("focus = indoctrination_focus");
	newFocus->available = " ";
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = political_commissars_focus\n";
	newFocus->completionReward += "			if = {\n";
	newFocus->completionReward += "				limit = { has_government = communism }\n";
	newFocus->completionReward += "				add_popularity = {\n";
	newFocus->completionReward += "					ideology = communism\n";
	newFocus->completionReward += "					popularity = 0.2\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}\n";
	newFocus->completionReward += "			add_political_power = 200";
	focuses.push_back(newFocus);
}


void HoI4FocusTree::addAbsolutistGenericFocuses()
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
	newFocus->id = "absolutism_focus";
	newFocus->icon = "GFX_focus_hun_elect_a_king";
	newFocus->prerequisites.push_back("focus = collectivist_ethos");
	newFocus->mutuallyExclusive = absolutistMutualExlusions;
	newFocus->available += "			OR = {\n";
	newFocus->available += "				has_government = absolutist\n";
	newFocus->available += "				has_government = neutrality\n";
	newFocus->available += "			}";
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 5\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 2\n";
	newFocus->aiWillDo += "				any_neighbor_country = {\n";
	newFocus->aiWillDo += "					is_major = yes\n";
	newFocus->aiWillDo += "					has_government = absolutist\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				AND = {\n";
	newFocus->aiWillDo += "					is_historical_focus_on = yes\n";
	newFocus->aiWillDo += "					NOT = { has_government = absolutist }\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = absolutism";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "royal_dictatorship_focus";
	newFocus->icon = "GFX_focus_rom_royal_dictatorship";
	newFocus->text = "Absolutism Focus";
	newFocus->prerequisites.push_back("focus = absolutism_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_political_power = 200\n";
	newFocus->completionReward += "				add_popularity = {\n";
	newFocus->completionReward += "					ideology = absolutist\n";
	newFocus->completionReward += "					popularity = 0.2\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "				add_ideas = royal_dictatorship_focus";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "royal_army_tradition_focus";
	newFocus->icon = "GFX_goal_generic_special_forces";
	newFocus->prerequisites.push_back("focus = royal_dictatorship_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			army_experience = 40\n";
	newFocus->completionReward += "			# add a elite division template?";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "historical_claims_focus";
	newFocus->icon = "GFX_goal_generic_occupy_states_ongoing_war";
	newFocus->prerequisites.push_back("focus = royal_army_tradition_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = historical_claims_focus";
	focuses.push_back(newFocus);
}


void HoI4FocusTree::addRadicalGenericFocuses()
{
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
	newFocus->id = "radical_focus";
	newFocus->icon = "GFX_goal_generic_consumer_goods";
	newFocus->prerequisites.push_back("focus = collectivist_ethos");
	newFocus->mutuallyExclusive = radicalMutualExclusions;
	newFocus->available += "			OR = {\n";
	newFocus->available += "				has_government = radical\n";
	newFocus->available += "				has_government = neutrality\n";
	newFocus->available += "			}";
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->aiWillDo += "			factor = 5\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 2\n";
	newFocus->aiWillDo += "				any_neighbor_country = {\n";
	newFocus->aiWillDo += "					is_major = yes\n";
	newFocus->aiWillDo += "					has_government = radical\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				AND = {\n";
	newFocus->aiWillDo += "					is_historical_focus_on = yes\n";
	newFocus->aiWillDo += "					NOT = { has_government = radical }\n";
	newFocus->aiWillDo += "				}\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = radicalism";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "private_channels_focus";
	newFocus->icon = "GFX_goal_generic_intelligence_exchange";
	newFocus->prerequisites.push_back("focus = radical_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			every_country = { add_opinion_modifier = { target = ROOT modifier = private_channels_trade }}";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "hardfought_market_focus";
	newFocus->icon = "GFX_focus_generic_license_production";
	newFocus->prerequisites.push_back("focus = private_channels_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = hardfought_market_focus";
	focuses.push_back(newFocus);

	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "army_provides_focus";
	newFocus->icon = "GFX_focus_generic_concessions";
	newFocus->prerequisites.push_back("focus = hardfought_market_focus");
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 5;
	newFocus->cost = 10;
	newFocus->availableIfCapitulated = true;
	newFocus->completionReward += "			add_ideas = army_provides_focus";
	focuses.push_back(newFocus);
}


shared_ptr<HoI4FocusTree> HoI4FocusTree::makeCustomizedCopy(shared_ptr<HoI4Country> country) const
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
	nextFreeColumn += 3;

	double WTModifier = 1;
	if (Home->getGovernmentIdeology() == "democratic")
	{
		string warPol = Home->getRulingParty()->war_policy;
		if (warPol == "jingoism")
			WTModifier = 0;
		if (warPol == "pro_military")
			WTModifier = 0.25;
		if (warPol == "anti_military")
			WTModifier = 0.5;
		if (warPol == "pacifism" || warPol == "pacifist")
			WTModifier = 0.5;
	}

	//War Propaganda
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
	newFocus->id = "WarProp" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_propaganda";
	newFocus->text += "War Propaganda";
	newFocus->available += "			threat > " + to_string(0.2*WTModifier);
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 0;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10";
	newFocus->completionReward += "			add_national_unity = 0.05";
	focuses.push_back(newFocus);

	//Prepare Intervention
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "PrepInter" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_occupy_states_ongoing_war";
	newFocus->text += "Prepare Intervention";
	newFocus->prerequisites.push_back("focus = WarProp" + Home->getTag());
	newFocus->available += "			threat > " + to_string(0.3 * WTModifier);
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10";
	newFocus->completionReward += "			add_political_power = 120";
	newFocus->completionReward += "			set_rule = { can_send_volunteers = yes }";
	focuses.push_back(newFocus);

	if (CountriesToContain.size() >= 2)
	{
		nextFreeColumn += -3;
	}
	if (CountriesToContain.size() == 1)
	{
		nextFreeColumn += -2;
	}

	//Limited Intervention
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "Lim" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_more_territorial_claims";
	newFocus->text += "Limited Intervention";
	newFocus->prerequisites.push_back("focus = PrepInter" + Home->getTag());
	newFocus->available += "			threat > " + to_string(0.5 * WTModifier);
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10";
	newFocus->completionReward += "			add_ideas = limited_interventionism";
	focuses.push_back(newFocus);

	nextFreeColumn += 2;

	for (int i = CountriesToContain.size() - 1; i >= 0; i--)
	{
		auto Country = CountriesToContain[i];

		auto possibleContainedCountryName = Country->getSourceCountry()->getName("english");
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

		//War Plan
		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "WarPlan" + Home->getTag() + Country->getTag();
		newFocus->icon = "GFX_goal_generic_position_armies";
		newFocus->text += "War Plan " + containedCountryName;
		newFocus->prerequisites.push_back("focus = PrepInter" + Home->getTag());
		newFocus->available += "			any_other_country = {";
		newFocus->available += "						original_tag = " + Country->getTag();
		newFocus->available += "						exists = yes";
		newFocus->available += "						NOT = { is_in_faction_with = " + Home->getTag() + " }\n";
		newFocus->available += "						OR = {";
		newFocus->available += "							has_offensive_war = yes";
		newFocus->available += "							has_added_tension_amount > 30";
		newFocus->available += "							}";
		newFocus->available += "			}";
		newFocus->xPos = nextFreeColumn;
		newFocus->yPos = 2;
		newFocus->cost = 10;
		newFocus->bypass += "					has_war_with = " + Country->getTag() + "\n";
		newFocus->aiWillDo += "			factor = 10";
		newFocus->completionReward += "			army_experience = 20\n";
		newFocus->completionReward += "			add_tech_bonus = {\n";
		newFocus->completionReward += "				name = land_doc_bonus\n";
		newFocus->completionReward += "				bonus = 0.5\n";
		newFocus->completionReward += "				uses = 1\n";
		newFocus->completionReward += "				category = land_doctrine\n";
		newFocus->completionReward += "			}";
		focuses.push_back(newFocus);

		//Embargo
		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Embargo" + Home->getTag() + Country->getTag();
		newFocus->icon = "GFX_goal_generic_trade";
		newFocus->text += "Embargo " + containedCountryName;
		newFocus->prerequisites.push_back("focus =  WarPlan" + Home->getTag() + Country->getTag());
		newFocus->available += "			any_other_country = {";
		newFocus->available += "						original_tag = " + Country->getTag();
		newFocus->available += "						exists = yes";
		newFocus->available += "						NOT = { is_in_faction_with = " + Home->getTag() + " }\n";
		newFocus->available += "						OR = {";
		newFocus->available += "							has_offensive_war = yes";
		newFocus->available += "							has_added_tension_amount > 30";
		newFocus->available += "							threat > 0.6";
		newFocus->available += "							}";
		newFocus->available += "			}";
		newFocus->xPos = nextFreeColumn;
		newFocus->yPos = 3;
		newFocus->cost = 10;
		newFocus->bypass += "					has_war_with = " + Country->getTag() + "\n";
		newFocus->aiWillDo += "			factor = 10";
		newFocus->completionReward += "			" + Country->getTag() + " = {\n";
		newFocus->completionReward += "				add_opinion_modifier = { target = " + Home->getTag() + " modifier = embargo }\n";
		newFocus->completionReward += "			}";
		focuses.push_back(newFocus);

		//WAR
		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "WAR" + Home->getTag() + Country->getTag();
		newFocus->icon = "GFX_goal_support_democracy";
		newFocus->text += "Enact War Plan " + containedCountryName;
		newFocus->available += "						has_war = no\n";
		newFocus->available += "			any_other_country = {";
		newFocus->available += "						original_tag = " + Country->getTag();
		newFocus->available += "						exists = yes";
		newFocus->available += "						NOT = { is_in_faction_with = " + Home->getTag() + " }\n";
		newFocus->available += "						OR = {";
		newFocus->available += "							has_offensive_war = yes";
		newFocus->available += "							has_added_tension_amount > 30";
		newFocus->available += "							threat > 0.6";
		newFocus->available += "							}";
		newFocus->available += "			}";
		newFocus->prerequisites.push_back("focus =  Embargo" + Home->getTag() + Country->getTag());
		newFocus->prerequisites.push_back("focus =  Lim" + Home->getTag());
		newFocus->xPos = nextFreeColumn;
		newFocus->yPos = 4;
		newFocus->cost = 10;
		newFocus->bypass += "					has_war_with = " + Country->getTag() + "\n";
		newFocus->aiWillDo += "			factor = 10";
		newFocus->completionReward += "			declare_war_on = {\n";
		newFocus->completionReward += "				type = puppet_wargoal_focus\n";
		newFocus->completionReward += "				target = " + Country->getTag() + "\n";
		newFocus->completionReward += "			}";
		focuses.push_back(newFocus);
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

	//Glory to Empire!
	shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
	newFocus->id = "EmpireGlory" + Home->getTag();
	newFocus->icon = "GFX_goal_anschluss";
	newFocus->text += "Glory to the Empire!";
	//newFocus->available += "			is_puppet = no\n";
	newFocus->xPos = nextFreeColumn + 5;
	newFocus->yPos = 0;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "				factor = 0\n";
	newFocus->aiWillDo += "				date < 1937.6.6\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_national_unity = 0.05";
	focuses.push_back(newFocus);

	//Colonies Focus
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "StrengthenColonies" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_position_armies";
	newFocus->text += "Strengthen the Colonies";
	newFocus->prerequisites.push_back("focus = EmpireGlory" + Home->getTag());
	newFocus->mutuallyExclusive = "focus = StrengthenHome" + Home->getTag();
	newFocus->xPos = nextFreeColumn + 4;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 0\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			navy_experience = 25";
	focuses.push_back(newFocus);

	//Home Focus
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "StrengthenHome" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_national_unity";
	newFocus->text += "Strengthen Home";
	newFocus->prerequisites.push_back("focus = EmpireGlory" + Home->getTag());
	newFocus->mutuallyExclusive = "focus = StrengthenColonies" + Home->getTag();
	newFocus->xPos = nextFreeColumn + 6;
	newFocus->yPos = 1;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			army_experience = 25";
	focuses.push_back(newFocus);

	//Colonial Factories
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "ColonialInd" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_construct_civ_factory";
	newFocus->text += "Colonial Industry Buildup";
	newFocus->prerequisites.push_back("focus = StrengthenColonies" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 2;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = arms_factory\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
	newFocus->completionReward += "					OR = {\n";
	newFocus->completionReward += "						is_in_home_area = no\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = industrial_complex\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = no\n";
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
	newFocus->completionReward += "						is_in_home_area = no\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = industrial_complex\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = no\n";
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
	newFocus->completionReward += "						is_in_home_area = no\n";
	newFocus->completionReward += "						NOT = {\n";
	newFocus->completionReward += "							owner = {\n";
	newFocus->completionReward += "								any_owned_state = {\n";
	newFocus->completionReward += "									free_building_slots = {\n";
	newFocus->completionReward += "										building = industrial_complex\n";
	newFocus->completionReward += "										size > 0\n";
	newFocus->completionReward += "										include_locked = yes\n";
	newFocus->completionReward += "									}\n";
	newFocus->completionReward += "									is_in_home_area = no\n";
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
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	//Colonial Highway
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "ColonialHwy" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_construct_infrastructure";
	newFocus->text += "Colonial Highway";
	newFocus->prerequisites.push_back("focus = ColonialInd" + Home->getTag());
	newFocus->mutuallyExclusive = "focus = ResourceFac" + Home->getTag();
	newFocus->xPos = nextFreeColumn;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			random_owned_state = {\n";
	newFocus->completionReward += "				limit = {\n";
	newFocus->completionReward += "					free_building_slots = {\n";
	newFocus->completionReward += "						building = infrastructure\n";
	newFocus->completionReward += "						size > 0\n";
	newFocus->completionReward += "						include_locked = yes\n";
	newFocus->completionReward += "					}\n";
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
	newFocus->completionReward += "				add_extra_state_shared_building_slots = 1\n";
	newFocus->completionReward += "				add_building_construction = {\n";
	newFocus->completionReward += "					type = infrastructure\n";
	newFocus->completionReward += "					level = 1\n";
	newFocus->completionReward += "					instant_build = yes\n";
	newFocus->completionReward += "				}\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	//improve resources
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "ResourceFac" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_oil_refinery";
	newFocus->text += "Improve Resource Factories";
	newFocus->prerequisites.push_back("focus = ColonialInd" + Home->getTag());
	newFocus->mutuallyExclusive = "focus = ColonialHwy" + Home->getTag();
	newFocus->xPos = nextFreeColumn + 2;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = improved_resource_industry";
	focuses.push_back(newFocus);

	//establish colonial army
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "ColonialArmy" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_allies_build_infantry";
	newFocus->text += "Establish Colonial Army";
	newFocus->prerequisites.push_back("focus = StrengthenColonies" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 4;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = militarism_focus";
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
		newFocus->available += "			" + target->getTag() + " = { is_in_faction = no }";
		newFocus->prerequisites.push_back("focus = ColonialArmy" + Home->getTag());
		newFocus->xPos = nextFreeColumn + 4;
		newFocus->yPos = 3;
		newFocus->cost = 10;
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() +"\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}";
		newFocus->aiWillDo += "			factor = 10\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}";
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
		newFocus->available += "			" + target->getTag() + " = { is_in_faction = no }";
		newFocus->prerequisites.push_back("focus = Protectorate" + Home->getTag() + targetColonies.front()->getTag());
		newFocus->xPos = nextFreeColumn + 4;
		newFocus->yPos = 4;
		newFocus->cost = 10;
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() +"\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}";
		focuses.push_back(newFocus);
	}

	//Trade Empire
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "TradeEmpire" + Home->getTag();
	newFocus->icon = "GFX_goal_anschluss";
	newFocus->text += "Fund the " + homeCountryAdjective + " Colonial Trade Corporation";
	newFocus->prerequisites.push_back("focus = ColonialHwy" + Home->getTag() + " focus = ResourceFac" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 1;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
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
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	//Home Industry Buildup
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "IndHome" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_production";
	newFocus->text += "Fund Industrial Improvement";
	newFocus->prerequisites.push_back("focus = StrengthenHome" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 7;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10";
	newFocus->completionReward += "			add_tech_bonus = {\n";
	newFocus->completionReward += "				name = industrial_bonus\n";
	newFocus->completionReward += "				bonus = 0.5\n";
	newFocus->completionReward += "				uses = 1\n";
	newFocus->completionReward += "				category = industry\n";
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	//National Highway
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "NationalHwy" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_construct_infrastructure";
	newFocus->text += "National Highway";
	newFocus->prerequisites.push_back("focus = IndHome" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 6;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
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
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	//National College
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "NatCollege" + Home->getTag();
	newFocus->icon = "GFX_goal_anschluss";
	newFocus->text += "Establish National College";
	newFocus->prerequisites.push_back("focus = IndHome" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 8;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10";
	newFocus->completionReward += "			add_ideas = national_college";
	focuses.push_back(newFocus);

	//Improve Factories
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "MilitaryBuildup" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_construct_mil_factory";
	newFocus->text += "Military Buildup";
	newFocus->prerequisites.push_back("focus = NatCollege" + Home->getTag());
	newFocus->prerequisites.push_back("focus = NationalHwy" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 9;
	newFocus->yPos = 4;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
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
	newFocus->completionReward += "			}";
	focuses.push_back(newFocus);

	//PrepBorder
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "PrepTheBorder" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_defence";
	newFocus->text += "Prepare the Border";
	newFocus->prerequisites.push_back("focus = StrengthenHome" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 10;
	newFocus->yPos = 2;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = border_buildup";
	focuses.push_back(newFocus);

	//Promote Nationalistic Spirit
	newFocus = make_shared<HoI4Focus>();
	newFocus->id = "NatSpirit" + Home->getTag();
	newFocus->icon = "GFX_goal_generic_political_pressure";
	newFocus->text += "Promote Nationalistic Spirit";
	newFocus->prerequisites.push_back("focus = PrepTheBorder" + Home->getTag());
	newFocus->xPos = nextFreeColumn + 10;
	newFocus->yPos = 3;
	newFocus->cost = 10;
	newFocus->aiWillDo += "			factor = 10\n";
	newFocus->aiWillDo += "			modifier = {\n";
	newFocus->aiWillDo += "			}";
	newFocus->completionReward += "			add_ideas = paramilitarism_focus";
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
		newFocus->available += "			" + target->getTag() + " = { is_in_faction = no }";
		newFocus->prerequisites.push_back("focus = PrepTheBorder" + Home->getTag());
		newFocus->xPos = nextFreeColumn + 12;
		newFocus->yPos = 3;
		newFocus->cost = 10;
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() + "\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}";
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
		newFocus->available += "			" + target->getTag() + " = { is_in_faction = no }";
		newFocus->prerequisites.push_back("focus = NatSpirit" + Home->getTag());
		newFocus->xPos = nextFreeColumn + 10;
		newFocus->yPos = 4;
		newFocus->cost = 10;
		newFocus->bypass += "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = {\n";
		newFocus->bypass += "					is_in_faction_with = " + target->getTag() + "\n";
		newFocus->bypass += "					has_war_with = " + target->getTag() + "\n";
		newFocus->bypass += "				}\n";
		newFocus->bypass += "				NOT = { country_exists = " + target->getTag() + " }\n";
		newFocus->bypass += "			}";
		newFocus->aiWillDo += "			factor = 5\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 0\n";
		newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 1 }\n";
		newFocus->aiWillDo += "			}";
		newFocus->completionReward += "			create_wargoal = {\n";
		newFocus->completionReward += "				type = annex_everything\n";
		newFocus->completionReward += "				target = " + target->getTag() + "\n";
		newFocus->completionReward += "			}";
		focuses.push_back(newFocus);
	}
}

void HoI4FocusTree::addCommunistCoupBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& coupTargets)
{
	if (coupTargets.size() > 0)
	{
		//Focus to increase Comm support and prereq for coupTargets
		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Home_of_Revolution" + Home->getTag();
		newFocus->icon = "GFX_goal_support_communism";
		newFocus->text = "Home of the Revolution";
		newFocus->xPos = nextFreeColumn + coupTargets.size() - 1;
		newFocus->yPos = 0;
		newFocus->cost = 10;
		newFocus->aiWillDo = "			factor = 5";
		//FIXME 
		//Need to get Drift Defense to work
		//newFocus->completionReward += "			drift_defence_factor = 0.5\n";
		newFocus->completionReward += "			add_ideas = communist_influence";
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

				newFocus = make_shared<HoI4Focus>();
				newFocus->id = "Influence_" + coupTargets[i]->getTag() + "_" + Home->getTag();
				newFocus->icon = "GFX_goal_generic_propaganda";
				newFocus->text = "Influence " + coupCountryName;
				newFocus->prerequisites.push_back("focus = Home_of_Revolution" + Home->getTag());
				newFocus->xPos = nextFreeColumn + i * 2;
				newFocus->yPos = 1;
				newFocus->cost = 10;
				newFocus->aiWillDo = "			factor = 5";
				newFocus->completionReward += "			" + coupTargets[i]->getTag() + " = {\n";
				newFocus->completionReward += "				if = {\n";
				newFocus->completionReward += "					limit = {\n";
				newFocus->completionReward += "						" + Home->getTag() + " = {\n";
				newFocus->completionReward += "							has_government = fascism\n";
				newFocus->completionReward += "						}\n";
				newFocus->completionReward += "					}\n";
				newFocus->completionReward += "					add_ideas = fascist_influence\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				if = {\n";
				newFocus->completionReward += "					limit = {\n";
				newFocus->completionReward += "						" + Home->getTag() + " = {\n";
				newFocus->completionReward += "							has_government = communism\n";
				newFocus->completionReward += "						}\n";
				newFocus->completionReward += "					}\n";
				newFocus->completionReward += "					add_ideas = communist_influence\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				if = {\n";
				newFocus->completionReward += "					limit = {\n";
				newFocus->completionReward += "						" + Home->getTag() + " = {\n";
				newFocus->completionReward += "							has_government = democratic\n";
				newFocus->completionReward += "						}\n";
				newFocus->completionReward += "					}\n";
				newFocus->completionReward += "					add_ideas = democratic_influence\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				country_event = { id = generic.1 }\n";
				newFocus->completionReward += "			}";
				focuses.push_back(newFocus);

				//Civil War
				newFocus = make_shared<HoI4Focus>();
				newFocus->id = "Coup_" + coupTargets[i]->getTag() + "_" + Home->getTag();
				newFocus->icon = "GFX_goal_generic_demand_territory";
				newFocus->text = "Civil War in " + coupCountryName;
				newFocus->prerequisites.push_back("focus = Influence_" + coupTargets[i]->getTag() + "_" + Home->getTag());
				newFocus->available = "			" + coupTargets[i]->getTag() + " = { communism > 0.5 }";
				newFocus->xPos = nextFreeColumn + i * 2;
				newFocus->yPos = 2;
				newFocus->cost = 10;
				newFocus->aiWillDo = "			factor = 5";
				newFocus->completionReward += "			" + coupTargets[i]->getTag() + " = {\n";
				newFocus->completionReward += "				start_civil_war = {\n";
				newFocus->completionReward += "					ideology = communism\n";
				newFocus->completionReward += "					size = 0.5\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}";
				focuses.push_back(newFocus);
			}
		}
		nextFreeColumn += 2 * coupTargets.size();
	}
	return;
}

void HoI4FocusTree::addCommunistWarBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& warTargets, HoI4Events* events)
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
		newFocus->aiWillDo = "			factor = 5";
		newFocus->completionReward += "			army_experience = 20\n";
		newFocus->completionReward += "			add_tech_bonus = {\n";
		newFocus->completionReward += "				bonus = 0.5\n";
		newFocus->completionReward += "				uses = 2\n";
		newFocus->completionReward += "				category = land_doctrine\n";
		newFocus->completionReward += "			}";
		focuses.push_back(newFocus);

		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "Inter_Com_Pres" + Home->getTag();
		newFocus->icon = "GFX_goal_generic_dangerous_deal";
		newFocus->text = "International Communist Pressure";//change to faction name later
		newFocus->prerequisites.push_back("focus = StrengthCom" + Home->getTag());
		newFocus->available = "			date > 1937.1.1";
		newFocus->xPos = nextFreeColumn + warTargets.size() - 1;
		newFocus->yPos = 1;
		newFocus->cost = 10;
		newFocus->aiWillDo = "			factor = 5";
		newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }";
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
				newFocus->prerequisites.push_back("focus = Inter_Com_Pres" + Home->getTag());
				newFocus->available = "			date > 1938." + to_string(v1) + "." + to_string(v2);
				newFocus->xPos = nextFreeColumn + i * 2;
				newFocus->yPos = 2;
				newFocus->cost = 10;
				newFocus->bypass += "					has_war_with = " + warTargets[i]->getTag() + "\n";
				newFocus->aiWillDo = "			factor = 5\n";
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

				newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = puppet_wargoal_focus\n";
				newFocus->completionReward += "				target = " + warTargets[i]->getTag() + "\n";
				newFocus->completionReward += "			}";
				focuses.push_back(newFocus);
			}
		}
		nextFreeColumn += warTargets.size() * 2;
	}
}

void HoI4FocusTree::addFascistAnnexationBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& annexationTargets, HoI4Events* events)
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
		newFocus->aiWillDo = "			factor = 5";
		//FIXME
		//Need to get Drift Defense to work
		//in modified generic focus? (tk)
		//newFocus->completionReward += "			drift_defence_factor = 0.5\n";
		newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
		newFocus->completionReward += "			add_ideas = fascist_influence";
		focuses.push_back(newFocus);

		//Focus to increase army support
		newFocus = make_shared<HoI4Focus>();
		newFocus->id = "mil_march" + Home->getTag();
		newFocus->icon = "GFX_goal_generic_allies_build_infantry";
		newFocus->text = "Establish Military March Day";
		newFocus->prerequisites.push_back("focus = The_third_way" + Home->getTag());
		newFocus->xPos = nextFreeColumn + annexationTargets.size() - 1;
		newFocus->yPos = 1;
		newFocus->cost = 10;
		newFocus->aiWillDo = "			factor = 5";
		newFocus->completionReward += "			army_experience = 20\n";
		newFocus->completionReward += "			add_tech_bonus = {\n";
		newFocus->completionReward += "				bonus = 0.5\n";
		newFocus->completionReward += "				uses = 2\n";
		newFocus->completionReward += "				category = land_doctrine\n";
		newFocus->completionReward += "			}";
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
				newFocus->available = "			" + annexationTargets[i]->getTag() + " = {\n";
				newFocus->available += "				is_in_faction = no\n";
				newFocus->available += "			}\n";
				newFocus->available += "			is_puppet = no\n";
				newFocus->available += "			date > 1937." + to_string(v1 + 5) + "." + to_string(v2 + 5);
				newFocus->prerequisites.push_back("focus = mil_march" + Home->getTag());
				newFocus->xPos = nextFreeColumn + i * 2;
				newFocus->yPos = 2;
				newFocus->cost = 10;
				newFocus->aiWillDo = "			factor = 10\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				date < 1937.6.6\n";
				newFocus->aiWillDo += "			}";
				newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + annexationTargets[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + annexationTargets[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(events->getCurrentNationFocusEventNum()) + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}";
				focuses.push_back(newFocus);

				events->createAnnexEvent(Home, annexationTargets[i]);
			}
		}
		nextFreeColumn += annexationTargets.size() * 2;
	}
}

void HoI4FocusTree::addFascistSudetenBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& sudetenTargets, const vector<vector<int>>& demandedStates, const HoI4World* world)
{
	HoI4Events* events = world->getEvents();

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
//					newFocus->prerequisites.push_back("focus = " + Home->getTag() + "_anschluss_" + sudetenTargets[i]->getTag());
				}
			}
			newFocus->xPos = nextFreeColumn + sudetenTargets.size() - 1;
			newFocus->yPos = 0;
		}
		newFocus->cost = 10;
		newFocus->aiWillDo = "	factor = 5";
		newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }";//give some claims or cores
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
				newFocus->available = "		available = { " + sudetenTargets[i]->getTag() + " = { is_in_faction = no }";
				newFocus->prerequisites.push_back("focus = expand_the_reich" + Home->getTag());
				newFocus->available = "			is_puppet = no\n";
				newFocus->available += "			date > 1938." + to_string(v1) + "." + to_string(v2);
				newFocus->xPos = nextFreeColumn + 2 * i;
				newFocus->yPos = 1;
				newFocus->cost = 10;
				newFocus->bypass += "  has_war_with = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->aiWillDo = "	factor = 10\n";
				newFocus->aiWillDo += "	modifier = {\n";
				newFocus->aiWillDo += "		factor = 0\n";
				newFocus->aiWillDo += "		date < 1937.6.6\n";
				newFocus->aiWillDo += "	}";
				newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + sudetenTargets[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(events->getCurrentNationFocusEventNum()) + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}";
				addFocus(newFocus);

				//FINISH HIM
				newFocus = make_shared<HoI4Focus>();
				newFocus->id = Home->getTag() + "_finish_" + sudetenTargets[i]->getTag();
				newFocus->icon = "GFX_goal_generic_territory_or_war";
				newFocus->text = "Fate of " + sudetenTargetCountryName;
				newFocus->available = sudetenTargets[i]->getTag() + " = { is_in_faction = no }";
				newFocus->prerequisites.push_back("focus =  " + Home->getTag() + "_sudeten_" + sudetenTargets[i]->getTag());
				newFocus->available = "			is_puppet = no";
				newFocus->xPos = nextFreeColumn + 2 * i;
				newFocus->yPos = 2;
				newFocus->cost = 10;
				newFocus->aiWillDo = "	factor = 10\n";
				newFocus->aiWillDo += "	modifier = {\n";
				newFocus->aiWillDo += "		factor = 0\n";
				newFocus->aiWillDo += "		date < 1937.6.6\n";
				newFocus->aiWillDo += "	}";
				newFocus->bypass += "		has_war_with = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = annex_everything\n";
				newFocus->completionReward += "				target = " + sudetenTargets[i]->getTag() + "\n";
				newFocus->completionReward += "			}";
				addFocus(newFocus);

				//events
				events->createSudetenEvent(Home, sudetenTargets[i], demandedStates[i]);
			}
		}
		nextFreeColumn += 2 * sudetenTargets.size();
	}
}


void HoI4FocusTree::addGPWarBranch(shared_ptr<HoI4Country> Home, const vector<shared_ptr<HoI4Country>>& newAllies, const vector<shared_ptr<HoI4Country>>& GCTargets, const string& ideology, HoI4Events* events)
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
		newFocus->aiWillDo = "			factor = 2\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "			factor = 10\n";
		newFocus->aiWillDo += "			date > 1938.1.1\n";
		newFocus->aiWillDo += "			}";
		newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
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
		newFocus->prerequisites.push_back("focus = " + ideologyShort + "_Summit" + Home->getTag());
		newFocus->xPos = nextFreeColumn + i * 2;
		newFocus->yPos = 1;
		newFocus->cost = 10;
		newFocus->aiWillDo = "			factor = 10";
		newFocus->bypass = "			OR = {\n";
		newFocus->bypass += "				" + Home->getTag() + " = { is_in_faction_with = " + newAlly->getTag() + "}\n";
		newFocus->bypass += "				has_war_with = " + newAlly->getTag() + "\n";
		newFocus->bypass += "				NOT = { country_exists = " + newAlly->getTag() + " }\n";
		newFocus->bypass += "			}\n";
		newFocus->completionReward += "			" + newAlly->getTag() + " = {\n";
		newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(events->getCurrentNationFocusEventNum()) + " }\n";
		newFocus->completionReward += "				add_opinion_modifier = { target = " + Home->getTag() + " modifier = positive_50 }\n";
		newFocus->completionReward += "			}";
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
		if (newAllies.size() > 0)
		{
			y2 = 2;
			for (unsigned int i2 = 0; i2 < newAllies.size(); i2++)
			{
				prereq += " focus = Alliance_" + newAllies[i2]->getTag() + Home->getTag();
			}
		}
		int v1 = rand() % 12 + 1;
		int v2 = rand() % 12 + 1;
		shared_ptr<HoI4Focus> newFocus = make_shared<HoI4Focus>();
		newFocus->id = "War" + GC->getTag() + Home->getTag();
		newFocus->icon = "GFX_goal_generic_major_war";
		newFocus->text = "War with " + warTargetCountryName;//change to faction name later
		newFocus->prerequisites.push_back(prereq);
		newFocus->available = "			has_war = no\n";
		newFocus->available += "			date > 1939." + to_string(v1) + "." + to_string(v2);
		newFocus->xPos = nextFreeColumn + i * 2;
		newFocus->yPos = y2;
		newFocus->cost = 10;
		newFocus->bypass += "		   has_war_with = " + GC->getTag() + "\n";
		newFocus->aiWillDo = "			factor = " + to_string(10 - GCTargets.size() * 5) + "\n";
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
			newFocus->aiWillDo += "			}\n";
		}
		newFocus->completionReward += "			add_named_threat = { threat = 5 name = " + newFocus->id + " }\n";
		newFocus->completionReward += "			declare_war_on = {\n";
		newFocus->completionReward += "				type = puppet_wargoal_focus\n";
		newFocus->completionReward += "				target = " + GC->getTag() + "\n";
		newFocus->completionReward += "			}";
		focuses.push_back(newFocus);
		i++;
	}
	nextFreeColumn += 2 * max(newAllies.size(), GCTargets.size());
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
