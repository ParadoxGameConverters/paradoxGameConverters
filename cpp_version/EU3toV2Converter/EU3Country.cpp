#include "EU3Country.h"


void EU3Country::init(Object* obj)
{
	tag = obj->getKey();

	vector<Object*> capitalObj = obj->getValue("capital");
	if (capitalObj.size() > 0)
	{
		capital = atoi( capitalObj[0]->getLeaf().c_str() );
	}
	else
	{
		capital = 0;
	}

	vector<Object*> techGroupObj = obj->getValue("technology_group");
	if (techGroupObj.size() > 0)
	{
		techGroup = techGroupObj[0]->getLeaf().c_str();
	}
	else
	{
		log("Error: Could not load the tech group for %s\n", tag.c_str());
	}

	vector<Object*> primaryCultureObj = obj->getValue("primary_culture");
	if (primaryCultureObj.size() > 0)
	{
		primaryCulture = primaryCultureObj[0]->getLeaf().c_str();
	}

	vector<Object*> acceptedCultureObj = obj->getValue("accepted_culture");
	for (unsigned int i = 0; i < acceptedCultureObj.size(); i++)
	{
		acceptedCultures.push_back(acceptedCultureObj[i]->getLeaf().c_str());
	}

	vector<Object*> religionObj = obj->getValue("religion");
	if (religionObj.size() > 0)
	{
		religion = religionObj[0]->getLeaf().c_str();
	}

	vector<Object*> prestigeObj = obj->getValue("prestige");
	string prestigeString = prestigeObj[0]->getLeaf();
	prestige = 100 * atof( prestigeString.c_str() );

	vector<Object*> techsObj = obj->getValue("technology");
	vector<Object*> techObj = techsObj[0]->getValue("land_tech");
	string techString = techObj[0]->getToken(0);
	landTech = atof( techString.c_str() );

	techsObj = obj->getValue("technology");
	techObj = techsObj[0]->getValue("naval_tech");
	techString = techObj[0]->getToken(0);
	navalTech = atof( techString.c_str() );

	techsObj = obj->getValue("technology");
	techObj = techsObj[0]->getValue("trade_tech");
	techString = techObj[0]->getToken(0);
	tradeTech = atof( techString.c_str() );

	techsObj = obj->getValue("technology");
	techObj = techsObj[0]->getValue("production_tech");
	techString = techObj[0]->getToken(0);
	productionTech = atof( techString.c_str() );

	techsObj = obj->getValue("technology");
	techObj = techsObj[0]->getValue("government_tech");
	techString = techObj[0]->getToken(0);
	governmentTech = atof( techString.c_str() );
}


string EU3Country::getTag()
{
	return tag;
}


void EU3Country::addProvince(EU3Province* province)
{
	provinces.push_back(province);
}


vector<EU3Province*>	EU3Country::getProvinces()
{
	return provinces;
}


int EU3Country::getCapital()
{
	return capital;
}


string EU3Country::getTechGroup()
{
	return techGroup;
}


string EU3Country::getPrimaryCulture()
{
	return primaryCulture;
}


vector<string> EU3Country::getAcceptedCultures()
{
	return acceptedCultures;
}

string EU3Country::getReligion()
{
	return religion;
}

double EU3Country::getPrestige()
{
	return prestige;
}


double EU3Country::getLandTech()
{
	return landTech;
}


double EU3Country::getNavalTech()
{
	return navalTech;
}


double EU3Country::getTradeTech()
{
	return tradeTech;
}


double EU3Country::getProductionTech()
{
	return productionTech;
}


double EU3Country::getGovernmentTech()
{
	return governmentTech;
}