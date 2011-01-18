#include "V2Country.h"


void V2Country::init(Object* obj)
{
	tag = obj->getKey();
}


string V2Country::getTag()
{
	return tag;
}