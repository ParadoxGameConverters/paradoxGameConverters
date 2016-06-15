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


#include "HoI4World.h"
#include <Windows.h>
#include <fstream>
#include <algorithm>
#include <io.h>
#include <list>
#include <queue>
#include <cmath>
#include <cfloat>
#include <sys/stat.h>
#include "ParadoxParserUTF8.h"
#include "Log.h"
#include "../Configuration.h"
#include "../../../common_items/WinUtils.h"
#include "../V2World/V2Province.h"
#include "../V2World/V2Party.h"
#include "HoI4Relations.h"
#include "HoI4State.h"



typedef struct fileWithCreateTime
{
	string	filename;
	time_t	createTime;
	bool operator < (const fileWithCreateTime &rhs) const
	{
		return createTime < rhs.createTime;
	};
} fileWithCreateTime;


void HoI4World::importProvinces()
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
				HoI4Province* newProvince = new HoI4Province(directories.front() + "\\" + provinceFileData.name);

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
}


void HoI4World::checkCoastalProvinces()
{
	// determine whether each province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise HoI4 crashes)
	//Object*	obj2 = parser_UTF8::doParseFile((Configuration::getHoI4Path() + "\\tfh\\map\\positions.txt").c_str());
	//vector<Object*> objProv = obj2->getLeaves();
	//if (objProv.size() == 0)
	//{
	//	LOG(LogLevel::Error) << "map\\positions.txt failed to parse.";
	//	exit(1);
	//}
	//for (auto itr: objProv)
	//{
	//	int provinceNum = _wtoi(itr->getKey().c_str());
	//	vector<Object*> objPos = itr->getValue("building_position");
	//	if (objPos.size() == 0)
	//	{
	//		continue;
	//	}
	//	vector<Object*> objNavalBase = objPos[0]->getValue("naval_base");
	//	if (objNavalBase.size() != 0)
	//	{
	//		// this province is coastal
	//		map<int, HoI4Province*>::iterator pitr = provinces.find(provinceNum);
	//		if (pitr != provinces.end())
	//		{
	//			pitr->second->setCoastal(true);
	//		}
	//	}
	//}
}


void HoI4World::importPotentialCountries()
{
	potentialCountries.clear();
	countries.clear();

	LOG(LogLevel::Info) << "Getting potential countries";
	set<string> countryFilenames;
	WinUtils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/common/country_tags", countryFilenames);

	for (auto countryFilename: countryFilenames)
	{
		ifstream HoI4CountriesInput(Configuration::getHoI4Path() + "/common/country_tags/" + countryFilename);
		if (!HoI4CountriesInput.is_open())
		{
			LOG(LogLevel::Error) << "Could not open " << countryFilename;
			exit(1);
		}

		while (!HoI4CountriesInput.eof())
		{
			string line;
			getline(HoI4CountriesInput, line);

			if ((line[0] == '#') || (line.size() < 3))
			{
				continue;
			}
			if (line.substr(0, 19) == "dynamic_tags  = yes")
			{
				break;
			}

			string tag;
			tag = line.substr(0, 3);

			string countryFileName;
			int start = line.find_first_of('/');
			int size = line.find_last_of('\"') - start;
			countryFileName = line.substr(start, size);

			HoI4Country* newCountry = new HoI4Country(tag, countryFileName, this);
			potentialCountries.insert(make_pair(tag, newCountry));
		}
		HoI4CountriesInput.close();
	}
}


void HoI4World::output() const
{
	outputCommonCountries();
	//outputAutoexecLua();
	//outputLocalisations();
	outputHistory();
}


