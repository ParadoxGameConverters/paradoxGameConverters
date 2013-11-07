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


typedef struct fileWithCreateTime
{
	string	filename;
	time_t	createTime;
	bool operator < (const fileWithCreateTime &rhs) const
	{
		return createTime < rhs.createTime;
	};
} fileWithCreateTime;


V2World::V2World(string V2Loc)
{
	log("\tImporting provinces.\n");
	printf("\tImporting provinces.\n");
	
	// set province names and numbers
	printf("\tSetting numbers.\n");
	provinces.clear();
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
	printf("\tSetting names.\n");
	vector<fileWithCreateTime> localisationFiles;

	struct _finddata_t	localisationData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (V2Loc + "\\localisation\\*.csv").c_str(), &localisationData)) == -1L)
	{
		log("Error: Could not open localisation directory.\n");
		return;
	}
	do
	{
		if (strcmp(localisationData.name, ".") == 0 || strcmp(localisationData.name, "..") == 0 )
		{
			continue;
		}
		fileWithCreateTime newFile;
		newFile.filename		= localisationData.name;
		newFile.createTime	= localisationData.time_create;
		localisationFiles.push_back(newFile);
	} while(_findnext(fileListing, &localisationData) == 0);
	_findclose(fileListing);

	sort(localisationFiles.begin(), localisationFiles.end());
	for (vector<fileWithCreateTime>::iterator i = localisationFiles.begin(); i < localisationFiles.end(); i++)
	{
		getProvinceLocalizations(V2Loc + "\\localisation\\" + i->filename);
	}

	// set province rgo types and life ratings
	printf("\tSetting rgo types and life ratings.\n");
	struct _finddata_t	provDirData;
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

			vector<V2Province*>::iterator i;
			for(i = provinces.begin(); i != provinces.end(); i++)
			{
				if ( (*i)->getNum() == provNum )
				{
					Object* provinceObj = doParseFile((provDirPath + provFileData.name).c_str());
					if (provinceObj == NULL)
					{
						log("Could not parse file %s\n", (provDirPath + provFileData.name).c_str());
						exit(-1);
					}
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
	log("\tImporting historical pops.\n");
	printf("\tImporting historical pops.\n");
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
	log("\tFinding coastal provinces.\n");
	printf("\tFinding coastal provinces.\n");
	Object*	obj2 = doParseFile((V2Loc + "\\map\\positions.txt").c_str());
	if (obj2 == NULL)
	{
		log("Could not parse file %s\n", (V2Loc + "\\map\\positions.txt").c_str());
		exit(-1);
	}
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

	countries.clear();

	log("\tGetting potential countries and building political parties.\n");
	printf("\tGetting potential countries and building political parties.\n");
	parties.clear();
	potentialCountries.clear();
	const date FirstStartDate = Configuration::getStartDate();
	ifstream V2CountriesInput;
	V2CountriesInput.open( (Configuration::getV2Path() + "\\common\\countries.txt").c_str() );
	if (!V2CountriesInput.is_open())
	{
		log("Error: Could not open countries.txt\n");
		printf("Error: Could not open countries.txt\n");
		exit(1);
	}

	int partiesIndex = 1;
	while (!V2CountriesInput.eof())
	{
		string line;
		getline(V2CountriesInput, line);

		if ( (line[0] == '#') || (line.size() < 3) || (line.substr(0,12) == "dynamic_tags") )
		{
			continue;
		}
		
		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start			= line.find_first_of('/');
		int size				= line.find_last_of('\"') - start;
		countryFileName	= line.substr(start, size);

		Object* countryData = doParseFile((Configuration::getV2Path() + "\\common\\countries\\" + countryFileName).c_str());
		if (countryData == NULL)
		{
			log("Could not parse file %s\n", (Configuration::getV2Path() + "\\common\\countries\\" + countryFileName).c_str());
			exit(-1);
		}

		vector<Object*> partyData = countryData->getValue("party");
		map<int, V2Party*> localParties;
		for (vector<Object*>::iterator itr = partyData.begin(); itr != partyData.end(); ++itr)
		{
			V2Party* newParty = new V2Party(*itr);
			parties.insert(make_pair(partiesIndex, newParty));
			localParties.insert(make_pair(partiesIndex, newParty));
			partiesIndex++;
		}

		if (tag == "REB")
		{
			continue;
		}
		V2Country* newCountry = new V2Country(tag, countryFileName, localParties, this);
		potentialCountries.push_back(newCountry);
	}
	V2CountriesInput.close();

	equalityLeft	= 6;
	libertyLeft		= 30;
}


void V2World::output(FILE* output) const
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


static int stateId = 0;
void V2World::outputHeader(FILE* output) const
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


void V2World::convertCountries(const EU3World& sourceWorld, const countryMapping& countryMap, const cultureMapping& cultureMap, const unionCulturesMap& unionCultures, const religionMapping& religionMap, const governmentMapping& governmentMap, const inverseProvinceMapping& inverseProvinceMap, const vector<techSchool>& techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt)
{
	vector<string> outputOrder;
	outputOrder.clear();
	for (unsigned int i = 0; i < potentialCountries.size(); i++)
	{
		outputOrder.push_back(potentialCountries[i]->getTag());
	}

	map<string, EU3Country*> sourceCountries = sourceWorld.getCountries();
	for (map<string, EU3Country*>::iterator i = sourceCountries.begin(); i != sourceCountries.end(); i++)
	{
		V2Country* newCountry;
		countryMapping::const_iterator iter;
		iter = countryMap.find(i->second->getTag());
		if (iter != countryMap.end())
		{
			for(vector<V2Country*>::iterator j = potentialCountries.begin(); j != potentialCountries.end(); j++)
			{
				if ( (*j)->getTag() == iter->second.c_str() )
				{
					newCountry = *j;
					newCountry->initFromEU3Country(i->second, outputOrder, countryMap, cultureMap, religionMap, unionCultures, governmentMap, inverseProvinceMap, techSchools, leaderMap, lt);
					newCountry->setNationalIdea(libertyLeft, equalityLeft);
					break;
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
			(*itr)->initFromHistory();
			countries.push_back(*itr);
		}
	}

	// put countries in the same order as potentialCountries was (this is the same order V2 will save them in)
	vector<V2Country*> sortedCountries;
	for (vector<string>::const_iterator oitr = outputOrder.begin(); oitr != outputOrder.end(); ++oitr)
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


void V2World::convertDiplomacy(const EU3World& sourceWorld, const countryMapping& countryMap)
{
	vector<EU3Agreement> agreements = sourceWorld.getDiplomacy()->getAgreements();
	for (vector<EU3Agreement>::iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		countryMapping::const_iterator newCountry1 = countryMap.find(itr->country1);
		if (newCountry1 == countryMap.end())
		{
			// log("Error: EU3 Country %s used in diplomatic agreement doesn't exist\n", itr->country1.c_str());
			continue;
		}
		countryMapping::const_iterator newCountry2 = countryMap.find(itr->country2);
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


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<EU3Province*> provinces;
	int totalPopulation;
};


void V2World::convertProvinces(const EU3World& sourceWorld, const provinceMapping& provinceMap, const countryMapping& countryMap, const cultureMapping& cultureMap, const religionMapping& religionMap)
{
	for (vector<V2Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		int destNum												= (*i)->getNum();
		provinceMapping::const_iterator provinceLink	= provinceMap.find(destNum);
		if ( (provinceLink == provinceMap.end()) || (provinceLink->second.size() == 0) )
		{
			log("Error: no source for %s (province #%d)\n", (*i)->getName().c_str() , destNum);
			continue;
		}
		else if (provinceLink->second[0] == 0) {
			continue;
		}

		EU3Province* oldProvince	= NULL;
		EU3Country* oldOwner			= NULL;
		// determine ownership by province count, or total population (if province count is tied)
		map<string, MTo1ProvinceComp> provinceBins;
		double newProvinceTotalPop = 0;
		for (vector<int>::const_iterator itr = provinceLink->second.begin(); itr != provinceLink->second.end(); ++itr)
		{
			EU3Province* province = sourceWorld.getProvince(*itr);
			if (!province)
			{
				log("Error: old province %d does not exist. Bad mapping?\n", provinceLink->second[0]);
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
			{
				provinceBins[tag] = MTo1ProvinceComp();
			}
			provinceBins[tag].provinces.push_back(province);
			provinceBins[tag].totalPopulation += province->getPopulation();
			newProvinceTotalPop += province->getPopulation();
			// I am the new owner if there is no current owner, or I have more provinces than the current owner,
			// or I have the same number of provinces, but more population, than the current owner
			if (	(oldOwner == NULL)
				|| (provinceBins[tag].provinces.size() > provinceBins[oldOwner->getTag()].provinces.size())
				|| ((provinceBins[tag].provinces.size() == provinceBins[oldOwner->getTag()].provinces.size())
					   && (provinceBins[tag].totalPopulation > provinceBins[oldOwner->getTag()].totalPopulation)))
			{
				oldOwner = owner;
				oldProvince = province;
			}
		}
		if (oldOwner == NULL)
		{
			continue;
		}

		countryMapping::const_iterator iter = countryMap.find(oldOwner->getTag());
		if (iter == countryMap.end())
		{
			log("Error: Could not map provinces owned by %s.\n", oldOwner->getTag().c_str());
		}
		else
		{
			(*i)->setOwner(iter->second);
			(*i)->convertFromOldProvince(oldProvince);

			for (map<string, MTo1ProvinceComp>::iterator mitr = provinceBins.begin(); mitr != provinceBins.end(); ++mitr)
			{
				for (vector<EU3Province*>::iterator vitr = mitr->second.provinces.begin(); vitr != mitr->second.provinces.end(); ++vitr)
				{
					// assign cores
					vector<EU3Country*> oldCores = (*vitr)->getCores(sourceWorld.getCountries());
					for(vector<EU3Country*>::iterator j = oldCores.begin(); j != oldCores.end(); j++)
					{
						// skip this core if the country is the owner of the EU3 province but not the V2 province
						// (i.e. "avoid boundary conflicts that didn't exist in EU3").
						// this country may still get core via a province that DID belong to the current V2 owner
						if (( (*j)->getTag() == mitr->first) && ( (*j)->getTag() != oldOwner->getTag()))
						{
							continue;
						}

						iter = countryMap.find( (*j)->getTag());
						if (iter != countryMap.end())
						{
							(*i)->addCore(iter->second);
						}
					}

					// determine demographics
					double provPopRatio = (*vitr)->getPopulation() / newProvinceTotalPop;
					vector<EU3PopRatio> popRatios = (*vitr)->getPopRatios();
					for (vector<EU3PopRatio>::iterator prItr = popRatios.begin(); prItr != popRatios.end(); ++prItr)
					{
						bool matched = false;
						string culture = "";
						for (cultureMapping::const_iterator cultureItr = cultureMap.begin(); (cultureItr != cultureMap.end()) && (!matched); cultureItr++)
						{
							if (cultureItr->srcCulture == prItr->culture)
							{
								bool match = true;
								for (vector<distinguisher>::const_iterator distiguisherItr = cultureItr->distinguishers.begin(); distiguisherItr != cultureItr->distinguishers.end(); distiguisherItr++)
								{
									if (distiguisherItr->first == DTOwner)
									{
										if ((*vitr)->getOwner()->getTag() != distiguisherItr->second)
											match = false;
									}
									else if (distiguisherItr->first == DTReligion)
									{
										if (prItr->religion != distiguisherItr->second)
											match = false;
									}
									else
									{
										log ("Error: Unhandled distinguisher type in culture rules.\n");
									}

								}
								if (match)
								{
									culture = cultureItr->dstCulture;
									matched = true;
								}
							}
						}
						if (!matched)
						{
							log("Error: Could not set culture for pops in province %d\n", destNum);
						}

						string religion = "";
						religionMapping::const_iterator religionItr = religionMap.find(prItr->religion);
						if (religionItr != religionMap.end())
						{
							religion = religionItr->second;
						}
						else
						{
							log("Error: Could not set religion for pops in province %d\n", destNum);
						}

						V2Demographic demographic;
						demographic.culture		= culture;
						demographic.religion		= religion;
						demographic.ratio			= prItr->popRatio * provPopRatio;
						demographic.oldCountry	= oldOwner;
						demographic.oldProvince	= *vitr;
								
						demographic.literacy = 0.1;
						V2Country* owner = getCountry( (*i)->getOwner());
						if ( (owner != NULL) && (owner->getTag() != "") )
						{
							if (owner->getPrimaryCulture() == culture)
							{
								demographic.literacy = owner->getLiteracy();
							}
							else
							{
								vector<string> acceptedCultures = owner->getAcceptedCultures();
								for (vector<string>::iterator acItr = acceptedCultures.begin(); acItr < acceptedCultures.end(); acItr++)
								{
									if ( *acItr == culture)
									{
										demographic.literacy = owner->getLiteracy();
									}
								}
							}
						}

						demographic.reactionary		= owner->getReactionary();
						demographic.conservative	= owner->getConservative();
						demographic.liberal			= owner->getLiberal();

						list< pair<int, double> > issues;
						vector< pair<int, int> > reactionaryIssues = owner->getReactionaryIssues();
						int reactionaryTotal = 0;
						for (unsigned int j = 0; j < reactionaryIssues.size(); j++)
						{
							reactionaryTotal += reactionaryIssues[j].second;
						}
						for (unsigned int j = 0; j < reactionaryIssues.size(); j++)
						{
							issues.push_back(make_pair(reactionaryIssues[j].first, (demographic.reactionary * reactionaryIssues[j].second / reactionaryTotal) ));
						}
								
						vector< pair<int, int> > conservativeIssues	= owner->getConservativeIssues();
						int conservativeTotal = 0;
						for (unsigned int j = 0; j < conservativeIssues.size(); j++)
						{
							conservativeTotal += conservativeIssues[j].second;
						}
						for (unsigned int j = 0; j < conservativeIssues.size(); j++)
						{
							issues.push_back(make_pair(conservativeIssues[j].first, (demographic.conservative * conservativeIssues[j].second / conservativeTotal) ));
						}

						vector< pair<int, int> > liberalIssues	= owner->getLiberalIssues();
						int liberalTotal = 0;
						for (unsigned int j = 0; j < liberalIssues.size(); j++)
						{
							liberalTotal += liberalIssues[j].second;
						}
						for (unsigned int j = 0; j < liberalIssues.size(); j++)
						{
							issues.push_back(make_pair(liberalIssues[j].first, (demographic.liberal * liberalIssues[j].second / liberalTotal) ));
						}
						for (list< pair<int, double> >::iterator j = issues.begin(); j != issues.end(); j++) // TODO: replace with a better algorithm
						{
							list< pair<int, double> >::iterator k = j;
							for (k++; k != issues.end(); k++)
							{
								if (j->first == k->first)
								{
									j->second += k->second;
									issues.erase(k);
									j = issues.begin();
									k = j;
									k++;
								}
							}
						}

						for (list< pair<int, double> >::iterator j = issues.begin(); j != issues.end(); j++)
						{
							demographic.issues.push_back(*j);
						}
								
						(*i)->addPopDemographic(demographic);
					}

					// set forts and naval bases
					if ((*vitr)->hasBuilding("fort4") || (*vitr)->hasBuilding("fort5") || (*vitr)->hasBuilding("fort6"))
					{
						(*i)->setFortLevel(1);
					}
					// note: HTTT has only shipyard
					if (   (*vitr)->hasBuilding("shipyard") || (*vitr)->hasBuilding("grand_shipyard")
						|| (*vitr)->hasBuilding("naval_arsenal") || (*vitr)->hasBuilding("naval_base"))
					{
						// place naval bases only in port provinces
						vector<int> candidates;
						candidates.push_back( (*i)->getNum() );
						candidates = getPortProvinces(candidates);
						if (candidates.size() > 0)
						{
							(*i)->setNavalBaseLevel(1);
						}
					}
				}
			}
		}
	}
}


void V2World::setupStates(const stateMapping& stateMap)
{
	list<V2Province*> unassignedProvs;
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		unassignedProvs.push_back(*itr);
	}

	list<V2Province*>::iterator iter;
	while(unassignedProvs.size() > 0)
	{
		iter = unassignedProvs.begin();
		int		provId	= (*iter)->getNum();
		string	owner		= (*iter)->getOwner();

		if (owner == "")
		{
			unassignedProvs.erase(iter);
			continue;
		}

		V2State* newState = new V2State(stateId, *iter);
		stateId++;
		stateMapping::const_iterator stateItr = stateMap.find(provId);
		vector<int> neighbors;
		if (stateItr != stateMap.end())
		{
			neighbors = stateItr->second;
		}
		bool colonised = (*iter)->wasColonised();
		newState->setColonised(colonised);
		iter = unassignedProvs.erase(iter);

		for (vector<int>::iterator i = neighbors.begin(); i != neighbors.end(); i++)
		{
			for(iter = unassignedProvs.begin(); iter != unassignedProvs.end(); iter++)
			{
				if ((*iter)->getNum() == *i)
				{
					if ((*iter)->getOwner() == owner)
					{
						if ((*iter)->wasColonised() == colonised)
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


void V2World::convertUncivReforms()
{
	for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		(*itr)->convertUncivReforms();
	}
}



void V2World::setupPops(EU3World& sourceWorld)
{
	for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		(*itr)->setupPops(sourceWorld);
	}
}


void V2World::addUnions(const unionMapping& unionMap)
{
	for (vector<V2Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		for (unionMapping::const_iterator unionItr = unionMap.begin(); unionItr != unionMap.end(); unionItr++)
		{
			if ( (*provItr)->hasCulture(unionItr->first, 0.5) && !(*provItr)->wasPaganConquest() && !(*provItr)->wasColonised() )
			{
				(*provItr)->addCore(unionItr->second);
			}
		}
	}
}


//#define TEST_V2_PROVINCES
void V2World::convertArmies(const EU3World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const map<int,int>& leaderIDMap)
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
	if (obj2 == NULL)
	{
		log("Could not parse file regiment_costs.txt\n");
		exit(-1);
	}
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
	for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		(*itr)->convertArmies(leaderIDMap, cost_per_regiment, inverseProvinceMap, provinces, port_whitelist);
	}
}


void V2World::convertTechs(const EU3World& sourceWorld)
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
		if( (Configuration::getV2Gametype() == "vanilla") || countries[i]->isCivilized() )
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
	if (Configuration::getV2Gametype() != "HOD")
	{
		for (unsigned int i = 0; i < countries.size(); i++)
		{
			if (countries[i]->getInventionState(VANILLA_romanticist_literature) == active)
			{
				numRomanticLit++;
			}
			if (countries[i]->getInventionState(VANILLA_romanticist_literature) == active)
			{
				numRomanticArt++;
			}
			if (countries[i]->getInventionState(VANILLA_romanticist_literature) == active)
			{
				numRomanticMusic++;
			}
		}
	}
	else if (Configuration::getV2Gametype() == "HOD")
	{
		for (unsigned int i = 0; i < countries.size(); i++)
		{
			if (countries[i]->getInventionState(HOD_romanticist_literature) == active)
			{
				numRomanticLit++;
			}
			if (countries[i]->getInventionState(HOD_romanticist_literature) == active)
			{
				numRomanticArt++;
			}
			if (countries[i]->getInventionState(HOD_romanticist_literature) == active)
			{
				numRomanticMusic++;
			}
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

	if (Configuration::getV2Gametype() != "HOD")
	{
		for (unsigned int i = 0; i < countries.size(); i++)
		{
			if (countries[i]->getInventionState(VANILLA_romanticist_literature) == active)
			{
				countries[i]->addPrestige(romanticLitPrestige);
			}
			if (countries[i]->getInventionState(VANILLA_romanticist_art) == active)
			{
				countries[i]->addPrestige(romanticArtPrestige);
			}
			if (countries[i]->getInventionState(VANILLA_romanticist_music) == active)
			{
				countries[i]->addPrestige(romanticMusicPrestige);
			}
		}
	}
	else if (Configuration::getV2Gametype() == "HOD")
	{
		for (unsigned int i = 0; i < countries.size(); i++)
		{
			if (countries[i]->getInventionState(HOD_romanticist_literature) == active)
			{
				countries[i]->addPrestige(romanticLitPrestige);
			}
			if (countries[i]->getInventionState(HOD_romanticist_art) == active)
			{
				countries[i]->addPrestige(romanticArtPrestige);
			}
			if (countries[i]->getInventionState(HOD_romanticist_music) == active)
			{
				countries[i]->addPrestige(romanticMusicPrestige);
			}
		}
	}
}


void V2World::allocateFactories(const EU3World& sourceWorld, const V2FactoryFactory& factoryBuilder)
{
	// determine average production tech
	map<string, EU3Country*> sourceCountries = sourceWorld.getCountries();
	double productionMean = 0.0f;
	int num = 1;
	for (map<string, EU3Country*>::iterator itr = sourceCountries.begin(); itr != sourceCountries.end(); ++itr)
	{
		if ( (itr)->second->getProvinces().size() == 0)
		{
			continue;
		}
		if (( (itr)->second->getTechGroup() != "western" ) && ( (itr)->second->getTechGroup() != "latin" ))
		{
			continue;
		}

		double prodTech = (itr)->second->getProductionTech();
		productionMean += ((prodTech - productionMean) / num);
		++num;
	}

	// give all extant civilized nations an industrial score
	deque<pair<double, V2Country*>> weightedCountries;
	for (vector<V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		if ( !(*itr)->isCivilized() )
		{
			continue;
		}

		const EU3Country* sourceCountry = (*itr)->getSourceCountry();
		if (sourceCountry == NULL)
		{
			continue;
		}

		if (sourceCountry->getProvinces().size() == 0)
		{
			continue;
		}

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
		{
			break;
		}
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
		{
			citr = factoryCounts.begin(); // loop around to beginning
		}
	}
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
