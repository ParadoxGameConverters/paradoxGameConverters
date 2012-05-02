#include "mappers.h"
#include "Log.h"
#include "Configuration.h"
#include "Parsers\Object.h"
#include "CK2World\CK2Title.h"
#include "EU3World\EU3Country.h"



provinceMapping initProvinceMap(Object* obj)
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

	vector<Object*> data = leaves[0]->getLeaves();

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

	return mapping;
}


inverseProvinceMapping invertProvinceMap(provinceMapping provinceMap)
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


int initCountryMap(countryMapping& mapping, vector<CK2Title*> CK2Titles, vector<EU3Country*> EU3Countries, vector<string> blockedNations, Object* rulesObj)
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
		vector<EU3Country*>::iterator EU3CountryPos = EU3Countries.end();
		unsigned int distance = 0;
		for (vector<string>::reverse_iterator j = rEU3Tags.rbegin(); j != rEU3Tags.rend(); ++j)
		{
			++distance;
			for (vector<EU3Country*>::iterator k = EU3Countries.begin(); k != EU3Countries.end(); ++k)
			{
				if ( (*k)->getTag() == *j)
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
		mapping.insert(make_pair<CK2Title*, EU3Country*>(*CK2TitlesPos, *EU3CountryPos));
		log("Added map %s -> %s (#%d)\n", (*CK2TitlesPos)->getTitleString().c_str(), (*EU3CountryPos)->getTag().c_str() , distance);

		//remove tags from the lists
		CK2Titles.erase(CK2TitlesPos);
		EU3Countries.erase(EU3CountryPos);
	}

	for (unsigned int j = 0; j < blockedNations.size(); ++j)
	{
		for (vector<EU3Country*>::iterator i = EU3Countries.begin(); i != EU3Countries.end(); ++i)
		{
			if ((*i)->getTag() == blockedNations[j])
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
			vector<EU3Country*>::iterator EU3TagPos = EU3Countries.begin();
			mapping.insert(make_pair<CK2Title*, EU3Country*>(*CK2TitlesPos, *EU3TagPos));
			log("Added map %s -> %s (fallback)\n", (*CK2TitlesPos)->getTitleString().c_str(), (*EU3TagPos)->getTag().c_str());

			//remove tags from the lists
			CK2Titles.erase(CK2TitlesPos);
			EU3Countries.erase(EU3TagPos);
		}
	}

	return CK2Titles.size();
}