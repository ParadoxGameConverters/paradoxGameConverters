#ifndef V2INVENTIONS_H_
#define V2INVENTIONS_H_

enum inventionTypes
{
	//ARMY_INVENTIONS

	//post_napoleonic_thought
	//strategic_mobility
	field_fortifications = 0,
	railroad_transport_groups,
	signal_detachments,

	//point_defense_system
	flexible_firepower,
	synchronized_heavy_support,
	staff_command_support,

	//deep_defense_system
	trench_systems,
	gas_attack_capability,
	gas_defense_capability,
	elastic_defense_system,
	flexible_counterattacks,

	//infiltration
	low_echelon_fire_support,
	stormtrooper_training,
	tank_experiments,
	tank_exploits,

	//modern_army_doctrine
	firepower_vs_mobility,
	grand_battle_plan_vs_superior_firepower,
	human_wave_vs_spearhead,

	//flintlock_rifles
	//muzzle_loaded_rifles
	//breech_loaded_rifles
	//machine_guns
	//bolt_action_rifles
	//modern_divisional_structure
	interwar_infantry,
	interwar_cavalry,
	rear_area_dumps,

	//bronze_muzzle_loaded_artillery
	//iron_muzzle_loaded_artillery
	//iron_breech_loaded_artillery
	//steel_breech_loaded_artillery
	//indirect_artillery_fire
	//heavy_armament
	tankette,
	light_tank,
	armoured_cars,
	anti_aircraft_artillery,
	improved_anti_aircraft_artillery,
	light_artillery,
	medium_artillery,
	heavy_artillery,

	//military_staff_system
	//military_plans
	telegraph_coordination,
	mobilization_time_tables,
	personnel_category_tables,

	//military_statistics
	ammunition_area_effect_calculations,
	empirical_combat_efficiency_matrix,
	combat_loss_evaluation_schemata,

	//military_logistics
	pre_planned_depot_systems,
	logistical_calculations,
	replenishment_transport_organisation,

	//military_directionism
	army_political_lobbying,
	army_military_political_decision_making,
	army_local_military_government,

	//aeronautics
	early_interwar_bombers,
	late_interwar_bombers,
	early_interwar_fighters,
	late_interwar_fighters,
	t_air_superiority,
	flying_circus_doctrine,
	force_substitution_doctrine,
	battlefield_destruction_doctrine,
	operational_destruction_doctrine,
	strategic_destruction_doctrine,

	//army_command_principle
	//army_professionalism
	army_academic_training,
	field_training,
	army_societal_status,

	//army_decision_making
	army_independent_command,
	army_continuous_issuing_of_orders,
	army_delegated_power,

	//army_risk_management
	army_wargames,
	army_alternative_plans,
	saddle_orders,

	//army_nco_training
	army_literate_ncos,
	nco_field_training,
	army_nco_tactical_training,

	//great_war_experience


	//COMMERCE_INVENTIONS

	//private_banks
	//stock_exchange
	multitude_of_financial_instruments,
	insurance_companies,
	regulated_buying_and_selling_of_stocks,

	//business_banks
	liquidation_of_assets,
	capital_for_investors,
	distribution_channels,

	//investment_banks
	industrial_rnd_investments,
	industrial_production_investments,
	industrial_distribution_investments,

	//bank_inspection_board
	private_bank_monitoring,
	market_behaviour_monitoring,

	//mutual_funds
	buying_on_margin,
	
	//no_standard
	//ad_hoc_money_bill_printing
	silver_standard,
	decimal_monetary_system,

	//private_bank_money_bill_printing
	double_standard,
	fixed_intramonetary_exchange_rates,

	//central_bank_money_bill_printing
	gold_standard,
	international_monetary_exchange_rates,

	//modern_central_bank_system
	state_bond_monetary_mechanism,
	interest_inflation_weapon,

	//market_determined_exchange_rates
	hyperinflation,

	//eary_classical_theory
	john_ramsay_mcculloch,
	nassau_william_sr,
	james_mill,

	//late_classical_theory
	john_elliot_cairnes,
	robert_torrens,
	john_stuart_mill,

	//collectivist_theory
	pierre_joseph_proudhon,
	karl_marx,
	friedrich_engels,
	eduard_bernstein,
	sidney_webb,
	karl_kautsky,
	jean_jaurès,

	//the_historical_theory
	friedrich_list,
	wilhelm_roscher,
	bruno_hildebrand,
	karl_knies,
	gustav_von_schmoller,
	adolph_wagner,
	william_cunningham,

	//neoclassical_theory
	johann_heinrich_von_thünen,
	léon_walras,
	stanley_jevons,
	carl_menger,
	thorstein_veblen,
	alfred_marshall,
	augustin_cournot,
	jules_dupuit,
	vilfredo_pareto,
	knut_wicksell,
	irving_fischer,
	joseph_schumpeter,
	eugen_von_böhm_bawerk,

