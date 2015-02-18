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



#include "V2State.h"
#include "V2Pop.h"
#include "V2Province.h"
#include "V2Factory.h"
#include "../log.h"


V2State::V2State(int newId, V2Province* firstProvince)
{
	id				= newId;
	colonial		= false;
	colonised	= false;
	provinces.clear();
	provinces.push_back(firstProvince);
	factories.clear();
	employedCraftsmen = 0;

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


int V2State::getCraftsmenPerFactory() const
{
	int totalCraftsmen = 0;
	for (vector<V2Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		vector<V2Pop*> pops = (*itr)->getPops("craftsmen");
		for (vector<V2Pop*>::iterator pitr = pops.begin(); pitr != pops.end(); ++pitr)
		{
			totalCraftsmen += (*pitr)->getSize();
		}
	}
	return totalCraftsmen / (factories.size() + 1);
}


bool V2State::provInState(int id) const
{
	for (vector<V2Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ( (*itr)->getNum() == id )
		{
			return true;
		}
	}

	return false;
}


int V2State::getStatePopulation() const
{
	int population = 0;
	for (vector<V2Province*>::const_iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		population += (*itr)->getOldPopulation();
	}
	return population;
}
