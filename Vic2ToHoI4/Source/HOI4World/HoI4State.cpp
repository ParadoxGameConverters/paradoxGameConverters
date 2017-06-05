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



HoI4State::HoI4State(const Vic2State* _sourceState, int _ID, string _ownerTag)
{
	sourceState = _sourceState;

	ID = _ID;
	provinces.clear();
	ownerTag = _ownerTag;
	capitalState = false;

	manpower = 0;

	civFactories = 0;
	milFactories = 0;
	dockyards = 0;
	category = "pastoral";
	infrastructure = 0;

	navalBases.clear();

	airbaseLevel = 0;

	resources.clear();

	victoryPointPosition = 0;
	victoryPointValue = 0;
}


void HoI4State::output(string _filename)
{
	string filename("output/" + Configuration::getOutputName() + "/history/states/" + _filename);
	ofstream out(filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open \"output/" + Configuration::getOutputName() + "/history/states/" + _filename;
		exit(-1);
	}

	out << "state={" << endl;
	out << "\tid=" << ID << endl;
	out << "\tname= \"STATE_" << ID << "\"" << endl;
	out << "\tmanpower = " << manpower << endl;
	out << endl;
	if (resources.size() > 0)
	{
		out << "\tresources={" << endl;
		for (auto resource: resources)
		{
			out << "\t\t" << resource.first << " = " << resource.second << endl;
		}
		out << "\t}" << endl;
	}
	out << "\tstate_category = "<< category << endl;
	out << "" << endl;
	out << "\thistory={" << endl;
	out << "\t\towner = " << ownerTag << endl;
	if ((victoryPointValue > 0) && (victoryPointPosition != 0))
	{
		out << "\t\tvictory_points = {" << endl;
		out << "\t\t\t" << victoryPointPosition << " " << victoryPointValue << endl;
		out << "\t\t}" << endl;
	}
	out << "\t\tbuildings = {" << endl;
	out << "\t\t\tinfrastructure = "<< infrastructure << endl;
	out << "\t\t\tindustrial_complex = " << civFactories << endl;
	out << "\t\t\tarms_factory = " << milFactories << endl;
	if (dockyards > 0)
	{
		out << "\t\t\tdockyard = " << dockyards << endl;
	}
		
	for (auto navalBase: navalBases)
	{
		out << "\t\t\t" << navalBase.first << " = {" << endl;
		out << "\t\t\t\tnaval_base = " << navalBase.second << endl;
		out << "\t\t\t}" << endl;
	}
	out << "\t\t\tair_base = "<< airbaseLevel << endl;
	out << "\t\t}" << endl;
	for (auto core: cores)
	{
		out << "\t\tadd_core_of = " << core << endl;
	}
	out << "\t}" << endl;
	out << endl;
	out << "\tprovinces={" << endl;
	out << "\t\t";
	for (auto provnum : provinces)
	{
		out << provnum << " ";
	}
	out << endl;
	out << "\t}" << endl;
	out << "}" << endl;

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

		int navalBaseLocation = determineNavalBaseLocation(sourceProvince);
		if (navalBaseLocation != -1)
		{
			addNavalBase(navalBaseLevel, navalBaseLocation);
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


int HoI4State::determineNavalBaseLocation(const V2Province* sourceProvince)
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

	return -1;
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
	if (
		(provMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end()) &&
		(isProvinceInState(provMapping->second[0]))
		)
	{
		assignVP(provMapping->second[0]);
		return true;
	}
	else
	{
		return false;
	}
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


int HoI4State::getMainNavalLocation() const
{
	int mainLocation = 0;
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
	auto vic2CapitalProvince = stateMapper::getCapitalProvince(sourceState->getStateID());
	bool VPCreated = assignVPFromVic2Province(vic2CapitalProvince);

	if (!VPCreated)
	{
		if (!sourceState->isPartialState())
		{
			LOG(LogLevel::Warning) << "Could not initially create VP for state " << ID << ", but state is not split";
		}
		for (auto province: sourceState->getProvinces())
		{
			if (province->getPopulation("aristocrats") > 0)
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
		LOG(LogLevel::Warning) << "Could not create VP for state";
	}
}


void HoI4State::addManpower()
{
	for (auto sourceProvince: sourceState->getProvinces())
	{
		manpower += static_cast<int>(sourceProvince->getTotalPopulation() * 4 * Configuration::getManpowerFactor());
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
	map<int, int> coastalProvinces = coastalHoI4ProvincesMapper::getCoastalProvinces();
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
	return (provinces.find(provinceNum) != provinces.end());
}