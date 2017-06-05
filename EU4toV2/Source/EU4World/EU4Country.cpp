/*Copyright(c) 2017 The Paradox Game Converters Project

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


#include "EU4Country.h"
#include "../Configuration.h"
#include "Log.h"
#include "Object.h"
#include "EU4Province.h"
#include "EU4Relations.h"
#include "EU4Leader.h"
#include "EU4Version.h"
#include "../Mappers/EU4CultureGroupMapper.h"
#include "../Mappers/IdeaEffectMapper.h"
#include "../V2World/V2Localisation.h"
#include <algorithm>
#include <boost/lexical_cast.hpp>



EU4Country::EU4Country(Object* obj, EU4Version* version)
{
	tag = obj->getKey();

	provinces.clear();
	cores.clear();
	inHRE					= false;
	holyRomanEmperor	= false;
	celestialEmperor	= false;

	vector<Object*> nameObj = obj->getValue("name");	// the object holding the name
	(!nameObj.empty()) ? name = nameObj[0]->getLeaf() : name = "";

	vector<Object*> customNameObj = obj->getValue("custom_name");	// the object holding the name
	(!customNameObj.empty()) ? randomName = V2Localisation::Convert(customNameObj[0]->getLeaf()) : randomName = "";

	vector<Object*> adjectiveObj = obj->getValue("adjective");	// the object holding the adjective
	(!adjectiveObj.empty()) ? adjective = adjectiveObj[0]->getLeaf() : adjective = "";

	vector<Object*> colorObjs = obj->getValue("map_color");
	if (!colorObjs.empty())
	{
		color = Color(colorObjs[0]);
		// Countries whose colors are included in the object here tend to be generated countries,
		// i.e. colonial nations which take on the color of their parent. To help distinguish 
		// these countries from their parent's other colonies we randomly adjust the color.
		color.RandomlyFlunctuate(30);
	}
	else
	{
		vector<Object*> colorObjs = obj->getValue("colors");
		if (colorObjs.size() > 0)
		{
			vector<Object*> countryColorObjs = colorObjs[0]->getValue("country_color");
			if (countryColorObjs.size() > 0)
			{
				color = Color(countryColorObjs[0]);
				// Countries whose colors are included in the object here tend to be generated countries,
				// i.e. colonial nations which take on the color of their parent. To help distinguish 
				// these countries from their parent's other colonies we randomly adjust the color.
				//color.RandomlyFlunctuate(30);
			}
		}
	}

	vector<Object*> capitalObj = obj->getValue("capital");	// the object holding the capital
	(capitalObj.size() > 0) ? capital = atoi( capitalObj[0]->getLeaf().c_str() ) : capital = 0;

	vector<Object*> nfObj = obj->getValue("national_focus");	// the object holding the national focus
	(nfObj.size() > 0) ? nationalFocus = atoi( nfObj[0]->getLeaf().c_str() ) : nationalFocus = 0;

	vector<Object*> techGroupObj = obj->getValue("technology_group");	// the object holding the technology group
	(techGroupObj.size() > 0) ? techGroup = techGroupObj[0]->getLeaf().c_str() : techGroup = "";

	embracedInstitutions.clear();
	vector<Object*> institutionsObj = obj->getValue("institutions"); // the object holding the institutions
	if (institutionsObj.size() > 0)
	{
		vector<string> institutionTokens = institutionsObj[0]->getTokens();
		for (unsigned int i = 0; i < institutionTokens.size(); i++) {
			embracedInstitutions.push_back(boost::lexical_cast<bool>(institutionTokens[i]));
		}
	}

	auto version20 = EU4Version("1.20.0.0");
	if (*version >= version20)
	{
		vector<Object*> isolationismObj = obj->getValue("isolationism"); // the object holding the isolationism
		(isolationismObj.size() > 0) ? isolationism = atof(isolationismObj[0]->getLeaf().c_str()) : isolationism = 1.0;
	}

	vector<Object*> primaryCultureObj = obj->getValue("primary_culture");	// the object holding the primary culture
	(primaryCultureObj.size() > 0) ? primaryCulture = primaryCultureObj[0]->getLeaf().c_str() : primaryCulture = "";

	acceptedCultures.clear();
	vector<Object*> acceptedCultureObj = obj->getValue("accepted_culture");	// the object holding the accepted cultures
	for (unsigned int i = 0; i < acceptedCultureObj.size(); i++)
	{
		acceptedCultures.push_back(acceptedCultureObj[i]->getLeaf().c_str());
	}

	auto version14 = EU4Version("1.14.0.0");
	if (*version >= version14)
	{
		bool wasUnion = false;
		if (Configuration::wasDLCActive("The Cossacks"))
		{
			vector <Object*> govRankObjs = obj->getValue("government_rank");
			if (atoi(govRankObjs[0]->getLeaf().c_str()) > 2)
			{
				wasUnion = true;
			}
		}
		else
		{
			vector <Object*> developmentObj = obj->getValue("realm_development");
			if (atof(developmentObj[0]->getLeaf().c_str()) >= 1000)
			{
				wasUnion = true;
			}
		}
		if (wasUnion)
		{
			culturalUnion = EU4CultureGroupMapper::getCulturalGroup(primaryCulture);
		}
	}
	else
	{
		vector<Object*> unionCultureObj = obj->getValue("culture_group_union");	// the object holding the cultural union group
		(unionCultureObj.size() > 0) ? culturalUnion = unionCultureObj[0]->getLeaf() : culturalUnion = "";
	}

	vector<Object*> religionObj = obj->getValue("religion");	// the object holding the religion
	(religionObj.size() > 0) ? religion = religionObj[0]->getLeaf().c_str() : religion = "";

	vector<Object*> scoreObj = obj->getValue("score");	// the object holding the score
	(scoreObj.size() > 0) ? score = 100 * atof(scoreObj[0]->getLeaf().c_str()) : score = 0.0;

	vector<Object*> stabilityObj = obj->getValue("stability");	// the object holding the stability
	(stabilityObj.size() > 0) ? stability = atof( stabilityObj[0]->getLeaf().c_str() ) : stability = -3.0;

	vector<Object*> techsObj = obj->getValue("technology");	// the object holding the technology levels
	if (techsObj.size() > 0)
	{
		vector<Object*> techObj = techsObj[0]->getValue("adm_tech");	// the object holding the technology under consideration
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

	determineFlagsAndModifiers(obj);

	possibleDaimyo = false;
	possibleShogun = false;
	leaders.clear();
	vector<Object*> historyObj = obj->getValue("history");	// the object holding the history for this country
	if (historyObj.size() > 0)
	{
		/*vector<Object*> daimyoObj = historyObj[0]->getValue("daimyo");	// the object holding the daimyo information for this country
		if (daimyoObj.size() > 0)
		{
			possibleDaimyo = true;
		}*/

		vector<Object*> historyLeaves = historyObj[0]->getLeaves();	// the object holding the individual histories for this country
		date hundredYearsOld = date("1740.1.1");							// one hundred years before conversion
		for (vector<Object*>::iterator itr = historyLeaves.begin(); itr != historyLeaves.end(); ++itr)
		{
			// grab leaders from history, ignoring those that are more than 100 years old...
			if (date((*itr)->getKey()) > hundredYearsOld)
			{
				vector<Object*> leaderObjs = (*itr)->getValue("leader");	// the leaders in this history
				for (vector<Object*>::iterator litr = leaderObjs.begin(); litr != leaderObjs.end(); ++litr)
				{
					EU4Leader* leader = new EU4Leader(*litr);	// a new leader
					leaders.push_back(leader);
				}
			}
		}
	}

	// figure out which leaders are active, and ditch the rest
	vector<Object*> activeLeaderObj = obj->getValue("leader");	// the object holding the active leaders
	vector<int> activeIds;													// the ids for the active leaders
	vector<EU4Leader*> activeLeaders;									// the active leaders themselves
	for (vector<Object*>::iterator itr = activeLeaderObj.begin(); itr != activeLeaderObj.end(); ++itr)
	{
		activeIds.push_back(atoi((*itr)->getLeaf("id").c_str()));
	}
	for (vector<EU4Leader*>::iterator itr = leaders.begin(); itr != leaders.end(); ++itr)
	{
		if (find(activeIds.begin(), activeIds.end(), (*itr)->getID()) != activeIds.end())
		{
			activeLeaders.push_back(*itr);
		}
	}
	leaders.swap(activeLeaders);

	vector<Object*> governmentObj = obj->getValue("government");	// the object holding the government
	(governmentObj.size() > 0) ? government = governmentObj[0]->getLeaf() : government = "";
	if (government == "daimyo") 
	{
		possibleDaimyo = true;
	}

	if (government == "shogunate")
	{
		possibleShogun = true;
	}
	// Read international relations leaves
	vector<Object*> relationLeaves = obj->getValue("active_relations");	// the object holding the active relationships
	vector<Object*> relationsLeaves = relationLeaves[0]->getLeaves();		// the objects holding the relationships themselves
	for (unsigned int i = 0; i < relationsLeaves.size(); ++i)
	{
		string key = relationsLeaves[i]->getKey();
		EU4Relations* rel = new EU4Relations(relationsLeaves[i]);
		relations.insert(make_pair(key, rel));
	}

	armies.clear();
	vector<Object*> armyObj = obj->getValue("army");	// the object sholding the armies
	for (std::vector<Object*>::iterator itr = armyObj.begin(); itr != armyObj.end(); ++itr)
	{
		EU4Army* army = new EU4Army(*itr);
		armies.push_back(army);
	}
	vector<Object*> navyObj = obj->getValue("navy");	// the objects holding the navies
	for (std::vector<Object*>::iterator itr = navyObj.begin(); itr != navyObj.end(); ++itr)
	{
		EU4Army* navy = new EU4Army(*itr);
		armies.push_back(navy);
	}

	determineInvestments();

	nationalIdeas.clear();
	vector<Object*> activeIdeasObj = obj->getValue("active_idea_groups");	// the objects holding the national ideas
	if (activeIdeasObj.size() > 0)
	{
		vector<Object*> ideasObj = activeIdeasObj[0]->getLeaves();		// the individual idea objects
		for (vector<Object*>::iterator ideaItr = ideasObj.begin(); ideaItr != ideasObj.end(); ideaItr++)
		{
			nationalIdeas.insert(make_pair((*ideaItr)->getKey(), atoi((*ideaItr)->getLeaf().c_str())));
		}
	}

	vector<Object*> legitObj = obj->getValue("legitimacy");	// the object holding the legitimacy
	(legitObj.size() > 0) ?	legitimacy = atof(legitObj[0]->getLeaf().c_str()) : legitimacy = 1.0;

	colony = false;
	vector<Object*> colonyObj = obj->getValue("parent");	// the object handling the colony flag
	if (colonyObj.size() > 0)
	{
		colony = true;
	}

	customNation = false;
	vector<Object*> customNationObj = obj->getValue("custom_name");	// the object handling the custom name (if there is one)
	if (customNationObj.size() > 0)
	{
		customNation = true;
	}

	libertyDesire = 0.0;
	vector<Object*> colonialSubjectObj = obj->getValue("is_colonial_subject");
	if (colonialSubjectObj.size() > 0)
	{
		string overlord;
		vector<Object*> overlordObj = obj->getValue("overlord");
		if (overlordObj.size() > 0)
		{
			overlord = overlordObj[0]->getLeaf();
		}

		auto relationship = relations.find(overlord);
		if (relationship != relations.end())
		{
			string attitude = relationship->second->getAttitude();
			if (attitude == "attitude_rebellious")
			{
				libertyDesire = 95.0;
			}
			else if (attitude == "attitude_disloyal")
			{
				libertyDesire = 90.0;
			}
			else if (attitude == "attitude_disloyal_vassal")	// for pre-1.14 games
			{
				libertyDesire = 90.0;
			}
			else if (attitude == "attitude_outraged")
			{
				libertyDesire = 85.0;
			}
			else if (attitude == "atittude_rivalry")
			{
				libertyDesire = 80.0;
			}
			else if (attitude == "attitude_hostile")
			{
				libertyDesire = 75.0;
			}
			else if (attitude == "attitude_threatened")
			{
				libertyDesire = 65.0;
			}
			else if (attitude == "attitude_neutral")
			{
				libertyDesire = 50.0;
			}
			else if (attitude == "attitude_defensive")
			{
				libertyDesire = 35.0;
			}
			else if (attitude == "attitude_domineering")
			{
				libertyDesire = 20.0;
			}
			else if (attitude == "attitude_protective")
			{
				libertyDesire = 15.0;
			}
			else if (attitude == "attitude_allied")
			{
				libertyDesire = 10.0;
			}
			else if (attitude == "attitude_friendly")
			{
				libertyDesire = 10.0;
			}
			else if (attitude == "attitude_loyal")
			{
				libertyDesire = 5.0;
			}
			else if (attitude == "attitude_overlord")
			{
				libertyDesire = 5.0;
			}
			else if (attitude == "attitude_vassal")	// for pre-1.14 games
			{
				libertyDesire = 5.0;
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown attitude type " << attitude << " while setting liberty desire for " << tag;
				libertyDesire = 95.0;
			}
		}
	}

	customFlag.flag = "-1";
	vector<Object*> customFlagObj = obj->getValue("country_colors");
	if (customFlagObj.size() > 0)
	{
		vector<Object*> flag = customFlagObj[0]->getValue("flag");
		vector<Object*> emblem = customFlagObj[0]->getValue("subject_symbol_index");
		vector<Object*> colours = customFlagObj[0]->getValue("flag_colors");

		if (flag.size() > 0 && emblem.size() > 0 && colours.size() > 0)
		{
			customFlag.flag = to_string(1+stoi(flag[0]->getLeaf()));
			customFlag.emblem = stoi(emblem[0]->getLeaf())+1;
			
			vector<string> colourtokens = colours[0]->getTokens();
			customFlag.colours = std::make_tuple(stoi(colourtokens[0]), stoi(colourtokens[1]), stoi(colourtokens[2]));
		}
	}

	vector<Object*> revolutionaryFlagObj = obj->getValue("revolutionary_colors");
	if (revolutionaryFlagObj.size() > 0)
	{
		vector<string> colourtokens = revolutionaryFlagObj[0]->getTokens();
		revolutionaryTricolour = std::make_tuple(stoi(colourtokens[0]), stoi(colourtokens[1]), stoi(colourtokens[2]));
	}
	revolutionary = false;
}


