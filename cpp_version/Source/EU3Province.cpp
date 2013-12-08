#include "EU3Province.h"
#include "EU3Country.h"
#include "Log.h"
#include "Parsers/Object.h"
#include <algorithm>



EU3Province::EU3Province(Object* obj) {
	num = atoi(obj->getKey().c_str());

	vector<Object*> ownerObjs;
	ownerObjs = obj->getValue("owner");
	if (ownerObjs.size() == 0)
	{
		ownerString = "";
	}
	else
	{
		ownerString = ownerObjs[0]->getLeaf();
	}
	owner = NULL;

	cores.clear();
	vector<Object*> coreObjs;
	coreObjs = obj->getValue("core");
	for (unsigned int i = 0; i < coreObjs.size(); i++)
	{
		cores.push_back(coreObjs[i]->getLeaf());
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

	centerOfTrade = false;

	ownershipHistory.clear();
	lastPossessedDate.clear();
	religionHistory.clear();
	cultureHistory.clear();
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
				thisCountry = historyObjs[i]->getLeaf();
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
				thisCountry = ownerObj[0]->getLeaf();

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

	if (cultureHistory.size() == 0)
	{
		vector<Object*> culObj = obj->getValue("culture");
		if (culObj.size() > 0)
		{
			date newDate;
			cultureHistory.push_back(make_pair(newDate, culObj[0]->getLeaf()));
		}
	}
	if (religionHistory.size() == 0)
	{
		vector<Object*> religObj = obj->getValue("religion");
		if (religObj.size() > 0)
		{
			date newDate;
			religionHistory.push_back(make_pair(newDate, religObj[0]->getLeaf()));
		}
	}

	popRatios.clear();

	buildings.clear();
	checkBuilding(obj, "weapons");
	checkBuilding(obj, "university");
	checkBuilding(obj, "wharf");
	checkBuilding(obj, "textile");
	checkBuilding(obj, "fine_arts_academy");
	checkBuilding(obj, "refinery");

	checkBuilding(obj, "fort1");
	checkBuilding(obj, "fort2");
	checkBuilding(obj, "fort3");
	checkBuilding(obj, "fort4");
	checkBuilding(obj, "fort5");
	checkBuilding(obj, "fort6");

	checkBuilding(obj, "dock");
	checkBuilding(obj, "drydock");
	checkBuilding(obj, "shipyard");
	checkBuilding(obj, "grand_shipyard");
	checkBuilding(obj, "naval_arsenal");
	checkBuilding(obj, "naval_base");

	checkBuilding(obj, "temple");
	checkBuilding(obj, "courthouse");
	checkBuilding(obj, "spy_agency");
	checkBuilding(obj, "town_hall");
	checkBuilding(obj, "college");
	checkBuilding(obj, "cathedral");

	checkBuilding(obj, "armory");
	checkBuilding(obj, "training_fields");
	checkBuilding(obj, "barracks");
	checkBuilding(obj, "regimental_camp");
	checkBuilding(obj, "arsenal");
	checkBuilding(obj, "conscription_center");

	checkBuilding(obj, "constable");
	checkBuilding(obj, "workshop");
	checkBuilding(obj, "counting_house");
	checkBuilding(obj, "treasury_office");
	checkBuilding(obj, "mint");
	checkBuilding(obj, "stock_exchange");

	checkBuilding(obj, "customs_house");

	buildPopRatios();
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


bool EU3Province::wasColonised() const
{
	// returns true if the first owner did not own the province at the beginning of the game,
	// but acquired it later through colonization
	if (ownershipHistory.size() > 0)
	{
		if ((ownershipHistory[0].first != date()) && (ownershipHistory[0].first != date("1399.10.14")))
		{
			return true;
		}
	}
	return false;
}


bool EU3Province::wasPaganConquest() const
{
	// returns true if the province was originally pagan, the current owner is non-pagan,
	// and the province was NOT colonized
	if (religionHistory.size() > 0 && !wasColonised())
	{
		// TODO: should probably read these from EU3 common/religion.txt
		string firstReligion = religionHistory[0].second;
		string ownerReligion = owner->getReligion();
		if (firstReligion == "animism" || firstReligion == "shamanism")
		{
			if (ownerReligion != "animism" && ownerReligion != "shamanism")
			{
				return true;
			}
		}
	}
	return false;
}


bool EU3Province::hasBuilding(string building) const
{
	int num = buildings.count(building);
	return (num > 0);
}


vector<EU3Country*> EU3Province::getCores(const map<string, EU3Country*>& countries) const
{
	vector<EU3Country*> coreOwners;
	for (vector<string>::const_iterator i = cores.begin(); i != cores.end(); i++)
	{
		map<string, EU3Country*>::const_iterator j = countries.find(*i);
		if (j != countries.end())
		{
			coreOwners.push_back(j->second);
		}
	}

	return coreOwners;
}


date EU3Province::getLastPossessedDate(string tag) const
{
	map<string, date>::const_iterator itr = lastPossessedDate.find(tag);
	if (itr != lastPossessedDate.end())
	{
		return itr->second;
	}
	return date();
}


void EU3Province::checkBuilding(const Object* provinceObj, string building)
{
	vector<Object*> buildingObj;
	buildingObj = provinceObj->getValue(building);
	if ((buildingObj.size() > 0) && (buildingObj[0]->getLeaf() == "yes"))
	{
		buildings[building] = true;
	}
}


void EU3Province::buildPopRatios()
{
	// fast-forward to 1620 (200 year decay means any changes before then will be at 100%)
	string curCulture		= "";
	string curReligion	= "";
	vector< pair<date, string> >::iterator cItr = cultureHistory.begin();
	vector< pair<date, string> >::iterator rItr = religionHistory.begin();
	while (cItr != cultureHistory.end() && cItr->first.year < 1620)
	{
		curCulture = cItr->second;
		++cItr;
	} 
	if (cItr != cultureHistory.end() && curCulture == "")
	{
		// no starting culture; use first settlement culture for starting pop even if it's after 1620
		curCulture = cItr->second;
	}
	while (rItr != religionHistory.end() && rItr->first.year < 1620)
	{
		curReligion = rItr->second;
		++rItr;
	}
	if (rItr != religionHistory.end() && curReligion == "")
	{
		// no starting religion; use first settlement religion for starting pop even if it's after 1620
		curReligion = rItr->second;
	}

	// build and scale historic culture-religion pairs
	EU3PopRatio pr;
	pr.culture	= curCulture;
	pr.religion	= curReligion;
	pr.popRatio	= 1.0;
	date cDate, rDate, lastLoopDate;
	while (cItr != cultureHistory.end() || rItr != religionHistory.end())
	{
		if (cItr == cultureHistory.end())
		{
			cDate = date("2000.1.1");
		}
		else
		{
			cDate = cItr->first;
		}
		if (rItr == religionHistory.end())
		{
			rDate = date("2000.1.1");
		}
		else
		{
			rDate = rItr->first;
		}
		if (cDate < rDate)
		{
			decayPopRatios(lastLoopDate, cDate, pr);
			popRatios.push_back(pr);
			for (vector<EU3PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
			{
				itr->popRatio /= 2.0;
			}
			pr.popRatio		= 0.5;
			pr.culture		= cItr->second;
			lastLoopDate	= cDate;
			++cItr;
		}
		else if (cDate == rDate)
		{
			// culture and religion change on the same day;
			decayPopRatios(lastLoopDate, cDate, pr);
			popRatios.push_back(pr);
			for (vector<EU3PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
			{
				itr->popRatio /= 2.0;
			}
			pr.popRatio		= 0.5;
			pr.culture		= cItr->second;
			pr.religion		= rItr->second;
			lastLoopDate	= cDate;
			++cItr;
			++rItr;
		}
		else if (rDate < cDate)
		{
			decayPopRatios(lastLoopDate, rDate, pr);
			popRatios.push_back(pr);
			for (vector<EU3PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
			{
				itr->popRatio /= 2.0;
			}
			pr.popRatio		= 0.5;
			pr.religion		= rItr->second;
			lastLoopDate	= rDate;
			++rItr;
		}
	}
	decayPopRatios(lastLoopDate, date("1821.1.1"), pr);
	popRatios.push_back(pr);
}


void EU3Province::decayPopRatios(date oldDate, date newDate, EU3PopRatio& currentPop)
{
	// quick out for initial state (no decay needed)
	if (oldDate == date())
	{
		return;
	}

	// quick out for same year (we do decay at year end)
	if (oldDate.year == newDate.year)
	{
		return;
	}

	// drop all non-current pops by a total of .0025 per year, divided proportionally
	double nonCurrentRatio = (1.0 - currentPop.popRatio);
	for (vector<EU3PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
	{
		itr->popRatio -= .0025 * (newDate.year - oldDate.year) * itr->popRatio / nonCurrentRatio ;
	}

	// increase current pop by .0025 per year
	currentPop.popRatio += .0025 * (newDate.year - oldDate.year);
}