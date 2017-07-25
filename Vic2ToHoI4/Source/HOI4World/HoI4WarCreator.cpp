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



#include "HoI4WarCreator.h"
#include "Log.h"
#include "HoI4Events.h"
#include "HoI4Faction.h"
#include "HoI4Focus.h"
#include "HoI4World.h"
#include "../Mappers/ProvinceDefinitions.h"
#include "../Mappers/ProvinceNeighborMapper.h"
#include "../Mappers/StateMapper.h"
#include "../V2World/V2Party.h"



void HoI4WarCreator::generateWars(HoI4World* world)
{
	theWorld = world;

	ofstream AILog("AI-log.txt");

	genericFocusTree = new HoI4FocusTree;
	genericFocusTree->addGenericFocusTree(world->getMajorIdeologies());

	determineProvinceOwners();
	fillCountryProvinces();
	addAllTargetsToWorldTargetMap();
	double worldStrength = calculateWorldStrength(AILog);

	set<HoI4Faction*> factionsAtWar;
	generateMajorWars(AILog, factionsAtWar);
	generateAdditionalWars(AILog, factionsAtWar, worldStrength);

	AILog.close();
}


void HoI4WarCreator::addAllTargetsToWorldTargetMap()
{
	for (auto greatPower: theWorld->getGreatPowers())
	{
		addTargetsToWorldTargetMap(greatPower);
	}
}


void HoI4WarCreator::addTargetsToWorldTargetMap(HoI4Country* country)
{
	int maxGCWars = 0;
	if (country->getGovernmentIdeology() != "democratic")
	{
		vector<HoI4Country*> GCTargets;
		for (auto GC: getDistancesToGreatPowers(country))
		{
			if (maxGCWars < 2)
			{
				string thetag = GC.second->getTag();
				string HowToTakeGC = HowToTakeLand(GC.second, country, 3);
				if (HowToTakeGC == "noactionneeded" || HowToTakeGC == "factionneeded" || HowToTakeGC == "morealliesneeded")
				{
					if (GC.second != country)
					{
						int relations = country->getRelations(GC.second->getTag())->getRelations();
						if (relations < 0)
						{
							string prereq = "";
							vector<HoI4Country*> tempvector;
							if (WorldTargetMap.find(GC.second) == WorldTargetMap.end())
							{
								tempvector.push_back(country);
								WorldTargetMap.insert(make_pair(GC.second, tempvector));
							}
							if (WorldTargetMap.find(GC.second) != WorldTargetMap.end())
							{
								tempvector = WorldTargetMap.find(GC.second)->second;
								if (find(tempvector.begin(), tempvector.end(), country) == tempvector.end())
									tempvector.push_back(country);

								WorldTargetMap[GC.second] = tempvector;
							}
							maxGCWars++;
						}
					}
				}
			}
		}
	}
}


map<double, HoI4Country*> HoI4WarCreator::getDistancesToGreatPowers(HoI4Country* country)
{
	map<double, HoI4Country*> GCDistance;
	for (auto GC: theWorld->getGreatPowers())
	{
		set<string> Allies = country->getAllies();
		if (std::find(Allies.begin(), Allies.end(), GC->getTag()) == Allies.end())
		{
			double distance = getDistanceBetweenCountries(country, GC);
			if (distance < 2200)
				GCDistance.insert(make_pair(distance, GC));
		}
	}

	return GCDistance;
}


double HoI4WarCreator::calculateWorldStrength(ofstream& AILog)
{
	double worldStrength = 0.0;
	for (auto Faction: theWorld->getFactions())
	{
		worldStrength += GetFactionStrength(Faction, 3);
	}

	AILog << "Total world strength: " << worldStrength << "\n\n";
	return worldStrength;
}


void HoI4WarCreator::generateMajorWars(ofstream& AILog, set<HoI4Faction*>& factionsAtWar)
{
	AILog << "Creating major wars\n";

	for (auto country: theWorld->getCountries())
	{
		if (isImportantCountry(country.second))
		{
			vector<HoI4Faction*> newFactionsAtWar;

			if (country.second->getGovernmentIdeology() == "fascism")
			{
				newFactionsAtWar = fascistWarMaker(country.second, AILog);
			}
			else if (country.second->getGovernmentIdeology() == "communism")
			{
				newFactionsAtWar = communistWarCreator(country.second, AILog);
			}
			else if (country.second->getGovernmentIdeology() == "absolutist")
			{
				newFactionsAtWar = absolutistWarCreator(country.second);
			}
			else if (country.second->getGovernmentIdeology() == "radical")
			{
				newFactionsAtWar = radicalWarCreator(country.second);
			}
			else if (country.second->getGovernmentIdeology() == "democratic")
			{
				newFactionsAtWar = democracyWarCreator(country.second);
			}


			factionsAtWar.insert(newFactionsAtWar.begin(), newFactionsAtWar.end());
		}
	}
}


