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



Vic2State::Vic2State(const Object* stateObj, string ownerTag)
{
	owner = ownerTag;
	partialState = false;

	addProvinces(stateObj);
	setFactoryLevel(stateObj);
}


void Vic2State::addProvinces(const Object* stateObj)
{
	vector<string> provinceIDs = getProvinceIDs(stateObj);
	for (auto provinceItr: provinceIDs)
	{
		provinces.insert(atoi(provinceItr.c_str()));
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
		auto fullState = stateMap.find(*provinces.begin());
		for (auto expectedProvince: fullState->second)
		{
			if (provinces.count(expectedProvince) == 0)
			{
				partialState = true;
				break;
			}
		}
	}
}


int Vic2State::getEmployedWorkers(const V2World* sourceWorld) const
{
	int workers = 0;
	for (auto provinceNum: provinces)
	{
		V2Province* sourceProvince = sourceWorld->getProvince(provinceNum);
		workers += sourceProvince->getEmployedWorkers();
	}

	return workers;
}