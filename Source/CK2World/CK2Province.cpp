#include "CK2Province.h"



void CK2Province::init(Object* obj, map<string, CK2Title*> titles)
{
	number = atoi( obj->getKey().c_str() );

	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if (key[0] == 'b')
		{
			CK2Barony* newBarony = new CK2Barony;
			newBarony->init( leaves[i], titles[key]);
			baronies.push_back(newBarony);
		}
	}
}


vector<CK2Barony*> CK2Province::getBaronies()
{
	return baronies;
}