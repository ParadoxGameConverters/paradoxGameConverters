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
#include "Log.h"
#include "Object.h"
#include "../Configuration.h"
#include <algorithm>
#include <fstream>



EU4Province::EU4Province(Object* obj)
{
	provTaxIncome = 0;
	provProdIncome = 0;
	provMPWeight = 0;
	provBuildingWeight = 0;
	provTradeGoodWeight = 0;
	provDevModifier = 0;


	numV2Provs = 0;

	num = 0 - atoi(obj->getKey().c_str());

	vector<Object*> baseTaxObjs;			// the object holding the base tax
	baseTaxObjs = obj->getValue("base_tax");
	baseTax = (baseTaxObjs.size() > 0) ? atof(baseTaxObjs[0]->getLeaf().c_str()) : 0.0f;

	vector<Object*> baseProdObjs;			// the object holding the base production
	baseProdObjs = obj->getValue("base_production");
	baseProd = (baseProdObjs.size() > 0) ? atof(baseProdObjs[0]->getLeaf().c_str()) : 0.0f;

	vector<Object*> baseManpowerObjs;		// the object holding the base manpower
	baseManpowerObjs = obj->getValue("base_manpower");
	manpower = (baseManpowerObjs.size() > 0) ? atof(baseManpowerObjs[0]->getLeaf().c_str()) : 0.0f;

	// for old versions of EU4 (< 1.12), copy tax to production if necessary
	if (baseProd == 0.0f && baseTax > 0.0f)
	{
		baseProd = baseTax;
	}

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

	vector<Object*> hreObj = obj->getValue("hre");
	if ((hreObj.size() > 0) && (hreObj[0]->getLeaf() == "yes"))
	{
		inHRE = true;
	}
	else
	{
		inHRE = false;
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

	vector<Object*> tradegoodsObj = obj->getValue("trade_goods");
	if (tradegoodsObj.size() > 0) 
	{
		tradeGoods = tradegoodsObj[0]->getLeaf();
	}
	else
	{
		tradeGoods = "";
	}

	vector<Object*> provNameObj = obj->getValue("name");
	if (provNameObj.size() > 0)
	{
		provName = provNameObj[0]->getLeaf();
	}
	else
	{
		provName = "";
	}

	// if we didn't have base manpower (EU4 < 1.12), check for manpower instead
	if (manpower == 0.0f)
	{
		vector<Object*> manpowerObj = obj->getValue("manpower");
		if (manpowerObj.size() > 0)
		{
			string manpowerStr = manpowerObj[0]->getLeaf();
			manpower = stod(manpowerStr);
		}
	}

	// great projects
	vector<Object*> projectsObj = obj->getValue("great_projects");
	if (projectsObj.size() > 0)
	{
		for (const auto& proj : projectsObj[0]->getTokens())
		{
			buildings[proj] = true;
		}
	}

	//LOG(LogLevel::Info) << "Check unique Buildings...";
	// unique buildings
	checkBuilding(obj, "march");
	checkBuilding(obj, "glorious_monument");
	checkBuilding(obj, "royal_palace");
	checkBuilding(obj, "admiralty");
	checkBuilding(obj, "war_college");
	checkBuilding(obj, "embassy");
	checkBuilding(obj, "tax_assessor");
	checkBuilding(obj, "grain_depot");
	checkBuilding(obj, "university");
	checkBuilding(obj, "fine_arts_academy");

	//LOG(LogLevel::Info) << "Check manus...";
	// Manufacturies
	checkBuilding(obj, "weapons");
	checkBuilding(obj, "wharf");
	checkBuilding(obj, "textile");
	checkBuilding(obj, "refinery");
	checkBuilding(obj, "plantations");
	checkBuilding(obj, "farm_estate");
	checkBuilding(obj, "tradecompany");

	//LOG(LogLevel::Info) << "Check buildings...";
	// base buildings 
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
	checkBuilding(obj, "marketplace");
	checkBuilding(obj, "trade_depot");
	checkBuilding(obj, "canal");
	checkBuilding(obj, "road_network");
	checkBuilding(obj, "post_office");

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
	// but acquired it later through colonization, and if the current culture does not match the original culture
	if (ownershipHistory.size() > 0)
	{
		if ((ownershipHistory[0].first != date()) && (ownershipHistory[0].first != Configuration::getFirstEU4Date()))
		{
			if	((cultureHistory.size() > 1) && (cultureHistory[0].second != cultureHistory[cultureHistory.size() - 1].second))
			{
				return true;
			}
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
			if	((cultureHistory.size() > 1) && (cultureHistory[0].second != cultureHistory[cultureHistory.size() - 1].second))
			{
				return firstReligion->isInfidelTo(ownerReligion);
			}
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
	date endDate = Configuration::getLastEU4Date();
	if (endDate < date("1821.1.1"))
	{
		endDate = date("1821.1.1");
	}
	date cutoffDate	 = endDate;
	cutoffDate.year	-= 200;

	// fast-forward to 200 years before the end date (200 year decay means any changes before then will be at 100%)
	string curCulture		= "";	// the current culture
	string curReligion	= "";	// the current religion
	vector< pair<date, string> >::iterator cItr = cultureHistory.begin();	// the culture under consideration
	while (cItr != cultureHistory.end() && cItr->first.year < cutoffDate.year)
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
	while (rItr != religionHistory.end() && rItr->first.year < cutoffDate.year)
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
	EU4PopRatio pr;		// a pop ratio
	pr.culture			= curCulture;
	pr.religion			= curReligion;
	pr.upperPopRatio	= 1.0;
	pr.middlePopRatio	= 1.0;
	pr.lowerPopRatio	= 1.0;
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
			for (auto itr: popRatios)
			{
				itr.upperPopRatio		/= 2.0;
				itr.middlePopRatio	/= 2.0;
			}
			pr.upperPopRatio	= 0.5;
			pr.middlePopRatio	= 0.5;
			pr.lowerPopRatio	= 0.0;
			pr.culture			= cItr->second;
			lastLoopDate		= cDate;
			++cItr;
		}
		else if (cDate == rDate)
		{
			// culture and religion change on the same day;
			decayPopRatios(lastLoopDate, cDate, pr);
			popRatios.push_back(pr);
			for (auto itr: popRatios)
			{
				itr.upperPopRatio		/= 2.0;
				itr.middlePopRatio	/= 2.0;
			}
			pr.upperPopRatio	= 0.5;
			pr.middlePopRatio	= 0.5;
			pr.lowerPopRatio	= 0.0;
			pr.culture			= cItr->second;
			pr.religion			= rItr->second;
			lastLoopDate		= cDate;
			++cItr;
			++rItr;
		}
		else if (rDate < cDate)
		{
			decayPopRatios(lastLoopDate, cDate, pr);
			popRatios.push_back(pr);
			for (auto itr: popRatios)
			{
				itr.upperPopRatio		/= 2.0;
				itr.middlePopRatio	/= 2.0;
			}
			pr.upperPopRatio	= 0.5;
			pr.middlePopRatio	= 0.5;
			pr.lowerPopRatio	= 0.0;
			pr.religion			= rItr->second;
			lastLoopDate		= rDate;
			++rItr;
		}
	}
	decayPopRatios(lastLoopDate, endDate, pr);

	if ((pr.culture != "") || (pr.religion != ""))
	{
		popRatios.push_back(pr);
	}
}


void	EU4Province::decayPopRatios(date oldDate, date newDate, EU4PopRatio& currentPop)
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
	double upperNonCurrentRatio	= (1.0 - currentPop.upperPopRatio);
	double middleNonCurrentRatio	= (1.0 - currentPop.middlePopRatio);
	double lowerNonCurrentRatio	= (1.0 - currentPop.lowerPopRatio);
	for (auto itr: popRatios)
	{
		itr.upperPopRatio		-= .0025 * (newDate.year - oldDate.year) * itr.upperPopRatio	/ upperNonCurrentRatio;
		itr.middlePopRatio	-= .0025 * (newDate.year - oldDate.year) * itr.middlePopRatio	/ middleNonCurrentRatio;
		itr.lowerPopRatio		-= .0025 * (newDate.year - oldDate.year) * itr.lowerPopRatio	/ lowerNonCurrentRatio;
	}
	
	// increase current pop by .0025 per year
	currentPop.upperPopRatio	+= .0025 * (newDate.year - oldDate.year);
	currentPop.middlePopRatio	+= .0025 * (newDate.year - oldDate.year);
	currentPop.lowerPopRatio	+= .0025 * (newDate.year - oldDate.year);
}


