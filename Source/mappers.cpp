#include "mappers.h"
#include "Log.h"
#include "Configuration.h"
#include "Parsers\Object.h"
#include "CK2World\CK2Version.h"
#include "CK2World\CK2Title.h"
#include "CK2World\CK2Province.h"
#include "CK2World\CK2Barony.h"
#include "CK2World\CK2Character.h"
#include "EU3World\EU3Country.h"



provinceMapping initProvinceMap(Object* obj, CK2Version* version)
{
	provinceMapping mapping;
	provinceMapping::iterator mapIter;

	vector<Object*> leaves = obj->getLeaves();

	if (leaves.size() < 1)
	{
		log ("Error: No province mapping definitions loaded.\n");
		printf("Error: No province mapping definitions loaded.\n");
		return mapping;
	}

	for (vector<Object*>::iterator itr = leaves.begin(); itr < leaves.end(); itr++)
	{
		if (  CK2Version( (*itr)->getKey() ) > *version  )
		{
			continue;
		}
		vector<Object*> data = (*itr)->getLeaves();

		for (unsigned int i = 0; i < data.size(); i++)
		{
			vector<int> CK2nums;
			vector<int> EU3nums;

			vector<Object*> maps = data[i]->getLeaves();

			for (unsigned int j = 0; j < maps.size(); j++)
			{
				if (maps[j]->getKey().compare("ck2") == 0)
				{
					CK2nums.push_back(atoi(maps[j]->getLeaf().c_str()));
				}
				else if (maps[j]->getKey().compare("eu3") == 0)
				{
					EU3nums.push_back(atoi(maps[j]->getLeaf().c_str()));
				}
				else
				{
					log("Warning: unknown data while mapping provinces.\n");
				}
			}

			if (CK2nums.size() == 0)
			{
				CK2nums.push_back(0);
			}

			for (unsigned int k = 0; k < EU3nums.size(); k++)
			{
				pair< int, vector<int> > insertMe;
				insertMe.first = EU3nums[k];
				insertMe.second = CK2nums;
				mapping.insert(insertMe);
			}
		}
	}

	return mapping;
}


inverseProvinceMapping invertProvinceMap(provinceMapping& provinceMap)
{
	inverseProvinceMapping retval;
	for (provinceMapping::iterator j = provinceMap.begin(); j != provinceMap.end(); j++)
	{
		for (unsigned int k = 0; k < j->second.size(); k++)
		{
			retval[j->second[k]].push_back(j->first);
		}
	}
	return retval;
}


vector<string> processBlockedNations(Object* obj)
{
	vector<string> blockedNations;

	vector<Object*> leaves = obj->getLeaves();
	if (leaves.size() < 1)
	{
		return blockedNations;
	}

	vector<Object*> nations = leaves[0]->getLeaves();
	for (unsigned int i = 0; i < nations.size(); i++)
	{
		blockedNations.push_back(nations[i]->getLeaf());
	}

	return blockedNations;
}


