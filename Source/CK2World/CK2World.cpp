#include "CK2World.h"
#include "..\Log.h"



void CK2World::init(Object* obj)
{
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

	// get titles
	vector<CK2Title*> titles;
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( (key.substr(0, 2) == "e_") || (key.substr(0, 2) == "k_") || (key.substr(0, 2) == "d_") || (key.substr(0, 2) == "c_") || (key.substr(0, 2) == "b_") )
		{
			CK2Title* newTitle = new CK2Title;
			newTitle->init(leaves[i]);
			titles.push_back(newTitle);
		}
	}

	// create tree of vassal/liege relationships
	for (unsigned int i = 0; i < titles.size(); i++)
	{
		string liege = titles[i]->getLiegeString();
		if (liege == "")
		{
			independentTitles.push_back(titles[i]);
		}
		else
		{
			for (unsigned int j = 0; j < titles.size(); j++)
			{
				if (titles[j]->getTitleString() == liege)
				{
					titles[i]->addLiege(titles[j]);
				}
			}
		}
	}

	log("There are a total of %d independent titles\n", independentTitles.size());
}


date CK2World::getEndDate()
{
	return endDate;
}


vector<CK2Title*> CK2World::getIndependentTitles()
{
	return independentTitles;
}


CK2World::~CK2World()
{
/*	while (independentTitles.size() > 0)
	{
		CK2Title* currentTitle = independentTitles[independentTitles.size() - 1];
		delete currentTitle;
		independentTitles.pop_back();
	}*/
}