void EU4Province::determineProvinceWeight()
{
	double trade_goods_weight			= getTradeGoodWeight();
	double manpower_weight				= manpower;
	double building_weight				= 0.0;
	double manpower_modifier			= 0.0;
	double manu_gp_mod					= 0.0;
	double building_tx_eff				= 0.0;
	double production_eff				= 0.0;
	double building_tx_income			= 0.0;
	double manpower_eff					= 0.0;
	double goods_produced_perc_mod	= 0.0;
	double trade_power					= 0.0;
	double trade_value					= 0.0;
	double trade_value_eff				= 0.0;
	double trade_power_eff				= 0.0;
	double dev_modifier				= 0.0;
	
	std::vector<double> provBuildingWeightVec = getProvBuildingWeight();

	// 0 building_weight, 1 manpower_modifier, 2 manu_gp_mod, 3 building_tx_eff, 4 production_eff
	// 5 building_tx_income, 6 manpower_eff, 7 goods_produced_perc_mod, 8 trade_power 9 trade_value
	// 10 trade_value_eff, 11 trade_power_eff;
	try {
		building_weight			= provBuildingWeightVec.at(0);
		manpower_modifier			= provBuildingWeightVec.at(1);
		manu_gp_mod					= provBuildingWeightVec.at(2);
		building_tx_eff			= provBuildingWeightVec.at(3);
		production_eff				= provBuildingWeightVec.at(4);
		building_tx_income		= provBuildingWeightVec.at(5);
		manpower_eff				= provBuildingWeightVec.at(6);
		goods_produced_perc_mod	= provBuildingWeightVec.at(7);
		trade_power					= provBuildingWeightVec.at(8);
		trade_value					= provBuildingWeightVec.at(9);
		trade_value_eff			= provBuildingWeightVec.at(10);
		trade_power_eff			= provBuildingWeightVec.at(11);
		dev_modifier			= provBuildingWeightVec.at(12);
	}
	catch (exception &e)
	{
		LOG(LogLevel::Error) << "Error in building weight vector: " << e.what();
	}

	// Check tag, ex. TIB has goods_produced +0.05
	// This needs to be hard coded unless there's some other way of figuring out modded national ambitions/ideas
	if (this->getOwnerString() == "TIB")
	{
		goods_produced_perc_mod += 0.05;
	}

	double goods_produced = (baseProd * 0.2) + manu_gp_mod + goods_produced_perc_mod + 0.03;

	// idea effects
	if ( (owner !=  NULL) && (owner->hasNationalIdea("bureaucracy")) )
	{
		building_tx_eff += 0.10;
	}
	if ( (owner !=  NULL) && (owner->hasNationalIdea("smithian_economics")) )
	{
		production_eff += 0.10;
	}

	// manpower
	manpower_weight *= 25;
	manpower_weight += manpower_modifier;
	manpower_weight *= ((1 + manpower_modifier) / 25); // should work now as intended

	//LOG(LogLevel::Info) << "Manpower Weight: " << manpower_weight;

	double total_tx = (baseTax + building_tx_income) * (1.0 + building_tx_eff + 0.15);
	double production_eff_tech = 0.5; // used to be 1.0

	double total_trade_value = ((getTradeGoodPrice() * goods_produced) + trade_value) * (1 + trade_value_eff);
	double production_income = total_trade_value * (1 + production_eff_tech + production_eff);
	//LOG(LogLevel::Info) << "province name: " << this->getProvName() 
	//	<< " trade good: " << tradeGoods 
	//	<< " Price: " << getTradeGoodPrice() 
	//	<< " trade value: " << trade_value 
	//	<< " trade value eff: " 
	//	<< (1 + trade_value_eff) 
	//	<< " goods produced: " << goods_produced 
	//	<< " production eff: " << production_eff 
	//	<< " Production: " << production_income;

	total_tx *= 1.5;
	manpower_weight *= 1;
	production_income *= 1.5;

	provBuildingWeight	= building_weight;
	provTaxIncome			= total_tx;
	provProdIncome			= production_income;
	provMPWeight			= manpower_weight;
	provTradeGoodWeight	= trade_goods_weight;
	provDevModifier	= dev_modifier;
	
	// dev modifier
	dev_modifier *= ( baseTax + baseProd + manpower );

	totalWeight = building_weight + dev_modifier + ( manpower_weight + production_income + total_tx );
	//i would change dev effect to 1, but your choice
	if (owner == NULL)
	{
		totalWeight = 0;
	}

	// 0: Goods produced; 1 trade goods price; 2: trade value efficiency; 3: production effiency; 4: trade value; 5: production income
	// 6: base tax; 7: building tax income 8: building tax eff; 9: total tax income; 10: total_trade_value
	provProductionVec.push_back(goods_produced);
	provProductionVec.push_back(getTradeGoodPrice());
	provProductionVec.push_back(1 + trade_value_eff);
	provProductionVec.push_back(1 + production_eff);
	provProductionVec.push_back(trade_value);
	provProductionVec.push_back(production_income);
	provProductionVec.push_back(baseTax);
	provProductionVec.push_back(building_tx_income);
	provProductionVec.push_back(1 + building_tx_eff);
	provProductionVec.push_back(total_tx);
	provProductionVec.push_back(total_trade_value);
	//LOG(LogLevel::Info) << "Num: " << num << " TAG: " << ownerString << " Weight: " << totalWeight;
}


