#include "EU3World.h"


void EU3World::init(Object* obj) {
	string key;	
	vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
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

	// add province owner info to countries
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		for (unsigned int j = 0; j < countries.size(); j++)
		{
			if (provinces[i].getOwner() == countries[j].getTag())
			{
				countries[j].addProvince( &provinces[i]);
				continue;
			}
		}
	}

	// add province core info to countries
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		for (unsigned int j = 0; j < countries.size(); j++)
		{
			vector<string> cores = provinces[i].getCores();
			for (unsigned int k = 0; k < cores.size(); k++)
			{
				if (cores[k] == countries[j].getTag())
				{
					countries[j].addCore( &provinces[i]);
					continue;
				}
			}
		}
	}

	vector<Object*> diploObj = obj->getValue("diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy.init(diploObj[0]);
	}
}


vector<EU3Country> EU3World::getCountries()
{
	return countries;
}


EU3Country* EU3World::getCountry(string tag)
{
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		if (countries[i].getTag() == tag)
		{
			return &(countries[i]);
		}
	}
	return NULL;
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


// calling vector::erase in a loop is really slow.  batch removals to avoid it.
void EU3World::removeCountries(vector<string>& tags)
{
	vector<EU3Country> newCountries;
	for (vector<EU3Country>::iterator ci = countries.begin(); ci != countries.end(); ++ci)
	{
		bool shouldBeRemoved = false;
		for (vector<string>::iterator ti = tags.begin(); ti != tags.end(); ++ti)
		{
			if (ci->getTag() == *ti)
			{
				shouldBeRemoved = true;
				break;
			}
		}
		if (!shouldBeRemoved)
			newCountries.push_back(*ci);
	}
	countries.swap(newCountries);
}


void EU3World::removeCountry(string tag)
{
	for (vector<EU3Country>::iterator i = countries.begin(); i < countries.end(); i++)
	{
		if (tag == i->getTag())
		{
			countries.erase(i);
			break;
		}
	}
}


EU3Diplomacy& EU3World::getDiplomacy()
{
	return diplomacy;
}


void EU3World::resolveRegimentTypes(const RegimentTypeMap& map)
{
	for (vector<EU3Country>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		itr->resolveRegimentTypes(map);
	}
}