#include "EU3Country.h"


void EU3Country::init(string newTag, string newCountryFile)
{
	tag			= newTag;
	countryFile	= newCountryFile;
}


string EU3Country::getTag()
{
	return tag;
}