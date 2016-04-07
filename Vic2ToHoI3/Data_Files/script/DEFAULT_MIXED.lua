
-- #######################################
-- Production Overides the main LUA with country specific ones

-- Handle special Build Unit, the @@@ is the Unit type in string format
-- Build_@@@@@(ic, voProductionData, viUnitsNeeded)

-- #####################
-- Exepected Returns
-- IC = How much IC is left after execution
-- Boolean = Flag indicating weather to execute generic code as well for the building type
-- #####################
-- Build_Underground(ic, voProductionData)
-- Build_NuclearReactor(ic, voProductionData)
-- Build_RocketTest(ic, voProductionData)
-- Build_Industry(ic, voProductionData)
-- Build_CoastalFort(ic, voProductionData)
-- Build_Fort(ic, voProductionData)
-- Build_AntiAir(ic, voProductionData)
-- Build_Radar(ic, voProductionData)
-- Build_Infrastructure(ic, voProductionData)
-- Build_AirBase(ic, voProductionData)

-- must return how much IC is left


-- #######################################
-- Diplomacy Hooks
-- These all must return a numeric score (100 being 100% chance of success)

-- DiploScore_OfferTrade(voDiploScoreObj)
-- DiploScore_Alliance(voDiploScoreObj)
-- DiploScore_InviteToFaction(viScore, ai, actor, recipient, observer)
-- DiploScore_JoinFaction(viScore, minister, faction)
-- DiploScore_JoinFactionGoal(viScore, ai, actor, recipient, observer, goal )
-- DiploScore_InfluenceNation(voDiploScoreObj)
-- DiploScore_Guarantee(voDiploScoreObj)
-- DiploScore_Embargo(voDiploScoreObj)
-- DiploScore_NonAgression(voAI, voCountryOne, voCountryTwo,observer)
-- DiploScore_GiveMilitaryAccess(viScore, voAI, voCountry)
-- DiploScore_CallAlly(viScore, ai, actor, recipient, observer)
-- DiploScore_RequestLendLease(viScore, ai, actor, recipient, observer)
-- DiploScore_Debt(voDiploScoreObj)
-- EvaluateLimitedWar(viScore, minister, target, warDesirability)


--##########################
-- Foreign Minister Hooks

-- ForeignMinister_EvaluateDecision(voDecision, voForeignMinisterData)
-- ForeignMinister_CallAlly(voForeignMinisterData)
-- ForeignMinister_Alignment(voForeignMinisterData)
-- ForeignMinister_MilitaryAccess(voForeignMinisterData)
-- ForeignMinister_Influence(voForeignMinisterData)
-- ForeignMinister_ProposeWar(voForeignMinisterData)

--##########################
-- Politics Minister Hooks

-- HandleMobilization(minister)
-- HandlePuppets(minister)

-- Handle special Law cases, the @@@ is the law group name in string format
-- CallLaw_@@@@@(minister, loCurrentLaw)

-- Changing of Ministers
--    Each method is passed an array of ministers that can be put into the position
-- Call_ChiefOfAir(ministerCountry, vaMinisters)
-- Call_ChiefOfNavy(ministerCountry, vaMinisters)
-- Call_ChiefOfArmy(ministerCountry, vaMinisters)
-- Call_MinisterOfIntelligence(ministerCountry, vaMinisters)
-- Call_ChiefOfStaff(ministerCountry, vaMinisters)
-- Call_ForeignMinister(ministerCountry, vaMinisters)
-- Call_ArmamentMinister(ministerCountry, vaMinisters)
-- Call_MinisterOfSecurity(ministerCountry, vaMinisters)

-- #######################################
-- Intelligence Hooks

-- Intel_Home(voIntelligenceData)
-- Intel_Priority(voIntelligenceData, voIntelCountry)
-- Intel_Mission(voIntelligenceData, voIntelCountry)
-- Intel_Priority_Ally(voIntelligenceData, voIntelCountry)
-- Intel_Mission_Ally(voIntelligenceData, voIntelCountry)

local P = {}
AI_DEFAULT_MIXED = P

-- #######################################
-- Start of Tech Research

