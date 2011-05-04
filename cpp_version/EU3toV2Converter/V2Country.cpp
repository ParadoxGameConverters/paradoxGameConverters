#include "V2Country.h"
#include "tempFuncs.h"


void V2Country::init(string newTag, string newCountryFile, vector<int> newParties)
{
	tag			= newTag;
	countryFile	= newCountryFile;
	parties		= newParties;

	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		inventions[i] = illegal;
	}
}


void V2Country::setSourceCountryIndex(int oldCountryIndex)
{
	sourceCountryIndex	= oldCountryIndex;
}


string V2Country::getTag()
{
	return tag;
}


void V2Country::addState(V2State newState)
{
	states.push_back(newState);
}


void V2Country::setCapital(int newCapital)
{
	capital = newCapital;
}


void V2Country::setcivilized(bool isIt)
{
	civilized = isIt;
}


void V2Country::setPrimaryCulture(string culture)
{
	primaryCulture = culture;
}


void V2Country::addAcceptedCulture(string culture)
{
	acceptedCultures.push_back(culture);
}


void V2Country::setReligion(string newReligion)
{
	religion = newReligion;
}


void V2Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "	{\n");
	if (capital > 0)
	{
		fprintf(output, "	capital=%d\n", capital);
	}
	outputTech(output);
	outputCountryHeader(output);
	fprintf(output, "	ruling_party=%d\n", parties[0]);
	for (unsigned int i = 0; i < parties.size(); i++)
	{
		fprintf(output, "	active_party=%d\n", parties[i]);
	}
	fprintf(output, "	religion=\"%s\"\n", religion.c_str());
	outputCountryHeader2(output);
	if (primaryCulture.size() > 0)
	{
		fprintf(output, "	primary_culture=\"%s\"\n", primaryCulture.c_str());
	}
	if (acceptedCultures.size() > 0)
	{
		fprintf(output, "	culture=\n");
		fprintf(output, "	{\n");
		for(unsigned int i = 0; i < acceptedCultures.size(); i++)
		{
			fprintf(output, "		\"%s\"\n", acceptedCultures[i].c_str());
		}
		fprintf(output, "	}\n");
	}
	outputInventions(output);
	fprintf(output, "	prestige=%f\n", prestige);
	outputCountryMiddle(output);
	if (civilized)
	{
		fprintf(output, "	civilized=yes\n");
	}
	else
	{
		fprintf(output, "	civilized=no\n");
	}
	for(unsigned int i = 0; i < states.size(); i++)
	{
		states[i].output(output);
	}
	fprintf(output, "}\n");
}


int V2Country::getSourceCountryIndex()
{
	return sourceCountryIndex;
}


void V2Country::setPrestige(double newPrestige)
{
	prestige = newPrestige;
}


void V2Country::setArmyTech(double newTechLevel)
{
	if (newTechLevel > 0)
	{
		armyTech = (int)newTechLevel;
	}
	
	if (newTechLevel > 7)
	{
		inventions[telegraph_coordination]		= possible;
		inventions[mobilization_time_tables]	= possible;
		inventions[personnel_category_tables]	= possible;
	}

	if (newTechLevel > 7.25)
	{
		inventions[telegraph_coordination]		= active;
		inventions[mobilization_time_tables]	= possible;
		inventions[personnel_category_tables]	= possible;
	}

	if (newTechLevel > 7.5)
	{
		inventions[mobilization_time_tables]	= active;
		inventions[personnel_category_tables]	= possible;
	}

	if (newTechLevel > 7.75)
	{
		inventions[personnel_category_tables]	= active;
	}

	if (newTechLevel > 8)
	{
		inventions[field_fortifications]			= possible;
		inventions[railroad_transport_groups]	= possible;
		inventions[signal_detachments]			= possible;
	}

	if (newTechLevel > 8.25)
	{
		inventions[field_fortifications]			= active;
		inventions[railroad_transport_groups]	= possible;
		inventions[signal_detachments]			= possible;
	}

	if (newTechLevel > 8.5)
	{
		inventions[railroad_transport_groups]	= active;
		inventions[signal_detachments]			= possible;
	}

	if (newTechLevel > 8.75)
	{
		inventions[signal_detachments]			= active;
	}

	if (newTechLevel > 9)
	{
		inventions[army_academic_training]	= possible;
		inventions[field_training]				= possible;
		inventions[army_societal_status]		= possible;
	}
	
	if (newTechLevel > 9.25)
	{
		inventions[army_academic_training]	= active;
		inventions[field_training]				= possible;
		inventions[army_societal_status]		= possible;
	}
	
	if (newTechLevel > 9.5)
	{
		inventions[field_training]				= active;
		inventions[army_societal_status]		= possible;
	}
	
	if (newTechLevel > 9.75)
	{
		inventions[army_societal_status]		= active;
	}
}


