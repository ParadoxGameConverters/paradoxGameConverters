#include "V2World.h"
#include <fstream>
#include <io.h>
#include "Parsers/Parser.h"
#include "Log.h"
#include "tempFuncs.h"


void V2World::init(string V2Loc)
{
	// set province names and numbers
	ifstream read;
	read.open( (V2Loc + "\\map\\definition.csv").c_str() );
	if (!read.is_open())
	{
		log("Error: Could not open \\map\\definition.csv\n");
		printf("Error: Could not open \\map\\definition.csv\n");
		read.close();
		exit(1);
	}

	string line;
	getline(read, line);
	while (!read.eof())
	{
		getline(read, line);
		unsigned int delimiter = line.find_first_of(';');
		if (delimiter == 0 || delimiter == string::npos)
		{
			continue;
		}
		int provNum = atoi( line.substr(0, delimiter).c_str() );

		V2Province newProv;
		newProv.init(provNum);
		provinces.push_back(newProv);
	}
	read.close();

	// set province names
	read.open( (V2Loc + "\\localisation\\text.csv").c_str() );
	while (!read.eof())
	{
		getline(read, line);
		if (line.substr(0,4) == "PROV" && isdigit(line.c_str()[4]))
		{
			int position = line.find_first_of(';');
			int num = atoi( line.substr(4, position - 4).c_str() );
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			for (unsigned int i = 0; i < provinces.size(); i++)
			{
				if (provinces[i].getNum() == num)
				{
					provinces[i].setName(name);
				}
			}
		}
	}
	read.close();
	read.open( (V2Loc + "\\localisation\\1.1.csv").c_str() );
	while (!read.eof())
	{
		getline(read, line);
		if (line.substr(0,4) == "PROV" && isdigit(line.c_str()[4]))
		{
			int position = line.find_first_of(';');
			int num = atoi( line.substr(4, position - 4).c_str() );
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			for (unsigned int i = 0; i < provinces.size(); i++)
			{
				if (provinces[i].getNum() == num)
				{
					provinces[i].setName(name);
				}
			}
		}
	}
	read.close();
	read.open( (V2Loc + "\\localisation\\1.2.csv").c_str() );
	while (!read.eof())
	{
		getline(read, line);
		if (line.substr(0,4) == "PROV" && isdigit(line.c_str()[4]))
		{
			int position = line.find_first_of(';');
			int num = atoi( line.substr(4, position - 4).c_str() );
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			for (unsigned int i = 0; i < provinces.size(); i++)
			{
				if (provinces[i].getNum() == num)
				{
					provinces[i].setName(name);
				}
			}
		}
	}
	read.close();
	read.open( (V2Loc + "\\localisation\\beta1.csv").c_str() );
	while (!read.eof())
	{
		getline(read, line);
		if (line.substr(0,4) == "PROV" && isdigit(line.c_str()[4]))
		{
			int position = line.find_first_of(';');
			int num = atoi( line.substr(4, position - 4).c_str() );
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			for (unsigned int i = 0; i < provinces.size(); i++)
			{
				if (provinces[i].getNum() == num)
				{
					provinces[i].setName(name);
				}
			}
		}
	}
	read.close();
	read.open( (V2Loc + "\\localisation\\beta2.csv").c_str() );
	while (!read.eof())
	{
		getline(read, line);
		if (line.substr(0,4) == "PROV" && isdigit(line.c_str()[4]))
		{
			int position = line.find_first_of(';');
			int num = atoi( line.substr(4, position - 4).c_str() );
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			for (unsigned int i = 0; i < provinces.size(); i++)
			{
				if (provinces[i].getNum() == num)
				{
					provinces[i].setName(name);
				}
			}
		}
	}
	read.close();

	// set province rgo types and life ratings
	struct _finddata_t	provDirData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (V2Loc + "\\history\\provinces\\*").c_str(), &provDirData)) == -1L)
	{
		log("Could not open province directories.\n");
		return;
	}
	do
	{
		if (strcmp(provDirData.name, ".") == 0 || strcmp(provDirData.name, "..") == 0 )
		{
			continue;
		}
		struct _finddata_t	provFileData;
		intptr_t					fileListing2;
		if ( (fileListing2 = _findfirst( (V2Loc + "\\history\\provinces\\" + provDirData.name + "\\*").c_str(), &provFileData)) == -1L)
		{
			return;
		}
		do
		{
			if (strcmp(provFileData.name, ".") == 0 || strcmp(provFileData.name, "..") == 0 )
			{
				continue;
			}
			string filename(provFileData.name);
			int provNum = atoi( filename.substr(0, filename.find_first_of(' ')).c_str() );
			int delimiter = filename.find_last_of(' ');
			string provName = filename.substr(delimiter + 1, filename.find_first_of('.') - delimiter - 1);

			vector<V2Province>::iterator i;
			for(i = provinces.begin(); i != provinces.end(); i++)
			{
				if (i->getNum() == provNum)
				{
					read.open( (V2Loc + "\\history\\provinces\\" + provDirData.name + "\\" + provFileData.name).c_str(), ios_base::binary );
					vector<string> lines;
					while (!read.eof())
					{
						getline(read, line);
						int delimiter2 = 0;
						do
						{
							int delimiter3 = line.find_first_of('\r', delimiter2);
							if (delimiter3 == line.size() - 1)
							{
								lines.push_back(line.substr(delimiter2, line.size() - delimiter2 - 1));
								delimiter2 = string::npos;
							}
							else if (delimiter3 == string::npos)
							{
								lines.push_back(line);
								delimiter2 = string::npos;
							}
							else
							{
								lines.push_back(line.substr(delimiter2, delimiter3 - delimiter2));
								delimiter2 = delimiter3 + 1;
							}
						} while (delimiter2 != string::npos);
					}
					for (unsigned int j = 0; j < lines.size(); j++)
					{
						if(lines[j].substr(0, 14) == "trade_goods = ")
						{
							string type = lines[j].substr(14, lines[j].size() - 14);
							i->setRgoType(type);
						}
						if(lines[j].substr(0, 14) == "life_rating = ")
						{
							int rating = atoi( lines[j].substr(14, lines[j].size() - 14).c_str() );
							i->setLifeRating(rating);
						}
					}
					read.close();
					break;
				}
			}
			if (i == provinces.end())
			{
				log("	Error: could not find province %d to add rgo and liferating.\n", provNum);
			}


			//provinces.push_back(newProv);
		} while(_findnext(fileListing2, &provFileData) == 0);
		_findclose(fileListing2);
	} while(_findnext(fileListing, &provDirData) == 0);
	_findclose(fileListing);

	// set V2 basic population levels
	struct _finddata_t	popsFileData;
	if ( (fileListing = _findfirst( (V2Loc + "\\history\\pops\\1836.1.1\\*").c_str(), &popsFileData)) == -1L)
	{
		log("Could not open pops files.\n");
		return;
	}
	do
	{
		if (strcmp(popsFileData.name, ".") == 0 || strcmp(popsFileData.name, "..") == 0 )
		{
			continue;
		}

		Object*	obj2;				// generic object
		ifstream	read;				// ifstream for reading files
		initParser();
		obj2 = Parser::topLevel;
		read.open( (V2Loc + "\\history\\pops\\1836.1.1\\" + popsFileData.name).c_str() );
		if (!read.is_open())
		{
			log("Error: Could not open %s\n", popsFileData.name);
			printf("Error: Could not open %s\n", popsFileData.name);
			read.close();
			continue;
		}
		readFile(read);
		read.close();
	
		vector<Object*> leaves = obj2->getLeaves();
		for (unsigned int j = 0; j < leaves.size(); j++)
		{
			int provNum = atoi(leaves[j]->getKey().c_str());
			unsigned int k = 0;
			while (k < provinces.size() && provNum != provinces[k].getNum())
			{
				k++;
			}
			if (k == provinces.size())
			{
				log("Could not find province %d for original pops.\n", provNum);
				continue;
			}
			else
			{
				vector<Object*> pops = leaves[j]->getLeaves();
				for(unsigned int l = 0; l < pops.size(); l++)
				{
					V2Pop newPop;
					newPop.setType(pops[l]->getKey());
					newPop.setCulture(pops[l]->getLeaf("culture"));
					newPop.setReligion(pops[l]->getLeaf("religion"));
					newPop.setSize(atoi(pops[l]->getLeaf("size").c_str()));
					provinces[k].addOldPop(newPop);
				}
			}
		}
	} while(_findnext(fileListing, &popsFileData) == 0);
	_findclose(fileListing);
}


