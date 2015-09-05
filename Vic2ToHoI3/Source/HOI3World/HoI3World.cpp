/*Copyright (c) 2015 The Paradox Game Converters Project

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


#include "HoI3World.h"
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
#include "../V2World/V2Province.h"
#include "../V2World/V2Party.h"
#include "HoI3Relations.h"


typedef struct fileWithCreateTime
{
	string	filename;
	time_t	createTime;
	bool operator < (const fileWithCreateTime &rhs) const
	{
		return createTime < rhs.createTime;
	};
} fileWithCreateTime;


HoI3World::HoI3World()
{
	LOG(LogLevel::Info) << "Importing provinces";

	struct _finddata_t	provinceFileData;
	intptr_t					fileListing = NULL;
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
					HoI3Province* newProvince = new HoI3Province(directories.front() + "\\" + provinceFileData.name);
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
			if ((fileListing = _findfirst((Configuration::getHoI3Path() + "\\history\\provinces" + directories.front() + "\\*.*").c_str(), &provinceFileData)) == -1L)
			{
				LOG(LogLevel::Error) << "Could not open directory " << Configuration::getHoI3Path() << "\\history\\provinces" << directories.front() << "\\*.*";
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
					HoI3Province* newProvince = new HoI3Province(directories.front() + "\\" + provinceFileData.name);
					provinces.insert(make_pair(newProvince->getNum(), newProvince));
				}
			} while (_findnext(fileListing, &provinceFileData) == 0);
			_findclose(fileListing);
			directories.pop_front();
		}
	}

	// determine whether each province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise HoI3 crashes)
	Object*	obj2 = doParseFile((Configuration::getHoI3Path() + "\\map\\positions.txt").c_str());
	vector<Object*> objProv = obj2->getLeaves();
	if (objProv.size() == 0)
	{
		LOG(LogLevel::Error) << "map\\positions.txt failed to parse.";
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
			map<int, HoI3Province*>::iterator pitr = provinces.find(provinceNum);
			if (pitr != provinces.end())
			{
				pitr->second->setCoastal(true);
			}
		}
	}

	//obj2 = doParseFile((HoI3Loc + "\\map\\continent.txt").c_str());
	//vector<Object*> objCont = obj2->getLeaves();
	//if (objCont.size() == 0)
	//{
	//	log("Error: map\\continent.txt failed to parse.");
	//	printf("Error: map\\continent.txt failed to parse.");
	//	exit(1);
	//}
	//for (vector<Object*>::iterator itr = objCont.begin(); itr != objCont.end(); ++itr)
	//{
	//	string cont = (*itr)->getKey();
	//	vector<string> provs = (*itr)->getTokens();
	//	for (vector<string>::iterator pitr = provs.begin(); pitr != provs.end(); ++pitr)
	//	{
	//		continents[atoi(pitr->c_str())] = cont;
	//	}
	//}


	countries.clear();

	LOG(LogLevel::Info) << "Getting potential countries";
	potentialCountries.clear();
	const date FirstStartDate("1936.1.1");
	ifstream HoI3CountriesInput;
	if (_stat(".\\blankMod\\output\\common\\countries.txt", &st) == 0)
	{
		HoI3CountriesInput.open(".\\blankMod\\output\\common\\countries.txt");
	}
	else
	{
		HoI3CountriesInput.open((Configuration::getHoI3Path() + "\\common\\countries.txt").c_str());
	}
	if (!HoI3CountriesInput.is_open())
	{
		LOG(LogLevel::Error) << "Could not open countries.txt";
		exit(1);
	}

	while (!HoI3CountriesInput.eof())
	{
		string line;
		getline(HoI3CountriesInput, line);

		if ((line[0] == '#') || (line.size() < 3))
		{
			continue;
		}

		string tag;
		tag = line.substr(0, 3);

		string countryFileName;
		int start = line.find_first_of('/');
		int size = line.find_last_of('\"') - start;
		countryFileName = line.substr(start, size);

		// BE: Unnecessary. Let vanilla HoI3 countries keep their common files. They will be overridden[sic] as necessary
		//Object* countryData;
		//if (_stat((string(".\\blankMod\\output\\common\\countries\\") + countryFileName).c_str(), &st) == 0)
		//{
		//	countryData = doParseFile((string(".\\blankMod\\output\\common\\countries\\") + countryFileName).c_str());
		//	if (countryData == NULL)
		//	{
		//		LOG(LogLevel::Warning) << "Could not parse file .\\blankMod\\output\\common\\countries\\" << countryFileName;
		//	}
		//}
		//else if (_stat((Configuration::getHoI3Path() + "\\common\\countries\\" + countryFileName).c_str(), &st) == 0)
		//{
		//	countryData = doParseFile((Configuration::getHoI3Path() + "\\common\\countries\\" + countryFileName).c_str());
		//	if (countryData == NULL)
		//	{
		//		LOG(LogLevel::Warning) << "Could not parse file " << Configuration::getHoI3Path() << "\\common\\countries\\" << countryFileName;
		//	}
		//}
		//else
		//{
		//	LOG(LogLevel::Debug) << "Could not find file common\\countries\\" << countryFileName << " - skipping";
		//	continue;
		//}

		HoI3Country* newCountry = new HoI3Country(tag, countryFileName, this);
		potentialCountries.push_back(newCountry);
	}
	HoI3CountriesInput.close();

	countryOrder.push_back("REB");
}

void HoI3World::output() const
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

	// Make sure Rebels are first in the list (Rebel flag used for unclaimed provinces).
	// Then put the V2 great powers in order
	for (unsigned i = 0; i < countryOrder.size(); i++)
	{
		const string hoiTag = countryOrder[i];
		HoI3Country* country = NULL;
		std::map<string, HoI3Country*>::const_iterator countryItr;

		if (hoiTag.empty())
		{
			LOG(LogLevel::Error) << "countryOrder entry #" << i << " empty!";
		}
		else if ((countryItr = countries.find(countryOrder[i])) == countries.end())
		{
			// Search potential countries. (This should normally only be REB, because only active countries should be in countryOrder)
			for (vector<HoI3Country*>::const_iterator pCountryItr = potentialCountries.begin(); pCountryItr != potentialCountries.end(); pCountryItr++)
			{
				if (hoiTag == (*pCountryItr)->getTag())
				{
					country = *pCountryItr;
				}
			}
		}
		else
		{
			country = countryItr->second;
		}

		if (country)
		{
			country->outputToCommonCountriesFile(allCountriesFile);
		}
		else
		{
			LOG(LogLevel::Error) << "countryOrder entry #" << i << " " << countryOrder[i] << " not found!";
		}
	}

	for (map<string, HoI3Country*>::const_iterator i = countries.begin(); i != countries.end(); i++)
	{
		// Skip countries that have already been processed
		for (unsigned j = 0; j < countryOrder.size(); j++)
		{
			if (countryOrder[j] == i->first)
			{
				continue;
			}
		}

		const HoI3Country& country = *i->second;
		country.outputToCommonCountriesFile(allCountriesFile);
	}

	// BE: There are bugs and crashes if all potential vanilla HoI3 countries are not in the common\countries.txt file
	// Also, the Rebels are to be included
	for (vector<HoI3Country*>::const_iterator i = potentialCountries.begin(); i != potentialCountries.end(); i++)
	{
		// Skip countries that have already been processed. (This should normally only be REB, because only active countries should be in countryOrder)
		for (unsigned j = 0; j < countryOrder.size(); j++)
		{
			if (countryOrder[j] == (*i)->getTag())
			{
				continue;
			}
		}

		const HoI3Country& country = **i;
		if (countries.find(country.getTag()) == countries.end())
		{
			country.outputToCommonCountriesFile(allCountriesFile);
		}
	}
	fprintf(allCountriesFile, "\n");
	fclose(allCountriesFile);

	// Create localisations for all new countries. We don't actually know the names yet so we just use the tags as the names.
	LOG(LogLevel::Debug) << "Writing localisation text";
	string localisationPath = "Output\\" + Configuration::getOutputName() + "\\localisation";
	if (!WinUtils::TryCreateFolder(localisationPath))
	{
		return;
	}

	string source = ".\\blankMod\\output\\localisation\\countries.csv";
	string dest = localisationPath + "\\countries.csv";
	WinUtils::TryCopyFile(source, dest);
	FILE* localisationFile;
	if (fopen_s(&localisationFile, dest.c_str(), "a") != 0)
	{
		LOG(LogLevel::Error) << "Could not update localisation text file";
		exit(-1);
	}
	for (map<string, HoI3Country*>::const_iterator i = countries.begin(); i != countries.end(); i++)
	{
		const HoI3Country& country = *i->second;
		if (country.isNewCountry())
		{
			country.outputLocalisation(localisationFile);
		}
	}
	fclose(localisationFile);

	LOG(LogLevel::Debug) << "Writing provinces";
	for (map<int, HoI3Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		i->second->output();
	}
	LOG(LogLevel::Debug) << "Writing countries";
	for (map<string, HoI3Country*>::const_iterator itr = countries.begin(); itr != countries.end(); itr++)
	{
		itr->second->output();
	}
	// Override vanilla history to suppress vanilla OOB and faction membership being read
	for (vector<HoI3Country*>::const_iterator i = potentialCountries.begin(); i != potentialCountries.end(); i++)
	{
		const HoI3Country& country = **i;
		if (countries.find(country.getTag()) == countries.end())
		{
			country.output();
		}
	}
	LOG(LogLevel::Debug) << "Writing diplomacy";
	diplomacy.output();
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
			provinces[num]->setName(name);
		}
	}
	read.close();
}


void HoI3World::convertCountries(const V2World &sourceWorld, CountryMapping countryMap, const governmentMapping& governmentMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap)
{
	vector<string> outputOrder;
	outputOrder.clear();
	for (unsigned int i = 0; i < potentialCountries.size(); i++)
	{
		outputOrder.push_back(potentialCountries[i]->getTag());
	}

	map<string, V2Country*> sourceCountries = sourceWorld.getCountries();
	for (map<string, V2Country*>::iterator i = sourceCountries.begin(); i != sourceCountries.end(); i++)
	{
		V2Country* sourceCountry = i->second;
		std::string V2Tag = sourceCountry->getTag();

		if (V2Tag == "REB")
		{
			continue;
		}

		HoI3Country* destCountry = NULL;
		const std::string& HoI3Tag = countryMap[V2Tag];

		if (!HoI3Tag.empty())
		{
			for (vector<HoI3Country*>::iterator j = potentialCountries.begin(); j != potentialCountries.end() && !destCountry; j++)
			{
				HoI3Country* candidateDestCountry = *j;
				if (candidateDestCountry->getTag() == HoI3Tag)
				{
					destCountry = candidateDestCountry;
				}
			}
			if (!destCountry)
			{ // No such V2 country exists yet for this tag so we make a new one.
				std::string countryFileName = '/' + sourceCountry->getName() + ".txt";
				destCountry = new HoI3Country(HoI3Tag, countryFileName, this, true);
			}
			V2Party* rulingParty = sourceWorld.getRulingParty(sourceCountry);
			if (rulingParty == NULL)
			{
				LOG(LogLevel::Error) << "Could not find the ruling party for " <<  sourceCountry->getTag() << ". Were all mods correctly included?";
				exit(-1);
			}
			destCountry->initFromV2Country(sourceWorld, sourceCountry, rulingParty->ideology, outputOrder, countryMap, governmentMap, inverseProvinceMap, leaderMap, V2Localisations, governmentJobs, namesMap, portraitMap, cultureMap);
			countries.insert(make_pair(HoI3Tag, destCountry));
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not convert V2 tag " << i->second->getTag() << " to HoI3";
		}
	}

	// ALL potential countries should be output to the file, otherwise some things don't get initialized right
	for (vector<HoI3Country*>::iterator itr = potentialCountries.begin(); itr != potentialCountries.end(); ++itr)
	{
		map<string, HoI3Country*>::iterator citr = countries.find((*itr)->getTag());
		if (citr == countries.end())
		{
			(*itr)->initFromHistory();
			//countries.insert(make_pair((*itr)->getTag(), *itr));
		}
	}

	const std::vector<string> &greatCountries = sourceWorld.getGreatCountries();
	for (unsigned i = 0; i < greatCountries.size(); i++)
	{
		const std::string& HoI3Tag = countryMap[greatCountries[i]];
		countryOrder.push_back(HoI3Tag);
	}
}


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<V2Province*> provinces;
	int totalPopulation;
};


void HoI3World::convertProvinces(const V2World &sourceWorld, provinceMapping provinceMap, CountryMapping countryMap)
{
	for (map<int, HoI3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		int destNum = i->first;
		provinceMapping::const_iterator provinceLink = provinceMap.find(destNum);
		if ((provinceLink == provinceMap.end()) || (provinceLink->second.size() == 0))
		{
			LOG(LogLevel::Warning) << "No source for " << i->second->getName() << " (province " << destNum << ')';
			continue;
		}
		else if (provinceLink->second[0] == 0)
		{
			continue;
		}
		else if (Configuration::getResetProvinces() == "yes")
		{
			continue;
		}

		i->second->clearCores();

		V2Province* oldProvince = NULL;
		V2Country* oldOwner = NULL;
		// determine ownership by province count, or total population (if province count is tied)
		map<string, MTo1ProvinceComp> provinceBins;
		double newProvinceTotalPop = 0;
		for (vector<int>::const_iterator itr = provinceLink->second.begin(); itr != provinceLink->second.end(); ++itr)
		{
			V2Province* province = sourceWorld.getProvince(*itr);
			if (!province)
			{
				LOG(LogLevel::Warning) << "Old province " << provinceLink->second[0] << " does not exist (bad mapping?)";
				continue;
			}
			V2Country* owner = province->getOwner();
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
			provinceBins[tag].totalPopulation += province->getTotalPopulation();
			newProvinceTotalPop += province->getTotalPopulation();
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
		if (oldOwner == NULL)
		{
			i->second->setOwner("");
			continue;
		}

		const std::string& HoI3Tag = countryMap[oldOwner->getTag()];
		if (HoI3Tag.empty())
		{
			LOG(LogLevel::Warning) << "Could not map provinces owned by " << oldOwner->getTag();
		}
		else
		{
			i->second->setOwner(HoI3Tag);
			map<string, HoI3Country*>::iterator ownerItr = countries.find(HoI3Tag);
			if (ownerItr != countries.end())
			{
				ownerItr->second->addProvince(i->second);
			}
			i->second->convertFromOldProvince(oldProvince);

			for (map<string, MTo1ProvinceComp>::iterator mitr = provinceBins.begin(); mitr != provinceBins.end(); ++mitr)
			{
				for (vector<V2Province*>::iterator vitr = mitr->second.provinces.begin(); vitr != mitr->second.provinces.end(); ++vitr)
				{
					vector<V2Country*> oldCores = (*vitr)->getCores(sourceWorld.getCountries());
					for(unsigned int j = 0; j < oldCores.size(); j++)
					{
						// skip this core if the country is the owner of the V2 province but not the HoI3 province
						// (i.e. "avoid boundary conflicts that didn't exist in V2").
						// this country may still get core via a province that DID belong to the current HoI3 owner
						if ((oldCores[j]->getTag() == mitr->first) && (oldCores[j] != oldOwner))
							continue;

						const std::string& coreOwner = countryMap[oldCores[j]->getTag()];
						if (coreOwner != "")
						{
							i->second->addCore(coreOwner);
						}
					}

					// TODO: preliminary improvement conversion
					int fortLevel = (*vitr)->getFort();
					fortLevel = max(0, (fortLevel - 5) * 2 + 1); // Only use 5 or 6 to reduce fort spam
					int navalBaseLevel = (*vitr)->getNavalBase();
					navalBaseLevel = max(0, (navalBaseLevel - 3) * 2 + 1); // Reduce spam
					provinces[destNum]->requireCoastalFort(fortLevel);
					provinces[destNum]->requireLandFort(fortLevel);
					provinces[destNum]->requireNavalBase(navalBaseLevel);
					provinces[destNum]->requireInfrastructure((int)Configuration::getMinInfra());
					if ((*vitr)->getInfra() > 0) // No infra stays at minInfra
					{
						provinces[destNum]->requireInfrastructure((*vitr)->getInfra() + 4); // BE: 6 is max V2 level. We want that to correspond with 100% infra.
					}

					//provinces[destNum]->setAvgMil((*vitr)->getAvgMil());
					// XXX: how shall we set industry?
					if (Configuration::getIcConversion() != "no")
					{
						if (Configuration::getIcConversion() == "gunther"){
							
						}
						else {
							provinces[destNum]->setNCrafts(((*vitr)->getPopulation("craftsmen")
							+ int((*vitr)->getPopulation("artisans") * 0.5)
							+ (*vitr)->getLiteracyWeightedPopulation("capitalists") * 2
							+ (*vitr)->getLiteracyWeightedPopulation("clerks") * 2) / mitr->second.provinces.size());
						}
						
					}

					double newManpower = (*vitr)->getPopulation("soldiers")
						+ (*vitr)->getPopulation("craftsmen") * 0.25 // Conscripts
						+ (*vitr)->getPopulation("labourers") * 0.25 // Conscripts
						+ (*vitr)->getPopulation("farmers") * 0.25; // Conscripts
					if (Configuration::getManpowerConversion() == "linear")
					{
						newManpower *= Configuration::getManpowerFactor() / mitr->second.provinces.size();
						newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
						provinces[destNum]->setManpower(newManpower);
					}
					else if (Configuration::getManpowerConversion() == "squareroot")
					{
						newManpower = sqrt(newManpower);
						newManpower *= Configuration::getManpowerFactor() / mitr->second.provinces.size();
						newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
						provinces[destNum]->setManpower(newManpower);
					}

					double newLeadership = (*vitr)->getLiteracyWeightedPopulation("clergymen") * 0.5
						+ (*vitr)->getPopulation("officers")
						+ (*vitr)->getLiteracyWeightedPopulation("clerks") // Clerks representing researchers
						+ (*vitr)->getLiteracyWeightedPopulation("capitalists") * 0.5
						+ (*vitr)->getLiteracyWeightedPopulation("bureaucrats") * 0.25
						+ (*vitr)->getLiteracyWeightedPopulation("aristocrats") * 0.25;
					if (Configuration::getLeadershipConversion() == "linear")
					{
						newLeadership *= Configuration::getLeadershipFactor() / mitr->second.provinces.size();
						newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
						provinces[destNum]->setLeadership(newLeadership);
					}
					else if (Configuration::getLeadershipConversion() == "squareroot")
					{
						newLeadership = sqrt(newLeadership);
						newLeadership *= Configuration::getLeadershipFactor() / mitr->second.provinces.size();
						newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
						provinces[destNum]->setLeadership(newLeadership);
					}
				}
			}
		}
	}
}


void HoI3World::convertCapitals(const V2World &sourceWorld, provinceMapping provinceMap)
{
	//inverseProvinceMapping inverseProvinceMap = invertProvinceMap(provinceMap);
	//map<string, V2Country*> oldCountries = sourceWorld.getCountries();
	//for (unsigned int i = 0; i < countries.size(); i++)
	//{
	//	int sourceCountryIndex = countries[i].getSourceCountryIndex();
	//	if (sourceCountryIndex >= 0)
	//	{
	//		//int oldCapital = oldCountries[sourceCountryIndex]->getCapital();
	//		//log("\n	V2tag: %s	old capital: %4d", oldCountries[sourceCountryIndex]->getTag().c_str(), oldCapital);
	//		//inverseProvinceMapping::iterator itr = inverseProvinceMap.find(oldCapital);
	//		//if (itr != inverseProvinceMap.end())
	//		//{
	//		//	int newCapital = itr->second[0];
	//		//	countries[i].setCapital(newCapital);
	//		//	log("	new capital: %d", newCapital);
	//		//}
	//	}
	//}
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
		LOG(LogLevel::Error) << "Could not read tech map!";
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
			LOG(LogLevel::Error) << "unhandled tech link with first key " << keys[0].c_str() << "!";
			break;
		case 1:
			techTechMap[tech] = targetTechs;
			break;
		case 2:
			for (int i = 0; i <= HODInventionType::HOD_naval_exercises; ++i)
			{
				if (tech == HODInventionNames[i])
				{
					invTechMap[i] = targetTechs;
					break;
				}
			}
			break;
		}
	}


	for (map<string, HoI3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		const std::string& HoI3Tag = i->first;
		HoI3Country* destCountry = i->second;
		const V2Country* sourceCountry = destCountry->getSourceCountry();
		const std::string V2Tag = i->first;
		vector<string> techs = sourceCountry->getTechs();

		for (vector<string>::iterator itr = techs.begin(); itr != techs.end(); ++itr)
		{
			map<string, vector<pair<string, int> > >::iterator mapItr = techTechMap.find(*itr);
			if (mapItr != techTechMap.end())
			{
				for (vector<pair<string, int> >::iterator jtr = mapItr->second.begin(); jtr != mapItr->second.end(); ++jtr)
				{
					destCountry->setTechnology(jtr->first, jtr->second);
				}
			}
		}

		for (int inv = 0; inv <= HODInventionType::HOD_naval_exercises; ++inv)
		{
			if (sourceCountry->getInventionState(static_cast<HODInventionType>(inv)) == active)
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
						destCountry->setTechnology(jtr->first, jtr->second);
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
	vector<int> newLocationCandidates;
	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		map<int, HoI3Province*>::const_iterator provinceItr = provinces.find(*litr);
		if (provinceItr != provinces.end() && provinceItr->second->hasNavalBase()) // BE: Add? && provinceItr->second->isNotBlacklistedPort()
		{
			newLocationCandidates.push_back(*litr);
		}
	}
	return newLocationCandidates;
}


void HoI3World::convertArmies(V2World& sourceWorld, inverseProvinceMapping inverseProvinceMap, const map<int, int>& leaderIDMap)
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

	map<string, multimap<HoI3RegimentType, unsigned> > unitTypeMap; // <vic, hoi>
	Object* obj = doParseFile("unit_mapping.txt");
	vector<Object*> leaves = obj->getLeaves();
	if (leaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No unit mapping definitions loaded.";
		return;
	}
	leaves = leaves[0]->getLeaves();
	for (vector<Object*>::iterator itr = leaves.begin(); itr != leaves.end(); ++itr)
	{
		vector<Object*> vicKeys = (*itr)->getValue("vic");
		if (vicKeys.size() < 1)
			LOG(LogLevel::Error) << "invalid unit mapping(no source).";
		else
		{
			// multimap allows multiple mapping and ratio mapping (e.g. 4 irregulars converted to 3 militia brigades and 1 infantry brigade)
			multimap<HoI3RegimentType, unsigned> hoiList;
			vector<Object*> hoiKeys = (*itr)->getValue("hoi0");
			for (vector<Object*>::iterator hoiKey = hoiKeys.begin(); hoiKey != hoiKeys.end(); ++hoiKey)
			{
				hoiList.insert(std::pair<HoI3RegimentType, unsigned>(HoI3RegimentType((*hoiKey)->getLeaf()), 0));
			}

			hoiKeys = (*itr)->getValue("hoi1");
			for (vector<Object*>::iterator hoiKey = hoiKeys.begin(); hoiKey != hoiKeys.end(); ++hoiKey)
			{
				hoiList.insert(std::pair<HoI3RegimentType, unsigned>(HoI3RegimentType((*hoiKey)->getLeaf()), 1));
			}

			hoiKeys = (*itr)->getValue("hoi2");
			for (vector<Object*>::iterator hoiKey = hoiKeys.begin(); hoiKey != hoiKeys.end(); ++hoiKey)
			{
				hoiList.insert(std::pair<HoI3RegimentType, unsigned>(HoI3RegimentType((*hoiKey)->getLeaf()), 2));
			}

			hoiKeys = (*itr)->getValue("hoi3");
			for (vector<Object*>::iterator hoiKey = hoiKeys.begin(); hoiKey != hoiKeys.end(); ++hoiKey)
			{
				hoiList.insert(std::pair<HoI3RegimentType, unsigned>(HoI3RegimentType((*hoiKey)->getLeaf()), 3));
			}

			hoiKeys = (*itr)->getValue("hoi4");
			for (vector<Object*>::iterator hoiKey = hoiKeys.begin(); hoiKey != hoiKeys.end(); ++hoiKey)
			{
				hoiList.insert(std::pair<HoI3RegimentType, unsigned>(HoI3RegimentType((*hoiKey)->getLeaf()), 4));
			}

			for (vector<Object*>::iterator vicKey = vicKeys.begin(); vicKey != vicKeys.end(); ++vicKey)
			{
				if (hoiList.empty())
				{
					LOG(LogLevel::Debug) << "V2 unit type " << (*vicKey)->getLeaf() << " has no HoI3 mapping. "
						<< "If another mapping for this type is not found, units of this type will not be converted.";
					// Don't prevent it from going into the unitTypeMap, because we want to record that we have at least seen it
				}

				if (unitTypeMap.find((*vicKey)->getLeaf()) == unitTypeMap.end())
				{
					unitTypeMap[(*vicKey)->getLeaf()] = hoiList;
				}
				else
				{
					for (multimap<HoI3RegimentType, unsigned>::const_iterator listItr = hoiList.begin();
						listItr != hoiList.end(); ++listItr)
					{
						unitTypeMap[(*vicKey)->getLeaf()].insert(*listItr);
					}
				}
			}
		}
	}

	HoI3RegimentType hqBrigade("hq_brigade");

	for (map<string, HoI3Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		const V2Country* oldCountry = itr->second->getSourceCountry();

		if (oldCountry == NULL)
			continue;

		int airForceIndex = 0;

		HoI3RegGroup::resetRegGroupNameCounts();

		// A V2 unit type counter to keep track of how many V2 units of this type were converted.
		// Used to distribute HoI3 unit types in case of multiple mapping
		map<string, unsigned> typeCount;

		// Convert actual armies
		vector<V2Army*> sourceArmies = oldCountry->getArmies();
		for (vector<V2Army*>::iterator aitr = sourceArmies.begin(); aitr != sourceArmies.end(); ++aitr)
		{
			V2Army* oldArmy = *aitr;
			HoI3RegGroup destArmy;
			destArmy.setName(oldArmy->getName());
			destArmy.setFuelSupplies(oldArmy->getSupplies());
			if (oldArmy->getNavy())
			{
				destArmy.setForceType(navy);
				destArmy.setAtSea(oldArmy->getAtSea());
			}
			else
			{
				destArmy.setForceType(land);
			}

		//	map<int, int>::const_iterator lmapitr = leaderIDMap.find(aitr->getLeaderID());
		//	if (lmapitr != leaderIDMap.end())
		//	{
		//		destArmy.setLeaderID(lmapitr->second);
		//	}

			vector<int> locationCandidates = getHoI3ProvinceNums(inverseProvinceMap, oldArmy->getLocation());
			if (locationCandidates.size() == 0)
			{
				LOG(LogLevel::Warning) << "Army or Navy " << oldArmy->getName() << " assigned to unmapped province " << oldArmy->getLocation() << "; placing units in the production queue.";
				destArmy.setProductionQueue(true);
			}

			bool usePort = false;
			// guarantee that navies are assigned to sea provinces, or land provinces with naval bases
			if (oldArmy->getNavy())
			{
				map<int, HoI3Province*>::const_iterator pitr = provinces.find(locationCandidates[0]);
				if (pitr != provinces.end() && pitr->second->isLand())
				{
					usePort = true;
					locationCandidates = getPortProvinces(locationCandidates);
					if (locationCandidates.size() == 0)
					{
						LOG(LogLevel::Warning) << "Navy " << oldArmy->getName() << " assigned to V2 province " << oldArmy->getLocation() << " which has no corresponding HoI3 port provinces; placing units in the production queue.";
						destArmy.setProductionQueue(true);
					}
				}
			}

			int selectedLocation;
			HoI3Province* locationProvince;
			if (locationCandidates.size() > 0) // BE TODO: Handle invalid location units in production queue
			{
				selectedLocation = locationCandidates[rand() % locationCandidates.size()];
				if (oldArmy->getNavy() && usePort)
				{
					vector<int>::iterator white = std::find(port_whitelist.begin(), port_whitelist.end(), selectedLocation);
					if (white == port_whitelist.end())
					{
						LOG(LogLevel::Debug) << "assigning navy to non - whitelisted port province " << selectedLocation << ".If you encounter crashes, try blacklisting this province.";
					}
				}
				destArmy.setLocation(selectedLocation);
				if (!oldArmy->getAtSea())
					destArmy.setLocationContinent(continents[selectedLocation]);
				map<int, HoI3Province*>::iterator pitr = provinces.find(selectedLocation);

				if (pitr != provinces.end())
					locationProvince = pitr->second;
				else
				{
					LOG(LogLevel::Warning) << "HoI3 Province for ID: " << selectedLocation << " not found. Placing units in the production queue";
					destArmy.setProductionQueue(true);
				}
			}

			// air units need to be split into air forces
			HoI3RegGroup destAirForce;
			destAirForce.setFuelSupplies(oldArmy->getSupplies());
			destAirForce.setForceType(air);

			if (!destArmy.getProductionQueue())
			{
				destAirForce.setLocation(selectedLocation);
				if (!oldArmy->getAtSea())
					destAirForce.setLocationContinent(continents[selectedLocation]);
			}

			vector<V2Regiment> sourceRegiments = oldArmy->getRegiments();
			for (vector<V2Regiment>::iterator ritr = sourceRegiments.begin(); ritr != sourceRegiments.end(); ++ritr)
			{
				HoI3Regiment destReg;
				destReg.setName(ritr->getName());

				map<string, multimap<HoI3RegimentType, unsigned> >::iterator typeMap = unitTypeMap.find(ritr->getType());
				if (typeMap == unitTypeMap.end())
				{
					LOG(LogLevel::Debug) << "Regiment " << ritr->getName() << " has unmapped unit type " << ritr->getType() << ", dropping.";
					continue;
				}
				else if (typeMap->second.empty()) // Silently skip the ones that have no mapping, because it was logged earlier
				{
					continue;
				}

				const multimap<HoI3RegimentType, unsigned> &hoiMapList = typeMap->second;
				unsigned destMapIndex = typeCount[ritr->getType()]++ % hoiMapList.size();
				multimap<HoI3RegimentType, unsigned>::const_iterator destTypeItr = hoiMapList.begin();

				// Count down from destMapIndex to iterate to the correct destination type
				for (; destMapIndex > 0 && destTypeItr != hoiMapList.end(); --destMapIndex)
				{
					++destTypeItr;
				}

				if (!destTypeItr->first.getUsableBy().empty()) // This unit type is exclusive
				{
					unsigned skippedCount = 0;
					while (skippedCount < hoiMapList.size())
					{
						const set<string> &usableByCountries = destTypeItr->first.getUsableBy();

						if (usableByCountries.empty() || usableByCountries.find(itr->first) != usableByCountries.end())
						{
							break;
						}

						++skippedCount;
						++destTypeItr;
						++typeCount[ritr->getType()];
						if (destTypeItr == hoiMapList.end())
						{
							destTypeItr = hoiMapList.begin();
						}
					}

					if (skippedCount == hoiMapList.size())
					{
						LOG(LogLevel::Warning) << "Regiment " << ritr->getName() << " has unit type " << ritr->getType()
							<< ", but it is mapped only to units exclusive to other countries. Dropping.";
						continue;
					}
				}

				// This should not happen, but just in case...
				//if (destTypeItr == hoiMapList.end())
				//{
				//	destTypeItr = hoiMapList.begin();
				//}

				destReg.setType(destTypeItr->first);
				destReg.setHistoricalModel(destTypeItr->second);

				double strengthFactor = 1.0;
				if (destReg.getForceType() != navy) // V2 and HoI naval units are already scaled correctly
					strengthFactor = ((double)destTypeItr->first.getMaxStrength()) / 3.0;
				destReg.setStrength(ritr->getStrength() * strengthFactor);
				destReg.setOrganization(ritr->getOrganization()); // XXX: needs scaling!
				destReg.setExperience(ritr->getExperience()); // XXX: needs scaling?

				// Add to army/navy or newly created air force as appropriate
				if (destReg.getForceType() != air)
					destArmy.addRegiment(destReg, true);
				else
					destAirForce.addRegiment(destReg, true);

				// Contribute to country's practicals
				itr->second->getPracticals()[destTypeItr->first.getPracticalBonus()] += Configuration::getPracticalsScale() * destTypeItr->first.getPracticalBonusFactor();
			}

			if (!destArmy.isEmpty() && !destArmy.getProductionQueue())
			{
				if (oldArmy->getNavy() && locationProvince->isLand())
				{
					// make sure a naval base is waiting for them
					locationProvince->requireNavalBase(min(10, locationProvince->getNavalBase() + destArmy.size()));
				}
				destArmy.createHQs(hqBrigade); // Generate HQs for all hierarchies
				itr->second->addArmy(destArmy);
			}
			else if (!destArmy.isEmpty())
			{
				itr->second->addArmy(destArmy);
			}

			if (!destAirForce.isEmpty() && !destArmy.getProductionQueue())
			{
				// we need to put an airbase here, so make sure we're in our own territory
				if (locationProvince->getOwner() == itr->first)
				{
					// make sure an airbase is waiting for them
					locationProvince->requireAirBase(min(10, locationProvince->getAirBase() + destAirForce.size()));

					stringstream name;
					name << ++airForceIndex << CardinalToOrdinal(airForceIndex) << " Air Force";
					destAirForce.setName(name.str());
					itr->second->addArmy(destAirForce);
				}
				else
				{
					LOG(LogLevel::Warning) << "Airforce attached to army " << oldArmy->getName() << " is not at a location where an airbase can be created; placing units in the production queue.";
					destArmy.setProductionQueue(true);
				}
			}

			if (!destAirForce.isEmpty() && destArmy.getProductionQueue())
			{
				destAirForce.setProductionQueue(true);
				itr->second->addArmy(destAirForce);
			}
		}

		// Anticipate practical points being awarded for completing the unit constructions
		for (vector<HoI3RegGroup>::const_iterator armyItr = itr->second->getArmies().begin(); armyItr != itr->second->getArmies().end(); ++armyItr)
		{
			if (armyItr->getProductionQueue())
			{
				armyItr->undoPracticalAddition(itr->second->getPracticals());
			}
		}
	}
}


void HoI3World::checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string& leader, string factionName)
{
	for (vector<string>::const_iterator itr = candidateTags.begin(); itr != candidateTags.end(); ++itr)
	{
		// get HoI3 tag from V2 tag
		string hoiTag = countryMap[*itr];
		if (hoiTag.empty())
		{
			LOG(LogLevel::Warning) << "Tag " << *itr << " requested for " << factionName << " faction, but is unmapped!";
			continue;
		}

		// find HoI3 nation and ensure that it has land
		auto citr = countries.find(hoiTag);
		if (citr != countries.end())
		{
			if (citr->second->getProvinces().size() == 0)
			{
				LOG(LogLevel::Warning) << "Tag " << *itr << " requested for " << factionName << " faction, but is landless!";
			}
			else
			{
				LOG(LogLevel::Debug) << *itr << " added to " << factionName  << " faction";
				citr->second->setFaction(factionName);
				if (leader == "")
				{
					leader = citr->first;
				}
			}
		}
		else
		{
			LOG(LogLevel::Warning) << "Tag " << *itr << " requested for " << factionName << " faction, but does not exist!";
		}
	}
}


void HoI3World::factionSatellites()
{
	// make sure that any vassals are in their master's faction
	const vector<HoI3Agreement> &agr = diplomacy.getAgreements();
	for (auto itr = agr.begin(); itr != agr.end(); ++itr)
	{
		if (itr->type == "vassal")
		{
			auto masterCountry		= countries.find(itr->country1);
			auto satelliteCountry	= countries.find(itr->country2);
			if ((masterCountry != countries.end()) && (masterCountry->second->getFaction() != "") && (satelliteCountry != countries.end()))
			{
				satelliteCountry->second->setFaction(masterCountry->second->getFaction());
			}
		}
	}
}


void HoI3World::configureFactions(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	string axisLeader;
	string alliesLeader;
	string cominternLeader;

	// find faction memebers
	if (Configuration::getFactionLeaderAlgo() == "manual")
	{
		LOG(LogLevel::Info) << "Manual faction allocation requested.";
		checkManualFaction(countryMap, Configuration::getManualAxisFaction(), axisLeader, "axis");
		checkManualFaction(countryMap, Configuration::getManualAlliesFaction(), alliesLeader, "allies");
		checkManualFaction(countryMap, Configuration::getManualCominternFaction(), cominternLeader, "comintern");
	}
	else if (Configuration::getFactionLeaderAlgo() == "auto")
	{
		LOG(LogLevel::Info) << "Auto faction allocation requested.";

		const vector<string>& greatCountries = sourceWorld.getGreatCountries();
		for (vector<string>::const_iterator countryItr = greatCountries.begin(); countryItr != greatCountries.end(); ++countryItr)
		{
			map<string, HoI3Country*>::iterator itr = countries.find(countryMap[*countryItr]);
			if (itr != countries.end())
			{
				HoI3Country* country = itr->second;
				const string government = country->getGovernment();
				const string ideology = country->getIdeology();
				if (
						(	government == "national_socialism" || government == "fascist_republic" || government == "germanic_fascist_republic" ||
							government == "right_wing_republic" || government == "hungarian_right_wing_republic" || government == "right_wing_autocrat" ||
							government == "absolute_monarchy" || government == "imperial"
						) &&
						(ideology == "national_socialist" || ideology == "fascistic" || ideology == "paternal_autocrat")
					)
				{
					if (axisLeader == "")
					{
						axisLeader = itr->first;
						country->setFaction("axis");
					}
					else
					{
						// Check if ally of leader
						const set<string>& allies = country->getAllies();
						if (allies.find(axisLeader) != allies.end())
						{
							country->setFaction("axis");
						}
					}
				}
				else if	(
								(	government == "social_conservatism" || government == "constitutional_monarchy" || government == "spanish_social_conservatism" ||
									government == "market_liberalism" || government == "social_democracy" || government == "social_liberalism"
								) &&
								(ideology == "social_conservative" || ideology == "market_liberal" || ideology == "social_liberal" || ideology == "social_democrat")
							)
				{
					if (alliesLeader == "")
					{
						alliesLeader = itr->first;
						country->setFaction("allies");
					}
					else
					{
						// Check if ally of leader
						const set<string> &allies = country->getAllies();
						if (allies.find(alliesLeader) != allies.end())
						{
							country->setFaction("allies");
						}
					}
				}
				// Allow left_wing_radicals, absolute monarchy and imperial. Being more tolerant for great powers, because we want comintern to be powerful
				else if	(
								(	
									government == "left_wing_radicals" || government == "socialist_republic" || government == "federal_socialist_republic" ||
									government == "absolute_monarchy" || government == "imperial"
								) &&
								(ideology == "left_wing_radical" || ideology == "leninist" || ideology == "stalinist")
							)
				{
					if (cominternLeader == "")
					{
						cominternLeader = itr->first; // Faction leader
						country->setFaction("comintern");
					}
					else
					{
						// Check if ally of leader
						const set<string> &allies = country->getAllies();
						if (allies.find(alliesLeader) != allies.end())
						{
							country->setFaction("comintern");
						}
					}
				}
			}
			else
			{
				LOG(LogLevel::Error) << "V2 great power " << *countryItr << " not found.";
			}
		}

		// Comintern get a boost to its membership for being internationalistic
		// BE: TODO If comintern is empty, find the next best leader
		// Go through all stalinist, leninist countries and add them to comintern if they're not hostile to leader
		for (map<string, HoI3Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
		{
			const string government = itr->second->getGovernment();
			const string ideology = itr->second->getIdeology();
			if (
					(government == "socialist_republic" || government == "federal_socialist_republic") &&
					(ideology == "left_wing_radical" || ideology == "leninist" || ideology == "stalinist")
				)
			{
				if (itr->second->getFaction() == "") // Skip if already a faction member
				{
					if (cominternLeader == "")
					{
						cominternLeader = itr->first; // Faction leader
						itr->second->setFaction("comintern");
					}
					else
					{
						// Check if enemy of leader
						bool enemy = false;
						const map<string, HoI3Relations*> &relations = itr->second->getRelations();
						map<string, HoI3Relations*>::const_iterator relationItr = relations.find(cominternLeader);
						if (relationItr != relations.end() && relationItr->second->atWar())
						{
							enemy = true;
						}

						if (!enemy)
						{
							itr->second->setFaction("comintern");
						}
					}
				}
			}
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Error: unrecognized faction algorithm \"" << Configuration::getFactionLeaderAlgo() << "\"!";
		return;
	}

	//set faction leaders to be earlier in the country ordering
	vector<string>::iterator i = countryOrder.begin();
	while (i != countryOrder.end())
	{
		if (*i == axisLeader)
		{
			countryOrder.erase(i);
			countryOrder.insert(countryOrder.begin(), axisLeader);
			break;
		}
		i++;
		if (i == countryOrder.end())
		{
			countryOrder.insert(countryOrder.begin(), axisLeader);
			break;
		}
	}
	i = countryOrder.begin();
	while (i != countryOrder.end())
	{
		if (*i == alliesLeader)
		{
			countryOrder.erase(i);
			countryOrder.insert(countryOrder.begin(), alliesLeader);
			break;
		}
		i++;
		if (i == countryOrder.end())
		{
			countryOrder.insert(countryOrder.begin(), axisLeader);
			break;
		}
	}
	i = countryOrder.begin();
	while (i != countryOrder.end())
	{
		if (*i == cominternLeader)
		{
			countryOrder.erase(i);
			countryOrder.insert(countryOrder.begin(), cominternLeader);
			break;
		}
		i++;
		if (i == countryOrder.end())
		{
			countryOrder.insert(countryOrder.begin(), axisLeader);
			break;
		}
	}
	for (vector<string>::iterator i = countryOrder.begin(); i != countryOrder.end(); i++)
	{
		if (*i == "REB")
		{
			countryOrder.erase(i);
			countryOrder.insert(countryOrder.begin(), "REB");
		}
	}

	// push satellites into the same faction as their parents
	factionSatellites();

	// set alignments
	for (map<string, HoI3Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		const string countryFaction = itr->second->getFaction();

		// force alignment for faction members
		if (countryFaction == "axis")
		{
			itr->second->getAlignment()->alignToAxis();
		}
		else if (countryFaction == "allies")
		{
			itr->second->getAlignment()->alignToAllied();
		}
		else if (countryFaction == "comintern")
		{
			itr->second->getAlignment()->alignToComintern();
		}
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
			if (axisLeader != "")
			{
				HoI3Relations* relObj = itr->second->getRelations(axisLeader);
				if (relObj != NULL)
				{
					double axisRelations = relObj->getRelations();
					if (axisRelations >= 0.0)
					{
						axisStart.moveTowardsAxis(axisRelations * positiveScale);
					}
					else // axisRelations < 0.0
					{
						axisStart.moveTowardsAxis(axisRelations * negativeScale);
					}
				}
			}
			if (alliesLeader != "")
			{
				HoI3Relations* relObj = itr->second->getRelations(alliesLeader);
				if (relObj != NULL)
				{
					double alliesRelations = relObj->getRelations();
					if (alliesRelations >= 0.0)
					{
						alliesStart.moveTowardsAllied(alliesRelations * positiveScale);
					}
					else // alliesRelations < 0.0
					{
						alliesStart.moveTowardsAllied(alliesRelations * negativeScale);
					}
				}
			}
			if (cominternLeader != "")
			{
				HoI3Relations* relObj = itr->second->getRelations(cominternLeader);
				if (relObj != NULL)
				{
					double cominternRelations = relObj->getRelations();
					if (cominternRelations >= 0.0)
					{
						cominternStart.moveTowardsComintern(cominternRelations * positiveScale);
					}
					else // cominternRelations < 0.0
					{
						cominternStart.moveTowardsComintern(cominternRelations * negativeScale);
					}
				}
			}
			(*(itr->second->getAlignment())) = HoI3Alignment::getCentroid(axisStart, alliesStart, cominternStart);
		}
	}
}


void HoI3World::convertDiplomacy(V2World& sourceWorld, CountryMapping countryMap)
{
	const vector<V2Agreement> &agreements = sourceWorld.getDiplomacy()->getAgreements();
	for (vector<V2Agreement>::const_iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		string HoI3Tag1 = countryMap[itr->country1];
		if (HoI3Tag1.empty())
		{
			LOG(LogLevel::Error) << "V2 Country " << itr->country1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		string HoI3Tag2 = countryMap[itr->country2];
		if (HoI3Tag2.empty())
		{
			LOG(LogLevel::Error) << "V2 Country " << itr->country2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		map<string, HoI3Country*>::iterator hoi3Country1 = countries.find(HoI3Tag1);
		map<string, HoI3Country*>::iterator hoi3Country2 = countries.find(HoI3Tag2);
		if (hoi3Country1 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI3 country " << HoI3Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		if (hoi3Country2 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI3 country " << HoI3Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		// BE: Superfluous?
		//HoI3Relations* r1 = hoi3Country1->second->getRelations(HoI3Tag2);
		//if (!r1)
		//{
		//	r1 = new HoI3Relations(HoI3Tag2);
		//	hoi3Country1->second->addRelation(r1);
		//}
		//HoI3Relations* r2 = hoi3Country2->second->getRelations(HoI3Tag1);
		//if (!r2)
		//{
		//	r2 = new HoI3Relations(HoI3Tag1);
		//	hoi3Country2->second->addRelation(r2);
		//}

		// XXX: unhandled V2 diplo types:
		// casus_belli: -> threat maybe?
		// warsubsidy: probably just drop, maybe cash trade...

		// shared diplo types
		if ((itr->type == "alliance") || (itr->type == "vassal"))
		{
			// copy agreement
			HoI3Agreement hoi3a;
			hoi3a.country1 = HoI3Tag1;
			hoi3a.country2 = HoI3Tag2;
			hoi3a.start_date = itr->start_date;
			hoi3a.type = itr->type;
			diplomacy.addAgreement(hoi3a);

			if (itr->type == "alliance")
			{
				hoi3Country1->second->editAllies().insert(HoI3Tag2);
			}
		}

		// XXX: unhandled HoI3 diplo types:
		// trade: probably just omit
		// influence: <- influence maybe?
	}

	// Relations and guarantees
	for (map<string, HoI3Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		const std::map<string, HoI3Relations*> &relations = itr->second->getRelations();

		for (std::map<string, HoI3Relations*>::const_iterator relationItr = relations.begin(); relationItr != relations.end(); ++relationItr)
		{
			HoI3Agreement hoi3a;
			if (itr->first < relationItr->first) // Put it in order to eliminate duplicate relations entries
			{
				hoi3a.country1 = itr->first;
				hoi3a.country2 = relationItr->first;
			}
			else
			{
				hoi3a.country2 = relationItr->first;
				hoi3a.country1 = itr->first;
			}

			hoi3a.value = relationItr->second->getRelations();
			hoi3a.start_date = date("1930.1.1"); // Arbitrary date
			hoi3a.type = "relation";
			diplomacy.addAgreement(hoi3a);

			if (relationItr->second->getGuarantee())
			{
				HoI3Agreement hoi3a;
				hoi3a.country1 = itr->first;
				hoi3a.country2 = relationItr->first;
				hoi3a.start_date = date("1930.1.1"); // Arbitrary date
				hoi3a.type = "guarantee";
				diplomacy.addAgreement(hoi3a);
			}
		}
	}
}


map<string, HoI3Country*> HoI3World::getPotentialCountries() const
{
	map<string, HoI3Country*> retVal;
	for (vector<HoI3Country*>::const_iterator i = potentialCountries.begin(); i != potentialCountries.end(); i++)
	{
		retVal[(*i)->getTag()] = *i;
	}

	return retVal;
}


void HoI3World::copyFlags(const V2World &sourceWorld, CountryMapping countryMap)
{
	LOG(LogLevel::Debug) << "Copying flags";

	// Create output folders.
	std::string outputGraphicsFolder = "Output\\" + Configuration::getOutputName() + "\\gfx";
	if (!WinUtils::TryCreateFolder(outputGraphicsFolder))
	{
		return;
	}
	std::string outputFlagFolder = outputGraphicsFolder + "\\flags";
	if (!WinUtils::TryCreateFolder(outputFlagFolder))
	{
		return;
	}

	const std::string folderPath = Configuration::getV2Path() + "\\gfx\\flags";
	map<string, V2Country*> sourceCountries = sourceWorld.getCountries();
	for (map<string, V2Country*>::iterator i = sourceCountries.begin(); i != sourceCountries.end(); i++)
	{
		std::string V2Tag = i->first;
		V2Country* sourceCountry = i->second;
		std::string V2FlagFile = sourceCountry->getFlagFile();
		const std::string& HoI3Tag = countryMap[V2Tag];

		bool flagCopied = false;
		vector<string> mods = Configuration::getVic2Mods();
		for (auto mod: mods)
		{
			string sourceFlagPath = Configuration::getV2Path() + "\\mod\\" + mod + "\\gfx\\flags\\"+ V2FlagFile;
			if (WinUtils::DoesFileExist(sourceFlagPath))
			{
				std::string destFlagPath = outputFlagFolder + '\\' + HoI3Tag + ".tga";
				flagCopied = WinUtils::TryCopyFile(sourceFlagPath, destFlagPath);
				if (flagCopied)
				{
					break;
				}
			}
		}
		if (!flagCopied)
		{
			std::string sourceFlagPath = folderPath + '\\' + V2FlagFile;
			if (WinUtils::DoesFileExist(sourceFlagPath))
			{
				std::string destFlagPath = outputFlagFolder + '\\' + HoI3Tag + ".tga";
				WinUtils::TryCopyFile(sourceFlagPath, destFlagPath);
			}
		}
	}
}