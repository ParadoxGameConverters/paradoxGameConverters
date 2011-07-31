#include "EU3Country.h"
#include "Log.h"


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
	if (prestigeObj.size() > 0)
	{
		string prestigeString = prestigeObj[0]->getLeaf();
		prestige = 100 * atof( prestigeString.c_str() );
	}
	else
	{
		prestige = -100.0;
	}

	vector<Object*> techsObj = obj->getValue("technology");
	if (techsObj.size() > 0)
	{
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
	else
	{
		landTech			= 0.0;
		navalTech		= 0.0;
		tradeTech		= 0.0;
		productionTech	= 0.0;
		governmentTech	= 0.0;
	}

	vector<Object*> flagObject		= obj->getValue("flags");
	vector<Object*> flagObjects	= flagObject[0]->getLeaves();
	for (unsigned int i = 0; i < flagObjects.size(); i++)
	{
		flags.push_back(flagObjects[i]->getKey());
	}


	possibleDaimyo = false;
	vector<Object*> historyObj	= obj->getValue("history");
	if (historyObj.size() > 0)
	{
		vector<Object*> daimyoObj = historyObj[0]->getValue("daimyo");
		if (daimyoObj.size() > 0)
		{
			possibleDaimyo = true;
		}
	}

	vector<Object*> governmentObj = obj->getValue("government");
	if (governmentObj.size() > 0)
	{
		government = governmentObj[0]->getLeaf();
	}

	// Read international relations leaves
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); ++i)
	{
		string key = leaves[i]->getKey();

		if ((key.size() == 3) && 
			(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
			(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
			(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z'))
		{
			EU3Relations rel;
			rel.init(leaves[i]);
			relations.push_back(rel);
		}
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


void EU3Country::clearProvinces()
{
	provinces.clear();
}


void EU3Country::addCore(EU3Province* core)
{
	cores.push_back(core);
}


void EU3Country::clearCores()
{
	cores.clear();
}


vector<EU3Province*>	EU3Country::getProvinces()
{
	return provinces;
}


vector<EU3Province*>	EU3Country::getCores()
{
	return cores;
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


void EU3Country::setPrestige(double newPrestige)
{
	prestige = newPrestige;
}


double EU3Country::getPrestige()
{
	return prestige;
}


void EU3Country::setLandTech(double newTech)
{
	landTech = newTech;
}


double EU3Country::getLandTech()
{
	return landTech;
}


void EU3Country::setNavalTech(double newTech)
{
	navalTech = newTech;
}


double EU3Country::getNavalTech()
{
	return navalTech;
}


void EU3Country::setTradeTech(double newTech)
{
	tradeTech = newTech;
}


double EU3Country::getTradeTech()
{
	return tradeTech;
}


void EU3Country::setProductionTech(double newTech)
{
	productionTech = newTech;
}


double EU3Country::getProductionTech()
{
	return productionTech;
}


void EU3Country::setGovernmentTech(double newTech)
{
	governmentTech = newTech;
}


double EU3Country::getGovernmentTech()
{
	return governmentTech;
}


vector<string> EU3Country::getFlags()
{
	return flags;
}


bool EU3Country::getPossibleDaimyo()
{
	return possibleDaimyo;
}


string EU3Country::getGovernment()
{
	return government;
}


vector<EU3Relations> EU3Country::getRelations()
{
	return relations;
}