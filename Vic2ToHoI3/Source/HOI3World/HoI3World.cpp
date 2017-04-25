/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include "ParadoxParser.h"
#include "Log.h"
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


void HoI3World::importProvinces(const provinceMapping& provinceMap)
{
	LOG(LogLevel::Info) << "Importing provinces";

	struct _finddata_t	provinceFileData;
	intptr_t					fileListing = NULL;
	list<string>			directories;

	directories.push_back("");
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

				int provinceNum = newProvince->getNum();
				auto provinceItr = provinces.find(provinceNum);
				if (provinceItr == provinces.end())
				{
					provinces.insert(make_pair(provinceNum, newProvince));
				}
				else
				{
					provinceItr->second->addFilename(directories.front() + "\\" + provinceFileData.name);
					provinceNum++;
				}
			}
		} while (_findnext(fileListing, &provinceFileData) == 0);
		_findclose(fileListing);
		directories.pop_front();
	}
	checkAllProvincesMapped(provinceMap);
}


void HoI3World::checkCoastalProvinces()
{
	// determine whether each province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise HoI3 crashes)
	Object*	obj2 = doParseFile((Configuration::getHoI3Path() + "\\tfh\\map\\positions.txt").c_str());
	vector<Object*> objProv = obj2->getLeaves();
	if (objProv.size() == 0)
	{
		LOG(LogLevel::Error) << "map\\positions.txt failed to parse.";
		exit(1);
	}
	for (auto itr: objProv)
	{
		int provinceNum = atoi(itr->getKey().c_str());
		vector<Object*> objPos = itr->getValue("building_position");
		if (objPos.size() == 0)
		{
			continue;
		}
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
}


void HoI3World::importPotentialCountries()
{
	countries.clear();

	LOG(LogLevel::Info) << "Getting potential countries";
	potentialCountries.clear();
	const date FirstStartDate("1936.1.1");
	ifstream HoI3CountriesInput;
	struct _stat st;
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

		HoI3Country* newCountry = new HoI3Country(tag, countryFileName, this);
		potentialCountries.insert(make_pair(tag, newCountry));
	}
	HoI3CountriesInput.close();

	countryOrder.push_back("REB");
}


void HoI3World::output() const
{
	outputCommonCountries();
	outputAutoexecLua();
	outputLocalisations();
	outputHistory();
}