void V2Country::setNavyTech(double newTechLevel)
{
	if (newTechLevel > 0)
	{
		navyTech = (int)newTechLevel;
	}

	if (newTechLevel > 0)
	{
		inventions[building_station_shipyards]	= active;
	}
	
	if (newTechLevel > 5)
	{
		inventions[mechanized_fishing_vessels]					= possible;
		inventions[steamer_automatic_construction_plants]	= possible;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}

	if (newTechLevel > 5.2)
	{
		inventions[mechanized_fishing_vessels]					= active;
		inventions[steamer_automatic_construction_plants]	= possible;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}
	
	if (newTechLevel > 5.4)
	{
		inventions[steamer_automatic_construction_plants]	= active;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}
	
	if (newTechLevel > 5.6)
	{
		inventions[steamer_transports]							= active;
		inventions[commerce_raiders]								= possible;
	}
	
	if (newTechLevel > 5.8)
	{
		inventions[commerce_raiders]								= active;
	}

	if (newTechLevel > 6)
	{
		inventions[optical_rangefinders]							= possible;
		inventions[gyrostabilized_fire_control]				= possible;
		inventions[armour_piercing_projectiles]				= possible;
		inventions[armour_piercing_exploding_projectiles]	= possible;
		inventions[armoured_turrets]								= possible;
	}

	if (newTechLevel > 6 + (1/6))
	{
		inventions[optical_rangefinders]							= active;
		inventions[gyrostabilized_fire_control]				= possible;
		inventions[armour_piercing_projectiles]				= possible;
		inventions[armour_piercing_exploding_projectiles]	= possible;
		inventions[armoured_turrets]								= possible;
	}
	
	if (newTechLevel > 6 + (2/6))
	{
		inventions[gyrostabilized_fire_control]				= active;
		inventions[armour_piercing_projectiles]				= possible;
		inventions[armour_piercing_exploding_projectiles]	= possible;
		inventions[armoured_turrets]								= possible;
	}
	
	if (newTechLevel > 6 + (3/6))
	{
		inventions[armour_piercing_projectiles]				= active;
		inventions[armour_piercing_exploding_projectiles]	= possible;
		inventions[armoured_turrets]								= possible;
	}
	
	if (newTechLevel > 6 + (4/6))
	{
		inventions[armour_piercing_exploding_projectiles]	= active;
		inventions[armoured_turrets]								= possible;
	}
	
	if (newTechLevel > 6 + (5/6))
	{
		inventions[armoured_turrets]								= active;
	}
	
	if (newTechLevel > 7)
	{
		inventions[night_training]						= possible;
		inventions[enemy_plotting_cards]				= possible;
		inventions[target_hit_profile_analysis]	= possible;
	}

	if (newTechLevel > 7.25)
	{
		inventions[night_training]						= active;
		inventions[enemy_plotting_cards]				= possible;
		inventions[target_hit_profile_analysis]	= possible;
	}
	
	if (newTechLevel > 7.5)
	{
		inventions[enemy_plotting_cards]				= active;
		inventions[target_hit_profile_analysis]	= possible;
	}
	
	if (newTechLevel > 7.75)
	{
		inventions[target_hit_profile_analysis]	= active;
	}
	
	if (newTechLevel > 8)
	{
		inventions[academic_training]			= possible;
		inventions[combat_station_training]	= possible;
		inventions[societal_status]			= possible;
	}

	if (newTechLevel > 8.25)
	{
		inventions[academic_training]			= active;
		inventions[combat_station_training]	= possible;
		inventions[societal_status]			= possible;
	}
	
	if (newTechLevel > 8.5)
	{
		inventions[combat_station_training]	= active;
		inventions[societal_status]			= possible;
	}
	
	if (newTechLevel > 8.75)
	{
		inventions[societal_status]			= active;
	}
	
	if (newTechLevel > 9)
	{
		inventions[long_range_fire_tactic]		= possible;
		inventions[speedy_maneuvering_tactic]	= possible;
	}

	if (newTechLevel > 9 + (1/3))
	{
		inventions[long_range_fire_tactic]		= active;
		inventions[speedy_maneuvering_tactic]	= possible;
	}
	
	if (newTechLevel > 9 + (2/3))
	{
		inventions[speedy_maneuvering_tactic]	= active;
	}
	}


