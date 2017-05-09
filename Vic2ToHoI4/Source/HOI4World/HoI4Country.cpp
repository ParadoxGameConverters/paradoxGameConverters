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



#include "HoI4Country.h"
#include "HoI4World.h"
#include "Log.h"
#include "ParadoxParserUTF8.h"
#include "HoI4Faction.h"
#include "HoI4Leader.h"
#include "HoI4Minister.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/GovernmentMapper.h"
#include "../Mappers/NamesMapper.h"
#include "../Mappers/GraphicsMapper.h"
#include "../Mappers/V2Localisations.h"
#include "../V2World/V2Relations.h"
#include "../V2World/V2Party.h"
#include "../Mappers/ProvinceMapper.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include <boost/algorithm/string.hpp>



HoI4Country::HoI4Country(string _tag, string _commonCountryFile, HoI4World* _theWorld)
{
	theWorld = _theWorld;

	tag = _tag;
	commonCountryFile = _commonCountryFile;
	provinces.clear();
	technologies.clear();

	capital = 0;
	governmentIdeology = "neutrality";
	leaderIdeology = "neutrality";
	rulingParty = nullptr;

	faction = nullptr;
	factionLeader = false;

	nationalUnity = 70;
	
	training_laws = "minimal_training";
	press_laws = "censored_press";
	industrial_policy_laws = "consumer_product_orientation";
	educational_investment_law = "minimal_education_investment";
	economic_law = "full_civilian_economy";
	conscription_law = "volunteer_army";
	civil_law = "limited_restrictions";

	relations.clear();
	allies.clear();
	puppets.clear();
	puppetMaster = "";
	practicals.clear();
	ministers.clear();
	rulingMinisters.clear();

	graphicalCulture = "Generic";
	majorNation = false;

	greatPower = false;

	nationalFocus = nullptr;

	srcCountry = nullptr;
	human = false;
}


void HoI4Country::initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _vic2ideology, map<int, int>& leaderMap, governmentJobsMap governmentJobs, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const map<int, int>& stateMap, map<int, HoI4State*> states)
{
	srcCountry = _srcCountry;
	filename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (filename == "")
	{
		filename = Utils::GetFileFromTag(Configuration::getHoI4Path() + "/tfh/history/countries/", tag);
	}
	if (filename == "")
	{
		filename = tag + " - " + commonCountryFile;
	}

	human = _srcCountry->isHuman();

	// Color
	color = srcCountry->getColor();

	//Civilized
	civilized = srcCountry->isCivilized();

	//threat
	threat = _srcCountry->getBadBoy() / 10.0;
	
	// graphical culture type
	auto cultureItr = cultureMap.find(srcCountry->getPrimaryCulture());
	if (cultureItr != cultureMap.end())
	{
		graphicalCulture = cultureItr->second;
	}
	else
	{
		graphicalCulture = "Generic";
	}

	// Government
	governmentIdeology = governmentMapper::getIdeologyForCountry(srcCountry, _vic2ideology);
	leaderIdeology = governmentMapper::getLeaderIdeologyForCountry(srcCountry, _vic2ideology);

	// Political parties
	rulingParty = _srcCountry->getRulingParty(_srcWorld.getParties());
	parties = _srcCountry->getActiveParties(_srcWorld.getParties());

	// Ministers
	/*for (unsigned int ideologyIdx = 0; ideologyIdx <= stalinist; ideologyIdx++)
	{
		for (auto job : governmentJobs)
		{
			HoI4Minister newMinister(namesMapper::getMaleNames(srcCountry->getPrimaryCulture()), namesMapper::getSurnames(srcCountry->getPrimaryCulture()), ideologyNames[ideologyIdx], job, governmentJobs, portraitMap[graphicalCulture]);
			ministers.push_back(newMinister);

			if (ideologyNames[ideologyIdx] == ideology)
			{
				rulingMinisters.push_back(newMinister);
			}
		}
	}*/

	// Faction is handled in HoI4World::configureFactions

	nationalUnity = 70.0 + (_srcCountry->getRevanchism() / 0.05) - (_srcCountry->getWarExhaustion() / 2.5);

	// civil law - democracies get open society, communist dicatorships get totalitarian, everyone else gets limited restrictions
	string srcGovernment = srcCountry->getGovernment();
	if (srcGovernment == "democracy" || srcGovernment == "hms_government")
	{
		civil_law = "open_society";
	}
	else if (srcGovernment == "proletarian_dictatorship")
	{
		civil_law = "totalitarian_system";
	}
	else
	{
		if (nationalUnity > 50.0)
		{
			civil_law = "limited_restrictions";
		}
		else
		{
			civil_law = "open_society";
		}
	}

	// conscription law - everyone starts with volunteer armies
	conscription_law = "volunteer_army";

	// economic law - everyone starts with full civilian economy
	economic_law = "full_civilian_economy";

	// educational investment law - from educational spending
	if (srcCountry->getEducationSpending() > 0.90)
	{
		educational_investment_law = "big_education_investment";
	}
	else if (srcCountry->getEducationSpending() > 0.70)
	{
		educational_investment_law = "medium_large_education_investment";
	}
	else if (srcCountry->getEducationSpending() > 0.40)
	{
		educational_investment_law = "average_education_investment";
	}
	else
	{
		educational_investment_law = "minimal_education_investment";
	}

	// industrial policy laws - everyone starts with consumer product orientation
	industrial_policy_laws = "consumer_product_orientation";

	// press laws - set from press reforms
	if (srcCountry->getReform("press_rights") == "free_press")
	{
		press_laws = "free_press";
	}
	else if (srcCountry->getReform("press_rights") == "censored_press")
	{
		press_laws = "censored_press";
	}
	else // press_rights == state_press
	{
		if ((srcGovernment == "proletarian_dictatorship") ||
			(srcGovernment == "fascist_dictatorship"))
		{
			press_laws = "propaganda_press";
		}
		else
		{
			press_laws = "state_press";
		}
	}

	// training laws - from military spending
	if (srcCountry->getMilitarySpending() > 0.90)
	{
		training_laws = "specialist_training";
	}
	else if (srcCountry->getMilitarySpending() > 0.70)
	{
		training_laws = "advanced_training";
	}
	else if (srcCountry->getMilitarySpending() > 0.40)
	{
		training_laws = "basic_training";
	}
	else
	{
		training_laws = "minimal_training";
	}

	// leaders
	vector<V2Leader*> srcLeaders = srcCountry->getLeaders();
	for (auto srcLeader : srcLeaders)
	{
		HoI4Leader newLeader(srcLeader, tag, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}

	Configuration::setLeaderIDForNextCountry();

	// Relations
	map<string, V2Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (auto itr : srcRelations)
		{
			const std::string& HoI4Tag = CountryMapper::getHoI4Tag(itr.second->getTag());
			if (!HoI4Tag.empty())
			{
				HoI4Relations* hoi2r = new HoI4Relations(HoI4Tag, itr.second);
				relations.insert(make_pair(HoI4Tag, hoi2r));
			}
		}
	}

	determineCapitalFromVic2(stateMap, states);
	auto state = states.find(capital)->second;
	if (isThisStateOwnedByUs(state))
	{
		state->setAsCapitalState();
	}

	// major nation
	majorNation = srcCountry->isGreatNation();
}


