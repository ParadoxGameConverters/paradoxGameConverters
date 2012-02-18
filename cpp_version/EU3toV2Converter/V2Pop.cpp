#include "V2Pop.h"
#include "Log.h"


static int nextId = 23000;


V2Pop::V2Pop()
{
	supportedRegiments = 0;
	size = 0;
	consciousness = 0.0;
	militancy = 0.0;
	literacy	= 10.0;

	id = nextId;
	nextId++;
}


void V2Pop::setType(string newType)
{
	type = newType;
	recalcMoney();
}


void V2Pop::setSize(int newSize)
{
	size = newSize;
	recalcMoney();
}


void V2Pop::setCulture(string newCulture)
{
	culture = newCulture;
}


void V2Pop::setReligion(string newReligion)
{
	religion = newReligion;
}


void V2Pop::setConsciousness(double con)
{
	consciousness = con;
}


void V2Pop::setMilitancy(double mil)
{
	militancy = mil;
}


void V2Pop::setLiteracy(double newLiteracy)
{
	literacy = newLiteracy;
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
	fprintf(output, "		money=%f\n", money);
	fprintf(output, "		con=%f\n", consciousness);
	fprintf(output, "		mil=%f\n", militancy);
	fprintf(output, "		literacy=%f\n", literacy);
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


void V2Pop::recalcMoney()
{
	money = 1000000.0;
	
	if (type == "" || size == 0)
		return;

	if (type == "aristocrats" || type == "capitalists")
	{
		money = 100.0 * size;
	}
	else if (type == "artisans" || type == "bureaucrats" || type == "clergymen" 
		  || type == "clerks" || type == "craftsmen" || type == "officers")
	{
		money = 10.0 * size;
	}
	else if (type == "farmers" || type == "labourers" || type == "soldiers")
	{
		money = 1.0 * size;
	}
	else if (type == "slaves")
	{
		money = 0.1 * size;
	}
	else
	{
		log("Error: Unexpected pop type %s!\n", type.c_str());
	}
}
