#include "CK2Character.h"



void CK2Character::init(Object* obj, map<int, CK2Dynasty*>& dynasties)
{
	name		= obj->getLeaf("birth_name");
	religion	= obj->getLeaf("religion");
	culture	= obj->getLeaf("culture");
	dynasty	= dynasties[ atoi( obj->getLeaf("dynasty").c_str() ) ];
}


string CK2Character::getName()
{
	return name;
}


CK2Dynasty* CK2Character::getDynasty()
{
	return dynasty;
}