-- Tech weights
--   1.0 = 100% the total needs to equal 1.0
function P.TechWeights(voTechnologyData)
	local laArray 
	if (voTechnologyData.PortsTotal > 0) then
		if (voTechnologyData.icTotal > 20 and voTechnologyData.icTotal <= 75 ) then	
			laArray= {
			0.28, -- _RESEARCH_LAND_
			0.17, -- _RESEARCH_LAND_DOC_
			0.15, -- _RESEARCH_AIR_
			0.12, -- _RESEARCH_AIR_DOC_
			0.07, -- _RESEARCH_NAVAL_
			0.04, -- _RESEARCH_NAVAL_DOC_
			0.15, -- _RESEARCH_INDUSTRIAL_
			0.01, -- _RESEARCH_SECRET_
			0.01}; -- _RESEARCH_UNKNOWN_
			
		elseif (voTechnologyData.icTotal <= 20) then
			laArray= {
			0.36, -- _RESEARCH_LAND_
			0.18, -- _RESEARCH_LAND_DOC_
			0.12, -- _RESEARCH_AIR_
			0.08, -- _RESEARCH_AIR_DOC_
			0.04, -- _RESEARCH_NAVAL_
			0.02, -- _RESEARCH_NAVAL_DOC_
			0.19, -- _RESEARCH_INDUSTRIAL_
			0.00, -- _RESEARCH_SECRET_
			0.01}; -- _RESEARCH_UNKNOWN_
		else
		 laArray= {
			0.26, -- _RESEARCH_LAND_
			0.17, -- _RESEARCH_LAND_DOC_
			0.14, -- _RESEARCH_AIR_
			0.10, -- _RESEARCH_AIR_DOC_
			0.11, -- _RESEARCH_NAVAL_
			0.09, -- _RESEARCH_NAVAL_DOC_
			0.12, -- _RESEARCH_INDUSTRIAL_
			0.02, -- _RESEARCH_SECRET_
			0.01}; -- _RESEARCH_UNKNOWN_
		
		end 
	else
		if (voTechnologyData.icTotal > 20 and voTechnologyData.icTotal <= 75 ) then	
			laArray= {
			0.30, -- _RESEARCH_LAND_
			0.18, -- _RESEARCH_LAND_DOC_
			0.17, -- _RESEARCH_AIR_
			0.14, -- _RESEARCH_AIR_DOC_
			0.02, -- _RESEARCH_NAVAL_
			0.01, -- _RESEARCH_NAVAL_DOC_
			0.16, -- _RESEARCH_INDUSTRIAL_
			0.01, -- _RESEARCH_SECRET_
			0.01}; -- _RESEARCH_UNKNOWN_
			
		elseif (voTechnologyData.icTotal <= 20) then
			laArray= {
			0.36, -- _RESEARCH_LAND_
			0.18, -- _RESEARCH_LAND_DOC_
			0.14, -- _RESEARCH_AIR_
			0.11, -- _RESEARCH_AIR_DOC_
			0.00, -- _RESEARCH_NAVAL_
			0.00, -- _RESEARCH_NAVAL_DOC_
			0.20, -- _RESEARCH_INDUSTRIAL_
			0.00, -- _RESEARCH_SECRET_
			0.01}; -- _RESEARCH_UNKNOWN_
		else
		 laArray= {
			0.28, -- _RESEARCH_LAND_
			0.18, -- _RESEARCH_LAND_DOC_
			0.16, -- _RESEARCH_AIR_
			0.14, -- _RESEARCH_AIR_DOC_
			0.04, -- _RESEARCH_NAVAL_
			0.02, -- _RESEARCH_NAVAL_DOC_
			0.15, -- _RESEARCH_INDUSTRIAL_
			0.02, -- _RESEARCH_SECRET_
			0.01}; -- _RESEARCH_UNKNOWN_
		
		end 
	end
		

	return laArray

end

