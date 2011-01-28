#include "V2Country.h"


void V2Country::init(Object* obj)
{
	tag = obj->getKey();
}


string V2Country::getTag()
{
	return tag;
}


void V2Country::addState(V2State newState)
{
	states.push_back(newState);
}


void V2Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	outputCountryHeader(output);
	outputCountryMiddle(output);
	for(unsigned int i = 0; i < states.size(); i++)
	{
		states[i].output(output);
	}
	outputCountryFooter(output);
	fprintf(output, "}\n");
}