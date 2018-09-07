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



#include "HoI4State.h"
#include "CoastalProvinces.h"
#include "StateCategories.h"
#include "../Configuration.h"
#include "../Mappers/ProvinceMapper.h"
#include "../V2World/StateDefinitions.h"
#include "../V2World/Province.h"
#include "../V2World/State.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>
#include <random>

// Currently not populated anywhere, so no forts will be created from them; they
// exist for future support of forts.
std::map<int, int> HoI4::State::landFortLevels;
std::map<int, int> HoI4::State::coastFortLevels;

class dockyardProvince: commonItems::parser
{
	public:
		explicit dockyardProvince(std::istream& theStream);

		int getDockyards() const { return dockyards; }

	private:
		int dockyards = 0;
};


dockyardProvince::dockyardProvince(std::istream& theStream)
{
	registerKeyword(std::regex("naval_base"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleInt baseInt(theStream);
		dockyards = baseInt.getInt();
	});
	registerKeyword(std::regex("[a-zA-Z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}


class stateBuildings: commonItems::parser
{
	public:
		explicit stateBuildings(std::istream& theStream);

		int getCivFactories() const { return civFactories; }
		int getMilFactories() const { return milFactories; }
		int getDockyards() const { return dockyards; }

	private:
		int civFactories = 0;
		int milFactories = 0;
		int dockyards = 0;
};


stateBuildings::stateBuildings(std::istream& theStream)
{
	registerKeyword(std::regex("industrial_complex"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleInt factoryInt(theStream);
		civFactories = factoryInt.getInt();
	});
	registerKeyword(std::regex("arms_factory"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleInt factoryInt(theStream);
		milFactories = factoryInt.getInt();
	});
	registerKeyword(std::regex("\\d+"), [this](const std::string& unused, std::istream& theStream)
	{
		dockyardProvince province(theStream);
		dockyards += province.getDockyards();
	});
	registerKeyword(std::regex("[a-zA-Z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}


class stateHistory: commonItems::parser
{
	public:
		explicit stateHistory(std::istream& theStream);

		int getCivFactories() const { return civFactories; }
		int getMilFactories() const { return milFactories; }
		int getDockyards() const { return dockyards; }
		std::string getOwner() const { return owner; }

	private:
		int civFactories = 0;
		int milFactories = 0;
		int dockyards = 0;
		std::string owner;
};


stateHistory::stateHistory(std::istream& theStream)
{
	registerKeyword(std::regex("owner"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString ownerString(theStream);
		owner = ownerString.getString();
	});
	registerKeyword(std::regex("buildings"), [this](const std::string& unused, std::istream& theStream)
	{
		stateBuildings theBuildings(theStream);
		civFactories = theBuildings.getCivFactories();
		milFactories = theBuildings.getMilFactories();
		dockyards = theBuildings.getDockyards();
	});
	registerKeyword(std::regex("[a-zA-Z0-9\\._]+"), commonItems::ignoreItem);

	parseStream(theStream);
}


HoI4::State::State(std::istream& theStream)
{
	registerKeyword(std::regex("impassable"), [this](const std::string& unused, std::istream& theStream){
		impassable = true;
		commonItems::ignoreItem(unused, theStream);
	});
	registerKeyword(std::regex("provinces"), [this](const std::string& unused, std::istream& theStream){
		commonItems::intList provinceNums(theStream);
		for (auto province: provinceNums.getInts())
		{
			provinces.insert(province);
		}
	});
	registerKeyword(std::regex("history"), [this](const std::string& unused, std::istream& theStream){
		stateHistory theHistory(theStream);
		civFactories = theHistory.getCivFactories();
		milFactories = theHistory.getMilFactories();
		dockyards = theHistory.getDockyards();
		ownerTag = theHistory.getOwner();
	});
	registerKeyword(std::regex("[a-zA-Z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);

	sourceState = nullptr;
}


HoI4::State::State(const Vic2::State* _sourceState, int _ID, const std::string& _ownerTag):
	sourceState(_sourceState),
	ID(_ID),
	ownerTag(_ownerTag)
{}


void HoI4::State::output(const std::string& _filename) const
{
	std::string filename("output/" + theConfiguration.getOutputName() + "/history/states/" + _filename);
	std::ofstream out(filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open \"output/" + theConfiguration.getOutputName() + "/history/states/" + _filename;
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
	if (ownerTag != "")
	{
		out << "\t\towner = " << ownerTag << "\n";
	}
	if ((victoryPointValue > 0) && (victoryPointPosition != 0))
	{
		if (theConfiguration.getDebug())
		{
			out << "\t\tvictory_points = { " << victoryPointPosition << " " << (victoryPointValue + 10) << " }\n";
			for (auto VP: debugVictoryPoints)
			{
				if (VP == victoryPointPosition)
				{
					continue;
				}
				out << "\t\tvictory_points = { " << VP << " 5 }\n";
			}
			for (auto VP: secondaryDebugVictoryPoints)
			{
				if (VP == victoryPointPosition)
				{
					continue;
				}
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
		
	for (auto provnum : provinces)
	{
                int naval = 0;
                if (navalBases.find(provnum) != navalBases.end())
                {
                        naval = navalBases.at(provnum);
                }
                int bunker = landFortLevels[provnum];
                int coastFort = coastFortLevels[provnum];
                if (naval == 0 && bunker == 0 && coastFort == 0)
                {
                        continue;
                }
                out << "\t\t\t" << provnum << " = {\n";
                if (naval != 0)
                {
                        out << "\t\t\t\tnaval_base = " << naval << "\n";
                }
                if (bunker != 0)
                {
                        out << "\t\t\t\tbunker = " << bunker << "\n";
                }
                if (coastFort != 0)
                {
                        out << "\t\t\t\tcoastal_bunker = " << coastFort << "\n";
                }
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


void HoI4::State::convertNavalBases(const coastalProvinces& theCoastalProvinces)
{
	for (auto sourceProvince: sourceState->getProvinces())
	{
		int navalBaseLevel = determineNavalBaseLevel(sourceProvince);
		if (navalBaseLevel == 0)
		{
			continue;
		}

		auto navalBaseLocation = determineNavalBaseLocation(sourceProvince, theCoastalProvinces);
		if (navalBaseLocation)
		{
			addNavalBase(navalBaseLevel, *navalBaseLocation);
		}
	}
}


int HoI4::State::determineNavalBaseLevel(const Vic2::Province* sourceProvince)
{
	int navalBaseLevel = sourceProvince->getNavalBaseLevel() * 2;
	if (navalBaseLevel > 10)
	{
		navalBaseLevel = 10;
	}

	return navalBaseLevel;
}


std::optional<int> HoI4::State::determineNavalBaseLocation(const Vic2::Province* sourceProvince, const coastalProvinces& theCoastalProvinces)
{
	if (auto mapping = theProvinceMapper.getVic2ToHoI4ProvinceMapping(sourceProvince->getNumber()))
	{
		for (auto HoI4ProvNum: *mapping)
		{
			if (theCoastalProvinces.isProvinceCoastal(HoI4ProvNum))
			{
				return HoI4ProvNum;
			}
		}
	}

	return {};
}


void HoI4::State::addNavalBase(int level, int location)
{
	if ((level > 0) && (provinces.find(location) != provinces.end()))
	{
                navalBases[location] = level;
        }
}


void HoI4::State::addCores(const std::vector<std::string>& newCores)
{
	for (auto newCore: newCores)
	{
		cores.insert(newCore);
	}
}


bool HoI4::State::assignVPFromVic2Province(int Vic2ProvinceNumber)
{
	if (auto mapping = theProvinceMapper.getVic2ToHoI4ProvinceMapping(Vic2ProvinceNumber))
	{
		for (auto province: *mapping)
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


void HoI4::State::assignVP(int location)
{
	victoryPointPosition = location;

	victoryPointValue = 1;
	if (cores.count(ownerTag) != 0)
	{
		victoryPointValue += 2;
	}
}


std::optional<int> HoI4::State::getMainNavalLocation() const
{
	std::optional<int> mainLocation;
	int mainSize = 0;
	for (const auto& navalBase: navalBases)
	{
		if (navalBase.second > mainSize)
		{
			mainLocation = navalBase.first;
			mainSize = navalBase.second;
		}
	}

	return mainLocation;
}


void HoI4::State::tryToCreateVP()
{
	bool VPCreated = false;

	auto vic2CapitalProvince = sourceState->getCapitalProvince();
	if (vic2CapitalProvince)
	{
		VPCreated = assignVPFromVic2Province(*vic2CapitalProvince);
	}

	if (!VPCreated)
	{
		if (theConfiguration.getDebug() && !sourceState->isPartialState() && !impassable && !hadImpassablePart)
		{
			LOG(LogLevel::Warning) << "Could not initially create VP for state " << ID << ", but state is not split.";
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


void HoI4::State::addDebugVPs()
{
	for (auto sourceProvinceNum: sourceState->getProvinceNums())
	{
		auto mapping = theProvinceMapper.getVic2ToHoI4ProvinceMapping(sourceProvinceNum);
		if (mapping && (isProvinceInState((*mapping)[0])))
		{
			debugVictoryPoints.insert((*mapping)[0]);
		}
		for (auto province: *mapping)
		{
			secondaryDebugVictoryPoints.insert(province);
		}
	}
}


void HoI4::State::addManpower()
{
	for (auto sourceProvince: sourceState->getProvinces())
	{
		bool provinceIsInState = false;
		if (auto mapping = theProvinceMapper.getVic2ToHoI4ProvinceMapping(sourceProvince->getNumber()))
		{
			for (auto HoI4Province: *mapping)
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
			manpower += static_cast<int>(sourceProvince->getTotalPopulation() * 4 * theConfiguration.getManpowerFactor());
		}
	}
}


void HoI4::State::convertIndustry(double workerFactoryRatio, const HoI4::stateCategories& theStateCategories, const coastalProvinces& theCoastalProvinces)
{
	int factories = determineFactoryNumbers(workerFactoryRatio);

	determineCategory(factories, theStateCategories);
	setInfrastructure(factories);
	setIndustry(factories, theCoastalProvinces);
	addVictoryPointValue(factories / 2);
}


int HoI4::State::determineFactoryNumbers(double workerFactoryRatio)
{
	double rawFactories = sourceState->getEmployedWorkers() * workerFactoryRatio;
	rawFactories = round(rawFactories);
	return constrainFactoryNumbers(rawFactories);
}


int HoI4::State::constrainFactoryNumbers(double rawFactories)
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


void HoI4::State::determineCategory(int factories, const HoI4::stateCategories& theStateCategories)
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

	category = theStateCategories.getBestCategory(stateSlots);
}


void HoI4::State::setInfrastructure(int factories)
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


static std::mt19937 randomnessEngine;
static std::uniform_int_distribution<> numberDistributor(0, 99);
void HoI4::State::setIndustry(int factories, const coastalProvinces& theCoastalProvinces)
{
	if (amICoastal(theCoastalProvinces))
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


bool HoI4::State::amICoastal(const coastalProvinces& theCoastalProvinces)
{
	auto coastalProvinces = theCoastalProvinces.getCoastalProvinces();
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


bool HoI4::State::isProvinceInState(int provinceNum)
{
	return (provinces.count(provinceNum) > 0);
}
