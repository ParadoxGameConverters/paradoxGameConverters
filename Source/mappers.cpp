#include "mappers.h"
#include "Log.h"



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