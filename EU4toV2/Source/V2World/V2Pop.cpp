#include "V2Pop.h"
#include "../Log.h"



static int nextId = 23000;


V2Pop::V2Pop(string _type, int _size, string _culture, string _religion)
{
	id = nextId;
	nextId++;

	type						= _type;
	size						= _size;
	culture					= _culture;
	religion					= _religion;
	supportedRegiments	= 0;
	money						= 0.0;
	consciousness			= 0.0;
	militancy				= 0.0;

	recalcMoney();
}


void V2Pop::output(FILE* output) const
{
	fprintf(output, "\t%s=\n", type.c_str());
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tid=%d\n", id);
	fprintf(output, "\t\tsize=%d\n", size);
	fprintf(output, "\t\t%s=%s\n", culture.c_str(), religion.c_str());
	fprintf(output, "\t\tmoney=%f\n", money);
	fprintf(output, "\t\tcon=%f\n", consciousness);
	fprintf(output, "\t\tmil=%f\n", militancy);
	fprintf(output, "\t}\n");
}


bool V2Pop::combine(const V2Pop& rhs)
{
	if ((culture == rhs.culture) && (religion == rhs.religion) && (type == rhs.type))
	{
		size += rhs.size;
		recalcMoney();
		return true;
	}
	else
	{
		return false;
	}
}


void V2Pop::recalcMoney()
{
	money = 1000000.0;
	
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


int getNextPopId()
{
	return nextId;
}
