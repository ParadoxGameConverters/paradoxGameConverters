#include "HoI3World.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include "Parsers/Parser.h"
#include "Log.h"
#include "Configuration.h"
#include "GovernmentMapper.h"


void HoI3World::init(string HoI3Loc)
{
	// set province names and numbers
	ifstream read;
	read.open( (HoI3Loc + "\\map\\definition.csv").c_str() );
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

		HoI3Province newProv;
		newProv.init(provNum);
		provinces.push_back(newProv);
	}
	read.close();
	read.clear();

	// set province names
	getProvinceLocalizations(HoI3Loc + "\\localisation\\province_names.csv");
	getProvinceLocalizations(HoI3Loc + "\\localisation\\gold.csv");

	// determine whether each province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise HoI3 crashes)
	Object*	obj2 = doParseFile((HoI3Loc + "\\map\\positions.txt").c_str());
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
			for (vector<HoI3Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
			{
				if (pitr->getNum() == provinceNum)
				{
					pitr->setCoastal(true);
					break;
				}
			}
		}
	}

	obj2 = doParseFile((HoI3Loc + "\\map\\continent.txt").c_str());
	vector<Object*> objCont = obj2->getLeaves();
	if (objCont.size() == 0)
	{
		log("Error: map\\continent.txt failed to parse.");
		printf("Error: map\\continent.txt failed to parse.");
		exit(1);
	}
	for (vector<Object*>::iterator itr = objCont.begin(); itr != objCont.end(); ++itr)
	{
		string cont = (*itr)->getKey();
		vector<string> provs = (*itr)->getTokens();
		for (vector<string>::iterator pitr = provs.begin(); pitr != provs.end(); ++pitr)
		{
			continents[atoi(pitr->c_str())] = cont;
		}
	}
}


void HoI3World::getProvinceLocalizations(string file)
{
	ifstream read;
	string line;
	read.open( file.c_str() );
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
					break;
				}
			}
		}
	}
	read.close();
}


void HoI3World::addPotentialCountries(ifstream &countriesMapping, string HoI3Loc)
{
	int partiesIndex = 1;
	const date FirstStartDate = Configuration::getStartDate();
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

		if (tag == "REB")
		{
			continue;
		}
		HoI3Country newCountry;
		newCountry.init(tag, countryFileName);
		potentialCountries.push_back(newCountry);
	}
}


