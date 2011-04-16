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


void V2World::addPotentialCountries(ifstream &countriesMapping, string V2Loc)
{
	int partiesIndex = 0;
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

		string countryFileName;
		int start			= line.find_first_of('/');
		int size				= line.find_last_of('\"') - start;
		countryFileName	= line.substr(start, size);

		int oldPartiesIndex = partiesIndex;
		ifstream countryFile( (V2Loc + "\\common\\countries\\" + countryFileName).c_str());
		while (!countryFile.eof())
		{
			string line2;
			getline(countryFile, line2);
			if (line2 == "party = {")
			{
				partiesIndex++;
			}
		}
		vector<int> parties;
		for(int i = oldPartiesIndex; i < partiesIndex; i++)
		{
			parties.push_back(i);
		}

		if (tag == "REB")
		{
			continue;
		}
		V2Country newCountry;
		newCountry.init(tag, countryFileName, parties);
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


void V2World::convertCountries(EU3World sourceWorld, countryMapping countryMap, cultureMapping cultureMap)
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

					if ( (sourceCountries[i].getTechGroup() == "western") || (sourceCountries[i].getTechGroup() == "eastern") || (sourceCountries[i].getTechGroup() == "ottoman"))
					{
						newCountry.setcivilized(true);
					}
					else
					{
						newCountry.setcivilized(false);
					}
					
					string srcCulture = sourceCountries[i].getPrimaryCulture();
					if (srcCulture.size() > 0)
					{
						cultureMapping::iterator iter2 = cultureMap.find(srcCulture);
						if (iter2 != cultureMap.end())
						{
							newCountry.setPrimaryCulture(iter2->second);
						}
						else
						{
							log("No culture mapping defined for %s (%s -> %s)\n", srcCulture.c_str(), sourceCountries[i].getTag().c_str(), newCountry.getTag().c_str());
						}
					}
					
					vector<string> acceptedCultures = sourceCountries[i].getAcceptedCultures();
					for (unsigned int k = 0; k < acceptedCultures.size(); k++)
					{
						cultureMapping::iterator iter2 = cultureMap.find(acceptedCultures[k]);
						if (iter2 != cultureMap.end())
						{
							newCountry.addAcceptedCulture(iter2->second);
						}
						else
						{
							log("No culture mapping defined for %s (%s -> %s)\n", srcCulture.c_str(), sourceCountries[i].getTag().c_str(), newCountry.getTag().c_str());
						}
					}
				}
			}
		}
		else
		{
			log("Error: Could not convert EU3 tag %s to V2.\n", sourceCountries[i].getTag().c_str());
			printf("Error: Could not convert EU3 tag %s to V2.\n", sourceCountries[i].getTag().c_str());
			vector<int> empty;
			newCountry.init("", "", empty);
		}

		countries.push_back(newCountry);
	}
}



void V2World::convertProvinces(EU3World sourceWorld, provinceMapping provMap, countryMapping contMap, cultureMapping cultureMap)
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
	
					cultureMapping::iterator iter2 = cultureMap.find(oldProvince->getCulture());
					if (iter2 != cultureMap.end())
					{
						provinces[i].setCulture(iter2->second);
					}
					else
					{
						log("Error: Could not set culture for province %d\n", destNum);
						provinces[i].setCulture("");
					}
					sourceWorld.getCountry(oldOwner);
					provinces[i].createPops(oldProvince, sourceWorld.getCountry(oldOwner));
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


