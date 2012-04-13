#include "EU3Ruler.h"
#include "..\Configuration.h"
#include "..\Log.h"
#include <fstream>



EU3Ruler::EU3Ruler(string newName, int dip, int adm, int mil, string newDynasty)
{
	name				= newName;
	diplomacy		= dip;
	administration	= adm;
	military			= mil;
	id					= Configuration::getID();
	dynasty			= newDynasty;
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
	int bonus		= ( stats[INTRIGUE] + stats[LEARNING] ) / (3 * 3);
	diplomacy		= stats[DIPLOMACY] / 3		+ bonus;
	administration	= stats[STEWARDSHIP] / 3 	+ bonus;
	military			= stats[MARTIAL] /3			+ bonus;
	
	int leftover	=	( stats[INTRIGUE] + stats[LEARNING] ) % (3 * 3);
	leftover			+= stats[DIPLOMACY] % 3;
	leftover			+= stats[STEWARDSHIP] % 3;
	leftover			+= stats[MARTIAL] % 3;

	diplomacy		+= leftover / (3 * 3);
	administration	+= leftover / (3 * 3);
	military			+= leftover / (3 * 3);
	leftover			%= (3 * 3);

	if ( (diplomacy >= administration) && (diplomacy >= military) )
	{
		diplomacy += leftover / 3;
	}
	else if (administration >= military)
	{
		administration += leftover / 3;
	}
	else
	{
		military += leftover / 3;
	}
}


void EU3Ruler::outputAsMonarch(FILE* output)
{
	fprintf(output,"			monarch=\n");
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
	fprintf(output,"				monarch_name=\"%s\"\n", monarchName.c_str());
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