void HoI3World::sortCountries(const vector<string>& order)
{
	vector<HoI3Country> sortedCountries;
	for (vector<string>::const_iterator oitr = order.begin(); oitr != order.end(); ++oitr)
	{
		for (vector<HoI3Country>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
		{
			if (itr->getTag() == (*oitr))
			{
				sortedCountries.push_back(*itr);
				break;
			}
		}
	}
	countries.swap(sortedCountries);
}


vector<string> HoI3World::getPotentialTags()
{
	vector<string> tagList;
	for (unsigned int i = 0; i < potentialCountries.size(); i++)
	{
		tagList.push_back(potentialCountries[i].getTag());
	}
	return tagList;
}


void HoI3World::convertCountries(V2World sourceWorld, countryMapping countryMap)
{
	vector<string> outputOrder = getPotentialTags();
	vector<V2Country> sourceCountries = sourceWorld.getCountries();
	for (unsigned int i = 0; i < sourceCountries.size(); i++)
	{
		HoI3Country newCountry;
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

					//XXX: Map prestige(?)

					// Map government types
					newCountry.setGovernment(getGovernmentForCountry(sourceCountries[i], sourceWorld));
					newCountry.setLastElection(sourceCountries[i].getLastElection());

					//XXX: set up ministers(?)

					// civil law - democracies get open society, communist dicatorships get totalitarian, everyone else gets limited restrictions
					if (sourceCountries[i].getGovernmentType() == "democracy" || sourceCountries[i].getGovernmentType() == "hms_government")
						newCountry.setCivilLaw("open_society");
					else if (sourceCountries[i].getGovernmentType() == "proletarian_dictatorship")
						newCountry.setCivilLaw("totalitarian_system");
					else
						newCountry.setCivilLaw("limited_restrictions");

					// conscription law - everyone starts with volunteer armies
					newCountry.setConscriptionLaw("volunteer_army");

					// economic law - everyone starts with full civilian economy
					newCountry.setEconomicLaw("full_civilian_economy");

					// educational investment law - from educational spending
					if (sourceCountries[i].getEducationSpending() > 0.90)
						newCountry.setEducationalLaw("big_education_investment");
					else if (sourceCountries[i].getEducationSpending() > 0.70)
						newCountry.setEducationalLaw("medium_large_education_investment");
					else if (sourceCountries[i].getEducationSpending() > 0.40)
						newCountry.setEducationalLaw("average_education_investment");
					else
						newCountry.setEducationalLaw("minimal_education_investment");

					// industrial policy laws - everyone starts with consumer product orientation
					newCountry.setIndustrialLaw("consumer_product_orientation");

					// press laws - set from press reforms
					if (sourceCountries[i].getReform("press_rights") == "free_press")
						newCountry.setPressLaw("free_press");
					else if (sourceCountries[i].getReform("press_rights") == "censored_press")
						newCountry.setPressLaw("censored_press");
					else // press_rights == state_press
					{
						if ((sourceCountries[i].getGovernmentType() == "proletarian_dicatorship") ||
							(sourceCountries[i].getGovernmentType() == "fascist_dictatorship"))
						{
							newCountry.setPressLaw("propaganda_press");
						}
						else
						{
							newCountry.setPressLaw("state_press");
						}
					}

					// training laws - from military spending
					if (sourceCountries[i].getMilitarySpending() > 0.90)
						newCountry.setTrainingLaw("specialist_training");
					else if (sourceCountries[i].getMilitarySpending() > 0.70)
						newCountry.setTrainingLaw("advanced_training");
					else if (sourceCountries[i].getMilitarySpending() > 0.40)
						newCountry.setTrainingLaw("basic_training");
					else
						newCountry.setTrainingLaw("minimal_training");

					vector<V2Relations> srcRelations = sourceCountries[i].getRelations();
					if (srcRelations.size() > 0)
					{
						for (vector<V2Relations>::iterator itr = srcRelations.begin(); itr != srcRelations.end(); ++itr)
						{
							// XXX: dunno what to do with influence level/sphere data
							countryMapping::iterator newTag = countryMap.find(itr->getCountry());
							if (newTag != countryMap.end())
							{
								HoI3Relations HoI3r;
								HoI3r.init(newTag->second);
								HoI3r.setRelations(itr->getRelations());
								HoI3r.setMilitaryAccess(itr->hasMilitaryAccess());
								HoI3r.setDiplomatLastSent(itr->getDiplomatLastSent());
								HoI3r.setLastWar(itr->getLastWar());
								newCountry.addRelations(HoI3r);
							}
						}
					}
					newCountry.sortRelations(outputOrder);

					//XXX: newCountry.setMoney(Configuration::getMoneyFactor() * sourceCountries[i].getMoney());

					/*XXX:
					vector<V2Loan> srcLoans = sourceCountries[i].getLoans();
					for (vector<V2Loan>::iterator itr = srcLoans.begin(); itr != srcLoans.end(); ++itr)
					{
						string lender = newCountry.getTag();
						if (itr->getLender() != "---")
						{
							countryMapping::iterator newTag = countryMap.find(itr->getLender());
							if (newTag != countryMap.end())
							{
								lender = newTag->second;
							}
							else
							{
								log("Error: lender %s could not be found for %s's loan!\n", itr->getLender().c_str(), newCountry.getTag().c_str());
							}
						}
						double size = Configuration::getMoneyFactor() * sourceCountries[i].inflationAdjust(itr->getAmount());
						newCountry.addLoan(lender, size, itr->getInterest() / 100.0f);
					}
					*/

					newCountry.setDiploPoints(2.5 * sourceCountries[i].getDiploPoints()); // 2-to-5 ratio based on alliance cost
				}
			}
		}
		else
		{
			log("Error: Could not convert V2 tag %s to HoI3.\n", sourceCountries[i].getTag().c_str());
			printf("Error: Could not convert V2 tag %s to HoI3.\n", sourceCountries[i].getTag().c_str());
			newCountry.init("", "");
		}

		countries.push_back(newCountry);
	}

	// ALL potential countries should be output to the file, otherwise some things don't get initialized right
	for (vector<HoI3Country>::iterator itr = potentialCountries.begin(); itr != potentialCountries.end(); ++itr)
	{
		vector<HoI3Country>::iterator citr = countries.begin();
		for (; citr != countries.end(); ++citr)
		{
			if (citr->getTag() == itr->getTag())
				break;
		}
		if (citr == countries.end())
		{
			countries.push_back(*itr);
			(--countries.end())->initFromHistory();
		}
	}

	// put countries in the same order as potentialCountries was (this is the same order HoI3 will save them in)
	sortCountries(outputOrder);
}


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<V2Province*> provinces;
	int totalPopulation;
};

