#include "EU4Diplomacy.h"
#include "Log.h"
#include "Parsers\Object.h"



EU4Agreement::EU4Agreement(Object *obj)
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
		startDate = date(objDate[0]->getLeaf());
	}
}


EU4Diplomacy::EU4Diplomacy()
{
	agreements.clear();
}


EU4Diplomacy::EU4Diplomacy(Object* obj)
{
	std::vector<Object*> objRMs = obj->getValue("royal_marriage");
	for (std::vector<Object*>::iterator itr = objRMs.begin(); itr != objRMs.end(); ++itr)
	{
		EU4Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objGaurantees = obj->getValue("guarantee");
	for (std::vector<Object*>::iterator itr = objGaurantees.begin(); itr != objGaurantees.end(); ++itr)
	{
		EU4Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objVassals = obj->getValue("vassal");
	for (std::vector<Object*>::iterator itr = objVassals.begin(); itr != objVassals.end(); ++itr)
	{
		EU4Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objSpheres = obj->getValue("sphere");
	for (std::vector<Object*>::iterator itr = objSpheres.begin(); itr != objSpheres.end(); ++itr)
	{
		EU4Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objAlliances = obj->getValue("alliance");
	for (std::vector<Object*>::iterator itr = objAlliances.begin(); itr != objAlliances.end(); ++itr)
	{
		EU4Agreement agr(*itr);
		agreements.push_back(agr);
	}

	std::vector<Object*> objUnions = obj->getValue("union");
	for (std::vector<Object*>::iterator itr = objUnions.begin(); itr != objUnions.end(); ++itr)
	{
		EU4Agreement agr(*itr);
		agreements.push_back(agr);
	}
}