void HoI4Country::determineCapitalFromVic2(const map<int, int>& provinceToStateIDMap, const map<int, HoI4State*>& states)
{
	int oldCapital = srcCountry->getCapital();
	auto itr = provinceMapper::getVic2ToHoI4ProvinceMapping().find(oldCapital);
	if (itr != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
	{
		auto capitalState = provinceToStateIDMap.find(itr->second[0]);
		if (capitalState != provinceToStateIDMap.end() && isStateValidForCapital(capitalState, states))
		{
			capital = capitalState->second;
			setCapitalInCapitalState(itr->second[0], states);
		}
		else
		{
			findBestCapital();
		}
	}
}

bool HoI4Country::isStateValidForCapital(map<int, int>::const_iterator capitalState, const map<int, HoI4State*>& states)
{
	auto state = states.find(capitalState->second)->second;
	return (isThisStateOwnedByUs(state) || isThisStateACoreWhileWeOwnNoStates(state));
}

bool HoI4Country::isThisStateOwnedByUs(const HoI4State* state) const
{
	return (state->getOwner() == tag);
}

bool HoI4Country::isThisStateACoreWhileWeOwnNoStates(const HoI4State* state) const
{
	for (auto core : state->getCores())
	{
		if (core == tag)
		{
			return true;
		}
	}

	return false;
}

void HoI4Country::setCapitalInCapitalState(int capitalProvince, const map<int, HoI4State*>& states)
{
	auto capitalState = states.find(capital);
	if (capitalState != states.end())
	{
		capitalState->second->setVPLocation(capitalProvince);
	}
}

void HoI4Country::findBestCapital()
{
	capital = 1;
	LOG(LogLevel::Warning) << "Could not properly set capital for " << tag;
}

// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void HoI4Country::initFromHistory()
{
	string fullFilename;
	filename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (filename == "")
	{
		filename = Utils::GetFileFromTag(Configuration::getHoI4Path() + "/history/countries/", tag);
	}
	else
	{
		fullFilename = string("./blankMod/output/history/countries/") + filename;
	}

	if (filename == "")
	{
		filename = tag + " - " + commonCountryFile;
		return;
	}
	else
	{
		fullFilename = Configuration::getHoI4Path() + "/history/countries/" + filename;
	}

	LOG(LogLevel::Debug) << "Parsing " << fullFilename;
	Object* obj = parser_UTF8::doParseFile(fullFilename);
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << fullFilename;
		exit(-1);
	}

	auto results = obj->getValue("ideology");
	if (results.size() > 0)
	{
		governmentIdeology = results[0]->getLeaf();
	}

	results = obj->getValue("capital");
	if (results.size() > 0)
	{
		capital = stoi(results[0]->getLeaf());
	}
}


void HoI4Country::setGovernmentToNeutral()
{
	governmentIdeology = "neutrality";
	leaderIdeology += "_neutral";
}


void HoI4Country::convertIdeologySupport(const set<string>& majorIdeologies)
{
	for (auto upperHouseIdeology: srcCountry->getUpperHouseComposition())
	{
		string ideology = governmentMapper::getSupportedIdeology(governmentIdeology, upperHouseIdeology.first);
		if (majorIdeologies.count(ideology) == 0)
		{
			ideology = "neutrality";
		}

		auto supportItr = ideologySupport.find(ideology);
		if (supportItr == ideologySupport.end())
		{
			ideologySupport.insert(make_pair(ideology, 0));
			supportItr = ideologySupport.find(ideology);
		}
		supportItr->second += static_cast<int>(upperHouseIdeology.second * 100);
	}

	int remainingSupport = 100;
	for (auto ideology: ideologySupport)
	{
		remainingSupport -= ideology.second;
	}
	auto supportItr = ideologySupport.find("neutrality");
	if (supportItr == ideologySupport.end())
	{
		ideologySupport.insert(make_pair("neutrality", 0));
		supportItr = ideologySupport.find("neutrality");
	}
	supportItr->second += remainingSupport;
}


void HoI4Country::generateLeaders(leaderTraitsMap leaderTraits, portraitMapping& portraitMap)
{
	// generated leaders
	int totalOfficers = 0;
	/*vector<V2Province*> srcProvinces = srcCountry->getCores();
	for (auto province : srcProvinces)
	{
		totalOfficers += province->getPopulation("officers");
	}*/

	unsigned int totalLand = 0;
	totalLand = totalOfficers / 300;
	if (totalLand > 350)
	{
		totalLand = 350;
	}
	if (totalLand < 10)
	{
		totalLand = 10;
	}
	if (factionLeader)
	{
		totalLand += 300;
	}
	for (unsigned int i = 0; i <= totalLand; i++)
	{
		auto portraits = portraitMap[graphicalCulture];
		auto maleNames = namesMapper::getMaleNames(srcCountry->getPrimaryCulture());
		auto surnames = namesMapper::getSurnames(srcCountry->getPrimaryCulture());
		HoI4Leader newLeader(maleNames, surnames, tag, "land", leaderTraits, portraits);
		leaders.push_back(newLeader);
	}

	unsigned int totalSea = 0;
	if (totalOfficers <= 1000)
	{
		totalSea = totalOfficers / 100;
	}
	else
	{
		totalSea = static_cast<int>(totalOfficers / 822.0 + 8.78);
	}
	if (totalSea > 100)
	{
		totalSea = 100;
	}
	if (totalSea < 1)
	{
		totalSea = 1;
	}
	if (factionLeader)
	{
		totalSea += 20;
	}
	for (unsigned int i = 0; i <= totalSea; i++)
	{
		auto portraits = portraitMap[graphicalCulture];
		auto maleNames = namesMapper::getMaleNames(srcCountry->getPrimaryCulture());
		auto surnames = namesMapper::getSurnames(srcCountry->getPrimaryCulture());
		HoI4Leader newLeader(maleNames, surnames, tag, "sea", leaderTraits, portraits);
		leaders.push_back(newLeader);
	}

	unsigned int totalAir = 0;
	if (totalOfficers <= 1000)
	{
		totalAir = totalOfficers / 100;
	}
	else
	{
		totalAir = static_cast<int>(totalOfficers / 925.0 + 12.62);
	}
	if (totalAir > 90)
	{
		totalAir = 90;
	}
	if (totalAir < 3)
	{
		totalAir = 3;
	}
	if (factionLeader)
	{
		totalAir += 20;
	}
	for (unsigned int i = 0; i <= totalAir; i++)
	{
		auto portraits = portraitMap[graphicalCulture];
		auto maleNames = namesMapper::getMaleNames(srcCountry->getPrimaryCulture());
		auto surnames = namesMapper::getSurnames(srcCountry->getPrimaryCulture());
		HoI4Leader newLeader(maleNames, surnames, tag, "air", leaderTraits, portraits);
		leaders.push_back(newLeader);
	}
}

