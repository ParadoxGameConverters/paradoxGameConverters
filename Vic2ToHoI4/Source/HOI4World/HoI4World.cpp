/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include "ParadoxParserUTF8.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../Configuration.h"
#include "../V2World/V2Diplomacy.h"
#include "../V2World/V2Party.h"
#include "HoI4Advisor.h"
#include "HoI4Agreement.h"
#include "HoI4Buildings.h"
#include "HoI4Country.h"
#include "HoI4Diplomacy.h"
#include "HoI4Events.h"
#include "HoI4Faction.h"
#include "HoI4FocusTree.h"
#include "HOI4Ideology.h"
#include "HoI4Localisation.h"
#include "HoI4OnActions.h"
#include "HoI4Province.h"
#include "HoI4State.h"
#include "HoI4StrategicRegion.h"
#include "HoI4SupplyZones.h"
#include "HoI4WarCreator.h"
#include "../Mappers/CountryMapping.h"
#include <fstream>
using namespace std;




HoI4World::HoI4World(const V2World* _sourceWorld):
	sourceWorld(_sourceWorld),
	states(new HoI4States(sourceWorld)),
	supplyZones(new HoI4SupplyZones),
	strategicRegions(),
	buildings(new HoI4Buildings(states->getProvinceToStateIDMap())),
	countries(),
	landedCountries(),
	greatPowers(),
	ideologies(),
	majorIdeologies(),
	ideologicalLeaderTraits(),
	ideologicalAdvisors(),
	ideologicalIdeas(),
	factions(),
	diplomacy(new HoI4Diplomacy),
	events(new HoI4Events),
	onActions(new HoI4OnActions),
	divisionTemplates(),
	leaderTraits()
{
	LOG(LogLevel::Info) << "Parsing HoI4 data";

	convertNavalBases();
	convertCountries();
	HoI4Localisation::addStateLocalisations(states);
	convertIndustry();
	convertResources();
	supplyZones->convertSupplyZones(states);
	convertStrategicRegions();
	convertDiplomacy();
	convertTechs();
	convertMilitaries();
	//convertArmies();
	//convertNavies();
	//convertAirforces();
	determineGreatPowers();
	importIdeologies();
	importLeaderTraits();
	importIdeologicalIdeas();
	convertGovernments();
	identifyMajorIdeologies();
	importIdeologicalMinisters();
	convertParties();
	events->createPoliticalEvents(majorIdeologies);
	events->createWarJustificationEvents(majorIdeologies);
	events->createElectionEvents(majorIdeologies, onActions);
	addCountryElectionEvents(majorIdeologies);
	addNeutrality();
	convertIdeologySupport();
	convertCapitalVPs();
	convertAirBases();
	createFactions();

	HoI4WarCreator warCreator(this);
}


shared_ptr<HoI4Country> HoI4World::findCountry(const string& countryTag)
{
	auto country = countries.find(countryTag);
	if (country == countries.end())
	{
		return {};
	}

	return country->second;
}

void HoI4World::convertNavalBases()
{
	for (auto state: states->getStates())
	{
		state.second->convertNavalBases();
	}
}


void HoI4World::convertCountries()
{
	LOG(LogLevel::Info) << "Converting countries";

	//initLeaderTraitsMap(leaderTraits);
	personalityMap landPersonalityMap;
	personalityMap seaPersonalityMap;
	//initLeaderPersonalityMap(landPersonalityMap, seaPersonalityMap);

	backgroundMap landBackgroundMap;
	backgroundMap seaBackgroundMap;
	//initLeaderBackgroundMap(obj->getLeaves()[0], landBackgroundMap, seaBackgroundMap);

	map<int, int> leaderMap;

	for (auto sourceItr : sourceWorld->getCountries())
	{
		convertCountry(sourceItr, leaderMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap);
	}

	HoI4Localisation::addNonenglishCountryLocalisations();
}


void HoI4World::convertCountry(pair<string, V2Country*> country, map<int, int>& leaderMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	// don't convert rebels
	if (country.first == "REB")
	{
		return;
	}

	HoI4Country* destCountry = nullptr;
	auto possibleHoI4Tag = CountryMapper::getHoI4Tag(country.first);
	if (possibleHoI4Tag)
	{
		auto possibleCountryName = country.second->getName("english");
		string countryName;
		if (possibleCountryName)
		{
			countryName = *possibleCountryName;
		}
		else
		{
			LOG(LogLevel::Error) << "Could not set country name when converting country";
			exit(-1);
		}

		std::string countryFileName = Utils::convert8859_15ToUTF8(countryName) + ".txt";
		int pipe = countryFileName.find_first_of('|');
		while (pipe != string::npos)
		{
			countryFileName.replace(pipe, 1, "");
			pipe = countryFileName.find_first_of('|');
		}
		int greater = countryFileName.find_first_of('>');
		while (greater != string::npos)
		{
			countryFileName.replace(greater, 1, "");
			greater = countryFileName.find_first_of('>');
		}
		int lesser = countryFileName.find_first_of('<');
		while (lesser != string::npos)
		{
			countryFileName.replace(lesser, 1, "");
			lesser = countryFileName.find_first_of('>');
		}
		destCountry = new HoI4Country(*possibleHoI4Tag, countryFileName, this);

		destCountry->initFromV2Country(*sourceWorld, country.second, states->getProvinceToStateIDMap(), states->getStates());
		countries.insert(make_pair(*possibleHoI4Tag, destCountry));
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not convert V2 tag " << country.first << " to HoI4";
	}

	HoI4Localisation::createCountryLocalisations(make_pair(country.first, *possibleHoI4Tag));
}


void HoI4World::importIdeologies()
{
	if (Configuration::getIdeologiesOptions() != ideologyOptions::keep_default)
	{
		importIdeologyFile("converterIdeologies.txt");
	}
	importIdeologyFile(Configuration::getHoI4Path() + "/common/ideologies/00_ideologies.txt");
}