function P.LandTechs(voTechnologyData)
	 local ignoreTech
	 local preferTech
	 
	 if (voTechnologyData.icTotal < 20) then
	 	ignoreTech = {
		{"cavalry_smallarms", 3}, 
		{"cavalry_support", 3},
		{"cavalry_guns", 3}, 
		{"cavalry_at", 3},
		{"armored_car_armour", 0},
		{"armored_car_gun", 0},
		{"paratrooper_infantry", 0},
		{"marine_infantry", 0},
		{"imporved_police_brigade", 0},
		{"desert_warfare_equipment", 0},
		{"jungle_warfare_equipment", 0},
		{"artic_warfare_equipment", 0},
		{"amphibious_warfare_equipment", 0},
		{"airborne_warfare_equipment", 0},
		{"lighttank_brigade", 0},
		{"lighttank_gun", 0},
		{"lighttank_engine", 0},
		{"lighttank_armour", 0},
		{"lighttank_reliability", 0},
		{"tank_brigade", 0},
		{"tank_gun", 0},
		{"tank_engine", 0},
		{"tank_armour", 0},
		{"tank_reliability", 0},
		{"heavy_tank_brigade", 0},
		{"heavy_tank_gun", 0},
		{"heavy_tank_engine", 0},
		{"heavy_tank_armour", 0},
		{"heavy_tank_reliability", 0},
		{"SP_brigade", 0},
		{"mechanised_infantry", 0},
		{"super_heavy_tank_brigade", 0},
		{"super_heavy_tank_gun", 0},
		{"super_heavy_tank_engine", 0},
		{"super_heavy_tank_armour", 0},
		{"super_heavy_tank_reliability", 0},
		{"rocket_art", 0},
		{"rocket_art_ammo", 0},
		{"rocket_carriage_sights", 0}};	

	 preferTech = {
		"militia_smallarms",
		"militia_support",
		"militia_guns",
		"militia_at",
		"infantry_activation",
		"smallarms_technology",
		"infantry_support",
		"infantry_guns",
		"infantry_at",
		"art_barrell_ammo",
		"art_carriage_sights"};
	 
	 elseif(voTechnologyData.icTotal < 75) then
	 	ignoreTech = {
		{"cavalry_smallarms", 3}, 
		{"cavalry_support", 3},
		{"cavalry_guns", 3}, 
		{"cavalry_at", 3},
		{"militia_smallarms", 1},
		{"militia_support", 1},
		{"militia_guns", 1},
		{"militia_at", 1},
		{"armored_car_armour", 0},
		{"armored_car_gun", 0},
		{"marine_infantry", 0},
		{"imporved_police_brigade", 0},
		{"lighttank_brigade", 0},
		{"lighttank_gun", 0},
		{"lighttank_engine", 0},
		{"lighttank_armour", 0},
		{"lighttank_reliability", 0},
		{"tank_brigade", 0},
		{"tank_gun", 0},
		{"tank_engine", 0},
		{"tank_armour", 0},
		{"tank_reliability", 0},
		{"heavy_tank_brigade", 0},
		{"heavy_tank_gun", 0},
		{"heavy_tank_engine", 0},
		{"heavy_tank_armour", 0},
		{"heavy_tank_reliability", 0},
		{"SP_brigade", 0},
		{"mechanised_infantry", 0},
		{"super_heavy_tank_brigade", 0},
		{"super_heavy_tank_gun", 0},
		{"super_heavy_tank_engine", 0},
		{"super_heavy_tank_armour", 0},
		{"super_heavy_tank_reliability", 0},
		{"rocket_art", 0},
		{"rocket_art_ammo", 0},
		{"rocket_carriage_sights", 0}};	

	 preferTech = {
		"militia_smallarms",
		"militia_support",
		"militia_guns",
		"militia_at",
		"infantry_activation",
		"smallarms_technology",
		"infantry_support",
		"infantry_guns",
		"infantry_at",
		"art_barrell_ammo",
		"art_carriage_sights"};
	 else
	 	ignoreTech = {
		{"cavalry_smallarms", 3}, 
		{"cavalry_support", 3},
		{"cavalry_guns", 3}, 
		{"cavalry_at", 3},
		{"militia_smallarms", 1},
		{"militia_support", 1},
		{"militia_guns", 1},
		{"militia_at", 1},
		{"armored_car_armour", 0},
		{"armored_car_gun", 0},
		{"imporved_police_brigade", 0},
		{"heavy_tank_brigade", 0},
		{"heavy_tank_gun", 0},
		{"heavy_tank_engine", 0},
		{"heavy_tank_armour", 0},
		{"heavy_tank_reliability", 0},
		{"super_heavy_tank_brigade", 0},
		{"super_heavy_tank_gun", 0},
		{"super_heavy_tank_engine", 0},
		{"super_heavy_tank_armour", 0},
		{"super_heavy_tank_reliability", 0},
		{"rocket_art", 0},
		{"rocket_art_ammo", 0},
		{"rocket_carriage_sights", 0}};	

	 preferTech = {
	 	"lighttank_brigade",
		"militia_smallarms",
		"militia_support",
		"militia_guns",
		"militia_at",
		"infantry_activation",
		"smallarms_technology",
		"infantry_support",
		"infantry_guns",
		"infantry_at",
		"art_barrell_ammo",
		"art_carriage_sights"};
	 end


		
	return ignoreTech, preferTech
end


