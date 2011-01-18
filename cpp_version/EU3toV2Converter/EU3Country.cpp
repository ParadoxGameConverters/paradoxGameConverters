#include "EU3Country.h"


void EU3Country::init(Object* obj)
{
	tag = obj->getKey();
}


string EU3Country::getTag()
{
	return tag;
}