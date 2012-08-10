#include "CK2Province.h"
#include "..\Parsers\Object.h"
#include "CK2Barony.h"
#include "..\Log.h"



CK2Province::CK2Province(Object* obj, map<string, CK2Title*> titles)
{
	number = atoi( obj->getKey().c_str() );

	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if (key[0] == 'b')
		{
			CK2Barony* newBarony = new CK2Barony( leaves[i], titles[key], this);
			baronies.push_back(newBarony);
		}
	}

	vector<Object*> cultureObj	= obj->getValue("culture");
	if (cultureObj.size() > 0)
	{
		culture  = cultureObj[0]->getLeaf();
	}
	else
	{
		culture = "";
	}

	vector<Object*> religionObj = obj->getValue("religion");
	if (religionObj.size() > 0)
	{
		religion  = religionObj[0]->getLeaf();
	}
	else
	{
		religion = "";
	}
}