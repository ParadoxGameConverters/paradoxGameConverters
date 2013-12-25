#include "EU4Relations.h"
#include "Parsers/Object.h"



EU4Relations::EU4Relations(Object* obj) {
	tag = obj->getKey();

	vector<Object*> valueObj = obj->getValue("value");
	if (valueObj.size() > 0)
	{
		value = atoi( valueObj[0]->getLeaf().c_str() );
	}
	else
	{
		value = 0;
	}

	vector<Object*> maObj = obj->getValue("military_access");
	if (maObj.size() > 0)
	{
		military_access = (maObj[0]->getLeaf() == "yes");
	}
	else
	{
		military_access = false;
	}

	vector<Object*> lastSendObj = obj->getValue("last_send_diplomat");
	if (lastSendObj.size() > 0)
	{
		last_send_diplomat = date(lastSendObj[0]);
	}
	else
	{
		last_send_diplomat = date();
	}

	vector<Object*> lastWarObj =  obj->getValue("last_war");
	if (lastWarObj.size() > 0)
	{
		last_war = date(lastWarObj[0]);
	}
	else
	{
		last_war = date();
	}
}
