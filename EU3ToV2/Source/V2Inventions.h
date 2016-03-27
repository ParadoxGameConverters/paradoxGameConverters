#ifndef V2INVENTIONS_H_
#define V2INVENTIONS_H_



#include <string>
using namespace std;



#define VANILLA_INVENTION_UNION(DO) \
	/*ARMY_INVENTIONS*/ \
 \
	/*post_napoleonic_thought*/ \
	/*strategic_mobility*/ \
	DO(VANILLA_, field_fortifications) \
	DO(VANILLA_, railroad_transport_groups) \
	DO(VANILLA_, signal_detachments) \
 \
	/*point_defense_system*/ \
	DO(VANILLA_, flexible_firepower) \
	DO(VANILLA_, synchronized_heavy_support) \
	DO(VANILLA_, staff_command_support) \
 \
	/*deep_defense_system*/ \
	DO(VANILLA_, trench_systems) \
	DO(VANILLA_, gas_attack_capability) \
	DO(VANILLA_, gas_defense_capability) \
	DO(VANILLA_, elastic_defense_system) \
	DO(VANILLA_, flexible_counterattacks) \
 \
	/*infiltration*/ \
	DO(VANILLA_, low_echelon_fire_support) \
	DO(VANILLA_, stormtrooper_training) \
	DO(VANILLA_, tank_experiments) \
	DO(VANILLA_, tank_exploits) \
 \
	/*modern_army_doctrine*/ \
	DO(VANILLA_, firepower_vs_mobility) \
	DO(VANILLA_, grand_battle_plan_vs_superior_firepower) \
	DO(VANILLA_, human_wave_vs_spearhead) \
 \
	/*flintlock_rifles*/ \
	/*muzzle_loaded_rifles*/ \
	/*breech_loaded_rifles*/ \
	/*machine_guns*/ \
	/*bolt_action_rifles*/ \
	/*modern_divisional_structure*/ \
	DO(VANILLA_, interwar_infantry) \
	DO(VANILLA_, interwar_cavalry) \
	DO(VANILLA_, rear_area_dumps) \
 \
	/*bronze_muzzle_loaded_artillery*/ \
	/*iron_muzzle_loaded_artillery*/ \
	/*iron_breech_loaded_artillery*/ \
	/*steel_breech_loaded_artillery*/ \
	/*indirect_artillery_fire*/ \
	/*heavy_armament*/ \
	DO(VANILLA_, tankette) \
	DO(VANILLA_, light_tank) \
	DO(VANILLA_, armoured_cars) \
	DO(VANILLA_, anti_aircraft_artillery) \
	DO(VANILLA_, improved_anti_aircraft_artillery) \
	DO(VANILLA_, light_artillery) \
	DO(VANILLA_, medium_artillery) \
	DO(VANILLA_, heavy_artillery) \
 \
	/*military_staff_system*/ \
	/*military_plans*/ \
	DO(VANILLA_, telegraph_coordination) \
	DO(VANILLA_, mobilization_time_tables) \
	DO(VANILLA_, personnel_category_tables) \
 \
	/*military_statistics*/ \
	DO(VANILLA_, ammunition_area_effect_calculations) \
	DO(VANILLA_, empirical_combat_efficiency_matrix) \
	DO(VANILLA_, combat_loss_evaluation_schemata) \
 \
	/*military_logistics*/ \
	DO(VANILLA_, pre_planned_depot_systems) \
	DO(VANILLA_, logistical_calculations) \
	DO(VANILLA_, replenishment_transport_organisation) \
 \
	/*military_directionism*/ \
	DO(VANILLA_, army_political_lobbying) \
	DO(VANILLA_, army_military_political_decision_making) \
	DO(VANILLA_, army_local_military_government) \
 \
	/*aeronautics*/ \
	DO(VANILLA_, early_interwar_bombers) \
	DO(VANILLA_, late_interwar_bombers) \
	DO(VANILLA_, early_interwar_fighters) \
	DO(VANILLA_, late_interwar_fighters) \
	DO(VANILLA_, t_air_superiority) \
	DO(VANILLA_, flying_circus_doctrine) \
	DO(VANILLA_, force_substitution_doctrine) \
	DO(VANILLA_, battlefield_destruction_doctrine) \
	DO(VANILLA_, operational_destruction_doctrine) \
	DO(VANILLA_, strategic_destruction_doctrine) \
 \
	/*army_command_principle*/ \
	/*army_professionalism*/ \
	DO(VANILLA_, army_academic_training) \
	DO(VANILLA_, field_training) \
	DO(VANILLA_, army_societal_status) \
 \
	/*army_decision_making*/ \
	DO(VANILLA_, army_independent_command) \
	DO(VANILLA_, army_continuous_issuing_of_orders) \
	DO(VANILLA_, army_delegated_power) \
 \
	/*army_risk_management*/ \
	DO(VANILLA_, army_wargames) \
	DO(VANILLA_, army_alternative_plans) \
	DO(VANILLA_, saddle_orders) \
 \
	/*army_nco_training*/ \
	DO(VANILLA_, army_literate_ncos) \
	DO(VANILLA_, nco_field_training) \
	DO(VANILLA_, army_nco_tactical_training) \
 \
	/*great_war_experience*/ \
 \
 \
	/*COMMERCE_INVENTIONS*/ \
 \
	/*private_banks*/ \
	/*stock_exchange*/ \
	DO(VANILLA_, multitude_of_financial_instruments) \
	DO(VANILLA_, insurance_companies) \
	DO(VANILLA_, regulated_buying_and_selling_of_stocks) \
 \
	/*business_banks*/ \
	DO(VANILLA_, liquidation_of_assets) \
	DO(VANILLA_, capital_for_investors) \
	DO(VANILLA_, distribution_channels) \
 \
	/*investment_banks*/ \
	DO(VANILLA_, industrial_rnd_investments) \
	DO(VANILLA_, industrial_production_investments) \
	DO(VANILLA_, industrial_distribution_investments) \
 \
	/*bank_inspection_board*/ \
	DO(VANILLA_, private_bank_monitoring) \
	DO(VANILLA_, market_behaviour_monitoring) \
 \
	/*mutual_funds*/ \
	DO(VANILLA_, buying_on_margin) \
	 \
	/*no_standard*/ \
	/*ad_hoc_money_bill_printing*/ \
	DO(VANILLA_, silver_standard) \
	DO(VANILLA_, decimal_monetary_system) \
 \
	/*private_bank_money_bill_printing*/ \
	DO(VANILLA_, double_standard) \
	DO(VANILLA_, fixed_intramonetary_exchange_rates) \
 \
	/*central_bank_money_bill_printing*/ \
	DO(VANILLA_, gold_standard) \
	DO(VANILLA_, international_monetary_exchange_rates) \
 \
	/*modern_central_bank_system*/ \
	DO(VANILLA_, state_bond_monetary_mechanism) \
	DO(VANILLA_, interest_inflation_weapon) \
 \
	/*market_determined_exchange_rates*/ \
	DO(VANILLA_, hyperinflation) \
 \
	/*eary_classical_theory*/ \
	DO(VANILLA_, john_ramsay_mcculloch) \
	DO(VANILLA_, nassau_william_sr) \
	DO(VANILLA_, james_mill) \
 \
	/*late_classical_theory*/ \
	DO(VANILLA_, john_elliot_cairnes) \
	DO(VANILLA_, robert_torrens) \
	DO(VANILLA_, john_stuart_mill) \
 \
	/*collectivist_theory*/ \
	DO(VANILLA_, pierre_joseph_proudhon) \
	DO(VANILLA_, karl_marx) \
	DO(VANILLA_, friedrich_engels) \
	DO(VANILLA_, eduard_bernstein) \
	DO(VANILLA_, sidney_webb) \
	DO(VANILLA_, karl_kautsky) \
	DO(VANILLA_, jean_jaurès) \
 \
	/*the_historical_theory*/ \
	DO(VANILLA_, friedrich_list) \
	DO(VANILLA_, wilhelm_roscher) \
	DO(VANILLA_, bruno_hildebrand) \
	DO(VANILLA_, karl_knies) \
	DO(VANILLA_, gustav_von_schmoller) \
	DO(VANILLA_, adolph_wagner) \
	DO(VANILLA_, william_cunningham) \
 \
	/*neoclassical_theory*/ \
	DO(VANILLA_, johann_heinrich_von_thünen) \
	DO(VANILLA_, léon_walras) \
	DO(VANILLA_, stanley_jevons) \
	DO(VANILLA_, carl_menger) \
	DO(VANILLA_, thorstein_veblen) \
	DO(VANILLA_, alfred_marshall) \
	DO(VANILLA_, augustin_cournot) \
	DO(VANILLA_, jules_dupuit) \
	DO(VANILLA_, vilfredo_pareto) \
	DO(VANILLA_, knut_wicksell) \
	DO(VANILLA_, irving_fischer) \
	DO(VANILLA_, joseph_schumpeter) \
	DO(VANILLA_, eugen_von_böhm_bawerk) \
 \
	/*keynesian_economics*/ \
	DO(VANILLA_, john_maynard_keynes) \
	DO(VANILLA_, richard_kahn) \
	DO(VANILLA_, ludwig_von_mises) \
	DO(VANILLA_, friedrich_hayek) \
 \
	/*freedom_of_trade*/ \
	/*market_structure*/ \
	DO(VANILLA_, polypoly_structure) \
	DO(VANILLA_, oligopoly_structure) \
	DO(VANILLA_, monopoly_structure) \
 \
	/*business_regulations*/ \
	DO(VANILLA_, limited_partnership_company) \
	DO(VANILLA_, joint_stock_company) \
	DO(VANILLA_, patent) \
	DO(VANILLA_, trademark) \
 \
	/*market_regulations*/ \
	DO(VANILLA_, anti_trust_laws) \
	DO(VANILLA_, financial_regulations) \
	DO(VANILLA_, banking_regulations) \
 \
	/*economic_responsibility*/ \
	DO(VANILLA_, working_board_of_directors) \
	DO(VANILLA_, bankruptcy_proceedings) \
	DO(VANILLA_, annual_report_proceedings) \
 \
	/*government_interventionism*/ \
	DO(VANILLA_, priming_the_pump_) \
	DO(VANILLA_, mixed_economy) \
	DO(VANILLA_, centralized_economic_planning) \
	DO(VANILLA_, autarky) \
	DO(VANILLA_, import_substitution_industrialization) \
 \
	/*guild_based_production*/ \
	/*organized_factories*/ \
	DO(VANILLA_, work_classification) \
	DO(VANILLA_, work_specialization) \
	DO(VANILLA_, work_control_and_hierarchy) \
 \
	/*scientific_management*/ \
	DO(VANILLA_, sequence_planning) \
	DO(VANILLA_, inventory_planning) \
	DO(VANILLA_, quality_assurance_procedure) \
 \
	/*time_saving_measures*/ \
	DO(VANILLA_, time_studies) \
	DO(VANILLA_, time_clock) \
	DO(VANILLA_, lighting) \
 \
	/*management_strategy*/ \
	DO(VANILLA_, old_style_family_business) \
	DO(VANILLA_, anynomous_investor_business) \
	DO(VANILLA_, marketized_smallholder_business) \
 \
	/*organizational_development*/ \
	DO(VANILLA_, i_scientific_management) \
	DO(VANILLA_, human_resource_development) \
 \
 \
	/*CULTURE_INVENTIONS*/ \
	/*classicism_n_early_romanticism*/ \
	/*romanticism*/ \
	DO(VANILLA_, romanticist_literature) \
	DO(VANILLA_, romanticist_art) \
	DO(VANILLA_, romanticist_music) \
 \
	/*realism*/ \
	DO(VANILLA_, realist_literature) \
	DO(VANILLA_, realist_art) \
	DO(VANILLA_, realist_music) \
 \
	/*impressionism*/ \
	DO(VANILLA_, impressionist_literature) \
	DO(VANILLA_, impressionist_art) \
	DO(VANILLA_, impressionist_music) \
 \
	/*expressionism*/ \
	DO(VANILLA_, expressionist_literature) \
	DO(VANILLA_, expressionist_art) \
	DO(VANILLA_, expressionist_music) \
 \
	/*mass_culture_n_the_avant_garde*/ \
	DO(VANILLA_, avantgarde_literature) \
	DO(VANILLA_, avantgarde_art) \
	DO(VANILLA_, avantgarde_music) \
	DO(VANILLA_, surrealism) \
	DO(VANILLA_, modern_design) \
	DO(VANILLA_, jazz) \
	DO(VANILLA_, socialist_realism) \
 \
	/*late_enlightenment_philosophy*/ \
	/*idealism*/ \
	DO(VANILLA_, neokantian_idealism) \
	DO(VANILLA_, hegelian_idealism) \
	DO(VANILLA_, proto_existentialism) \
 \
	/*empiricism*/ \
	DO(VANILLA_, pragmatism) \
	DO(VANILLA_, i_empiricism) \
 \
	/*analytic_philosophy*/ \
	DO(VANILLA_, i_analytic_philosophy) \
	DO(VANILLA_, modern_logics) \
	DO(VANILLA_, logical_positivism) \
	DO(VANILLA_, philosophy_of_language) \
 \
	/*phenomenology_n_hermeneutic*/ \
	DO(VANILLA_, phenomenology) \
	DO(VANILLA_, hermeneutics) \
 \
	/*anti__rationalism*/ \
	DO(VANILLA_, existentialism) \
	DO(VANILLA_, nihilism_and_the_will) \
	DO(VANILLA_, futurism) \
	DO(VANILLA_, quantum_theory) \
 \
	/*malthusian_thought*/ \
	/*positivism*/ \
	/*functionalism*/ \
	/*biologism*/ \
	DO(VANILLA_, darwinism) \
	DO(VANILLA_, social_darwinism) \
 \
	/*social_science*/ \
	DO(VANILLA_, sociology) \
	DO(VANILLA_, social_psychology) \
	DO(VANILLA_, political_science) \
	DO(VANILLA_, social_anthropology) \
 \
	/*social_alienation*/ \
	DO(VANILLA_, the_revolt_of_the_masses) \
	DO(VANILLA_, the_decline_of_the_west_and_the_lost_generation) \
 \
	/*enlightenment_thought*/ \
	/*ideological_thought*/ \
	DO(VANILLA_, authoritarianism) \
	DO(VANILLA_, hierarchical_order) \
	DO(VANILLA_, traditionalism) \
	DO(VANILLA_, political_religion) \
	DO(VANILLA_, paternalism) \
	DO(VANILLA_, constitutionalism) \
	DO(VANILLA_, individualism) \
	DO(VANILLA_, rationalism) \
	DO(VANILLA_, secularization) \
	DO(VANILLA_, social_conscience) \
	DO(VANILLA_, egalitarianism) \
	DO(VANILLA_, collectivism) \
	DO(VANILLA_, determinism) \
	DO(VANILLA_, atheism) \
	DO(VANILLA_, social_justice) \
 \
	/*state_n_government*/ \
	DO(VANILLA_, populism_vs_establishment) \
	DO(VANILLA_, clericalism_vs_anticlericalism) \
	DO(VANILLA_, bureaucracy_vs_nepotism) \
	DO(VANILLA_, meritocracy_vs_aristocracy) \
 \
	/*nationalism_n_imperialism*/ \
	DO(VANILLA_, national_fraternity) \
	DO(VANILLA_, biased_multiculturalism) \
	DO(VANILLA_, manifest_destiny) \
	DO(VANILLA_, expansionism) \
	DO(VANILLA_, mission_to_civilize) \
	DO(VANILLA_, separatism) \
 \
	/*revolution_n_counterrevolution*/ \
	DO(VANILLA_, dogma_of_violence) \
	DO(VANILLA_, dogma_of_mass_action) \
	DO(VANILLA_, terrorism) \
	DO(VANILLA_, secret_police) \
	DO(VANILLA_, womens_suffrage_movement) \
	DO(VANILLA_, rhetoric_of_hate) \
	DO(VANILLA_, nostalgic_romanticism) \
	DO(VANILLA_, anti_egalitarianism) \
	DO(VANILLA_, vangaurd_party) \
 \
	/*mass_politics*/ \
	DO(VANILLA_, the_doctrine_of_fascism) \
	DO(VANILLA_, paramilitary_groups) \
	DO(VANILLA_, the_police_state) \
 \
	/*introspectionism*/ \
	/*associationism*/ \
	/*phenominalism*/ \
	/*experimental_psychology*/ \
	/*psychoanalysis*/ \
	/*behaviorism*/ \
	DO(VANILLA_, mass_advertising) \
	DO(VANILLA_, national_radio_networks) \
	DO(VANILLA_, the_talkies) \
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
	DO(VANILLA_, daimlers_automobile) \
	DO(VANILLA_, wright_n_langleys_aeroplanes) \
	DO(VANILLA_, tractors) \
 \
	/*electrical_power_generation*/ \
	DO(VANILLA_, direct_current) \
	DO(VANILLA_, alternating_current) \
	DO(VANILLA_, polyphase_system) \
 \
	/*publishing_industry*/ \
	/*mechanical_production*/ \
	DO(VANILLA_, sharp_n_roberts_power_loom) \
	DO(VANILLA_, jacquard_power_loom) \
	DO(VANILLA_, northrop_power_loom) \
	DO(VANILLA_, mechanical_saw) \
	DO(VANILLA_, mechanical_precision_saw) \
	DO(VANILLA_, hussey_n_mccormicks_reaping_machine) \
	DO(VANILLA_, pitts_threshing_machine) \
	DO(VANILLA_, mechanized_slaughtering_block) \
 \
	/*interchangeable_parts*/ \
	DO(VANILLA_, precision_work) \
	DO(VANILLA_, machine_tools) \
	DO(VANILLA_, whitney_n_colts_standardized_small_arms) \
	DO(VANILLA_, food_packing_system) \
	DO(VANILLA_, integral_distillery_process) \
	DO(VANILLA_, jiggs) \
 \
	/*semi_automatization*/ \
	DO(VANILLA_, electric_machinery) \
	DO(VANILLA_, oil_pumping_machinery) \
	DO(VANILLA_, drying_techniques) \
 \
	/*assembly_line*/ \
	DO(VANILLA_, fordism) \
	DO(VANILLA_, industrialized_armament_industry) \
	DO(VANILLA_, refrigerator_technology) \
 \
	/*shift_work*/ \
 \
	/*Mechanized_mining*/ \
	DO(VANILLA_, ammunition_production) \
	DO(VANILLA_, small_arms_production) \
	DO(VANILLA_, explosives_production) \
	DO(VANILLA_, artillery_production) \
 \
	/*clean_coal*/ \
	DO(VANILLA_, pit_coal) \
	DO(VANILLA_, coke) \
 \
	/*cheap_iron*/ \
	DO(VANILLA_, advanced_ore_roasting_processes) \
	DO(VANILLA_, advanced_ore_grinding_processes) \
	DO(VANILLA_, advanced_ore_smelting_processes) \
	DO(VANILLA_, bessemer_steel) \
 \
	/*cheap_steel*/ \
	DO(VANILLA_, martin_steel) \
	DO(VANILLA_, thomas_steel) \
 \
	/*advanced_metallurgy*/ \
	DO(VANILLA_, electric_rolling_techniques) \
	DO(VANILLA_, steel_alloys) \
 \
	/*electric_furnace*/ \
	DO(VANILLA_, stainless_steel) \
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
	DO(VANILLA_, combat_medicine) \
	DO(VANILLA_, aerial_bacteria_and_antiseptic_principle) \
	DO(VANILLA_, vaccination) \
	DO(VANILLA_, chemotherapy) \
	DO(VANILLA_, genetics_heredity) \
	DO(VANILLA_, clinical_thermometers_and_binaural_stethoscopes) \
	DO(VANILLA_, prophylaxis_against_malaria) \
	DO(VANILLA_, pressure_chambers_for_thorax_surgery) \
 \
	/*inorganic_chemistry*/ \
	DO(VANILLA_, artificial_fertilizers) \
	DO(VANILLA_, nitroglycerin) \
	DO(VANILLA_, chemical_wood_pulp_process) \
	DO(VANILLA_, rubber_vulcanization) \
	DO(VANILLA_, catalytic_fermentative_process) \
 \
	/*organic_chemistry*/ \
	DO(VANILLA_, synthetic_dye) \
	DO(VANILLA_, gaslights) \
	DO(VANILLA_, carbon_soil_enrichment) \
	DO(VANILLA_, cracking) \
 \
	/*electricity*/ \
	DO(VANILLA_, x_rays) \
	DO(VANILLA_, edison_light_bulb) \
	DO(VANILLA_, bell_n_greys_telephones) \
	DO(VANILLA_, electrical_lighting) \
 \
	/*synthetic_polymers*/ \
	DO(VANILLA_, rayon) \
	DO(VANILLA_, bakelite) \
 \
 \
	/*NAVY INVENTIONS*/ \
 \
	/*post_nelsonian_thought*/ \
	/*battleship_column_doctrine*/ \
	DO(VANILLA_, long_range_fire_tactic) \
	DO(VANILLA_, speedy_maneuvering_tactic) \
 \
	/*raider_group_doctrine*/ \
	DO(VANILLA_, sealane_hunters) \
	DO(VANILLA_, fast_ship_killer) \
	DO(VANILLA_, big_ship_competitor) \
	DO(VANILLA_, standard_raider_group_doctrine) \
 \
	/*blue_and_brown_water_schools*/ \
	DO(VANILLA_, blue_water_school) \
	DO(VANILLA_, brown_water_school) \
 \
	/*high_sea_battle_fleet*/ \
	DO(VANILLA_, offensive_attitude) \
	DO(VANILLA_, defensive_attitude) \
 \
	/*modern_naval_doctrine*/ \
	DO(VANILLA_, decisive_firepower_doctrine) \
	DO(VANILLA_, naval_firepower_doctrine) \
 \
	/*clipper_design*/ \
	DO(VANILLA_, building_station_shipyards) \
 \
	/*steamers*/ \
	DO(VANILLA_, mechanized_fishing_vessels) \
	DO(VANILLA_, steamer_automatic_construction_plants) \
	DO(VANILLA_, steamer_transports) \
	DO(VANILLA_, commerce_raiders) \
 \
	/*iron_steamers*/ \
	DO(VANILLA_, ironclads) \
	DO(VANILLA_, monitors) \
 \
	/*steel_steamers*/ \
	DO(VANILLA_, protected_cruisers) \
	DO(VANILLA_, pre_dreadnought) \
 \
	/*steam_turbine_ships*/ \
	DO(VANILLA_, dreadnoughts) \
 \
	/*oil_driven_ships*/ \
	DO(VANILLA_, advanced_cruisers) \
	DO(VANILLA_, modern_cruisers) \
	DO(VANILLA_, advanced_battleships) \
	DO(VANILLA_, modern_battleships) \
 \
	/*naval_design_bureaus*/ \
	/*fire_control_systems*/ \
	DO(VANILLA_, optical_rangefinders) \
	DO(VANILLA_, gyrostabilized_fire_control) \
 \
	/*weapon_platforms*/ \
	DO(VANILLA_, armour_piercing_projectiles) \
	DO(VANILLA_, armour_piercing_exploding_projectiles) \
	DO(VANILLA_, armoured_turrets) \
 \
	/*main_armament*/ \
	DO(VANILLA_, bomb_guns) \
	DO(VANILLA_, rifled_guns) \
	DO(VANILLA_, quick_firing_guns) \
	DO(VANILLA_, armoured_citadels) \
	DO(VANILLA_, heavy_caliber_guns) \
 \
	/*advanced_naval_design*/ \
	DO(VANILLA_, on_board_recon_aircraft) \
	DO(VANILLA_, gun_hydraulics) \
	DO(VANILLA_, heavy_armour_plates) \
	DO(VANILLA_, large_hull_form) \
 \
	/*modern_naval_design*/ \
	DO(VANILLA_, wireless) \
	DO(VANILLA_, advanced_fire_control) \
	DO(VANILLA_, fifteen_inch_main_armament) \
 \
	/*alphabetic_flag_signaling*/ \
	/*naval_plans*/ \
	DO(VANILLA_, night_training) \
	DO(VANILLA_, enemy_plotting_cards) \
	DO(VANILLA_, target_hit_profile_analysis) \
 \
	/*naval_statistics*/ \
	DO(VANILLA_, hit_evaluation_reports) \
	DO(VANILLA_, empirical_interdiction_reports) \
	DO(VANILLA_, naval_strike_success_criterium) \
 \
	/*naval_logistics*/ \
	DO(VANILLA_, underway_refueling) \
	DO(VANILLA_, ammunition_conservation_system) \
	DO(VANILLA_, radio_telegraphy) \
 \
	/*naval_directionism*/ \
	DO(VANILLA_, political_lobbying) \
	DO(VANILLA_, military_political_decision_making) \
	DO(VANILLA_, local_military_government) \
 \
	/*naval_integration*/ \
	DO(VANILLA_, war_gamming) \
	DO(VANILLA_, combined_arms_planning) \
 \
	/*the_command_principle*/ \
	/*naval_professionalism*/ \
	DO(VANILLA_, academic_training) \
	DO(VANILLA_, combat_station_training) \
	DO(VANILLA_, societal_status) \
 \
	/*naval_decision_making*/ \
	DO(VANILLA_, independent_command) \
	DO(VANILLA_, continuous_issuing_of_orders) \
	DO(VANILLA_, delegated_power) \
 \
	/*naval_risk_management*/ \
	DO(VANILLA_, wargames) \
	DO(VANILLA_, alternative_plans) \
	DO(VANILLA_, n_saddle_orders) \
 \
	/*naval_nco_training*/ \
	DO(VANILLA_, literate_ncos) \
	DO(VANILLA_, nco_combat_station_training) \
	DO(VANILLA_, nco_tactical_training) \
 \
 	/*modern_naval_training*/ \
	DO(VANILLA_, amphibious_warfare_training) \
	DO(VANILLA_, naval_exercises)



