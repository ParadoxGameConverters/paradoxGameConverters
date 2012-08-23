#include "CK2Character.h"
#include "..\log.h"
#include "..\Configuration.h"
#include "..\Parsers\Object.h"
#include "CK2Dynasty.h"
#include "CK2Trait.h"
#include "CK2Barony.h"
#include "CK2Province.h"
#include "CK2Title.h"



CK2Character::CK2Character(Object* obj, map<int, CK2Dynasty*>& dynasties, map<int, CK2Trait*>& traitTypes, date theDate)
{
	num			= atoi( obj->getKey().c_str() );
	name			= obj->getLeaf("birth_name");
	religion		= obj->getLeaf("religion");
	culture		= obj->getLeaf("culture");

	dynasty		= NULL;
	map<int, CK2Dynasty*>::iterator dynItr	= dynasties.find(  atoi( obj->getLeaf("dynasty").c_str() )  );
	if (dynItr != dynasties.end())
	{
		dynasty = dynItr->second;
		dynasty->addMember(this);
	}
	else
	{
		log("		Error: no dynasty for character %d (%s).\n", num, name.c_str());
	}
	birthDate	= obj->getLeaf("birth_date");
	age			= theDate.diffInYears(birthDate);
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
	titles.clear();

	vector<Object*> fatherObj = obj->getValue("father");
	if (fatherObj.size() > 0)
	{
		fatherNum = atoi( fatherObj[0]->getLeaf().c_str() );
	}
	else
	{
		fatherNum = -1;
	}
	father = NULL;
	vector<Object*> motherObj = obj->getValue("mother");
	if (motherObj.size() > 0)
	{
		motherNum = atoi( motherObj[0]->getLeaf().c_str() );
	}
	else
	{
		motherNum = -1;
	}
	mother = NULL;
	children.clear();
	vector<Object*> guardianObj = obj->getValue("guardian");
	if (guardianObj.size() > 0)
	{
		guardianNum = atoi( guardianObj[0]->getLeaf().c_str() );
	}
	else
	{
		guardianNum = -1;
	}
	guardian = NULL;
	vector<Object*> regentObj = obj->getValue("regent");
	if (regentObj.size() > 0)
	{
		regentNum = atoi( regentObj[0]->getLeaf().c_str() );
	}
	else
	{
		regentNum = -1;
	}
	regent = NULL;

	memset(advisors, 0, sizeof(advisors));
	vector<Object*> employerObj = obj->getValue("employer");
	if (employerObj.size() > 0)
	{
		employerNum = atoi( employerObj[0]->getLeaf().c_str() );
	}
	else
	{
		employerNum = -1;
	}
	jobType = NONE;
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
	vector<Object*> actionObj = obj->getValue("action");
	if (actionObj.size() > 0)
	{
		action = actionObj[0]->getLeaf();
	}
	else
	{
		action = "";
	}
	vector<Object*> hostObj = obj->getValue("host");
	if (hostObj.size() > 0)
	{
		hostNum = atoi( hostObj[0]->getLeaf().c_str() );
	}
	else
	{
		hostNum = -1;
	}
	locationNum				= -1;
	capitalString			= "";
	vector<Object*> demesneObj = obj->getValue("demesne");
	if (demesneObj.size() > 0)
	{
		vector<Object*> capitalObj = demesneObj[0]->getValue("capital");
		if (capitalObj.size() > 0)
		{
			capitalString = capitalObj[0]->getLeaf();
		}
	}
	capital = NULL;

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
		log("		Error: no attributes for character %d (%s).\n", num, name.c_str());
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
}


void CK2Character::addTitle(CK2Title* newTitle)
{
	titles.push_back(newTitle);
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

	if (regentNum != -1)
	{
		regent = characters[regentNum];
	}
}


void CK2Character::setEmployer(map<int, CK2Character*>& characters, map<string, CK2Barony*>& baronies)
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


CK2Character* CK2Character::getPrimogenitureHeir(string genderLaw, CK2Character* currentHolder)
{
	CK2Character* heir = NULL;

	// unless absolute cognatic, consider male children first
	for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
	{
		if (	( (*i) != currentHolder ) &&
			   ( !(*i)->isDead() ) && !(*i)->isBastard() &&
				( !(*i)->isFemale() || (genderLaw == "true_cognatic") ) 
			)
		{
			heir = *i;
		}
	}

	// unless absolute cognatic, consider only male lines
	if (heir == NULL)
	{
		for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
		{
			if ( ( (*i) != currentHolder ) &&
				  ( !(*i)->isBastard() ) &&
				  ( !(*i)->isFemale() || (genderLaw == "true_cognatic") )
				)
			{
				heir = *i;
			}
		}
	}

	// no heirs in male lines, so consider female children
	if (heir == NULL)
	{
		for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
		{
			if (  ( (*i) != currentHolder ) &&
				   ( !(*i)->isDead() ) && !(*i)->isBastard() &&
					( !(*i)->isFemale() || (genderLaw == "cognatic") )
				)
			{
				heir = *i;
			}
		}
	}

	// no heirs in male lines, so consider female lines
	if (heir == NULL)
	{
		for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
		{
			if (	( (*i) != currentHolder ) &&
				   ( !(*i)->isDead() ) && (*i)->isBastard() &&
					( !(*i)->isFemale() || (genderLaw == "cognatic") )
				)
			{
				heir = *i;
			}
		}
	}

	return heir;
}


