#include "CK2Barony.h"
#include "CK2Title.h"
#include "CK2Character.h"
#include "..\Parsers\Object.h"
#include "..\Log.h"



CK2Barony::CK2Barony(Object* obj, CK2Title* newTitle, CK2Province* newProvince)
{
	title		= newTitle;
	province	= newProvince;

	type = obj->getLeaf("type");

	vector<Object*> leaves = obj->getLeaves();
	for (vector<Object*>::iterator i = leaves.begin(); i < leaves.end(); i++)
	{
		string key = (*i)->getKey();
		if ( (key.substr(0, 3) == "ca_") || (key.substr(0, 3) == "ct_") || (key.substr(0, 3) == "tp_") )
		{
			buildings.insert( make_pair(key, true) );
		}
	}

	proxyMultiplier = 1;
	if (title->getTitleString() == title->getHolder()->getCapitalString())
	{
		vector<CK2Title*> titles = title->getHolder()->getTitles();
		for (vector<CK2Title*>::const_iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
		{
			if ( ((*titleItr)->getTitleString().substr(0,2) == "c_") && (proxyMultiplier < 2) )
			{
				proxyMultiplier = 2;
			}
			if ( ((*titleItr)->getTitleString().substr(0,2) == "d_") && (proxyMultiplier < 3) )
			{
				proxyMultiplier = 3;
			}
			if ( ((*titleItr)->getTitleString().substr(0,2) == "k_") && (proxyMultiplier < 4) )
			{
				proxyMultiplier = 4;
			}
			if ( ((*titleItr)->getTitleString().substr(0,2) == "e_") && (proxyMultiplier < 5) )
			{
				proxyMultiplier = 5;
			}
		}
	}

	determineBaseTaxProxy();
	determinePopProxy();
	determineManpowerProxy();
}


void CK2Barony::determineBaseTaxProxy()
{
	baseTaxProxy = 0.0f;
	if (type == "city")
	{
		baseTaxProxy += 12.0f;
		if (buildings.find("ct_wall_1")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ct_wall_2")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ct_wall_3")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ct_wall_4")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ct_wall_5")->second)
		{
			baseTaxProxy += 0.5f;
		}
		if (buildings.find("ct_marketplace_1")->second)
		{
			baseTaxProxy += 2.5f;
		}
		if (buildings.find("ct_marketplace_2")->second)
		{
			baseTaxProxy += 3.0f;
		}
		if (buildings.find("ct_marketplace_3")->second)
		{
			baseTaxProxy += 3.5f;
		}
		if (buildings.find("ct_marketplace_4")->second)
		{
			baseTaxProxy += 4.0f;
		}
		if (buildings.find("ct_marketplace_5")->second)
		{
			baseTaxProxy += 4.5f;
		}
		if (buildings.find("ct_marketplace_6")->second)
		{
			baseTaxProxy += 5.0f;
		}
		if (buildings.find("ct_port_1")->second)
		{
			baseTaxProxy += 2.5f;
		}
		if (buildings.find("ct_port_2")->second)
		{
			baseTaxProxy += 3.0f;
		}
		if (buildings.find("ct_port_3")->second)
		{
			baseTaxProxy += 3.5f;
		}
		if (buildings.find("ct_port_4")->second)
		{
			baseTaxProxy += 4.0f;
		}
		if (buildings.find("ct_port_5")->second)
		{
			baseTaxProxy += 4.5f;
		}
		if (buildings.find("ct_marketplace_1_mus")->second)
		{
			baseTaxProxy += 2.5f;
		}
		if (buildings.find("ct_marketplace_2_mus")->second)
		{
			baseTaxProxy += 3.0f;
		}
		if (buildings.find("ct_marketplace_3_mus")->second)
		{
			baseTaxProxy += 3.5f;
		}
		if (buildings.find("ct_marketplace_4_mus")->second)
		{
			baseTaxProxy += 4.0f;
		}
		if (buildings.find("ct_marketplace_5_mus")->second)
		{
			baseTaxProxy += 4.5f;
		}
		if (buildings.find("ct_marketplace_6_mus")->second)
		{
			baseTaxProxy += 5.0f;
		}
	}
	else if (type == "castle")
	{
		baseTaxProxy += 4.0f;
		if (buildings.find("ca_wall_1")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ca_wall_2")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ca_wall_3")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ca_wall_4")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("ca_wall_5")->second)
		{
			baseTaxProxy += 0.5f;
		}
		if (buildings.find("ca_town_1")->second)
		{
			baseTaxProxy += 1.5f;
		}
		if (buildings.find("ca_town_2")->second)
		{
			baseTaxProxy += 2.0f;
		}
		if (buildings.find("ca_town_3")->second)
		{
			baseTaxProxy += 2.5f;
		}
		if (buildings.find("ca_town_4")->second)
		{
			baseTaxProxy += 3.0f;
		}
		if (buildings.find("ca_town_5")->second)
		{
			baseTaxProxy += 3.5f;
		}
	}
	else if (type == "temple")
	{
		baseTaxProxy += 8.0f;
		if (buildings.find("tp_wall_1")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("tp_wall_2")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("tp_wall_3")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("tp_wall_4")->second)
		{
			baseTaxProxy += 0.2f;
		}
		if (buildings.find("tp_wall_5")->second)
		{
			baseTaxProxy += 0.5f;
		}
		if (buildings.find("tp_town_1")->second)
		{
			baseTaxProxy += 1.5f;
		}
		if (buildings.find("tp_town_2")->second)
		{
			baseTaxProxy += 2.0f;
		}
		if (buildings.find("tp_town_3")->second)
		{
			baseTaxProxy += 2.5f;
		}
		if (buildings.find("tp_town_4")->second)
		{
			baseTaxProxy += 3.0f;
		}
		if (buildings.find("tp_town_5")->second)
		{
			baseTaxProxy += 3.5f;
		}
		if (buildings.find("tp_town_1_mus")->second)
		{
			baseTaxProxy += 1.2f;
		}
		if (buildings.find("tp_town_2_mus")->second)
		{
			baseTaxProxy += 1.7f;
		}
		if (buildings.find("tp_town_3_mus")->second)
		{
			baseTaxProxy += 2.2f;
		}
		if (buildings.find("tp_town_4_mus")->second)
		{
			baseTaxProxy += 2.7f;
		}
		if (buildings.find("tp_town_5_mus")->second)
		{
			baseTaxProxy += 3.2f;
		}
		if (buildings.find("tp_town_1_mus")->second)
		{
			baseTaxProxy += 1.2f;
		}
	}
	else
	{
		log("Note! Unhandled barony type %s\n", type.c_str());
	}

	baseTaxProxy *= proxyMultiplier;
}