double HoI4WarCreator::calculatePercentOfWorldAtWar(ofstream& AILog, const set<HoI4Faction*>& factionsAtWar, double worldStrength)
{
	double countriesAtWarStrength = 0.0;
	for (auto faction : factionsAtWar)
	{
		countriesAtWarStrength += GetFactionStrength(faction, 3);
	}

	double percentOfWorldAtWar = countriesAtWarStrength / worldStrength;
	AILog << "Fraction of world at war " << percentOfWorldAtWar << "\n";

	return percentOfWorldAtWar;
}

void HoI4WarCreator::generateAdditionalWars(ofstream& AILog, set<HoI4Faction*>& factionsAtWar, double worldStrength)
{
	vector<HoI4Country*> countriesEvilnessSorted = findEvilCountries();

	for (int i = countriesEvilnessSorted.size() - 1; i >= 0; i--)
	{
		if (!isImportantCountry(countriesEvilnessSorted[i]))
		{
			AILog << "Checking for war in " + countriesEvilnessSorted[i]->getSourceCountry()->getName("english") << "\n";
			vector <HoI4Faction*> newCountriesatWar;
			newCountriesatWar = neighborWarCreator(countriesEvilnessSorted[i], AILog);

			for (auto addedFactions : newCountriesatWar)
			{
				if (std::find(factionsAtWar.begin(), factionsAtWar.end(), addedFactions) == factionsAtWar.end())
				{
					factionsAtWar.insert(addedFactions);
				}
			}
		}
	}
}

bool HoI4WarCreator::isImportantCountry(HoI4Country * country)
{
	if (country->isGreatPower() || country->isHuman())
	{
		return true;
	}
	return false;
}

vector<HoI4Country*> HoI4WarCreator::findEvilCountries()
{
	map<double, HoI4Country*> countryEvilness;
	vector<HoI4Country*> countriesEvilnessSorted;

	for (auto country : theWorld->getCountries())
	{
		double v1 = rand() % 95 + 1;
		v1 = v1 / 100;
		double evilness = v1;
		string government = "";
		if (country.second->getGovernmentIdeology() == "fascism")
			evilness += 5;
		if (country.second->getGovernmentIdeology() == "absolutist")
			evilness += 3;
		if (country.second->getGovernmentIdeology() == "communist")
			evilness += 3;
		if (country.second->getGovernmentIdeology() == "anarcho_liberal")
			evilness += 3;
		V2Party* countryrulingparty = country.second->getRulingParty();
	
		if (countryrulingparty->war_policy == "jingoism")
			evilness += 3;
		else if (countryrulingparty->war_policy == "pro_military")
			evilness += 2;
		else if (countryrulingparty->war_policy == "anti_military")
			evilness -= 1;
	
		if (evilness > 2)
		{
			countryEvilness.insert(make_pair(evilness, country.second));
		}
	}
	//put them into a vector so we know their order
	for (auto iterator = countryEvilness.begin(); iterator != countryEvilness.end(); ++iterator)
	{
		countriesEvilnessSorted.push_back(iterator->second);
	}

	return countriesEvilnessSorted;
}


void HoI4WarCreator::fillCountryProvinces()
{
	LOG(LogLevel::Info) << "Filling Map Information";

	for (auto country : theWorld->getCountries())
	{
		country.second->setProvinceCount(0);
	}
	for (auto state : theWorld->getStates())
	{
		auto ownercountry = theWorld->getCountries().find(state.second->getOwner());
		for (auto prov : state.second->getProvinces())
		{
			//fix later
			ownercountry->second->setProvinceCount(ownercountry->second->getProvinceCount() + 1);
		}
	}
}
void HoI4WarCreator::setSphereLeaders(const V2World* sourceWorld)
{
	for (auto greatPower: theWorld->getGreatPowers())
	{
		auto relations = greatPower->getRelations();
		for (auto relation : relations)
		{
			if (relation.second->getSphereLeader())
			{
				string tag = relation.second->getTag();
				auto spheredcountry = theWorld->getCountries().find(tag);
				if (spheredcountry != theWorld->getCountries().end())
				{
					spheredcountry->second->setSphereLeader(greatPower->getTag());
				}
			}
		}
	}
}