void HoI3World::outputCommonCountries() const
{
	// Create common\countries path.
	string countriesPath = "Output\\" + Configuration::getOutputName() + "\\common\\countries";
	if (!WinUtils::TryCreateFolder(countriesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output\\" + Configuration::getOutputName() + "\\common\\countries\"";
		exit(-1);
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
			// Search potential countries. This should normally only be REB, because only active countries should be in countryOrder
			auto potentialCountry = potentialCountries.find(hoiTag);
			if (potentialCountry != potentialCountries.end())
			{
				country = potentialCountry->second;
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

	for (auto countryItr: countries)
	{
		// Skip countries that have already been processed
		for (auto orderItr: countryOrder)
		{
			if (orderItr == countryItr.first)
			{
				continue;
			}
		}

		countryItr.second->outputToCommonCountriesFile(allCountriesFile);
	}

	// There are bugs and crashes if all potential vanilla HoI3 countries are not in the common\countries.txt file
	// Also, the Rebels are to be included
	for (auto potentialItr: potentialCountries)
	{
		// Skip countries that have already been processed. (This should normally only be REB, because only active countries should be in countryOrder)
		for (auto orderItr: countryOrder)
		{
			if (orderItr == potentialItr.first)
			{
				continue;
			}
		}

		if (countries.find(potentialItr.first) == countries.end())
		{
			potentialItr.second->outputToCommonCountriesFile(allCountriesFile);
		}
	}
	fprintf(allCountriesFile, "\n");
	fclose(allCountriesFile);
}


void HoI3World::outputAutoexecLua() const
{
	// output autoexec.lua
	FILE* autoexec;
	if (fopen_s(&autoexec, ("Output\\" + Configuration::getOutputName() + "\\script\\autoexec.lua").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create autoexec.lua";
		exit(-1);
	}

	ifstream sourceFile;
	sourceFile.open("autoexecTEMPLATE.lua", ifstream::in);
	if (!sourceFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open autoexecTEMPLATE.lua";
		exit(-1);
	}
	while (!sourceFile.eof())
	{
		string line;
		getline(sourceFile, line);
		fprintf(autoexec, "%s\n", line.c_str());
	}
	sourceFile.close();

	for (auto country : potentialCountries)
	{
		fprintf(autoexec, "require('%s')\n", country.first.c_str());
	}
	fprintf(autoexec, "\n");
	fclose(autoexec);
}


void HoI3World::outputLocalisations() const
{
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
}


void HoI3World::outputHistory() const
{
	LOG(LogLevel::Debug) << "Writing provinces";
	for (auto provItr: provinces)
	{
		provItr.second->output();
	}
	LOG(LogLevel::Debug) << "Writing countries";
	for (auto countryItr: countries)
	{
		countryItr.second->output();
	}
	// Override vanilla history to suppress vanilla OOB and faction membership being read
	for (auto potentialItr: potentialCountries)
	{
		if (countries.find(potentialItr.first) == countries.end())
		{
			potentialItr.second->output();
		}
	}
	LOG(LogLevel::Debug) << "Writing diplomacy";
	diplomacy.output();
}


void HoI3World::getProvinceLocalizations(const string& file)
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


void HoI3World::convertCountries(const V2World &sourceWorld, const CountryMapping& countryMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	vector<string> outputOrder;
	outputOrder.clear();
	for (auto potentialItr: potentialCountries)
	{
		outputOrder.push_back(potentialItr.first);
	}

	for (auto sourceItr: sourceWorld.getCountries())
	{
		// don't convert rebels
		if (sourceItr.first == "REB")
		{
			continue;
		}

		HoI3Country* destCountry = NULL;
		const std::string& HoI3Tag = countryMap[sourceItr.first];
		if (!HoI3Tag.empty())
		{
			auto candidateDestCountry = potentialCountries.find(HoI3Tag);
			if (candidateDestCountry != potentialCountries.end())
			{
				destCountry = candidateDestCountry->second;
			}
			if (destCountry == NULL) // No such HoI3 country exists yet for this tag so make a new one
			{
				std::string countryFileName = '/' + sourceItr.second->getName() + ".txt";
				destCountry = new HoI3Country(HoI3Tag, countryFileName, this, true);
			}
			V2Party* rulingParty = sourceWorld.getRulingParty(sourceItr.second);
			if (rulingParty == NULL)
			{
				LOG(LogLevel::Error) << "Could not find the ruling party for " <<  sourceItr.first << ". Were all mods correctly included?";
				exit(-1);
			}
			destCountry->initFromV2Country(sourceWorld, sourceItr.second, rulingParty->ideology, outputOrder, countryMap, inverseProvinceMap, leaderMap, V2Localisations, governmentJobs, namesMap, portraitMap, cultureMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap);
			countries.insert(make_pair(HoI3Tag, destCountry));
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not convert V2 tag " << sourceItr.first << " to HoI3";
		}
	}

	// initialize all potential countries
	// ALL potential countries should be output to the file, otherwise some things don't get initialized right in HoI3
	for (auto potentialItr: potentialCountries)
	{
		map<string, HoI3Country*>::iterator citr = countries.find(potentialItr.first);
		if (citr == countries.end())
		{
			potentialItr.second->initFromHistory();
		}
	}

	for (auto greatItr: sourceWorld.getGreatCountries())
	{
		countryOrder.push_back(countryMap[greatItr]);
	}
}


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<V2Province*> provinces;
	int totalPopulation;
};


void HoI3World::convertProvinceOwners(const V2World &sourceWorld, const provinceMapping& provinceMap, const CountryMapping& countryMap)
{
	for (auto provItr : provinces)
	{
		// get the appropriate mapping
		provinceMapping::const_iterator provinceLink = provinceMap.find(provItr.first);
		if ((provinceLink == provinceMap.end()) || (provinceLink->second.size() == 0))
		{
			LOG(LogLevel::Warning) << "No source for " << provItr.second->getName() << " (province " << provItr.first << ')';
			continue;
		}
		else if (provinceLink->second[0] == 0)
		{
			continue;
		}

		provItr.second->clearCores();

		V2Province*	oldProvince = NULL;
		V2Country*	oldOwner = NULL;

		// determine ownership by province count, or total population (if province count is tied)
		map<string, MTo1ProvinceComp> provinceBins;
		double newProvinceTotalPop = 0;
		for (auto srcProvItr : provinceLink->second)
		{
			V2Province* srcProvince = sourceWorld.getProvince(srcProvItr);
			if (!srcProvince)
			{
				LOG(LogLevel::Warning) << "Old province " << provinceLink->second[0] << " does not exist (bad mapping?)";
				continue;
			}
			V2Country* owner = srcProvince->getOwner();
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
			provinceBins[tag].provinces.push_back(srcProvince);
			provinceBins[tag].totalPopulation += srcProvince->getTotalPopulation();
			newProvinceTotalPop += srcProvince->getTotalPopulation();
			// I am the new owner if there is no current owner, or I have more provinces than the current owner,
			// or I have the same number of provinces, but more population, than the current owner
			if ((oldOwner == NULL)
				|| (provinceBins[tag].provinces.size() > provinceBins[oldOwner->getTag()].provinces.size())
				|| ((provinceBins[tag].provinces.size() == provinceBins[oldOwner->getTag()].provinces.size())
					&& (provinceBins[tag].totalPopulation > provinceBins[oldOwner->getTag()].totalPopulation)))
			{
				oldOwner = owner;
				oldProvince = srcProvince;
			}
		}
		if (oldOwner == NULL)
		{
			provItr.second->setOwner("");
			continue;
		}

		// convert from the source provinces
		const string HoI3Tag = countryMap[oldOwner->getTag()];
		if (HoI3Tag.empty())
		{
			LOG(LogLevel::Warning) << "Could not map provinces owned by " << oldOwner->getTag();
		}
		else
		{
			provItr.second->setOwner(HoI3Tag);
			map<string, HoI3Country*>::iterator ownerItr = countries.find(HoI3Tag);
			if (ownerItr != countries.end())
			{
				ownerItr->second->addProvince(provItr.second);
			}
			provItr.second->convertFromOldProvince(oldProvince);

			for (auto srcOwnerItr : provinceBins)
			{
				for (auto srcProvItr : srcOwnerItr.second.provinces)
				{
					// convert cores
					vector<V2Country*> oldCores = srcProvItr->getCores(sourceWorld.getCountries());
					for (auto oldCoreItr : oldCores)
					{
						// skip this core if the country is the owner of the V2 province but not the HoI3 province
						// (i.e. "avoid boundary conflicts that didn't exist in V2").
						// this country may still get core via a province that DID belong to the current HoI3 owner
						if ((oldCoreItr->getTag() == srcOwnerItr.first) && (oldCoreItr != oldOwner))
						{
							continue;
						}

						const string coreOwner = countryMap[oldCoreItr->getTag()];
						if (coreOwner != "")
						{
							provItr.second->addCore(coreOwner);
						}
					}
				}
			}
		}
	}
}


void HoI3World::convertNavalBases(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap)
{
	// convert naval bases. There should only be one per Vic2 naval base
	for (auto mapping: inverseProvinceMap)
	{
		auto sourceProvince = sourceWorld.getProvince(mapping.first);
		int navalBaseLevel = sourceProvince->getNavalBase();
		navalBaseLevel = max(0, (navalBaseLevel - 3) * 2 + 1);
		if (mapping.second.size() > 0)
		{
			auto destProvince = provinces.find(mapping.second[0]);
			if (destProvince != provinces.end())
			{
				destProvince->second->requireNavalBase(navalBaseLevel);
			}
		}
	}
}


void HoI3World::convertProvinceItems(const V2World& sourceWorld, const provinceMapping& provinceMap, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, const HoI3AdjacencyMapping& HoI3AdjacencyMap)
{
	// now that all provinces have had owners and cores set, convert their other items
	for (auto mapping: inverseProvinceMap)
	{
		// get the source province
		int srcProvinceNum = mapping.first;
		V2Province* sourceProvince	= sourceWorld.getProvince(srcProvinceNum);
		if (sourceProvince == NULL)
		{
			continue;
		}

		// convert items that apply to all destination provinces
		for (auto dstProvinceNum: mapping.second)
		{
			// get the destination province
			auto dstProvItr = provinces.find(dstProvinceNum);
			if (dstProvItr == provinces.end())
			{
				continue;
			}

			// source provinces from other owners should not contribute to the destination province
			if (countryMap[sourceProvince->getOwnerString()] != dstProvItr->second->getOwner())
			{
				continue;
			}

			// determine if this is a border province or not
			bool borderProvince = false;
			if (HoI3AdjacencyMap.size() > static_cast<unsigned int>(dstProvinceNum))
			{
				const vector<adjacency> adjacencies = HoI3AdjacencyMap[dstProvinceNum];
				for (auto adj: adjacencies)
				{
					auto province				= provinces.find(dstProvinceNum);
					auto adjacentProvince	= provinces.find(adj.to);
					if ((province != provinces.end()) && (adjacentProvince != provinces.end()) && (province->second->getOwner() != adjacentProvince->second->getOwner()))
					{
						borderProvince = true;
						break;
					}
				}
			}

			// convert forts, naval bases, and infrastructure
			int fortLevel = sourceProvince->getFort();
			fortLevel = max(0, (fortLevel - 5) * 2 + 1);
			if (dstProvItr->second->getNavalBase() > 0)
			{
				dstProvItr->second->requireCoastalFort(fortLevel);
			}
			if (borderProvince)
			{
				dstProvItr->second->requireLandFort(fortLevel);
			}
			dstProvItr->second->requireInfrastructure((int)Configuration::getMinInfra());
			if (sourceProvince->getInfra() > 0) // No infra stays at minInfra
			{
				dstProvItr->second->requireInfrastructure(sourceProvince->getInfra() + 4);
			}

			if ((Configuration::getLeadershipConversion() == "linear") || (Configuration::getLeadershipConversion() == "squareroot"))
			{
				dstProvItr->second->setLeadership(0.0);
			}
			if ((Configuration::getManpowerConversion() == "linear") || (Configuration::getManpowerConversion() == "squareroot"))
			{
				dstProvItr->second->setManpower(0.0);
			}
			if ((Configuration::getIcConversion() == "squareroot") || (Configuration::getIcConversion() == "linear") ||(Configuration::getIcConversion() == "logarithmic"))
			{
				dstProvItr->second->setRawIndustry(0.0);
				dstProvItr->second->setActualIndustry(0);
			}
		}

		// convert items that apply to only one destination province
		if (mapping.second.size() > 0)
		{
			// get the destination province
			auto dstProvItr = provinces.find(mapping.second[0]);
			if (dstProvItr == provinces.end())
			{
				continue;
			}

			// convert industry
			double industry = sourceProvince->getEmployedWorkers();
			if (Configuration::getIcConversion() == "squareroot")
			{
				industry = sqrt(double(industry)) * 0.01294;
				dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
			}
			else if (Configuration::getIcConversion() == "linear")
			{
				industry = double(industry) * 0.0000255;
				dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
			}
			else if (Configuration::getIcConversion() == "logarithmic")
			{
				industry = log(max(1, industry / 70000)) / log(2) * 5.33;
				dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
			}
					
			// convert manpower
			double newManpower = sourceProvince->getPopulation("soldiers")
				+ sourceProvince->getPopulation("craftsmen") * 0.25 // Conscripts
				+ sourceProvince->getPopulation("labourers") * 0.25 // Conscripts
				+ sourceProvince->getPopulation("farmers") * 0.25; // Conscripts
			if (Configuration::getManpowerConversion() == "linear")
			{
				newManpower *= 0.00003 * Configuration::getManpowerFactor();
				newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
				dstProvItr->second->addManpower(newManpower);
			}
			else if (Configuration::getManpowerConversion() == "squareroot")
			{
				newManpower = sqrt(newManpower);
				newManpower *= 0.0076 * Configuration::getManpowerFactor();
				newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
				dstProvItr->second->addManpower(newManpower);
			}

			// convert leadership
			double newLeadership = sourceProvince->getLiteracyWeightedPopulation("clergymen") * 0.5
				+ sourceProvince->getPopulation("officers")
				+ sourceProvince->getLiteracyWeightedPopulation("clerks") // Clerks representing researchers
				+ sourceProvince->getLiteracyWeightedPopulation("capitalists") * 0.5
				+ sourceProvince->getLiteracyWeightedPopulation("bureaucrats") * 0.25
				+ sourceProvince->getLiteracyWeightedPopulation("aristocrats") * 0.25;
			if (Configuration::getLeadershipConversion() == "linear")
			{
				newLeadership *= 0.00001363 * Configuration::getLeadershipFactor();
				newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
				dstProvItr->second->addLeadership(newLeadership);
			}
			else if (Configuration::getLeadershipConversion() == "squareroot")
			{
				newLeadership = sqrt(newLeadership);
				newLeadership *= 0.00147 * Configuration::getLeadershipFactor();
				newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
				dstProvItr->second->addLeadership(newLeadership);
			}
		}
	}
}


void HoI3World::convertTechs(const V2World& sourceWorld)
{
	map<string, vector<pair<string, int> > > techTechMap;
	map<string, vector<pair<string, int> > > invTechMap;

	// build tech maps - the code is ugly so the file can be pretty
	Object* obj = doParseFile("tech_mapping.txt");
	vector<Object*> objs = obj->getValue("tech_map");
	if (objs.size() < 1)
	{
		LOG(LogLevel::Error) << "Could not read tech map!";
		exit(1);
	}
	objs = objs[0]->getValue("link");
	for (auto itr: objs)
	{
		vector<string> keys = itr->getKeys();
		int status = 0; // 0 = unhandled, 1 = tech, 2 = invention
		vector<pair<string, int> > targetTechs;
		string tech = "";
		for (auto master: keys)
		{
			if ((status == 0) && (master == "v2_inv"))
			{
				tech = itr->getLeaf("v2_inv");
				status = 2;
			}
			else if ((status == 0) && (master == "v2_tech"))
			{
				tech = itr->getLeaf("v2_tech");
				status = 1;
			}
			else
			{
				int value = atoi(itr->getLeaf(master).c_str());
				targetTechs.push_back(pair<string, int>(master, value));
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
				invTechMap[tech] = targetTechs;
				break;
		}
	}


	for (auto dstCountry: countries)
	{
		const V2Country*	sourceCountry	= dstCountry.second->getSourceCountry();
		vector<string>		techs				= sourceCountry->getTechs();

		for (auto techName: techs)
		{
			auto mapItr = techTechMap.find(techName);
			if (mapItr != techTechMap.end())
			{
				for (auto HoI3TechItr: mapItr->second)
				{
					dstCountry.second->setTechnology(HoI3TechItr.first, HoI3TechItr.second);
				}
			}
		}

		vector<string> srcInventions = sourceCountry->getInventions();
		for (auto invItr: srcInventions)
		{
			auto mapItr = invTechMap.find(invItr);
			if (mapItr == invTechMap.end())
			{
				continue;
			}
			else
			{
				for (auto HoI3TechItr : mapItr->second)
				{
					dstCountry.second->setTechnology(HoI3TechItr.first, HoI3TechItr.second);
				}
			}
		}
	}
}


static string CardinalToOrdinal(int cardinal)
{
	int hundredRem	= cardinal % 100;
	int tenRem		= cardinal % 10;
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


vector<int> HoI3World::getPortProvinces(const vector<int>& locationCandidates)
{
	vector<int> newLocationCandidates;
	for (auto litr: locationCandidates)
	{
		map<int, HoI3Province*>::const_iterator provinceItr = provinces.find(litr);
		if ((provinceItr != provinces.end()) && (provinceItr->second->hasNavalBase()))
		{
			newLocationCandidates.push_back(litr);
		}
	}

	return newLocationCandidates;
}


unitTypeMapping HoI3World::getUnitMappings()
{
	// parse the mapping file
	map<string, multimap<HoI3RegimentType, unsigned> > unitTypeMap; // <vic, hoi>
	Object* obj = doParseFile("unit_mapping.txt");
	vector<Object*> leaves = obj->getLeaves();
	if (leaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No unit mapping definitions loaded.";
		exit(-1);
	}

	// figure out which set of mappings to use
	int modIndex = -1;
	int defaultIndex = 0;
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ((Configuration::getVic2Mods().size() > 0) && (Configuration::getVic2Mods()[0] == key))
		{
			modIndex = i;
		}
		if (key == "default")
		{
			defaultIndex = i;
		}
	}
	if (modIndex != -1)
	{
		leaves = leaves[modIndex]->getLeaves();
	}
	else
	{
		leaves = leaves[defaultIndex]->getLeaves();
	}

	// read the mappings
	for (auto leaf: leaves)
	{
		vector<Object*> vicKeys = leaf->getValue("vic");
		if (vicKeys.size() < 1)
		{
			LOG(LogLevel::Error) << "invalid unit mapping(no source).";
		}
		else
		{
			// multimap allows multiple mapping and ratio mapping (e.g. 4 irregulars converted to 3 militia brigades and 1 infantry brigade)
			multimap<HoI3RegimentType, unsigned> hoiList;
			vector<Object*> hoiKeys = leaf->getValue("hoi0");
			for (auto hoiKey: hoiKeys)
			{
				hoiList.insert(make_pair(HoI3RegimentType(hoiKey->getLeaf()), 0));
			}

			hoiKeys = leaf->getValue("hoi1");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI3RegimentType(hoiKey->getLeaf()), 1));
			}

			hoiKeys = leaf->getValue("hoi2");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI3RegimentType(hoiKey->getLeaf()), 2));
			}

			hoiKeys = leaf->getValue("hoi3");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI3RegimentType(hoiKey->getLeaf()), 3));
			}

			hoiKeys = leaf->getValue("hoi4");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI3RegimentType(hoiKey->getLeaf()), 4));
			}

			for (auto vicKey: vicKeys)
			{
				if (unitTypeMap.find(vicKey->getLeaf()) == unitTypeMap.end())
				{
					unitTypeMap[vicKey->getLeaf()] = hoiList;
				}
				else
				{
					for (auto listItr: hoiList)
					{
						unitTypeMap[vicKey->getLeaf()].insert(listItr);
					}
				}
			}
		}
	}

	return unitTypeMap;
}