int initCountryMap(countryMapping& mapping, vector<CK2Title*>& CK2Titles, map<string, EU3Country*>& EU3Countries, vector<string>& blockedNations, Object* rulesObj)
{
	mapping.clear();
	countryMapping::iterator mapIter;

	// get rules
	vector<Object*>		leaves = rulesObj->getLeaves();
	if (leaves.size() < 1)
	{
		log ("Error: No country mapping definitions loaded.\n");
		printf("Error: No country mapping definitions loaded.\n");
		return -1;
	}
	vector<Object*> rules = leaves[0]->getLeaves();

	for (unsigned int i = 0; i < rules.size(); ++i)
	{
		vector<Object*> rule = rules[i]->getLeaves();
		string			rCK2Title;
		vector<string>	rEU3Tags;
		for (unsigned int j = 0; j < rule.size(); j++)
		{
			if (rule[j]->getKey().compare("CK2") == 0)
			{		 
				rCK2Title = rule[j]->getLeaf();
			}
			else if (rule[j]->getKey().compare("EU3") == 0)
			{
				rEU3Tags.push_back(rule[j]->getLeaf());
			}
			else
			{
				log("Warning: unknown data while mapping countries.\n");
			}
		}

		//find CK2 title from the rule
		vector<CK2Title*>::iterator CK2TitlesPos = CK2Titles.end();
		for (vector<CK2Title*>::iterator j = CK2Titles.begin(); j != CK2Titles.end(); j++)
		{
			if ((*j)->getTitleString() == rCK2Title)
			{
				CK2TitlesPos = j;
				break;
			}
		}
		if (CK2TitlesPos == CK2Titles.end())
		{
			continue;
		}

		if (rCK2Title == Configuration::getHRETitle())
		{
			CK2Titles.erase(CK2TitlesPos);
			continue;
		}

		//find EU3 tag from the rule
		map<string, EU3Country*>::iterator EU3CountryPos = EU3Countries.end();
		unsigned int distance = 0;
		for (vector<string>::reverse_iterator j = rEU3Tags.rbegin(); j != rEU3Tags.rend(); ++j)
		{
			++distance;
			for (map<string, EU3Country*>::iterator k = EU3Countries.begin(); k != EU3Countries.end(); ++k)
			{
				if ( k->second->getTag() == *j)
				{
					EU3CountryPos = k;
					break;
				}
			}
			if (EU3CountryPos != EU3Countries.end())
			{
				break;
			}
		}
		if (EU3CountryPos == EU3Countries.end())
		{
			continue;
		}

		//add the mapping
		mapping.insert( make_pair(*CK2TitlesPos, EU3CountryPos->second) );
		log("	Added map %s -> %s (#%d)\n", (*CK2TitlesPos)->getTitleString().c_str(), EU3CountryPos->second->getTag().c_str() , distance);

		//remove tags from the lists
		CK2Titles.erase(CK2TitlesPos);
		EU3Countries.erase(EU3CountryPos);
	}

	for (unsigned int j = 0; j < blockedNations.size(); ++j)
	{
		for (map<string, EU3Country*>::iterator i = EU3Countries.begin(); i != EU3Countries.end(); ++i)
		{
			if (i->second->getTag() == blockedNations[j])
			{
				EU3Countries.erase(i);
				break;
			}
		}
	}

	while ( (CK2Titles.size() > 0) && (EU3Countries.size() > 0) )
	{
		vector<CK2Title*>::iterator CK2TitlesPos = CK2Titles.begin();
		if ((*CK2TitlesPos)->getTitleString() == "e_rebels")
		{
			//mapping.insert(make_pair<string, string>(*CK2TitlesPos, "REB")); // TODO: map rebels nation
			CK2Titles.erase(CK2TitlesPos);
		}
		else if ((*CK2TitlesPos)->getTitleString() == "e_pirates")
		{
			//mapping.insert(make_pair<string, string>(*CK2TitlesPos, "PIR")); // TODO: map pirates nation
			CK2Titles.erase(CK2TitlesPos);
		}
		else if ((*CK2TitlesPos)->getTitleString() == Configuration::getHRETitle())
		{
			CK2Titles.erase(CK2TitlesPos);
		}
		else
		{
			map<string, EU3Country*>::iterator EU3TagPos = EU3Countries.begin();
			mapping.insert( make_pair(*CK2TitlesPos, EU3TagPos->second) );
			log("	Added map %s -> %s (fallback)\n", (*CK2TitlesPos)->getTitleString().c_str(), EU3TagPos->second->getTag().c_str());

			//remove tags from the lists
			CK2Titles.erase(CK2TitlesPos);
			EU3Countries.erase(EU3TagPos);
		}
	}

	return CK2Titles.size();
}


cultureMapping initCultureMap(Object* obj) // TODO: consider cleaning up the distinguishers
{
	cultureMapping cultureMap;
	vector<Object*> links = obj->getLeaves();

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>			cultures	= (*i)->getLeaves();

		vector<string>				srcCultures;
		string						dstCulture;
		vector< distinguisher > distinguishers;
		for (vector<Object*>::iterator j = cultures.begin(); j != cultures.end(); j++)
		{
			if ( (*j)->getKey() == "eu3" )
			{
				dstCulture = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "ck2" )
			{
				srcCultures.push_back( (*j)->getLeaf() );
			}
			if ( (*j)->getKey() == "de_jure" )
			{
				distinguisher newD;
				newD.first	= DTDeJure;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "kingdom_culture" )
			{
				distinguisher newD;
				newD.first	= DTKingdomCulture;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "religion" )
			{
				distinguisher newD;
				newD.first	= DTReligion;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "hre_member" )
			{
				distinguisher newD;
				newD.first	= DTHREMember;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
		}

		for (vector<string>::iterator j = srcCultures.begin(); j != srcCultures.end(); j++)
		{
			cultureStruct rule;
			rule.srcCulture		= (*j);
			rule.dstCulture		= dstCulture;
			rule.distinguishers	= distinguishers;
			cultureMap.push_back(rule);
		}
	}

	return cultureMap;
}


