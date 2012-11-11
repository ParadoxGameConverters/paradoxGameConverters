#include "CK2World.h"
#include "..\Log.h"
#include "..\Configuration.h"
#include "..\Parsers\Object.h"
#include "CK2Building.h"
#include "CK2Version.h"
#include "CK2Title.h"
#include "CK2Province.h"
#include "CK2Barony.h"
#include "CK2Character.h"
#include "CK2Dynasty.h"
#include "CK2Trait.h"
#include "CK2Techs.h"



CK2World::CK2World()
{
	buildingFactory = new CK2BuildingFactory;

	version = NULL;
	endDate = date();
	independentTitles.clear();
	hreMembers.clear();
	dynasties.clear();
	characters.clear();
	traits.clear();
	titles.clear();
	provinces.clear();
	baronies.clear();
}


void CK2World::init(Object* obj, const religionGroupMapping& religionGroupMap, const cultureGroupMapping& cultureGroupMap)
{
	// get version
	vector<Object*> versionObj = obj->getValue("version");
	if (versionObj.size() > 0)
	{
		version = new CK2Version( versionObj[0]->getLeaf() );
	}
	else
	{
		log("\tError: Unknown version format.\n");
		printf("\tError: Unknown version format.\n");
		version = new CK2Version("0.0");
	}

	// get conversion date
	vector<Object*> dateObj = obj->getValue("date");
	if (dateObj.size() > 0)
	{
		date newDate( dateObj[0]->getLeaf() );
		endDate = newDate;
	}
	else
	{
		date newDate("1399.10.14");
	}

	// get dynasties
	printf("\tGetting dynasties from save\n");
	vector<Object*> dynastyLeaves = obj->getValue("dynasties");
	dynastyLeaves = dynastyLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < dynastyLeaves.size(); i++)
	{
		int number = atoi( dynastyLeaves[i]->getKey().c_str() );
		CK2Dynasty* newDynasty = new CK2Dynasty(dynastyLeaves[i]);
		dynasties.insert( make_pair(number, newDynasty) );
	}
	CK2Dynasty* newDynasty = new CK2Dynasty(0, "Lowborn");
	dynasties.insert( make_pair(0, newDynasty) );

	// get characters
	printf("\tGetting characters\n");
	vector<Object*> characterLeaves = obj->getValue("character");
	characterLeaves = characterLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < characterLeaves.size(); i++)
	{
		int number = atoi( characterLeaves[i]->getKey().c_str() );
		CK2Character* newCharacter = new CK2Character(characterLeaves[i], dynasties, traits, endDate);
		characters.insert( make_pair(number, newCharacter) );
	}

	printf("\tCreating family trees\n");
	for (map<int, CK2Character*>::iterator i = characters.begin(); i != characters.end(); i++)
	{
		i->second->setParents(characters);
	}

	// get titles
	printf("\tGetting titles\n");
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( (key.substr(0, 2) == "e_") || (key.substr(0, 2) == "k_") || (key.substr(0, 2) == "d_") || (key.substr(0, 2) == "c_") || (key.substr(0, 2) == "b_") )
		{
			map<string, CK2Title*>::iterator titleItr = potentialTitles.find(key);
			if (titleItr == potentialTitles.end())
			{
				log("\t\tWarning: tried to create title %s, but it is not a potential title.\n", key.c_str());
				continue;
			}
			titleItr->second->init(leaves[i], characters);
			titles.insert( make_pair(titleItr->second->getTitleString(), titleItr->second) );
		}
	}

	// get provinces
	printf("\tGetting provinces\n");
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if (atoi(key.c_str()) > 0)
		{
			CK2Province* newProvince = new CK2Province(leaves[i], titles, buildingFactory, religionGroupMap, cultureGroupMap);
			provinces.insert( make_pair(atoi(key.c_str()), newProvince) );

			vector<CK2Barony*> newBaronies = newProvince->getBaronies();
			for (unsigned int j = 0; j < newBaronies.size(); j++)
			{
				string title = newBaronies[j]->getTitle()->getTitleString();
				baronies.insert( make_pair(title, newBaronies[j]) );
			}
		}
	}

	// create tree of vassal/liege relationships
	printf("\tRelating vassals and lieges\n");
	string hreTitle = Configuration::getHRETitle();
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		string liege = i->second->getLiegeString();
		if (i->second->getHolder() == NULL)
		{
			continue;
		}
		if (liege == "")
		{
			independentTitles.insert(make_pair(i->first, i->second));
		}
		else if (liege == hreTitle)
		{
			i->second->addToHRE();
			independentTitles.insert((make_pair(i->first, i->second)));
			hreMembers.insert(make_pair(i->first, i->second));
		}
		else
		{
			i->second->setLiege(titles[liege]);
		}
	}

	// create tree of De Jure lieges
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		i->second->setDeJureLiege(potentialTitles);
	}

	// determine heirs
	printf("\tDetermining heirs\n");
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		i->second->determineHeir(characters);
	}

	printf("\tSetting employers\n");
	for (map<int, CK2Character*>::iterator i = characters.begin(); i != characters.end(); i++)
	{
		CK2Character* character = i->second;
		if (character != NULL)
		{
			character->setEmployer(characters, baronies);
		}
	}

	log("\tThere are a total of %d titles\n", titles.size());
	log("\tThere are a total of %d independent titles\n", independentTitles.size());
	log("\tThere are a total of %d hre members\n", hreMembers.size());
}


