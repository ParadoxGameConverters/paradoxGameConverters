/*Copyright (c) 2014 The Paradox Game Converters Project

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

#include "../EU4World/EU4Province.h"
#include "V2State.h"
#include "V2Pop.h"
#include "V2Province.h"
#include "V2Factory.h"
#include "Log.h"

V2State::V2State(int newId, V2Province* firstProvince)
{
	id = newId;
	colonial = false;
	provinces.clear();
	provinces.push_back(firstProvince);

	factories.clear();
}

void V2State::addRailroads()
{
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		(*itr)->setRailLevel(1);
	}
}

bool V2State::isCoastal() const
{
	for (vector<V2Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->isCoastal())
		{
			return true;
		}
	}
	return false;
}

bool V2State::hasLocalSupply(string product) const
{
	for (vector<V2Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->getRgoType() == product)
		{
			return true;
		}
	}
	return false;
}

double V2State::getSuppliedInputs(const V2Factory* factory) const
{
	// find out the needs
	map<string, float>	inputs = factory->getInputs();
	int						numNeeds = inputs.size();

	// find out what we have from both RGOs and existing factories
	map<string, float> supplies;
	for (vector<V2Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		string rgo = (*itr)->getRgoType();
		supplies[rgo] += 1.0;
	}
	for (vector<const V2Factory*>::const_iterator itr = factories.begin(); itr != factories.end(); itr++)
	{
		supplies[(*itr)->getOutputGoods()] += 1.0;
	}

	// determine how many of the inputs are supplied
	int totalSupplied = 0;
	for (map<string, float>::const_iterator inputItr = inputs.begin(); inputItr != inputs.end(); inputItr++)
	{
		map<string, float>::const_iterator supplyItr = supplies.find(inputItr->first);
		if (supplyItr != supplies.end())
		{
			totalSupplied++;
		}
	}

	return (1.0 * totalSupplied / numNeeds);
}

bool V2State::provInState(int id) const
{
	for (vector<V2Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->getNum() == id)
		{
			return true;
		}
	}

	return false;
}

void V2State::addFactory(V2Factory* factory)
{
	provinces[0]->addFactory(factory);
	factories.push_back(factory);
}

bool V2State::hasLandConnection() const
{
	if (provinces.size() > 0)
	{
		return provinces[0]->hasLandConnection();
	}
	else
	{
		return false;
	}
}

double V2State::getManuRatio() const
{
	// get all source provinces
	set<const EU4Province*> srcProvinces;
	for (auto itr = provinces.begin(); itr != provinces.end(); itr++)
	{
		srcProvinces.insert((*itr)->getSrcProvince());
	}

	// count the manufactories in the source provinces
	int numManus = 0;
	for (auto itr = srcProvinces.begin(); itr != srcProvinces.end(); itr++)
	{
		if ((*itr)->hasBuilding("refinery") ||
			(*itr)->hasBuilding("wharf") ||
			(*itr)->hasBuilding("weapons") ||
			(*itr)->hasBuilding("textile") ||
			(*itr)->hasBuilding("farm_estate") ||
			(*itr)->hasBuilding("plantations") ||
			(*itr)->hasBuilding("tradecompany")
			)
		{
			numManus++;
		}
	}

	return (numManus / srcProvinces.size());
}

void	V2State::colloectNavalBase()
{
	//Only one naval base in a state
	V2Province* prov = nullptr;
	int level = 0;
	for (auto province : provinces)
	{
		if (!prov)
		{
			prov = province;
		}
		level += province->getNavalBaseLevel();
		if (prov->getNavalBaseLevel() < province->getNavalBaseLevel())
		{
			prov = province;
		}
	}
	LOG(LogLevel::Debug) << this->provinces.size() << " provinces in state " << this->id;
	for (auto province : provinces)
	{
		province->setNavalBaseLevel(0);
		LOG(LogLevel::Debug) << province->getName() << " naval base set to 0";	//test
	}
	prov->setNavalBaseLevel(level > 2 ? 2 : level);
}