void HoI4World::outputCommonCountries() const
{
	// Create common\countries path.
	string countriesPath = "Output\\" + Configuration::getOutputName() + "\\common";
	if (!WinUtils::TryCreateFolder(countriesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output\\" + Configuration::getOutputName() + "\\common\"";
		exit(-1);
	}
	if (!WinUtils::TryCreateFolder(countriesPath + "\\countries"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output\\" + Configuration::getOutputName() + "\\common\\countries\"";
		exit(-1);
	}
	if (!WinUtils::TryCreateFolder(countriesPath + "\\country_tags"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output\\" + Configuration::getOutputName() + "\\common\\country_tags\"";
		exit(-1);
	}

	// Output common\countries.txt
	LOG(LogLevel::Debug) << "Writing countries file";
	FILE* allCountriesFile;
	if (fopen_s(&allCountriesFile, ("Output\\" + Configuration::getOutputName() + "\\common\\country_tags\\01_countries.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create countries file";
		exit(-1);
	}

	for (auto countryItr: countries)
	{
		if (potentialCountries.find(countryItr.first) == potentialCountries.end())
		{
			countryItr.second->outputToCommonCountriesFile(allCountriesFile);
		}
	}
	fprintf(allCountriesFile, "\n");
	fclose(allCountriesFile);
}


void HoI4World::outputAutoexecLua() const
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


void HoI4World::outputLocalisations() const
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
	for (auto country: countries)
	{
		if (country.second->isNewCountry())
		{
			country.second->outputLocalisation(localisationFile);
		}
	}
	fclose(localisationFile);
}


void HoI4World::outputHistory() const
{
	LOG(LogLevel::Debug) << "Writing states";
	string statesPath = "Output/" + Configuration::getOutputName() + "/history/states";
	if (!WinUtils::TryCreateFolder(statesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history/states";
		exit(-1);
	}
	for (auto state: states)
	{
		state.second->output();
	}
	LOG(LogLevel::Debug) << "Writing countries";
	string unitsPath = "Output/" + Configuration::getOutputName() + "/history/units";
	if (!WinUtils::TryCreateFolder(unitsPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history/units";
		exit(-1);
	}
	for (auto countryItr: countries)
	{
		countryItr.second->output(states.size());
	}
	// Override vanilla history to suppress vanilla OOB and faction membership being read
	for (auto potentialItr: potentialCountries)
	{
		if (countries.find(potentialItr.first) == countries.end())
		{
			potentialItr.second->output(states.size());
		}
	}
	//LOG(LogLevel::Debug) << "Writing diplomacy";
	//diplomacy.output();
}


void HoI4World::getProvinceLocalizations(const string& file)
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


void HoI4World::convertCountries(const V2World &sourceWorld, const CountryMapping& countryMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const HoI4StateMapping& stateMap)
{
	for (auto sourceItr: sourceWorld.getCountries())
	{
		// don't convert rebels
		if (sourceItr.first == "REB")
		{
			continue;
		}

		HoI4Country* destCountry = NULL;
		const std::string& HoI4Tag = countryMap[sourceItr.first];
		if (!HoI4Tag.empty())
		{
			auto candidateDestCountry = potentialCountries.find(HoI4Tag);
			if (candidateDestCountry != potentialCountries.end())
			{
				destCountry = candidateDestCountry->second;
			}
			if (destCountry == NULL) // No such HoI4 country exists yet for this tag so make a new one
			{
				std::string countryFileName = '/' + sourceItr.second->getName() + ".txt";
				destCountry = new HoI4Country(HoI4Tag, countryFileName, this, true);
			}
			V2Party* rulingParty = sourceWorld.getRulingParty(sourceItr.second);
			if (rulingParty == NULL)
			{
				LOG(LogLevel::Error) << "Could not find the ruling party for " <<  sourceItr.first << ". Were all mods correctly included?";
				exit(-1);
			}
			destCountry->initFromV2Country(sourceWorld, sourceItr.second, rulingParty->ideology, countryMap, inverseProvinceMap, leaderMap, V2Localisations, governmentJobs, namesMap, portraitMap, cultureMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, stateMap, states);
			countries.insert(make_pair(HoI4Tag, destCountry));
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not convert V2 tag " << sourceItr.first << " to HoI4";
		}
	}

	// initialize all potential countries
	// ALL potential countries should be output to the file, otherwise some things don't get initialized right in HoI4
	for (auto potentialItr: potentialCountries)
	{
		map<string, HoI4Country*>::iterator citr = countries.find(potentialItr.first);
		if (citr == countries.end())
		{
			potentialItr.second->initFromHistory();
		}
	}
}


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<V2Province*> provinces;
	int totalPopulation;
};

void HoI4World::convertProvinceOwners(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, HoI4StateMapping& stateMap)
{
	// TODO - determine province owners for all HoI4 provinces
	

	//	loop through the vic2 countries
	int stateID = 1;
	for (auto country: sourceWorld.getCountries())
	{
		//	determine the relevant HoI4 country
		string HoI4Tag = countryMap.GetHoI4Tag(country.first);

		//	loop through the states in the vic2 country
		for (auto vic2State: country.second->getStates())
		{
			//	create a matching HoI4 state
			int provincecount = 0;
			HoI4State* newState = new HoI4State(stateID, HoI4Tag);

			//	loop through the provinces in the vic2 state
			for (auto vic2Province: vic2State.getProvinces())
			{
				//	TODO - if the matching HoI4 provinces are owned by this country, add it to the HoI4 state
				auto provMapping = inverseProvinceMap.find(vic2Province);
				if (provMapping != inverseProvinceMap.end())
				{
					for (auto HoI4ProvNum: provMapping->second)
					{
						if (HoI4ProvNum != 0)
						{
							newState->addProvince(HoI4ProvNum);
							stateMap.insert(make_pair(HoI4ProvNum, stateID));
							provincecount++;
						}
					}
				}
			}

			//	if the state is not empty, add it to this list of states
			if (provincecount != 0)
			{
				states.insert(make_pair(stateID, newState));
				stateID++;
			}
		}
	}

	//for (auto provItr : provinces)
	//{
	//	// get the appropriate mapping
	//	provinceMapping::const_iterator provinceLink = provinceMap.find(provItr.first);
	//	if ((provinceLink == provinceMap.end()) || (provinceLink->second.size() == 0))
	//	{
	//		LOG(LogLevel::Warning) << "No source for " << provItr.second->getName() << " (province " << provItr.first << ')';
	//		continue;
	//	}
	//	else if (provinceLink->second[0] == 0)
	//	{
	//		continue;
	//	}

	//	provItr.second->clearCores();

	//	V2Province*	oldProvince = NULL;
	//	V2Country*	oldOwner = NULL;

	//	// determine ownership by province count, or total population (if province count is tied)
	//	map<string, MTo1ProvinceComp> provinceBins;
	//	double newProvinceTotalPop = 0;
	//	for (auto srcProvItr : provinceLink->second)
	//	{
	//		V2Province* srcProvince = sourceWorld.getProvince(srcProvItr);
	//		if (!srcProvince)
	//		{
	//			LOG(LogLevel::Warning) << "Old province " << provinceLink->second[0] << " does not exist (bad mapping?)";
	//			continue;
	//		}
	//		V2Country* owner = srcProvince->getOwner();
	//		string tag;
	//		if (owner != NULL)
	//		{
	//			tag = owner->getTag();
	//		}
	//		else
	//		{
	//			tag = "";
	//		}

	//		if (provinceBins.find(tag) == provinceBins.end())
	//		{
	//			provinceBins[tag] = MTo1ProvinceComp();
	//		}
	//		provinceBins[tag].provinces.push_back(srcProvince);
	//		provinceBins[tag].totalPopulation += srcProvince->getTotalPopulation();
	//		newProvinceTotalPop += srcProvince->getTotalPopulation();
	//		// I am the new owner if there is no current owner, or I have more provinces than the current owner,
	//		// or I have the same number of provinces, but more population, than the current owner
	//		if ((oldOwner == NULL)
	//			|| (provinceBins[tag].provinces.size() > provinceBins[oldOwner->getTag()].provinces.size())
	//			|| ((provinceBins[tag].provinces.size() == provinceBins[oldOwner->getTag()].provinces.size())
	//				&& (provinceBins[tag].totalPopulation > provinceBins[oldOwner->getTag()].totalPopulation)))
	//		{
	//			oldOwner = owner;
	//			oldProvince = srcProvince;
	//		}
	//	}
	//	if (oldOwner == NULL)
	//	{
	//		provItr.second->setOwner("");
	//		continue;
	//	}

	//	// convert from the source provinces
	//	const string HoI4Tag = countryMap[oldOwner->getTag()];
	//	if (HoI4Tag.empty())
	//	{
	//		LOG(LogLevel::Warning) << "Could not map provinces owned by " << oldOwner->getTag();
	//	}
	//	else
	//	{
	//		provItr.second->setOwner(HoI4Tag);
	//		map<string, HoI4Country*>::iterator ownerItr = countries.find(HoI4Tag);
	//		if (ownerItr != countries.end())
	//		{
	//			ownerItr->second->addProvince(provItr.second);
	//		}
	//		provItr.second->convertFromOldProvince(oldProvince);

	//		for (auto srcOwnerItr : provinceBins)
	//		{
	//			for (auto srcProvItr : srcOwnerItr.second.provinces)
	//			{
	//				// convert cores
	//				vector<V2Country*> oldCores = srcProvItr->getCores(sourceWorld.getCountries());
	//				for (auto oldCoreItr : oldCores)
	//				{
	//					// skip this core if the country is the owner of the V2 province but not the HoI4 province
	//					// (i.e. "avoid boundary conflicts that didn't exist in V2").
	//					// this country may still get core via a province that DID belong to the current HoI4 owner
	//					if ((oldCoreItr->getTag() == srcOwnerItr.first) && (oldCoreItr != oldOwner))
	//					{
	//						continue;
	//					}

	//					const string coreOwner = countryMap[oldCoreItr->getTag()];
	//					if (coreOwner != "")
	//					{
	//						provItr.second->addCore(coreOwner);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}


void HoI4World::convertNavalBases(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap)
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


void HoI4World::convertProvinceItems(const V2World& sourceWorld, const provinceMapping& provinceMap, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
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
			if (HoI4AdjacencyMap.size() > static_cast<unsigned int>(dstProvinceNum))
			{
				const vector<adjacency> adjacencies = HoI4AdjacencyMap[dstProvinceNum];
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


void HoI4World::convertTechs(const V2World& sourceWorld)
{
	map<string, vector<pair<string, int> > > techTechMap;
	map<string, vector<pair<string, int> > > invTechMap;

	// build tech maps - the code is ugly so the file can be pretty
	Object* obj = parser_UTF8::doParseFile("tech_mapping.txt");
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
		vector<string>	techs				= sourceCountry->getTechs();

		for (auto techName: techs)
		{
			auto mapItr = techTechMap.find(techName);
			if (mapItr != techTechMap.end())
			{
				for (auto HoI4TechItr: mapItr->second)
				{
					dstCountry.second->setTechnology(HoI4TechItr.first, HoI4TechItr.second);
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
				for (auto HoI4TechItr : mapItr->second)
				{
					dstCountry.second->setTechnology(HoI4TechItr.first, HoI4TechItr.second);
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


vector<int> HoI4World::getPortProvinces(const vector<int>& locationCandidates)
{
	vector<int> newLocationCandidates;
	for (auto litr: locationCandidates)
	{
		map<int, HoI4Province*>::const_iterator provinceItr = provinces.find(litr);
		if ((provinceItr != provinces.end()) && (provinceItr->second->hasNavalBase()))
		{
			newLocationCandidates.push_back(litr);
		}
	}

	return newLocationCandidates;
}


unitTypeMapping HoI4World::getUnitMappings()
{
	// parse the mapping file
	map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMap; // <vic, hoi>
	Object* obj = parser_UTF8::doParseFile("unit_mapping.txt");
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
			multimap<HoI4RegimentType, unsigned> hoiList;
			vector<Object*> hoiKeys = leaf->getValue("hoi0");
			for (auto hoiKey: hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 0));
			}

			hoiKeys = leaf->getValue("hoi1");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 1));
			}

			hoiKeys = leaf->getValue("hoi2");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 2));
			}

			hoiKeys = leaf->getValue("HoI4");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 3));
			}

			hoiKeys = leaf->getValue("hoi4");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 4));
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


vector<int> HoI4World::getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
{
	vector<int> portLocationCandidates = getPortProvinces(locationCandidates);
	if (portLocationCandidates.size() == 0)
	{
		// if none of the mapped provinces are ports, try to push the navy out to sea
		for (auto candidate : locationCandidates)
		{
			if (HoI4AdjacencyMap.size() > static_cast<unsigned int>(candidate))
			{
				auto newCandidates = HoI4AdjacencyMap[candidate];
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


int HoI4World::getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner)
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
			auto adjacencies = HoI4AdjacencyMap[provNum];
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


vector<HoI4Regiment*> HoI4World::convertRegiments(const unitTypeMapping& unitTypeMap, vector<V2Regiment*>& sourceRegiments, map<string, unsigned>& typeCount, const pair<string, HoI4Country*>& country)
{
	vector<HoI4Regiment*> destRegiments;

	for (auto regItr: sourceRegiments)
	{
		HoI4Regiment* destReg = new HoI4Regiment();
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

		const multimap<HoI4RegimentType, unsigned>& hoiMapList = typeMap->second;
		unsigned destMapIndex = typeCount[regItr->getType()]++ % hoiMapList.size();
		multimap<HoI4RegimentType, unsigned>::const_iterator destTypeItr = hoiMapList.begin();

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


HoI4RegGroup* HoI4World::createArmy(const inverseProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string tag, const V2Army* oldArmy, vector<HoI4Regiment*>& regiments, int& airForceIndex)
{
	HoI4RegGroup* destArmy	= new HoI4RegGroup();

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
	vector<int> locationCandidates = getHoI4ProvinceNums(inverseProvinceMap, oldArmy->getLocation());
	if (locationCandidates.size() == 0)
	{
		LOG(LogLevel::Warning) << "Army or Navy " << oldArmy->getName() << " assigned to unmapped province " << oldArmy->getLocation() << "; placing units in the production queue.";
		destArmy->setProductionQueue(true);
	}

	// guarantee that navies are assigned to sea provinces, or land provinces with naval bases
	if ((locationCandidates.size() > 0) && (oldArmy->getNavy()) && (!oldArmy->getAtSea()))
	{
		map<int, HoI4Province*>::const_iterator pitr = provinces.find(locationCandidates[0]);
		if (pitr != provinces.end() && pitr->second->isLand())
		{
			locationCandidates = getPortLocationCandidates(locationCandidates, HoI4AdjacencyMap);
			if (locationCandidates.size() == 0)
			{
				LOG(LogLevel::Warning) << "Navy " << oldArmy->getName() << " assigned to V2 province " << oldArmy->getLocation() << " which has no corresponding HoI4 port provinces or adjacent sea provinces; placing units in the production queue.";
				destArmy->setProductionQueue(true);
			}
		}
	}

	// pick the actual location
	int selectedLocation;
	HoI4Province* locationProvince = NULL;
	if (locationCandidates.size() > 0)
	{
		selectedLocation = locationCandidates[rand() % locationCandidates.size()];
		destArmy->setLocation(selectedLocation);
		map<int, HoI4Province*>::iterator pitr = provinces.find(selectedLocation);
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
		for (auto regiment: regiments)
		{
			destArmy->addRegiment(*regiment, true);
		}
		return destArmy;
	}

	// air units need to be split into air forces, so create an air force regiment to hold any air units
	HoI4RegGroup destWing;
	destWing.setForceType(air);
	HoI4Province* airLocationProvince = NULL;
	if (!destArmy->getProductionQueue())
	{
		int airLocation = -1;
		if (locationProvince != NULL)
		{
			airLocation = getAirLocation(locationProvince, HoI4AdjacencyMap, tag);
		}
		destWing.setLocation(airLocation);
		map<int, HoI4Province*>::iterator pitr = provinces.find(airLocation);
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
	vector<HoI4Regiment*> unprocessedRegiments;
	for (auto regiment: regiments)
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
	regiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// put all 'armored' type regiments together
	HoI4RegGroup armored;
	armored.setForceType(land);
	armored.setLocation(selectedLocation);
	for (auto regiment: regiments)
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
	regiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// put all 'specialist' type regiments together
	HoI4RegGroup specialist;
	specialist.setForceType(land);
	specialist.setLocation(selectedLocation);
	for (auto regiment: regiments)
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
	regiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// make infantry and militia divisions, with support regiments added
	while (true)
	{
		HoI4RegGroup newGroup;
		newGroup.setForceType(land);
		newGroup.setLocation(selectedLocation);

		// get the first regiment (required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "infantry_brigade") ||
				 ((*regiment)->getType().getName() == "militia_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}
		if (newGroup.isEmpty())
		{
			break;
		}

		// get the second regiment (not required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "infantry_brigade") ||
				((*regiment)->getType().getName() == "militia_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}

		// get the support first regiment (not required, non-engineer preferred)
		int numSupport = 0;
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "anti_air_brigade") ||
				 ((*regiment)->getType().getName() == "anti_tank_brigade") ||
				 ((*regiment)->getType().getName() == "artillery_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				numSupport++;
				break;
			}
		}

		// get the second support regiment (not required, non-engineer preferred)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "anti_air_brigade") ||
				((*regiment)->getType().getName() == "anti_tank_brigade") ||
				((*regiment)->getType().getName() == "artillery_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				numSupport++;
				break;
			}
		}

		// add engineers as needed and available
		if (numSupport < 2)
		{
			for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
			{
				if ((*regiment)->getType().getName() == "engineer_brigade")
				{
					newGroup.addRegiment(**regiment, true);
					regiments.erase(regiment);
					numSupport++;
					break;
				}
			}
		}
		if (numSupport < 2)
		{
			for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
			{
				if ((*regiment)->getType().getName() == "engineer_brigade")
				{
					newGroup.addRegiment(**regiment, true);
					regiments.erase(regiment);
					break;
				}
			}
		}

		destArmy->addChild(newGroup, true);
	}

	// put together cavalry units (allow engineers to be included)
	while (true)
	{
		HoI4RegGroup newGroup;
		newGroup.setForceType(land);
		newGroup.setLocation(selectedLocation);

		// get the first regiment (required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "cavalry_brigade"))
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}
		if (newGroup.isEmpty())
		{
			break;
		}

		// get the second regiment (required to continue)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "cavalry_brigade"))
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}
		if (newGroup.size() < 2)
		{
			destArmy->addChild(newGroup, true);
			break;
		}

		// get the first engineer regiment (not required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if ((*regiment)->getType().getName() == "engineer_brigade")
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}

		// get the second engineer regiment (not required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if ((*regiment)->getType().getName() == "engineer_brigade")
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}

		destArmy->addChild(newGroup, true);
	}

	if (regiments.size() > 0)
	{
		LOG(LogLevel::Warning) << "Leftover regiments in " << tag << "'s army " << oldArmy->getName() << ". Likely too many support units.";
		for (auto regiment: regiments)
		{
			HoI4RegGroup newGroup;
			newGroup.setForceType(land);
			newGroup.setLocation(selectedLocation);
			newGroup.addRegiment(*regiment, true);
			destArmy->addChild(newGroup, true);
		}
	}
	

	return destArmy;
}


void HoI4World::convertArmies(const V2World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
{
	//unitTypeMapping unitTypeMap = getUnitMappings();

	//// define the headquarters brigade type
	//HoI4RegimentType hqBrigade("hq_brigade");

	//// convert each country's armies
	//for (auto country: countries)
	//{
	//	const V2Country* oldCountry = country.second->getSourceCountry();
	//	if (oldCountry == NULL)
	//	{
	//		continue;
	//	}

	//	int airForceIndex = 0;
	//	HoI4RegGroup::resetHQCounts();
	//	HoI4RegGroup::resetRegGroupNameCounts();

	//	// A V2 unit type counter to keep track of how many V2 units of this type were converted.
	//	// Used to distribute HoI4 unit types in case of multiple mapping
	//	map<string, unsigned> typeCount;

	//	// Convert actual armies
	//	for (auto oldArmy: oldCountry->getArmies())
	//	{
	//		// convert the regiments
	//		vector<HoI4Regiment*> regiments = convertRegiments(unitTypeMap, oldArmy->getRegiments(), typeCount, country);

	//		// place the regiments into armies
	//		HoI4RegGroup* army = createArmy(inverseProvinceMap, HoI4AdjacencyMap, country.first, oldArmy, regiments, airForceIndex);
	//		army->setName(oldArmy->getName());

	//		// add the converted units to the country
	//		if ((army->getForceType() == land) && (!army->isEmpty()) && (!army->getProductionQueue()))
	//		{
	//			army->createHQs(hqBrigade); // Generate HQs for all hierarchies
	//			country.second->addArmy(army);
	//		}
	//		else if (!army->isEmpty())
	//		{
	//			country.second->addArmy(army);
	//		}
	//	}

	//	// Anticipate practical points being awarded for completing the unit constructions
	//	for (auto armyItr: country.second->getArmies())
	//	{
	//		if (armyItr->getProductionQueue())
	//		{
	//			armyItr->undoPracticalAddition(country.second->getPracticals());
	//		}
	//	}
	//}
}


void HoI4World::checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string leader, const string& factionName)
{
	bool leaderSet = false;
	for (auto candidate: candidateTags)
	{
		// get HoI4 tag from V2 tag
		string hoiTag = countryMap[candidate];
		if (hoiTag.empty())
		{
			LOG(LogLevel::Warning) << "Tag " << candidate << " requested for " << factionName << " faction, but is unmapped!";
			continue;
		}

		// find HoI4 nation and ensure that it has land
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


void HoI4World::factionSatellites()
{
	// make sure that any vassals are in their master's faction
	const vector<HoI4Agreement>& agreements = diplomacy.getAgreements();
	for (auto agreement: agreements)
	{
		if (agreement.type == "vassa")
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


void HoI4World::setFactionMembers(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	// find faction memebers
	if (Configuration::getFactionLeaderAlgo() == "manua")
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
			auto itr = countries.find(countryMap[countryItr]);
			if (itr != countries.end())
			{
				HoI4Country* country = itr->second;
				const string government = country->getGovernment();
				const string ideology = country->getIdeology();
				if (
					(government == "national_socialism" || government == "fascist_republic" || government == "germanic_fascist_republic" ||
						government == "right_wing_republic" || government == "hungarian_right_wing_republic" || government == "right_wing_autocrat" ||
						government == "absolute_monarchy" || government == "imperia"
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
						(ideology == "social_conservative" || ideology == "market_libera" || ideology == "social_libera" || ideology == "social_democrat")
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
						government == "absolute_monarchy" || government == "imperia"
						) &&
						(ideology == "left_wing_radica" || ideology == "leninist" || ideology == "stalinist")
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
		for (auto country: countries)
		{
			const string government = country.second->getGovernment();
			const string ideology = country.second->getIdeology();
			if (
				(government == "socialist_republic" || government == "federal_socialist_republic") &&
				(ideology == "left_wing_radica" || ideology == "leninist" || ideology == "stalinist")
				)
			{
				if (country.second->getFaction() == "") // Skip if already a faction member
				{
					if (cominternLeader == "")
					{
						cominternLeader = country.first; // Faction leader
						country.second->setFaction("comintern");
						country.second->setFactionLeader();
					}
					else
					{
						// Check if enemy of leader
						bool enemy = false;
						const map<string, HoI4Relations*>& relations = country.second->getRelations();
						auto relationItr = relations.find(cominternLeader);
						if (relationItr != relations.end() && relationItr->second->atWar())
						{
							enemy = true;
						}

						if (!enemy)
						{
							country.second->setFaction("comintern");
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


void HoI4World::setAlignments()
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
			HoI4Alignment axisStart;
			HoI4Alignment alliesStart;
			HoI4Alignment cominternStart;
			if (axisLeader != "")
			{
				HoI4Relations* relObj = country.second->getRelations(axisLeader);
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
				HoI4Relations* relObj = country.second->getRelations(alliesLeader);
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
				HoI4Relations* relObj = country.second->getRelations(cominternLeader);
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
			(*(country.second->getAlignment())) = HoI4Alignment::getCentroid(axisStart, alliesStart, cominternStart);
		}
	}
}


void HoI4World::configureFactions(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	setFactionMembers(sourceWorld, countryMap);
	factionSatellites(); // push satellites into the same faction as their parents
	setAlignments();
}


void HoI4World::generateLeaders(const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap)
{
	for (auto country: countries)
	{
		country.second->generateLeaders(leaderTraits, namesMap, portraitMap);
	}
}


void HoI4World::consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap)
{
	double totalManpower		= 0.0;
	double totalLeadership	= 0.0;
	double totalIndustry		= 0.0;
	for (auto countryItr: countries)
	{
		countryItr.second->consolidateProvinceItems(inverseProvinceMap, totalManpower, totalLeadership, totalIndustry);
	}

	double suggestedManpowerConst		= 3302.8 * Configuration::getManpowerFactor()	/ totalManpower;
	LOG(LogLevel::Debug) << "Total manpower was " << totalManpower << ". Changing the manpower factor to " << suggestedManpowerConst << " would match vanilla HoI4.";

	double suggestedLeadershipConst	=  212.0 * Configuration::getLeadershipFactor() / totalLeadership;
	LOG(LogLevel::Debug) << "Total leadership was " << totalLeadership << ". Changing the leadership factor to " << suggestedLeadershipConst << " would match vanilla HoI4.";

	double suggestedIndustryConst		= 1746.0 * Configuration::getIcFactor() / totalIndustry;
	LOG(LogLevel::Debug) << "Total IC was " << totalIndustry << ". Changing the IC factor to " << suggestedIndustryConst << " would match vanilla HoI4.";
}


void HoI4World::convertVictoryPoints(const V2World& sourceWorld, const CountryMapping& countryMap)
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
		const std::string& HoI4Tag = countryMap[country];
		auto countryItr = countries.find(HoI4Tag);
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


void HoI4World::convertDiplomacy(const V2World& sourceWorld, const CountryMapping& countryMap)
{
	for (auto agreement: sourceWorld.getDiplomacy()->getAgreements())
	{
		string HoI4Tag1 = countryMap[agreement.country1];
		if (HoI4Tag1.empty())
		{
			continue;
		}
		string HoI4Tag2 = countryMap[agreement.country2];
		if (HoI4Tag2.empty())
		{
			continue;
		}

		map<string, HoI4Country*>::iterator HoI4Country1 = countries.find(HoI4Tag1);
		map<string, HoI4Country*>::iterator HoI4Country2 = countries.find(HoI4Tag2);
		if (HoI4Country1 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI4 country " << HoI4Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		if (HoI4Country2 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI4 country " << HoI4Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		// shared diplo types
		if ((agreement.type == "alliance") || (agreement.type == "vassa"))
		{
			// copy agreement
			HoI4Agreement HoI4a;
			HoI4a.country1		= HoI4Tag1;
			HoI4a.country2		= HoI4Tag2;
			HoI4a.start_date	= agreement.start_date;
			HoI4a.type			= agreement.type;
			diplomacy.addAgreement(HoI4a);

			if (agreement.type == "alliance")
			{
				HoI4Country1->second->editAllies().insert(HoI4Tag2);
			}
		}
	}

	// Relations and guarantees
	for (auto country: countries)
	{
		for (auto relationItr: country.second->getRelations())
		{
			HoI4Agreement HoI4a;
			if (country.first < relationItr.first) // Put it in order to eliminate duplicate relations entries
			{
				HoI4a.country1 = country.first;
				HoI4a.country2 = relationItr.first;
			}
			else
			{
				HoI4a.country2 = relationItr.first;
				HoI4a.country1 = country.first;
			}

			HoI4a.value = relationItr.second->getRelations();
			HoI4a.start_date = date("1930.1.1"); // Arbitrary date
			HoI4a.type = "relation";
			diplomacy.addAgreement(HoI4a);

			if (relationItr.second->getGuarantee())
			{
				HoI4Agreement HoI4a;
				HoI4a.country1 = country.first;
				HoI4a.country2 = relationItr.first;
				HoI4a.start_date = date("1930.1.1"); // Arbitrary date
				HoI4a.type = "guarantee";
				diplomacy.addAgreement(HoI4a);
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


void HoI4World::copyFlags(const V2World &sourceWorld, const CountryMapping& countryMap)
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
		const std::string& HoI4Tag = countryMap[V2Tag];

		bool flagCopied = false;
		vector<string> mods = Configuration::getVic2Mods();
		for (auto mod: mods)
		{
			string sourceFlagPath = Configuration::getV2Path() + "\\mod\\" + mod + "\\gfx\\flags\\"+ V2FlagFile;
			if (WinUtils::DoesFileExist(sourceFlagPath))
			{
				std::string destFlagPath = outputFlagFolder + '\\' + HoI4Tag + ".tga";
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
				std::string destFlagPath = outputFlagFolder + '\\' + HoI4Tag + ".tga";
				WinUtils::TryCopyFile(sourceFlagPath, destFlagPath);
			}
		}
	}
}


void HoI4World::recordAllLandProvinces()
{
	ifstream definitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	while (true)
	{
		string line;
		getline(definitions, line);
		int pos = line.find_first_of(';');
		if (pos == string::npos)
		{
			break;
		}
		int provNum = atoi(line.substr(0, pos).c_str());
		
		line = line.substr(pos + 1, line.length());
		int pos2 = line.find_first_of(';');
		line = line.substr(pos2 + 1, line.length());
		int pos3 = line.find_first_of(';');
		line = line.substr(pos3 + 1, line.length());
		int pos4 = line.find_first_of(';');
		line = line.substr(pos4 + 1, line.length());
		int pos5 = line.find_first_of(';');
		line = line.substr(0, pos5);

		if (line == "land")
		{
			landProvinces.insert(provNum);
		}
	}
}


void HoI4World::checkAllProvincesMapped(const provinceMapping& provinceMap)
{
	ifstream definitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	while (true)
	{
		string line;
		getline(definitions, line);
		int pos = line.find_first_of(';');
		if (pos == string::npos)
		{
			break;
		}
		int provNum = atoi(line.substr(0, pos).c_str());

		provinceMapping::const_iterator num = provinceMap.find(provNum);
		if (num == provinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for HoI4 province " << provNum;
		}
	}

	definitions.close();
}


void HoI4World::setAIFocuses(const AIFocusModifiers& focusModifiers)
{
	for (auto countryItr: countries)
	{
		countryItr.second->setAIFocuses(focusModifiers);
	}
}


void HoI4World::addMinimalItems(const inverseProvinceMapping& inverseProvinceMap)
{
	for (auto country : countries)
	{
		country.second->addMinimalItems(inverseProvinceMap);
	}
}