void CK2World::addBuildingTypes(Object* obj)
{
	buildingFactory->addBuildingTypes(obj);
}


void CK2World::addDynasties(Object* obj)
{
	vector<Object*> dynastyLeaves = obj->getLeaves();
	for (unsigned int i = 0; i < dynastyLeaves.size(); i++)
	{
		int number = atoi( dynastyLeaves[i]->getKey().c_str() );
		CK2Dynasty* newDynasty = new CK2Dynasty(dynastyLeaves[i]);
		dynasties.insert( make_pair(number, newDynasty) );
	}
}


void CK2World::addTraits(Object* obj)
{
	vector<Object*> traitLeaves = obj->getLeaves();
	int offset = traits.size() + 1;
	for (unsigned int i = 0; i < traitLeaves.size(); i++)
	{
		CK2Trait* newTrait = new CK2Trait(traitLeaves[i]);
		traits.insert( make_pair(i + offset, newTrait) );
	}
}


void CK2World::addPotentialTitles(Object* obj)
{
	vector<Object*> leaves = obj->getLeaves();
	for (vector<Object*>::iterator itr = leaves.begin(); itr < leaves.end(); itr++)
	{
		map<string, CK2Title*>::iterator titleItr = potentialTitles.find( (*itr)->getKey() );
		if (titleItr == potentialTitles.end())
		{
			CK2Title* newTitle = new CK2Title( (*itr)->getKey() );
			potentialTitles.insert( make_pair((*itr)->getKey(), newTitle) );
			titleItr = potentialTitles.find( (*itr)->getKey() );
		}
		else
		{
			log("Note! The CK2World::addPotentialTitles() condition is needed!\n");
		}
		titleItr->second->addDeJureVassals( (*itr)->getLeaves(), potentialTitles, this );
	}
}


void CK2World::removeDeadTitles()
{
	for(map<string, CK2Title*>::iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
	{
		if (	(titleItr->second->getVassals().size() == 0) && 
				(titleItr->second->getDeJureVassals().size() == 0) &&
				(titleItr->second->getDeJureLiege() == NULL) &&
				(titleItr->second->getLiege() == NULL)
			)
		{
			log("\tRemoving %s from potential titles (has neither de facto nor de jure vassals or lieges).\n", titleItr->second->getTitleString().c_str());
			map<string, CK2Title*>::iterator eraseItr = potentialTitles.find(titleItr->second->getTitleString());
			if (eraseItr != potentialTitles.end())
			{
				potentialTitles.erase(eraseItr);
			}
			eraseItr = independentTitles.find(titleItr->second->getTitleString());
			if (eraseItr != independentTitles.end())
			{
				independentTitles.erase(eraseItr);
			}
			eraseItr = hreMembers.find(titleItr->second->getTitleString());
			if (eraseItr != hreMembers.end())
			{
				hreMembers.erase(eraseItr);
			}

			titleItr = titles.erase(titleItr);
			titleItr--;
		}
	}

	log("\tThere are a total of %d titles\n", titles.size());
	log("\tThere are a total of %d independent titles\n", independentTitles.size());
	log("\tThere are a total of %d hre members\n", hreMembers.size());
}


void CK2World::mergeTitles()
{
	bool useInheritance = true; // default to "inheritance"
	std::string mergeTitlesSetting = Configuration::getMergeTitles();
	if (mergeTitlesSetting == "never")
		return;
	else if (mergeTitlesSetting == "inheritance")
		useInheritance = true;
	else if (mergeTitlesSetting == "always")
		useInheritance = false;

	for (map<int, CK2Character*>::iterator citr = characters.begin(); citr != characters.end(); ++citr)
	{
		if (!citr->second) continue;
		citr->second->mergeTitles(useInheritance);
	}
}


vector<double> CK2World::getAverageTechLevels() const
{
	vector<double> avgTechLevels;
	avgTechLevels.resize(TECH_LEGALISM);
	for (unsigned int i = 0; i < TECH_LEGALISM; i++)
	{
		avgTechLevels[i] = 0.0f;
	}
	for(map<int, CK2Province*>::const_iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		vector<double> currentTechLevels = provItr->second->getTechLevels();
		for (unsigned int i = 0; i < TECH_LEGALISM; i++)
		{
			avgTechLevels[i] += currentTechLevels[i];
		}
	}
	for (unsigned int i = 0; i < TECH_LEGALISM; i++)
	{
		avgTechLevels[i] /= provinces.size();
	}

	return avgTechLevels;
}
