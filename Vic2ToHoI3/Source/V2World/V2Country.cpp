/*Copyright (c) 2015 The Paradox Game Converters Project

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
#include "../Log.h"
#include "../Configuration.h"
#include "../Parsers/Parser.h"
#include "V2World.h"
#include "V2Province.h"
#include "V2Factory.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Party.h"



V2Country::V2Country(Object* obj, const inventionNumToName& iNumToName)
{
	tag = obj->getKey();
	provinces.clear();
	cores.clear();
	adjective = tag;

	greatNationRanking = -1; // BE: Default to not great nation. This is set later in V2World.

	vector<Object*> nameObj = obj->getValue("domain_region");	// BE: the region name for dynamically generated dominions
	if (!nameObj.empty())
	{
		name = nameObj[0]->getLeaf().c_str();
		adjective = name;
	}
	else
	{
		name = "";
		adjective = "";
	}

	vector<Object*> capitalObj = obj->getValue("capital");	// the object holding the capital
	(capitalObj.size() > 0) ? capital = atoi(capitalObj[0]->getLeaf().c_str()) : capital = 0;

	vector<Object*> primaryCultureObj = obj->getValue("primary_culture");	// the object holding the primary culture
	(primaryCultureObj.size() > 0) ? primaryCulture = primaryCultureObj[0]->getLeaf().c_str() : primaryCulture = "";

	vector<Object*> techsObj = obj->getValue("technology");	// the object holding the technology levels
	if (techsObj.size() > 0)
	{
		techs = techsObj[0]->getKeys();
	}

	inventions.clear();
	techsObj = obj->getValue("active_inventions");
	if (techsObj.size() > 0)
	{
		vector<string> active_inventions = techsObj[0]->getTokens();
		for (vector<string>::iterator itr = active_inventions.begin(); itr != active_inventions.end(); ++itr)
		{
			int i = atoi(itr->c_str());
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
	vector<Object*> partyObj = obj->getValue("active_party");
	for (auto party: partyObj)
	{
		activeParties.push_back(atoi(party->getLeaf().c_str()));
	}

	partyObj = obj->getValue("ruling_party");
	if (partyObj.size() > 0)
	{
		rulingPartyId = atoi(partyObj[0]->getLeaf().c_str()); // Numerical ID
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
	vector<Object*> spendingObj = obj->getValue("education_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		(settingsObj.size() > 0) ? educationSpending = atof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	spendingObj = obj->getValue("military_spending");
	if (spendingObj.size() > 0)
	{
		vector<Object*> settingsObj = spendingObj[0]->getValue("settings");
		(settingsObj.size() > 0) ? militarySpending = atof(settingsObj[0]->getLeaf().c_str()) : 0.0;
	}

	vector<Object*> revanchismObj = obj->getValue("revanchism");
	if (revanchismObj.size() > 0)
	{
		revanchism = atof(revanchismObj[0]->getLeaf().c_str());
	}
	else
	{
		revanchism = 0.0;
	}

	vector<Object*> warExhaustionObj = obj->getValue("war_exhaustion");
	if (warExhaustionObj.size() > 0)
	{
		warExhaustion = atof(warExhaustionObj[0]->getLeaf().c_str());
	}
	else
	{
		warExhaustion = 0.0;
	}

	// Read reforms
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); ++i)
	{
		string key = leaves[i]->getKey();

		if (key == "slavery" || key == "vote_franchise" || key == "upper_house_composition" ||
			key == "voting_system" || key == "public_meetings" || key == "press_rights" ||
			key == "trade_unions" || key == "political_parties" || key == "wage_reform" ||
			key == "work_hours" || key == "safety_regulations" || key == "unemployment_subsidies" ||
			key == "pensions" || key == "health_care" || key == "school_reforms")
		{
			reformsArray[key] = leaves[i]->getLeaf();
		}
	}

	vector<Object*> governmentObj = obj->getValue("government");	// the object holding the government
	(governmentObj.size() > 0) ? government = governmentObj[0]->getLeaf() : government = "";

	auto upperHouseObj = obj->getValue("upper_house");
	auto ideologiesObj = upperHouseObj[0]->getLeaves();
	for (auto ideologyObj: ideologiesObj)
	{
		upperHouseComposition.insert(make_pair(ideologyObj[0].getKey(), atof(ideologyObj[0].getLeaf().c_str())));
	}

	flagFile = tag;
	if (government == "proletarian_dictatorship")
	{
		flagFile += "_communist";
	}
	else if (government == "presidential_dictatorship" || government == "bourgeois_dictatorship" || government == "democracy")
	{
		flagFile += "_republic";
	}
	else if (government == "fascist_dictatorship")
	{
		flagFile += "_fascist";
	}
	else if (government == "absolute_monarchy" || government == "prussian_constitutionalism" || government == "hms_government")
	{
		flagFile += "_monarchy";
	}

	flagFile += ".tga";

	// Read international relations leaves
	for (unsigned int i = 0; i < leaves.size(); ++i)
	{
		string key = leaves[i]->getKey();

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
			V2Relations* rel = new V2Relations(leaves[i]);
			relations.insert(make_pair(rel->getTag(), rel));
		}
	}

	armies.clear();
	vector<Object*> armyObj = obj->getValue("army");	// the object sholding the armies
	for (std::vector<Object*>::iterator itr = armyObj.begin(); itr != armyObj.end(); ++itr)
	{
		V2Army* army = new V2Army(*itr);
		armies.push_back(army);
	}
	vector<Object*> navyObj = obj->getValue("navy");	// the objects holding the navies
	for (std::vector<Object*>::iterator itr = navyObj.begin(); itr != navyObj.end(); ++itr)
	{
		V2Army* navy = new V2Army(*itr);
		armies.push_back(navy);
	}

	// read in states
	int count = 0;
	vector<Object*> statesObj = obj->getValue("state"); // each state in the country
	for (auto statesItr : statesObj)
	{
		V2State newState;
		// get the provinces in the state
		vector<Object*> provinceObj = statesItr[0].getValue("provinces");
		if (provinceObj.size() > 0)
		{
			vector<string> provinceIDs = provinceObj[0]->getTokens();
			for (auto provinceItr: provinceIDs)
			{
				newState.provinces.push_back(atoi(provinceItr.c_str()));
			}
		}

		// count the employees in the state (for factory conversion)
		//vector<Object*> buildingsObj = statesItr[0].getValue("state_buildings"); // each factory in the state
		//for (auto buildingsItr : buildingsObj)
		//{
		//	vector<Object*> employmentObj = buildingsItr[0].getValue("employment"); // each employment entry in the factory.
		//	for (auto employmentItr : employmentObj)
		//	{
		//		vector<Object*> employeesObj = employmentItr[0].getValue("employees"); // each employee entry in employment
		//		for (auto employeesItr : employeesObj)
		//		{
		//			vector<Object*> employeeObj = employeesItr[0].getLeaves(); // each employee object in employees
		//			for (auto employeeItr : employeeObj)
		//			{
		//				// this should work, except that the employee object is blank. I suspect more parser updates are in our future
		//				vector<Object*> countObj = employeeItr[0].getValue("count");
		//				if (countObj.size() > 0)
		//				{
		//					count += atoi(countObj[0]->getLeaf().c_str());
		//				}

		//				//something where you get the pop type and count total clerks and total craftsmen differently
		//			}
		//		}
		//	}
		//}

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
		for (auto provinceItr: target->provinces)
		{
			addProvince(provinceItr.first, provinceItr.second);
			provinceItr.second->setOwner(this);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());

		// give merged nation any techs owned by either nation
		vector<string> ttechs = target->getTechs();
		for (vector<string>::iterator tech = ttechs.begin(); tech != ttechs.end(); ++tech)
		{
			vector<string>::iterator stech = std::find(techs.begin(), techs.end(), *tech);
			if (stech == techs.end())
				techs.push_back(*tech);
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

std::string V2Country::getReform(std::string reform) const
{
	map<string, string>::const_iterator itr = reformsArray.find(reform);
	if (itr == reformsArray.end())
		return "";

	return itr->second;
}
string V2Country::getName(const string& language) const
{
	if (namesByLanguage.empty() && language == "english")
	{
		return name;
	}

	map<string, string>::const_iterator findIter = namesByLanguage.find(language);
	if (findIter != namesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}

string V2Country::getAdjective(const string& language) const
{
	if (adjectivesByLanguage.empty() && language == "english")
	{
		return adjective;
	}

	map<string, string>::const_iterator findIter = adjectivesByLanguage.find(language);
	if (findIter != adjectivesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}

double V2Country::getUpperHousePercentage(string ideology) const
{
	map<string, double>::const_iterator itr = upperHouseComposition.find(ideology);
	if (itr == upperHouseComposition.end())
		return 0.0;

	return itr->second;
}
