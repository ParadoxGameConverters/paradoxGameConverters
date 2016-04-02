/*Copyright (c) 2015 The Paradox Game Converters Project

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



V2Province::V2Province(Object* obj)
{
	num = atoi(obj->getKey().c_str());

	vector<Object*> ownerObjs;				// the object holding the owner
	ownerObjs = obj->getValue("owner");
	(ownerObjs.size() == 0) ? ownerString = "" : ownerString = ownerObjs[0]->getLeaf();
	owner = NULL;

	cores.clear();
	vector<Object*> coreObjs;				// the object holding the cores
	coreObjs = obj->getValue("core");
	for (unsigned int i = 0; i < coreObjs.size(); i++)
	{
		cores.push_back(coreObjs[i]->getLeaf());
	}

	vector<Object*> buildingObjs;
	fortLevel = 0;
	buildingObjs = obj->getValue("fort");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			fortLevel = atoi(tokens[0].c_str());
		}
	}
	navalBaseLevel = 0;
	buildingObjs = obj->getValue("naval_base");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			navalBaseLevel = atoi(tokens[0].c_str());
		}
	}
	railLevel = 0;
	buildingObjs = obj->getValue("railroad");
	if (buildingObjs.size() > 0)
	{
		vector<string> tokens = buildingObjs[0]->getTokens();
		if (tokens.size() > 0)
		{
			railLevel = atoi(tokens[0].c_str());
		}
	}

	// read pops
	vector<Object*> children = obj->getLeaves();
	for (vector<Object*>::iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		string key = (*itr)->getKey();
		if (key == "aristocrats" || key == "artisans" || key == "bureaucrats" || key == "capitalists" || key == "clergymen"
			|| key == "craftsmen" || key == "clerks" || key == "farmers" || key == "soldiers" || key == "officers"
			|| key == "labourers" || key == "slaves")
		{
			V2Pop* pop = new V2Pop(*itr);
			pops.push_back(pop);
		}
	}
}


vector<V2Country*> V2Province::getCores(const map<string, V2Country*>& countries) const
{
	vector<V2Country*> coreOwners;
	for (vector<string>::const_iterator i = cores.begin(); i != cores.end(); i++)
	{
		map<string, V2Country*>::const_iterator j = countries.find(*i);
		if (j != countries.end())
		{
			coreOwners.push_back(j->second);
		}
	}

	return coreOwners;
}


void V2Province::addCore(string newCore)
{
	// only add if unique
	if ( find(cores.begin(), cores.end(), newCore) == cores.end() )
	{
		cores.push_back(newCore);
	}
}


void V2Province::removeCore(string tag)
{
	for (vector<string>::iterator i = cores.begin(); i != cores.end(); i++)
	{
		if (*i == tag)
		{
			cores.erase(i);
			if (cores.size() == 0)
			{
				break;
			}
			i = cores.begin();
		}
	}
}


int V2Province::getTotalPopulation() const
{
	int total = 0;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		total += (*itr)->getSize();
	}
	return total;
}


//static bool PopSortBySizePredicate(const V2Pop* pop1, const V2Pop* pop2)
//{
//	return (pop1->getSize() > pop2->getSize());
//}



int V2Province::getPopulation(string type) const
{
	int totalPop = 0;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		// empty string for type gets total population
		if (type == "" || type == (*itr)->getType())
		{
			totalPop += (*itr)->getSize();
		}
	}
	return totalPop;
}


int V2Province::getLiteracyWeightedPopulation(string type) const
{
	double literacyWeight = Configuration::getLiteracyWeight();
	int totalPop = 0;
	for (vector<V2Pop*>::const_iterator itr = pops.begin(); itr != pops.end(); ++itr)
	{
		// empty string for type gets total population
		if (type == "" || type == (*itr)->getType())
		{
			totalPop += int((*itr)->getSize() * ((*itr)->getLiteracy() * literacyWeight + (1.0 - literacyWeight)));
		}
	}
	return totalPop;
}