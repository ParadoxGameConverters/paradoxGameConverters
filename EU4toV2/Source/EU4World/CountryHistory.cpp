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



#include "CountryHistory.h"



EU4::countryHistory::countryHistory(std::istream& theStream):
	items()
{
	auto ignore = getNextToken(theStream);

	registerKeyword(std::regex("government"), commonItems::ignoreString);
	registerKeyword(std::regex("government_rank"), commonItems::ignoreString);
	registerKeyword(std::regex("mercantilism"), commonItems::ignoreString);
	registerKeyword(std::regex("technology_group"), commonItems::ignoreString);
	registerKeyword(std::regex("historical_friend"), commonItems::ignoreString);
	registerKeyword(std::regex("historical_rival"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_type"), commonItems::ignoreString);
	registerKeyword(std::regex("primary_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("religion"), commonItems::ignoreString);
	registerKeyword(std::regex("religious_school"), commonItems::ignoreString);
	registerKeyword(std::regex("secondary_religion"), commonItems::ignoreString);
	registerKeyword(std::regex("add_harmonized_religion"), commonItems::ignoreString);
	registerKeyword(std::regex("add_accepted_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("capital"), commonItems::ignoreString);
	registerKeyword(std::regex("national_focus"), commonItems::ignoreString);
	registerKeyword(std::regex("fixed_capital"), commonItems::ignoreString);
	registerKeyword(std::regex("mercantilism"), commonItems::ignoreString);
	registerKeyword(std::regex("unlock_cult"), commonItems::ignoreString);
	registerKeyword(std::regex("elector"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_type"), commonItems::ignoreString);



	registerKeyword(std::regex("\\d+\\.\\d+\\.\\d+"), [this](const std::string& date, std::istream& theStream)
		{
			countryHistoryDate theDate(theStream);
			for (auto& item: theDate.getItems())
			{
				items.emplace_back(std::dynamic_pointer_cast<historyItem>(item));
			}
		}
	);

	parseStream(theStream);
}


EU4::countryHistoryDate::countryHistoryDate(std::istream& theStream):
	items()
{
	registerKeyword(std::regex("monarch"), commonItems::ignoreObject);
	registerKeyword(std::regex("queen"), commonItems::ignoreObject);
	registerKeyword(std::regex("heir"), commonItems::ignoreObject);
	registerKeyword(std::regex("leader"), [this](const std::string& date, std::istream& theStream)
		{
			shared_ptr<historyLeader> newLeader = make_unique<historyLeader>(theStream);
			items.emplace_back(newLeader);
		}
	);
	registerKeyword(std::regex("changed_country_mapcolor_from"), commonItems::ignoreObject);
	registerKeyword(std::regex("government"), commonItems::ignoreString);
	registerKeyword(std::regex("religion"), commonItems::ignoreString);
	registerKeyword(std::regex("add_accepted_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("adm_tech"), commonItems::ignoreString);
	registerKeyword(std::regex("dip_tech"), commonItems::ignoreString);
	registerKeyword(std::regex("mil_tech"), commonItems::ignoreString);
	registerKeyword(std::regex("capital"), commonItems::ignoreString);
	registerKeyword(std::regex("changed_tag_from"), commonItems::ignoreString);
	registerKeyword(std::regex("changed_country_name_from"), commonItems::ignoreString);
	registerKeyword(std::regex("changed_country_adjective_from"), commonItems::ignoreString);
	registerKeyword(std::regex("set_country_flag"), commonItems::ignoreString);
	registerKeyword(std::regex("clr_country_flag"), commonItems::ignoreString);
	registerKeyword(std::regex("set_global_flag"), commonItems::ignoreString);
	registerKeyword(std::regex("union"), commonItems::ignoreString);
	registerKeyword(std::regex("primary_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("religion"), commonItems::ignoreString);
	registerKeyword(std::regex("government_rank"), commonItems::ignoreString);
	registerKeyword(std::regex("decision"), commonItems::ignoreString);
	registerKeyword(std::regex("remove_accepted_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("mercantilism"), commonItems::ignoreString);
	registerKeyword(std::regex("elector"), commonItems::ignoreString);
	registerKeyword(std::regex("revolution_target"), commonItems::ignoreString);
	registerKeyword(std::regex("religious_school"), commonItems::ignoreString);
	registerKeyword(std::regex("technology_group"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_type"), commonItems::ignoreString);

	registerKeyword(std::regex("national_focus"), commonItems::ignoreString);
	registerKeyword(std::regex("trade_port"), commonItems::ignoreString);

	//idea groups

	//Administrative
	registerKeyword(std::regex("organized_mercenary_payment"), commonItems::ignoreString);
	registerKeyword(std::regex("organised_mercenary_payment"), commonItems::ignoreString);
	registerKeyword(std::regex("adaptability"), commonItems::ignoreString);
	registerKeyword(std::regex("benefits_for_mercenaries"), commonItems::ignoreString);
	registerKeyword(std::regex("bookkeeping"), commonItems::ignoreString);
	registerKeyword(std::regex("organised_mercenary_recruitment"), commonItems::ignoreString);
	registerKeyword(std::regex("administrative_efficiency_idea"), commonItems::ignoreString);
	registerKeyword(std::regex("civil_servants"), commonItems::ignoreString);

	//Economic
	registerKeyword(std::regex("bureaucracy"), commonItems::ignoreString);
	registerKeyword(std::regex("organized_construction"), commonItems::ignoreString);
	registerKeyword(std::regex("organised_construction"), commonItems::ignoreString);
	registerKeyword(std::regex("national_bank"), commonItems::ignoreString);
	registerKeyword(std::regex("debt_and_loans"), commonItems::ignoreString);
	registerKeyword(std::regex("centralization"), commonItems::ignoreString);
	registerKeyword(std::regex("nationalistic_enthusiasm"), commonItems::ignoreString);
	registerKeyword(std::regex("smithian_economics"), commonItems::ignoreString);

	//Expansion
	registerKeyword(std::regex("additional_colonists"), commonItems::ignoreString);
	registerKeyword(std::regex("additional_merchants"), commonItems::ignoreString);
	registerKeyword(std::regex("faster_colonists"), commonItems::ignoreString);
	registerKeyword(std::regex("organized_recruiting"), commonItems::ignoreString);
	registerKeyword(std::regex("organised_recruiting"), commonItems::ignoreString);
	registerKeyword(std::regex("additional_diplomats"), commonItems::ignoreString);
	registerKeyword(std::regex("improved_shipyards"), commonItems::ignoreString);
	registerKeyword(std::regex("competetive_merchants"), commonItems::ignoreString);

	//Humanist
	registerKeyword(std::regex("tolerance_idea"), commonItems::ignoreString);
	registerKeyword(std::regex("local_traditions"), commonItems::ignoreString);
	registerKeyword(std::regex("ecumenism"), commonItems::ignoreString);
	registerKeyword(std::regex("indirect_rule"), commonItems::ignoreString);
	registerKeyword(std::regex("cultural_ties"), commonItems::ignoreString);
	registerKeyword(std::regex("benevolence"), commonItems::ignoreString);
	registerKeyword(std::regex("humanist_tolerance"), commonItems::ignoreString);

	//Innovative
	registerKeyword(std::regex("patron_of_the_arts"), commonItems::ignoreString);
	registerKeyword(std::regex("pragmatism"), commonItems::ignoreString);
	registerKeyword(std::regex("scientific_revolution"), commonItems::ignoreString);
	registerKeyword(std::regex("dynamic_court"), commonItems::ignoreString);
	registerKeyword(std::regex("resilient_state"), commonItems::ignoreString);
	registerKeyword(std::regex("optimism"), commonItems::ignoreString);
	registerKeyword(std::regex("formalized_officer_corps"), commonItems::ignoreString);

	//Religious
	registerKeyword(std::regex("missionary_schools"), commonItems::ignoreString);
	registerKeyword(std::regex("church_attendance_duty"), commonItems::ignoreString);
	registerKeyword(std::regex("divine_supremacy"), commonItems::ignoreString);
	registerKeyword(std::regex("devoutness"), commonItems::ignoreString);
	registerKeyword(std::regex("religious_tradition"), commonItems::ignoreString);
	registerKeyword(std::regex("inquisition"), commonItems::ignoreString);
	registerKeyword(std::regex("deus_vult"), commonItems::ignoreString);

	//Diplomatic
	registerKeyword(std::regex("foreign_embassies"), commonItems::ignoreString);
	registerKeyword(std::regex("cabinet"), commonItems::ignoreString);
	registerKeyword(std::regex("war_cabinet"), commonItems::ignoreString);
	registerKeyword(std::regex("benign_diplomats"), commonItems::ignoreString);
	registerKeyword(std::regex("experienced_diplomats"), commonItems::ignoreString);
	registerKeyword(std::regex("flexible_negotiation"), commonItems::ignoreString);
	registerKeyword(std::regex("diplomatic_corps"), commonItems::ignoreString);

	//Espionage
	registerKeyword(std::regex("efficient_spies"), commonItems::ignoreString);
	registerKeyword(std::regex("agent_training"), commonItems::ignoreString);
	registerKeyword(std::regex("vetting"), commonItems::ignoreString);
	registerKeyword(std::regex("additional_loyalist_recruitment"), commonItems::ignoreString);
	registerKeyword(std::regex("claim_fabrication"), commonItems::ignoreString);
	registerKeyword(std::regex("privateers"), commonItems::ignoreString);
	registerKeyword(std::regex("audit_checks"), commonItems::ignoreString);

	//Exploration
	registerKeyword(std::regex("colonial_ventures"), commonItems::ignoreString);
	registerKeyword(std::regex("quest_for_the_new_world"), commonItems::ignoreString);
	registerKeyword(std::regex("overseas_exploration"), commonItems::ignoreString);
	registerKeyword(std::regex("land_of_opportunity"), commonItems::ignoreString);
	registerKeyword(std::regex("vice_roys"), commonItems::ignoreString);
	registerKeyword(std::regex("free_colonies"), commonItems::ignoreString);
	registerKeyword(std::regex("global_empire"), commonItems::ignoreString);

	//Influence
	registerKeyword(std::regex("tribute_system"), commonItems::ignoreString);
	registerKeyword(std::regex("establish_cadet_branches"), commonItems::ignoreString);
	registerKeyword(std::regex("integrated_elites"), commonItems::ignoreString);
	registerKeyword(std::regex("state_propaganda"), commonItems::ignoreString);
	registerKeyword(std::regex("diplomatic_influence"), commonItems::ignoreString);
	registerKeyword(std::regex("postal_service"), commonItems::ignoreString);
	registerKeyword(std::regex("marcher_lords"), commonItems::ignoreString);

	//Maritime
	registerKeyword(std::regex("sea_hawks"), commonItems::ignoreString);
	registerKeyword(std::regex("merchant_marine"), commonItems::ignoreString);
	registerKeyword(std::regex("sheltered_ports"), commonItems::ignoreString);
	registerKeyword(std::regex("grand_navy"), commonItems::ignoreString);
	registerKeyword(std::regex("ships_penny"), commonItems::ignoreString);
	registerKeyword(std::regex("excellent_shipwrights"), commonItems::ignoreString);
	registerKeyword(std::regex("naval_fighting_instruction"), commonItems::ignoreString);

	//Trade
	registerKeyword(std::regex("shrewd_commerce_practise"), commonItems::ignoreString);
	registerKeyword(std::regex("free_trade"), commonItems::ignoreString);
	registerKeyword(std::regex("merchant_adventures"), commonItems::ignoreString);
	registerKeyword(std::regex("national_trade_policy"), commonItems::ignoreString);
	registerKeyword(std::regex("overseas_merchants"), commonItems::ignoreString);
	registerKeyword(std::regex("trade_manipulation"), commonItems::ignoreString);
	registerKeyword(std::regex("fast_negotiations"), commonItems::ignoreString);

	//Aristocratic
	registerKeyword(std::regex("noble_knights"), commonItems::ignoreString);
	registerKeyword(std::regex("military_traditions"), commonItems::ignoreString);
	registerKeyword(std::regex("local_nobility"), commonItems::ignoreString);
	registerKeyword(std::regex("sefdom"), commonItems::ignoreString);
	registerKeyword(std::regex("noble_officers"), commonItems::ignoreString);
	registerKeyword(std::regex("international_diplomacy"), commonItems::ignoreString);
	registerKeyword(std::regex("noble_connections"), commonItems::ignoreString);

	//Defensive
	registerKeyword(std::regex("battlefield_commisions"), commonItems::ignoreString);
	registerKeyword(std::regex("military_drill"), commonItems::ignoreString);
	registerKeyword(std::regex("improved_manuever"), commonItems::ignoreString);
	registerKeyword(std::regex("regimental_system"), commonItems::ignoreString);
	registerKeyword(std::regex("defensive_mentality"), commonItems::ignoreString);
	registerKeyword(std::regex("supply_trains"), commonItems::ignoreString);
	registerKeyword(std::regex("improved_foraging"), commonItems::ignoreString);

	//Naval
	registerKeyword(std::regex("boarding_parties"), commonItems::ignoreString);
	registerKeyword(std::regex("improved_rams"), commonItems::ignoreString);
	registerKeyword(std::regex("naval_cadets"), commonItems::ignoreString);
	registerKeyword(std::regex("naval_glory"), commonItems::ignoreString);
	registerKeyword(std::regex("press_gangs"), commonItems::ignoreString);
	registerKeyword(std::regex("oak_forests_for_ships"), commonItems::ignoreString);
	registerKeyword(std::regex("superior_seamanship"), commonItems::ignoreString);

	//Offensive
	registerKeyword(std::regex("bayonet_leaders"), commonItems::ignoreString);
	registerKeyword(std::regex("national_conscripts"), commonItems::ignoreString);
	registerKeyword(std::regex("superior_firepower"), commonItems::ignoreString);
	registerKeyword(std::regex("glorious_arms"), commonItems::ignoreString);
	registerKeyword(std::regex("engineer_corps"), commonItems::ignoreString);
	registerKeyword(std::regex("grand_army"), commonItems::ignoreString);
	registerKeyword(std::regex("esprit_de_corps"), commonItems::ignoreString);

	//Plutocratic
	registerKeyword(std::regex("tradition_of_payment"), commonItems::ignoreString);
	registerKeyword(std::regex("abolished_serfdom"), commonItems::ignoreString);
	registerKeyword(std::regex("bill_of_rights"), commonItems::ignoreString);
	registerKeyword(std::regex("free_merchants"), commonItems::ignoreString);
	registerKeyword(std::regex("free_subjects"), commonItems::ignoreString);
	registerKeyword(std::regex("free_cities"), commonItems::ignoreString);
	registerKeyword(std::regex("emancipation"), commonItems::ignoreString);

	//Quality
	registerKeyword(std::regex("private_to_marshal"), commonItems::ignoreString);
	registerKeyword(std::regex("quality_education"), commonItems::ignoreString);
	registerKeyword(std::regex("finest_of_horses"), commonItems::ignoreString);
	registerKeyword(std::regex("escort_ships"), commonItems::ignoreString);
	registerKeyword(std::regex("naval_drill"), commonItems::ignoreString);
	registerKeyword(std::regex("copper_bottoms"), commonItems::ignoreString);
	registerKeyword(std::regex("massed_battery"), commonItems::ignoreString);

	//Quantity
	registerKeyword(std::regex("mass_army"), commonItems::ignoreString);
	registerKeyword(std::regex("the_young_can_serve"), commonItems::ignoreString);
	registerKeyword(std::regex("enforced_service"), commonItems::ignoreString);
	registerKeyword(std::regex("the_old_and_infirm"), commonItems::ignoreString);
	registerKeyword(std::regex("mercenary_contracts"), commonItems::ignoreString);
	registerKeyword(std::regex("garrison_conscription"), commonItems::ignoreString);
	registerKeyword(std::regex("expanded_supply_trains"), commonItems::ignoreString);




	registerKeyword(std::regex("napoleonic_warfare"), commonItems::ignoreString);
	registerKeyword(std::regex("migrative_traditions"), commonItems::ignoreString);
	registerKeyword(std::regex("smoke_ceremonies"), commonItems::ignoreString);
	registerKeyword(std::regex("warrior_societies"), commonItems::ignoreString);
	registerKeyword(std::regex("clan_societies"), commonItems::ignoreString);
	registerKeyword(std::regex("travois"), commonItems::ignoreString);
	registerKeyword(std::regex("wampum_belts"), commonItems::ignoreString);
	registerKeyword(std::regex("raid_warfare"), commonItems::ignoreString);
	registerKeyword(std::regex("law_of_peace"), commonItems::ignoreString);
	registerKeyword(std::regex("tribal_adoptions"), commonItems::ignoreString);
	registerKeyword(std::regex("clan_migrations"), commonItems::ignoreString);
	registerKeyword(std::regex("wampum_belts"), commonItems::ignoreString);
	registerKeyword(std::regex("trail_networks"), commonItems::ignoreString);
	registerKeyword(std::regex("talking_leaves"), commonItems::ignoreString);
	registerKeyword(std::regex("flexible_warfare"), commonItems::ignoreString);
	registerKeyword(std::regex("trade_diplomacy"), commonItems::ignoreString);
	registerKeyword(std::regex("tribal_constitution"), commonItems::ignoreString);


	registerKeyword(std::regex("inti_reform_1"), commonItems::ignoreString);
	registerKeyword(std::regex("inti_reform_2"), commonItems::ignoreString);
	registerKeyword(std::regex("inti_reform_3"), commonItems::ignoreString);
	registerKeyword(std::regex("inti_reform_4"), commonItems::ignoreString);
	registerKeyword(std::regex("inti_reform_5"), commonItems::ignoreString);


	registerKeyword(std::regex("patron_of_art"), commonItems::ignoreString);







	parseStream(theStream);
}


EU4::historyLeader::historyLeader(std::istream& theStream)
{
	type = "leader";
	theLeader = std::make_unique<EU4::leader>(EU4::leader(theStream));
}