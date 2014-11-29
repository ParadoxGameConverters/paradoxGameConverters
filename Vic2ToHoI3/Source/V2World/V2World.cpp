/*Copyright (c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "V2World.h"
#include <Windows.h>
#include <fstream>
#include <algorithm>
#include <io.h>
#include <list>
#include <queue>
#include <cmath>
#include <cfloat>
#include <sys/stat.h>
#include "../Parsers/Parser.h"
#include "../Log.h"
#include "../Configuration.h"
#include "../WinUtils.h"
#include "../EU4World/EU4World.h"
#include "../EU4World/EU4Relations.h"
#include "../EU4World/EU4Leader.h"
#include "../EU4World/EU4Province.h"
#include "../EU4World/EU4Diplomacy.h"
#include "V2Province.h"
#include "V2State.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Leader.h"
#include "V2Pop.h"
#include "V2Country.h"
#include "V2Reforms.h"
#include "V2Flags.h"
#include "V2LeaderTraits.h"
#include "V2Version.h"



typedef struct fileWithCreateTime
{
	string	filename;
	time_t	createTime;
	bool operator < (const fileWithCreateTime &rhs) const
	{
		return createTime < rhs.createTime;
	};
} fileWithCreateTime;



V2World::V2World()
{
	LOG(LogLevel::Info) << "Importing provinces";

	struct _finddata_t	provinceFileData;
	intptr_t					fileListing	= NULL;
	list<string>			directories;
	directories.push_back("");

	struct _stat st;
	if (_stat(".\\blankMod\\output\\history\\provinces\\", &st) == 0)
	{
		while (directories.size() > 0)
		{
			if ((fileListing = _findfirst((string(".\\blankMod\\output\\history\\provinces") + directories.front() + "\\*.*").c_str(), &provinceFileData)) == -1L)
			{
				LOG(LogLevel::Error) << "Could not open directory .\\blankMod\\output\\history\\provinces" << directories.front() << "\\*.*";
				exit(-1);
			}

			do
			{
				if (strcmp(provinceFileData.name, ".") == 0 || strcmp(provinceFileData.name, "..") == 0)
				{
					continue;
				}
				if (provinceFileData.attrib & _A_SUBDIR)
				{
					string newDirectory = directories.front() + "\\" + provinceFileData.name;
					directories.push_back(newDirectory);
				}
				else
				{
					V2Province* newProvince = new V2Province(directories.front() + "\\" + provinceFileData.name);
					provinces.insert(make_pair(newProvince->getNum(), newProvince));
				}
			} while (_findnext(fileListing, &provinceFileData) == 0);
			_findclose(fileListing);
			directories.pop_front();
		}
	}
	else
	{
		while (directories.size() > 0)
		{
			if ((fileListing = _findfirst((Configuration::getV2Path() + "\\history\\provinces" + directories.front() + "\\*.*").c_str(), &provinceFileData)) == -1L)
			{
				LOG(LogLevel::Error) << "Could not open directory " << Configuration::getV2Path() << "\\history\\provinces" << directories.front() << "\\*.*";
				exit(-1);
			}

			do
			{
				if (strcmp(provinceFileData.name, ".") == 0 || strcmp(provinceFileData.name, "..") == 0)
				{
					continue;
				}
				if (provinceFileData.attrib & _A_SUBDIR)
				{
					string newDirectory = directories.front() + "\\" + provinceFileData.name;
					directories.push_back(newDirectory);
				}
				else
				{
					V2Province* newProvince = new V2Province(directories.front() + "\\" + provinceFileData.name);
					provinces.insert(make_pair(newProvince->getNum(), newProvince));
				}
			} while (_findnext(fileListing, &provinceFileData) == 0);
			_findclose(fileListing);
			directories.pop_front();
		}
	}
	
	// set V2 basic population levels
	//log("\tImporting historical pops.\n");
	//printf("\tImporting historical pops.\n");
	//struct _finddata_t	popsFileData;
	//if ( (fileListing = _findfirst( (V2Loc + "\\history\\pops\\1836.1.1\\*").c_str(), &popsFileData)) == -1L)
	//{
	//	log("Could not open pops files.\n");
	//	return;
	//}
	//do
	//{
	//	if (strcmp(popsFileData.name, ".") == 0 || strcmp(popsFileData.name, "..") == 0 )
	//	{
	//		continue;
	//	}

	//	Object*	obj2;				// generic object
	//	ifstream	read;				// ifstream for reading files
	//	initParser();
	//	obj2 = getTopLevel();
	//	read.open( (V2Loc + "\\history\\pops\\1836.1.1\\" + popsFileData.name).c_str() );
	//	if (!read.is_open())
	//	{
	//		log("Error: Could not open %s\n", popsFileData.name);
	//		printf("Error: Could not open %s\n", popsFileData.name);
	//		read.close();
	//		read.clear();
	//		continue;
	//	}
	//	readFile(read);
	//	read.close();
	//	read.clear();
	//
	//	vector<Object*> leaves = obj2->getLeaves();
	//	for (unsigned int j = 0; j < leaves.size(); j++)
	//	{
	//		int provNum = atoi(leaves[j]->getKey().c_str());
	//		unsigned int k = 0;
	//		while (k < provinces.size() && provNum != provinces[k]->getNum())
	//		{
	//			k++;
	//		}
	//		if (k == provinces.size())
	//		{
	//			log("Could not find province %d for original pops.\n", provNum);
	//			continue;
	//		}
	//		else
	//		{
	//			vector<Object*> pops = leaves[j]->getLeaves();
	//			for(unsigned int l = 0; l < pops.size(); l++)
	//			{
	//				vector< pair<int, double> > issues;
	//				issues.clear();
	//				V2Pop* newPop = new V2Pop(pops[l]->getKey(), atoi(pops[l]->getLeaf("size").c_str()), pops[l]->getLeaf("culture"), pops[l]->getLeaf("religion"), 10.0, 0.0, 0.0, 0.0, issues);
	//				provinces[k]->addOldPop(newPop);
	//			}
	//		}
	//	}
	//} while(_findnext(fileListing, &popsFileData) == 0);
	//_findclose(fileListing);

	// determine whether a province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise Vicky crashes)
	//log("\tFinding coastal provinces.\n");
	//printf("\tFinding coastal provinces.\n");
	//Object*	obj2 = doParseFile((V2Loc + "\\map\\positions.txt").c_str());
	//if (obj2 == NULL)
	//{
	//	log("Could not parse file %s\n", (V2Loc + "\\map\\positions.txt").c_str());
	//	exit(-1);
	//}
	//vector<Object*> objProv = obj2->getLeaves();
	//if (objProv.size() == 0)
	//{
	//	log("Error: map\\positions.txt failed to parse.");
	//	printf("Error: map\\positions.txt failed to parse.");
	//	exit(1);
	//}
	//for (vector<Object*>::iterator itr = objProv.begin(); itr != objProv.end(); ++itr)
	//{
	//	int provinceNum = atoi((*itr)->getKey().c_str());
	//	vector<Object*> objPos = (*itr)->getValue("building_position");
	//	if (objPos.size() == 0)
	//		continue;
	//	vector<Object*> objNavalBase = objPos[0]->getValue("naval_base");
	//	if (objNavalBase.size() != 0)
	//	{
	//		// this province is coastal
	//		for (vector<V2Province*>::iterator pitr = provinces.begin(); pitr != provinces.end(); ++pitr)
	//		{
	//			if ( (*pitr)->getNum() == provinceNum)
	//			{
	//				(*pitr)->setCoastal(true);
	//				break;
	//			}
	//		}
	//	}
	//}

	countries.clear();

	LOG(LogLevel::Info) << "Getting potential countries";
	potentialCountries.clear();
	dynamicCountries.clear();
	const date FirstStartDate("1836.1.1");
	ifstream V2CountriesInput;
	if (_stat(".\\blankMod\\output\\common\\countries.txt", &st) == 0)
	{
		V2CountriesInput.open(".\\blankMod\\output\\common\\countries.txt");
	}
	else
	{
		V2CountriesInput.open( (Configuration::getV2Path() + "\\common\\countries.txt").c_str() );
	}
	if (!V2CountriesInput.is_open())
	{
		LOG(LogLevel::Error) << "Could not open countries.txt";
		exit(1);
	}

	bool	staticSection	= true;
	while (!V2CountriesInput.eof())
	{
		string line;
		getline(V2CountriesInput, line);

		if ((line[0] == '#') || (line.size() < 3))
		{
			continue;
		}
		else if (line.substr(0, 12) == "dynamic_tags")
		{
			staticSection = false;
			continue;
		}

		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start			= line.find_first_of('/');
		int size				= line.find_last_of('\"') - start;
		countryFileName	= line.substr(start, size);

		Object* countryData;
		if (_stat((string(".\\blankMod\\output\\common\\countries\\") + countryFileName).c_str(), &st) == 0)
		{
			countryData = doParseFile((string(".\\blankMod\\output\\common\\countries\\") + countryFileName).c_str());
			if (countryData == NULL)
			{
				LOG(LogLevel::Warning) << "Could not parse file .\\blankMod\\output\\common\\countries\\" << countryFileName;
			}
		}
		else if (_stat((Configuration::getV2Path() + "\\common\\countries\\" + countryFileName).c_str(), &st) == 0)
		{
			countryData = doParseFile((Configuration::getV2Path() + "\\common\\countries\\" + countryFileName).c_str());
			if (countryData == NULL)
			{
				LOG(LogLevel::Warning) << "Could not parse file " << Configuration::getV2Path() << "\\common\\countries\\" << countryFileName;
			}
		}
		else
		{
			LOG(LogLevel::Debug) << "Could not find file common\\countries\\" << countryFileName << " - skipping";
			continue;
		}

		vector<Object*> partyData = countryData->getValue("party");
		vector<V2Party*> localParties;
		for (vector<Object*>::iterator itr = partyData.begin(); itr != partyData.end(); ++itr)
		{
			V2Party* newParty = new V2Party(*itr);
			localParties.push_back(newParty);
		}

		V2Country* newCountry = new V2Country(tag, countryFileName, localParties, this);
		if (staticSection)
		{
			potentialCountries.push_back(newCountry);
		}
		else
		{
			potentialCountries.push_back(newCountry);
			dynamicCountries.push_back(newCountry);
		}
	}
	V2CountriesInput.close();

	colonies.clear();
}

// BE: For V2 -> HoI3
V2World::V2World(Object* obj)
{
	provinces.clear();
	countries.clear();
	string key;
	vector<Object*> leaves = obj->getLeaves();

	// Get great nation indices
	unsigned countriesIndex = 1; // Starts from 1 at REB
	vector<Object*> greatNationsObj = obj->getValue("great_nations");
	map<int, int> greatNationIndices; // Map greatNation index to its ranking (i.e. 0 - 7)
	if (greatNationsObj.size() > 0)
	{
		vector<string> greatNations = greatNationsObj[0]->getTokens();
		for (unsigned int i = 0; i < greatNations.size(); i++)
		{
			greatNationIndices.insert(make_pair(atoi(greatNations[i].c_str()), i));
		}

		greatCountries.resize(greatNations.size());
	}

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
		{
			provinces[atoi(key.c_str())] = new V2Province(leaves[i]);
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) &&
			(
				(
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') &&
				(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				)
				||
				( // Dominions
				(key.c_str()[0] == 'D') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
				||
				( // Others (From previous conversion)
				(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') &&
				(key.c_str()[1] >= '0') && (key.c_str()[1] <= '9') &&
				(key.c_str()[2] >= '0') && (key.c_str()[2] <= '9')
				)
			)
		)
		{
			countries[key] = new V2Country(leaves[i]);

			map<int, int>::iterator rankingItr = greatNationIndices.find(countriesIndex++);
			if (rankingItr != greatNationIndices.end())
			{
				LOG(LogLevel::Debug) << "Set " << key << " as Great Power #" << rankingItr->second;
				countries[key]->setGreatNationRanking(rankingItr->second);
				greatCountries[rankingItr->second] = key;
			}
		}
	}

	// add province owner info to countries
	for (map<int, V2Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		map<string, V2Country*>::iterator j = countries.find(i->second->getOwnerString());
		if (j != countries.end())
		{
			j->second->addProvince(i->second);
			i->second->setOwner(j->second);
		}
	}

	// add province core info to countries
	for (map<int, V2Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		vector<V2Country*> cores = i->second->getCores(countries);	// the cores held on this province
		for (vector<V2Country*>::iterator j = cores.begin(); j != cores.end(); j++)
		{
			(*j)->addCore(i->second);
		}
	}

	// BE: Cull countries with neither cores nor owned provinces (i.e. dead countries and uncreated dominions)
	this->removeEmptyNations();

	// Diplomacy
	vector<Object*> diploObj = obj->getValue("diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy = V2Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = V2Diplomacy();
	}

	// BE: Get country colours and parties
	struct _stat st;
	ifstream V2CountriesInput;
	V2CountriesInput.open((Configuration::getV2Path() + "\\common\\countries.txt").c_str());
	if (!V2CountriesInput.is_open())
	{
		LOG(LogLevel::Error) << "Could not open V2 countries.txt";
		exit(1);
	}
	while (!V2CountriesInput.eof())
	{
		string line;
		getline(V2CountriesInput, line);

		if ((line[0] == '#') || (line.size() < 3))
		{
			continue;
		}
		else if (line.substr(0, 12) == "dynamic_tags")
		{
			continue;
		}

		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start = line.find_first_of('/');
		int size = line.find_last_of('\"') - start;
		countryFileName = line.substr(start, size);

		Object* countryData;
		if (_stat((Configuration::getV2Path() + "\\common\\countries\\" + countryFileName).c_str(), &st) == 0)
		{
			countryData = doParseFile((Configuration::getV2Path() + "\\common\\countries\\" + countryFileName).c_str());
			if (countryData == NULL)
			{
				LOG(LogLevel::Warning) << "Could not parse file " << Configuration::getV2Path() << "\\common\\countries\\" << countryFileName;
			}
		}
		else
		{
			LOG(LogLevel::Debug) << "Could not find file V2 common\\countries\\" << countryFileName << " - skipping";
			continue;
		}

		vector<Object*> colorObj = countryData->getValue("color");
		if (colorObj.size() > 0)
		{
			vector<string> rgb = colorObj[0]->getTokens();
			if (rgb.size() == 3)
			{
				if (countries.find(tag) != countries.end())
				{
					countries[tag]->setColor(Color(atoi(rgb[0].c_str()), atoi(rgb[1].c_str()), atoi(rgb[2].c_str())));
				}
			}
		}

		// Get party information
		// Reuse old variable
		leaves = countryData->getLeaves();

		for (unsigned int i = 0; i < leaves.size(); i++)
		{
			key = leaves[i]->getKey();
			if (key == "party")
			{
				parties.push_back(new V2Party(leaves[i]));
			}
		}
	}
	V2CountriesInput.close();
}

void V2World::output() const
{
	// Create common\countries path.
	string countriesPath = "Output\\" + Configuration::getOutputName() + "\\common\\countries";
	if (!WinUtils::TryCreateFolder(countriesPath))
	{
		return;
	}

	// Output common\countries.txt
	LOG(LogLevel::Debug) << "Writing countries file";
	FILE* allCountriesFile;
	if (fopen_s(&allCountriesFile, ("Output\\" + Configuration::getOutputName() + "\\common\\countries.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create countries file";
		exit(-1);
	}
	for (map<string, V2Country*>::const_iterator i = countries.begin(); i != countries.end(); i++)
	{
		const V2Country& country = *i->second;
		country.outputToCommonCountriesFile(allCountriesFile);
	}
	fprintf(allCountriesFile, "\n");
	fprintf(allCountriesFile, "##HoD Dominions\n");
	fprintf(allCountriesFile, "dynamic_tags = yes # any tags after this is considered dynamic dominions\n");
	for (vector<V2Country*>::const_iterator i = dynamicCountries.begin(); i != dynamicCountries.end(); i++)
	{
		(*i)->outputToCommonCountriesFile(allCountriesFile);
	}
	fclose(allCountriesFile);

	// Create flags for all new countries.
	V2Flags flags;
	flags.SetV2Tags(countries);
	flags.Output();

	// Create localisations for all new countries. We don't actually know the names yet so we just use the tags as the names.
	LOG(LogLevel::Debug) << "Writing localisation text";
	string localisationPath = "Output\\" + Configuration::getOutputName() + "\\localisation";
	if (!WinUtils::TryCreateFolder(localisationPath))
	{
		return;
	}
	string source = Configuration::getV2Path() + "\\localisation\\new_countries.csv";
	string dest = localisationPath + "\\countries.csv";
	WinUtils::TryCopyFile(source, dest);
	FILE* localisationFile;
	if (fopen_s(&localisationFile, dest.c_str(), "a") != 0)
	{
		LOG(LogLevel::Error) << "Could not update localisation text file";
		exit(-1);
	}
	for (map<string, V2Country*>::const_iterator i = countries.begin(); i != countries.end(); i++)
	{
		const V2Country& country = *i->second;
		if (country.isNewCountry())
		{
			country.outputLocalisation(localisationFile);
		}
	}
	fclose(localisationFile);

	LOG(LogLevel::Debug) << "Writing provinces";
	for (map<int, V2Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		//i->second->sortPops();
		i->second->output();
	}
	LOG(LogLevel::Debug) << "Writing countries";
	for (map<string, V2Country*>::const_iterator itr = countries.begin(); itr != countries.end(); itr++)
	{
		itr->second->output();
	}
	for (vector<V2Country*>::const_iterator itr = dynamicCountries.begin(); itr != dynamicCountries.end(); itr++)
	{
		(*itr)->isANewCountry();
		(*itr)->output();
	}
	diplomacy.output();
	/*if(Configuration::getV2Gametype() == "HOD")
	{
		for (map< int, set<string> >::const_iterator colonyIter = colonies.begin(); colonyIter != colonies.end(); colonyIter++)
		{
			fprintf(output, "region=\n");
			fprintf(output, "{\n");
			fprintf(output, "	index=%d\n", colonyIter->first);
			fprintf(output, "	phase=0\n");
			fprintf(output, "	temperature=0.000\n");
			for (set<string>::iterator countriesIter = colonyIter->second.begin(); countriesIter != colonyIter->second.end(); countriesIter++)
			{
				fprintf(output, "	colony=\n");
				fprintf(output, "	{\n");
				fprintf(output, "		tag=\"%s\"\n", countriesIter->c_str());
				fprintf(output, "		points=1\n");
				fprintf(output, "		invest=80\n");
				fprintf(output, "		date=\"1836.1.1\"\n");
				fprintf(output, "	}\n");
			}
			fprintf(output, "}\n");
		}
	}
	*/
}


