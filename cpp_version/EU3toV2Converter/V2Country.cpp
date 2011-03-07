#include "V2Country.h"



void V2Country::init(string newTag, string newCountryFile)
{
	tag			= newTag;
	countryFile	= newCountryFile;
}


void V2Country::setSourceCountryIndex(int oldCountryIndex)
{
	sourceCountryIndex	= oldCountryIndex;
}


string V2Country::getTag()
{
	return tag;
}


void V2Country::addState(V2State newState)
{
	states.push_back(newState);
}


void V2Country::setCapital(int newCapital)
{
	capital = newCapital;
}


void V2Country::setcivilized(bool isIt)
{
	civilized = isIt;
}


void V2Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	if (capital > 0)
	{
		fprintf(output, "	capital=%d\n", capital);
	}
	outputCountryHeader(output);
	outputCountryMiddle(output);
	if (civilized)
	{
		fprintf(output, "	civilized=yes\n");
	}
	else
	{
		fprintf(output, "	civilized=no\n");
	}
	for(unsigned int i = 0; i < states.size(); i++)
	{
		states[i].output(output);
	}
	fprintf(output, "}\n");
}


int V2Country::getSourceCountryIndex()
{
	return sourceCountryIndex;
}