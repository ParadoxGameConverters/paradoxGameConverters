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