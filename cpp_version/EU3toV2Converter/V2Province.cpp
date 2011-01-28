#include "V2Province.h"


void V2Province::init(Object* obj) {
	num = atoi(obj->getKey().c_str());

	vector<Object*> nameObj;
	nameObj = obj->getValue("name");
	if (nameObj.size() == 0)
	{
		name = "";
	}
	else
	{
		name = nameObj[0]->getLeaf();
	}
}


int V2Province::getNum()
{
	return num;
}


void V2Province::setOwner(string newOwner)
{
	owner = newOwner;
}


string V2Province::getOwner()
{
	return owner;
}


void V2Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	fprintf(output, "	name=%s\n", name.c_str());
	fprintf(output, "	owner=\"%s\"\n", owner.c_str());
	fprintf(output, "	controller=\"%s\"\n", owner.c_str());
	//output pops here
	outputProvStuff(output);
	fprintf(output, "}\n");
}