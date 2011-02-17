// ****************************************************************************
// *																								  *
// *			  EU3 to Victoria 2 conversion project							*
// *																								  *
// ****************************************************************************


#include "stdafx.h"
#include "Mapper.h"
#include "Log.h"

#include <algorithm>


provinceMapping initProvinceMap(Object* obj)
{
	provinceMapping mapping;
	provinceMapping::iterator mapIter;
	//std::set<std::string> blanks;

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
		vector<int> EU3nums;
		vector<int> V2nums;

		vector<Object*> euMaps = data[i]->getLeaves();

		for (unsigned int j = 0; j < euMaps.size(); j++)
		{
			if (euMaps[j]->getKey().compare("eu3") == 0)
			{
				EU3nums.push_back(atoi(euMaps[j]->getLeaf().c_str()));
			}
			else if (euMaps[j]->getKey().compare("vic") == 0)
			{
				V2nums.push_back(atoi(euMaps[j]->getLeaf().c_str()));
			}
			else
			{
				log("Warning: unknown data while mapping provinces.\n");
			}
		}

		for (unsigned int k = 0; k < V2nums.size(); k++)
		{
			pair< int, vector<int> > insertMe;
			insertMe.first = V2nums[k];
			insertMe.second = EU3nums;
			mapping.insert(insertMe);
		}
	}

	return mapping;
}


countryMapping initCountryMap(vector<string> EU3Tags, vector<string> V2Tags, Object* rulesObj)
{
	countryMapping mapping;
	countryMapping::iterator mapIter;

	vector<Object*>		leaves = rulesObj->getLeaves();

	if (leaves.size() < 1)
	{
		log ("Error: No country mapping definitions loaded.\n");
		printf("Error: No country mapping definitions loaded.\n");
		return mapping;
	}

	vector<Object*> rules = leaves[0]->getLeaves();

	for (unsigned int i = 0; i < rules.size(); ++i)
	{
		vector<Object*> rule = rules[i]->getLeaves();
		string			rEU3Tag;
		vector<string>	rV2Tags;
		for (unsigned int j = 0; j < rule.size(); j++)
		{
			if (rule[j]->getKey().compare("eu3") == 0)
			{		 
				rEU3Tag = rule[j]->getLeaf();
			}
			else if (rule[j]->getKey().compare("vic") == 0)
			{
				rV2Tags.push_back(rule[j]->getLeaf());
			}
			else
			{
				log("Warning: unknown data while mapping countries.\n");
			}
		}

		//find EU3 tag from the rule
		vector<string>::iterator EU3TagPos = EU3Tags.end();
		for (vector<string>::iterator j = EU3Tags.begin(); j != EU3Tags.end(); j++)
		{
			if (*j == rEU3Tag)
			{
				EU3TagPos = j;
				break;
			}
		}
		if (EU3TagPos == EU3Tags.end())
		{
			continue;
		}

		//find V2 tag from the rule
		vector<string>::iterator V2TagPos = V2Tags.end();
		for (unsigned int j = 0; j < rV2Tags.size(); j++)
		{
			for (vector<string>::iterator k = V2Tags.begin(); k != V2Tags.end(); k++)
			{
				if (*k == rV2Tags[j])
				{
					V2TagPos = k;
					break;
				}
			}
		}
		if (V2TagPos == V2Tags.end())
		{
			continue;
		}

		//add the mapping
		mapping.insert(make_pair<string, string>(*EU3TagPos, *V2TagPos));

		//remove tags from the lists
		EU3Tags.erase(EU3TagPos);
		V2Tags.erase(V2TagPos);
	}

	while ( (EU3Tags.size() > 0) && (V2Tags.size() > 0) )
	{
		vector<string>::iterator V2TagPos = V2Tags.begin();
		vector<string>::iterator EU3TagPos = EU3Tags.begin();
		mapping.insert(make_pair<string, string>(*EU3TagPos, *V2TagPos));

		//remove tags from the lists
		EU3Tags.erase(EU3TagPos);
		V2Tags.erase(V2TagPos);
	}

	return mapping;
}


vector<string> getEU3Tags(EU3World srcWorld)
{
	vector<EU3Country>	EU3Countries = srcWorld.getCountries();
	vector<string>			EU3Tags;

	for (unsigned int i = 0; i < EU3Countries.size(); i++)
	{
		EU3Tags.push_back(EU3Countries[i].getTag());
	}

	return EU3Tags;
}


stateMapping initStateMap(Object* obj)
{
	stateMapping stateMap;
	vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		vector<string> provinces = leaves[i]->getTokens();
		vector<int>		neighbors;

		for (unsigned int j = 0; j < provinces.size(); j++)
		{
			neighbors.push_back(atoi(provinces[j].c_str()));
		}
		for (unsigned int j = 0; j < neighbors.size(); j++)
		{
			stateMap.insert(make_pair<int, vector<int> >(neighbors[j], neighbors));
		}
	}

	return stateMap;
}