#include "EU3History.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2History.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"



EU3History::EU3History()
{
	monarch	= NULL;
	regent	= NULL;
	heir		= NULL;
	advisor	= NULL;
}


void EU3History::init(CK2History* src)
{
	when = src->getWhen();

	monarch	= NULL;
	regent	= NULL;
	heir		= NULL;
	advisor	= NULL;

	CK2Character* holder = src->getHolder();
	if (holder != NULL)
	{
		CK2Character* CK2Regent = holder->getRegent();
		if (CK2Regent != NULL)
		{
			regent	= new EU3Ruler(CK2Regent);
			heir		= new EU3Ruler(holder);
		}
		else
		{
			monarch = new EU3Ruler(holder);
		}
	}
}


void EU3History::initMonarch(EU3Ruler* newMonarch, date newWhen)
{
	when = newWhen;
	
	monarch	= newMonarch;
	regent	= NULL;
	heir		= NULL;
	advisor	= NULL;
}


void EU3History::initRegent(EU3Ruler* newRegent, date newWhen)
{
	when = newWhen;
	
	monarch	= NULL;
	regent	= newRegent;
	heir		= NULL;
	advisor	= NULL;
}


void EU3History::initHeir(EU3Ruler* newHeir, date newWhen)
{
	when = newWhen;

	monarch	= NULL;
	regent	= NULL;
	heir		= newHeir;
	advisor	= NULL;
}


void EU3History::initAdvisor(EU3Advisor* newAdvisor)
{
	when = newAdvisor->getDate();

	monarch	= NULL;
	regent	= NULL;
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
	if (regent != NULL)
	{
		regent->outputAsRegent(output);
		fprintf(output, "		}\n");
		fprintf(output, "		%d.%d.%d=\n", when.year, when.month, when.day);
		fprintf(output, "		{\n");
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


EU3Ruler* EU3History::getRegent()
{
	return regent;
}


EU3Ruler* EU3History::getHeir()
{
	return heir;
}