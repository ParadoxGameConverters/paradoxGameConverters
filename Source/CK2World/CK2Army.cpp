#include "CK2Army.h"
#include "..\log.h"



CK2Subunit::CK2Subunit(const Object* obj)
{
}


CK2Army::CK2Army(const Object* obj)
{
	name = obj->getLeaf("name");
	vector<Object*> subunitObjs = obj->getValue("sub_unit");
	for (unsigned int i = 0; i < subunitObjs.size(); i++)
	{
		CK2Subunit* newSubunit = new CK2Subunit(subunitObjs[i]);
		subunits.push_back(newSubunit);
	}

	location = atoi( obj->getLeaf("location").c_str() );
}