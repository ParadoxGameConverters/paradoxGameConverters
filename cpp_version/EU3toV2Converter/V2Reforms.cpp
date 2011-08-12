#include "V2Reforms.h"
#include "tempFuncs.h"
#include "Log.h"


void V2Reforms::init(EU3Country* srcCountry)
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
	governmentEffects(srcCountry);
}


void V2Reforms::governmentEffects(EU3Country* srcCountry)
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
	}
	else if (government == "bureaucratic_despotism")
	{
		slavery							+= 0;
		vote_franchise					+= 5;
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
	}
	else 
	{
		log("Error: Undefined government type while setting reforms for %s: %s\n", srcCountry->getTag().c_str(), government.c_str());
	}
}


void V2Reforms::output(FILE* output)
{
	outputTempReforms(output);

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
	if (voting_system >= 5)
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