string HoI4WarCreator::HowToTakeLand(HoI4Country* TargetCountry, HoI4Country* AttackingCountry, double time)
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
vector<HoI4Country*> HoI4WarCreator::GetMorePossibleAllies(HoI4Country* CountryThatWantsAllies)
{
	int maxcountries = 0;
	vector<HoI4Country*> newPossibleAllies;
	set<string> currentAllies = CountryThatWantsAllies->getAllies();
	vector<HoI4Country*> CountriesWithin1000Miles; //Rename to actual distance
	for (auto country : theWorld->getCountries())
	{
		if (country.second->getProvinceCount() != 0)
		{
			HoI4Country* country2 = country.second;
			if (getDistanceBetweenCountries(CountryThatWantsAllies, country2) <= 1000
				&& country2 != CountryThatWantsAllies)
				if (std::find(currentAllies.begin(), currentAllies.end(), country2->getTag()) == currentAllies.end())
				{
					CountriesWithin1000Miles.push_back(country2);
				}
		}
	}
	string yourIdeology = CountryThatWantsAllies->getGovernmentIdeology();
	//look for all capitals within a distance of Berlin to Tehran
	for (unsigned int i = 0; i < CountriesWithin1000Miles.size(); i++)
	{
		string allyIdeology = CountriesWithin1000Miles[i]->getGovernmentIdeology();
		//possible government matches
		if (
				(allyIdeology == yourIdeology) /* ||
				// add other possible combinations here, but maybe coordinate with HoI4World::governmentsAllowFaction()
				*/
			)
		{

			if (maxcountries < 2)
			{
				//FIXME
				//check if we are friendly at all?
				HoI4Relations* relationswithposally = CountryThatWantsAllies->getRelations(CountriesWithin1000Miles[i]->getTag());
				int rel = relationswithposally->getRelations();
				int size = findFaction(CountriesWithin1000Miles[i])->getMembers().size();
				double armysize = CountriesWithin1000Miles[i]->getStrengthOverTime(1.0);
				//for now can only ally with people not in a faction, and must be worth adding
				if (relationswithposally->getRelations() >= -50 && findFaction(CountriesWithin1000Miles[i])->getMembers().size() <= 1)
				{
					//ok we dont hate each other, lets check how badly we need each other, well I do, the only reason I am here is im trying to conquer a neighbor and am not strong enough!
					//if (GetFactionStrength(findFaction(country)) < 20000) //maybe also check if he has any fascist/comm neighbors he doesnt like later?

					//well that ally is weak, he probably wants some friends
					if (relationswithposally->getRelations() >= -50 && relationswithposally->getRelations() < 0)
					{
						//will take some NF to ally
						newPossibleAllies.push_back(CountriesWithin1000Miles[i]);
						maxcountries++;
					}
					if (relationswithposally->getRelations() >= 0)
					{
						//well we are positive, 1 NF to add to ally should be fine
						newPossibleAllies.push_back(CountriesWithin1000Miles[i]);
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


double HoI4WarCreator::getDistanceBetweenCountries(const HoI4Country* country1, const HoI4Country* country2)
{
	if (!bothCountriesHaveCapitals(country1, country2))
	{
		return 100000;
	}

	pair<int, int> country1Position = getCapitalPosition(country1);
	pair<int, int> country2Position = getCapitalPosition(country2);

	return getDistanceBetweenPoints(country1Position, country2Position);
}


bool HoI4WarCreator::bothCountriesHaveCapitals(const HoI4Country* Country1, const HoI4Country* Country2)
{
	return (Country1->getCapitalProv() != 0) && (Country2->getCapitalProv() != 0);
}


pair<int, int> HoI4WarCreator::getCapitalPosition(const HoI4Country* country)
{
	auto capitalState = theWorld->getStates().find(country->getCapitalProv());
	int capitalProvince = *(capitalState->second->getProvinces().begin());
	return getProvincePosition(capitalProvince);
}


pair<int, int> HoI4WarCreator::getProvincePosition(int provinceNum)
{
	if (provincePositions.size() == 0)
	{
		establishProvincePositions();
	}

	auto itr = provincePositions.find(provinceNum);
	return itr->second;
}


void HoI4WarCreator::establishProvincePositions()
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


void HoI4WarCreator::processPositionLine(const string& line)
{
	vector<string> tokenizedLine = tokenizeLine(line);
	addProvincePosition(tokenizedLine);
}


void HoI4WarCreator::addProvincePosition(const vector<string>& tokenizedLine)
{
	int province = stoi(tokenizedLine[0]);
	int x = stoi(tokenizedLine[2]);
	int y = stoi(tokenizedLine[4]);

	provincePositions.insert(make_pair(province, make_pair(x, y)));
}


vector<string> HoI4WarCreator::tokenizeLine(const string& line)
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


double HoI4WarCreator::getDistanceBetweenPoints(pair<int, int> point1, pair<int, int> point2)
{
	int xDistance = abs(point2.first - point1.first);
	if (xDistance > 2625)
	{
		xDistance = 5250 - xDistance;
	}

	int yDistance = point2.second - point1.second;

	return sqrt(pow(xDistance, 2) + pow(yDistance, 2));
}


double HoI4WarCreator::GetFactionStrengthWithDistance(HoI4Country* HomeCountry, vector<HoI4Country*> Faction, double time)
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


HoI4Faction* HoI4WarCreator::findFaction(HoI4Country* CheckingCountry)
{
	for (auto faction : theWorld->getFactions())
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
	HoI4Faction* tempFaction = new HoI4Faction(CheckingCountry, myself);
	return tempFaction;
}


map<string, HoI4Country*> HoI4WarCreator::getNeighbors(const HoI4Country* checkingCountry)
{
	map<string, HoI4Country*> neighbors = getImmediateNeighbors(checkingCountry);
	if (neighbors.size() == 0)
	{
		neighbors = getNearbyCountries(checkingCountry);
	}

	return neighbors;
}


map<string, HoI4Country*> HoI4WarCreator::getImmediateNeighbors(const HoI4Country* checkingCountry)
{
	map<string, HoI4Country*> neighbors;

	for (auto province: checkingCountry->getProvinces())
	{
		for (int provinceNumber: provinceNeighborMapper::getNeighbors(province))
		{
			if (!provinceDefinitions::isLandProvince(province))
			{
				continue;
			}

			auto provinceToOwnerItr = provinceToOwnerMap.find(provinceNumber);
			if (provinceToOwnerItr == provinceToOwnerMap.end())
			{
				continue;
			}

			string ownerTag = provinceToOwnerItr->second;
			HoI4Country* ownerCountry = theWorld->getCountries().find(ownerTag)->second;
			if (ownerCountry != checkingCountry)
			{
				neighbors.insert(make_pair(ownerTag, ownerCountry));
			}
		}
	}

	return neighbors;
}


map<string, HoI4Country*> HoI4WarCreator::getNearbyCountries(const HoI4Country* checkingCountry)
{
	map<string, HoI4Country*> neighbors;

	for (auto countryItr: theWorld->getCountries())
	{
		HoI4Country* country = countryItr.second;
		if (country->getCapitalProv() != 0)
		{
			//IMPROVE
			//need to get further neighbors, as well as countries without capital in an area
			double distance = getDistanceBetweenCountries(checkingCountry, country);
			if (distance <= 500 && country->getProvinceCount() > 0)
			{
				neighbors.insert(countryItr);
			}
		}
	}

	return neighbors;
}


void HoI4WarCreator::determineProvinceOwners()
{
	for (auto state: theWorld->getStates())
	{
		for (auto province: state.second->getProvinces())
		{
			string owner = state.second->getOwner();
			provinceToOwnerMap.insert(make_pair(province, owner));
		}
	}
}


double HoI4WarCreator::GetFactionStrength(HoI4Faction* Faction, int years)
{
	double strength = 0;
	for (auto country : Faction->getMembers())
	{
		strength += country->getStrengthOverTime(years);
	}
	return strength;
}


vector<HoI4Faction*> HoI4WarCreator::fascistWarMaker(HoI4Country* Leader, ofstream& AILog)
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
	for (auto neigh: AllNeighbors)
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
		newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
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
				newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + nan[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + nan[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(theWorld->getEvents()->getCurrentNationFocusEventNum()) + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "			}";
				FocusTree->addFocus(newFocus);

				theWorld->getEvents()->createAnnexEvent(Leader, nan[i]);
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
		newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }";//give some claims or cores
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
				newFocus->bypass += "  has_war_with = " + nan[i]->getTag() + "\n";
				newFocus->aiWillDo = "	factor = 10\n";
				newFocus->aiWillDo += "	modifier = {\n";
				newFocus->aiWillDo += "		factor = 0\n";
				newFocus->aiWillDo += "		date < 1937.6.6\n";
				newFocus->aiWillDo += "	}";
				newFocus->completionReward += "			add_named_threat = { threat = 2 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			army_experience = 10\n";
				newFocus->completionReward += "			if = {\n";
				newFocus->completionReward += "				limit = {\n";
				newFocus->completionReward += "					country_exists = " + nan[i]->getTag() + "\n";
				newFocus->completionReward += "				}\n";
				newFocus->completionReward += "				" + nan[i]->getTag() + " = {\n";
				newFocus->completionReward += "					country_event = NFEvents." + to_string(theWorld->getEvents()->getCurrentNationFocusEventNum()) + "\n";
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
				newFocus->bypass += "		has_war_with = " + nan[i]->getTag() + "\n";
				newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
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
					for (int prov: provinceNeighborMapper::getNeighbors(leaderprov))
					{
						if (!provinceDefinitions::isLandProvince(prov))
						{
							continue;
						}

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

				theWorld->getEvents()->createSudatenEvent(Leader, nan[0], demandedstates);
			}
		}
		nan.clear();

	}
	//events for allies
	vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);
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
		newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
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
		newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(theWorld->getEvents()->getCurrentNationFocusEventNum()) + " }\n";
		newFocus->completionReward += "				add_opinion_modifier = { target = " + Leader->getTag() + " modifier = positive_50 }\n";
		newFocus->completionReward += "			}";
		FocusTree->addFocus(newFocus);

		if (Leader->getFaction() == nullptr)
		{
			vector<HoI4Country*> self;
			self.push_back(Leader);
			HoI4Faction* newFaction = new HoI4Faction(Leader, self);
			Leader->setFaction(newFaction);
		}

		theWorld->getEvents()->createFactionEvents(Leader, newAllies[i]);
	}

	vector<HoI4Faction*> FactionsAttackingMe;
	int maxGCAlliance = 0;
	if (WorldTargetMap.find(Leader) != WorldTargetMap.end())
	{
		for (HoI4Country* country: WorldTargetMap.find(Leader)->second)
		{
			HoI4Faction* attackingFaction = findFaction(country);
			if (find(FactionsAttackingMe.begin(), FactionsAttackingMe.end(), attackingFaction) == FactionsAttackingMe.end())
			{
				FactionsAttackingMe.push_back(attackingFaction);
			}
		}
		double FactionsAttackingMeStrength = 0;
		for (HoI4Faction* attackingFaction: FactionsAttackingMe)
		{
			FactionsAttackingMeStrength += GetFactionStrengthWithDistance(Leader, attackingFaction->getMembers(), 3);
		}
		AILog << "\t" << Leader->getSourceCountry()->getName("english") << " is under threat, there are " << FactionsAttackingMe.size() << " faction(s) attacking them, I have a strength of " << GetFactionStrength(findFaction(Leader), 3) << " and they have a strength of " << FactionsAttackingMeStrength << "\n";
		if (FactionsAttackingMeStrength > GetFactionStrength(findFaction(Leader), 3))
		{
			vector<HoI4Country*> GCAllies;

			for (HoI4Country* GC: theWorld->getGreatPowers())
			{
				int relations = Leader->getRelations(GC->getTag())->getRelations();
				if (relations > 0 && maxGCAlliance < 1)
				{
					AILog << "\t" << Leader->getSourceCountry()->getName("english") << " can attempt to ally " << GC->getSourceCountry()->getName("english") << "\n";
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
					newFocus->bypass += "				" + Leader->getTag() + " = { is_in_faction_with = " + GC->getTag() + "}\n";
					newFocus->bypass += "				has_war_with = " + GC->getTag() + "\n";
					newFocus->bypass += "				NOT = { country_exists = " + GC->getTag() + " }\n";
					newFocus->bypass += "			}\n";
					newFocus->completionReward += "			" + GC->getTag() + " = {\n";
					newFocus->completionReward += "				country_event = { hours = 6 id = NFEvents." + to_string(theWorld->getEvents()->getCurrentNationFocusEventNum()) + " }\n";
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
					theWorld->getEvents()->createFactionEvents(Leader, GC);
					maxGCAlliance++;
				}
			}
		}
	}

	//Declaring war with Great Country
	map<double, HoI4Country*> GCDistance;
	vector<HoI4Country*> GCDistanceSorted;
	//get great countries with a distance
	for (auto GC: theWorld->getGreatPowers())
	{
		double distance = getDistanceBetweenCountries(Leader, GC);
		GCDistance.insert(make_pair(distance, GC));
	}
	//put them into a vector so we know their order
	for (auto iterator = GCDistance.begin(); iterator != GCDistance.end(); ++iterator)
	{
		GCDistanceSorted.push_back(iterator->second);
	}

	vector<HoI4Country*> GCTargets;
	for (auto GC: GCDistanceSorted)
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
	for (auto GC: GCTargets)
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
				newFocus->bypass += "		has_war_with = " + GC->getTag() + "\n";
				newFocus->completionReward += "			add_named_threat = { threat = 5 name = " + newFocus->id + " }\n";
				newFocus->completionReward += "			declare_war_on = {\n";
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


vector<HoI4Faction*> HoI4WarCreator::communistWarCreator(HoI4Country* Leader, ofstream& AILog)
{
	vector<HoI4Faction*> CountriesAtWar;
	//communism still needs great country war events
	LOG(LogLevel::Info) << "Calculating AI for " + Leader->getSourceCountry()->getName("english");
	LOG(LogLevel::Info) << "Calculating Neighbors for " + Leader->getSourceCountry()->getName("english");
	map<string, HoI4Country*> AllNeighbors = getNeighbors(Leader);
	map<string, HoI4Country*> Neighbors;
	for (auto neigh: AllNeighbors)
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
			for (auto party : neigh.second->getIdeologySupport())
			{
				if ((party.first == "socialist") || (party.first == "communist") || (party.first == "anarcho_liberal"))
					com += party.second;
			}
			if (com > 25 && neigh.second->getRulingParty()->ideology != "communist" && HowToTakeLand(neigh.second, Leader, 2.5) == "coup")
			{
				//look for neighboring countries to spread communism too(Need 25 % or more Communism support), Prioritizing those with "Communism Allowed" Flags, prioritizing those who are weakest
				//	Method() Influence Ideology and Attempt Coup
				coups.push_back(neigh.second);
			}
			else if (neighFaction->getMembers().size() == 1 && neigh.second->getRulingParty()->ideology != "communist")
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

	vector<HoI4Country*> TargetsByTech;
	bool first = true;
	//FIXME 
	//Right now just uses everyone in forcedtakover, doesnt use nan, fn, ect...
	for (auto country : forcedtakeover)
	{
		if (first)
		{
			TargetsByTech.push_back(country);
			first = false;
		}
		else
		{
			//makes sure not a coup target
			if (find(coups.begin(), coups.end(), country) == coups.end())
			{
				if (TargetsByTech.front()->getTechnologyCount() < country->getTechnologyCount())
				{
					TargetsByTech.insert(TargetsByTech.begin(), country);
				}
				else
					TargetsByTech.push_back(country);
			}
		}
	}

	// Candidates for Get Allies foci
	vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);

	//Declaring war with Great Country
	map<double, HoI4Country*> GCDistance;
	vector<HoI4Country*> GCDistanceSorted;
	for (auto GC : theWorld->getGreatPowers())
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
	for (auto GC : GCDistanceSorted)
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
			//TODO
		}
	}
	int maxGCWars = 0;
	int start = 0;
	for (auto GC : GCTargets)
	{
		int relations = Leader->getRelations(GC->getTag())->getRelations();
		if (relations < 0)
		{
			GCTargets.push_back(GC);
			maxGCWars++;
		}
		if (maxGCWars >= 2) break;
	}

	HoI4FocusTree* FocusTree = genericFocusTree->makeCustomizedCopy(Leader);
	FocusTree->addCommunistCoupBranch(Leader, forcedtakeover);
	FocusTree->addCommunistWarBranch(Leader, TargetsByTech, theWorld->getEvents());
	FocusTree->addCommunistGPWarBranch(Leader, newAllies, GCTargets, theWorld->getEvents());
	Leader->addNationalFocus(FocusTree);

	return CountriesAtWar;
}


