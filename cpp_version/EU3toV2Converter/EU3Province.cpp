#include "EU3Province.h"


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
}


int EU3Province::getNum()
{
	return num;
}


string EU3Province::getOwner()
{
	return owner;
}