	//keynesian_economics
	john_maynard_keynes,
	richard_kahn,
	ludwig_von_mises,
	friedrich_hayek,

	//freedom_of_trade
	//market_structure
	polypoly_structure,
	oligopoly_structure,
	monopoly_structure,

	//business_regulations
	limited_partnership_company,
	joint_stock_company,
	patent,
	trademark,

	//market_regulations
	anti_trust_laws,
	financial_regulations,
	banking_regulations,

	//economic_responsibility
	working_board_of_directors,
	bankruptcy_proceedings,
	annual_report_proceedings,

	//government_interventionism
	priming_the_pump_,
	mixed_economy,
	centralized_economic_planning,
	autarky,
	import_substitution_industrialization,

	//guild_based_production
	//organized_factories
	work_classification,
	work_specialization,
	work_control_and_hierarchy,

	//scientific_management
	sequence_planning,
	inventory_planning,
	quality_assurance_procedure,

	//time_saving_measures
	time_studies,
	time_clock,
	lighting,

	//management_strategy
	old_style_family_business,
	anynomous_investor_business,
	marketized_smallholder_business,

	//organizational_development
	i_scientific_management,
	human_resource_development,


	//CULTURE_INVENTIONS
	//classicism_n_early_romanticism
	//romanticism
	romanticist_literature,
	romanticist_art,
	romanticist_music,

	//realism
	realist_literature,
	realist_art,
	realist_music,

	//impressionism
	impressionist_literature,
	impressionist_art,
	impressionist_music,

	//expressionism
	expressionist_literature,
	expressionist_art,
	expressionist_music,

	//mass_culture_n_the_avant_garde
	avantgarde_literature,
	avantgarde_art,
	avantgarde_music,
	surrealism,
	modern_design,
	jazz,
	socialist_realism,

	//late_enlightenment_philosophy
	//idealism
	neokantian_idealism,
	hegelian_idealism,
	proto_existentialism,

	//empiricism
	pragmatism,
	i_empiricism,

	//analytic_philosophy
	i_analytic_philosophy,
	modern_logics,
	logical_positivism,
	philosophy_of_language,

	//phenomenology_n_hermeneutic
	phenomenology,
	hermeneutics,

	//anti__rationalism
	existentialism,
	nihilism_and_the_will,
	futurism,
	quantum_theory,

	//malthusian_thought
	//positivism
	//functionalism
	//biologism
	darwinism,
	social_darwinism,

	//social_science
	sociology,
	social_psychology,
	political_science,
	social_anthropology,

	//social_alienation
	the_revolt_of_the_masses,
	the_decline_of_the_west_and_the_lost_generation,

	//enlightenment_thought
	//ideological_thought
	authoritarianism,
	hierarchical_order,
	traditionalism,
	political_religion,
	paternalism,
	constitutionalism,
	individualism,
	rationalism,
	secularization,
	social_conscience,
	egalitarianism,
	collectivism,
	determinism,
	atheism,
	social_justice,

	//state_n_government
	populism_vs_establishment,
	clericalism_vs_anticlericalism,
	bureaucracy_vs_nepotism,
	meritocracy_vs_aristocracy,

	//nationalism_n_imperialism
	national_fraternity,
	biased_multiculturalism,
	manifest_destiny,
	expansionism,
	mission_to_civilize,
	separatism,

	//revolution_n_counterrevolution
	dogma_of_violence,
	dogma_of_mass_action,
	terrorism,
	secret_police,
	womens_suffrage_movement,
	rhetoric_of_hate,
	nostalgic_romanticism,
	anti_egalitarianism,
	vangaurd_party,

	//mass_politics
	the_doctrine_of_fascism,
	paramilitary_groups,
	the_police_state,

	//introspectionism
	//associationism
	//phenominalism
	//experimental_psychology
	//psychoanalysis
	//behaviorism
	mass_advertising,
	national_radio_networks,
	the_talkies,


	//INDUSTRY INVENTIONS


	//water_wheel_power
	//practical_steam_engine
	//high_and_low_pressure_steam_engines
	//steam_turbines
	//combustion_engine
	daimlers_automobile,
	wright_n_langleys_aeroplanes,
	tractors,

	//electrical_power_generation
	direct_current,
	alternating_current,
	polyphase_system,

	//publishing_industry
	//mechanical_production
	sharp_n_roberts_power_loom,
	jacquard_power_loom,
	northrop_power_loom,
	mechanical_saw,
	mechanical_precision_saw,
	hussey_n_mccormicks_reaping_machine,
	pitts_threshing_machine,
	mechanized_slaughtering_block,

