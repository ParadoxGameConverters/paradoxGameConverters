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
#include <fstream>
#include <algorithm>
#include <list>
#include <queue>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 
//#include <IL\il.h>
//#include <IL\ilu.h>
//#include <IL\ilut.h>
//#include <IL\config.h>
//#include <IL\devil_internal_exports.h>
//#include <IL\ilu_region.h>
//#include <IL\ilut_config.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include "ParadoxParserUTF8.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../Configuration.h"
#include "../V2World/V2Province.h"
#include "../V2World/V2Party.h"
#include "HoI4Relations.h"
#include "HoI4State.h"
#include "HoI4SupplyZone.h"



void HoI4World::importSuppplyZones(const map<int, vector<int>>& defaultStateToProvinceMap, map<int, int>& provinceToSupplyZoneMap)
{
	LOG(LogLevel::Info) << "Importing supply zones";

	set<string> supplyZonesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/map/supplyareas", supplyZonesFiles);
	for (auto supplyZonesFile: supplyZonesFiles)
	{
		// record the filename
		int num = stoi(supplyZonesFile.substr(0, supplyZonesFile.find_first_of('-')));
		supplyZonesFilenames.insert(make_pair(num, supplyZonesFile));

		// record the other data
		Object* fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/map/supplyareas/" + supplyZonesFile);
		if (fileObj == nullptr)
		{
			LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/map/supplyareas/" << supplyZonesFile;
			exit(-1);
		}
		auto supplyAreaObj	= fileObj->getValue("supply_area");
		int ID					= stoi(supplyAreaObj[0]->getLeaf("id"));
		int value				= stoi(supplyAreaObj[0]->getLeaf("value"));

		HoI4SupplyZone* newSupplyZone = new HoI4SupplyZone(ID, value);
		supplyZones.insert(make_pair(ID, newSupplyZone));

		// map the provinces to the supply zone
		auto statesObj = supplyAreaObj[0]->getValue("states");
		for (auto idString: statesObj[0]->getTokens())
		{
			auto mapping = defaultStateToProvinceMap.find(stoi(idString));
			for (auto province: mapping->second)
			{
				provinceToSupplyZoneMap.insert(make_pair(province, ID));
			}
		}
	}
}


void HoI4World::importStrategicRegions()
{
	set<string> filenames;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/map/strategicregions/", filenames);
	for (auto filename: filenames)
	{
		HoI4StrategicRegion* newRegion = new HoI4StrategicRegion(filename);
		strategicRegions.insert(make_pair(newRegion->getID(), newRegion));

		for (auto province: newRegion->getOldProvinces())
		{
			provinceToStratRegionMap.insert(make_pair(province, newRegion->getID()));
		}
	}
}


void HoI4World::checkCoastalProvinces()
{
	// determine whether each province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise HoI4 crashes)
	//Object*	obj2 = parser_UTF8::doParseFile((Configuration::getHoI4Path() + "/tfh/map/positions.txt").c_str());
	//vector<Object*> objProv = obj2->getLeaves();
	//if (objProv.size() == 0)
	//{
	//	LOG(LogLevel::Error) << "map/positions.txt failed to parse.";
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
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/common/country_tags", countryFilenames);

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
	outputColorsfile();
	//outputAutoexecLua();
	outputLocalisations();
	outputHistory();
	//output events
	Utils::copyFolder("events", "output/" + Configuration::getOutputName());
	outputMap();
	outputSupply();
}