void HoI4Country::convertNavy(map<int, HoI4State*> states)
{
	int navalport = 0;

	// count the heavy and light Vic2 ships
	double heavyShip = 0;
	double lightShip = 0;
	for (auto army : srcCountry->getArmies())
	{
		for (auto regiment : army->getRegiments())
		{
			string type = regiment->getType();
			if (type == "battleship")
			{
				heavyShip += 0.08;
			}
			if (type == "dreadnought")
			{
				heavyShip += 0.1;
			}
			if (type == "cruiser")
			{
				lightShip += 0.1;
			}
		}
	}

	// determine the HoI4 ships
	double BB = 0;
	double BC = 0;
	double HC = 0;
	double LC = 0;
	double DD = 0;
	double CV = 0;
	double SB = 0;
	convoys = static_cast<int>(Configuration::getForceMultiplier() * ((heavyShip * 0.21945 * 40) + (lightShip * 1.88 * 4)));

	for (auto tech : technologies)
	{
		if ((tech.first == "early_light_cruiser") && (tech.second == 1))
		{
			LC = Configuration::getForceMultiplier() * lightShip * .47;
		}
		if ((tech.first == "early_destroyer") && (tech.second == 1))
		{
			DD = Configuration::getForceMultiplier() * lightShip * 1.88;
		}
		if ((tech.first == "early_submarine") && (tech.second == 1))
		{
			SB = Configuration::getForceMultiplier() * lightShip * .705;
		}
		if ((tech.first == "early_heavy_cruiser") && (tech.second == 1))
		{
			HC = Configuration::getForceMultiplier() * heavyShip * 0.2926;
		}
		if ((tech.first == "early_battlecruiser") && (tech.second == 1))
		{
			BC = Configuration::getForceMultiplier() * heavyShip * 0.073;
		}
		if ((tech.first == "early_battleship") && (tech.second == 1))
		{
			CV = Configuration::getForceMultiplier() * heavyShip * 0.073;
			BB = Configuration::getForceMultiplier() * heavyShip * 0.21945;
		}
	}

	for (int i = 0; i < CV; i++)
	{
		if (technologies.find("CAS1") != technologies.end() && technologies.find("naval_bomber1") != technologies.end() && technologies.find("early_carrier") != technologies.end())
		{
			HoI4Ship newShip("Carrier", "carrier", tag);
			ships.push_back(newShip);
		}
	}
	for (int i = 0; i < BB; i++)
	{
		HoI4Ship newShip("Battleship", "battleship", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < BC; i++)
	{
		HoI4Ship newShip("Battle Cruiser", "battle_cruiser", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < HC; i++)
	{
		HoI4Ship newShip("Heavy Cruiser", "heavy_cruiser", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < LC; i++)
	{
		HoI4Ship newShip("Light Cruiser", "light_cruiser", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < DD; i++)
	{
		HoI4Ship newShip("Destroyer", "destroyer", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < SB; i++)
	{
		HoI4Ship newShip("Submarine", "submarine", tag);
		ships.push_back(newShip);
	}

	for (auto state : states)
	{
		if ((state.second->getOwner() == tag) && (state.second->getMainNavalLocation() != 0))
		{
			navalLocation = state.second->getMainNavalLocation();
		}
	}
}

void HoI4Country::convertAirforce()
{
	int airplanes = 0;
	for (auto army : srcCountry->getArmies())
	{
		for (auto regiment : army->getRegiments())
		{
			string type = regiment->getType();
			if (type == "plane")
			{
				airplanes += 1;
			}
		}
	}

	auto techItr = technologies.find("early_fighter");
	if (techItr != technologies.end())
	{
		int amount = Configuration::getForceMultiplier() * airplanes;
		if (amount == 0)
		{
			amount = 1;
		}
		HoI4Airplane newPlane(string("fighter_equipment_0"), tag, amount);
		planes.push_back(newPlane);
	}
	techItr = technologies.find("early_bomber");
	if (techItr != technologies.end())
	{
		HoI4Airplane newPlane(string("tac_bomber_equipment_0"), tag, Configuration::getForceMultiplier() * airplanes);
		planes.push_back(newPlane);
	}
	techItr = technologies.find("CAS1");
	if (techItr != technologies.end())
	{
		HoI4Airplane newPlane(string("CAS_equipment_1"), tag, Configuration::getForceMultiplier() * airplanes);
		planes.push_back(newPlane);
	}
}

void HoI4Country::convertArmyDivisions()
{
	// get the total number of source brigades and the number of source brigades per location
	int infantryBrigades = 0;
	int artilleryBrigades = 0;
	int supportBrigades = 0;
	int tankBrigades = 0;
	int cavalryBrigades = 0;
	int cavalrySupportBrigades = 0;
	int mountainBrigades = 0;
	const double adjustment = 0.1 * Configuration::getForceMultiplier();

	map<int, double> locations;
	int totalRegiments = 0;
	for (auto army : srcCountry->getArmies())
	{
		// get the number of source brigades per location
		int HoI4location = 0;
		auto provMapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(army->getLocation());
		if (provMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
		{
			for (auto HoI4ProvNum : provMapping->second)
			{
				if (HoI4ProvNum != 0 && provinces.find(HoI4ProvNum) != provinces.end())
				{
					HoI4location = HoI4ProvNum;
				}
			}
		}

		// no weight for locations we don't own
		totalRegiments += army->getRegiments().size();
		if (provinces.find(HoI4location) != provinces.end()) {
			locations[HoI4location] += army->getRegiments().size();
		}

		// get the total number of source brigades
		for (auto regiment : army->getRegiments())
		{
			string type = regiment->getType();
			if (type == "artillery")
			{
				infantryBrigades += 2;
				artilleryBrigades++;
			}
			else if (type == "cavalry")
			{
				cavalryBrigades += 3;
			}
			else if (type == "cuirassier" || type == "dragoon" || type == "hussar")
			{
				cavalryBrigades += 3;
				cavalrySupportBrigades++;
			}
			else if (type == "engineer")
			{
				supportBrigades += 3;
			}
			else if (type == "guard")
			{
				mountainBrigades += 2;
			}
			else if (type == "infantry")
			{
				infantryBrigades += 3;
			}
			else if (type == "irregular")
			{
				infantryBrigades += 1;
			}
			else if (type == "tank")
			{
				tankBrigades++;
			}
		}
	}
	double InfWep = 0;
	if (technologies.find("infantry_weapons1") == technologies.end())
	{
		InfWep = 0.3;
	}
	if (technologies.find("infantry_weapons1") != technologies.end())
	{
		InfWep = 0.5;
	}
	armyStrength = 0;
	armyStrength = static_cast<long>((100 * InfWep*infantryBrigades) + (supportBrigades * 175) + (artilleryBrigades * 126) + (tankBrigades * 1135) + (cavalryBrigades * 120 * InfWep) + (mountainBrigades * 140 * InfWep));
	// calculate the number of brigades in different types of divisions
	int infantryPerDivision = 0;
	if (infantryBrigades <= 45)
	{
		infantryPerDivision = 3;
	}
	else if (infantryBrigades <= 90)
	{
		infantryPerDivision = 6;
	}
	else
	{
		infantryPerDivision = 9;
	}

	int tanksPerDivision = 0;
	if (tankBrigades <= 5)
	{
		tanksPerDivision = 1;
	}
	else if (tankBrigades <= 10)
	{
		tanksPerDivision = 2;
	}
	else
	{
		tanksPerDivision = 3;
	}

	int cavalryPerDivision = 0;
	if (cavalryBrigades <= 9)
	{
		cavalryPerDivision = 1;
	}
	else if (cavalryBrigades <= 18)
	{
		cavalryPerDivision = 2;
	}
	else
	{
		cavalryPerDivision = 3;
	}

	int mountainPerDivision = 0;
	if (mountainBrigades <= 9)
	{
		mountainPerDivision = 1;
	}
	else if (mountainBrigades <= 18)
	{
		mountainPerDivision = 2;
	}
	else
	{
		mountainPerDivision = 3;
	}

	int advancedIndex = -1;
	int mediumIndex = -1;
	int basicIndex = -1;
	int tankIndex = -1;
	bool mediumSupport = false;
	// create division templates
	if (tankBrigades > 0)
	{
		HoI4DivisionTemplateType newDivisionTemplate("Tank Division");
		for (int i = 0; i < tanksPerDivision; i++)
		{
			HoI4RegimentType lightArmorRegimentOne("light_armor", 0, i);
			newDivisionTemplate.addRegiment(lightArmorRegimentOne);
			HoI4RegimentType lightArmorRegimentTwo("light_armor", 1, i);
			newDivisionTemplate.addRegiment(lightArmorRegimentTwo);
			HoI4RegimentType motorizedrRegiment("motorized", 2, i);
			newDivisionTemplate.addRegiment(motorizedrRegiment);
		}
		tankIndex = divisionTemplates.size();
		divisionTemplates.push_back(newDivisionTemplate);
	}
	if (cavalryBrigades > 0)
	{
		HoI4DivisionTemplateType newDivisionTemplate("Cavalry Division");
		for (int i = 0; i < cavalryPerDivision; i++)
		{
			HoI4RegimentType cavalryRegimentOne("cavalry", 0, i);
			newDivisionTemplate.addRegiment(cavalryRegimentOne);
			HoI4RegimentType cavalryRegimentTwo("cavalry", 1, i);
			newDivisionTemplate.addRegiment(cavalryRegimentTwo);
			HoI4RegimentType cavalryRegimentThree("cavalry", 2, i);
			newDivisionTemplate.addRegiment(cavalryRegimentThree);
		}
		divisionTemplates.push_back(newDivisionTemplate);
	}
	if (mountainBrigades > 0)
	{
		HoI4DivisionTemplateType newDivisionTemplate("Mountaineers");
		for (int i = 0; i < cavalryPerDivision; i++)
		{
			HoI4RegimentType mountainRegimentOne("mountaineers", 0, i);
			newDivisionTemplate.addRegiment(mountainRegimentOne);
			HoI4RegimentType mountainRegimentTwo("mountaineers", 1, i);
			newDivisionTemplate.addRegiment(mountainRegimentTwo);
			HoI4RegimentType mountainRegimentThree("mountaineers", 2, i);
			newDivisionTemplate.addRegiment(mountainRegimentThree);
		}
		divisionTemplates.push_back(newDivisionTemplate);
	}
	if ((artilleryBrigades > 0) || (supportBrigades > 0))
	{
		if (3 * artilleryBrigades > infantryPerDivision * supportBrigades)
		{
			//there are more brigades with artillery than with support, meddiv will have only art
			HoI4DivisionTemplateType newDivisionTemplate("Support Infantry Division");
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegiment("infantry", 0, i);
				newDivisionTemplate.addRegiment(infantryRegiment);
			}
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegimentOne("infantry", i, 0);
				newDivisionTemplate.addRegiment(infantryRegimentOne);
				HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
				newDivisionTemplate.addRegiment(infantryRegimentTwo);
				HoI4RegimentType infantryRegimentThree("infantry", i, 2);
				newDivisionTemplate.addRegiment(infantryRegimentThree);
				HoI4RegimentType artilleryRegiment("artillery_brigade", i, 3);
				newDivisionTemplate.addRegiment(artilleryRegiment);
			}
			mediumIndex = divisionTemplates.size();
			divisionTemplates.push_back(newDivisionTemplate);

			if (supportBrigades > 0)
			{
				//have both support brigades and artillery, have superdiv
				HoI4DivisionTemplateType newDivisionTemplate("Advance Infantry Division");
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegiment("infantry", 0, i);
					newDivisionTemplate.addRegiment(infantryRegiment);
				}
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegimentOne("infantry", i, 0);
					newDivisionTemplate.addRegiment(infantryRegimentOne);
					HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
					newDivisionTemplate.addRegiment(infantryRegimentTwo);
					HoI4RegimentType infantryRegimentThree("infantry", i, 2);
					newDivisionTemplate.addRegiment(infantryRegimentThree);
					HoI4RegimentType artilleryRegiment("artillery_brigade", i, 3);
					newDivisionTemplate.addRegiment(artilleryRegiment);
				}
				HoI4RegimentType engineerRegiment("engineer", 0, 0);
				newDivisionTemplate.addSupportRegiment(engineerRegiment);
				HoI4RegimentType reconRegiment("recon", 0, 1);
				newDivisionTemplate.addSupportRegiment(reconRegiment);

				advancedIndex = divisionTemplates.size();
				divisionTemplates.push_back(newDivisionTemplate);
			}
		}
		else
		{
			//there are more brigades with support then artillery, meddiv will have only support
			HoI4DivisionTemplateType newDivisionTemplate("Support Infantry Division");
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegiment("infantry", 0, i);
				newDivisionTemplate.addRegiment(infantryRegiment);
			}
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegimentOne("infantry", i, 0);
				newDivisionTemplate.addRegiment(infantryRegimentOne);
				HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
				newDivisionTemplate.addRegiment(infantryRegimentTwo);
				HoI4RegimentType infantryRegimentThree("infantry", i, 2);
				newDivisionTemplate.addRegiment(infantryRegimentThree);
			}
			HoI4RegimentType engineerRegiment("engineer", 0, 0);
			newDivisionTemplate.addSupportRegiment(engineerRegiment);
			HoI4RegimentType reconRegiment("recon", 0, 1);
			newDivisionTemplate.addSupportRegiment(reconRegiment);
			mediumSupport = true;
			mediumIndex = divisionTemplates.size();
			divisionTemplates.push_back(newDivisionTemplate);

			if (artilleryBrigades != 0)
			{
				//have both supportbrigs and artillery, have superdiv
				HoI4DivisionTemplateType newDivisionTemplate("Advance Infantry Division");
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegiment("infantry", 0, i);
					newDivisionTemplate.addRegiment(infantryRegiment);
				}
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegimentOne("infantry", i, 0);
					newDivisionTemplate.addRegiment(infantryRegimentOne);
					HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
					newDivisionTemplate.addRegiment(infantryRegimentTwo);
					HoI4RegimentType infantryRegimentThree("infantry", i, 2);
					newDivisionTemplate.addRegiment(infantryRegimentThree);
					HoI4RegimentType artilleryRegiment("artillery_brigade", i, 3);
					newDivisionTemplate.addRegiment(artilleryRegiment);
				}
				HoI4RegimentType engineerRegiment("engineer", 0, 0);
				newDivisionTemplate.addSupportRegiment(engineerRegiment);
				HoI4RegimentType reconRegiment("recon", 0, 1);
				newDivisionTemplate.addSupportRegiment(reconRegiment);
				advancedIndex = divisionTemplates.size();
				divisionTemplates.push_back(newDivisionTemplate);
			}
		}
	}

	// basic infantry division
	HoI4DivisionTemplateType newDivisionTemplate("Basic Infantry Division");
	for (int i = 0; i < (infantryPerDivision / 3); i++)
	{
		HoI4RegimentType infantryRegiment("infantry", 0, i);
		newDivisionTemplate.addRegiment(infantryRegiment);
	}
	for (int i = 0; i < (infantryPerDivision / 3); i++)
	{
		HoI4RegimentType infantryRegimentOne("infantry", i, 0);
		newDivisionTemplate.addRegiment(infantryRegimentOne);
		HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
		newDivisionTemplate.addRegiment(infantryRegimentTwo);
		HoI4RegimentType infantryRegimentThree("infantry", i, 2);
		newDivisionTemplate.addRegiment(infantryRegimentThree);
	}
	basicIndex = divisionTemplates.size();
	divisionTemplates.push_back(newDivisionTemplate);

	// calculate number of units per location
	double totalWeight = 0;
	if (0 == locations.size())
	{
		locations[capital] = totalRegiments;
	}
	for (auto const location : locations)
	{
		totalWeight += location.second;
	}
	int numberOfDivisions = infantryBrigades / infantryPerDivision;
	for (auto& location : locations)
	{
		if (totalWeight > 0)
		{
			// Use ceiling here to avoid losing units to, eg, numberOfDivisions = 12,
			// totalWeight = 13. This can happen in the presence of aircraft.
			location.second = ceil(location.second * adjustment * numberOfDivisions / totalWeight);
		}
	}

	// place units
	int numCav = 1;
	int numTank = 1;
	int numMountain = 1;
	int numAdvanced = 1;
	int numMedium = 1;
	int numBasic = 1;

	infantryBrigades = static_cast<int>(0.5 + adjustment * infantryBrigades);
	artilleryBrigades = static_cast<int>(0.5 + adjustment * artilleryBrigades);
	supportBrigades = static_cast<int>(0.5 + adjustment * supportBrigades);
	tankBrigades = static_cast<int>(0.5 + adjustment * tankBrigades);
	cavalryBrigades = static_cast<int>(0.5 + adjustment * cavalryBrigades);
	cavalrySupportBrigades = static_cast<int>(0.5 + adjustment * cavalrySupportBrigades);
	mountainBrigades = static_cast<int>(0.5 + adjustment * mountainBrigades);

	for (auto const location : locations)
	{
		int unitsInProv = 0;
		while (unitsInProv < location.second)
		{
			if (infantryBrigades >= infantryPerDivision)
			{
				int infLocation = 0;
				if (location.first != 0)
				{
					infLocation = location.first;
				}
				else if (capital != 0)
				{
					infLocation = capital;
				}
				else
				{
					LOG(LogLevel::Warning) << "When converting units for " << tag << ", one of the locations for unit placement was undefined!";
					break;
				}
				if (tankBrigades > 0)
				{
					HoI4DivisionType newDivision(to_string(numTank++) + ". Tank Division", "Tank Division", infLocation);
					divisions.push_back(newDivision);
					tankBrigades -= tanksPerDivision;
				}
				if (cavalryBrigades > 0)
				{
					HoI4DivisionType newDivision(to_string(numCav++) + ". Cavalry Division", "Cavalry Division", infLocation);
					divisions.push_back(newDivision);
					cavalryBrigades -= cavalryPerDivision;
				}
				if (mountainBrigades > 0)
				{
					HoI4DivisionType newDivision(to_string(numMountain++) + ". Mountaineers", "Mountaineers", infLocation);
					divisions.push_back(newDivision);
					mountainBrigades -= mountainPerDivision;
				}

				if ((3 * artilleryBrigades >= infantryPerDivision) && (supportBrigades >= 1))
				{
					// Super Placement
					HoI4DivisionType newDivision(to_string(numAdvanced++) + ". Advance Infantry Division", "Advance Infantry Division", infLocation);
					divisions.push_back(newDivision);
					artilleryBrigades -= (infantryPerDivision / 3);
					supportBrigades--;
				}
				else if ((3 * artilleryBrigades >= infantryPerDivision) || (supportBrigades >= 1))
				{
					// Med Placement
					HoI4DivisionType newDivision(to_string(numMedium++) + ". Support Infantry Division", "Support Infantry Division", infLocation);
					divisions.push_back(newDivision);
					artilleryBrigades -= (infantryPerDivision / 3);
					supportBrigades--;
				}
				else
				{
					// Bad Placement
					HoI4DivisionType newDivision(to_string(numBasic++) + ". Basic Infantry Division", "Basic Infantry Division", infLocation);
					divisions.push_back(newDivision);
				}
				infantryBrigades -= infantryPerDivision;
				unitsInProv++;
			}
			else
				break;
		}
	}
	if (artilleryBrigades >= numAdvanced && advancedIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, 2);
		divisionTemplates[advancedIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numAdvanced;
	}
	if (artilleryBrigades >= numMedium && mediumIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, mediumSupport ? 2 : 0);
		divisionTemplates[mediumIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numMedium;
	}
	if (artilleryBrigades >= numBasic && basicIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, 0);
		divisionTemplates[basicIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numBasic;
	}
	if (artilleryBrigades >= numTank && tankIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, 0);
		divisionTemplates[tankIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numTank;
	}
}

