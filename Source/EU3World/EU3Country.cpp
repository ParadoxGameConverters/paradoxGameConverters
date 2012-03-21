#include "EU3Country.h"


void EU3Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	fprintf(output, "}\n");
}


void EU3Country::init(string newTag, string newCountryFile)
{
	tag			= newTag;
	countryFile	= newCountryFile;
}


string EU3Country::getTag()
{
	return tag;
}