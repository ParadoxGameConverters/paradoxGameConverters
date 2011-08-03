#include "V2World.h"
#include <fstream>
#include <algorithm>
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
	read.clear();

	// set province names
	read.open( (V2Loc + "\\localisation\\text.csv").c_str() );
	while (read.good() && !read.eof())
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
	read.clear();
	read.open( (V2Loc + "\\localisation\\1.1.csv").c_str() );
	while (read.good() && !read.eof())
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
	read.clear();
	read.open( (V2Loc + "\\localisation\\1.2.csv").c_str() );
	while (read.good() && !read.eof())
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
	read.clear();
	read.open( (V2Loc + "\\localisation\\beta1.csv").c_str() );
	while (read.good() && !read.eof())
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
	read.clear();
	read.open( (V2Loc + "\\localisation\\beta2.csv").c_str() );
	while (read.good() && !read.eof())
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
	read.clear();

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
					read.clear();
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
			read.clear();
			continue;
		}
		readFile(read);
		read.close();
		read.clear();
	
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

	// determine whether a province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise Vicky crashes)
	Object*	obj2;				// generic object
	initParser();
	obj2 = Parser::topLevel;
	read.open( (V2Loc + "\\map\\positions.txt").c_str() );
	if (!read.is_open())
	{
		log("Error: Could not open map\\positions.txt\n");
		printf("Error: Could not open map\\positions.txt\n");
		exit(1);
	}
	readFile(read);
	read.close();
	read.clear();

	vector<Object*> objProv = obj2->getLeaves();
	if (objProv.size() == 0)
	{
		log("Error: map\\positions.txt failed to parse.");
		printf("Error: map\\positions.txt failed to parse.");
		exit(1);
	}
	for (vector<Object*>::iterator itr = objProv.begin(); itr != objProv.end(); ++itr)
	{
		int provinceNum = atoi((*itr)->getKey().c_str());
		vector<Object*> objPos = (*itr)->getValue("building_position");
		if (objPos.size() == 0)
			continue;
		vector<Object*> objNavalBase = objPos[0]->getValue("naval_base");
		if (objNavalBase.size() != 0)
		{
			// this province is coastal
			for (vector<V2Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
			{
				if (pitr->getNum() == provinceNum)
				{
					pitr->setCoastal(true);
					break;
				}
			}
		}
	}
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


void V2World::convertCountries(EU3World sourceWorld, countryMapping countryMap, cultureMapping cultureMap, religionMapping religionMap, governmentMapping governmentMap)
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
							log("Error: No religion mapping defined for %s (%s -> %s)\n", srcReligion.c_str(), sourceCountries[i].getTag().c_str(), newCountry.getTag().c_str());
						}
					}

					newCountry.setPrestige(sourceCountries[i].getPrestige() + 100);

					string srcGovernment = sourceCountries[i].getGovernment();
					if (srcGovernment.size() > 0)
					{
						governmentMapping::iterator iter2 = governmentMap.find(srcGovernment);
						if (iter2 != governmentMap.end())
						{
							newCountry.setGovernment(iter2->second);
						}
						else
						{
							log("Error: No government mapping defined for %s (%s -> %s)\n", srcGovernment.c_str(), sourceCountries[i].getTag().c_str(), newCountry.getTag().c_str());
						}
					}

					vector<EU3Relations> srcRelations = sourceCountries[i].getRelations();
					if (srcRelations.size() > 0)
					{
						for (vector<EU3Relations>::iterator itr = srcRelations.begin(); itr != srcRelations.end(); ++itr)
						{
							countryMapping::iterator newTag = countryMap.find(itr->getCountry());
							if (newTag != countryMap.end())
							{
								V2Relations v2r;
								v2r.init(newTag->second);
								v2r.setRelations(itr->getRelations());
								v2r.setMilitaryAccess(itr->hasMilitaryAccess());
								v2r.setDiplomatLastSent(itr->getDiplomatLastSent());
								v2r.setLastWar(itr->getLastWar());
								newCountry.addRelations(v2r);
							}
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
			if (!oldProvince)
			{
				log("Error: old province %d does not exist.  Bad mapping?\n", sourceNums[0]);
				continue;
			}
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


void V2World::addUnions(unionMapping unionMap)
{
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		for (unsigned int j = 0; j < unionMap.size(); j++)
		{
			if (unionMap[j].first == provinces[i].getCulture())
			{
				provinces[i].addCore(unionMap[j].second);
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
				if (j == provinceMap.end())
					break;
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
		iter = unassignedProvs.begin();
		int		provId	= iter->getNum();
		string	owner		= iter->getOwner();

		if (owner == "")
		{
			unassignedProvs.erase(iter);
			continue;
		}

		V2State newState(stateId);
		stateId++;
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
				iter2->addState(newState);
			}
		}
	}
}

void V2World::convertDiplomacy(EU3World sourceWorld, countryMapping countryMap)
{
	vector<EU3Agreement> agreements = sourceWorld.getDiplomacy().getAgreements();
	for (vector<EU3Agreement>::iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		countryMapping::iterator newCountry1 = countryMap.find(itr->country1);
		if (newCountry1 == countryMap.end())
		{
			log("Error: EU3 Country %s used in diplomatic agreement doesn't exist\n", itr->country1.c_str());
			continue;
		}
		countryMapping::iterator newCountry2 = countryMap.find(itr->country2);
		if (newCountry2 == countryMap.end())
		{
			log("Error: EU3 Country %s used in diplomatic agreement doesn't exist\n", itr->country2.c_str());
			continue;
		}

		V2Country* v2Country1 = NULL;
		V2Country* v2Country2 = NULL;
		for (vector<V2Country>::iterator citr = countries.begin(); citr != countries.end(); ++citr)
		{
			if (citr->getTag() == newCountry1->second)
			{
				v2Country1 = &(*citr);
			}
			else if (citr->getTag() == newCountry2->second)
			{
				v2Country2 = &(*citr);
			}
			if (v2Country1 && v2Country2)
				break;
		}
		if (!v2Country1)
		{
			log("Error: Vic2 country %s used in diplomatic agreement doesn't exist\n", newCountry1->second.c_str());
			continue;
		}
		if (!v2Country2)
		{
			log("Error: Vic2 country %s used in diplomatic agreement doesn't exist\n", newCountry2->second.c_str());
			continue;
		}
		V2Relations* r1 = v2Country1->getRelations(newCountry2->second);
		if (!r1)
		{
			log("Error: Vic2 country %s has no relations with %s\n", newCountry1->second.c_str(), newCountry2->second.c_str());
			continue;
		}
		V2Relations* r2 = v2Country2->getRelations(newCountry1->second);
		if (!r2)
		{
			log("Error: Vic2 country %s has no relations with %s\n", newCountry2->second.c_str(), newCountry1->second.c_str());
			continue;
		}

		if ((itr->type == "royal_marriage" || itr->type == "guarantee"))
		{
			// influence level +1, but never exceed 4
			if (r1->getLevel() < 4)
				r1->setLevel(r1->getLevel() + 1);
		}
		if (itr->type == "royal_marriage")
		{
			// royal marriage is bidirectional; influence level +1, but never exceed 4
			if (r2->getLevel() < 4)
				r2->setLevel(r2->getLevel() + 1);
		}
		if ((itr->type == "sphere") || (itr->type == "vassal") || (itr->type == "union"))
		{
			// influence level = 5
			r1->setLevel(5);
			/* FIXME: is this desirable?
			// if relations are too poor, country2 will immediately eject country1's ambassadors at the start of the game
			// so, for stability's sake, give their relations a boost
			if (r1->getRelations() < 1)
				r1->setRelations(1);
			if (r2->getRelations() < 1)
				r2->setRelations(1);
			*/
		}
		if ((itr->type == "alliance") || (itr->type == "vassal"))
		{
			// copy agreement
			V2Agreement v2a;
			v2a.country1 = newCountry1->second;
			v2a.country2 = newCountry2->second;
			v2a.start_date = itr->start_date;
			v2a.type = itr->type;
			diplomacy.addAgreement(v2a);
		}
	}
}


// this should probably be in a data file instead of hardcoded
// the number is the raw strength of that unit type needed in EU3 for one V2 regiment to be created
// (see file "unit_strength.txt" for how much EU3 units count for)
// these are based off typical end-game EU3 units being converted 3-to-1 (1000-to-3000 man regiments) for ground
// and roughly by #cannons for 1-to-1 conversion of endgame sea units
static const double cost_per_regiment[num_reg_categories] =
{
	135.0, // infantry - 45 points per 1000, 3000-man regiments
	135.0, // cavalry - 45 points per 1000, 3000-man regiments
	120.0, // artillery - 40 points per 1000, 3000-man regiments
	60.0, // big ship - 1-to-1 for threedeckers
	30.0, // light ship - 1-to-1 for heavy frigates
	30.0, // galley - 3-to-2 for archipelago frigates (turn into light ships in vicky)
	24.0, // transport - 1-to-1 for all transports
};

bool V2World::addRegimentToArmy(V2Army* army, RegimentCategory rc, const inverseProvinceMapping& inverseProvinceMap)
{
	V2Regiment reg((RegimentCategory)rc);
	// TODO: what to do about regiment names?
	if (!army->getNavy())
	{
		// Armies need to be associated with pops
		int eu3Home = army->getSourceArmy()->getProbabilisticHomeProvince(rc);
		const vector<int>& homeCandidates = getV2ProvinceNums(inverseProvinceMap, eu3Home);
		if (homeCandidates.size() == 0)
		{
			log("Error: Regiment in army %s has unmapped home province %d; dissolving to pool.\n", army->getName().c_str(), eu3Home);
			return false;
		}
		int newHomeProvince = homeCandidates[int(homeCandidates.size() * ((double)rand() / RAND_MAX))];
		for (vector<V2Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
		{
			if (pitr->getNum() == newHomeProvince)
			{
				vector<V2Pop> pops = pitr->getPops("soldiers");
				if (pops.size() == 0)
				{
					log("Error: V2 province %d is home for a regiment, but has no soldier pops! Dissolving regiment to pool.\n", newHomeProvince);
					return false;
				}
				reg.setPopID(pops[int(pops.size() * ((double)rand() / RAND_MAX))].getID());
				break;
			}
		}
	}
	reg.setStrength(army->getSourceArmy()->getAverageStrength(rc) * (army->getNavy() ? 100.0 : 3.0));
	army->addRegiment(reg);
	return true;
}

void V2World::convertArmies(EU3World sourceWorld, provinceMapping provinceMap)
{
	// hack for naval bases.  not ALL naval bases are in port provinces, and if you spawn a navy at a naval base in
	// a non-port province, Vicky crashes....
	vector<int> port_whitelist;
	{
		int temp = 0;
		ifstream s("port_whitelist.txt");
		while (s.good() && !s.eof())
		{
			s >> temp;
			port_whitelist.push_back(temp);
		}
		s.close();
	}
	vector<int> port_blacklist;
	{
		int temp = 0;
		ifstream s("port_blacklist.txt");
		while (s.good() && !s.eof())
		{
			s >> temp;
			port_blacklist.push_back(temp);
		}
		s.close();
	}

	inverseProvinceMapping inverseProvinceMap = invertProvinceMap(provinceMap);
	vector<EU3Country> sourceCountries = sourceWorld.getCountries();
	for (vector<V2Country>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		EU3Country* oldCountry = &sourceCountries[itr->getSourceCountryIndex()];

		// set up armies with whatever regiments they deserve, rounded down
		// and keep track of the remainders for later
		double countryRemainder[num_reg_categories] = { 0.0 };
		vector<EU3Army> sourceArmies = oldCountry->getArmies();
		for (vector<EU3Army>::iterator aitr = sourceArmies.begin(); aitr != sourceArmies.end(); ++aitr)
		{
			V2Army army;
			army.setSourceArmy(&(*aitr));
			army.setName(aitr->getName());
			army.setAtSea(aitr->getAtSea());
			for (int rc = infantry; rc < num_reg_categories; ++rc)
			{
				int typeStrength = aitr->getTotalTypeStrength((RegimentCategory)rc);
				if (typeStrength == 0) // no regiments of this type
					continue;

				// if we have ships, we must be a navy
				bool isNavy = (rc >= big_ship); 
				army.setNavy(isNavy);

				double regimentCount = typeStrength / cost_per_regiment[rc];
				int regimentsToCreate = (int)floor(regimentCount);
				double regimentRemainder = regimentCount - regimentsToCreate;
				countryRemainder[rc] += regimentRemainder;

				double avg_strength = aitr->getAverageStrength((RegimentCategory)rc);
				for (int i = 0; i < regimentsToCreate; ++i)
				{
					if (!addRegimentToArmy(&army, (RegimentCategory)rc, inverseProvinceMap))
					{
						// couldn't add, dissolve into pool
						countryRemainder[rc] += 1.0;
					}
				}
			}
			vector<int> locationCandidates = getV2ProvinceNums(inverseProvinceMap, aitr->getLocation());
			if (locationCandidates.size() == 0)
			{
				log("Error: Army or Navy %s assigned to unmapped province %d; dissolving to pool.\n", aitr->getName().c_str(), aitr->getLocation());
				int regimentCounts[num_reg_categories] = { 0 };
				army.getRegimentCounts(regimentCounts);
				for (int rc = infantry; rc < num_reg_categories; ++rc)
				{
					countryRemainder[rc] += regimentCounts[rc];
				}
				continue;
			}
			// guarantee that navies are assigned to provinces with naval bases
			if (army.getNavy())
			{
				for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
				{
					vector<int>::iterator black = std::find(port_blacklist.begin(), port_blacklist.end(), *litr);
					if (black != port_blacklist.end())
					{
						locationCandidates.erase(litr);
						break;
					}
				}
				for (vector<V2Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
				{
					for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
					{
						if (pitr->getNum() == (*litr))
						{
							if (!pitr->isCoastal())
							{
								locationCandidates.erase(litr);
								--pitr;
								break;
							}
						}
					}
				}
				if (locationCandidates.size() == 0)
				{
					log("Error: Navy %s assigned to EU3 province %d which has no corresponding coastal V2 provinces; dissolving to pool.\n", aitr->getName().c_str(), aitr->getLocation());
					int regimentCounts[num_reg_categories] = { 0 };
					army.getRegimentCounts(regimentCounts);
					for (int rc = infantry; rc < num_reg_categories; ++rc)
					{
						countryRemainder[rc] += regimentCounts[rc];
					}
					continue;
				}
			}
			int selectedLocation = locationCandidates[int(locationCandidates.size() * ((double)rand() / RAND_MAX))];
			if (army.getNavy())
			{
				vector<int>::iterator white = std::find(port_whitelist.begin(), port_whitelist.end(), selectedLocation);
				if (white == port_whitelist.end())
				{
					log("Warning: assigning navy to non-whitelisted port province %d.  If the save crashes, try blacklisting this province.\n", selectedLocation);
					ofstream s("port_greylist.txt", ios_base::app);
					s << selectedLocation << "\n";
					s.close();
				}
			}
			army.setLocation(selectedLocation);
			itr->addArmy(army);
		}

		// allocate the remainders from the whole country to the armies according to their need, rounding up
		for (int rc = infantry; rc < num_reg_categories; ++rc)
		{
			if (countryRemainder[rc] > 0.0)
			{
				log("Allocating regiments of type %d from the remainder pool for %s (total: %4lf)\n", rc, itr->getTag().c_str(), countryRemainder[rc]);
			}
			while (countryRemainder[rc] > 0.0)
			{
				V2Army* army = itr->getArmyForRemainder((RegimentCategory)rc);
				if (!army)
				{
					log("Error: no suitable army or navy found for %s's pooled regiments of type %d!\n", itr->getTag().c_str(), rc);
					break;
				}
				if (addRegimentToArmy(army, (RegimentCategory)rc, inverseProvinceMap))
				{
					countryRemainder[rc] -= 1.0;
				}
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
	double highestLand;

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
	unsigned int i = 0;
	while (sourceCountries[i].getProvinces().size() == 0)
	{
		i++;
	}
	highestLand			= oldLandMean			= landMean			= sourceCountries[i].getLandTech();
	highestNaval		= oldNavalMean			= navalMean			= sourceCountries[i].getNavalTech();
	highestTrade		= oldTradeMean			= tradeMean			= sourceCountries[i].getTradeTech();
	highestProduction	= oldProductionMean	= productionMean	= sourceCountries[i].getProductionTech();
	highestGovernment	= oldGovernmentMean	= governmentMean	= sourceCountries[i].getGovernmentTech();

	for (i++; i < sourceCountries.size(); i++)
	{
		if (sourceCountries[i].getProvinces().size() == 0)
		{
			continue;
		}
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
	diplomacy.output(output);
}
