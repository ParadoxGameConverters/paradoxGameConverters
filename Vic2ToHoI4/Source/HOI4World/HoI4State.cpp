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



#include "HoI4State.h"
#include <fstream>
#include <random>
#include "../Configuration.h"
#include "../Mappers/CoastalHoI4Provinces.h"
#include "../Mappers/ProvinceMapper.h"
#include "../Mappers/StateCategoryMapper.h"
#include "../Mappers/StateMapper.h"
#include "../V2World/V2Province.h"
#include "../V2World/Vic2State.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



HoI4State::HoI4State(const Vic2State* _sourceState, int _ID, const string& _ownerTag):
	sourceState(_sourceState),
	ID(_ID),
	provinces(),
	ownerTag(_ownerTag),
	cores(),
	capitalState(false),
	impassable(false),
	manpower(0),
	civFactories(0),
	milFactories(0),
	dockyards(0),
	category("pastoral"),
	infrastructure(0),
	navalBases(),
	airbaseLevel(0),
	resources(),
	victoryPointPosition(0),
	victoryPointValue(0),
	debugVictoryPoints(),
	secondaryDebugVictoryPoints()
{}


void HoI4State::output(const string& _filename) const
{
	string filename("output/" + Configuration::getOutputName() + "/history/states/" + _filename);
	ofstream out(filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open \"output/" + Configuration::getOutputName() + "/history/states/" + _filename;
		exit(-1);
	}

	out << "state={" << "\n";
	out << "\tid=" << ID << "\n";
	out << "\tname= \"STATE_" << ID << "\"\n";
	out << "\tmanpower = " << manpower << "\n";
	out << "\n";
	if (resources.size() > 0)
	{
		out << "\tresources={\n";
		for (auto resource: resources)
		{
			out << "\t\t" << resource.first << " = " << resource.second << "\n";
		}
		out << "\t}\n";
	}
	out << "\tstate_category = "<< category << "\n";
	if (impassable)
	{
		out << "\timpassable = yes\n";
	}
	out << "\n";
	out << "\thistory={\n";
	out << "\t\towner = " << ownerTag << "\n";
	if ((victoryPointValue > 0) && (victoryPointPosition != 0))
	{
		if (Configuration::getDebug())
		{
			for (auto VP: debugVictoryPoints)
			{
				out << "\t\tvictory_points = { " << VP << " 10 }\n";
			}
			for (auto VP: secondaryDebugVictoryPoints)
			{
				out << "\t\tvictory_points = { " << VP << " 1 }\n";
			}
		}
		else
		{
			out << "\t\tvictory_points = { " << victoryPointPosition << " " << victoryPointValue << " }\n";
		}
	}
	out << "\t\tbuildings = {\n";
	out << "\t\t\tinfrastructure = "<< infrastructure << "\n";
	out << "\t\t\tindustrial_complex = " << civFactories << "\n";
	out << "\t\t\tarms_factory = " << milFactories << "\n";
	if (dockyards > 0)
	{
		out << "\t\t\tdockyard = " << dockyards << "\n";
	}
		
	for (auto navalBase: navalBases)
	{
		out << "\t\t\t" << navalBase.first << " = {\n";
		out << "\t\t\t\tnaval_base = " << navalBase.second << "\n";
		out << "\t\t\t}\n";
	}
	out << "\t\t\tair_base = "<< airbaseLevel << "\n";
	out << "\t\t}\n";
	for (auto core: cores)
	{
		out << "\t\tadd_core_of = " << core << "\n";
	}
	out << "\t}\n";
	out << "\n";
	out << "\tprovinces={\n";
	out << "\t\t";
	for (auto provnum : provinces)
	{
		out << provnum << " ";
	}
	out << "\n";
	out << "\t}\n";
	out << "}\n";

	out.close();
}


