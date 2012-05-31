#include "EU3Ruler.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2Dynasty.h"
#include "..\Configuration.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include <fstream>



EU3Ruler::EU3Ruler()
{
	name				= "";
	regnalNum		= -1;
	diplomacy		= -1;
	administration	= -1;
	military			= -1;
	id					= -1;
	dynasty			= "";
	birthDate		= (date)"1.1.1";
	deathDate		= (date)"1.1.1";
	claim				= -1;
	monarchName		= "";
	female			= false;
}


EU3Ruler::EU3Ruler(Object* obj)
{
	vector<Object*> nameObjs = obj->getValue("name");
	if (nameObjs.size() > 0)
	{
		name = nameObjs[0]->getLeaf().c_str();
	}
	else
	{
		name = "nameless";
	}
	regnalNum = -1;

	vector<Object*> dipObjs = obj->getValue("DIP");
	if (dipObjs.size() > 0)
	{
		diplomacy = atoi( dipObjs[0]->getLeaf().c_str() );
		if (diplomacy < 1)
		{
			diplomacy = 1;
		}
	}
	else
	{
		diplomacy = 1;
	}

	vector<Object*> admObjs = obj->getValue("ADM");
	if (admObjs.size() > 0)
	{
		administration = atoi( admObjs[0]->getLeaf().c_str() );
		if (administration < 1)
		{
			administration = 1;
		}
	}
	else
	{
		administration = 1;
	}

	vector<Object*> milObjs = obj->getValue("MIL");
	if (milObjs.size() > 0)
	{
		military = atoi( milObjs[0]->getLeaf().c_str() );
		if (military < 1)
		{
			military = 1;
		}
	}
	else
	{
		military = 1;
	}

	id = Configuration::getID();

	vector<Object*> dynastyObjs = obj->getValue("dynasty");
	if (dynastyObjs.size() > 0)
	{
		dynasty = dynastyObjs[0]->getLeaf();
	}
	else
	{
		dynasty = "blank";
	}

	vector<Object*> birthdateObjs = obj->getValue("birth_Date");
	if (birthdateObjs.size() > 0)
	{
		birthDate = birthdateObjs[0]->getLeaf();
	}
	else
	{
		birthDate = (string)"1.1.1";
	}

	vector<Object*> deathdateObjs = obj->getValue("death_Date");
	if (deathdateObjs.size() > 0)
	{
		deathDate = deathdateObjs[0]->getLeaf();
	}
	else
	{
		deathDate = (string)"1.1.1";
	}

	vector<Object*> claimObjs = obj->getValue("claim");
	if (claimObjs.size() > 0)
	{
		claim = atoi( claimObjs[0]->getLeaf().c_str() );
	}
	else
	{
		claim = 100;
	}

	vector<Object*> monarchNameObjs = obj->getValue("monarch_name");
	if (monarchNameObjs.size() > 0)
	{
		monarchName = monarchNameObjs[0]->getLeaf();
	}
	else
	{
		monarchName = "";
	}

	vector<Object*> femaleObjs = obj->getValue("female");
	if (femaleObjs.size() > 0)
	{
		female = (femaleObjs[0]->getLeaf() == "yes");
	}
	else
	{
		female = false;
	}
}


EU3Ruler::EU3Ruler(CK2Character* src)
{
	name				= "";
	regnalNum		= 1;
	diplomacy		= 1;
	administration	= 1;
	military			= 1;
	id					= Configuration::getID();
	dynasty			= "blank";
	birthDate		= src->getBirthDate();
	deathDate		= src->getDeathDate();
	claim				= 100;
	monarchName		= "";
	female			= src->isFemale();

	name = src->getName();
	CK2Dynasty* dynPointer = src->getDynasty();
	if (dynPointer != NULL)
	{
		dynasty = dynPointer->getName();
	}
	else
	{
		log("Error: %s does not have a dynasty!\n", name.c_str());
	}

	int* stats = src->getStats();
	int bonus		= ( stats[INTRIGUE] + stats[LEARNING] ) / (6 * 3);

	diplomacy		=	3;
	administration	=	3;
	military			=	3;

	diplomacy		+= stats[DIPLOMACY]		/ 6	+ bonus;
	administration	+= stats[STEWARDSHIP]	/ 6	+ bonus;
	military			+= stats[MARTIAL]			/ 6	+ bonus;
	
	int leftover	=	( stats[INTRIGUE] + stats[LEARNING] ) % (6 * 3);
	leftover			+= stats[DIPLOMACY]		% 6;
	leftover			+= stats[STEWARDSHIP]	% 6;
	leftover			+= stats[MARTIAL]			% 6;

	diplomacy		+= leftover / (6 * 3);
	administration	+= leftover / (6 * 3);
	military			+= leftover / (6 * 3);
	leftover			%= (6 * 3);

	if ( (diplomacy <= administration) && (diplomacy <= military) )
	{
		diplomacy += leftover / 6;
	}
	else if (administration <= military)
	{
		administration += leftover / 6;
	}
	else
	{
		military += leftover / 6;
	}

	if (diplomacy < 3)
	{
		int* stats = src->getStats();
		log("%s had a lower diplomacy than 3 (%d) %d %d %d %d %d.\n", name.c_str(), diplomacy, stats[0], stats[1], stats[2], stats[3], stats[4]);
		diplomacy = 3;
	}
	if (military < 3)
	{
		int* stats = src->getStats();
		log("%s had a lower military than 3 (%d) %d %d %d %d %d.\n", name.c_str(), military, stats[0], stats[1], stats[2], stats[3], stats[4]);
		military = 3;
	}
	if (administration < 3)
	{
		int* stats = src->getStats();
		log("%s had a lower administration than 3 (%d) %d %d %d %d %d.\n", name.c_str(), administration, stats[0], stats[1], stats[2], stats[3], stats[4]);
		administration = 3;
	}

	if (diplomacy > 9)
	{
		int* stats = src->getStats();
		log("%s had a higher diplomacy than 9 (%d) %d %d %d %d %d.\n", name.c_str(), diplomacy, stats[0], stats[1], stats[2], stats[3], stats[4]);
		diplomacy = 9;
	}
	if (military > 9)
	{
		int* stats = src->getStats();
		log("%s had a higher military than 9. (%d) %d %d %d %d %d.\n", name.c_str(), military, stats[0], stats[1], stats[2], stats[3], stats[4]);
		military = 9;
	}
	if (administration > 9)
	{
		int* stats = src->getStats();
		log("%s had a higher administration than 9 (%d) %d %d %d %d %d.\n", name.c_str(), administration, stats[0], stats[1], stats[2], stats[3], stats[4]);
		administration = 9;
	}
}