void CK2Barony::determinePopProxy()
{
	popProxy = 0.0f;
	if (type == "city")
	{
		popProxy += 12.0f;
		if (buildings.find("ct_marketplace_1")->second)
		{
			popProxy += 2.5f;
		}
		if (buildings.find("ct_marketplace_2")->second)
		{
			popProxy += 3.0f;
		}
		if (buildings.find("ct_marketplace_3")->second)
		{
			popProxy += 3.5f;
		}
		if (buildings.find("ct_marketplace_4")->second)
		{
			popProxy += 4.0f;
		}
		if (buildings.find("ct_marketplace_5")->second)
		{
			popProxy += 4.5f;
		}
		if (buildings.find("ct_marketplace_6")->second)
		{
			popProxy += 5.0f;
		}
		if (buildings.find("ct_port_1")->second)
		{
			popProxy += 2.5f;
		}
		if (buildings.find("ct_port_2")->second)
		{
			popProxy += 3.0f;
		}
		if (buildings.find("ct_port_3")->second)
		{
			popProxy += 3.5f;
		}
		if (buildings.find("ct_port_4")->second)
		{
			popProxy += 4.0f;
		}
		if (buildings.find("ct_port_5")->second)
		{
			popProxy += 4.5f;
		}
		if (buildings.find("ct_marketplace_1_mus")->second)
		{
			popProxy += 2.5f;
		}
		if (buildings.find("ct_marketplace_2_mus")->second)
		{
			popProxy += 3.0f;
		}
		if (buildings.find("ct_marketplace_3_mus")->second)
		{
			popProxy += 3.5f;
		}
		if (buildings.find("ct_marketplace_4_mus")->second)
		{
			popProxy += 4.0f;
		}
		if (buildings.find("ct_marketplace_5_mus")->second)
		{
			popProxy += 4.5f;
		}
		if (buildings.find("ct_marketplace_6_mus")->second)
		{
			popProxy += 5.0f;
		}
	}
	else if (type == "castle")
	{
		popProxy += 4.0f;
		if (buildings.find("ca_town_1")->second)
		{
			popProxy += 1.5f;
		}
		if (buildings.find("ca_town_2")->second)
		{
			popProxy += 2.0f;
		}
		if (buildings.find("ca_town_3")->second)
		{
			popProxy += 2.5f;
		}
		if (buildings.find("ca_town_4")->second)
		{
			popProxy += 3.0f;
		}
		if (buildings.find("ca_town_5")->second)
		{
			popProxy += 3.5f;
		}
	}
	else if (type == "temple")
	{
		popProxy += 8.0f;
		if (buildings.find("tp_town_1")->second)
		{
			popProxy += 1.5f;
		}
		if (buildings.find("tp_town_2")->second)
		{
			popProxy += 2.0f;
		}
		if (buildings.find("tp_town_3")->second)
		{
			popProxy += 2.5f;
		}
		if (buildings.find("tp_town_4")->second)
		{
			popProxy += 3.0f;
		}
		if (buildings.find("tp_town_5")->second)
		{
			popProxy += 3.5f;
		}
		if (buildings.find("tp_town_1_mus")->second)
		{
			popProxy += 1.2f;
		}
		if (buildings.find("tp_town_2_mus")->second)
		{
			popProxy += 1.7f;
		}
		if (buildings.find("tp_town_3_mus")->second)
		{
			popProxy += 2.2f;
		}
		if (buildings.find("tp_town_4_mus")->second)
		{
			popProxy += 2.7f;
		}
		if (buildings.find("tp_town_5_mus")->second)
		{
			popProxy += 3.2f;
		}
		if (buildings.find("tp_town_1_mus")->second)
		{
			popProxy += 1.2f;
		}
	}
	else
	{
		log("Note! Unhandled barony type %s\n", type.c_str());
	}

	popProxy *= proxyMultiplier;
}