double EU4Province::getTradeGoodPrice() const
{
	// Trade goods
	/*
	chinaware
	grain
	fish
	tabacco
	iron
	copper
	cloth
	ivory
	slaves
	salt
	wool
	fur
	gold
	sugar
	naval_supplies
	tea
	coffee
	spices
	wine
	cocoa
	silk
	dyes
	tropical_wood
	*/
	//LOG(LogLevel::Info) << "Trade Goods Price";
	double tradeGoodsPrice = 0;

	if (tradeGoods == "chinaware")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "grain")
	{
		tradeGoodsPrice = 2;
	}
	else if (tradeGoods == "fish")
	{
		tradeGoodsPrice = 2.5;
	}
	else if (tradeGoods == "tabacco")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "iron")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "copper")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "cloth")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "slaves")
	{
		tradeGoodsPrice = 2;
	}
	else if (tradeGoods == "salt")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "gold")
	{
		tradeGoodsPrice = 6;
	}
	else if (tradeGoods == "fur")
	{
		tradeGoodsPrice = 2;
	}
	else if (tradeGoods == "sugar")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "naval_supplies")
	{
		tradeGoodsPrice = 2;
	}
	else if (tradeGoods == "tea")
	{
		tradeGoodsPrice = 2;
	}
	else if (tradeGoods == "coffee")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "spices")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "wine")
	{
		tradeGoodsPrice = 2.5;
	}
	else if (tradeGoods == "cocoa")
	{
		tradeGoodsPrice = 4;
	}
	else if (tradeGoods == "ivory")
	{
		tradeGoodsPrice = 4;
	}
	else if (tradeGoods == "wool")
	{
		tradeGoodsPrice = 2.5;
	}
	else if (tradeGoods == "cotton")
	{
		tradeGoodsPrice = 3;
	}
	else if (tradeGoods == "dyes")
	{
		tradeGoodsPrice = 4;
	}
	else if (tradeGoods == "tropical_wood")
	{
		tradeGoodsPrice = 2;
	}
	else if (tradeGoods == "silk")
	{
		tradeGoodsPrice = 4;
	}
	else
	{
		// anything ive missed
		tradeGoodsPrice = 1;
	}

	return tradeGoodsPrice;
}


