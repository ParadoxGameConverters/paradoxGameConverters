/*Copyright (c) 2018 The Paradox Game Converters Project

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
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../Configuration.h"
#include "../V2World/Diplomacy.h"
#include "../V2World/Party.h"
#include "Advisor.h"
#include "HoI4Agreement.h"
#include "HoI4Buildings.h"
#include "HoI4Country.h"
#include "Decisions.h"
#include "HoI4Diplomacy.h"
#include "Events.h"
#include "HoI4Faction.h"
#include "HoI4FocusTree.h"
#include "Ideas.h"
#include "IdeologicalAdvisors.h"
#include "HOI4Ideology.h"
#include "IdeologyFile.h"
#include "HoI4Localisation.h"
#include "Names.h"
#include "HoI4Province.h"
#include "HoI4State.h"
#include "HoI4StrategicRegion.h"
#include "HoI4WarCreator.h"
#include "Resources.h"
#include "StateCategories.h"
#include "SupplyZones.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/TechMapper.h"
#include "ParserHelpers.h"
#include <fstream>
using namespace std;




HoI4::World::World(const Vic2::World* _sourceWorld):
	sourceWorld(_sourceWorld),
	countryMap(_sourceWorld),
	states(new HoI4States(sourceWorld, countryMap)),
	supplyZones(new HoI4::SupplyZones(states->getDefaultStates())),
	theIdeas(std::make_unique<HoI4::Ideas>()),
	decisions(make_unique<HoI4::decisions>()),
	peaces(make_unique<HoI4::AIPeaces>()),
	diplomacy(new HoI4Diplomacy),
	events(new HoI4::Events),
	onActions(make_unique<HoI4::OnActions>())
{
	LOG(LogLevel::Info) << "Parsing HoI4 data";

	theCoastalProvinces.init(theMapData);
	buildings = new Buildings(*states, theCoastalProvinces, theMapData),
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
	convertGovernments();
	identifyMajorIdeologies();
	importIdeologicalMinisters();
	convertParties();
	events->createPoliticalEvents(majorIdeologies);
	events->createWarJustificationEvents(majorIdeologies);
	events->importElectionEvents(majorIdeologies, *onActions);
	addCountryElectionEvents(majorIdeologies);
	events->createStabilityEvents(majorIdeologies);
	theIdeas->updateIdeas(majorIdeologies);
	decisions->updateDecisions(majorIdeologies, *events);
	peaces->updateAIPeaces(majorIdeologies);
	addNeutrality();
	convertIdeologySupport();
	convertCapitalVPs();
	convertAirBases();
	if (theConfiguration.getCreateFactions())
	{
		createFactions();
	}

	HoI4WarCreator warCreator(this, theMapData);

	addFocusTrees();
	adjustResearchFocuses();
}


shared_ptr<HoI4Country> HoI4::World::findCountry(const string& countryTag)
{
	auto country = countries.find(countryTag);
	if (country == countries.end())
	{
		return {};
	}

	return country->second;
}

void HoI4::World::convertNavalBases()
{
	for (auto state: states->getStates())
	{
		state.second->convertNavalBases(theCoastalProvinces);
	}
}


void HoI4::World::convertCountries()
{
	LOG(LogLevel::Info) << "Converting countries";

	for (auto sourceItr : sourceWorld->getCountries())
	{
		convertCountry(sourceItr);
	}

	HoI4Localisation::addNonenglishCountryLocalisations();
}


void HoI4::World::convertCountry(pair<string, Vic2::Country*> country)
{
	// don't convert rebels
	if (country.first == "REB")
	{
		return;
	}

	HoI4Country* destCountry = nullptr;
	auto possibleHoI4Tag = countryMap.getHoI4Tag(country.first);
	if (!possibleHoI4Tag)
	{
		LOG(LogLevel::Warning) << "Could not convert V2 tag " << country.first << " to HoI4";
	}
	else
	{
		auto possibleCountryName = country.second->getName("english");
		string countryName;
		if (possibleCountryName)
		{
			countryName = *possibleCountryName;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not set country name when converting country";
		}

		std::string countryFileName = Utils::convertWin1252ToUTF8(countryName) + ".txt";
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

		destCountry->initFromV2Country(*sourceWorld, country.second, states->getProvinceToStateIDMap(), states->getStates(), theNames, theGraphics, countryMap);
		countries.insert(make_pair(*possibleHoI4Tag, destCountry));
		HoI4Localisation::createCountryLocalisations(make_pair(country.first, *possibleHoI4Tag), governmentMap);
		HoI4Localisation::updateMainCountryLocalisation(destCountry->getTag() + "_" + destCountry->getGovernmentIdeology(), country.first, country.second->getGovernment());
	}
}


void HoI4::World::importIdeologies()
{
	clearRegisteredKeywords();
	registerKeyword(std::regex("ideologies"), [this](const std::string& unused, std::istream& theStream)
	{
		IdeologyFile theFile(theStream);
		for (auto ideology: theFile.getIdeologies())
		{
			ideologies.insert(ideology);
		}
	});

	if (theConfiguration.getIdeologiesOptions() != ideologyOptions::keep_default)
	{
		parseFile("converterIdeologies.txt");
	}
	parseFile(theConfiguration.getHoI4Path() + "/common/ideologies/00_ideologies.txt");
}


void HoI4::World::importLeaderTraits()
{
	clearRegisteredKeywords();
	registerKeyword(std::regex("[a-z]+"), [this](const std::string& ideologyName, std::istream& theStream){
		commonItems::stringsOfItems traits(theStream);
		ideologicalLeaderTraits.insert(make_pair(ideologyName, traits.getStrings()));
	});
	parseFile("converterLeaderTraits.txt");
}


void HoI4::World::importIdeologicalMinisters()
{
	HoI4::IdeologicalAdvisors theAdvisors;
	auto theAcutalAdvisors = theAdvisors.getAdvisors();
	ideologicalAdvisors.swap(theAcutalAdvisors);
}


void HoI4::World::convertGovernments()
{
	for (auto country: countries)
	{
		country.second->convertGovernment(*sourceWorld, governmentMap);
	}
}


void HoI4::World::convertParties()
{
	for (auto country: countries)
	{
		country.second->convertParties(majorIdeologies, governmentMap);
	}
}


void HoI4::World::identifyMajorIdeologies()
{
	if (theConfiguration.getIdeologiesOptions() == ideologyOptions::keep_major)
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


void HoI4::World::addNeutrality()
{
	for (auto country: countries)
	{
		if (majorIdeologies.count(country.second->getGovernmentIdeology()) == 0)
		{
			country.second->setGovernmentToExistingIdeology(majorIdeologies, ideologies, governmentMap);
		}
	}
}


void HoI4::World::convertIdeologySupport()
{
	for (auto country: countries)
	{
		country.second->convertIdeologySupport(majorIdeologies, governmentMap);
	}
}


void HoI4::World::convertIndustry()
{
	LOG(LogLevel::Info) << "Converting industry";

	addStatesToCountries();

	map<string, double> factoryWorkerRatios = calculateFactoryWorkerRatios();
	putIndustryInStates(factoryWorkerRatios);

	calculateIndustryInCountries();
	reportIndustryLevels();
}


void HoI4::World::addStatesToCountries()
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


map<string, double> HoI4::World::calculateFactoryWorkerRatios()
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


map<string, double> HoI4::World::getIndustrialWorkersPerCountry()
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


double HoI4::World::getTotalWorldWorkers(const map<string, double>& industrialWorkersPerCountry)
{
	double totalWorldWorkers = 0.0;
	for (auto countryWorkers: industrialWorkersPerCountry)
	{
		totalWorldWorkers += countryWorkers.second;
	}

	return totalWorldWorkers;
}


map<string, double> HoI4::World::adjustWorkers(const map<string, double>& industrialWorkersPerCountry, double totalWorldWorkers)
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
		double newWorkers = countryWorkers.second - theConfiguration.getIndustrialShapeFactor() * delta;
		adjustedWorkers.insert(make_pair(countryWorkers.first, newWorkers));
	}

	return adjustedWorkers;
}


double HoI4::World::getWorldwideWorkerFactoryRatio(const map<string, double>& workersInCountries, double totalWorldWorkers)
{
	double baseIndustry = 0.0;
	for (auto countryWorkers: workersInCountries)
	{
		baseIndustry += countryWorkers.second * 0.000019;
	}

	int defaultFactories = 1189;
	HoI4::Version onePointFour("1.4.0");
	if (theConfiguration.getHOI4Version() >= onePointFour)
	{
		defaultFactories = 1201;
	}
	double deltaIndustry = baseIndustry - defaultFactories;
	deltaIndustry -= landedCountries.size();
	double newIndustry = baseIndustry - theConfiguration.getIcFactor() * deltaIndustry;
	double acutalWorkerFactoryRatio = newIndustry / totalWorldWorkers;

	return acutalWorkerFactoryRatio;
}


void HoI4::World::putIndustryInStates(const map<string, double>& factoryWorkerRatios)
{
	HoI4::stateCategories theStateCategories;

	for (auto HoI4State : states->getStates())
	{
		auto ratioMapping = factoryWorkerRatios.find(HoI4State.second->getOwner());
		if (ratioMapping == factoryWorkerRatios.end())
		{
			continue;
		}

		HoI4State.second->convertIndustry(ratioMapping->second, theStateCategories, theCoastalProvinces);
	}
}


void HoI4::World::calculateIndustryInCountries()
{
	for (auto country: countries)
	{
		country.second->calculateIndustry();
	}
}


void HoI4::World::reportIndustryLevels()
{
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

	if (theConfiguration.getDebug())
	{
		reportCountryIndustry();
		reportDefaultIndustry();
	}
}


void HoI4::World::reportCountryIndustry()
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


void HoI4::World::reportDefaultIndustry()
{
	map<string, array<int, 3>> countryIndustry;

	for (auto state: states->getDefaultStates())
	{
		pair<string, array<int, 3>> stateData = getDefaultStateIndustry(state.second);

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


pair<string, array<int, 3>> HoI4::World::getDefaultStateIndustry(const HoI4::State* state)
{
	int civilianFactories = state->getCivFactories();
	int militaryFactories = state->getMilFactories();
	int dockyards = state->getDockyards();

	string owner = state->getOwner();

	array<int, 3> industry = { militaryFactories, civilianFactories, dockyards };
	pair<string, array<int, 3>> stateData = make_pair(owner, industry);
	return stateData;
}


void HoI4::World::reportDefaultIndustry(const map<string, array<int, 3>>& countryIndustry)
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


void HoI4::World::convertResources()
{
	resources resourceMap;

	for (auto state: states->getStates())
	{
		for (auto provinceNumber: state.second->getProvinces())
		{
			for (auto resource: resourceMap.getResourcesInProvince(provinceNumber))
			{
				state.second->addResource(resource.first, resource.second);
			}
		}
	}
}


void HoI4::World::convertStrategicRegions()
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


map<int, int> HoI4::World::importStrategicRegions()
{
	map<int, int> provinceToStrategicRegionMap;

	set<string> filenames;
	Utils::GetAllFilesInFolder(theConfiguration.getHoI4Path() + "/map/strategicregions/", filenames);
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


map<int, int> HoI4::World::determineUsedRegions(const HoI4::State* state, map<int, int>& provinceToStrategicRegionMap)
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


optional<int> HoI4::World::determineMostUsedRegion(const map<int, int>& usedRegions) const
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


void HoI4::World::addProvincesToRegion(const HoI4::State* state, int regionNum)
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


void HoI4::World::addLeftoverProvincesToRegions(const map<int, int>& provinceToStrategicRegionMap)
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


void HoI4::World::convertDiplomacy()
{
	LOG(LogLevel::Info) << "Converting diplomacy";
	convertAgreements();
	convertRelations();
}


void HoI4::World::convertAgreements()
{
	for (auto agreement : sourceWorld->getDiplomacy()->getAgreements())
	{
		auto possibleHoI4Tag1 = countryMap.getHoI4Tag(agreement->getCountry1());
		if (!possibleHoI4Tag1)
		{
			continue;
		}
		auto possibleHoI4Tag2 = countryMap.getHoI4Tag(agreement->getCountry2());
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

		if ((agreement->getType() == "alliance") || (agreement->getType() == "vassal"))
		{
			HoI4Agreement* HoI4a = new HoI4Agreement(*possibleHoI4Tag1, *possibleHoI4Tag2, agreement);
			diplomacy->addAgreement(HoI4a);
		}

		if (agreement->getType() == "alliance")
		{
			HoI4Country1->second->editAllies().insert(*possibleHoI4Tag2);
			HoI4Country2->second->editAllies().insert(*possibleHoI4Tag1);
		}

		if (agreement->getType() == "vassal")
		{
			HoI4Country1->second->addPuppet(*possibleHoI4Tag2);
			HoI4Country2->second->setPuppetmaster(*possibleHoI4Tag1);
		}
	}
}


void HoI4::World::convertRelations()
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


void HoI4::World::convertTechs()
{
	LOG(LogLevel::Info) << "Converting techs and research bonuses";

	techMapper techMap;

	for (auto dstCountry: countries)
	{
		auto sourceCountry = dstCountry.second->getSourceCountry();

		for (auto technology: sourceCountry->getTechs())
		{
			addTechs(dstCountry.second, technology, techMap);
			addResearchBonuses(dstCountry.second, technology, techMap);
		}
		for (auto invention: sourceCountry->getInventions())
		{
			addTechs(dstCountry.second, invention, techMap);
			addResearchBonuses(dstCountry.second, invention, techMap);
		}
	}
}


void HoI4::World::addTechs(shared_ptr<HoI4Country> country, const string& oldTech, const techMapper& techMap)
{
	for (auto HoI4TechItr: techMap.getHoI4Techs(oldTech))
	{
		country->setTechnology(HoI4TechItr.first, HoI4TechItr.second);
	}
}


void HoI4::World::addResearchBonuses(shared_ptr<HoI4Country> country, const string& oldTech, const techMapper& techMap)
{
	for (auto HoI4TechItr: techMap.getResearchBonuses(oldTech))
	{
		country->setResearchBonus(HoI4TechItr.first, HoI4TechItr.second);
	}
}


void HoI4::World::convertMilitaries()
{
	auto specificMappings = theMilitaryMappings.getMilitaryMappings(theConfiguration.getVic2Mods());

	convertArmies(specificMappings);
	convertNavies(specificMappings.getUnitMap());
	convertAirforces(specificMappings.getUnitMap());
}


void HoI4::World::convertArmies(const militaryMappings& theMilitaryMappings)
{
	LOG(LogLevel::Info) << "Converting armies";

	for (auto country: countries)
	{
		country.second->convertArmies(theMilitaryMappings);
	}
}


void HoI4::World::convertNavies(const map<string, HoI4::UnitMap>& unitMap)
{
	LOG(LogLevel::Info) << "Converting navies";

	for (auto country : countries)
	{
		country.second->convertNavies(unitMap, theCoastalProvinces, states->getProvinceToStateIDMap());
		country.second->convertConvoys(unitMap);
	}
}


void HoI4::World::convertAirforces(const map<string, HoI4::UnitMap>& unitMap)
{
	LOG(LogLevel::Info) << "Converting air forces";

	for (auto country : countries)
	{
		country.second->convertAirforce(unitMap);
	}
}


void HoI4::World::determineGreatPowers()
{
	for (auto greatPowerVic2Tag: sourceWorld->getGreatPowers())
	{
		auto possibleGreatPowerTag = countryMap.getHoI4Tag(greatPowerVic2Tag);
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


void HoI4::World::convertCapitalVPs()
{
	LOG(LogLevel::Info) << "Adding bonuses to capitals";

	addBasicCapitalVPs();
	addGreatPowerVPs();
	addStrengthVPs();
}


void HoI4::World::addBasicCapitalVPs()
{
	for (auto countryItr: countries)
	{
		countryItr.second->addVPsToCapital(5);
	}
}


void HoI4::World::addGreatPowerVPs()
{
	for (auto greatPower: greatPowers)
	{
		greatPower->addVPsToCapital(5);
	}
}


void HoI4::World::addStrengthVPs()
{
	double greatestStrength = getStrongestCountryStrength();
	for (auto country: countries)
	{
		int VPs = calculateStrengthVPs(country.second, greatestStrength);
		country.second->addVPsToCapital(VPs);
	}
}


double HoI4::World::getStrongestCountryStrength() const
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


int HoI4::World::calculateStrengthVPs(shared_ptr<HoI4Country> country, double greatestStrength) const
{
	double relativeStrength = country->getStrengthOverTime(1.0) / greatestStrength;
	return static_cast<int>(relativeStrength * 30.0);
}


void HoI4::World::convertAirBases()
{
	addBasicAirBases();
	addCapitalAirBases();
	addGreatPowerAirBases();
}


void HoI4::World::addBasicAirBases()
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


void HoI4::World::addCapitalAirBases()
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


void HoI4::World::addGreatPowerAirBases()
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


void HoI4::World::createFactions()
{
	LOG(LogLevel::Info) << "Creating Factions";

	ofstream factionsLog;
	if (theConfiguration.getDebug())
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
		if (theConfiguration.getDebug())
		{
			factionsLog << "\n";
		}

		vector<shared_ptr<HoI4Country>> factionMembers;
		factionMembers.push_back(leader);

		string leaderIdeology = leader->getGovernmentIdeology();
		if (theConfiguration.getDebug())
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
				if (theConfiguration.getDebug())
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

			if (theConfiguration.getDebug())
			{
				factionsLog << "Faction Strength in 1939," << factionMilStrength << "\n";
			}
		}
	}

	if (theConfiguration.getDebug())
	{
		factionsLog.close();
	}
}


void HoI4::World::logFactionMember(ofstream& factionsLog, shared_ptr<HoI4Country> member) const
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


optional<string> HoI4::World::returnSphereLeader(shared_ptr<HoI4Country> possibleSphereling) const
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


bool HoI4::World::governmentsAllowFaction(const string& leaderIdeology, const string& allyGovernment) const
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


void HoI4::World::addFocusTrees()
{
	for (auto country: countries)
	{
		if (country.second->isGreatPower() || (country.second->getStrengthOverTime(3) > 4500))
		{
			country.second->addGenericFocusTree(majorIdeologies);
		}
	}
}


void HoI4::World::adjustResearchFocuses()
{
	for (auto country: countries)
	{
		country.second->adjustResearchFocuses();
	}
}


void HoI4::World::addCountryElectionEvents(const set<string>& majorIdeologies)
{
	for (auto country: countries)
	{
		events->addPartyChoiceEvent(country.first, country.second->getParties(), *onActions, majorIdeologies);
	}
}


void HoI4::World::output()
{
	LOG(LogLevel::Info) << "Outputting world";

	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/history";
		exit(-1);
	}

	outputCommonCountries();
	outputColorsfile();
	outputNames();
	outputUnitNames();
	HoI4Localisation::output();
	states->output();
	diplomacy->output();
	outputMap();
	supplyZones->output();
	outputRelations();
	outputGenericFocusTree();
	outputCountries();
	buildings->output();
	decisions->output();
	events->output();
	onActions->output(majorIdeologies);
	decisions->output();
	peaces->output(majorIdeologies);
	outputIdeologies();
	outputLeaderTraits();
	outputIdeas();
	outputScriptedTriggers();
	outputBookmarks();
}


void HoI4::World::outputCommonCountries() const
{
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/country_tags"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/common/country_tags\"";
		exit(-1);
	}

	LOG(LogLevel::Debug) << "Writing countries file";
	ofstream allCountriesFile("output/" + theConfiguration.getOutputName() + "/common/country_tags/00_countries.txt");
	if (!allCountriesFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create countries file";
		exit(-1);
	}

	for (auto country: countries)
	{
		if (country.second->getCapitalStateNum() != 0)
		{
			country.second->outputToCommonCountriesFile(allCountriesFile);
		}
	}

	allCountriesFile << "\n";
	allCountriesFile.close();
}


void HoI4::World::outputColorsfile() const
{
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/countries"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/common/countries\"";
		exit(-1);
	}

	ofstream output("output/" + theConfiguration.getOutputName() + "/common/countries/colors.txt");
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open output/" << theConfiguration.getOutputName() << "/common/countries/colors.txt";
		exit(-1);
	}

	output << "#reload countrycolors\n";
	for (auto country: countries)
	{
		if (country.second->getCapitalStateNum() != 0)
		{
			country.second->outputColors(output);
		}
	}

	output.close();
}


void HoI4::World::outputNames() const
{
	ofstream namesFile("output/" + theConfiguration.getOutputName() + "/common/names/01_names.txt");
	namesFile << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy

	if (!namesFile.is_open())
	{
		Log(LogLevel::Error) << "Could not open output/" << theConfiguration.getOutputName() << "/common/names/01_names.txt";
		exit(-1);
	}

	for (auto country: countries)
	{
		if (country.second->getCapitalStateNum() != 0)
		{
			country.second->outputToNamesFiles(namesFile, theNames);
		}
	}
}

void HoI4::World::outputUnitNames() const
{
	ofstream namesFile("output/" + theConfiguration.getOutputName() + "/common/units/names/01_names.txt");
	namesFile << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy

	if (!namesFile.is_open())
	{
		Log(LogLevel::Error) << "Could not open output/" << theConfiguration.getOutputName() << "/common/units/names/01_names.txt";
		exit(-1);
	}

	for (auto country : countries)
	{
		if (country.second->getCapitalStateNum() != 0)
		{
			country.second->outputToUnitNamesFiles(namesFile);
		}
	}
}


void HoI4::World::outputMap() const
{
	LOG(LogLevel::Debug) << "Writing Map Info";

	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/map"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/map";
		exit(-1);
	}

	ofstream rocketSitesFile("output/" + theConfiguration.getOutputName() + "/map/rocketsites.txt");
	if (!rocketSitesFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create output/" << theConfiguration.getOutputName() << "/map/rocketsites.txt";
		exit(-1);
	}
	for (auto state: states->getStates())
	{
		auto provinces = state.second->getProvinces();
		rocketSitesFile << state.second->getID() << " = { " << *provinces.begin() << " }\n";
	}
	rocketSitesFile.close();

	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/map/strategicregions"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/map/strategicregions";
		exit(-1);
	}
	for (auto strategicRegion: strategicRegions)
	{
		strategicRegion.second->output("output/" + theConfiguration.getOutputName() + "/map/strategicregions/");
	}
}


void HoI4::World::outputGenericFocusTree() const
{
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/national_focus"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/common/national_focus\"";
		exit(-1);
	}

	HoI4FocusTree genericFocusTree;
	genericFocusTree.addGenericFocusTree(majorIdeologies);
	genericFocusTree.output("output/" + theConfiguration.getOutputName() + "/common/national_focus/generic.txt");
}


void HoI4::World::outputCountries()
{
	LOG(LogLevel::Debug) << "Writing countries";
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/history";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/countries"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/history";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/states"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/history/states";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/units"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + theConfiguration.getOutputName() + "/history/units";
		exit(-1);
	}

	auto activeIdeologicalAdvisors = getActiveIdeologicalAdvisors();
	for (auto country: countries)
	{
		if (country.second->getCapitalStateNum() != 0)
		{
			auto specificMilitaryMappings = theMilitaryMappings.getMilitaryMappings(theConfiguration.getVic2Mods());
			country.second->output(activeIdeologicalAdvisors, specificMilitaryMappings.getDivisionTemplates(), theNames, theGraphics);
		}
	}

	ofstream ideasFile("output/" + theConfiguration.getOutputName() + "/interface/converter_ideas.gfx");
	if (!ideasFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open output/" << theConfiguration.getOutputName() << "/interface/ideas.gfx";
		exit(-1);
	}

	ideasFile << "spriteTypes = {\n";
	for (auto country: countries)
	{
		if (country.second->getCapitalStateNum() != 0)
		{
			country.second->outputIdeaGraphics(ideasFile, theGraphics);
		}
	}
	ideasFile << "\n";
	ideasFile << "}\n";
}


set<const HoI4::Advisor*, HoI4::advisorCompare> HoI4::World::getActiveIdeologicalAdvisors() const
{
	set<const HoI4::Advisor*, HoI4::advisorCompare> theAdvisors;
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


void HoI4::World::outputRelations() const
{
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/opinion_modifiers"))
	{
		Log(LogLevel::Error) << "Could not create output/" + theConfiguration.getOutputName() + "/common/opinion_modifiers/";
		exit(-1);
	}

	ofstream out("output/" + theConfiguration.getOutputName() + "/common/opinion_modifiers/01_opinion_modifiers.txt");
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


void HoI4::World::outputIdeologies() const
{
	if (!Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/ideologies/"))
	{
		Log(LogLevel::Error) << "Could not create output/" + theConfiguration.getOutputName() + "/common/ideologies/";
	}
	ofstream ideologyFile("output/" + theConfiguration.getOutputName() + "/common/ideologies/00_ideologies.txt");
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


void HoI4::World::outputLeaderTraits() const
{
	ofstream traitsFile("output/" + theConfiguration.getOutputName() + "/common/country_leader/converterTraits.txt");
	traitsFile << "leader_traits = {\n";
	for (auto majorIdeology: majorIdeologies)
	{
		auto ideologyTraits = ideologicalLeaderTraits.find(majorIdeology);
		if (ideologyTraits != ideologicalLeaderTraits.end())
		{
			for (auto trait: ideologyTraits->second)
			{
				traitsFile << "\t";
				traitsFile << trait;
			}
		}
	}
	traitsFile << "}";
	traitsFile.close();
}


void HoI4::World::outputIdeas() const
{
	theIdeas->output(majorIdeologies);
}


void HoI4::World::outputScriptedTriggers() const
{
	ofstream triggersFile("output/" + theConfiguration.getOutputName() + "/common/scripted_triggers/convertedTriggers.txt");

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

	triggersFile << "has_unsupported_manpower_law = {\n";
	triggersFile << "	if = {\n";
	triggersFile << "		limit = {\n";
	triggersFile << "			has_idea = limited_conscription\n";
	triggersFile << "		}\n";
	triggersFile << "		has_idea = limited_conscription\n";
	triggersFile << "		has_war_support < 0.1\n";
	triggersFile << "	}\n";
	triggersFile << "	else_if = {\n";
	triggersFile << "		limit = {\n";
	triggersFile << "			has_idea = extensive_conscription\n";
	triggersFile << "		}\n";
	triggersFile << "		has_idea = extensive_conscription\n";
	triggersFile << "		has_war_support < 0.2\n";
	for (auto majorIdeology: majorIdeologies)
	{
		if ((majorIdeology == "democratic") || (majorIdeology == "neutrality"))
		{
			 continue;
		}

		triggersFile << "		NOT = { has_government = " << majorIdeology << " }\n";
	}
	triggersFile << "	}\n";
	triggersFile << "	else_if = {\n";
	triggersFile << "		limit = {\n";
	triggersFile << "			has_idea = service_by_requirement\n";
	triggersFile << "		}\n";
	triggersFile << "		has_idea = service_by_requirement\n";
	triggersFile << "		has_war_support < 0.6\n";
	for (auto majorIdeology: majorIdeologies)
	{
		if ((majorIdeology == "democratic") || (majorIdeology == "neutrality"))
		{
			continue;
		}

		triggersFile << "		NOT = { has_government = " << majorIdeology << " }\n";
	}
	triggersFile << "	}\n";
	triggersFile << "	else_if = {\n";
	triggersFile << "		limit = {\n";
	triggersFile << "			has_idea = all_adults_serve\n";
	triggersFile << "		}\n";
	triggersFile << "		has_idea = all_adults_serve\n";
	triggersFile << "	}\n";
	triggersFile << "	else_if = {\n";
	triggersFile << "		limit = {\n";
	triggersFile << "			has_idea = scraping_the_barrel\n";
	triggersFile << "		}\n";
	triggersFile << "		has_idea = scraping_the_barrel\n";
	triggersFile << "	}\n";
	triggersFile << "	else = {\n";
	triggersFile << "		always = no\n";
	triggersFile << "	}\n";
	triggersFile << "}\n";

	triggersFile.close();
}


void HoI4::World::outputBookmarks() const
{
	ofstream bookmarkFile("output/" + theConfiguration.getOutputName() + "/common/bookmarks/the_gathering_storm.txt");

	bookmarkFile << "bookmarks = {\n";
	bookmarkFile << "	bookmark = {\n";
	bookmarkFile << "		name = ""GATHERING_STORM_NAME""\n";
	bookmarkFile << "		desc = ""GATHERING_STORM_DESC""\n";
	bookmarkFile << "		date = 1936.1.1.12\n";
	bookmarkFile << "		picture = ""GFX_select_date_1936""\n";
	bookmarkFile << "		default_country = \"---\"\n";
	bookmarkFile << "		default = yes\n";

	for (auto greatPower : greatPowers)
	{
		//Vic2 Great powers become majors in bookmark
		bookmarkFile << "		" + greatPower->getTag() + "= {\n";
		bookmarkFile << "			history = \"OTHER_GATHERING_STORM_DESC\"\n";
		bookmarkFile << "			ideology = " + greatPower->getGovernmentIdeology() + "\n";
		bookmarkFile << "			ideas = { great_power }\n";
		bookmarkFile << "		}\n";
	}
	
	bookmarkFile << "		\"---\"= {\n";
	bookmarkFile << "			history = \"OTHER_GATHERING_STORM_DESC\"\n";
	bookmarkFile << "		}\n";

	for (auto country : countries)
	{
		if (country.second->isGreatPower() != true)
		{
			if (country.second->getStrengthOverTime(3) > 4500)
			{
				//add minor countries to the bookmark, only those with custom focustree are visible due to Hoi4 limitations
				//Bookmark window has room for 22 minor countries, going over this seems to not cause any issues however
				bookmarkFile << "		" + country.second->getTag() + " = {\n";
				bookmarkFile << "			minor = yes\n";
				bookmarkFile << "			history = \"OTHER_GATHERING_STORM_DESC\"\n";
				bookmarkFile << "			ideology = " + country.second->getGovernmentIdeology() + "\n";
				bookmarkFile << "			ideas = { }\n";
				bookmarkFile << "		}\n";
			}
		}
	}

	bookmarkFile << "effect = {\n";
	bookmarkFile << "randomize_weather = 22345 # <-Obligatory in every bookmark !\n";
	bookmarkFile << "#123 = { rain_light = yes }\n";
	bookmarkFile << "	}\n";
	bookmarkFile << "}\n";
	bookmarkFile.close();
}


/*vector<int> HoI4::World::getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
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


vector<int> HoI4::World::getPortProvinces(const vector<int>& locationCandidates)
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


int HoI4::World::getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner)
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
