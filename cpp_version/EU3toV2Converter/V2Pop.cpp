#include "V2Pop.h"


static int nextId = 23000;


V2Pop::V2Pop()
{
	id = nextId;
	nextId++;
}


void V2Pop::setType(string newType)
{
	type = newType;
}


void V2Pop::setSize(int newSize)
{
	size = newSize;
}


void V2Pop::setCulture(string newCulture)
{
	culture = newCulture;
}


void V2Pop::setReligion(string newReligion)
{
	religion = newReligion;
}


int V2Pop::getSize()
{
	return size;
}


string V2Pop::getType()
{
	return type;
}


int V2Pop::getID()
{
	return id;
}


void V2Pop::output(FILE* output)
{
	fprintf(output, "	%s=\n", type.c_str());
	fprintf(output, "	{\n");
	fprintf(output, "		id=%d\n", id);
	fprintf(output, "		size=%d\n", size);
	fprintf(output, "		%s=%s\n", culture.c_str(), religion.c_str());
	fprintf(output, "		money=1000000.00000\n");
	fprintf(output, "	}\n");
}