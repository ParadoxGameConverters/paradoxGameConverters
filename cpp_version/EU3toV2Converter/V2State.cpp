#include "V2State.h"



V2State::V2State(int newId)
{
	id = newId;
}


void V2State::addProvince(V2Province newProvince)
{
	provinces.push_back(newProvince);
}