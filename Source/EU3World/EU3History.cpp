#include "EU3History.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2History.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"



EU3History::EU3History()
{
	when = date();
	monarch	= NULL;
	regent	= NULL;
	heir		= NULL;
	advisor	= NULL;
}


EU3History::EU3History(CK2History* src)
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


EU3History::EU3History(date _when, EU3Ruler* _monarch, EU3Ruler* _regent, EU3Ruler* _heir, EU3Advisor* _advisor)
{
	when		= _when;
	monarch	= _monarch;
	regent	= _regent;
	heir		= _heir;
	advisor	= _advisor;
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