function P.LandDoctrinesTechs(voTechnologyData)

	 local ignoreTech
	 local preferTech
	 
	 if (voTechnologyData.icTotal < 20) then
	 	 ignoreTech = {
		{"mobile_warfare", 0},
		{"elastic_defence", 0},
		{"spearhead_doctrine", 0},
		{"schwerpunkt", 0},
		{"blitzkrieg", 0},
		{"superior_firepower", 0},
		{"operational_level_command_structure", 0},
		{"tactical_command_structure", 0},
		{"integrated_support_doctrine", 0},
		{"mechanized_offensive", 0},
		{"combined_arms_warfare", 0},
		{"special_forces", 1}};
		
		 preferTech = {
		"infantry_warfare",
		"operational_level_organisation"}	
		
	 elseif(voTechnologyData.icTotal < 75) then
	 	 ignoreTech = {
		{"mobile_warfare", 0},
		{"elastic_defence", 0},
		{"spearhead_doctrine", 0},
		{"schwerpunkt", 0},
		{"blitzkrieg", 0},
		{"superior_firepower", 0},
		{"operational_level_command_structure", 1},
		{"tactical_command_structure", 0},
		{"integrated_support_doctrine", 0},
		{"mechanized_offensive", 0},
		{"combined_arms_warfare", 0},
		{"special_forces", 3}};
		
		 preferTech = {
		"infantry_warfare",
		"mass_assault",
		"operational_level_organisation",
		"peoples_army"}	
	 else
	 	ignoreTech = {
		{"mobile_warfare", 3},
		{"elastic_defence", 3},
		{"spearhead_doctrine", 3},
		{"schwerpunkt", 3},
		{"blitzkrieg", 3},
		{"superior_firepower", 0},
		{"mechanized_offensive", 3},
		{"combined_arms_warfare", 3}};
		
		 preferTech = {
		"infantry_warfare",
		"mass_assault",
		"operational_level_organisation",
		"special_forces",
		"peoples_army"}		 
	 end
	 

		
	return ignoreTech, preferTech
end

function P.AirTechs(voTechnologyData)
	local preferTech
	local ignoreTech
	 if (voTechnologyData.icTotal < 20) then
	 	ignoreTech = {
		{"basic_four_engine_airframe", 0},
		{"basic_strategic_bomber", 0},
		{"large_fueltank", 0},
		{"four_engine_airframe", 0},
		{"strategic_bomber_armament", 0},
		{"cargo_hold", 0},
		{"large_bomb", 0},
		{"advanced_aircraft_design", 0},
		{"small_airsearch_radar", 0},
		{"medium_airsearch_radar", 0},
		{"large_airsearch_radar", 0},
		{"small_navagation_radar", 0},
		{"medium_navagation_radar", 0},
		{"large_navagation_radar", 0},
		{"rocket_interceptor_tech", 0},
		{"basic_medium_fueltank", 0},
		{"twin_engine_aircraft_design", 0},
		{"basic_twin_engine_airframe", 0},
		{"basic_bomb", 0},
		{"drop_tanks", 0},
		{"jet_engine", 0}};

	preferTech = {
		"single_engine_aircraft_design",
		"basic_aeroengine",
		"basic_small_fueltank",
		"basic_single_engine_airframe",
		"basic_aircraft_machinegun"};
		
		
	 elseif(voTechnologyData.icTotal < 75) then
	 		ignoreTech = {
		{"basic_four_engine_airframe", 0},
		{"basic_strategic_bomber", 0},
		{"large_fueltank", 0},
		{"four_engine_airframe", 0},
		{"strategic_bomber_armament", 0},
		{"cargo_hold", 0},
		{"large_bomb", 0},
		{"advanced_aircraft_design", 0},
		{"small_airsearch_radar", 0},
		{"medium_airsearch_radar", 0},
		{"large_airsearch_radar", 0},
		{"small_navagation_radar", 0},
		{"medium_navagation_radar", 0},
		{"large_navagation_radar", 0},
		{"rocket_interceptor_tech", 0},
		{"drop_tanks", 0},
		{"jet_engine", 0}};

	preferTech = {
		"single_engine_aircraft_design",
		"basic_aeroengine",
		"basic_small_fueltank",
		"basic_single_engine_airframe",
		"basic_aircraft_machinegun"}; 
	 else 
	 	ignoreTech = {
		{"basic_four_engine_airframe", 0},
		{"basic_strategic_bomber", 0},
		{"large_fueltank", 0},
		{"four_engine_airframe", 0},
		{"strategic_bomber_armament", 0},
		{"cargo_hold", 0},
		{"large_bomb", 0},	
		{"rocket_interceptor_tech", 0},
		{"drop_tanks", 0}};

	preferTech = {
		"single_engine_aircraft_design",
		"basic_aeroengine",
		"basic_small_fueltank",
		"basic_single_engine_airframe",
		"basic_aircraft_machinegun",
		"multi_role_fighter_development",
		"twin_engine_aircraft_design",
		"basic_medium_fueltank",
		"basic_twin_engine_airframe",
		"basic_bomb"}; 
	 end
		
	return ignoreTech, preferTech
end