vector<CK2Character*> CK2Character::getPotentialOpenHeirs(string genderLaw, CK2Character* currentHolder)
{
	vector<CK2Character*> potentialHeirs;
	potentialHeirs.clear();

	// unless absolute cognatic, consider male children first
	for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
	{
		if (	( (*i) != currentHolder ) &&
			   ( !(*i)->isDead() ) && !(*i)->isBastard() &&
				( !(*i)->isFemale() || (genderLaw == "true_cognatic") ) 
			)
		{
			potentialHeirs.push_back(*i);
		}
	}

	// unless absolute cognatic, consider only male lines
	if (potentialHeirs.size() == 0)
	{
		for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
		{
			if ( ( (*i) != currentHolder ) &&
				  ( !(*i)->isBastard() ) &&
				  ( !(*i)->isFemale() || (genderLaw == "true_cognatic") )
				)
			{
				potentialHeirs = (*i)->getPotentialOpenHeirs(genderLaw, currentHolder);
			}
		}
	}

	// no heirs in male lines, so consider female children
	if (potentialHeirs.size() == 0)
	{
		for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
		{
			if (  ( (*i) != currentHolder ) &&
				   ( !(*i)->isDead() ) && !(*i)->isBastard() &&
					( !(*i)->isFemale() || (genderLaw == "cognatic") )
				)
			{
				potentialHeirs.push_back(*i);
			}
		}
	}

	// no heirs in male lines, so consider female lines
	if (potentialHeirs.size() == 0)
	{
		for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
		{
			if (	( (*i) != currentHolder ) &&
				   ( !(*i)->isDead() ) && (*i)->isBastard() &&
					( !(*i)->isFemale() || (genderLaw == "cognatic") )
				)
			{
				potentialHeirs = (*i)->getPotentialOpenHeirs(genderLaw, currentHolder);
			}
		}
	}

	return potentialHeirs;
}


void CK2Character::setGavelkindHeirs(string genderLaw)
{
	vector<CK2Character*> heirs;
	CK2Character* tempHolder = this;
	do
	{
			heirs = tempHolder->getGavelkindHeirs(genderLaw);
			tempHolder = tempHolder->getFather();
			if (tempHolder == NULL)
			{
				break;
			}
	} while (heirs.size() <= 0);
	if (heirs.size() <= 0)
	{
		return;
	}

	vector<CK2Title*>	empireTitles;
	vector<CK2Title*>	kingdomTitles;
	vector<CK2Title*>	duchyTitles;
	vector<CK2Title*>	countyTitles;
	vector<CK2Title*>	baronyTitles;
	for (vector<CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		if ( (*i)->getSuccessionLaw() == "gavelkind")
		{
			string titleString = (*i)->getTitleString();
			if ( titleString.substr(0, 2) == "e_")
			{
				empireTitles.push_back(*i);
			}
			else if ( titleString.substr(0, 2) == "k_")
			{
				kingdomTitles.push_back(*i);
			}
			else if ( titleString.substr(0, 2) == "d_")
			{
				duchyTitles.push_back(*i);
			}
			else if ( titleString.substr(0, 2) == "c_")
			{
				countyTitles.push_back(*i);
			}
			else if ( titleString.substr(0, 2) == "b_")
			{
				baronyTitles.push_back(*i);
			}
		}
	}

	vector<CK2Character*>::iterator heirItr = heirs.begin();
	for (vector<CK2Title*>::iterator i = empireTitles.begin(); i != empireTitles.end(); i++)
	{
		if (heirItr == heirs.end())
		{
			heirItr = heirs.begin();
		}

		(*i)->setHeir(*heirItr);
		heirItr++;
	}

	heirItr = heirs.begin();
	for (vector<CK2Title*>::iterator i = kingdomTitles.begin(); i != kingdomTitles.end(); i++)
	{
		if (heirItr == heirs.end())
		{
			heirItr = heirs.begin();
		}

		(*i)->setHeir(*heirItr);
		heirItr++;
	}

	heirItr = heirs.begin();
	for (vector<CK2Title*>::iterator i = duchyTitles.begin(); i != duchyTitles.end(); i++)
	{
		if (heirItr == heirs.end())
		{
			heirItr = heirs.begin();
		}

		(*i)->setHeir(*heirItr);
		heirItr++;
	}

	heirItr = heirs.begin();
	for (vector<CK2Title*>::iterator i = countyTitles.begin(); i != countyTitles.end(); i++)
	{
		if (heirItr == heirs.end())
		{
			heirItr = heirs.begin();
		}

		(*i)->setHeir(*heirItr);
		heirItr++;
	}

	heirItr = heirs.begin();
	for (vector<CK2Title*>::iterator i = baronyTitles.begin(); i != baronyTitles.end(); i++)
	{
		if (heirItr == heirs.end())
		{
			heirItr = heirs.begin();
		}

		(*i)->setHeir(*heirItr);
		heirItr++;
	}
}


vector<CK2Character*> CK2Character::getGavelkindHeirs(string genderLaw)
{
	vector<CK2Character*> heirs;

	// try male children
	for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
	{
		if ( !(*i)->isBastard() && !(*i)->isDead() && !(*i)->isFemale())
		{
			heirs.push_back(*i);
		}
	}

	// try decendants of oldest eligible child
	if (heirs.size() <= 0)
	{
		for (list<CK2Character*>::iterator i = children.begin(); ( i != children.end() && heirs.size() <= 0 ); i++)
		{
			if (   !(*i)->isBastard() && 
				  ( !(*i)->isFemale() || (genderLaw == "true_cognatic") )
				)
			{
				heirs = (*i)->getGavelkindHeirs(genderLaw);
			}
		}
	}

	// try female children
	if (heirs.size() <= 0)
	{
		for (list<CK2Character*>::iterator i = children.begin(); i != children.end(); i++)
		{
			if ( !(*i)->isBastard() && !(*i)->isDead() && genderLaw == "cognatic")
			{
				heirs.push_back(*i);
			}
		}
	}

	return heirs;
}