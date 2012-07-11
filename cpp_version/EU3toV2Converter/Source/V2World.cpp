#include "V2World.h"
#include <fstream>
#include <algorithm>
#include <io.h>
#include <list>
#include <math.h>
#include <float.h>
#include "Parsers/Parser.h"
#include "Log.h"
#include "V2LeaderTraits.h"
#include "tempFuncs.h"
#include "Configuration.h"
#include "EU3World.h"
#include "EU3Relations.h"
#include "EU3Loan.h"
#include "EU3Leader.h"
#include "EU3Province.h"
#include "EU3Diplomacy.h"
#include "V2Province.h"
#include "V2State.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Leader.h"
#include "V2Pop.h"
#include "V2Country.h"
#include "V2Reforms.h"



void V2World::init(string V2Loc)
{
	buildParties();

	equalityLeft	= 6;
	libertyLeft		= 30;

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

		V2Province* newProv = new V2Province(provNum);
		provinces.push_back(newProv);
	}
	read.close();
	read.clear();

	// set province names
	getProvinceLocalizations(V2Loc + "\\localisation\\text.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\1.1.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\1.2.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\beta1.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\beta2.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\beta3.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\1.3.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\newtext.csv");
	getProvinceLocalizations(V2Loc + "\\localisation\\1.4.csv");
	if (Configuration::getV2Gametype() == "AHD")
	{
		getProvinceLocalizations(V2Loc + "\\localisation\\housedivided.csv");
		getProvinceLocalizations(V2Loc + "\\localisation\\housedivided2_1.csv");
	}


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
		string provDirPath = V2Loc + "\\history\\provinces\\" + provDirData.name + "\\";
		if (strcmp(provDirData.name, ".") == 0 || strcmp(provDirData.name, "..") == 0 )
		{
			continue;
		}
		struct _finddata_t	provFileData;
		intptr_t					fileListing2;
		if ( (fileListing2 = _findfirst( (provDirPath + "*").c_str(), &provFileData)) == -1L)
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

			vector<V2Province*>::iterator i;
			for(i = provinces.begin(); i != provinces.end(); i++)
			{
				if ( (*i)->getNum() == provNum )
				{
					Object* provinceObj = doParseFile((provDirPath + provFileData.name).c_str());
					(*i)->importHistory(provinceObj);
					break;
				}
			}
			if (i == provinces.end())
			{
				log("	Error: could not find province %d to add rgo and liferating.\n", provNum);
			}
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
		obj2 = getTopLevel();
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
			while (k < provinces.size() && provNum != provinces[k]->getNum())
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
					vector< pair<int, double> > issues;
					issues.clear();
					V2Pop* newPop = new V2Pop(pops[l]->getKey(), atoi(pops[l]->getLeaf("size").c_str()), pops[l]->getLeaf("culture"), pops[l]->getLeaf("religion"), 10.0, 0.0, 0.0, 0.0, issues);
					provinces[k]->addOldPop(newPop);
				}
			}
		}
	} while(_findnext(fileListing, &popsFileData) == 0);
	_findclose(fileListing);

	// determine whether a province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise Vicky crashes)
	Object*	obj2 = doParseFile((V2Loc + "\\map\\positions.txt").c_str());
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
			for (vector<V2Province*>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
			{
				if ( (*pitr)->getNum() == provinceNum)
				{
					(*pitr)->setCoastal(true);
					break;
				}
			}
		}
	}
}


void V2World::getProvinceLocalizations(string file)
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
				if (provinces[i]->getNum() == num)
				{
					provinces[i]->setName(name);
					break;
				}
			}
		}
	}
	read.close();
}


void V2World::addPotentialCountries(ifstream &countriesMapping, string V2Loc)
{
	int partiesIndex = 1;
	const date FirstStartDate = Configuration::getStartDate();
	while (!countriesMapping.eof())
	{
		string line;
		getline(countriesMapping, line);

		if ( (line.size() < 3) || (line[0] == '#') )
		{
			continue;
		}
		
		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start			= line.find_first_of('/');
		int size				= line.find_last_of('\"') - start;
		countryFileName	= line.substr(start, size);

		vector<int> parties;
		Object* countryData = doParseFile((V2Loc + "\\common\\countries\\" + countryFileName).c_str());

		vector<Object*> partyData = countryData->getValue("party");
		for (vector<Object*>::iterator itr = partyData.begin(); itr != partyData.end(); ++itr)
		{
			parties.push_back(partiesIndex);
			partiesIndex++;
		}

		if (tag == "REB")
		{
			continue;
		}
		V2Country* newCountry = new V2Country(tag, countryFileName, parties, this);
		potentialCountries.push_back(newCountry);
	}
}


void V2World::sortCountries(const vector<string>& order)
{
	vector<V2Country*> sortedCountries;
	for (vector<string>::const_iterator oitr = order.begin(); oitr != order.end(); ++oitr)
	{
		for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
		{
			if ( (*itr)->getTag() == (*oitr) )
			{
				sortedCountries.push_back(*itr);
				break;
			}
		}
	}
	countries.swap(sortedCountries);
}


vector<string> V2World::getPotentialTags()
{
	vector<string> tagList;
	for (unsigned int i = 0; i < potentialCountries.size(); i++)
	{
		tagList.push_back(potentialCountries[i]->getTag());
	}
	return tagList;
}