function P.AirDoctrineTechs(voTechnologyData)
	local preferTech
	local ignoreTech
	
	 if (voTechnologyData.icTotal < 20) then
	 	 ignoreTech = {
		{"forward_air_control", 0},
		{"battlefield_interdiction", 0},
		{"bomber_targerting_focus", 0},
		{"fighter_targerting_focus", 0}, 
		{"heavy_bomber_pilot_training", 0},
		{"heavy_bomber_groundcrew_training", 0},
		{"strategic_bombardment_tactics", 0},
		{"airborne_assault_tactics", 0},
		{"cas_pilot_training", 0},
		{"cas_groundcrew_training", 0},
		{"ground_attack_tactics", 0},
		{"tac_pilot_training", 0},
		{"interdiction_tactics", 0},
		{"tactical_air_command", 0},
		{"strategic_air_command", 0}};

	 preferTech = {
		"fighter_pilot_training",
		"interception_tactics"};	
		
		
	 elseif(voTechnologyData.icTotal < 75) then
	 	 	 ignoreTech = {
		{"forward_air_control", 0},
		{"battlefield_interdiction", 0},
		{"bomber_targerting_focus", 0},
		{"fighter_targerting_focus", 0}, 
		{"heavy_bomber_pilot_training", 0},
		{"heavy_bomber_groundcrew_training", 0},
		{"strategic_bombardment_tactics", 0},
		{"airborne_assault_tactics", 0},
		{"strategic_air_command", 0}};

	 preferTech = {
		"fighter_pilot_training",
		"interception_tactics",
		"cas_pilot_training",
		"tac_pilot_training"};	
	 else
	 	ignoreTech = {
		{"forward_air_control", 0},
		{"bomber_targerting_focus", 0},
		{"fighter_targerting_focus", 0}, 
		{"heavy_bomber_pilot_training", 0},
		{"heavy_bomber_groundcrew_training", 0},
		{"strategic_bombardment_tactics", 0},
		{"strategic_air_command", 0}};

	 preferTech = {
		"fighter_pilot_training",
		"interception_tactics",
		"cas_pilot_training",
		"cas_groundcrew_training",
		"ground_attack_tactics",
		"tac_pilot_training",
		"interdiction_tactics",
		"tactical_air_command"};	
	 end		
		
	return ignoreTech, preferTech
end

function P.NavalTechs(voTechnologyData)
	local ignoreTech
	local preferTech
	
	 if (voTechnologyData.icTotal < 20) then
	 	ignoreTech = {
		{"lightcruiser_technology", 0},
		{"lightcruiser_armament", 0},
		{"lightcruiser_antiaircraft", 0},
		{"lightcruiser_engine", 0},
		{"lightcruiser_armour", 0},
		{"heavycruiser_technology", 0},
		{"heavycruiser_armament", 0},
		{"heavycruiser_antiaircraft", 0},
		{"heavycruiser_engine", 0},
		{"heavycruiser_armour", 0},
		{"battlecruiser_technology", 0},
		{"battleship_technology", 0},
		{"capitalship_armament", 0},
		{"battlecruiser_antiaircraft", 0},
		{"battlecruiser_engine", 0},
		{"battlecruiser_armour", 0},
		{"battleship_antiaircraft", 0},
		{"battleship_engine", 0},
		{"battleship_armour", 0},
		{"super_heavy_battleship_technology", 0},
		{"cag_development", 0},
		{"escort_carrier_technology", 0},
		{"carrier_technology", 0},
		{"carrier_antiaircraft", 0},
		{"carrier_engine", 0},
		{"carrier_armour", 0},
		{"carrier_hanger", 0},
		{"largewarship_radar", 0}};

	preferTech = {
		"destroyer_technology",
		"destroyer_armament",
		"destroyer_antiaircraft",
		"destroyer_engine",
		"destroyer_armour"};
	 elseif(voTechnologyData.icTotal < 75) then
	 	 	ignoreTech = {
		{"lightcruiser_technology", 0},
		{"lightcruiser_armament", 0},
		{"lightcruiser_antiaircraft", 2},
		{"lightcruiser_engine", 0},
		{"lightcruiser_armour", 0},
		{"heavycruiser_technology", 0},
		{"heavycruiser_armament", 0},
		{"heavycruiser_antiaircraft", 0},
		{"heavycruiser_engine", 0},
		{"heavycruiser_armour", 0},
		{"battlecruiser_technology", 2},
		{"capitalship_armament", 3},
		{"battlecruiser_antiaircraft", 2},
		{"battlecruiser_engine", 3},
		{"battlecruiser_armour", 3},
		{"battleship_antiaircraft", 2},
		{"battleship_engine", 0},
		{"battleship_armour", 0},
		{"super_heavy_battleship_technology", 0},
		{"carrier_antiaircraft", 2},
		{"carrier_engine", 1},
		{"carrier_armour", 1},
		{"carrier_hanger", 1},
		{"largewarship_radar", 0}};

	preferTech = {
		"destroyer_technology",
		"destroyer_armament",
		"destroyer_antiaircraft",
		"destroyer_engine",
		"destroyer_armour"};
	 else
	 	 ignoreTech = {
	 	{"lightcruiser_technology", 0},
		{"lightcruiser_armament", 0},
		{"lightcruiser_antiaircraft", 2},
		{"lightcruiser_engine", 0},
		{"lightcruiser_armour", 0},
	 	{"heavycruiser_technology", 0},
	 	{"battleship_antiaircraft", 0},
		{"battleship_engine", 0},
		{"battleship_armour", 0},
		{"battleship_technology", 0},
		{"heavycruiser_armament", 0},
		{"heavycruiser_antiaircraft", 2},
		{"heavycruiser_engine", 0},
		{"heavycruiser_armour", 0},
		{"super_heavy_battleship_technology", 0},
		{"largewarship_radar", 1}};

	preferTech = {
		"destroyer_technology",
		"destroyer_antiaircraft",
		"destroyer_engine",
		"carrier_technology",
		"carrier_antiaircraft",
		"carrier_engine",
		"carrier_armour",
		"carrier_hanger"};
	 end

		
		
	return ignoreTech, preferTech