void V2World::addPotentialCountries(ifstream &countriesMapping, string V2Loc)
{
	int partiesIndex = 1;
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
		vector<int> parties;
		while (!countryFile.eof())
		{
			string line2;
			getline(countryFile, line2);
			if (line2 == "party = {")
			{
				getline(countryFile, line2);	// party name
				getline(countryFile, line2);	// start date
				int start2	= line2.find_first_of('=') + 2;
				int size2	= line2.size() - start2;
				line2 = line2.substr(start2, size2);

				int end = line2.find_first_of('.');
				line2 = line2.substr(0, end);  // now should be just the year

				if (atoi(line2.c_str()) <= 1836)
				{
					parties.push_back(partiesIndex);
				}

				partiesIndex++;
			}
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


void V2World::convertCountries(EU3World sourceWorld, countryMapping countryMap, cultureMapping cultureMap, religionMapping religionMap)
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

					string srcReligion = sourceCountries[i].getReligion();
					if (srcReligion.size() > 0)
					{
						religionMapping::iterator iter2 = religionMap.find(srcReligion);
						if (iter2 != religionMap.end())
						{
							newCountry.setReligion(iter2->second);
						}
						else
						{
							log("No religion mapping defined for %s (%s -> %s)\n", srcReligion.c_str(), sourceCountries[i].getTag().c_str(), newCountry.getTag().c_str());
						}
					}

					newCountry.setPrestige(sourceCountries[i].getPrestige() + 100);
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



void V2World::convertProvinces(EU3World sourceWorld, provinceMapping provMap, countryMapping contMap, cultureMapping cultureMap, religionMapping religionMap)
{
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		int destNum					= provinces[i].getNum();
		vector<int> sourceNums	= provMap[destNum];
		if (sourceNums.size() == 0)
		{
			log("Error: no source for %s (province #%d)\n", provinces[i].getName().c_str() , destNum);
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

					vector<string> oldCores = oldProvince->getCores();
					for(unsigned int j = 0; j < oldCores.size(); j++)
					{
						iter = contMap.find(oldCores[j]);
						if (iter != contMap.end())
						{
							provinces[i].addCore(iter->second);
						}
					}
	
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

					religionMapping::iterator iter3 = religionMap.find(oldProvince->getReligion());
					if (iter3 != religionMap.end())
					{
						provinces[i].setReligion(iter3->second);
					}
					else
					{
						log("Error: Could not set religion for province %d\n", destNum);
						provinces[i].setReligion("");
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
	vector<EU3Country> oldCountries = sourceWorld.getCountries();
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		int oldCapital = oldCountries[countries[i].getSourceCountryIndex()].getCapital();
		log("\n	EU3tag: %s	old capital: %4d", oldCountries[i].getTag().c_str(), oldCapital);
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
						log("	new capital: %d", newCapital);
						j = provinceMap.end();
						break;
					}
				}
			}
		}
	}
	log("\n");
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
	
	double oldLandMean;
	double landMean;
	double oldLandS = 0.0;
	double newLandS;
	double	highestLand;

	double oldNavalMean;
	double navalMean;
	double oldNavalS = 0.0;
	double newNavalS;
	double highestNaval;

	double oldTradeMean;
	double tradeMean;
	double oldTradeS = 0.0;
	double newTradeS;
	double highestTrade;

	double oldProductionMean;
	double productionMean;
	double oldProductionS = 0.0;
	double newProductionS;
	double highestProduction;

	double oldGovernmentMean;
	double governmentMean;
	double oldGovernmentS = 0.0;
	double newGovernmentS;
	double highestGovernment;

	int num = 2;

	highestLand			= oldLandMean			= landMean			= sourceCountries[0].getLandTech();
	highestNaval		= oldNavalMean			= navalMean			= sourceCountries[0].getNavalTech();
	highestTrade		= oldTradeMean			= tradeMean			= sourceCountries[0].getTradeTech();
	highestProduction	= oldProductionMean	= productionMean	= sourceCountries[0].getProductionTech();
	highestGovernment	= oldGovernmentMean	= governmentMean	= sourceCountries[0].getGovernmentTech();

	for (unsigned int i = 1; i < sourceCountries.size(); i++)
	{
		double newTech	= sourceCountries[i].getLandTech();
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

	double landStdDev			= sqrt( (num > 1) ? (newLandS/(num - 1)) : 0.0 );
	double navalStdDev		= sqrt( (num > 1) ? (newNavalS/(num - 1)) : 0.0 );
	double tradeStdDev		= sqrt( (num > 1) ? (newTradeS/(num - 1)) : 0.0 );
	double productionStdDev	= sqrt( (num > 1) ? (newProductionS/(num - 1)) : 0.0 );
	double governmentStdDev	= sqrt( (num > 1) ? (newGovernmentS/(num - 1)) : 0.0 );

	double landScale			= (5		* landStdDev)			/ (highestLand			- landMean);
	double navalScale			= (5		* navalStdDev)			/ (highestNaval		- navalMean);
	double tradeScale			= (5		* tradeStdDev)			/ (highestTrade		- tradeMean);
	double productionScale	= (4.5	* productionStdDev)	/ (highestProduction	- productionMean);
	double governmentScale	= (4.5	* governmentStdDev)	/ (highestGovernment	- governmentMean);

	for (unsigned int i = 0; i < countries.size(); i++)
	{
		double armyTech = ((landScale * (sourceCountries[countries[i].getSourceCountryIndex()].getLandTech() - landMean) / landStdDev) + 4.5);
		countries[i].setArmyTech(armyTech);
		log("%s has army tech of %f\n", countries[i].getTag().c_str(), armyTech);

		double navyTech = (navalScale * (sourceCountries[countries[i].getSourceCountryIndex()].getNavalTech() - navalMean) / navalStdDev) + 5;
		countries[i].setNavyTech(navyTech);
		log("%s has navy tech of %f\n", countries[i].getTag().c_str(), navyTech);

		double commerceTech = (tradeScale * (sourceCountries[countries[i].getSourceCountryIndex()].getTradeTech() - tradeMean) / tradeStdDev) + 5;
		countries[i].setCommerceTech(commerceTech);
		log("%s has commerce tech of %f\n", countries[i].getTag().c_str(), commerceTech);

		double industryTech = (productionScale * (sourceCountries[countries[i].getSourceCountryIndex()].getProductionTech() - productionMean) / productionStdDev) + 5;
		countries[i].setIndustryTech(industryTech);
		log("%s has industry tech of %f\n", countries[i].getTag().c_str(), industryTech);

		double cultureTech = ((governmentScale * (sourceCountries[countries[i].getSourceCountryIndex()].getGovernmentTech() - governmentMean) / governmentStdDev) + 4.5);
		countries[i].setCultureTech(cultureTech);
		log("%s has culture tech of %f\n", countries[i].getTag().c_str(), cultureTech);
	}

	int numRomanticLit = 0;
	int numRomanticArt = 0;
	int numRomanticMusic = 0;
	int numPressureChambers = 0;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		if (countries[i].getInventionState(romanticist_literature) == active)
		{
			numRomanticLit++;
		}
		if (countries[i].getInventionState(romanticist_art) == active)
		{
			numRomanticArt++;
		}
		if (countries[i].getInventionState(romanticist_music) == active)
		{
			numRomanticMusic++;
		}
		if (countries[i].getInventionState(pressure_chambers_for_thorax_surgery) == active)
		{
			numPressureChambers++;
		}
	}

	double romanticLitPrestige = 0;
	for (int i = 1; i <= numRomanticLit; i++)
	{
		romanticLitPrestige += 1.0/i;
	}
	romanticLitPrestige *= 20;
	romanticLitPrestige /= numRomanticLit;

	double romanticArtPrestige = 0;
	for (int i = 1; i <= numRomanticArt; i++)
	{
		romanticArtPrestige += 1.0/i;
	}
	romanticArtPrestige *= 20;
	romanticArtPrestige /= numRomanticArt;

	double romanticMusicPrestige = 0;
	for (int i = 1; i <= numRomanticMusic; i++)
	{
		romanticMusicPrestige += 1.0/i;
	}
	romanticMusicPrestige *= 20;
	romanticMusicPrestige /= numRomanticMusic;

	double pressureChambersPrestige = 0;
	for (int i = 1; i <= numPressureChambers; i++)
	{
		pressureChambersPrestige += 1.0/i;
	}
	pressureChambersPrestige *= 50;
	pressureChambersPrestige /= numPressureChambers;

	for (unsigned int i = 0; i < countries.size(); i++)
	{
		if (countries[i].getInventionState(romanticist_literature) == active)
		{
			countries[i].addPrestige(romanticLitPrestige);
		}
		if (countries[i].getInventionState(romanticist_art) == active)
		{
			countries[i].addPrestige(romanticArtPrestige);
		}
		if (countries[i].getInventionState(romanticist_music) == active)
		{
			countries[i].addPrestige(romanticMusicPrestige);
		}
		if (countries[i].getInventionState(pressure_chambers_for_thorax_surgery) == active)
		{
			countries[i].addPrestige(pressureChambersPrestige);
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