void HoI4World::importIdeologyFile(const string& filename)
{
	auto fileObject = parser_UTF8::doParseFile(filename);
	if (fileObject)
	{
		auto ideologiesObjects = fileObject->getLeaves();
		if (ideologiesObjects.size() > 0)
		{
			for (auto ideologyObject: ideologiesObjects[0]->getLeaves())
			{
				string ideologyName = ideologyObject->getKey();
				HoI4Ideology* newIdeology = new HoI4Ideology(ideologyObject);
				ideologies.insert(make_pair(ideologyName, newIdeology));
			}
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse " << filename;
		exit(-1);
	}
}


void HoI4World::importLeaderTraits()
{
	auto fileObject = parser_UTF8::doParseFile("converterLeaderTraits.txt");
	if (fileObject)
	{
		auto ideologyObjects = fileObject->getLeaves();
		for (auto ideologyObject: ideologyObjects)
		{
			string ideaName = ideologyObject->getKey();
			ideologicalLeaderTraits.insert(make_pair(ideaName, ideologyObject->getLeaves()));
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse converterLeaderTraits.txt";
	}
}


void HoI4World::importIdeologicalMinisters()
{
	auto fileObject = parser_UTF8::doParseFile("ideologicalAdvisors.txt");
	if (fileObject)
	{
		auto ideologyObjects = fileObject->getLeaves();
		for (auto ideologyObject: ideologyObjects)
		{
			string ideaName = ideologyObject->getKey();
			HoI4Advisor* newAdvisor = new HoI4Advisor(ideologyObject->getLeaves()[0]);
			ideologicalAdvisors.insert(make_pair(ideaName, newAdvisor));
		}

		int ministerEventNum = 1;
		for (auto ideology: majorIdeologies)
		{
			if (ideology == "neutrality")
			{
				continue;
			}
			auto advisor = ideologicalAdvisors.find(ideology);
			if (advisor != ideologicalAdvisors.end())
			{
				advisor->second->addEventNum(ministerEventNum);
			}
			ministerEventNum += 6;
		}
	}
}


void HoI4World::convertGovernments()
{
	for (auto country: countries)
	{
		country.second->convertGovernment(*sourceWorld);
	}
}


void HoI4World::convertParties()
{
	for (auto country: countries)
	{
		country.second->convertParties(majorIdeologies);
	}
}


void HoI4World::importIdeologicalIdeas()
{
	auto fileObject = parser_UTF8::doParseFile("ideologicalIdeas.txt");
	if (fileObject)
	{
		auto ideologyObjects = fileObject->getLeaves();
		for (auto ideologyObject: ideologyObjects)
		{
			string ideaName = ideologyObject->getKey();
			ideologicalIdeas.insert(make_pair(ideaName, ideologyObject->getLeaves()));
		}
	}
}


void HoI4World::identifyMajorIdeologies()
{
	if (Configuration::getIdeologiesOptions() == ideologyOptions::keep_major)
	{
		for (auto greatPower: greatPowers)
		{
			majorIdeologies.insert(greatPower->getGovernmentIdeology());
		}

		for (auto country: countries)
		{
			if (country.second->isHuman())
			{
				majorIdeologies.insert(country.second->getGovernmentIdeology());
			}
		}
		majorIdeologies.insert("neutrality");
	}
	else
	{
		for (auto ideology: ideologies)
		{
			majorIdeologies.insert(ideology.first);
		}
	}
}


void HoI4World::addNeutrality()
{
	for (auto country: countries)
	{
		if (majorIdeologies.count(country.second->getGovernmentIdeology()) == 0)
		{
			country.second->setGovernmentToExistingIdeology(majorIdeologies, ideologies);
		}
	}
}


void HoI4World::convertIdeologySupport()
{
	for (auto country: countries)
	{
		country.second->convertIdeologySupport(majorIdeologies);
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


double HoI4World::getTotalWorldWorkers(const map<string, double>& industrialWorkersPerCountry)
{
	double totalWorldWorkers = 0.0;
	for (auto countryWorkers: industrialWorkersPerCountry)
	{
		totalWorldWorkers += countryWorkers.second;
	}

	return totalWorldWorkers;
}


map<string, double> HoI4World::adjustWorkers(const map<string, double>& industrialWorkersPerCountry, double totalWorldWorkers)
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


double HoI4World::getWorldwideWorkerFactoryRatio(const map<string, double>& workersInCountries, double totalWorldWorkers)
{
	double baseIndustry = 0.0;
	for (auto countryWorkers: workersInCountries)
	{
		baseIndustry += countryWorkers.second * 0.000019;
	}

	int defaultFactories = 1189;
	HOI4Version onePointFour("1.4.0");
	if (Configuration::getHOI4Version() >= onePointFour)
	{
		defaultFactories = 1201;
	}
	double deltaIndustry = baseIndustry - (defaultFactories - landedCountries.size());
	double newIndustry = baseIndustry - Configuration::getIcFactor() * deltaIndustry;
	double acutalWorkerFactoryRatio = newIndustry / totalWorldWorkers;

	return acutalWorkerFactoryRatio;
}


void HoI4World::putIndustryInStates(const map<string, double>& factoryWorkerRatios)
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

	if (Configuration::getDebug())
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

	reportDefaultIndustry(countryIndustry);
}


pair<string, array<int, 3>> HoI4World::getDefaultStateIndustry(const string& stateFilename)
{
	auto fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/history/states/" + stateFilename);
	if (fileObj)
	{
		auto stateObj = fileObj->safeGetObject("state");
		auto historyObj = stateObj->safeGetObject("history");
		auto buildingsObj = historyObj->safeGetObject("buildings");

		int civilianFactories = 0;
		int militaryFactories = 0;
		int dockyards = 0;
		if (buildingsObj != nullptr)
		{
			civilianFactories = buildingsObj->safeGetInt("industrial_complex");
			militaryFactories = buildingsObj->safeGetInt("arms_factory");
			dockyards = buildingsObj->safeGetInt("dockyard");
		}

		string owner = historyObj->safeGetString("owner");

		array<int, 3> industry = { militaryFactories, civilianFactories, dockyards };
		pair<string, array<int, 3>> stateData = make_pair(owner, industry);
		return stateData;
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/history/states/" << stateFilename;
		exit(-1);
	}
}


void HoI4World::reportDefaultIndustry(const map<string, array<int, 3>>& countryIndustry)
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
	auto resourceMap = importResourceMap();

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


map<int, map<string, double>> HoI4World::importResourceMap() const
{
	map<int, map<string, double>> resourceMap;

	auto fileObj = parser_UTF8::doParseFile("resources.txt");
	if (fileObj)
	{
		auto resourcesObj = fileObj->safeGetObject("resources");
		for (auto linkObj: resourcesObj->getValue("link"))
		{
			int provinceNumber = linkObj->safeGetInt("province");
			auto mapping = resourceMap.find(provinceNumber);
			if (mapping == resourceMap.end())
			{
				map<string, double> resources;
				resourceMap.insert(make_pair(provinceNumber, resources));
				mapping = resourceMap.find(provinceNumber);
			}

			auto resourcesObj = linkObj->safeGetObject("resources");
			for (auto resource: resourcesObj->getLeaves())
			{
				string resourceName = resource->getKey();
				mapping->second[resourceName] += stof(resource->getLeaf());
			}
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not read resources.txt";
		exit(-1);
	}

	return resourceMap;
}


void HoI4World::convertStrategicRegions()
{
	map<int, int> provinceToStrategicRegionMap = importStrategicRegions();

	for (auto state : states->getStates())
	{
		map<int, int> usedRegions = determineUsedRegions(state.second, provinceToStrategicRegionMap);
		auto bestRegion = determineMostUsedRegion(usedRegions);
		if (bestRegion)
		{
			addProvincesToRegion(state.second, *bestRegion);
		}
	}
	addLeftoverProvincesToRegions(provinceToStrategicRegionMap);
}


map<int, int> HoI4World::importStrategicRegions()
{
	map<int, int> provinceToStrategicRegionMap;

	set<string> filenames;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/map/strategicregions/", filenames);
	for (auto filename: filenames)
	{
		HoI4StrategicRegion* newRegion = new HoI4StrategicRegion(filename);
		strategicRegions.insert(make_pair(newRegion->getID(), newRegion));

		for (auto province: newRegion->getOldProvinces())
		{
			provinceToStrategicRegionMap.insert(make_pair(province, newRegion->getID()));
		}
	}

	return provinceToStrategicRegionMap;
}


map<int, int> HoI4World::determineUsedRegions(const HoI4State* state, map<int, int>& provinceToStrategicRegionMap)
{
	map<int, int> usedRegions;	// region ID -> number of provinces in that region

	for (auto province: state->getProvinces())
	{
		auto mapping = provinceToStrategicRegionMap.find(province);
		if (mapping == provinceToStrategicRegionMap.end())
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

		provinceToStrategicRegionMap.erase(mapping);
	}

	return usedRegions;
}


optional<int> HoI4World::determineMostUsedRegion(const map<int, int>& usedRegions) const
{
	int mostProvinces = 0;
	optional<int> bestRegion;
	for (auto region: usedRegions)
	{
		if (region.second > mostProvinces)
		{
			bestRegion = region.first;
			mostProvinces = region.second;
		}
	}

	return bestRegion;
}


void HoI4World::addProvincesToRegion(const HoI4State* state, int regionNum)
{
	auto region = strategicRegions.find(regionNum);
	if (region == strategicRegions.end())
	{
		LOG(LogLevel::Warning) << "Strategic region " << regionNum << " was not in the list of regions.";
		return;
	}

	for (auto province : state->getProvinces())
	{
		region->second->addNewProvince(province);
	}
}


void HoI4World::addLeftoverProvincesToRegions(const map<int, int>& provinceToStrategicRegionMap)
{
	for (auto mapping: provinceToStrategicRegionMap)
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


void HoI4World::convertDiplomacy()
{
	LOG(LogLevel::Info) << "Converting diplomacy";
	convertAgreements();
	convertRelations();
}


void HoI4World::convertAgreements()
{
	for (auto agreement : sourceWorld->getDiplomacy()->getAgreements())
	{
		auto possibleHoI4Tag1 = CountryMapper::getHoI4Tag(agreement->country1);
		if (!possibleHoI4Tag1)
		{
			continue;
		}
		auto possibleHoI4Tag2 = CountryMapper::getHoI4Tag(agreement->country2);
		if (!possibleHoI4Tag2)
		{
			continue;
		}

		auto HoI4Country1 = countries.find(*possibleHoI4Tag1);
		auto HoI4Country2 = countries.find(*possibleHoI4Tag2);
		if (HoI4Country1 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI4 country " << *possibleHoI4Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		if (HoI4Country2 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI4 country " << *possibleHoI4Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		if ((agreement->type == "alliance") || (agreement->type == "vassal"))
		{
			HoI4Agreement* HoI4a = new HoI4Agreement(*possibleHoI4Tag1, *possibleHoI4Tag2, agreement);
			diplomacy->addAgreement(HoI4a);
		}

		if (agreement->type == "alliance")
		{
			HoI4Country1->second->editAllies().insert(*possibleHoI4Tag2);
			HoI4Country2->second->editAllies().insert(*possibleHoI4Tag1);
		}

		if (agreement->type == "vassal")
		{
			HoI4Country1->second->addPuppet(*possibleHoI4Tag2);
			HoI4Country2->second->setPuppetmaster(*possibleHoI4Tag1);
		}
	}
}


void HoI4World::convertRelations()
{
	for (auto country: countries)
	{
		for (auto relationItr: country.second->getRelations())
		{
			string country1, country2;
			if (country.first < relationItr.first) // Put it in order to eliminate duplicate relations entries
			{
				country1 = country.first;
				country2 = relationItr.first;
			}
			else
			{
				country2 = relationItr.first;
				country1 = country.first;
			}

			HoI4Agreement* HoI4a = new HoI4Agreement(country1, country2, "relation", relationItr.second->getRelations(), date("1936.1.1"));
			diplomacy->addAgreement(HoI4a);

			if (relationItr.second->getGuarantee())
			{
				HoI4Agreement* HoI4a = new HoI4Agreement(country.first, relationItr.first, "guarantee", 0, date("1936.1.1"));
				diplomacy->addAgreement(HoI4a);
			}
			if (relationItr.second->getSphereLeader())
			{
				HoI4Agreement* HoI4a = new HoI4Agreement(country.first, relationItr.first, "sphere", 0, date("1936.1.1"));
				diplomacy->addAgreement(HoI4a);
			}
		}
	}
}


void HoI4World::convertTechs()
{
	LOG(LogLevel::Info) << "Converting techs and research bonuses";

	map<string, vector<pair<string, int>>> techMap = importTechMap();
	map<string, vector<pair<string, int>>> researchBonusMap = importResearchBonusMap();

	for (auto dstCountry : countries)
	{
		const V2Country* sourceCountry = dstCountry.second->getSourceCountry();

		for (auto technology : sourceCountry->getTechs())
		{
			addTechs(dstCountry.second, technology, techMap);
			addResearchBonuses(dstCountry.second, technology, researchBonusMap);
		}
		for (auto invention : sourceCountry->getInventions())
		{
			addTechs(dstCountry.second, invention, techMap);
			addResearchBonuses(dstCountry.second, invention, researchBonusMap);
		}
	}
}


map<string, vector<pair<string, int>>> HoI4World::importTechMap() const
{
	map<string, vector<pair<string, int>>> techMap;

	auto fileObj = parser_UTF8::doParseFile("tech_mapping.txt");
	if (fileObj)
	{
		auto mapObj = fileObj->safeGetObject("tech_map");
		if (mapObj == nullptr)
		{
			LOG(LogLevel::Error) << "Could not read tech map";
			exit(-1);
		}
		for (auto link: mapObj->getValue("link"))
		{
			vector<pair<string, int> > targetTechs;
			string tech = "";

			for (auto leaf: link->getLeaves())
			{
				string key = leaf->getKey();
				if (key == "vic2")
				{
					tech = leaf->getLeaf();
				}
				else
				{
					int value = link->safeGetInt(key);
					targetTechs.push_back(pair<string, int>(key, value));
				}
			}

			techMap[tech] = targetTechs;
		}

		return techMap;
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse tech_mapping.txt";
		exit(-1);
	}
}

map<string, vector<pair<string, int>>> HoI4World::importResearchBonusMap() const
{
	map<string, vector<pair<string, int>>> researchBonusMap;

	auto fileObj = parser_UTF8::doParseFile("tech_mapping.txt");
	if (fileObj)
	{
		auto mapObj = fileObj->safeGetObject("bonus_map");
		if (mapObj == nullptr)
		{
			LOG(LogLevel::Error) << "Could not read bonus map";
			exit(-1);
		}

		for (auto link : mapObj->getValue("link"))
		{
			vector<pair<string, int> > targetTechs;
			string tech = "";

			for (auto leaf: link->getLeaves())
			{
				string key = leaf->getKey();
				if (key == "vic2")
				{
					tech = leaf->getLeaf();
				}
				else
				{
					int value = link->safeGetInt(key);
					targetTechs.push_back(pair<string, int>(key, value));
				}
			}

			researchBonusMap[tech] = targetTechs;
		}

		return researchBonusMap;
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse tech_mapping.txt";
		exit(-1);
	}
}

void HoI4World::addTechs(shared_ptr<HoI4Country> country, const string& oldTech, const map<string, vector<pair<string, int>>>& techMap)
{
	auto mapItr = techMap.find(oldTech);
	if (mapItr == techMap.end())
	{
		return;
	}
	if (mapItr != techMap.end())
	{
		for (auto HoI4TechItr: mapItr->second)
		{
			country->setTechnology(HoI4TechItr.first, HoI4TechItr.second);
		}
	}
}

void HoI4World::addResearchBonuses(shared_ptr<HoI4Country> country, const string& oldTech, const map<string, vector<pair<string, int>>>& researchBonusMap)
{
	auto mapItr = researchBonusMap.find(oldTech);
	if (mapItr == researchBonusMap.end())
	{
		return;
	}
	if (mapItr != researchBonusMap.end())
	{
		for (auto HoI4TechItr : mapItr->second)
		{
			country->setResearchBonus(HoI4TechItr.first, HoI4TechItr.second);
		}
	}
}

map<string, HoI4UnitMap> HoI4World::importUnitMap() const
{
	/* HARDCODED! TO DO : IMPLEMENT PARSING of unit_mapping.txt */

	map<string, HoI4UnitMap> unitMap;

	unitMap["irregular"] = HoI4UnitMap();

	unitMap["infantry"] = HoI4UnitMap("land","infantry","infantry_equipment_0",3);
	unitMap["regular"] = HoI4UnitMap("land","infantry","infantry_equipment_0",3);
	unitMap["engineer"] = HoI4UnitMap("land", "infantry", "infantry_equipment_0", 3);
	unitMap["guard"] = HoI4UnitMap("land", "infantry", "infantry_equipment_0", 3);
	unitMap["specops"] = HoI4UnitMap("land", "infantry", "infantry_equipment_0", 3);

	unitMap["artillery"] = HoI4UnitMap("land", "artillery_brigade", "artillery_equipment_1", 3);
	unitMap["horse_artillery"] = HoI4UnitMap("land", "artillery_brigade", "artillery_equipment_1", 3);

	unitMap["cavalry"] = HoI4UnitMap();

	unitMap["hussar"] = HoI4UnitMap("land", "cavalry", "infantry_equipment_0", 3);
	unitMap["cuirassier"] = HoI4UnitMap("land", "cavalry", "infantry_equipment_0", 3);
	unitMap["dragoon"] = HoI4UnitMap("land", "cavalry", "infantry_equipment_0", 3);

	unitMap["tank"] = HoI4UnitMap("land", "light_armor", "gw_tank_equipment", 1);

	unitMap["plane"] = HoI4UnitMap("air", "fighter", "fighter_equipment_0", 20);
	unitMap["bomber"] = HoI4UnitMap("air", "tac_bomber", "tac_bomber_equipment_0", 20);
	unitMap["transport_plane"] = HoI4UnitMap("air", "transport_plane", "transport_plane_equipment_0", 20);

	unitMap["manowar"] = HoI4UnitMap();
	unitMap["frigate"] = HoI4UnitMap();
	unitMap["commerce_raider"] = HoI4UnitMap("naval", "destroyer", "destroyer_1", 1);
	unitMap["ironclad"] = HoI4UnitMap();
	unitMap["monitor"] = HoI4UnitMap();
	unitMap["cruiser"] = HoI4UnitMap("naval", "light_cruiser", "light_cruiser_1", 1);
	unitMap["battleship"] = HoI4UnitMap("naval", "heavy_cruiser", "heavy_cruiser_1", 1);
	unitMap["dreadnought"] = HoI4UnitMap("naval", "battleship", "battleship_1", 1);
	unitMap["submarine"] = HoI4UnitMap("naval", "submarine", "submarine_1", 1);
	unitMap["carrier"] = HoI4UnitMap("naval", "carrier", "carrier", 1);
	unitMap["clipper_transport"] = HoI4UnitMap();
	unitMap["steam_transport"] = HoI4UnitMap("convoy", "convoy", "convoy_1", 1);
	
	return unitMap;
}

vector<HoI4DivisionTemplateType> HoI4World::importDivisionTemplates() const
{
	/* HARDCODED! TO DO : IMPLEMENT PARSING of unit_mapping.txt */

	vector<HoI4DivisionTemplateType> templateList;
	HoI4DivisionTemplateType armoredTemplate("Armored Division");

	armoredTemplate.addRegiment(HoI4RegimentType("light_armor", 0, 0));
	armoredTemplate.addRegiment(HoI4RegimentType("light_armor", 0, 1));
	armoredTemplate.addRegiment(HoI4RegimentType("light_armor", 0, 2));

	armoredTemplate.addRegiment(HoI4RegimentType("light_armor", 1, 0));
	armoredTemplate.addRegiment(HoI4RegimentType("light_armor", 1, 1));
	armoredTemplate.addRegiment(HoI4RegimentType("light_armor", 1, 2));

	armoredTemplate.addRegiment(HoI4RegimentType("motorized", 2, 0));
	armoredTemplate.addRegiment(HoI4RegimentType("motorized", 2, 1));
	armoredTemplate.addRegiment(HoI4RegimentType("motorized", 2, 2));

	armoredTemplate.addSupportRegiment(HoI4RegimentType("artillery",0,0));

	templateList.push_back(armoredTemplate);

	HoI4DivisionTemplateType mechanizedTemplate("Mechanized Division");

	mechanizedTemplate.addRegiment(HoI4RegimentType("light_armor", 0, 0));
	mechanizedTemplate.addRegiment(HoI4RegimentType("light_armor", 0, 1));
	mechanizedTemplate.addRegiment(HoI4RegimentType("light_armor", 0, 2));

	mechanizedTemplate.addRegiment(HoI4RegimentType("motorized", 1, 0));
	mechanizedTemplate.addRegiment(HoI4RegimentType("motorized", 1, 1));
	mechanizedTemplate.addRegiment(HoI4RegimentType("motorized", 1, 2));

	mechanizedTemplate.addRegiment(HoI4RegimentType("motorized", 2, 0));
	mechanizedTemplate.addRegiment(HoI4RegimentType("motorized", 2, 1));
	mechanizedTemplate.addRegiment(HoI4RegimentType("motorized", 2, 2));

	mechanizedTemplate.addSupportRegiment(HoI4RegimentType("artillery", 0, 0));

	templateList.push_back(mechanizedTemplate);

	HoI4DivisionTemplateType motorizedTemplate("Motorized Division");

	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 0, 0));
	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 0, 1));
	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 0, 2));

	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 1, 0));
	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 1, 1));
	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 1, 2));

	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 2, 0));
	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 2, 1));
	motorizedTemplate.addRegiment(HoI4RegimentType("motorized", 2, 2));

	motorizedTemplate.addSupportRegiment(HoI4RegimentType("artillery", 0, 0));

	templateList.push_back(motorizedTemplate);

	HoI4DivisionTemplateType assaultDivTemplate("Assault Division");

	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 0, 0));
	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 0, 1));
	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 0, 2));

	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 1, 0));
	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 1, 1));
	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 1, 2));

	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 2, 0));
	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 2, 1));
	assaultDivTemplate.addRegiment(HoI4RegimentType("infantry", 2, 2));

	assaultDivTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 3, 0));
	assaultDivTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 3, 1));
	assaultDivTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 3, 2));

	assaultDivTemplate.addRegiment(HoI4RegimentType("light_armor", 4, 0));

	templateList.push_back(assaultDivTemplate);

	HoI4DivisionTemplateType assaultBrigadeTemplate("Assault Brigade");

	assaultBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 0));
	assaultBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 1));
	assaultBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 2));

	assaultBrigadeTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 1, 0));
	
	assaultBrigadeTemplate.addRegiment(HoI4RegimentType("light_armor", 2, 0));

	templateList.push_back(assaultBrigadeTemplate);

	HoI4DivisionTemplateType infantryDivisionTemplate("Infantry Division");

	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 0, 0));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 0, 1));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 0, 2));

	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 1, 0));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 1, 1));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 1, 2));

	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 2, 0));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 2, 1));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 2, 2));

	infantryDivisionTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 3, 0));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 3, 1));
	infantryDivisionTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 3, 2));

	templateList.push_back(infantryDivisionTemplate);

	HoI4DivisionTemplateType infantryBrigadeTemplate("Infantry Brigade");

	infantryBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 0));
	infantryBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 1));
	infantryBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 2));

	infantryBrigadeTemplate.addRegiment(HoI4RegimentType("artillery_brigade", 1, 0));

	templateList.push_back(infantryBrigadeTemplate);

	HoI4DivisionTemplateType lightInfantryDivisionTemplate("Light Infantry Division");

	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 0, 0));
	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 0, 1));
	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 0, 2));

	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 1, 0));
	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 1, 1));
	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 1, 2));

	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 2, 0));
	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 2, 1));
	lightInfantryDivisionTemplate.addRegiment(HoI4RegimentType("infantry", 2, 2));

	templateList.push_back(lightInfantryDivisionTemplate);

	HoI4DivisionTemplateType lightInfantryBrigadeTemplate("Light Infantry Brigade");

	lightInfantryBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 0));
	lightInfantryBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 1));
	lightInfantryBrigadeTemplate.addRegiment(HoI4RegimentType("infantry", 0, 2));

	templateList.push_back(lightInfantryBrigadeTemplate);

	HoI4DivisionTemplateType cavalryDivisionTemplate("Cavalry Division");

	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 0, 0));
	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 0, 1));
	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 0, 2));

	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 1, 0));
	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 1, 1));
	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 1, 2));

	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 2, 0));
	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 2, 1));
	cavalryDivisionTemplate.addRegiment(HoI4RegimentType("cavalry", 2, 2));

	templateList.push_back(cavalryDivisionTemplate);

	HoI4DivisionTemplateType cavalryBrigadeTemplate("Cavalry Brigade");

	cavalryBrigadeTemplate.addRegiment(HoI4RegimentType("cavalry", 0, 0));
	cavalryBrigadeTemplate.addRegiment(HoI4RegimentType("cavalry", 0, 1));
	cavalryBrigadeTemplate.addRegiment(HoI4RegimentType("cavalry", 0, 2));

	templateList.push_back(cavalryBrigadeTemplate);

	return templateList;
}