void HoI4Country::addProvince(int _province)
{
	provinces.insert(_province);
	if (capital == 0)
	{
		capital = _province;
	}
}

void HoI4Country::addState(HoI4State* _state)
{
	states.insert(make_pair(_state->getID(), _state));
	for (const auto province : _state->getProvinces()) {
		addProvince(province);
	}
}

HoI4Relations* HoI4Country::getRelations(string withWhom) const
{
	map<string, HoI4Relations*>::const_iterator i = relations.find(withWhom);
	if (i != relations.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}

vector<int> HoI4Country::getPortProvinces(vector<int> locationCandidates, map<int, HoI4Province*> allProvinces)
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

	for (auto litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		auto black = std::find(port_blacklist.begin(), port_blacklist.end(), *litr);
		if (black != port_blacklist.end())
		{
			locationCandidates.erase(litr);
			break;
		}
	}
	for (auto litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		auto pitr = allProvinces.find(*litr);
		if (pitr != allProvinces.end())
		{
			if (!pitr->second->hasNavalBase())
			{
				locationCandidates.erase(litr);
				--pitr;
				break;
			}
		}
	}

	return locationCandidates;
}


void HoI4Country::setTechnology(string tech, int level)
{
	// don't allow downgrades
	map<string, int>::iterator techEntry = technologies.find(tech);
	if (techEntry == technologies.end() || technologies[tech] < level)
		technologies[tech] = level;
}