vector<HoI4Faction*> HoI4WarCreator::democracyWarCreator(HoI4Country* Leader)
{
	vector<HoI4Faction*> CountriesAtWar;
	map<int, HoI4Country*> CountriesToContain;
	vector<HoI4Country*> vCountriesToContain;
	set<string> Allies = Leader->getAllies();
	int v1 = rand() % 100;
	v1 = v1 / 100;
	HoI4FocusTree* FocusTree = genericFocusTree->makeCustomizedCopy(Leader);
	for (auto GC: theWorld->getGreatPowers())
	{
		double relation = Leader->getRelations(GC->getTag())->getRelations();
		if (relation < 100 && GC->getGovernmentIdeology() != "democratic" && std::find(Allies.begin(), Allies.end(), GC->getTag()) == Allies.end())
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
		FocusTree->addDemocracyNationalFocuses(Leader, vCountriesToContain);
	}

	Leader->addNationalFocus(FocusTree);

	return CountriesAtWar;
}


vector<HoI4Faction*> HoI4WarCreator::absolutistWarCreator(HoI4Country* country)
{
	HoI4FocusTree* focusTree = genericFocusTree->makeCustomizedCopy(country);

	LOG(LogLevel::Info) << "Doing neighbor calcs for " + country->getSourceCountry()->getName("english");

	vector<HoI4Country*> weakNeighbors = findWeakNeighbors(country);
	vector<HoI4Country*> weakColonies = findWeakColonies(country);
	focusTree->addAbsolutistEmpireNationalFocuses(country, weakColonies, weakNeighbors);

	vector<HoI4Country*> greatPowerTargets = getGreatPowerTargets(country);
	vector<HoI4Faction*> CountriesAtWar = addGreatPowerWars(country, focusTree, greatPowerTargets);
	addTradeEvents(country, greatPowerTargets);

	country->addNationalFocus(focusTree);

	return CountriesAtWar;
}