vector<int> HoI3World::getPortLocationCandidates(const vector<int>& locationCandidates, const HoI3AdjacencyMapping& HoI3AdjacencyMap)
{
	vector<int> portLocationCandidates = getPortProvinces(locationCandidates);
	if (portLocationCandidates.size() == 0)
	{
		// if none of the mapped provinces are ports, try to push the navy out to sea
		for (auto candidate : locationCandidates)
		{
			if (HoI3AdjacencyMap.size() > static_cast<unsigned int>(candidate))
			{
				auto newCandidates = HoI3AdjacencyMap[candidate];
				for (auto newCandidate : newCandidates)
				{
					auto candidateProvince = provinces.find(newCandidate.to);
					if (candidateProvince == provinces.end())	// if this was not an imported province but has an adjacency, we can assume it's a sea province
					{
						portLocationCandidates.push_back(newCandidate.to);
					}
				}
			}
		}
	}
	return portLocationCandidates;
}


int HoI3World::getAirLocation(HoI3Province* locationProvince, const HoI3AdjacencyMapping& HoI3AdjacencyMap, string owner)
{
	queue<int>		openProvinces;
	map<int, int>	closedProvinces;
	openProvinces.push(locationProvince->getNum());
	closedProvinces.insert(make_pair(locationProvince->getNum(), locationProvince->getNum()));
	while (openProvinces.size() > 0)
	{
		int provNum = openProvinces.front();
		openProvinces.pop();

		auto province = provinces.find(provNum);
		if ((province != provinces.end()) && (province->second->getOwner() == owner) && (province->second->getAirBase() > 0))
		{
			return provNum;
		}
		else
		{
			auto adjacencies = HoI3AdjacencyMap[provNum];
			for (auto thisAdjacency: adjacencies)
			{
				auto closed = closedProvinces.find(thisAdjacency.to);
				if (closed == closedProvinces.end())
				{
					openProvinces.push(thisAdjacency.to);
					closedProvinces.insert(make_pair(thisAdjacency.to, thisAdjacency.to));
				}
			}
		}
	}

	return -1;
}


