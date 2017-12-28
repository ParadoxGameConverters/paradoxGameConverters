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



#include "Vic2State.h"
#include "V2Country.h"
#include "V2Province.h"
#include "V2World.h"
#include "../Mappers/StateMapper.h"
#include "Log.h"



Vic2State::Vic2State(shared_ptr<Object> stateObj, const string& ownerTag):
	owner(ownerTag),
	stateID(""),
	partialState(false),
	provinceNums(),
	provinces(),
	factoryLevel(0),
	employedWorkers(0)
{
	addProvinceNums(stateObj);
	setID();
	setFactoryLevel(stateObj);
}


void Vic2State::addProvinceNums(shared_ptr<Object> stateObj)
{
	for (auto provinceItr: stateObj->safeGetTokens("provinces"))
	{
		provinceNums.insert(stoi(provinceItr));
	}
}


void Vic2State::setID()
{
	auto stateIDMapping = stateMapper::getStateIdMapping().find(*provinceNums.begin());
	if (stateIDMapping != stateMapper::getStateIdMapping().end())
	{
		stateID = stateIDMapping->second;
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not find the state for Vic2 province " << *provinceNums.begin() << ".";
	}
}


void Vic2State::setFactoryLevel(shared_ptr<Object> stateObj)
{
	for (auto buildingObj: stateObj->getValue("state_buildings"))
	{
		factoryLevel += buildingObj->safeGetInt("level");
	}
}


void Vic2State::determineEmployedWorkers()
{
	workerStruct workers = countEmployedWorkers();
	workers = limitWorkersByFactoryLevels(workers);
	employedWorkers = determineEmployedWorkersScore(workers);
}


workerStruct Vic2State::countEmployedWorkers() const
{
	workerStruct workers;

	for (auto province: provinces)
	{
		workers.craftsmen += province->getPopulation("craftsmen");
		workers.clerks += province->getPopulation("clerks");
		workers.artisans += province->getPopulation("artisans");
		workers.capitalists += province->getLiteracyWeightedPopulation("capitalists");
	}

	return workers;
}


workerStruct Vic2State::limitWorkersByFactoryLevels(const workerStruct& workers) const
{
	workerStruct newWorkers;
	if ((workers.craftsmen + workers.clerks) > (factoryLevel * 10000))
	{
		newWorkers.craftsmen = static_cast<int>((factoryLevel * 10000.0f) / (workers.craftsmen + workers.clerks) * workers.craftsmen);
		newWorkers.clerks = static_cast<int>((factoryLevel * 10000.0f) / (workers.craftsmen + workers.clerks) * workers.clerks);
		newWorkers.artisans = workers.artisans;
	}
	else
	{
		newWorkers = workers;
	}

	return newWorkers;
}


int Vic2State::determineEmployedWorkersScore(const workerStruct& workers) const
{
	int employedWorkerScore = workers.craftsmen + (workers.clerks * 2) + static_cast<int>(workers.artisans * 0.5) + (workers.capitalists * 2);
	if (ownerHasNoCores())
	{
		employedWorkerScore /= 2;
	}

	return employedWorkerScore;
}


bool Vic2State::ownerHasNoCores() const
{
	for (auto province: provinces)
	{
		for (auto country: province->getCores())
		{
			if (country->getTag() == owner)
			{
				return false;
			}
		}
	}

	return true;
}


void Vic2State::determineIfPartialState()
{
	if (provinces.size() > 0)
	{
		auto fullState = stateMapper::getStateMapping().find(*provinceNums.begin());
		for (auto expectedProvince: fullState->second)
		{
			if (provinceNums.count(expectedProvince) == 0)
			{
				partialState = true;
				break;
			}
		}
	}
}


int Vic2State::getPopulation() const
{
	int population = 0;
	for (auto province: provinces)
	{
		population += province->getPopulation();
	}

	return population;
}


int Vic2State::getAverageRailLevel() const
{
	int totalRailLevel = 0;
	for (auto province: provinces)
	{
		totalRailLevel += province->getRailLevel();
	}

	if (provinces.size() > 0)
	{
		return (totalRailLevel / provinces.size());
	}
	else
	{
		return 0;
	}
}