vector<HoI4Faction*> HoI4WarCreator::neighborWarCreator(HoI4Country * country, ofstream & AILog)
{
	// add small wars against neigbors for non-great powers
	vector<HoI4Faction*> countriesAtWar;
	vector<HoI4Country*> weakNeighbors = findWeakNeighbors(country);
	int numWarsWithNeighbors = 0;
	HoI4FocusTree* focusTree = genericFocusTree->makeCustomizedCopy(country);

	AILog << "Look for neighbors to attack for " + country->getSourceCountry()->getName("english") << "\n";

	for (auto target : weakNeighbors)
	{
		if (numWarsWithNeighbors >= 2)
		{
			break;
		}

		int relations = 0;
		HoI4Relations* relationsObj = country->getRelations(target->getTag());
		if (relationsObj)
		{
			relations = relationsObj->getRelations();
		}

		if (relations >= 0)
		{
			continue;
		}

		set<string> Allies = country->getAllies();
		date startDate = date("1937.01.01");
		startDate.delayedByMonths(relations / -4);
		if (Allies.find(target->getTag()) == Allies.end())
		{
			countriesAtWar.push_back(findFaction(country));
			AILog << "Creating focus to attack " + target->getSourceCountry()->getName("english") << "\n";

			HoI4Focus* newFocus = new HoI4Focus;
			newFocus->id = "War" + target->getTag() + country->getTag();
			newFocus->icon = "GFX_goal_generic_major_war";
			newFocus->text = "War with " + target->getSourceCountry()->getName("english");//change to faction name later
			newFocus->available = "			has_war = no\n";
			newFocus->available += "			date > " + startDate.toString();
			newFocus->xPos = 24;
			newFocus->yPos = 0;
			newFocus->cost = 10;
			newFocus->bypass += "          has_war_with = " + target->getTag() + "\n";
			newFocus->aiWillDo = "			factor = " + to_string(10 - numWarsWithNeighbors * 5) + "\n";
			newFocus->aiWillDo += "			modifier = {\n";
			newFocus->aiWillDo += "				factor = 0\n";
			newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 0.8 }\n";
			newFocus->aiWillDo += "			}";
			if (weakNeighbors.size() > 2) //make ai have this as a 0 modifier if they are at war
			{
				newFocus->aiWillDo += "\n";
				newFocus->aiWillDo += "			modifier = {\n";
				newFocus->aiWillDo += "				factor = 0\n";
				newFocus->aiWillDo += "				OR = {\n";
				for (auto target2 : weakNeighbors)
				{
					if (target != target2)
					{
						newFocus->aiWillDo += "					has_war_with = " + target2->getTag() + "\n";
					}
				}
				newFocus->aiWillDo += "				}\n";
				newFocus->aiWillDo += "			}";
			}
			newFocus->completionReward += "			add_named_threat = { threat = 3 name = " + newFocus->id + " }\n";
			newFocus->completionReward += "			create_wargoal = {\n";
			newFocus->completionReward += "				type = annex_everything\n";
			newFocus->completionReward += "				target = " + target->getTag() + "\n";
			newFocus->completionReward += "			}";
			focusTree->addFocus(newFocus);

			numWarsWithNeighbors++;
		}
	}

	if (numWarsWithNeighbors > 0)
	{
		country->addNationalFocus(focusTree);
	}

	return countriesAtWar;
}