vector<HoI3Regiment*> HoI3World::convertRegiments(const unitTypeMapping& unitTypeMap, vector<V2Regiment*>& sourceRegiments, map<string, unsigned>& typeCount, const pair<string, HoI3Country*>& country)
{
	vector<HoI3Regiment*> destRegiments;

	for (auto regItr: sourceRegiments)
	{
		HoI3Regiment* destReg = new HoI3Regiment();
		destReg->setName(regItr->getName());

		auto typeMap = unitTypeMap.find(regItr->getType());
		if (typeMap == unitTypeMap.end())
		{
			LOG(LogLevel::Debug) << "Regiment " << regItr->getName() << " has unmapped unit type " << regItr->getType() << ", dropping.";
			continue;
		}
		else if (typeMap->second.empty()) // Silently skip the ones that purposefully have no mapping
		{
			continue;
		}

		const multimap<HoI3RegimentType, unsigned>& hoiMapList = typeMap->second;
		unsigned destMapIndex = typeCount[regItr->getType()]++ % hoiMapList.size();
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

				if (usableByCountries.empty() || usableByCountries.find(country.first) != usableByCountries.end())
				{
					break;
				}

				++skippedCount;
				++destTypeItr;
				++typeCount[regItr->getType()];
				if (destTypeItr == hoiMapList.end())
				{
					destTypeItr = hoiMapList.begin();
				}
			}

			if (skippedCount == hoiMapList.size())
			{
				LOG(LogLevel::Warning) << "Regiment " << regItr->getName() << " has unit type " << regItr->getType() << ", but it is mapped only to units exclusive to other countries. Dropping.";
				continue;
			}
		}

		destReg->setType(destTypeItr->first);
		destReg->setHistoricalModel(destTypeItr->second);
		destReg->setReserve(true);
		destRegiments.push_back(destReg);

		// Contribute to country's practicals
		country.second->getPracticals()[destTypeItr->first.getPracticalBonus()] += Configuration::getPracticalsScale() * destTypeItr->first.getPracticalBonusFactor();
	}

	return destRegiments;
}


