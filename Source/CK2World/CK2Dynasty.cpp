#include "CK2Dynasty.h"
#include "CK2Character.h"
#include "..\Parsers\Object.h"



CK2Dynasty::CK2Dynasty(Object* obj)
{
	num = atoi( obj->getKey().c_str() );
	vector<Object*> nameLeaves = obj->getValue("name");
	if (nameLeaves.size() > 0)
	{
		name = nameLeaves[0]->getLeaf();
	}
	else
	{
		name = "";
	}
}


CK2Dynasty::CK2Dynasty(int newNum, string newName)
{
	num	= newNum;
	name	= newName;
}


void CK2Dynasty::addMember(CK2Character* newMember)
{
	members.push_back(newMember);
}


CK2Character* CK2Dynasty::getSenoirityHeir(string genderLaw)
{
	CK2Character* heir = NULL;
	date heirBirthDate = "1500.12.31";

	// unless absolute cognatic, consider only males
	for(unsigned int i = 0; i < members.size(); i++)
	{
		if ( (!members[i]->isDead()) && (members[i]->getBirthDate() < heirBirthDate) && (!members[i]->isBastard()) && (!members[i]->isFemale() || (genderLaw == "true_cognatic")) )
		{
			heir				= members[i];
			heirBirthDate	= heir->getBirthDate();
		}
	}

	if (heir != NULL)
	{
		return heir;
	}

	// no heirs in male lines, so consider females
	for(unsigned int i = 0; i < members.size(); i++)
	{
		if ( (!members[i]->isDead()) && (members[i]->getBirthDate() < heirBirthDate) && (!members[i]->isBastard()) )
		{
			heir				= members[i];
			heirBirthDate	= heir->getBirthDate();
		}
	}

	return heir;
}