void HoI3World::convertProvinces(V2World sourceWorld, provinceMapping provMap, countryMapping contMap)
{
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		int destNum				= provinces[i].getNum();
		vector<int> sourceNums	= provMap[destNum];
		if (sourceNums.size() == 0)
		{
			log("Error: no source for %s (province #%d)\n", provinces[i].getName().c_str() , destNum);
		}
		else if (sourceNums[0] == 0)
		{
			continue;
		}
		else
		{
			V2Province* oldProvince = NULL;
			string oldOwner = "";
			// determine ownership by province count, or total population (if province count is tied)
			map<string, MTo1ProvinceComp> provinceBins;
			double newProvinceTotalPop = 0;
			for (vector<int>::iterator itr = sourceNums.begin(); itr != sourceNums.end(); ++itr)
			{
				V2Province* province = sourceWorld.getProvince(*itr);
				if (!province)
				{
					log("Error: old province %d does not exist.  Bad mapping?\n", sourceNums[0]);
					continue;
				}
				string tag = province->getOwner();
				if (provinceBins.find(tag) == provinceBins.end())
					provinceBins[tag] = MTo1ProvinceComp();
				provinceBins[tag].provinces.push_back(province);
				provinceBins[tag].totalPopulation += province->getPopulation();
				newProvinceTotalPop += province->getPopulation();
				// I am the new owner if there is no current owner, or I have more provinces than the current owner,
				// or I have the same number of provinces, but more population, than the current owner
				if (   (oldOwner == "")
					|| (provinceBins[tag].provinces.size() > provinceBins[oldOwner].provinces.size())
					|| ((provinceBins[tag].provinces.size() == provinceBins[oldOwner].provinces.size())
					    && (provinceBins[tag].totalPopulation > provinceBins[oldOwner].totalPopulation)))
				{
					oldOwner = tag;
					oldProvince = province;
				}
			}

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

					for (vector<HoI3Country>::iterator citr = countries.begin(); citr != countries.end(); ++citr)
					{
						if (citr->getTag() == iter->second)
						{
							citr->addProvince(i);
							break;
						}
					}

					for (map<string, MTo1ProvinceComp>::iterator mitr = provinceBins.begin(); mitr != provinceBins.end(); ++mitr)
					{
						for (vector<V2Province*>::iterator vitr = mitr->second.provinces.begin(); vitr != mitr->second.provinces.end(); ++vitr)
						{
							vector<string> oldCores = (*vitr)->getCores();
							for(unsigned int j = 0; j < oldCores.size(); j++)
							{
								// skip this core if the country is the owner of the V2 province but not the HoI3 province
								// (i.e. "avoid boundary conflicts that didn't exist in V2").
								// this country may still get core via a province that DID belong to the current HoI3 owner
								if ((oldCores[j] == mitr->first) && (oldCores[j] != oldOwner))
									continue;

								iter = contMap.find(oldCores[j]);
								if (iter != contMap.end())
								{
									provinces[i].addCore(iter->second);
								}
							}

							// TODO: preliminary improvement conversion
							// 1-to-1 conversion as here means max vic = 1/2 max HoI level.  maybe want to scale?
							provinces[i].requireCoastalFort((*vitr)->getFort());
							provinces[i].requireLandFort((*vitr)->getFort());
							provinces[i].requireNavalBase((*vitr)->getNavalBase());
							provinces[i].requireInfrastructure((int)Configuration::getMinInfra());
							provinces[i].requireInfrastructure((*vitr)->getInfra());

							provinces[i].setAvgMil((*vitr)->getAvgMil());
							provinces[i].setNCrafts((*vitr)->getPopulation("craftsmen") / mitr->second.provinces.size()
												  + (*vitr)->getPopulation("artisans") / mitr->second.provinces.size()
												  + (*vitr)->getPopulation("clerks") * 2 / mitr->second.provinces.size());

							// XXX: how shall we set industry?
						}
					}
				}
			}
		}
	}
}

void HoI3World::output(FILE* output)
{
	fprintf(output, "date=\"%s\"\n", Configuration::getStartDate().toString().c_str());
	fprintf(output, "automate_trade=no\n");
	fprintf(output, "automate_sliders=0\n");
	fprintf(output, "automate_tech_sliders=0\n");
	fprintf(output, "rebel=1\n");
	fprintf(output, "convoy=1\n");
	fprintf(output, "theatre=1\n");
	fprintf(output, "unit=%d\n", HoI3ArmyID().id);
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		provinces[i].output(output);
	}
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		countries[i].output(output);
	}
	fprintf(output, "faction=\n");
	fprintf(output, "{\n");
	fprintf(output, "axis=\n");
	fprintf(output, "{\n");
	for (vector<string>::iterator cty = axisFaction.begin(); cty != axisFaction.end(); ++cty)
	{
		fprintf(output, "\tcountry=\"%s\"\n", cty->c_str());
	}
	fprintf(output, "}\n");
	fprintf(output, "allies=\n");
	fprintf(output, "{\n");
	for (vector<string>::iterator cty = alliesFaction.begin(); cty != alliesFaction.end(); ++cty)
	{
		fprintf(output, "\tcountry=\"%s\"\n", cty->c_str());
	}
	fprintf(output, "}\n");
	fprintf(output, "comintern=\n");
	fprintf(output, "{\n");
	for (vector<string>::iterator cty = cominternFaction.begin(); cty != cominternFaction.end(); ++cty)
	{
		fprintf(output, "\tcountry=\"%s\"\n", cty->c_str());
	}
	fprintf(output, "}\n");
	fprintf(output, "}\n");
	diplomacy.output(output);
}


