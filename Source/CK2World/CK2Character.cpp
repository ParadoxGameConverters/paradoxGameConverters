#include "CK2Character.h"
#include <algorithm>
#include "..\log.h"
#include "..\Configuration.h"
#include "..\Parsers\Object.h"
#include "CK2Dynasty.h"
#include "CK2Trait.h"
#include "CK2Barony.h"
#include "CK2Province.h"
#include "CK2Title.h"
#include "CK2Techs.h"
#include "CK2War.h"
#include "CK2Religion.h"
#include "CK2Army.h"



CK2Character::CK2Character(Object* obj, const map<int, CK2Dynasty*>& dynasties, const map<int, CK2Trait*>& traitTypes, date theDate)
{
	num			= atoi( obj->getKey().c_str() );
	name			= obj->getLeaf("birth_name");
	religion		= CK2Religion::getReligion(obj->getLeaf("religion"));
	culture		= obj->getLeaf("culture");

	vector<Object*> pobjs = obj->getValue("prestige");
	if (pobjs.size() > 0)
		prestige = atof(pobjs[0]->getLeaf().c_str());
	else
		prestige = 0.0;

	pobjs = obj->getValue("piety");
	if (pobjs.size() > 0)
		piety = atof(pobjs[0]->getLeaf().c_str());
	else
		piety = 0.0;

	pobjs = obj->getValue("score");
	if (pobjs.size() > 0)
		score = atof(pobjs[0]->getLeaf().c_str());
	else
		score = 0.0;

	dynasty		= NULL;
	map<int, CK2Dynasty*>::const_iterator dynItr	= dynasties.find(  atoi( obj->getLeaf("dynasty").c_str() )  );
	if (dynItr != dynasties.end())
	{
		dynasty = dynItr->second;
		dynasty->addMember(this);
	}
	else
	{
		log("\t\tError: no dynasty for character %d (%s).\n", num, name.c_str());
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

	vector<Object*> spouseObj = obj->getValue("spouse");
	for (vector<Object*>::iterator itr = spouseObj.begin(); itr != spouseObj.end(); ++itr)
	{
		spouseNums.push_back(atoi((*itr)->getLeaf().c_str()));
	}

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
	primaryTitleString		= "";
	vector<Object*> demesneObj = obj->getValue("demesne");
	if (demesneObj.size() > 0)
	{
		vector<Object*> capitalObj = demesneObj[0]->getValue("capital");
		if (capitalObj.size() > 0)
		{
			capitalString = capitalObj[0]->getLeaf();
		}
		vector<Object*> primaryObj = demesneObj[0]->getValue("primary");
		if (primaryObj.size() > 0)
		{
			primaryTitleString = primaryObj[0]->getLeaf();
		}
		vector<Object*> armyObjs = demesneObj[0]->getValue("army");
		for (unsigned int i = 0; i < armyObjs.size(); i++)
		{
			CK2Army* newArmy = new CK2Army(armyObjs[i]);
			armies.push_back(newArmy);
		}
		vector<Object*> navyObjs = demesneObj[0]->getValue("navy");
		for (unsigned int i = 0; i < navyObjs.size(); i++)
		{
			CK2Army* newNavy = new CK2Army(navyObjs[i]);
			navies.push_back(newNavy);
		}
	}
	capital = NULL;
	primaryTitle = NULL;

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
		log("\t\tError: no attributes for character %d (%s).\n", num, name.c_str());
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
			traitNums.push_back( atoi(traitsStrings[i].c_str()) );
		}
	}

	for (unsigned int i = 0; i < traitNums.size(); i++)
	{
		map<int, CK2Trait*>::const_iterator itr = traitTypes.find(traitNums[i]);
		if (itr == traitTypes.end())
		{
			if (dynasty != NULL)
				log("Error: %s %s had extra trait %d\n", name.c_str(), dynasty->getName().c_str(), traitNums[i]);
			else
				log("Error: %s had extra trait %d\n", name.c_str(), traitNums[i]);
			continue;
		}
		CK2Trait* currentTrait = itr->second;
		traits.push_back(currentTrait);
		stats[DIPLOMACY]		+= currentTrait->diplomacy;
		stats[MARTIAL]			+= currentTrait->martial;
		stats[STEWARDSHIP]	+= currentTrait->stewardship;
		stats[INTRIGUE]		+= currentTrait->intrigue;
		stats[LEARNING]		+= currentTrait->learning;
	}

	for (unsigned int i = 0; i < 5; i++)
	{
		if (stats[i] < 0)
		{
			stats[i] = 0;
		}
	}

	memset(stateStats, 0, 5*sizeof(int));

	primaryHolding = NULL;
}


