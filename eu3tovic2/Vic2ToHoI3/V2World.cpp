#include "V2World.h"
#include "Log.h"
#include "Configuration.h"

void V2World::init(Object* obj) {
	string key;	
	vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
		{
			V2Province province;
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
			V2Country country;
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
				break;
			}
		}
	}

	// add province core info to countries
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		vector<string> cores = provinces[i].getCores();
		for (unsigned int j = 0; j < countries.size(); j++)
		{
			for (unsigned int k = 0; k < cores.size(); k++)
			{
				if (cores[k] == countries[j].getTag())
				{
					countries[j].addCore( &provinces[i]);
					break;
				}
			}
		}
	}

	// Diplomacy
	vector<Object*> diploObj = obj->getValue("diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy.init(diploObj[0]);
	}
}


vector<V2Country> V2World::getCountries()
{
	return countries;
}


V2Country* V2World::getCountry(string tag)
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


V2Province* V2World::getProvince(int provNum)
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
void V2World::removeCountries(vector<string>& tags)
{
	vector<V2Country> newCountries;
	for (vector<V2Country>::iterator ci = countries.begin(); ci != countries.end(); ++ci)
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


void V2World::removeCountry(string tag)
{
	for (vector<V2Country>::iterator i = countries.begin(); i < countries.end(); i++)
	{
		if (tag == i->getTag())
		{
			countries.erase(i);
			break;
		}
	}
}