void V2World::convertCountries(EU3World sourceWorld, countryMapping countryMap, cultureMapping cultureMap, unionCulturesMap unionCultures, religionMapping religionMap, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, vector<techSchool> techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt)
{
	vector<string> outputOrder = getPotentialTags();
	map<string, EU3Country*> sourceCountries = sourceWorld.getCountries();
	for (map<string, EU3Country*>::iterator i = sourceCountries.begin(); i != sourceCountries.end(); i++)
	{
		V2Country* newCountry;
		countryMapping::iterator iter;
		iter = countryMap.find(i->second->getTag());
		if (iter != countryMap.end())
		{
			for(unsigned int j = 0; j < potentialCountries.size(); j++)
			{
				if (potentialCountries[j]->getTag() == iter->second.c_str())
				{
					newCountry = potentialCountries[j];
					newCountry->initFromEU3Country(i->second, outputOrder, countryMap, cultureMap, religionMap, unionCultures, governmentMap, inverseProvinceMap, techSchools, leaderMap, lt);
					newCountry->setNationalIdea(libertyLeft, equalityLeft);
				}
			}
		}
		else
		{
			log("Error: Could not convert EU3 tag %s to V2.\n", i->second->getTag().c_str());
			printf("Error: Could not convert EU3 tag %s to V2.\n", i->second->getTag().c_str());
		}

		countries.push_back(newCountry);
	}

	// ALL potential countries should be output to the file, otherwise some things don't get initialized right
	for (vector<V2Country*>::iterator itr = potentialCountries.begin(); itr != potentialCountries.end(); ++itr)
	{
		vector<V2Country*>::iterator citr = countries.begin();
		for (; citr != countries.end(); ++citr)
		{
			if ( (*citr)->getTag() == (*itr)->getTag())
				break;
		}
		if (citr == countries.end())
		{
			countries.push_back(*itr);
			( *(--countries.end()) )->initFromHistory();
		}
	}

	// put countries in the same order as potentialCountries was (this is the same order V2 will save them in)
	sortCountries(outputOrder);
}


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<EU3Province*> provinces;
	int totalPopulation;
};