string EU3Ruler::getName()
{
	return name;
}


void EU3Ruler::setRegnalNum(int newNum)
{
	regnalNum = newNum;
}


int EU3Ruler::getRegnalNum()
{
	return regnalNum;
}


void EU3Ruler::outputAsMonarch(FILE* output)
{
	fprintf(output,"			monarch=\n");
	fprintf(output,"			{\n");
	fprintf(output,"				name=\"%s ", name.c_str());
	outputRegnalNum(output);
	fprintf(output, "\"\n");
	fprintf(output,"				DIP=%d\n", diplomacy);
	fprintf(output,"				ADM=%d\n", administration);
	fprintf(output,"				MIL=%d\n", military);
	if (female)
	{
		fprintf(output, "				female=yes\n");
	}
	fprintf(output,"				id=\n");
	fprintf(output,"				{\n");
	fprintf(output,"					id=%d\n", id);
	fprintf(output,"					type=37\n");
	fprintf(output,"				}\n");
	fprintf(output,"				dynasty=\"%s\"\n", dynasty.c_str());
	fprintf(output,"			}\n");
}


void EU3Ruler::outputAsHeir(FILE* output)
{
	fprintf(output,"			heir=\n");
	fprintf(output,"			{\n");
	fprintf(output,"				name=\"%s\"\n", name.c_str());
	fprintf(output,"				DIP=%d\n", diplomacy);
	fprintf(output,"				ADM=%d\n", administration);
	fprintf(output,"				MIL=%d\n", military);
	if (female)
	{
		fprintf(output, "				female=yes\n");
	}
	fprintf(output,"				id=\n");
	fprintf(output,"				{\n");
	fprintf(output,"					id=%d\n", id);
	fprintf(output,"					type=37\n");
	fprintf(output,"				}\n");
	fprintf(output,"				dynasty=\"%s\"\n", dynasty.c_str());
	fprintf(output,"				birth_date=\"%d.%d.%d\"\n", birthDate.year, birthDate.month, birthDate.day);
	fprintf(output,"				death_date=\"%d.%d.%d\"\n", deathDate.year, deathDate.month, deathDate.day);
	fprintf(output,"				claim=%d\n", claim);
	if (monarchName != "")
	{
		fprintf(output,"				monarch_name=\"%s\"\n", monarchName.c_str());
	}
	else
	{
		fprintf(output, "				monarch_name=\"%s ", name.c_str());
		outputRegnalNum(output);
		fprintf(output, "\"\n");
	}
	fprintf(output,"			}\n");
}


int EU3Ruler::getID()
{
	return id;
}


date EU3Ruler::getBirthDate()
{
	return birthDate;
}


void EU3Ruler::outputRegnalNum(FILE* output)
{
	if (regnalNum != -1)
	{
		// algorithm adapted from http://www.blackwasp.co.uk/NumberToRoman.aspx
		// Set up key numerals and numeral pairs
		int		values[13]		= { 1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1 };
		string	numerals[13]	= { "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
		
		// Loop through each of the values to diminish the number
		int number = regnalNum;
		for (int i = 0; i < 13; i++)
		{
			// If the number being converted is less than the test value, append
			// the corresponding numeral or numeral pair to the resultant string
			while (number >= values[i])
			{
				number -= values[i];
				fprintf(output, "%s", numerals[i].c_str());
			}
		}
	}
}