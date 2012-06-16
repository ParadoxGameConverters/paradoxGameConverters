#include "CK2Title.h"
#include "..\Parsers\Object.h"
#include "CK2Character.h"
#include "CK2Title.h"
#include "CK2Dynasty.h"
#include "CK2History.h"
#include "..\Log.h"



CK2Title::CK2Title()
{
	titleString		= "";
	holder			= NULL;
	heir				= NULL;
	successionLaw	= "";
	genderLaw		= "";
	history.clear();
	liegeString		= "";
	liege				= NULL;
	vassals.clear();

	independent		= true;
	inHRE				= false;
}



void CK2Title::init(Object* obj,  map<int, CK2Character*>& characters)
{
	titleString = obj->getKey();
	vector<Object*> liegeObjs = obj->getValue("liege");
	if (liegeObjs.size() > 0)
	{
		liegeString = liegeObjs[0]->getLeaf();
	}

	vector<Object*> holderObjs = obj->getValue("holder");
	if (holderObjs.size() > 0)
	{
		holder = characters[ atoi( holderObjs[0]->getLeaf().c_str() ) ];
		if (holder != NULL)
		{
			holder->addTitle(this);
		}
	}

	genderLaw = obj->getLeaf("gender");
	successionLaw = obj->getLeaf("succession");

	vector<Object*> leavesObj = obj->getLeaves();
	for (unsigned int i = 0; i < leavesObj.size(); i++)
	{
		if (leavesObj[i]->getKey() == "nomination")
		{
			vector<Object*> nomineeObj = leavesObj[i]->getValue("nominee");
			int nomineeId = atoi( nomineeObj[0]->getLeaf("id").c_str() );

			bool nomineeMarked = false;
			for (unsigned int j = 0; j < nominees.size(); j++)
			{
				if (nominees[j].first == nomineeId)
				{
					nominees[j].second++;
					nomineeMarked = true;
				}
			}
			if (nomineeMarked == false)
			{
				nominees.push_back( make_pair(nomineeId, 1) );
			}
		}
	}

	vector<Object*> historyObjs = obj->getValue("history");
	if (historyObjs.size() > 0)
	{
		historyObjs = historyObjs[0]->getLeaves();
		for (unsigned int i = 0; i < historyObjs.size(); i++)
		{
			CK2History* newHistory = new CK2History();
			newHistory->init(historyObjs[i], characters);
			history.push_back(newHistory);
		}
	}
}


void CK2Title::addLiege(CK2Title* newLiege)
{
	liege = newLiege;
	liege->addVassal(this);

	independent = false;
}


void CK2Title::addVassal(CK2Title* vassal)
{
	vassals.push_back(vassal);
}


void CK2Title::addToHRE()
{
	inHRE = true;
}


string CK2Title::getTitleString()
{
	return titleString;
}


CK2Character* CK2Title::getHolder()
{
	return holder;
}


void CK2Title::determineHeir(map<int, CK2Character*>& characters)
{
	if (holder != NULL)
	{
		if (successionLaw == "primogeniture")
		{
			CK2Character* tempHolder = holder;
			do
			{
				heir = tempHolder->getPrimogenitureHeir(genderLaw, holder);
				tempHolder = tempHolder->getFather();
				if (tempHolder == NULL)
				{
					break;
				}
			} while (heir == NULL);
		}
		else if (successionLaw == "gavelkind")
		{
			if(heir == NULL) // if the heir is not null, we've already set this
			{
				holder->setGavelkindHeirs(genderLaw);
			}
		}
		else if (successionLaw == "seniority")
		{
			heir = holder->getDynasty()->getSenoirityHeir(genderLaw);
		}
		else if (successionLaw == "feudal_elective")
		{
			heir = getFeudalElectiveHeir(characters);
		}
		else if (successionLaw == "turkish_succession")
		{
			heir = getTurkishSuccessionHeir();
		}
	}
}


void CK2Title::setHeir(CK2Character* newHeir)
{
	heir = newHeir;
}


CK2Character* CK2Title::getHeir()
{
	return heir;
}


