#include "V2Reforms.h"
#include "Log.h"
#include "Configuration.h"
#include "EU3Country.h"
#include "V2Country.h"
#include "V2State.h"
#include "V2Province.h"



V2Reforms::V2Reforms(const EU3Country* srcCountry)
{
	slavery							= 0;
	vote_franchise					= 0;
	upper_house_composition		= 0;
	voting_system					= 0;
	public_meetings				= 0;
	press_rights					= 0;
	trade_unions					= 0;
	political_parties				= 0;
	wage_reforms					= 0;
	work_hours						= 0;
	safety_regulations			= 0;
	unemployment_subsidies		= 0;
	pensions							= 0;
	health_care						= 0;
	school_reforms					= 0;
	governmentEffects(srcCountry);
	sliderEffects(srcCountry);
	flagEffects(srcCountry);
	modifierEffects(srcCountry);
}


void V2Reforms::output(FILE* output) const
{
	fprintf(output, "	wage_reform=no_minimum_wage\n");
	fprintf(output, "	work_hours=no_work_hour_limit\n");
	fprintf(output, "	safety_regulations=no_safety\n");
	fprintf(output, "	unemployment_subsidies=no_subsidies\n");
	fprintf(output, "	pensions=no_pensions\n");
	fprintf(output, "	health_care=no_health_care\n");

	if (Configuration::getV2Gametype() == "AHD")
	{
		if (school_reforms >= 10)
		{
			fprintf(output, "	school_reforms=low_schools\n");
		}
		else
		{
			fprintf(output, "	school_reforms=no_schools\n");
		}
	}

	if (slavery >= 1)
	{
		fprintf(output, "	slavery=no_slavery\n");
	}
	else
	{
		fprintf(output, "	slavery=yes_slavery\n");
	}

	if (vote_franchise >= 20)
	{
		fprintf(output, "	vote_franschise=universal_voting\n");
	}
	else if (vote_franchise >= 15)
	{
		fprintf(output, "	vote_franschise=universal_weighted_voting\n");
	}
	else if (vote_franchise >= 10)
	{
		fprintf(output, "	vote_franschise=wealth_voting\n");
	}
	else if (vote_franchise >= 5)
	{
		fprintf(output, "	vote_franschise=wealth_weighted_voting\n");
	}
	else if (vote_franchise >= 0)
	{
		fprintf(output, "	vote_franschise=landed_voting\n");
	}
	else
	{
		fprintf(output, "	vote_franschise=none_voting\n");
	}

	if (upper_house_composition >= 10)
	{
		fprintf(output, "	upper_house_composition=population_equal_weight\n");
	}
	else if (upper_house_composition >= 5)
	{
		fprintf(output, "	upper_house_composition=state_equal_weight\n");
	}
	else if (upper_house_composition >= 0)
	{
		fprintf(output, "	upper_house_composition=appointed\n");
	}
	else
	{
		fprintf(output, "	upper_house_composition=party_appointed\n");
	}

	if (voting_system >= 10)
	{
		fprintf(output, "	voting_system=proportional_representation\n");
	}
	else if (voting_system >= 5)
	{
		fprintf(output, "	voting_system=jefferson_method\n");
	}
	else
	{
		fprintf(output, "	voting_system=first_past_the_post\n");
	}

	if (public_meetings >= 5)
	{
		fprintf(output, "	public_meetings=yes_meeting\n");
	}
	else
	{
		fprintf(output, "	public_meetings=no_meeting\n");
	}

	if (press_rights >= 10)
	{
		fprintf(output, "	press_rights=free_press\n");
	}
	else if (press_rights >= 5)
	{
		fprintf(output, "	press_rights=censored_press\n");
	}
	else
	{
		fprintf(output, "	press_rights=state_press\n");
	}

	if (trade_unions >= 15)
	{
		fprintf(output, "	trade_unions=all_trade_unions\n");
	}
	else if (trade_unions >= 10)
	{
		fprintf(output, "	trade_unions=non_socialist\n");
	}
	else if (trade_unions >= 5)
	{
		fprintf(output, "	trade_unions=state_controlled\n");
	}
	else
	{
		fprintf(output, "	trade_unions=no_trade_unions\n");
	}

	if (political_parties >= 20)
	{
		fprintf(output, "	political_parties=secret_ballots\n");
	}
	else if (political_parties >= 15)
	{
		fprintf(output, "	political_parties=non_secret_ballots\n");
	}
	else if (political_parties >= 10)
	{
		fprintf(output, "	political_parties=gerrymandering\n");
	}
	else if (political_parties >= 5)
	{
		fprintf(output, "	political_parties=harassment\n");
	}
	else
	{
		fprintf(output, "	political_parties=underground_parties\n");
	}
}