void HoI3World::convertCapitals(V2World sourceWorld, provinceMapping provinceMap)
{
	inverseProvinceMapping inverseProvinceMap = invertProvinceMap(provinceMap);
	vector<V2Country> oldCountries = sourceWorld.getCountries();
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		int sourceCountryIndex = countries[i].getSourceCountryIndex();
		if (sourceCountryIndex >= 0)
		{
			int oldCapital = oldCountries[sourceCountryIndex].getCapital();
			log("\n	V2tag: %s	old capital: %4d", oldCountries[sourceCountryIndex].getTag().c_str(), oldCapital);
			inverseProvinceMapping::iterator itr = inverseProvinceMap.find(oldCapital);
			if (itr != inverseProvinceMap.end())
			{
				int newCapital = itr->second[0];
				countries[i].setCapital(newCapital);
				log("	new capital: %d", newCapital);
			}
		}
	}
	log("\n");
}

void HoI3World::convertTechs(V2World& sourceWorld)
{
	map<string, vector<pair<string, int> > > techTechMap;
	map<int, vector<pair<string, int> > > invTechMap;

	// build tech maps - the code is ugly so the file can be pretty
	Object* obj = doParseFile("tech_mapping.txt");
	vector<Object*> objs = obj->getValue("tech_map");
	if (objs.size() < 1)
	{
		log("Error: Could not read tech map!\n");
		printf("Error: Could not read tech map!\n");
		exit(1);
	}
	objs = objs[0]->getValue("link");
	for (vector<Object*>::iterator itr = objs.begin(); itr != objs.end(); ++itr)
	{
		vector<string> keys = (*itr)->getKeys();
		int status = 0; // 0 = unhandled, 1 = tech, 2 = invention
		vector<pair<string, int> > targetTechs;
		string tech = "";
		for (vector<string>::iterator master = keys.begin(); master != keys.end(); ++master)
		{
			if (status == 0 && (*master) == "v2_inv")
			{
				tech = (*itr)->getLeaf("v2_inv");
				status = 2;
			}
			else if (status == 0 && (*master) == "v2_tech")
			{
				tech = (*itr)->getLeaf("v2_tech");
				status = 1;
			}
			else
			{
				int value = atoi((*itr)->getLeaf(*master).c_str());
				targetTechs.push_back(pair<string, int>(*master, value));
			}
		}
		switch (status)
		{
		case 0:
			log("Error: unhandled tech link with first key %s!", keys[0].c_str());
			break;
		case 1:
			techTechMap[tech] = targetTechs;
			break;
		case 2:
			for (int i = 0; i <= naval_exercises; ++i)
			{
				if (tech == inventionNames[i])
				{
					invTechMap[i] = targetTechs;
					break;
				}
			}
			break;
		}
	}

	vector<V2Country> oldCountries = sourceWorld.getCountries();
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		int sourceCountryIndex = countries[i].getSourceCountryIndex();
		if (sourceCountryIndex >= 0)
		{
			vector<string> techs = oldCountries[sourceCountryIndex].getTechs();
			for (vector<string>::iterator itr = techs.begin(); itr != techs.end(); ++itr)
			{
				map<string, vector<pair<string, int> > >::iterator mapItr = techTechMap.find(*itr);
				if (mapItr == techTechMap.end())
				{
					continue;
				}
				else
				{
					for (vector<pair<string, int> >::iterator jtr = mapItr->second.begin(); jtr != mapItr->second.end(); ++jtr)
					{
						countries[i].setTechnology(jtr->first, jtr->second);
					}
				}
			}
			inventionStatus* inventions = oldCountries[sourceCountryIndex].getInventionStatusArray();
			for (int inv = 0; inv <= naval_exercises; ++inv)
			{
				if (inventions[inv] == active)
				{
					map<int, vector<pair<string, int> > >::iterator mapItr = invTechMap.find(inv);
					if (mapItr == invTechMap.end())
					{
						continue;
					}
					else
					{
						for (vector<pair<string, int> >::iterator jtr = mapItr->second.begin(); jtr != mapItr->second.end(); ++jtr)
						{
							countries[i].setTechnology(jtr->first, jtr->second);
						}
					}
				}
			}
		}
	}
}