HoI3RegGroup* HoI3World::createArmy(const inverseProvinceMapping& inverseProvinceMap, const HoI3AdjacencyMapping& HoI3AdjacencyMap, string tag, const V2Army* oldArmy, vector<HoI3Regiment*>& sourceRegiments, int& airForceIndex)
{
	HoI3RegGroup* destArmy	= new HoI3RegGroup();

	// determine if an army or navy
	if (oldArmy->getNavy())
	{
		destArmy->setForceType(navy);
		destArmy->setAtSea(oldArmy->getAtSea());
	}
	else
	{
		destArmy->setForceType(land);
	}

	// get potential locations
	vector<int> locationCandidates = getHoI3ProvinceNums(inverseProvinceMap, oldArmy->getLocation());
	if (locationCandidates.size() == 0)
	{
		LOG(LogLevel::Warning) << "Army or Navy " << oldArmy->getName() << " assigned to unmapped province " << oldArmy->getLocation() << "; placing units in the production queue.";
		destArmy->setProductionQueue(true);
	}

	// guarantee that navies are assigned to sea provinces, or land provinces with naval bases
	if ((locationCandidates.size() > 0) && (oldArmy->getNavy()) && (!oldArmy->getAtSea()))
	{
		map<int, HoI3Province*>::const_iterator pitr = provinces.find(locationCandidates[0]);
		if (pitr != provinces.end() && pitr->second->isLand())
		{
			locationCandidates = getPortLocationCandidates(locationCandidates, HoI3AdjacencyMap);
			if (locationCandidates.size() == 0)
			{
				LOG(LogLevel::Warning) << "Navy " << oldArmy->getName() << " assigned to V2 province " << oldArmy->getLocation() << " which has no corresponding HoI3 port provinces or adjacent sea provinces; placing units in the production queue.";
				destArmy->setProductionQueue(true);
			}
		}
	}

	// pick the actual location
	int selectedLocation;
	HoI3Province* locationProvince = NULL;
	if (locationCandidates.size() > 0)
	{
		selectedLocation = locationCandidates[rand() % locationCandidates.size()];
		destArmy->setLocation(selectedLocation);
		map<int, HoI3Province*>::iterator pitr = provinces.find(selectedLocation);
		if (pitr != provinces.end())
		{
			locationProvince = pitr->second;
		}
		else if (!oldArmy->getNavy())
		{
			destArmy->setProductionQueue(true);
		}
	}

	// handle navies
	if (oldArmy->getNavy())
	{
		for (auto regiment: sourceRegiments)
		{
			destArmy->addRegiment(*regiment, true);
		}
		return destArmy;
	}

	// air units need to be split into air forces, so create an air force regiment to hold any air units
	HoI3RegGroup destWing;
	destWing.setForceType(air);
	HoI3Province* airLocationProvince = NULL;
	if (!destArmy->getProductionQueue())
	{
		int airLocation = -1;
		if (locationProvince != NULL)
		{
			airLocation = getAirLocation(locationProvince, HoI3AdjacencyMap, tag);
		}
		destWing.setLocation(airLocation);
		map<int, HoI3Province*>::iterator pitr = provinces.find(airLocation);
		if (pitr != provinces.end())
		{
			airLocationProvince = pitr->second;
		}
		else
		{
			destWing.setProductionQueue(true);
		}
	}
	else
	{
		destWing.setProductionQueue(true);
	}

	// separate all air regiments into the air force
	vector<HoI3Regiment*> unprocessedRegiments;
	for (auto regiment: sourceRegiments)
	{
		// Add to army/navy or newly created air force as appropriate
		if (regiment->getForceType() == air)
		{
			destWing.addRegiment(*regiment, true);
		}
		else
		{
			unprocessedRegiments.push_back(regiment);
		}
	}
	if (!destWing.isEmpty())
	{
		stringstream name;
		name << ++airForceIndex << CardinalToOrdinal(airForceIndex) << " Air Force";
		destWing.setName(name.str());

		if (!destWing.getProductionQueue())
		{
			// make sure an airbase is waiting for them
			airLocationProvince->requireAirBase(min(10, airLocationProvince->getAirBase() + (destWing.size() * 2)));
		}
		destArmy->addChild(destWing, true);
	}
	sourceRegiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// put all 'armored' type regiments together
	HoI3RegGroup armored;
	armored.setForceType(land);
	armored.setLocation(selectedLocation);
	for (auto regiment: sourceRegiments)
	{
		// Add to army/navy or newly created air force as appropriate
		if ((regiment->getType().getName() == "light_armor_brigade") ||
			 (regiment->getType().getName() == "armor_brigade") ||
			 (regiment->getType().getName() == "armored_car_brigade") ||
			 (regiment->getType().getName() == "tank_destroyer_brigade") ||
			 (regiment->getType().getName() == "motorized_brigade")
			)
		{
			armored.addRegiment(*regiment, true);
		}
		else
		{
			unprocessedRegiments.push_back(regiment);
		}
	}
	if (!armored.isEmpty())
	{
		destArmy->addChild(armored, true);
	}
	sourceRegiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// put all 'specialist' type regiments together
	HoI3RegGroup specialist;
	specialist.setForceType(land);
	specialist.setLocation(selectedLocation);
	for (auto regiment: sourceRegiments)
	{
		// Add to army/navy or newly created air force as appropriate
		if ((regiment->getType().getName() == "bergsjaeger_brigade") ||
			 (regiment->getType().getName() == "marine_brigade") ||
			 (regiment->getType().getName() == "police_brigade")
			)
		{
			armored.addRegiment(*regiment, true);
		}
		else
		{
			unprocessedRegiments.push_back(regiment);
		}
	}
	if (!specialist.isEmpty())
	{
		destArmy->addChild(specialist, true);
	}
	sourceRegiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// make infantry and militia divisions, with support regiments added
	while (true)
	{
		HoI3RegGroup newGroup;
		newGroup.setForceType(land);
		newGroup.setLocation(selectedLocation);

		// get the first regiment (required)
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if ((regiment->getType().getName() == "infantry_brigade") ||
				 (regiment->getType().getName() == "militia_brigade")
				)
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				break;
			}
		}
		if (newGroup.isEmpty())
		{
			break;
		}

		// get the second regiment (not required)
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if ((regiment->getType().getName() == "infantry_brigade") ||
				(regiment->getType().getName() == "militia_brigade")
				)
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				break;
			}
		}

		// get the support first regiment (not required, non-engineer preferred)
		int numSupport = 0;
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if ((regiment->getType().getName() == "anti_air_brigade") ||
				 (regiment->getType().getName() == "anti_tank_brigade") ||
				 (regiment->getType().getName() == "artillery_brigade")
				)
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				numSupport++;
				break;
			}
		}

		// get the second support regiment (not required, non-engineer preferred)
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if ((regiment->getType().getName() == "anti_air_brigade") ||
				(regiment->getType().getName() == "anti_tank_brigade") ||
				(regiment->getType().getName() == "artillery_brigade")
				)
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				numSupport++;
				break;
			}
		}

		// add engineers as needed and available
		if (numSupport < 2)
		{
			for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
			{
				HoI3Regiment* regiment = *itr;
				if (regiment->getType().getName() == "engineer_brigade")
				{
					newGroup.addRegiment(*regiment, true);
					sourceRegiments.erase(itr);
					numSupport++;
					break;
				}
			}
		}
		if (numSupport < 2)
		{
			for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
			{
				HoI3Regiment* regiment = *itr;
				if (regiment->getType().getName() == "engineer_brigade")
				{
					newGroup.addRegiment(*regiment, true);
					sourceRegiments.erase(itr);
					break;
				}
			}
		}

		destArmy->addChild(newGroup, true);
	}

	// put together cavalry units (allow engineers to be included)
	while (true)
	{
		HoI3RegGroup newGroup;
		newGroup.setForceType(land);
		newGroup.setLocation(selectedLocation);

		// get the first regiment (required)
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if ((regiment->getType().getName() == "cavalry_brigade"))
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				break;
			}
		}
		if (newGroup.isEmpty())
		{
			break;
		}

		// get the second regiment (required to continue)
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if ((regiment->getType().getName() == "cavalry_brigade"))
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				break;
			}
		}
		if (newGroup.size() < 2)
		{
			destArmy->addChild(newGroup, true);
			break;
		}

		// get the first engineer regiment (not required)
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if (regiment->getType().getName() == "engineer_brigade")
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				break;
			}
		}

		// get the second engineer regiment (not required)
		for (vector<HoI3Regiment*>::iterator itr = sourceRegiments.begin(); itr != sourceRegiments.end(); itr++)
		{
			HoI3Regiment* regiment = *itr;
			if (regiment->getType().getName() == "engineer_brigade")
			{
				newGroup.addRegiment(*regiment, true);
				sourceRegiments.erase(itr);
				break;
			}
		}

		destArmy->addChild(newGroup, true);
	}

	if (sourceRegiments.size() > 0)
	{
		LOG(LogLevel::Warning) << "Leftover regiments in " << tag << "'s army " << oldArmy->getName() << ". Likely too many support units.";
		for (auto regiment: sourceRegiments)
		{
			HoI3RegGroup newGroup;
			newGroup.setForceType(land);
			newGroup.setLocation(selectedLocation);
			newGroup.addRegiment(*regiment, true);
			destArmy->addChild(newGroup, true);
		}
	}
	

	return destArmy;
}