void HoI4World::convertMilitaries()
{
	map<string, HoI4UnitMap> unitMap = importUnitMap();
	divisionTemplates = importDivisionTemplates();

	convertArmies(unitMap, divisionTemplates);
	convertNavies(unitMap);
	convertAirforces(unitMap);
}


void HoI4World::convertArmies(const map<string, HoI4UnitMap>& unitMap, const vector<HoI4DivisionTemplateType>& divisionTemplates)
{
	LOG(LogLevel::Info) << "Converting armies";

	for (auto country: countries)
	{
		country.second->convertArmyDivisions(unitMap, divisionTemplates);
	}
}


void HoI4World::convertNavies(const map<string, HoI4UnitMap>& unitMap)
{
	LOG(LogLevel::Info) << "Converting navies";

	for (auto country : countries)
	{
		country.second->convertNavy(unitMap);
		country.second->convertConvoys(unitMap);
	}
}


void HoI4World::convertAirforces(const map<string, HoI4UnitMap>& unitMap)
{
	LOG(LogLevel::Info) << "Converting air forces";

	for (auto country : countries)
	{
		country.second->convertAirforce(unitMap);
	}
}


void HoI4World::determineGreatPowers()
{
	for (auto greatPowerVic2Tag: sourceWorld->getGreatPowers())
	{
		auto possibleGreatPowerTag = CountryMapper::getHoI4Tag(greatPowerVic2Tag);
		if (possibleGreatPowerTag)
		{
			auto greatPower = countries.find(*possibleGreatPowerTag);
			if (greatPower != countries.end())
			{
				greatPowers.push_back(greatPower->second);
				greatPower->second->setGreatPower();
			}
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


double HoI4World::getStrongestCountryStrength() const
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


int HoI4World::calculateStrengthVPs(shared_ptr<HoI4Country> country, double greatestStrength) const
{
	double relativeStrength = country->getStrengthOverTime(1.0) / greatestStrength;
	return static_cast<int>(relativeStrength * 30.0);
}


void HoI4World::convertAirBases()
{
	addBasicAirBases();
	addCapitalAirBases();
	addGreatPowerAirBases();
}


void HoI4World::addBasicAirBases()
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
}


void HoI4World::addCapitalAirBases()
{
	for (auto country: countries)
	{
		auto capitalState = country.second->getCapitalState();
		if (capitalState != nullptr)
		{
			capitalState->addAirBase(5);
		}
	}
}


void HoI4World::addGreatPowerAirBases()
{
	for (auto greatPower: greatPowers)
	{
		auto capitalState = greatPower->getCapitalState();
		if (capitalState != nullptr)
		{
			capitalState->addAirBase(5);
		}
	}
}


void HoI4World::createFactions()
{
	LOG(LogLevel::Info) << "Creating Factions";

	ofstream factionsLog;
	if (Configuration::getDebug())
	{
		factionsLog.open("factions-logs.csv");
		factionsLog << "name,government,initial strength,factory strength per year,factory strength by 1939\n";
	}

	for (auto leader : greatPowers)
	{
		if (leader->isInFaction())
		{
			continue;
		}
		if (Configuration::getDebug())
		{
			factionsLog << "\n";
		}

		vector<shared_ptr<HoI4Country>> factionMembers;
		factionMembers.push_back(leader);

		string leaderIdeology = leader->getGovernmentIdeology();
		if (Configuration::getDebug())
		{
			logFactionMember(factionsLog, leader);
		}
		double factionMilStrength = leader->getStrengthOverTime(3.0);

		std::set<std::string> alliesAndPuppets = leader->getAllies();
		for (auto puppetTag : leader->getPuppets())
		{
			alliesAndPuppets.insert(puppetTag);
		}

		for (auto allyTag : alliesAndPuppets)
		{
			auto allycountry = findCountry(allyTag);
			if (!allycountry)
			{
				continue;
			}
			string allygovernment = allycountry->getGovernmentIdeology();
			auto possibleSphereLeader = returnSphereLeader(allycountry);

			if (
					((possibleSphereLeader) && (*possibleSphereLeader == leader->getTag())) ||
					((!possibleSphereLeader) && governmentsAllowFaction(leaderIdeology, allygovernment))
				)
			{
				if (Configuration::getDebug())
				{
					logFactionMember(factionsLog, allycountry);
				}
				factionMembers.push_back(allycountry);

				factionMilStrength += (allycountry)->getStrengthOverTime(1.0);
				//also add the allies' puppets to the faction
				for (auto puppetTag : (allycountry)->getPuppets())
				{
					auto puppetcountry = findCountry(puppetTag);
					if (!puppetcountry)
					{
						continue;
					}
					logFactionMember(factionsLog, puppetcountry);
					factionMembers.push_back(puppetcountry);

					factionMilStrength += (puppetcountry)->getStrengthOverTime(1.0);
				}
			}
		}

		if (factionMembers.size() > 1)
		{
			auto newFaction = make_shared<HoI4Faction>(leader, factionMembers);
			for (auto member : factionMembers)
			{
				member->setFaction(newFaction);
			}
			factions.push_back(newFaction);

			if (Configuration::getDebug())
			{
				factionsLog << "Faction Strength in 1939," << factionMilStrength << "\n";
			}
		}
	}

	if (Configuration::getDebug())
	{
		factionsLog.close();
	}
}


void HoI4World::logFactionMember(ofstream& factionsLog, shared_ptr<HoI4Country> member) const
{
	auto possibleName = member->getSourceCountry()->getName("english");
	if (possibleName)
	{
		factionsLog << *possibleName << ",";
		factionsLog << member->getGovernmentIdeology() << ",";
		factionsLog << member->getMilitaryStrength() << ",";
		factionsLog << member->getEconomicStrength(1.0) << ",";
		factionsLog << member->getEconomicStrength(3.0) << "\n";
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not get name when logging faction member";
	}
}


optional<string> HoI4World::returnSphereLeader(shared_ptr<HoI4Country> possibleSphereling) const
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

	return {};
}


bool HoI4World::governmentsAllowFaction(const string& leaderIdeology, const string& allyGovernment) const
{
	if (leaderIdeology == allyGovernment)
	{
		return true;
	}
	else if (leaderIdeology == "absolutist" && allyGovernment == "fascism")
	{
		return true;
	}
	else if (leaderIdeology == "democratic" && allyGovernment == "absolutist")
	{
		return true;
	}
	else if (leaderIdeology == "communism" && allyGovernment == "fascism")
	{
		return true;
	}
	else if (leaderIdeology == "fascism" && (allyGovernment == "communism" || allyGovernment == "absolutist"))
	{
		return true;
	}
	else
	{
		return false;
	}
}


void HoI4World::addCountryElectionEvents(const set<string>& majorIdeologies)
{
	for (auto country: countries)
	{
		events->addPartyChoiceEvent(country.first, country.second->getParties(), onActions, majorIdeologies);
	}
}


void HoI4World::output() const
{
	LOG(LogLevel::Info) << "Outputting world";

	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/history"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/history";
		exit(-1);
	}

	outputCommonCountries();
	outputColorsfile();
	outputNames();
	HoI4Localisation::output();
	states->output();
	diplomacy->output();
	outputMap();
	supplyZones->output();
	outputRelations();
	outputGenericFocusTree();
	outputCountries();
	buildings->output();
	events->output();
	onActions->output();
	outputIdeologies();
	outputLeaderTraits();
	outputIdeologicalIdeas();
	outputScriptedTriggers();
	outputOnActions();
}


