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

		// acquire the target's treasury and income, as well as their liabilities
		//XXX: treasury += target->treasury;
		//XXX: estMonthlyIncome += target->estMonthlyIncome;
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