void CK2Barony::determineManpowerProxy()
{
	manpowerProxy = 0.0f;
	if (type == "castle")
	{
		manpowerProxy += 225;
		if (buildings.find("ca_militia_barracks_1")->second)
		{
			manpowerProxy += 90;
		}
		if (buildings.find("ca_militia_barracks_2")->second)
		{
			manpowerProxy += 110;
		}
		if (buildings.find("ca_militia_barracks_3")->second)
		{
			manpowerProxy += 150;
		}
		if (buildings.find("ca_barracks_1")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("ca_barracks_2")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("ca_barracks_3")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ca_barracks_4")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ca_barracks_5")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_barracks_6")->second)
		{
			manpowerProxy += 95;
		}
		if (buildings.find("ca_stable_1")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("ca_stable_2")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("ca_stable_3")->second)
		{
			manpowerProxy += 60;
		}
		if (buildings.find("ca_stable_4")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ca_stable_5")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ca_warrior_cult")->second)
		{
			manpowerProxy += 200;
		}
		if (buildings.find("ca_culture_saxon_english_1")->second)
		{
			manpowerProxy += 60;
		}
		if (buildings.find("ca_culture_saxon_english_2")->second)
		{
			manpowerProxy += 80;
		}
		if (buildings.find("ca_culture_saxon_english_3")->second)
		{
			manpowerProxy += 100;
		}
		if (buildings.find("ca_culture_saxon_english_4")->second)
		{
			manpowerProxy += 120;
		}
		if (buildings.find("ca_culture_scottish_1")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ca_culture_scottish_2")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ca_culture_scottish_3")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_culture_scottish_4")->second)
		{
			manpowerProxy += 95;
		}
		if (buildings.find("ca_culture_group_baltic_1")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ca_culture_group_baltic_2")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ca_culture_group_baltic_3")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_culture_group_baltic_4")->second)
		{
			manpowerProxy += 95;
		}
		if (buildings.find("ca_culture_group_altaic_1")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("ca_culture_group_altaic_2")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ca_culture_group_altaic_3")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ca_culture_group_altaic_4")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_culture_frank_norman_german_1")->second)
		{
			manpowerProxy += 35;
		}
		if (buildings.find("ca_culture_frank_norman_german_2")->second)
		{
			manpowerProxy += 40;
		}
		if (buildings.find("ca_culture_frank_norman_german_3")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("ca_culture_frank_norman_german_4")->second)
		{
			manpowerProxy += 50;
		}
		if (buildings.find("ca_culture_group_iberian_1")->second)
		{
			manpowerProxy += 30;
		}
		if (buildings.find("ca_culture_group_iberian_2")->second)
		{
			manpowerProxy += 40;
		}
		if (buildings.find("ca_culture_group_iberian_3")->second)
		{
			manpowerProxy += 50;
		}
		if (buildings.find("ca_culture_group_iberian_4")->second)
		{
			manpowerProxy += 60;
		}
		if (buildings.find("ca_culture_italian_1")->second)
		{
			manpowerProxy += 35;
		}
		if (buildings.find("ca_culture_italian_2")->second)
		{
			manpowerProxy += 50;
		}
		if (buildings.find("ca_culture_italian_3")->second)
		{
			manpowerProxy += 60;
		}
		if (buildings.find("ca_culture_italian_4")->second)
		{
			manpowerProxy += 70;
		}
		if (buildings.find("ca_culture_russian_1")->second)
		{
			manpowerProxy += 35;
		}
		if (buildings.find("ca_culture_russian_2")->second)
		{
			manpowerProxy += 50;
		}
		if (buildings.find("ca_culture_russian_3")->second)
		{
			manpowerProxy += 60;
		}
		if (buildings.find("ca_culture_russian_4")->second)
		{
			manpowerProxy += 70;
		}
		if (buildings.find("ca_culture_group_north_germanic_1")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ca_culture_group_north_germanic_2")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ca_culture_group_north_germanic_3")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_culture_group_north_germanic_4")->second)
		{
			manpowerProxy += 95;
		}
		if (buildings.find("ca_culture_hungarian_1")->second)
		{
			manpowerProxy += 30;
		}
		if (buildings.find("ca_culture_hungarian_2")->second)
		{
			manpowerProxy += 40;
		}
		if (buildings.find("ca_culture_hungarian_3")->second)
		{
			manpowerProxy += 50;
		}
		if (buildings.find("ca_culture_hungarian_4")->second)
		{
			manpowerProxy += 60;
		}
		if (buildings.find("ca_culture_group_arabic_1")->second)
		{
			manpowerProxy += 30;
		}
		if (buildings.find("ca_culture_group_arabic_2")->second)
		{
			manpowerProxy += 40;
		}
		if (buildings.find("ca_culture_group_arabic_3")->second)
		{
			manpowerProxy += 50;
		}
		if (buildings.find("ca_culture_group_arabic_4")->second)
		{
			manpowerProxy += 60;
		}
		if (buildings.find("ca_culture_group_byzantine_1")->second)
		{
			manpowerProxy += 30;
		}
		if (buildings.find("ca_culture_group_byzantine_2")->second)
		{
			manpowerProxy += 35;
		}
		if (buildings.find("ca_culture_group_byzantine_3")->second)
		{
			manpowerProxy += 40;
		}
		if (buildings.find("ca_culture_group_byzantine_4")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("ca_culture_irish_1")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ca_culture_irish_2")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ca_culture_irish_3")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_culture_irish_4")->second)
		{
			manpowerProxy += 95;
		}
		if (buildings.find("ca_culture_ethiopian_1")->second)
		{
			manpowerProxy += 110;
		}
		if (buildings.find("ca_culture_ethiopian_2")->second)
		{
			manpowerProxy += 130;
		}
		if (buildings.find("ca_culture_ethiopian_3")->second)
		{
			manpowerProxy += 150;
		}
		if (buildings.find("ca_culture_ethiopian_4")->second)
		{
			manpowerProxy += 170;
		}
		if (buildings.find("ca_culture_west_african_1")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_culture_west_african_2")->second)
		{
			manpowerProxy += 125;
		}
		if (buildings.find("ca_culture_west_african_3")->second)
		{
			manpowerProxy += 165;
		}
		if (buildings.find("ca_culture_west_african_4")->second)
		{
			manpowerProxy += 205;
		}
		if (buildings.find("ca_culture_group_south_slavic_1")->second)
		{
			manpowerProxy += 25;
		}
		if (buildings.find("ca_culture_group_south_slavic_2")->second)
		{
			manpowerProxy += 35;
		}
		if (buildings.find("ca_culture_group_south_slavic_3")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("ca_culture_group_south_slavic_4")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("ca_culture_dutch_1")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("ca_culture_dutch_2")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ca_culture_dutch_3")->second)
		{
			manpowerProxy += 125;
		}
		if (buildings.find("ca_culture_dutch_4")->second)
		{
			manpowerProxy += 155;
		}

		if (buildings.find("ca_wall_1")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("ca_wall_2")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("ca_wall_3")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("ca_wall_4")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("ca_wall_5")->second)
		{
			manpowerProxy *= 1.1f;
		}
		if (buildings.find("ca_keep_1")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("ca_keep_2")->second)
		{
			manpowerProxy *= 1.1f;
		}
		if (buildings.find("ca_keep_3")->second)
		{
			manpowerProxy *= 1.15f;
		}
		if (buildings.find("ca_keep_4")->second)
		{
			manpowerProxy *= 1.2f;
		}
		if (buildings.find("ca_keep_5")->second)
		{
			manpowerProxy *= 1.25f;
		}
		if (buildings.find("ca_keep_6")->second)
		{
			manpowerProxy *= 1.3f;
		}
		if (buildings.find("ca_keep_1_mus")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("ca_keep_2_mus")->second)
		{
			manpowerProxy *= 1.1f;
		}
		if (buildings.find("ca_keep_3_mus")->second)
		{
			manpowerProxy *= 1.15f;
		}
		if (buildings.find("ca_keep_4_mus")->second)
		{
			manpowerProxy *= 1.2f;
		}
		if (buildings.find("ca_keep_5_mus")->second)
		{
			manpowerProxy *= 1.25f;
		}
		if (buildings.find("ca_keep_6_mus")->second)
		{
			manpowerProxy *= 1.3f;
		}
	}
	else if (type == "city")
	{
		manpowerProxy += 115;

		if (buildings.find("ct_barracks_1")->second)
		{
			manpowerProxy += 90;
		}
		if (buildings.find("ct_barracks_2")->second)
		{
			manpowerProxy += 110;
		}
		if (buildings.find("ct_barracks_3")->second)
		{
			manpowerProxy += 130;
		}
		if (buildings.find("ct_barracks_4")->second)
		{
			manpowerProxy += 150;
		}
		if (buildings.find("ct_barracks_5")->second)
		{
			manpowerProxy += 170;
		}
		if (buildings.find("ct_barracks_6")->second)
		{
			manpowerProxy += 190;
		}
		if (buildings.find("ct_guard_1")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("ct_guard_2")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("ct_guard_3")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("ct_guard_4")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("ct_guard_5")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("ct_barracks_1")->second)
		{
			manpowerProxy += 90;
		}

		if (buildings.find("ct_wall_1")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("ct_wall_2")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("ct_wall_3")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("ct_wall_4")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("ct_wall_5")->second)
		{
			manpowerProxy *= 1.075f;
		}
		if (buildings.find("ct_training_grounds_1")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("ct_training_grounds_2")->second)
		{
			manpowerProxy *= 1.1f;
		}
		if (buildings.find("ct_training_grounds_3")->second)
		{
			manpowerProxy *= 1.1f;
		}
		if (buildings.find("ct_training_grounds_4")->second)
		{
			manpowerProxy *= 1.125f;
		}
		if (buildings.find("ct_training_grounds_5")->second)
		{
			manpowerProxy *= 1.15f;
		}
		if (buildings.find("ct_training_grounds_6")->second)
		{
			manpowerProxy *= 1.15f;
		}
	}
	else if (type == "temple")
	{
		manpowerProxy += 125;
		if (buildings.find("tp_barracks_1")->second)
		{
			manpowerProxy += 90;
		}
		if (buildings.find("tp_barracks_2")->second)
		{
			manpowerProxy += 110;
		}
		if (buildings.find("tp_barracks_3")->second)
		{
			manpowerProxy += 130;
		}
		if (buildings.find("tp_barracks_4")->second)
		{
			manpowerProxy += 150;
		}
		if (buildings.find("tp_barracks_5")->second)
		{
			manpowerProxy += 170;
		}
		if (buildings.find("tp_barracks_6")->second)
		{
			manpowerProxy += 190;
		}
		if (buildings.find("tp_elite_barracks_1")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("tp_elite_barracks_2")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("tp_elite_barracks_3")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("tp_elite_barracks_4")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("tp_elite_barracks_5")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("tp_elite_barracks_6")->second)
		{
			manpowerProxy += 95;
		}
		if (buildings.find("tp_barracks_1_mus")->second)
		{
			manpowerProxy += 90;
		}
		if (buildings.find("tp_barracks_2_mus")->second)
		{
			manpowerProxy += 110;
		}
		if (buildings.find("tp_barracks_3_mus")->second)
		{
			manpowerProxy += 130;
		}
		if (buildings.find("tp_barracks_4_mus")->second)
		{
			manpowerProxy += 150;
		}
		if (buildings.find("tp_barracks_5_mus")->second)
		{
			manpowerProxy += 170;
		}
		if (buildings.find("tp_barracks_6_mus")->second)
		{
			manpowerProxy += 190;
		}
		if (buildings.find("tp_elite_barracks_1_mus")->second)
		{
			manpowerProxy += 45;
		}
		if (buildings.find("tp_elite_barracks_2_mus")->second)
		{
			manpowerProxy += 55;
		}
		if (buildings.find("tp_elite_barracks_3_mus")->second)
		{
			manpowerProxy += 65;
		}
		if (buildings.find("tp_elite_barracks_4_mus")->second)
		{
			manpowerProxy += 75;
		}
		if (buildings.find("tp_elite_barracks_5_mus")->second)
		{
			manpowerProxy += 85;
		}
		if (buildings.find("tp_elite_barracks_6_mus")->second)
		{
			manpowerProxy += 95;
		}

		if (buildings.find("tp_wall_1")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("tp_wall_2")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("tp_wall_3")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("tp_wall_4")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("tp_wall_5")->second)
		{
			manpowerProxy *= 1.075f;
		}
		if (buildings.find("tp_monastery_1")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("tp_monastery_2")->second)
		{
			manpowerProxy *= 1.075f;
		}
		if (buildings.find("tp_monastery_3")->second)
		{
			manpowerProxy *= 1.1f;
		}
		if (buildings.find("tp_monastery_4")->second)
		{
			manpowerProxy *= 1.125f;
		}
		if (buildings.find("tp_monastery_5")->second)
		{
			manpowerProxy *= 1.15f;
		}
		if (buildings.find("tp_monastery_6")->second)
		{
			manpowerProxy *= 1.15f;
		}
		if (buildings.find("tp_wall_1_mus")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("tp_wall_2_mus")->second)
		{
			manpowerProxy *= 1.025f;
		}
		if (buildings.find("tp_wall_3_mus")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("tp_wall_4_mus")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("tp_wall_5_mus")->second)
		{
			manpowerProxy *= 1.075f;
		}
		if (buildings.find("tp_monastery_1_mus")->second)
		{
			manpowerProxy *= 1.05f;
		}
		if (buildings.find("tp_monastery_2_mus")->second)
		{
			manpowerProxy *= 1.075f;
		}
		if (buildings.find("tp_monastery_3_mus")->second)
		{
			manpowerProxy *= 1.1f;
		}
		if (buildings.find("tp_monastery_4_mus")->second)
		{
			manpowerProxy *= 1.125f;
		}
		if (buildings.find("tp_monastery_5_mus")->second)
		{
			manpowerProxy *= 1.15f;
		}
		if (buildings.find("tp_monastery_6_mus")->second)
		{
			manpowerProxy *= 1.15f;
		}
	}
	else
	{
		log("Note! Unhandled barony type %s\n", type.c_str());
	}

	manpowerProxy *= proxyMultiplier;
}