void HoI4World::outputCommonCountries() const
{
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/common/country_tags"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/common/country_tags\"";
		exit(-1);
	}

	LOG(LogLevel::Debug) << "Writing countries file";
	ofstream allCountriesFile("output/" + Configuration::getOutputName() + "/common/country_tags/00_countries.txt");
	if (!allCountriesFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create countries file";
		exit(-1);
	}

	for (auto countryItr: countries)
	{
		if (countryItr.second->getCapitalStateNum() != 0)
		{
			countryItr.second->outputToCommonCountriesFile(allCountriesFile);
		}
	}

	allCountriesFile << "\n";
	allCountriesFile.close();
}


void HoI4World::outputColorsfile() const
{
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/common/countries"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/common/countries\"";
		exit(-1);
	}

	ofstream output("output/" + Configuration::getOutputName() + "/common/countries/colors.txt");
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open output/" << Configuration::getOutputName() << "/common/countries/colors.txt";
		exit(-1);
	}

	output << "#reload countrycolors\n";
	for (auto countryItr: countries)
	{
		if (countryItr.second->getCapitalStateNum() != 0)
		{
			countryItr.second->outputColors(output);
		}
	}

	output.close();
}


void HoI4World::outputNames() const
{
	ofstream namesFile("output/" + Configuration::getOutputName() + "/common/names/01_names.txt");
	namesFile << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy

	if (!namesFile.is_open())
	{
		Log(LogLevel::Error) << "Could not open output/" << Configuration::getOutputName() << "/common/names/01_names.txt";
		exit(-1);
	}

	for (auto country: countries)
	{
		country.second->outputToNamesFiles(namesFile);
	}
}