static string CardinalToOrdinal(int cardinal)
{
	int hundredRem = cardinal % 100;
	int tenRem = cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}


vector<int> HoI3World::getPortProvinces(vector<int> locationCandidates)
{
	for (vector<HoI3Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
	{
		for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
		{
			if (pitr->getNum() == (*litr))
			{
				if (!pitr->isCoastal() || pitr->isBlacklistedPort())
				{
					locationCandidates.erase(litr);
					--pitr;
					break;
				}
			}
		}
	}
	return locationCandidates;
}


HoI3RegGroup HoI3World::createTheatre(HoI3Province* province)
{
	HoI3RegGroup newTheatre;
	newTheatre.setCommandLevel(theatre);
	newTheatre.setForceType(land);
	newTheatre.setFuelSupplies(100.0);
	newTheatre.setLocation(province->getNum());
	newTheatre.setLocationContinent(continents[province->getNum()]);
	newTheatre.setName(province->getName() + " HQ");
	return newTheatre;
}


void HoI3World::convertArmies(V2World sourceWorld, provinceMapping provinceMap, const map<int,int>& leaderIDMap)
{
	// hack for naval bases.  not ALL naval bases are in port provinces, and if you spawn a navy at a naval base in
	// a non-port province, HoI3 can crash....
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

	map<string, HoI3RegimentType> unitTypeMap; // <vic, hoi>
	Object* obj = doParseFile("unit_mapping.txt");
	vector<Object*> leaves = obj->getLeaves();
	if (leaves.size() < 1)
	{
		log ("Error: No unit mapping definitions loaded.\n");
		printf("Error: No unit mapping definitions loaded.\n");
		return;
	}
	leaves = leaves[0]->getLeaves();
	for (vector<Object*>::iterator itr = leaves.begin(); itr != leaves.end(); ++itr)
	{
		vector<Object*> keys = (*itr)->getValue("hoi");
		if (keys.size() > 1)
			log("Error: invalid unit mapping (multiple targets).\n");
		else if (keys.size() == 1)
		{
			HoI3RegimentType hoiKey(keys[0]->getLeaf());
			vector<Object*> keys = (*itr)->getValue("vic");
			if (keys.size() < 1)
				log("Error: invalid unit mapping (no source).\n");
			for (vector<Object*>::iterator key = keys.begin(); key != keys.end(); ++key)
			{
				unitTypeMap[(*key)->getLeaf()] = hoiKey;
			}
		}
	}

	HoI3RegimentType hqBrigade("hq_brigade");

	inverseProvinceMapping inverseProvinceMap = invertProvinceMap(provinceMap);
	vector<V2Country> sourceCountries = sourceWorld.getCountries();
	for (vector<HoI3Country>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		int sourceCountryIndex = itr->getSourceCountryIndex();
		if (sourceCountryIndex < 0)
			continue;
		V2Country* oldCountry = &sourceCountries[sourceCountryIndex];

		int airForceIndex = 0;

		HoI3RegGroup::resetRegGroupNameCounts();

		// Convert actual armies
		vector<V2Army> sourceArmies = oldCountry->getArmies();
		for (vector<V2Army>::iterator aitr = sourceArmies.begin(); aitr != sourceArmies.end(); ++aitr)
		{
			HoI3RegGroup destArmy;
			destArmy.setName(aitr->getName());
			destArmy.setFuelSupplies(aitr->getSupplies());
			if (aitr->getNavy())
			{
				destArmy.setForceType(navy);
				destArmy.setAtSea(aitr->getAtSea());
			}
			else
			{
				destArmy.setForceType(land);
			}

			map<int, int>::const_iterator lmapitr = leaderIDMap.find(aitr->getLeaderID());
			if (lmapitr != leaderIDMap.end())
			{
				destArmy.setLeaderID(lmapitr->second);
			}

			vector<int> locationCandidates = getHoI3ProvinceNums(inverseProvinceMap, aitr->getLocation());
			if (locationCandidates.size() == 0)
			{
				log("Error: Army or Navy %s assigned to unmapped province %d; dropping.\n", aitr->getName().c_str(), aitr->getLocation());
				continue;
			}

			bool usePort = false;
			// guarantee that navies are assigned to sea provinces, or land provinces with naval bases
			if (aitr->getNavy())
			{
				for (vector<HoI3Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
				{
					if ((pitr->getNum() == locationCandidates[0]) && pitr->isLand())
					{
						usePort = true;
						break;
					}
				}
				if (usePort)
				{
					locationCandidates = getPortProvinces(locationCandidates);
					if (locationCandidates.size() == 0)
					{
						log("Error: Navy %s assigned to V2 province %d which has no corresponding HoI3 port provinces; dropping.\n", aitr->getName().c_str(), aitr->getLocation());
						continue;
					}
				}
			}
			int selectedLocation = locationCandidates[int(locationCandidates.size() * ((double)rand() / RAND_MAX))];
			if (aitr->getNavy() && usePort)
			{
				vector<int>::iterator white = std::find(port_whitelist.begin(), port_whitelist.end(), selectedLocation);
				if (white == port_whitelist.end())
				{
					log("Warning: assigning navy to non-whitelisted port province %d.  If you encounter crashes, try blacklisting this province.\n", selectedLocation);
				}
			}
			destArmy.setLocation(selectedLocation);
			if (!aitr->getAtSea())
				destArmy.setLocationContinent(continents[selectedLocation]);
			HoI3Province* locationProvince;
			for (vector<HoI3Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
			{
				if (pitr->getNum() == selectedLocation)
					locationProvince = &(*pitr);
			}

			// air units need to be split into air forces
			HoI3RegGroup destAirForce;
			destAirForce.setFuelSupplies(aitr->getSupplies());
			destAirForce.setForceType(air);
			destAirForce.setLocation(selectedLocation);
			if (!aitr->getAtSea())
				destAirForce.setLocationContinent(continents[selectedLocation]);

			vector<V2Regiment> sourceRegiments = aitr->getRegiments();
			for (vector<V2Regiment>::iterator ritr = sourceRegiments.begin(); ritr != sourceRegiments.end(); ++ritr)
			{
				HoI3Regiment destReg;
				destReg.setName(ritr->getName());

				map<string, HoI3RegimentType>::iterator typeMap = unitTypeMap.find(ritr->getType());
				if (typeMap == unitTypeMap.end())
				{
					log("Regiment %s has unmapped unit type %s, dropping.\n", ritr->getName().c_str(), ritr->getType().c_str());
					continue;
				}
				destReg.setType(typeMap->second);

				double strengthFactor = 1.0;
				if (destReg.getForceType() != navy) // V2 and HoI naval units are already scaled correctly
					strengthFactor = ((double)typeMap->second.getMaxStrength()) / 3.0;
				destReg.setStrength(ritr->getStrength() * strengthFactor);
				destReg.setOrganization(ritr->getOrganization()); // XXX: needs scaling!
				destReg.setExperience(ritr->getExperience()); // XXX: needs scaling?

				// Add to army/navy or newly created air force as appropriate
				if (destReg.getForceType() != air)
					destArmy.addRegiment(destReg, true);
				else
					destAirForce.addRegiment(destReg, true);
			}

			if (!destArmy.isEmpty())
			{
				if (aitr->getNavy() && locationProvince->isLand())
				{
					// make sure a naval base is waiting for them
					locationProvince->requireNavalBase(1);
				}
				itr->addArmy(destArmy);
			}
			if (!destAirForce.isEmpty())
			{
				// we need to put an airbase here, so make sure we're in our own territory
				if (locationProvince->getOwner() != itr->getTag())
				{
					log("Airforce attached to army %s is not at a location where an airbase can be created; dropping.\n", aitr->getName().c_str());
					continue;
				}
				stringstream name;
				name << ++airForceIndex << CardinalToOrdinal(airForceIndex) << " Air Force";
				destAirForce.setName(name.str());
				// make sure an airbase is waiting for them
				locationProvince->requireAirBase(1);
				itr->addArmy(destAirForce);
			}
		}

		// Create a theatre in the capital
		HoI3Province* capProv = NULL;
		for (vector<HoI3Province>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
		{
			if (pitr->getNum() == itr->getCapital())
			{
				capProv = &(*pitr);
				break;
			}
		}
		if (!capProv)
			log("Error: no capital for country %s, can't generate theatres!\n", itr->getTag().c_str());
		else
		{
			HoI3RegGroup theatre = createTheatre(capProv);
			itr->addTheatre(theatre);
			vector<int> provs = itr->getProvinces();
			if (provs.size() > 100)
			{
				// need more theatres - find the least-militant province on each continent
				string capCont = continents[capProv->getNum()];
				map<string, pair<HoI3Province*, double> > theatres;
				theatres[capCont] = pair<HoI3Province*, double>(capProv, 0.0);
				for (vector<int>::iterator pitr = provs.begin(); pitr != provs.end(); ++pitr)
				{
					string pCont = continents[provinces[*pitr].getNum()];
					if (pCont == capCont)
						continue;
					else
					{
						map<string, pair<HoI3Province*, double> >::iterator titr = theatres.find(pCont);
						if (titr == theatres.end() || provinces[*pitr].getAvgMil() < titr->second.second)
						{
							theatres[pCont] = pair<HoI3Province*, double>(&(provinces[*pitr]), provinces[*pitr].getAvgMil());
						}
					}
				}
				for (map<string, pair<HoI3Province*, double> >::iterator titr = theatres.begin(); titr != theatres.end(); ++titr)
				{
					if (titr->first != capCont)
					{
						HoI3RegGroup theatre = createTheatre(titr->second.first);
						itr->addTheatre(theatre);
					}
				}
			}
		}

		// Generate HQ units
		itr->createArmyHQs(hqBrigade);
	}
}


void HoI3World::checkManualFaction(const countryMapping& countryMap, const vector<string>& candidateTags, vector<string>& destination, string logName)
{
	for (vector<string>::const_iterator itr = candidateTags.begin(); itr != candidateTags.end(); ++itr)
	{
		// get HoI3 tag from V2 tag
		countryMapping::const_iterator mitr = countryMap.find(*itr);
		if (mitr == countryMap.end())
		{
			log("Tag %s requested for %s faction, but is unmapped!\n", itr->c_str(), logName.c_str());
			continue;
		}
		string hoiTag = mitr->second;

		// find HoI3 nation and ensure that it has land
		bool found = false;
		for (vector<HoI3Country>::iterator citr = countries.begin(); citr != countries.end(); ++citr)
		{
			if (citr->getTag() == hoiTag)
			{
				if (citr->getProvinces().size() == 0)
				{
					log("Tag %s requested for %s faction, but is landless!\n", itr->c_str(), logName.c_str());
				}
				else
				{
					log("%s added to %s faction\n", itr->c_str(), logName.c_str());
					destination.push_back(*itr);
				}
				found = true;
				break;
			}
		}
		if (!found)
			log("Tag %s requested for %s faction, but does not exist!\n", itr->c_str(), logName.c_str());
	}
}


void HoI3World::factionSatellites(vector<string>& faction)
{
	// make sure that any vassals are in their master's faction
	vector<HoI3Agreement> agr = diplomacy.getAgreements();
	for (vector<HoI3Agreement>::iterator itr = agr.begin(); itr != agr.end(); ++itr)
	{
		if (itr->type == "vassal")
		{
			vector<string>::iterator masterTag = std::find(faction.begin(), faction.end(), itr->country1);
			if (masterTag != faction.end())
			{
				vector<string>::iterator slaveTag = std::find(faction.begin(), faction.end(), itr->country2);
				if (slaveTag == faction.end())
				{
					log("%s added to same faction as %s due to satellite status.\n", itr->country2.c_str(), itr->country1.c_str());
					faction.push_back(itr->country2);
				}
			}
		}
	}
}


void HoI3World::configureFactions(const countryMapping& countryMap)
{
	// find faction memebers
	if (Configuration::getFactionLeaderAlgo() == "manual")
	{
		log("Manual faction allocation requested.\n");
		checkManualFaction(countryMap, Configuration::getManualAxisFaction(), axisFaction, "Axis");
		checkManualFaction(countryMap, Configuration::getManualAlliesFaction(), alliesFaction, "Allies");
		checkManualFaction(countryMap, Configuration::getManualCominternFaction(), cominternFaction, "Comintern");
	}
	else if (Configuration::getFactionLeaderAlgo() == "auto")
	{
		log("Error: The requested faction algorithm is not yet implemented; skipping!\n");
		return;
	}
	else
	{
		printf("Error: unrecognized faction algorithm \"%s\"!", Configuration::getFactionLeaderAlgo().c_str());
		log("Error: unrecognized faction algorithm \"%s\"!", Configuration::getFactionLeaderAlgo().c_str());
		return;
	}

	// push satellites into the same faction as their parents
	factionSatellites(axisFaction);
	factionSatellites(alliesFaction);
	factionSatellites(cominternFaction);

	// set alignments
	for (vector<HoI3Country>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		bool axisMember = false;
		bool alliesMember = false;
		bool cominternMember = false;
		vector<string>::iterator axisTag = std::find(axisFaction.begin(), axisFaction.end(), itr->getTag());
		axisMember = (axisTag != axisFaction.end());
		vector<string>::iterator alliesTag = std::find(alliesFaction.begin(), alliesFaction.end(), itr->getTag());
		alliesMember = (alliesTag != alliesFaction.end());
		vector<string>::iterator cominternTag = std::find(cominternFaction.begin(), cominternFaction.end(), itr->getTag());
		cominternMember = (cominternTag != cominternFaction.end());
		if (   (axisMember && alliesMember)
			|| (axisMember && cominternMember)
			|| (alliesMember && cominternMember))
		{
			log("%s is a member of two factions!\n", itr->getTag().c_str());
		}

		// force alignment for faction members
		if (axisMember)
			itr->getAlignment()->alignToAxis();
		else if (alliesMember)
			itr->getAlignment()->alignToAllied();
		else if (cominternMember)
			itr->getAlignment()->alignToComintern();
		else
		{
			// scale for positive relations - 230 = distance from corner to circumcenter, 200 = max relations
			static const double positiveScale = (230.0 / 200.0);
			// scale for negative relations - 116 = distance from circumcenter to side opposite corner, 200 = max relations
			static const double negativeScale = (116.0 / 200.0);

			// weight alignment for non-members based on relations with faction leaders
			HoI3Alignment axisStart;
			HoI3Alignment alliesStart;
			HoI3Alignment cominternStart;
			if (axisFaction.size() > 0)
			{
				HoI3Relations* relObj = itr->getRelations(axisFaction[0]);
				if (relObj != NULL)
				{
					double axisRelations = relObj->getRelations();
					if (axisRelations >= 0.0)
						axisStart.moveTowardsAxis(axisRelations * positiveScale);
					else // axisRelations < 0.0
						axisStart.moveTowardsAxis(axisRelations * negativeScale);
				}
			}
			if (alliesFaction.size() > 0)
			{
				HoI3Relations* relObj = itr->getRelations(alliesFaction[0]);
				if (relObj != NULL)
				{
					double alliesRelations = relObj->getRelations();
					if (alliesRelations >= 0.0)
						alliesStart.moveTowardsAllied(alliesRelations * positiveScale);
					else // alliesRelations < 0.0
						alliesStart.moveTowardsAllied(alliesRelations * negativeScale);
				}
			}
			if (cominternFaction.size() > 0)
			{
				HoI3Relations* relObj = itr->getRelations(cominternFaction[0]);
				if (relObj != NULL)
				{
					double cominternRelations = relObj->getRelations();
					if (cominternRelations >= 0.0)
						cominternStart.moveTowardsComintern(cominternRelations * positiveScale);
					else // cominternRelations < 0.0
						cominternStart.moveTowardsComintern(cominternRelations * negativeScale);
				}
			}
			(*(itr->getAlignment())) = HoI3Alignment::getCentroid(axisStart, alliesStart, cominternStart);
		}
	}
}


void HoI3World::convertDiplomacy(V2World sourceWorld, countryMapping countryMap)
{
	vector<V2Agreement> agreements = sourceWorld.getDiplomacy().getAgreements();
	for (vector<V2Agreement>::iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		countryMapping::iterator newCountry1 = countryMap.find(itr->country1);
		if (newCountry1 == countryMap.end())
		{
			// log("Error: V2 Country %s used in diplomatic agreement doesn't exist\n", itr->country1.c_str());
			continue;
		}
		countryMapping::iterator newCountry2 = countryMap.find(itr->country2);
		if (newCountry2 == countryMap.end())
		{
			// log("Error: V2 Country %s used in diplomatic agreement doesn't exist\n", itr->country2.c_str());
			continue;
		}

		HoI3Country* hoi3Country1 = NULL;
		HoI3Country* hoi3Country2 = NULL;
		for (vector<HoI3Country>::iterator citr = countries.begin(); citr != countries.end(); ++citr)
		{
			if (citr->getTag() == newCountry1->second)
			{
				hoi3Country1 = &(*citr);
			}
			else if (citr->getTag() == newCountry2->second)
			{
				hoi3Country2 = &(*citr);
			}
			if (hoi3Country1 && hoi3Country2)
				break;
		}
		if (!hoi3Country1)
		{
			log("Error: HoI3 country %s used in diplomatic agreement doesn't exist\n", newCountry1->second.c_str());
			continue;
		}
		if (!hoi3Country2)
		{
			log("Error: HoI3 country %s used in diplomatic agreement doesn't exist\n", newCountry2->second.c_str());
			continue;
		}

		// XXX: unhandled V2 diplo types:
		// casus_belli: -> threat maybe?
		// warsubsidy: probably just drop, maybe cash trade...

		// shared diplo types
		if ((itr->type == "alliance") || (itr->type == "vassal"))
		{
			// copy agreement
			HoI3Agreement hoi3a;
			hoi3a.country1 = newCountry1->second;
			hoi3a.country2 = newCountry2->second;
			hoi3a.start_date = itr->start_date;
			hoi3a.type = itr->type;
			diplomacy.addAgreement(hoi3a);
		}

		// XXX: unhandled HoI3 diplo types:
		// guarantee: <- sphere maybe?
		// trade: probably just omit
		// influence: <- influence maybe?
	}
}