double EU4Province::getTradeGoodWeight() const
{
	// Trade goods
	/*
	chinaware
	grain
	fish
	tabacco
	iron
	copper
	cloth
	ivory
	slaves
	salt
	wool
	fur
	gold
	sugar
	naval_supplies
	tea
	coffee
	spices
	wine
	cocoa
	*/

	double trade_goods_weight = 0;
	if (tradeGoods == "chinaware")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "grain")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "fish")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "tabacco")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "iron")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "copper")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "cloth")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "slaves")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "salt")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "gold")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "fur")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "sugar")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "naval_supplies")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "tea")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "coffee")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "spices")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "wine")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "cocoa")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "ivory")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "wool")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "cotton")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "silk")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "tropical_wood")
	{
		trade_goods_weight = 2;
	}
	else if (tradeGoods == "dyes")
	{
		trade_goods_weight = 2;
	}
	else
	{
		// anything ive missed
		trade_goods_weight = 0;
	}

	return trade_goods_weight;
}


vector<double> EU4Province::getProvBuildingWeight() const
{
	double building_weight				= 0.0;
	double manpower_modifier			= 0.0;
	double manu_gp_mod					= 0.0;
	double building_tx_eff				= 0.0;
	double production_eff				= 0.0;
	double building_tx_income			= 0.0;
	double manpower_eff					= 0.0;
	double goods_produced_perc_mod	= 0.0;
	double trade_power					= 0.0;
	double trade_value					= 0.0;
	double trade_value_eff				= 0.0;
	double trade_power_eff				= 0.0;
	double dev_modifier				= 0.0;

	// unique buildings
	/*
	if (hasBuilding("march"))
	{
		building_weight += 2;
		manpower_modifier += 75;
	}

	if (hasBuilding("glorious_monument"))
	{
		building_weight += 2;
	}

	if (hasBuilding("royal_palace"))
	{
		building_weight += 2;
	}

	if (hasBuilding("admiralty"))
	{
		building_weight += 2;
	}

	if (hasBuilding("war_college"))
	{
		building_weight += 2;
	}

	if (hasBuilding("embassy"))
	{
		building_weight += 2;
	}

	if (hasBuilding("tax_assessor"))
	{
		building_weight += 2;
	}

	if (hasBuilding("grain_depot"))
	{
		building_weight += 2;
	}

	if (hasBuilding("university"))
	{
		building_weight += 2;
	}

	if (hasBuilding("fine_arts_academy"))
	{
		building_weight += 2;
	}

	// manfacturies building
	if (hasBuilding("weapons"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("wharf"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("textile"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("refinery"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("plantations"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("farm_estate"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("tradecompany"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	// Base buildings
	if (hasBuilding("fort1"))
	{
		building_weight += 1;
	}
	if (hasBuilding("fort2"))
	{
		building_weight += 2;
	}
	if (hasBuilding("fort3"))
	{
		building_weight += 3;
	}
	if (hasBuilding("fort4"))
	{
		building_weight += 4;

	}
	if (hasBuilding("fort5"))
	{
		building_weight += 5;
	}
	if (hasBuilding("fort6"))
	{
		building_weight += 6;
	}
	if (hasBuilding("dock"))
	{
		building_weight++;
	}

	if (hasBuilding("drydock"))
	{
		building_weight += 2;
	}

	if (hasBuilding("shipyard"))
	{
		building_weight += 3;
	}

	if (hasBuilding("grand_shipyard"))
	{
		building_weight += 4;
	}

	if (hasBuilding("naval_arsenal"))
	{
		building_weight += 5;
	}

	if (hasBuilding("naval_base"))
	{
		building_weight += 6;
	}

	if (hasBuilding("temple"))
	{
		building_weight += 1;
		building_tx_income += 1.0;
	}

	if (hasBuilding("courthouse"))
	{
		building_weight += 2;
		building_tx_eff += 0.10;
	}

	if (hasBuilding("spy_agency"))
	{
		building_weight += 3;
		building_tx_eff += 0.20;
	}

	if (hasBuilding("town_hall"))
	{
		building_weight += 4;
		building_tx_eff += 0.25;
	}

	if (hasBuilding("college"))
	{
		building_weight += 5;
		building_tx_eff += 0.50;
	}

	if (hasBuilding("cathedral"))
	{
		building_weight += 6;
		building_tx_income += 3.0;
	}

	if (hasBuilding("armory"))
	{
		building_weight += 1;
		manpower_modifier += 25;
	}

	if (hasBuilding("training_fields"))
	{
		building_weight += 2;
		manpower_modifier += 25;
	}

	if (hasBuilding("barracks"))
	{
		building_weight += 3;
		manpower_modifier += 25;
		manpower_eff += 0.10;
	}

	if (hasBuilding("regimental_camp"))
	{
		building_weight += 4;
		manpower_eff += 0.20;
	}

	if (hasBuilding("arsenal"))
	{
		building_weight += 5;
		manpower_modifier += 50;
	}

	if (hasBuilding("conscription_center"))
	{
		building_weight += 6;
		manpower_modifier += 50;
		manpower_eff += 0.50;
	}
	if (hasBuilding("constable"))
	{
		building_weight += 1;
		production_eff += 0.2;
	}

	if (hasBuilding("workshop"))
	{
		building_weight += 2;
		goods_produced_perc_mod += 0.2;
	}

	if (hasBuilding("counting_house"))
	{
		building_weight += 3;
	}

	if (hasBuilding("treasury_office"))
	{
		building_weight += 4;
	}

	if (hasBuilding("mint"))
	{
		building_weight += 5;
		production_eff += 0.5;
	}

	if (hasBuilding("stock_exchange"))
	{
		building_weight += 6;
		goods_produced_perc_mod += 0.50;
	}
	if (hasBuilding("customs_house"))
	{
		building_weight += 6;
		trade_power += 10;
		trade_value += 2;
	}

	if (hasBuilding("marketplace"))
	{
		building_weight++;
		trade_power += 2;
	}

	if (hasBuilding("trade_depot"))
	{
		building_weight += 2;
		trade_value += 1;
		trade_power_eff += 0.25;
	}
	if (hasBuilding("canal"))
	{
		building_weight += 3;
		trade_power += 2;
		trade_value_eff += 0.25;
	}
	if (hasBuilding("road_network"))
	{
		building_weight += 4;
		trade_power_eff += 0.25;
	}

	if (hasBuilding("post_office"))
	{
		building_weight += 5;
		trade_power += 3;
		trade_power_eff += 0.5;
	}*/

	/*if (hasBuilding("march"))
	{
		building_weight += 2;
		manpower_modifier += 75;
	}

	if (hasBuilding("glorious_monument"))
	{
		building_weight += 2;
		building_tx_income += 1;
		manpower_eff += 0.05;
	}

	if (hasBuilding("royal_palace"))
	{
		building_weight += 2;
	}

	if (hasBuilding("admiralty"))
	{
		building_weight += 2;
	}

	if (hasBuilding("war_college"))
	{
		building_weight += 2;
	}

	if (hasBuilding("embassy"))
	{
		building_weight += 2;
	}

	if (hasBuilding("tax_assessor"))
	{
		building_weight += 2;
	}

	if (hasBuilding("grain_depot"))
	{
		building_weight += 2;
	}

	if (hasBuilding("university"))
	{
		building_weight += 2;
	}

	if (hasBuilding("fine_arts_academy"))
	{
		building_weight += 2;
	}

	// manfacturies building
	if (hasBuilding("weapons"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("wharf"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("textile"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("refinery"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("plantations"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("farm_estate"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	if (hasBuilding("tradecompany"))
	{
		building_weight += 5;
		manu_gp_mod = 1.0;
	}

	// Base buildings
	if (hasBuilding("fort1"))
	{
		building_weight += 1;
	}
	if (hasBuilding("fort2"))
	{
		building_weight += 2;
	}
	if (hasBuilding("fort3"))
	{
		building_weight += 3;
	}
	if (hasBuilding("fort4"))
	{
		building_weight += 4;

	}
	if (hasBuilding("fort5"))
	{
		building_weight += 5;
	}
	if (hasBuilding("fort6"))
	{
		building_weight += 6;
	}
	if (hasBuilding("dock"))
	{
		building_weight++;
	}

	if (hasBuilding("drydock"))
	{
		building_weight += 2;
	}

	if (hasBuilding("shipyard"))
	{
		building_weight += 3;
	}

	if (hasBuilding("grand_shipyard"))
	{
		building_weight += 4;
	}

	if (hasBuilding("naval_arsenal"))
	{
		building_weight += 5;
	}

	if (hasBuilding("naval_base"))
	{
		building_weight += 6;
	}

	if (hasBuilding("temple"))
	{
		building_weight += 1;
		building_tx_income += 1.0;
	}

	if (hasBuilding("courthouse"))
	{
		building_weight += 2;
		building_tx_eff += 0.10;
		building_tx_income += 1.0;
	}

	if (hasBuilding("spy_agency"))
	{
		building_weight += 3;
		building_tx_eff += 0.30;
		building_tx_income += 1.0;
	}

	if (hasBuilding("town_hall"))
	{
		building_weight += 4;
		building_tx_eff += 0.55;
		building_tx_income += 1.0;
	}

	if (hasBuilding("college"))
	{
		building_weight += 5;
		building_tx_eff += 1.05;
		building_tx_income += 1.0;
	}

	if (hasBuilding("cathedral"))
	{
		building_weight += 6;
		building_tx_eff += 1.05;
		building_tx_income += 4.0;
	}

	if (hasBuilding("armory"))
	{
		building_weight += 1;
		manpower_modifier += 25;
	}

	if (hasBuilding("training_fields"))
	{
		building_weight += 2;
		manpower_modifier += 50;
	}

	if (hasBuilding("barracks"))
	{
		building_weight += 3;
		manpower_modifier += 75;
		manpower_eff += 0.10;
	}

	if (hasBuilding("regimental_camp"))
	{
		building_weight += 4;
		manpower_eff += 0.30;
		manpower_modifier += 75;
	}

	if (hasBuilding("arsenal"))
	{
		building_weight += 5;
		manpower_eff += 0.30;
		manpower_modifier += 125;
	}

	if (hasBuilding("conscription_center"))
	{
		building_weight += 6;
		manpower_modifier += 175;
		manpower_eff += 0.80;
	}
	if (hasBuilding("constable"))
	{
		building_weight += 1;
		production_eff += 0.2;
	}

	if (hasBuilding("workshop"))
	{
		building_weight += 2;
		goods_produced_perc_mod += 0.2;
		production_eff += 0.2;
	}

	if (hasBuilding("counting_house"))
	{
		goods_produced_perc_mod += 0.2;
		production_eff += 0.2;
		building_weight += 3;
	}

	if (hasBuilding("treasury_office"))
	{
		building_weight += 4;
		goods_produced_perc_mod += 0.2;
		production_eff += 0.2;
	}

	if (hasBuilding("mint"))
	{
		building_weight += 5;
		goods_produced_perc_mod += 0.2;
		production_eff += 0.7;
	}

	if (hasBuilding("stock_exchange"))
	{
		building_weight += 6;
		production_eff += 0.7;
		goods_produced_perc_mod += 0.70;
	}
	if (hasBuilding("customs_house"))
	{
		building_weight += 6;
		trade_value_eff += 0.25;
		trade_value += 3;
		trade_power_eff += 1;
		trade_power += 17;
	}

	if (hasBuilding("marketplace"))
	{
		building_weight++;
		trade_power += 2;
	}

	if (hasBuilding("trade_depot"))
	{
		building_weight += 2;
		trade_value += 1;
		trade_power_eff += 0.25;
		trade_power += 2;
	}
	if (hasBuilding("canal"))
	{
		building_weight += 3;
		trade_value_eff += 0.25;
		trade_value += 1;
		trade_power_eff += 0.25;
		trade_power += 4;
	}
	if (hasBuilding("road_network"))
	{
		building_weight += 4;
		trade_value_eff += 0.25;
		trade_value += 1;
		trade_power_eff += 0.5;
		trade_power += 4;
	}

	if (hasBuilding("post_office"))
	{
		building_weight += 5;
		trade_value_eff += 0.25;
		trade_value += 1;
		trade_power_eff += 1;
		trade_power += 7;
	}*/

	
if (hasBuilding("university"))
    {
        building_weight += 6;
    }

    // manfacturies building
    if (hasBuilding("weapons"))
    {
        manu_gp_mod = 1.0;
    }

    if (hasBuilding("wharf"))
    {
        manu_gp_mod = 1.0;
    }

    if (hasBuilding("textile"))
    {
        manu_gp_mod = 1.0;
    }

    if (hasBuilding("refinery"))
    {
        manu_gp_mod = 1.0;
    }

    if (hasBuilding("plantations"))
    {
        manu_gp_mod = 1.0;
    }

    if (hasBuilding("farm_estate"))
    {
        manu_gp_mod = 1.0;
    }

    if (hasBuilding("tradecompany"))
    {
        manu_gp_mod = 1.0;
    }

    // Base buildings
    if (hasBuilding("fort1"))
    {
        building_weight += 4;
    }
    if (hasBuilding("fort2"))
    {
        building_weight += 8;
    }
    if (hasBuilding("fort3"))
    {
        building_weight += 12;
    }
    if (hasBuilding("fort4"))
    {
        building_weight += 16;

    }
    if (hasBuilding("dock"))
    {
        building_weight += 6;
    }

    if (hasBuilding("drydock"))
    {
        building_weight += 12;
    }

    if (hasBuilding("shipyard"))
    {
        dev_modifier += 0.1;
    }

    if (hasBuilding("grand_shipyard"))
    {
        dev_modifier += 0.2;
    }

    if (hasBuilding("temple"))
    {
        building_tx_eff += 0.40;
    }

    if (hasBuilding("courthouse"))
    {
        dev_modifier += 0.075;
    }

    if (hasBuilding("town_hall"))
    {
        dev_modifier += 0.15;
    }

    if (hasBuilding("cathedral"))
    {
        building_tx_eff += 0.6;
        dev_modifier += 0.05;
    }

    if (hasBuilding("training_fields"))
    {
        manpower_eff += 1.00;
    }

    if (hasBuilding("barracks"))
    {
        manpower_eff += 0.50;
    }

    if (hasBuilding("regimental_camp"))
    {
        building_weight += 6;
    }

    if (hasBuilding("conscription_center"))
    {
        building_weight += 12;
    }

    if (hasBuilding("workshop"))
    {
        production_eff += 0.5;
    }

    if (hasBuilding("counting_house"))
    {
        production_eff += 1.0;
    }

    if (hasBuilding("stock_exchange"))
    {
        dev_modifier += 0.225;
    }

    if (hasBuilding("marketplace"))
    {
        dev_modifier += 0.075;
    }

    if (hasBuilding("trade_depot"))
    {
        dev_modifier += 0.15;
    }

    if (hasBuilding("center_of_trade"))
    {
        building_weight += 24;
    }

    if (hasBuilding("inland_center_of_trade"))
    {
        building_weight += 12;
    }

    if (hasBuilding("natural_harbor"))
    {
        building_weight += 15;
    }

    if (hasBuilding("stora_kopparberget_modifier"))
    {
        manu_gp_mod = 5.0;
    }

    if (hasBuilding("cerro_rico_modifier"))
    {
        manu_gp_mod = 3.0;
    }

    if (hasBuilding("spice_islands_modifier"))
    {
        manu_gp_mod = 3.0;
    }

	std::vector<double> provBuildingWeightVec;
	provBuildingWeightVec.push_back(building_weight);
	provBuildingWeightVec.push_back(manpower_modifier);
	provBuildingWeightVec.push_back(manu_gp_mod);
	provBuildingWeightVec.push_back(building_tx_eff);
	provBuildingWeightVec.push_back(production_eff);
	provBuildingWeightVec.push_back(building_tx_income);
	provBuildingWeightVec.push_back(manpower_eff);
	provBuildingWeightVec.push_back(goods_produced_perc_mod);
	provBuildingWeightVec.push_back(trade_power);
	provBuildingWeightVec.push_back(trade_value);
	provBuildingWeightVec.push_back(trade_value_eff);
	provBuildingWeightVec.push_back(trade_power_eff);
	provBuildingWeightVec.push_back(dev_modifier);
	// 0 building_weight, 1 manpower_modifier, 2 manu_gp_mod, 3 building_tx_eff, 4 production_eff
	// 5 building_tx_income, 6 manpower_eff, 7 goods_produced_perc_mod, 8 trade_power 9 trade_value
	// 10 trade_value_eff, 11 trade_power_eff;
	return provBuildingWeightVec;
}
