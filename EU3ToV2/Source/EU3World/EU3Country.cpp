/*Copyright (c) 2014 The Paradox Game Converters Project

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


#include "EU3Country.h"
#include "../Log.h"
#include "../Parsers/Object.h"
#include "EU3Province.h"
#include "EU3Relations.h"
#include "EU3Loan.h"
#include "EU3Leader.h"
#include <algorithm>



EU3Country::EU3Country(Object* obj)
{
	tag = obj->getKey();

	provinces.clear();
	cores.clear();

	vector<Object*> nameObj = obj->getValue("name");
	if (!nameObj.empty())
	{
		name = nameObj[0]->getLeaf();
	}

	vector<Object*> adjectiveObj = obj->getValue("adjective");
	if (!adjectiveObj.empty())
	{
		adjective = adjectiveObj[0]->getLeaf();
	}

	vector<Object*> colorObj = obj->getValue("map_color");
	if (!colorObj.empty())
	{
		color = Color(colorObj[0]);
		// Countries whose colors are included in the object here tend to be generated countries,
		// i.e. colonial nations which take on the color of their parent. To help distinguish 
		// these countries from their parent's other colonies we randomly adjust the color.
		color.RandomlyFlunctuate(30);
	}

	vector<Object*> capitalObj = obj->getValue("capital");
	if (capitalObj.size() > 0)
	{
		capital = atoi( capitalObj[0]->getLeaf().c_str() );
	}
	else
	{
		capital = 0;
	}

	vector<Object*> nfObj = obj->getValue("national_focus");
	if (nfObj.size() > 0)
	{
		nationalFocus = atoi( nfObj[0]->getLeaf().c_str() );
	}
	else
	{
		nationalFocus = 0;
	}

	vector<Object*> techGroupObj = obj->getValue("technology_group");
	if (techGroupObj.size() > 0)
	{
		techGroup = techGroupObj[0]->getLeaf().c_str();
	}
	else
	{
		techGroup = "";
	}

	vector<Object*> primaryCultureObj = obj->getValue("primary_culture");
	if (primaryCultureObj.size() > 0)
	{
		primaryCulture = primaryCultureObj[0]->getLeaf().c_str();
	}
	else
	{
		primaryCulture = "";
	}

	acceptedCultures.clear();
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
	else
	{
		religion = "";
	}

	vector<Object*> prestigeObj = obj->getValue("prestige");
	if (prestigeObj.size() > 0)
	{
		prestige = 100 * atof( prestigeObj[0]->getLeaf().c_str() );
	}
	else
	{
		prestige = -100.0;
	}

	vector<Object*> cultureObj = obj->getValue("cultural_tradition");
	if (cultureObj.size() > 0)
	{
		culture = 100 * atof( cultureObj[0]->getLeaf().c_str() );
	}
	else
	{
		culture = 0.0;
	}

	vector<Object*> armyTraditionObj = obj->getValue("army_tradition");
	if (armyTraditionObj.size() > 0)
	{
		armyTradition = 100 * atof( armyTraditionObj[0]->getLeaf().c_str() );
	}
	else
	{
		armyTradition = 0.0;
	}

	vector<Object*> navyTraditionObj = obj->getValue("navy_tradition");
	if (navyTraditionObj.size() > 0)
	{
		navyTradition = 100 * atof( navyTraditionObj[0]->getLeaf().c_str() );
	}
	else
	{
		navyTradition = 0.0;
	}

	vector<Object*> stabilityObj = obj->getValue("stability");
	if (stabilityObj.size() > 0)
	{
		stability = atof( stabilityObj[0]->getLeaf().c_str() );
	}
	else
	{
		stability = -3.0;
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

	vector<Object*> incomeObj = obj->getValue("estimated_monthly_income");
	if (incomeObj.size() > 0)
	{
		estMonthlyIncome = atof(incomeObj[0]->getLeaf().c_str());
	}
	else
	{
		estMonthlyIncome = 0.0;
	}

	vector<Object*> investmentObj = obj->getValue("distribution");
	if (investmentObj.size() > 0)
	{
		armyInvestment			= atof( investmentObj[0]->getTokens()[2].c_str() ) * estMonthlyIncome;
		navyInvestment			= atof( investmentObj[0]->getTokens()[3].c_str() ) * estMonthlyIncome;
		commerceInvestment	= atof( investmentObj[0]->getTokens()[4].c_str() ) * estMonthlyIncome;
		industryInvestment	= atof( investmentObj[0]->getTokens()[5].c_str() ) * estMonthlyIncome;
		cultureInvestment		= atof( investmentObj[0]->getTokens()[6].c_str() ) * estMonthlyIncome;
	}
	else
	{
		armyInvestment			= 0.0;
		navyInvestment			= 0.0;
		commerceInvestment	= 0.0;
		industryInvestment	= 0.0;
		cultureInvestment		= 0.0;
	}

	flags.clear();
	vector<Object*> flagObject	= obj->getValue("flags");
	if (flagObject.size() > 0)
	{
		vector<Object*> flagObjects = flagObject[0]->getLeaves();
		for (unsigned int i = 0; i < flagObjects.size(); i++)
		{
			flags[flagObjects[i]->getKey()] = true;
		}
	}
	flagObject	= obj->getValue("hidden_flags");
	if (flagObject.size() > 0)
	{
		vector<Object*> flagObjects = flagObject[0]->getLeaves();
		for (unsigned int i = 0; i < flagObjects.size(); i++)
		{
			flags[flagObjects[i]->getKey()] = true;
		}
	}

	modifiers.clear();
	vector<Object*> modifierObject	= obj->getValue("modifier");
	for (unsigned int i = 0; i < modifierObject.size(); i++)
	{
		vector<Object*> nameObject = modifierObject[i]->getLeaves();
		if (nameObject.size() > 0)
		{
			modifiers[nameObject[0]->getLeaf()] = true;
		}
	}

	possibleDaimyo = false;
	leaders.clear();
	vector<Object*> historyObj	= obj->getValue("history");
	if (historyObj.size() > 0)
	{
		vector<Object*> daimyoObj = historyObj[0]->getValue("daimyo");
		if (daimyoObj.size() > 0)
		{
			possibleDaimyo = true;
		}

		vector<Object*> historyLeaves = historyObj[0]->getLeaves();
		date hundredYearsOld = date("1740.1.1");
		for (vector<Object*>::iterator itr = historyLeaves.begin(); itr != historyLeaves.end(); ++itr)
		{
			// grab leaders from history, ignoring those that are more than 100 years old...
			if (date((*itr)->getKey()) > hundredYearsOld)
			{
				vector<Object*> leaderObjs = (*itr)->getValue("leader");
				for (vector<Object*>::iterator litr = leaderObjs.begin(); litr != leaderObjs.end(); ++litr)
				{
					EU3Leader* leader = new EU3Leader(*litr);
					leaders.push_back(leader);
				}
			}
		}
	}

	// figure out which leaders are active, and ditch the rest
	vector<Object*> activeLeaderObj = obj->getValue("leader");
	vector<int> activeIds;
	vector<EU3Leader*> activeLeaders;
	for (vector<Object*>::iterator itr = activeLeaderObj.begin(); itr != activeLeaderObj.end(); ++itr)
	{
		activeIds.push_back(atoi((*itr)->getLeaf("id").c_str()));
	}
	for (vector<EU3Leader*>::iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
	{
		if (find(activeIds.begin(), activeIds.end(), (*itr)->getID()) != activeIds.end())
			activeLeaders.push_back(*itr);
	}
	leaders.swap(activeLeaders);

	vector<Object*> governmentObj = obj->getValue("government");
	if (governmentObj.size() > 0)
	{
		government = governmentObj[0]->getLeaf();
	}
	else
	{
		government = "";
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
			EU3Relations* rel = new EU3Relations(leaves[i]);
			relations.push_back(rel);
		}
	}

	armies.clear();
	vector<Object*> armyObj = obj->getValue("army");
	for (std::vector<Object*>::iterator itr = armyObj.begin(); itr != armyObj.end(); ++itr)
	{
		EU3Army* army = new EU3Army(*itr);
		armies.push_back(army);
	}
	vector<Object*> navyObj = obj->getValue("navy");
	for (std::vector<Object*>::iterator itr = navyObj.begin(); itr != navyObj.end(); ++itr)
	{
		EU3Army* navy = new EU3Army(*itr);
		armies.push_back(navy);
	}

	vector<Object*> sliderObj = obj->getValue("centralization_decentralization");
	if (sliderObj.size() > 0)
	{
		centralization_decentralization = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		centralization_decentralization = 0;
	}
	sliderObj = obj->getValue("aristocracy_plutocracy");
	if (sliderObj.size() > 0)
	{
		aristocracy_plutocracy = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		aristocracy_plutocracy = 0;
	}
	sliderObj = obj->getValue("serfdom_freesubjects");
	if (sliderObj.size() > 0)
	{
		serfdom_freesubjects = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		serfdom_freesubjects = 0;
	}
	sliderObj = obj->getValue("innovative_narrowminded");
	if (sliderObj.size() > 0)
	{
		innovative_narrowminded = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		innovative_narrowminded = 0;
	}
	sliderObj = obj->getValue("mercantilism_freetrade");
	if (sliderObj.size() > 0)
	{
		mercantilism_freetrade = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		mercantilism_freetrade = 0;
	}
	sliderObj = obj->getValue("offensive_defensive");
	if (sliderObj.size() > 0)
	{
		offensive_defensive = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		offensive_defensive = 0;
	}
	sliderObj = obj->getValue("land_naval");
	if (sliderObj.size() > 0)
	{
		land_naval = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		land_naval = 0;
	}
	sliderObj = obj->getValue("quality_quantity");
	if (sliderObj.size() > 0)
	{
		quality_quantity = atoi( sliderObj[0]->getLeaf().c_str() );
	}
	else
	{
		quality_quantity = 0;
	}

	nationalIdeas.clear();
	checkIdea(obj, "press_gangs");
	checkIdea(obj, "grand_navy");
	checkIdea(obj, "sea_hawks");
	checkIdea(obj, "superior_seamanship");
	checkIdea(obj, "naval_glory");
	checkIdea(obj, "excellent_shipwrights");
	checkIdea(obj, "naval_fighting_instruction");
	checkIdea(obj, "naval_provisioning");
	checkIdea(obj, "grand_army");
	checkIdea(obj, "military_drill");
	checkIdea(obj, "engineer_corps");
	checkIdea(obj, "battlefield_commisions");
	checkIdea(obj, "glorious_arms");
	checkIdea(obj, "national_conscripts");
	checkIdea(obj, "regimental_system");
	checkIdea(obj, "napoleonic_warfare");
	checkIdea(obj, "land_of_opportunity");
	checkIdea(obj, "merchant_adventures");
	checkIdea(obj, "colonial_ventures");
	checkIdea(obj, "shrewd_commerce_practise");
	checkIdea(obj, "vice_roys");
	checkIdea(obj, "quest_for_the_new_world");
	checkIdea(obj, "scientific_revolution");
	checkIdea(obj, "improved_foraging");
	checkIdea(obj, "vetting");
	checkIdea(obj, "bureaucracy");
	checkIdea(obj, "national_bank");
	checkIdea(obj, "national_trade_policy");
	checkIdea(obj, "espionage");
	checkIdea(obj, "bill_of_rights");
	checkIdea(obj, "smithian_economics");
	checkIdea(obj, "liberty_egalite_fraternity");
	checkIdea(obj, "ecumenism");
	checkIdea(obj, "church_attendance_duty");
	checkIdea(obj, "divine_supremacy");
	checkIdea(obj, "patron_of_art");
	checkIdea(obj, "deus_vult");
	checkIdea(obj, "humanist_tolerance");
	checkIdea(obj, "cabinet");
	checkIdea(obj, "revolution_and_counter");

	vector<Object*> moneyObj = obj->getValue("treasury");
	if (moneyObj.size() > 0)
	{
		treasury = atof(moneyObj[0]->getLeaf().c_str());
	}
	else
	{
		treasury = 0.0;
	}

	moneyObj = obj->getValue("last_bankrupt");
	if (moneyObj.size() > 0)
	{
		last_bankrupt = date(moneyObj[0]);
	}
	else
	{
		last_bankrupt = date();
	}

	loans.clear();
	moneyObj = obj->getValue("loan");
	for (vector<Object*>::iterator itr = moneyObj.begin(); itr != moneyObj.end(); ++itr)
	{
		EU3Loan* loan = new EU3Loan(*itr);
		loans.push_back(loan);
	}

	vector<Object*> diploObj = obj->getValue("diplomats");
	if (diploObj.size() > 0)
	{
		diplomats = atoi(diploObj[0]->getLeaf().c_str());
	}
	else
	{
		diplomats = 0;
	}

	vector<Object*> badboyObj = obj->getValue("badboy");
	if (badboyObj.size() > 0)
	{
		badboy = atof(badboyObj[0]->getLeaf().c_str());
	}
	else
	{
		badboy = 0.0;
	}

	vector<Object*> legitObj = obj->getValue("legitimacy");
	if (legitObj.size() > 0)
	{
		legitimacy = atof(legitObj[0]->getLeaf().c_str());
	}
	else
	{
		legitimacy = 1.0;
	}

	moneyObj = obj->getValue("inflation");
	if (moneyObj.size() > 0)
	{
		inflation = atof(moneyObj[0]->getLeaf().c_str());
	}
	else
	{
		inflation = 0.0;
	}
}


void EU3Country::readFromCommonCountry(const string& fileName, Object* obj)
{
	if (name.empty())
	{
		// For this country's name we will use the stem of the file name.
		size_t extPos = fileName.find_last_of('.');
		name = fileName.substr(0, extPos);
	}

	if (!color)
	{
		// Read country color.
		auto colorObj = obj->getValue("color");
		if (colorObj[0])
		{
			color = Color(colorObj[0]);
		}
	}
}


void EU3Country::setLocalisationName(const unsigned int language, const string& name)
{
	namesByLanguage[language] = name;
}


void EU3Country::setLocalisationAdjective(const unsigned int language, const string& adjective)
{
	adjectivesByLanguage[language] = adjective;
}


void EU3Country::addProvince(EU3Province* province)
{
	provinces.push_back(province);
}


void EU3Country::addCore(EU3Province* core)
{
	cores.push_back(core);
}


bool EU3Country::hasModifier(string modifier) const
{
	map<string, bool>::const_iterator itr = modifiers.find(modifier);
	return (itr != modifiers.end());
}


bool EU3Country::hasNationalIdea(string ni) const
{
	set<string>::const_iterator itr = nationalIdeas.find(ni);
	return (itr != nationalIdeas.end());
}


bool EU3Country::hasFlag(string flag) const
{
	map<string, bool>::const_iterator itr = flags.find(flag);
	return (itr != flags.end());
}


void EU3Country::resolveRegimentTypes(const RegimentTypeMap& map)
{
	for (vector<EU3Army*>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		(*itr)->resolveRegimentTypes(map);
	}
}


int EU3Country::getManufactoryCount() const
{
	int retval = 0;
	for (vector<EU3Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->hasBuilding("weapons"))
			++retval;
		if ((*itr)->hasBuilding("wharf"))
			++retval;
		if ((*itr)->hasBuilding("textile"))
			++retval;
		if ((*itr)->hasBuilding("refinery"))
			++retval;
	}
	return retval;
}


double EU3Country::inflationAdjust(double input) const
{
	return (input / (1.0 + (inflation / 100.0)));
}


double EU3Country::getBadboyLimit() const
{
	double badboyLimit = 30.0;

	// gov effects
	if (government == "despotic_monarchy" || government == "tribal_despotism")
		badboyLimit += 10.0;

	// modifier effects
	if (hasModifier("the_licensing_of_the_press_act"))
		badboyLimit -= 3.0;
	if (hasModifier("the_witchcraft_act"))
		badboyLimit -= 2.0;
	if (hasModifier("the_education_act"))
		badboyLimit -= 2.0;
	if (hasModifier("revocation_of_restraint_of_appeals"))
		badboyLimit -= 2.0;
	if (hasModifier("colonial_expansions"))
		badboyLimit -= 3.0;
	if (hasModifier("hire_privateers"))
		badboyLimit -= 3.0;
	if (hasModifier("the_anti_piracy_act"))
		badboyLimit -= 3.0;

	// ruler effects (DIP, tribal MIL) not taken into account - assume the best (DIP 8/MIL 8)
	badboyLimit += 8.0;

	// legitimacy effect (-5 at 0, +5 at 100)
	badboyLimit += 10 * (legitimacy - 0.5);

	return badboyLimit;
}


void EU3Country::eatCountry(EU3Country* target)
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
			target->provinces[j]->setOwner(this);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());
		leaders.insert(leaders.end(), target->leaders.begin(), target->leaders.end());

		// acquire the target's treasury and income, as well as their liabilities
		treasury += target->treasury;
		estMonthlyIncome += target->estMonthlyIncome;
		loans.insert(loans.end(), target->loans.begin(), target->loans.end());

		// rebalance prestige, badboy, inflation and techs from weighted average
		prestige					= myWeight * prestige					+ targetWeight * target->prestige;
		badboy					= myWeight * badboy						+ targetWeight * target->badboy * (getBadboyLimit() / target->getBadboyLimit());
		inflation				= myWeight * inflation					+ targetWeight * target->inflation;
		landTech					= myWeight * landTech					+ targetWeight * target->landTech;
		navalTech				= myWeight * navalTech					+ targetWeight * target->navalTech;
		tradeTech				= myWeight * tradeTech					+ targetWeight * target->tradeTech;
		productionTech			= myWeight * productionTech			+ targetWeight * target->productionTech;
		governmentTech			= myWeight * governmentTech			+ targetWeight * target->governmentTech;
		armyInvestment			= myWeight * armyInvestment			+ targetWeight * target->armyInvestment;
		navyInvestment			= myWeight * navyInvestment			+ targetWeight * target->navyInvestment;
		commerceInvestment	= myWeight * commerceInvestment		+ targetWeight * target->commerceInvestment;
		industryInvestment	= myWeight * industryInvestment		+ targetWeight * target->industryInvestment;
		cultureInvestment		= myWeight * cultureInvestment		+ targetWeight * target->cultureInvestment;
		estMonthlyIncome		= myWeight * estMonthlyIncome			+ targetWeight * target->estMonthlyIncome;
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();

	LOG(LogLevel::Debug) << "Merged " << target->tag << " into " << tag;
}


string EU3Country::getName(const unsigned int language) const
{
	if (namesByLanguage.empty() && language == 1) // english
	{
		return name;
	}

	map<int, string>::const_iterator findIter = namesByLanguage.find(language);
	if (findIter != namesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}


string EU3Country::getAdjective(const unsigned int language) const
{
	if (adjectivesByLanguage.empty() && language == 1)	// english
	{
		return adjective;
	}

	map<int, string>::const_iterator findIter = adjectivesByLanguage.find(language);
	if (findIter != adjectivesByLanguage.end())
	{
		return findIter->second;
	}
	else
	{
		return "";
	}
}


void EU3Country::checkIdea(const Object* countryObj, const string idea)
{
	vector<Object*> niObj;
	niObj = countryObj->getValue(idea);
	if ((niObj.size() > 0) && (niObj[0]->getLeaf() == "yes"))
	{
		nationalIdeas.insert(idea);
	}
}


void EU3Country::clearProvinces()
{
	provinces.clear();
}


void EU3Country::clearCores()
{
	cores.clear();
}