#define MAKE_UNION(TYPE, VAR) TYPE ## VAR,
enum vanillaInventionType {
    VANILLA_INVENTION_UNION(MAKE_UNION)
};


#define MAKE_STRINGS(TYPE, VAR) #VAR,
const char* const vanillaInventionNames[VANILLA_naval_exercises+1] = {
    VANILLA_INVENTION_UNION(MAKE_STRINGS)
};


#define HOD_INVENTION_UNION(DO) \
	/*ARMY_INVENTIONS*/ \
 \
	/*post_napoleonic_thought*/ \
	DO(HOD_, post_napoleonic_army_doctrine) \
 \
	/*strategic_mobility*/ \
	DO(HOD_, field_fortifications) \
	DO(HOD_, railroad_transport_groups) \
 \
	/*point_defense_system*/ \
	DO(HOD_, signal_detachments) \
	DO(HOD_, flexible_firepower) \
	DO(HOD_, synchronized_heavy_support) \
	DO(HOD_, staff_command_support) \
 \
	/*deep_defense_system*/ \
	DO(HOD_, trench_systems) \
	DO(HOD_, gas_attack_capability) \
	DO(HOD_, gas_defense_capability) \
	DO(HOD_, elastic_defense_system) \
	DO(HOD_, flexible_counterattacks) \
 \
	/*infiltration*/ \
	DO(HOD_, low_echelon_fire_support) \
	DO(HOD_, stormtrooper_training) \
	DO(HOD_, tank_experiments) \
	DO(HOD_, tank_exploits) \
 \
	/*modern_army_doctrine*/ \
	DO(HOD_, firepower_vs_mobility) \
	DO(HOD_, grand_battle_plan_vs_superior_firepower) \
	DO(HOD_, human_wave_vs_spearhead) \
 \
	/*flintlock_rifles*/ \
	DO(HOD_, flintlock_rifle_armament) \
 \
	/*muzzle_loaded_rifles*/ \
	DO(HOD_, muzzle_loaded_rifle_armament) \
 \
	/*breech_loaded_rifles*/ \
	DO(HOD_, breech_loaded_rifle_armament) \
	DO(HOD_, colonial_negotiations) \
 \
	/*machine_guns*/ \
	DO(HOD_, machine_gun_armament) \
 \
	/*bolt_action_rifles*/ \
	DO(HOD_, bolt_action_rifle_armament) \
 \
	/*modern_divisional_structure*/ \
	DO(HOD_, interwar_infantry) \
	DO(HOD_, interwar_cavalry) \
	DO(HOD_, rear_area_dumps) \
 \
	/*iron_breech_loaded_artillery*/ \
	/*indirect_artillery_fire*/ \
	/*heavy_armament*/ \
	DO(HOD_, tankette) \
	DO(HOD_, light_tank) \
	DO(HOD_, armoured_cars) \
	DO(HOD_, anti_aircraft_artillery) \
	DO(HOD_, improved_anti_aircraft_artillery) \
	DO(HOD_, light_artillery) \
	DO(HOD_, medium_artillery) \
	DO(HOD_, heavy_artillery) \
 \
	/*military_staff_system*/ \
	DO(HOD_, cuirassier_activation) \
	DO(HOD_, dragoon_activation) \
	DO(HOD_, hussar_activation) \
 \
	/*military_plans*/ \
	DO(HOD_, telegraph_coordination) \
	DO(HOD_, mobilization_time_tables) \
	DO(HOD_, personnel_category_tables) \
 \
	/*military_statistics*/ \
	DO(HOD_, ammunition_area_effect_calculations) \
	DO(HOD_, empirical_combat_efficiency_matrix) \
	DO(HOD_, combat_loss_evaluation_schemata) \
 \
	/*military_logistics*/ \
	DO(HOD_, pre_planned_depot_systems) \
	DO(HOD_, logistical_calculations) \
	DO(HOD_, replenishment_transport_organisation) \
 \
	/*military_directionism*/ \
	DO(HOD_, army_political_lobbying) \
	DO(HOD_, army_military_political_decision_making) \
	DO(HOD_, army_local_military_government) \
 \
	/*aeronautics*/ \
	DO(HOD_, military_aviation) \
	DO(HOD_, early_interwar_bombers) \
	DO(HOD_, late_interwar_bombers) \
	DO(HOD_, early_interwar_fighters) \
	DO(HOD_, late_interwar_fighters) \
	DO(HOD_, t_air_superiority) \
	DO(HOD_, flying_circus_doctrine) \
	DO(HOD_, force_substitution_doctrine) \
	DO(HOD_, battlefield_destruction_doctrine) \
	DO(HOD_, operational_destruction_doctrine) \
	DO(HOD_, strategic_destruction_doctrine) \
 \
	/*army_professionalism*/ \
	DO(HOD_, army_academic_training) \
	DO(HOD_, field_training) \
	DO(HOD_, army_societal_status) \
 \
	/*army_decision_making*/ \
	DO(HOD_, army_independent_command) \
	DO(HOD_, army_continuous_issuing_of_orders) \
	DO(HOD_, army_delegated_power) \
 \
	/*army_risk_management*/ \
	DO(HOD_, army_wargames) \
	DO(HOD_, army_alternative_plans) \
	DO(HOD_, saddle_orders) \
 \
	/*army_nco_training*/ \
	DO(HOD_, army_literate_ncos) \
	DO(HOD_, nco_field_training) \
	DO(HOD_, army_nco_tactical_training) \
 \
 \
	/*COMMERCE INVENTIONS*/ \
 \
	/*stock_exchange*/ \
	DO(HOD_, multitude_of_financial_instruments) \
	DO(HOD_, insurance_companies) \
	DO(HOD_, regulated_buying_and_selling_of_stocks) \
 \
	/*business_banks*/ \
	DO(HOD_, liquidation_of_assets) \
	DO(HOD_, capital_for_investors) \
	DO(HOD_, distribution_channels) \
 \
	/*investment_banks*/ \
	DO(HOD_, industrial_rnd_investments) \
	DO(HOD_, industrial_production_investments) \
	DO(HOD_, industrial_distribution_investments) \
 \
	/*bank_inspection_board*/ \
	DO(HOD_, private_bank_monitoring) \
	DO(HOD_, market_behaviour_monitoring) \
 \
	/*mutual_funds*/ \
	DO(HOD_, buying_on_margin) \
 \
	/*ad_hoc_money_bill_printing*/ \
	DO(HOD_, silver_standard) \
	DO(HOD_, decimal_monetary_system) \
 \
	/*private_bank_money_bill_printing*/ \
	DO(HOD_, double_standard) \
	DO(HOD_, fixed_intramonetary_exchange_rates) \
 \
	/*central_bank_money_bill_printing*/ \
	DO(HOD_, gold_standard) \
	DO(HOD_, international_monetary_exchange_rates) \
 \
	/*modern_central_bank_system*/ \
	DO(HOD_, state_bond_monetary_mechanism) \
	DO(HOD_, interest_inflation_weapon) \
 \
	/*market_determined_exchange_rates*/ \
	DO(HOD_, hyperinflation) \
 \
	/*eary_classical_theory*/ \
	DO(HOD_, john_ramsay_mcculloch) \
	DO(HOD_, nassau_william_sr) \
	DO(HOD_, james_mill) \
 \
	/*late_classical_theory*/ \
	DO(HOD_, john_elliot_cairnes) \
	DO(HOD_, robert_torrens) \
	DO(HOD_, john_stuart_mill) \
 \
	/*collectivist_theory*/ \
	DO(HOD_, pierre_joseph_proudhon) \
	DO(HOD_, karl_marx) \
	DO(HOD_, friedrich_engels) \
	DO(HOD_, eduard_bernstein) \
	DO(HOD_, sidney_webb) \
	DO(HOD_, karl_kautsky) \
	DO(HOD_, jean_jaurès) \
 \
	/*the_historical_theory*/ \
	DO(HOD_, friedrich_list) \
	DO(HOD_, wilhelm_roscher) \
	DO(HOD_, bruno_hildebrand) \
	DO(HOD_, karl_knies) \
	DO(HOD_, gustav_von_schmoller) \
	DO(HOD_, adolph_wagner) \
	DO(HOD_, william_cunningham) \
 \
	/*neoclassical_theory*/ \
	DO(HOD_, johann_heinrich_von_thünen) \
	DO(HOD_, léon_walras) \
	DO(HOD_, stanley_jevons) \
	DO(HOD_, carl_menger) \
	DO(HOD_, thorstein_veblen) \
	DO(HOD_, alfred_marshall) \
	DO(HOD_, augustin_cournot) \
	DO(HOD_, jules_dupuit) \
	DO(HOD_, vilfredo_pareto) \
	DO(HOD_, knut_wicksell) \
	DO(HOD_, irving_fischer) \
	DO(HOD_, joseph_schumpeter) \
	DO(HOD_, eugen_von_böhm_bawerk) \
 \
	/*keynesian_economics*/ \
	DO(HOD_, john_maynard_keynes) \
	DO(HOD_, richard_kahn) \
	DO(HOD_, ludwig_von_mises) \
	DO(HOD_, friedrich_hayek) \
 \
	/*market_structure*/ \
	DO(HOD_, polypoly_structure) \
	DO(HOD_, oligopoly_structure) \
	DO(HOD_, monopoly_structure) \
 \
	/*business_regulations*/ \
	DO(HOD_, limited_partnership_company) \
	DO(HOD_, joint_stock_company) \
	DO(HOD_, patent) \
	DO(HOD_, trademark) \
 \
	/*market_regulations*/ \
	DO(HOD_, anti_trust_laws) \
	DO(HOD_, financial_regulations) \
	DO(HOD_, banking_regulations) \
 \
	/*economic_responsibility*/ \
	DO(HOD_, working_board_of_directors) \
	DO(HOD_, bankruptcy_proceedings) \
	DO(HOD_, annual_report_proceedings) \
 \
	/*government_interventionism*/ \
	DO(HOD_, priming_the_pump_) \
	DO(HOD_, mixed_economy) \
	DO(HOD_, centralized_economic_planning) \
	DO(HOD_, autarky) \
	DO(HOD_, import_substitution_industrialization) \
 \
	/*organized_factories*/ \
	DO(HOD_, work_classification) \
	DO(HOD_, work_specialization) \
	DO(HOD_, work_control_and_hierarchy) \
 \
	/*scientific_management*/ \
	DO(HOD_, sequence_planning) \
	DO(HOD_, inventory_planning) \
	DO(HOD_, quality_assurance_procedure) \
 \
	/*time_saving_measures*/ \
	DO(HOD_, time_studies) \
	DO(HOD_, time_clock) \
	DO(HOD_, lighting) \
 \
	/*management_strategy*/ \
	DO(HOD_, old_style_family_business) \
	DO(HOD_, anynomous_investor_business) \
	DO(HOD_, marketized_smallholder_business) \
 \
	/*organizational_development*/ \
	DO(HOD_, i_scientific_management) \
	DO(HOD_, human_resource_development) \
 \
 \
	/*CULTURE INVENTIONS*/ \
 \
	/*romanticism*/ \
	DO(HOD_, romanticist_literature) \
	DO(HOD_, romanticist_art) \
	DO(HOD_, romanticist_music) \
 \
	/*realism*/ \
	DO(HOD_, realist_literature) \
	DO(HOD_, realist_art) \
	DO(HOD_, realist_music) \
 \
	/*impressionism*/ \
	DO(HOD_, impressionist_literature) \
	DO(HOD_, impressionist_art) \
	DO(HOD_, impressionist_music) \
 \
	/*expressionism*/ \
	DO(HOD_, expressionist_literature) \
	DO(HOD_, expressionist_art) \
	DO(HOD_, expressionist_music) \
 \
	/*mass_culture_n_the_avant_garde*/ \
	DO(HOD_, avantgarde_literature) \
	DO(HOD_, avantgarde_art) \
	DO(HOD_, avantgarde_music) \
	DO(HOD_, surrealism) \
	DO(HOD_, modern_design) \
	DO(HOD_, jazz) \
	DO(HOD_, socialist_realism) \
 \
	/*idealism*/ \
	DO(HOD_, neokantian_idealism) \
	DO(HOD_, hegelian_idealism) \
	DO(HOD_, proto_existentialism) \
 \
	/*empiricism*/ \
	DO(HOD_, pragmatism) \
	DO(HOD_, i_empiricism) \
 \
	/*analytic_philosophy*/ \
	DO(HOD_, i_analytic_philosophy) \
	DO(HOD_, modern_logics) \
	DO(HOD_, logical_positivism) \
	DO(HOD_, philosophy_of_language) \
 \
	/*phenomenology_n_hermeneutic*/ \
	DO(HOD_, phenomenology) \
	DO(HOD_, hermeneutics) \
 \
	/*anti__rationalism*/ \
	DO(HOD_, existentialism) \
	DO(HOD_, nihilism_and_the_will) \
	DO(HOD_, futurism) \
	DO(HOD_, quantum_theory) \
 \
	/*biologism*/ \
	DO(HOD_, darwinism) \
	DO(HOD_, social_darwinism) \
 \
	/*social_science*/ \
	DO(HOD_, sociology) \
	DO(HOD_, social_psychology) \
	DO(HOD_, political_science) \
	DO(HOD_, social_anthropology) \
 \
	/*social_alienation*/ \
	DO(HOD_, the_revolt_of_the_masses) \
	DO(HOD_, the_decline_of_the_west_and_the_lost_generation) \
 \
	/*the rights of man - called enlightenment_thought in files...*/ \
	DO(HOD_, paternalism) \
	DO(HOD_, constitutionalism) \
	DO(HOD_, atheism) \
	DO(HOD_, egalitarianism) \
	DO(HOD_, rationalism) \
 \
	/*ideological_thought*/ \
	DO(HOD_, authoritarianism) \
	DO(HOD_, hierarchical_order) \
	DO(HOD_, traditionalism) \
	DO(HOD_, political_religion) \
	DO(HOD_, individualism) \
	DO(HOD_, secularization) \
	DO(HOD_, social_conscience) \
	DO(HOD_, collectivism) \
	DO(HOD_, determinism) \
	DO(HOD_, social_justice) \
 \
	/*state_n_government*/ \
	DO(HOD_, populism_vs_establishment) \
	DO(HOD_, clericalism_vs_anticlericalism) \
	DO(HOD_, bureaucracy_vs_nepotism) \
	DO(HOD_, meritocracy_vs_aristocracy) \
 \
	/*nationalism_n_imperialism*/ \
	DO(HOD_, national_fraternity) \
	DO(HOD_, biased_multiculturalism) \
	DO(HOD_, manifest_destiny) \
	DO(HOD_, expansionism) \
	DO(HOD_, mission_to_civilize) \
	DO(HOD_, separatism) \
 \
	/*revolution_n_counterrevolution*/ \
	DO(HOD_, dogma_of_violence) \
	DO(HOD_, dogma_of_mass_action) \
	DO(HOD_, terrorism) \
	DO(HOD_, secret_police) \
	DO(HOD_, womens_suffrage_movement) \
	DO(HOD_, rhetoric_of_hate) \
	DO(HOD_, nostalgic_romanticism) \
	DO(HOD_, anti_egalitarianism) \
	DO(HOD_, vangaurd_party) \
 \
	/*mass_politics*/ \
	DO(HOD_, the_doctrine_of_fascism) \
	DO(HOD_, paramilitary_groups) \
	DO(HOD_, the_police_state) \
 \
	/*behaviorism*/ \
	DO(HOD_, mass_advertising) \
	DO(HOD_, national_radio_networks) \
	DO(HOD_, the_talkies) \
 \
 \
	/*INDUSTRY INVENTIONS*/ \