void HoI4World::outputMap() const
{
	LOG(LogLevel::Debug) << "Writing Map Info";

	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/map"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/map";
		exit(-1);
	}

	ofstream rocketSitesFile("output/" + Configuration::getOutputName() + "/map/rocketsites.txt");
	if (!rocketSitesFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create output/" << Configuration::getOutputName() << "/map/rocketsites.txt";
		exit(-1);
	}
	for (auto state: states->getStates())
	{
		auto provinces = state.second->getProvinces();
		rocketSitesFile << state.second->getID() << " = { " << *provinces.begin() << " }\n";
	}
	rocketSitesFile.close();

	ofstream airportsFile("output/" + Configuration::getOutputName() + "/map/airports.txt");
	if (!airportsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create output/" << Configuration::getOutputName() << "/map/airports.txt";
		exit(-1);
	}
	for (auto state: states->getStates())
	{
		auto provinces = state.second->getProvinces();
		airportsFile << state.second->getID() << " = { " << *provinces.begin() << " }\n";
	}
	airportsFile.close();

	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/map/strategicregions"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/map/strategicregions";
		exit(-1);
	}
	for (auto strategicRegion: strategicRegions)
	{
		strategicRegion.second->output("output/" + Configuration::getOutputName() + "/map/strategicregions/");
	}
}


