#include "EU3Leader.h"
#include "Log.h"
#include "Parsers\Object.h"



EU3Leader::EU3Leader(Object *obj)
{
	vector<Object*> subObj = obj->getValue("name");
	if (subObj.size() > 0)
	{
		name = subObj[0]->getLeaf();
	}
	subObj = obj->getValue("type");
	if (subObj.size() > 0)
	{
		type = subObj[0]->getLeaf();
	}
	subObj = obj->getValue("manuever");
	if (subObj.size() > 0)
	{
		manuever = atoi(subObj[0]->getLeaf().c_str());
	}
	subObj = obj->getValue("fire");
	if (subObj.size() > 0)
	{
		fire = atoi(subObj[0]->getLeaf().c_str());
	}
	subObj = obj->getValue("shock");
	if (subObj.size() > 0)
	{
		shock = atoi(subObj[0]->getLeaf().c_str());
	}
	subObj = obj->getValue("siege");
	if (subObj.size() > 0)
	{
		siege = atoi(subObj[0]->getLeaf().c_str());
	}
	subObj = obj->getValue("id");
	if (subObj.size() > 0)
	{
		subObj = subObj[0]->getValue("id");
		id = atoi(subObj[0]->getLeaf().c_str());
	}
	subObj = obj->getValue("activation");
	if (subObj.size() > 0)
	{
		activationDate = date(subObj[0]->getLeaf());
	}
}


bool EU3Leader::isLand() const
{
	if (type == "general" || type == "conquistador")
		return true;
	else if (type == "explorer" || type == "admiral")
		return false;
	else
	{
		log("Error: Unknown leader type %s!\n", type.c_str());
	}
	return false;
}