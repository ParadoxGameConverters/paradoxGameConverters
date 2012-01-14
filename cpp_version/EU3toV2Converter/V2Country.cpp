#include "V2Country.h"
#include "tempFuncs.h"
#include "Log.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include <io.h>
#include "Configuration.h"
#include "Parsers/Parser.h"


void V2Country::init(string newTag, string newCountryFile, vector<int> newParties)
{
	tag			= newTag;
	countryFile	= newCountryFile;
	parties		= newParties;

	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		inventions[i] = illegal;
	}

	plurality = 0.0;
	capital = 0;
	diploPoints = 0.0;
	badboy = 0.0;
	prestige = 0.0;
	money = 0.0;

	sourceCountryIndex = -1;
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void V2Country::initFromHistory()
{
	string V2Loc = Configuration::getV2Path();
	string filename = V2Loc + "\\history\\countries\\" + tag + "*.txt";
	struct _finddata_t	fileData;
	intptr_t			fileListing;
	if ( (fileListing = _findfirst(filename.c_str(), &fileData)) != -1L)
	{
		Object* obj = doParseFile( (V2Loc + "\\history\\countries\\" + fileData.name).c_str() );

		vector<Object*> results = obj->getValue("primary_culture");
		if (results.size() > 0)
		{
			setPrimaryCulture(results[0]->getLeaf());
		}

		results = obj->getValue("culture");
		for (vector<Object*>::iterator itr = results.begin(); itr != results.end(); ++itr)
		{
			addAcceptedCulture((*itr)->getLeaf());
		}

		results = obj->getValue("religion");
		if (results.size() > 0)
		{
			setReligion(results[0]->getLeaf());
		}

		results = obj->getValue("government");
		if (results.size() > 0)
		{
			setGovernment(results[0]->getLeaf());
		}

		results = obj->getValue("civilized");
		if (results.size() > 0)
		{
			setCivilized(results[0]->getLeaf() == "yes");
		}

		results = obj->getValue("nationalvalue");
		if (results.size() > 0)
			nationalValue = results[0]->getLeaf();
		else
			nationalValue = "nv_order";

		results = obj->getValue("capital");
		if (results.size() > 0)
		{
			capital = atoi(results[0]->getLeaf().c_str());
		}
	}
	_findclose(fileListing);
}


void V2Country::setSourceCountryIndex(int oldCountryIndex)
{
	sourceCountryIndex	= oldCountryIndex;
}


string V2Country::getTag() const
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


void V2Country::setCivilized(bool isIt)
{
	civilized = isIt;
}


bool V2Country::isCivilized()
{
	return civilized;
}


void V2Country::setPrimaryCulture(string culture)
{
	primaryCulture = culture;
}


string V2Country::getPrimaryCulture() const
{
	return primaryCulture;
}


void V2Country::addAcceptedCulture(string culture)
{
	acceptedCultures.push_back(culture);
}


void V2Country::setReligion(string newReligion)
{
	religion = newReligion;
}


string V2Country::getReligion()
{
	return religion;
}


void V2Country::sortRelations(const vector<string>& order)
{
	vector<V2Relations> sortedRelations;
	for (vector<string>::const_iterator oitr = order.begin(); oitr != order.end(); ++oitr)
	{
		for (vector<V2Relations>::iterator itr = relations.begin(); itr != relations.end(); ++itr)
		{
			if (itr->getTag() == (*oitr))
			{
				sortedRelations.push_back(*itr);
				break;
			}
		}
	}
	relations.swap(sortedRelations);
}