end

function P.NavalDoctrineTechs(voTechnologyData)


	local preferTech
	local ignoreTech
	 if (voTechnologyData.icTotal < 20) then
	 
	 ignoreTech = {
		{"fleet_auxiliary_carrier_doctrine", 0},
		{"light_cruiser_escort_role", 0},
		{"light_cruiser_crew_training", 0},
		{"carrier_group_doctrine", 0},
		{"carrier_crew_training", 0},
		{"carrier_task_force", 0},
		{"naval_underway_repleshment", 0},
		{"radar_training", 0},
		{"battlefleet_concentration_doctrine", 0},
		{"battleship_crew_training", 0}};

	 preferTech = {
	 	"sea_lane_defence",
		"destroyer_escort_role",
		"destroyer_crew_training"};	
	 elseif(voTechnologyData.icTotal < 75) then
	 	 
	 ignoreTech = {
		{"fleet_auxiliary_carrier_doctrine", 0},
		{"light_cruiser_escort_role", 0},
		{"light_cruiser_crew_training", 0},
		{"carrier_group_doctrine", 0},
		{"carrier_crew_training", 0},
		{"carrier_task_force", 0},
		{"naval_underway_repleshment", 0},
		{"radar_training", 0}};

	 preferTech = {
		"sea_lane_defence",
		"destroyer_escort_role",
		"destroyer_crew_training",
		"commander_decision_making"};	
	 else
	 	 
	 ignoreTech = {
		{"naval_underway_repleshment", 0}};

	 preferTech = {
		"sea_lane_defence",
		"destroyer_escort_role",
		"destroyer_crew_training",
		"commerce_defence",
		"basing",
		"carrier_task_force",
		"carrier_group_doctrine",
		"commander_decision_making"};	
	 end
	
		
	return ignoreTech, preferTech
end

function P.IndustrialTechs(voTechnologyData)
 	local ignoreTech
	local preferTech
	if (voTechnologyData.icTotal < 20) then
	 	 ignoreTech = {
		{"oil_to_coal_conversion", 0},
		{"heavy_aa_guns", 0},
		{"radio_detection_equipment", 0},
		{"radar", 0},
		{"decryption_machine", 0},
		{"encryption_machine", 0},
		{"rocket_tests", 0},
		{"rocket_engine", 0},
		{"theorical_jet_engine", 0},
		{"atomic_research", 0},
		{"nuclear_research", 0},
		{"isotope_seperation", 0},
		{"civil_nuclear_research", 0},
		{"oil_refinning", 0},
		{"steel_production", 0},
		{"raremetal_refinning_techniques", 0},
		{"coal_processing_technologies", 0}};

	 preferTech = {
		"agriculture",
		"industral_production",
		"industral_efficiency",
		"supply_production",
		"education"};
	elseif(voTechnologyData.icTotal < 100) then
	 	 ignoreTech = {
		{"oil_to_coal_conversion", 0},
		{"heavy_aa_guns", 3},
		{"radio_detection_equipment", 1},
		{"decryption_machine", 1},
		{"encryption_machine", 1},
		{"rocket_tests", 0},
		{"rocket_engine", 0},
		{"theorical_jet_engine", 0},
		{"atomic_research", 0},
		{"nuclear_research", 0},
		{"isotope_seperation", 0},
		{"civil_nuclear_research", 0},
		{"oil_refinning", 0},
		{"steel_production", 0},
		{"raremetal_refinning_techniques", 0},
		{"coal_processing_technologies", 0}};

	 preferTech = {
		"agriculture",
		"industral_production",
		"industral_efficiency",
		"supply_production",
		"education"};	 
	else
	 	 ignoreTech = {
		{"oil_to_coal_conversion", 1},
		{"heavy_aa_guns", 5},
		{"radio_detection_equipment", 1},
		{"decryption_machine", 2},
		{"encryption_machine", 2},
		{"atomic_research", 0},
		{"nuclear_research", 0},
		{"isotope_seperation", 0},
		{"civil_nuclear_research", 0},
		{"oil_refinning", 0},
		{"steel_production", 0},
		{"raremetal_refinning_techniques", 0},
		{"coal_processing_technologies", 0}};

	 preferTech = {
		"agriculture",
		"industral_production",
		"industral_efficiency",
		"supply_production",
		"education"};	 
	end

	 
		
	return ignoreTech, preferTech
