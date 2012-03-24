#include "CK2Dynasty.h"



void CK2Dynasty::init(Object* obj)
{
	num = atoi( obj->getKey().c_str() );
	vector<Object*> nameLeaves = obj->getValue("name");
	if (nameLeaves.size() > 0)
	{
		name = nameLeaves[0]->getLeaf();
	}
	else
	{
		name = "";
	}
}


int CK2Dynasty::getNum()
{
	return num;
}


string CK2Dynasty::getName()
{
	return name;
}