void HoI4Country::calculateIndustry()
{
	militaryFactories = 0.0;
	civilianFactories = 0.0;
	dockyards = 0.0;
	for (auto state : states)
	{
		civilianFactories += state.second->getCivFactories();
		militaryFactories += state.second->getMilFactories();
		dockyards += state.second->getDockyards();
	}
}

void HoI4Country::reportIndustry(ofstream& out)
{
	if (states.size() > 0)
	{
		out << tag << ',';
		out << militaryFactories << ',';
		out << civilianFactories << ',';
		out << dockyards << ',';
		out << militaryFactories + civilianFactories + dockyards << '\n';
	}
}

void HoI4Country::addVPsToCapital(int VPs)
{
	auto capital = getCapital();
	if (capital != NULL)
	{
		capital->addVictoryPointValue(VPs);
	}
}

HoI4State* HoI4Country::getCapital(void)
{
	auto capitalItr = states.find(capital);
	if (capitalItr == states.end())
	{
		if (states.size() > 0)
		{
			capitalItr = states.begin();
		}
		else
		{
			return nullptr;
		}
	}

	return capitalItr->second;
}

double HoI4Country::getStrengthOverTime(double years) const
{
	return getMilitaryStrength() + getEconomicStrength(years);
}

double HoI4Country::getMilitaryStrength() const
{
	return armyStrength;
}

