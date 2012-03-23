#include "CK2Character.h"



void CK2Character::init(Object* obj)
{
	name		= obj->getLeaf("birth_name");
	religion	= obj->getLeaf("religion");
	culture	= obj->getLeaf("culture");
}


string CK2Character::getName()
{
	return name;
}