end

function P.SecretWeaponTechs(voTechnologyData)
--	local ignoreTech = {"all"}
	local 	 preferTech = {
		"radar_guided_missile",
		"radar_guided_bomb",
		"sam",
		"aam"};	
		
	return nil, preferTech
end

function P.OtherTechs(voTechnologyData)
	local ignoreTech = {
		{"naval_engineering_research", 0},
		{"submarine_engineering_research", 0},
		{"aeronautic_engineering_research", 0},
		{"rocket_science_research", 0},
		{"chemical_engineering_research", 0},
		{"nuclear_physics_research", 0},
		{"jetengine_research", 0},
		{"mechanicalengineering_research", 1},
		{"automotive_research", 1},
		{"electornicegineering_research", 0},
		{"artillery_research", 0},
		{"mobile_research", 1},
		{"militia_research", 0},
		{"infantry_research", 1},
		{"civil_defence", 2}};

	return ignoreTech, nil
end

-- END OF TECH RESEARCH OVERIDES
-- #######################################

-- Land Brigades vs Air Units ratio
--   If Air Ratio is met AI will shift its Air IC to build land units
function P.LandToAirRatio(voProductionData)
	local laArray = {
		10, -- Land Briages
		3}; -- Air
		
	return laArray
end

function P.ProductionWeights(voProductionData)
	local laArray
	
	
		-- Check to see if manpower is to low
	-- More than 30 brigades so build stuff that does not use manpower


	
	if (voProductionData.ManpowerTotal < 30 and voProductionData.LandCountTotal > 30)
	or voProductionData.ManpowerTotal < 10 then
		laArray = {
			0.0, -- Land
			0.45, -- Air
			0.40, -- Sea
			0.15}; -- Other	
	elseif voProductionData.IsAtWar then
		if (voProductionData.icTotal ) < 20 then
			laArray = {
			0.85, -- Land
			0.15, -- Air
			0.0, -- Sea
			0.0}; -- Other
		elseif (voProductionData.icTotal ) < 40 then
			laArray = {
			0.70, -- Land
			0.25, -- Air
			0.00, -- Sea
			0.0}; -- Other
		elseif (voProductionData.icTotal ) < 75 then
			laArray = {
			0.62, -- Land
			0.25, -- Air
			0.10, -- Sea
			0.02}; -- Other
		else
			laArray = {
			0.57, -- Land
			0.28, -- Air
			0.10, -- Sea
			0.05}; -- Other
		end		
			
	else
		if (voProductionData.icTotal ) < 20 then
			laArray = {
			0.80, -- Land
			0.15, -- Air
			0.05, -- Sea
			0.0}; -- Other
		elseif (voProductionData.icTotal ) < 40 then
			laArray = {
			0.65, -- Land
			0.23, -- Air
			0.08, -- Sea
			0.04}; -- Other
		elseif (voProductionData.icTotal ) < 75 then
			laArray = {
			0.52, -- Land
			0.31, -- Air
			0.12, -- Sea
			0.05}; -- Other
		else
			laArray = {
			0.48, -- Land
			0.31, -- Air
			0.13, -- Sea
			0.08}; -- Other
		end
	end
	
	return laArray

end

-- Land ratio distribution
function P.LandRatio(voProductionData)
	local laArray

	if (voProductionData.icTotal ) < 20 then
			laArray = {
				militia_brigade = 6,
				garrison_brigade = 0,
				infantry_brigade = 2,
				motorized_brigade = 0,
				mechanized_brigade = 0,
				light_armor_brigade = 0,
				armor_brigade = 0,
				heavy_armor_brigade = 0};
		elseif (voProductionData.icTotal ) < 40 then
			laArray = {
				militia_brigade = 1,
				garrison_brigade = 0,
				infantry_brigade = 6,
				motorized_brigade = 0,
				mechanized_brigade = 0,
				light_armor_brigade = 0,
				armor_brigade = 0,
				heavy_armor_brigade = 0};
		elseif (voProductionData.icTotal ) < 75 then
			laArray = {
				militia_brigade = 0,
				garrison_brigade = 2,
				infantry_brigade = 30,
				motorized_brigade = 1,
				mechanized_brigade = 0,
				light_armor_brigade = 0,
				armor_brigade = 0,
				heavy_armor_brigade = 0};
		else
			laArray = {
				militia_brigade = 0,
				garrison_brigade = 2,
				infantry_brigade = 35,
				motorized_brigade = 2,
				mechanized_brigade = 1,
				light_armor_brigade = 1.25,
				armor_brigade = 1,
				heavy_armor_brigade = 0};
		end

	return laArray
