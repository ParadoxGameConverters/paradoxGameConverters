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
	vector<Object*> movementObj = obj->getValue("movement_progress");
	if (movementObj.size() > 0)
	{
		movementProgress = atof( movementObj[0]->getLeaf().c_str() );
	}
	else
	{
		movementProgress = 0.0F;
	}
	
	vector<Object*> pathObj = obj->getValue("path");
	if (pathObj.size() > 0)
	{
		vector<string> pathStrs = pathObj[0]->getTokens();
		for (unsigned int i = 0; i < pathStrs.size(); i++)
		{
			path.push_back( atoi(pathStrs[i].c_str()) );
		}
	}

	location = atoi( obj->getLeaf("location").c_str() );
}