void V2World::convertProvinces(EU3World sourceWorld, provinceMapping provMap, countryMapping contMap, cultureMapping cultureMap, religionMapping religionMap)
{
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		int destNum					= provinces[i]->getNum();
		vector<int> sourceNums	= provMap[destNum];
		if (sourceNums.size() == 0)
		{
			log("Error: no source for %s (province #%d)\n", provinces[i]->getName().c_str() , destNum);
			//provinces[i].setOwner("\"ERR\"");
		}
		else if (sourceNums[0] == 0) {
			continue;
		}
		else
		{
			EU3Province* oldProvince = NULL;
			EU3Country* oldOwner = NULL;
			// determine ownership by province count, or total population (if province count is tied)
			map<string, MTo1ProvinceComp> provinceBins;
			double newProvinceTotalPop = 0;
			for (vector<int>::iterator itr = sourceNums.begin(); itr != sourceNums.end(); ++itr)
			{
				EU3Province* province = sourceWorld.getProvince(*itr);
				if (!province)
				{
					log("Error: old province %d does not exist.  Bad mapping?\n", sourceNums[0]);
					continue;
				}
				EU3Country* owner = province->getOwner();
				string tag;
				if (owner != NULL)
				{
					tag = owner->getTag();
				}
				else
				{
					tag = "";
				}
				if (provinceBins.find(tag) == provinceBins.end())
					provinceBins[tag] = MTo1ProvinceComp();
				provinceBins[tag].provinces.push_back(province);
				provinceBins[tag].totalPopulation += province->getPopulation();
				newProvinceTotalPop += province->getPopulation();
				// I am the new owner if there is no current owner, or I have more provinces than the current owner,
				// or I have the same number of provinces, but more population, than the current owner
				if (   (oldOwner == NULL)
					|| (provinceBins[tag].provinces.size() > provinceBins[oldOwner->getTag()].provinces.size())
					|| ((provinceBins[tag].provinces.size() == provinceBins[oldOwner->getTag()].provinces.size())
					    && (provinceBins[tag].totalPopulation > provinceBins[oldOwner->getTag()].totalPopulation)))
				{
					oldOwner = owner;
					oldProvince = province;
				}
			}

			if (oldOwner != NULL)
			{
				countryMapping::iterator iter = contMap.find(oldOwner->getTag());
				if (iter == contMap.end())
				{
					log("Error: Could not map provinces owned by %s.\n", oldOwner->getTag().c_str());
				}
				else
				{
					provinces[i]->setOwner(iter->second);
					provinces[i]->convertFromOldProvince(oldProvince);

					for (map<string, MTo1ProvinceComp>::iterator mitr = provinceBins.begin(); mitr != provinceBins.end(); ++mitr)
					{
						for (vector<EU3Province*>::iterator vitr = mitr->second.provinces.begin(); vitr != mitr->second.provinces.end(); ++vitr)
						{
							vector<EU3Country*> oldCores = (*vitr)->getCores(sourceWorld.getCountries());
							for(unsigned int j = 0; j < oldCores.size(); j++)
							{
								// skip this core if the country is the owner of the EU3 province but not the V2 province
								// (i.e. "avoid boundary conflicts that didn't exist in EU3").
								// this country may still get core via a province that DID belong to the current V2 owner
								if ((oldCores[j]->getTag() == mitr->first) && (oldCores[j]->getTag() != oldOwner->getTag()))
									continue;

								iter = contMap.find(oldCores[j]->getTag());
								if (iter != contMap.end())
								{
									provinces[i]->addCore(iter->second);
								}
							}

							double provPopRatio = (*vitr)->getPopulation() / newProvinceTotalPop;
							vector<EU3PopRatio> popRatios = (*vitr)->getPopRatios();
							for (vector<EU3PopRatio>::iterator pritr = popRatios.begin(); pritr != popRatios.end(); ++pritr)
							{
								bool matched = false;
								string culture = "";
								for (size_t k = 0; (k < cultureMap.size()) && (!matched); k++)
								{
									if (cultureMap[k].srcCulture == pritr->culture)
									{
										bool match = true;
										for (size_t j = 0; j < cultureMap[k].distinguishers.size(); j++)
										{
											if (cultureMap[k].distinguishers[j].first == DTOwner)
											{
												if ((*vitr)->getOwner()->getTag() != cultureMap[k].distinguishers[j].second)
													match = false;
											}
											else if (cultureMap[k].distinguishers[j].first == DTReligion)
											{
												if (pritr->religion != cultureMap[k].distinguishers[j].second)
													match = false;
											}
											else
											{
												log ("Error: Unhandled distinguisher type in culture rules.\n");
											}

										}
										if (match)
										{
											culture = cultureMap[k].dstCulture;
											matched = true;
										}
									}
								}
								if (!matched)
								{
									log("Error: Could not set culture for pops in province %d\n", destNum);
								}

								string religion = "";
								religionMapping::iterator iter3 = religionMap.find(pritr->religion);
								if (iter3 != religionMap.end())
								{
									religion = iter3->second;
								}
								else
								{
									log("Error: Could not set religion for pops in province %d\n", destNum);
								}

								V2Demographic demographic;
								demographic.culture = culture;
								demographic.religion = religion;
								demographic.ratio = pritr->popRatio * provPopRatio;
								demographic.oldCountry = oldOwner;
								demographic.oldProvince = *vitr;
								
								demographic.literacy = 0.1;
								V2Country* owner = getCountry(provinces[i]->getOwner());
								if ( (owner != NULL) && (owner->getTag() != "") )
								{
									vector<string> acceptedCultures = owner->getAcceptedCultures();
									for (unsigned int l = 0; l < acceptedCultures.size(); l++)
									{
										if (acceptedCultures[l] == culture)
										{
											demographic.literacy = owner->getLiteracy();
										}
									}
									if (owner->getPrimaryCulture() == culture)
									{
										demographic.literacy = owner->getLiteracy();
									}
								}

								demographic.reactionary		= owner->getReactionary();
								demographic.conservative	= owner->getConservative();
								demographic.liberal			= owner->getLiberal();

								list< pair<int, double> > issues;
								vector< pair<int, int> > reactionaryIssues	= owner->getReactionaryIssues();
								int reactionaryTotal = 0;
								for (unsigned int j = 0; j < reactionaryIssues.size(); j++)
								{
									reactionaryTotal += reactionaryIssues[j].second;
								}
								for (unsigned int j = 0; j < reactionaryIssues.size(); j++)
								{
									issues.push_back( make_pair(reactionaryIssues[j].first, (demographic.reactionary * reactionaryIssues[j].second / reactionaryTotal) ) );
								}
								
								vector< pair<int, int> > conservativeIssues	= owner->getConservativeIssues();
								int conservativeTotal = 0;
								for (unsigned int j = 0; j < conservativeIssues.size(); j++)
								{
									conservativeTotal += conservativeIssues[j].second;
								}
								for (unsigned int j = 0; j < conservativeIssues.size(); j++)
								{
									issues.push_back( make_pair(conservativeIssues[j].first, (demographic.conservative * conservativeIssues[j].second / conservativeTotal) ) );
								}

								vector< pair<int, int> > liberalIssues	= owner->getLiberalIssues();
								int liberalTotal = 0;
								for (unsigned int j = 0; j < liberalIssues.size(); j++)
								{
									liberalTotal += liberalIssues[j].second;
								}
								for (unsigned int j = 0; j < liberalIssues.size(); j++)
								{
									issues.push_back( make_pair(liberalIssues[j].first, (demographic.liberal * liberalIssues[j].second / liberalTotal) ) );
								}
								for (list< pair<int, double> >::iterator j = issues.begin(); j != issues.end(); j++)
								{
									list< pair<int, double> >::iterator k = j;
									for (k++; k != issues.end(); k++)
									{
										if (j->first == k->first)
										{
											j->second += k->second;
											issues.erase(k);
											k = j;
											k++;
										}
									}
								}

								for (list< pair<int, double> >::iterator j = issues.begin(); j != issues.end(); j++)
								{
									demographic.issues.push_back(*j);
								}
								
								provinces[i]->addPopDemographic(demographic);
							}

							if ((*vitr)->hasBuilding("fort4") || (*vitr)->hasBuilding("fort5") || (*vitr)->hasBuilding("fort6"))
							{
								provinces[i]->setFortLevel(1);
							}
							// note: HTTT has only shipyard
							if (   (*vitr)->hasBuilding("shipyard") || (*vitr)->hasBuilding("grand_shipyard")
								|| (*vitr)->hasBuilding("naval_arsenal") || (*vitr)->hasBuilding("naval_base"))
							{
								// place naval bases only in port provinces
								vector<int> candidates;
								candidates.push_back(provinces[i]->getNum());
								candidates = getPortProvinces(candidates);
								if (candidates.size() > 0)
								{
									provinces[i]->setNavalBaseLevel(1);
								}
							}
						}
					}
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
			if ( (provinces[i]->hasCulture(unionMap[j].first, 0.5)) && (!provinces[i]->wasPaganConquest()) && (!provinces[i]->wasColonised()) )
			{
				provinces[i]->addCore(unionMap[j].second);
			}
		}
	}
}


static int stateId = 0;
void V2World::setupStates(stateMapping stateMap)
{
	list<V2Province*> unassignedProvs;
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
		unassignedProvs.push_back(*itr);

	list<V2Province*>::iterator iter;
	while(unassignedProvs.size() > 0)
	{
		iter = unassignedProvs.begin();
		int		provId	= (*iter)->getNum();
		string	owner	= (*iter)->getOwner();

		if (owner == "")
		{
			unassignedProvs.erase(iter);
			continue;
		}

		V2State* newState = new V2State(stateId, *iter);
		stateId++;
		vector<int> neighbors	= stateMap[provId];
		bool colonial				= (*iter)->isColonial();
		newState->setColonial(colonial);
		iter = unassignedProvs.erase(iter);

		for (vector<int>::iterator i = neighbors.begin(); i != neighbors.end(); i++)
		{
			for(iter = unassignedProvs.begin(); iter != unassignedProvs.end(); iter++)
			{
				if ((*iter)->getNum() == *i)
				{
					if ((*iter)->getOwner() == owner)
					{
						if ((*iter)->isColonial() == colonial)
						{
							newState->addProvince(*iter);
							iter = unassignedProvs.erase(iter);
						}
					}
				}
			}
		}

		for (vector<V2Country*>::iterator iter2 = countries.begin(); iter2 != countries.end(); iter2++)
		{
			if ( (*iter2)->getTag() == owner)
			{
				(*iter2)->addState(newState);
			}
		}
	}
}


void V2World::convertDiplomacy(EU3World sourceWorld, countryMapping countryMap)
{
	vector<EU3Agreement> agreements = sourceWorld.getDiplomacy()->getAgreements();
	for (vector<EU3Agreement>::iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		countryMapping::iterator newCountry1 = countryMap.find(itr->country1);
		if (newCountry1 == countryMap.end())
		{
			// log("Error: EU3 Country %s used in diplomatic agreement doesn't exist\n", itr->country1.c_str());
			continue;
		}
		countryMapping::iterator newCountry2 = countryMap.find(itr->country2);
		if (newCountry2 == countryMap.end())
		{
			// log("Error: EU3 Country %s used in diplomatic agreement doesn't exist\n", itr->country2.c_str());
			continue;
		}

		V2Country* v2Country1 = NULL;
		V2Country* v2Country2 = NULL;
		for (vector<V2Country*>::iterator citr = countries.begin(); citr != countries.end(); ++citr)
		{
			if ( (*citr)->getTag() == newCountry1->second )
			{
				v2Country1 = *citr;
			}
			else if ( (*citr)->getTag() == newCountry2->second )
			{
				v2Country2 = *citr;
			}
			if (v2Country1 && v2Country2)
				break;
		}
		if (!v2Country1)
		{
			log("	Error: Vic2 country %s used in diplomatic agreement doesn't exist\n", newCountry1->second.c_str());
			continue;
		}
		if (!v2Country2)
		{
			log("	Error: Vic2 country %s used in diplomatic agreement doesn't exist\n", newCountry2->second.c_str());
			continue;
		}
		V2Relations* r1 = v2Country1->getRelations(newCountry2->second);
		if (!r1)
		{
			log("	Error: Vic2 country %s has no relations with %s\n", newCountry1->second.c_str(), newCountry2->second.c_str());
			continue;
		}
		V2Relations* r2 = v2Country2->getRelations(newCountry1->second);
		if (!r2)
		{
			log("	Error: Vic2 country %s has no relations with %s\n", newCountry2->second.c_str(), newCountry1->second.c_str());
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
			v2a.start_date = itr->startDate;
			v2a.type = itr->type;
			diplomacy.addAgreement(v2a);
		}
	}
}


vector<int> V2World::getPortProvinces(vector<int> locationCandidates)
{
	// hack for naval bases.  not ALL naval bases are in port provinces, and if you spawn a navy at a naval base in
	// a non-port province, Vicky crashes....
	static vector<int> port_blacklist;
	if (port_blacklist.size() == 0)
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

	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		vector<int>::iterator black = std::find(port_blacklist.begin(), port_blacklist.end(), *litr);
		if (black != port_blacklist.end())
		{
			locationCandidates.erase(litr);
			break;
		}
	}
	for (vector<V2Province*>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
	{
		for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
		{
			if ( (*pitr)->getNum() == (*litr) )
			{
				if ( !(*pitr)->isCoastal() )
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


//#define TEST_V2_PROVINCES
void V2World::convertArmies(EU3World sourceWorld, inverseProvinceMapping inverseProvinceMap, const map<int,int>& leaderIDMap)
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

	// get cost per regiment values
	double cost_per_regiment[num_reg_categories] = { 0.0 };
	Object*	obj2 = doParseFile("regiment_costs.txt");
	vector<Object*> objTop = obj2->getLeaves();
	if (objTop.size() == 0 || objTop[0]->getLeaves().size() == 0)
	{
		log("	Error: regment_costs.txt failed to parse.");
		printf("	Error: regiment_costs.txt failed to parse.");
		exit(1);
	}
	for (int i = 0; i < num_reg_categories; ++i)
	{
		string regiment_cost = objTop[0]->getLeaf(RegimentCategoryNames[i]);
		cost_per_regiment[i] = atoi(regiment_cost.c_str());
	}

	// convert armies
	map<string, EU3Country*> sourceCountries = sourceWorld.getCountries();
	for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		(*itr)->convertArmies(leaderIDMap, cost_per_regiment, inverseProvinceMap, provinces, port_whitelist);
	}
}


void V2World::convertTechs(EU3World sourceWorld)
{
	map<string, EU3Country*> sourceCountries = sourceWorld.getCountries();
	
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
	map<string, EU3Country*>::iterator i = sourceCountries.begin();
	while (i->second->getProvinces().size() == 0)
	{
		i++;
	}
	highestLand			= oldLandMean			= landMean			= i->second->getLandTech();
	highestNaval		= oldNavalMean			= navalMean			= i->second->getNavalTech();
	highestTrade		= oldTradeMean			= tradeMean			= i->second->getTradeTech();
	highestProduction	= oldProductionMean	= productionMean	= i->second->getProductionTech();
	highestGovernment	= oldGovernmentMean	= governmentMean	= i->second->getGovernmentTech();

	for (i++; i != sourceCountries.end(); i++)
	{
		if (i->second->getProvinces().size() == 0)
		{
			continue;
		}
		double newTech	= i->second->getLandTech();
		landMean			= oldLandMean + ((newTech - oldLandMean) / num);
		newLandS			= oldLandS + ((newTech - oldLandMean) * (newTech - landMean));
		oldLandMean		= landMean; 
		oldLandS			= newLandS;
		if (newTech > highestLand)
		{
			highestLand = newTech;
		}

		newTech			= i->second->getNavalTech();
		navalMean		= oldNavalMean + ((newTech - oldNavalMean) / num);
		newNavalS		= oldNavalS + ((newTech - oldNavalMean) * (newTech - navalMean));
		oldNavalMean	= navalMean; 
		oldNavalS		= newNavalS;
		if (newTech > highestNaval)
		{
			highestNaval = newTech;
		}

		newTech			= i->second->getTradeTech();
		tradeMean		= oldTradeMean + ((newTech - oldTradeMean) / num);
		newTradeS		= oldTradeS + ((newTech - oldTradeMean) * (newTech - tradeMean));
		oldTradeMean	= tradeMean; 
		oldTradeS		= newTradeS;
		if (newTech > highestTrade)
		{
			highestTrade = newTech;
		}

		newTech				= i->second->getProductionTech();
		productionMean		= oldProductionMean + ((newTech - oldProductionMean) / num);
		newProductionS		= oldProductionS + ((newTech - oldProductionMean) * (newTech - productionMean));
		oldProductionMean	= productionMean; 
		oldProductionS		= newProductionS;
		if (newTech > highestProduction)
		{
			highestProduction = newTech;
		}

		newTech				= i->second->getGovernmentTech();
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

	double landScale			= (2.5	* landStdDev)			/ (highestLand			- landMean);
	double navalScale			= (7		* navalStdDev)			/ (highestNaval		- navalMean);
	double tradeScale			= (4.5	* tradeStdDev)			/ (highestTrade		- tradeMean);
	double productionScale	= (3.5	* productionStdDev)	/ (highestProduction	- productionMean);
	double governmentScale	= (3		* governmentStdDev)	/ (highestGovernment	- governmentMean);

	for (unsigned int i = 0; i < countries.size(); i++)
	{
		if( countries[i]->isCivilized() || (Configuration::getV2Gametype() != "AHD") )
		{
			countries[i]->setArmyTech(landMean, landScale, landStdDev);
			countries[i]->setNavyTech(navalMean, navalScale, navalStdDev);
			countries[i]->setCommerceTech(tradeMean, tradeScale, tradeStdDev);
			countries[i]->setIndustryTech(productionMean, productionScale, productionStdDev);
			countries[i]->setCultureTech(governmentMean, governmentScale, governmentStdDev);
		}
	}

	int numRomanticLit = 0;
	int numRomanticArt = 0;
	int numRomanticMusic = 0;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		if (countries[i]->getInventionState(romanticist_literature) == active)
		{
			numRomanticLit++;
		}
		if (countries[i]->getInventionState(romanticist_art) == active)
		{
			numRomanticArt++;
		}
		if (countries[i]->getInventionState(romanticist_music) == active)
		{
			numRomanticMusic++;
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

	for (unsigned int i = 0; i < countries.size(); i++)
	{
		if (countries[i]->getInventionState(romanticist_literature) == active)
		{
			countries[i]->addPrestige(romanticLitPrestige);
		}
		if (countries[i]->getInventionState(romanticist_art) == active)
		{
			countries[i]->addPrestige(romanticArtPrestige);
		}
		if (countries[i]->getInventionState(romanticist_music) == active)
		{
			countries[i]->addPrestige(romanticMusicPrestige);
		}
	}
}


void V2World::allocateFactories(EU3World sourceWorld, V2FactoryFactory& factoryBuilder)
{
	// determine average production tech
	map<string, EU3Country*> sourceCountries = sourceWorld.getCountries();
	double productionMean = 0.0f;
	int num = 1;
	for (map<string, EU3Country*>::iterator itr = sourceCountries.begin(); itr != sourceCountries.end(); ++itr)
	{
		if ( (itr)->second->getProvinces().size() == 0)
			continue;
		if (( (itr)->second->getTechGroup() != "western" ) && ( (itr)->second->getTechGroup() != "latin" ))
			continue;

		double prodTech = (itr)->second->getProductionTech();
		productionMean += ((prodTech - productionMean) / num);
		++num;
	}

	// give all extant civilized nations an industrial score
	deque<pair<double, V2Country*>> weightedCountries;
	for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		if ( !(*itr)->isCivilized() )
			continue;

		const EU3Country* sourceCountry = (*itr)->getSourceCountry();
		if (sourceCountry == NULL)
			continue;

		if (sourceCountry->getProvinces().size() == 0)
			continue;

		// modified manufactory weight follows diminishing returns curve y = x^(3/4)+log((x^2)/5+1)
		int manuCount = sourceCountry->getManufactoryCount();
		double manuWeight = pow(manuCount, 0.75) + log((manuCount * manuCount) / 5.0 + 1.0);
		double industryWeight = (sourceCountry->getProductionTech() - productionMean) + manuWeight;
		// having one manufactory and average tech is not enough; you must have more than one, or above-average tech
		if (industryWeight > 1.0)
		{
			weightedCountries.push_back(pair<double, V2Country*>(industryWeight, *itr));
		}
	}
	if (weightedCountries.size() < 1)
	{
		log("Error: no countries are able to accept factories!\n");
		return;
	}
	sort(weightedCountries.begin(), weightedCountries.end());

	// allow a maximum of 10 (plus any tied at tenth place) countries to recieve factories
	deque<pair<double, V2Country*>> restrictCountries;
	double threshold = 1.0;
	double totalIndWeight = 0.0;
	for (deque<pair<double, V2Country*>>::reverse_iterator itr = weightedCountries.rbegin();
		itr != weightedCountries.rend(); ++itr)
	{
		if ((restrictCountries.size() > 10) && (itr->first < (threshold - FLT_EPSILON)))
			break;
		restrictCountries.push_front(*itr); // preserve sort
		totalIndWeight += itr->first;
		threshold = itr->first;
	}
	weightedCountries.swap(restrictCountries);

	// determine how many factories each eligible nation gets
	deque<V2Factory*> factoryList = factoryBuilder.buildFactories();
	vector<pair<int, V2Country*>> factoryCounts;
	for (deque<pair<double, V2Country*>>::iterator itr = weightedCountries.begin(); itr != weightedCountries.end(); ++itr)
	{
		int factories = int(((itr->first / totalIndWeight) * factoryList.size()) + 0.5 /*round*/);
		log("	%s has industrial weight %2.2lf granting max %d factories\n", itr->second->getTag().c_str(), itr->first, factories);
		factoryCounts.push_back(pair<int, V2Country*>(factories, itr->second));
	}

	// allocate the factories
	vector<pair<int, V2Country*>>::iterator lastReceptiveCountry = factoryCounts.begin();
	vector<pair<int, V2Country*>>::iterator citr = factoryCounts.begin();
	while (factoryList.size() > 0)
	{
		bool accepted = false;
		if (citr->first > 0) // can take more factories
		{
			for (deque<V2Factory*>::iterator qitr = factoryList.begin(); qitr != factoryList.end(); ++qitr)
			{
				if ( citr->second->addFactory(*qitr) )
				{
					--(citr->first);
					lastReceptiveCountry = citr;
					accepted = true;
					factoryList.erase(qitr);
					break;
				}
			}
		}
		if (!accepted && citr == lastReceptiveCountry)
		{
			log("No countries will accept any of the remaining factories!\n");
			for (deque<V2Factory*>::iterator qitr = factoryList.begin(); qitr != factoryList.end(); ++qitr)
			{
				log("\t%s\n", (*qitr)->getTypeName().c_str());
			}
			break;
		}
		if (++citr == factoryCounts.end())
			citr = factoryCounts.begin(); // loop around to beginning
	}
}


void V2World::buildParties()
{
	ifstream V2CountriesInput;
	V2CountriesInput.open( (Configuration::getV2Path() + "\\common\\countries.txt").c_str() );
	if (!V2CountriesInput.is_open())
	{
		log("Error: Could not open countries.txt\n");
		printf("Error: Could not open countries.txt\n");
		exit(1);
	}
	V2Party* emptyParty = new V2Party;
	parties.push_back(emptyParty); // Avoid off by one errors and ugly code everywhere
	while (!V2CountriesInput.eof())
	{
		string line;
		getline(V2CountriesInput, line);

		if ( (line[0] == '#') | (line.size() < 3) )
		{
			continue;
		}
		
		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start		= line.find_first_of('/');
		int size		= line.find_last_of('\"') - start;
		countryFileName	= line.substr(start, size);

		Object* countryData = doParseFile((Configuration::getV2Path() + "\\common\\countries\\" + countryFileName).c_str());

		vector<Object*> partyData = countryData->getValue("party");
		for (vector<Object*>::iterator itr = partyData.begin(); itr != partyData.end(); ++itr)
		{
			V2Party* newParty = new V2Party(*itr);
			parties.push_back(newParty);
		}
	}
	V2CountriesInput.close();
}


void V2World::setupPops(EU3World& sourceWorld)
{
	for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		(*itr)->setupPops(sourceWorld);
	}
}


V2Country* V2World::getCountry(string tag)
{
	vector<V2Country*>::iterator itr;

	for (itr = countries.begin(); itr != countries.end(); itr++)
	{
		if ( 0 == (tag.compare( (*itr)->getTag() )) )
		{
			return *itr;
		}
	}

	return NULL;
}


V2Party* V2World::getParty(int index)
{
	 return parties[index];
}


void V2World::output(FILE* output)
{
	outputHeader(output);
	outputTempHeader(output);
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		provinces[i]->sortPops();
		provinces[i]->output(output);
	}
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		countries[i]->output(output);
	}
	diplomacy.output(output);
}


void V2World::outputHeader(FILE* output)
{
	fprintf(output, "date=\"%s\"\n", Configuration::getStartDate().toString().c_str());
	fprintf(output, "automate_trade=no\n");
	fprintf(output, "automate_sliders=0\n");
	fprintf(output, "unit=%d\n", V2ArmyID().id);
	fprintf(output, "state=%d\n", stateId);
	fprintf(output, "start_date=\"%s\"\n", Configuration::getStartDate().toString().c_str());
	fprintf(output, "start_pop_index=%d\n", getNextPopId());
	fprintf(output, "worldmarket=\n");
	fprintf(output, "{\n");
	fprintf(output, "	worldmarket_pool=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=0.46634\n");
	fprintf(output, "		small_arms=1.01407\n");
	fprintf(output, "		artillery=1.43967\n");
	fprintf(output, "		canned_food=4.16602\n");
	fprintf(output, "		cotton=966.92960\n");
	fprintf(output, "		dye=785.78574\n");
	fprintf(output, "		wool=1794.33109\n");
	fprintf(output, "		silk=192.24536\n");
	fprintf(output, "		coal=1698.31195\n");
	fprintf(output, "		sulphur=201.85480\n");
	fprintf(output, "		iron=498.86673\n");
	fprintf(output, "		timber=1038.01337\n");
	fprintf(output, "		tropical_wood=258.42496\n");
	fprintf(output, "		precious_metal=17.16943\n");
	fprintf(output, "		steel=395.23450\n");
	fprintf(output, "		cement=3.23746\n");
	fprintf(output, "		machine_parts=3.84375\n");
	fprintf(output, "		glass=52.54648\n");
	fprintf(output, "		fertilizer=66.62204\n");
	fprintf(output, "		explosives=18.46304\n");
	fprintf(output, "		clipper_convoy=0.05063\n");
	fprintf(output, "		steamer_convoy=2.83932\n");
	fprintf(output, "		fabric=139.08908\n");
	fprintf(output, "		lumber=101.34860\n");
	fprintf(output, "		paper=3.35300\n");
	fprintf(output, "		cattle=813.88202\n");
	fprintf(output, "		fish=1535.25745\n");
	fprintf(output, "		fruit=2686.31516\n");
	fprintf(output, "		grain=4360.54388\n");
	fprintf(output, "		tobacco=2200.08780\n");
	fprintf(output, "		tea=2673.66977\n");
	fprintf(output, "		coffee=1334.11459\n");
	fprintf(output, "		opium=1118.43161\n");
	fprintf(output, "		wine=7.32648\n");
	fprintf(output, "		liquor=0.69968\n");
	fprintf(output, "		regular_clothes=34.41812\n");
	fprintf(output, "		luxury_clothes=0.40475\n");
	fprintf(output, "		furniture=0.29919\n");
	fprintf(output, "		luxury_furniture=0.38611\n");
	fprintf(output, "	}\n");
	fprintf(output, "	price_pool=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=17.52002\n");
	fprintf(output, "		small_arms=37.00000\n");
	fprintf(output, "		artillery=60.00000\n");
	fprintf(output, "		canned_food=16.00000\n");
	fprintf(output, "		barrels=98.00000\n");
	fprintf(output, "		aeroplanes=110.00000\n");
	fprintf(output, "		cotton=1.97998\n");
	fprintf(output, "		dye=11.97998\n");
	fprintf(output, "		wool=0.67999\n");
	fprintf(output, "		silk=10.00000\n");
	fprintf(output, "		coal=2.28998\n");
	fprintf(output, "		sulphur=6.02002\n");
	fprintf(output, "		iron=3.47998\n");
	fprintf(output, "		timber=0.89999\n");
	fprintf(output, "		tropical_wood=5.37997\n");
	fprintf(output, "		rubber=7.00000\n");
	fprintf(output, "		oil=12.00000\n");
	fprintf(output, "		precious_metal=7.50000\n");
	fprintf(output, "		steel=4.67999\n");
	fprintf(output, "		cement=15.97998\n");
	fprintf(output, "		machine_parts=36.47998\n");
	fprintf(output, "		glass=2.92001\n");
	fprintf(output, "		fuel=25.00000\n");
	fprintf(output, "		fertilizer=10.00000\n");
	fprintf(output, "		explosives=20.02002\n");
	fprintf(output, "		clipper_convoy=42.00000\n");
	fprintf(output, "		steamer_convoy=64.97998\n");
	fprintf(output, "		electric_gear=16.00000\n");
	fprintf(output, "		fabric=1.82001\n");
	fprintf(output, "		lumber=1.02002\n");
	fprintf(output, "		paper=3.42001\n");
	fprintf(output, "		cattle=1.97998\n");
	fprintf(output, "		fish=1.47998\n");
	fprintf(output, "		fruit=1.77997\n");
	fprintf(output, "		grain=2.17999\n");
	fprintf(output, "		tobacco=1.10001\n");
	fprintf(output, "		tea=2.60001\n");
	fprintf(output, "		coffee=2.07999\n");
	fprintf(output, "		opium=3.20001\n");
	fprintf(output, "		automobiles=70.00000\n");
	fprintf(output, "		telephones=16.00000\n");
	fprintf(output, "		wine=9.72003\n");
	fprintf(output, "		liquor=6.42001\n");
	fprintf(output, "		regular_clothes=5.82001\n");
	fprintf(output, "		luxury_clothes=65.02002\n");
	fprintf(output, "		furniture=4.92001\n");
	fprintf(output, "		luxury_furniture=59.02002\n");
	fprintf(output, "		radio=16.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	last_price_history=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	supply_pool=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	last_supply_pool=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	price_change=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	discovered_goods=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		ammunition=1.00000\n");
	fprintf(output, "		small_arms=1.00000\n");
	fprintf(output, "		artillery=1.00000\n");
	fprintf(output, "		canned_food=1.00000\n");
	fprintf(output, "		cotton=1.00000\n");
	fprintf(output, "		dye=1.00000\n");
	fprintf(output, "		wool=1.00000\n");
	fprintf(output, "		silk=1.00000\n");
	fprintf(output, "		coal=1.00000\n");
	fprintf(output, "		sulphur=1.00000\n");
	fprintf(output, "		iron=1.00000\n");
	fprintf(output, "		timber=1.00000\n");
	fprintf(output, "		tropical_wood=1.00000\n");
	fprintf(output, "		rubber=1.00000\n");
	fprintf(output, "		oil=1.00000\n");
	fprintf(output, "		precious_metal=1.00000\n");
	fprintf(output, "		steel=1.00000\n");
	fprintf(output, "		cement=1.00000\n");
	fprintf(output, "		machine_parts=1.00000\n");
	fprintf(output, "		glass=1.00000\n");
	fprintf(output, "		fertilizer=1.00000\n");
	fprintf(output, "		explosives=1.00000\n");
	fprintf(output, "		clipper_convoy=1.00000\n");
	fprintf(output, "		steamer_convoy=1.00000\n");
	fprintf(output, "		fabric=1.00000\n");
	fprintf(output, "		lumber=1.00000\n");
	fprintf(output, "		paper=1.00000\n");
	fprintf(output, "		cattle=1.00000\n");
	fprintf(output, "		fish=1.00000\n");
	fprintf(output, "		fruit=1.00000\n");
	fprintf(output, "		grain=1.00000\n");
	fprintf(output, "		tobacco=1.00000\n");
	fprintf(output, "		tea=1.00000\n");
	fprintf(output, "		coffee=1.00000\n");
	fprintf(output, "		opium=1.00000\n");
	fprintf(output, "		wine=1.00000\n");
	fprintf(output, "		liquor=1.00000\n");
	fprintf(output, "		regular_clothes=1.00000\n");
	fprintf(output, "		luxury_clothes=1.00000\n");
	fprintf(output, "		furniture=1.00000\n");
	fprintf(output, "		luxury_furniture=1.00000\n");
	fprintf(output, "	}\n");
	fprintf(output, "	actual_sold=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	actual_sold_world=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	real_demand=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	demand=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "	player_balance=\n");
	fprintf(output, "	{\n");
	fprintf(output, "	}\n");
	fprintf(output, "}\n");
}


map<string, V2Country*> V2World::getPotentialCountries() const
{
	map<string, V2Country*> retVal;
	for (vector<V2Country*>::const_iterator i = potentialCountries.begin(); i != potentialCountries.end(); i++)
	{
		retVal[ (*i)->getTag() ] = *i;
	}

	return retVal;
}
