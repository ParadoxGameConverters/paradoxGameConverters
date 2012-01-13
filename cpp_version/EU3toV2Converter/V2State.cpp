#include "V2State.h"
#include "tempFuncs.h"


V2State::V2State(int newId)
{
	id = newId;
}


void V2State::addProvince(V2Province* newProvince)
{
	provinces.push_back(newProvince);
}


void V2State::addFactory(V2Factory factory)
{
	factories.push_back(factory);
}


void V2State::setColonial(bool isIt)
{
	colonial = isIt;
}


bool V2State::isColonial()
{
	return colonial;
}


bool V2State::isCoastal()
{
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->isCoastal())
			return true;
	}
	return false;
}


bool V2State::hasLocalSupply(string product)
{
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ((*itr)->getRgoType() == product)
			return true;
	}
	return false;
}


int V2State::getCraftsmenPerFactory()
{
	int totalCraftsmen = 0;
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		vector<V2Pop> pops = (*itr)->getPops("craftsmen");
		for (vector<V2Pop>::iterator pitr = pops.begin(); pitr != pops.end(); ++pitr)
		{
			totalCraftsmen += pitr->getSize();
		}
	}
	return totalCraftsmen / (factories.size() + 1);
}


int V2State::getFactoryCount()
{
	return factories.size();
}


int V2State::getID()
{
	return id;
}


int V2State::getStatePopulation()
{
	int population = 0;
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		population += (*itr)->getOldPopulation();
	}
	return population;
}


bool V2State::hasCOT()
{
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ( (*itr)->getCOT() )
		{
			return true;
		}
	}

	return false;
}


void V2State::setupPops(EU3World& sourceWorld, string primaryCulture, vector<string> acceptedCultures, string religion, double nationalConModifier, double nationalMilModifier)
{
	int statePopulation = getStatePopulation();
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		(*itr)->doCreatePops((itr == provinces.begin()), statePopulation, sourceWorld, hasCOT());
		(*itr)->setPopConMil(primaryCulture, acceptedCultures, religion, nationalConModifier, nationalMilModifier);
	}
}


void V2State::output(FILE* output)
{
	fprintf(output, "	state=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		id=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			id=%d\n", id);
	fprintf(output, "			type=47\n");
	fprintf(output, "		}\n");
	fprintf(output, "		provinces=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			");
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		fprintf(output, "%d ", provinces[i]->getNum());
	}
	fprintf(output, "\n");
	fprintf(output, "		}\n");
	for (vector<V2Factory>::iterator itr = factories.begin(); itr != factories.end(); ++itr)
	{
		itr->output(output);
	}
	if (colonial)
	{
		fprintf(output, "		is_colonial=yes\n");
	}
	fprintf(output, "	}\n");
}