bool scoresSorter(pair<V2Country*, int> first, pair<V2Country*, int> second)
{
	return (first.second > second.second);
}


void V2World::convertCountries(const EU4World& sourceWorld, const CountryMapping& countryMap, const cultureMapping& cultureMap, const unionCulturesMap& unionCultures, const religionMapping& religionMap, const governmentMapping& governmentMap, const inverseProvinceMapping& inverseProvinceMap, const vector<techSchool>& techSchools, map<int, int>& leaderMap, const V2LeaderTraits& lt, const map<string, double>& UHLiberalIdeas, const map<string, double>& UHReactionaryIdeas, const vector< pair<string, int> >& literacyIdeas, const map<string, int>& orderIdeas, const map<string, int>& libertyIdeas, const map<string, int>& equalityIdeas)
{
	vector<string> outputOrder;
	outputOrder.clear();
	for (unsigned int i = 0; i < potentialCountries.size(); i++)
	{
		outputOrder.push_back(potentialCountries[i]->getTag());
	}

	map<string, EU4Country*> sourceCountries = sourceWorld.getCountries();
	for (map<string, EU4Country*>::iterator i = sourceCountries.begin(); i != sourceCountries.end(); i++)
	{
		EU4Country* sourceCountry = i->second;
		std::string EU4Tag = sourceCountry->getTag();
		V2Country* destCountry = NULL;
		const std::string& V2Tag = countryMap[EU4Tag];
		if (!V2Tag.empty())
		{
			for (vector<V2Country*>::iterator j = potentialCountries.begin(); j != potentialCountries.end() && !destCountry; j++)
			{
				V2Country* candidateDestCountry = *j;
				if (candidateDestCountry->getTag() == V2Tag)
				{
					destCountry = candidateDestCountry;
				}
			}
			if (!destCountry)
			{ // No such V2 country exists yet for this tag so we make a new one.
				std::string countryFileName = '/' + sourceCountry->getName() + ".txt";
				destCountry = new V2Country(V2Tag, countryFileName, std::vector<V2Party*>(), this, true);
			}
			destCountry->initFromEU4Country(sourceCountry, outputOrder, countryMap, cultureMap, religionMap, unionCultures, governmentMap, inverseProvinceMap, techSchools, leaderMap, lt, UHLiberalIdeas, UHReactionaryIdeas, literacyIdeas);
			countries.insert(make_pair(V2Tag, destCountry));
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not convert EU4 tag " << i->second->getTag() << " to V2";
		}
	}

	// set national values
	list< pair<V2Country*, int> > libertyScores;
	list< pair<V2Country*, int> > equalityScores;
	set<V2Country*>					valuesUnset;
	for (map<string, V2Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		int libertyScore = 1;
		int equalityScore = 1;
		int orderScore = 1;
		countryItr->second->getNationalValueScores(libertyScore, equalityScore, orderScore, orderIdeas, libertyIdeas, equalityIdeas);
		if (libertyScore > orderScore)
		{
			libertyScores.push_back( make_pair(countryItr->second, libertyScore) );
		}
		if ((equalityScore > orderScore) && (equalityScore > libertyScore))
		{
			equalityScores.push_back(make_pair(countryItr->second, equalityScore));
		}
		valuesUnset.insert(countryItr->second);
		LOG(LogLevel::Debug) << "Value scores for " << countryItr->first << ": order = " << orderScore << ", liberty = " << libertyScore << ", equality = " << equalityScore;
	}
	equalityScores.sort(scoresSorter);
	int equalityLeft = 5;
	for (list< pair<V2Country*, int> >::iterator equalItr = equalityScores.begin(); equalItr != equalityScores.end(); equalItr++)
	{
		if (equalityLeft < 1)
		{
			break;
		}
		set<V2Country*>::iterator unsetItr = valuesUnset.find(equalItr->first);
		if (unsetItr != valuesUnset.end())
		{
			valuesUnset.erase(unsetItr);
			equalItr->first->setNationalValue("nv_equality");
			equalityLeft--;
			LOG(LogLevel::Debug) << equalItr->first->getTag() << " got national value equality";
		}
	}
	libertyScores.sort(scoresSorter);
	int libertyLeft = 20;
	for (list< pair<V2Country*, int> >::iterator libItr = libertyScores.begin(); libItr != libertyScores.end(); libItr++)
	{
		if (libertyLeft < 1)
		{
			break;
		}
		set<V2Country*>::iterator unsetItr = valuesUnset.find(libItr->first);
		if (unsetItr != valuesUnset.end())
		{
			valuesUnset.erase(unsetItr);
			libItr->first->setNationalValue("nv_liberty");
			libertyLeft--;
			LOG(LogLevel::Debug) << libItr->first->getTag() << " got national value liberty";
		}
	}
	for (set<V2Country*>::iterator unsetItr = valuesUnset.begin(); unsetItr != valuesUnset.end(); unsetItr++)
	{
		(*unsetItr)->setNationalValue("nv_order");
		LOG(LogLevel::Debug) << (*unsetItr)->getTag() << " got national value order";
	}

	// set prestige
	LOG(LogLevel::Debug) << "Setting prestige";
	double highestScore = 0.0;
	for (map<string, V2Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		double score = 0.0;
		const EU4Country* srcCountry = countryItr->second->getSourceCountry();
		if (srcCountry != NULL)
		{
			score = srcCountry->getScore();
		}
		if (score > highestScore)
		{
			highestScore = score;
		}
	}
	for (map<string, V2Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		double score = 0.0;
		const EU4Country* srcCountry = countryItr->second->getSourceCountry();
		if (srcCountry != NULL)
		{
			score = srcCountry->getScore();
		}
		double prestige = (score * 99.0 / highestScore) + 1;
		countryItr->second->addPrestige(prestige);
		LOG(LogLevel::Debug) << countryItr->first << " had " << prestige << " prestige";
	}

	// ALL potential countries should be output to the file, otherwise some things don't get initialized right
	for (vector<V2Country*>::iterator itr = potentialCountries.begin(); itr != potentialCountries.end(); ++itr)
	{
		map<string, V2Country*>::iterator citr = countries.find((*itr)->getTag());
		if (citr == countries.end())
		{
			(*itr)->initFromHistory();
			countries.insert(make_pair((*itr)->getTag(), *itr));
		}
	}

	// put countries in the same order as potentialCountries was (this is the same order V2 will save them in)
	/*vector<V2Country*> sortedCountries;
	for (vector<string>::const_iterator oitr = outputOrder.begin(); oitr != outputOrder.end(); ++oitr)
	{
		map<string, V2Country*>::iterator itr = countries.find((*itr)->getTag());
		{
			if ( (*itr)->getTag() == (*oitr) )
			{
				sortedCountries.push_back(*itr);
				break;
			}
		}
	}
	countries.swap(sortedCountries);*/
}


