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


bool scoresSorter(pair<V2Country*, int> first, pair<V2Country*, int> second)
{
	return (first.second > second.second);
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
