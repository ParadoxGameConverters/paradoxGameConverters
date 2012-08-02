#include "CK2Trait.h"
#include "..\Parsers\Object.h"



CK2Trait::CK2Trait(Object* obj)
{
	vector<Object*> diplomacyObj = obj->getValue("diplomacy");
	if (diplomacyObj.size() > 0)
	{
		diplomacy = atoi( diplomacyObj[0]->getLeaf().c_str() );
	}

	vector<Object*> martialObj = obj->getValue("martial");
	if (martialObj.size() > 0)
	{
		martial = atoi( martialObj[0]->getLeaf().c_str() );
	}

	vector<Object*> stewardshipObj = obj->getValue("stewardship");
	if (stewardshipObj.size() > 0)
	{
		stewardship = atoi( stewardshipObj[0]->getLeaf().c_str() );
	}

	vector<Object*> intrigueObj = obj->getValue("intrigue");
	if (intrigueObj.size() > 0)
	{
		intrigue = atoi( intrigueObj[0]->getLeaf().c_str() );
	}

	vector<Object*> learningObj = obj->getValue("learning");
	if (learningObj.size() > 0)
	{
		learning = atoi( learningObj[0]->getLeaf().c_str() );
	}
}