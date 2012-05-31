#include "CK2History.h"
#include "..\Parsers\Object.h"



CK2History::CK2History()
{
	when		= (date)"1.1.1";
	holder	= NULL;
}


void CK2History::init(Object* obj, map<int, CK2Character*>& characters)
{
	when = obj->getKey();
	vector<Object*> historyObj = obj->getLeaves();

	holder = NULL;
	for (unsigned int i = 0; i < historyObj.size(); i++)
	{
		string key = historyObj[i]->getKey();
		if (key == "holder")
		{
			holder = characters[ atoi( historyObj[i]->getLeaf().c_str() ) ];
		}
	}
}


date CK2History::getWhen()
{
	return when;
}


CK2Character* CK2History::getHolder()
{
	return holder;
}