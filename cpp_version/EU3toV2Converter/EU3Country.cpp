#include "EU3Country.h"


void EU3Country::init(Object* obj)
{
	tag = obj->getKey();
}


string EU3Country::getTag()
{
	return tag;
}


void EU3Country::addProvince(EU3Province* province)
{
	provinces.push_back(province);
}


vector<EU3Province*>	EU3Country::getProvinces()
{
	return provinces;
}