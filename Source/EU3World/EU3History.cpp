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
}


void EU3History::output(FILE* output)
{
	fprintf(output, "		%d.%d.%d=\n", when.year, when.month, when.day);
	fprintf(output, "		{\n");
	if (monarch != NULL)
	{
		monarch->output(output);
	}
	fprintf(output, "		}\n");
}


EU3Ruler* EU3History::getMonarch()
{
	return monarch;
}