void HoI4World::outputGenericFocusTree() const
{
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/common/national_focus"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/common/national_focus\"";
		exit(-1);
	}

	HoI4FocusTree genericFocusTree;
	genericFocusTree.addGenericFocusTree(majorIdeologies);
	genericFocusTree.output("output/" + Configuration::getOutputName() + "/common/national_focus/generic.txt");
}


void HoI4World::outputCountries() const
{
	LOG(LogLevel::Debug) << "Writing countries";
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/history"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/history";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/history/countries"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/history";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/history/states"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/history/states";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/history/units"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/history/units";
		exit(-1);
	}

	for (auto country: countries)
	{
		country.second->output(getActiveIdeologicalAdvisors(), divisionTemplates);
	}

	ofstream ideasFile("output/" + Configuration::getOutputName() + "/interface/converter_ideas.gfx");
	if (!ideasFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open output/" << Configuration::getOutputName() << "/interface/ideas.gfx";
		exit(-1);
	}

	ideasFile << "spriteTypes = {\n";
	for (auto country: countries)
	{
		country.second->outputIdeaGraphics(ideasFile);
	}
	ideasFile << "\n";
	ideasFile << "}\n";
}


set<const HoI4Advisor*, advisorCompare> HoI4World::getActiveIdeologicalAdvisors() const
{
	set<const HoI4Advisor*, advisorCompare> theAdvisors;
	for (auto ideology: majorIdeologies)
	{
		auto ideologicalAdvisor = ideologicalAdvisors.find(ideology);
		if (ideologicalAdvisor != ideologicalAdvisors.end())
		{
			theAdvisors.insert(ideologicalAdvisor->second);
		}
	}

	return theAdvisors;
}