double HoI4Country::getEconomicStrength(double years) const
{
	double militarySectorStrength = militaryFactories * 3 * 365 * years;
	double civilianSectorStrength = civilianFactories * 0.469 * 0.5 * 3 * 365 * 0.5* years * years; /*.469 is milfac per year, .5 since half are used by consumer goods*/

	return militarySectorStrength + civilianSectorStrength;
}

void HoI4Country::addPuppet(string countryTag)
{
	puppets.insert(countryTag);
	return;
}


void HoI4Country::outputToCommonCountriesFile(ofstream& countriesFile) const
{
	countriesFile << tag.c_str() << " = \"countries/" << Utils::convertUTF8ToASCII(commonCountryFile) << "\"\n";
}


void HoI4Country::outputColors(ofstream& out) const
{
	out << tag << " = {\n";
	out << "\tcolor = rgb { " << color << " }\n";
	out << "\tcolor_ui = rgb { " << color << " }";
	out << "}\n";
}


void HoI4Country::outputToNamesFiles(ofstream& namesFile) const
{
	namesFile << tag << " = {\n";

	namesFile << "\tmale = {\n";
	namesFile << "\t\tnames = {\n";
	outputNamesSet(namesFile, namesMapper::getMaleNames(srcCountry->getPrimaryCulture()), "\t\t\t");
	namesFile << "\t\t}\n";
	namesFile << "\t}\n";

	namesFile << "\tfemale = {\n";
	namesFile << "\t\tnames = {\n";
	outputNamesSet(namesFile, namesMapper::getFemaleNames(srcCountry->getPrimaryCulture()), "\t\t\t");
	namesFile << "\t\t}\n";
	namesFile << "\t}\n";

	namesFile << "\tsurnames = {\n";
	outputNamesSet(namesFile, namesMapper::getSurnames(srcCountry->getPrimaryCulture()), "\t\t");
	namesFile << "\t}\n";

	namesFile << "\tcallsigns = {\n";
	outputNamesSet(namesFile, namesMapper::getCallsigns(srcCountry->getPrimaryCulture()), "\t\t");
	namesFile << "\t}\n";

	namesFile << "}\n";
}


