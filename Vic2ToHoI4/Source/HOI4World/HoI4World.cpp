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
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../Configuration.h"
#include "../V2World/Vic2Agreement.h"
#include "../V2World/V2Diplomacy.h"
#include "../V2World/V2Province.h"
#include "../V2World/V2Party.h"
#include "HoI4Faction.h"
#include "HoI4Focus.h"
#include "HoI4FocusTree.h"
#include "HoI4Relations.h"
#include "HoI4State.h"
#include "HoI4SupplyZone.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/ProvinceMapper.h"
#include "../Mappers/StateMapper.h"



HoI4World::HoI4World(const V2World* _sourceWorld)
{
	LOG(LogLevel::Info) << "Parsing HoI4 data";
	sourceWorld = _sourceWorld;

	map<int, vector<int>> HoI4DefaultStateToProvinceMap;
	states = new HoI4States(sourceWorld);
	states->importStates(HoI4DefaultStateToProvinceMap);

	importSuppplyZones(HoI4DefaultStateToProvinceMap);
	importStrategicRegions();
	checkAllProvincesMapped();
	states->convertStates();
	convertNavalBases();
	convertCountries();
	states->addLocalisations();
	convertIndustry();
	convertResources();
	convertSupplyZones();
	convertStrategicRegions();
	convertDiplomacy();
	convertTechs();
	generateLeaders();
	convertArmies();
	convertNavies();
	convertAirforces();
	getGreatPowers();
	convertCapitalVPs();
	convertAirBases();
	thatsgermanWarCreator();
	buildings = new HoI4Buildings(states->getProvinceToStateIDMap());
}