void CK2Character::readOpinionModifiers(Object* obj)
{
	vector<Object*> leaves = obj->getLeaves();
	for (vector<Object*>::iterator itr = leaves.begin(); itr != leaves.end(); ++itr)
	{
		int charId = atoi((*itr)->getKey().c_str());
		if (charId == 0)
			continue;  // shouldn't happen
		vector<Object*> modifiers = (*itr)->getLeaves();
		for (vector<Object*>::iterator mitr = modifiers.begin(); mitr != modifiers.end(); ++mitr)
		{
			CK2Opinion opinion(*mitr);
			opinionMods[charId].push_back(opinion);
		}
	}
}


CK2Character* CK2Character::getPrimarySpouse() const
{
	// FIXME: is first living spouse always primary?
	for (vector<CK2Character*>::const_iterator itr = spouses.begin(); itr != spouses.end(); ++itr)
	{
		if (!(*itr)->isDead())
		{
			return (*itr);
		}
	}
	return NULL;
}


void CK2Character::setStateStats()
{
	// start with regent's stats or mine
	if (regent != NULL)
		memcpy(stateStats, regent->getStats(), 5*sizeof(int));
	else
		memcpy(stateStats, stats, 5*sizeof(int));

	// add 1/2 of primary spouse's stats
	CK2Character* spouse = getPrimarySpouse();
	if (spouse != NULL)
	{
		for (int i = 0; i < 5; ++i)
			stateStats[i] += spouse->getStats()[i] / 2;
	}

	// add relevant advisors
	for (int i = 0; i < 5; ++i)
	{
		if (advisors[i] != NULL)
			stateStats[i] += advisors[i]->getStats()[i];
	}
}


void CK2Character::addTitle(CK2Title* newTitle)
{
	titles.push_back(newTitle);
}


