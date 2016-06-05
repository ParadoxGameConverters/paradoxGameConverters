/*Copyright (c) 2016 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "V2Country.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fstream>
#include <sstream>
#include "Log.h"
#include "../Configuration.h"
#include "ParadoxParser.h"
#include "V2Army.h"
#include "V2Factory.h"
#include "V2Leader.h"
#include "V2Party.h"
#include "V2Province.h"
#include "V2Relations.h"
#include "V2World.h"



V2Country::V2Country(Object* obj, const inventionNumToName& iNumToName, map<wstring, wstring>& armyTechs, map<wstring, wstring>& navyTechs, const continentMapping& continentMap)
{
	tag = obj->getKey();
	provinces.clear();
	cores.clear();
	adjective = tag;

	greatNation = false; // Default to not great nation. This is set later in V2World.

	vector<Object*> nameObj = obj->getValue(L"domain_region");	// the region name for dynamically generated dominions
	if (!nameObj.empty())
	{
		name = nameObj[0]->getLeaf().c_str();
		adjective = name;
	}
	else
	{
		name = L"";
		adjective = L"";
	}

	vector<Object*> capitalObj = obj->getValue(L"capital");	// the object holding the capital
	(capitalObj.size() > 0) ? capital = _wtoi(capitalObj[0]->getLeaf().c_str()) : capital = 0;

	auto continent = continentMap.find(capital);
	if (continent != continentMap.end())
	{
		capitalContinent = continent->second;
	}

	vector<Object*> primaryCultureObj = obj->getValue(L"primary_culture");	// the object holding the primary culture
	(primaryCultureObj.size() > 0) ? primaryCulture = primaryCultureObj[0]->getLeaf().c_str() : primaryCulture = L"";

	vector<Object*> techsObj = obj->getValue(L"technology");	// the object holding the technology levels
	if (techsObj.size() > 0)
	{
		techs = techsObj[0]->getKeys();
	}
	numArmyTechs	= 0;
	numNavyTechs	= 0;
	for (auto tech: techs)
	{
		if (armyTechs.find(tech) != armyTechs.end())
		{
			 numArmyTechs++;
		}
		if (navyTechs.find(tech) != navyTechs.end())
		{
			 numNavyTechs++;
		}
	}

	inventions.clear();
	techsObj = obj->getValue(L"active_inventions");
	if (techsObj.size() > 0)
	{
		vector<wstring> active_inventions = techsObj[0]->getTokens();
		for (vector<wstring>::iterator itr = active_inventions.begin(); itr != active_inventions.end(); ++itr)
		{
			int i = _wtoi(itr->c_str());
			auto jtr = iNumToName.find(i);
			if (jtr == iNumToName.end())
			{
				LOG(LogLevel::Warning) << tag << " has an invalid invention. Is this using a mod that changed inventions?";
			}
			else
			{
				inventions.push_back(jtr->second);
			}
		}
	}

	activeParties.clear();
	vector<Object*> partyObj = obj->getValue(L"active_party");
	for (auto party: partyObj)
	{
		activeParties.push_back(_wtoi(party->getLeaf().c_str()));
	}

	partyObj = obj->getValue(L"ruling_party");
	if (partyObj.size() > 0)
	{
		rulingPartyId = _wtoi(partyObj[0]->getLeaf().c_str()); // Numerical ID
	}
	else if (activeParties.size() > 0)
	{
		rulingPartyId = activeParties[0];
	}
	else
	{
		rulingPartyId = 0; // Bad value. For Rebel faction.
	}

	// Read spending
	vector<Object*> spendingObj = obj->getValue(L"education_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue(L"settings");
		(settingsObj.size() > 0) ? educationSpending = _wtof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	spendingObj = obj->getValue(L"military_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue(L"settings");
		(settingsObj.size() > 0) ? militarySpending = _wtof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	vector<Object*> revanchismObj = obj->getValue(L"revanchism");
	if (revanchismObj.size() > 0)
	{
		revanchism = _wtof(revanchismObj[0]->getLeaf().c_str());
	}
	else
	{
		revanchism = 0.0;
	}

	vector<Object*> warExhaustionObj = obj->getValue(L"war_exhaustion");
	if (warExhaustionObj.size() > 0)
	{
		warExhaustion = _wtof(warExhaustionObj[0]->getLeaf().c_str());
	}
	else
	{
		warExhaustion = 0.0;
	}

	// Read reforms
	map<wstring, wstring> reformTypes = governmentMapper::getInstance()->getReformTypes();

	vector<Object*> leaves = obj->getLeaves();
	for (auto leaf: leaves)
	{
		wstring key = leaf->getKey();

		if (reformTypes.find(key) != reformTypes.end())
		{
			reformsArray[key] = leaf->getLeaf();
		}
	}

	vector<Object*> governmentObj = obj->getValue(L"government");	// the object holding the government
	(governmentObj.size() > 0) ? government = governmentObj[0]->getLeaf() : government = L"";

	auto upperHouseObj = obj->getValue(L"upper_house");
	auto ideologiesObj = upperHouseObj[0]->getLeaves();
	for (auto ideologyObj: ideologiesObj)
	{
		upperHouseComposition.insert(make_pair(ideologyObj[0].getKey(), _wtof(ideologyObj[0].getLeaf().c_str())));
	}

	flagFile = tag;
	if (government == L"proletarian_dictatorship")
	{
		flagFile += L"_communist";
	}
	else if (government == L"presidential_dictatorship" || government == L"bourgeois_dictatorship" || government == L"democracy")
	{
		flagFile += L"_republic";
	}
	else if (government == L"fascist_dictatorship")
	{
		flagFile += L"_fascist";
	}
	else if (government == L"absolute_monarchy" || government == L"prussian_constitutionalism" || government == L"hms_government")
	{
		flagFile += L"_monarchy";
	}

	flagFile += L".tga";

	// Read international relations leaves
	for (auto leaf: leaves)
	{
		wstring key = leaf->getKey();

		if ((key.size() == 3) &&
			(
				(
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') &&
				(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				)
				||
				( // Dominions
				(key.c_str()[0] == 'D') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
				||
				( // Others (From previous conversion)
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
			)
		)
		{
			V2Relations* rel = new V2Relations(leaf);
			relations.insert(make_pair(rel->getTag(), rel));
		}
	}

	armies.clear();
	vector<Object*> armyObj = obj->getValue(L"army");	// the object sholding the armies
	for (auto armyItr: armyObj)
	{
		V2Army* army = new V2Army(armyItr);
		armies.push_back(army);
	}
	vector<Object*> navyObj = obj->getValue(L"navy");	// the objects holding the navies
	for (auto navyItr: navyObj)
	{
		V2Army* navy = new V2Army(navyItr);
		armies.push_back(navy);

		// get transported armies
		vector<Object*> armyObj = navyItr->getValue(L"army");	// the object sholding the armies
		for (auto armyItr: armyObj)
		{
			V2Army* army = new V2Army(armyItr);
			armies.push_back(army);
		}
	}

	leaders.clear();
	vector<Object*> leaderObj = obj->getValue(L"leader");	// the objects holding the leaders
	for (auto itr: leaderObj)
	{
		V2Leader* leader = new V2Leader(itr);
		leaders.push_back(leader);
	}

	vector<Object*> techSchoolObj = obj->getValue(L"schools");	// the objects holding the tech school
	if (techSchoolObj.size() > 0)
	{
		techSchool = techSchoolObj[0]->getLeaf();
	}

	// read in states
	vector<Object*> statesObj = obj->getValue(L"state"); // each state in the country
	for (auto statesItr : statesObj)
	{
		V2State newState;
		// get the provinces in the state
		vector<Object*> provinceObj = statesItr[0].getValue(L"provinces");
		if (provinceObj.size() > 0)
		{
			vector<wstring> provinceIDs = provinceObj[0]->getTokens();
			for (auto provinceItr: provinceIDs)
			{
				newState.provinces.push_back(_wtoi(provinceItr.c_str()));
			}
		}

		// count the employees in the state (for factory conversion)
		int levelCount = 0;
		vector<Object*> buildingsObj = statesItr[0].getValue(L"state_buildings"); // each factory in the state
		for (auto buildingsItr : buildingsObj)
		{
			vector<Object*> levelObj = buildingsItr[0].getValue(L"level"); // each employment entry in the factory.
			if (levelObj.size() > 0)
			{
				levelCount += _wtoi(levelObj[0]->getLeaf().c_str());
			}
		}
		newState.factoryLevels = levelCount;
		states.push_back(newState);
	}
}


void V2Country::addCore(V2Province* core)
{
	cores.push_back(core);
}

void V2Country::eatCountry(V2Country* target)
{
	// autocannibalism is forbidden
	if (target->getTag() == tag)
	{
		return;
	}

	// for calculation of weighted averages
	int totalProvinces = target->provinces.size() + provinces.size();
	if (totalProvinces == 0)
	{
		totalProvinces = 1;
	}
	const double myWeight = (double)provinces.size() / (double)totalProvinces;						// the amount of influence from the main country
	const double targetWeight = (double)target->provinces.size() / (double)totalProvinces;		// the amount of influence from the target country

	// acquire target's cores (always)
	for (auto core: target->cores)
	{
		addCore(core);
		core->addCore(tag);
		core->removeCore(target->tag);
	}

	// everything else, do only if this country actually currently exists
	if (target->provinces.size() > 0)
	{
		// acquire target's provinces
		for (auto provinceItr: target->provinces)
		{
			addProvince(provinceItr.first, provinceItr.second);
			provinceItr.second->setOwner(this);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());

		// give merged nation any techs owned by either nation
		vector<wstring> targetTechs = target->getTechs();
		for (auto tech: targetTechs)
		{
			vector<wstring>::iterator stech = std::find(techs.begin(), techs.end(), tech);
			if (stech == techs.end())
			{
				techs.push_back(tech);
			}
		}

		// and do the same with inventions
		auto targetInventions = target->getInventions();
		for (auto itr: targetInventions)
		{
			inventions.push_back(itr);
		}
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();

	LOG(LogLevel::Debug) << "Merged " << target->tag << " into " << tag;
}

void V2Country::clearProvinces()
{
	provinces.clear();
}


void V2Country::clearCores()
{
	cores.clear();
}


void V2Country::putWorkersInProvinces()
{
	for (auto state : states)
	{
		// get the employable workers
		int craftsmen		= 0;
		int clerks			= 0;
		int artisans		= 0;
		int capitalists	= 0;
		for (auto provinceNum : state.provinces)
		{
			auto province = provinces.find(provinceNum);
			if (province != provinces.end())
			{
				craftsmen	+= province->second->getPopulation(L"craftsmen");
				clerks		+= province->second->getPopulation(L"clerks");
				artisans		+= province->second->getPopulation(L"aristans");
				capitalists	+= province->second->getLiteracyWeightedPopulation(L"capitalists");
			}
		}

		// limit craftsmen and clerks by factory levels
		if ((craftsmen + clerks) > (state.factoryLevels * 10000))
		{
			float newCraftsmen	= (state.factoryLevels * 10000.0f) / (craftsmen + clerks) * craftsmen;
			float newClerks		= (state.factoryLevels * 10000.0f) / (craftsmen + clerks) * clerks;

			craftsmen	= static_cast<int>(newCraftsmen);
			clerks		= static_cast<int>(newClerks);
		}

		// determine an actual 'employed workers' score
		int employedWorkers = craftsmen + (clerks * 2) + static_cast<int>(artisans * 0.5) + (capitalists * 2);

		if (state.provinces.size() > 0)
		{
			auto employmentProvince = provinces.find(state.provinces.front());
			if (employmentProvince != provinces.end())
			{
				employmentProvince->second->setEmployedWorkers(employedWorkers);
			}
		}
	}
}


std::wstring V2Country::getReform(std::wstring reform) const
{
	map<wstring, wstring>::const_iterator itr = reformsArray.find(reform);
	if (itr == reformsArray.end())
	{
		return L"";
	}

	return itr->second;
}


wstring V2Country::getName(const wstring& language) const
{
	if (namesByLanguage.empty() && language == L"english")
	{
		return name;
	}

	map<wstring, wstring>::const_iterator findIter = namesByLanguage.find(language);
	if (findIter != namesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return L"";
	}
}


wstring V2Country::getAdjective(const wstring& language) const
{
	if (adjectivesByLanguage.empty() && language == L"english")
	{
		return adjective;
	}

	map<wstring, wstring>::const_iterator findIter = adjectivesByLanguage.find(language);
	if (findIter != adjectivesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return L"";
	}
}


double V2Country::getUpperHousePercentage(wstring ideology) const
{
	map<wstring, double>::const_iterator itr = upperHouseComposition.find(ideology);
	if (itr == upperHouseComposition.end())
		return 0.0;

	return itr->second;
}