void V2Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	if (capital > 0)
	{
		fprintf(output, "	capital=%d\n", capital);
	}
	outputTech(output);
	outputElection(output);
	reforms.output(output);
	outputCountryHeader(output);
	fprintf(output, "	ruling_party=%d\n", parties[0]);
	for (unsigned int i = 0; i < parties.size(); i++)
	{
		fprintf(output, "	active_party=%d\n", parties[i]);
	}
	fprintf(output, "	diplomatic_points=%f\n", diploPoints);
	fprintf(output, "	religion=\"%s\"\n", religion.c_str());
	fprintf(output, "	government=%s\n", government.c_str());
	fprintf(output, "	plurality=%f\n", plurality);
	outputCountryHeader2(output);
	fprintf(output, "	leadership=%f\n", leadership);
	for (vector<V2Leader>::iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
	{
		itr->output(output);
	}
	for (vector<V2Army>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		itr->output(output);
	}
	for (vector<V2Relations>::iterator itr = relations.begin(); itr != relations.end(); ++itr)
	{
		itr->output(output);
	}
	outputInventions(output);
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
	fprintf(output, "	prestige=%f\n", prestige);
	fprintf(output, "	bank=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		money=%f\n", bankReserves);
	fprintf(output, "		money_lent=0.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	money=%f\n", money);
	fprintf(output, "	last_bankrupt=\"%s\"\n", lastBankrupt.toString().c_str());
	for (vector<V2Creditor>::iterator itr = creditors.begin(); itr != creditors.end(); ++itr)
	{
		itr->output(output);
	}
	fprintf(output, "	nationalvalue=\"%s\"\n", nationalValue.c_str());
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
	fprintf(output, "	badboy=%f\n", badboy);
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


void V2Country::setLeadership(double newLeadership)
{
	leadership = newLeadership;
}


void V2Country::setArmyTech(double newTechLevel)
{
	if (newTechLevel >= 0)
	{
		techs.push_back("flintlock_rifles");
	}
	if (newTechLevel >= 0.25)
	{
		techs.push_back("bronze_muzzle_loaded_artillery");
	}
	if (newTechLevel >= 2)
	{
		techs.push_back("post_napoleonic_thought");
	}
	if (newTechLevel >= 3)
	{
		techs.push_back("army_command_principle");
	}
	if (newTechLevel >= 4)
	{
		techs.push_back("military_staff_system");
	}
	if (newTechLevel >= 5)
	{
		techs.push_back("army_professionalism");
	}

	if (newTechLevel >= 5)
	{
		inventions[army_academic_training]	= active;
		inventions[field_training]				= active;
		inventions[army_societal_status]		= active;
	}
}


void V2Country::setNavyTech(double newTechLevel)
{
	if (newTechLevel >= 0)
	{
		techs.push_back("post_nelsonian_thought");
	}
	if (newTechLevel >= 0.25)
	{
		techs.push_back("the_command_principle");
	}
	if (newTechLevel >= 4)
	{
		techs.push_back("clipper_design");
	}
	if (newTechLevel >= 4)
	{
		techs.push_back("naval_design_bureaus");
	}
	if (newTechLevel >= 4)
	{
		techs.push_back("alphabetic_flag_signaling");
	}
	if (newTechLevel >= 6)
	{
		techs.push_back("battleship_column_doctrine");
	}
	if (newTechLevel >= 6)
	{
		techs.push_back("steamers");
	}
	if (newTechLevel >= 7)
	{
		techs.push_back("naval_professionalism");
	}

	if (newTechLevel >= 4)
	{
		inventions[building_station_shipyards]	= possible;
	}
	if (newTechLevel >= 4.5)
	{
		inventions[building_station_shipyards]	= active;
	}

	if (newTechLevel >= 6)
	{
		inventions[long_range_fire_tactic]						= possible;
		inventions[speedy_maneuvering_tactic]					= possible;
		inventions[mechanized_fishing_vessels]					= possible;
		inventions[steamer_automatic_construction_plants]	= possible;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}
	if (newTechLevel >= 6 + (1/7))
	{
		inventions[long_range_fire_tactic]						= active;
		inventions[speedy_maneuvering_tactic]					= possible;
		inventions[mechanized_fishing_vessels]					= possible;
		inventions[steamer_automatic_construction_plants]	= possible;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}
	if (newTechLevel >= 6 + (2/7))
	{
		inventions[speedy_maneuvering_tactic]					= active;
		inventions[mechanized_fishing_vessels]					= possible;
		inventions[steamer_automatic_construction_plants]	= possible;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}
	if (newTechLevel >= 6 + (3/7))
	{
		inventions[mechanized_fishing_vessels]					= active;
		inventions[steamer_automatic_construction_plants]	= possible;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}
	if (newTechLevel >= 6 + (4/7))
	{
		inventions[steamer_automatic_construction_plants]	= active;
		inventions[steamer_transports]							= possible;
		inventions[commerce_raiders]								= possible;
	}
	if (newTechLevel >= 6 + (5/7))
	{
		inventions[steamer_transports]							= active;
		inventions[commerce_raiders]								= possible;
	}
	if (newTechLevel >= 6 + (6/7))
	{
		inventions[commerce_raiders]								= active;
	}
	
	if (newTechLevel >= 7)
	{
		inventions[academic_training]			= active;
		inventions[combat_station_training]	= active;
		inventions[societal_status]			= active;
	}
}


void V2Country::setCommerceTech(double newTechLevel)
{
	techs.push_back("no_standard");
	if (newTechLevel >= 1)
	{
		techs.push_back("guild_based_production");
	}
	if (newTechLevel >= 2)
	{
		techs.push_back("private_banks");
	}
	if (newTechLevel >= 3)
	{
		techs.push_back("early_classical_theory_and_critique");
	}
	if (newTechLevel >= 3.25)
	{
		techs.push_back("freedom_of_trade");
	}
	if (newTechLevel >= 6)
	{
		techs.push_back("stock_exchange");
	}
	if (newTechLevel >= 8)
	{
		techs.push_back("ad_hoc_money_bill_printing");
	}
	if (newTechLevel >= 8)
	{
		techs.push_back("market_structure");
	}
	if (newTechLevel >= 9)
	{
		techs.push_back("late_classical_theory");
	}

	if (newTechLevel >= 3.25)
	{
		inventions[john_ramsay_mcculloch]	= possible;
		inventions[nassau_william_sr]			= possible;
		inventions[james_mill]					= possible;
	}
	if (newTechLevel >= 3.9375)
	{
		inventions[john_ramsay_mcculloch]	= active;
		inventions[nassau_william_sr]			= possible;
		inventions[james_mill]					= possible;
	}
	if (newTechLevel >= 4.625)
	{
		inventions[nassau_william_sr]			= active;
		inventions[james_mill]					= possible;
	}
	if (newTechLevel >= 5.3125)
	{
		inventions[james_mill]					= active;
	}

	if (newTechLevel >= 6)
	{
		inventions[multitude_of_financial_instruments]		= possible;
		inventions[insurance_companies]							= possible;
		inventions[regulated_buying_and_selling_of_stocks]	= possible;
	}
	if (newTechLevel >= 6.25)
	{
		inventions[multitude_of_financial_instruments]		= active;
		inventions[insurance_companies]							= possible;
		inventions[regulated_buying_and_selling_of_stocks]	= possible;
	}
	if (newTechLevel >= 6.5)
	{
		inventions[insurance_companies]							= active;
		inventions[regulated_buying_and_selling_of_stocks]	= possible;
	}
	if (newTechLevel >= 6.75)
	{
		inventions[regulated_buying_and_selling_of_stocks]	= active;
	}

	if (newTechLevel >= 8)
	{
		inventions[silver_standard]			= possible;
		inventions[decimal_monetary_system]	= possible;
		inventions[polypoly_structure]		= possible;
		inventions[oligopoly_structure]		= possible;
		inventions[monopoly_structure]		= possible;
	}
	if (newTechLevel >= 8 + (1/6))
	{
		inventions[silver_standard]			= active;
		inventions[decimal_monetary_system]	= possible;
		inventions[polypoly_structure]		= possible;
		inventions[oligopoly_structure]		= possible;
		inventions[monopoly_structure]		= possible;
	}
	if (newTechLevel >= 8 + (2/6))
	{
		inventions[decimal_monetary_system]	= active;
		inventions[polypoly_structure]		= possible;
		inventions[oligopoly_structure]		= possible;
		inventions[monopoly_structure]		= possible;
	}
	if (newTechLevel >= 8 + (3/6))
	{
		inventions[polypoly_structure]		= active;
		inventions[oligopoly_structure]		= possible;
		inventions[monopoly_structure]		= possible;
	}
	if (newTechLevel >= 8 + (4/6))
	{
		inventions[oligopoly_structure]		= active;
		inventions[monopoly_structure]		= possible;
	}
	if (newTechLevel >= 8 + (5/6))
	{
		inventions[monopoly_structure]		= active;
	}

	if (newTechLevel >= 9)
	{
		inventions[john_elliot_cairnes]	= active;
		inventions[robert_torrens]			= active;
		inventions[john_stuart_mill]		= active;
	}
}


void V2Country::setIndustryTech(double newTechLevel)
{
	if (newTechLevel >= 0)
	{
		techs.push_back("water_wheel_power");
	}
	if (newTechLevel >= 1)
	{
		techs.push_back("publishing_industry");
	}
	if (newTechLevel >= 3)
	{
		techs.push_back("mechanized_mining");
	}
	if (newTechLevel >= 3)
	{
		techs.push_back("basic_chemistry");
	}
	if (newTechLevel >= 4)
	{
		techs.push_back("practical_steam_engine");
	}
	if (newTechLevel >= 5)
	{
		techs.push_back("experimental_railroad");
	}
	if (newTechLevel >= 6)
	{
		techs.push_back("mechanical_production");
	}
	if (newTechLevel >= 7)
	{
		techs.push_back("clean_coal");
	}

	if (newTechLevel >= 3)
	{
		inventions[ammunition_production]	= possible;
		inventions[small_arms_production]	= possible;
		inventions[explosives_production]	= possible;
		inventions[artillery_production]		= possible;
	}
	if (newTechLevel >= 3.2)
	{
		inventions[ammunition_production]	= active;
		inventions[small_arms_production]	= possible;
		inventions[explosives_production]	= possible;
		inventions[artillery_production]		= possible;
	}
	if (newTechLevel >= 3.4)
	{
		inventions[small_arms_production]	= active;
		inventions[explosives_production]	= possible;
		inventions[artillery_production]		= possible;
	}
	if (newTechLevel >= 3.6)
	{
		inventions[explosives_production]	= active;
		inventions[artillery_production]		= possible;
	}
	if (newTechLevel >= 3.8)
	{
		inventions[artillery_production]		= active;
	}

	if (newTechLevel >= 6)
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
	if (newTechLevel >= 6 + (1/9))
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
	if (newTechLevel >= 6 + (2/9))
	{
		inventions[jacquard_power_loom]						= active;
		inventions[northrop_power_loom]						= possible;
		inventions[mechanical_saw]								= possible;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	if (newTechLevel >= 6 + (3/9))
	{
		inventions[northrop_power_loom]						= active;
		inventions[mechanical_saw]								= possible;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	if (newTechLevel >= 6 + (4/9))
	{
		inventions[mechanical_saw]								= active;
		inventions[mechanical_precision_saw]				= possible;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	if (newTechLevel >= 6 + (5/9))
	{
		inventions[mechanical_precision_saw]				= active;
		inventions[hussey_n_mccormicks_reaping_machine]	= possible;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	if (newTechLevel >= 6 + (6/9))
	{
		inventions[hussey_n_mccormicks_reaping_machine]	= active;
		inventions[pitts_threshing_machine]					= possible;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	if (newTechLevel >= 6 + (7/9))
	{
		inventions[pitts_threshing_machine]					= active;
		inventions[mechanized_slaughtering_block]			= possible;
	}
	if (newTechLevel >= 6 + (8/9))
	{
		inventions[mechanized_slaughtering_block]			= active;
	}

	if (newTechLevel >= 7)
	{
		inventions[pit_coal]	= active;
		inventions[coke]		= active;
	}
}


void V2Country::setCultureTech(double newTechLevel)
{
	techs.push_back("classicism_n_early_romanticism");
	techs.push_back("late_enlightenment_philosophy");
	if (newTechLevel >= 2)
	{
		techs.push_back("enlightenment_thought");
	}
	if (newTechLevel >= 4)
	{
		techs.push_back("malthusian_thought");
	}
	if (newTechLevel >= 4)
	{
		techs.push_back("introspectionism");
	}
	if (newTechLevel >= 5)
	{
		techs.push_back("romanticism");
	}


	if (newTechLevel >= 5)
	{
		inventions[romanticist_literature]	= possible;
		inventions[romanticist_art]			= possible;
		inventions[romanticist_music]			= possible;
	}
	if (newTechLevel >= 5 + (1/3))
	{
		inventions[romanticist_literature]	= active;
		inventions[romanticist_art]			= possible;
		inventions[romanticist_music]			= possible;
	}
	if (newTechLevel >= 5 + (2/3))
	{
		inventions[romanticist_art]			= active;
		inventions[romanticist_music]			= possible;
	}
	if (newTechLevel >= 6)
	{
		inventions[romanticist_music]			= active;
	}
}


void V2Country::outputTech(FILE* output)
{
	fprintf(output, "	technology=\n");
	fprintf(output, "	{\n");

	for (vector<string>::iterator itr = techs.begin(); itr != techs.end(); ++itr)
	{
		fprintf(output, "\t\t"); fprintf(output, itr->c_str()); fprintf(output, "={1 0.000}\n");
	}

	fprintf(output, "	}\n");
}


void V2Country::outputInventions(FILE* output)
{
	fprintf(output, "	active_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == active)
		{
			fprintf(output, "%d ", i + 1);
		}
	}
	fprintf(output, "\n	}\n");

	fprintf(output, "	possible_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == possible)
		{
			fprintf(output, "%d ", i + 1);
		}
	}
	fprintf(output, "\n	}\n");

	fprintf(output, "	illegal_inventions=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		");
	for (unsigned int i = 0; i < naval_exercises; i++)
	{
		if (inventions[i] == illegal)
		{
			fprintf(output, "%d ", i + 1);
		}
	}
	fprintf(output, "\n	}\n");
}


void V2Country::outputElection(FILE* output)
{
	date electionDate = Configuration::getStartDate();

	if (electionDate.month == 12)
	{
		electionDate.month = 1;
		electionDate.year++;
	}
	else
	{
		electionDate.month++;
	}
	electionDate.year -= 4;
	fprintf(output, "last_election=%s\n", electionDate.toString().c_str());
}


inventionStatus V2Country::getInventionState(inventionType invention)
{
	return inventions[invention];
}


void V2Country::addPrestige(double additionalPrestige)
{
	prestige += additionalPrestige;
}


void V2Country::setGovernment(string newGovernment)
{
	government = newGovernment;
}


void V2Country::addRelations(V2Relations _rel)
{
	relations.push_back(_rel);
}


V2Relations* V2Country::getRelations(string withWhom)
{
	for (size_t i = 0; i < relations.size(); ++i)
	{
		if (relations[i].getTag() == withWhom)
			return &relations[i];
	}
	return NULL;
}


void V2Country::addArmy(V2Army _army)
{
	armies.push_back(_army);
}


// find the army most in need of a regiment of this category
V2Army*	V2Country::getArmyForRemainder(RegimentCategory rc)
{
	V2Army* retval = NULL;
	double retvalRemainder = -1000.0;
	for (vector<V2Army>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		// only add units to armies that originally had units of the same category
		if (itr->getSourceArmy()->getTotalTypeStrength(rc) > 0)
		{
			if (itr->getArmyRemainder(rc) > retvalRemainder)
			{
				retvalRemainder = itr->getArmyRemainder(rc);
				retval = &(*itr);
			}
		}
	}
	return retval;
}


void V2Country::setReforms(EU3Country* srcCountry)
{
	reforms.init(srcCountry);
}


static bool FactoryCandidateSortPredicate(const pair<int, V2State*>& lhs, const pair<int, V2State*>& rhs)
{
	if (lhs.first != rhs.first)
		return lhs.first > rhs.first;
	return lhs.second->getID() < rhs.second->getID();
}


bool V2Country::addFactory(V2Factory factory)
{
	// check factory techs
	string requiredTech = factory.getRequiredTech();
	if (requiredTech != "")
	{
		vector<string>::iterator itr = find(techs.begin(), techs.end(), requiredTech);
		if (itr == techs.end())
		{
			log("%s rejected %s (missing reqd tech: %s)\n", tag.c_str(), factory.getTypeName().c_str(), requiredTech.c_str());
			return false;
		}
	}
	
	// check factory inventions
	inventionType requiredInvention = factory.getRequiredInvention();
	if (requiredInvention >= 0 && inventions[requiredInvention] != active)
	{
		log("%s rejected %s (missing reqd invention: %s)\n", tag.c_str(), factory.getTypeName().c_str(), inventionNames[requiredInvention]);
		return false;
	}

	// find a state to add the factory to, which meets the factory's requirements
	vector<pair<int, V2State*>> candidates;
	for (vector<V2State>::iterator itr = states.begin(); itr != states.end(); ++itr)
	{
		if (itr->isColonial())
			continue;

		if (itr->getFactoryCount() >= 8)
			continue;

		if (factory.requiresCoastal())
		{
			if (!itr->isCoastal())
				continue;
		}

		vector<string> requiredProducts = factory.getRequiredRGO();
		if (requiredProducts.size() > 0)
		{
			bool hasInput = false;
			for (vector<string>::iterator prod = requiredProducts.begin(); prod != requiredProducts.end(); ++prod)
			{
				if (itr->hasLocalSupply(*prod))
				{
					hasInput = true;
					break;
				}
			}
			if (!hasInput)
				continue;
		}

		candidates.push_back(pair<int, V2State*>(itr->getCraftsmenPerFactory(), &(*itr)));
	}

	sort(candidates.begin(), candidates.end(), FactoryCandidateSortPredicate);

	if (candidates.size() == 0)
	{
		log("%s rejected %s (no candidate states)\n", tag.c_str(), factory.getTypeName().c_str());
		return false;
	}

	V2State* target = candidates[0].second;
	target->addFactory(factory);
	log("%s accepted %s (%d candidate states)\n", tag.c_str(), factory.getTypeName().c_str(), candidates.size());
	return true;
}


void V2Country::setNationalIdea(EU3Country* srcCountry, int& libertyLeft, int& equalityLeft)
{
	int orderScore = 0;
	orderScore += srcCountry->getOffensiveDefensive();
	orderScore += srcCountry->getInnovativeNarrowminded();
	orderScore += srcCountry->getQualityQuantity();
	vector<string> nationalIdeas = srcCountry->getNationalIdeas();
	for (unsigned int i = 0; i < nationalIdeas.size(); i++)
	{
		if (nationalIdeas[i] == "deus_vult")
		{
			orderScore += 2;
		}
		if (nationalIdeas[i] == "church_attendance_duty")
		{
			orderScore += 2;
		}
		if (nationalIdeas[i] == "divine_supremacy")
		{
			orderScore += 2;
		}
		if (nationalIdeas[i] == "national_conscripts")
		{
			orderScore += 2;
		}
		if (nationalIdeas[i] == "press_gangs")
		{
			orderScore += 1;
		}
		if (nationalIdeas[i] == "military_drill")
		{
			orderScore += 1;
		}
		if (nationalIdeas[i] == "bureaucracy")
		{
			orderScore += 1;
		}
	}

	int libertyScore = 0;
	libertyScore += srcCountry->getCentralizationDecentralization();
	libertyScore += srcCountry->getSerfdomFreesubjects();
	libertyScore += srcCountry->getMercantilismFreetrade();
	for (unsigned int i = 0; i < nationalIdeas.size(); i++)
	{
		if (nationalIdeas[i] == "liberty_egalite_fraternity")
		{
			libertyScore += 4;
		}
		if (nationalIdeas[i] == "smithian_economics")
		{
			libertyScore += 2;
		}
		if (nationalIdeas[i] == "bill_of_rights")
		{
			libertyScore += 2;
		}
		if (nationalIdeas[i] == "scientific_revolution")
		{
			libertyScore += 1;
		}
		if (nationalIdeas[i] == "ecumenism")
		{
			libertyScore += 1;
		}
	}

	int equalityScore = 0;
	equalityScore += srcCountry->getAristocracyPlutocracy();
	equalityScore += srcCountry->getSerfdomFreesubjects();
	for (unsigned int i = 0; i < nationalIdeas.size(); i++)
	{
		if (nationalIdeas[i] == "liberty_egalite_fraternity")
		{
			equalityScore += 4;
		}
		if (nationalIdeas[i] == "humanist_tolerance")
		{
			equalityScore += 2;
		}
		if (nationalIdeas[i] == "bill_of_rights")
		{
			equalityScore += 2;
		}
		if (nationalIdeas[i] == "ecumenism")
		{
			equalityScore += 1;
		}
	}

	if ( (equalityScore > orderScore) && (equalityScore >= libertyScore) && (equalityLeft > 0) )
	{
		nationalValue = "nv_equality";
		equalityLeft--;
	}
	else if ( (libertyScore > orderScore) && (libertyLeft > 0) )
	{
		nationalValue = "nv_liberty";
		libertyLeft--;
	}
	else
	{
		nationalValue = "nv_order";
	}
}


void V2Country::setMoney(double _cash)
{
	money = _cash;
}


void V2Country::setLastBankrupt(date _lastBankrupt)
{
	lastBankrupt = _lastBankrupt;
}


void V2Country::addLoan(string creditor, double size, double interest)
{
	bool found = false;
	for (vector<V2Creditor>::iterator itr = creditors.begin(); itr != creditors.end(); ++itr)
	{
		if (creditor == itr->getCountry())
		{
			itr->addLoan(size, interest);
			found = true;
			break;
		}
	}
	if (!found)
	{
		V2Creditor cred;
		cred.setCountry(creditor);
		cred.addLoan(size, interest);
		creditors.push_back(cred);
	}
}


void V2Country::setBankReserves(double money)
{
	bankReserves = money;
}


void V2Country::setDiploPoints(double points)
{
	diploPoints = points;
}


void V2Country::setBadboy(double _badboy)
{
	badboy = _badboy;
}


void V2Country::addLeader(V2Leader leader)
{
	leaders.push_back(leader);
}


void V2Country::setupPops(EU3World& sourceWorld)
{
	if (states.size() < 1) // skip entirely for empty nations
		return;

	EU3Country oldCountry = sourceWorld.getCountries()[sourceCountryIndex];

	// calculate NATIONAL con and mil modifiers (province modifiers are done in V2Province::setPopConMil)
	double con = 0.0;
	double mil = 0.0;
	// 0 to 3 points of mil from stability, based on a similar discontinuous function to that used by EU3 to calc revolt risk
	double stability = oldCountry.getStability();
	if (stability > -FLT_EPSILON) // positive stability: 3 stab -> 0 mil ==> 0 stab -> 1 mil
		mil += fabs(stability - 3.0) / 3.0;
	else // negative stability: 0 stab -> 1 mil ==> -3 stab -> 3 mil
		mil += fabs(stability) * 2.0 / 3.0 + 1.0;
	// 0 to 2 points of con from serfdom<->free subjects
	double serf_fs = oldCountry.getSerfdomFreesubjects();
	con += (serf_fs * 2.0 / 5.0) + 1.0;
	// TODO: national decisions, national events, war exhaustion

	// create the pops
	for (vector<V2State>::iterator itr = states.begin(); itr != states.end(); ++itr)
	{
		itr->setupPops(sourceWorld, primaryCulture, acceptedCultures, religion, con, mil);
	}
}


void V2Country::setLiteracy(double newLiteracy)
{
	literacy = newLiteracy;
}


double V2Country::getLiteracy()
{
	return literacy;
}