void HoI4World::importSuppplyZones(const map<int, vector<int>>& defaultStateToProvinceMap)
{
	LOG(LogLevel::Info) << "Importing supply zones";

	set<string> supplyZonesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/map/supplyareas", supplyZonesFiles);
	for (auto supplyZonesFile : supplyZonesFiles)
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
		auto supplyAreaObj = fileObj->getValue("supply_area");
		int ID = stoi(supplyAreaObj[0]->getLeaf("id"));
		int value = stoi(supplyAreaObj[0]->getLeaf("value"));

		HoI4SupplyZone* newSupplyZone = new HoI4SupplyZone(ID, value);
		supplyZones.insert(make_pair(ID, newSupplyZone));

		// map the provinces to the supply zone
		auto statesObj = supplyAreaObj[0]->getValue("states");
		for (auto idString : statesObj[0]->getTokens())
		{
			auto mapping = defaultStateToProvinceMap.find(stoi(idString));
			for (auto province : mapping->second)
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
	for (auto filename : filenames)
	{
		HoI4StrategicRegion* newRegion = new HoI4StrategicRegion(filename);
		strategicRegions.insert(make_pair(newRegion->getID(), newRegion));

		for (auto province : newRegion->getOldProvinces())
		{
			provinceToStratRegionMap.insert(make_pair(province, newRegion->getID()));
		}
	}
}


void HoI4World::output() const
{
	LOG(LogLevel::Info) << "Outputting world";

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

	outputCommonCountries();
	outputColorsfile();
	//outputAutoexecLua();
	outputLocalisations();
	outputHistory();
	outputMap();
	outputSupply();
	outputRelations();
	outputCountries();
	buildings->output();
	events.output();
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
	if (fopen_s(&allCountriesFile, ("Output/" + Configuration::getOutputName() + "/common/country_tags/00_countries.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create countries file";
		exit(-1);
	}

	for (auto countryItr : countries)
	{
		if (countryItr.second->getCapitalNum() != 0)
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

	output << "#reload countrycolors\n";
	for (auto countryItr : countries)
	{
		if (countryItr.second->getCapitalNum() != 0)
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
	for (auto state : states->getStates())
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
	for (auto state : states->getStates())
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
	for (auto strategicRegion : strategicRegions)
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

	LOG(LogLevel::Debug) << "Writing diplomacy";
	//diplomacy.output();
}


void HoI4World::getProvinceLocalizations(const string& file)
{
	ifstream read;
	string line;
	read.open(file);
	while (read.good() && !read.eof())
	{
		getline(read, line);
		if (line.substr(0, 4) == "PROV" && isdigit(line[4]))
		{
			int position = line.find_first_of(';');
			int num = stoi(line.substr(4, position - 4));
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			provinces[num]->setName(name);
		}
	}
	read.close();
}


void HoI4World::convertCountries()
{
	LOG(LogLevel::Info) << "Converting countries";

	// Parse leader traits
	LOG(LogLevel::Info) << "Parsing government jobs";
	/*parser_UTF8::initParser();
	obj = parser_UTF8::doParseFile("leader_traits.txt");
	if (obj == NULL)
	{
	LOG(LogLevel::Error) << "Could not parse file leader_traits.txt";
	exit(-1);
	}*/
	//initLeaderTraitsMap(obj->getLeaves()[0], leaderTraits);

	// Parse government jobs
	/*LOG(LogLevel::Info) << "Parsing government jobs";
	parser_UTF8::initParser();
	obj = parser_UTF8::doParseFile("governmentJobs.txt");
	if (obj == NULL)
	{
	LOG(LogLevel::Error) << "Could not parse file governmentJobs.txt";
	exit(-1);
	}*/
	governmentJobsMap governmentJobs;
	//initGovernmentJobTypes(obj->getLeaves()[0], governmentJobs);

	// parse culture mapping
	LOG(LogLevel::Info) << "Parsing culture mappings";
	Object* obj = parser_UTF8::doParseFile("culture_map.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file culture_map.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse culture_map.txt";
		exit(-1);
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(obj->getLeaves()[0]);

	// parse personality mapping
	LOG(LogLevel::Info) << "Parsing personality mappings";
	/*obj = parser_UTF8::doParseFile("personality_map.txt");
	if (obj == NULL)
	{
	LOG(LogLevel::Error) << "Could not parse file personality_map.txt";
	exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
	LOG(LogLevel::Error) << "Failed to parse personality_map.txt";
	return 1;
	}*/
	personalityMap landPersonalityMap;
	personalityMap seaPersonalityMap;
	//initLeaderPersonalityMap(obj->getLeaves()[0], landPersonalityMap, seaPersonalityMap);

	// parse background mapping
	LOG(LogLevel::Info) << "Parsing background mappings";
	/*obj = parser_UTF8::doParseFile("background_map.txt");
	if (obj == NULL)
	{
	LOG(LogLevel::Error) << "Could not parse file background_map.txt";
	exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
	LOG(LogLevel::Error) << "Failed to parse background_map.txt";
	return 1;
	}*/
	backgroundMap landBackgroundMap;
	backgroundMap seaBackgroundMap;
	//initLeaderBackgroundMap(obj->getLeaves()[0], landBackgroundMap, seaBackgroundMap);

	// parse names
	LOG(LogLevel::Info) << "Parsing names";
	for (auto itr: Configuration::getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod cultures";
		obj = parser_8859_15::doParseFile((Configuration::getV2Path() + "/mod/" + itr + "/common/cultures.txt"));
		if (obj != NULL)
		{
			initNamesMapping(obj, namesMap);
		}
	}
	obj = parser_8859_15::doParseFile((Configuration::getV2Path() + "/common/cultures.txt"));
	if (obj != NULL)
	{
		initNamesMapping(obj, namesMap);
	}

	// parse portraits list
	LOG(LogLevel::Info) << "Parsing portraits list";
	obj = parser_UTF8::doParseFile("portraits.txt");
	if (obj != NULL)
	{
		initPortraitMapping(obj, portraitMap);
	}

	map<int, int> leaderMap;

	for (auto sourceItr : sourceWorld->getCountries())
	{
		// don't convert rebels
		if (sourceItr.first == "REB")
		{
			continue;
		}

		HoI4Country* destCountry = NULL;
		const std::string& HoI4Tag = CountryMapper::getHoI4Tag(sourceItr.first);
		if (!HoI4Tag.empty())
		{
			std::string countryFileName = '/' + sourceItr.second->getName("english") + ".txt";
			destCountry = new HoI4Country(HoI4Tag, countryFileName, this, true);
			V2Party* rulingParty = sourceItr.second->getRulingParty(sourceWorld->getParties());
			if (rulingParty == NULL)
			{
				LOG(LogLevel::Error) << "Could not find the ruling party for " << sourceItr.first << ". Were all mods correctly included?";
				exit(-1);
			}
			destCountry->initFromV2Country(*sourceWorld, sourceItr.second, rulingParty->ideology, leaderMap, governmentJobs, namesMap, portraitMap, cultureMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, states->getProvinceToStateIDMap(), states->getStates());
			countries.insert(make_pair(HoI4Tag, destCountry));
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not convert V2 tag " << sourceItr.first << " to HoI4";
		}

		localisation.readFromCountry(sourceItr.second, HoI4Tag);
	}
	localisation.addNonenglishCountryLocalisations();
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
	for (auto zone : supplyZones)
	{
		auto filenameMap = supplyZonesFilenames.find(zone.first);
		if (filenameMap != supplyZonesFilenames.end())
		{
			zone.second->output(filenameMap->second);
		}
	}
}


void HoI4World::outputCountries() const
{
	for (auto country : countries)
	{
		country.second->output(states->getStates(), factions);
	}
}


void HoI4World::convertNavalBases()
{
	for (auto state: states->getStates())
	{
		state.second->convertNavalBases();
	}
}


void HoI4World::convertIndustry()
{
	LOG(LogLevel::Info) << "Converting industry";

	addStatesToCountries();

	map<string, double> factoryWorkerRatios = calculateFactoryWorkerRatios();
	putIndustryInStates(factoryWorkerRatios);

	calculateIndustryInCountries();
	reportIndustryLevels();
}


void HoI4World::addStatesToCountries()
{
	for (auto state: states->getStates())
	{
		auto owner = countries.find(state.second->getOwner());
		if (owner != countries.end())
		{
			owner->second->addState(state.second);
		}
	}

	for (auto country: countries)
	{
		if (country.second->getStates().size() > 0)
		{
			landedCountries.insert(country);
		}
	}
}


map<string, double> HoI4World::calculateFactoryWorkerRatios()
{
	map<string, double> industrialWorkersPerCountry = getIndustrialWorkersPerCountry();
	double totalWorldWorkers = getTotalWorldWorkers(industrialWorkersPerCountry);
	map<string, double> adjustedWorkersPerCountry = adjustWorkers(industrialWorkersPerCountry, totalWorldWorkers);
	double acutalWorkerFactoryRatio = getWorldwideWorkerFactoryRatio(adjustedWorkersPerCountry, totalWorldWorkers);

	map<string, double> factoryWorkerRatios;
	for (auto country: landedCountries)
	{
		auto adjustedWorkers = adjustedWorkersPerCountry.find(country.first);
		double factories = adjustedWorkers->second * acutalWorkerFactoryRatio;

		auto Vic2Country = country.second->getSourceCountry();
		long actualWorkers = Vic2Country->getEmployedWorkers();

		if (actualWorkers > 0)
		{
			factoryWorkerRatios.insert(make_pair(country.first, factories / actualWorkers));
		}
	}

	return factoryWorkerRatios;
}


map<string, double> HoI4World::getIndustrialWorkersPerCountry()
{
	map<string, double> industrialWorkersPerCountry;
	for (auto country: landedCountries)
	{
		auto Vic2Country = country.second->getSourceCountry();
		long employedWorkers = Vic2Country->getEmployedWorkers();
		if (employedWorkers > 0)
		{
			industrialWorkersPerCountry.insert(make_pair(country.first, employedWorkers));
		}
	}

	return industrialWorkersPerCountry;
}


double HoI4World::getTotalWorldWorkers(map<string, double> industrialWorkersPerCountry)
{
	double totalWorldWorkers = 0.0;
	for (auto countryWorkers: industrialWorkersPerCountry)
	{
		totalWorldWorkers += countryWorkers.second;
	}

	return totalWorldWorkers;
}


map<string, double> HoI4World::adjustWorkers(map<string, double> industrialWorkersPerCountry, double totalWorldWorkers)
{
	double meanWorkersPerCountry = totalWorldWorkers / industrialWorkersPerCountry.size();

	map<string, double> workersDelta;
	for (auto countryWorkers: industrialWorkersPerCountry)
	{
		double delta = countryWorkers.second - meanWorkersPerCountry;
		workersDelta.insert(make_pair(countryWorkers.first, delta));
	}

	map<string, double> adjustedWorkers;
	for (auto countryWorkers: industrialWorkersPerCountry)
	{
		double delta = workersDelta.find(countryWorkers.first)->second;
		double newWorkers = countryWorkers.second - Configuration::getIndustrialShapeFactor() * delta;
		adjustedWorkers.insert(make_pair(countryWorkers.first, newWorkers));
	}

	return adjustedWorkers;
}


double HoI4World::getWorldwideWorkerFactoryRatio(map<string, double> workersInCountries, double totalWorldWorkers)
{
	double baseIndustry = 0.0;
	for (auto countryWorkers: workersInCountries)
	{
		baseIndustry += countryWorkers.second * 0.000019;
	}

	double deltaIndustry = baseIndustry - (1189 - landedCountries.size());
	double newIndustry = baseIndustry - Configuration::getIcFactor() * deltaIndustry;
	double acutalWorkerFactoryRatio = newIndustry / totalWorldWorkers;

	return acutalWorkerFactoryRatio;
}


void HoI4World::putIndustryInStates(map<string, double> factoryWorkerRatios)
{
	for (auto HoI4State : states->getStates())
	{
		auto ratioMapping = factoryWorkerRatios.find(HoI4State.second->getOwner());
		if (ratioMapping == factoryWorkerRatios.end())
		{
			continue;
		}

		HoI4State.second->convertIndustry(ratioMapping->second);
	}
}


void HoI4World::calculateIndustryInCountries()
{
	for (auto country: countries)
	{
		country.second->calculateIndustry();
	}
}


void HoI4World::reportIndustryLevels()
{
	map<string, int> countryIndustry;
	int militaryFactories = 0;
	int civilialFactories = 0;
	int dockyards = 0;
	for (auto state: states->getStates())
	{
		militaryFactories += state.second->getMilFactories();
		civilialFactories += state.second->getCivFactories();
		dockyards += state.second->getDockyards();
	}

	LOG(LogLevel::Debug) << "Total factories: " << (militaryFactories + civilialFactories + dockyards);
	LOG(LogLevel::Debug) << "\t" << militaryFactories << " military factories";
	LOG(LogLevel::Debug) << "\t" << civilialFactories << " civilian factories";
	LOG(LogLevel::Debug) << "\t" << dockyards << " dockyards";

	if (Configuration::getICStats())
	{
		reportCountryIndustry();
		reportDefaultIndustry();
	}
}


void HoI4World::reportCountryIndustry()
{
	ofstream report("convertedIndustry.csv");
	report << "tag,military factories,civilian factories,dockyards,total factories\n";
	if (report.is_open())
	{
		for (auto country: countries)
		{
			country.second->reportIndustry(report);
		}
	}
}


void HoI4World::reportDefaultIndustry()
{
	map<string, array<int, 3>> countryIndustry;

	set<string> stateFilenames;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/history/states", stateFilenames);
	for (auto stateFilename: stateFilenames)
	{
		pair<string, array<int, 3>> stateData = getDefaultStateIndustry(stateFilename);

		auto country = countryIndustry.find(stateData.first);
		if (country == countryIndustry.end())
		{
			countryIndustry.insert(make_pair(stateData.first, stateData.second));
		}
		else
		{
			country->second[0] += stateData.second[0];
			country->second[1] += stateData.second[1];
			country->second[2] += stateData.second[2];
		}
	}

	outputDefaultIndustry(countryIndustry);
}


pair<string, array<int, 3>> HoI4World::getDefaultStateIndustry(string stateFilename)
{
	Object* fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/history/states/" + stateFilename);
	if (fileObj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/history/states/" << stateFilename;
		exit(-1);
	}
	auto stateObj = fileObj->getValue("state");
	auto historyObj = stateObj[0]->getValue("history");
	auto buildingsObj = historyObj[0]->getValue("buildings");

	auto civilianFactoriesObj = buildingsObj[0]->getValue("industrial_complex");
	int civilianFactories = 0;
	if (civilianFactoriesObj.size() > 0)
	{
		civilianFactories = stoi(civilianFactoriesObj[0]->getLeaf());
	}

	auto militaryFactoriesObj = buildingsObj[0]->getValue("arms_factory");
	int militaryFactories = 0;
	if (militaryFactoriesObj.size() > 0)
	{
		militaryFactories = stoi(militaryFactoriesObj[0]->getLeaf());
	}

	auto dockyardsObj = buildingsObj[0]->getValue("dockyard");
	int dockyards = 0;
	if (dockyardsObj.size() > 0)
	{
		dockyards = stoi(dockyardsObj[0]->getLeaf());
	}

	auto ownerObj = historyObj[0]->getValue("owner");
	string owner = ownerObj[0]->getLeaf();

	array<int, 3> industry = { militaryFactories, civilianFactories, dockyards };
	pair<string, array<int, 3>> stateData = make_pair(owner, industry);
	return stateData;
}


void HoI4World::outputDefaultIndustry(const map<string, array<int, 3>>& countryIndustry)
{
	ofstream report("defaultIndustry.csv");
	report << "tag,military factories,civilian factories,dockyards,total factories\n";
	if (report.is_open())
	{
		for (auto country: countryIndustry)
		{
			report << country.first << ',';
			report << country.second[0] << ',';
			report << country.second[1] << ',';
			report << country.second[2] << ',';
			report << country.second[0] + country.second[1] + country.second[2] << '\n';
		}
	}
}


void HoI4World::convertResources()
{
	Object* fileObj = parser_UTF8::doParseFile("resources.txt");
	if (fileObj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not read resources.txt";
		exit(-1);
	}

	auto resourcesObj = fileObj->getValue("resources");
	auto linksObj = resourcesObj[0]->getValue("link");

	map<int, map<string, double> > resourceMap;
	for (auto linkObj : linksObj)
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
		for (auto resource : actualResources)
		{
			string	resourceName = resource->getKey();
			double	amount = stof(resource->getLeaf());
			mapping->second[resourceName] += amount;
		}
	}

	for (auto state : states->getStates())
	{
		for (auto provinceNumber : state.second->getProvinces())
		{
			auto mapping = resourceMap.find(provinceNumber);
			if (mapping != resourceMap.end())
			{
				for (auto resource : mapping->second)
				{
					state.second->addResource(resource.first, resource.second);
				}
			}
		}
	}
}


void HoI4World::convertSupplyZones()
{
	for (auto state : states->getStates())
	{
		for (auto province : state.second->getProvinces())
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
	for (auto state : states->getStates())
	{
		// figure out which strategic regions are represented
		map<int, int> usedRegions;	// region ID -> number of provinces in that region
		for (auto province : state.second->getProvinces())
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
		int mostProvinces = 0;
		int bestRegion = 0;
		for (auto region : usedRegions)
		{
			if (region.second > mostProvinces)
			{
				bestRegion = region.first;
				mostProvinces = region.second;
			}
		}

		// add the state's province to the region
		auto region = strategicRegions.find(bestRegion);
		if (region == strategicRegions.end())
		{
			LOG(LogLevel::Warning) << "Strategic region " << bestRegion << " was not in the list of regions.";
			continue;
		}
		for (auto province : state.second->getProvinces())
		{
			region->second->addNewProvince(province);
		}
	}

	// add leftover provinces back to their strategic regions
	for (auto mapping : provinceToStratRegionMap)
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
	LOG(LogLevel::Info) << "Converting techs";

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
	for (auto itr : objs)
	{
		vector<string> keys = itr->getKeys();
		int status = 0; // 0 = unhandled, 1 = tech, 2 = invention
		vector<pair<string, int> > targetTechs;
		string tech = "";
		for (auto master : keys)
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
				int value = stoi(itr->getLeaf(master));
				targetTechs.push_back(pair<string, int>(master, value));
			}
		}
		switch (status)
		{
		case 0:
			LOG(LogLevel::Error) << "unhandled tech link with first key " << keys[0] << "!";
			break;
		case 1:
			techTechMap[tech] = targetTechs;
			break;
		case 2:
			invTechMap[tech] = targetTechs;
			break;
		}
	}


	for (auto dstCountry : countries)
	{
		const V2Country*	sourceCountry = dstCountry.second->getSourceCountry();
		vector<string>	techs = sourceCountry->getTechs();

		for (auto techName : techs)
		{
			auto mapItr = techTechMap.find(techName);
			if (mapItr != techTechMap.end())
			{
				for (auto HoI4TechItr : mapItr->second)
				{
					dstCountry.second->setTechnology(HoI4TechItr.first, HoI4TechItr.second);
				}
			}
		}

		auto srcInventions = sourceCountry->getInventions();
		for (auto invItr : srcInventions)
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


vector<int> HoI4World::getPortProvinces(const vector<int>& locationCandidates)
{
	vector<int> newLocationCandidates;
	for (auto litr : locationCandidates)
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
			for (auto thisAdjacency : adjacencies)
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


void HoI4World::convertArmies(const HoI4AdjacencyMapping& HoI4AdjacencyMap)
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


void HoI4World::generateLeaders()
{
	LOG(LogLevel::Info) << "Generating Leaders";

	for (auto country : countries)
	{
		country.second->generateLeaders(leaderTraits, namesMap, portraitMap);
	}
}


void HoI4World::convertArmies()
{
	LOG(LogLevel::Info) << "Converting armies and navies";

	for (auto country : countries)
	{
		country.second->convertArmyDivisions();
	}
}


void HoI4World::convertNavies()
{
	for (auto country : countries)
	{
		country.second->convertNavy(states->getStates());
	}
}


void HoI4World::convertAirforces()
{
	for (auto country : countries)
	{
		country.second->convertAirforce();
	}
}


void HoI4World::getGreatPowers()
{
	for (auto greatPowerVic2Tag: sourceWorld->getGreatPowers())
	{
		string greatPowerTag = CountryMapper::getHoI4Tag(greatPowerVic2Tag);
		auto greatPower = countries.find(greatPowerTag);
		if (greatPower != countries.end())
		{
			greatPowers.push_back(greatPower->second);
			greatPower->second->setGreatPower();
		}
	}
}


void HoI4World::convertCapitalVPs()
{
	LOG(LogLevel::Info) << "Adding bonuses to capitals";

	addBasicCapitalVPs();
	addGreatPowerVPs();
	addStrengthVPs();
}


void HoI4World::convertAirBases()
{
	for (auto state: states->getStates())
	{
		int numFactories = (state.second->getCivFactories() + state.second->getMilFactories()) / 4;
		state.second->addAirBase(numFactories);

		if (state.second->getInfrastructure() > 5)
		{
			state.second->addAirBase(1);
		}
	}

	for (auto country: countries)
	{
		auto capitalState = country.second->getCapital();
		if (capitalState != nullptr)
		{
			capitalState->addAirBase(5);
		}
	}
	for (auto greatPower: greatPowers)
	{
		auto capitalState = greatPower->getCapital();
		if (capitalState != nullptr)
		{
			capitalState->addAirBase(5);
		}
	}
}


void HoI4World::addBasicCapitalVPs()
{
	for (auto countryItr: countries)
	{
		countryItr.second->addVPsToCapital(5);
	}
}


void HoI4World::addGreatPowerVPs()
{
	for (auto greatPower: greatPowers)
	{
		greatPower->addVPsToCapital(5);
	}
}


void HoI4World::addStrengthVPs()
{
	double greatestStrength = getStrongestCountryStrength();
	for (auto country: countries)
	{
		int VPs = calculateStrengthVPs(country.second, greatestStrength);
		country.second->addVPsToCapital(VPs);
	}
}


double HoI4World::getStrongestCountryStrength()
{
	double greatestStrength = 0.0;
	for (auto country: countries)
	{
		double currentStrength = country.second->getStrengthOverTime(1.0);
		if (currentStrength > greatestStrength)
		{
			greatestStrength = currentStrength;
		}
	}

	return greatestStrength;
}


int HoI4World::calculateStrengthVPs(HoI4Country* country, double greatestStrength)
{
	double relativeStrength = country->getStrengthOverTime(1.0) / greatestStrength;
	return static_cast<int>(relativeStrength * 30.0);
}


void HoI4World::convertDiplomacy()
{
	LOG(LogLevel::Info) << "Converting diplomacy";

	for (auto agreement : sourceWorld->getDiplomacy()->getAgreements())
	{
		string HoI4Tag1 = CountryMapper::getHoI4Tag(agreement->country1);
		if (HoI4Tag1.empty())
		{
			continue;
		}
		string HoI4Tag2 = CountryMapper::getHoI4Tag(agreement->country2);
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
		if ((agreement->type == "alliance") || (agreement->type == "vassa"))
		{
			// copy agreement
			HoI4Agreement* HoI4a = new HoI4Agreement;
			HoI4a->country1 = HoI4Tag1;
			HoI4a->country2 = HoI4Tag2;
			HoI4a->start_date = agreement->start_date;
			HoI4a->type = agreement->type;
			diplomacy.addAgreement(HoI4a);

			if (agreement->type == "alliance")
			{
				HoI4Country1->second->editAllies().insert(HoI4Tag2);
				HoI4Country2->second->editAllies().insert(HoI4Tag1);
			}
		}
	}

	// Relations and guarantees
	for (auto country : countries)
	{
		for (auto relationItr : country.second->getRelations())
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
	for (auto province : provinces)
	{
		for (auto core : province.second->getCores())
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


void HoI4World::checkAllProvincesMapped()
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
		int provNum = stoi(line.substr(0, pos));
		if (provNum == 0)
		{
			continue;
		}

		auto num = provinceMapper::getHoI4ToVic2ProvinceMapping().find(provNum);
		if (num == provinceMapper::getHoI4ToVic2ProvinceMapping().end())
		{
			LOG(LogLevel::Warning) << "No mapping for HoI4 province " << provNum;
		}
	}

	definitions.close();
}


void HoI4World::fillCountryProvinces()
{
	for (auto country : countries)
	{
		country.second->setProvinceCount(0);
	}
	for (auto state : states->getStates())
	{
		auto ownercountry = countries.find(state.second->getOwner());
		for (auto prov : state.second->getProvinces())
		{
			//fix later
			ownercountry->second->setProvinceCount(ownercountry->second->getProvinceCount() + 1);
		}
	}
}
void HoI4World::setSphereLeaders(const V2World* sourceWorld)
{
	for (auto greatPower: greatPowers)
	{
		auto relations = greatPower->getRelations();
		for (auto relation : relations)
		{
			if (relation.second->getSphereLeader())
			{
				string tag = relation.second->getTag();
				auto spheredcountry = countries.find(tag);
				if (spheredcountry != countries.end())
				{
					spheredcountry->second->setSphereLeader(greatPower->getTag());
				}
			}
		}
	}
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


void HoI4World::outputRelations() const
{
	string opinion_modifiers;
	for (auto country : countries)
	{
		string countryrelation;
		for (auto country2 : countries)
		{
			if (country.second->getTag() != country2.second->getTag() && country.second->getRelations(country2.second->getTag()) != NULL &&country2.second->getRelations(country2.second->getTag()) != NULL)
			{
				opinion_modifiers += country.second->getTag() + "_" + country2.second->getTag() + " = {\n\tvalue = " + to_string(country.second->getRelations(country2.second->getTag())->getRelations()*1.5) + "\n}\n";
				countryrelation += "add_opinion_modifier = { target = " + country2.second->getTag() + " modifier = " + country.second->getTag() + "_" + country2.second->getTag() + "}\n";
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

	string filename("Output/" + Configuration::getOutputName() + "/common/opinion_modifiers/01_opinion_modifiers.txt");
	ofstream out;
	out.open(filename);
	{
		out << "opinion_modifiers = {\n";
		out << opinion_modifiers;
		out << "}\n";
	}
	out.close();
}


void HoI4World::thatsgermanWarCreator()
{
	genericFocusTree = new HoI4FocusTree;
	genericFocusTree->addGenericFocusTree();

	//FIX ALL FIXMES AND ADD CONQUEST GOAL
	//MAKE SURE THIS WORKS
	//IMPROVE
	//MAKE ARMY STRENGTH CALCS MORE ACCURATE!!
	LOG(LogLevel::Info) << "Filling Map Information";
	determineProvinceOwners();
	fillCountryProvinces();
	LOG(LogLevel::Info) << "Filling province neighbors";
	fillProvinceNeighbors();
	LOG(LogLevel::Info) << "Creating Factions";
	createFactions();
	//outputting the country and factions

	for (auto AllGC: greatPowers)
	{
		int maxGCWars = 0;
		if ((AllGC->getGovernment() != "hms_government" || (AllGC->getGovernment() == "hms_government" && (AllGC->getRulingParty().war_pol == "jingoism" || AllGC->getRulingParty().war_pol == "pro_military"))) && AllGC->getGovernment() != "democratic")
		{
			map<double, HoI4Country*> GCDistance;
			vector<HoI4Country*> GCDistanceSorted;
			//get great countries with a distance
			for (auto GC: greatPowers)
			{
				set<string> Allies = AllGC->getAllies();
				if (std::find(Allies.begin(), Allies.end(), GC->getTag()) == Allies.end())
				{
					double distance = getDistanceBetweenCountries(AllGC, GC);
					if (distance < 2200)
						GCDistance.insert(make_pair(distance, GC));
				}
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
				if (maxGCWars < 1)
				{
					string thetag = GC->getTag();
					string HowToTakeGC = HowToTakeLand(GC, AllGC, 3);
					if (HowToTakeGC == "noactionneeded" || HowToTakeGC == "factionneeded" || HowToTakeGC == "morealliesneeded")
					{
						if (GC != AllGC)
						{
							int relations = AllGC->getRelations(GC->getTag())->getRelations();
							if (relations < 0)
							{
								string prereq = "";
								vector<HoI4Country*> tempvector;
								if (WorldTargetMap.find(GC) == WorldTargetMap.end())
								{
									tempvector.push_back(AllGC);
									WorldTargetMap.insert(make_pair(GC, tempvector));
								}
								if (WorldTargetMap.find(GC) != WorldTargetMap.end())
								{
									tempvector = WorldTargetMap.find(GC)->second;
									if (find(tempvector.begin(), tempvector.end(), AllGC) == tempvector.end())
										tempvector.push_back(AllGC);

									WorldTargetMap[GC] = tempvector;
								}
								maxGCWars++;
							}
						}
					}
				}
			}
		}
	}

	//Files To show results
	ofstream AILog("AI-log.txt");
	vector<HoI4Country*> LeaderCountries;
	//getting total strength of all factions

	double WorldStrength = 0;
	vector<HoI4Faction*> CountriesAtWar;


	//Initial Checks
	for (auto Faction : factions)
	{
		WorldStrength += GetFactionStrength(Faction, 3);
	}
	AILog << WorldStrength << endl;

	//check relevancies
	bool fascismIsRelevant = false;
	bool communismIsRelevant = false;
	for (auto Faction : factions)
	{
		//this might need to change to add factions together
		HoI4Country* Leader = Faction->getLeader();
		if (Leader->getGovernment() == "absolute_monarchy" || Leader->getGovernment() == "fascism")
		{
			if (GetFactionStrength(Faction, 3) > WorldStrength*0.1)
			{
				fascismIsRelevant = true;
			}
		}

		if (Leader->getGovernment() == "communism" || Leader->getGovernment() == "syndicalism")
		{
			if (GetFactionStrength(Faction, 3) > WorldStrength*0.1)
			{
				communismIsRelevant = true;
			}
		}
	}

	if (fascismIsRelevant)
	{
		AILog << "Fascism is Relevant\n";
	}
	if (communismIsRelevant)
	{
		AILog << "Communist is Relevant\n";
	}
	AILog << "\n";

	//time to do events for coms and fascs if they are relevant
	LOG(LogLevel::Info) << "Calculating Fasc/Com AI";
	for (auto leader: greatPowers)
	{
		LeaderCountries.push_back(leader);
		if ((leader->getGovernment() == "fascism") || leader->getRulingIdeology() == "fascism")
		{
			vector <HoI4Faction*> newCountriesatWar;
			newCountriesatWar = FascistWarMaker(leader, sourceWorld);
			for (auto addedFactions : newCountriesatWar)
			{
				if (std::find(CountriesAtWar.begin(), CountriesAtWar.end(), addedFactions) == CountriesAtWar.end())
				{
					CountriesAtWar.push_back(addedFactions);
				}
			}
		}
		if (leader->getGovernment() == "absolute_monarchy" || (leader->getGovernment() == "prussian_constitutionalism" && leader->getRulingParty().war_pol == "jingoism"))
		{
			vector <HoI4Faction*> newCountriesatWar;
			newCountriesatWar = MonarchyWarCreator(leader);
			for (auto addedFactions : newCountriesatWar)
			{
				if (std::find(CountriesAtWar.begin(), CountriesAtWar.end(), addedFactions) == CountriesAtWar.end()) {
					CountriesAtWar.push_back(addedFactions);
				}
			}
		}
		if ((leader->getGovernment() == "communism"))
		{
			vector <HoI4Faction*> newCountriesatWar;
			newCountriesatWar = CommunistWarCreator(leader, sourceWorld);
			for (auto addedFactions : newCountriesatWar)
			{
				if (std::find(CountriesAtWar.begin(), CountriesAtWar.end(), addedFactions) == CountriesAtWar.end()) {
					CountriesAtWar.push_back(addedFactions);
				}
			}
		}
	}
	double CountriesAtWarStrength = 0.0;
	AILog << "initial conversion complete, checking who is at war:\n";
	for (auto faction : CountriesAtWar)
	{
		AILog << faction->getLeader()->getSourceCountry()->getName("english") + " with strength of " + to_string(GetFactionStrength(faction, 3)) << "\n";
		CountriesAtWarStrength += GetFactionStrength(faction, 3);
	}
	AILog << "percentage of world at war" + to_string(CountriesAtWarStrength / WorldStrength) + "\n";
	if (CountriesAtWarStrength / WorldStrength < 0.8)
	{
		AILog << "looking for democracies\n";
		//Lets find out countries Evilness
		for (auto GC: greatPowers)
		{
			if ((GC->getGovernment() == "hms_government" && (GC->getRulingParty().war_pol == "pacifism" || GC->getRulingParty().war_pol == "anti_military")) || GC->getGovernment() == "democratic")
			{
				AILog << "added a Democracy to make more wars " + GC->getSourceCountry()->getName("english") << "\n";
				vector <HoI4Faction*> newCountriesatWar;
				newCountriesatWar = DemocracyWarCreator(GC, sourceWorld);
				//add that faction to new countries at war
				for (auto addedFactions : newCountriesatWar)
				{
					if (std::find(CountriesAtWar.begin(), CountriesAtWar.end(), addedFactions) == CountriesAtWar.end()) {
						CountriesAtWar.push_back(addedFactions);
					}
				}
			}
		}
	}
	if (CountriesAtWarStrength / WorldStrength < 0.8)
	{
		//Lets find out countries Evilness
		map<double, HoI4Country*> GCEvilness;
		vector<HoI4Country*> GCEvilnessSorted;
		for (auto GC: greatPowers)
		{
			if (GC->getGovernment() == "prussian_constitutionalism" || GC->getGovernment() == "hms_government" || GC->getGovernment() == "absolute_monarchy" && std::find(LeaderCountries.begin(), LeaderCountries.end(), GC) == LeaderCountries.end() && (GC->getGovernment() != "hms_government" || (GC->getGovernment() == "hms_government" && (GC->getRulingParty().war_pol == "jingoism" || GC->getRulingParty().war_pol == "pro_military"))) && GC->getGovernment() != "democratic")
			{
				double v1 = rand() % 95 + 1;
				v1 = v1 / 100;
				double evilness = v1;
				string government = "";
				if (GC->getGovernment() == "absolute_monarchy")
					evilness += 3;
				else if (GC->getGovernment() == "prussian_constitutionalism")
					evilness += 2;
				else if (GC->getGovernment() == "hms_government")
					evilness += 1;
				HoI4Party countryrulingparty = GC->getRulingParty();

				if (countryrulingparty.war_pol == "jingoism")
					evilness += 3;
				else if (countryrulingparty.war_pol == "pro_military")
					evilness += 2;
				else if (countryrulingparty.war_pol == "anti_military")
					evilness += 1;

				//need to add ruling party to factor
				GCEvilness.insert(make_pair(evilness, GC));
			}
		}
		//put them into a vector so we know their order
		for (auto iterator = GCEvilness.begin(); iterator != GCEvilness.end(); ++iterator)
		{
			GCEvilnessSorted.push_back(iterator->second);
		}
		for (int i = GCEvilnessSorted.size() - 1; i > 0; i--)
		{
			AILog << "added country to make more wars " + GCEvilnessSorted[i]->getSourceCountry()->getName("english") << "\n";
			vector <HoI4Faction*> newCountriesatWar;
			newCountriesatWar = MonarchyWarCreator(GCEvilnessSorted[i]);
			//add that faction to new countries at war
			for (auto addedFactions : newCountriesatWar)
			{
				if (std::find(CountriesAtWar.begin(), CountriesAtWar.end(), addedFactions) == CountriesAtWar.end()) {
					CountriesAtWar.push_back(addedFactions);
				}
			}
			//then check how many factions are now at war
			AILog << "countries at war:\n";
			CountriesAtWarStrength = 0;
			for (auto faction : CountriesAtWar)
			{
				CountriesAtWarStrength += GetFactionStrength(faction, 3);
				AILog << faction->getLeader()->getSourceCountry()->getName("english") + " with strength of " + to_string(GetFactionStrength(faction, 3)) << "\n";
			}
			AILog << "percentage of world at war" + to_string(CountriesAtWarStrength / WorldStrength) << "\n";
			if (CountriesAtWarStrength / WorldStrength >= 0.8)
			{
				break;
			}
		}
	}
	AILog << aiOutputLog;
	AILog.close();
}

string HoI4World::HowToTakeLand(HoI4Country* TargetCountry, HoI4Country* AttackingCountry, double time)
{
	string s;
	string type;
	if (TargetCountry != AttackingCountry)
	{
		HoI4Faction* targetFaction = findFaction(TargetCountry);
		vector<HoI4Country*> moreAllies = GetMorePossibleAllies(AttackingCountry);
		HoI4Faction* myFaction = findFaction(AttackingCountry);
		//right now assumes you are stronger then them

		double myFactionDisStrength = GetFactionStrengthWithDistance(AttackingCountry, myFaction->getMembers(), time);
		double enemyFactionDisStrength = GetFactionStrengthWithDistance(TargetCountry, targetFaction->getMembers(), time);
		//lets check if I am stronger then their faction
		if (AttackingCountry->getStrengthOverTime(time) >= GetFactionStrength(targetFaction, static_cast<int>(time)))
		{
			//we are stronger, and dont even need ally help
			//ADD CONQUEST GOAL
			type = "noactionneeded";
			s += "Can kill " + TargetCountry->getSourceCountry()->getName("english") + " by ourselves\n\t I have a strength of " + to_string(AttackingCountry->getStrengthOverTime(time));
			s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName("english") + " has a strength of " + to_string(TargetCountry->getStrengthOverTime(time));
			s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + "\n";
		}
		else
		{
			//lets check if my faction is stronger

			if (myFactionDisStrength >= enemyFactionDisStrength)
			{
				//ADD CONQUEST GOAL
				type = "factionneeded";
				s += "Can kill " + TargetCountry->getSourceCountry()->getName("english") + " with our faction\n\t I have a strength of " + to_string(AttackingCountry->getStrengthOverTime(time));
				s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName("english") + " has a strength of " + to_string(TargetCountry->getStrengthOverTime(time));
				s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + "\n";
			}
			else
			{
				//FIXME
				//hmm I am still weaker, maybe need to look for allies?
				type = "morealliesneeded";
				//targettype.insert(make_pair("newallies", moreAllies));
				myFactionDisStrength = GetFactionStrengthWithDistance(AttackingCountry, myFaction->getMembers(), time) + GetFactionStrengthWithDistance(AttackingCountry, moreAllies, time);
				enemyFactionDisStrength = GetFactionStrengthWithDistance(TargetCountry, targetFaction->getMembers(), time);
				if (GetFactionStrengthWithDistance(AttackingCountry, myFaction->getMembers(), time) >= GetFactionStrengthWithDistance(TargetCountry, targetFaction->getMembers(), time))
				{
					//ADD CONQUEST GOAL
					s += "Can kill " + TargetCountry->getSourceCountry()->getName("english") + " with our faction Once I have more allies\n\t I have a strength of " + to_string(AttackingCountry->getStrengthOverTime(1.0));
					s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName("english") + " has a strength of " + to_string(TargetCountry->getStrengthOverTime(1.0));
					s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + "\n";
				}
				else
				{
					//Time to Try Coup
					type = "coup";
					s += "Cannot kill " + TargetCountry->getSourceCountry()->getName("english") + ", time to try coup\n";
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
			if (getDistanceBetweenCountries(CountryThatWantsAllies, country2) <= 500)
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
				int rel = relationswithposally->getRelations();
				int size = findFaction(CountriesWithin500Miles[i])->getMembers().size();
				double armysize = CountriesWithin500Miles[i]->getStrengthOverTime(1.0);
				//for now can only ally with people not in a faction, and must be worth adding
				if (relationswithposally->getRelations() >= -50 && findFaction(CountriesWithin500Miles[i])->getMembers().size() <= 1)
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
					if (relationswithposally->getRelations() >= 0)
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


double HoI4World::getDistanceBetweenCountries(const HoI4Country* country1, const HoI4Country* country2)
{
	if (!bothCountriesHaveCapitals(country1, country2))
	{
		return 100000;
	}

	pair<int, int> country1Position = getCapitalPosition(country1);
	pair<int, int> country2Position = getCapitalPosition(country2);

	return getDistanceBetweenPoints(country1Position, country2Position);
}


bool HoI4World::bothCountriesHaveCapitals(const HoI4Country* Country1, const HoI4Country* Country2)
{
	return (Country1->getCapitalProv() != 0) && (Country2->getCapitalProv() != 0);
}


pair<int, int> HoI4World::getCapitalPosition(const HoI4Country* country)
{
	auto capitalState = states->getStates().find(country->getCapitalProv());
	int capitalProvince = *(capitalState->second->getProvinces().begin());
	return getProvincePosition(capitalProvince);
}


pair<int, int> HoI4World::getProvincePosition(int provinceNum)
{
	if (provincePositions.size() == 0)
	{
		establishProvincePositions();
	}

	auto itr = provincePositions.find(provinceNum);
	return itr->second;
}


void HoI4World::establishProvincePositions()
{
	ifstream positionsFile("positions.txt");
	if (!positionsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open positions.txt";
		exit(-1);
	}

	string line;
	while (getline(positionsFile, line))
	{
		processPositionLine(line);
	}

	positionsFile.close();
}


void HoI4World::processPositionLine(const string& line)
{
	vector<string> tokenizedLine = tokenizeLine(line);
	addProvincePosition(tokenizedLine);
}


void HoI4World::addProvincePosition(const vector<string>& tokenizedLine)
{
	int province = stoi(tokenizedLine[0]);
	int x = stoi(tokenizedLine[2]);
	int y = stoi(tokenizedLine[4]);

	provincePositions.insert(make_pair(province, make_pair(x, y)));
}


vector<string> HoI4World::tokenizeLine(const string& line)
{
	vector<string> parts;
	stringstream ss(line);
	string tok;
	while (getline(ss, tok, ';'))
	{
		parts.push_back(tok);
	}

	return parts;
}


double HoI4World::getDistanceBetweenPoints(pair<int, int> point1, pair<int, int> point2)
{
	int xDistance = abs(point2.first - point1.first);
	if (xDistance > 2625)
	{
		xDistance = 5250 - xDistance;
	}

	int yDistance = point2.second - point1.second;

	return sqrt(pow(xDistance, 2) + pow(yDistance, 2));
}


double HoI4World::GetFactionStrengthWithDistance(HoI4Country* HomeCountry, vector<HoI4Country*> Faction, double time)
{
	double strength = 0.0;
	for (auto country: Faction)
	{
		double distanceMulti = 1;
		if (country == HomeCountry)
		{
			distanceMulti = 1;
		}
		else
			distanceMulti = getDistanceBetweenCountries(HomeCountry, country);

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

		strength += country->getStrengthOverTime(time) * distanceMulti;
	}
	return strength;
}
HoI4Faction* HoI4World::findFaction(HoI4Country* CheckingCountry)
{
	for (auto faction : factions)
	{
		vector<HoI4Country*> FactionMembers = faction->getMembers();
		if (std::find(FactionMembers.begin(), FactionMembers.end(), CheckingCountry) != FactionMembers.end())
		{
			//if country is in faction list, it is part of that faction
			return faction;
		}
	}
	vector<HoI4Country*> myself;
	myself.push_back(CheckingCountry);
	HoI4Faction* newFaction = new HoI4Faction(CheckingCountry, myself);
	CheckingCountry->setFaction(newFaction);
	return newFaction;
}


map<string, HoI4Country*> HoI4World::getNeighbors(const HoI4Country* checkingCountry)
{
	map<string, HoI4Country*> neighbors = getImmediateNeighbors(checkingCountry);
	if (neighbors.size() == 0)
	{
		neighbors = getNearbyCountries(checkingCountry);
	}

	return neighbors;
}


map<string, HoI4Country*> HoI4World::getImmediateNeighbors(const HoI4Country* checkingCountry)
{
	map<string, HoI4Country*> neighbors;

	for (auto province: checkingCountry->getProvinces())
	{
		for (int provinceNumber: provinceNeighbors.find(province)->second)
		{
			auto provinceToOwnerItr = provinceToOwnerMap.find(provinceNumber);
			if (provinceToOwnerItr == provinceToOwnerMap.end())
			{
				continue;
			}

			string ownerTag = provinceToOwnerItr->second;
			HoI4Country* ownerCountry = countries.find(ownerTag)->second;
			if (ownerCountry != checkingCountry)
			{
				neighbors.insert(make_pair(ownerTag, ownerCountry));
			}
		}
	}

	return neighbors;
}


map<string, HoI4Country*> HoI4World::getNearbyCountries(const HoI4Country* checkingCountry)
{
	map<string, HoI4Country*> neighbors;

	for (auto countryItr: countries)
	{
		HoI4Country* country = countryItr.second;
		if (country->getCapitalProv() != 0)
		{
			//IMPROVE
			//need to get further neighbors, as well as countries without capital in an area
			double distance = getDistanceBetweenCountries(checkingCountry, country);
			if (distance <= 500)
			{
				neighbors.insert(countryItr);
			}
		}
	}

	return neighbors;
}


void HoI4World::determineProvinceOwners()
{
	for (auto state: states->getStates())
	{
		for (auto province: state.second->getProvinces())
		{
			string owner = state.second->getOwner();
			provinceToOwnerMap.insert(make_pair(province, owner));
		}
	}
}


void HoI4World::createFactions()
{
	ofstream factionsLog("factions-logs.csv");
	factionsLog << "name,government,initial strength,factory strength per year,factory strength by 1939\n";

	for (auto leader: greatPowers)
	{
		if (leader->isInFaction())
		{
			continue;
		}
		factionsLog << "\n";

		vector<HoI4Country*> factionMembers;
		factionMembers.push_back(leader);

		string leaderGovernment = leader->getGovernment();
		logFactionMember(factionsLog, leader);
		double factionMilStrength = leader->getStrengthOverTime(3.0);

		for (auto allyTag: leader->getAllies())
		{
			auto ally = countries.find(allyTag);
			if (ally == countries.end())
			{
				continue;
			}

			HoI4Country* allycountry = ally->second;
			string allygovernment = allycountry->getGovernment();
			string sphereLeader = returnSphereLeader(allycountry);

			if ((sphereLeader == leader->getTag()) || ((sphereLeader == "") && governmentsAllowFaction(leaderGovernment, allygovernment)))
			{
				logFactionMember(factionsLog, allycountry);
				factionMembers.push_back(allycountry);

				factionMilStrength += allycountry->getStrengthOverTime(1.0);
			}
		}

		HoI4Faction* newFaction = new HoI4Faction(leader, factionMembers);
		for (auto member: factionMembers)
		{
			member->setFaction(newFaction);
		}
		factions.push_back(newFaction);

		factionsLog << "Faction Strength in 1939," << factionMilStrength << "\n";
	}

	factionsLog.close();
}


void HoI4World::logFactionMember(ofstream& factionsLog, const HoI4Country* member)
{
	factionsLog << member->getSourceCountry()->getName("english") << ",";
	factionsLog << member->getGovernment() << ",";
	factionsLog << member->getMilitaryStrength() << ",";
	factionsLog << member->getEconomicStrength(1.0) << ",";
	factionsLog << member->getEconomicStrength(3.0) << "\n";
}


bool HoI4World::governmentsAllowFaction(string leaderGovernment, string allyGovernment)
{
	if (leaderGovernment == allyGovernment)
	{
		return true;
	}
	else if (leaderGovernment == "absolute_monarchy" && (allyGovernment == "fascism" || allyGovernment == "democratic" || allyGovernment == "prussian_constitutionalism" || allyGovernment == "hms_government"))
	{
		return true;
	}
	else if (leaderGovernment == "democratic" && (allyGovernment == "hms_government" || allyGovernment == "absolute_monarchy" || allyGovernment == "prussian_constitutionalism"))
	{
		return true;
	}
	else if (leaderGovernment == "prussian_constitutionalism" && (allyGovernment == "hms_government" || allyGovernment == "absolute_monarchy" || allyGovernment == "democratic" || allyGovernment == "fascism"))
	{
		return true;
	}
	else if (leaderGovernment == "hms_government" && (allyGovernment == "democratic" || allyGovernment == "absolute_monarchy" || allyGovernment == "prussian_constitutionalism"))
	{
		return true;
	}
	else if (leaderGovernment == "communism" && (allyGovernment == "syndicalism"))
	{
		return true;
	}
	else if (leaderGovernment == "syndicalism" && (allyGovernment == "communism" || allyGovernment == "fascism"))
	{
		return true;
	}
	else if (leaderGovernment == "fascism" && (allyGovernment == "syndicalism" || allyGovernment == "absolute_monarchy" || allyGovernment == "prussian_constitutionalism"))
	{
		return true;
	}
	else
	{
		return false;
	}
}


double HoI4World::GetFactionStrength(HoI4Faction* Faction, int years)
{
	double strength = 0;
	for (auto country : Faction->getMembers())
	{
		strength += country->getStrengthOverTime(years);
	}
	return strength;
}


string HoI4World::returnSphereLeader(HoI4Country* possibleSphereling)
{
	for (auto greatPower: greatPowers)
	{
		auto relations = greatPower->getRelations();
		auto relation = relations.find(possibleSphereling->getTag());
		if (relation != relations.end())
		{
			if (relation->second->getSphereLeader())
			{
				return greatPower->getTag();
			}
		}
	}

	return "";
}


vector<HoI4Faction*> HoI4World::FascistWarMaker(HoI4Country* Leader, const V2World* sourceWorld)
{
	vector<HoI4Faction*> CountriesAtWar;
	LOG(LogLevel::Info) << "Calculating AI for " + Leader->getSourceCountry()->getName("english");
	//too many lists, need to clean up
	vector<HoI4Country*> Targets;
	vector<HoI4Country*> Anchluss;
	vector<HoI4Country*> Sudaten;
	vector<HoI4Country*> EqualTargets;
	vector<HoI4Country*> DifficultTargets;
	//getting country provinces and its neighbors
	map<string, HoI4Country*> AllNeighbors = getNeighbors(Leader);
	map<string, HoI4Country*> CloseNeighbors;
	//gets neighbors that are actually close to you
	for each (auto neigh in AllNeighbors)
	{
		if (neigh.second->getCapitalProv() != 0)
		{
			//IMPROVE
			//need to get further neighbors, as well as countries without capital in an area
			double distance = getDistanceBetweenCountries(Leader, neigh.second);
			if (distance <= 500)
				CloseNeighbors.insert(neigh);
		}
	}

	set<string> Allies = Leader->getAllies();
	//should add method to look for cores you dont own
	//should add method to look for more allies

	//lets look for weak neighbors
	LOG(LogLevel::Info) << "Doing Neighbor calcs for " + Leader->getSourceCountry()->getName("english");
	for (auto neigh : CloseNeighbors)
	{
		//lets check to see if they are not our ally and not a great country
		if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !neigh.second->isGreatPower())
		{
			volatile double enemystrength = neigh.second->getStrengthOverTime(1.5);
			volatile double mystrength = Leader->getStrengthOverTime(1.5);
			//lets see their strength is at least < 20%
			if (neigh.second->getStrengthOverTime(1.5) < Leader->getStrengthOverTime(1.5)*0.2 && findFaction(neigh.second)->getMembers().size() == 1)
			{
				//they are very weak
				Anchluss.push_back(neigh.second);
			}
			//if not, lets see their strength is at least < 60%
			else if (neigh.second->getStrengthOverTime(1.5) < Leader->getStrengthOverTime(1.0)*0.6 && neigh.second->getStrengthOverTime(1.0) > Leader->getStrengthOverTime(1.0)*0.2 && findFaction(neigh.second)->getMembers().size() == 1)
			{
				//they are weak and we can get 1 of these countries in sudaten deal
				Sudaten.push_back(neigh.second);
			}
			//if not, lets see their strength is at least = to ours%
			else if (neigh.second->getStrengthOverTime(1.0) < Leader->getStrengthOverTime(1.0))
			{
				//EqualTargets.push_back(neigh);
				EqualTargets.push_back(neigh.second);
			}
			//if not, lets see their strength is at least < 120%
			else if (neigh.second->getStrengthOverTime(1.0) < Leader->getStrengthOverTime(1.0)*1.2)
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
		if (type == "noactionneeded")
		{
			//too many vectors, need to clean up
			nan.push_back(target);
			anchlussnan.push_back(target);
		}
	}
	//gives us generic focus tree start
	HoI4FocusTree* FocusTree = genericFocusTree->makeCustomizedCopy(Leader);
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
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "The_third_way" + Leader->getTag();
		newFocus->icon     = "GFX_goal_support_fascism";
		newFocus->text     = "The Third Way!";
		newFocus->xPos     = x;
		newFocus->yPos     = 0;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 5";
		//FIXME 
		//Need to get Drift Defense to work
		//newFocus->completionReward += "			drift_defence_factor = 0.5\n";
		newFocus->completionReward += "			add_ideas = fascist_influence";
		FocusTree->addFocus(newFocus);

		//Focus to increase army support
		newFocus = new HoI4Focus;
		newFocus->id       = "mil_march" + Leader->getTag();
		newFocus->icon     = "GFX_goal_generic_allies_build_infantry";
		newFocus->text     = "Establish Military March Day";
		newFocus->prerequisites.push_back("focus = The_third_way" + Leader->getTag());
		newFocus->xPos     = x;
		newFocus->yPos     = 1;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 5";
		newFocus->completionReward += "			army_experience = 20\n";
		newFocus->completionReward += "			add_tech_bonus = {\n";
		newFocus->completionReward += "				bonus = 0.5\n";
		newFocus->completionReward += "				uses = 2\n";
		newFocus->completionReward += "				category = land_doctrine\n";
		newFocus->completionReward += "			}";
		FocusTree->addFocus(newFocus);

		for (unsigned int i = 0; i < 2; i++)
		{
			int start = 0;
			if (nan.size() >= 2)
				start = -1;
			if (i < nan.size())
			{
				//int x = i * 3;
				string annexername = Leader->getSourceCountry()->getName("english");
				string annexedname = nan[i]->getSourceCountry()->getName("english");
				findFaction(Leader)->addMember(nan[i]);
				//for random date
				int v1 = rand() % 5 + 1;
				int v2 = rand() % 5 + 1;
				//focus for anschluss
				newFocus = new HoI4Focus;
				newFocus->id       = Leader->getTag() + "_anschluss_" + nan[i]->getTag();
				newFocus->icon     = "GFX_goal_anschluss";
				newFocus->text     = "Union with " + annexedname;
				newFocus->available = "			" + nan[i]->getTag() + " = {\n";
				newFocus->available += "				is_in_faction = no\n";
				newFocus->available += "			}\n";
				newFocus->available += "			is_puppet = no\n";
				newFocus->available += "			date > 1937." + to_string(v1 + 5) + "." + to_string(v2 + 5);
				newFocus->prerequisites.push_back("focus = mil_march" + Leader->getTag());
				newFocus->xPos     = x + i * 2 + start;
				newFocus->yPos     = 2;
				newFocus->cost     = 10;
				newFocus->aiWillDo = "			factor = 10\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				date < 1937.6.6\n";
				newFocus->aiWillDo += "			}";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + nan[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + nan[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(EventNumber) + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);

				events.createAnnexEvent(Leader, nan[i]);
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
		if (type == "noactionneeded")
		{
			nan.push_back(target);
		}
	}
	if (nan.size() >= 1)
	{
		//if it can easily take these targets as they are not in an alliance, you can get annexation event

		//Focus to increase empire size more
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "Expand_the_Reich" + Leader->getTag();
		newFocus->icon     = "GFX_goal_generic_political_pressure";//something about claiming land
		newFocus->text     = "Expand the Reich";
		if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
		{
			//if there are anschlusses, make this event require at least 1 anschluss, else, its the start of a tree
			for (unsigned int i = 0; i < 2; i++)
			{
				if (i < anchlussnan.size())
				{
					newFocus->prerequisites.push_back("focus = " + Leader->getTag() + "_anschluss_" + anchlussnan[i]->getTag());
				}
			}
			newFocus->xPos     = takenSpots.back();
			newFocus->yPos     = 3;
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
			newFocus->xPos     = takenSpots.back();
			newFocus->yPos     = 0;
			takenSpotsy.push_back(2);
		}
		newFocus->cost     = 10;
		newFocus->aiWillDo = "	factor = 5";
		newFocus->completionReward += "			add_named_threat = { threat = 2 name = \"Fascist Expansion\" }";//give some claims or cores
		FocusTree->addFocus(newFocus);

		for (unsigned int i = 0; i < 1; i++)
		{
			if (i < nan.size())
			{
				int x = i * 3;
				string annexername = Leader->getSourceCountry()->getName("english");
				string annexedname = nan[i]->getSourceCountry()->getName("english");
				int v1 = rand() % 8 + 1;
				int v2 = rand() % 8 + 1;
				//focus for sudaten
				newFocus = new HoI4Focus;
				newFocus->id       = Leader->getTag() + "_sudaten_" + nan[i]->getTag();
				newFocus->icon     = "GFX_goal_anschluss";
				newFocus->text     = "Demand Territory from " + annexedname;
				newFocus->available = "		available = { " + nan[i]->getTag() + " = { is_in_faction = no }";
				newFocus->prerequisites.push_back("focus = Expand_the_Reich" + Leader->getTag());
				newFocus->available  = "			is_puppet = no\n";
				newFocus->available += "			date > 1938." + to_string(v1) + "." + to_string(v2);
				if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
				{
					//figuring out position again
					newFocus->xPos     = takenSpots.back();
					newFocus->yPos     = 4;
				}
				else
				{
					newFocus->xPos     = takenSpots.back();
					newFocus->yPos     = 1;
				}
				newFocus->cost     = 10;
				newFocus->aiWillDo = "	factor = 10\n";
				newFocus->aiWillDo += "	modifier = {\n";
				newFocus->aiWillDo += "		factor = 0\n";
				newFocus->aiWillDo += "		date < 1937.6.6\n";
				newFocus->aiWillDo += "	}";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + nan[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + nan[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(events.getCurrentNationFocusEventNum()) + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);

				//FINISH HIM
				newFocus = new HoI4Focus;
				newFocus->id       = Leader->getTag() + "_finish_" + nan[i]->getTag();
				newFocus->icon     = "GFX_goal_generic_territory_or_war";
				newFocus->text     = "Fate of " + annexedname;
				newFocus->available = nan[i]->getTag() + " = { is_in_faction = no }";
				newFocus->prerequisites.push_back("focus =  " + Leader->getTag() + "_sudaten_" + nan[i]->getTag());
				newFocus->available = "			is_puppet = no";
				if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
				{
					newFocus->xPos     = takenSpots.back();
					newFocus->yPos     = 5;
				}
				else
				{
					newFocus->xPos     = takenSpots.back();
					newFocus->yPos     = 2;
				}
				newFocus->cost     = 10;
				newFocus->aiWillDo = "	factor = 10\n";
				newFocus->aiWillDo += "	modifier = {\n";
				newFocus->aiWillDo += "		factor = 0\n";
				newFocus->aiWillDo += "		date < 1937.6.6\n";
				newFocus->aiWillDo += "	}";
				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = annex_everything\n";
				newFocus->completionReward += "				target = " + nan[i]->getTag() + "\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);

				//events
				//find neighboring states to take in sudaten deal
				set<string> demandedstates;
				for (auto leaderprov : Leader->getProvinces())
				{
					vector<int> thisprovNeighbors = provinceNeighbors.find(leaderprov)->second;
					for (int prov : thisprovNeighbors)
					{
						if (provinceToOwnerMap.find(prov) != provinceToOwnerMap.end())
						{
							string owner = provinceToOwnerMap.find(prov)->second;
							if (owner == nan[i]->getTag())
							{
								auto provinceToStateIdMapping = stateMapper::getStateIdMapping().find(prov);
								/* v does not contain x */
								demandedstates.insert(provinceToStateIdMapping->second);

							}
						}
					}
				}

				events.createSudatenEvent(Leader, nan[0], demandedstates);
			}
		}
		nan.clear();

	}
	//events for allies
	vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);
	for each (auto newally in newAllies)
	{
		findFaction(Leader)->addMember(newally);
	}
	if (newAllies.size() > 0)
	{
		//Focus to call summit, maybe have events from summit
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "Fas_Summit" + Leader->getTag();
		newFocus->icon     = "GFX_goal_generic_allies_build_infantry";
		newFocus->text     = "Call for the Fascist Summit";
		newFocus->xPos     = takenSpots.back() + 4;
		newFocus->yPos     = 0;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 2\n";
		newFocus->aiWillDo += "			modifier = {\n";
		newFocus->aiWillDo += "				factor = 10\n";
		newFocus->aiWillDo += "				date > 1938.1.1\n";
		newFocus->aiWillDo += "			}";
		//FIXME
		//newFocus->completionReward += "			opinion_gain_monthly_factor = 1.0";
		FocusTree->addFocus(newFocus);
	}

	for (unsigned int i = 0; i < newAllies.size(); i++)
	{
		int displacement = 0;
		if (newAllies.size() == 2)
			displacement = -1;
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "Alliance_" + newAllies[i]->getTag() + Leader->getTag();
		newFocus->icon     = "GFX_goal_generic_allies_build_infantry";
		newFocus->text     = "Alliance with " + newAllies[i]->getSourceCountry()->getName("english");
		newFocus->prerequisites.push_back("focus =  Fas_Summit" + Leader->getTag());
		newFocus->xPos     = takenSpots.back() + 4 + i * 2 + displacement;
		newFocus->yPos     = 1;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 10";
		newFocus->bypass = "			OR = {\n";
		newFocus->bypass += "				" + Leader->getTag() + " = { is_in_faction_with = " + newAllies[i]->getTag() + " }\n";
		newFocus->bypass += "				has_war_with = " + newAllies[i]->getTag() + "\n";
		newFocus->bypass += "				NOT = { country_exists = " + newAllies[i]->getTag() + " }\n";
		newFocus->bypass += "			}";
		newFocus->completionReward += "			" + newAllies[i]->getTag() + " = {\n";
		newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(events.getCurrentNationFocusEventNum()) + " }\n";
		newFocus->completionReward += "				add_opinion_modifier = { target = " + Leader->getTag() + " modifier = ger_ita_alliance_focus }\n";
		newFocus->completionReward += "			}";
		FocusTree->addFocus(newFocus);

		if (newAllies[i]->getFaction() == nullptr)
		{
			vector<HoI4Country*> self;
			self.push_back(newAllies[i]);
			HoI4Faction* newFaction = new HoI4Faction(newAllies[i], self);
			newAllies[i]->setFaction(newFaction);
		}
		events.createFactionEvents(Leader, newAllies[i]);

		i++;
	}

	vector<HoI4Faction*> FactionsAttackingMe;
	int maxGCAlliance = 0;
	if (WorldTargetMap.find(Leader) != WorldTargetMap.end())
	{
		for each (HoI4Country* country in WorldTargetMap.find(Leader)->second)
		{
			HoI4Faction* attackingFaction = findFaction(country);
			if (find(FactionsAttackingMe.begin(), FactionsAttackingMe.end(), attackingFaction) == FactionsAttackingMe.end())
			{
				FactionsAttackingMe.push_back(attackingFaction);
			}
		}
		double FactionsAttackingMeStrength = 0;
		for each (HoI4Faction* attackingFaction in FactionsAttackingMe)
		{
			FactionsAttackingMeStrength += GetFactionStrengthWithDistance(Leader, attackingFaction->getMembers(), 3);
		}
		aiOutputLog += Leader->getSourceCountry()->getName("english") + " is under threat, there are " + to_string(FactionsAttackingMe.size()) + " faction(s) attacking them, I have a strength of " + to_string(GetFactionStrength(findFaction(Leader), 3)) + " and they have a strength of " + to_string(FactionsAttackingMeStrength) + "\n";
		if (FactionsAttackingMeStrength > GetFactionStrength(findFaction(Leader), 3))
		{
			vector<HoI4Country*> GCAllies;

			for (HoI4Country* GC: greatPowers)
			{
				int relations = Leader->getRelations(GC->getTag())->getRelations();
				if (relations > 0 && maxGCAlliance < 1)
				{
					aiOutputLog += Leader->getSourceCountry()->getName("english") + " can attempt to ally " + GC->getSourceCountry()->getName("english") + "\n";
					HoI4Focus* newFocus = new HoI4Focus;
					newFocus->id       = "Alliance_" + GC->getTag() + Leader->getTag();
					newFocus->icon     = "GFX_goal_generic_allies_build_infantry";
					newFocus->text     = "Alliance with " + GC->getSourceCountry()->getName("english");
					newFocus->prerequisites.push_back("focus = Fas_Summit" + Leader->getTag());
					newFocus->xPos     = takenSpots.back() + 6;
					newFocus->yPos     = 2;
					newFocus->cost     = 15;
					newFocus->aiWillDo = "	factor = 10";
					newFocus->bypass = "			OR = {\n";
					newFocus->bypass = "				" + Leader->getTag() + " = { is_in_faction_with = " + GC->getTag() + "}\n";
					newFocus->bypass = "				has_war_with = " + GC->getTag() + "\n";
					newFocus->bypass = "				NOT = { country_exists = " + GC->getTag() + " }\n";
					newFocus->bypass = "			}\n";
					newFocus->completionReward += "			" + GC->getTag() + " = {\n";
					newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(events.getCurrentNationFocusEventNum()) + " }\n";
					newFocus->completionReward += "				add_opinion_modifier = { target = " + Leader->getTag() + " modifier = ger_ita_alliance_focus }\n";
					newFocus->completionReward += "			}";
					FocusTree->addFocus(newFocus);

					if (GC->getFaction() == nullptr)
					{
						vector<HoI4Country*> self;
						self.push_back(GC);
						HoI4Faction* newFaction = new HoI4Faction(GC, self);
						GC->setFaction(newFaction);
					}
					events.createFactionEvents(Leader, GC);
					maxGCAlliance++;
				}
			}
		}
	}

	//Declaring war with Great Country
	map<double, HoI4Country*> GCDistance;
	vector<HoI4Country*> GCDistanceSorted;
	//get great countries with a distance
	for (auto GC: greatPowers)
	{
		double distance = getDistanceBetweenCountries(Leader, GC);
		if (distance < 2200)
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
			if (GC != Leader)
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
		int relations = Leader->getRelations(GC->getTag())->getRelations();
		if (relations < 0)
		{
			string prereq = "";
			set<string> Allies = Leader->getAllies();
			if (maxGCWars < 1 && std::find(Allies.begin(), Allies.end(), GC->getTag()) == Allies.end())
			{
				CountriesAtWar.push_back(findFaction(Leader));
				CountriesAtWar.push_back(findFaction(GCTargets[0]));
				CountriesAtWar.push_back(findFaction(GC));
				AggressorFactions.push_back((Leader));
				int y2 = 0;
				//figuring out location of WG
				if (newAllies.size() > 0)
				{
					y2 = 2;

					for (unsigned int i = 0; (i < 2) && (i < newAllies.size()); i++)
					{
						prereq += " focus = Alliance_" + newAllies[i]->getTag() + Leader->getTag();
					}
				}
				int v1 = rand() % 12 + 1;
				int v2 = rand() % 12 + 1;
				HoI4Focus* newFocus = new HoI4Focus;
				newFocus->id       = "War" + GC->getTag() + Leader->getTag();
				newFocus->icon     = "GFX_goal_generic_major_war";
				newFocus->text     = "War with " + GC->getSourceCountry()->getName("english");//change to faction name later
				if (prereq != "")
				{
					newFocus->prerequisites.push_back(prereq);
				}
				newFocus->available = "			has_war = no\n";
				newFocus->available += "			date > 1939." + to_string(v1) + "." + to_string(v2);
				newFocus->xPos     = takenSpots.back() + start + 3 + maxGCWars * 2;
				newFocus->yPos     = y2;
				//newFocus->yPos     = takenSpotsy.back() + 1;
				newFocus->cost     = 10;
				newFocus->aiWillDo = "			factor = " + to_string(10 - maxGCWars * 5) + "\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				strength_ratio = { tag = " + GC->getTag() + " ratio < 1 }\n";
				newFocus->aiWillDo += "			}";
				if (GCTargets.size() > 1)
				{
					newFocus->aiWillDo += "\n";

					//make ai have this as a 0 modifier if they are at war
					newFocus->aiWillDo += "			modifier = {\n";
					newFocus->aiWillDo += "				factor = 0\n";
					newFocus->aiWillDo += "				OR = {\n";
					for (unsigned int i2 = 0; i2 < GCTargets.size(); i2++)
					{
						if (GC != GCTargets[i2])
						{
							newFocus->aiWillDo += "					has_war_with = " + GC->getTag() + "\n";
						}

					}
					newFocus->aiWillDo += "				}\n";
					newFocus->aiWillDo += "			}";
				}
				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = annex_everything\n";
				newFocus->completionReward += "				target = " + GC->getTag() + "\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);
				maxGCWars++;
			}
		}
	}
	//insert these values in targetmap for use later possibly?
	//needs cleanup, too many vectors!
	TargetMap.insert(make_pair("noactionneeded", nan));
	TargetMap.insert(make_pair("factionneeded", fn));
	TargetMap.insert(make_pair("morealliesneeded", man));
	TargetMap.insert(make_pair("coup", coup));

	Leader->addNationalFocus(FocusTree);

	return CountriesAtWar;
}


vector<HoI4Faction*> HoI4World::CommunistWarCreator(HoI4Country* Leader, const V2World* sourceWorld)
{
	vector<HoI4Faction*> CountriesAtWar;
	//communism still needs great country war events
	LOG(LogLevel::Info) << "Calculating AI for " + Leader->getSourceCountry()->getName("english");
	LOG(LogLevel::Info) << "Calculating Neighbors for " + Leader->getSourceCountry()->getName("english");
	map<string, HoI4Country*> AllNeighbors = getNeighbors(Leader);
	map<string, HoI4Country*> Neighbors;
	for each (auto neigh in AllNeighbors)
	{
		if (neigh.second->getCapitalProv() != 0)
		{
			//IMPROVE
			//need to get further neighbors, as well as countries without capital in an area
			double distance = getDistanceBetweenCountries(Leader, neigh.second);
			if (distance <= 400)
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
	LOG(LogLevel::Info) << "Doing Neighbor calcs for " + Leader->getSourceCountry()->getName("english");
	for (auto neigh : Neighbors)
	{
		//lets check to see if they are our ally and not a great country
		if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !neigh.second->isGreatPower())
		{
			double com = 0;
			HoI4Faction* neighFaction = findFaction(neigh.second);
			for (auto party : neigh.second->getParties())
			{
				if (party.name.find("socialist") != string::npos || party.name.find("communist") != string::npos || party.name.find("anarcho_liberal") != string::npos)
					com += party.popularity;
			}
			if (com > 25 && neigh.second->getRulingParty().ideology != "communist" && HowToTakeLand(neigh.second, Leader, 2.5) == "coup")
			{
				//look for neighboring countries to spread communism too(Need 25 % or more Communism support), Prioritizing those with "Communism Allowed" Flags, prioritizing those who are weakest
				//	Method() Influence Ideology and Attempt Coup
				coups.push_back(neigh.second);
			}
			else if (neighFaction->getMembers().size() == 1 && neigh.second->getRulingParty().ideology != "communist")
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
	takenSpots.push_back(22);
	HoI4FocusTree* FocusTree = genericFocusTree->makeCustomizedCopy(Leader);
	if (coups.size() > 0)
	{
		if (coups.size() == 1)
		{
			takenSpots.push_back(24);
		}
		if (coups.size() >= 2)
		{
			takenSpots.push_back(25);
		}
		//Focus to increase Comm support and prereq for coups
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "Home_of_Revolution" + Leader->getTag();
		newFocus->icon     = "GFX_goal_support_communism";
		newFocus->text     = "Home of the Revolution";
		newFocus->xPos     = takenSpots.back();
		newFocus->yPos     = 0;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 5";
		//FIXME 
		//Need to get Drift Defense to work
		//newFocus->completionReward += "			drift_defence_factor = 0.5\n";
		newFocus->completionReward += "			add_ideas = communist_influence";
		FocusTree->addFocus(newFocus);

		for (unsigned int i = 0; i < 2; i++)
		{
			if (i < coups.size())
			{
				newFocus = new HoI4Focus;
				newFocus->id       = "Influence_" + coups[i]->getTag() + "_" + Leader->getTag();
				newFocus->icon     = "GFX_goal_generic_propaganda";
				newFocus->text     = "Influence " + coups[i]->getSourceCountry()->getName("english");
				newFocus->prerequisites.push_back("focus = Home_of_Revolution" + Leader->getTag());
				newFocus->xPos     = 24 + i * 2;
				newFocus->yPos     = 1;
				newFocus->cost     = 10;
				newFocus->aiWillDo = "			factor = 5";
				newFocus->completionReward += "			" + coups[i]->getTag() + " = {\n";
				newFocus->completionReward += "				if = {\n";
				newFocus->completionReward += "					limit = {\n";
				newFocus->completionReward += "						" + Leader->getTag() + " = {\n";
				newFocus->completionReward += "							has_government = fascism\n";
				newFocus->completionReward += "						}\n";
				newFocus->completionReward += "					}\n";
				newFocus->completionReward += "					add_ideas = fascist_influence\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				if = {\n";
				newFocus->completionReward += "					limit = {\n";
				newFocus->completionReward += "						" + Leader->getTag() + " = {\n";
				newFocus->completionReward += "							has_government = communism\n";
				newFocus->completionReward += "						}\n";
				newFocus->completionReward += "					}\n";
				newFocus->completionReward += "					add_ideas = communist_influence\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				if = {\n";
				newFocus->completionReward += "					limit = {\n";
				newFocus->completionReward += "						" + Leader->getTag() + " = {\n";
				newFocus->completionReward += "							has_government = democratic\n";
				newFocus->completionReward += "						}\n";
				newFocus->completionReward += "					}\n";
				newFocus->completionReward += "					add_ideas = democratic_influence\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				country_event = { id = generic.1 }\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);

				//Civil War
				newFocus = new HoI4Focus;
				newFocus->id       = "Coup_" + coups[i]->getTag() + "_" + Leader->getTag();
				newFocus->icon     = "GFX_goal_generic_demand_territory";
				newFocus->text     = "Civil War in " + coups[i]->getSourceCountry()->getName("english");
				newFocus->prerequisites.push_back("focus = Influence_" + coups[i]->getTag() + "_" + Leader->getTag());
				newFocus->available = "			" + coups[i]->getTag() + " = { communism > 0.5 }";
				newFocus->xPos     = 24 + i * 2;
				newFocus->yPos     = 2;
				newFocus->cost     = 10;
				newFocus->aiWillDo = "			factor = 5";
				newFocus->completionReward += "			" + coups[i]->getTag() + " = {\n";
				newFocus->completionReward += "				start_civil_war = {\n";
				newFocus->completionReward += "					ideology = communism\n";
				newFocus->completionReward += "					size = 0.5\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);
			}
		}
	}
	if (forcedtakeover.size() > 0)
	{

		//Strengthen Commitern
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "StrengthCom" + Leader->getTag();
		newFocus->icon     = "GFX_goal_support_communism";
		newFocus->text     = "Strengthen The Comintern";//change to faction name later
		newFocus->xPos     = takenSpots.back() + 5;//fixme
		newFocus->yPos     = 0;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 5";
		newFocus->completionReward += "			army_experience = 20\n";
		newFocus->completionReward += "			add_tech_bonus = {\n";
		newFocus->completionReward += "				bonus = 0.5\n";
		newFocus->completionReward += "				uses = 2\n";
		newFocus->completionReward += "				category = land_doctrine\n";
		newFocus->completionReward += "			}";
		FocusTree->addFocus(newFocus);

		newFocus = new HoI4Focus;
		newFocus->id       = "Inter_Com_Pres" + Leader->getTag();
		newFocus->icon     = "GFX_goal_generic_dangerous_deal";
		newFocus->text     = "International Communist Pressure";//change to faction name later
		newFocus->prerequisites.push_back("focus = StrengthCom" + Leader->getTag());
		newFocus->available = "			date > 1937.1.1";
		newFocus->xPos     = takenSpots.back() + 5;
		newFocus->yPos     = 1;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 5";
		newFocus->completionReward += "			add_named_threat = { threat = 2 name = \"Socialist World Republic\" }";
		//FIXME
		//maybe add some claims?
		FocusTree->addFocus(newFocus);

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
				newFocus = new HoI4Focus;
				newFocus->id       = "War" + TargetsbyIC[i]->getTag() + Leader->getTag();
				newFocus->icon     = "GFX_goal_generic_major_war";
				newFocus->text     = "War with " + TargetsbyIC[i]->getSourceCountry()->getName("english");//change to faction name later
				newFocus->prerequisites.push_back("focus = Inter_Com_Pres" + Leader->getTag());
				newFocus->available = "			date > 1938." + to_string(v1) + "." + to_string(v2);
				newFocus->xPos     = takenSpots.back() + 3 + i * 2;
				newFocus->yPos     = 2;
				newFocus->cost     = 10;
				newFocus->aiWillDo = "			factor = 5\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				strength_ratio = { tag = " + TargetsbyIC[i]->getTag() + " ratio < 1 }\n";
				newFocus->aiWillDo += "			}";
				if (TargetsbyIC.size() > 1)
				{
					newFocus->aiWillDo += "\n";
					newFocus->aiWillDo += "			modifier = {\n";
					newFocus->aiWillDo += "				factor = 0\n";
					newFocus->aiWillDo += "				OR = {\n";
					for (int i2 = 0; i2 < 3; i2++)
					{
						if (i != i2)
							newFocus->aiWillDo += "					has_war_with = " + TargetsbyIC[i]->getTag() + "\n";
					}
					newFocus->aiWillDo += "				}\n";
					newFocus->aiWillDo += "			}";
				}

				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = puppet_wargoal_focus\n";
				newFocus->completionReward += "				target = " + TargetsbyIC[i]->getTag() + "\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);
			}
		}
		takenSpots.push_back(takenSpots.back() + 6);
	}
	//events for allies
	vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);
	if (newAllies.size() > 0)
	{
		//Focus to call summit, maybe have events from summit
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "Com_Summit" + Leader->getTag();
		newFocus->icon     = "GFX_goal_generic_allies_build_infantry";
		newFocus->text     = "Call for the Communist Summit";
		newFocus->xPos     = takenSpots.back() + 3;
		newFocus->yPos     = 0;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 2\n";
		newFocus->aiWillDo = "			modifier = {\n";
		newFocus->aiWillDo = "			factor = 10\n";
		newFocus->aiWillDo = "			date > 1938.1.1\n";
		newFocus->aiWillDo = "			}";
		FocusTree->addFocus(newFocus);
	}

	int i = 0;
	for (auto newAlly: newAllies)
	{
		HoI4Focus* newFocus = new HoI4Focus;
		newFocus->id       = "Alliance_" + newAlly->getTag() + Leader->getTag();
		newFocus->icon     = "GFX_goal_generic_allies_build_infantry";
		newFocus->text     = "Alliance with " + newAlly->getSourceCountry()->getName("english");
		newFocus->prerequisites.push_back("focus = Com_Summit" + Leader->getTag());
		newFocus->xPos     = takenSpots.back() + 3 + i;
		newFocus->yPos     = 1;
		newFocus->cost     = 10;
		newFocus->aiWillDo = "			factor = 10";
		newFocus->bypass = "			OR = {\n";
		newFocus->bypass = "				" + Leader->getTag() + " = { is_in_faction_with = " + newAlly->getTag() + "}\n";
		newFocus->bypass = "				has_war_with = " + newAlly->getTag() + "\n";
		newFocus->bypass = "				NOT = { country_exists = " + newAlly->getTag() + " }\n";
		newFocus->bypass = "			}\n";
		newFocus->completionReward += "			" + newAlly->getTag() + " = {\n";
		newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(events.getCurrentNationFocusEventNum()) + " }\n";
		newFocus->completionReward += "				add_opinion_modifier = { target = " + Leader->getTag() + " modifier = ger_ita_alliance_focus }\n";
		newFocus->completionReward += "			}";
		FocusTree->addFocus(newFocus);

		if (newAlly->getFaction() == nullptr)
		{
			vector<HoI4Country*> self;
			self.push_back(newAlly);
			HoI4Faction* newFaction = new HoI4Faction(newAlly, self);
			newAlly->setFaction(newFaction);
		}
		events.createFactionEvents(Leader, newAlly);

		i++;
	}

	vector<HoI4Faction*> FactionsAttackingMe;
	int maxGCAlliance = 0;
	if (WorldTargetMap.find(Leader) != WorldTargetMap.end())
	{
		for each (HoI4Country* country in WorldTargetMap.find(Leader)->second)
		{
			HoI4Faction* attackingFaction = findFaction(country);
			if (find(FactionsAttackingMe.begin(), FactionsAttackingMe.end(), attackingFaction) == FactionsAttackingMe.end())
			{
				FactionsAttackingMe.push_back(attackingFaction);
			}
		}
		double FactionsAttackingMeStrength = 0;
		for each (HoI4Faction* attackingFaction in FactionsAttackingMe)
		{
			FactionsAttackingMeStrength += GetFactionStrengthWithDistance(Leader, attackingFaction->getMembers(), 3);
		}
		aiOutputLog += Leader->getSourceCountry()->getName("english") + " is under threat, there are " + to_string(FactionsAttackingMe.size()) + " faction(s) attacking them, I have a strength of " + to_string(GetFactionStrength(findFaction(Leader), 3)) + " and they have a strength of " + to_string(FactionsAttackingMeStrength) + "\n";
		if (FactionsAttackingMeStrength > GetFactionStrength(findFaction(Leader), 3))
		{
			vector<HoI4Country*> GCAllies;

			for (HoI4Country* GC: greatPowers)
			{
				int relations = Leader->getRelations(GC->getTag())->getRelations();
				if (relations > 0 && maxGCAlliance < 1)
				{
					aiOutputLog += Leader->getSourceCountry()->getName("english") + " can attempt to ally " + GC->getSourceCountry()->getName("english") + "\n";
					HoI4Focus* newFocus = new HoI4Focus;
					newFocus->id       = "Alliance_" + GC->getTag() + Leader->getTag();
					newFocus->icon     = "GFX_goal_generic_allies_build_infantry";
					newFocus->text     = "Alliance with " + GC->getSourceCountry()->getName("english");
					newFocus->prerequisites.push_back("focus = Com_Summit" + Leader->getTag());
					newFocus->xPos     = takenSpots.back() + 4;
					newFocus->yPos     = 2;
					newFocus->cost     = 15;
					newFocus->aiWillDo = "			factor = 10";
					newFocus->bypass = "			OR = {\n";
					newFocus->bypass = "				" + Leader->getTag() + " = { is_in_faction_with = " + GC->getTag() + "}\n";
					newFocus->bypass = "				has_war_with = " + GC->getTag() + "\n";
					newFocus->bypass = "				NOT = { country_exists = " + GC->getTag() + " }\n";
					newFocus->bypass = "			}\n";
					newFocus->completionReward += "			" + GC->getTag() + " = {\n";
					newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(events.getCurrentNationFocusEventNum()) + " }\n";
					newFocus->completionReward += "				add_opinion_modifier = { target = " + Leader->getTag() + " modifier = ger_ita_alliance_focus }\n";
					newFocus->completionReward += "			}";
					FocusTree->addFocus(newFocus);

					if (GC->getFaction() == nullptr)
					{
						vector<HoI4Country*> self;
						self.push_back(GC);
						HoI4Faction* newFaction = new HoI4Faction(GC, self);
						GC->setFaction(newFaction);
					}
					events.createFactionEvents(Leader, GC);
					maxGCAlliance++;
				}
			}
		}
	}


	//Declaring war with Great Country

	map<double, HoI4Country*> GCDistance;
	vector<HoI4Country*> GCDistanceSorted;
	for (auto GC: greatPowers)
	{
		double distance = getDistanceBetweenCountries(Leader, GC);
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
		int relations = Leader->getRelations(GC->getTag())->getRelations();
		if (relations < 0)
		{
			string prereq = "";
			if (maxGCWars < 1 && std::find(Allies.begin(), Allies.end(), GC->getTag()) == Allies.end())
			{
				CountriesAtWar.push_back(findFaction(Leader));
				CountriesAtWar.push_back(findFaction(GCTargets[0]));
				CountriesAtWar.push_back(findFaction(GC));
				AggressorFactions.push_back((Leader));
				int y2 = 0;
				//figuring out location of WG
				if (newAllies.size() > 0)
				{
					y2 = 2;

					for (unsigned int i = 0; (i < 2) && (i < newAllies.size()); i++)
						prereq += " focus = Alliance_" + newAllies[i]->getTag() + Leader->getTag();
				}
				int v1 = rand() % 12 + 1;
				int v2 = rand() % 12 + 1;
				HoI4Focus* newFocus = new HoI4Focus;
				newFocus->id       = "War" + GC->getTag() + Leader->getTag();
				newFocus->icon     = "GFX_goal_generic_major_war";
				newFocus->text     = "War with " + GC->getSourceCountry()->getName("english");//change to faction name later
				newFocus->prerequisites.push_back(prereq);
				newFocus->available = "			has_war = no\n";
				newFocus->available += "			date > 1939." + to_string(v1) + "." + to_string(v2);
				newFocus->xPos     = takenSpots.back() + 3 + maxGCWars * 2;
				newFocus->yPos     = y2 + maxGCAlliance;
				//newFocus->yPos     = takenSpotsy.back() + 1;
				newFocus->cost     = 10;
				newFocus->aiWillDo = "			factor = " + to_string(10 - maxGCWars * 5) + "\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "					factor = 0\n";
				newFocus->aiWillDo += "					strength_ratio = { tag = " + GC->getTag() + " ratio < 1 }\n";
				newFocus->aiWillDo += "			}";
				if (GCTargets.size() > 1)
				{
					newFocus->aiWillDo = "\n";

					//make ai have this as a 0 modifier if they are at war
					newFocus->aiWillDo += "			modifier = {\n";
					newFocus->aiWillDo += "				factor = 0\n";
					newFocus->aiWillDo += "				OR = {\n";
					for (int i2 = 0; i2 < 3; i2++)
					{
						if (GC != GCTargets[i2])
						{
							newFocus->aiWillDo += "					has_war_with = " + GC->getTag() + "\n";
						}

					}
					newFocus->aiWillDo += "				}\n";
					newFocus->aiWillDo += "			}\n";
				}

				newFocus->completionReward += "			create_wargoal = {\n";
				newFocus->completionReward += "				type = puppet_wargoal_focus\n";
				newFocus->completionReward += "				target = " + GC->getTag() + "\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);

				maxGCWars++;
			}
		}
	}

	Leader->addNationalFocus(FocusTree);

	return CountriesAtWar;
}


vector<HoI4Faction*> HoI4World::DemocracyWarCreator(HoI4Country* Leader, const V2World* sourceWorld)
{
	vector<HoI4Faction*> CountriesAtWar;
	map<int, HoI4Country*> CountriesToContain;
	vector<HoI4Country*> vCountriesToContain;
	set<string> Allies = Leader->getAllies();
	int v1 = rand() % 100;
	v1 = v1 / 100;
	HoI4FocusTree* FocusTree = genericFocusTree->makeCustomizedCopy(Leader);
	for (auto GC: greatPowers)
	{
		double relation = Leader->getRelations(GC->getTag())->getRelations();
		if (relation < 100 && (GC->getGovernment() != "hms_government" || (GC->getGovernment() == "hms_government" && (GC->getRulingParty().war_pol == "jingoism" || GC->getRulingParty().war_pol == "pro_military"))) && GC->getGovernment() != "democratic" && std::find(Allies.begin(), Allies.end(), GC->getTag()) == Allies.end())
		{
			string HowToTakeGC = HowToTakeLand(GC, Leader, 3);
			//if (HowToTakeGC == "noactionneeded" || HowToTakeGC == "factionneeded")
			{
				CountriesAtWar.push_back(findFaction(Leader));
				CountriesToContain.insert(make_pair(static_cast<int>(relation + v1), GC));
			}
		}
	}
	for (auto country : CountriesToContain)
	{
		vCountriesToContain.push_back(country.second);
	}
	if (vCountriesToContain.size() > 0)
	{
		FocusTree->addDemocracyNationalFocuses(Leader, vCountriesToContain, 27);
	}

	Leader->addNationalFocus(FocusTree);

	return CountriesAtWar;
}


vector<HoI4Faction*> HoI4World::MonarchyWarCreator(HoI4Country* country)
{
	HoI4FocusTree* focusTree = genericFocusTree->makeCustomizedCopy(country);

	LOG(LogLevel::Info) << "Doing neighbor calcs for " + country->getSourceCountry()->getName("english");

	vector<HoI4Country*> weakNeighbors = findWeakNeighbors(country);
	vector<HoI4Country*> weakColonies = findWeakColonies(country);
	focusTree->addMonarchyEmpireNationalFocuses(country, weakColonies, weakNeighbors);

	vector<HoI4Country*> greatPowerTargets = getGreatPowerTargets(country);
	vector<HoI4Faction*> CountriesAtWar = addGreatPowerWars(country, focusTree, greatPowerTargets);
	addTradeEvents(country, greatPowerTargets);

	country->addNationalFocus(focusTree);

	return CountriesAtWar;
}


vector<HoI4Country*> HoI4World::findWeakNeighbors(const HoI4Country* country)
{
	vector<HoI4Country*> weakNeighbors;

	auto allies = country->getAllies();
	for (auto neighbor: findCloseNeighbors(country))
	{
		if (allies.find(neighbor.second->getTag()) != allies.end())
		{
			continue;
		}
		if (neighbor.second->isGreatPower())
		{
			continue;
		}

		double enemystrength = neighbor.second->getStrengthOverTime(1.5);
		double mystrength = country->getStrengthOverTime(1.5);
		if (
				(enemystrength < (mystrength * 0.2)) &&
				(findFaction(neighbor.second)->getMembers().size() == 1)
			)
		{
			weakNeighbors.push_back(neighbor.second);
		}
	}

	return weakNeighbors;
}


map<string, HoI4Country*> HoI4World::findCloseNeighbors(const HoI4Country* country)
{
	map<string, HoI4Country*> closeNeighbors;

	for (auto neighbor: getNeighbors(country))
	{
		if (neighbor.second->getCapitalProv() != 0)
		{
			double distance = getDistanceBetweenCountries(country, neighbor.second);
			if (distance <= 500)
			{
				closeNeighbors.insert(neighbor);
			}
		}
	}

	return closeNeighbors;
}


vector<HoI4Country*> HoI4World::findWeakColonies(const HoI4Country* country)
{
	vector<HoI4Country*> weakColonies;

	auto allies = country->getAllies();
	for (auto neighbor: findFarNeighbors(country))
	{
		if (allies.find(neighbor.second->getTag()) != allies.end())
		{
			continue;
		}
		if (neighbor.second->isGreatPower())
		{
			continue;
		}

		double enemystrength = neighbor.second->getStrengthOverTime(1.5);
		double mystrength = country->getStrengthOverTime(1.5);
		if (
				(enemystrength < mystrength * 0.2) &&
				(findFaction(neighbor.second)->getMembers().size() == 1)
			)
		{
			weakColonies.push_back(neighbor.second);
		}
	}

	return weakColonies;
}


map<string, HoI4Country*> HoI4World::findFarNeighbors(const HoI4Country* country)
{
	map<string, HoI4Country*> farNeighbors;

	for (auto neighbor: getNeighbors(country))
	{
		if (neighbor.second->getCapitalProv() != 0)
		{
			double distance = getDistanceBetweenCountries(country, neighbor.second);
			if (distance > 500)
			{
				farNeighbors.insert(neighbor);
			}
		}
	}

	if (farNeighbors.size() == 0) // find all nearby countries
	{
		for (auto otherCountry: countries)
		{
			if (otherCountry.second->getCapitalProv() != 0)
			{
				double distance = getDistanceBetweenCountries(country, otherCountry.second);
				if ((distance <= 1000) && (otherCountry.second->getProvinceCount() > 0))
				{
					farNeighbors.insert(otherCountry);
				}
			}
		}
	}

	return farNeighbors;
}


vector<HoI4Country*> HoI4World::getGreatPowerTargets(HoI4Country* country)
{
	vector<HoI4Country*> greatPowerTargets;

	for (auto greatPower: getGPsByDistance(country))
	{
		string prereqsNeeded = HowToTakeLand(greatPower.second, country, 3);
		if (prereqsNeeded == "noactionneeded" || prereqsNeeded == "factionneeded")
		{
			if (greatPower.second != country)
			{
				greatPowerTargets.push_back(greatPower.second);
			}
		}
		if (prereqsNeeded == "morealliesneeded")
		{
		}
	}

	return greatPowerTargets;
}


map<double, HoI4Country*> HoI4World::getGPsByDistance(const HoI4Country* country)
{
	map<double, HoI4Country*> distanceToGPMap;
	for (auto greatPower: greatPowers)
	{
		double distance = getDistanceBetweenCountries(country, greatPower);
		if (distance < 1200)
		{
			distanceToGPMap.insert(make_pair(distance, greatPower));
		}
	}

	return distanceToGPMap;
}


vector<HoI4Faction*> HoI4World::addGreatPowerWars(HoI4Country* country, HoI4FocusTree* FocusTree, const vector<HoI4Country*>& greatPowerTargets)
{
	vector<HoI4Faction*> countriesAtWar;

	int numWarsWithGreatPowers = 0;
	for (auto target: greatPowerTargets)
	{
		if (numWarsWithGreatPowers >= 1)
		{
			break;
		}

		int relations = country->getRelations(target->getTag())->getRelations();
		if (relations >= 0)
		{
			continue;
		}

		set<string> Allies = country->getAllies();
		if (Allies.find(target->getTag()) == Allies.end())
		{
			countriesAtWar.push_back(findFaction(country));

			HoI4Focus* newFocus = new HoI4Focus;
			newFocus->id       = "War" + target->getTag() + country->getTag();
			newFocus->icon     = "GFX_goal_generic_major_war";
			newFocus->text     = "War with " + target->getSourceCountry()->getName("english");//change to faction name later
			newFocus->prerequisites.push_back("focus =  MilitaryBuildup" + country->getTag());
			newFocus->available = "			has_war = 20\n";
			newFocus->available += "			date > 1939.1.1";
			newFocus->xPos     = 31 + numWarsWithGreatPowers * 2;
			newFocus->yPos     = 5;
			newFocus->cost     = 10;
			newFocus->aiWillDo = "			factor = " + to_string(10 - numWarsWithGreatPowers * 5) + "\n";
			newFocus->aiWillDo += "			modifier = {\n";
			newFocus->aiWillDo += "				factor = 0\n";
			newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 0.8 }\n";
			newFocus->aiWillDo += "			}";
			if (greatPowerTargets.size() > 1) //make ai have this as a 0 modifier if they are at war
			{
				newFocus->aiWillDo += "\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				OR = {\n";
				for (auto target2: greatPowerTargets)
				{
					if (target != target2)
					{
						newFocus->aiWillDo += "					has_war_with = " + target2->getTag() + "\n";
					}
				}
				newFocus->aiWillDo += "				}\n";
				newFocus->aiWillDo += "			}";
			}
			newFocus->completionReward += "			create_wargoal = {\n";
			newFocus->completionReward += "				type = annex_everything\n";
			newFocus->completionReward += "				target = " + target->getTag() + "\n";
			newFocus->completionReward += "			}";
			FocusTree->addFocus(newFocus);

			numWarsWithGreatPowers++;
		}
	}

	return countriesAtWar;
}


void HoI4World::addTradeEvents(const HoI4Country* country, const vector<HoI4Country*>& greatPowerTargets)
{
	for (auto greatPowerTarget: greatPowerTargets)
	{
		int relations = country->getRelations(greatPowerTarget->getTag())->getRelations();
		if (relations >= 0)
		{
			continue;
		}

		events.createTradeEvent(country, greatPowerTarget);
	}
}//7467