void V2World::convertDiplomacy(const EU4World& sourceWorld, const CountryMapping& countryMap)
{
	vector<EU4Agreement> agreements = sourceWorld.getDiplomacy()->getAgreements();
	for (vector<EU4Agreement>::iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		const std::string& EU4Tag1 = itr->country1;
		const std::string& V2Tag1 = countryMap[EU4Tag1];
		if (V2Tag1.empty())
		{
			LOG(LogLevel::Warning) << "EU4 Country " << EU4Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		const std::string& EU4Tag2 = itr->country2;
		const std::string& V2Tag2 = countryMap[EU4Tag2];
		if (V2Tag2.empty())
		{
			LOG(LogLevel::Warning) << "EU4 Country " << EU4Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		map<string, V2Country*>::iterator country1 = countries.find(V2Tag1);
		map<string, V2Country*>::iterator country2 = countries.find(V2Tag2);
		if (country1 == countries.end())
		{
			LOG(LogLevel::Warning) << "Vic2 country " << V2Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		if (country2 == countries.end())
		{
			LOG(LogLevel::Warning) << "Vic2 country " << V2Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		V2Relations* r1 = country1->second->getRelations(V2Tag2);
		if (!r1)
		{
			r1 = new V2Relations(V2Tag2);
			country1->second->addRelation(r1);
		}
		V2Relations* r2 = country2->second->getRelations(V2Tag1);
		if (!r2)
		{
			r2 = new V2Relations(V2Tag1);
			country2->second->addRelation(r2);
		}

		if (itr->type == "is_colonial")
		{
			country1->second->absorbColony(country2->second);
		}
		if ((itr->type == "royal_marriage") || (itr->type == "guarantee"))
		{
			// influence level +1, but never exceed 4
			if (r1->getLevel() < 4)
			{
				r1->setLevel(r1->getLevel() + 1);
			}
		}
		if (itr->type == "royal_marriage")
		{
			// royal marriage is bidirectional; influence level +1, but never exceed 4
			if (r2->getLevel() < 4)
			{
				r2->setLevel(r2->getLevel() + 1);
			}
		}
		if ((itr->type == "vassal") || (itr->type == "union") || (itr->type == "protectorate"))
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
		if ((itr->type == "alliance") || (itr->type == "vassal") || (itr->type == "union") || (itr->type == "guarantee"))
		{
			// copy agreement
			V2Agreement v2a;
			v2a.country1 = V2Tag1;
			v2a.country2 = V2Tag2;
			v2a.start_date = itr->startDate;
			v2a.type = itr->type;
			diplomacy.addAgreement(v2a);
		}
	}
}


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<EU4Province*> provinces;
	int totalPopulation;
};


void V2World::convertProvinces(const EU4World& sourceWorld, const provinceMapping& provinceMap, const resettableMap& resettableProvinces, const CountryMapping& countryMap, const cultureMapping& cultureMap, const religionMapping& religionMap, const stateIndexMapping& stateIndexMap)
{
	for (map<int, V2Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		int destNum												= i->first;
		provinceMapping::const_iterator provinceLink	= provinceMap.find(destNum);
		if ( (provinceLink == provinceMap.end()) || (provinceLink->second.size() == 0) )
		{
			LOG(LogLevel::Warning) << "No source for " << i->second->getName() << " (province " << destNum << ')';
			continue;
		}
		else if (provinceLink->second[0] == 0)
		{
			continue;
		}
		else if ((Configuration::getResetProvinces() == "yes") && (resettableProvinces.count(destNum) > 0))
		{
			continue;
		}

		i->second->clearCores();

		EU4Province*	oldProvince		= NULL;
		EU4Country*		oldOwner			= NULL;
		// determine ownership by province count, or total population (if province count is tied)
		map<string, MTo1ProvinceComp> provinceBins;
		double newProvinceTotalPop = 0;
		for (vector<int>::const_iterator itr = provinceLink->second.begin(); itr != provinceLink->second.end(); ++itr)
		{
			EU4Province* province = sourceWorld.getProvince(*itr);
			if (!province)
			{
				LOG(LogLevel::Warning) << "Old province " << provinceLink->second[0] << " does not exist (bad mapping?)";
				continue;
			}
			EU4Country* owner = province->getOwner();
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
			if ((Configuration::getV2Gametype() == "HOD") && /*(province->getPopulation() < 1000)*/ false && (owner != NULL))
			{
				stateIndexMapping::const_iterator stateIndexMapping = stateIndexMap.find(i->first);
				if (stateIndexMapping == stateIndexMap.end())
				{
					LOG(LogLevel::Warning) << "Could not find state index for province " << i->first;
					continue;
				}
				else
				{
					map< int, set<string> >::iterator colony = colonies.find(stateIndexMapping->second);
					if (colony == colonies.end())
					{
						set<string> countries;
						countries.insert(owner->getTag());
						colonies.insert(make_pair(stateIndexMapping->second, countries));
					}
					else
					{
						colony->second.insert(owner->getTag());
					}
				}
			}
			else
			{
				provinceBins[tag].provinces.push_back(province);
				provinceBins[tag].totalPopulation += province->getPopulation();
				newProvinceTotalPop += province->getPopulation();
				// I am the new owner if there is no current owner, or I have more provinces than the current owner,
				// or I have the same number of provinces, but more population, than the current owner
				if ((oldOwner == NULL)
					 || (provinceBins[tag].provinces.size() > provinceBins[oldOwner->getTag()].provinces.size())
					 || ((provinceBins[tag].provinces.size() == provinceBins[oldOwner->getTag()].provinces.size())
					 && (provinceBins[tag].totalPopulation > provinceBins[oldOwner->getTag()].totalPopulation)))
				{
					oldOwner = owner;
					oldProvince = province;
				}
			}
		}
		if (oldOwner == NULL)
		{
			i->second->setOwnerString("");
			continue;
		}

		const std::string& V2Tag = countryMap[oldOwner->getTag()];
		if (V2Tag.empty())
		{
			LOG(LogLevel::Warning) << "Could not map provinces owned by " << oldOwner->getTag();
		}
		else
		{
			i->second->setOwnerString(V2Tag);
			map<string, V2Country*>::iterator ownerItr = countries.find(V2Tag);
			if (ownerItr != countries.end())
			{
				ownerItr->second->addProvince(i->second);
			}
			i->second->convertFromOldProvince(oldProvince);

			for (map<string, MTo1ProvinceComp>::iterator mitr = provinceBins.begin(); mitr != provinceBins.end(); ++mitr)
			{
				for (vector<EU4Province*>::iterator vitr = mitr->second.provinces.begin(); vitr != mitr->second.provinces.end(); ++vitr)
				{
					// assign cores
					vector<EU4Country*> oldCores = (*vitr)->getCores(sourceWorld.getCountries());
					for(vector<EU4Country*>::iterator j = oldCores.begin(); j != oldCores.end(); j++)
					{
						std::string coreEU4Tag = (*j)->getTag();
						// skip this core if the country is the owner of the EU4 province but not the V2 province
						// (i.e. "avoid boundary conflicts that didn't exist in EU4").
						// this country may still get core via a province that DID belong to the current V2 owner
						if (( coreEU4Tag == mitr->first) && ( coreEU4Tag != oldOwner->getTag()))
						{
							continue;
						}

						const std::string& coreV2Tag = countryMap[coreEU4Tag];
						if (!coreV2Tag.empty())
						{
							i->second->addCore(coreV2Tag);
						}
					}

					// determine demographics
					double provPopRatio = (*vitr)->getPopulation() / newProvinceTotalPop;
					vector<EU4PopRatio> popRatios = (*vitr)->getPopRatios();
					for (vector<EU4PopRatio>::iterator prItr = popRatios.begin(); prItr != popRatios.end(); ++prItr)
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
										LOG(LogLevel::Warning) << "Unhandled distinguisher type in culture rules";
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
							LOG(LogLevel::Warning) << "Could not set culture for pops in province " << destNum;
						}

						string religion = "";
						religionMapping::const_iterator religionItr = religionMap.find(prItr->religion);
						if (religionItr != religionMap.end())
						{
							religion = religionItr->second;
						}
						else
						{
							LOG(LogLevel::Warning) << "Could not set religion for pops in province " << destNum;
						}

						V2Demographic demographic;
						demographic.culture		= culture;
						demographic.religion		= religion;
						demographic.ratio			= prItr->popRatio * provPopRatio;
						demographic.oldCountry	= oldOwner;
						demographic.oldProvince	= *vitr;
								
						i->second->addPopDemographic(demographic);
					}

					// set forts and naval bases
					if ((*vitr)->hasBuilding("fort4") || (*vitr)->hasBuilding("fort5") || (*vitr)->hasBuilding("fort6"))
					{
						i->second->setFortLevel(1);
					}
				}
			}
		}
	}
}


void V2World::setupColonies(const adjacencyMapping& adjacencyMap, const continentMapping& continentMap)
{
	for (map<string, V2Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		// find all land connections to capitals
		map<int, V2Province*>	openProvinces = provinces;
		queue<int>					goodProvinces;

		map<int, V2Province*>::iterator openItr = openProvinces.find(countryItr->second->getCapital());
		if (openItr == openProvinces.end())
		{
			continue;
		}
		openItr->second->setLandConnection(true);
		goodProvinces.push(openItr->first);
		openProvinces.erase(openItr);

		do
		{
			int currentProvince = goodProvinces.front();
			goodProvinces.pop();
			if (currentProvince > static_cast<int>(adjacencyMap.size()))
			{
				LOG(LogLevel::Warning) << "No adjacency mapping for province " << currentProvince;
				continue;
			}
			vector<adjacency> adjacencies = adjacencyMap[currentProvince];
			for (unsigned int i = 0; i < adjacencies.size(); i++)
			{
				map<int, V2Province*>::iterator openItr = openProvinces.find(adjacencies[i].to);
				if (openItr == openProvinces.end())
				{
					continue;
				}
				if (openItr->second->getOwnerString() != countryItr->first)
				{
					continue;
				}
				openItr->second->setLandConnection(true);
				goodProvinces.push(openItr->first);
				openProvinces.erase(openItr);
			}
		} while (goodProvinces.size() > 0);

		// find all provinces on the same continent as the owner's capital
		string capitalContinent = "";
		map<int, V2Province*>::iterator capital = provinces.find(countryItr->second->getCapital());
		if (capital != provinces.end())
		{
			continentMapping::const_iterator itr = continentMap.find(capital->first);
			if (itr != continentMap.end())
			{
				capitalContinent = itr->second;
			}
			else
			{
				continue;
			}
				
		}
		else
		{
			continue;
		}
		vector<V2Province*> ownedProvinces = countryItr->second->getProvinces();
		for (vector<V2Province*>::iterator provItr = ownedProvinces.begin(); provItr != ownedProvinces.end(); provItr++)
		{
			continentMapping::const_iterator itr = continentMap.find((*provItr)->getNum());
			if ((itr != continentMap.end()) && (itr->second == capitalContinent))
			{
				(*provItr)->setSameContinent(true);
			}
		}
	}

	for (map<int, V2Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		provItr->second->determineColonial();
	}
}

static int stateId = 0;
void V2World::setupStates(const stateMapping& stateMap)
{
	list<V2Province*> unassignedProvs;
	for (map<int, V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		unassignedProvs.push_back(itr->second);
	}

	list<V2Province*>::iterator iter;
	while(unassignedProvs.size() > 0)
	{
		iter = unassignedProvs.begin();
		int		provId	= (*iter)->getNum();
		string	owner		= (*iter)->getOwnerString();

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
					if ((*iter)->getOwnerString() == owner)
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

		map<string, V2Country*>::iterator iter2 = countries.find(owner);
		if (iter2 != countries.end())
		{
			iter2->second->addState(newState);
		}
	}
}


void V2World::convertUncivReforms()
{
	for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		itr->second->convertUncivReforms();
	}
}