void EU4Country::determineInvestments()
{
	armyInvestment = 32.0;
	navyInvestment = 32.0;
	commerceInvestment = 32.0;
	industryInvestment = 32.0;
	cultureInvestment = 32.0;

	for (auto idea: nationalIdeas)
	{
		armyInvestment += ideaEffectMapper::getArmyInvestmentFromIdea(idea.first, idea.second);
		commerceInvestment += ideaEffectMapper::getCommerceInvestmentFromIdea(idea.first, idea.second);
		cultureInvestment += ideaEffectMapper::getCultureInvestmentFromIdea(idea.first, idea.second);
		industryInvestment += ideaEffectMapper::getIndustryInvestmentFromIdea(idea.first, idea.second);
		navyInvestment += ideaEffectMapper::getNavyInvestmentFromIdea(idea.first, idea.second);
	}
}


void EU4Country::determineFlagsAndModifiers(Object* obj)
{
	flags.clear();
	vector<Object*> flagObject = obj->getValue("flags");	// the object holding the flags set for this country
	if (flagObject.size() > 0)
	{
		vector<Object*> flagObjects = flagObject[0]->getLeaves();	// the individual flag objects
		for (unsigned int i = 0; i < flagObjects.size(); i++)
		{
			flags[flagObjects[i]->getKey()] = true;
		}
	}
	flagObject = obj->getValue("hidden_flags");	// the object holding the hidden flags set for this country
	if (flagObject.size() > 0)
	{
		vector<Object*> flagObjects = flagObject[0]->getLeaves();	// the individual hidden flag objects
		for (unsigned int i = 0; i < flagObjects.size(); i++)
		{
			flags[flagObjects[i]->getKey()] = true;
		}
	}

	modifiers.clear();
	vector<Object*> modifierObject = obj->getValue("modifier");	// the object holding the modifiers for this country
	for (unsigned int i = 0; i < modifierObject.size(); i++)
	{
		vector<Object*> nameObject = modifierObject[i]->getLeaves();	// the individual modifier objects
		if (nameObject.size() > 0)
		{
			modifiers[nameObject[0]->getLeaf()] = true;
		}
	}
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
		const auto colorObj = obj->getValue("color");
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
	int retval = 0;	// the number of manus
	for (vector<EU4Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->hasBuilding("weapons"))		++retval;
		if ((*itr)->hasBuilding("wharf"))		++retval;
		if ((*itr)->hasBuilding("textile"))		++retval;
		if ((*itr)->hasBuilding("refinery"))	++retval;
	}
	return retval;
}