string determineEU3Culture(string CK2Culture, const cultureMapping& cultureMap, const CK2Province* srcProvince)
{
	if (CK2Culture[0] == '"')
	{
		CK2Culture = CK2Culture.substr(1, CK2Culture.size() - 2);
	}
	for (cultureMapping::const_iterator itr = cultureMap.begin(); itr < cultureMap.end(); itr++)
	{
		if (itr->srcCulture != CK2Culture)
		{
			continue;
		}

		bool matchConditions = true;
		for (vector<distinguisher>::const_iterator DTItr = itr->distinguishers.begin(); DTItr < itr->distinguishers.end(); DTItr++)
		{
			switch (DTItr->first)
			{
				case DTDeJure:
					{
						bool subCondition = false;
						CK2Title* title = srcProvince->getBaronies()[0]->getTitle()->getLiege();
						while(title != NULL)
						{
							if (title->getTitleString() == DTItr->second)
							{
								subCondition = true;
								break;
							}
							title = title->getDeJureLiege();
						}
						matchConditions = subCondition;
					}
					break;
				case DTKingdomCulture:
					{
						CK2Title* kingdomTitle = srcProvince->getBaronies()[0]->getTitle();
						while (!kingdomTitle->isIndependent())
						{
							kingdomTitle = kingdomTitle->getLiege();
						}
						if (kingdomTitle->getHolder()->getCulture() != DTItr->second)
						{
							matchConditions = false;
						}
					}
					break;
				case DTReligion:
					if (srcProvince->getReligion() != DTItr->second)
					{
						matchConditions = false;
					}
					break;
				case DTHREMember:
					{
						bool subCondition = false;
						CK2Title* kingdomTitle = srcProvince->getBaronies()[0]->getTitle();
						while (!kingdomTitle->isIndependent())
						{
							if (kingdomTitle->getLiegeString() == "e_hre")
							{
								subCondition = true;
								break;
							}
							kingdomTitle = kingdomTitle->getLiege();
						}
						matchConditions = subCondition;
					}
					break;
			}
		}
		if (!matchConditions)
		{
			continue;
		}
		return itr->dstCulture;
	}

	return "";
}


void addCultureGroupMappings(Object* obj, cultureGroupMapping& map)
{
	vector<Object*> groups = obj->getLeaves();
	for (vector<Object*>::iterator groupsItr = groups.begin(); groupsItr < groups.end(); groupsItr++)
	{
		vector<Object*> cultures = (*groupsItr)->getLeaves();
		for (vector<Object*>::iterator culturesItr = cultures.begin(); culturesItr < cultures.end(); culturesItr++)
		{
			map.insert( make_pair((*culturesItr)->getKey(), (*groupsItr)->getKey()) );
		}
	}
}


religionMapping initReligionMap(Object* obj)
{
	religionMapping religionMap;
	vector<Object*> links = obj->getLeaves();

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>			link	= (*i)->getLeaves();

		string srcReligion;
		string dstReligion;
		for (vector<Object*>::iterator j = link.begin(); j != link.end(); j++)
		{
			if ( (*j)->getKey() == "eu3" )
			{
				dstReligion = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "ck2" )
			{
				srcReligion = (*j)->getLeaf();
			}
		}

		religionMap.insert( make_pair(srcReligion, dstReligion) );
	}

	return religionMap;
}


void addReligionGroupMappings(Object* obj, religionGroupMapping& map)
{
	vector<Object*> groups = obj->getLeaves();
	for (vector<Object*>::iterator groupsItr = groups.begin(); groupsItr < groups.end(); groupsItr++)
	{
		vector<Object*> religions = (*groupsItr)->getLeaves();
		for (vector<Object*>::iterator religionsItr = religions.begin(); religionsItr < religions.end(); religionsItr++)
		{
			map.insert( make_pair((*religionsItr)->getKey(), (*groupsItr)->getKey()) );
		}
	}
}