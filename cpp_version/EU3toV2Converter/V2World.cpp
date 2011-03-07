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


void V2World::addPotentialCountries(ifstream &countriesMapping)
{
	while (!countriesMapping.eof())
	{
		string line;
		getline(countriesMapping, line);

		if ( (line[0] == '#') | (line.size() < 3) )
		{
			continue;
		}
		
		string tag;
		tag = line.substr(0, 3);
		if (tag == "REB")
		{
			continue;
		}

		string countryFile;
		int start	= line.find_first_of('/');
		int size		= line.find_last_of('\"') - start;
		countryFile	= line.substr(start, size);

		V2Country newCountry;
		newCountry.init(tag, countryFile);
		potentialCountries.push_back(newCountry);
	}
}


vector<string> V2World::getPotentialTags()
{
	vector<string> tagList;
	for (unsigned int i = 0; i < potentialCountries.size(); i++)
	{
		tagList.push_back(potentialCountries[i].getTag());
	}
	return tagList;
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
			for(unsigned int j = 0; j < potentialCountries.size(); j++)
			{
				if (potentialCountries[j].getTag() == iter->second.c_str())
				{
					newCountry = potentialCountries[j];
					newCountry.setSourceCountryIndex(i);
				}
			}
		}
		else
		{
			log("Error: Could not convert EU3 tag %s to V2.\n", sourceCountries[i].getTag().c_str());
			printf("Error: Could not convert EU3 tag %s to V2.\n", sourceCountries[i].getTag().c_str());
			newCountry.init("", "");
		}

		countries.push_back(newCountry);
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
			EU3Province* oldProvince	= sourceWorld.getProvince(sourceNums[0]);
			string oldOwner				= oldProvince->getOwner();
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
					provinces[i].setColonial(oldProvince->isColony());
				}
			}
		}
	}
}


void V2World::convertCapitals(EU3World sourceWorld, provinceMapping provinceMap)
{
	log("Converting Capitals:\n");
	vector<EU3Country> oldCountries = sourceWorld.getCountries();
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		int oldCapital = oldCountries[countries[i].getSourceCountryIndex()].getCapital();
		log("	old capital: %4d", oldCapital);
		countries[i].setCapital(0);
		if (oldCapital > 0)
		{
			for (provinceMapping::iterator j = provinceMap.begin(); j != provinceMap.end(); j++)
			{
				for (unsigned int k = 0; k < j->second.size(); k++)
				{
					if (j->second[k] == oldCapital)
					{
						int newCapital = j->first;
						countries[i].setCapital(newCapital);
						log("	new capital: %d\n", newCapital);
						j = provinceMap.end();
						break;
					}
				}
			}
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

	int stateId = 0;
	vector<V2Province>::iterator iter;
	while(unassignedProvs.size() > 0)
	{
		log("Creating new state (#%d).\n", stateId);
		iter = unassignedProvs.begin();
		int		provId	= iter->getNum();
		string	owner		= iter->getOwner();

		if (owner == "")
		{
			log("Province %d has no owner, done creating state.\n", iter->getNum());
			unassignedProvs.erase(iter);
			continue;
		}

		V2State newState(stateId);
		stateId++;
		log("\tAdding initial province (%d) to state.\n", iter->getNum());
		newState.addProvince(*iter);
		vector<int> neighbors	= stateMap[provId];
		bool colonial				= iter->isColonial();
		newState.setColonial(colonial);
		unassignedProvs.erase(iter);

		for (unsigned int i = 0; i < neighbors.size(); i++)
		{
			for(iter = unassignedProvs.begin(); iter != unassignedProvs.end(); iter++)
			{
				if (iter->getNum() == neighbors[i])
				{
					if(iter->getOwner() == owner)
					{
						if ( (iter->isColonial()) == colonial)
						{
							log("\tAdding province %d to state.\n", iter->getNum());
							newState.addProvince(*iter);
							unassignedProvs.erase(iter);
						}
					}
				}
			}
		}

		for (vector<V2Country>::iterator iter2 = countries.begin(); iter2 != countries.end(); iter2++)
		{
			if (iter2->getTag() == owner)
			{
				log("Adding state to %s.\n", iter2->getTag().c_str());
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