void V2Reforms::governmentEffects(const EU3Country* srcCountry)
{
	string government = srcCountry->getGovernment();
	if (government == "steppe_horde")
	{
		slavery							+= 0;
		vote_franchise					+= 0;
		upper_house_composition		+= -5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "celestial_empire")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= -5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "despotic_monarchy")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "feudal_monarchy")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "merchant_republic")
	{
		slavery							+= 0;
		vote_franchise					+= 5;
		upper_house_composition		+= 0;
		voting_system					+= 10;
		public_meetings				+= 5;
		press_rights					+= 5;
		trade_unions					+= 5;
		political_parties				+= 5;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "imperial_government")
	{
		slavery							+= 0;
		vote_franchise					+= -10;
		upper_house_composition		+= -5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "noble_republic")
	{
		slavery							+= 0;
		vote_franchise					+= 0;
		upper_house_composition		+= 0;
		voting_system					+= 5;
		public_meetings				+= 0;
		press_rights					+= 5;
		trade_unions					+= 0;
		political_parties				+= 5;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "administrative_monarchy")
	{
		slavery							+= 0;
		vote_franchise					+= 5;
		upper_house_composition		+= 5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 5;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "administrative_republic")
	{
		slavery							+= 0;
		vote_franchise					+= 5;
		upper_house_composition		+= 5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 5;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "absolute_monarchy")
	{
		slavery							+= 0;
		vote_franchise					+= -10;
		upper_house_composition		+= -5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "republican_dictatorship")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= -5;
		voting_system					+= 5;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "constitutional_monarchy")
	{
		slavery							+= 0;
		vote_franchise					+= 10;
		upper_house_composition		+= 5;
		voting_system					+= 0;
		public_meetings				+= 5;
		press_rights					+= 0;
		trade_unions					+= 5;
		political_parties				+= 10;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "enlightened_despotism")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "constitutional_republic")
	{
		slavery							+= 0;
		vote_franchise					+= 10;
		upper_house_composition		+= 5;
		voting_system					+= 5;
		public_meetings				+= 5;
		press_rights					+= 0;
		trade_unions					+= 5;
		political_parties				+= 10;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "bureaucratic_despotism")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= 5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "theocratic_government")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "papal_government")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "tribal_despotism")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= -5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "tribal_federation")
	{
		slavery							+= 0;
		vote_franchise					+= 0;
		upper_house_composition		+= -5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "tribal_democracy")
	{
		slavery							+= 0;
		vote_franchise					+= 10;
		upper_house_composition		+= 10;
		voting_system					+= 0;
		public_meetings				+= 5;
		press_rights					+= 5;
		trade_unions					+= 0;
		political_parties				+= 5;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "revolutionary_republic")
	{
		slavery							+= 0;
		vote_franchise					+= 5;
		upper_house_composition		+= 5;
		voting_system					+= 5;
		public_meetings				+= 5;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else if (government == "revolutionary_empire")
	{
		slavery							+= 0;
		vote_franchise					+= -5;
		upper_house_composition		+= -5;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	else 
	{
		log("Error: Undefined government type while setting reforms for %s: %s\n", srcCountry->getTag().c_str(), government.c_str());
	}
}


void V2Reforms::sliderEffects(const EU3Country* srcCountry)
{
	int centralization_decentralization = srcCountry->getCentralizationDecentralization();
	slavery							+= centralization_decentralization * 0;
	vote_franchise					+= centralization_decentralization * 0;
	upper_house_composition		+= centralization_decentralization * 0;
	voting_system					+= centralization_decentralization * 0;
	public_meetings				+= centralization_decentralization * 0;
	press_rights					+= centralization_decentralization * 0;
	trade_unions					+= centralization_decentralization * 0;
	political_parties				+= centralization_decentralization * 0;
	wage_reforms					+= centralization_decentralization * 0;
	work_hours						+= centralization_decentralization * 0;
	safety_regulations			+= centralization_decentralization * 0;
	unemployment_subsidies		+= centralization_decentralization * 0;
	pensions							+= centralization_decentralization * 0;
	health_care						+= centralization_decentralization * 0;
	school_reforms					+= centralization_decentralization * 0;
	
	int aristocracy_plutocracy = srcCountry->getAristocracyPlutocracy();
	slavery							+= aristocracy_plutocracy * 0;
	vote_franchise					+= aristocracy_plutocracy * 0;
	upper_house_composition		+= aristocracy_plutocracy * 1;
	voting_system					+= aristocracy_plutocracy * 0;
	public_meetings				+= aristocracy_plutocracy * 0;
	press_rights					+= aristocracy_plutocracy * 0;
	trade_unions					+= aristocracy_plutocracy * 0;
	political_parties				+= aristocracy_plutocracy * 0;
	wage_reforms					+= aristocracy_plutocracy * 0;
	work_hours						+= aristocracy_plutocracy * 0;
	safety_regulations			+= aristocracy_plutocracy * 0;
	unemployment_subsidies		+= aristocracy_plutocracy * 0;
	pensions							+= aristocracy_plutocracy * 0;
	health_care						+= aristocracy_plutocracy * 0;
	school_reforms					+= aristocracy_plutocracy * 0;

	int serfdom_freesubjects = srcCountry->getSerfdomFreesubjects();
	slavery							+= serfdom_freesubjects * 0;
	vote_franchise					+= serfdom_freesubjects * 2;
	upper_house_composition		+= serfdom_freesubjects * 1;
	voting_system					+= serfdom_freesubjects * 0;
	public_meetings				+= serfdom_freesubjects * 1;
	press_rights					+= serfdom_freesubjects * 1;
	trade_unions					+= serfdom_freesubjects * 1;
	political_parties				+= serfdom_freesubjects * 1;
	wage_reforms					+= serfdom_freesubjects * 0;
	work_hours						+= serfdom_freesubjects * 0;
	safety_regulations			+= serfdom_freesubjects * 0;
	unemployment_subsidies		+= serfdom_freesubjects * 0;
	pensions							+= serfdom_freesubjects * 0;
	health_care						+= serfdom_freesubjects * 0;
	school_reforms					+= serfdom_freesubjects * 0;

	int innovative_narrowminded = srcCountry->getInnovativeNarrowminded();
	slavery							+= innovative_narrowminded * 0;
	vote_franchise					+= innovative_narrowminded * 0;
	upper_house_composition		+= innovative_narrowminded * 0;
	voting_system					+= innovative_narrowminded * -1;
	public_meetings				+= innovative_narrowminded * -1;
	press_rights					+= innovative_narrowminded * -1;
	trade_unions					+= innovative_narrowminded * -1;
	political_parties				+= innovative_narrowminded * -1;
	wage_reforms					+= innovative_narrowminded * -1;
	work_hours						+= innovative_narrowminded * -1;
	safety_regulations			+= innovative_narrowminded * -1;
	unemployment_subsidies		+= innovative_narrowminded * -1;
	pensions							+= innovative_narrowminded * -1;
	health_care						+= innovative_narrowminded * -1;
	school_reforms					+= innovative_narrowminded * 0;
	
	int mercantilism_freetrade = srcCountry->getMercantilismFreetrade();
	slavery							+= mercantilism_freetrade * 0;
	vote_franchise					+= mercantilism_freetrade * 0;
	upper_house_composition		+= mercantilism_freetrade * 0;
	voting_system					+= mercantilism_freetrade * 0;
	public_meetings				+= mercantilism_freetrade * 0;
	press_rights					+= mercantilism_freetrade * 0;
	trade_unions					+= mercantilism_freetrade * 0;
	political_parties				+= mercantilism_freetrade * 0;
	wage_reforms					+= mercantilism_freetrade * 0;
	work_hours						+= mercantilism_freetrade * 0;
	safety_regulations			+= mercantilism_freetrade * 0;
	unemployment_subsidies		+= mercantilism_freetrade * 0;
	pensions							+= mercantilism_freetrade * 0;
	health_care						+= mercantilism_freetrade * 0;
	school_reforms					+= mercantilism_freetrade * 0;
	
	int offensive_defensive = srcCountry->getOffensiveDefensive();
	slavery							+= offensive_defensive * 0;
	vote_franchise					+= offensive_defensive * 0;
	upper_house_composition		+= offensive_defensive * 0;
	voting_system					+= offensive_defensive * 0;
	public_meetings				+= offensive_defensive * 0;
	press_rights					+= offensive_defensive * 0;
	trade_unions					+= offensive_defensive * 0;
	political_parties				+= offensive_defensive * 0;
	wage_reforms					+= offensive_defensive * 0;
	work_hours						+= offensive_defensive * 0;
	safety_regulations			+= offensive_defensive * 0;
	unemployment_subsidies		+= offensive_defensive * 0;
	pensions							+= offensive_defensive * 0;
	health_care						+= offensive_defensive * 0;
	school_reforms					+= offensive_defensive * 0;
	
	int land_naval = srcCountry->getLandNaval();
	slavery							+= land_naval * 0;
	vote_franchise					+= land_naval * 0;
	upper_house_composition		+= land_naval * 0;
	voting_system					+= land_naval * 0;
	public_meetings				+= land_naval * 0;
	press_rights					+= land_naval * 0;
	trade_unions					+= land_naval * 0;
	political_parties				+= land_naval * 0;
	wage_reforms					+= land_naval * 0;
	work_hours						+= land_naval * 0;
	safety_regulations			+= land_naval * 0;
	unemployment_subsidies		+= land_naval * 0;
	pensions							+= land_naval * 0;
	health_care						+= land_naval * 0;
	school_reforms					+= land_naval * 0;
	
	int quality_quantity = srcCountry->getQualityQuantity();
	slavery							+= quality_quantity * 0;
	vote_franchise					+= quality_quantity * 0;
	upper_house_composition		+= quality_quantity * 0;
	voting_system					+= quality_quantity * 0;
	public_meetings				+= quality_quantity * 0;
	press_rights					+= quality_quantity * 0;
	trade_unions					+= quality_quantity * 0;
	political_parties				+= quality_quantity * 0;
	wage_reforms					+= quality_quantity * 0;
	work_hours						+= quality_quantity * 0;
	safety_regulations			+= quality_quantity * 0;
	unemployment_subsidies		+= quality_quantity * 0;
	pensions							+= quality_quantity * 0;
	health_care						+= quality_quantity * 0;
	school_reforms					+= quality_quantity * 0;
}


void V2Reforms::flagEffects(const EU3Country* srcCountry)
{
	if ( srcCountry->hasFlag("") )
	{
		slavery							+= 0;
		vote_franchise					+= 0;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
}


void V2Reforms::modifierEffects(const EU3Country* srcCountry)
{
	if (srcCountry->hasModifier("the_abolish_slavery_act"))
	{
		slavery							+= 1;
		vote_franchise					+= 0;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 0;
	}
	if (srcCountry->hasModifier("the_school_establishment_act"))
	{
		slavery							+= 1;
		vote_franchise					+= 0;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 5;
	}
	if (srcCountry->hasModifier("the_education_act"))
	{
		slavery							+= 1;
		vote_franchise					+= 0;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 5;
	}
	if (srcCountry->hasModifier("sunday_schools"))
	{
		slavery							+= 1;
		vote_franchise					+= 0;
		upper_house_composition		+= 0;
		voting_system					+= 0;
		public_meetings				+= 0;
		press_rights					+= 0;
		trade_unions					+= 0;
		political_parties				+= 0;
		wage_reforms					+= 0;
		work_hours						+= 0;
		safety_regulations			+= 0;
		unemployment_subsidies		+= 0;
		pensions							+= 0;
		health_care						+= 0;
		school_reforms					+= 5;
	}
}


V2UncivReforms::V2UncivReforms(int westernizationProgress, double milFocus, double socioEcoFocus, V2Country* country)
{
	int westernizationCost[16];
	westernizationCost[0]	= 10;
	westernizationCost[1]	= 10;
	westernizationCost[2]	= 10;
	westernizationCost[3]	= 0;
	westernizationCost[4]	= 10;
	westernizationCost[5]	= 10;
	westernizationCost[6]	= 20;
	westernizationCost[7]	= 30;

	westernizationCost[8]	= 10;
	westernizationCost[9]	= 10;
	westernizationCost[10]	= 10;
	westernizationCost[11]	= 15;
	westernizationCost[12]	= 15;
	westernizationCost[13]	= 15;
	westernizationCost[14]	= 15;
	westernizationCost[15]	= 15;

	double socioEconProgress = westernizationProgress * socioEcoFocus;
	for (unsigned int i = 0; i < 8; i++)
	{
		if (socioEconProgress >= westernizationCost[i] - 0.001)
		{
			reforms[i]			= true;
			socioEconProgress	-= westernizationCost[i];
		}
		else
		{
			reforms[i] = false;
		}
	}

	double milProgress = westernizationProgress * milFocus;
	for (unsigned int i = 8; i < 16; i++)
	{
		if (milProgress >= westernizationCost[i] - 0.001)
		{
			reforms[i]	= true;
			milProgress	-= westernizationCost[i];
		}
		else
		{
			reforms[i] = false;
		}
	}

	double remainingProgress = socioEconProgress + milProgress;
	if (socioEconProgress >= milProgress)
	{
		for (unsigned int i = 0; i < 8; i++)
		{
			if (reforms[i]	== true)
			{
				continue;
			}
			else if (remainingProgress >= westernizationCost[i] - 0.001)
			{
				reforms[i]			= true;
				remainingProgress	-=westernizationCost[i];
			}
		}
	}
	else
	{
		for (unsigned int i = 8; i < 16; i++)
		{
			if (reforms[i]	== true)
			{
				continue;
			}
			else if (remainingProgress >= westernizationCost[i] - 0.001)
			{
				reforms[i]			= true;
				remainingProgress	-= westernizationCost[i];
			}
		}
	}

	country->addResearchPoints(remainingProgress * 800);

	if (reforms[5] == true)
	{
		country->addRailroadtoCapitalState();
	}

	if (reforms[9] == true)
	{
		country->addTech("flintlock_rifles");
	}

	if (reforms[10] == true)
	{
		country->addTech("post_napoleonic_thought");
		vector<V2Province*> provinces = country->getProvinces();
		for (vector<V2Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
		{
			if ( (*i)->getNum() == country->getCapital())
			{
				(*i)->setFortLevel(2);
			}
		}
	}

	if (reforms[11] == true)
	{
		country->addTech("military_staff_system");
	}

	if (reforms[15] == true)
	{
		country->addTech("post_nelsonian_thought");
	}
}


void V2UncivReforms::output(FILE* output) const
{
	if (reforms[0]) {
		fprintf(output, "	land_reform=yes_land_reform\n");
	}
	else
	{
		fprintf(output, "	land_reform=no_land_reform\n");
	}

	if (reforms[1]) {
		fprintf(output, "	admin_reform=yes_admin_reform\n");
	}
	else
	{
		fprintf(output, "	admin_reform=no_admin_reform\n");
	}

	if (reforms[3]) {
		fprintf(output, "	finance_reform=finance_reform_two\n");
	}
	else if (reforms[2]) {
		fprintf(output, "	finance_reform=yes_finance_reform\n");
	}
	else
	{
		fprintf(output, "	finance_reform=no_finance_reform\n");
	}

	if (reforms[4]) {
		fprintf(output, "	education_reform=yes_education_reform\n");
	}
	else
	{
		fprintf(output, "	education_reform=no_education_reform\n");
	}

	if (reforms[5]) {
		fprintf(output, "	transport_improv=yes_transport_improv\n");
	}
	else
	{
		fprintf(output, "	transport_improv=no_transport_improv\n");
	}

	if (reforms[6]) {
		fprintf(output, "	pre_indust=yes_pre_indust\n");
	}
	else
	{
		fprintf(output, "	pre_indust=no_pre_indust\n");
	}

	if (reforms[7]) {
		fprintf(output, "	industrial_construction=yes_industrial_construction\n");
	}
	else
	{
		fprintf(output, "	industrial_construction=no_industrial_construction\n");
	}

	if (reforms[8]) {
		fprintf(output, "	foreign_training=yes_foreign_training\n");
	}
	else
	{
		fprintf(output, "	foreign_training=no_foreign_training\n");
	}

	if (reforms[9]) {
		fprintf(output, "	foreign_weapons=yes_foreign_weapons\n");
	}
	else
	{
		fprintf(output, "	foreign_weapons=no_foreign_weapons\n");
	}

	if (reforms[10]) {
		fprintf(output, "	military_constructions=yes_military_constructions\n");
	}
	else
	{
		fprintf(output, "	military_constructions=no_military_constructions\n");
	}

	if (reforms[11]) {
		fprintf(output, "	foreign_officers=yes_foreign_officers\n");
	}
	else
	{
		fprintf(output, "	foreign_officers=no_foreign_officers\n");
	}

	if (reforms[12]) {
		fprintf(output, "	army_schools=yes_army_schools\n");
	}
	else
	{
		fprintf(output, "	army_schools=no_army_schools\n");
	}

	if (reforms[13]) {
		fprintf(output, "	foreign_naval_officers=yes_foreign_naval_officers\n");
	}
	else
	{
		fprintf(output, "	foreign_naval_officers=no_foreign_naval_officers\n");
	}

	if (reforms[14]) {
		fprintf(output, "	naval_schools=yes_naval_schools\n");
	}
	else
	{
		fprintf(output, "	naval_schools=no_naval_schools\n");
	}

	if (reforms[14]) {
		fprintf(output, "	foreign_navies=yes_foreign_navies\n");
	}
	else
	{
		fprintf(output, "	foreign_navies=no_foreign_navies\n");
	}
}