\
	/*combustion_engine*/ \
	DO(HOD_, daimlers_automobile) \
	DO(HOD_, wright_n_langleys_aeroplanes) \
	DO(HOD_, tractors) \
\
	/*electrical_power_generation*/ \
	DO(HOD_, direct_current) \
	DO(HOD_, alternating_current) \
	DO(HOD_, polyphase_system) \
\
	/*mechanical_production*/ \
	DO(HOD_, sharp_n_roberts_power_loom) \
	DO(HOD_, jacquard_power_loom) \
	DO(HOD_, northrop_power_loom) \
	DO(HOD_, mechanical_saw) \
	DO(HOD_, mechanical_precision_saw) \
	DO(HOD_, hussey_n_mccormicks_reaping_machine) \
	DO(HOD_, pitts_threshing_machine) \
	DO(HOD_, mechanized_slaughtering_block) \
\
	/*interchangeable_parts*/ \
	DO(HOD_, precision_work) \
	DO(HOD_, machine_tools) \
	DO(HOD_, whitney_n_colts_standardized_small_arms) \
	DO(HOD_, food_packing_system) \
	DO(HOD_, integral_distillery_process) \
	DO(HOD_, jiggs) \
\
	/*semi_automatization*/ \
	DO(HOD_, electric_machinery) \
	DO(HOD_, oil_pumping_machinery) \
	DO(HOD_, drying_techniques) \
