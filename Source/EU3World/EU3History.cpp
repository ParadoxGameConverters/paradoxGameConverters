#include "EU3History.h"
#include "..\CK2World\CK2Character.h"


void EU3History::init(CK2History* src)
{
	when = src->getWhen();

	monarch = NULL;
	CK2Character* holder = src->getHolder();
	if (holder != NULL)
	{
		monarch = new EU3Ruler(holder);
	}

	heir = NULL;
}


void EU3History::initHeir(EU3Ruler* newHeir)
{
	when = newHeir->getBirthDate();

	monarch = NULL;
	heir = newHeir;
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
	fprintf(output, "		}\n");
}


EU3Ruler* EU3History::getMonarch()
{
	return monarch;
}