// ****************************************************************************
// *																								  *
// *			  EU3 to Victoria 2 conversion project							*
// *																								  *
// ****************************************************************************

#include "stdafx.h"
#include "RulesetHandler.h"
#include "Log.h"



/*RulesetHandler* RulesetHandler::s_calculator = NULL;


RulesetHandler* RulesetHandler::Instance()
{
	if (NULL == s_calculator)
	{
		s_calculator = new RulesetHandler();
	}

	return s_calculator;
}


RulesetHandler::RulesetHandler()
{
}


RulesetHandler::~RulesetHandler()
{
}


void RulesetHandler::ProcessOrderLibertyEquality(World* world, std::map<std::string, std::string>& rules)
{
	std::string eqBonusName, libBonusName;  
	int maxEqCountries = 0;
	int maxLibCountries = 0;
	int curEqCountries = 0;
	int curLibCountries = 0;
	std::map<std::string, std::string>::iterator iter;
	std::map<int, int> eqCountryMap;
	std::map<int, int> libCountryMap;
	std::map<int, int>::reverse_iterator mapIter;
	std::ostringstream stream;

	if (world == NULL)
	{
		// Error
		return;
	}

	iter = rules.find("equality_bonus");
	if (iter != rules.end())
	{
		// Skip the $ at the front
		eqBonusName = (*iter).second.substr(1);
	}

	iter = rules.find("liberty_bonus");
	if (iter != rules.end())
	{
		// Skip the $ at the front
		libBonusName = (*iter).second.substr(1);
	}

	iter = rules.find("max_equality_countries");
	if (iter != rules.end())
	{
		maxEqCountries = atoi((*iter).second.c_str());
	}

	iter = rules.find("max_liberty_countries");
	if (iter != rules.end())
	{
		// Skip the $ at the front
		maxLibCountries = atoi((*iter).second.c_str());
	}

	std::vector<Country*> countries = world->GetAllExistingCountries();
	std::vector<std::string> ideas;

	for (unsigned int i = 0; i < countries.size(); i++)
	{
		ideas.push_back("\"nv_order\"");

		if (countries[i]->m_flags.GetFlagInt(eqBonusName) >= 0)
			eqCountryMap.insert(std::make_pair<int, int>(countries[i]->m_flags.GetFlagInt(eqBonusName), i));
		if (countries[i]->m_flags.GetFlagInt(libBonusName) >= 0)
			libCountryMap.insert(std::make_pair<int, int>(countries[i]->m_flags.GetFlagInt(libBonusName), i));
	}

	for (mapIter = eqCountryMap.rbegin(); mapIter != eqCountryMap.rend(); mapIter++)
	{
		ideas[(*mapIter).second] = "\"nv_equality\"";
		curEqCountries++;

		log("Country %s has equality value of %d and will be set to Equality NV.\n", countries[(*mapIter).second]->GetName().c_str(), (*mapIter).first);

		if (curEqCountries > maxEqCountries)
			break;
	}

	for (mapIter = libCountryMap.rbegin(); mapIter != libCountryMap.rend(); mapIter++)
	{
		// Skip equality countries
		if (ideas[(*mapIter).second].compare("order") != 0)
			continue;

		ideas[(*mapIter).second] = "\"nv_liberty\"";
		curLibCountries++;

		log("Country %s has liberty value of %d and will be set to Liberty NV.\n", countries[(*mapIter).second]->GetName().c_str(), (*mapIter).first);

		if (curLibCountries > maxLibCountries)
			break;
	}

	for (unsigned int i = 0; i < countries.size(); i++)
	{		
		countries[i]->SetProperty("nationalvalue", ideas[i]);

		log("Country %s now has national value: %s\n", countries[i]->GetName().c_str(), ideas[i].c_str());
	}
}*/