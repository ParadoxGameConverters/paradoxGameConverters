#include "EU4Country.h"
#include "../Log.h"
#include "../Parsers/Object.h"
#include "EU4Province.h"
#include "EU4Relations.h"
#include "EU4Loan.h"
#include "EU4Leader.h"
#include <algorithm>


EU4Country::EU4Country(Object* obj)
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

	vector<Object*> unionCultureObj = obj->getValue("culture_group_union");
	if (unionCultureObj.size() > 0)
	{
		culturalUnion = unionCultureObj[0]->getLeaf();
	}
	else
	{
		culturalUnion = "";
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

	vector<Object*> scoreObj = obj->getValue("score");
	if (scoreObj.size() > 0)
	{
		score = 100 * atof(scoreObj[0]->getLeaf().c_str());
	}
	else
	{
		score = 0.0;
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
		vector<Object*> techObj = techsObj[0]->getValue("adm_tech");
		admTech = atof( techObj[0]->getLeaf().c_str() );

		techsObj = obj->getValue("technology");
		techObj = techsObj[0]->getValue("dip_tech");
		dipTech = atof( techObj[0]->getLeaf().c_str() );

		techsObj = obj->getValue("technology");
		techObj = techsObj[0]->getValue("mil_tech");
		milTech = atof( techObj[0]->getLeaf().c_str() );
	}
	else
	{
		admTech		= 0.0;
		dipTech		= 0.0;
		milTech		= 0.0;
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
		landInvestment			= atof( investmentObj[0]->getTokens()[2].c_str() ) * estMonthlyIncome;
		navalInvestment			= atof( investmentObj[0]->getTokens()[3].c_str() ) * estMonthlyIncome;
		tradeInvestment			= atof( investmentObj[0]->getTokens()[4].c_str() ) * estMonthlyIncome;
		productionInvestment	= atof( investmentObj[0]->getTokens()[5].c_str() ) * estMonthlyIncome;
		governmentInvestment	= atof( investmentObj[0]->getTokens()[6].c_str() ) * estMonthlyIncome;
	}
	else
	{
		landInvestment			= 0.0;
		navalInvestment		= 0.0;
		tradeInvestment		= 0.0;
		productionInvestment	= 0.0;
		governmentInvestment	= 0.0;
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
					EU4Leader* leader = new EU4Leader(*litr);
					leaders.push_back(leader);
				}
			}
		}
	}

	// figure out which leaders are active, and ditch the rest
	vector<Object*> activeLeaderObj = obj->getValue("leader");
	vector<int> activeIds;
	vector<EU4Leader*> activeLeaders;
	for (vector<Object*>::iterator itr = activeLeaderObj.begin(); itr != activeLeaderObj.end(); ++itr)
	{
		activeIds.push_back(atoi((*itr)->getLeaf("id").c_str()));
	}
	for (vector<EU4Leader*>::iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
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
	vector<Object*> relationLeaves = obj->getValue("active_relations");
	vector<Object*> relationsLeaves = relationLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < relationsLeaves.size(); ++i)
	{
		string key = relationsLeaves[i]->getKey();
		EU4Relations* rel = new EU4Relations(relationsLeaves[i]);
		relations.push_back(rel);
	}

	armies.clear();
	vector<Object*> armyObj = obj->getValue("army");
	for (std::vector<Object*>::iterator itr = armyObj.begin(); itr != armyObj.end(); ++itr)
	{
		EU4Army* army = new EU4Army(*itr);
		armies.push_back(army);
	}
	vector<Object*> navyObj = obj->getValue("navy");
	for (std::vector<Object*>::iterator itr = navyObj.begin(); itr != navyObj.end(); ++itr)
	{
		EU4Army* navy = new EU4Army(*itr);
		armies.push_back(navy);
	}

	nationalIdeas.clear();
	vector<Object*> activeIdeasObj = obj->getValue("active_idea_groups");
	if (activeIdeasObj.size() > 0)
	{
		vector<Object*> ideasObj = activeIdeasObj[0]->getLeaves();
		for (vector<Object*>::iterator ideaItr = ideasObj.begin(); ideaItr != ideasObj.end(); ideaItr++)
		{
			nationalIdeas.insert(make_pair((*ideaItr)->getKey(), atoi((*ideaItr)->getLeaf().c_str())));
		}
	}

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
		EU4Loan* loan = new EU4Loan(*itr);
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