void HoI4State::convertNavalBases()
{
	for (auto sourceProvince: sourceState->getProvinces())
	{
		int navalBaseLevel = determineNavalBaseLevel(sourceProvince);
		if (navalBaseLevel == 0)
		{
			continue;
		}

		auto navalBaseLocation = determineNavalBaseLocation(sourceProvince);
		if (navalBaseLocation)
		{
			addNavalBase(navalBaseLevel, *navalBaseLocation);
		}
	}
}


int HoI4State::determineNavalBaseLevel(const V2Province* sourceProvince)
{
	int navalBaseLevel = sourceProvince->getNavalBaseLevel() * 2;
	if (navalBaseLevel > 10)
	{
		navalBaseLevel = 10;
	}

	return navalBaseLevel;
}


optional<int> HoI4State::determineNavalBaseLocation(const V2Province* sourceProvince)
{
	auto provinceMapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(sourceProvince->getNumber());
	if (provinceMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
	{
		for (auto HoI4ProvNum: provinceMapping->second)
		{
			if (coastalHoI4ProvincesMapper::isProvinceCoastal(HoI4ProvNum))
			{
				return HoI4ProvNum;
			}
		}
	}

	return {};
}


void HoI4State::addNavalBase(int level, int location)
{
	if ((level > 0) && (provinces.find(location) != provinces.end()))
	{
		navalBases.push_back(make_pair(location, level));
	}
}


void HoI4State::addCores(const vector<string>& newCores)
{
	for (auto newCore: newCores)
	{
		cores.insert(newCore);
	}
}


bool HoI4State::assignVPFromVic2Province(int Vic2ProvinceNumber)
{
	auto provMapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(Vic2ProvinceNumber);
	if (provMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
	{
		for (auto province: provMapping->second)
		{
			if (isProvinceInState(province))
			{
				assignVP(province);
				return true;
			}
		}
	}

	return false;
}


void HoI4State::assignVP(int location)
{
	victoryPointPosition = location;

	victoryPointValue = 1;
	if (cores.count(ownerTag) != 0)
	{
		victoryPointValue += 2;
	}
}


optional<int> HoI4State::getMainNavalLocation() const
{
	optional<int> mainLocation;
	int mainSize = 0;
	for (auto navalBase: navalBases)
	{
		if (navalBase.second > mainSize)
		{
			mainLocation = navalBase.first;
			mainSize = navalBase.second;
		}
	}

	return mainLocation;
}


void HoI4State::tryToCreateVP()
{
	bool VPCreated = false;

	auto vic2CapitalProvince = stateMapper::getCapitalProvince(sourceState->getStateID());
	if (vic2CapitalProvince)
	{
		VPCreated = assignVPFromVic2Province(*vic2CapitalProvince);
	}

	if (!VPCreated)
	{
		if (Configuration::getDebug() && !sourceState->isPartialState() && !impassable)
		{
			LOG(LogLevel::Warning) << "Could not initially create VP for state " << ID << ", but state is not split (or was the passable part of a partially passable state).";
		}
		for (auto province: sourceState->getProvinces())
		{
			if (
				 (province->getPopulation("aristocrats") > 0) ||
				 (province->getPopulation("bureaucrats") > 0) ||
				 (province->getPopulation("capitalists") > 0)
				)
			{
				VPCreated = assignVPFromVic2Province(province->getNumber());
				if (VPCreated)
				{
					break;
				}
			}
		}
	}

	if (!VPCreated)
	{
		for (auto province: sourceState->getProvinces())
		{
			VPCreated = assignVPFromVic2Province(province->getNumber());
			if (VPCreated)
			{
				break;
			}
		}
	}

	if (!VPCreated)
	{
		LOG(LogLevel::Warning) << "Could not create VP for state " << ID;
	}

	addDebugVPs();
}


void HoI4State::addDebugVPs()
{
	for (auto sourceProvinceNum: sourceState->getProvinceNums())
	{
		auto provMapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(sourceProvinceNum);
		if (
				(provMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end()) &&
				(isProvinceInState(provMapping->second[0]))
			)
		{
			debugVictoryPoints.insert(provMapping->second[0]);
		}
		for (unsigned int i = 1; i < provMapping->second.size(); i++)
		{
			secondaryDebugVictoryPoints.insert(provMapping->second[i]);
		}
	}
}


void HoI4State::addManpower()
{
	for (auto sourceProvince: sourceState->getProvinces())
	{
		bool provinceIsInState = false;
		auto mapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(sourceProvince->getNumber());
		if (mapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
		{
			for (auto HoI4Province: mapping->second)
			{
				if (isProvinceInState(HoI4Province))
				{
					provinceIsInState = true;
					break;
				}
			}
		}

		if (provinceIsInState)
		{
			manpower += static_cast<int>(sourceProvince->getTotalPopulation() * 4 * Configuration::getManpowerFactor());
		}
	}
}


void HoI4State::convertIndustry(double workerFactoryRatio)
{
	int factories = determineFactoryNumbers(workerFactoryRatio);

	determineCategory(factories);
	setInfrastructure(factories);
	setIndustry(factories);
	addVictoryPointValue(factories / 2);
}


int HoI4State::determineFactoryNumbers(double workerFactoryRatio)
{
	double rawFactories = sourceState->getEmployedWorkers() * workerFactoryRatio;
	rawFactories = round(rawFactories);
	return constrainFactoryNumbers(rawFactories);
}


int HoI4State::constrainFactoryNumbers(double rawFactories)
{
	int factories = static_cast<int>(rawFactories);

	int upperLimit = 12;
	if (capitalState)
	{
		upperLimit = 11;
	}

	if (factories < 0)
	{
		factories = 0;
	}
	else if (factories > upperLimit)
	{
		factories = upperLimit;
	}

	return factories;
}


void HoI4State::determineCategory(int factories)
{
	if (capitalState)
	{
		factories++;
	}

	int population = sourceState->getPopulation();
	int stateSlots = population / 120000;
	if (factories >= stateSlots)
	{
		stateSlots = factories + 2;
	}

	for (auto possibleCategory: stateCategoryMapper::getStateCategories())
	{
		if (stateSlots >= possibleCategory.first)
		{
			category = possibleCategory.second;
		}
	}
}


void HoI4State::setInfrastructure(int factories)
{
	infrastructure = 3;
	infrastructure += sourceState->getAverageRailLevel() / 2;

	if (factories > 4)
	{
		infrastructure++;
	}
	if (factories > 6)
	{
		infrastructure++;
	}
	if (factories > 10)
	{
		infrastructure++;
	}
}


static mt19937 randomnessEngine;
static uniform_int_distribution<> numberDistributor(0, 99);
void HoI4State::setIndustry(int factories)
{
	if (amICoastal())
	{
		// distribute military factories, civilian factories, and dockyards using unseeded random
		//		20% chance of dockyard
		//		57% chance of civilian factory
		//		23% chance of military factory
		for (int i = 0; i < factories; i++)
		{
			double randomNum = numberDistributor(randomnessEngine);
			if (randomNum > 76)
			{
				milFactories++;
			}
			else if (randomNum > 19)
			{
				civFactories++;
			}
			else
			{
				dockyards++;
			}
		}
	}
	else
	{
		// distribute military factories, civilian factories, and dockyards using unseeded random
		//		 0% chance of dockyard
		//		71% chance of civilian factory
		//		29% chance of military factory
		for (int i = 0; i < factories; i++)
		{
			double randomNum = numberDistributor(randomnessEngine);
			if (randomNum > 70)
			{
				milFactories++;
			}
			else
			{
				civFactories++;
			}
		}
	}
}


bool HoI4State::amICoastal()
{
	auto coastalProvinces = coastalHoI4ProvincesMapper::getCoastalProvinces();
	for (auto province: provinces)
	{
		auto itr = coastalProvinces.find(province);
		if (itr != coastalProvinces.end())
		{
			return true;
		}
	}

	return false;
}


bool HoI4State::isProvinceInState(int provinceNum)
{
	return (provinces.count(provinceNum) > 0);
}