void HoI3World::convertArmies(const V2World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const HoI3AdjacencyMapping& HoI3AdjacencyMap)
{
	unitTypeMapping unitTypeMap = getUnitMappings();

	// define the headquarters brigade type
	HoI3RegimentType hqBrigade("hq_brigade");

	// convert each country's armies
	for (auto country: countries)
	{
		const V2Country* oldCountry = country.second->getSourceCountry();
		if (oldCountry == NULL)
		{
			continue;
		}

		int airForceIndex = 0;
		HoI3RegGroup::resetHQCounts();
		HoI3RegGroup::resetRegGroupNameCounts();

		// A V2 unit type counter to keep track of how many V2 units of this type were converted.
		// Used to distribute HoI3 unit types in case of multiple mapping
		map<string, unsigned> typeCount;

		// Convert actual armies
		for (auto oldArmy: oldCountry->getArmies())
		{
			// convert the regiments
			vector<HoI3Regiment*> regiments = convertRegiments(unitTypeMap, oldArmy->getRegiments(), typeCount, country);

			// place the regiments into armies
			HoI3RegGroup* army = createArmy(inverseProvinceMap, HoI3AdjacencyMap, country.first, oldArmy, regiments, airForceIndex);
			army->setName(oldArmy->getName());

			// add the converted units to the country
			if ((army->getForceType() == land) && (!army->isEmpty()) && (!army->getProductionQueue()))
			{
				army->createHQs(hqBrigade); // Generate HQs for all hierarchies
				country.second->addArmy(army);
			}
			else if (!army->isEmpty())
			{
				country.second->addArmy(army);
			}
		}

		// Anticipate practical points being awarded for completing the unit constructions
		for (auto armyItr: country.second->getArmies())
		{
			if (armyItr->getProductionQueue())
			{
				armyItr->undoPracticalAddition(country.second->getPracticals());
			}
		}
	}
}