void V2Country::setCommerceTech(double newTechLevel)
{
	if (newTechLevel > 0)
	{
		commerceTech = (int)newTechLevel;
	}

	if (newTechLevel > 4)
	{
		inventions[john_ramsay_mcculloch]	= possible;
		inventions[nassau_william_sr]			= possible;
		inventions[james_mill]					= possible;
	}
	
	if (newTechLevel > 4.25)
	{
		inventions[john_ramsay_mcculloch]	= active;
		inventions[nassau_william_sr]			= possible;
		inventions[james_mill]					= possible;
	}
	
	if (newTechLevel > 4.5)
	{
		inventions[nassau_william_sr]			= active;
		inventions[james_mill]					= possible;
	}
	
	if (newTechLevel > 4.75)
	{
		inventions[james_mill]					= active;
	}

	if (newTechLevel > 5)
	{
		inventions[silver_standard]			= possible;
		inventions[decimal_monetary_system]	= possible;
	}
	
	if (newTechLevel > 5 + (1/3))
	{
		inventions[silver_standard]			= active;
		inventions[decimal_monetary_system]	= possible;
	}
	
	if (newTechLevel > 5 + (2/3))
	{
		inventions[decimal_monetary_system]	= active;
	}

	if (newTechLevel > 6)
	{
		inventions[polypoly_structure]	= possible;
		inventions[oligopoly_structure]	= possible;
		inventions[monopoly_structure]	= possible;
	}
	
	if (newTechLevel > 6.25)
	{
		inventions[polypoly_structure]	= active;
		inventions[oligopoly_structure]	= possible;
		inventions[monopoly_structure]	= possible;
	}
	
	if (newTechLevel > 6.5)
	{
		inventions[oligopoly_structure]	= active;
		inventions[monopoly_structure]	= possible;
	}
	
	if (newTechLevel > 6.75)
	{
		inventions[monopoly_structure]	= active;
	}

	if (newTechLevel > 7)
	{
		inventions[work_classification]			= possible;
		inventions[work_specialization]			= possible;
		inventions[work_control_and_hierarchy]	= possible;
	}
	
	if (newTechLevel > 7.25)
	{
		inventions[work_classification]			= active;
		inventions[work_specialization]			= possible;
		inventions[work_control_and_hierarchy]	= possible;
	}
	
	if (newTechLevel > 7.5)
	{
		inventions[work_specialization]			= active;
		inventions[work_control_and_hierarchy]	= possible;
	}
	
	if (newTechLevel > 7.75)
	{
		inventions[work_control_and_hierarchy]	= active;
	}

	if (newTechLevel > 8)
	{
		inventions[john_elliot_cairnes]	= possible;
		inventions[robert_torrens]			= possible;
		inventions[john_stuart_mill]		= possible;
	}
	
	if (newTechLevel > 8.25)
	{
		inventions[john_elliot_cairnes]	= active;
		inventions[robert_torrens]			= possible;
		inventions[john_stuart_mill]		= possible;
	}
	
	if (newTechLevel > 8.5)
	{
		inventions[robert_torrens]			= active;
		inventions[john_stuart_mill]		= possible;
	}
	
	if (newTechLevel > 8.75)
	{
		inventions[john_stuart_mill]		= active;
	}

	if (newTechLevel > 9)
	{
		inventions[multitude_of_financial_instruments]		= possible;
		inventions[insurance_companies]							= possible;
		inventions[regulated_buying_and_selling_of_stocks]	= possible;
	}
	
	if (newTechLevel > 9.25)
	{
		inventions[multitude_of_financial_instruments]		= active;
		inventions[insurance_companies]							= possible;
		inventions[regulated_buying_and_selling_of_stocks]	= possible;
	}
	
	if (newTechLevel > 9.5)
	{
		inventions[insurance_companies]							= active;
		inventions[regulated_buying_and_selling_of_stocks]	= possible;
	}
	
	if (newTechLevel > 9.75)
	{
		inventions[regulated_buying_and_selling_of_stocks]	= active;
	}
}


