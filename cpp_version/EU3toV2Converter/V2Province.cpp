#include "V2Province.h"


void V2Province::init(Object* obj) {
	num = atoi(obj->getKey().c_str());

	vector<Object*> nameObj;
	nameObj = obj->getValue("name");
	if (nameObj.size() == 0)
	{
		name = "";
	}
	else
	{
		name = nameObj[0]->getLeaf();
	}
}


int V2Province::getNum()
{
	return num;
}


void V2Province::setOwner(string newOwner)
{
	owner = newOwner;
}