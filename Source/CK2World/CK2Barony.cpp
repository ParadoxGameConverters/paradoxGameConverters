#include "CK2Barony.h"
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
}


float CK2Barony::getPopProxy() const
{
	float popProxy = 0;
	if (type == "city")
	{
		popProxy += 12.0f;
	}
	else if (type == "castle")
	{
		popProxy += 4.0f;
	}
	else if (type == "temple")
	{
		popProxy += 8.0f;
	}
	else
	{
		log("Note! Unhandled barony type %s\n", type.c_str());
	}

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

	return popProxy;
}