void HoI4Country::outputNamesSet(ofstream& namesFile, const vector<string>& names, const string& tabs) const
{
	if (names[0] != "null")
	{
		namesFile << tabs;

		for (unsigned int i = 0; i < names.size(); i++)
		{
			namesFile << '\"' << names[i] << '\"';
			if (i == names.size())
			{
				continue;
			}
			else if (((i + 1) % 10) == 0)
			{
				namesFile << "\n";
				namesFile << tabs;
			}
			else
			{
				namesFile << " ";
			}
		}

		namesFile << '\n';
	}
}


void HoI4Country::output(const map<int, HoI4State*>& states, const vector<HoI4Faction*>& Factions) const
{
	outputHistory(states, Factions);
	outputOOB();
	outputCommonCountryFile();
	outputIdeas();

	//fprintf(output, "graphical_culture = %s\n", graphicalCulture.c_str());

	if (nationalFocus != nullptr)
	{
		nationalFocus->output();
	}
}


void HoI4Country::outputHistory(const map<int, HoI4State*>& states, const vector<HoI4Faction*>& Factions) const
{
	ofstream output("Output/" + Configuration::getOutputName() + "/history/countries/" + Utils::convertUTF8ToASCII(filename));
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/history/countries" << Utils::convertUTF8ToASCII(filename);
		exit(-1);
	}
	output << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy

	if (((capital > 0) && (capital <= static_cast<int>(states.size()))))
	{
		output << "capital = " << capital << '\n';
	}
	else if (states.size() > 0)
	{
		output << "capital = " << states.begin()->first << '\n';
	}
	else
	{
		output << "capital = 1\n";
	}

	if (majorNation)
	{
		output << "set_research_slots = 4\n";
	}
	
	if(threat!=0)
	{
		output<<"add_named_threat = { threat = "<<threat<<" name = badboy }\n";
	}
	output << "\n";
	output << "oob = \"" << tag << "_OOB\"\n";
	output << "\n";

	output << "# Starting tech\n";
	output << "set_technology = {\n";
	for (auto tech : technologies)
	{
		output << tech.first << " = 1\n";
	}
	output << "}\n";

	output << "set_convoys = " << convoys << '\n';
	output << "\n";

	if (puppets.size() > 0)
	{
		output << "# DIPLOMACY\n";
		output << "if = {\n";
		output << "    limit = {\n";
		output << "        has_dlc = \"Together for Victory\"\n";
		output << "    }\n";
		for (auto puppet : puppets)
		{
			output << "    set_autonomy = {\n";
			output << "        target = " << puppet << "\n";
			output << "        autonomous_state = autonomy_dominion\n";
			output << "        freedom_level = 0.4\n";
			output << "    }\n";
		}
		output << "    else = {\n";
		for (auto puppet : puppets)
		{
			output << "        puppet = " << puppet << "\n";
		}
		output << "    }\n";
		output << "}\n";
		output << "\n";

		output << "if = {\n";
		output << "    limit = {has_dlc = \"Together for Victory\" }\n";
		output << "\n";
		output << "    add_to_tech_sharing_group = " << tag <<"_research\n";
		output << "}\n\n";
	}

	if (puppetMaster != "")
	{
		output << "if = {\n";
		output << "    limit = {has_dlc = \"Together for Victory\" }\n";
		output << "\n";
		output << "    add_to_tech_sharing_group = " << puppetMaster << "_research\n";
		output << "}\n\n";
	}

	output << "set_politics = {\n";
	output << "\n";
	output << "    parties = {\n";
	for (auto ideology: ideologySupport)
	{
		output << "        " << ideology.first << " = {\n";
		output << "            popularity = " << ideology.second << "\n";
		output << "        }\n";
	}
	output << "    }\n";
	output << "    \n";

	output << "    ruling_party = " << governmentIdeology << "\n";
	output << "    last_election = \"1936.1.1\"\n";
	output << "    election_frequency = 48\n";
	output << "    elections_allowed = no\n";
	output << "}\n";

	outputRelations(output);
	output << "\n";

	for (auto Faction : Factions)
	{
		if (Faction->getLeader()->getTag() == tag)
		{
			output << "create_faction = \"Alliance of " + getSourceCountry()->getName("english") + "\"\n";
			for (auto factionmem : Faction->getMembers())
			{
				output << "add_to_faction = " + factionmem->getTag() + "\n";
			}
		}
	}
	output << '\n';

	output << "add_ideas = {\n";
	if (majorNation)
	{
		output << "\tgreat_power\n";
	}
	if (!civilized)
	{
		output << "\tuncivilized\n";
	}
	if (rulingParty->war_policy == "jingoism")
	{
		output << "\tpartial_economic_mobilisation\n";
	}
	if (rulingParty->war_policy == "pro_military")
	{
		output << "\tlow_economic_mobilisation\n";
	}
	output << "}\n";

	outputCountryLeader(output);

	output << "\n";
	output << "1939.1.1 = {\n";
	output << "}\n" << endl;
	output.close();
}


void HoI4Country::outputRelations(ofstream& output) const
{
	for (auto relation: relations)
	{
		if (relation.first != tag)
		{
			output << "add_opinion_modifier = { target = " << relation.first << " modifier = ";
			if (relation.second->getRelations() < 0)
			{
				output << "negative_";
			}
			else
			{
				output << "positive_";
			}
			output << abs(relation.second->getRelations()) << " }\n";
		}
	}
}


void HoI4Country::outputCountryLeader(ofstream& output) const
{
	string firstName = namesMapper::getMaleName(srcCountry->getPrimaryCulture());
	string surname = namesMapper::getSurname(srcCountry->getPrimaryCulture());
	string portrait = graphicsMapper::getLeaderPortrait(srcCountry->getPrimaryCultureGroup(), governmentIdeology);

	output << "create_country_leader = {\n";
	output << "    name = \"" << firstName << " " << surname << "\"\n";
	output << "    desc = \"POLITICS_" << boost::to_upper_copy(firstName) << "_" << boost::to_upper_copy(surname) << "_DESC\"\n";
	output << "    picture = \"" << portrait << "\"\n";
	output << "    expire = \"1965.1.1\"\n";
	output << "    ideology = " << leaderIdeology << "\n";
	output << "    traits = {\n";
	output << "    }\n";
	output << "}\n";
}