void V2World::setupPops(EU4World& sourceWorld)
{
	for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		itr->second->setupPops(sourceWorld);
	}
}


void V2World::addUnions(const unionMapping& unionMap)
{
	for (map<int, V2Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		for (unionMapping::const_iterator unionItr = unionMap.begin(); unionItr != unionMap.end(); unionItr++)
		{
			if ( provItr->second->hasCulture(unionItr->first, 0.5) && !provItr->second->wasInfidelConquest() && !provItr->second->wasColonised() )
			{
				provItr->second->addCore(unionItr->second);
			}
		}
	}
}


//#define TEST_V2_PROVINCES
void V2World::convertArmies(const EU4World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const map<int,int>& leaderIDMap)
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
		LOG(LogLevel::Error) << "Could not parse file regiment_costs.txt";
		exit(-1);
	}
	vector<Object*> objTop = obj2->getLeaves();
	if (objTop.size() == 0 || objTop[0]->getLeaves().size() == 0)
	{
		LOG(LogLevel::Error) << "regment_costs.txt failed to parse";
		exit(1);
	}
	for (int i = 0; i < num_reg_categories; ++i)
	{
		string regiment_cost = objTop[0]->getLeaf(RegimentCategoryNames[i]);
		cost_per_regiment[i] = atoi(regiment_cost.c_str());
	}

	// convert armies
	for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		itr->second->convertArmies(leaderIDMap, cost_per_regiment, inverseProvinceMap, provinces, port_whitelist);
	}
}