end

-- Special Forces ratio distribution
function P.SpecialForcesRatio(voProductionData)
	local laRatio = {
		30, -- Land
		1}; -- Special Force Unit

	local laUnits = {
		marine_brigade = 2,
		bergsjaeger_brigade = 3,
		paratrooper_brigade = 0};
	
	return laRatio, laUnits	
end

-- Which units should get 1 more Support unit with Superior Firepower tech
--- Firepower is on ignore list but just in case human player researches it
function P.FirePower(voProductionData)
	local laArray = {"infantry_brigade"};
		
	return laArray
end

-- Air ratio distribution
function P.AirRatio(voProductionData)

	local laArray

	if (voProductionData.icTotal ) < 20 then
			laArray = {
			interceptor = 1,
			multi_role = 0,
			cas = 0,
			tactical_bomber = 0,
			naval_bomber = 0,
			strategic_bomber = 0};
		elseif (voProductionData.icTotal ) < 40 then
			laArray = {
			interceptor = 2,
			multi_role = 0,
			cas = 1,
			tactical_bomber = 0,
			naval_bomber = 0,
			strategic_bomber = 0};
		elseif (voProductionData.icTotal ) < 75 then
			laArray = {
			interceptor = 5,
			multi_role = 0,
			cas = 2,
			tactical_bomber = 2,
			naval_bomber = 0,
			strategic_bomber = 0};
		else
			laArray = {
			interceptor = 8,
			multi_role = 2,
			cas = 3,
			tactical_bomber = 4,
			naval_bomber = 0,
			strategic_bomber = 0};
		end
		

	return laArray
end

-- Flying Bomb/Rocket distribution against total Air Power
function P.RocketRatio(voProductionData)
	local laArray = {
		10, -- Air
		1}; -- Bomb/Rockety
	
	return laArray
end

-- Naval ratio distribution
function P.NavalRatio(voProductionData)
	local laArray
		if (voProductionData.icTotal ) < 20 then
			laArray = {
				destroyer = 8,
				submarine = 2,
				nuclear_submarine = 0,
				light_cruiser = 0,
				heavy_cruiser = 0,
				battleship = 0,
				escort_carrier = 1,
				carrier = 0};
		elseif (voProductionData.icTotal ) < 40 then
			laArray = {
				destroyer = 8,
				submarine = 3,
				nuclear_submarine = 0,
				light_cruiser = 0,
				heavy_cruiser = 0,
				battleship = 0,
				escort_carrier = 0.5,
				carrier = 1};
		elseif (voProductionData.icTotal ) < 75 then
			laArray = {
				destroyer = 8,
				submarine = 3,
				nuclear_submarine = 0,
				light_cruiser = 0,
				heavy_cruiser = 0,
				battleship = 1,
				battlecruiser = 1,
				escort_carrier = 0.75,
				carrier = 2};
		else
			laArray = {
				destroyer = 8,
				submarine = 2,
				nuclear_submarine = 0.5,
				light_cruiser = 0,
				heavy_cruiser = 0,
				battleship = 0,
				battlecruiser= 1,
				escort_carrier = 1,
				carrier = 2};
		end
	
	return laArray
end

-- Transport to Land unit distribution
function P.TransportLandRatio(voProductionData)
	local laArray = {
		30, -- Land
		2,  -- transport
		1}  -- invasion craft
  
	return laArray
end

-- Convoy Ratio control
--- NOTE: If goverment is in Exile these parms are ignored
function P.ConvoyRatio(voProductionData)
	local laArray = {
		5, -- Percentage extra (adds to 100 percent so if you put 10 it will make it 110% of needed amount)
		10, -- If Percentage extra is less than this it will force it up to the amount entered
		20, -- If Percentage extra is greater than this it will force it down to this
		5} -- Escort to Convoy Ratio (Number indicates how many convoys needed to build 1 escort)
  
	return laArray
end

-- Do not build Rocket Sites
function P.Build_RocketTest(ic, voProductionData)
	-- Only build Rocket Sites if 1940 or greater
	if voProductionData.Year <= 1939 then
		return ic, false
	else
		-- Small Country so wait till 1942 before we worry about this
		if (voProductionData.icTotal < 75 and voProductionData.Year <= 1942) then
			return ic, false
		end
	end
	
	return ic, true	
end

return AI_DEFAULT_MIXED