void V2Country::setIndustryTech(double newTechLevel)
{
	if (newTechLevel > 0)
	{
		industryTech = (int)newTechLevel;
	}

	if (newTechLevel > 4)
	{
		inventions[ammunition_production]	= possible;
		inventions[small_arms_production]	= possible;
		inventions[explosives_production]	= possible;
		inventions[artillery_production]		= possible;
	}
	
	if (newTechLevel > 4.2)
	{
		inventions[ammunition_production]	= active;
		inventions[small_arms_production]	= possible;
		inventions[explosives_production]	= possible;
		inventions[artillery_production]		= possible;
	}
	
	if (newTechLevel > 4.4)
	{
		inventions[small_arms_production]	= active;
		inventions[explosives_production]	= possible;
		inventions[artillery_production]		= possible;
	}
	
	if (newTechLevel > 4.6)
	{
		inventions[explosives_production]	= active;
		inventions[artillery_production]		= possible;
	}
	
	if (newTechLevel > 4.8)
	{
		inventions[artillery_production]		= active;
	}

	if (newTechLevel > 5)
	{
		inventions[pit_coal]	= possible;
		inventions[coke]		= possible;
	}
	
	if (newTechLevel > 5 + (1/3))
	{
		inventions[pit_coal]	= active;
		inventions[coke]		= possible;
	}
	
	if (newTechLevel > 5 + (2/3))
	{
		inventions[coke]		= active;
	}

	if (newTechLevel > 6)
	{
		inventions[combat_medicine]											= possible;
		inventions[aerial_bacteria_and_antiseptic_principle]			= possible;
		inventions[vaccination]													= possible;
		inventions[chemotherapy]												= possible;
		inventions[genetics_heredity]											= possible;
		inventions[clinical_thermometers_and_binaural_stethoscopes]	= possible;
		inventions[prophylaxis_against_malaria]							= possible;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (1/9))
	{
		inventions[combat_medicine]											= active;
		inventions[aerial_bacteria_and_antiseptic_principle]			= possible;
		inventions[vaccination]													= possible;
		inventions[chemotherapy]												= possible;
		inventions[genetics_heredity]											= possible;
		inventions[clinical_thermometers_and_binaural_stethoscopes]	= possible;
		inventions[prophylaxis_against_malaria]							= possible;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (2/9))
	{
		inventions[aerial_bacteria_and_antiseptic_principle]			= active;
		inventions[vaccination]													= possible;
		inventions[chemotherapy]												= possible;
		inventions[genetics_heredity]											= possible;
		inventions[clinical_thermometers_and_binaural_stethoscopes]	= possible;
		inventions[prophylaxis_against_malaria]							= possible;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (3/9))
	{
		inventions[vaccination]													= active;
		inventions[chemotherapy]												= possible;
		inventions[genetics_heredity]											= possible;
		inventions[clinical_thermometers_and_binaural_stethoscopes]	= possible;
		inventions[prophylaxis_against_malaria]							= possible;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (4/9))
	{
		inventions[chemotherapy]												= active;
		inventions[genetics_heredity]											= possible;
		inventions[clinical_thermometers_and_binaural_stethoscopes]	= possible;
		inventions[prophylaxis_against_malaria]							= possible;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (5/9))
	{
		inventions[genetics_heredity]											= active;
		inventions[clinical_thermometers_and_binaural_stethoscopes]	= possible;
		inventions[prophylaxis_against_malaria]							= possible;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (6/9))
	{
		inventions[clinical_thermometers_and_binaural_stethoscopes]	= active;
		inventions[prophylaxis_against_malaria]							= possible;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (7/9))
	{
		inventions[prophylaxis_against_malaria]							= active;
		inventions[pressure_chambers_for_thorax_surgery]				= possible;
	}
	
	if (newTechLevel > 6 + (8/9))
	{
		inventions[pressure_chambers_for_thorax_surgery]				= active;
	}

	if (newTechLevel > 8)
	{
		inventions[sharp_n_roberts_power_loom]				= possible;
		inventions[jacquard_power_loom]						= possible;
		inventions[northrop_power_loom]						= possible;
		inventions[mechanical_saw]								= possible;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (1/9))
	{
		inventions[sharp_n_roberts_power_loom]				= active;
		inventions[jacquard_power_loom]						= possible;
		inventions[northrop_power_loom]						= possible;
		inventions[mechanical_saw]								= possible;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (2/9))
	{
		inventions[jacquard_power_loom]						= active;
		inventions[northrop_power_loom]						= possible;
		inventions[mechanical_saw]								= possible;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (3/9))
	{
		inventions[northrop_power_loom]						= active;
		inventions[mechanical_saw]								= possible;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (4/9))
	{
		inventions[mechanical_saw]								= active;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (5/9))
	{
		inventions[mechanical_precision_saw]				= active;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (6/9))
	{
		inventions[hussey_n_mccormicks_reaping_machine]	= active;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (7/9))
	{
		inventions[pitts_threshing_machine]					= active;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	
	if (newTechLevel > 8 + (8/9))
	{
		inventions[mechanized_slaughtering_block]			= active;
	}
}


void V2Country::setCultureTech(double newTechLevel)
{
	if (newTechLevel > 0)
	{
		cultureTech = (int)newTechLevel;
	}

	if (newTechLevel > 7)
	{
		inventions[romanticist_literature]	= possible;
		inventions[romanticist_art]			= possible;
		inventions[romanticist_music]			= possible;
	}
	
	if (newTechLevel > 7.25)
	{
		inventions[romanticist_literature]	= active;
		inventions[romanticist_art]			= possible;
		inventions[romanticist_music]			= possible;
	}
	
	if (newTechLevel > 7.5)
	{
		inventions[romanticist_art]			= active;
		inventions[romanticist_music]			= possible;
	}
	
	if (newTechLevel > 7.75)
	{
		inventions[romanticist_music]			= active;
	}

	if (newTechLevel > 8)
	{
		inventions[authoritarianism]		= possible;
		inventions[hierarchical_order]	= possible;
		inventions[traditionalism]			= possible;
		inventions[political_religion]	= possible;
		inventions[paternalism]				= possible;
		inventions[constitutionalism]		= possible;
		inventions[individualism]			= possible;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.0625)
	{
		inventions[authoritarianism]		= active;
		inventions[hierarchical_order]	= possible;
		inventions[traditionalism]			= possible;
		inventions[political_religion]	= possible;
		inventions[paternalism]				= possible;
		inventions[constitutionalism]		= possible;
		inventions[individualism]			= possible;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.125)
	{
		inventions[hierarchical_order]	= active;
		inventions[traditionalism]			= possible;
		inventions[political_religion]	= possible;
		inventions[paternalism]				= possible;
		inventions[constitutionalism]		= possible;
		inventions[individualism]			= possible;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.1875)
	{
		inventions[traditionalism]			= active;
		inventions[political_religion]	= possible;
		inventions[paternalism]				= possible;
		inventions[constitutionalism]		= possible;
		inventions[individualism]			= possible;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.25)
	{
		inventions[political_religion]	= active;
		inventions[paternalism]				= possible;
		inventions[constitutionalism]		= possible;
		inventions[individualism]			= possible;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.3125)
	{
		inventions[paternalism]				= active;
		inventions[constitutionalism]		= possible;
		inventions[individualism]			= possible;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.375)
	{
		inventions[constitutionalism]		= active;
		inventions[individualism]			= possible;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.4375)
	{
		inventions[individualism]			= active;
		inventions[rationalism]				= possible;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.5)
	{
		inventions[rationalism]				= active;
		inventions[secularization]			= possible;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.5625)
	{
		inventions[secularization]			= active;
		inventions[social_conscience]		= possible;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.625)
	{
		inventions[social_conscience]		= active;
		inventions[egalitarianism]			= possible;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.6875)
	{
		inventions[egalitarianism]			= active;
		inventions[collectivism]			= possible;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.75)
	{
		inventions[collectivism]			= active;
		inventions[determinism]				= possible;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.8125)
	{
		inventions[determinism]				= active;
		inventions[atheism]					= possible;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.875)
	{
		inventions[atheism]					= active;
		inventions[social_justice]			= possible;
	}
	
	if (newTechLevel > 8.93875)
	{
		inventions[social_justice]			= active;
	}
}


void V2Country::outputTech(FILE* output)
{
	fprintf(output, "	technology=\n");
	fprintf(output, " {\n");
	if (armyTech >= 0)
	{
		fprintf(output, "		flintlock_rifles={1 0.000}\n");
	}
	if (armyTech >= 1)
	{
		fprintf(output, "		military_staff_system={1 0.000}\n");
	}
	if (armyTech >= 2)
	{
		fprintf(output, "		post_napoleonic_thought={1 0.000}\n");
	}
	if (armyTech >= 3)
	{
		fprintf(output, "		bronze_muzzle_loaded_artillery={1 0.000}\n");
	}
	if (armyTech >= 4)
	{
		fprintf(output, "		army_command_principle={1 0.000}\n");
	}
	if (armyTech >= 5)
	{
		fprintf(output, "		muzzle_loaded_rifles={1 0.000}\n");
	}
	if (armyTech >= 6)
	{
		fprintf(output, "		iron_muzzle_loaded_artillery={1 0.000}\n");
	}
	if (armyTech >= 7)
	{
		fprintf(output, "		military_plans={1 0.000}\n");
	}
	if (armyTech >= 8)
	{
		fprintf(output, "		strategic_mobility={1 0.000}\n");
	}
	if (armyTech >= 9)
	{
		fprintf(output, "		army_professionalism={1 0.000}\n");
	}

	if (navyTech >= 0)
	{
		fprintf(output, "		clipper_design={1 0.000}\n");
	}
	if (navyTech >= 1)
	{
		fprintf(output, "		naval_design_bureaus={1 0.000}\n");
	}
	if (navyTech >= 2)
	{
		fprintf(output, "		post_nelsonian_thought={1 0.000}\n");
	}
	if (navyTech >= 3)
	{
		fprintf(output, "		alphabetic_flag_signaling={1 0.000}\n");
	}
	if (navyTech >= 4)
	{
		fprintf(output, "		the_command_principle={1 0.000}\n");
	}
	if (navyTech >= 5)
	{
		fprintf(output, "		steamers={1 0.000}\n");
	}
	if (navyTech >= 6)
	{
		fprintf(output, "		fire_control_systems={1 0.000}\n");
	}
	if (navyTech >= 7)
	{
		fprintf(output, "		naval_plans={1 0.000}\n");
	}
	if (navyTech >= 8)
	{
		fprintf(output, "		naval_professionalism={1 0.000}\n");
	}
	if (navyTech >= 9)
	{
		fprintf(output, "		battleship_column_doctrine={1 0.000}\n");
	}

	if (commerceTech >= 0)
	{
		fprintf(output, "		no_standard={1 0.000}\n");
	}
	if (commerceTech >= 1)
	{
		fprintf(output, "		guild_based_production={1 0.000}\n");
	}
	if (commerceTech >= 2)
	{
		fprintf(output, "		freedom_of_trade={1 0.000}\n");
	}
	if (commerceTech >= 3)
	{
		fprintf(output, "		private_banks={1 0.000}\n");
	}
	if (commerceTech >= 4)
	{
		fprintf(output, "		early_classical_theory_and_critique={1 0.000}\n");
	}
	if (commerceTech >= 5)
	{
		fprintf(output, "		ad_hoc_money_bill_printing={1 0.000}\n");
	}
	if (commerceTech >= 6)
	{
		fprintf(output, "		market_structure={1 0.000}\n");
	}
	if (commerceTech >= 7)
	{
		fprintf(output, "		organized_factories={1 0.000}\n");
	}
	if (commerceTech >= 8)
	{
		fprintf(output, "		late_classical_theory={1 0.000}\n");
	}
	if (commerceTech >= 9)
	{
		fprintf(output, "		stock_exchange={1 0.000}\n");
	}

	if (industryTech >= 0)
	{
		fprintf(output, "		water_wheel_power={1 0.000}\n");
	}
	if (industryTech >= 1)
	{
		fprintf(output, "		basic_chemistry={1 0.000}\n");
	}
	if (industryTech >= 2)
	{
		fprintf(output, "		publishing_industry={1 0.000}\n");
	}
	if (industryTech >= 3)
	{
		fprintf(output, "		experimental_railroad={1 0.000}\n");
	}
	if (industryTech >= 4)
	{
		fprintf(output, "		mechanized_mining={1 0.000}\n");
	}
	if (industryTech >= 5)
	{
		fprintf(output, "		clean_coal={1 0.000}\n");
	}
	if (industryTech >= 6)
	{
		fprintf(output, "		medicine={1 0.000}\n");
	}
	if (industryTech >= 7)
	{
		fprintf(output, "		practical_steam_engine={1 0.000}\n");
	}
	if (industryTech >= 8)
	{
		fprintf(output, "		mechanical_production={1 0.000}\n");
	}

	if (cultureTech >= 0)
	{
		fprintf(output, "		malthusian_thought={1 0.000}\n");
	}
	if (cultureTech >= 1)
	{
		fprintf(output, "		late_enlightenment_philosophy={1 0.000}\n");
	}
	if (cultureTech >= 2)
	{
		fprintf(output, "		enlightenment_thought={1 0.000}\n");
	}
	if (cultureTech >= 3)
	{
		fprintf(output, "		classicism_n_early_romanticism={1 0.000}\n");
	}
	if (cultureTech >= 4)
	{
		fprintf(output, "		introspectionism={1 0.000}\n");
	}
	if (cultureTech >= 5)
	{
		fprintf(output, "		positivism={1 0.000}\n");
	}
	if (cultureTech >= 6)
	{
		fprintf(output, "		associationism={1 0.000}\n");
	}
	if (cultureTech >= 7)
	{
		fprintf(output, "		romanticism={1 0.000}\n");
	}
	if (cultureTech >= 8)
	{
		fprintf(output, "		ideological_thought={1 0.000}\n");
	}

	fprintf(output, "	}\n");
}


void V2Country::outputInventions(FILE* output)
{
	fprintf(output, "	possible_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		\n");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == possible)
		{
			fprintf(output, "%d ", i + 1);
		}
	}

	fprintf(output, "	}\n");
	fprintf(output, "	illegal_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		\n");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == illegal)
		{
			fprintf(output, "%d ", i + 1);
		}
	}

	fprintf(output, "	}\n");
	fprintf(output, "	active_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		\n");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == active)
		{
			fprintf(output, "%d ", i + 1);
		}
	}
	fprintf(output, "	}\n");
}


inventionStatus V2Country::getInventionState(inventionTypes invention)
{
	return inventions[invention];
}


void V2Country::addPrestige(double additionalPrestige)
{
	prestige += additionalPrestige;
}