vector<HoI4Faction*> HoI4WarCreator::radicalWarCreator(HoI4Country* country)
{
	return absolutistWarCreator(country);
}

vector<HoI4Country*> HoI4WarCreator::findWeakNeighbors(const HoI4Country* country)
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
			(enemystrength < (mystrength * 0.5)) &&
			(findFaction(neighbor.second)->getMembers().size() == 1)
			)
		{
			weakNeighbors.push_back(neighbor.second);
		}
	}

	return weakNeighbors;
}


map<string, HoI4Country*> HoI4WarCreator::findCloseNeighbors(const HoI4Country* country)
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


vector<HoI4Country*> HoI4WarCreator::findWeakColonies(const HoI4Country* country)
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
			(enemystrength < mystrength * 0.5) &&
			(findFaction(neighbor.second)->getMembers().size() == 1)
			)
		{
			weakColonies.push_back(neighbor.second);
		}
	}

	return weakColonies;
}


map<string, HoI4Country*> HoI4WarCreator::findFarNeighbors(const HoI4Country* country)
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
		for (auto otherCountry: theWorld->getCountries())
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


vector<HoI4Country*> HoI4WarCreator::getGreatPowerTargets(HoI4Country* country)
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


map<double, HoI4Country*> HoI4WarCreator::getGPsByDistance(const HoI4Country* country)
{
	map<double, HoI4Country*> distanceToGPMap;
	for (auto greatPower: theWorld->getGreatPowers())
	{
		double distance = getDistanceBetweenCountries(country, greatPower);
		if (distance < 1200)
		{
			distanceToGPMap.insert(make_pair(distance, greatPower));
		}
	}

	return distanceToGPMap;
}