\
	/*assembly_line*/ \
	DO(HOD_, fordism) \
	DO(HOD_, industrialized_armament_industry) \
	DO(HOD_, refrigerator_technology) \
\
	/*Metulurgy*/ \
	DO(HOD_, ammunition_production) \
	DO(HOD_, small_arms_production) \
	DO(HOD_, explosives_production) \
	DO(HOD_, artillery_production) \
\
	/*clean_coal*/ \
	DO(HOD_, pit_coal) \
	DO(HOD_, coke) \
\
	/*cheap_iron*/ \
	DO(HOD_, advanced_ore_roasting_processes) \
	DO(HOD_, advanced_ore_grinding_processes) \
	DO(HOD_, advanced_ore_smelting_processes) \
	DO(HOD_, bessemer_steel) \
\
	/*cheap_steel*/ \
	DO(HOD_, martin_steel) \
	DO(HOD_, thomas_steel) \
\
	/*advanced_metallurgy*/ \
	DO(HOD_, electric_rolling_techniques) \
	DO(HOD_, steel_alloys) \
\
	/*electric_furnace*/ \
	DO(HOD_, stainless_steel) \
\
	/*early_railroad*/ \
	/*iron_railroad*/ \
	/*steel_railroad*/ \
	/*medicine*/ \
	DO(HOD_, combat_medicine) \
	DO(HOD_, aerial_bacteria_and_antiseptic_principle) \
	DO(HOD_, vaccination) \
	DO(HOD_, chemotherapy) \
	DO(HOD_, genetics_heredity) \
	DO(HOD_, clinical_thermometers_and_binaural_stethoscopes) \
	DO(HOD_, prophylaxis_against_malaria) \
	DO(HOD_, pressure_chambers_for_thorax_surgery) \