void HoI4World::outputCommonCountries() const
{
	// Create common\countries path.
	string countriesPath = "Output/" + Configuration::getOutputName() + "/common";
	/*if (!Utils::TryCreateFolder(countriesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common\"";
		exit(-1);
	}*/
	if (!Utils::TryCreateFolder(countriesPath + "/countries"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common/countries\"";
		exit(-1);
	}
	if (!Utils::TryCreateFolder(countriesPath + "/country_tags"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common/country_tags\"";
		exit(-1);
	}

	// Output common\countries.txt
	LOG(LogLevel::Debug) << "Writing countries file";
	FILE* allCountriesFile;
	if (fopen_s(&allCountriesFile, ("Output/" + Configuration::getOutputName() + "/common/country_tags/01_countries.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create countries file";
		exit(-1);
	}

	for (auto countryItr: countries)
	{
		if ((potentialCountries.find(countryItr.first) == potentialCountries.end()) && (countryItr.second->getCapitalNum() != 0))
		{
			countryItr.second->outputToCommonCountriesFile(allCountriesFile);
		}
	}
	fprintf(allCountriesFile, "\n");
	fclose(allCountriesFile);
}


void HoI4World::outputColorsfile() const
{

	ofstream output;
	output.open(("Output/" + Configuration::getOutputName() + "/common/countries/colors.txt"));
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open Output/" << Configuration::getOutputName() << "/common/countries/colors.txt";
		exit(-1);
	}

	output << "#reload countrycolors\r\n";
	for (auto countryItr: countries)
	{
		if (	(potentialCountries.find(countryItr.first) == potentialCountries.end()) &&
				(countryItr.second->getCapitalNum() != 0)
			)
		{
			countryItr.second->outputColors(output);
		}
	}

	output.close();
}


void HoI4World::outputAutoexecLua() const
{
	// output autoexec.lua
	FILE* autoexec;
	if (fopen_s(&autoexec, ("Output/" + Configuration::getOutputName() + "/script/autoexec.lua").c_str(), "w") != 0)
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

	for (auto country: potentialCountries)
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
	string localisationPath = "Output/" + Configuration::getOutputName() + "/localisation";
	if (!Utils::TryCreateFolder(localisationPath))
	{
		LOG(LogLevel::Error) << "Could not create localisation folder";
		exit(-1);
	}

	localisation.output(localisationPath);
}


void HoI4World::outputMap() const
{
	LOG(LogLevel::Debug) << "Writing Map Info";

	// create the map folder
	if (!Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/map"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/map";
		exit(-1);
	}

	// create the rocket sites file
	ofstream rocketSitesFile("Output/" + Configuration::getOutputName() + "/map/rocketsites.txt");
	if (!rocketSitesFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create Output/" << Configuration::getOutputName() << "/map/rocketsites.txt";
		exit(-1);
	}
	for (auto state: states->getStates())
	{
		auto provinces = state.second->getProvinces();
		rocketSitesFile << state.second->getID() << " = { " << *provinces.begin() << " }\n";
	}
	rocketSitesFile.close();

	// create the airports file
	ofstream airportsFile("Output/" + Configuration::getOutputName() + "/map/airports.txt");
	if (!airportsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create Output/" << Configuration::getOutputName() << "/map/airports.txt";
		exit(-1);
	}
	for (auto state: states->getStates())
	{
		auto provinces = state.second->getProvinces();
		airportsFile << state.second->getID() << " = { " << *provinces.begin() << " }\n";
	}
	airportsFile.close();

	// output strategic regions
	if (!Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/map/strategicregions"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/map/strategicregions";
		exit(-1);
	}
	for (auto strategicRegion: strategicRegions)
	{
		strategicRegion.second->output("Output/" + Configuration::getOutputName() + "/map/strategicregions/");
	}
}


void HoI4World::outputHistory() const
{
	states->output();	

	LOG(LogLevel::Debug) << "Writing countries";
	string unitsPath = "Output/" + Configuration::getOutputName() + "/history/units";
	if (!Utils::TryCreateFolder(unitsPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history/units";
		exit(-1);
	}
	/*for (auto countryItr: countries)
	{
		countryItr.second->output(states->getStates(), );
	}*/
	// Override vanilla history to suppress vanilla OOB and faction membership being read
	vector<vector<HoI4Country*>> empty;
	for (auto potentialItr: potentialCountries)
	{
		if (countries.find(potentialItr.first) == countries.end())
		{
			potentialItr.second->output(states->getStates(), empty, "asdf");
		}
	}
	LOG(LogLevel::Debug) << "Writing diplomacy";
	//diplomacy.output();
}


void HoI4World::getProvinceLocalizations(const string& file)
{
	ifstream read;
	string line;
	read.open(file.c_str());
	while (read.good() && !read.eof())
	{
		getline(read, line);
		if (line.substr(0, 4) == "PROV" && isdigit(line.c_str()[4]))
		{
			int position = line.find_first_of(';');
			int num = atoi(line.substr(4, position - 4).c_str());
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			provinces[num]->setName(name);
		}
	}
	read.close();
}


void HoI4World::convertCountries(const CountryMapping& countryMap, const Vic2ToHoI4ProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	for (auto sourceItr: sourceWorld->getCountries())
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
			V2Party* rulingParty = sourceWorld->getRulingParty(sourceItr.second);
			if (rulingParty == NULL)
			{
				LOG(LogLevel::Error) << "Could not find the ruling party for " << sourceItr.first << ". Were all mods correctly included?";
				exit(-1);
			}
			destCountry->initFromV2Country(*sourceWorld, sourceItr.second, rulingParty->ideology, countryMap, inverseProvinceMap, leaderMap, V2Localisations, governmentJobs, namesMap, portraitMap, cultureMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, states->getProvinceToStateIDMap(), states->getStates());
			countries.insert(make_pair(HoI4Tag, destCountry));
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not convert V2 tag " << sourceItr.first << " to HoI4";
		}

		localisation.readFromCountry(sourceItr.second, HoI4Tag);
	}

	// initialize all potential countries
	// ALL potential countries should be output to the file, otherwise some things don't get initialized right in HoI4
	LOG(LogLevel::Debug) << "initializing potential countries";
	for (auto potentialItr: potentialCountries)
	{
		map<string, HoI4Country*>::iterator citr = countries.find(potentialItr.first);
		if (citr == countries.end())
		{
			potentialItr.second->initFromHistory();
		}
	}
}


void HoI4World::outputSupply() const
{
	// create the folders
	if (!Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/map/supplyareas"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/map/supplyareas";
		exit(-1);
	}

	// output the supply zones
	for (auto zone: supplyZones)
	{
		auto filenameMap = supplyZonesFilenames.find(zone.first);
		if (filenameMap != supplyZonesFilenames.end())
		{
			zone.second->output(filenameMap->second);
		}
	}
}


void HoI4World::convertNavalBases(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap)
{
	Object* fileObj			= parser_UTF8::doParseFile("navalprovinces.txt");
	auto linkObj				= fileObj->getValue("link");
	auto navalProvincesObj	= linkObj[0]->getValue("province");

	// create a lookup table of naval provinces
	unordered_map<int, int> navalProvinces;
	for (auto provice: navalProvincesObj)
	{
		int navalProvince = stoi(provice->getLeaf());
		navalProvinces.insert(make_pair(navalProvince, navalProvince));
	}

	// convert naval bases. There should only be one per HoI4 state
	for (auto state: states->getStates())
	{
		auto vic2State = state.second->getSourceState();

		int		navalBaseLevel		= 0;
		int		navalBaseLocation	= 0;
		for (auto provinceNum: vic2State->getProvinces())
		{
			V2Province* sourceProvince = sourceWorld->getProvince(provinceNum);
			if (sourceProvince->getNavalBase() > 0)
			{
				navalBaseLevel += sourceProvince->getNavalBase();

				// set the naval base in only coastal provinces
				if (navalBaseLocation == 0)
				{
					auto provinceMapping = inverseProvinceMap.find(provinceNum);
					if (provinceMapping != inverseProvinceMap.end())
					{
						for (auto HoI4ProvNum: provinceMapping->second)
						{
							if (navalProvinces.find(HoI4ProvNum) != navalProvinces.end())
							{
								navalBaseLocation	= HoI4ProvNum;
								break;
							}
						}
					}
				}
			}
		}

		if (navalBaseLocation != 0)
		{
			state.second->setNavalBase(navalBaseLevel, navalBaseLocation);
		}
	}
}


void HoI4World::convertIndustry()
{
	// calculate the factory/worker ratio for every country
	map<string, double> ratioMap;
	for (auto HoI4Country: countries)
	{
		// get the Vic2 country
		auto Vic2Country = HoI4Country.second->getSourceCountry();

		// get the total number of employed/employable workers
		double employedWorkersAdjusted = 0;
		for (auto sourceProvince: Vic2Country->getProvinces())
		{
			// takes employed workers and divides by 100,000 to convert
			employedWorkersAdjusted += sourceProvince.second->getEmployedWorkers() / 100000.0;
		}

		// calculate the ratio between Vic2 employed workers and HoI4 factories
		double sinPart		= sin(employedWorkersAdjusted / 150) * 100;
		double logpart		= log10(employedWorkersAdjusted) * 15;
		double HoI4TotalFactories = sinPart + logpart + 5;
		if (employedWorkersAdjusted != 0)
		{
			ratioMap[HoI4Country.second->getTag()] = HoI4TotalFactories / employedWorkersAdjusted;
		}
		else
		{
			ratioMap[HoI4Country.second->getTag()] = 0.0;
		}
	}

	//	loop through the HoI4 states to set the factory levels
	for (auto HoI4State: states->getStates())
	{
		auto Vic2State = HoI4State.second->getSourceState();

		auto ratioMapping = ratioMap.find(HoI4State.second->getOwner());
		if (ratioMapping == ratioMap.end())
		{
			continue;
		}
		double	stateWorkers		= 0;
		int		stateFactories		= 0;
		int		population			= 0;
		int		railLevel			= 0;
		for (auto provinceNum: Vic2State->getProvinces())
		{
			// get population, rail level, and workers to convert slots and states*conversion percentage
			V2Province* sourceProvince = sourceWorld->getProvince(provinceNum);

			population		+= sourceProvince->getLiteracyWeightedPopulation();
			railLevel		 = sourceProvince->getInfra();
			stateWorkers	+= sourceProvince->getEmployedWorkers() * ratioMapping->second / 100000;
		}

		// make sure there are no negative factories
		if (stateWorkers < 0)
		{
			stateWorkers = 0;
		}
		stateFactories = static_cast<int>(round(stateWorkers));
		if ((stateFactories + HoI4State.second->getDockyards()) > 12) // limit factories by 12 (the max slots)
		{
			stateFactories = 12 - HoI4State.second->getDockyards();
		}

		// determine state category
		int stateSlots = population / 120000; // one slot is given per 120,000 people (need to change)
		if ((stateFactories + HoI4State.second->getDockyards()) >= stateSlots)
		{
			stateSlots = stateFactories + HoI4State.second->getDockyards() + 2;
		}

		string category = "";
		if (stateSlots >= 12)
		{
			category = "megalopolis";
		}
		else if (stateSlots >= 10)
		{
			category = "metropolis";
		}
		else if (stateSlots >= 8)
		{
			category = "large_city";
		}
		else if (stateSlots >= 6)
		{
			category = "city";
		}
		else if (stateSlots >= 5)
		{
			category = "large_town";
		}
		else if (stateSlots >= 4)
		{
			category = "town";
		}
		else if (stateSlots >= 2)
		{
			category = "rural";
		}
		else if (stateSlots >= 1)
		{
			category = "pastoral";
		}
		else
		{
			category = "enclave";
		}

		//better rails for better industry
		if (stateFactories > 10)
		{
			railLevel += 3;
		}
		else if (stateFactories > 6)
		{
			railLevel += 2;
		}
		else if (stateFactories > 4)
		{
			railLevel++;
		}

		// distribute military and civilian factories using unseeded random
		//		0-6 gives a civilian factory, 7-9 gives a military factory
		int civilianFactories	= 0;
		int militaryFactories	= 0;
		for (int i = 0; i < stateFactories; i++)
		{
			int randomNum = rand() % 10;
			if (randomNum > 6)
			{
				militaryFactories++;
			}
			else
			{
				civilianFactories++;
			}
		}
		HoI4State.second->setIndustry(civilianFactories, militaryFactories, category, railLevel);
	}

	//// now that all provinces have had owners and cores set, convert their other items
	//for (auto mapping: inverseProvinceMap)
	//{
	//	// get the source province
	//	int srcProvinceNum = mapping.first;
	//	V2Province* sourceProvince = sourceWorld->getProvince(srcProvinceNum);
	//	if (sourceProvince == NULL)
	//	{
	//		continue;
	//	}

	//	// convert items that apply to all destination provinces
	//	for (auto dstProvinceNum: mapping.second)
	//	{
	//		// get the destination province
	//		auto dstProvItr = provinces.find(dstProvinceNum);
	//		if (dstProvItr == provinces.end())
	//		{
	//			continue;
	//		}

	//		// source provinces from other owners should not contribute to the destination province
	//		if (countryMap[sourceProvince->getOwnerString()] != dstProvItr->second->getOwner())
	//		{
	//			continue;
	//		}

	//		// determine if this is a border province or not
	//		bool borderProvince = false;
	//		if (HoI4AdjacencyMap.size() > static_cast<unsigned int>(dstProvinceNum))
	//		{
	//			const vector<adjacency> adjacencies = HoI4AdjacencyMap[dstProvinceNum];
	//			for (auto adj: adjacencies)
	//			{
	//				auto province = provinces.find(dstProvinceNum);
	//				auto adjacentProvince = provinces.find(adj.to);
	//				if ((province != provinces.end()) && (adjacentProvince != provinces.end()) && (province->second->getOwner() != adjacentProvince->second->getOwner()))
	//				{
	//					borderProvince = true;
	//					break;
	//				}
	//			}
	//		}

	//		// convert forts, naval bases, and infrastructure
	//		int fortLevel = sourceProvince->getFort();
	//		fortLevel = max(0, (fortLevel - 5) * 2 + 1);
	//		if (dstProvItr->second->getNavalBase() > 0)
	//		{
	//			dstProvItr->second->requireCoastalFort(fortLevel);
	//		}
	//		if (borderProvince)
	//		{
	//			dstProvItr->second->requireLandFort(fortLevel);
	//		}
	//		dstProvItr->second->requireInfrastructure((int)Configuration::getMinInfra());
	//		if (sourceProvince->getInfra() > 0) // No infra stays at minInfra
	//		{
	//			dstProvItr->second->requireInfrastructure(sourceProvince->getInfra() + 4);
	//		}

	//		if ((Configuration::getLeadershipConversion() == "linear") || (Configuration::getLeadershipConversion() == "squareroot"))
	//		{
	//			dstProvItr->second->setLeadership(0.0);
	//		}
	//		if ((Configuration::getManpowerConversion() == "linear") || (Configuration::getManpowerConversion() == "squareroot"))
	//		{
	//			dstProvItr->second->setManpower(0.0);
	//		}
	//		if ((Configuration::getIcConversion() == "squareroot") || (Configuration::getIcConversion() == "linear") || (Configuration::getIcConversion() == "logarithmic"))
	//		{
	//			dstProvItr->second->setRawIndustry(0.0);
	//			dstProvItr->second->setActualIndustry(0);
	//		}
	//	}

	//	// convert items that apply to only one destination province
	//	if (mapping.second.size() > 0)
	//	{
	//		// get the destination province
	//		auto dstProvItr = provinces.find(mapping.second[0]);
	//		if (dstProvItr == provinces.end())
	//		{
	//			continue;
	//		}

	//		// convert industry
	//		double industry = sourceProvince->getEmployedWorkers();
	//		if (Configuration::getIcConversion() == "squareroot")
	//		{
	//			industry = sqrt(double(industry)) * 0.01294;
	//			dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
	//		}
	//		else if (Configuration::getIcConversion() == "linear")
	//		{
	//			industry = double(industry) * 0.0000255;
	//			dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
	//		}
	//		else if (Configuration::getIcConversion() == "logarithmic")
	//		{
	//			industry = log(max(1.0, industry / 70000)) / log(2) * 5.33;
	//			dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
	//		}

	//		// convert manpower
	//		double newManpower = sourceProvince->getPopulation("soldiers")
	//			+ sourceProvince->getPopulation("craftsmen") * 0.25 // Conscripts
	//			+ sourceProvince->getPopulation("labourers") * 0.25 // Conscripts
	//			+ sourceProvince->getPopulation("farmers") * 0.25; // Conscripts
	//		if (Configuration::getManpowerConversion() == "linear")
	//		{
	//			newManpower *= 0.00003 * Configuration::getManpowerFactor();
	//			newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
	//			dstProvItr->second->addManpower(newManpower);
	//		}
	//		else if (Configuration::getManpowerConversion() == "squareroot")
	//		{
	//			newManpower = sqrt(newManpower);
	//			newManpower *= 0.0076 * Configuration::getManpowerFactor();
	//			newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
	//			dstProvItr->second->addManpower(newManpower);
	//		}

	//		// convert leadership
	//		double newLeadership = sourceProvince->getLiteracyWeightedPopulation("clergymen") * 0.5
	//			+ sourceProvince->getPopulation("officers")
	//			+ sourceProvince->getLiteracyWeightedPopulation("clerks") // Clerks representing researchers
	//			+ sourceProvince->getLiteracyWeightedPopulation("capitalists") * 0.5
	//			+ sourceProvince->getLiteracyWeightedPopulation("bureaucrats") * 0.25
	//			+ sourceProvince->getLiteracyWeightedPopulation("aristocrats") * 0.25;
	//		if (Configuration::getLeadershipConversion() == "linear")
	//		{
	//			newLeadership *= 0.00001363 * Configuration::getLeadershipFactor();
	//			newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
	//			dstProvItr->second->addLeadership(newLeadership);
	//		}
	//		else if (Configuration::getLeadershipConversion() == "squareroot")
	//		{
	//			newLeadership = sqrt(newLeadership);
	//			newLeadership *= 0.00147 * Configuration::getLeadershipFactor();
	//			newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
	//			dstProvItr->second->addLeadership(newLeadership);
	//		}
	//	}
	//}
}


void HoI4World::convertResources()
{
	Object* fileObj = parser_UTF8::doParseFile("resources.txt");
	if (fileObj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not read resources.txt";
		exit(-1);
	}

	auto resourcesObj	= fileObj->getValue("resources");
	auto linksObj		= resourcesObj[0]->getValue("link");

	map<int, map<string, double> > resourceMap;
	for (auto linkObj: linksObj)
	{
		int provinceNumber = stoi(linkObj->getLeaf("province"));
		auto mapping = resourceMap.find(provinceNumber);
		if (mapping == resourceMap.end())
		{
			map<string, double> resources;
			resourceMap.insert(make_pair(provinceNumber, resources));
			mapping = resourceMap.find(provinceNumber);
		}

		auto resourcesObj = linkObj->getValue("resources");
		auto actualResources = resourcesObj[0]->getLeaves();
		for (auto resource: actualResources)
		{
			string	resourceName	= resource->getKey();
			double	amount			= stof(resource->getLeaf());
			mapping->second[resourceName] += amount;
		}
	}

	for (auto state: states->getStates())
	{
		for (auto provinceNumber: state.second->getProvinces())
		{
			auto mapping = resourceMap.find(provinceNumber);
			if (mapping != resourceMap.end())
			{
				for (auto resource: mapping->second)
				{
					state.second->addResource(resource.first, resource.second);
				}
			}
		}
	}
}


void HoI4World::convertSupplyZones(const map<int, int>& provinceToSupplyZoneMap)
{
	for (auto state: states->getStates())
	{
		for (auto province: state.second->getProvinces())
		{
			auto mapping = provinceToSupplyZoneMap.find(province);
			if (mapping != provinceToSupplyZoneMap.end())
			{
				auto supplyZone = supplyZones.find(mapping->second);
				if (supplyZone != supplyZones.end())
				{
					supplyZone->second->addState(state.first);
					break;
				}
			}
		}
	}
}


void HoI4World::convertStrategicRegions()
{
	// assign the states to strategic regions
	for (auto state: states->getStates())
	{
		// figure out which strategic regions are represented
		map<int, int> usedRegions;	// region ID -> number of provinces in that region
		for (auto province: state.second->getProvinces())
		{
			auto mapping = provinceToStratRegionMap.find(province);
			if (mapping == provinceToStratRegionMap.end())
			{
				LOG(LogLevel::Warning) << "Province " << province << " had no original strategic region";
				continue;
			}

			auto usedRegion = usedRegions.find(mapping->second);
			if (usedRegion == usedRegions.end())
			{
				usedRegions.insert(make_pair(mapping->second, 1));
			}
			else
			{
				usedRegion->second++;
			}

			provinceToStratRegionMap.erase(mapping);
		}

		// pick the most represented strategic region
		int mostProvinces	= 0;
		int bestRegion		= 0;
		for (auto region: usedRegions)
		{
			if (region.second > mostProvinces)
			{
				bestRegion		= region.first;
				mostProvinces	= region.second;
			}
		}

		// add the state's province to the region
		auto region = strategicRegions.find(bestRegion);
		if (region == strategicRegions.end())
		{
			LOG(LogLevel::Warning) << "Strategic region " << bestRegion << " was not in the list of regions.";
			continue;
		}
		for (auto province: state.second->getProvinces())
		{
			region->second->addNewProvince(province);
		}
	}

	// add leftover provinces back to their strategic regions
	for (auto mapping: provinceToStratRegionMap)
	{
		auto region = strategicRegions.find(mapping.second);
		if (region == strategicRegions.end())
		{
			LOG(LogLevel::Warning) << "Strategic region " << mapping.second << " was not in the list of regions.";
			continue;
		}
		region->second->addNewProvince(mapping.first);
	}
}


void HoI4World::convertTechs()
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
		const V2Country*	sourceCountry = dstCountry.second->getSourceCountry();
		vector<string>	techs = sourceCountry->getTechs();

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
				for (auto HoI4TechItr: mapItr->second)
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


vector<int> HoI4World::getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
{
	vector<int> portLocationCandidates = getPortProvinces(locationCandidates);
	if (portLocationCandidates.size() == 0)
	{
		// if none of the mapped provinces are ports, try to push the navy out to sea
		for (auto candidate: locationCandidates)
		{
			if (HoI4AdjacencyMap.size() > static_cast<unsigned int>(candidate))
			{
				auto newCandidates = HoI4AdjacencyMap[candidate];
				for (auto newCandidate: newCandidates)
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


void HoI4World::convertArmies(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
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
				LOG(LogLevel::Debug) << candidate << " added to " << factionName << " faction";
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
	const vector<const HoI4Agreement*>& agreements = diplomacy.getAgreements();
	for (auto agreement: agreements)
	{
		if (agreement->type == "vassal")
		{
			auto masterCountry = countries.find(agreement->country1);
			auto satelliteCountry = countries.find(agreement->country2);
			if ((masterCountry != countries.end()) && (masterCountry->second->getFaction() != "") && (satelliteCountry != countries.end()))
			{
				satelliteCountry->second->setFaction(masterCountry->second->getFaction());
			}
		}
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


void HoI4World::configureFactions(const CountryMapping& countryMap)
{
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


void HoI4World::convertArmies(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap)
{
	for (auto country: countries)
	{
		country.second->convertArmyDivisions(inverseProvinceMap);
	}
}


void HoI4World::convertNavies()
{
	for (auto country: countries)
	{
		country.second->convertNavy(states->getStates());
	}
}


void HoI4World::convertVictoryPoints(const CountryMapping& countryMap)
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
	const std::vector<string>& greatCountries = sourceWorld->getGreatCountries();
	for (auto country: sourceWorld->getGreatCountries())
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


void HoI4World::convertDiplomacy(const CountryMapping& countryMap)
{
	for (auto agreement: sourceWorld->getDiplomacy()->getAgreements())
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
			HoI4Agreement* HoI4a = new HoI4Agreement;
			HoI4a->country1 = HoI4Tag1;
			HoI4a->country2 = HoI4Tag2;
			HoI4a->start_date = agreement.start_date;
			HoI4a->type = agreement.type;
			diplomacy.addAgreement(HoI4a);

			if (agreement.type == "alliance")
			{
				HoI4Country1->second->editAllies().insert(HoI4Tag2);
				HoI4Country2->second->editAllies().insert(HoI4Tag1);
			}
		}
	}

	// Relations and guarantees
	for (auto country: countries)
	{
		for (auto relationItr: country.second->getRelations())
		{
			HoI4Agreement* HoI4a = new HoI4Agreement;
			if (country.first < relationItr.first) // Put it in order to eliminate duplicate relations entries
			{
				HoI4a->country1 = country.first;
				HoI4a->country2 = relationItr.first;
			}
			else
			{
				HoI4a->country2 = relationItr.first;
				HoI4a->country1 = country.first;
			}

			HoI4a->value = relationItr.second->getRelations();
			HoI4a->start_date = date("1930.1.1"); // Arbitrary date
			HoI4a->type = "relation";
			diplomacy.addAgreement(HoI4a);

			if (relationItr.second->getGuarantee())
			{
				HoI4Agreement* HoI4a = new HoI4Agreement;
				HoI4a->country1 = country.first;
				HoI4a->country2 = relationItr.first;
				HoI4a->start_date = date("1930.1.1"); // Arbitrary date
				HoI4a->type = "guarantee";
				diplomacy.addAgreement(HoI4a);
			}
			if (relationItr.second->getSphereLeader())
			{
				HoI4Agreement* HoI4a = new HoI4Agreement;
				HoI4a->country1 = country.first;
				HoI4a->country2 = relationItr.first;
				HoI4a->start_date = date("1930.1.1"); // Arbitrary date
				HoI4a->type = "sphere";
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


void HoI4World::createIdeologyFiles()
{
	Utils::copyFolder("NeededFiles/interface", "output/" + Configuration::getOutputName()+"/gfx");
	Utils::copyFolder("NeededFiles/ideologies", "output/" + Configuration::getOutputName() + "/common");
	Utils::copyFolder("NeededFiles/ideas", "output/" + Configuration::getOutputName()+"/common");
	std::string outputGraphicsFolder = "Output/" + Configuration::getOutputName() + "/interface";
	if (!Utils::TryCreateFolder(outputGraphicsFolder))
	{
		return;
	}
	Utils::TryCopyFile("NeededFiles/countrypoliticsview.gfx", "Output/" + Configuration::getOutputName() + "/interface/countrypoliticsview.gfx");
	Utils::copyFolder("NeededFiles/defines", "Output/" + Configuration::getOutputName() + "/common");
}



void HoI4World::checkAllProvincesMapped(const HoI4ToVic2ProvinceMapping& provinceMap)
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
		if (provNum == 0)
		{
			continue;
		}

		auto num = provinceMap.find(provNum);
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


void HoI4World::addMinimalItems(const Vic2ToHoI4ProvinceMapping& inverseProvinceMap)
{
	for (auto country: countries)
	{
		country.second->addMinimalItems(inverseProvinceMap);
	}
}

void HoI4World::fillCountryProvinces()
{
	for (auto country : countries)
	{
		country.second->setProvinceCount(0);
	}
	for(auto state : states->getStates())
	{
		auto ownercountry = countries.find(state.second->getOwner());
		for (auto prov : state.second->getProvinces())
		{
			//fix later
			ownercountry->second->setProvinceCount(ownercountry->second->getProvinceCount() + 1);
		}
	}
}
void HoI4World::setSphereLeaders(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	const vector<string>& greatCountries = sourceWorld.getGreatCountries();
	for (auto countryItr : greatCountries)
	{
		auto itr = countries.find(countryMap[countryItr]);
		if (itr != countries.end())
		{
			HoI4Country* country = itr->second;
			auto relations = country->getRelations();
			for (auto relation : relations)
			{
				if (relation.second->getSphereLeader())
				{
					string tag = relation.second->getTag();
					auto spheredcountry = countries.find(tag);
					if (spheredcountry != countries.end())
					{
						spheredcountry->second->setSphereLeader(country->getTag());
					}
				}
			}
		}
	}
}
string HoI4World::createAnnexEvent(HoI4Country* Annexer, HoI4Country* Annexed, int eventnumber)
{
	string Events;
	string annexername = Annexer->getSourceCountry()->getName();
	string annexedname = Annexed->getSourceCountry()->getName();
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "annex." + to_string(eventnumber) + "\r\n";
	Events += "	title = \"" + annexername + " Demands " + annexedname + "!\"\r\n";
	Events += "	desc = \"Today " + annexername + " sent an envoy to us with a proposition of an union. We are alone and in this world, and a union with " + annexername + " might prove to be fruiteful.";
	Events += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"\r\n";
	Events += "	picture = GFX_report_event_hitler_parade\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = { # Accept\r\n";
	Events += "		name = \"We accept the Union\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 30\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = -15\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size < 40 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 45\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "annex." + to_string(eventnumber + 1) + " }\r\n";//+1 accept
	Events += "		}\r\n";
	Events += "		custom_effect_tooltip = GAME_OVER_TT\r\n";
	Events += "	}\r\n";
	Events += "	option = { # Refuse\r\n";
	Events += "		name = \"We Refuse!\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 10 \r\n";
	Events += "\r\n";
	Events += "			modifier = {\r\n";
	Events += "				factor = 0\r\n";
	Events += "				GER = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 20\r\n";
	Events += "				GER = { has_army_size = { size < 30 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	//Events += "			add_opinion_modifier = { target = ROOT modifier = " + Annexer->getTag() + "_anschluss_rejected }\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "annex." + to_string(eventnumber + 2) + " }\r\n";//+2 refuse
	Events += "			if = { limit = { is_in_faction_with = " + Annexed->getTag() + " }\r\n";
	Events += "				remove_from_faction = " + Annexed->getTag() + "\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	Events += "\r\n";
	//Country Refuses!
	Events += "# Austria refuses Anschluss\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "annex." + to_string(eventnumber+2) + "\r\n";
	Events += "	title = \"" + annexedname + " Refuses!\"\r\n";
	Events += "	desc = \"" + annexedname + " Refused our proposed union! This is an insult to us that cannot go unanswered!\"\r\n";
	Events += "	picture = GFX_report_event_german_troops\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"It's time for war\"\r\n";
	Events += "		create_wargoal = {\r\n";
	Events += "				type = puppet_wargoal_focus\r\n";
	Events += "			target = " + Annexed->getTag() + "\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}";
	//accepts
	Events += "# Austrian Anschluss Completed\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "annex." + to_string(eventnumber+1) + "\r\n";
	Events += "	title = \"" + annexedname + " accepts!\"\r\n";
	Events += "	desc = \"" + annexedname + " accepted our proposed union, their added strength will push us to greatness!\"\r\n";
	Events += "	picture = GFX_report_event_german_speech\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"A stronger Union!\"\r\n";
	for (auto cstate : countriesStates.find(Annexed->getTag())->second)
	{
		Events += "		" + to_string(cstate) + " = {\r\n";
		Events += "			if = {\r\n";
		Events += "				limit = { is_owned_by = " + Annexed->getTag() + " }\r\n";
		Events += "				add_core_of = " + Annexer->getTag() + "\r\n";
		Events += "			}\r\n";
		Events += "		}\r\n";

	}
	Events += "\r\n";
	Events += "		annex_country = { target = " + Annexed->getTag() + " transfer_troops = yes }\r\n";
	Events += "		add_political_power = 50\r\n";
	Events += "		add_named_threat = { threat = 2 name = \"" + annexername + " annexed " + annexedname + "\" }\r\n";
	Events += "		set_country_flag = " + Annexed->getTag() + "_annexed\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	return Events;
}
string HoI4World::createSudatenEvent(HoI4Country* Annexer, HoI4Country* Annexed, int eventnumber, vector<int> claimedStates)
{
	string Events;
	//flesh out this event more, possibly make it so allies have a chance to help?
	string annexername = Annexer->getSourceCountry()->getName();
	string annexedname = Annexed->getSourceCountry()->getName();
	Events += "#Sudaten Events\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber) + "\r\n";
	Events += "	title = \"" + annexername + " Demands " + annexedname + "!\"\r\n";
	Events += "	desc = \"" + annexername + " has recently been making claims to our bordering states, saying that these states are full of " + Annexer->getSourceCountry()->getAdjective("english") + " people and that the territory should be given to them. Although it ";
	Events += "is true that recently our neighboring states have had an influx of " + Annexer->getSourceCountry()->getAdjective("english") + " people in the recent years, we cannot give up our lands because a few "+ Annexer->getSourceCountry()->getAdjective("english") +" settled down in our land. " ;
	Events += "In response " + annexername + " has called for a conference, demanding their territory in exchange for peace. How do we resond? ";
	Events += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"\r\n";
	Events += "	picture = GFX_report_event_hitler_parade\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = { # Accept\r\n";
	Events += "		name = \"We Accept\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 30\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = -15\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size < 40 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 45\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 1) + " }\r\n";//+1 accept
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "	option = { # Refuse\r\n";
	Events += "		name = \"We Refuse!\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 10 \r\n";
	Events += "\r\n";
	Events += "			modifier = {\r\n";
	Events += "				factor = 0\r\n";
	Events += "				GER = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 20\r\n";
	Events += "				GER = { has_army_size = { size < 30 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	//Events += "			add_opinion_modifier = { target = ROOT modifier = " + Annexer->getTag() + "_anschluss_rejected }\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 2) + " }\r\n";//+2 refuse
	Events += "			if = { limit = { is_in_faction_with = " + Annexed->getTag() + " }\r\n";
	Events += "				remove_from_faction = " + Annexed->getTag() + "\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	Events += "\r\n";
	//Country Refuses!
	Events += "# refuses Sudaten\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 2) + "\r\n";
	Events += "	title = \"" + annexedname + " Refuses!\"\r\n";
	Events += "	desc = \"" + annexedname + " Refused our proposed proposition! This is an insult to us that cannot go unanswered!\"\r\n";
	Events += "	picture = GFX_report_event_german_troops\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"It's time for war\"\r\n";
	Events += "		create_wargoal = {\r\n";
	Events += "				type = puppet_wargoal_focus\r\n";
	Events += "			target = " + Annexed->getTag() + "\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}";
	//accepts
	Events += "#  Sudaten Completed\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 1) + "\r\n";
	Events += "	title = \"" + annexedname + " accepts!\"\r\n";
	Events += "	desc = \"" + annexedname + " accepted our proposed demands, the added lands will push us to greatness!\"\r\n";
	Events += "	picture = GFX_report_event_german_speech\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"A stronger Union!\"\r\n";
	for (auto cstate : claimedStates)
	{
		Events += "		" + to_string(cstate) + " = { add_core_of = " + Annexer->getTag() + " }\r\n";
		Events += "		" + Annexer->getTag() + " = { transfer_state =  " + to_string(cstate) + " }\r\n";
	}
	Events += "		set_country_flag = " + Annexed->getTag() + "_demanded\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	return Events;
}
string HoI4World::createMonarchyEmpireNF(HoI4Country* Home, HoI4Country* Annexed1, HoI4Country* Annexed2, HoI4Country* Annexed3, HoI4Country* Annexed4, int ProtectorateNumber, int AnnexNumber, int x)
{
	string FocusTree = "";
	//Glory to Empire!
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = EmpireGlory" + Home->getTag()+"\r\n";
	FocusTree += "		icon = GFX_goal_anschluss\r\n";
	FocusTree += "		text = \"Glory to the Empire!\"\r\n";
	FocusTree += "		available = {\r\n";
	//FocusTree += "			is_puppet = no\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		\r\n";
	FocusTree += "		x =  29\r\n";
	FocusTree += "		y = 0\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "				factor = 0\r\n";
	FocusTree += "				date < 1937.6.6\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			add_national_unity = 0.1\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//Colonies Focus
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = StrengthenColonies" + Home->getTag()+"\r\n";
	FocusTree += "		icon = GFX_goal_anschluss\r\n";
	FocusTree += "		text = \"Strengthen the Colonies\"\r\n";
	FocusTree += "		prerequisite = { focus = EmpireGlory" + Home->getTag() + " }\r\n";
	FocusTree += "		mutually_exclusive = { focus = StrengthenHome" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  28\r\n";
	FocusTree += "		y = 1\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	//FocusTree += "			army_experience = 10\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";
	//Home Focus
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = StrengthenHome" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_anschluss\r\n";
	FocusTree += "		text = \"Strengthen Home\"\r\n";
	FocusTree += "		prerequisite = { focus = EmpireGlory" + Home->getTag() + " }\r\n";
	FocusTree += "		mutually_exclusive = { focus = StrengthenColonies" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  30\r\n";
	FocusTree += "		y = 1\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	//FocusTree += "			army_experience = 10\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//Colonial Factories
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = ColonialInd" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_construct_civ_factory\r\n";
	FocusTree += "		text = \"Colonial Industry Buildup\"\r\n";
	FocusTree += "		prerequisite = { focus = StrengthenColonies" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  26\r\n";
	FocusTree += "		y = 2\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = arms_factory\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = no\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = industrial_complex\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = no\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = arms_factory\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = arms_factory\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = no\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = industrial_complex\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = no\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = arms_factory\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = arms_factory\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = no\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = industrial_complex\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = no\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = arms_factory\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//Colonial Highway
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = ColonialHwy" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_construct_infrastructure\r\n";
	FocusTree += "		text = \"Colonial Highway\"\r\n";
	FocusTree += "		prerequisite = { focus = ColonialInd" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  24\r\n";
	FocusTree += "		y = 3\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = infrastructure\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = no\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = infrastructure\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = no\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = infrastructure\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = infrastructure\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = no\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = infrastructure\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = no\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = infrastructure\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = infrastructure\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = no\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = infrastructure\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = no\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = infrastructure\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//improve resources
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = ResourceFac" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_oil_refinery\r\n";
	FocusTree += "		text = \"Improve Resource Factories\"\r\n";
	FocusTree += "		prerequisite = { focus = ColonialInd" + Home->getTag() + " }\r\n";
	FocusTree += "		mutually_exclusive = { focus = StrengthenColonies" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  26\r\n";
	FocusTree += "		y = 3\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			add_ideas = improved_resource_industry\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//establish colonial army
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = ColonialArmy" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
	FocusTree += "		text = \"Establish Colonial Army\"\r\n";
	FocusTree += "		prerequisite = { focus = StrengthenColonies" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  28\r\n";
	FocusTree += "		y = 2\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	//find out how to increase non core pop, or add cores
	//FocusTree += "			local_resources_factor = 0.2\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";
	string protectorateNFs = "";
	//establish protectorate
	if (ProtectorateNumber >= 1)
	{
		FocusTree += "focus = {\r\n";
		FocusTree += "		id = Protectorate" + Home->getTag() + Annexed1->getTag() + "\r\n";
		FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
		FocusTree += "		text = \"Establish Protectorate over " + Annexed1->getSourceCountry()->getName() + "\"\r\n";
		FocusTree += "		prerequisite = { focus = ColonialArmy" + Home->getTag() + " }\r\n";
		FocusTree += "		x = 28\r\n";
		FocusTree += "		y = 3\r\n";
		FocusTree += "		cost = 10\r\n";
		FocusTree += "		ai_will_do = {\r\n";
		FocusTree += "			factor = 5\r\n";
		FocusTree += "			modifier = {\r\n";
		FocusTree += "			factor = 0\r\n";
		FocusTree += "			strength_ratio = { tag = " + Annexed1->getTag() + " ratio < 1 }\r\n";
		FocusTree += "			}";
		FocusTree += "		}	\r\n";
		FocusTree += "		completion_reward = {\r\n";
		FocusTree += "			create_wargoal = {\r\n";
		FocusTree += "				type = puppet_wargoal_focus\r\n";
		FocusTree += "				target = " + Annexed1->getTag() + "\r\n";
		FocusTree += "			}";
		FocusTree += "		}\r\n";
		FocusTree += "	}\r\n";
		protectorateNFs += " Protectorate" + Home->getTag() + Annexed1->getTag();
	}
	if (ProtectorateNumber >= 2)
	{
		FocusTree += "focus = {\r\n";
		FocusTree += "		id = Protectorate" + Home->getTag() + Annexed2->getTag() + "\r\n";
		FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
		FocusTree += "		text = \"Establish Protectorate over " + Annexed2->getSourceCountry()->getName() + "\"\r\n";
		FocusTree += "		prerequisite = { focus = Protectorate" + Home->getTag() + Annexed1->getTag() + " }\r\n";
		FocusTree += "		x = 28\r\n";
		FocusTree += "		y = 4\r\n";
		FocusTree += "		cost = 10\r\n";
		FocusTree += "		ai_will_do = {\r\n";
		FocusTree += "			factor = 5\r\n";
		FocusTree += "			modifier = {\r\n";
		FocusTree += "			factor = 0\r\n";
		FocusTree += "			strength_ratio = { tag = " + Annexed2->getTag() + " ratio < 1 }\r\n";
		FocusTree += "			}";
		FocusTree += "		}	\r\n";
		FocusTree += "		completion_reward = {\r\n";
		FocusTree += "			create_wargoal = {\r\n";
		FocusTree += "				type = puppet_wargoal_focus\r\n";
		FocusTree += "				target = " + Annexed2->getTag() + "\r\n";
		FocusTree += "			}";
		FocusTree += "		}\r\n";
		FocusTree += "	}\r\n";
		protectorateNFs += " Protectorate" + Home->getTag() + Annexed2->getTag();
	}
	
	//Trade Empire
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = TradeEmpire" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_anschluss\r\n";
	FocusTree += "		text = \"Fund the "+Home->getSourceCountry()->getAdjective("english") +" Colonial Trade Corporation\"\r\n";
	FocusTree += "		prerequisite = { focus = ColonialHwy" + Home->getTag() + " focus = ResourceFac" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  25\r\n";
	FocusTree += "		y = 4\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			add_ideas = established_traders";
	FocusTree += "			set_country_flag = established_traders";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = infrastructure\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = no\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = infrastructure\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = no\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 2\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = dockyard\r\n";
	FocusTree += "					level = 2\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//Home Industry Buildup
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = IndHome" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_production\r\n";
	FocusTree += "		text = \"Fund Industrial Improvement\"\r\n";
	FocusTree += "		prerequisite = { focus = StrengthenHome" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  31\r\n";
	FocusTree += "		y = 2\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	//add tech
	//FocusTree += "			research_time_factor = -0.1\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//National Highway
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = NationalHwy" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_construct_infrastructure\r\n";
	FocusTree += "		text = \"National Highway\"\r\n";
	FocusTree += "		prerequisite = { focus = IndHome" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  30\r\n";
	FocusTree += "		y = 3\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = infrastructure\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = yes\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = infrastructure\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = yes\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = infrastructure\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = infrastructure\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = yes\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = infrastructure\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = yes\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = infrastructure\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = infrastructure\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = yes\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = infrastructure\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = yes\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = infrastructure\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//National College
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = NatCollege" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_anschluss\r\n";
	FocusTree += "		text = \"Establish National College\"\r\n";
	FocusTree += "		prerequisite = { focus = IndHome" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  32\r\n";
	FocusTree += "		y = 3\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			add_ideas = national_college\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//Improve Factories
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = MilitaryBuildup" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_construct_mil_factory\r\n";
	FocusTree += "		text = \"Military Buildup\"\r\n";
	FocusTree += "		prerequisite = { focus = NatCollege" + Home->getTag() + " }\r\n";
	FocusTree += "		prerequisite = { focus = NationalHwy" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  31\r\n";
	FocusTree += "		y = 4\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = arms_factory\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = yes\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = arms_factory\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = yes\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = arms_factory\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = arms_factory\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = yes\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = arms_factory\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = yes\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = arms_factory\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			random_owned_state = {\r\n";
	FocusTree += "				limit = {\r\n";
	FocusTree += "					free_building_slots = {\r\n";
	FocusTree += "						building = arms_factory\r\n";
	FocusTree += "						size > 0\r\n";
	FocusTree += "						include_locked = yes\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "					OR = {\r\n";
	FocusTree += "						is_in_home_area = yes\r\n";
	FocusTree += "						NOT = {\r\n";
	FocusTree += "							owner = {\r\n";
	FocusTree += "								any_owned_state = {\r\n";
	FocusTree += "									free_building_slots = {\r\n";
	FocusTree += "										building = arms_factory\r\n";
	FocusTree += "										size > 0\r\n";
	FocusTree += "										include_locked = yes\r\n";
	FocusTree += "									}\r\n";
	FocusTree += "									is_in_home_area = yes\r\n";
	FocusTree += "								}\r\n";
	FocusTree += "							}\r\n";
	FocusTree += "						}\r\n";
	FocusTree += "					}\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "				add_extra_state_shared_building_slots = 1\r\n";
	FocusTree += "				add_building_construction = {\r\n";
	FocusTree += "					type = arms_factory\r\n";
	FocusTree += "					level = 1\r\n";
	FocusTree += "					instant_build = yes\r\n";
	FocusTree += "				}\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//PrepBorder
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = PrepTheBorder" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_defence\r\n";
	FocusTree += "		text = \"Prepare the Border\"\r\n";
	FocusTree += "		prerequisite = { focus = StrengthenHome" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  34\r\n";
	FocusTree += "		y = 2\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			add_ideas = border_buildup\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//Promote Nationalistic Spirit
	FocusTree += "		focus = { \r\n";
	FocusTree += "		id = NatSpirit" + Home->getTag() + "\r\n";
	FocusTree += "		icon = GFX_goal_generic_political_pressure\r\n";
	FocusTree += "		text = \"Promote Nationalistic Spirit\"\r\n";
	FocusTree += "		prerequisite = { focus = PrepTheBorder" + Home->getTag() + " }\r\n";
	FocusTree += "		x =  34\r\n";
	FocusTree += "		y = 3\r\n";
	FocusTree += "		cost = 10\r\n";
	FocusTree += "		ai_will_do = {\r\n";
	FocusTree += "			factor = 1\r\n";
	FocusTree += "			modifier = {\r\n";
	FocusTree += "			}\r\n";
	FocusTree += "		}	\r\n";
	FocusTree += "		completion_reward = {\r\n";
	FocusTree += "			add_ideas = paramilitarism_focus\r\n";
	FocusTree += "		}\r\n";
	FocusTree += "	}";

	//ANNEX
	if (AnnexNumber >= 1)
	{
		FocusTree += "focus = {\r\n";
		FocusTree += "		id = Annex" + Home->getTag() + Annexed3->getTag() + "\r\n";
		FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
		FocusTree += "		text = \"Conquer " + Annexed3->getSourceCountry()->getName() + "\"\r\n";
		FocusTree += "		prerequisite = { focus = NatSpirit" + Home->getTag() + " }\r\n";
		FocusTree += "		x = 34\r\n";
		FocusTree += "		y = 4\r\n";
		FocusTree += "		cost = 10\r\n";
		FocusTree += "		ai_will_do = {\r\n";
		FocusTree += "			factor = 5\r\n";
		FocusTree += "			modifier = {\r\n";
		FocusTree += "			factor = 0\r\n";
		FocusTree += "			strength_ratio = { tag = " + Annexed3->getTag() + " ratio < 1 }\r\n";
		FocusTree += "			}";
		FocusTree += "		}	\r\n";
		FocusTree += "		completion_reward = {\r\n";
		FocusTree += "			create_wargoal = {\r\n";
		FocusTree += "				type = annex_everything\r\n";
		FocusTree += "				target = " + Annexed3->getTag() + "\r\n";
		FocusTree += "			}";
		FocusTree += "		}\r\n";
		FocusTree += "	}\r\n";
	}
	if (AnnexNumber >= 2)
	{
		FocusTree += "focus = {\r\n";
		FocusTree += "		id = Annex" + Home->getTag() + Annexed4->getTag() + "\r\n";
		FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
		FocusTree += "		text = \"Conquer " + Annexed4->getSourceCountry()->getName() + "\"\r\n";
		FocusTree += "		prerequisite = { focus = NatSpirit" + Home->getTag() + " }\r\n";
		FocusTree += "		x = 36\r\n";
		FocusTree += "		y = 4\r\n";
		FocusTree += "		cost = 10\r\n";
		FocusTree += "		ai_will_do = {\r\n";
		FocusTree += "			factor = 5\r\n";
		FocusTree += "			modifier = {\r\n";
		FocusTree += "			factor = 0\r\n";
		FocusTree += "			strength_ratio = { tag = " + Annexed4->getTag() + " ratio < 1 }\r\n";
		FocusTree += "			}";
		FocusTree += "		}	\r\n";
		FocusTree += "		completion_reward = {\r\n";
		FocusTree += "			create_wargoal = {\r\n";
		FocusTree += "				type = annex_everything\r\n";
		FocusTree += "				target = " + Annexed4->getTag() + "\r\n";
		FocusTree += "			}";
		FocusTree += "		}\r\n";
		FocusTree += "	}\r\n";
	}


	return FocusTree;
}
void HoI4World::fillProvinceNeighbors()
{
	std::ifstream file("adj.txt");
	std::string str;
	while (std::getline(file, str))
	{
		//	char output[100];
		//myReadFile >> output;
		vector<string> parts;
		stringstream ss(str); // Turn the string into a stream.
		string tok;
		char delimiter = ';';
		//crashes
		while (getline(ss, tok, delimiter))
			parts.push_back(tok);
		vector<int> provneighbors;
		for (unsigned int i = 5; i < parts.size(); i++)
		{
			//crashes
			int neighborprov = stoi(parts[i]);
			provneighbors.push_back(neighborprov);
		}
		provinceNeighbors.insert(make_pair(stoi(parts[0]), provneighbors));
	}
	file.close();
}
string HoI4World::genericFocusTreeCreator(HoI4Country* CreatingCountry)
{
	string s;
//DOES NOT INCLUDE LAST BRACKET!
	s += "focus_tree = { \r\n";
	s += "	id = german_focus\r\n";
	s += "	\r\n";
	s += "	country = {\r\n";
	s += "		factor = 0\r\n";
	s += "		\r\n";
	s += "		modifier = {\r\n";
	s += "			add = 10\r\n";
	s += "			tag = " + CreatingCountry->getTag() + "\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	default = no\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = army_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
	s += "		x = 1\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			army_experience = 5\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = land_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = land_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = equipment_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_small_arms\r\n";
	s += "		prerequisite = { focus = army_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 0\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = infantry_weapons_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = infantry_weapons\r\n";
	s += "				category = artillery\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = motorization_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_motorized\r\n";
	s += "		prerequisite = { focus = army_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		bypass = { has_tech = motorised_infantry }\r\n";
	s += "		x = 2\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = motorized_bonus\r\n";
	s += "				bonus = 0.75\r\n";
	s += "				technology = motorised_infantry\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = doctrine_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_doctrines\r\n";
	s += "		prerequisite = { focus = army_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 1\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			army_experience = 5\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = land_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = land_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = equipment_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_artillery\r\n";
	s += "		prerequisite = { focus = equipment_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 0\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = infantry_artillery_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = infantry_weapons\r\n";
	s += "				category = artillery\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = mechanization_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_build_tank\r\n";
	s += "		prerequisite = { focus = motorization_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 2\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = motorized_bonus\r\n";
	s += "				ahead_reduction = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = motorized_equipment\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = doctrine_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_doctrines\r\n";
	s += "		prerequisite = { focus = doctrine_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 1\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			army_experience = 5\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = land_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = land_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = equipment_effort_3" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_artillery2\r\n";
	s += "		prerequisite = { focus = equipment_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 0\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = infantry_artillery_bonus\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				category = infantry_weapons\r\n";
	s += "				category = artillery\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = armor_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_tanks\r\n";
	s += "		prerequisite = { focus = mechanization_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 2\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = armor_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				category = armor\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = special_forces" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_special_forces\r\n";
	s += "		prerequisite = { focus = equipment_effort_3" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = doctrine_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = armor_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 1\r\n";
	s += "		y = 6\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = special_forces_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = paratroopers\r\n";
	s += "				technology = paratroopers2\r\n";
	s += "				technology = marines\r\n";
	s += "				technology = marines2\r\n";
	s += "				technology = tech_mountaineers\r\n";
	s += "				technology = tech_mountaineers2\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = aviation_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_build_airforce\r\n";
	s += "		x = 5\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "			if = { limit = { has_country_flag = aviation_effort_AB }\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = air_base\r\n";
	s += "					level = 2\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}			\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						NOT = {\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = air_base\r\n";
	s += "								size > 1\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				capital_scope = {\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = fighter_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_fighter\r\n";
	s += "		prerequisite = { focus = aviation_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = bomber_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 4\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = fighter_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				technology = early_fighter\r\n";
	s += "				technology = fighter1\r\n";
	s += "				technology = fighter2\r\n";
	s += "				technology = fighter3\r\n";
	s += "				technology = heavy_fighter1\r\n";
	s += "				technology = heavy_fighter2\r\n";
	s += "				technology = heavy_fighter3\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = bomber_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_bomber\r\n";
	s += "		prerequisite = { focus = aviation_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = fighter_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 6\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = bomber_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				technology = strategic_bomber1\r\n";
	s += "				technology = strategic_bomber2\r\n";
	s += "				technology = strategic_bomber3\r\n";
	s += "				category = tactical_bomber\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = aviation_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_doctrine\r\n";
	s += "		prerequisite = { focus = bomber_focus focus = fighter_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 5\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "			if = { limit = { has_country_flag = aviation_effort_2_AB }\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = air_base\r\n";
	s += "					level = 2\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name =  air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		completion_reward = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						NOT = {\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = air_base\r\n";
	s += "								size > 1\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_2_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				capital_scope = {\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}				\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_2_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name =  air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = CAS_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_CAS\r\n";
	s += "		prerequisite = { focus = aviation_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = motorization_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 4\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = CAS_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				category = cas_bomber\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = rocket_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_rocketry\r\n";
	s += "		prerequisite = { focus = aviation_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = infrastructure_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 5\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = jet_rocket_bonus\r\n";
	s += "				ahead_reduction = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				category = rocketry\r\n";
	s += "				category = jet_technology\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.25\r\n";
	s += "				always = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = NAV_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_naval_bomber\r\n";
	s += "		prerequisite = { focus = aviation_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = flexible_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 6\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = nav_bomber_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				category = naval_bomber\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = naval_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_naval_dockyard\r\n";
	s += "		x = 9\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		available = {\r\n";
	s += "			any_state = {\r\n";
	s += "				is_coastal = yes\r\n";
	s += "				is_controlled_by = ROOT\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			navy_experience = 25\r\n";
	s += "			add_extra_state_shared_building_slots = 3\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = dockyard\r\n";
	s += "				level = 3\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		\r\n";
	s += "		completion_reward = {\r\n";
	s += "			navy_experience = 25\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					NOT = {\r\n";
	s += "						any_owned_state = {\r\n";
	s += "							dockyard > 0\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = dockyard\r\n";
	s += "								size > 2\r\n";
	s += "								include_locked = yes\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					any_owned_state = {\r\n";
	s += "						is_coastal = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						is_coastal = yes\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_extra_state_shared_building_slots = 3\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = dockyard\r\n";
	s += "						level = 3\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				set_country_flag = naval_effort_built\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					NOT = { has_country_flag = naval_effort_built }\r\n";
	s += "					any_owned_state = {\r\n";
	s += "						dockyard > 0\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						dockyard > 0\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_extra_state_shared_building_slots = 3\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = dockyard\r\n";
	s += "						level = 3\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				set_country_flag = naval_effort_built\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					NOT = { has_country_flag = naval_effort_built }\r\n";
	s += "					NOT = {\r\n";
	s += "						any_owned_state = {\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = dockyard\r\n";
	s += "								size > 2\r\n";
	s += "								include_locked = yes\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						controller = { tag = ROOT }\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_extra_state_shared_building_slots = 3\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = dockyard\r\n";
	s += "						level = 3\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = flexible_navy" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_build_navy\r\n";
	s += "		prerequisite = { focus = naval_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = large_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 8\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = sub_op_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				technology = convoy_interdiction_ti\r\n";
	s += "				technology = unrestricted_submarine_warfare\r\n";
	s += "				technology = wolfpacks\r\n";
	s += "				technology = advanced_submarine_warfare\r\n";
	s += "				technology = combined_operations_raiding\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = large_navy" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_doctrines_tactics\r\n";
	s += "		prerequisite = { focus = naval_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = flexible_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = fleet_in_being_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				category = fleet_in_being_tree\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = submarine_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_submarine\r\n";
	s += "		prerequisite = { focus = flexible_navy focus = large_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 8\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = ss_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = early_submarine\r\n";
	s += "				technology = basic_submarine\r\n";
	s += "				technology = improved_submarine\r\n";
	s += "				technology = advanced_submarine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = cruiser_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_cruiser\r\n";
	s += "		prerequisite = { focus = large_navy focus = flexible_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = cr_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = improved_light_cruiser\r\n";
	s += "				technology = advanced_light_cruiser\r\n";
	s += "				technology = improved_heavy_cruiser\r\n";
	s += "				technology = advanced_heavy_cruiser\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = destroyer_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_wolf_pack\r\n";
	s += "		prerequisite = { focus = submarine_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 8\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = dd_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = early_destroyer\r\n";
	s += "				technology = basic_destroyer\r\n";
	s += "				technology = improved_destroyer\r\n";
	s += "				technology = advanced_destroyer\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = capital_ships_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_battleship\r\n";
	s += "		prerequisite = { focus = cruiser_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			navy_experience = 25\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = capital_ships_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = basic_battlecruiser\r\n";
	s += "				technology = basic_battleship\r\n";
	s += "				technology = improved_battleship\r\n";
	s += "				technology = advanced_battleship\r\n";
	s += "				technology = heavy_battleship\r\n";
	s += "				technology = heavy_battleship2\r\n";
	s += "				technology = early_carrier\r\n";
	s += "				technology = basic_carrier\r\n";
	s += "				technology = improved_carrier\r\n";
	s += "				technology = advanced_carrier\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = industrial_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_production\r\n";
	s += "		x = 13\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = industrial_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = industry\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 3\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				date < 1939.1.1\r\n";
	s += "				OR = { \r\n";
	s += "					# we dont want chinese minors to go crazy on slots early since they get eaten\r\n";
	s += "					tag = GXC \r\n";
	s += "					tag = YUN\r\n";
	s += "					tag = SHX\r\n";
	s += "					tag = XSM\r\n";
	s += "					tag = BEL\r\n";
	s += "					tag = LUX\r\n";
	s += "					tag = HOL\r\n";
	s += "					tag = DEN\r\n";
	s += "\r\n";
	s += "					# we also dont want tiny nations to go crazy with slots right away\r\n";
	s += "					num_of_controlled_states < 2\r\n";
	s += "				}				\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = construction_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_civ_factory\r\n";
	s += "		prerequisite = { focus = industrial_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = construction_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}					\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = industrial_complex\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = industrial_complex\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = industrial_complex\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = production_effort"+CreatingCountry->getTag()+"\r\n";
	s += "		icon = GFX_goal_generic_construct_mil_factory\r\n";
	s += "		prerequisite = { focus = industrial_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2			\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = production_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = arms_factory\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = arms_factory\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = arms_factory\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = construction_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_civ_factory\r\n";
	s += "		prerequisite = { focus = construction_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = construction_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = industrial_complex\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = industrial_complex\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = industrial_complex\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = production_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_mil_factory\r\n";
	s += "		prerequisite = { focus = production_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = production_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = arms_factory\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = arms_factory\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = arms_factory\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = infrastructure_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_infrastructure\r\n";
	s += "		prerequisite = { focus = construction_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = infrastructure_effort_tt\r\n";
	s += "				all_owned_state = {			\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = production_effort_3" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_mil_factory\r\n";
	s += "		prerequisite = { focus = production_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = production_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}					\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = arms_factory\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = arms_factory\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = arms_factory\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = infrastructure_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_infrastructure\r\n";
	s += "		prerequisite = { focus = infrastructure_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = infrastructure_effort_tt\r\n";
	s += "				all_owned_state = {			\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = construction_effort_3" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_civ_factory\r\n";
	s += "		prerequisite = { focus = infrastructure_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = construction_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size < 2\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 2\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = industrial_complex\r\n";
	s += "				level = 2\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size > 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = industrial_complex\r\n";
	s += "										size > 1\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 2\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = industrial_complex\r\n";
	s += "					level = 2\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = nuclear_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_wonderweapons\r\n";
	s += "		prerequisite = { focus = infrastructure_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = nuclear_bonus\r\n";
	s += "				ahead_reduction = 0.5\r\n";
	s += "				category = nuclear\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.25\r\n";
	s += "				always = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = extra_tech_slot" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_research\r\n";
	s += "		prerequisite = { focus = infrastructure_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_research_slot = 1\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	focus = {\r\n";
	s += "		id = extra_tech_slot_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_research\r\n";
	s += "		prerequisite = { focus = extra_tech_slot" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			num_of_factories > 50\r\n";
	s += "		}\r\n";
	s += "		cancel_if_invalid = no\r\n";
	s += "		continue_if_invalid = yes\r\n";
	s += "		x = 12\r\n";
	s += "		y = 6\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_research_slot = 1\r\n";
	s += "		}\r\n";
	s += "	}	\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = secret_weapons" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_secret_weapon\r\n";
	s += "		prerequisite = { focus = infrastructure_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = secret_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 4\r\n";
	s += "				category = electronics\r\n";
	s += "				category = nuclear\r\n";
	s += "				category = rocketry\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.25\r\n";
	s += "				always = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = political_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_demand_territory\r\n";
	s += "		x = 19\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_political_power = 120\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = collectivist_ethos" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_national_unity #icon = GFX_goal_tripartite_pact\r\n";
	s += "		prerequisite = { focus = political_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = liberty_ethos" + CreatingCountry->getTag() + "}\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = fascism\r\n";
	s += "				has_government = communism\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 18\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 5\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				OR = {\r\n";
	s += "					is_historical_focus_on = yes\r\n";
	s += "					has_idea = neutrality_idea\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = collectivist_ethos_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = nationalism_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_support_fascism #icon = GFX_goal_tripartite_pact\r\n";
	s += "		prerequisite = { focus = collectivist_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = internationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = fascism\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 16\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 5\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 2\r\n";
	s += "				any_neighbor_country = {\r\n";
	s += "					is_major = yes\r\n";
	s += "					has_government = fascism\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = nationalism\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	focus = {\r\n";
	s += "		id = internationalism_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_support_communism #icon = GFX_goal_tripartite_pact\r\n";
	s += "		prerequisite = { focus = collectivist_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = nationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = communism\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 18\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 5\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 2\r\n";
	s += "				any_neighbor_country = {\r\n";
	s += "					is_major = yes\r\n";
	s += "					has_government = communism\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = internationalism\r\n";
	s += "		}\r\n";
	s += "	}	\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = liberty_ethos" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_support_democracy\r\n";
	s += "		prerequisite = { focus = political_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = collectivist_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = democratic\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 20\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 95\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.1\r\n";
	s += "				any_neighbor_country = {\r\n";
	s += "					is_major = yes\r\n";
	s += "					OR = {\r\n";
	s += "						has_government = communism\r\n";
	s += "						has_government = fascism\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				NOT = {\r\n";
	s += "					any_neighbor_country = {\r\n";
	s += "						is_major = yes\r\n";
	s += "						has_government = democratic\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = liberty_ethos_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = militarism" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_political_pressure\r\n";
	s += "		prerequisite = { focus = nationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 16\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_idea = neutrality_idea }\r\n";
	s += "				remove_ideas = neutrality_idea\r\n";
	s += "			}			\r\n";
	s += "			add_ideas = militarism_focus\r\n";
	s += "			army_experience = 20\r\n";
	s += "			set_rule = { can_send_volunteers = yes }\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = political_correctness" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_dangerous_deal\r\n";
	s += "		prerequisite = { focus = internationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 18\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_idea = neutrality_idea }\r\n";
	s += "				remove_ideas = neutrality_idea\r\n";
	s += "			}		\r\n";
	s += "			add_political_power = 200\r\n";
	s += "			add_ideas = idea_political_correctness\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = neutrality_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_neutrality_focus\r\n";
	s += "		prerequisite = { focus = liberty_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = interventionism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 20\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { NOT = { has_idea = neutrality_idea } }\r\n";
	s += "				add_ideas = neutrality_idea\r\n";
	s += "			}\r\n";
	s += "			add_political_power = 150\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = interventionism_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_political_pressure\r\n";
	s += "		prerequisite = { focus = liberty_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = neutrality_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 22\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				has_idea = neutrality_idea\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_idea = neutrality_idea }\r\n";
	s += "				remove_ideas = neutrality_idea\r\n";
	s += "			}	\r\n";
	s += "			set_rule = { can_send_volunteers = yes }\r\n";
	s += "			add_political_power = 150\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = military_youth" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_more_territorial_claims\r\n";
	s += "		prerequisite = { focus = militarism" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 16\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = military_youth_focus\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = fascism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = fascism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = communism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = communism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = deterrence" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_defence\r\n";
	s += "		prerequisite = { focus = neutrality_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 20\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = deterrence\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = volunteer_corps" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
	s += "		prerequisite = { focus = interventionism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 22\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = volunteer_corps_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = paramilitarism" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_military_sphere\r\n";
	s += "		prerequisite = { focus = military_youth" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 16\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = paramilitarism_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = indoctrination_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_propaganda\r\n";
	s += "		prerequisite = { focus = political_correctness" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 18\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = indoctrination_focus\r\n";
	s += "			add_political_power = 150\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = foreign_expeditions" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_more_territorial_claims\r\n";
	s += "		prerequisite = { focus = volunteer_corps" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 22\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = foreign_expeditions_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = why_we_fight" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_propaganda\r\n";
	s += "		prerequisite = { focus = foreign_expeditions focus = deterrence" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = { \r\n";
	s += "			OR = { \r\n";
	s += "				threat > 0.75 \r\n";
	s += "				has_defensive_war = yes \r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		continue_if_invalid = yes\r\n";
	s += "		\r\n";
	s += "		x = 20\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { NOT = { has_idea = neutrality_idea } }\r\n";
	s += "				set_rule = { can_create_factions = yes }\r\n";
	s += "			}\r\n";
	s += "			add_ideas = why_we_fight_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = political_commissars" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_forceful_treaty\r\n";
	s += "		prerequisite = { focus = indoctrination_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "		}\r\n";
	s += "		x = 18\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = political_commissars_focus\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = fascism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = fascism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = communism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = communism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_political_power = 200\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = ideological_fanaticism" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_demand_territory\r\n";
	s += "		prerequisite = { focus = paramilitarism" + CreatingCountry->getTag() + " focus = political_commissars" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 17\r\n";
	s += "		y = 6\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = ideological_fanaticism_focus\r\n";
	s += "			set_rule = {\r\n";
	s += "				can_create_factions = yes\r\n";
	s += "			}\r\n";
	s += "			hidden_effect = {\r\n";
	s += "				set_rule = { can_use_kamikaze_pilots = yes }\r\n";
	s += "			}\r\n";
	s += "			custom_effect_tooltip = kamikaze_focus_tooltip\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	focus = {\r\n";
	s += "		id = technology_sharing" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_scientific_exchange\r\n";
	s += "		prerequisite = { focus = ideological_fanaticism" + CreatingCountry->getTag() + " focus = why_we_fight" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			has_war = yes\r\n";
	s += "			is_in_faction = yes\r\n";
	s += "			OR = {\r\n";
	s += "				num_of_factories > 50\r\n";
	s += "				any_country = {\r\n";
	s += "					is_in_faction_with = ROOT\r\n";
	s += "					num_of_factories > 50\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "		x = 19\r\n";
	s += "		y = 7\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					original_research_slots < 3\r\n";
	s += "				}\r\n";
	s += "				add_research_slot = 1\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					original_research_slots > 2\r\n";
	s += "				}\r\n";
	s += "				add_tech_bonus = {\r\n";
	s += "					name = electronics_bonus\r\n";
	s += "					bonus = 0.5\r\n";
	s += "					uses = 1\r\n";
	s += "					category = electronics\r\n";
	s += "				}\r\n";
	s += "				add_tech_bonus = {\r\n";
	s += "					name = industrial_bonus\r\n";
	s += "					bonus = 0.5\r\n";
	s += "					uses = 1\r\n";
	s += "					category = industry\r\n";
	s += "				}	\r\n";
	s += "				add_tech_bonus = {\r\n";
	s += "					name = infantry_weapons_bonus\r\n";
	s += "					bonus = 0.5\r\n";
	s += "					uses = 1\r\n";
	s += "					category = infantry_weapons\r\n";
	s += "					category = artillery\r\n";
	s += "				}				\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "	}	\r\n";
	s += "\r\n";
	return s;
}
void HoI4World::fillCountryIC()
{

	for (auto country : countries)
	{
		vector<int> countrystates;
		int countryICMIL = 0;
		int countryICCIV = 0;
		for (auto state : states->getStates())
		{
			if (state.second->getOwner() == country.second->getTag())
			{
				countryICCIV += state.second->getCivFactories();
				countryICMIL += state.second->getMilFactories();
				countrystates.push_back(state.first);
			}
		}
		countriesStates.insert(make_pair(country.second->getTag(), countrystates));
		countriesICMIL.insert(make_pair(country.second->getTag(), countryICMIL));
		countriesICCIV.insert(make_pair(country.second->getTag(), countryICCIV));
	}
}
double HoI4World::getStrengthOverTime(HoI4Country* Country, double years)
{
	return Country->getArmyStrength() + countriesICMIL.find(Country->getTag())->second * 3 * 365 *years + countriesICCIV.find(Country->getTag())->second*.469*.5 /*.469 is milfac per year, .5 since half are used by consumer goods*/*3*365;
}
double HoI4World::getInitialStrength(HoI4Country* Country)
{
	return Country->getArmyStrength();
}
double HoI4World::getAddedStrength(HoI4Country* Country, double years)
{
	return countriesICMIL.find(Country->getTag())->second * 3 * 365 * years + countriesICCIV.find(Country->getTag())->second*.469*.5 /*.469 is milfac per year, .5 since half are used by consumer goods*/ * 3 * 365;
}
void HoI4World::outputRelations()
{
	string opinion_modifiers;
	for (auto country : countries)
	{
		string countryrelation;
		for (auto country2 : countries)
		{
			if (country.second->getTag() != country2.second->getTag() && country.second->getRelations(country2.second->getTag()) != NULL &&country2.second->getRelations(country2.second->getTag()) != NULL)
			{
				opinion_modifiers += country.second->getTag() + "_" + country2.second->getTag() + " = {\r\n\tvalue = " + to_string(country.second->getRelations(country2.second->getTag())->getRelations()*1.5) + "\r\n}\r\n";
				countryrelation += "add_opinion_modifier = { target = " + country2.second->getTag() + " modifier = " + country.second->getTag() + "_" + country2.second->getTag() + "}\r\n";
			}
		}
		country.second->setRelations(countryrelation);
	}
	std::string outputcommon = "Output/" + Configuration::getOutputName() + "/common";
	if (!Utils::TryCreateFolder(outputcommon))
	{
		return;
	}
	
	std::string outputopinionfolder = "Output/" + Configuration::getOutputName() + "/common/opinion_modifiers";
	if (!Utils::TryCreateFolder(outputopinionfolder))
	{
		return;
	}
	//Utils::copyFolder("NeededFiles/defines", "Output/" + Configuration::getOutputName() + "/common");
	string filename("Output/" + Configuration::getOutputName() + "/common/opinion_modifiers/01_opinion_modifiers.txt");
	ofstream out;
	out.open(filename);
	{
		out << "opinion_modifiers = {\r\n";
		out << opinion_modifiers;
		out << "}\r\n";
	}
	out.close();
}
void HoI4World::thatsgermanWarCreator(const V2World &sourceWorld, const CountryMapping& countryMap)
{

	//FIX ALL FIXMES AND ADD CONQUEST GOAL
	//MAKE SURE THIS WORKS
	//IMPROVE
	//MAKE ARMY STRENGTH CALCS MORE ACCURATE!!
	LOG(LogLevel::Info) << "Filling Map Information";
	fillProvinces();
	fillCountryIC();
	fillCountryProvinces();
	LOG(LogLevel::Info) << "Filling province neighbors";
	fillProvinceNeighbors();
	LOG(LogLevel::Info) << "Creating Factions";
	Factions = CreateFactions(sourceWorld, countryMap);
	//outputting the country and factions
	for (auto country : countries)
	{
		int i = 1;
		string FactionName;
		for (auto faction : Factions)
		{
			if (country.second->getTag() == faction.front()->getTag())
			{
				//wtf does this do? idk
				FactionName = to_string(i++);
			}
		}
		country.second->output(states->getStates(), Factions, FactionName);
	}
	bool fascismrelevant = false;
	bool communismrelevant = false;
	//output folders
	string NFpath = "Output/" + Configuration::getOutputName() + "/common/national_focus";
	if (!Utils::TryCreateFolder(NFpath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common/national_focus\"";
		exit(-1);
	}
	string eventpath = "Output/" + Configuration::getOutputName() + "/events";
	if (!Utils::TryCreateFolder(eventpath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/events\"";
		exit(-1);
	}
	//Files To show results
	string filename("Output/AI.txt");
	ofstream out;
	//getting total strength of all factions
	double WorldStrength = 0;
	out.open(filename);
	{
		for (auto Faction : Factions)
		{
			WorldStrength += GetFactionStrength(Faction);
		}
		out << WorldStrength << endl;
		//check relevancies
		for (auto Faction : Factions)
		{
			//this might need to change to add factions together
			HoI4Country* Leader = GetFactionLeader(Faction);
			if (Leader->getGovernment() == "absolute_monarchy" || Leader->getGovernment() == "fascism")
				if (GetFactionStrength(Faction) > WorldStrength*0.1)
					fascismrelevant = true;

			if (Leader->getGovernment() == "communism" || Leader->getGovernment() == "syndicalism")
				if (GetFactionStrength(Faction) > WorldStrength*0.1)
					communismrelevant = true;

		}
		if (fascismrelevant)
			out << "Fascism is Relevant" << endl;
		if (communismrelevant)
			out << "Communist is Relevant" << endl;
		out << endl;
		//time to do events for coms and fascs if they are relevant
		LOG(LogLevel::Info) << "Calculating Fasc/Com AI";
		for (auto Faction : Factions)
		{
			HoI4Country* Leader = GetFactionLeader(Faction);
			if ((Leader->getGovernment() == "fascism") && fascismrelevant)
			{
				LOG(LogLevel::Info) << "Calculating AI for " + Leader->getSourceCountry()->getName();
				out << Leader->getSourceCountry()->getName() << endl;
				//too many lists, need to clean up
				vector<HoI4Country*> Targets;
				vector<HoI4Country*> Anchluss;
				vector<HoI4Country*> Sudaten;
				vector<HoI4Country*> EqualTargets;
				vector<HoI4Country*> DifficultTargets;
				//getting country provinces and its neighbors
				vector<int> leaderProvs = getCountryProvinces(Leader);
				map<string, HoI4Country*> AllNeighbors = findNeighbors(leaderProvs, Leader);
				map<string, HoI4Country*> CloseNeighbors;
				//gets neighbors that are actually close to you
				for each (auto neigh in AllNeighbors)
				{
					if (neigh.second->getCapitalProv() != 0)
					{
						//IMPROVE
						//need to get further neighbors, as well as countries without capital in an area
						double distance = GetDistance(Leader, neigh.second);
						if (distance <= 500)
							CloseNeighbors.insert(neigh);
					}
				}

				set<string> Allies = Leader->getAllies();
				//should add method to look for cores you dont own
				//should add method to look for more allies

				//lets look for weak neighbors
				LOG(LogLevel::Info) << "Doing Neighbor calcs for " + Leader->getSourceCountry()->getName();
				for (auto neigh : CloseNeighbors)
				{
					//lets check to see if they are not our ally and not a great country
					if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !checkIfGreatCountry(neigh.second, sourceWorld, countryMap))
					{
						volatile double enemystrength = getStrengthOverTime(neigh.second, 1.5);
						volatile double mystrength = getStrengthOverTime(Leader, 1.5);
						//lets see their strength is at least < 20%
						if (getStrengthOverTime(neigh.second,1.5) < getStrengthOverTime(Leader,1.5)*0.2 && findFaction(neigh.second).size() == 1)
						{
							//they are very weak
							Anchluss.push_back(neigh.second);
						}
						//if not, lets see their strength is at least < 60%
						else if (getStrengthOverTime(neigh.second, 1.5) < getStrengthOverTime(Leader, 1)*0.6 && getStrengthOverTime(neigh.second, 1) > getStrengthOverTime(Leader, 1)*0.2 && findFaction(neigh.second).size() == 1)
						{
							//they are weak and we can get 1 of these countries in sudaten deal
							Sudaten.push_back(neigh.second);
						}
						//if not, lets see their strength is at least = to ours%
						else if (getStrengthOverTime(neigh.second, 1) < getStrengthOverTime(Leader, 1))
						{
							//EqualTargets.push_back(neigh);
							EqualTargets.push_back(neigh.second);
						}
						//if not, lets see their strength is at least < 120%
						else if (getStrengthOverTime(neigh.second, 1) < getStrengthOverTime(Leader, 1)*1.2)
						{
							//StrongerTargets.push_back(neigh);
							DifficultTargets.push_back(neigh.second);

						}
						
					}
				}
				//string that contains all events
				string Events;
				string s;
				map<string, vector<HoI4Country*>> TargetMap;
				vector<HoI4Country*> anchlussnan;
				vector<HoI4Country*> sudatennan;
				vector<HoI4Country*> nan;
				vector<HoI4Country*> fn;
				vector<HoI4Country*> man;
				vector<HoI4Country*> coup;
				int EventNumber = 0;
				//x is used for the x position of our last NF, so we can place them correctly
				vector<int> takenSpots;
				vector<int> takenSpotsy;
				int x = 22;
				takenSpots.push_back(x);
				//look through every anchluss and see its difficulty
				for (auto target : Anchluss)
				{
					string type;
					//outputs are for HowToTakeLand()
					//noactionneeded -  Can take target without any help
					//factionneeded - can take target and faction with attackers faction helping
					//morealliesneeded - can take target with more allies, comes with "newallies" in map
					//coup - cant take over, need to coup
					type = HowToTakeLand(target, Leader, 1.5);
					out << type + " " + target->getSourceCountry()->getName() << endl;
					if (type == "noactionneeded")
					{
						//too many vectors, need to clean up
						nan.push_back(target);
						anchlussnan.push_back(target);
					}
				}
				//gives us generic focus tree start
				string FocusTree = genericFocusTreeCreator(Leader);
				if (nan.size() >= 1)
				{
					//if it can easily take these targets as they are not in an alliance, you can get annexation event
						if (nan.size() == 1)
						{
							x = 24;
							takenSpots.push_back(x);
						}
						if (nan.size() >= 2)
						{
							x = 25;
							takenSpots.push_back(x);
						}
						takenSpotsy.push_back(2);
						//Focus to increase fascist support and prereq for anschluss
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = The_third_way" + Leader->getTag() + "\r\n";
						FocusTree += "		icon = GFX_goal_support_fascism\r\n";
						FocusTree += "		text = \"The Third Way!\"\r\n";
						FocusTree += "		x = " + to_string(x) + "\r\n";
						FocusTree += "		y = 0\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						//FIXME 
						//Need to get Drift Defense to work
						//FocusTree += "			drift_defence_factor = 0.5\r\n";
						FocusTree += "			add_ideas = fascist_influence\r\n";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";

						//Focus to increase army support
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = mil_march" + Leader->getTag() + "\r\n";
						FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
						FocusTree += "		text = \"Establish Military March Day\"\r\n";
						FocusTree += "		prerequisite = { focus = The_third_way" + Leader->getTag() + " }\r\n";
						FocusTree += "		x = " + to_string(x) + "\r\n";
						FocusTree += "		y = 1\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						FocusTree += "			army_experience = 20\r\n";
						FocusTree += "		add_tech_bonus = { \r\n";
						FocusTree += "				bonus = 0.5\r\n";
						FocusTree += "				uses = 2\r\n";
						FocusTree += "				category = land_doctrine\r\n";
						FocusTree += "			}";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";
						Events += "add_namespace = " + Leader->getTag() + "annex\r\n";
						
						for (unsigned int i = 0; i < 2; i++)
						{
							int start = 0;
							if (nan.size() >= 2)
								start = -1;
							if (i < nan.size())
							{
								//int x = i * 3;
								string annexername = Leader->getSourceCountry()->getName();
								string annexedname = nan[i]->getSourceCountry()->getName();
								//for random date
								int v1 = rand() % 5 + 1;
								int v2 = rand() % 5 + 1;
								//focus for anschluss
								FocusTree += "		focus = { \r\n";
								FocusTree += "		id = "+Leader->getTag() +"_anschluss_"+nan[i]->getTag()+"\r\n";
								FocusTree += "		icon = GFX_goal_anschluss\r\n";
								FocusTree += "		text = \"Union with "+annexedname+"\"\r\n";
								FocusTree += "		prerequisite = { focus = mil_march" + Leader->getTag() + " }\r\n";
								FocusTree += "		available = {\r\n";
								FocusTree += "			is_puppet = no\r\n";
								FocusTree += "		    date > 1937." + to_string(v1+5) + "." + to_string(v2+5) + "\r\n";
								FocusTree += "		}\r\n";
								FocusTree += "		\r\n";
								FocusTree += "		x = " + to_string(x + i*2 + start) + "\r\n";
								FocusTree += "		y = 2\r\n";
								FocusTree += "		cost = 10\r\n";
								FocusTree += "		ai_will_do = {\r\n";
								FocusTree += "			factor = 10\r\n";
								FocusTree += "			modifier = {\r\n";
								FocusTree += "				factor = 0\r\n";
								FocusTree += "				date < 1937.6.6\r\n";
								FocusTree += "			}\r\n";
								FocusTree += "		}	\r\n";
								FocusTree += "		completion_reward = {\r\n";
								FocusTree += "			army_experience = 10\r\n";
								FocusTree += "			if = {\r\n";
								FocusTree += "				limit = {\r\n";
								FocusTree += "					country_exists = " + nan[i]->getTag() + "\r\n";
								FocusTree += "				}\r\n";
								FocusTree += "				" + nan[i]->getTag() + " = {\r\n";
								FocusTree += "					country_event = " + Leader->getTag() + "annex."+to_string(EventNumber)+"\r\n";
								FocusTree += "				}\r\n";
								FocusTree += "			}\r\n";
								FocusTree += "		}\r\n";
								FocusTree += "	}";

								//events
								Events += createAnnexEvent(Leader, nan[i], EventNumber);
								EventNumber += 3;
							}
						}
					nan.clear();

				}
				for (auto target : Sudaten)
				{
					string type;
					//outputs are
					//noactionneeded -  Can take target without any help
					//factionneeded - can take target and faction with attackers faction helping
					//morealliesneeded - can take target with more allies, comes with "newallies" in map
					//coup - cant take over, need to coup
					type = HowToTakeLand(target, Leader, 2.5);
					out << type + " " + target->getSourceCountry()->getName() << endl;
					if (type == "noactionneeded")
					{
						nan.push_back(target);
					}
				}
				if (nan.size() >= 1)
				{
					//if it can easily take these targets as they are not in an alliance, you can get annexation event
					
					//Focus to increase empire size more
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Expand_the_Reich" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_political_pressure\r\n";//something about claiming land
					FocusTree += "		text = \"Expand the Reich\"\r\n";
					if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
					{
						//if there are anschlusses, make this event require at least 1 anschluss, else, its the start of a tree
						FocusTree += "		prerequisite = { ";
						for (unsigned int i = 0; i < 2; i++)
						{
							if (i < anchlussnan.size())
							{
								FocusTree += " focus = " + Leader->getTag() + "_anschluss_" + anchlussnan[i]->getTag() + " ";
							}
						}
						FocusTree += "\r\n }\r\n";
						FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
						FocusTree += "		y = 3\r\n";
						takenSpotsy.push_back(5);
					}
					else
					{
						//figuring out position
						int x = takenSpots.back();
						takenSpots.push_back(x);
						if (nan.size() == 1)
						{
							x += 2;
							takenSpots.push_back(x);
						}
						if (nan.size() >= 2)
						{
							x += 3;
							takenSpots.push_back(x);
						}
						FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
						FocusTree += "		y = 0\r\n";
						takenSpotsy.push_back(2);
					}
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "			add_named_threat = { threat = 2 name = \"Fascist Expansion\" }\r\n";//give some claims or cores
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
					for (unsigned int i = 0; i < 1; i++)
					{
						if (i < nan.size())
						{
							int x = i * 3;
							string annexername = Leader->getSourceCountry()->getName();
							string annexedname = nan[i]->getSourceCountry()->getName();
							int v1 = rand() % 8 + 1;
							int v2 = rand() % 8 + 1;
							//focus for sudaten
							FocusTree += "		focus = { \r\n";
							FocusTree += "		id = " + Leader->getTag() + "_sudaten_" + nan[i]->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_anschluss\r\n";
							FocusTree += "		text = \"Demand Territory from " + annexedname + "\"\r\n";
							FocusTree += "		prerequisite = { focus = Expand_the_Reich" + Leader->getTag() + " }\r\n";
							FocusTree += "		available = {\r\n";
							FocusTree += "			is_puppet = no\r\n";
							FocusTree += "		    date > 1938." + to_string(v1) + "." + to_string(v2) + "\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "		\r\n";
							if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
							{
								//figuring out position again
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 4\r\n";
							}
							else
							{
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 1\r\n";
							}
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 10\r\n";
							FocusTree += "			modifier = {\r\n";
							FocusTree += "				factor = 0\r\n";
							FocusTree += "				date < 1937.6.6\r\n";
							FocusTree += "			}\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			army_experience = 10\r\n";
							FocusTree += "			if = {\r\n";
							FocusTree += "				limit = {\r\n";
							FocusTree += "					country_exists = " + nan[i]->getTag() + "\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				" + nan[i]->getTag() + " = {\r\n";
							FocusTree += "					country_event = " + Leader->getTag() + "sudaten." + to_string(EventNumber) + "\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "			}\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}";

							//FINISH HIM
							FocusTree += "		focus = { \r\n";
							FocusTree += "		id = " + Leader->getTag() + "_finish_" + nan[i]->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_territory_or_war\r\n";
							FocusTree += "		text = \"Fate of " + annexedname + "\"\r\n";
							FocusTree += "		prerequisite = { focus =  " + Leader->getTag() + "_sudaten_" + nan[i]->getTag() + " }\r\n";
							FocusTree += "		available = {\r\n";
							FocusTree += "			is_puppet = no\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "		\r\n";
							if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
							{
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 5\r\n";
							}
							else
							{
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 2\r\n";
							}
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 10\r\n";
							FocusTree += "			modifier = {\r\n";
							FocusTree += "				factor = 0\r\n";
							FocusTree += "				date < 1937.6.6\r\n";
							FocusTree += "			}\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "		create_wargoal = {\r\n";
							FocusTree += "				type = annex_everything\r\n";
							FocusTree += "			target = " + nan[i]->getTag() + "\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}";

							//events
							//find neighboring states to take in sudaten deal
							vector<int> demandedstates;
							for (auto leaderprov : leaderProvs)
							{
								vector<int> thisprovNeighbors = provinceNeighbors.find(leaderprov)->second;
								for (int prov : thisprovNeighbors)
								{
									if (provincemap.find(prov) == provincemap.end())
									{
									}
									else
									{
										vector<string> stuff = provincemap.find(prov)->second;
										if (stuff.size() >= 1)
										{
											int statenumber = stoi(stuff.front());
											//string ownertag = "";
											auto ownertag = provincemap.find(prov);
											if (ownertag != provincemap.end())
											{
												vector<string> tags = ownertag->second;
												if (tags.size() >= 1)
												{
													if (tags[1] == nan[i]->getTag())
													{
									
															/* v does not contain x */
															demandedstates.push_back(statenumber);
														
													}
												}
											}
										}
									}
								}
							}
							Events += "add_namespace = " + Leader->getTag() + "sudaten\r\n";
							Events += createSudatenEvent(Leader, nan[0], EventNumber, demandedstates);
							EventNumber += 3;
						}
					}
					nan.clear();

				}
				//events for allies
				vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);
				for each (auto newally in newAllies)
				{
					findFaction(Leader).push_back(newally);
					out << "added Ally " + newally->getSourceCountry()->getName() + " with init strength of " + to_string(getInitialStrength(newally)) + " and a yearly strength of " + to_string(getAddedStrength(newally, 1)) + "\r\n";
 				}
				if (newAllies.size() > 0)
				{
					//Focus to call summit, maybe have events from summit
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Fas_Summit" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Call for the Fascist Summit\"\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back()+4) + "\r\n";
					FocusTree += "		y = 0\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 0\r\n";
					FocusTree += "			modifier = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "			date > 1938.1.1\r\n";
					FocusTree += "			}";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					//FIXME
					//FocusTree += "			opinion_gain_monthly_factor = 1.0 \r\n";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				for (unsigned int i = 0; i < newAllies.size(); i++)
				{
					int displacement = 0;
					if (newAllies.size() == 2)
						displacement = -1;
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Alliance_"+newAllies[i]->getTag() + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Alliance with "+newAllies[i]->getSourceCountry()->getName()+"\"\r\n";
					FocusTree += "		prerequisite = { focus =  Fas_Summit" + Leader->getTag() + " }\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 4 + i*2 + displacement) + "\r\n";
					FocusTree += "		y = 1\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 10\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		bypass = { \r\n";
					FocusTree += "			\r\n";
					FocusTree += "			OR = {\r\n";
					FocusTree += "				is_in_faction_with = "+newAllies[i]->getTag()+"\r\n";
					FocusTree += "				has_war_with = " + newAllies[i]->getTag() + "\r\n";
					FocusTree += "				NOT = { country_exists = " + newAllies[i]->getTag() + " }\r\n";
					FocusTree += "			}\r\n";
					FocusTree += "		}\r\n";
					
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "		" + newAllies[i]->getTag() + " = {\r\n";
					FocusTree += "			add_opinion_modifier = { target = " + Leader->getTag() + " modifier = ger_ita_alliance_focus } \r\n";
					FocusTree += "		}";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				//Declaring war with Great Country
				vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);
				map<double, HoI4Country*> GCDistance;
				vector<HoI4Country*> GCDistanceSorted;
				//get great countries with a distance
				for each (auto GC in GreatCountries)
				{
					double distance = GetDistance(Leader, GC);
					if(distance < 1200)
						GCDistance.insert(make_pair(distance, GC));
				}
				//put them into a vector so we know their order
				for (auto iterator = GCDistance.begin(); iterator != GCDistance.end(); ++iterator)
				{
					GCDistanceSorted.push_back(iterator->second);
				}
				sort(GCDistanceSorted.begin(), GCDistanceSorted.end());
				vector<HoI4Country*> GCTargets;
				for each (auto GC in GCDistanceSorted)
				{
					string thetag = GC->getTag();
					string HowToTakeGC = HowToTakeLand(GC, Leader, 3);
					if (HowToTakeGC == "noactionneeded" || HowToTakeGC == "factionneeded" || HowToTakeGC == "morealliesneeded")
					{
						if(GC != Leader)
							GCTargets.push_back(GC);
					}

				}
				int maxGCWars = 0;
				int start = 0;
				if (GCTargets.size() == 2)
				{
					start = -1;
				}

				for each (auto GC in GCTargets)
				{
					string prereq = "";
					if (maxGCWars < 2)
					{
						int y2 = 0;
						//figuring out location of WG
						if (newAllies.size() > 0)
						{
							y2 = 2;
							prereq = " 	prerequisite = { ";

							for (int i = 0; i < 2; i++)
								prereq += " focus = Alliance_" + newAllies[i]->getTag() + Leader->getTag();

						prereq += "}\r\n";
						}
						int v1 = rand() % 12 + 1;
						int v2 = rand() % 12 + 1;
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = War" + GC->getTag() + Leader->getTag() + "\r\n";
						FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
						FocusTree += "		text = \"War with " + GC->getSourceCountry()->getName() + "\"\r\n";//change to faction name later
						if(prereq != "")
							FocusTree += prereq;
						FocusTree += "		available = {   date > 1938." + to_string(v1) + "." + to_string(v2) + "} \r\n";
						FocusTree += "		x = " + to_string(takenSpots.back() + start + 3 + maxGCWars*2) + "\r\n";
						FocusTree += "		y = " + to_string(y2) + "\r\n";
						//FocusTree += "		y = " + to_string(takenSpotsy.back() + 1) + "\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "			modifier = {\r\n";
						FocusTree += "			factor = 0\r\n";
						FocusTree += "			strength_ratio = { tag = " + GC->getTag() + " ratio < 1 }\r\n";
						FocusTree += "			}";
						if (GCTargets.size() > 1)
						{
							//make ai have this as a 0 modifier if they are at war
							FocusTree += "modifier = {\r\n	factor = 0\r\n	OR = {";
							for (int i2 = 0; i2 < 3; i2++)
							{
								if (GC != GCTargets[i2])
								{
									FocusTree += "has_war_with = " + GC->getTag()+"\r\n";
								}

							}
							FocusTree += "}\r\n}";
						}
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						FocusTree += "			create_wargoal = {\r\n";
						FocusTree += "				type = puppet_wargoal_focus\r\n";
						FocusTree += "				target = " + GC->getTag() + "\r\n";
						FocusTree += "			}";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";
						maxGCWars++;
					}
				}
				//insert these values in targetmap for use later possibly?
				//needs cleanup, too many vectors!
				TargetMap.insert(make_pair("noactionneeded", nan));
				TargetMap.insert(make_pair("factionneeded", fn));
				TargetMap.insert(make_pair("morealliesneeded", man));
				TargetMap.insert(make_pair("coup", coup));
				//actual eventoutput
				FocusTree += "\r\n}";
				
				//output National Focus
				string filenameNF("Output/" + Configuration::getOutputName() + "/common/national_focus/" + Leader->getSourceCountry()->getTag() + "_NF.txt");
				ofstream out2;
				out2.open(filenameNF);
				{
					out2 << FocusTree;
				}
				out2.close();
				//output events
				string filenameevents("Output/" + Configuration::getOutputName() + "/events/" + Leader->getSourceCountry()->getTag() + "_events.txt");
				//string filename2("Output/NF.txt");
				ofstream outevents;
				outevents.open(filenameevents);
				{
					outevents << Events;
				}
				outevents.close();

				out << s;
				out << endl;
			}
			if (Leader->getGovernment() == "absolute_monarchy")
			{
				//this is for monarchy events, dont need for random
				out << Leader->getSourceCountry()->getName() << endl;
				//too many lists, need to clean up
				vector<HoI4Country*> Targets;
				vector<HoI4Country*> WeakNeighbors;
				vector<HoI4Country*> WeakColonies;
				vector<HoI4Country*> EqualTargets;
				vector<HoI4Country*> DifficultTargets;
				//getting country provinces and its neighbors
				vector<int> leaderProvs = getCountryProvinces(Leader);
				map<string, HoI4Country*> AllNeighbors = findNeighbors(leaderProvs, Leader);
				map<string, HoI4Country*> CloseNeighbors;
				map<string, HoI4Country*> FarNeighbors;
				//gets neighbors that are actually close to you
				for each (auto neigh in AllNeighbors)
				{
					if (neigh.second->getCapitalProv() != 0)
					{
						//IMPROVE
						//need to get further neighbors, as well as countries without capital in an area
						double distance = GetDistance(Leader, neigh.second);
						if (distance <= 500)
							CloseNeighbors.insert(neigh);
						else
							FarNeighbors.insert(neigh);
					}
				}
				//if farneighbors is 0, try to find colonial conquest
				if (FarNeighbors.size() == 0)
				{
					for (auto Colcountry : countries)
					{

						HoI4Country* country2 = Colcountry.second;
						if (country2->getCapitalProv() != 0)
						{
							//IMPROVE
							//but this should never happen since the AI shouldnt even take this unless they already have colonies
							double distance = GetDistance(Leader, country2);
							if (distance <= 1000 && Colcountry.second->getProvinceCount() > 0)
								FarNeighbors.insert(Colcountry);
						}
					}
				}
				set<string> Allies = Leader->getAllies();
				//should add method to look for cores you dont own
				//should add method to look for more allies

				//lets look for weak neighbors
				LOG(LogLevel::Info) << "Doing Neighbor calcs for " + Leader->getSourceCountry()->getName();
				for (auto neigh : CloseNeighbors)
				{
					//lets check to see if they are not our ally and not a great country
					if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !checkIfGreatCountry(neigh.second, sourceWorld, countryMap))
					{
						volatile double enemystrength = getStrengthOverTime(neigh.second, 1.5);
						volatile double mystrength = getStrengthOverTime(Leader, 1.5);
						//lets see their strength is at least < 20%
						if (getStrengthOverTime(neigh.second, 1.5) < getStrengthOverTime(Leader, 1.5)*0.2 && findFaction(neigh.second).size() == 1)
						{
							//they are very weak
							WeakNeighbors.push_back(neigh.second);
						}
					}
				}
				for (auto neigh : FarNeighbors)
				{
					//lets check to see if they are not our ally and not a great country
					if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !checkIfGreatCountry(neigh.second, sourceWorld, countryMap))
					{
						volatile double enemystrength = getStrengthOverTime(neigh.second, 1.5);
						volatile double mystrength = getStrengthOverTime(Leader, 1.5);
						//lets see their strength is at least < 20%
						if (getStrengthOverTime(neigh.second, 1.5) < getStrengthOverTime(Leader, 1.5)*0.2 && findFaction(neigh.second).size() == 1)
						{
							//they are very weak
							WeakColonies.push_back(neigh.second);
						}
					}
				}
				string FocusTree = genericFocusTreeCreator(Leader);
				FocusTree += createMonarchyEmpireNF(Leader, WeakColonies.front(), WeakColonies.back(), WeakNeighbors.front(), WeakNeighbors.back(), WeakColonies.size(), WeakNeighbors.size(), 0);
				//Declaring war with Great Country
				vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);
				map<double, HoI4Country*> GCDistance;
				vector<HoI4Country*> GCDistanceSorted;
				//get great countries with a distance
				for each (auto GC in GreatCountries)
				{
					double distance = GetDistance(Leader, GC);
					if (distance < 1200)
						GCDistance.insert(make_pair(distance, GC));
				}
				//put them into a vector so we know their order
				for (auto iterator = GCDistance.begin(); iterator != GCDistance.end(); ++iterator)
				{
					GCDistanceSorted.push_back(iterator->second);
				}
				sort(GCDistanceSorted.begin(), GCDistanceSorted.end());
				vector<HoI4Country*> GCTargets;
				for each (auto GC in GCDistanceSorted)
				{
					string thetag = GC->getTag();
					string HowToTakeGC = HowToTakeLand(GC, Leader, 3);
					if (HowToTakeGC == "noactionneeded" || HowToTakeGC == "factionneeded")
					{
						if (GC != Leader)
							GCTargets.push_back(GC);
					}
					if (HowToTakeGC == "morealliesneeded")
					{

					}

				}
				int maxGCWars = 0;
				int start = 0;
				if (GCTargets.size() == 2)
				{
					start = -1;
				}

				for each (auto GC in GCTargets)
				{
					string prereq = "";
					if (maxGCWars < 2)
					{
						int y2 = 0;
						//figuring out location of WG
						/*if (newAllies.size() > 0)
						{
							y2 = 2;
							prereq = " 	prerequisite = { ";

							for (int i = 0; i < 2; i++)
								prereq += " focus = Alliance_" + newAllies[i]->getTag() + Leader->getTag();

							prereq += "}\r\n";
						}*/
						int v1 = rand() % 12 + 1;
						int v2 = rand() % 12 + 1;
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = War" + GC->getTag() + Leader->getTag() + "\r\n";
						FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
						FocusTree += "		text = \"War with " + GC->getSourceCountry()->getName() + "\"\r\n";//change to faction name later
						FocusTree += "		prerequisite = { focus =  MilitaryBuildup" + Leader->getTag() + " }\r\n";
						FocusTree += "		available = {   date > 1938." + to_string(v1) + "." + to_string(v2) + "} \r\n";
						FocusTree += "		x = 31" + to_string(maxGCWars*2)+"\r\n";
						FocusTree += "		y = 4\r\n";
						//FocusTree += "		y = " + to_string(takenSpotsy.back() + 1) + "\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "			modifier = {\r\n";
						FocusTree += "			factor = 0\r\n";
						FocusTree += "			strength_ratio = { tag = " + GC->getTag() + " ratio < 1 }\r\n";
						FocusTree += "			}";
						if (GCTargets.size() > 1)
						{
							//make ai have this as a 0 modifier if they are at war
							FocusTree += "modifier = {\r\n	factor = 0\r\n	OR = {";
							for (int i2 = 0; i2 < 3; i2++)
							{
								if (GC != GCTargets[i2])
								{
									FocusTree += "has_war_with = " + GC->getTag() + "\r\n";
								}

							}
							FocusTree += "}\r\n}";
						}
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						FocusTree += "			create_wargoal = {\r\n";
						FocusTree += "				type = puppet_wargoal_focus\r\n";
						FocusTree += "				target = " + GC->getTag() + "\r\n";
						FocusTree += "			}";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";
						maxGCWars++;
					}
				}
				FocusTree += "\r\n}";
				string Events = "";
				Events += "add_namespace = "+Leader->getTag()+"\r\n";
				int eventNumber = 0;
				for each (auto GC in GCTargets)
				{
					Events += "country_event = {\r\n";
					Events += "	id = " + Leader->getTag() + "." + to_string(eventNumber) +"\r\n";
					Events += "	title = \"Trade Incident\"\r\n";
					Events += "	desc = \"One of our convoys was sunk by "+GC->getSourceCountry()->getName()+"\"\r\n";
					Events += "	picture = GFX_report_event_chinese_soldiers_fighting\r\n";
					Events += "	\r\n";
					Events += "	is_triggered_only = yes\r\n";
					Events += "	\r\n";
					Events += " trigger = {";
					Events += "		has_country_flag = established_traders";
					Events += "		NOT = { has_country_flag = established_traders_activated }";
					Events += " }";
					Events += "	option = { # Breaking point!\r\n";
					Events += "		name = \"They will Pay!\"\r\n";
					Events += "		ai_chance = { factor = 85 }\r\n";
					Events += "		effect_tooltip = {\r\n";
					Events += "			"+Leader->getTag()+" = {\r\n";
					Events += "			set_country_flag = established_traders_activated";
					Events += "			create_wargoal = {\r\n";
					Events += "				type = annex_everything\r\n";
					Events += "				target = " + GC->getTag() + "\r\n";
					Events += "			}\r\n";
					Events += "		}\r\n";
					Events += "	}\r\n";
					Events += "}";
				}
				//output events
				string filenameevents("Output/" + Configuration::getOutputName() + "/events/" + Leader->getSourceCountry()->getTag() + "_events.txt");
				//string filename2("Output/NF.txt");
				ofstream outevents;
				outevents.open(filenameevents);
				{
					outevents << Events;
				}
				outevents.close();

				//output National Focus
				string filenameNF("Output/" + Configuration::getOutputName() + "/common/national_focus/" + Leader->getSourceCountry()->getTag() + "_NF.txt");
				ofstream out2;
				out2.open(filenameNF);
				{
					out2 << FocusTree;
				}
				out2.close();
			}
			if ((Leader->getGovernment() == "communism") && communismrelevant)
			{
				//communism still needs great country war events
				LOG(LogLevel::Info) << "Calculating AI for " + Leader->getSourceCountry()->getName();
				out << Leader->getSourceCountry()->getName() << endl;
				vector<int> leaderProvs = getCountryProvinces(Leader);
				LOG(LogLevel::Info) << "Calculating Neighbors for " + Leader->getSourceCountry()->getName();
				map<string, HoI4Country*> AllNeighbors = findNeighbors(leaderProvs, Leader);
				map<string, HoI4Country*> Neighbors;
				for each (auto neigh in AllNeighbors)
				{
					if (neigh.second->getCapitalProv() != 0)
					{
						//IMPROVE
						//need to get further neighbors, as well as countries without capital in an area
						double distance = GetDistance(Leader, neigh.second);
						if (distance <= 500)
							Neighbors.insert(neigh);
					}
				}
				set<string> Allies = Leader->getAllies();
				vector<HoI4Country*> Targets;
				map<string, vector<HoI4Country*>> NationalFocusesMap;
				vector<HoI4Country*> coups;
				vector<HoI4Country*> forcedtakeover;

				//if (Permanant Revolution)
					//Decide between Anti - Democratic Focus, Anti - Monarch Focus, or Anti - Fascist Focus(Look at all great powers and get average relation between each ideology, the one with the lowest average relation leads to that focus).
					//Attempt to ally with other Communist Countries(with Permanant Revolution)
				LOG(LogLevel::Info) << "Doing Neighbor calcs for " + Leader->getSourceCountry()->getName();
				for (auto neigh : Neighbors)
				{
					//lets check to see if they are our ally and not a great country
					if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !checkIfGreatCountry(neigh.second, sourceWorld, countryMap))
					{
						double com = 0;

						for (auto party : neigh.second->getParties())
						{
							if (party.name.find("socialist") != string::npos || party.name.find("communist") != string::npos || party.name.find("anarcho_liberal") != string::npos)
								com += party.popularity;
						}
						if (com > 25)
						{
							//look for neighboring countries to spread communism too(Need 25 % or more Communism support), Prioritizing those with "Communism Allowed" Flags, prioritizing those who are weakest
							//	Method() Influence Ideology and Attempt Coup
							out << "/t" + neigh.second->getSourceCountry()->getName() + " Attempt Coup" << endl;
							coups.push_back(neigh.second);
						}
						else if(findFaction(neigh.second).size() == 1)
						{
							//	Then look for neighboring countries to spread communism by force, prioritizing weakest first
							forcedtakeover.push_back(neigh.second);
							//	Depending on Anti - Ideology Focus, look for allies in alternate ideologies to get to ally with to declare war against Anti - Ideology Country.
						}
					}
				}
				//if (Socialism in One State)
				//	Events / Focuses to increase Industrialization and defense of the country, becomes Isolationist
				//	Eventually gets events to drop Socialism in One state and switch to permanant revolution(Maybe ? )

				string s;
				map<string, vector<HoI4Country*>> TargetMap;
				vector<HoI4Country*> nan;
				vector<HoI4Country*> fn;
				vector<HoI4Country*> man;
				vector<HoI4Country*> coup;
				for (auto target : forcedtakeover)
				{
					string type;
					//outputs are
					//noactionneeded -  Can take target without any help
					//factionneeded - can take target and faction with attackers faction helping
					//morealliesneeded - can take target with more allies, comes with "newallies" in map
					//coup - cant take over, need to coup
					type = HowToTakeLand(target, Leader, 2.5);
					out << type + " " + target->getSourceCountry()->getName() << endl;
					if (type == "noactionneeded")
						nan.push_back(target);
					else if (type == "factionneeded")
						fn.push_back(target);
					else if (type == "morealliesneeded")
						man.push_back(target);
					else if (type == "coup")
						coup.push_back(target);
				}
				//insert these values in targetmap for use later possibly?
				TargetMap.insert(make_pair("noactionneeded", nan));
				TargetMap.insert(make_pair("factionneeded", fn));
				TargetMap.insert(make_pair("morealliesneeded", man));
				TargetMap.insert(make_pair("coup", coup));
				//actual eventoutput
				vector<int> takenSpots;
				string FocusTree = genericFocusTreeCreator(Leader);
				if (coups.size() > 0)
				{
					int x = 22;
					takenSpots.push_back(x);
					if (coups.size() == 1)
					{
						x = 24;
						takenSpots.push_back(x);
					}
					if (coups.size() >= 2)
					{
						x = 25;
						takenSpots.push_back(x);
					}
					//Focus to increase Comm support and prereq for coups
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = Home_of_Revolution"+Leader->getTag()+"\r\n";
						FocusTree += "		icon = GFX_goal_support_communism\r\n";
						FocusTree += "		text = \"Home of the Revolution\"\r\n";
						FocusTree += "		x = "+to_string(x)+"\r\n";
						FocusTree += "		y = 0\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						//FIXME 
						//Need to get Drift Defense to work
						//FocusTree += "			drift_defence_factor = 0.5\r\n";
						FocusTree += "			add_ideas = communist_influence\r\n";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";
						
					for (unsigned int i = 0; i < 2; i++)
					{
						if (i < coups.size())
						{
							FocusTree += "focus = {\r\n";
							FocusTree += "		id = Influence_" +coups[i]->getTag()+"_" +Leader->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_propaganda\r\n";
							FocusTree += "		text = \"Influence "+ coups[i]->getSourceCountry()->getName() +"\"\r\n";
							FocusTree += "		prerequisite = { focus = Home_of_Revolution" + Leader->getTag() + " }\r\n";
							FocusTree += "		x = " + to_string(24+i*2) + "\r\n";
							FocusTree += "		y = 1\r\n";
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 5\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			" + coups[i]->getTag()+" = {\r\n";
							FocusTree += "				if = {\r\n";
							FocusTree += "					limit = {\r\n";
							FocusTree += "						" + Leader->getTag() + " = {\r\n";
							FocusTree += "							has_government = fascism\r\n";
							FocusTree += "						}\r\n";
							FocusTree += "					}\r\n";
							FocusTree += "					add_ideas = fascist_influence\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				if = {\r\n";
							FocusTree += "					limit = {\r\n";
							FocusTree += "						" + Leader->getTag() + " = {\r\n";
							FocusTree += "							has_government = communism\r\n";
							FocusTree += "						}\r\n";
							FocusTree += "					}\r\n";
							FocusTree += "					add_ideas = communist_influence\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				if = {\r\n";
							FocusTree += "					limit = {\r\n";
							FocusTree += "						" + Leader->getTag() + " = {\r\n";
							FocusTree += "							has_government = democratic\r\n";
							FocusTree += "						}\r\n";
							FocusTree += "					}\r\n";
							FocusTree += "					add_ideas = democratic_influence\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				country_event = { id = generic.1 }";
							FocusTree += "			}\r\n";
							FocusTree += "			\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}\r\n";
							//Civil War
							FocusTree += "focus = {\r\n";
							FocusTree += "		id = Coup_" + coups[i]->getTag() + "_" + Leader->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_demand_territory\r\n";
							FocusTree += "		text = \"Civil War in " + coups[i]->getSourceCountry()->getName() + "\"\r\n";
							FocusTree += "		prerequisite = { focus = Influence_" + coups[i]->getTag() + "_" + Leader->getTag() + " }\r\n";
							FocusTree += "		available = {\r\n";
							FocusTree += "		"+ coups[i]->getTag() +" = { communism > 0.5 } ";
							FocusTree += "		}";
							FocusTree += "		x = " + to_string(24 + i * 2) + "\r\n";
							FocusTree += "		y = 2\r\n";
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 5\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			" + coups[i]->getTag() + " = {\r\n";
							FocusTree += "						start_civil_war = {\r\n";
							FocusTree += "							ideology = communism\r\n";
							FocusTree += "							size = 0.5\r\n";
							FocusTree += "					}";
							FocusTree += "			}\r\n";
							FocusTree += "			\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}\r\n";
						}
					}
				}
				if (forcedtakeover.size() > 0)
				{
					
					//Strengthen Commitern
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = StrengthCom" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_support_communism\r\n";
					FocusTree += "		text = \"Strengthen The Comintern\"\r\n";//change to faction name later
					FocusTree += "		x = " + to_string(takenSpots.back() + 5) + "\r\n";//fixme
					FocusTree += "		y = 0\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "			army_experience = 20\r\n";
					FocusTree += "		add_tech_bonus = { \r\n";
					FocusTree += "				bonus = 0.5\r\n";
					FocusTree += "				uses = 2\r\n";
					FocusTree += "				category = land_doctrine\r\n";
					FocusTree += "			}";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";

					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Inter_Com_Pres" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_dangerous_deal\r\n";
					FocusTree += "		text = \"International Communist Pressure\"\r\n";//change to faction name later
					FocusTree += "		prerequisite = { focus = StrengthCom" + Leader->getTag() + " }\r\n";
					FocusTree += "		available = {  date > 1937.1.1 } \r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 5) + "\r\n";
					FocusTree += "		y = 1\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "			add_named_threat = { threat = 2 name = \"Socialist World Republic\" }\r\n";
					//FIXME
					//maybe add some claims?
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
					vector<HoI4Country*> TargetsbyIC; //its actually by tech lol
					bool first = true;
					//FIXME 
					//Right now just uses everyone in forcedtakover, doesnt use nan, fn, ect...
					for (auto country : forcedtakeover)
					{
						if (first)
						{
							TargetsbyIC.push_back(country);
							first = false;
						}
						else
						{
							//makes sure not a coup target
							if (find(coups.begin(), coups.end(), country) == coups.end())
							{
								if (TargetsbyIC.front()->getTechnologyCount() < country->getTechnologyCount())
								{
									TargetsbyIC.insert(TargetsbyIC.begin(), country);
								}
								else
									TargetsbyIC.push_back(country);
							}
						}
					}
					for (unsigned int i = 0; i < 3; i++)
					{
						if (i < TargetsbyIC.size())
						{
							int v1 = rand() % 12 + 1;
							int v2 = rand() % 12 + 1;
							FocusTree += "focus = {\r\n";
							FocusTree += "		id = War"+ TargetsbyIC[i]->getTag() + Leader->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
							FocusTree += "		text = \"War with "+ TargetsbyIC[i]->getSourceCountry()->getName()+"\"\r\n";//change to faction name later
							FocusTree += "		prerequisite = { focus = Inter_Com_Pres" + Leader->getTag() + " }\r\n";
							FocusTree += "		available = {   date > 1938."+to_string(v1)+"."+to_string(v2)+"} \r\n";
							FocusTree += "		x = " + to_string(takenSpots.back() + 3 + i*2) + "\r\n";
							FocusTree += "		y = 2\r\n";
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 5\r\n";
							FocusTree += "			modifier = {\r\n";
							FocusTree += "			factor = 0\r\n";
							FocusTree += "			strength_ratio = { tag = " + TargetsbyIC[i]->getTag() + " ratio < 1 }\r\n";
							FocusTree += "			}";
							if (TargetsbyIC.size() > 1)
							{
								FocusTree += "modifier = {\r\n	factor = 0\r\n	OR = {";
									for (int i2 = 0; i2 < 3; i2++)
									{
										if (i != i2)
											FocusTree += "has_war_with = " + TargetsbyIC[i]->getTag()+"\r\n";
									}
									FocusTree += "}\r\n}";
							}
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			create_wargoal = {\r\n";
							FocusTree += "				type = puppet_wargoal_focus\r\n";
							FocusTree += "				target = "+ TargetsbyIC[i]->getTag()+"\r\n";
							FocusTree += "			}";
							FocusTree += "		}\r\n";
							FocusTree += "	}\r\n";
							
						}
					}
					takenSpots.push_back(takenSpots.back() + 5);
				}
				//events for allies
				vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);
				if (newAllies.size() > 0)
				{
					//Focus to call summit, maybe have events from summit
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Com_Summit" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Call for the Communist Summit\"\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 3) + "\r\n";
					FocusTree += "		y = 0\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 0\r\n";
					FocusTree += "			modifier = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "			date > 1938.1.1\r\n";
					FocusTree += "			}";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					//FocusTree += "			opinion_gain_monthly_factor = 1.0 \r\n";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				for (unsigned int i = 0; i < newAllies.size(); i++)
				{
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Alliance_" + newAllies[i]->getTag() + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Alliance with " + newAllies[i]->getSourceCountry()->getName() + "\"\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 3 + i) + "\r\n";
					FocusTree += "		y = 1\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 10\r\n";
					FocusTree += "		bypass = { \r\n";
					FocusTree += "			\r\n";
					FocusTree += "			OR = {\r\n";
					FocusTree += "				is_in_faction_with = " + newAllies[i]->getTag() + "\r\n";
					FocusTree += "				has_war_with = " + newAllies[i]->getTag() + "\r\n";
					FocusTree += "				NOT = { country_exists = " + newAllies[i]->getTag() + " }\r\n";
					FocusTree += "			}\r\n";
					FocusTree += "		}\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "		" + newAllies[i]->getTag() + " = {\r\n";
					FocusTree += "			add_opinion_modifier = { target = "+Leader->getTag() +" modifier = ger_ita_alliance_focus } \r\n";
					FocusTree += "		}";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				FocusTree += "\r\n}";
				string filename2("Output/" + Configuration::getOutputName() + "/common/national_focus/"+Leader->getSourceCountry()->getTag()+"_NF.txt");
				//string filename2("Output/NF.txt");
				ofstream out2;
				out2.open(filename2);
				{
					out2 << FocusTree;
				}
				out2.close();
				out << s;
				out << endl;
			}
		}
		out.close();
	}
}
string HoI4World::HowToTakeLand(HoI4Country* TargetCountry, HoI4Country* AttackingCountry, double time)
{
	string s;
	string type;
		if (TargetCountry != AttackingCountry)
		{
			vector<HoI4Country*> targetFaction = findFaction(TargetCountry);
			vector<HoI4Country*> moreAllies = GetMorePossibleAllies(AttackingCountry);
			vector<HoI4Country*> myFaction = findFaction(AttackingCountry);
			//right now assumes you are stronger then them

			double myFactionDisStrength = GetFactionStrengthWithDistance(AttackingCountry, myFaction, time);
			double enemyFactionDisStrength = GetFactionStrengthWithDistance(TargetCountry, targetFaction, time);
			//lets check if I am stronger then their faction
			if (getStrengthOverTime(AttackingCountry,time) >= GetFactionStrength(targetFaction))
			{
				//we are stronger, and dont even need ally help
				//ADD CONQUEST GOAL
				type = "noactionneeded";
				s += "Can kill " + TargetCountry->getSourceCountry()->getName() + " by ourselves\r\n\t I have a strength of " + to_string(getStrengthOverTime(AttackingCountry, time));
				s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName() + " has a strength of " + to_string(getStrengthOverTime(TargetCountry, time));
				s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + " \r\n";
			}
			else
			{
				//lets check if my faction is stronger
				
				if (myFactionDisStrength >= enemyFactionDisStrength)
				{
					//ADD CONQUEST GOAL
					type = "factionneeded";
					s += "Can kill " + TargetCountry->getSourceCountry()->getName() + " with our faction\r\n\t I have a strength of " + to_string(getStrengthOverTime(AttackingCountry, time));
					s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName() + " has a strength of "+ to_string(getStrengthOverTime(TargetCountry, time));
					s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + " \r\n";
				}
				else
				{
					//FIXME
					//hmm I am still weaker, maybe need to look for allies?
					type = "morealliesneeded";
					//targettype.insert(make_pair("newallies", moreAllies));
					myFactionDisStrength = GetFactionStrengthWithDistance(AttackingCountry, myFaction, time) + GetFactionStrengthWithDistance(AttackingCountry, moreAllies, time);
					enemyFactionDisStrength = GetFactionStrengthWithDistance(TargetCountry, targetFaction, time);
					if (GetFactionStrengthWithDistance(AttackingCountry, myFaction, time) >= GetFactionStrengthWithDistance(TargetCountry, targetFaction, time))
					{
						//ADD CONQUEST GOAL
						s += "Can kill " + TargetCountry->getSourceCountry()->getName() + " with our faction Once I have more allies\r\n\t I have a strength of " + to_string(getStrengthOverTime(AttackingCountry,1));
						s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName() + " has a strength of " + to_string(getStrengthOverTime(TargetCountry, 1));
						s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + " \r\n";
					}
					else
					{
						//Time to Try Coup
						type = "coup";
						s += "Cannot kill " + TargetCountry->getSourceCountry()->getName() + ", time to try coup\r\n";
					}
				}

			}
		}
		return type;
}
vector<HoI4Country*> HoI4World::GetMorePossibleAllies(HoI4Country* CountryThatWantsAllies)
{
	int maxcountries = 0;
	vector<HoI4Country*> newPossibleAllies;
	set<string> currentAllies = CountryThatWantsAllies->getAllies();
	//set<string> currentAllies = CountryThatWantsAllies->getAllies();
	vector<HoI4Country*> CountriesWithin500Miles; //Rename to actual distance
	for (auto country : countries)
	{
		if (country.second->getProvinceCount() != 0)
		{
			HoI4Country* country2 = country.second;
			if (GetDistance(CountryThatWantsAllies, country2) <= 500)
				if (std::find(currentAllies.begin(), currentAllies.end(), country2->getTag()) == currentAllies.end())
				{
					CountriesWithin500Miles.push_back(country2);
				}
		}
	}
	string yourgovernment = CountryThatWantsAllies->getGovernment();
	volatile vector<HoI4Country*> vCountriesWithin500Miles = CountriesWithin500Miles;
	//look for all capitals within a distance of Berlin to Tehran
	for (unsigned int i = 0; i < CountriesWithin500Miles.size(); i++)
	{
		string allygovernment = CountriesWithin500Miles[i]->getGovernment();
		//possible government matches
		if (allygovernment == yourgovernment
			|| (yourgovernment == "absolute_monarchy" && (allygovernment == "fascism" || allygovernment == "democratic" || allygovernment == "prussian_constitutionalism" || allygovernment == "hms_government"))
			|| (yourgovernment == "democratic" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
			|| (yourgovernment == "prussian_constitutionalism" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "democratic" || allygovernment == "fascism"))
			|| (yourgovernment == "hms_government" && (allygovernment == "democratic" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
			|| (yourgovernment == "communism" && (allygovernment == "syndicalism"))
			|| (yourgovernment == "syndicalism" && (allygovernment == "communism" || allygovernment == "fascism"))
			|| (yourgovernment == "fascism" && (allygovernment == "syndicalism" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism" || allygovernment == "hms_government")))
		{

			if (maxcountries < 2)
			{
				//FIXME
				//check if we are friendly at all?
				HoI4Relations* relationswithposally = CountryThatWantsAllies->getRelations(CountriesWithin500Miles[i]->getTag());
				volatile int rel = relationswithposally->getRelations();
				volatile int size = findFaction(CountriesWithin500Miles[i]).size();
				volatile int armysize = getStrengthOverTime(CountriesWithin500Miles[i], 1);
				//for now can only ally with people not in a faction, and must be worth adding
				if (relationswithposally->getRelations() >= -50 && findFaction(CountriesWithin500Miles[i]).size() <= 1)
				{
					//ok we dont hate each other, lets check how badly we need each other, well I do, the only reason I am here is im trying to conquer a neighbor and am not strong enough!
					//if (GetFactionStrength(findFaction(country)) < 20000) //maybe also check if he has any fascist/comm neighbors he doesnt like later?
					
						//well that ally is weak, he probably wants some friends
						if (relationswithposally->getRelations() >= -50 && relationswithposally->getRelations() < 0)
						{
							//will take some NF to ally
							newPossibleAllies.push_back(CountriesWithin500Miles[i]);
							maxcountries++;
						}
						if(relationswithposally->getRelations() >= 0)
						{
							//well we are positive, 1 NF to add to ally should be fine
							newPossibleAllies.push_back(CountriesWithin500Miles[i]);
							maxcountries++;
						}
					
					/*else if (relationswithposally->getRelations() > 0)
					{
						//we are friendly, add 2 NF for ally? Good way to decide how many alliances there will be
						newPossibleAllies.push_back(country);
						i++;
					}*/
					
				}
			}
			
		}
	}
	return newPossibleAllies;
}
double HoI4World::GetDistance(HoI4Country* Country1, HoI4Country* Country2)
{
	map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMap; // <vic, hoi>
	volatile HoI4Country* Country23 = Country2;
	auto C1state = states->getStates().find(Country1->getCapitalProv());
	auto C2state = states->getStates().find(Country2->getCapitalProv());
	int C1prov;
	int C2prov;
	if (Country1->getCapitalProv() == 0 || Country2->getCapitalProv() == 0)
		return 100000;
	else
	{
		C1prov = *(C1state->second->getProvinces().begin());
		C2prov = *(C2state->second->getProvinces().begin());
	}
	double C1x = 0;
	double C1y = 0;
	double C2x = 0;
	double C2y = 0;
	std::ifstream file("positions.txt");
	std::string str;
	while (std::getline(file, str))
	{
		vector<string> parts;
		stringstream ss(str);
		string tok;
		char delimiter = ';';
		while (getline(ss, tok, delimiter))
		{
			parts.push_back(tok);
		}
		if (stoi(parts[0]) == C1prov)
		{
			C1x = stoi(parts[2]);
			C1y = stoi(parts[4]);
		}
		else if (stoi(parts[0]) == C2prov)
		{
			C2x = stoi(parts[2]);
			C2y = stoi(parts[4]);
		}
		else if (C1x != 0 && C1y != 0 && C2x != 0 && C2y != 0)
			break;

	}
	double xdist = abs(C2x - C1x);
	if (xdist > 2625)
		xdist = 5250 - xdist;
	double distance = sqrt(pow(xdist, 2) + pow(C2y - C1y, 2));
	return distance;
}
double HoI4World::GetFactionStrengthWithDistance(HoI4Country* HomeCountry, vector<HoI4Country*> Faction, double time)
{
	double strength;
	for (auto country : Faction)
	{
		double distanceMulti = 1;
		if (country == HomeCountry)
		{
			 distanceMulti = 1;
		}
		else
			 distanceMulti = GetDistance(HomeCountry, country);

		if (distanceMulti < 300)
			distanceMulti = 1;
		else if (distanceMulti < 500)
			distanceMulti = 0.9;
		else if (distanceMulti < 750)
			distanceMulti = 0.8;
		else if (distanceMulti < 1000)
			distanceMulti = 0.7;
		else if (distanceMulti < 1500)
			distanceMulti = 0.5;
		else if (distanceMulti < 2000)
			distanceMulti = 0.3;
		else
			distanceMulti = 0.2;

		strength += getStrengthOverTime(country, time) * distanceMulti;
	}
	return strength;
}
vector<HoI4Country*> HoI4World::findFaction(HoI4Country* CheckingCountry)
{
	vector<HoI4Country*> ourFaction;
	ourFaction.push_back(CheckingCountry);
	for (auto faction : Factions)
	{
		if (std::find(faction.begin(), faction.end(), CheckingCountry) != faction.end())
		{
			//if country is in faction list, it is part of that faction
			ourFaction = faction;
		}
	}
	return ourFaction;
}
bool HoI4World::checkIfGreatCountry(HoI4Country* checkingCountry, const V2World &sourceWorld, const CountryMapping& countryMap)
{
	bool isGreatCountry = false;
	vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);

	if (std::find(GreatCountries.begin(), GreatCountries.end(), checkingCountry) != GreatCountries.end())
	{
		isGreatCountry = true;
	}
	return isGreatCountry;

}
map<string, HoI4Country*> HoI4World::findNeighbors(vector<int> CountryProvs, HoI4Country* CheckingCountry)
{
	map<string, HoI4Country*> Neighbors;
	string filename("Output/" + CheckingCountry->getSourceCountry()->getName() + "neigh.txt");
	ofstream out;
	vector<HoI4Province> provinces2;
	out.open(filename);
	{
		for (auto prov : CountryProvs)
		{
			vector<int> thisprovNeighbors = provinceNeighbors.find(prov)->second;
			for (int prov : thisprovNeighbors)
			{
				//string ownertag = "";
				auto ownertag = provincemap.find(prov);
				if (ownertag != provincemap.end())
				{
					vector<string> tags = ownertag->second;
					if (tags.size() >= 1)
					{
						HoI4Country* ownerCountry = countries.find(tags[1])->second;
						if (ownerCountry != CheckingCountry && ownerCountry->getProvinceCount() > 0 )
						{
							//if not already in neighbors
							if (Neighbors.find(tags[1]) == Neighbors.end())
							{
								Neighbors.insert(make_pair(tags[1], ownerCountry));
							}
						}
					}
				}
			}		
		}
		if (Neighbors.size() == 0)
		{

			for (auto country : countries)
			{

				HoI4Country* country2 = country.second;
				if (country2->getCapitalProv() != 0)
				{
					//IMPROVE
					//need to get further neighbors, as well as countries without capital in an area
					double distance = GetDistance(CheckingCountry, country2);
					if (distance <= 500 && country.second->getProvinceCount() > 0 )
						Neighbors.insert(country);
				}
			}
		}
		out << CheckingCountry->getSourceCountry()->getName() + " " << endl;

		for (auto neigh : Neighbors)
			out << neigh.second->getSourceCountry()->getName() + " ";
		out << endl;
		out.close();
	}
	return Neighbors;
}
void HoI4World::fillProvinces()
{
	for (auto state : states->getStates())
	{
		for (auto prov : state.second->getProvinces())
		{
			string owner = state.second->getOwner();
			int stateID = state.second->getID();
			vector<string> provinceinfo;
			provinceinfo.push_back(to_string(stateID));
			provinceinfo.push_back(owner);
			provincemap.insert(pair<int, vector<string>>(prov, provinceinfo));
			// HoI4Province* newprov = new HoI4Province(owner, stateID);
			//provinces.insert(pair<int, HoI4Province*>(prov, newprov));
		}
	}
}
vector<int> HoI4World::getCountryProvinces(HoI4Country* Country)
{
	vector<int> provinces;
	for (auto state : states->getStates())
	{
		if (state.second->getOwner() == Country->getTag())
		{
			for (auto prov : state.second->getProvinces())
			{

				provinces.push_back(prov);
			}
		}
	}
	return provinces;
}
vector<vector<HoI4Country*>> HoI4World::CreateFactions(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	vector<vector<HoI4Country*>> Factions2;
	string filename("Output/Factions.txt");
	ofstream out;
	out.open(filename);
	{
		vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);
		vector<string> usedCountries;
		for (auto country : GreatCountries)
		{
			if (std::find(usedCountries.begin(), usedCountries.end(), country->getTag()) == usedCountries.end())
			{
				//checks to make sure its not creating a faction when already in one
				vector<HoI4Country*> Faction;
				double FactionMilStrength = 0;
				Faction.push_back(country);
				string yourgovernment = country->getGovernment();
				auto allies = country->getAllies();
				vector<int> yourbrigs = country->getBrigs();
				auto yourrelations = country->getRelations();
				out << country->getSourceCountry()->getName() << " " + yourgovernment + " initial strength:" + to_string(getInitialStrength(country)) + " Factory Strength per year: "+to_string(getAddedStrength(country,1))+" allies: \n";
				usedCountries.push_back(country->getTag());
				FactionMilStrength = getStrengthOverTime(country, 1);
				for (auto ally : allies)
				{

					auto itrally = countries.find(ally);
					if (itrally != countries.end())
					{
						HoI4Country* allycountry = itrally->second;
						string allygovernment = allycountry->getGovernment();
						string name = "";
						vector<int> allybrigs = allycountry->getBrigs();
						for (auto country : countries)
						{
							if (country.second->getTag() == ally)
								name = country.second->getSourceCountry()->getName();
						}
						string sphere = returnIfSphere(country, allycountry, sourceWorld, countryMap);

						if (allygovernment == yourgovernment || sphere == country->getTag()
							|| (yourgovernment == "absolute_monarchy" && (allygovernment == "fascism" || allygovernment == "democratic" || allygovernment == "prussian_constitutionalism" || allygovernment == "hms_government"))
							|| (yourgovernment == "democratic" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
							|| (yourgovernment == "prussian_constitutionalism" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "democratic" || allygovernment == "fascism"))
							|| (yourgovernment == "hms_government" && (allygovernment == "democratic" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
							|| (yourgovernment == "communism" && (allygovernment == "syndicalism"))
							|| (yourgovernment == "syndicalism" && (allygovernment == "communism" || allygovernment == "fascism"))
							|| (yourgovernment == "fascism" && (allygovernment == "syndicalism" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism")))
						{
							bool canally = false;
							//if there is a sphere leader, it is not blank
							if (sphere != "")
							{
								//if sphere is equal to great power in question, can ally
								if (sphere == country->getTag())
								{
									canally = true;
								}
							}
							else
								canally = true;

							if (canally)
							{
								usedCountries.push_back(allycountry->getTag());
								out << "\t" + name + " " + allygovernment + " initial strength:" + to_string(getInitialStrength(allycountry)) + " Factory Strength per year: " + to_string(getAddedStrength(allycountry, 1)) << endl;
								FactionMilStrength += getStrengthOverTime(allycountry, 1);
								Faction.push_back(allycountry);
							}
						}
					}

				}
				out << "\tFaction Strength: " + to_string(FactionMilStrength) << endl;
				out << endl;
				Factions2.push_back(Faction);
			}

		}


		out.close();
	}
	return Factions2;
}
HoI4Country* HoI4World::GetFactionLeader(vector<HoI4Country*> Faction)
{
	return Faction.front();
}
double HoI4World::GetFactionStrength(vector<HoI4Country*> Faction)
{
	double strength = 0;
	for (auto country : Faction)
	{
		strength += getStrengthOverTime(country, 1);
	}
	return strength;
}
vector<HoI4Country*> HoI4World::returnGreatCountries(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	const vector<string>& greatCountries = sourceWorld.getGreatCountries();
	vector<HoI4Country*> GreatCountries;
	for (auto countryItr : greatCountries)
	{
		auto itr = countries.find(countryMap[countryItr]);
		if (itr != countries.end())
		{
			GreatCountries.push_back(itr->second);
		}
	}
	return GreatCountries;
}
string HoI4World::returnIfSphere(HoI4Country* leadercountry, HoI4Country* posLeaderCountry, const V2World &sourceWorld, const CountryMapping& countryMap)
{
	vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);
	for (auto country : GreatCountries)
	{
		auto relations = country->getRelations();
		for (auto relation : relations)
		{
			if (relation.second->getSphereLeader())
			{

				string tag = relation.second->getTag();
				auto spheredcountry = countries.find(tag);
				if (spheredcountry != countries.end())
				{
					if (posLeaderCountry->getTag() == spheredcountry->second->getTag())
					{
						return country->getTag();
					}

				}

			}
		}
	}

}
