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
#include "HoI4Advisor.h"
#include "HoI4World.h"
#include "Log.h"
#include "ParadoxParserUTF8.h"
#include "HoI4Faction.h"
#include "HoI4Leader.h"
#include "HoI4Localisation.h"
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

	nationalUnity = 0.70;
	
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


void HoI4Country::initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const map<int, int>& stateMap, map<int, HoI4State*> states)
{
	srcCountry = _srcCountry;

	determineFilename();

	human = srcCountry->isHuman();
	color = srcCountry->getColor();
	civilized = srcCountry->isCivilized();
	threat = srcCountry->getBadBoy() / 10.0;
	graphicalCulture = graphicsMapper::getGraphicalCulture(srcCountry->getPrimaryCultureGroup());
	graphicalCulture2d = graphicsMapper::get2dGraphicalCulture(srcCountry->getPrimaryCultureGroup());

	convertGovernment(_srcWorld);
	initIdeas();

	nationalUnity = 0.70 + (srcCountry->getRevanchism() / 5.0) - (srcCountry->getWarExhaustion() / 100.0 / 2.5);

	//convertLaws();
	//convertLeaders(portraitMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap);
	convertRelations();

	determineCapitalFromVic2(stateMap, states);
	auto state = states.find(capital)->second;
	if (isThisStateOwnedByUs(state))
	{
		state->setAsCapitalState();
	}

	majorNation = srcCountry->isGreatNation();
}


void HoI4Country::determineFilename()
{
	filename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (filename == "")
	{
		filename = Utils::GetFileFromTag(Configuration::getHoI4Path() + "/history/countries/", tag);
	}
	if (filename == "")
	{
		filename = tag + " - " + commonCountryFile;
	}
}


void HoI4Country::convertGovernment(const V2World& sourceWorld)
{
	rulingParty = srcCountry->getRulingParty(sourceWorld.getParties());
	if (rulingParty == nullptr)
	{
		LOG(LogLevel::Error) << "Could not find the ruling party for " << srcCountry->getTag() << ". Most likely a mod was not included.";
		LOG(LogLevel::Error) << "Double-check your settings, and remember to included EU4 to Vic2 mods. See the FAQ for more information.";
		exit(-1);
	}

	governmentIdeology = governmentMapper::getIdeologyForCountry(srcCountry, rulingParty->ideology);
	leaderIdeology = governmentMapper::getLeaderIdeologyForCountry(srcCountry, rulingParty->ideology);
	parties = srcCountry->getActiveParties(sourceWorld.getParties());
}


