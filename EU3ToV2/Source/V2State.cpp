#include "V2State.h"
#include "tempFuncs.h"
#include "V2Pop.h"
#include "V2Province.h"
#include "V2Factory.h"
#include "Log.h"


V2State::V2State(int newId, V2Province* firstProvince)
{
	id				= newId;
	colonial		= false;
	colonised	= false;
	provinces.clear();
	provinces.push_back(firstProvince);
	factories.clear();
}


void V2State::output(FILE* output) const
{
	fprintf(output, "\tstate=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tid=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\tid=%d\n", id);
	fprintf(output, "\t\t\ttype=47\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tprovinces=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t");
	for (vector<V2Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		fprintf(output, "%d ", (*i)->getNum());
	}
	fprintf(output, "\n");
	fprintf(output, "\t\t}\n");
	for (vector<const V2Factory*>::const_iterator itr = factories.begin(); itr != factories.end(); ++itr)
	{
		(*itr)->output(output);
	}
	if (colonial)
	{		
	}
	if (colonised)
	{
		if ((Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM"))
		{
			fprintf(output, "\t\tis_colonial=2\n");
		}
		else
		{
			fprintf(output, "\t\tis_colonial=yes\n");
		}
	}
	fprintf(output, "\t}\n");
}


void V2State::addRailroads()
{
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		(*itr)->setRailLevel(1);
	}
}


void V2State::setupPops(WorldType game, string primaryCulture, vector<string> acceptedCultures, string religion, double nationalConModifier, double nationalMilModifier)
{
	int	statePopulation = getStatePopulation();
	bool	cot = hasCOT();
	for (vector<V2Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		(*itr)->doCreatePops(game, (itr == provinces.begin()), statePopulation, cot);
		(*itr)->setPopConMil(primaryCulture, acceptedCultures, religion, nationalConModifier, nationalMilModifier);
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