void EU4Country::eatCountry(EU4Country* target)
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
		for (unsigned int j = 0; j < target->provinces.size(); j++)
		{
			addProvince(target->provinces[j]);
			target->provinces[j]->setOwner(this);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());
		leaders.insert(leaders.end(), target->leaders.begin(), target->leaders.end());

		// rebalance prestige, badboy, inflation and techs from weighted average
		score						= myWeight * score						+ targetWeight * target->score;
		admTech					= myWeight * admTech						+ targetWeight * target->admTech;
		dipTech					= myWeight * dipTech						+ targetWeight * target->dipTech;
		milTech					= myWeight * milTech						+ targetWeight * target->milTech;
		armyInvestment			= myWeight * armyInvestment			+ targetWeight * target->armyInvestment;
		navyInvestment			= myWeight * navyInvestment			+ targetWeight * target->navyInvestment;
		commerceInvestment	= myWeight * commerceInvestment		+ targetWeight * target->commerceInvestment;
		industryInvestment	= myWeight * industryInvestment		+ targetWeight * target->industryInvestment;
		cultureInvestment		= myWeight * cultureInvestment		+ targetWeight * target->cultureInvestment;
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();

	LOG(LogLevel::Debug) << "Merged " << target->tag << " into " << tag;
}


void EU4Country::takeArmies(EU4Country* target)
{
	// acquire target's armies, navies, admirals, and generals
	armies.insert(armies.end(), target->armies.begin(), target->armies.end());
	leaders.insert(leaders.end(), target->leaders.begin(), target->leaders.end());
	target->clearArmies();
}


void EU4Country::clearArmies()
{
	armies.clear();
	leaders.clear();
}


bool EU4Country::cultureSurvivesInCores()
{
	for (auto core: cores)
	{
		if (core->getOwner() == NULL)
		{
			continue;
		}
		if (core->getOwner()->getPrimaryCulture() == primaryCulture)
		{
			continue;
		}

		if (core->getCulturePercent(primaryCulture) >= 0.5)
		{
			return true;
		}
	}

	return false;
}


string EU4Country::getName(const string& language) const
{
	if (!randomName.empty())
	{
		return randomName;
	}

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
	if (!randomName.empty())
	{
		return randomName;
	}

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

int EU4Country::numEmbracedInstitutions() const {
	int total = 0;
	for (unsigned int i = 0; i < embracedInstitutions.size(); i++) {
		if (embracedInstitutions[i]) total++;
	}
	return total;
}


void EU4Country::clearProvinces()
{
	provinces.clear();
}


void EU4Country::clearCores()
{
	cores.clear();
}