bool EU4Country::isUnusedCountry() const
{
	return capital == 0;
}

void EU4Country::readFromCommonCountry(const string& fileName, Object* obj)
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


void EU4Country::setLocalisationName(const string& language, const string& name)
{
	namesByLanguage[language] = name;
}


void EU4Country::setLocalisationAdjective(const string& language, const string& adjective)
{
	adjectivesByLanguage[language] = adjective;
}


void EU4Country::addProvince(EU4Province* province)
{
	provinces.push_back(province);
}


void EU4Country::addCore(EU4Province* core)
{
	cores.push_back(core);
}


bool EU4Country::hasModifier(string modifier) const
{
	map<string, bool>::const_iterator itr = modifiers.find(modifier);
	return (itr != modifiers.end());
}


int EU4Country::hasNationalIdea(string ni) const
{
	map<string, int>::const_iterator itr = nationalIdeas.find(ni);
	if (itr != nationalIdeas.end())
	{
		return itr->second;
	}
	else
	{
		return -1;
	}
}


bool EU4Country::hasFlag(string flag) const
{
	map<string, bool>::const_iterator itr = flags.find(flag);
	return (itr != flags.end());
}


void EU4Country::resolveRegimentTypes(const RegimentTypeMap& map)
{
	for (vector<EU4Army*>::iterator itr = armies.begin(); itr != armies.end(); ++itr)
	{
		(*itr)->resolveRegimentTypes(map);
	}
}


int EU4Country::getManufactoryCount() const
{
	int retval = 0;
	for (vector<EU4Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
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


double EU4Country::inflationAdjust(double input) const
{
	return (input / (1.0 + (inflation / 100.0)));
}


double EU4Country::getBadboyLimit() const
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


void EU4Country::eatCountry(EU4Country* target)
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
		score						= myWeight * score						+ targetWeight * target->score;
		badboy					= myWeight * badboy						+ targetWeight * target->badboy * (getBadboyLimit() / target->getBadboyLimit());
		inflation				= myWeight * inflation					+ targetWeight * target->inflation;
		admTech					= myWeight * admTech						+ targetWeight * target->admTech;
		dipTech					= myWeight * dipTech						+ targetWeight * target->dipTech;
		milTech					= myWeight * milTech						+ targetWeight * target->milTech;
		landInvestment			= myWeight * landInvestment			+ targetWeight * target->landInvestment;
		navalInvestment		= myWeight * navalInvestment			+ targetWeight * target->navalInvestment;
		tradeInvestment		= myWeight * tradeInvestment			+ targetWeight * target->tradeInvestment;
		productionInvestment	= myWeight * productionInvestment	+ targetWeight * target->productionInvestment;
		governmentInvestment	= myWeight * governmentInvestment	+ targetWeight * target->governmentInvestment;
		estMonthlyIncome		= myWeight * estMonthlyIncome			+ targetWeight * target->estMonthlyIncome;
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();

	LOG(LogLevel::Debug) << "Merged " << target->tag << " into " << tag;
}


string EU4Country::getName(const string& language) const
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


string EU4Country::getAdjective(const string& language) const
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


void EU4Country::checkIdea(const Object* countryObj, const string idea)
{
	vector<Object*> niObj;
	niObj = countryObj->getValue(idea);
	if ((niObj.size() > 0) && (niObj[0]->getLeaf() == "yes"))
	{
		nationalIdeas[idea] = true;
	}
}


void EU4Country::clearProvinces()
{
	provinces.clear();
}


void EU4Country::clearCores()
{
	cores.clear();
}