void HoI4World::outputRelations() const
{
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/common/opinion_modifiers"))
	{
		Log(LogLevel::Error) << "Could not create output/" + Configuration::getOutputName() + "/common/opinion_modifiers/";
		exit(-1);
	}

	ofstream out("output/" + Configuration::getOutputName() + "/common/opinion_modifiers/01_opinion_modifiers.txt");
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not create 01_opinion_modifiers.txt.";
		exit(-1);
	}

	out << "opinion_modifiers = {\n";
	for (int i = -200; i <= 200; i++)
	{
		if (i < 0)
		{
			out << "negative_";
		}
		else
		{
			out << "positive_";
		}
		out << abs(i) << " = {\n";
		out << "\tvalue = " << i << "\n";
		out << "}\n";
	}
 	out << "private_channels_trade = {\n";
	out << "\ttrade = yes\n";
	out << "\tvalue = 15\n";
	out << "}\n";
	out << "absolutist_in_government = {\n";
	out << "\tvalue = 30\n";
	out << "}\n";
	out << "communism_in_government = {\n";
	out << "\tvalue = 30\n";
	out << "}\n";
	out << "fascism_in_government = {\n";
	out << "\tvalue = 30\n";
	out << "}\n";
	out << "radical_in_government = {\n";
	out << "\tvalue = 30\n";
	out << "}\n";

	out << "}\n";

	out.close();
}


