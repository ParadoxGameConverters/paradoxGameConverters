/*Copyright(c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


#include "EU4Province.h"
#include "EU4Country.h"
#include "EU4Religion.h"
#include "../Log.h"
#include "../Parsers/Object.h"
#include "../Configuration.h"
#include <algorithm>



EU4Province::EU4Province(Object* obj) {
	num = 0 - atoi(obj->getKey().c_str());

	vector<Object*> ownerObjs;				// the object holding the owner
	ownerObjs = obj->getValue("owner");
	(ownerObjs.size() == 0) ? ownerString = "" : ownerString = ownerObjs[0]->getLeaf();
	owner = NULL;

	cores.clear();
	vector<Object*> coreObjs;				// the object holding the cores
	coreObjs = obj->getValue("core");
	for (unsigned int i = 0; i < coreObjs.size(); i++)
	{
		cores.push_back(coreObjs[i]->getLeaf());
	}

	colony = false;

	ownershipHistory.clear();
	lastPossessedDate.clear();
	religionHistory.clear();
	cultureHistory.clear();
	vector<Object*> historyObj = obj->getValue("history");				// the objects holding the history of this province
	if (historyObj.size() > 0)
	{
		vector<Object*> historyObjs = historyObj[0]->getLeaves();		// the object holding the current history point
		string lastOwner;				// the last owner of the province
		string thisCountry;			// the current owner of the province
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

			vector<Object*> ownerObj = historyObjs[i]->getValue("owner");	// the object holding the current historical owner change
			if (ownerObj.size() > 0)
			{
				const date newDate(historyObjs[i]->getKey());	// the date this happened
				thisCountry = ownerObj[0]->getLeaf();

				map<string, date>::iterator itr = lastPossessedDate.find(lastOwner);
				if (itr != lastPossessedDate.end())
					itr->second = newDate;
				else
					lastPossessedDate.insert(make_pair(lastOwner, newDate));
				lastOwner = thisCountry;

				ownershipHistory.push_back(make_pair(newDate, thisCountry));
			}
			vector<Object*> culObj = historyObjs[i]->getValue("culture");	// the object holding the current historical culture change
			if (culObj.size() > 0)
			{
				const date newDate(historyObjs[i]->getKey());	// the date this happened
				cultureHistory.push_back(make_pair(newDate, culObj[0]->getLeaf()));
			}
			vector<Object*> religObj = historyObjs[i]->getValue("religion");	// the object holding the current historical religion change
			if (religObj.size() > 0)
			{
				const date newDate(historyObjs[i]->getKey());	// the date this happened
				religionHistory.push_back(make_pair(newDate, religObj[0]->getLeaf()));
			}
		}
	}
	sort(ownershipHistory.begin(), ownershipHistory.end());
	sort(cultureHistory.begin(), cultureHistory.end());
	sort(religionHistory.begin(), religionHistory.end());

	if (num == 1)
	{
		Configuration::setFirstEU4Date(ownershipHistory[0].first);
	}

	if (cultureHistory.size() == 0)
	{
		vector<Object*> culObj = obj->getValue("culture");	// the object holding the current culture
		if (culObj.size() > 0)
		{
			const date newDate;	// the default date
			cultureHistory.push_back(make_pair(newDate, culObj[0]->getLeaf()));
		}
	}
	if (religionHistory.size() == 0)
	{
		vector<Object*> religObj = obj->getValue("religion");	// the object holding the current religion
		if (religObj.size() > 0)
		{
			const date newDate;	// the default date
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


void EU4Province::addCore(string tag)
{
	cores.push_back(tag);
}


void EU4Province::removeCore(string tag)
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


bool EU4Province::wasColonised() const
{
	// returns true if the first owner did not own the province at the beginning of the game,
	// but acquired it later through colonization
	if (ownershipHistory.size() > 0)
	{
		if ((ownershipHistory[0].first != date()) && (ownershipHistory[0].first != Configuration::getFirstEU4Date()))
		{
			return true;
		}
	}
	return false;
}


bool EU4Province::wasInfidelConquest() const
{
	// returns true if the province was originally pagan, the current owner is non-pagan,
	// and the province was NOT colonized
	if (religionHistory.size() > 0 && !wasColonised())
	{
		EU4Religion* firstReligion = EU4Religion::getReligion(religionHistory[0].second);	// the first religion of this province
		EU4Religion* ownerReligion = EU4Religion::getReligion(owner->getReligion());			// the owner's religion
		if ((firstReligion == NULL) || (ownerReligion == NULL))
		{
			LOG(LogLevel::Warning) << "Unhandled religion in EU4 province " << num;
			return true;
		}
		else
		{
			return firstReligion->isInfidelTo(ownerReligion);
		}
	}
	return false;
}


bool EU4Province::hasBuilding(string building) const
{
	const int num = buildings.count(building);	// the number of this building
	return (num > 0);
}


vector<EU4Country*> EU4Province::getCores(const map<string, EU4Country*>& countries) const
{
	vector<EU4Country*> coreOwners;	// the core holders
	for (vector<string>::const_iterator i = cores.begin(); i != cores.end(); i++)
	{
		map<string, EU4Country*>::const_iterator j = countries.find(*i);
		if (j != countries.end())
		{
			coreOwners.push_back(j->second);
		}
	}

	return coreOwners;
}


date EU4Province::getLastPossessedDate(string tag) const
{
	map<string, date>::const_iterator itr = lastPossessedDate.find(tag);	// the last date the country possessed this province
	if (itr != lastPossessedDate.end())
	{
		return itr->second;
	}
	return date();
}


void EU4Province::checkBuilding(const Object* provinceObj, string building)
{
	vector<Object*> buildingObj;	// the object holding the building
	buildingObj = provinceObj->getValue(building);
	if ((buildingObj.size() > 0) && (buildingObj[0]->getLeaf() == "yes"))
	{
		buildings[building] = true;
	}
}


void EU4Province::buildPopRatios()
{
	// fast-forward to 1620 (200 year decay means any changes before then will be at 100%)
	string curCulture		= "";	// the current culture
	string curReligion	= "";	// the current religion
	vector< pair<date, string> >::iterator cItr = cultureHistory.begin();	// the culture under consideration
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
	vector< pair<date, string> >::iterator rItr = religionHistory.begin();	// the religion under consideration
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
	EU4PopRatio pr;	// a pop ratio
	pr.culture	= curCulture;
	pr.religion	= curReligion;
	pr.popRatio	= 1.0;
	date cDate, rDate, lastLoopDate;	// the dates this culture dominated, this religion dominated, and the former relevant date
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
			for (vector<EU4PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
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
			for (vector<EU4PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
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
			for (vector<EU4PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
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


void EU4Province::decayPopRatios(date oldDate, date newDate, EU4PopRatio& currentPop)
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
	const double nonCurrentRatio = (1.0 - currentPop.popRatio);
	for (vector<EU4PopRatio>::iterator itr = popRatios.begin(); itr != popRatios.end(); ++itr)
	{
		itr->popRatio -= .0025 * (newDate.year - oldDate.year) * itr->popRatio / nonCurrentRatio ;
	}

	// increase current pop by .0025 per year
	currentPop.popRatio += .0025 * (newDate.year - oldDate.year);
}