void V2World::convertTechs(EU3World sourceWorld)
{
	vector<EU3Country> sourceCountries = sourceWorld.getCountries();
	
	float oldLandMean;
	float landMean;
	float oldLandS = 0.0;
	float newLandS;
	float	highestLand;

	float oldNavalMean;
	float navalMean;
	float oldNavalS = 0.0;
	float newNavalS;
	float highestNaval;

	float oldTradeMean;
	float tradeMean;
	float oldTradeS = 0.0;
	float newTradeS;
	float highestTrade;

	float oldProductionMean;
	float productionMean;
	float oldProductionS = 0.0;
	float newProductionS;
	float highestProduction;

	float oldGovernmentMean;
	float governmentMean;
	float oldGovernmentS = 0.0;
	float newGovernmentS;
	float highestGovernment;

	int num = 2;

	highestLand			= oldLandMean			= landMean			= sourceCountries[0].getLandTech();
	highestNaval		= oldNavalMean			= navalMean			= sourceCountries[0].getNavalTech();
	highestTrade		= oldTradeMean			= tradeMean			= sourceCountries[0].getTradeTech();
	highestProduction	= oldProductionMean	= productionMean	= sourceCountries[0].getProductionTech();
	highestGovernment	= oldGovernmentMean	= governmentMean	= sourceCountries[0].getGovernmentTech();

	for (unsigned int i = 1; i < sourceCountries.size(); i++)
	{
		float newTech	= sourceCountries[i].getLandTech();
		landMean			= oldLandMean + ((newTech - oldLandMean) / num);
		newLandS			= oldLandS + ((newTech - oldLandMean) * (newTech - landMean));
		oldLandMean		= landMean; 
		oldLandS			= newLandS;
		if (newTech > highestLand)
		{
			highestLand = newTech;
		}

		newTech			= sourceCountries[i].getNavalTech();
		navalMean		= oldNavalMean + ((newTech - oldNavalMean) / num);
		newNavalS		= oldNavalS + ((newTech - oldNavalMean) * (newTech - navalMean));
		oldNavalMean	= navalMean; 
		oldNavalS		= newNavalS;
		if (newTech > highestNaval)
		{
			highestNaval = newTech;
		}

		newTech			= sourceCountries[i].getTradeTech();
		tradeMean		= oldTradeMean + ((newTech - oldTradeMean) / num);
		newTradeS		= oldTradeS + ((newTech - oldTradeMean) * (newTech - tradeMean));
		oldTradeMean	= tradeMean; 
		oldTradeS		= newTradeS;
		if (newTech > highestTrade)
		{
			highestTrade = newTech;
		}

		newTech				= sourceCountries[i].getProductionTech();
		productionMean		= oldProductionMean + ((newTech - oldProductionMean) / num);
		newProductionS		= oldProductionS + ((newTech - oldProductionMean) * (newTech - productionMean));
		oldProductionMean	= productionMean; 
		oldProductionS		= newProductionS;
		if (newTech > highestProduction)
		{
			highestProduction = newTech;
		}

		newTech				= sourceCountries[i].getGovernmentTech();
		governmentMean		= oldGovernmentMean + ((newTech - oldGovernmentMean) / num);
		newGovernmentS		= oldGovernmentS + ((newTech - oldGovernmentMean) * (newTech - governmentMean));
		oldGovernmentMean	= governmentMean; 
		oldGovernmentS		= newGovernmentS;
		if (newTech > highestGovernment)
		{
			highestGovernment = newTech;
		}

		num++;
	}

	float landStdDev			= sqrt( (num > 1) ? (newLandS/(num - 1)) : 0.0 );
	float navalStdDev			= sqrt( (num > 1) ? (newNavalS/(num - 1)) : 0.0 );
	float tradeStdDev			= sqrt( (num > 1) ? (newTradeS/(num - 1)) : 0.0 );
	float productionStdDev	= sqrt( (num > 1) ? (newProductionS/(num - 1)) : 0.0 );
	float governmentStdDev	= sqrt( (num > 1) ? (newGovernmentS/(num - 1)) : 0.0 );

	float landScale			= (5		* landStdDev)			/ (highestLand			- landMean);
	float navalScale			= (5		* navalStdDev)			/ (highestNaval		- navalMean);
	float tradeScale			= (5		* tradeStdDev)			/ (highestTrade		- tradeMean);
	float productionScale	= (4.5	* productionStdDev)	/ (highestProduction	- productionMean);
	float governmentScale	= (4.5	* governmentStdDev)	/ (highestGovernment	- governmentMean);

	for (unsigned int i = 0; i < countries.size(); i++)
	{
		int armyTech = (int)(landScale * (sourceCountries[countries[i].getSourceCountryIndex()].getLandTech() - landMean) / landStdDev) + 4.5;
		countries[i].setArmyTech(armyTech);
		log("%s has army tech of %d\n", countries[i].getTag().c_str(), armyTech);

		int navyTech = (int)(navalScale * (sourceCountries[countries[i].getSourceCountryIndex()].getNavalTech() - navalMean) / navalStdDev) + 5;
		countries[i].setNavyTech(navyTech);
		log("%s has navy tech of %d\n", countries[i].getTag().c_str(), navyTech);

		int commerceTech = (int)(tradeScale * (sourceCountries[countries[i].getSourceCountryIndex()].getTradeTech() - tradeMean) / tradeStdDev) + 5;
		countries[i].setCommerceTech(commerceTech);
		log("%s has commerce tech of %d\n", countries[i].getTag().c_str(), commerceTech);

		int industryTech = (int)(productionScale * (sourceCountries[countries[i].getSourceCountryIndex()].getProductionTech() - productionMean) / productionStdDev) + 5;
		countries[i].setIndustryTech(industryTech);
		log("%s has industry tech of %d\n", countries[i].getTag().c_str(), industryTech);

		int cultureTech = (int)(governmentScale * (sourceCountries[countries[i].getSourceCountryIndex()].getGovernmentTech() - governmentMean) / governmentStdDev) + 4.5;
		countries[i].setCultureTech(cultureTech);
		log("%s has culture tech of %d\n", countries[i].getTag().c_str(), cultureTech);
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
