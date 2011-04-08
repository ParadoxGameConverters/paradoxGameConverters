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