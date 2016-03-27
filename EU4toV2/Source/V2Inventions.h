#ifndef V2INVENTIONS_H_
#define V2INVENTIONS_H_



#include <string>
using namespace std;



#define INVENTION_UNION(DO) \
	/*ARMY_INVENTIONS*/ \
 \
	/*post_napoleonic_thought*/ \
	/*strategic_mobility*/ \
	DO(field_fortifications) \
	DO(railroad_transport_groups) \
	DO(signal_detachments) \
 \
	/*point_defense_system*/ \
	DO(flexible_firepower) \
	DO(synchronized_heavy_support) \
	DO(staff_command_support) \
 \
	/*deep_defense_system*/ \
	DO(trench_systems) \
	DO(gas_attack_capability) \
	DO(gas_defense_capability) \
	DO(elastic_defense_system) \
	DO(flexible_counterattacks) \
 \
	/*infiltration*/ \
	DO(low_echelon_fire_support) \
	DO(stormtrooper_training) \
	DO(tank_experiments) \
	DO(tank_exploits) \
 \
	/*modern_army_doctrine*/ \
	DO(firepower_vs_mobility) \
	DO(grand_battle_plan_vs_superior_firepower) \
	DO(human_wave_vs_spearhead) \
 \
	/*flintlock_rifles*/ \
	/*muzzle_loaded_rifles*/ \
	/*breech_loaded_rifles*/ \
	/*machine_guns*/ \
	/*bolt_action_rifles*/ \
	/*modern_divisional_structure*/ \
	DO(interwar_infantry) \
	DO(interwar_cavalry) \
	DO(rear_area_dumps) \
 \
	/*bronze_muzzle_loaded_artillery*/ \
	/*iron_muzzle_loaded_artillery*/ \
	/*iron_breech_loaded_artillery*/ \
	/*steel_breech_loaded_artillery*/ \
	/*indirect_artillery_fire*/ \
	/*heavy_armament*/ \
	DO(tankette) \
	DO(light_tank) \
	DO(armoured_cars) \
	DO(anti_aircraft_artillery) \
	DO(improved_anti_aircraft_artillery) \
	DO(light_artillery) \
	DO(medium_artillery) \
	DO(heavy_artillery) \
 \
	/*military_staff_system*/ \
	/*military_plans*/ \
	DO(telegraph_coordination) \
	DO(mobilization_time_tables) \
	DO(personnel_category_tables) \
 \
	/*military_statistics*/ \
	DO(ammunition_area_effect_calculations) \
	DO(empirical_combat_efficiency_matrix) \
	DO(combat_loss_evaluation_schemata) \
 \
	/*military_logistics*/ \
	DO(pre_planned_depot_systems) \
	DO(logistical_calculations) \
	DO(replenishment_transport_organisation) \
 \
	/*military_directionism*/ \
	DO(army_political_lobbying) \
	DO(army_military_political_decision_making) \
	DO(army_local_military_government) \
 \
	/*aeronautics*/ \
	DO(early_interwar_bombers) \
	DO(late_interwar_bombers) \
	DO(early_interwar_fighters) \
	DO(late_interwar_fighters) \
	DO(t_air_superiority) \
	DO(flying_circus_doctrine) \
	DO(force_substitution_doctrine) \
	DO(battlefield_destruction_doctrine) \
	DO(operational_destruction_doctrine) \
	DO(strategic_destruction_doctrine) \
 \
	/*army_command_principle*/ \
	/*army_professionalism*/ \
	DO(army_academic_training) \
	DO(field_training) \
	DO(army_societal_status) \
 \
	/*army_decision_making*/ \
	DO(army_independent_command) \
	DO(army_continuous_issuing_of_orders) \
	DO(army_delegated_power) \
 \
	/*army_risk_management*/ \
	DO(army_wargames) \
	DO(army_alternative_plans) \
	DO(saddle_orders) \
 \
	/*army_nco_training*/ \
	DO(army_literate_ncos) \
	DO(nco_field_training) \
	DO(army_nco_tactical_training) \
 \
	/*great_war_experience*/ \
 \
 \
	/*COMMERCE_INVENTIONS*/ \
 \
	/*private_banks*/ \
	/*stock_exchange*/ \
	DO(multitude_of_financial_instruments) \
	DO(insurance_companies) \
	DO(regulated_buying_and_selling_of_stocks) \
 \
	/*business_banks*/ \
	DO(liquidation_of_assets) \
	DO(capital_for_investors) \
	DO(distribution_channels) \
 \
	/*investment_banks*/ \
	DO(industrial_rnd_investments) \
	DO(industrial_production_investments) \
	DO(industrial_distribution_investments) \
 \
	/*bank_inspection_board*/ \
	DO(private_bank_monitoring) \
	DO(market_behaviour_monitoring) \
 \
	/*mutual_funds*/ \
	DO(buying_on_margin) \
	 \
	/*no_standard*/ \
	/*ad_hoc_money_bill_printing*/ \
	DO(silver_standard) \
	DO(decimal_monetary_system) \
 \
	/*private_bank_money_bill_printing*/ \
	DO(double_standard) \
	DO(fixed_intramonetary_exchange_rates) \
 \
	/*central_bank_money_bill_printing*/ \
	DO(gold_standard) \
	DO(international_monetary_exchange_rates) \
 \
	/*modern_central_bank_system*/ \
	DO(state_bond_monetary_mechanism) \
	DO(interest_inflation_weapon) \
 \
	/*market_determined_exchange_rates*/ \
	DO(hyperinflation) \
 \
	/*eary_classical_theory*/ \
	DO(john_ramsay_mcculloch) \
	DO(nassau_william_sr) \
	DO(james_mill) \
 \
	/*late_classical_theory*/ \
	DO(john_elliot_cairnes) \
	DO(robert_torrens) \
	DO(john_stuart_mill) \
 \
	/*collectivist_theory*/ \
	DO(pierre_joseph_proudhon) \
	DO(karl_marx) \
	DO(friedrich_engels) \
	DO(eduard_bernstein) \
	DO(sidney_webb) \
	DO(karl_kautsky) \
	DO(jean_jaurès) \
 \
	/*the_historical_theory*/ \
	DO(friedrich_list) \
	DO(wilhelm_roscher) \
	DO(bruno_hildebrand) \
	DO(karl_knies) \
	DO(gustav_von_schmoller) \
	DO(adolph_wagner) \
	DO(william_cunningham) \
 \
	/*neoclassical_theory*/ \
	DO(johann_heinrich_von_thünen) \
	DO(léon_walras) \
	DO(stanley_jevons) \
	DO(carl_menger) \
	DO(thorstein_veblen) \
	DO(alfred_marshall) \
	DO(augustin_cournot) \
	DO(jules_dupuit) \
	DO(vilfredo_pareto) \
	DO(knut_wicksell) \
	DO(irving_fischer) \
	DO(joseph_schumpeter) \
	DO(eugen_von_böhm_bawerk) \
 \
	/*keynesian_economics*/ \
	DO(john_maynard_keynes) \
	DO(richard_kahn) \
	DO(ludwig_von_mises) \
	DO(friedrich_hayek) \
 \
	/*freedom_of_trade*/ \
	/*market_structure*/ \
	DO(polypoly_structure) \
	DO(oligopoly_structure) \
	DO(monopoly_structure) \
 \
	/*business_regulations*/ \
	DO(limited_partnership_company) \
	DO(joint_stock_company) \
	DO(patent) \
	DO(trademark) \
 \
	/*market_regulations*/ \
	DO(anti_trust_laws) \
	DO(financial_regulations) \
	DO(banking_regulations) \
 \
	/*economic_responsibility*/ \
	DO(working_board_of_directors) \
	DO(bankruptcy_proceedings) \
	DO(annual_report_proceedings) \
 \
	/*government_interventionism*/ \
	DO(priming_the_pump_) \
	DO(mixed_economy) \
	DO(centralized_economic_planning) \
	DO(autarky) \
	DO(import_substitution_industrialization) \
 \
	/*guild_based_production*/ \
	/*organized_factories*/ \
	DO(work_classification) \
	DO(work_specialization) \
	DO(work_control_and_hierarchy) \
 \
	/*scientific_management*/ \
	DO(sequence_planning) \
	DO(inventory_planning) \
	DO(quality_assurance_procedure) \
 \
	/*time_saving_measures*/ \
	DO(time_studies) \
	DO(time_clock) \
	DO(lighting) \
 \
	/*management_strategy*/ \
	DO(old_style_family_business) \
	DO(anynomous_investor_business) \
	DO(marketized_smallholder_business) \
 \
	/*organizational_development*/ \
	DO(i_scientific_management) \
	DO(human_resource_development) \
 \
 \
	/*CULTURE_INVENTIONS*/ \
	/*classicism_n_early_romanticism*/ \
	/*romanticism*/ \
	DO(romanticist_literature) \
	DO(romanticist_art) \
	DO(romanticist_music) \
 \
	/*realism*/ \
	DO(realist_literature) \
	DO(realist_art) \
	DO(realist_music) \
 \
	/*impressionism*/ \
	DO(impressionist_literature) \
	DO(impressionist_art) \
	DO(impressionist_music) \
 \
	/*expressionism*/ \
	DO(expressionist_literature) \
	DO(expressionist_art) \
	DO(expressionist_music) \
 \
	/*mass_culture_n_the_avant_garde*/ \
	DO(avantgarde_literature) \
	DO(avantgarde_art) \
	DO(avantgarde_music) \
	DO(surrealism) \
	DO(modern_design) \
	DO(jazz) \
	DO(socialist_realism) \
 \
	/*late_enlightenment_philosophy*/ \
	/*idealism*/ \
	DO(neokantian_idealism) \
	DO(hegelian_idealism) \
	DO(proto_existentialism) \
 \
	/*empiricism*/ \
	DO(pragmatism) \
	DO(i_empiricism) \
 \
	/*analytic_philosophy*/ \
	DO(i_analytic_philosophy) \
	DO(modern_logics) \
	DO(logical_positivism) \
	DO(philosophy_of_language) \
 \
	/*phenomenology_n_hermeneutic*/ \
	DO(phenomenology) \
	DO(hermeneutics) \
 \
	/*anti__rationalism*/ \
	DO(existentialism) \
	DO(nihilism_and_the_will) \
	DO(futurism) \
	DO(quantum_theory) \
 \
	/*malthusian_thought*/ \
	/*positivism*/ \
	/*functionalism*/ \
	/*biologism*/ \
	DO(darwinism) \
	DO(social_darwinism) \
 \
	/*social_science*/ \
	DO(sociology) \
	DO(social_psychology) \
	DO(political_science) \
	DO(social_anthropology) \
 \
	/*social_alienation*/ \
	DO(the_revolt_of_the_masses) \
	DO(the_decline_of_the_west_and_the_lost_generation) \
 \
	/*enlightenment_thought*/ \
	/*ideological_thought*/ \
	DO(authoritarianism) \
	DO(hierarchical_order) \
	DO(traditionalism) \
	DO(political_religion) \
	DO(paternalism) \
	DO(constitutionalism) \
	DO(individualism) \
	DO(rationalism) \
	DO(secularization) \
	DO(social_conscience) \
	DO(egalitarianism) \
	DO(collectivism) \
	DO(determinism) \
	DO(atheism) \
	DO(social_justice) \
 \
	/*state_n_government*/ \
	DO(populism_vs_establishment) \
	DO(clericalism_vs_anticlericalism) \
	DO(bureaucracy_vs_nepotism) \
	DO(meritocracy_vs_aristocracy) \
 \
	/*nationalism_n_imperialism*/ \
	DO(national_fraternity) \
	DO(biased_multiculturalism) \
	DO(manifest_destiny) \
	DO(expansionism) \
	DO(mission_to_civilize) \
	DO(separatism) \
 \
	/*revolution_n_counterrevolution*/ \
	DO(dogma_of_violence) \
	DO(dogma_of_mass_action) \
	DO(terrorism) \
	DO(secret_police) \
	DO(womens_suffrage_movement) \
	DO(rhetoric_of_hate) \
	DO(nostalgic_romanticism) \
	DO(anti_egalitarianism) \
	DO(vangaurd_party) \
 \
	/*mass_politics*/ \
	DO(the_doctrine_of_fascism) \
	DO(paramilitary_groups) \
	DO(the_police_state) \
 \
	/*introspectionism*/ \
	/*associationism*/ \
	/*phenominalism*/ \
	/*experimental_psychology*/ \
	/*psychoanalysis*/ \
	/*behaviorism*/ \
	DO(mass_advertising) \
	DO(national_radio_networks) \
	DO(the_talkies) \
 \
 \
	/*INDUSTRY INVENTIONS */ \
 \
 \
	/*water_wheel_power*/ \
	/*practical_steam_engine*/ \
	/*high_and_low_pressure_steam_engines*/ \
	/*steam_turbines*/ \
	/*combustion_engine*/ \
	DO(daimlers_automobile) \
	DO(wright_n_langleys_aeroplanes) \
	DO(tractors) \
 \
	/*electrical_power_generation*/ \
	DO(direct_current) \
	DO(alternating_current) \
	DO(polyphase_system) \
 \
	/*publishing_industry*/ \
	/*mechanical_production*/ \
	DO(sharp_n_roberts_power_loom) \
	DO(jacquard_power_loom) \
	DO(northrop_power_loom) \
	DO(mechanical_saw) \
	DO(mechanical_precision_saw) \
	DO(hussey_n_mccormicks_reaping_machine) \
	DO(pitts_threshing_machine) \
	DO(mechanized_slaughtering_block) \
 \
	/*interchangeable_parts*/ \
	DO(precision_work) \
	DO(machine_tools) \
	DO(whitney_n_colts_standardized_small_arms) \
	DO(food_packing_system) \
	DO(integral_distillery_process) \
	DO(jiggs) \
 \
	/*semi_automatization*/ \
	DO(electric_machinery) \
	DO(oil_pumping_machinery) \
	DO(drying_techniques) \
 \
	/*assembly_line*/ \
	DO(fordism) \
	DO(industrialized_armament_industry) \
	DO(refrigerator_technology) \
 \
	/*shift_work*/ \
 \
	/*Mechanized_mining*/ \
	DO(ammunition_production) \
	DO(small_arms_production) \
	DO(explosives_production) \
	DO(artillery_production) \
 \
	/*clean_coal*/ \
	DO(pit_coal) \
	DO(coke) \
 \
	/*cheap_iron*/ \
	DO(advanced_ore_roasting_processes) \
	DO(advanced_ore_grinding_processes) \
	DO(advanced_ore_smelting_processes) \
	DO(bessemer_steel) \
 \
	/*cheap_steel*/ \
	DO(martin_steel) \
	DO(thomas_steel) \
 \
	/*advanced_metallurgy*/ \
	DO(electric_rolling_techniques) \
	DO(steel_alloys) \
 \
	/*electric_furnace*/ \
	DO(stainless_steel) \
 \
	/*experimental_railroad*/ \
	/*early_railroad*/ \
	/*iron_railroad*/ \
	/*steel_railroad*/ \
	/*integral_rail_system*/ \
	/*limited_access_road*/ \
 \
	/*basic_chemistry*/ \
	/*medicine*/ \
	DO(combat_medicine) \
	DO(aerial_bacteria_and_antiseptic_principle) \
	DO(vaccination) \
	DO(chemotherapy) \
	DO(genetics_heredity) \
	DO(clinical_thermometers_and_binaural_stethoscopes) \
	DO(prophylaxis_against_malaria) \
	DO(pressure_chambers_for_thorax_surgery) \
 \
	/*inorganic_chemistry*/ \
	DO(artificial_fertilizers) \
	DO(nitroglycerin) \
	DO(chemical_wood_pulp_process) \
	DO(rubber_vulcanization) \
	DO(catalytic_fermentative_process) \
 \
	/*organic_chemistry*/ \
	DO(synthetic_dye) \
	DO(gaslights) \
	DO(carbon_soil_enrichment) \
	DO(cracking) \
 \
	/*electricity*/ \
	DO(x_rays) \
	DO(edison_light_bulb) \
	DO(bell_n_greys_telephones) \
	DO(electrical_lighting) \
 \
	/*synthetic_polymers*/ \
	DO(rayon) \
	DO(bakelite) \
 \
 \
	/*NAVY INVENTIONS*/ \
 \
	/*post_nelsonian_thought*/ \
	/*battleship_column_doctrine*/ \
	DO(long_range_fire_tactic) \
	DO(speedy_maneuvering_tactic) \
 \
	/*raider_group_doctrine*/ \
	DO(sealane_hunters) \
	DO(fast_ship_killer) \
	DO(big_ship_competitor) \
	DO(standard_raider_group_doctrine) \
 \
	/*blue_and_brown_water_schools*/ \
	DO(blue_water_school) \
	DO(brown_water_school) \
 \
	/*high_sea_battle_fleet*/ \
	DO(offensive_attitude) \
	DO(defensive_attitude) \
 \
	/*modern_naval_doctrine*/ \
	DO(decisive_firepower_doctrine) \
	DO(naval_firepower_doctrine) \
 \
	/*clipper_design*/ \
	DO(building_station_shipyards) \
 \
	/*steamers*/ \
	DO(mechanized_fishing_vessels) \
	DO(steamer_automatic_construction_plants) \
	DO(steamer_transports) \
	DO(commerce_raiders) \
 \
	/*iron_steamers*/ \
	DO(ironclads) \
	DO(monitors) \
 \
	/*steel_steamers*/ \
	DO(protected_cruisers) \
	DO(pre_dreadnought) \
 \
	/*steam_turbine_ships*/ \
	DO(dreadnoughts) \
 \
	/*oil_driven_ships*/ \
	DO(advanced_cruisers) \
	DO(modern_cruisers) \
	DO(advanced_battleships) \
	DO(modern_battleships) \
 \
	/*naval_design_bureaus*/ \
	/*fire_control_systems*/ \
	DO(optical_rangefinders) \
	DO(gyrostabilized_fire_control) \
 \
	/*weapon_platforms*/ \
	DO(armour_piercing_projectiles) \
	DO(armour_piercing_exploding_projectiles) \
	DO(armoured_turrets) \
 \
	/*main_armament*/ \
	DO(bomb_guns) \
	DO(rifled_guns) \
	DO(quick_firing_guns) \
	DO(armoured_citadels) \
	DO(heavy_caliber_guns) \
 \
	/*advanced_naval_design*/ \
	DO(on_board_recon_aircraft) \
	DO(gun_hydraulics) \
	DO(heavy_armour_plates) \
	DO(large_hull_form) \
 \
	/*modern_naval_design*/ \
	DO(wireless) \
	DO(advanced_fire_control) \
	DO(fifteen_inch_main_armament) \
 \
	/*alphabetic_flag_signaling*/ \
	/*naval_plans*/ \
	DO(night_training) \
	DO(enemy_plotting_cards) \
	DO(target_hit_profile_analysis) \
 \
	/*naval_statistics*/ \
	DO(hit_evaluation_reports) \
	DO(empirical_interdiction_reports) \
	DO(naval_strike_success_criterium) \
 \
	/*naval_logistics*/ \
	DO(underway_refueling) \
	DO(ammunition_conservation_system) \
	DO(radio_telegraphy) \
 \
	/*naval_directionism*/ \
	DO(political_lobbying) \
	DO(military_political_decision_making) \
	DO(local_military_government) \
 \
	/*naval_integration*/ \
	DO(war_gamming) \
	DO(combined_arms_planning) \
 \
	/*the_command_principle*/ \
	/*naval_professionalism*/ \
	DO(academic_training) \
	DO(combat_station_training) \
	DO(societal_status) \
 \
	/*naval_decision_making*/ \
	DO(independent_command) \
	DO(continuous_issuing_of_orders) \
	DO(delegated_power) \
 \
	/*naval_risk_management*/ \
	DO(wargames) \
	DO(alternative_plans) \
	DO(n_saddle_orders) \
 \
	/*naval_nco_training*/ \
	DO(literate_ncos) \
	DO(nco_combat_station_training) \
	DO(nco_tactical_training) \
 \
 	/*modern_naval_training*/ \
	DO(amphibious_warfare_training) \
	DO(naval_exercises)



#define MAKE_UNION(VAR) VAR,
enum inventionType {
    INVENTION_UNION(MAKE_UNION)
};


#define MAKE_STRINGS(VAR) #VAR,
const char* const inventionNames[naval_exercises+1] = {
    INVENTION_UNION(MAKE_STRINGS)
};


enum inventionStatus
{
	illegal,
	possible,
	active
};



#endif	// V2INVENTIONS_H_