void CK2Character::removeTitle(CK2Title* oldTitle)
{
	vector<CK2Title*>::iterator itr = std::find(titles.begin(), titles.end(), oldTitle);
	if (itr != titles.end())
		titles.erase(itr);
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

	spouses.clear();
	for (vector<int>::iterator itr = spouseNums.begin(); itr != spouseNums.end(); ++itr)
		spouses.push_back(characters[*itr]);

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
			map<string, CK2Barony*>::iterator itr = baronies.find(hostCapitalString);
			if (itr != baronies.end())
			{
				CK2Barony* homeBarony = itr->second;
				if (homeBarony != NULL)
				{
					locationNum = homeBarony->getProvince()->getNumber();
				}
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


CK2Character* CK2Character::getUltimogenitureHeir(string genderLaw, CK2Character* currentHolder)
{
	CK2Character* heir = NULL;

	// unless absolute cognatic, consider male children first
	for (list<CK2Character*>::reverse_iterator i = children.rbegin(); i != children.rend(); i++)
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
		for (list<CK2Character*>::reverse_iterator i = children.rbegin(); i != children.rend(); i++)
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
		for (list<CK2Character*>::reverse_iterator i = children.rbegin(); i != children.rend(); i++)
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
		for (list<CK2Character*>::reverse_iterator i = children.rbegin(); i != children.rend(); i++)
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


void CK2Character::mergeTitles(bool useInheritance)
{
	if (titles.size() > 1) // can't merge 1 or fewer...
	{
		// first try merging everything into the primary title
		if (primaryTitle != NULL)
		{
			for (vector<CK2Title*>::reverse_iterator titr = titles.rbegin(); titr != titles.rend(); ++titr)
			{
				if ((*titr) != primaryTitle)
					primaryTitle->eatTitle((*titr), useInheritance);
			}
		}

		// then merge anything that remains
		for (vector<CK2Title*>::iterator sitr = titles.begin(); (sitr != (titles.end() - 1)) && (sitr != titles.end()); ++sitr)
		{
			for (vector<CK2Title*>::reverse_iterator titr = titles.rbegin(); (titr+1).base() != sitr; ++titr)
			{
				(*sitr)->eatTitle((*titr), useInheritance);
			}
		}
	}
}


void CK2Character::setPrimaryTitle(const map<string, CK2Title*>& titleMap)
{
	// find the title
	map<string, CK2Title*>::const_iterator itr = titleMap.find(primaryTitleString);
	if ((itr != titleMap.end()) && (itr->first != Configuration::getHRETitle()))
	{
		primaryTitle = itr->second;

		// sanity check
		if (primaryTitle->getHolder() != this)
			log("Error: primary title of %s is %s, but s/he does not hold it.\n", getName().c_str(), primaryTitleString.c_str());
	}
	else
	{
		for (vector<CK2Title*>::iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
		{
			if (((*titleItr)->getTitleString().substr(0,2) == "e_") && ((*titleItr)->getTitleString() != Configuration::getHRETitle()))
			{
				primaryTitle = *titleItr;
				return;
			}
		}
		for (vector<CK2Title*>::iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
		{
			if (((*titleItr)->getTitleString().substr(0,2) == "k_") && ((*titleItr)->getTitleString() != Configuration::getHRETitle()))
			{
				primaryTitle = *titleItr;
				return;
			}
		}
		for (vector<CK2Title*>::iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
		{
			if (((*titleItr)->getTitleString().substr(0,2) == "d_") && ((*titleItr)->getTitleString() != Configuration::getHRETitle()))
			{
				primaryTitle = *titleItr;
				return;
			}
		}
		for (vector<CK2Title*>::iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
		{
			if (((*titleItr)->getTitleString().substr(0,2) == "c_") && ((*titleItr)->getTitleString() != Configuration::getHRETitle()))
			{
				primaryTitle = *titleItr;
				return;
			}
		}
		for (vector<CK2Title*>::iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
		{
			if (((*titleItr)->getTitleString().substr(0,2) == "b_") && ((*titleItr)->getTitleString() != Configuration::getHRETitle()))
			{
				primaryTitle = *titleItr;
				return;
			}
		}
	}
	if ((primaryTitle == NULL) && (titles.size() > 0))
	{
		log("Warning: %s %s does not have a primary title.\n", name.c_str(), dynasty->getName().c_str());
	}
}


vector<CK2Character*> CK2Character::getCloseRelations() const
{
	vector<CK2Character*> rels;

	// my mother and her children (my siblings and half-siblings)
	if (mother != NULL)
	{
		rels.push_back(mother);
		rels.insert(rels.end(), mother->children.begin(), mother->children.end());
	}

	// my father and his children by women other than my mother (my half-siblings)
	if (father != NULL)
	{
		rels.push_back(father);
		for (list<CK2Character*>::iterator itr = father->children.begin(); itr != father->children.end(); ++itr)
		{
			if ((*itr)->mother != mother)
				rels.push_back((*itr));
		}
	}

	// my children
	rels.insert(rels.end(), children.begin(), children.end());

	return rels;
}


bool CK2Character::isCloseRelationOf(const CK2Character* other) const
{
	// my parent
	if (other == mother || other == father)
		return true;

	// my child
	if (other->mother == this || other->father == this)
		return true;

	// same mother (sibling or half-sibling)
	if (mother != NULL && mother == other->mother)
		return true;

	// same father (half-sibling)
	if (father != NULL && father == other->father)
		return true;

	return false;
}


bool CK2Character::isRMWith(const CK2Character* other) const
{
	vector<CK2Character*>::const_iterator spouse = std::find(spouses.begin(), spouses.end(), other);
	if (spouse != spouses.end())
		return true;

	for (vector<CK2Character*>::const_iterator itr = spouses.begin(); itr != spouses.end(); ++itr)
	{
		// RM with my spouse
		if ((*itr) == other)
			return true;

		// RM with my living spouse's close relations (e.g. my father-in-law)
		vector<CK2Character*> sRels = (*itr)->getCloseRelations();
		for (vector<CK2Character*>::const_iterator scitr = sRels.begin(); scitr != sRels.end(); ++scitr)
		{
			if ((*scitr) == other)
				return true;
		}
	}

	vector<CK2Character*> myRels = getCloseRelations();
	for (vector<CK2Character*>::const_iterator itr = myRels.begin(); itr != myRels.end(); ++itr)
	{
		// RM directly with my close relations (not of my dynasty...e.g. a half-brother on my mother's side)
		if ((*itr) == other && (*itr)->dynasty != dynasty)
			return true;

		for (vector<CK2Character*>::const_iterator sitr = (*itr)->spouses.begin(); sitr != (*itr)->spouses.end(); ++sitr)
		{
			if (!(*sitr)->isDead())
			{
				// RM with my close relations' living spouses (e.g. my sister's husband)
				if ((*sitr) == other)
					return true;

				// RM with my close relations' living spouses' close relations (e.g. my sister's husband's father)
				vector<CK2Character*> sRels = (*sitr)->getCloseRelations();
				for (vector<CK2Character*>::const_iterator scitr = sRels.begin(); scitr != sRels.end(); ++scitr)
				{
					if ((*scitr) == other)
						return true;
				}
			}
		}
	}

	return false;
}


bool CK2Character::isAlliedWith(const CK2Character* other) const
{
	// same dynasty
	if (this->dynasty == other->dynasty)
		return true;

	// or royal marriage
	return isRMWith(other);
}


bool CK2Character::isDirectVassalOf(const CK2Character* other) const
{
	return (primaryTitle && primaryTitle->getLiege() && primaryTitle->getLiege()->getHolder()
			&& primaryTitle->getLiege()->getHolder() == other);
}


int CK2Character::getOpinionOf(const CK2Character* other) const
{
	int relations = 0;

	// ***** scaled intrinsics (variable - e.g. prestige, state diplomacy, piety...)

	// State Diplomacy - anyone who's not my host
	if (hostNum != other->num)
		relations += other->getStateStats()[DIPLOMACY] / 2;

	// Personal Diplomacy - my host only (for courtiers)
	if (hostNum == other->num)
		relations += other->getStats()[DIPLOMACY];

	// Piety - if we're the same religion
	if (religion->isSameReligion(other->religion))
	{
		// and we're both muslim, or I'm a cleric
		if (religion->getGroup() == "muslim" || (primaryHolding && primaryHolding->getType() == "temple"))
		{
			// add 1/25 of piety, or 20, whichever is larger
			relations += (int)floor(max(other->piety / 25.0, 20.0));
		}
	}

	// Prestige - 1/100, or 20, whichever is larger
	relations += (int)floor(max(other->prestige / 100.0, 20.0));

	// Vassal relations modifiers
	if (this->isDirectVassalOf(other))
	{
		// Desmense Too Big
		int exceededBy = other->holdings.size() - other->getDemesneCap();
		if (exceededBy > 0)
		{
			relations += -10 * exceededBy;
		}

		// FIXME: Long Reign
		// FIXME: Short Reign
		// FIXME: Too Many Held Duchies
	}


	// ***** fixed intrinsics (fixed opinion - e.g. same dynasty, father of child...)
	// These are not saved in the CK2 file, unfortunately

	// Same Dynasty
	if (dynasty == other->dynasty)
		relations += CK2Opinion::getBaseValue("same_dynasty");

	// Ally
	if (this->isAlliedWith(other))
		relations += CK2Opinion::getBaseValue("opinion_ally");

	// At War
	for (vector<CK2War*>::const_iterator itr = wars.begin(); itr != wars.end(); ++itr)
	{
		if ((*itr)->areEnemies(this->num, other->num))
			relations += CK2Opinion::getBaseValue("opinion_at_war");
	}

	// Mother or Father of Child
	if (other->mother == this)
		relations += CK2Opinion::getBaseValue("opinion_mother_child");
	if (other->father == this)
		relations += CK2Opinion::getBaseValue("opinion_father_of_child");

	// Muslim brother or half-brother
	if (religion->getGroup() == "muslim")
	{
		if (this->mother == other->mother && this->father == other->father)
		{
			// Muslim Brother
			relations += CK2Opinion::getBaseValue("opinion_brother_muslim");
		}
		else if (this->mother == other->mother || this->father == other->father)
		{
			// Muslim Half-brother
			relations += CK2Opinion::getBaseValue("opinion_half_brother_muslim");
		}
	}

	// FIXME: Claimant vs. Holder

	// De Jure Liege - does not affect opinion modifier (just revolt chance), so we can ignore

	// FIXME: De Jure Liege Hog ("wants control of...")
	// FIXME: Pretender
	// FIXME: Nominee
	// FIXME: Non-liege Nominee
	// FIXME: Foreign Culture
	// FIXME: Related Culture

	// Infidel
	if (religion->isInfidelTo(other->religion))
	{
		relations += CK2Opinion::getBaseValue("opinion_infidel");
	}

	// Heretic
	if (religion->isHereticTo(other->religion))
	{
		relations += CK2Opinion::getBaseValue("opinion_heretic");
	}

	// Related Religion
	if (religion->isRelatedTo(other->religion))
	{
		relations += CK2Opinion::getBaseValue("opinion_related_religion");
	}

	// FIXME: Rightful Religious Head

	// Female Heir
	if (this->isDirectVassalOf(other) && primaryTitle->getLiege()->getHeir() && primaryTitle->getLiege()->getHeir()->isFemale())
	{
		relations += CK2Opinion::getBaseValue("opinion_female_heir");
	}

	// Female Ruler
	if (this->isDirectVassalOf(other) && other->isFemale() && !this->isFemale())
	{
		relations += CK2Opinion::getBaseValue("opinion_female_ruler");
	}

	// FIXME: Defending My Titles
	// FIXME: Defending Against Infidel
	// FIXME: Pope vs. Antipope
	// FIXME: Pope vs. Antipope's Controller
	// FIXME: Cleric vs. Crusader (current crusade, not trait)
	// FIXME: Cleric vs. Non-crusader (current crusade)

	// Wrong Government Type (counts and above only)
	if (this->isDirectVassalOf(other) && this->primaryTitleString.substr(0,2) != "b_")
	{
		if (this->primaryHolding->getType() != other->primaryHolding->getType())
		{
			relations += CK2Opinion::getBaseValue("opinion_count_wrong_gov_vs_liege");
		}
	}


	// ***** FIXME: Crown law modifiers (levy law/tax law)


	// ***** Trait opinion modifers (incl. complements and conflicts - e.g. Greedy/Charitable, Kind/Kind)
	for (vector<CK2Trait*>::const_iterator trait = other->traits.begin(); trait != other->traits.end(); ++trait)
	{
		if (this->isDirectVassalOf(other))
		{
			relations += (*trait)->vassal_opinion;
		}
		else if (other->isDirectVassalOf(this))
		{
			relations += (*trait)->liege_opinion;
		}
		// sex appeal: both female and I'm gay, both male and I'm gay, or different gender and I'm straight
		if ((this->isFemale() != other->isFemale()) == (!this->hasTrait("homosexual")))
		{
			relations += (*trait)->sex_appeal_opinion;
		}
		if (this->hasTrait((*trait)->name))
		{
			relations += (*trait)->same_opinion;
			if (this->religion->isSameReligion(other->religion))
			{
				relations += (*trait)->same_opinion_if_same_religion;
			}
		}
		for (vector<CK2Trait*>::const_iterator itr = traits.begin(); itr != traits.end(); ++itr)
		{
			if ((*trait)->isOppositeOf(*itr))
				relations += (*itr)->opposite_opinion;
		}
		if (primaryHolding && primaryHolding->getType() == "temple")
		{
			relations += (*trait)->church_opinion;
		}
		if (this->mother == other->mother && this->father == other->father && this->birthDate == other->birthDate)
		{
			relations += (*trait)->twin_opinion;
		}
		for (vector<CK2Character*>::const_iterator itr = spouses.begin(); itr != spouses.end(); ++itr)
		{
			if (*itr == other)
				relations += (*trait)->spouse_opinion;
		}
		if (this->religion->isSameReligion(other->religion))
		{
			relations += (*trait)->same_religion_opinion;
		}
		else if (this->religion->isInfidelTo(other->religion))
		{
			relations += (*trait)->infidel_opinion;
		}
		if (this->religion->getGroup() == "muslim")
		{
			relations += (*trait)->muslim_opinion;
		}
		if (this->dynasty == other->dynasty)
		{
			relations += (*trait)->dynasty_opinion;
		}
	}

	// ***** rel. blocks (timed - e.g. broke alliance, lover, cuckolded)
	map<int, vector<CK2Opinion>>::const_iterator opinions = opinionMods.find(other->getNum());
	if (opinions != opinionMods.end())
	{
		for (vector<CK2Opinion>::const_iterator itr = opinions->second.begin(); itr != opinions->second.end(); ++itr)
		{
			relations += itr->getTotalOpinion();
		}
	}

	return min(max(relations, -100), 100);
}


bool CK2Character::hasTrait(string traitName) const
{
	for (vector<CK2Trait*>::const_iterator itr = traits.begin(); itr != traits.end(); ++itr)
	{
		if ((*itr)->name == traitName)
			return true;
	}
	return false;
}


int CK2Character::getDemesneCap() const
{
	// http://forum.paradoxplaza.com/forum/showthread.php?584969-What-are-the-high-and-low-bound-demense-limits-for-different-ranks-authority-tech&p=13429280&viewfull=1#post13429280

	int rulerTier = 0;
	if (primaryTitleString.substr(0, 2) == "b_")
		rulerTier = 1;
	else if (primaryTitleString.substr(0, 2) == "c_")
		rulerTier = 2;
	else if (primaryTitleString.substr(0, 2) == "d_")
		rulerTier = 3;
	else if (primaryTitleString.substr(0, 2) == "k_")
		rulerTier = 4;
	else if (primaryTitleString.substr(0, 2) == "e_")
		rulerTier = 5;
	if (rulerTier == 0) // not a ruler - no demesne
		return 0;

	double stewardshipBonus = stats[STEWARDSHIP];
	CK2Character* spouse = getPrimarySpouse();
	if (spouse)
		stewardshipBonus += floor(spouse->getStats()[STEWARDSHIP] / 2.0);
	stewardshipBonus /= 4.0;

	double successionFactor = 1.0;
	if (primaryTitle->getSuccessionLaw() == "gavelkind")
		successionFactor = 1.3;

	int legalismBonus = 0;
	double legalismTech = capital->getTechLevels()[TECH_LEGALISM];
	if (legalismTech >= 1.0)
	{
		if (rulerTier >= 2)
			legalismBonus++;
	}
	if (legalismTech >= 2.0)
	{
		if (rulerTier == 1 || rulerTier >= 3)
			legalismBonus++;
	}
	if (legalismTech >= 3.0)
	{
		if (rulerTier == 2 || rulerTier >= 4)
			legalismBonus++;
	}
	if (legalismTech >= 4.0)
	{
		if (rulerTier >= 3)
			legalismBonus++;
	}
	if (legalismTech >= 5.0)
	{
		if (rulerTier == 5)
			legalismBonus++;
	}
	
	return (int)(floor( (rulerTier + stewardshipBonus) * successionFactor) + legalismBonus);
}