void HoI3World::checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string leader, const string& factionName)
{
	bool leaderSet = false;
	for (auto candidate: candidateTags)
	{
		// get HoI3 tag from V2 tag
		string hoiTag = countryMap[candidate];
		if (hoiTag.empty())
		{
			LOG(LogLevel::Warning) << "Tag " << candidate << " requested for " << factionName << " faction, but is unmapped!";
			continue;
		}

		// find HoI3 nation and ensure that it has land
		auto citr = countries.find(hoiTag);
		if (citr != countries.end())
		{
			if (citr->second->getProvinces().size() == 0)
			{
				LOG(LogLevel::Warning) << "Tag " << candidate << " requested for " << factionName << " faction, but is landless!";
			}
			else
			{
				LOG(LogLevel::Debug) << candidate << " added to " << factionName  << " faction";
				citr->second->setFaction(factionName);
				if (leader == "")
				{
					leader = citr->first;
				}
				if (!leaderSet)
				{
					citr->second->setFactionLeader();
					leaderSet = true;
				}
			}
		}
		else
		{
			LOG(LogLevel::Warning) << "Tag " << candidate << " requested for " << factionName << " faction, but does not exist!";
		}
	}
}


void HoI3World::factionSatellites()
{
	// make sure that any vassals are in their master's faction
	const vector<HoI3Agreement>& agreements = diplomacy.getAgreements();
	for (auto agreement: agreements)
	{
		if (agreement.type == "vassal")
		{
			auto masterCountry		= countries.find(agreement.country1);
			auto satelliteCountry	= countries.find(agreement.country2);
			if ((masterCountry != countries.end()) && (masterCountry->second->getFaction() != "") && (satelliteCountry != countries.end()))
			{
				satelliteCountry->second->setFaction(masterCountry->second->getFaction());
			}
		}
	}
}


void HoI3World::setFactionMembers(const V2World &sourceWorld, const CountryMapping& countryMap)
{
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
		for (auto countryItr : greatCountries)
		{
			map<string, HoI3Country*>::iterator itr = countries.find(countryMap[countryItr]);
			if (itr != countries.end())
			{
				HoI3Country* country = itr->second;
				const string government = country->getGovernment();
				const string ideology = country->getIdeology();
				if (
					(government == "national_socialism" || government == "fascist_republic" || government == "germanic_fascist_republic" ||
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
						country->setFactionLeader();
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
				else if (
					(government == "social_conservatism" || government == "constitutional_monarchy" || government == "spanish_social_conservatism" ||
						government == "market_liberalism" || government == "social_democracy" || government == "social_liberalism"
						) &&
						(ideology == "social_conservative" || ideology == "market_liberal" || ideology == "social_liberal" || ideology == "social_democrat")
					)
				{
					if (alliesLeader == "")
					{
						alliesLeader = itr->first;
						country->setFaction("allies");
						country->setFactionLeader();
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
				else if (
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
						country->setFactionLeader();
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
				LOG(LogLevel::Error) << "V2 great power " << countryItr << " not found.";
			}
		}

		// Comintern get a boost to its membership for being internationalistic
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
						itr->second->setFactionLeader();
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
		exit(-1);
	}
}


void HoI3World::setAlignments()
{
	// set alignments
	for (auto country: countries)
	{
		const string countryFaction = country.second->getFaction();

		// force alignment for faction members
		if (countryFaction == "axis")
		{
			country.second->getAlignment()->alignToAxis();
		}
		else if (countryFaction == "allies")
		{
			country.second->getAlignment()->alignToAllied();
		}
		else if (countryFaction == "comintern")
		{
			country.second->getAlignment()->alignToComintern();
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
				HoI3Relations* relObj = country.second->getRelations(axisLeader);
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
				HoI3Relations* relObj = country.second->getRelations(alliesLeader);
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
				HoI3Relations* relObj = country.second->getRelations(cominternLeader);
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
			(*(country.second->getAlignment())) = HoI3Alignment::getCentroid(axisStart, alliesStart, cominternStart);
		}
	}
}


void HoI3World::configureFactions(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	setFactionMembers(sourceWorld, countryMap);

	//set faction leaders to be earlier in the country ordering
	for (auto country = countryOrder.begin(); country != countryOrder.end(); country++)
	{
		if (*country == axisLeader)
		{
			countryOrder.erase(country);
			break;
		}
	}
	if (axisLeader != "")
	{
		countryOrder.insert(countryOrder.begin(), axisLeader);
	}

	for (auto country = countryOrder.begin(); country != countryOrder.end(); country++)
	{
		if (*country == alliesLeader)
		{
			countryOrder.erase(country);
			break;
		}
	}
	if (alliesLeader != "")
	{
		countryOrder.insert(countryOrder.begin(), alliesLeader);
	}

	for (auto country = countryOrder.begin(); country != countryOrder.end(); country++)
	{
		if (*country == cominternLeader)
		{
			countryOrder.erase(country);
			break;
		}
	}
	if (cominternLeader != "")
	{
		countryOrder.insert(countryOrder.begin(), cominternLeader);
	}
	
	for (auto country = countryOrder.begin(); country != countryOrder.end(); country++)
	{
		if (*country == "REB")
		{
			countryOrder.erase(country);
			break;
		}
	}
	countryOrder.insert(countryOrder.begin(), "REB");

	factionSatellites(); // push satellites into the same faction as their parents
	setAlignments();
}


void HoI3World::generateLeaders(const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap)
{
	for (auto country: countries)
	{
		country.second->generateLeaders(leaderTraits, namesMap, portraitMap);
	}
}


void HoI3World::consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap)
{
	double totalManpower		= 0.0;
	double totalLeadership	= 0.0;
	double totalIndustry		= 0.0;
	for (auto countryItr: countries)
	{
		countryItr.second->consolidateProvinceItems(inverseProvinceMap, totalManpower, totalLeadership, totalIndustry);
	}

	double suggestedManpowerConst		= 3302.8 * Configuration::getManpowerFactor()	/ totalManpower;
	LOG(LogLevel::Debug) << "Total manpower was " << totalManpower << ". Changing the manpower factor to " << suggestedManpowerConst << " would match vanilla HoI3.";

	double suggestedLeadershipConst	=  212.0 * Configuration::getLeadershipFactor() / totalLeadership;
	LOG(LogLevel::Debug) << "Total leadership was " << totalLeadership << ". Changing the leadership factor to " << suggestedLeadershipConst << " would match vanilla HoI3.";

	double suggestedIndustryConst		= 1746.0 * Configuration::getIcFactor() / totalIndustry;
	LOG(LogLevel::Debug) << "Total IC was " << totalIndustry << ". Changing the IC factor to " << suggestedIndustryConst << " would match vanilla HoI3.";
}


void HoI3World::convertVictoryPoints(const V2World& sourceWorld, const CountryMapping& countryMap)
{
	// all country capitals get five VP
	for (auto countryItr: countries)
	{
		auto capitalItr = countryItr.second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(5);
		}
	}

	// Great Power capitals get another five
	const std::vector<string>& greatCountries = sourceWorld.getGreatCountries();
	for (auto country: sourceWorld.getGreatCountries())
	{
		const std::string& HoI3Tag = countryMap[country];
		auto countryItr = countries.find(HoI3Tag);
		if (countryItr != countries.end())
		{
			auto capitalItr = countryItr->second->getCapital();
			if (capitalItr != NULL)
			{
				capitalItr->addPoints(5);
			}
		}
	}

	// alliance leaders get another ten
	auto countryItr = countries.find(axisLeader);
	if (countryItr != countries.end())
	{
		auto capitalItr = countryItr->second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(10);
		}
	}
	countryItr = countries.find(alliesLeader);
	if (countryItr != countries.end())
	{
		auto capitalItr = countryItr->second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(10);
		}
	}
	countryItr = countries.find(cominternLeader);
	if (countryItr != countries.end())
	{
		auto capitalItr = countryItr->second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(10);
		}
	}
}


