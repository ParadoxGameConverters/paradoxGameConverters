#include "EU3Province.h"
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
		culture == "";
	}
	else
	{
		culture = cultureObj[0]->getLeaf();
	}

	vector<Object*> religionObj;
	religionObj = obj->getValue("religion");
	if (religionObj.size() == 0)
	{
		religion == "";
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
		for (unsigned int i = 0; i < historyObjs.size(); i++)
		{
			if (historyObjs[i]->getKey() == "owner")
			{
				lastOwner = historyObjs[i]->getLeaf().substr(1,3);
				continue;
			}

			vector<Object*> ownerObj = historyObjs[i]->getValue("owner");
			if (ownerObj.size() > 0)
			{
				date newDate;
				string dateString = historyObjs[i]->getKey();
				newDate.year		= atoi( dateString.substr(0, 4).c_str() );
				newDate.month		= atoi( dateString.substr(5, dateString.find_last_of('.') - 5).c_str() );
				newDate.day			= atoi( dateString.substr(dateString.find_last_of('.') + 1, 2).c_str() );

				unsigned int j;
				for (j = 0; j < lastPossessedDate.size(); j++)
				{
					if (lastPossessedDate[j].first == lastOwner)
					{
						lastPossessedDate[j] = make_pair(lastOwner, newDate);
						break;
					}
				}
				if (j == lastPossessedDate.size())
				{
					lastPossessedDate.push_back( make_pair(lastOwner, newDate) );
				}
				lastOwner			= ownerObj[0]->getLeaf().substr(1, 3);
			}
		}
	}
}


int EU3Province::getNum()
{
	return num;
}


string EU3Province::getOwner()
{
	return owner;
}


vector<string> EU3Province::getCores()
{
	return cores;
}


string EU3Province::getCulture()
{
	return culture;
}


string EU3Province::getReligion()
{
	return religion;
}


bool EU3Province::isColony()
{
	return colony;
}


date EU3Province::getLastPossessedDate(string Tag)
{
	date newDate;
	for (unsigned int i = 0; i < lastPossessedDate.size(); i++)
	{
		if (lastPossessedDate[i].first == Tag)
		{
			newDate.year	= lastPossessedDate[i].second.year;
			newDate.month	= lastPossessedDate[i].second.month;
			newDate.day		= lastPossessedDate[i].second.day;
		}
	}
	return newDate;
}