string CK2Title::getSuccessionLaw()
{
	return successionLaw;
}


vector<CK2History*> CK2Title::getHistory()
{
	return history;
}


string CK2Title::getLiegeString()
{
	return liegeString;
}


CK2Title* CK2Title::getLiege()
{
	return liege;
}


bool CK2Title::isIndependent()
{
	return independent;
}


bool CK2Title::isInHRE()
{
	return inHRE;
}


CK2Character* CK2Title::getFeudalElectiveHeir(map<int, CK2Character*>& characters)
{
	int nominee = -1;
	int mostVotes = 0;
	for (unsigned int i = 0; i < nominees.size(); i++)
	{
		if (nominees[i].second > mostVotes)
		{
			nominee		= nominees[i].first;
			mostVotes	= nominees[i].second;
		}
	}

	return characters[nominee];
}


CK2Character* CK2Title::getTurkishSuccessionHeir()
{
	vector<CK2Character*> potentialHeirs;
	potentialHeirs.clear();
	CK2Character* tempHolder = holder;
	do
	{
		potentialHeirs = tempHolder->getPotentialOpenHeirs(genderLaw, holder);
		tempHolder = tempHolder->getFather();
		if (tempHolder == NULL)
		{
			break;
		}
	} while (potentialHeirs.size() == 0);

	int largestDemesne = 0;
	for (vector<CK2Character*>::iterator i = potentialHeirs.begin(); i != potentialHeirs.end(); i++)
	{
		vector<CK2Title*> titles = (*i)->getTitles();
		int demesne = 0;
		for (vector<CK2Title*>::iterator j = titles.begin(); j != titles.end(); j++)
		{
			if ( (*j)->getTitleString().substr(0, 2) == "k_" )
			{
				demesne++;
			}
		}
		if (demesne > largestDemesne)
		{
			heir = *i;
			largestDemesne = demesne;
		}
	}

	if (heir == NULL)
	{
		for (vector<CK2Character*>::iterator i = potentialHeirs.begin(); i != potentialHeirs.end(); i++)
		{
			vector<CK2Title*> titles = (*i)->getTitles();
			int demesne = 0;
			for (vector<CK2Title*>::iterator j = titles.begin(); j != titles.end(); j++)
			{
				if ( (*j)->getTitleString().substr(0, 2) == "d_" )
				{
					demesne++;
				}
			}
			if (demesne > largestDemesne)
			{
				heir = *i;
				largestDemesne = demesne;
			}
		}
	}

	if (heir == NULL)
	{
		for (vector<CK2Character*>::iterator i = potentialHeirs.begin(); i != potentialHeirs.end(); i++)
		{
			vector<CK2Title*> titles = (*i)->getTitles();
			int demesne = 0;
			for (vector<CK2Title*>::iterator j = titles.begin(); j != titles.end(); j++)
			{
				if ( (*j)->getTitleString().substr(0, 2) == "c_" )
				{
					demesne++;
				}
			}
			if (demesne > largestDemesne)
			{
				heir = *i;
				largestDemesne = demesne;
			}
		}
	}

	if (heir == NULL)
	{
		for (vector<CK2Character*>::iterator i = potentialHeirs.begin(); i != potentialHeirs.end(); i++)
		{
			vector<CK2Title*> titles = (*i)->getTitles();
			int demesne = 0;
			for (vector<CK2Title*>::iterator j = titles.begin(); j != titles.end(); j++)
			{
				if ( (*j)->getTitleString().substr(0, 2) == "b_" )
				{
					demesne++;
				}
			}
			if (demesne > largestDemesne)
			{
				heir = *i;
				largestDemesne = demesne;
			}
		}
	}

	if ( (heir == NULL) && (potentialHeirs.size() > 0) )
	{
		heir = potentialHeirs[0];
	}

	return heir;
}



CK2Title::~CK2Title()
{
	//TODO: learn why this crashes things
/*	while (vassals.size() > 0)
	{
		CK2Title* currentTitle = vassals[vassals.size() - 1];
		delete currentTitle;
		vassals.pop_back();
	}*/
}