void HoI4World::outputIdeologies() const
{
	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/common/ideologies/"))
	{
		Log(LogLevel::Error) << "Could not create output/" + Configuration::getOutputName() + "/common/ideologies/";
	}
	ofstream ideologyFile("output/" + Configuration::getOutputName() + "/common/ideologies/00_ideologies.txt");
	ideologyFile << "ideologies = {\n";
	ideologyFile << "\t\n";
	for (auto ideologyName: majorIdeologies)
	{
		auto ideology = ideologies.find(ideologyName);
		if (ideology != ideologies.end())
		{
			ideology->second->output(ideologyFile);
		}
	}
	ideologyFile << "}";
	ideologyFile.close();
}


void HoI4World::outputLeaderTraits() const
{
	ofstream traitsFile("output/" + Configuration::getOutputName() + "/common/country_leader/converterTraits.txt");
	traitsFile << "leader_traits = {\n";
	for (auto majorIdeology: majorIdeologies)
	{
		auto ideologyTraits = ideologicalLeaderTraits.find(majorIdeology);
		if (ideologyTraits != ideologicalLeaderTraits.end())
		{
			for (auto trait: ideologyTraits->second)
			{
				traitsFile << "\n";
				traitsFile << *trait;
			}
		}
	}
	traitsFile << "}";
	traitsFile.close();
}


void HoI4World::outputIdeologicalIdeas() const
{
	ofstream ideasFile("output/" + Configuration::getOutputName() + "/common/ideas/convertedIdeas.txt");
	ideasFile << "ideas = {\n";
	ideasFile << "\tcountry = {\n";
	for (auto majorIdeology: majorIdeologies)
	{
		auto ideologicalIdea = ideologicalIdeas.find(majorIdeology);
		if (ideologicalIdea != ideologicalIdeas.end())
		{
			for (auto idea: ideologicalIdea->second)
			{
				ideasFile << *idea;
				ideasFile << "\n";
			}
		}
	}
	ideasFile << "\t}\n";
	ideasFile << "}";
	ideasFile.close();
}


void HoI4World::outputScriptedTriggers() const
{
	ofstream triggersFile("output/" + Configuration::getOutputName() + "/common/scripted_triggers/convertedTriggers.txt");
	triggersFile << "can_lose_democracy_support = {\n";
	for (auto ideology: majorIdeologies)
	{
		if (ideology == "democratic")
		{
			triggersFile << "\tdemocratic > 0.65\n";
		}
		else
		{
			triggersFile << "\t" << ideology << " < 0.18\n";
		}
	}
	triggersFile << "}\n";
	triggersFile.close();
}


void HoI4World::outputOnActions() const
{
	ofstream onActionsFile("output/" + Configuration::getOutputName() + "/common/on_actions/01_on_actions.txt");
	onActionsFile << "on_actions = {\n";
	onActionsFile << "	on_government_change = {\n";
	onActionsFile << "		effect = {\n";
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		onActionsFile << "			if = {\n";
		onActionsFile << "				limit = { has_government = " + ideology + " }\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = " + ideology + "_partisans_recruiting }\n";
		onActionsFile << "					remove_ideas = " + ideology + "_partisans_recruiting\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = " + ideology + "_revolutionaries }\n";
		onActionsFile << "					remove_ideas = " + ideology + "_revolutionaries\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = " + ideology + "_defeated }\n";
		onActionsFile << "					remove_ideas = " + ideology + "_defeated\n";
		onActionsFile << "				}\n";
		onActionsFile << "			}\n";
	}
	if (majorIdeologies.count("democratic") > 0)
	{
		onActionsFile << "			if = {\n";
		onActionsFile << "				limit = { has_government = democratic }\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = democratic_opposition_voicing_protests }\n";
		onActionsFile << "					remove_ideas = democratic_opposition_voicing_protests\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = democratic_revolutionaries }\n";
		onActionsFile << "					remove_ideas = democratic_revolutionaries\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = reign_of_terror }\n";
		onActionsFile << "					remove_ideas = reign_of_terror\n";
		onActionsFile << "				}\n";
		onActionsFile << "			}\n";
	}
	onActionsFile << "		}\n";
	onActionsFile << "	}\n";

	onActionsFile << "	on_startup = {\n";
	onActionsFile << "		effect = {\n";
	onActionsFile << "			set_province_name = { id = 587 name = \"Köln\"} #Cologne\n";
	onActionsFile << "			set_province_name = { id = 957 name = \"Vladivostok\"} #Haishenwai\n";
	onActionsFile << "			set_province_name = { id = 1025 name = \"Kokura\"} #Fukuoka\n";
	onActionsFile << "			set_province_name = { id = 1047 name = \"Guangzhou\"} #Canton\n";
	onActionsFile << "			set_province_name = { id = 1182 name = \"Tokyo\"} #Edo\n";
	onActionsFile << "			set_province_name = { id = 1440 name = \"San Juan\"} #Puerto Rico\n";
	onActionsFile << "			set_province_name = { id = 1843 name = \"Miami\"} #Tampa\n";
	onActionsFile << "			set_province_name = { id = 3151 name = \"Leningrad\"} #Saint Petersburg\n";
	onActionsFile << "			set_province_name = { id = 3152 name = \"Tallinn\"} #Reval\n";
	onActionsFile << "			set_province_name = { id = 3529 name = \"Stalingrad\"} #Tsaritsyn\n";
	onActionsFile << "			set_province_name = { id = 4180 name = \"Honolulu\"} #Hawaii\n";
	onActionsFile << "			set_province_name = { id = 4268 name = \"Nouméa\"} #New Caledonia\n";
	onActionsFile << "			set_province_name = { id = 4333 name = \"Astana\"} #Qaraganda\n";
	onActionsFile << "			set_province_name = { id = 4709 name = \"Ürümqi\"} #Díhuà\n";
	onActionsFile << "			set_province_name = { id = 4801 name = \"Ulaanbaatar\"} #Urga\n";
	onActionsFile << "			set_province_name = { id = 6115 name = \"Oslo\"} #Christiania\n";
	onActionsFile << "			set_province_name = { id = 7371 name = \"Kuching\"} #Brunei\n";
	onActionsFile << "			set_province_name = { id = 11437 name = \"Dnipropetrovsk\"} #Ekaterinoslav\n";
	onActionsFile << "			set_province_name = { id = 12674 name = \"Reykjavik\"} #Iceland\n";
	onActionsFile << "		}\n";
	onActionsFile << "	}\n";
	onActionsFile << "}\n";
	onActionsFile.close();
}


/*vector<int> HoI4World::getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
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

}*/