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



#include "V2Province.h"
#include "Log.h"
#include "Object.h"
#include "V2Pop.h"
using namespace std;



V2Province::V2Province(shared_ptr<Object> obj)
{
	readOwner(obj);
	readCores(obj);
	readForts(obj);
	readNavalBases(obj);
	readRails(obj);
	readPops(obj);
	readRgo(obj);
	
	number = stoi(obj->getKey());
}


void V2Province::readOwner(shared_ptr<Object> obj)
{
	owner = NULL;

	vector<shared_ptr<Object>> ownerObjs = obj->getValue("owner");
	if (ownerObjs.size() == 0)
	{
		ownerString = "";
	}
	else
	{
		ownerString = ownerObjs[0]->getLeaf();
	}
}


void V2Province::readCores(shared_ptr<Object> obj)
{
	cores.clear();

	for (auto coreObj: obj->getValue("core"))
	{
		coreStrings.insert(coreObj->getLeaf());
	}
}


void V2Province::readForts(shared_ptr<Object> obj)
{
	fortLevel = 0;

	vector<shared_ptr<Object>> buildingObjs = obj->getValue("fort");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			fortLevel = stoi(tokens[0]);
		}
	}
}


void V2Province::readNavalBases(shared_ptr<Object> obj)
{
	navalBaseLevel = 0;

	vector<shared_ptr<Object>> buildingObjs = obj->getValue("naval_base");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			navalBaseLevel = stoi(tokens[0]);
		}
	}
}


void V2Province::readRails(shared_ptr<Object> obj)
{
	railLevel = 0;

	vector<shared_ptr<Object>> buildingObjs = obj->getValue("railroad");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			railLevel = stoi(tokens[0]);
		}
	}
}

void V2Province::readRgo(shared_ptr<Object> obj)
{
	shared_ptr<Object> rgoObj = obj->safeGetObject("rgo");
	if (!rgoObj) return;
	string goods = rgoObj->safeGetString("goods_type");
	shared_ptr<Object> employment = rgoObj->safeGetObject("employment");
	if (!employment) return;
	shared_ptr<Object> employees = employment->safeGetObject("employees");
	if (!employees) return;
	vector<shared_ptr<Object>> pops = employees->getLeaves();
	int workers = 0;
	for (const auto& pop : pops) {
		workers += pop->safeGetInt("count");
	}
	rgo = V2Rgo(goods, workers);
}

void V2Province::readPops(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> children = obj->getLeaves();
	for (auto child: children)
	{
		if (isPopObject(child))
		{
			V2Pop* pop = new V2Pop(child);
			pops.push_back(pop);
		}
	}
}


bool V2Province::isPopObject(shared_ptr<Object> obj)
{
	string key = obj->getKey();

	if ( key == "aristocrats" ||
		  key == "artisans" ||
		  key == "bureaucrats" ||
		  key == "capitalists" ||
		  key == "clergymen"	||
		  key == "craftsmen" ||
		  key == "clerks" ||
		  key == "farmers" ||
		  key == "soldiers" ||
		  key == "officers" ||
		  key == "labourers" ||
		  key == "slaves"
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void V2Province::setCores(const map<string, V2Country*>& countries)
{
	for (auto coreString: coreStrings)
	{
		auto countryItr = countries.find(coreString);
		if (countryItr != countries.end())
		{
			cores.insert(countryItr->second);
		}
	}
}


int V2Province::getTotalPopulation() const
{
	return getPopulation("");
}


int V2Province::getPopulation(string type) const
{
	int totalPopulation = 0;
	for (auto pop: pops)
	{
		if (type == "" || type == pop->getType())
		{
			totalPopulation += pop->getSize();
		}
	}

	return totalPopulation;
}


int V2Province::getLiteracyWeightedPopulation(string type) const
{
	int totalPopulation = 0;
	for (auto pop: pops)
	{
		if (type == "" || type == pop->getType())
		{
			totalPopulation += calculateLiteracyWeightedPop(pop);
		}
	}
	return totalPopulation;

	return 0;
}


double V2Province::getPercentageWithCultures(const set<string>& cultures) const
{
	int totalPopulation = 0;
	int populationOfCultures = 0;

	for (auto pop: pops)
	{
		totalPopulation += pop->getSize();
		if (cultures.count(pop->getCulture()) > 0)
		{
			populationOfCultures += pop->getSize();
		}
	}

	return 1.0 * populationOfCultures / totalPopulation;
}


int V2Province::calculateLiteracyWeightedPop(const V2Pop* thePop) const
{
	return int(thePop->getSize() * (thePop->getLiteracy() * 0.9 + 0.1));
}
