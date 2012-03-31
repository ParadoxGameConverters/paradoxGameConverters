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
	}
	else
	{
		diplomacy = 1;
	}

	vector<Object*> admObjs = obj->getValue("ADM");
	if (admObjs.size() > 0)
	{
		administration = atoi( admObjs[0]->getLeaf().c_str() );
	}
	else
	{
		administration = 1;
	}

	vector<Object*> milObjs = obj->getValue("MIL");
	if (milObjs.size() > 0)
	{
		military = atoi( milObjs[0]->getLeaf().c_str() );
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
}


void EU3Ruler::output(FILE* output)
{
	fprintf(output,"			{\n");
	fprintf(output,"				name=\"%s\"\n", name.c_str());
	fprintf(output,"				DIP=%d\n", diplomacy);
	fprintf(output,"				ADM=%d\n", administration);
	fprintf(output,"				MIL=%d\n", military);
	fprintf(output,"				id=\n");
	fprintf(output,"				{\n");
	fprintf(output,"					id=%d\n", id);
	fprintf(output,"					type=37\n");
	fprintf(output,"				}\n");
	fprintf(output,"				dynasty=\"%s\"\n", dynasty.c_str());
	fprintf(output,"				birth_date=\"%d.%d.%d\"\n", birthDate.year, birthDate.month, birthDate.day);
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