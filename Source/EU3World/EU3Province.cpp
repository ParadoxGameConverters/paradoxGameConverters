#include "EU3Province.h"


void EU3Province::init(int newNum, Object* obj)
{
	num = newNum;
	owner = obj[0].getLeaf("owner");
}


void EU3Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	fprintf(output, "	owner=\"%s\"\n", owner.c_str());
	fprintf(output, "}\n");
}