// ****************************************************************************
// *                                                                          *
// *		     Victoria 2 to HoI3 conversion project                        *
// *                                                                          *
// ****************************************************************************


#include "Mapper.h"
#include "Log.h"
#include <algorithm>


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
		vector<int> V2nums;
		vector<int> HoI3nums;

		vector<Object*> vicMaps = data[i]->getLeaves();

		for (unsigned int j = 0; j < vicMaps.size(); j++)
		{
			if (vicMaps[j]->getKey().compare("vic") == 0)
			{
				V2nums.push_back(atoi(vicMaps[j]->getLeaf().c_str()));
			}
			else if (vicMaps[j]->getKey().compare("hoi") == 0)
			{
				HoI3nums.push_back(atoi(vicMaps[j]->getLeaf().c_str()));
			}
			else
			{
				log("Warning: unknown data while mapping provinces.\n");
			}
		}

		if (V2nums.size() == 0)
		{
			V2nums.push_back(0);
		}

		for (unsigned int k = 0; k < HoI3nums.size(); k++)
		{
			pair< int, vector<int> > insertMe;
			insertMe.first = HoI3nums[k];
			insertMe.second = V2nums;
			mapping.insert(insertMe);
		}
	}

	return mapping;
}


// invert the sense of a province map.  makes army conversion tolerable.
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


static const vector<int> empty_vec;
const vector<int>& getHoI3ProvinceNums(const inverseProvinceMapping& invProvMap, int v2ProvinceNum)
{
	inverseProvinceMapping::const_iterator itr = invProvMap.find(v2ProvinceNum);
	if (itr == invProvMap.end())
		return empty_vec;
	else
		return itr->second;
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


int initCountryMap(countryMapping& mapping, vector<string> V2Tags, vector<string> HoI3Tags, vector<string> blockedNations, Object* rulesObj)
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
		string			rV2Tag;
		vector<string>	rHoI3Tags;
		for (unsigned int j = 0; j < rule.size(); j++)
		{
			if (rule[j]->getKey().compare("vic") == 0)
			{		 
				rV2Tag = rule[j]->getLeaf();
			}
			else if (rule[j]->getKey().compare("hoi") == 0)
			{
				rHoI3Tags.push_back(rule[j]->getLeaf());
			}
			else
			{
				log("Warning: unknown data while mapping countries.\n");
			}
		}

		//find V2 tag from the rule
		vector<string>::iterator V2TagPos = V2Tags.end();
		for (vector<string>::iterator j = V2Tags.begin(); j != V2Tags.end(); j++)
		{
			if (*j == rV2Tag)
			{
				V2TagPos = j;
				break;
			}
		}
		if (V2TagPos == V2Tags.end())
		{
			continue;
		}

		//find HoI3 tag from the rule
		vector<string>::iterator HoI3TagPos = HoI3Tags.end();
		unsigned int distance = 0;
		for (vector<string>::reverse_iterator j = rHoI3Tags.rbegin(); j != rHoI3Tags.rend(); ++j)
		{
			++distance;
			for (vector<string>::iterator k = HoI3Tags.begin(); k != HoI3Tags.end(); ++k)
			{
				if (*k == *j)
				{
					HoI3TagPos = k;
					break;
				}
			}
			if (HoI3TagPos != HoI3Tags.end())
			{
				break;
			}
		}
		if (HoI3TagPos == HoI3Tags.end())
		{
			continue;
		}

		//add the mapping
		mapping.insert(make_pair<string, string>(*V2TagPos, *HoI3TagPos));
		log("Added map %s -> %s (#%d)\n", V2TagPos->c_str(), HoI3TagPos->c_str(), distance);

		//remove tags from the lists
		V2Tags.erase(V2TagPos);
		HoI3Tags.erase(HoI3TagPos);
	}

	for (unsigned int j = 0; j < blockedNations.size(); ++j)
	{
		for (vector<string>::iterator i = HoI3Tags.begin(); i != HoI3Tags.end(); ++i)
		{
			if (*i == blockedNations[j])
			{
				HoI3Tags.erase(i);
				break;
			}
		}
	}

	while ( (V2Tags.size() > 0) && (HoI3Tags.size() > 0) )
	{
		vector<string>::iterator V2TagPos = V2Tags.begin();
		if ( (*V2TagPos == "REB") )
		{
			mapping.insert(make_pair<string, string>(*V2TagPos, "REB"));
			V2Tags.erase(V2TagPos);
		}
		else
		{
			vector<string>::iterator HoI3TagPos = HoI3Tags.begin();
			mapping.insert(make_pair<string, string>(*V2TagPos, *HoI3TagPos));
			log("Added map %s -> %s (fallback)\n", V2TagPos->c_str(), HoI3TagPos->c_str());

			//remove tags from the lists
			V2Tags.erase(V2TagPos);
			HoI3Tags.erase(HoI3TagPos);
		}
	}

	return V2Tags.size();
}


void mergeNations(V2World& world, Object* mergeObj)
{
	vector<Object*> rules = mergeObj->getValue("merge_nations");
	if (rules.size() < 0)
	{
		log("No nations have merging requested (skipping).\n");
		return;
	}

	rules = rules[0]->getLeaves();
	for (vector<Object*>::iterator itr = rules.begin(); itr != rules.end(); ++itr)
	{
		vector<Object*> thisMerge = (*itr)->getLeaves();
		string masterTag;
		vector<string> slaveTags;
		bool enabled = false;
		for (vector<Object*>::iterator jtr = thisMerge.begin(); jtr != thisMerge.end(); ++jtr)
		{
			if ((*jtr)->getKey() == "merge" && (*jtr)->getLeaf() == "yes")
				enabled = true;
			else if ((*jtr)->getKey() == "master")
				masterTag = (*jtr)->getLeaf();
			else if ((*jtr)->getKey() == "slave")
				slaveTags.push_back((*jtr)->getLeaf());
		}
		V2Country* master = world.getCountry(masterTag);
		if (enabled && master != NULL && slaveTags.size() > 0)
		{
			for (vector<string>::iterator sitr = slaveTags.begin(); sitr != slaveTags.end(); ++sitr)
			{
				master->eatCountry(world.getCountry(*sitr));
			}
		}
	}
}


void removeEmptyNations(V2World& world)
{
	vector<V2Country> countries = world.getCountries();

	vector<string> tagsToRemove;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		vector<V2Province*> provinces	= countries[i].getProvinces();
		vector<V2Province*> cores		= countries[i].getCores();
		if ( (provinces.size()) == 0 && (cores.size() == 0) )
		{
			tagsToRemove.push_back(countries[i].getTag());
		}
	}
	world.removeCountries(tagsToRemove);
}


vector<string> getV2Tags(V2World& srcWorld)
{
	vector<V2Country>	V2Countries = srcWorld.getCountries();
	vector<string>			V2Tags;

	for (unsigned int i = 0; i < V2Countries.size(); i++)
	{
		V2Tags.push_back(V2Countries[i].getTag());
	}

	return V2Tags;
}