#include "CK2World.h"
#include "..\Log.h"
#include "..\Configuration.h"
#include "..\Parsers\Object.h"
#include "CK2Version.h"
#include "CK2Title.h"
#include "CK2Province.h"
#include "CK2Barony.h"
#include "CK2Character.h"
#include "CK2Dynasty.h"
#include "CK2Trait.h"



CK2World::CK2World()
{
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


void CK2World::init(Object* obj)
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
	printf("	Getting dynasties from save\n");
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
	printf("	Getting characters\n");
	vector<Object*> characterLeaves = obj->getValue("character");
	characterLeaves = characterLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < characterLeaves.size(); i++)
	{
		int number = atoi( characterLeaves[i]->getKey().c_str() );
		CK2Character* newCharacter = new CK2Character(characterLeaves[i], dynasties, traits, endDate);
		characters.insert( make_pair(number, newCharacter) );
	}

	printf("	Creating family trees\n");
	for (map<int, CK2Character*>::iterator i = characters.begin(); i != characters.end(); i++)
	{
		i->second->setParents(characters);
	}

	// get titles
	printf("	Getting titles\n");
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( (key.substr(0, 2) == "e_") || (key.substr(0, 2) == "k_") || (key.substr(0, 2) == "d_") || (key.substr(0, 2) == "c_") || (key.substr(0, 2) == "b_") )
		{
			map<string, CK2Title*>::iterator titleItr = potentialTitles.find(key);
			if (titleItr == potentialTitles.end())
			{
				log("Error: tried to create title %s, but it is not a potential title.\n", key.c_str());
				CK2Title* newTitle = new CK2Title(key);
				potentialTitles.insert( make_pair(key, newTitle) );
				titleItr = potentialTitles.find(key);
			}
			titleItr->second->init(leaves[i], characters);
			titles.insert( make_pair(titleItr->second->getTitleString(), titleItr->second) );
		}
	}

	// get provinces
	printf("	Getting provinces\n");
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if (atoi(key.c_str()) > 0)
		{
			CK2Province* newProvince = new CK2Province(leaves[i], titles);
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
	printf("	Relating vassals and lieges\n");
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
			independentTitles.push_back(i->second);
		}
		else if (liege == hreTitle)
		{
			i->second->addToHRE();
			independentTitles.push_back(i->second);
			hreMembers.push_back(i->second);
		}
		else
		{
			i->second->addLiege(titles[liege]);
		}
	}

	// create tree of De Jure lieges
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		i->second->setDeJureLiege(potentialTitles);
	}

	// determine heirs
	printf("	Determining heirs\n");
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		i->second->determineHeir(characters);
	}

	printf("	Setting employers\n");
	for (map<int, CK2Character*>::iterator i = characters.begin(); i != characters.end(); i++)
	{
		CK2Character* character = i->second;
		if (character != NULL)
		{
			character->setEmployer(characters, baronies);
		}
	}

	log("	There are a total of %d independent titles\n", independentTitles.size());
	log("	There are a total of %d hre members\n", hreMembers.size());
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