	//interchangeable_parts
	precision_work,
	machine_tools,
	whitney_n_colts_standardized_small_arms,
	food_packing_system,
	integral_distillery_process,
	jiggs,

	//semi_automatization
	electric_machinery,
	oil_pumping_machinery,
	drying_techniques,

	//assembly_line
	fordism,
	industrialized_armament_industry,
	refrigerator_technology,

	//shift_work

	//Mechanized_mining
	ammunition_production,
	small_arms_production,
	explosives_production,
	artillery_production,

	//clean_coal
	pit_coal,
	coke,

	//cheap_iron
	advanced_ore_roasting_processes,
	advanced_ore_grinding_processes,
	advanced_ore_smelting_processes,
	bessemer_steel,

	//cheap_steel
	martin_steel,
	thomas_steel,

	//advanced_metallurgy
	electric_rolling_techniques,
	steel_alloys,

	//electric_furnace
	stainless_steel,

	//experimental_railroad
	//early_railroad
	//iron_railroad
	//steel_railroad
	//integral_rail_system
	//limited_access_road

	//basic_chemistry
	//medicine
	combat_medicine,
	aerial_bacteria_and_antiseptic_principle,
	vaccination,
	chemotherapy,
	genetics_heredity,
	clinical_thermometers_and_binaural_stethoscopes,
	prophylaxis_against_malaria,
	pressure_chambers_for_thorax_surgery,

	//inorganic_chemistry
	artificial_fertilizers,
	nitroglycerin,
	chemical_wood_pulp_process,
	rubber_vulcanization,
	catalytic_fermentative_process,

	//organic_chemistry
	synthetic_dye,
	gaslights,
	carbon_soil_enrichment,
	cracking,

	//electricity
	x_rays,
	edison_light_bulb,
	bell_n_greys_telephones,
	electrical_lighting,

	//synthetic_polymers
	rayon,
	bakelite,


	//NAVY INVENTIONS

	//post_nelsonian_thought
	//battleship_column_doctrine
	long_range_fire_tactic,
	speedy_maneuvering_tactic,

	//raider_group_doctrine
	sealane_hunters,
	fast_ship_killer,
	big_ship_competitor,
	standard_raider_group_doctrine,

	//blue_and_brown_water_schools
	blue_water_school,
	brown_water_school,

	//high_sea_battle_fleet
	offensive_attitude,
	defensive_attitude,

	//modern_naval_doctrine
	decisive_firepower_doctrine,
	naval_firepower_doctrine,

	//clipper_design
	building_station_shipyards,

	//steamers
	mechanized_fishing_vessels,
	steamer_automatic_construction_plants,
	steamer_transports,
	commerce_raiders,

	//iron_steamers
	ironclads,
	monitors,

	//steel_steamers
	protected_cruisers,
	pre_dreadnought,

	//steam_turbine_ships
	dreadnoughts,

	//oil_driven_ships
	advanced_cruisers,
	modern_cruisers,
	advanced_battleships,
	modern_battleships,

	//naval_design_bureaus
	//fire_control_systems
	optical_rangefinders,
	gyrostabilized_fire_control,

	//weapon_platforms
	armour_piercing_projectiles,
	armour_piercing_exploding_projectiles,
	armoured_turrets,

	//main_armament
	bomb_guns,
	rifled_guns,
	quick_firing_guns,
	armoured_citadels,
	heavy_caliber_guns,

	//advanced_naval_design
	on_board_recon_aircraft,
	gun_hydraulics,
	heavy_armour_plates,
	large_hull_form,

	//modern_naval_design
	wireless,
	advanced_fire_control,
	fifteen_inch_main_armament,

	//alphabetic_flag_signaling
	//naval_plans
	night_training,
	enemy_plotting_cards,
	target_hit_profile_analysis,

	//naval_statistics
	hit_evaluation_reports,
	empirical_interdiction_reports,
	naval_strike_success_criterium,

	//naval_logistics
	underway_refueling,
	ammunition_conservation_system,
	radio_telegraphy,

	//naval_directionism
	political_lobbying,
	military_political_decision_making,
	local_military_government,

	//naval_integration
	war_gamming,
	combined_arms_planning,

	//the_command_principle
	//naval_professionalism
	academic_training,
	combat_station_training,
	societal_status,

	//naval_decision_making
	independent_command,
	continuous_issuing_of_orders,
	delegated_power,

	//naval_risk_management
	wargames,
	alternative_plans,
	n_saddle_orders,

	//naval_nco_training
	literate_ncos,
	nco_combat_station_training,
	nco_tactical_training,

	//modern_naval_training
	amphibious_warfare_training,
	naval_exercises
};


enum inventionStatus
{
	illegal,
	possible,
	active
};


#endif	// V2INVENTIONS_H_