vector<HoI4Faction*> HoI4WarCreator::addGreatPowerWars(HoI4Country* country, HoI4FocusTree* FocusTree, const vector<HoI4Country*>& greatPowerTargets)
{
	vector<HoI4Faction*> countriesAtWar;

	int numWarsWithGreatPowers = 0;
	for (auto target: greatPowerTargets)
	{
		if (numWarsWithGreatPowers >= 2)
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
			newFocus->available = "			has_war = no\n";
			newFocus->available += "			date > 1939.1.1";
			newFocus->xPos     = 31 + numWarsWithGreatPowers * 2;
			newFocus->yPos     = 5;
			newFocus->cost     = 10;
			newFocus->aiWillDo = "			factor = " + to_string(10 - numWarsWithGreatPowers * 5) + "\n";
			newFocus->aiWillDo += "			modifier = {\n";
			newFocus->aiWillDo += "				factor = 0\n";
			newFocus->aiWillDo += "				strength_ratio = { tag = " + target->getTag() + " ratio < 0.8 }\n";
			newFocus->aiWillDo += "			}";
			if (greatPowerTargets.size() > 2) //make ai have this as a 0 modifier if they are at war
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
			newFocus->bypass += " 		has_war_with = " + target->getTag() + "\n";
			newFocus->completionReward += "			add_named_threat = { threat = 5 name = " + newFocus->id + " }\n";
			newFocus->completionReward += "			declare_war_on = {\n";
			newFocus->completionReward += "				type = annex_everything\n";
			newFocus->completionReward += "				target = " + target->getTag() + "\n";
			newFocus->completionReward += "			}";
			FocusTree->addFocus(newFocus);

			numWarsWithGreatPowers++;
		}
	}

	return countriesAtWar;
}


void HoI4WarCreator::addTradeEvents(const HoI4Country* country, const vector<HoI4Country*>& greatPowerTargets)
{
	for (auto greatPowerTarget: greatPowerTargets)
	{
		int relations = country->getRelations(greatPowerTarget->getTag())->getRelations();
		if (relations >= 0)
		{
			continue;
		}

		theWorld->getEvents()->createTradeEvent(country, greatPowerTarget);
	}
}