void V2World::convertTechs(const EU4World& sourceWorld)
{
	map<string, EU4Country*> sourceCountries = sourceWorld.getCountries();
	
	double oldArmyMean;
	double armyMean;
	double highestArmy;

	double oldNavyMean;
	double navyMean;
	double highestNavy;

	double oldCommerceMean;
	double commerceMean;
	double highestCommerce;

	double oldCultureMean;
	double cultureMean;
	double highestCulture;

	double oldIndustryMean;
	double industryMean;
	double highestIndustry;

	int num = 2;
	map<string, EU4Country*>::iterator i = sourceCountries.begin();
	while (i->second->getProvinces().size() == 0)
	{
		i++;
	}
	highestArmy			= oldArmyMean		= armyMean		= i->second->getAdmTech() + i->second->getMilTech();
	highestNavy			= oldNavyMean		= navyMean		= i->second->getMilTech() + i->second->getDipTech();
	highestCommerce	= oldCommerceMean	= commerceMean	= i->second->getAdmTech() + i->second->getDipTech();
	highestCulture		= oldCultureMean	= cultureMean	= i->second->getDipTech();
	highestIndustry	= oldIndustryMean	= industryMean	= i->second->getMilTech() + i->second->getAdmTech() + i->second->getDipTech();

	for (i++; i != sourceCountries.end(); i++)
	{
		if (i->second->getProvinces().size() == 0)
		{
			continue;
		}
		double newTech	= i->second->getAdmTech() + i->second->getMilTech();
		armyMean			= oldArmyMean + ((newTech - oldArmyMean) / num);
		oldArmyMean		= armyMean; 
		if (newTech > highestArmy)
		{
			highestArmy = newTech;
		}

		newTech		= i->second->getMilTech() + i->second->getDipTech();
		navyMean		= oldNavyMean + ((newTech - oldNavyMean) / num);
		oldNavyMean	= navyMean;
		if (newTech > highestNavy)
		{
			highestNavy = newTech;
		}

		newTech				= i->second->getAdmTech() + i->second->getDipTech();
		commerceMean		= oldCommerceMean + ((newTech - oldCommerceMean) / num);
		oldCommerceMean	= commerceMean;
		if (newTech > highestCommerce)
		{
			highestCommerce = newTech;
		}

		newTech			= i->second->getDipTech();
		cultureMean		= oldCultureMean + ((newTech - oldCultureMean) / num);
		oldCultureMean	= cultureMean;
		if (newTech > highestCulture)
		{
			highestCulture = newTech;
		}

		newTech				= i->second->getMilTech() + i->second->getAdmTech() + i->second->getDipTech();
		industryMean		= oldIndustryMean + ((newTech - oldIndustryMean) / num);
		oldIndustryMean	= industryMean;
		if (newTech > highestIndustry)
		{
			highestIndustry = newTech;
		}

		num++;
	}

	for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); itr++)
	{
		if ((Configuration::getV2Gametype() == "vanilla") || itr->second->isCivilized())
		{
			itr->second->setArmyTech(armyMean, highestArmy);
			itr->second->setNavyTech(navyMean, highestNavy);
			itr->second->setCommerceTech(commerceMean, highestCommerce);
			itr->second->setIndustryTech(industryMean, highestIndustry);
			itr->second->setCultureTech(cultureMean, highestCulture);
		}
	}

	int numRomanticLit = 0;
	int numRomanticArt = 0;
	int numRomanticMusic = 0;
	if (Configuration::getV2Gametype() != "HOD")
	{
		for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); itr++)
		{
			if (itr->second->getInventionState(VANILLA_romanticist_literature) == active)
			{
				numRomanticLit++;
			}
			if (itr->second->getInventionState(VANILLA_romanticist_literature) == active)
			{
				numRomanticArt++;
			}
			if (itr->second->getInventionState(VANILLA_romanticist_literature) == active)
			{
				numRomanticMusic++;
			}
		}
	}
	else if (Configuration::getV2Gametype() == "HOD")
	{
		for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); itr++)
		{
			if (itr->second->getInventionState(HOD_romanticist_literature) == active)
			{
				numRomanticLit++;
			}
			if (itr->second->getInventionState(HOD_romanticist_literature) == active)
			{
				numRomanticArt++;
			}
			if (itr->second->getInventionState(HOD_romanticist_literature) == active)
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
		for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); itr++)
		{
			if (itr->second->getInventionState(VANILLA_romanticist_literature) == active)
			{
				itr->second->addPrestige(romanticLitPrestige);
			}
			if (itr->second->getInventionState(VANILLA_romanticist_art) == active)
			{
				itr->second->addPrestige(romanticArtPrestige);
			}
			if (itr->second->getInventionState(VANILLA_romanticist_music) == active)
			{
				itr->second->addPrestige(romanticMusicPrestige);
			}
		}
	}
	else if (Configuration::getV2Gametype() == "HOD")
	{
		for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); itr++)
		{
			if (itr->second->getInventionState(HOD_romanticist_literature) == active)
			{
				itr->second->addPrestige(romanticLitPrestige);
			}
			if (itr->second->getInventionState(HOD_romanticist_art) == active)
			{
				itr->second->addPrestige(romanticArtPrestige);
			}
			if (itr->second->getInventionState(HOD_romanticist_music) == active)
			{
				itr->second->addPrestige(romanticMusicPrestige);
			}
		}
	}
}


