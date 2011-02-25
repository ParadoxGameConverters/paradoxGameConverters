#include "V2Country.h"


void V2Country::init(string newTag, EU3Country* oldCountry)
{
	tag				= newTag;
	sourceCountry	= oldCountry;
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
	for(unsigned int i = 0; i < states.size(); i++)
	{
		states[i].output(output);
	}
	fprintf(output, "}\n");
}


EU3Country* V2Country::getSourceCountry()
{
	return sourceCountry;
}