#include "CK2Character.h"



CK2Character::CK2Character()
{
	name			= "";
	religion		= "";
	culture		= "";
	dynasty		= NULL;
	birthDate	= (string)"1.1.1";
	dead			= true;
	deathDate	= (string)"1.1.1";
	fatherNum	= -1;
	father		= NULL;
	motherNum	= -1;
	mother		= NULL;
	children.clear();
	female		= false;
}


void CK2Character::init(Object* obj, map<int, CK2Dynasty*>& dynasties)
{
	name			= obj->getLeaf("birth_name");
	religion		= obj->getLeaf("religion");
	culture		= obj->getLeaf("culture");
	dynasty		= dynasties[ atoi( obj->getLeaf("dynasty").c_str() ) ];
	birthDate	= obj->getLeaf("birth_date");

	vector<Object*> fatherObj = obj->getValue("father");
	if (fatherObj.size() > 0)
	{
		fatherNum = atoi( fatherObj[0]->getLeaf().c_str() );
	}
	else
	{
		fatherNum = -1;
	}
	vector<Object*> motherObj = obj->getValue("mother");
	if (motherObj.size() > 0)
	{
		motherNum = atoi( motherObj[0]->getLeaf().c_str() );
	}
	else
	{
		motherNum = -1;
	}

	vector<Object*> deathObj = obj->getValue("death_date");
	if (deathObj.size() > 0)
	{
		dead			= true;
		deathDate	= deathObj[0]->getLeaf();
	}
	else
	{
		dead			= false;
		deathDate	= (string)"1.1.1";
	}

	vector<Object*> femaleObj = obj->getValue("female");
	if (femaleObj.size() > 0)
	{
		female = ( femaleObj[0]->getLeaf() == "yes" );
	}
	else
	{
		female = false;
	}
}


string CK2Character::getName()
{
	return name;
}


CK2Dynasty* CK2Character::getDynasty()
{
	return dynasty;
}


date CK2Character::getBirthDate()
{
	return birthDate;
}


void CK2Character::setParents(map<int, CK2Character*>& characters)
{
	if (fatherNum != -1)
	{
		father = characters[fatherNum];
		father->addChild(this);
	}

	if (motherNum != -1)
	{
		mother = characters[motherNum];
		mother->addChild(this);
	}
}


void CK2Character::addChild(CK2Character* newChild)
{
	bool inserted = false;
	for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
	{
		if ((*i)->getBirthDate() > newChild->getBirthDate())
		{
			children.insert(i, newChild);
			inserted = true;
		}
	}
	if (!inserted)
	{
		children.push_back(newChild);
	}
}


bool CK2Character::isDead()
{
	return dead;
}


date CK2Character::getDeathDate()
{
	return deathDate;
}


bool CK2Character::isFemale()
{
	return female;
}


CK2Character* CK2Character::getFather()
{
	return father;
}


CK2Character* CK2Character::getPrimogenitureHeir()
{
	for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
	{
		if ( !(*i)->isDead() )
		{
			return *i;
		}
		else
		{
			CK2Character* possibleHeir = (*i)->getPrimogenitureHeir();
			if (possibleHeir != NULL)
			{
				return possibleHeir;
			}
		}
	}

	return NULL;
}