void V2World::allocateFactories(const EU4World& sourceWorld, const V2FactoryFactory& factoryBuilder)
{
	// determine average production tech
	map<string, EU4Country*> sourceCountries = sourceWorld.getCountries();
	double productionMean = 0.0f;
	int num = 1;
	for (map<string, EU4Country*>::iterator itr = sourceCountries.begin(); itr != sourceCountries.end(); ++itr)
	{
		if ( (itr)->second->getProvinces().size() == 0)
		{
			continue;
		}
		if (( (itr)->second->getTechGroup() != "western" ) && ( (itr)->second->getTechGroup() != "latin" ))
		{
			continue;
		}

		double prodTech = (itr)->second->getAdmTech();
		productionMean += ((prodTech - productionMean) / num);
		++num;
	}

	// give all extant civilized nations an industrial score
	deque<pair<double, V2Country*>> weightedCountries;
	for (map<string, V2Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		if ( !itr->second->isCivilized() )
		{
			continue;
		}

		const EU4Country* sourceCountry = itr->second->getSourceCountry();
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
		double industryWeight = (sourceCountry->getAdmTech() - productionMean) + manuWeight;
		// having one manufactory and average tech is not enough; you must have more than one, or above-average tech
		if (industryWeight > 1.0)
		{
			weightedCountries.push_back(pair<double, V2Country*>(industryWeight, itr->second));
		}
	}
	if (weightedCountries.size() < 1)
	{
		LOG(LogLevel::Warning) << "No countries are able to accept factories";
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
		LOG(LogLevel::Debug) << itr->second->getTag() << " has industrial weight " << itr->first << " granting max " << factories << " factories";
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
			Log logOutput(LogLevel::Debug);
			logOutput << "No countries will accept any of the remaining factories:\n";
			for (deque<V2Factory*>::iterator qitr = factoryList.begin(); qitr != factoryList.end(); ++qitr)
			{
				logOutput << "\t  " << (*qitr)->getTypeName() << '\n';
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


map<string, V2Country*> V2World::getDynamicCountries() const
{
	map<string, V2Country*> retVal;
	for (vector<V2Country*>::const_iterator i = dynamicCountries.begin(); i != dynamicCountries.end(); i++)
	{
		retVal[(*i)->getTag()] = *i;
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
	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		map<int, V2Province*>::iterator pitr = provinces.find(*litr);
		if (pitr != provinces.end())
		{
			if ( !pitr->second->isCoastal() )
			{
				locationCandidates.erase(litr);
				--pitr;
				break;
			}
		}
	}
	return locationCandidates;
}


V2Country* V2World::getCountry(string tag) const
{
	map<string, V2Country*>::const_iterator itr = countries.find(tag);
	if (itr != countries.end())
	{
		return itr->second;
	}
	else
	{
		return NULL;
	}
}

void V2World::removeCountry(string tag)
{
	countries.erase(tag);
}

V2Province* V2World::getProvince(int provNum) const
{
	map<int, V2Province*>::const_iterator i = provinces.find(provNum);
	return (i != provinces.end()) ? i->second : NULL;
}

void V2World::checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const
{
	for (map<int, V2Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		inverseProvinceMapping::const_iterator j = inverseProvinceMap.find(i->first);
		if (j == inverseProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for province " << i->first;
		}
	}
}

void V2World::setLocalisations(V2Localisation& localisation)
{
	for (map<string, V2Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		const auto& nameLocalisations = localisation.GetTextInEachLanguage(countryItr->second->getTag());	// the names in all languages
		for (const auto& nameLocalisation : nameLocalisations)	// the name under consideration
		{
			const std::string& language = nameLocalisation.first;	// the language
			const std::string& name = nameLocalisation.second;		// the name of the country in this language
			countryItr->second->setLocalisationName(language, name);
		}
		const auto& adjectiveLocalisations = localisation.GetTextInEachLanguage(countryItr->second->getTag() + "_ADJ");	// the adjectives in all languages
		for (const auto& adjectiveLocalisation : adjectiveLocalisations)	// the adjective under consideration
		{
			const std::string& language = adjectiveLocalisation.first;		// the language
			const std::string& adjective = adjectiveLocalisation.second;	// the adjective for the country in this language
			countryItr->second->setLocalisationAdjective(language, adjective);
		}
	}
}

V2Party* V2World::getRulingParty(const V2Country* country) const
{
	if (country->getRulingPartyId() > parties.size() || country->getRulingPartyId() < 1)
	{
		return NULL;
	}

	return parties[country->getRulingPartyId() - 1]; // Subtract 1, because party ID starts from index of 1
}

void V2World::removeEmptyNations()
{
	for (map<string, V2Country*>::iterator i = countries.begin(); i != countries.end();)
	{
		if (i->second->getCores().size() < 1 && i->second->getProvinces().size() < 1)
		{
			i = countries.erase(i); // i points to the next element
		}
		else
		{
			++i;
		}
	}
}
