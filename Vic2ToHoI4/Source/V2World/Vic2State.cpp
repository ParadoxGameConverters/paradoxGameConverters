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



Vic2State::Vic2State(const Object* stateObj, string ownerTag)
{
	owner = ownerTag;
	partialState = false;

	addProvinceNums(stateObj);
	setID();
	setFactoryLevel(stateObj);
}


void Vic2State::addProvinceNums(const Object* stateObj)
{
	vector<string> provinceIDs = getProvinceIDs(stateObj);
	for (auto provinceItr: provinceIDs)
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


vector<string> Vic2State::getProvinceIDs(const Object* stateObj)
{
	vector<string> provinceIDs;
	vector<Object*> provinceObjs = stateObj->getValue("provinces");
	if (provinceObjs.size() > 0)
	{
		provinceIDs = provinceObjs[0]->getTokens();
	}

	return provinceIDs;
}


void Vic2State::setFactoryLevel(const Object* stateObj)
{
	factoryLevel = 0;

	vector<Object*> buildingsObjs = stateObj->getValue("state_buildings");
	for (auto buildingObj: buildingsObjs)
	{
		addBuildingLevel(buildingObj);
	}
}


void Vic2State::addBuildingLevel(const Object* buildingObj)
{
	vector<Object*> levelObjs = buildingObj->getValue("level");
	if (levelObjs.size() > 0)
	{
		factoryLevel += stoi(levelObjs[0]->getLeaf());
	}
}


void Vic2State::determineEmployedWorkers()
{
	workerStruct workers = countEmployedWorkers();
	workers = limitWorkersByFactoryLevels(workers);
	employedWorkers = determineEmployedWorkersScore(workers);
}


workerStruct Vic2State::countEmployedWorkers()
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


workerStruct Vic2State::limitWorkersByFactoryLevels(workerStruct workers)
{
	if ((workers.craftsmen + workers.clerks) > (factoryLevel * 10000))
	{
		float newCraftsmen = (factoryLevel * 10000.0f) / (workers.craftsmen + workers.clerks) * workers.craftsmen;
		float newClerks = (factoryLevel * 10000.0f) / (workers.craftsmen + workers.clerks) * workers.clerks;

		workers.craftsmen	= static_cast<int>(newCraftsmen);
		workers.clerks = static_cast<int>(newClerks);
	}

	return workers;
}


int Vic2State::determineEmployedWorkersScore(workerStruct workers)
{
	int employedWorkerScore = workers.craftsmen + (workers.clerks * 2) + static_cast<int>(workers.artisans * 0.5) + (workers.capitalists * 2);
	if (ownerHasNoCores())
	{
		employedWorkerScore /= 2;
	}

	return employedWorkerScore;
}


bool Vic2State::ownerHasNoCores()
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
	partialState = false;
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

	return (totalRailLevel / provinces.size());
}
