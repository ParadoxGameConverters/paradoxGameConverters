/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include "V2Province.h"
#include "V2World.h"
#include "log.h"



Vic2State::Vic2State(const Object* stateObj, string ownerTag)
{
	owner = ownerTag;
	partialState = false;

	addProvinceNums(stateObj);
	setFactoryLevel(stateObj);
}


void Vic2State::putWorkersInProvinces()
{
	// get the employable workers
	int craftsmen		= 0;
	int clerks			= 0;
	int artisans		= 0;
	int capitalists	= 0;
	for (auto province: provinces)
	{
		craftsmen	+= province->getPopulation("craftsmen");
		clerks		+= province->getPopulation("clerks");
		artisans		+= province->getPopulation("aristans");
		capitalists	+= province->getLiteracyWeightedPopulation("capitalists");
	}

	// limit craftsmen and clerks by factory levels
	if ((craftsmen + clerks) > (factoryLevel * 10000))
	{
		float newCraftsmen	= (factoryLevel * 10000.0f) / (craftsmen + clerks) * craftsmen;
		float newClerks		= (factoryLevel * 10000.0f) / (craftsmen + clerks) * clerks;

		craftsmen	= static_cast<int>(newCraftsmen);
		clerks		= static_cast<int>(newClerks);
	}

	// determine an actual 'employed workers' score
	int employedWorkers = craftsmen + (clerks * 2) + static_cast<int>(artisans * 0.5) + (capitalists * 2);

	if (provinces.size() > 0)
	{
		auto employmentProvince = provinces.begin();
		(*employmentProvince)->setEmployedWorkers(employedWorkers);
	}
}


void Vic2State::setID(const stateIdMapping& stateIdMap)
{
	auto stateIDMapping = stateIdMap.find(*provinceNums.begin());
	if (stateIDMapping != stateIdMap.end())
	{
		stateID = stateIDMapping->second;
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not find the state for Vic2 province " << *provinces.begin() << ".";
	}
}


void Vic2State::addProvinceNums(const Object* stateObj)
{
	vector<string> provinceIDs = getProvinceIDs(stateObj);
	for (auto provinceItr: provinceIDs)
	{
		provinceNums.insert(atoi(provinceItr.c_str()));
	}
}


vector<string> Vic2State::getProvinceIDs(const Object* stateObj)
{
	vector<string> provinceIDs;
	vector<Object*> provinceObj = stateObj->getValue("provinces");
	if (provinceObj.size() > 0)
	{
		provinceIDs = provinceObj[0]->getTokens();
	}

	return provinceIDs;
}


void Vic2State::setFactoryLevel(const Object* stateObj)
{
	factoryLevel = 0;

	vector<Object*> buildingsObj = stateObj->getValue("state_buildings");
	for (auto buildingObj: buildingsObj)
	{
		addBuildingLevel(buildingObj);
	}
}


void Vic2State::addBuildingLevel(const Object* buildingObj)
{
	vector<Object*> levelObj = buildingObj->getValue("level");
	if (levelObj.size() > 0)
	{
		factoryLevel += atoi(levelObj[0]->getLeaf().c_str());
	}
}


void Vic2State::determinePartialState(const stateMapping& stateMap)
{
	partialState = false;
	if (provinces.size() > 0)
	{
		auto fullState = stateMap.find(*provinceNums.begin());
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


int Vic2State::getEmployedWorkers() const
{
	int workers = 0;
	for (auto province: provinces)
	{
		workers += province->getEmployedWorkers();
	}

	return workers;
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
		totalRailLevel += province->getInfra();
	}

	return (totalRailLevel / provinces.size());
}