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


countryMapping initCountryMap(Object* obj)
{
	countryMapping mapping;
	countryMapping::iterator mapIter;
	//std::set<std::string> blanks;

	vector<Object*> leaves = obj->getLeaves();

	if (leaves.size() < 1)
	{
		log ("Error: No country mapping definitions loaded.\n");
		printf("Error: No country mapping definitions loaded.\n");
		return mapping;
	}

	vector<Object*> data = leaves[0]->getLeaves();

	for (unsigned int i = 0; i < data.size(); i++)
	{
		vector<string>	EU3Tags;
		string			V2Tag;

		vector<Object*> maps = data[i]->getLeaves();

		for (unsigned int j = 0; j < maps.size(); j++)
		{
			if (maps[j]->getKey().compare("eu3") == 0)
			{		 
				EU3Tags.push_back(maps[j]->getLeaf());
			}
			else if (maps[j]->getKey().compare("vic") == 0)
			{
				V2Tag = maps[j]->getLeaf();
			}
			else
			{
				log("Warning: unknown data while mapping countries.\n");
			}
		}

		for (unsigned int k = 0; k < EU3Tags.size(); k++)
		{
			mapping.insert(make_pair<string, string>(EU3Tags[k], V2Tag));
		}
	}

	return mapping;
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