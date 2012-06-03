#include "EU3History.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2History.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"



EU3History::EU3History()
{
	monarch	= NULL;
	heir		= NULL;
	advisor	= NULL;
}


void EU3History::init(CK2History* src)
{
	when = src->getWhen();

	monarch = NULL;
	CK2Character* holder = src->getHolder();
	if (holder != NULL)
	{
		monarch = new EU3Ruler(holder);
	}

	heir		= NULL;
	advisor	= NULL;
}


void EU3History::initMonarch(EU3Ruler* newMonarch, date newWhen)
{
	when = newWhen;
	
	monarch	= newMonarch;
	heir		= NULL;
	advisor	= NULL;
}


void EU3History::initHeir(EU3Ruler* newHeir, date newWhen)
{
	when = newWhen;

	monarch	= NULL;
	heir		= newHeir;
	advisor	= NULL;
}


void EU3History::initAdvisor(EU3Advisor* newAdvisor)
{
	when = newAdvisor->getDate();
	when.year += 16;

	monarch	= NULL;
	heir		= NULL;
	advisor	= newAdvisor;
}


void EU3History::output(FILE* output)
{
	fprintf(output, "		%d.%d.%d=\n", when.year, when.month, when.day);
	fprintf(output, "		{\n");
	if (monarch != NULL)
	{
		monarch->outputAsMonarch(output);
	}
	if (heir != NULL)
	{
		heir->outputAsHeir(output);
	}
	if (advisor != NULL)
	{
		advisor->outputInProvince(output);
	}
	fprintf(output, "		}\n");
}


date EU3History::getWhen()
{
	return when;
}


EU3Ruler* EU3History::getMonarch()
{
	return monarch;
}