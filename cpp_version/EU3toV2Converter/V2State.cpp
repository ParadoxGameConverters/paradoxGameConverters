#include "V2State.h"
#include "tempFuncs.h"


V2State::V2State(int newId)
{
	id = newId;
}


void V2State::addProvince(V2Province newProvince)
{
	provinces.push_back(newProvince);
}


void V2State::setColonial(bool isIt)
{
	colonial = isIt;
}


void V2State::output(FILE* output)
{
	fprintf(output, "	state=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		id=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			id=%d\n", id);
	fprintf(output, "			type=47\n");
	fprintf(output, "		}\n");
	fprintf(output, "		provinces=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			");
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		fprintf(output, "%d ", provinces[i].getNum());
	}
	fprintf(output, "\n");
	fprintf(output, "		}\n");
	if (colonial)
	{
		fprintf(output, "		is_colonial=yes\n");
	}
	fprintf(output, "	}\n");
}