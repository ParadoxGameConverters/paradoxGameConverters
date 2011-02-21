#include "tempFuncs.h"


static int popId  = 23000;

void outputHeader(FILE* output)
{
	fprintf(output, "id=\n");
	fprintf(output, "{\n");
	fprintf(output, "	id=2107\n");
	fprintf(output, "	type=4713\n");
	fprintf(output, "}\n");
	fprintf(output, "\n");
}

void outputProvStuff(FILE* output, int provId, string rgoType)
{
	fprintf(output, "	rgo=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		employment=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			province_id=%d\n", provId);
	fprintf(output, "		}\n");
	fprintf(output, "		last_income=0.0\n");
	fprintf(output, "		goods_type=%s\n", rgoType.c_str());
	fprintf(output, "	}\n");
	fprintf(output, "	life_rating=20\n");
}


void outputPopStuff(FILE* output, int population)
{
	fprintf(output, "	aristocrats=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		id=%d\n", popId++);
	fprintf(output, "		size=%d\n", population/3);
	fprintf(output, "		native_american_minor=animist\n");
	fprintf(output, "	}\n");
	fprintf(output, "	artisans=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		id=%d\n", popId++);
	fprintf(output, "		size=%d\n", population/3);
	fprintf(output, "		native_american_minor=animist\n");
	fprintf(output, "	}\n");
	fprintf(output, "	farmers=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		id=%d\n", popId++);
	fprintf(output, "		size=%d\n", population/3);
	fprintf(output, "		native_american_minor=animist\n");
	fprintf(output, "	}\n");
}


void outputCountryHeader(FILE* output)
{
	fprintf(output, "	wage_reform=no_minimum_wage\n");
	fprintf(output, "	work_hours=no_work_hour_limit\n");
	fprintf(output, "	safety_regulations=trinket_safety\n");
	fprintf(output, "	unemployment_subsidies=trinket_subsidies\n");
	fprintf(output, "	pensions=no_pensions\n");
	fprintf(output, "	health_care=trinket_health_care\n");
	fprintf(output, "	slavery=no_slavery\n");
	fprintf(output, "	vote_franschise=wealth_voting\n");
	fprintf(output, "	upper_house_composition=appointed\n");
	fprintf(output, "	voting_system=first_past_the_post\n");
	fprintf(output, "	public_meetings=yes_meeting\n");
	fprintf(output, "	press_rights=censored_press\n");
	fprintf(output, "	trade_unions=non_socialist\n");
	fprintf(output, "	political_parties=non_secret_ballots\n");
	fprintf(output, "	upper_house=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		reactionary=0.04999\n");
	fprintf(output, "		conservative=0.82999\n");
	fprintf(output, "		liberal=0.12000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	ruling_party=1\n");
	fprintf(output, "	active_party=1\n");
	fprintf(output, "	active_party=2\n");
	fprintf(output, "	active_party=5\n");
	fprintf(output, "	rich_tax=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		current=0.00003\n");
	fprintf(output, "		tax_income=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			0.00000 282668.31470 0.00000 0.00000 152561.20709 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000\n");
	fprintf(output, "		}\n");
	fprintf(output, "		tax_eff=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			0.00000 688123.59158 0.00000 0.00000 430797.42029 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000\n");
	fprintf(output, "		}\n");
	fprintf(output, "		total=1118921.01187\n");
	fprintf(output, "		rangeLimitMax=0.50000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=22\n");
	fprintf(output, "		min_tax=21\n");
	fprintf(output, "	}\n");
	fprintf(output, "	middle_tax=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		current=0.06256\n");
	fprintf(output, "		tax_income=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			0.00000 0.00000 725076.43088 5609.40536 0.00000 13224.41605 19321.00604 0.00000 0.00000 0.00000 1179.08530 0.00000 0.00000\n");
	fprintf(output, "		}\n");
	fprintf(output, "		tax_eff=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			0.00000 0.00000 1792486.48352 11217.41019 0.00000 26446.96585 62271.06470 0.00000 0.00000 0.00000 2357.68372 0.00000 0.00000\n");
	fprintf(output, "		}\n");
	fprintf(output, "		total=1894779.60797\n");
	fprintf(output, "		rangeLimitMax=0.50000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=22\n");
	fprintf(output, "		min_tax=21\n");
	fprintf(output, "	}\n");
	fprintf(output, "	poor_tax=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		current=0.11591\n");
	fprintf(output, "		tax_income=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 133922.48761 2311114.97299 229248.16516 0.00000 0.00000 14694.64551\n");
	fprintf(output, "		}\n");
	fprintf(output, "		tax_eff=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 0.00000 368526.35513 5680316.87845 461141.38962 0.00000 0.00000 29381.26956\n");
	fprintf(output, "		}\n");
	fprintf(output, "		total=6539365.89276\n");
	fprintf(output, "		rangeLimitMax=0.50000\n");
	fprintf(output, "		rangeLimitMin=0.00000\n");
	fprintf(output, "		max_tax=22\n");
	fprintf(output, "		min_tax=21\n");
	fprintf(output, "	}\n");
}


void outputCountryMiddle(FILE* output)
{
	fprintf(output, "	money=75314.11652\n");
}


void outputStateStuff(FILE* output)
{
	fprintf(output, "		is_colonial=yes\n");
	fprintf(output, "		savings=0.15726\n");
	fprintf(output, "		interest=0.00000\n");
	fprintf(output, "		popproject=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			input_goods=\n");
	fprintf(output, "			{\n");
	fprintf(output, "			}\n");
	fprintf(output, "			money=0.00000\n");
	fprintf(output, "			building=33\n");
	fprintf(output, "			province=14\n");
	fprintf(output, "			index=0\n");
	fprintf(output, "			type=1\n");
	fprintf(output, "			money2=yes\n");
	fprintf(output, "		}\n");
}