#include "V2Pop.h"

void V2Pop::init(Object *obj)
{
	type = obj->getKey();

	vector<Object*> childObj = obj->getValue("id");
	if (childObj.size() > 0)
	{
		id = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("size");
	if (childObj.size() > 0)
	{
		size = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("money");
	if (childObj.size() > 0)
	{
		money = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("mil");
	if (childObj.size() > 0)
	{
		mil = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("con");
	if (childObj.size() > 0)
	{
		con = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("literacy");
	if (childObj.size() > 0)
	{
		lit = atoi(childObj[0]->getLeaf().c_str());
	}
}