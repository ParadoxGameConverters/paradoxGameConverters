#include "EU3World.h"


void EU3World::init(Object* obj) {
	std::string key;	
	std::vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0 && atoi(key.c_str()) < 2000)
		{
			EU3Province province;
			province.init(leaves[i]);
			provinces.push_back(province);
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) && 
					(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
					(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
					(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				  )
		{
			EU3Country country;
			country.init(leaves[i]);
			countries.push_back(country);
		}
	}
}


vector<EU3Country> EU3World::getCountries()
{
	return countries;
}


EU3Province* EU3World::getProvince(int provNum)
{
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		if (provinces[i].getNum() == provNum)
		{
			return &(provinces[i]);
		}
	}
	return NULL;
}