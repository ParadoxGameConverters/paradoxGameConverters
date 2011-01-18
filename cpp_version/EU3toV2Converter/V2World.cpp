#include "V2World.h"

void V2World::init(Object* obj)
{
	std::string key;	
	std::vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0 && atoi(key.c_str()) < 2000)
		{
			V2Province province;
			province.init(leaves[i]);
			provinces.push_back(province);
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) && 
					(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
					(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
					(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				  )
		{
			V2Country country;
			country.init(leaves[i]);
			countries.push_back(country);
		}
	}
}


void V2World::convertProvinces(EU3World sourceWorld, provinceMapping provMap, countryMapping contMap)
{
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		int destNum					= provinces[i].getNum();
		vector<int> sourceNums	= provMap[destNum];
		if (sourceNums.size() == 0)
		{
			log("Error: no source for V2 province %d\n", destNum);
			//provinces[i].setOwner("\"ERR\"");
		}
		else
		{
			string oldOwner = sourceWorld.getProvince(sourceNums[0])->getOwner();
			if (oldOwner != "")
			{
				countryMapping::iterator iter = contMap.find(oldOwner);
				if (iter == contMap.end())
				{
					log("Error: Could not map provinces owned by %s.\n", oldOwner.c_str());
				}
				else
				{
					provinces[i].setOwner(iter->second);
				}
			}
		}
	}
}