\
	/*inorganic_chemistry*/ \
	DO(HOD_, artificial_fertilizers) \
	DO(HOD_, nitroglycerin) \
	DO(HOD_, chemical_wood_pulp_process) \
	DO(HOD_, rubber_vulcanization) \
	DO(HOD_, catalytic_fermentative_process) \
\
	/*organic_chemistry*/ \
	DO(HOD_, synthetic_dye) \
	DO(HOD_, gaslights) \
	DO(HOD_, carbon_soil_enrichment) \
	DO(HOD_, cracking) \
\
	/*electricity*/ \
	DO(HOD_, x_rays) \
	DO(HOD_, edison_light_bulb) \
	DO(HOD_, bell_n_greys_telephones) \
	DO(HOD_, electrical_lighting) \
\
	/*synthetic_polymers*/ \
	DO(HOD_, rayon) \
	DO(HOD_, bakelite) \
	DO(HOD_, vacuum_tube) \
	DO(HOD_, tulls_seed_drill) \
	DO(HOD_, rotherham_plough) \
	DO(HOD_, telephone_switchboard) \
 \
 \
	/*NAVY INVENTIONS*/ \
\
	/*torpedo attacks*/ \
	DO(HOD_, torpedo_attacks) \
\
	/*battleship_column_doctrine*/ \
	DO(HOD_, long_range_fire_tactic) \
	DO(HOD_, speedy_maneuvering_tactic) \
\
	/*raider_group_doctrine*/ \
	DO(HOD_, sealane_hunters) \
	DO(HOD_, fast_ship_killer) \
	DO(HOD_, big_ship_competitor) \
	DO(HOD_, standard_raider_group_doctrine) \
\
	/*blue_and_brown_water_schools*/ \
	DO(HOD_, blue_water_school) \
	DO(HOD_, brown_water_school) \
\
	/*high_sea_battle_fleet*/ \
	DO(HOD_, offensive_attitude) \
	DO(HOD_, defensive_attitude) \
\
	/*modern_naval_doctrine*/ \
	DO(HOD_, decisive_firepower_doctrine) \
	DO(HOD_, naval_firepower_doctrine) \
\
	/*clipper_design*/ \
	DO(HOD_, building_station_shipyards) \
\
	/*steamers*/ \
	DO(HOD_, mechanized_fishing_vessels) \
	DO(HOD_, steamer_automatic_construction_plants) \
	DO(HOD_, steamer_transports) \
	DO(HOD_, commerce_raiders) \
\
	/*iron_steamers*/ \
	DO(HOD_, ironclads) \
	DO(HOD_, monitors) \
	DO(HOD_, torpedo_boats) \
\
	/*steel_steamers*/ \
	DO(HOD_, protected_cruisers) \
	DO(HOD_, pre_dreadnought) \
\
	/*steam_turbine_ships*/ \
	DO(HOD_, dreadnoughts) \
	DO(HOD_, advanced_cruisers) \
	DO(HOD_, modern_cruisers) \
	DO(HOD_, advanced_battleships) \
	DO(HOD_, modern_battleships) \
\
	/*fire_control_systems*/ \
	DO(HOD_, optical_rangefinders) \
	DO(HOD_, gyrostabilized_fire_control) \
	DO(HOD_, armour_piercing_projectiles) \
	DO(HOD_, armour_piercing_exploding_projectiles) \
	DO(HOD_, armoured_turrets) \
\
	/*main_armament*/ \
	DO(HOD_, bomb_guns) \
	DO(HOD_, rifled_guns) \
	DO(HOD_, quick_firing_guns) \
	DO(HOD_, armoured_citadels) \
	DO(HOD_, heavy_caliber_guns) \
\
	/*advanced_naval_design*/ \
	DO(HOD_, on_board_recon_aircraft) \
	DO(HOD_, gun_hydraulics) \
	DO(HOD_, heavy_armour_plates) \
	DO(HOD_, large_hull_form) \
\
	/*modern_naval_design*/ \
	DO(HOD_, wireless) \
	DO(HOD_, advanced_fire_control) \
	DO(HOD_, 15_inch_main_armament) \
\
	/*naval_plans*/ \
	DO(HOD_, night_training) \
	DO(HOD_, enemy_plotting_cards) \
	DO(HOD_, target_hit_profile_analysis) \
\
	/*naval_statistics*/ \
	DO(HOD_, hit_evaluation_reports) \
	DO(HOD_, empirical_interdiction_reports) \
	DO(HOD_, naval_strike_success_criterium) \
\
	/*naval_logistics*/ \
	DO(HOD_, underway_refueling) \
	DO(HOD_, ammunition_conservation_system) \
	DO(HOD_, radio_telegraphy) \
\
	/*naval_directionism*/ \
	DO(HOD_, political_lobbying) \
	DO(HOD_, military_political_decision_making) \
	DO(HOD_, local_military_government) \
\
	/*naval_integration*/ \
	DO(HOD_, war_gamming) \
	DO(HOD_, combined_arms_planning) \
\
	/*naval_professionalism*/ \
	DO(HOD_, academic_training) \
	DO(HOD_, combat_station_training) \
	DO(HOD_, societal_status) \
