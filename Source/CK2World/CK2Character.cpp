#include "CK2Character.h"
#include "..\log.h"
#include "..\Configuration.h"
#include "..\Parsers\Object.h"
#include "CK2Dynasty.h"
#include "CK2Trait.h"
#include "CK2Barony.h"
#include "CK2Province.h"



CK2Character::CK2Character()
{
	num			= -1;
	name			= "";
	religion		= "";
	culture		= "";
	dynasty		= NULL;
	birthDate	= (string)"1.1.1";
	dead			= true;
	deathDate	= (string)"1.1.1";
	female		= false;
	bastard		= false;
	traits.clear();
	memset(stats, 0, sizeof(stats));

	fatherNum	= -1;
	father		= NULL;
	motherNum	= -1;
	mother		= NULL;
	children.clear();
	guardianNum	= -1;
	guardian		= NULL;

	memset(advisors, NULL, sizeof(advisors));
	employerNum	= -1;
	jobType		= NONE;
	hostNum		= -1;
	locationNum	= -1;
}


void CK2Character::init(Object* obj, map<int, CK2Dynasty*>& dynasties, map<int, CK2Trait*>& traitTypes, date theDate)
{
	num			= atoi( obj->getKey().c_str() );
	name			= obj->getLeaf("birth_name");
	religion		= obj->getLeaf("religion");
	culture		= obj->getLeaf("culture");
	dynasty		= dynasties[ atoi( obj->getLeaf("dynasty").c_str() ) ];
	if (dynasty != NULL)
	{
		dynasty->addMember(this);
	}
	birthDate	= obj->getLeaf("birth_date");
	age			= theDate.diffInYears(birthDate);

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
	vector<Object*> guardianObj = obj->getValue("guardian");
	if (guardianObj.size() > 0)
	{
		guardianNum = atoi( guardianObj[0]->getLeaf().c_str() );
	}
	else
	{
		guardianNum = -1;
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

	vector<Object*> bastardObj = obj->getValue("is_bastard");
	if (bastardObj.size() > 0)
	{
		bastard = ( bastardObj[0]->getLeaf() == "yes" );
	}
	else
	{
		bastard = false;
	}

	vector<Object*> employerObj = obj->getValue("employer");
	if (employerObj.size() > 0)
	{
		employerNum = atoi( employerObj[0]->getLeaf().c_str() );
	}

	vector<Object*> hostObj = obj->getValue("host");
	if (hostObj.size() > 0)
	{
		hostNum = atoi( hostObj[0]->getLeaf().c_str() );
	}

	if (Configuration::getAdvisorsType() == "DasGuntLord01")
	{
		vector<Object*> jobObj = obj->getValue("job_title");
		if (jobObj.size() > 0)
		{
			string jobTitle = jobObj[0]->getLeaf();
			if (jobTitle == "job_chancellor")
			{
				jobType = CHANCELLOR;
			}
			else if (jobTitle == "job_marshal")
			{
				jobType = MARSHAL;
			}
			else if (jobTitle == "job_treasurer")
			{
				jobType = STEWARD;
			}
			else if (jobTitle == "job_spymaster")
			{
				jobType = SPYMASTER;
			}
			else if (jobTitle == "job_spiritual")
			{
				jobType = CHAPLAIN;
			}
		}
	}

	vector<Object*> attributesObj = obj->getValue("attributes");
	if (attributesObj.size() > 0)
	{
		vector<string> attributeTokens = attributesObj[0]->getTokens();
		stats[DIPLOMACY]		= atoi( attributeTokens[0].c_str() );
		stats[MARTIAL]			= atoi( attributeTokens[1].c_str() );
		stats[STEWARDSHIP]	= atoi( attributeTokens[2].c_str() );
		stats[INTRIGUE]		= atoi( attributeTokens[3].c_str() );
		stats[LEARNING]		= atoi( attributeTokens[4].c_str() );
	}
	else
	{
		stats[DIPLOMACY]		= 0;
		stats[MARTIAL]			= 0;
		stats[STEWARDSHIP]	= 0;
		stats[INTRIGUE]		= 0;
		stats[LEARNING]		= 0;
	}

	vector<Object*> traitsObj = obj->getValue("traits");
	if (traitsObj.size() > 0)
	{
		vector<string> traitsStrings = traitsObj[0]->getTokens();
		for (unsigned int i = 0; i < traitsStrings.size(); i++)
		{
			traits.push_back( atoi(traitsStrings[i].c_str()) );
		}
	}

	for (unsigned int i = 0; i < traits.size(); i++)
	{
		CK2Trait* currentTrait = traitTypes[ traits[i] ];
		if (currentTrait == NULL)
		{
			if (dynasty != NULL)
				log("Error: %s %s had extra trait %d\n", name.c_str(), dynasty->getName().c_str(), traits[i]);
			else
				log("Error: %s had extra trait %d\n", name.c_str(), traits[i]);
		}
		else
		{
			stats[DIPLOMACY]		+= currentTrait->diplomacy;
			stats[MARTIAL]			+= currentTrait->martial;
			stats[STEWARDSHIP]	+= currentTrait->stewardship;
			stats[INTRIGUE]		+= currentTrait->intrigue;
			stats[LEARNING]		+= currentTrait->learning;
		}
	}

	for (unsigned int i = 0; i < 5; i++)
	{
		if (stats[i] < 0)
		{
			stats[i] = 0;
		}
	}


	vector<Object*> demesneObj = obj->getValue("demesne");
	if (demesneObj.size() > 0)
	{
		vector<Object*> capitalObj = demesneObj[0]->getValue("capital");
		if (capitalObj.size() > 0)
		{
			capitalString = capitalObj[0]->getLeaf();
		}
	}
}


int CK2Character::getNum()
{
	return num;
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

	if (guardianNum != -1)
	{
		guardian = characters[guardianNum];
		if (age < 16)
		{
			int* guardianStats = guardian->getStats();
			for (unsigned int i = 0; i < 5; i++)
			{
				stats[i] += int( ((16 - age) / 16)  * guardianStats[i] );
			}
		}
	}
	else
	{
		if ( (age < 16) && (father != NULL) )
		{
			int* guardianStats = father->getStats();
			for (unsigned int i = 0; i < 5; i++)
			{
				stats[i] += int( ((16 - age) / 16)  * guardianStats[i] );
			}
		}
		else if ( (age < 16) && (mother != NULL) )
		{
			int* guardianStats = mother->getStats();
			for (unsigned int i = 0; i < 5; i++)
			{
				stats[i] += int( ((16 - age) / 16)  * guardianStats[i] );
			}
		}
	}
}


void CK2Character::setEmployer(map<int, CK2Character*>& characters, map<string, CK2Barony*>& baronies)
{
	if (Configuration::getAdvisorsType() == "DasGuntLord01")
	{
		if ( (hostNum != -1) && (jobType != NONE) )
		{
			CK2Character* employer = characters[hostNum];
			if (employer != NULL)
			{
				characters[hostNum]->addAdvisor(this, jobType);
			}
			else
			{
				log("%s %s has an invalid employer. (%d)\n", name.c_str(), dynasty->getName().c_str(), hostNum);
			}
		}
	}

	if (hostNum != -1)
	{
		CK2Character* host = characters[employerNum];
		if (host != NULL)
		{
			string hostCapitalString = host->getCapitalString();
			CK2Barony* homeBarony = baronies[hostCapitalString];
			if (homeBarony != NULL)
			{
				locationNum = homeBarony->getProvince()->getNumber();
			}
		}
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


void CK2Character::addAdvisor(CK2Character* newAdvisor, advisorTypes type)
{
	advisors[type] = newAdvisor;
}


CK2Character** CK2Character::getAdvisors()
{
	return advisors;
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


bool CK2Character::isBastard()
{
	return bastard;
}


int* CK2Character::getStats()
{
	return stats;
}


CK2Character* CK2Character::getFather()
{
	return father;
}


CK2Character* CK2Character::getPrimogenitureHeir(string genderLaw)
{
	// unless absolute cognatic, consider only males
	for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
	{
		if ( (*i)->isBastard() )
		{
			continue;
		}
		if (	( !(*i)->isDead() ) &&
				( !(*i)->isFemale() || (genderLaw == "true_cognatic") )
			)
		{
			return *i;
		}
		else
		{
			if ( !(*i)->isFemale() || (genderLaw == "true_cognatic") )
			{
				CK2Character* possibleHeir = (*i)->getPrimogenitureHeir(genderLaw);
				if (possibleHeir != NULL)
				{
					return possibleHeir;
				}
			}
		}
	}

	// no heirs in male lines, so consider females
	for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
	{
		if ( !(*i)->isFemale() )
		{
			continue;
		}
		if ( (*i)->isBastard() )
		{
			continue;
		}
		if (	( !(*i)->isDead() ) &&
				( !(*i)->isFemale() || (genderLaw == "cognatic") )
			)
		{
			return *i;
		}
		else
		{
			CK2Character* possibleHeir = (*i)->getPrimogenitureHeir(genderLaw);
			if (possibleHeir != NULL)
			{
				return possibleHeir;
			}
		}
	}

	return NULL;
}


advisorTypes CK2Character::getJobType()
{
	return jobType;
}


int CK2Character::getLocationNum()
{
	return locationNum;
}


string CK2Character::getCapitalString()
{
	return capitalString;
}