void HoI3World::convertDiplomacy(const V2World& sourceWorld, const CountryMapping& countryMap)
{
	for (auto agreement: sourceWorld.getDiplomacy()->getAgreements())
	{
		string HoI3Tag1 = countryMap[agreement.country1];
		if (HoI3Tag1.empty())
		{
			continue;
		}
		string HoI3Tag2 = countryMap[agreement.country2];
		if (HoI3Tag2.empty())
		{
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

		// shared diplo types
		if ((agreement.type == "alliance") || (agreement.type == "vassal"))
		{
			// copy agreement
			HoI3Agreement hoi3a;
			hoi3a.country1		= HoI3Tag1;
			hoi3a.country2		= HoI3Tag2;
			hoi3a.start_date	= agreement.start_date;
			hoi3a.type			= agreement.type;
			diplomacy.addAgreement(hoi3a);

			if (agreement.type == "alliance")
			{
				hoi3Country1->second->editAllies().insert(HoI3Tag2);
			}
		}
	}

	// Relations and guarantees
	for (auto country: countries)
	{
		for (auto relationItr: country.second->getRelations())
		{
			HoI3Agreement hoi3a;
			if (country.first < relationItr.first) // Put it in order to eliminate duplicate relations entries
			{
				hoi3a.country1 = country.first;
				hoi3a.country2 = relationItr.first;
			}
			else
			{
				hoi3a.country2 = relationItr.first;
				hoi3a.country1 = country.first;
			}

			hoi3a.value = relationItr.second->getRelations();
			hoi3a.start_date = date("1930.1.1"); // Arbitrary date
			hoi3a.type = "relation";
			diplomacy.addAgreement(hoi3a);

			if (relationItr.second->getGuarantee())
			{
				HoI3Agreement hoi3a;
				hoi3a.country1 = country.first;
				hoi3a.country2 = relationItr.first;
				hoi3a.start_date = date("1930.1.1"); // Arbitrary date
				hoi3a.type = "guarantee";
				diplomacy.addAgreement(hoi3a);
			}
		}
	}

	// decrease neutrality for countries with unowned cores
	map<string, string> hasLoweredNeutrality;
	for (auto province: provinces)
	{
		for (auto core: province.second->getCores())
		{
			if (province.second->getOwner() != core)
			{
				auto repeat = hasLoweredNeutrality.find(core);
				if (repeat == hasLoweredNeutrality.end())
				{
					auto country = countries.find(core);
					if (country != countries.end())
					{
						country->second->lowerNeutrality(20.0);
						hasLoweredNeutrality.insert(make_pair(core, core));
					}
				}
			}
		}
	}
}


void HoI3World::copyFlags(const V2World &sourceWorld, const CountryMapping& countryMap)
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
	for (auto country: sourceWorld.getCountries())
	{
		std::string V2Tag = country.first;
		V2Country* sourceCountry = country.second;
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


void HoI3World::checkAllProvincesMapped(const provinceMapping& provinceMap)
{
	for (auto province: provinces)
	{
		provinceMapping::const_iterator num = provinceMap.find(province.first);
		if (num == provinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for HoI3 province " << province.first;
		}
	}
}


void HoI3World::setAIFocuses(const AIFocusModifiers& focusModifiers)
{
	for (auto countryItr: countries)
	{
		countryItr.second->setAIFocuses(focusModifiers);
	}
}


void HoI3World::addMinimalItems(const inverseProvinceMapping& inverseProvinceMap)
{
	for (auto country : countries)
	{
		country.second->addMinimalItems(inverseProvinceMap);
	}
}