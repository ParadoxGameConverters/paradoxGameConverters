#include "EU3Province.h"
#include "Log.h"


void EU3Province::init(Object* obj) {
	num = atoi(obj->getKey().c_str());

	vector<Object*> ownerObjs;
	ownerObjs = obj->getValue("owner");
	if (ownerObjs.size() == 0)
	{
		owner = "";
	}
	else
	{
		owner = ownerObjs[0]->getLeaf();
		owner = owner.substr(1,3);
	}

	vector<Object*> coreObjs;
	coreObjs = obj->getValue("core");
	for (unsigned int i = 0; i < coreObjs.size(); i++)
	{
		cores.push_back(coreObjs[i]->getLeaf().substr(1,3));
	}

	vector<Object*> cultureObj;
	cultureObj = obj->getValue("culture");
	if (cultureObj.size() == 0)
	{
		culture == "";
	}
	else
	{
		culture = cultureObj[0]->getLeaf();
	}

	vector<Object*> religionObj;
	religionObj = obj->getValue("religion");
	if (religionObj.size() == 0)
	{
		religion == "";
	}
	else
	{
		religion = religionObj[0]->getLeaf();
	}


	colony = true;
	vector<Object*> popObj = obj->getValue("citysize");
	if (popObj.size() > 0)
	{
		population	= atoi( (popObj)[0]->getLeaf().c_str() );
		if (population >= 1000)
		{
			colony = false;
		}
	}
	else
	{
		popObj		= obj->getValue("native_size");
		if (popObj.size() > 0)
		{
			population	= atoi( (popObj)[0]->getLeaf().c_str() );
		}
		else
		{
			population = 0;
		}
	}
}


int EU3Province::getNum()
{
	return num;
}


string EU3Province::getOwner()
{
	return owner;
}


vector<string> EU3Province::getCores()
{
	return cores;
}


string EU3Province::getCulture()
{
	return culture;
}


string EU3Province::getReligion()
{
	return religion;
}


bool EU3Province::isColony()
{
	return colony;
}