#include "EU3Country.h"


void EU3Country::init(Object* obj)
{
	tag = obj->getKey();

	vector<Object*> capitalObj = obj->getValue("capital");
	if (capitalObj.size() > 0)
	{
		capital = atoi( capitalObj[0]->getLeaf().c_str() );
	}
	else
	{
		capital = 0;
	}
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


int EU3Country::getCapital()
{
	return capital;
}