#include "EU3Province.h"


void EU3Province::init(int newNum, Object* obj)
{
	num = newNum;
	vector<Object*> ownerObj = obj[0].getValue("owner");
	if (ownerObj.size() > 0)
	{
		owner = ownerObj[0]->getLeaf();
	}
	else
	{
		owner = "";
	}
}


void EU3Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	if (owner != "")
	{
		fprintf(output, "	owner=\"%s\"\n", owner.c_str());
	}
	fprintf(output, "}\n");
}