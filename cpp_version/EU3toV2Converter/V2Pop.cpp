#include "V2Pop.h"


static int nextId = 23000;


V2Pop::V2Pop()
{
	supportedRegiments = 0;

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


int V2Pop::getSize() const
{
	return size;
}


string V2Pop::getType() const
{
	return type;
}


int V2Pop::getID() const
{
	return id;
}


string V2Pop::getCulture() const
{
	return culture;
}


string V2Pop::getReligion() const
{
	return religion;
}


int V2Pop::getSupportedRegimentCount() const
{
	return supportedRegiments;
}


void V2Pop::setSupportedRegimentCount(int regiments)
{
	supportedRegiments = regiments;
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


int getNextPopId()
{
	return nextId;
}


bool V2Pop::canCombine(const V2Pop& rhs)
{
	return ((culture == rhs.getCulture()) && (religion == rhs.getReligion()) && (type == rhs.getType()));
}
