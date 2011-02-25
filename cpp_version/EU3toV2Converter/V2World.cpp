#include "V2World.h"

void V2World::init(Object* obj)
{
	std::string key;	
	std::vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
		{
			V2Province province;
			province.init(leaves[i]);
			provinces.push_back(province);
		}
	}

}


void V2World::convertCountries(EU3World sourceWorld, countryMapping countryMap)
{
	vector<EU3Country> sourceCountries = sourceWorld.getCountries();
	for (unsigned int i = 0; i < sourceCountries.size(); i++)
	{
		V2Country newCountry;
		countryMapping::iterator iter;
		iter = countryMap.find(sourceCountries[i].getTag());
		if (iter != countryMap.end())
		{
			newCountry.init(iter->second.c_str(), &(sourceCountries[i]));
			log("Checking pointers, old capital: %d\n", sourceCountries[i].getCapital());
			log("	Checking pointers, old capital: %d\n", newCountry.getSourceCountry()->getCapital());
		}
		else
		{
			log("Error: Could not convert EU3 tag %s to V2.\n", sourceCountries[i].getTag().c_str());
			printf("Error: Could not convert EU3 tag %s to V2.\n", sourceCountries[i].getTag().c_str());
			newCountry.init("", &sourceCountries[i]);
		}

		countries.push_back(newCountry);
		log("		Checking pointers, old capital: %d\n", countries[i].getSourceCountry()->getCapital()); 
	}

	log("Reviewing capitals (again)\n");
	for (unsigned int j = 0; j < countries.size(); j++)
	{
		log("	Old capital: %d\n", countries[j].getSourceCountry()->getCapital());
	}
}



void V2World::convertProvinces(EU3World sourceWorld, provinceMapping provMap, countryMapping contMap)
{
	log("Checking capitals before province conversion.\n");
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		log("	capital = %d\n", countries[i].getSourceCountry()->getCapital());
	}

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

	log("Checking capitals after province conversion.\n");
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		log("	capital = %d\n", countries[i].getSourceCountry()->getCapital());
	}
}


void V2World::convertCapitals(provinceMapping provinceMap)
{
	log("Checking capitals before capital conversion.\n");
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		log("	capital = %d\n", countries[i].getSourceCountry()->getCapital());
	}

	log("Converting Capitals:\n");
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		int oldCapital = countries[i].getSourceCountry()->getCapital();
		log("	old capital: %d", countries[i].getSourceCountry()->getCapital());

		if (oldCapital > 0)
		{
			int newCapital = provinceMap.find(oldCapital)->second[0];
			countries[i].setCapital(newCapital);
			log("	new capital: %d\n", newCapital);
		}
		else
		{
			log("\n");
		}
	}
}



void V2World::setupStates(stateMapping stateMap)
{
	vector<V2Province> unassignedProvs;
	unassignedProvs = provinces;

	vector<V2Province>::iterator iter;
	while(unassignedProvs.size() > 0)
	{
		iter = unassignedProvs.begin();
		int		provId	= iter->getNum();
		string	owner		= iter->getOwner();
		unassignedProvs.erase(iter);

		if (owner == "")
		{
			continue;
		}

		V2State newState(provId);
		vector<int> neighbors = stateMap[provId];

		for (unsigned int i = 0; i < neighbors.size(); i++)
		{
			for(iter = unassignedProvs.begin(); iter != unassignedProvs.end(); iter++)
			{
				if (iter->getNum() == neighbors[i])
				{
					if(iter->getOwner() == owner)
					{
						newState.addProvince(*iter);
						unassignedProvs.erase(iter);
					}
				}
			}
		}

		for (vector<V2Country>::iterator iter2 = countries.begin(); iter2 != countries.end(); iter2++)
		{
			if (iter2->getTag() == owner)
			{
				iter2->addState(newState);
			}
		}
	}
}


void V2World::output(FILE* output)
{
	outputHeader(output);
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		provinces[i].output(output);
	}
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		countries[i].output(output);
	}
}