\
	/*naval_decision_making*/ \
	DO(HOD_, independent_command) \
	DO(HOD_, continuous_issuing_of_orders) \
	DO(HOD_, delegated_power) \
\
	/*naval_risk_management*/ \
	DO(HOD_, wargames) \
	DO(HOD_, alternative_plans) \
	DO(HOD_, n_saddle_orders) \
\
	/*naval_nco_training*/ \
	DO(HOD_, literate_ncos) \
	DO(HOD_, nco_combat_station_training) \
	DO(HOD_, nco_tactical_training) \
\
	/*modern_naval_training*/ \
	DO(HOD_, amphibious_warfare_training) \
	DO(HOD_, naval_exercises)


#define MAKE_UNION(TYPE,VAR) TYPE ## VAR,
enum HODInventionType {
    HOD_INVENTION_UNION(MAKE_UNION)
};


#define MAKE_STRINGS(TYPE,VAR) #VAR,
const char* const HODInventionNames[HOD_naval_exercises+1] = {
    HOD_INVENTION_UNION(MAKE_STRINGS)
};


#define HOD_NNM_INVENTION_UNION(DO) \
	/*NNM INVENTIONS*/ \
	\
	DO(HOD_NNM_, declaration_of_the_rights_of_man)\
	DO(HOD_NNM_, caste_privileges)\
	DO(HOD_NNM_, sati_abolished)\
	DO(HOD_NNM_, pig_fat_cartridges)\
	DO(HOD_NNM_, spirit_of_freedom)\
	DO(HOD_NNM_, carlism)\
	DO(HOD_NNM_, the_dark_continent)\
	\
	\
	/*ARMY_INVENTIONS*/ \
	\
	/*post_napoleonic_thought*/ \
	DO(HOD_NNM_, post_napoleonic_army_doctrine) \
	\
	/*strategic_mobility*/ \
	DO(HOD_NNM_, field_fortifications) \
	DO(HOD_NNM_, railroad_transport_groups) \
	\
	/*point_defense_system*/ \
	DO(HOD_NNM_, signal_detachments) \
	DO(HOD_NNM_, flexible_firepower) \
	DO(HOD_NNM_, synchronized_heavy_support) \
	DO(HOD_NNM_, staff_command_support) \
	\
	/*deep_defense_system*/ \
	DO(HOD_NNM_, trench_systems) \
	DO(HOD_NNM_, gas_attack_capability) \
	DO(HOD_NNM_, gas_defense_capability) \
	DO(HOD_NNM_, elastic_defense_system) \
	DO(HOD_NNM_, flexible_counterattacks) \
	\
	/*infiltration*/ \
	DO(HOD_NNM_, low_echelon_fire_support) \
	DO(HOD_NNM_, stormtrooper_training) \
	DO(HOD_NNM_, tank_experiments) \
	DO(HOD_NNM_, tank_exploits) \
	\
	/*modern_army_doctrine*/ \
	DO(HOD_NNM_, firepower_vs_mobility) \
	DO(HOD_NNM_, grand_battle_plan_vs_superior_firepower) \
	DO(HOD_NNM_, human_wave_vs_spearhead) \
	\
	/*flintlock_rifles*/ \
	DO(HOD_NNM_, flintlock_rifle_armament) \
	\
	/*muzzle_loaded_rifles*/ \
	DO(HOD_NNM_, muzzle_loaded_rifle_armament) \
	\
	/*breech_loaded_rifles*/ \
	DO(HOD_NNM_, breech_loaded_rifle_armament) \
	DO(HOD_NNM_, colonial_negotiations) \
	\
	/*machine_guns*/ \
	DO(HOD_NNM_, machine_gun_armament) \
	\
	/*bolt_action_rifles*/ \
	DO(HOD_NNM_, bolt_action_rifle_armament) \
	\
	/*modern_divisional_structure*/ \
	DO(HOD_NNM_, interwar_infantry) \
	DO(HOD_NNM_, interwar_cavalry) \
	DO(HOD_NNM_, rear_area_dumps) \
	\
	/*iron_breech_loaded_artillery*/ \
	/*indirect_artillery_fire*/ \
	/*heavy_armament*/ \
	DO(HOD_NNM_, tankette) \
	DO(HOD_NNM_, light_tank) \
	DO(HOD_NNM_, armoured_cars) \
	DO(HOD_NNM_, anti_aircraft_artillery) \
	DO(HOD_NNM_, improved_anti_aircraft_artillery) \
	DO(HOD_NNM_, light_artillery) \
	DO(HOD_NNM_, medium_artillery) \
	DO(HOD_NNM_, heavy_artillery) \
	\
	/*military_staff_system*/ \
	DO(HOD_NNM_, cuirassier_activation) \
	DO(HOD_NNM_, dragoon_activation) \
	DO(HOD_NNM_, hussar_activation) \
	\
	/*military_plans*/ \
	DO(HOD_NNM_, telegraph_coordination) \
	DO(HOD_NNM_, mobilization_time_tables) \
	DO(HOD_NNM_, personnel_category_tables) \
	\
	/*military_statistics*/ \
	DO(HOD_NNM_, ammunition_area_effect_calculations) \
	DO(HOD_NNM_, empirical_combat_efficiency_matrix) \
	DO(HOD_NNM_, combat_loss_evaluation_schemata) \
	\
	/*military_logistics*/ \
	DO(HOD_NNM_, pre_planned_depot_systems) \
	DO(HOD_NNM_, logistical_calculations) \
	DO(HOD_NNM_, replenishment_transport_organisation) \
	\
	/*military_directionism*/ \
	DO(HOD_NNM_, army_political_lobbying) \
	DO(HOD_NNM_, army_military_political_decision_making) \
	DO(HOD_NNM_, army_local_military_government) \
	\
	/*aeronautics*/ \
	DO(HOD_NNM_, military_aviation) \
	DO(HOD_NNM_, early_interwar_bombers) \
	DO(HOD_NNM_, late_interwar_bombers) \
	DO(HOD_NNM_, early_interwar_fighters) \
	DO(HOD_NNM_, late_interwar_fighters) \
	DO(HOD_NNM_, t_air_superiority) \
	DO(HOD_NNM_, flying_circus_doctrine) \
	DO(HOD_NNM_, force_substitution_doctrine) \
	DO(HOD_NNM_, battlefield_destruction_doctrine) \
	DO(HOD_NNM_, operational_destruction_doctrine) \
	DO(HOD_NNM_, strategic_destruction_doctrine) \
	\
	/*army_professionalism*/ \
	DO(HOD_NNM_, army_academic_training) \
	DO(HOD_NNM_, field_training) \
	DO(HOD_NNM_, army_societal_status) \
	\
	/*army_decision_making*/ \
	DO(HOD_NNM_, army_independent_command) \
	DO(HOD_NNM_, army_continuous_issuing_of_orders) \
	DO(HOD_NNM_, army_delegated_power) \
	\
	/*army_risk_management*/ \
	DO(HOD_NNM_, army_wargames) \
	DO(HOD_NNM_, army_alternative_plans) \
	DO(HOD_NNM_, saddle_orders) \
	\
	/*army_nco_training*/ \
	DO(HOD_NNM_, army_literate_ncos) \
	DO(HOD_NNM_, nco_field_training) \
	DO(HOD_NNM_, army_nco_tactical_training) \
	\
	\
	/*COMMERCE INVENTIONS*/ \
	\
	/*stock_exchange*/ \
	DO(HOD_NNM_, multitude_of_financial_instruments) \
	DO(HOD_NNM_, insurance_companies) \
	DO(HOD_NNM_, regulated_buying_and_selling_of_stocks) \
	\
	/*business_banks*/ \
	DO(HOD_NNM_, liquidation_of_assets) \
	DO(HOD_NNM_, capital_for_investors) \
	DO(HOD_NNM_, distribution_channels) \
	\
	/*investment_banks*/ \
	DO(HOD_NNM_, industrial_rnd_investments) \
	DO(HOD_NNM_, industrial_production_investments) \
	DO(HOD_NNM_, industrial_distribution_investments) \
	\
	/*bank_inspection_board*/ \
	DO(HOD_NNM_, private_bank_monitoring) \
	DO(HOD_NNM_, market_behaviour_monitoring) \
	\
	/*mutual_funds*/ \
	DO(HOD_NNM_, buying_on_margin) \
	\
	/*ad_hoc_money_bill_printing*/ \
	DO(HOD_NNM_, silver_standard) \
	DO(HOD_NNM_, decimal_monetary_system) \
	\
	/*private_bank_money_bill_printing*/ \
	DO(HOD_NNM_, double_standard) \
	DO(HOD_NNM_, fixed_intramonetary_exchange_rates) \
	\
	/*central_bank_money_bill_printing*/ \
	DO(HOD_NNM_, gold_standard) \
	DO(HOD_NNM_, international_monetary_exchange_rates) \
	\
	/*modern_central_bank_system*/ \
	DO(HOD_NNM_, state_bond_monetary_mechanism) \
	DO(HOD_NNM_, interest_inflation_weapon) \
	\
	/*market_determined_exchange_rates*/ \
	DO(HOD_NNM_, hyperinflation) \
	\
	/*eary_classical_theory*/ \
	DO(HOD_NNM_, john_ramsay_mcculloch) \
	DO(HOD_NNM_, nassau_william_sr) \
	DO(HOD_NNM_, james_mill) \
	\
	/*late_classical_theory*/ \
	DO(HOD_NNM_, john_elliot_cairnes) \
	DO(HOD_NNM_, robert_torrens) \
	DO(HOD_NNM_, john_stuart_mill) \
	\
	/*collectivist_theory*/ \
	DO(HOD_NNM_, pierre_joseph_proudhon) \
	DO(HOD_NNM_, karl_marx) \
	DO(HOD_NNM_, friedrich_engels) \
	DO(HOD_NNM_, eduard_bernstein) \
	DO(HOD_NNM_, sidney_webb) \
	DO(HOD_NNM_, karl_kautsky) \
	DO(HOD_NNM_, jean_jaurès) \
	\
	/*the_historical_theory*/ \
	DO(HOD_NNM_, friedrich_list) \
	DO(HOD_NNM_, wilhelm_roscher) \
	DO(HOD_NNM_, bruno_hildebrand) \
	DO(HOD_NNM_, karl_knies) \
	DO(HOD_NNM_, gustav_von_schmoller) \
	DO(HOD_NNM_, adolph_wagner) \
	DO(HOD_NNM_, william_cunningham) \
	\
	/*neoclassical_theory*/ \
	DO(HOD_NNM_, johann_heinrich_von_thünen) \
	DO(HOD_NNM_, léon_walras) \
	DO(HOD_NNM_, stanley_jevons) \
	DO(HOD_NNM_, carl_menger) \
	DO(HOD_NNM_, thorstein_veblen) \
	DO(HOD_NNM_, alfred_marshall) \
	DO(HOD_NNM_, augustin_cournot) \
	DO(HOD_NNM_, jules_dupuit) \
	DO(HOD_NNM_, vilfredo_pareto) \
	DO(HOD_NNM_, knut_wicksell) \
	DO(HOD_NNM_, irving_fischer) \
	DO(HOD_NNM_, joseph_schumpeter) \
	DO(HOD_NNM_, eugen_von_böhm_bawerk) \
	\
	/*keynesian_economics*/ \
	DO(HOD_NNM_, john_maynard_keynes) \
	DO(HOD_NNM_, richard_kahn) \
	DO(HOD_NNM_, ludwig_von_mises) \
	DO(HOD_NNM_, friedrich_hayek) \
	\
	/*market_structure*/ \
	DO(HOD_NNM_, polypoly_structure) \
	DO(HOD_NNM_, oligopoly_structure) \
	DO(HOD_NNM_, monopoly_structure) \
	\
	/*business_regulations*/ \
	DO(HOD_NNM_, limited_partnership_company) \
	DO(HOD_NNM_, joint_stock_company) \
	DO(HOD_NNM_, patent) \
	DO(HOD_NNM_, trademark) \
	\
	/*market_regulations*/ \
	DO(HOD_NNM_, anti_trust_laws) \
	DO(HOD_NNM_, financial_regulations) \
	DO(HOD_NNM_, banking_regulations) \
	\
	/*economic_responsibility*/ \
	DO(HOD_NNM_, working_board_of_directors) \
	DO(HOD_NNM_, bankruptcy_proceedings) \
	DO(HOD_NNM_, annual_report_proceedings) \
	\
	/*government_interventionism*/ \
	DO(HOD_NNM_, priming_the_pump_) \
	DO(HOD_NNM_, mixed_economy) \
	DO(HOD_NNM_, centralized_economic_planning) \
	DO(HOD_NNM_, autarky) \
	DO(HOD_NNM_, import_substitution_industrialization) \
	\
	/*organized_factories*/ \
	DO(HOD_NNM_, work_classification) \
	DO(HOD_NNM_, work_specialization) \
	DO(HOD_NNM_, work_control_and_hierarchy) \
	\
	/*scientific_management*/ \
	DO(HOD_NNM_, sequence_planning) \
	DO(HOD_NNM_, inventory_planning) \
	DO(HOD_NNM_, quality_assurance_procedure) \
	\
	/*time_saving_measures*/ \
	DO(HOD_NNM_, time_studies) \
	DO(HOD_NNM_, time_clock) \
	DO(HOD_NNM_, lighting) \
	\
	/*management_strategy*/ \
	DO(HOD_NNM_, old_style_family_business) \
	DO(HOD_NNM_, anynomous_investor_business) \
	DO(HOD_NNM_, marketized_smallholder_business) \
	\
	/*organizational_development*/ \
	DO(HOD_NNM_, i_scientific_management) \
	DO(HOD_NNM_, human_resource_development) \
	\
	\
	/*CULTURE INVENTIONS*/ \
	\
	/*romanticism*/ \
	DO(HOD_NNM_, romanticist_literature) \
	DO(HOD_NNM_, romanticist_art) \
	DO(HOD_NNM_, romanticist_music) \
	\
	/*realism*/ \
	DO(HOD_NNM_, realist_literature) \
	DO(HOD_NNM_, realist_art) \
	DO(HOD_NNM_, realist_music) \
	\
	/*impressionism*/ \
	DO(HOD_NNM_, impressionist_literature) \
	DO(HOD_NNM_, impressionist_art) \
	DO(HOD_NNM_, impressionist_music) \
	\
	/*expressionism*/ \
	DO(HOD_NNM_, expressionist_literature) \
	DO(HOD_NNM_, expressionist_art) \
	DO(HOD_NNM_, expressionist_music) \
	\
	/*mass_culture_n_the_avant_garde*/ \
	DO(HOD_NNM_, avantgarde_literature) \
	DO(HOD_NNM_, avantgarde_art) \
	DO(HOD_NNM_, avantgarde_music) \
	DO(HOD_NNM_, surrealism) \
	DO(HOD_NNM_, modern_design) \
	DO(HOD_NNM_, jazz) \
	DO(HOD_NNM_, socialist_realism) \
	\
	/*idealism*/ \
	DO(HOD_NNM_, neokantian_idealism) \
	DO(HOD_NNM_, hegelian_idealism) \
	DO(HOD_NNM_, proto_existentialism) \
	\
	/*empiricism*/ \
	DO(HOD_NNM_, pragmatism) \
	DO(HOD_NNM_, i_empiricism) \
	\
	/*analytic_philosophy*/ \
	DO(HOD_NNM_, i_analytic_philosophy) \
	DO(HOD_NNM_, modern_logics) \
	DO(HOD_NNM_, logical_positivism) \
	DO(HOD_NNM_, philosophy_of_language) \
	\
	/*phenomenology_n_hermeneutic*/ \
	DO(HOD_NNM_, phenomenology) \
	DO(HOD_NNM_, hermeneutics) \
	\
	/*anti__rationalism*/ \
	DO(HOD_NNM_, existentialism) \
	DO(HOD_NNM_, nihilism_and_the_will) \
	DO(HOD_NNM_, futurism) \
	DO(HOD_NNM_, quantum_theory) \
	\
	/*biologism*/ \
	DO(HOD_NNM_, darwinism) \
	DO(HOD_NNM_, social_darwinism) \
	\
	/*social_science*/ \
	DO(HOD_NNM_, sociology) \
	DO(HOD_NNM_, social_psychology) \
	DO(HOD_NNM_, political_science) \
	DO(HOD_NNM_, social_anthropology) \
	\
	/*social_alienation*/ \
	DO(HOD_NNM_, the_revolt_of_the_masses) \
	DO(HOD_NNM_, the_decline_of_the_west_and_the_lost_generation) \
	\
	/*the rights of man - called enlightenment_thought in files...*/ \
	DO(HOD_NNM_, paternalism) \
	DO(HOD_NNM_, constitutionalism) \
	DO(HOD_NNM_, atheism) \
	DO(HOD_NNM_, egalitarianism) \
	DO(HOD_NNM_, rationalism) \
	\
	/*ideological_thought*/ \
	DO(HOD_NNM_, authoritarianism) \
	DO(HOD_NNM_, hierarchical_order) \
	DO(HOD_NNM_, traditionalism) \
	DO(HOD_NNM_, political_religion) \
	DO(HOD_NNM_, individualism) \
	DO(HOD_NNM_, secularization) \
	DO(HOD_NNM_, social_conscience) \
	DO(HOD_NNM_, collectivism) \
	DO(HOD_NNM_, determinism) \
	DO(HOD_NNM_, social_justice) \
	\
	/*state_n_government*/ \
	DO(HOD_NNM_, populism_vs_establishment) \
	DO(HOD_NNM_, clericalism_vs_anticlericalism) \
	DO(HOD_NNM_, bureaucracy_vs_nepotism) \
	DO(HOD_NNM_, meritocracy_vs_aristocracy) \
	\
	/*nationalism_n_imperialism*/ \
	DO(HOD_NNM_, national_fraternity) \
	DO(HOD_NNM_, biased_multiculturalism) \
	DO(HOD_NNM_, manifest_destiny) \
	DO(HOD_NNM_, expansionism) \
	DO(HOD_NNM_, mission_to_civilize) \
	DO(HOD_NNM_, separatism) \
	\
	/*revolution_n_counterrevolution*/ \
	DO(HOD_NNM_, dogma_of_violence) \
	DO(HOD_NNM_, dogma_of_mass_action) \
	DO(HOD_NNM_, terrorism) \
	DO(HOD_NNM_, secret_police) \
	DO(HOD_NNM_, womens_suffrage_movement) \
	DO(HOD_NNM_, rhetoric_of_hate) \
	DO(HOD_NNM_, nostalgic_romanticism) \
	DO(HOD_NNM_, anti_egalitarianism) \
	DO(HOD_NNM_, vangaurd_party) \
	\
	/*mass_politics*/ \
	DO(HOD_NNM_, the_doctrine_of_fascism) \
	DO(HOD_NNM_, paramilitary_groups) \
	DO(HOD_NNM_, the_police_state) \
	\
	/*behaviorism*/ \
	DO(HOD_NNM_, mass_advertising) \
	DO(HOD_NNM_, national_radio_networks) \
	DO(HOD_NNM_, the_talkies) \
	\
	\
	/*INDUSTRY INVENTIONS*/ \
	\
	/*combustion_engine*/ \
	DO(HOD_NNM_, daimlers_automobile) \
	DO(HOD_NNM_, wright_n_langleys_aeroplanes) \
	DO(HOD_NNM_, tractors) \
	\
	/*electrical_power_generation*/ \
	DO(HOD_NNM_, direct_current) \
	DO(HOD_NNM_, alternating_current) \
	DO(HOD_NNM_, polyphase_system) \
	\
	/*mechanical_production*/ \
	DO(HOD_NNM_, sharp_n_roberts_power_loom) \
	DO(HOD_NNM_, jacquard_power_loom) \
	DO(HOD_NNM_, northrop_power_loom) \
	DO(HOD_NNM_, mechanical_saw) \
	DO(HOD_NNM_, mechanical_precision_saw) \
	DO(HOD_NNM_, hussey_n_mccormicks_reaping_machine) \
	DO(HOD_NNM_, pitts_threshing_machine) \
	DO(HOD_NNM_, mechanized_slaughtering_block) \
	\
	/*interchangeable_parts*/ \
	DO(HOD_NNM_, precision_work) \
	DO(HOD_NNM_, machine_tools) \
	DO(HOD_NNM_, whitney_n_colts_standardized_small_arms) \
	DO(HOD_NNM_, food_packing_system) \
	DO(HOD_NNM_, integral_distillery_process) \
	DO(HOD_NNM_, jiggs) \
	\
	/*semi_automatization*/ \
	DO(HOD_NNM_, electric_machinery) \
	DO(HOD_NNM_, oil_pumping_machinery) \
	DO(HOD_NNM_, drying_techniques) \
	\
	/*assembly_line*/ \
	DO(HOD_NNM_, fordism) \
	DO(HOD_NNM_, industrialized_armament_industry) \
	DO(HOD_NNM_, refrigerator_technology) \
	\
	/*Metulurgy*/ \
	DO(HOD_NNM_, ammunition_production) \
	DO(HOD_NNM_, small_arms_production) \
	DO(HOD_NNM_, explosives_production) \
	DO(HOD_NNM_, artillery_production) \
	\
	/*clean_coal*/ \
	DO(HOD_NNM_, pit_coal) \
	DO(HOD_NNM_, coke) \
	\
	/*cheap_iron*/ \
	DO(HOD_NNM_, advanced_ore_roasting_processes) \
	DO(HOD_NNM_, advanced_ore_grinding_processes) \
	DO(HOD_NNM_, advanced_ore_smelting_processes) \
	DO(HOD_NNM_, bessemer_steel) \
	\
	/*cheap_steel*/ \
	DO(HOD_NNM_, martin_steel) \
	DO(HOD_NNM_, thomas_steel) \
	\
	/*advanced_metallurgy*/ \
	DO(HOD_NNM_, electric_rolling_techniques) \
	DO(HOD_NNM_, steel_alloys) \
	\
	/*electric_furnace*/ \
	DO(HOD_NNM_, stainless_steel) \
	\
	/*early_railroad*/ \
	/*iron_railroad*/ \
	/*steel_railroad*/ \
	/*medicine*/ \
	DO(HOD_NNM_, combat_medicine) \
	DO(HOD_NNM_, aerial_bacteria_and_antiseptic_principle) \
	DO(HOD_NNM_, vaccination) \
	DO(HOD_NNM_, chemotherapy) \
	DO(HOD_NNM_, genetics_heredity) \
	DO(HOD_NNM_, clinical_thermometers_and_binaural_stethoscopes) \
	DO(HOD_NNM_, prophylaxis_against_malaria) \
	DO(HOD_NNM_, pressure_chambers_for_thorax_surgery) \
	\
	/*inorganic_chemistry*/ \
	DO(HOD_NNM_, artificial_fertilizers) \
	DO(HOD_NNM_, nitroglycerin) \
	DO(HOD_NNM_, chemical_wood_pulp_process) \
	DO(HOD_NNM_, rubber_vulcanization) \
	DO(HOD_NNM_, catalytic_fermentative_process) \
	\
	/*organic_chemistry*/ \
	DO(HOD_NNM_, synthetic_dye) \
	DO(HOD_NNM_, gaslights) \
	DO(HOD_NNM_, carbon_soil_enrichment) \
	DO(HOD_NNM_, cracking) \
	\
	/*electricity*/ \
	DO(HOD_NNM_, x_rays) \
	DO(HOD_NNM_, edison_light_bulb) \
	DO(HOD_NNM_, bell_n_greys_telephones) \
	DO(HOD_NNM_, electrical_lighting) \
	\
	/*synthetic_polymers*/ \
	DO(HOD_NNM_, rayon) \
	DO(HOD_NNM_, bakelite) \
	DO(HOD_NNM_, vacuum_tube) \
	DO(HOD_NNM_, tulls_seed_drill) \
	DO(HOD_NNM_, rotherham_plough) \
	DO(HOD_NNM_, telephone_switchboard) \
	\
	\
	/*NAVY INVENTIONS*/ \
	\
	/*torpedo attacks*/ \
	DO(HOD_NNM_, torpedo_attacks) \
	\
	/*battleship_column_doctrine*/ \
	DO(HOD_NNM_, long_range_fire_tactic) \
	DO(HOD_NNM_, speedy_maneuvering_tactic) \
	\
	/*raider_group_doctrine*/ \
	DO(HOD_NNM_, sealane_hunters) \
	DO(HOD_NNM_, fast_ship_killer) \
	DO(HOD_NNM_, big_ship_competitor) \
	DO(HOD_NNM_, standard_raider_group_doctrine) \
	\
	/*blue_and_brown_water_schools*/ \
	DO(HOD_NNM_, blue_water_school) \
	DO(HOD_NNM_, brown_water_school) \
	\
	/*high_sea_battle_fleet*/ \
	DO(HOD_NNM_, offensive_attitude) \
	DO(HOD_NNM_, defensive_attitude) \
	\
	/*modern_naval_doctrine*/ \
	DO(HOD_NNM_, decisive_firepower_doctrine) \
	DO(HOD_NNM_, naval_firepower_doctrine) \
	\
	/*clipper_design*/ \
	DO(HOD_NNM_, building_station_shipyards) \
	\
	/*steamers*/ \
	DO(HOD_NNM_, mechanized_fishing_vessels) \
	DO(HOD_NNM_, steamer_automatic_construction_plants) \
	DO(HOD_NNM_, steamer_transports) \
	DO(HOD_NNM_, commerce_raiders) \
	\
	/*iron_steamers*/ \
	DO(HOD_NNM_, ironclads) \
	DO(HOD_NNM_, monitors) \
	DO(HOD_NNM_, torpedo_boats) \
	\
	/*steel_steamers*/ \
	DO(HOD_NNM_, protected_cruisers) \
	DO(HOD_NNM_, pre_dreadnought) \
	\
	/*steam_turbine_ships*/ \
	DO(HOD_NNM_, dreadnoughts) \
	DO(HOD_NNM_, advanced_cruisers) \
	DO(HOD_NNM_, modern_cruisers) \
	DO(HOD_NNM_, advanced_battleships) \
	DO(HOD_NNM_, modern_battleships) \
	\
	/*fire_control_systems*/ \
	DO(HOD_NNM_, optical_rangefinders) \
	DO(HOD_NNM_, gyrostabilized_fire_control) \
	DO(HOD_NNM_, armour_piercing_projectiles) \
	DO(HOD_NNM_, armour_piercing_exploding_projectiles) \
	DO(HOD_NNM_, armoured_turrets) \
	\
	/*main_armament*/ \
	DO(HOD_NNM_, bomb_guns) \
	DO(HOD_NNM_, rifled_guns) \
	DO(HOD_NNM_, quick_firing_guns) \
	DO(HOD_NNM_, armoured_citadels) \
	DO(HOD_NNM_, heavy_caliber_guns) \
	\
	/*advanced_naval_design*/ \
	DO(HOD_NNM_, on_board_recon_aircraft) \
	DO(HOD_NNM_, gun_hydraulics) \
	DO(HOD_NNM_, heavy_armour_plates) \
	DO(HOD_NNM_, large_hull_form) \
	\
	/*modern_naval_design*/ \
	DO(HOD_NNM_, wireless) \
	DO(HOD_NNM_, advanced_fire_control) \
	DO(HOD_NNM_, 15_inch_main_armament) \
	\
	/*naval_plans*/ \
	DO(HOD_NNM_, night_training) \
	DO(HOD_NNM_, enemy_plotting_cards) \
	DO(HOD_NNM_, target_hit_profile_analysis) \
	\
	/*naval_statistics*/ \
	DO(HOD_NNM_, hit_evaluation_reports) \
	DO(HOD_NNM_, empirical_interdiction_reports) \
	DO(HOD_NNM_, naval_strike_success_criterium) \
	\
	/*naval_logistics*/ \
	DO(HOD_NNM_, underway_refueling) \
	DO(HOD_NNM_, ammunition_conservation_system) \
	DO(HOD_NNM_, radio_telegraphy) \
	\
	/*naval_directionism*/ \
	DO(HOD_NNM_, political_lobbying) \
	DO(HOD_NNM_, military_political_decision_making) \
	DO(HOD_NNM_, local_military_government) \
	\
	/*naval_integration*/ \
	DO(HOD_NNM_, war_gamming) \
	DO(HOD_NNM_, combined_arms_planning) \
	\
	/*naval_professionalism*/ \
	DO(HOD_NNM_, academic_training) \
	DO(HOD_NNM_, combat_station_training) \
	DO(HOD_NNM_, societal_status) \
	\
	/*naval_decision_making*/ \
	DO(HOD_NNM_, independent_command) \
	DO(HOD_NNM_, continuous_issuing_of_orders) \
	DO(HOD_NNM_, delegated_power) \
	\
	/*naval_risk_management*/ \
	DO(HOD_NNM_, wargames) \
	DO(HOD_NNM_, alternative_plans) \
	DO(HOD_NNM_, n_saddle_orders) \
	\
	/*naval_nco_training*/ \
	DO(HOD_NNM_, literate_ncos) \
	DO(HOD_NNM_, nco_combat_station_training) \
	DO(HOD_NNM_, nco_tactical_training) \
	\
	/*modern_naval_training*/ \
	DO(HOD_NNM_, amphibious_warfare_training) \
	DO(HOD_NNM_, naval_exercises)\
	


#define MAKE_UNION(TYPE,VAR) TYPE ## VAR,
enum HODNNMInventionType {
	HOD_NNM_INVENTION_UNION(MAKE_UNION)
};


#define MAKE_STRINGS(TYPE,VAR) #VAR,
const char* const HODNNMInventionNames[HOD_NNM_naval_exercises + 1] = {
	HOD_NNM_INVENTION_UNION(MAKE_STRINGS)
};


enum inventionStatus
{
	illegal,
	possible,
	active
};



#endif	// V2INVENTIONS_H_
