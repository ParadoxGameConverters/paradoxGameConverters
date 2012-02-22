#include "V2Country.h"
#include "Log.h"
#include <algorithm>


void V2Country::init(Object* obj)
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

	vector<Object*> primaryCultureObj = obj->getValue("primary_culture");
	if (primaryCultureObj.size() > 0)
	{
		primaryCulture = primaryCultureObj[0]->getLeaf().c_str();
	}

	vector<Object*> techObj = obj->getValue("technology");
	if (techObj.size() > 0)
	{
		techs = techObj[0]->getKeys();
	}

	for (int i = 0; i <= naval_exercises; ++i)
	{
		inventions[i] = illegal; // we don't care if they're possible or not
	}

	techObj = obj->getValue("active_inventions");
	if (techObj.size() > 0)
	{
		vector<string> active_inventions = techObj[0]->getTokens();
		for (vector<string>::iterator itr = active_inventions.begin(); itr != active_inventions.end(); ++itr)
		{
			int i = atoi(itr->c_str());
			inventions[i] = active;
		}
	}

	vector<Object*> uhObj = obj->getValue("upper_house");
	if (uhObj.size() > 0)
	{
		vector<Object*> uhComp = uhObj[0]->getLeaves();
		for (vector<Object*>::iterator itr = uhComp.begin(); itr != uhComp.end(); ++itr)
		{
			upperHouseComposition[(*itr)->getKey()] = atof((*itr)->getLeaf().c_str());
		}
	}

	vector<Object*> partyObj = obj->getValue("ruling_party");
	if (partyObj.size() > 0)
	{
		rulingParty = atoi(partyObj[0]->getLeaf().c_str());
	}

	partyObj = obj->getValue("active_party");
	for (vector<Object*>::iterator itr = partyObj.begin(); itr != partyObj.end(); ++itr)
	{
		parties.push_back(atoi((*itr)->getLeaf().c_str()));
	}

	vector<Object*> moneyObj = obj->getValue("money");
	if (moneyObj.size() > 0)
	{
		money = atof(moneyObj[0]->getLeaf().c_str());
	}

	vector<Object*> diploObj = obj->getValue("diplomatic_points");
	if (diploObj.size() > 0)
	{
		diploPoints = atof(diploObj[0]->getLeaf().c_str());
	}

	vector<Object*> govObj = obj->getValue("government");
	if (govObj.size() > 0)
	{
		government = govObj[0]->getLeaf();
	}

	// Read spending
	vector<Object*> spendingObj = obj->getValue("education_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		if (settingsObj.size() > 0)
		{
			educationSpending = atof(settingsObj[0]->getLeaf().c_str());
		}
	}
	spendingObj = obj->getValue("crime_fighting");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		if (settingsObj.size() > 0)
		{
			adminSpending = atof(settingsObj[0]->getLeaf().c_str());
		}
	}
	spendingObj = obj->getValue("social_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		if (settingsObj.size() > 0)
		{
			socialSpending = atof(settingsObj[0]->getLeaf().c_str());
		}
	}
	spendingObj = obj->getValue("military_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		if (settingsObj.size() > 0)
		{
			militarySpending = atof(settingsObj[0]->getLeaf().c_str());
		}
	}

	// Read reforms
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); ++i)
	{
		string key = leaves[i]->getKey();

		if (key == "slavery" || key == "vote_franchise" || key == "upper_house_composition" ||
		    key == "voting_system" || key == "public_meetings" || key == "press_rights" ||
			key == "trade_unions" || key == "political_parties" || key == "wage_reforms" ||
			key == "work_hours" || key == "safety_regulations" || key == "unemployment_subsidies" ||
			key == "pensions" || key == "health_care")
		{
			reforms[key] = leaves[i]->getLeaf();
		}
	}

	// Read international relations leaves
	for (unsigned int i = 0; i < leaves.size(); ++i)
	{
		string key = leaves[i]->getKey();

		if ((key.size() == 3) && 
			(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
			(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
			(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z'))
		{
			V2Relations rel;
			rel.init(leaves[i]);
			relations.push_back(rel);
		}
	}

	vector<Object*> armyObj = obj->getValue("army");
	for (std::vector<Object*>::iterator itr = armyObj.begin(); itr != armyObj.end(); ++itr)
	{
		V2Army army;
		army.init(*itr);
		armies.push_back(army);
	}
	vector<Object*> navyObj = obj->getValue("navy");
	for (std::vector<Object*>::iterator itr = navyObj.begin(); itr != navyObj.end(); ++itr)
	{
		V2Army navy;
		navy.init(*itr);
		armies.push_back(navy);
	}
}


string V2Country::getTag() const
{
	return tag;
}


void V2Country::addProvince(V2Province* province)
{
	provinces.push_back(province);
}


void V2Country::clearProvinces()
{
	provinces.clear();
}


void V2Country::addCore(V2Province* core)
{
	cores.push_back(core);
}


void V2Country::clearCores()
{
	cores.clear();
}


vector<V2Province*>	V2Country::getProvinces()
{
	return provinces;
}


vector<V2Province*>	V2Country::getCores()
{
	return cores;
}


int V2Country::getCapital()
{
	return capital;
}


void V2Country::eatCountry(V2Country* target)
{
	// autocannibalism is forbidden
	if (target->getTag() == tag)
		return;

	// for calculation of weighted averages
	int totalProvinces = target->provinces.size() + provinces.size();
	if (totalProvinces == 0)
		totalProvinces = 1;
	double myWeight = (double)provinces.size() / (double)totalProvinces;
	double targetWeight = (double)target->provinces.size() / (double)totalProvinces;

	// acquire target's cores (always)
	for (unsigned int j = 0; j < target->cores.size(); j++)
	{
		addCore(target->cores[j]);
		target->cores[j]->addCore(tag);
		target->cores[j]->removeCore(target->tag);
	}

	// everything else, do only if this country actually currently exists
	if (target->provinces.size() > 0)
	{
		// acquire target's provinces
		for (unsigned int j = 0; j < target->provinces.size(); j++)
		{
			addProvince(target->provinces[j]);
			target->provinces[j]->setOwner(tag);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());
		//XXX: leaders.insert(leaders.end(), target->leaders.begin(), target->leaders.end());

		// acquire the target's treasury and reserves, as well as their liabilities
		money += target->money;
		bankReserves += target->bankReserves;
		//XXX: loans.insert(loans.end(), target->loans.begin(), target->loans.end());

		// rebalance prestige, badboy from weighted average
		prestige		= myWeight * prestige		+ targetWeight * target->prestige;
		badboy			= myWeight * badboy			+ targetWeight * target->badboy;

		// give merged nation any techs owned by either nation
		vector<string> ttechs = target->getTechs();
		for (vector<string>::iterator tech = ttechs.begin(); tech != ttechs.end(); ++tech)
		{
			vector<string>::iterator stech = std::find(techs.begin(), techs.end(), *tech);
			if (stech == techs.end())
				techs.push_back(*tech);
		}

		// and do the same with inventions
		inventionStatus* tinvs = target->getInventionStatusArray();
		for (int i = 0; i < naval_exercises; ++i)
		{
			inventions[i] = std::max(inventions[i], tinvs[i]);
		}
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();

	log("Merged %s into %s\n", target->tag.c_str(), tag.c_str());
}


double V2Country::getUpperHousePercentage(string ideology)
{
	map<string, double>::iterator itr = upperHouseComposition.find(ideology);
	if (itr == upperHouseComposition.end())
		return 0.0;

	return itr->second;
}


string V2Country::getReform(string reform)
{
	map<string,string>::iterator itr = reforms.find(reform);
	if (itr == reforms.end())
		return "";
	
	return itr->second;
}