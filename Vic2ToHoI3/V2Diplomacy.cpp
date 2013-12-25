#include "V2Diplomacy.h"
#include "Log.h"

V2Agreement::V2Agreement(Object *obj)
{
	type = obj->getKey();

	std::vector<Object*> objFirst = obj->getValue("first");
	if (objFirst.size() > 0)
	{
		country1 = objFirst[0]->getLeaf();
	}
	else
	{
		log("Error: diplomatic agreement (%s) has no first party.\n", type.c_str());
	}

	std::vector<Object*> objSecond = obj->getValue("second");
	if (objSecond.size() > 0)
	{
		country2 = objSecond[0]->getLeaf();
	}
	else
	{
		log("Error: diplomatic agreement (%s) has no second party.\n", type.c_str());
	}

	std::vector<Object*> objDate = obj->getValue("start_date");
	if (objDate.size() > 0)
	{
		start_date = date(objDate[0]->getLeaf());
	}
}

void V2Diplomacy::init(Object* obj)
{
	std::vector<Object*> objVassals = obj->getValue("vassal");
	for (std::vector<Object*>::iterator itr = objVassals.begin(); itr != objVassals.end(); ++itr)
	{
		V2Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objAlliances = obj->getValue("alliance");
	for (std::vector<Object*>::iterator itr = objAlliances.begin(); itr != objAlliances.end(); ++itr)
	{
		V2Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objCBs = obj->getValue("causus_belli");
	for (std::vector<Object*>::iterator itr = objCBs.begin(); itr != objCBs.end(); ++itr)
	{
		V2Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objSubsidies = obj->getValue("warsubsidy");
	for (std::vector<Object*>::iterator itr = objSubsidies.begin(); itr != objSubsidies.end(); ++itr)
	{
		V2Agreement agr(*itr);
		agreements.push_back(agr);
	}
}
