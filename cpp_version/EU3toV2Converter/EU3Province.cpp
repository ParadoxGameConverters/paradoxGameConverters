#include "EU3Province.h"
#include <algorithm>
#include "Log.h"


void EU3Province::init(Object* obj) {
	num = atoi(obj->getKey().c_str());

	vector<Object*> ownerObjs;
	ownerObjs = obj->getValue("owner");
	if (ownerObjs.size() == 0)
	{
		owner = "";
	}
	else
	{
		owner = ownerObjs[0]->getLeaf();
		owner = owner.substr(1,3);
	}

	vector<Object*> coreObjs;
	coreObjs = obj->getValue("core");
	for (unsigned int i = 0; i < coreObjs.size(); i++)
	{
		cores.push_back(coreObjs[i]->getLeaf().substr(1,3));
	}

	vector<Object*> cultureObj;
	cultureObj = obj->getValue("culture");
	if (cultureObj.size() == 0)
	{
		culture = "";
	}
	else
	{
		culture = cultureObj[0]->getLeaf();
	}

	vector<Object*> religionObj;
	religionObj = obj->getValue("religion");
	if (religionObj.size() == 0)
	{
		religion = "";
	}
	else
	{
		religion = religionObj[0]->getLeaf();
	}


	colony = true;
	vector<Object*> popObj = obj->getValue("citysize");
	if (popObj.size() > 0)
	{
		population	= atoi( (popObj)[0]->getLeaf().c_str() );
		if (population >= 1000)
		{
			colony = false;
		}
	}
	else
	{
		popObj		= obj->getValue("native_size");
		if (popObj.size() > 0)
		{
			population	= atoi( (popObj)[0]->getLeaf().c_str() );
		}
		else
		{
			population = 0;
		}
	}

	vector<Object*> historyObj = obj->getValue("history");
	if (historyObj.size() > 0)
	{
		vector<Object*> historyObjs = historyObj[0]->getLeaves();
		string lastOwner;
		string thisCountry;
		for (unsigned int i = 0; i < historyObjs.size(); i++)
		{
			if (historyObjs[i]->getKey() == "owner")
			{
				thisCountry = historyObjs[i]->getLeaf().substr(1,3);
				lastOwner = thisCountry;
				ownershipHistory.push_back(make_pair(date(), thisCountry));
				continue;
			}
			else if (historyObjs[i]->getKey() == "culture")
			{
				cultureHistory.push_back(make_pair(date(), historyObjs[i]->getLeaf()));
				continue;
			}
			else if (historyObjs[i]->getKey() == "religion")
			{
				religionHistory.push_back(make_pair(date(), historyObjs[i]->getLeaf()));
				continue;
			}

			vector<Object*> ownerObj = historyObjs[i]->getValue("owner");
			if (ownerObj.size() > 0)
			{
				date newDate(historyObjs[i]->getKey());
				thisCountry = ownerObj[0]->getLeaf().substr(1, 3);

				map<string, date>::iterator itr = lastPossessedDate.find(lastOwner);
				if (itr != lastPossessedDate.end())
					itr->second = newDate;
				else
					lastPossessedDate.insert(make_pair(lastOwner, newDate));
				lastOwner = thisCountry;

				ownershipHistory.push_back(make_pair(newDate, thisCountry));
			}
			vector<Object*> culObj = historyObjs[i]->getValue("culture");
			if (culObj.size() > 0)
			{
				date newDate(historyObjs[i]->getKey());
				cultureHistory.push_back(make_pair(newDate, culObj[0]->getLeaf()));
			}
			vector<Object*> religObj = historyObjs[i]->getValue("religion");
			if (religObj.size() > 0)
			{
				date newDate(historyObjs[i]->getKey());
				religionHistory.push_back(make_pair(newDate, religObj[0]->getLeaf()));
			}
		}
	}

	sort(ownershipHistory.begin(), ownershipHistory.end());
	sort(cultureHistory.begin(), cultureHistory.end());
	sort(religionHistory.begin(), religionHistory.end());
}


int EU3Province::getNum()
{
	return num;
}


void EU3Province::setOwner(string newOwner)
{
	owner = newOwner;
}


string EU3Province::getOwner()
{
	return owner;
}


vector<string> EU3Province::getCores()
{
	return cores;
}


void EU3Province::addCore(string tag)
{
	cores.push_back(tag);
}


void EU3Province::removeCore(string tag)
{
	for (vector<string>::iterator i = cores.begin(); i != cores.end(); i++)
	{
		if (*i == tag)
		{
			cores.erase(i);
			if (cores.size() == 0)
			{
				break;
			}
			i = cores.begin();
		}
	}
}


string EU3Province::getCulture()
{
	return culture;
}


string EU3Province::getReligion()
{
	return religion;
}


int EU3Province::getPopulation()
{
	return population;
}


bool EU3Province::isColony()
{
	return colony;
}


bool EU3Province::wasColonised()
{
	// returns true if the first owner did not own the province at the beginning of the game,
	// but acquired it later through colonization
	if (ownershipHistory.size() > 0)
	{
		if (ownershipHistory[0].first != date())
		{
			return true;
		}
	}
	return false;
}


date EU3Province::getLastPossessedDate(string Tag)
{
	map<string, date>::iterator itr = lastPossessedDate.find(Tag);
	if (itr != lastPossessedDate.end())
		return itr->second;
	return date();
}