void HoI4Country::initIdeas()
{
	HoI4Localisation::addIdeaLocalisation(tag + "_tank_manufacturer", namesMapper::getCarCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_motorized_equipment_manufacturer", namesMapper::getCarCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_infantry_equipment_manufacturer", namesMapper::getWeaponCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_artillery_manufacturer", namesMapper::getWeaponCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_light_aircraft_manufacturer", namesMapper::getAircraftCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_medium_aircraft_manufacturer", namesMapper::getAircraftCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_heavy_aircraft_manufacturer", namesMapper::getAircraftCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_naval_aircraft_manufacturer", namesMapper::getAircraftCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_naval_manufacturer", namesMapper::getNavalCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_industrial_concern", namesMapper::getIndustryCompanyName(srcCountry->getPrimaryCulture()));
	HoI4Localisation::addIdeaLocalisation(tag + "_electronics_concern", namesMapper::getElectronicCompanyName(srcCountry->getPrimaryCulture()));
}


/*void HoI4Country::convertLaws()
{
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
}*/


/*void HoI4Country::convertLeaders(portraitMapping& portraitMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	vector<V2Leader*> srcLeaders = srcCountry->getLeaders();
	for (auto srcLeader : srcLeaders)
	{
		HoI4Leader newLeader(srcLeader, tag, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}

	Configuration::setLeaderIDForNextCountry();
}*/


void HoI4Country::convertRelations()
{
	map<string, V2Relations*> srcRelations = srcCountry->getRelations();
	for (auto srcRelation: srcRelations)
	{
		const string& HoI4Tag = CountryMapper::getHoI4Tag(srcRelation.second->getTag());
		if (HoI4Tag != "")
		{
			auto newRelation = new HoI4Relations(HoI4Tag, srcRelation.second);
			relations.insert(make_pair(HoI4Tag, newRelation));
		}
	}
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
	for (auto core: state->getCores())
	{
		if (core == tag)
		{
			return true;
		}
	}

	return false;
}


void HoI4Country::setCapitalInCapitalState(int capitalProvince, const map<int, HoI4State*>& _states)
{
	auto capitalState = _states.find(capital);
	if (capitalState != _states.end())
	{
		capitalState->second->setVPLocation(capitalProvince);
	}
}

void HoI4Country::findBestCapital()
{
	capital = 0;
	LOG(LogLevel::Warning) << "Could not properly set capital for " << tag;
}


void HoI4Country::initFromHistory()
{
	string fullFilename;
	filename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (filename == "")
	{
		filename = Utils::GetFileFromTag(Configuration::getHoI4Path() + "/history/countries/", tag);
		fullFilename = Configuration::getHoI4Path() + "/history/countries/" + filename;
	}
	else
	{
		fullFilename = "./blankMod/output/history/countries/" + filename;
	}

	LOG(LogLevel::Debug) << "Parsing " << fullFilename;
	shared_ptr<Object> obj = parser_UTF8::doParseFile(fullFilename);
	if (obj == nullptr)
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
		if (majorIdeologies.count(ideology) == 0 && Configuration::getDropMinorIdeologies())
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
		int amount = static_cast<int>(Configuration::getForceMultiplier() * airplanes);
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
		HoI4Airplane newPlane(string("tac_bomber_equipment_0"), tag, static_cast<int>(Configuration::getForceMultiplier() * airplanes));
		planes.push_back(newPlane);
	}

	techItr = technologies.find("CAS1");
	if (techItr != technologies.end())
	{
		HoI4Airplane newPlane(string("CAS_equipment_1"), tag, static_cast<int>(Configuration::getForceMultiplier() * airplanes));
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


void HoI4Country::addState(HoI4State* _state)
{
	states.insert(make_pair(_state->getID(), _state));

	if (capital == 0)
	{
		capital = _state->getID();
		_state->setAsCapitalState();
	}

	for (const auto province: _state->getProvinces())
	{
		addProvince(province);
	}
}


void HoI4Country::addProvince(int _province)
{
	provinces.insert(_province);
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
		return nullptr;
	}
}


void HoI4Country::setTechnology(string tech, int level)
{
	map<string, int>::iterator techEntry = technologies.find(tech);
	if ((techEntry == technologies.end()) || (techEntry->second < level))
	{
		technologies[tech] = level;
	}
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
	if (capital != nullptr)
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
	double civilianSectorStrength = civilianFactories * 0.469 * 0.5 * 3 * 365 * 0.5 * years * years; /*.469 is milfac per year, .5 since half are used by consumer goods*/

	return militarySectorStrength + civilianSectorStrength;
}


void HoI4Country::outputToCommonCountriesFile(ofstream& countriesFile) const
{
	countriesFile << tag.c_str() << " = \"countries/" << Utils::normalizeUTF8Path(commonCountryFile) << "\"\n";
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


void HoI4Country::output(const set<const HoI4Advisor*>& ideologicalMinisters) const
{
	outputHistory();
	outputOOB();
	outputCommonCountryFile();
	outputIdeas(ideologicalMinisters);

	if (nationalFocus != nullptr)
	{
		nationalFocus->output("output/" + Configuration::getOutputName() + "/common/national_focus/" + srcCountry->getTag() + "_NF.txt");
	}
}


void HoI4Country::outputHistory() const
{
	ofstream output("output/" + Configuration::getOutputName() + "/history/countries/" + Utils::normalizeUTF8Path(filename));
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open " << "output/" << Configuration::getOutputName() << "/history/countries/" << Utils::normalizeUTF8Path(filename);
		exit(-1);
	}
	output << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy

	outputCapital(output);
	outputResearchSlots(output);
	outputThreat(output);
	outputOOB(output);
	outputTechnology(output);
	outputConvoys(output);
	outputPuppets(output);
	outputPolitics(output);
	outputRelations(output);
	outputFactions(output);
	outputIdeas(output);
	outputNationalUnity(output);
	outputCountryLeader(output);

	output.close();
}


void HoI4Country::outputCapital(ofstream& output) const
{
	if (capital > 0)
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
}


void HoI4Country::outputResearchSlots(ofstream& output) const
{
	if (majorNation)
	{
		output << "set_research_slots = 4\n";
	}
}


void HoI4Country::outputThreat(ofstream& output) const
{
	if (threat != 0.0)
	{
		output << "add_named_threat = { threat = "<< threat << " name = infamy }\n";
	}
	output << "\n";
}


void HoI4Country::outputOOB(ofstream& output) const
{
	output << "oob = \"" << tag << "_OOB\"\n";
	output << "\n";
}


void HoI4Country::outputTechnology(ofstream& output) const
{
	output << "# Starting tech\n";
	output << "set_technology = {\n";
	for (auto tech : technologies)
	{
		output << tech.first << " = 1\n";
	}
	output << "}\n";
}


void HoI4Country::outputConvoys(ofstream& output) const
{
	output << "set_convoys = " << convoys << '\n';
	output << "\n";
}


void HoI4Country::outputPuppets(ofstream& output) const
{
	if (puppets.size() > 0)
	{
		output << "# DIPLOMACY\n";
		output << "if = {\n";
		output << "    limit = {\n";
		output << "        has_dlc = \"Together for Victory\"\n";
		output << "    }\n";
		for (auto puppet : puppets)
		{
			if (governmentIdeology == "fascism")
			{
				output << "    set_autonomy = {\n";
				output << "        target = " << puppet << "\n";
				output << "        autonomous_state = autonomy_integrated_puppet\n";
				output << "    }\n";
			}
			else
			{
				output << "    set_autonomy = {\n";
				output << "        target = " << puppet << "\n";
				output << "        autonomous_state = autonomy_dominion\n";
				output << "        freedom_level = 0.4\n";
				output << "    }\n";
			}
		}
		output << "    else = {\n";
		for (auto puppet : puppets)
		{
			if (governmentIdeology == "fascism")
			{
				output << "        set_autonomy = {\n";
				output << "            target = " << puppet << "\n";
				output << "            autonomous_state = autonomy_puppet << \n";
				output << "        }\n";
			}
			else
			{
				output << "        puppet = " << puppet << "\n";
			}
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
}


void HoI4Country::outputPolitics(ofstream& output) const
{
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
	if (areElectionsAllowed())
	{
		output << "    elections_allowed = yes\n";
	}
	else
	{
		output << "    elections_allowed = no\n";
	}
	output << "}\n";
}


void HoI4Country::outputRelations(ofstream& output) const
{
	for (auto relation: relations)
	{
		if (relation.first != tag)
		{
			output << "add_opinion_modifier = { target = " << relation.first << " modifier = ";
			int relationsValue = relation.second->getRelations();
			if (relationsValue < 0)
			{
				output << "negative_";
			}
			else
			{
				output << "positive_";
			}
			output << abs(relationsValue) << " }\n";
		}
	}
	output << "\n";
}


bool HoI4Country::areElectionsAllowed(void) const
{
	if (
			(governmentIdeology == "democratic") ||
			(
				(governmentIdeology == "neutrality") &&
				(
					(leaderIdeology == "conservatism_neutral") ||
					(leaderIdeology == "liberalism_neutral") ||
					(leaderIdeology == "socialism_neutral")
				)
			)
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void HoI4Country::outputFactions(ofstream& output) const
{
	if ((faction != nullptr) && (faction->getLeader() == this))
	{
		output << "create_faction = \"Alliance of " + getSourceCountry()->getName("english") + "\"\n";
		for (auto factionMember : faction->getMembers())
		{
			output << "add_to_faction = " + factionMember->getTag() + "\n";
		}
	}

	output << '\n';
}


void HoI4Country::outputIdeas(ofstream& output) const
{
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
}


void HoI4Country::outputNationalUnity(ofstream& output) const
{
	if (states.size() > 0)
	{
		output << "set_national_unity = " << nationalUnity << "\n";
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
	ofstream output("output/" + Configuration::getOutputName() + "/history/units/" + tag + "_OOB.txt");
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open output/" << Configuration::getOutputName() << "/history/units/" << tag << "_OOB.txt";
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
	ofstream output("output/" + Configuration::getOutputName() + "/common/countries/" + Utils::normalizeUTF8Path(commonCountryFile));
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open " << "output/" << Configuration::getOutputName() << "/common/countries/" << Utils::normalizeUTF8Path(commonCountryFile);
		exit(-1);
	}

	if ((graphicalCulture != "") && (graphicalCulture2d != ""))
	{
		output << "graphical_culture = " << graphicalCulture << "\n";
		output << "graphical_culture_2d = " << graphicalCulture2d << "\n";
	}
	output << "color = { " << color << " }\n";

	output.close();
}


void HoI4Country::outputIdeas(const set<const HoI4Advisor*>& ideologicalAdvisors) const
{
	ofstream ideasFile("output/" + Configuration::getOutputName() + "/common/ideas/" + tag + ".txt");
	if (!ideasFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open output/" << Configuration::getOutputName() << "/common/ideas/" << tag << ".txt";
		exit(-1);
	}

	ideasFile << "ideas = {\n";
	ideasFile << "\tpolitical_advisor = {\n";
	for (auto ideologicalAdvisor: ideologicalAdvisors)
	{
		ideologicalAdvisor->output(ideasFile, tag);
	}
	ideasFile << "\t}\n";

	ideasFile << "\ttank_manufacturer = { \n";
	ideasFile << "\t\tdesigner = yes\n";
	ideasFile << "\t\t\n";
	ideasFile << "\t\t" << tag << "_tank_manufacturer = {\n";
	ideasFile << "\t\t\tpicture = generic_tank_manufacturer_1\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tarmor = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { tank_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t}\n";
	ideasFile << "\tnaval_manufacturer = { \n";
	ideasFile << "\t\tdesigner = yes\n";
	ideasFile << "\t\t" << tag << "_naval_manufacturer = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_naval_manufacturer_1\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tnaval_equipment = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { naval_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t}\n";
	ideasFile << "\taircraft_manufacturer = { \n";
	ideasFile << "\t\tdesigner = yes\n";
	ideasFile << "\t\t" << tag << "_light_aircraft_manufacturer = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_air_manufacturer_1\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tair_equipment = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { light_aircraft_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t\t\n";
	ideasFile << "\t\t" << tag << "_medium_aircraft_manufacturer = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_air_manufacturer_3\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tair_equipment = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { medium_aircraft_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t\t\n";
	ideasFile << "\t\t" << tag << "_heavy_aircraft_manufacturer = {\n";
	ideasFile << "\t\t\tpicture = generic_air_manufacturer_2\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tair_equipment = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { heavy_aircraft_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t\t\n";
	ideasFile << "\t\t" << tag << "_naval_aircraft_manufacturer = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_naval_manufacturer_2\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tair_equipment = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { naval_aircraft_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t}\n";
	ideasFile << "\tindustrial_concern = {\n";
	ideasFile << "\t\t" << tag << "_industrial_concern = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_industrial_concern_1\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tindustry = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { industrial_concern }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t\t" << tag << "_electronics_concern = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_electronics_concern_1\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\telectronics = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { electronics_concern }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t}\n";
	ideasFile << "\tmateriel_manufacturer = {\n";
	ideasFile << "\t\tdesigner = yes\n";
	ideasFile << "\t\t" << tag << "_motorized_equipment_manufacturer = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_motorized_equipment_manufacturer_3\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tmotorized_equipment = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { motorized_equipment_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t\t\n";
	ideasFile << "\t\t" << tag << "_infantry_equipment_manufacturer = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_infantry_equipment_manufacturer_2\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tinfantry_weapons = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { infantry_equipment_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t\t\n";
	ideasFile << "\t\t" << tag << "_artillery_manufacturer = {\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tpicture = generic_artillery_manufacturer_2\n";
	ideasFile << "\t\t\tallowed = {\n";
	ideasFile << "\t\t\t\toriginal_tag = " << tag << "\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tcost = 150\n";
	ideasFile << "\t\t\tremoval_cost = 10\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\tresearch_bonus = {\n";
	ideasFile << "\t\t\t\tartillery = 0.10\n";
	ideasFile << "\t\t\t}\n";
	ideasFile << "\t\t\t\n";
	ideasFile << "\t\t\ttraits = { artillery_manufacturer }\n";
	ideasFile << "\t\t}\n";
	ideasFile << "\t}\n";

	ideasFile << "}\n";
}


void HoI4Country::outputIdeaGraphics(ofstream& ideasFile) const
{

	ideasFile << "\tspriteType = {\n";
	ideasFile << "\t\tname = \"GFX_idea_" << tag << "_communist_advisor\"\n";
	ideasFile << "\t\ttexturefile = \"" << graphicsMapper::getIdeologyMinisterPortrait(srcCountry->getPrimaryCultureGroup(), "communism") << "\"\n";
	ideasFile << "\t}\n";


	ideasFile << "\tspriteType = {\n";
	ideasFile << "\t\tname = \"GFX_idea_" << tag << "_democratic_advisor\"\n";
	ideasFile << "\t\ttexturefile = \"" << graphicsMapper::getIdeologyMinisterPortrait(srcCountry->getPrimaryCultureGroup(), "democratic") << "\"\n";
	ideasFile << "\t}\n";


	ideasFile << "\tspriteType = {\n";
	ideasFile << "\t\tname = \"GFX_idea_" << tag << "_fascist_advisor\"\n";
	ideasFile << "\t\ttexturefile = \"" << graphicsMapper::getIdeologyMinisterPortrait(srcCountry->getPrimaryCultureGroup(), "fascism") << "\"\n";
	ideasFile << "\t}\n";
}