void HoI4Country::outputOOB() const
{
	ofstream output("Output/" + Configuration::getOutputName() + "/history/units/" + tag + "_OOB.txt");
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open Output/" << Configuration::getOutputName() << "/history/units/" << tag << "_OOB.txt";
		exit(-1);
	}
	output << "\xEF\xBB\xBF";	// add the BOM to make HoI4 happy

										/*for (auto armyItr: armies)
										{
										if (armyItr->getProductionQueue())
										{
										armyItr->outputIntoProductionQueue(output, tag);
										}
										else
										{
										armyItr->output(output);
										}
										}*/
	output << "start_equipment_factor = 0\n";
	for (auto divisionTemplate : divisionTemplates)
	{
		output << divisionTemplate;
		output << endl;
	}
	output << "### No BHU air forces ###\n";
	output << "instant_effect = {\n";
	output << "\tadd_equipment_production = {\n";
	output << "\t\tequipment = {\n";
	output << "\t\t\ttype = infantry_equipment_0\n";
	output << "\t\t\tcreator = \"" << tag << "\"\n";
	output << "\t\t}\n";
	output << "\t\trequested_factories = 1\n";
	output << "\t\tprogress = 0.88\n";
	output << "\t\tefficiency = 100\n";
	output << "\t}\n";
	output << "}\n";
	output << "units = {\n";
	for (auto division : divisions)
	{
		output << division;
	}
	if (ships.size() > 0)
	{
		output << "\tnavy = {" << endl;
		output << "\t\tname = \"Grand Fleet\"" << endl;
		output << "\t\tlocation = " << navalLocation << endl;
		for (auto ship : ships)
		{
			output << ship;
		}
		output << "\t}" << endl;
	}
	output << "}\n";
	if (planes.size() > 0)
	{
		output << "air_wings = {\n";
		output << "\t" << capital << " = {\n";
		for (auto plane : planes)
		{
			output << plane;
		}
		output << "\t}\n";
		output << "}\n";
	}
	output.close();
}


void HoI4Country::outputCommonCountryFile() const
{
	ofstream output("Output/" + Configuration::getOutputName() + "/common/countries/" + Utils::convertUTF8ToASCII(commonCountryFile));
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/common/countries/" << Utils::convertUTF8ToASCII(commonCountryFile);
		exit(-1);
	}

	output << "color = { " << color << " }" << endl;

	output.close();
}


void HoI4Country::outputIdeas() const
{
	ofstream ideasFile("Output/" + Configuration::getOutputName() + "/common/ideas/" + tag + ".txt");
	if (!ideasFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open Output/" << Configuration::getOutputName() << "/common/ideas/" << tag << ".txt";
		exit(-1);
	}

	ideasFile << "ideas = {\n";
	ideasFile << "\tpolitical_advisor = {\n";

	ideasFile << "\t\t" << tag << "_communist_advisor = {\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = \"" << tag << "\"\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\ttraits = { communist_revolutionary }\n";
	ideasFile << "\n";
	ideasFile << "\t\t\ton_add = {\n";
	ideasFile << "\t\t\t\tcountry_event = political.1\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\tdo_effect = {\n";
	ideasFile << "\t\t\t\tNOT = {\n";
	ideasFile << "\t\t\t\t\thas_government = communism\n";
	ideasFile << "\t\t\t\t}\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\tai_will_do = {\n";
	ideasFile << "\t\t\t\tfactor = 0\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t}\n";

	ideasFile << "\t\t" << tag << "_democratic_advisor = {\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = \"" << tag << "\"\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\ttraits = { democratic_reformer }\n";
	ideasFile << "\n";
	ideasFile << "\t\t\ton_add = {\n";
	ideasFile << "\t\t\t\tcountry_event = political.13\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\tdo_effect = {\n";
	ideasFile << "\t\t\t\tNOT = {\n";
	ideasFile << "\t\t\t\t\thas_government = democratic\n";
	ideasFile << "\t\t\t\t}\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\tai_will_do = {\n";
	ideasFile << "\t\t\t\tfactor = 0\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t}\n";

	ideasFile << "\t\t" << tag << "_fascist_advisor = {\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = \"" << tag << "\"\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\ttraits = { fascist_demagogue }\n";
	ideasFile << "\n";
	ideasFile << "\t\t\ton_add = {\n";
	ideasFile << "\t\t\t\tcountry_event = political.7\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\tdo_effect = {\n";
	ideasFile << "\t\t\t\tNOT = {\n";
	ideasFile << "\t\t\t\t\thas_government = fascism\n";
	ideasFile << "\t\t\t\t}\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\n";
	ideasFile << "\t\t\tai_will_do = {\n";
	ideasFile << "\t\t\t\tfactor = 0\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t}\n";

	ideasFile << "\t}\n";
	ideasFile << "}\n";
}


void HoI4Country::outputIdeaGraphics(ofstream& ideasFile) const
{
	ideasFile << "\n";
	ideasFile << "\tspriteType = {\n";
	ideasFile << "\t\tname = \"GFX_idea_" << tag << "_communist_advisor\"\n";
	ideasFile << "\t\ttexturefile = \"" << graphicsMapper::getIdeologyMinisterPortrait(srcCountry->getPrimaryCultureGroup(), "communism") << "\"\n";
	ideasFile << "\t}\n";

	ideasFile << "\n";
	ideasFile << "\tspriteType = {\n";
	ideasFile << "\t\tname = \"GFX_idea_" << tag << "_democratic_advisor\"\n";
	ideasFile << "\t\ttexturefile = \"" << graphicsMapper::getIdeologyMinisterPortrait(srcCountry->getPrimaryCultureGroup(), "democratic") << "\"\n";
	ideasFile << "\t}\n";

	ideasFile << "\n";
	ideasFile << "\tspriteType = {\n";
	ideasFile << "\t\tname = \"GFX_idea_" << tag << "_fascist_advisor\"\n";
	ideasFile << "\t\ttexturefile = \"" << graphicsMapper::getIdeologyMinisterPortrait(srcCountry->getPrimaryCultureGroup(), "fascism") << "\"\n";
	ideasFile << "\t}\n";
}