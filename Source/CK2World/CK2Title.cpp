#include "CK2Title.h"
#include "..\Parsers\Object.h"
#include "CK2World.h"
#include "CK2Character.h"
#include "CK2Title.h"
#include "CK2Dynasty.h"
#include "CK2History.h"
#include "..\Log.h"
#include <algorithm>
#include "..\Configuration.h"
#include "..\EU3World\EU3Country.h"


CK2Title::CK2Title(string _titleString)
{
	titleString			= _titleString;
	holder				= NULL;
	heir					= NULL;
	successionLaw		= "";
	genderLaw			= "";
	nominees.clear();
	history.clear();
	liegeString			= "";
	liege					= NULL;
	vassals.clear();
	deJureLiegeString	= "";
	deJureLiege			= NULL;
	independent			= true;
	inHRE					= false;
	dstCountry			= NULL;
}


void CK2Title::init(Object* obj,  map<int, CK2Character*>& characters)
{
	titleString = obj->getKey();
	holder = NULL;
	vector<Object*> holderObjs = obj->getValue("holder");
	if (holderObjs.size() > 0)
	{
		holder = characters[ atoi( holderObjs[0]->getLeaf().c_str() ) ];
		if (holder != NULL)
		{
			holder->addTitle(this);
		}
	}
	heir = NULL;
	successionLaw = obj->getLeaf("succession");
	genderLaw = obj->getLeaf("gender");

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
			CK2History* newHistory = new CK2History(historyObjs[i], characters);
			history.push_back(newHistory);
		}
	}

	vector<Object*> liegeObjs = obj->getValue("liege");
	if (liegeObjs.size() > 0)
	{
		liegeString = liegeObjs[0]->getLeaf();
	}
	liege = NULL;

	vassals.clear();

	vector<Object*> deJureLiegeObjs = obj->getValue("de_jure_liege");
	if (deJureLiegeObjs.size() > 0)
	{
		deJureLiegeString = deJureLiegeObjs[0]->getLeaf();
	}
	else
	{
		deJureLiegeString = "";
	}
	if (deJureLiegeString[0] == '"')
	{
		deJureLiegeString = deJureLiegeString.substr(1, deJureLiegeString.size() - 2);
	}

	independent = true;
	inHRE = false;
}


void CK2Title::setLiege(CK2Title* newLiege)
{
	if (liege)
		liege->removeVassal(this);

	liege = newLiege;
	liege->addVassal(this);
	liegeString = newLiege->getTitleString();

	independent = false;
}


void CK2Title::addVassal(CK2Title* vassal)
{
	vassals.push_back(vassal);
}


void CK2Title::removeVassal(CK2Title* vassal)
{
	vector<CK2Title*>::iterator itr = find(vassals.begin(), vassals.end(), vassal);
	if (itr != vassals.end())
		vassals.erase(itr);
}


void CK2Title::addToHRE()
{
	inHRE = true;
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


void CK2Title::setDeJureLiege(const map<string, CK2Title*>& titles)
{
	if (  (deJureLiegeString != "") && (deJureLiegeString != "---") && ( (deJureLiege == NULL) || (deJureLiege->getTitleString() != deJureLiegeString ) )  )
	{
		map<string, CK2Title*>::const_iterator titleItr = titles.find(deJureLiegeString);
		if (titleItr != titles.end())
		{
			if (deJureLiege != NULL)
			{
				deJureLiege->removeDeJureVassal(this);
			}
			deJureLiege = titleItr->second;
			deJureLiege->addDeJureVassal(this);
		}
	}
}


void CK2Title::setDeJureLiege(CK2Title* _deJureLiege)
{
	if (deJureLiege)
		deJureLiege->removeDeJureVassal(this);

	 deJureLiege = _deJureLiege;
	 deJureLiegeString = _deJureLiege->getTitleString();
	 _deJureLiege->addDeJureVassal(this);
}


void CK2Title::addDeJureVassals(vector<Object*> obj, map<string, CK2Title*>& titles, CK2World* world)
{
	for (vector<Object*>::iterator itr = obj.begin(); itr < obj.end(); itr++)
	{
		string substr = (*itr)->getKey().substr(0, 2);
		if ( (substr != "e_") && (substr != "k_") && (substr != "d_") && (substr != "c_") && (substr != "b_") )
		{
			continue;
		}
		map<string, CK2Title*>::iterator titleItr = titles.find( (*itr)->getKey() );
		if (titleItr == titles.end())
		{
			CK2Title* newTitle = new CK2Title( (*itr)->getKey() );
			titles.insert( make_pair((*itr)->getKey(), newTitle) );
			titleItr = titles.find( (*itr)->getKey() );
		}
		else
		{
			log("Note! The CK2Title::addDeJureVassals() else condition is needed!\n");
		}
		titleItr->second->setDeJureLiege(this);
		titleItr->second->addDeJureVassals( (*itr)->getLeaves(), titles, world );
	}
}


void CK2Title::removeDeJureVassal(CK2Title* vassal)
{
	for (vector<CK2Title*>::iterator itr = deJureVassals.begin(); itr < deJureVassals.end(); itr++)
	{
		if (*itr == vassal)
		{
			deJureVassals.erase(itr);
			break;
		}
	}
}


void CK2Title::getCultureWeights(map<string, int>& cultureWeights, const cultureMapping& cultureMap) const
{
	for (vector<CK2Title*>::const_iterator vassalItr = vassals.begin(); vassalItr < vassals.end(); vassalItr++)
	{
		(*vassalItr)->getCultureWeights(cultureWeights, cultureMap);
	}

	int weight = 0;
	if (titleString.substr(0, 2) == "e_")
	{
		weight = 5;
	}
	else if (titleString.substr(0, 2) == "k_")
	{
		weight = 4;
	}
	else if (titleString.substr(0, 2) == "d_")
	{
		weight = 3;
	}
	else if (titleString.substr(0, 2) == "c_")
	{
		weight = 2;
	}
	else if (titleString.substr(0, 2) == "b_")
	{
		weight = 1;
	}

	CK2Province* capital = holder->getCapital();
	if (capital != NULL)
	{
		string culture = determineEU3Culture(holder->getCulture(), cultureMap, capital);
		cultureWeights[culture] += weight;
	}
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


bool CK2Title::eatTitle(CK2Title* target, bool checkInheritance)
{
	// see if it's valid to consume the target title

	// can't autocephalate
	if (target == this)
	{
		log("\tAssert: title attempted to autocephalate.\n");
		return false;
	}

	// merged titles must have the same holder, and the holder must exist
	if (!getHolder())
		return false;
	if (getHolder() != target->getHolder())
		return false;

	// don't merge barony titles (causes problems elsewhere, and there's no real reason to)
	if (titleString.substr(0,2) == "b_" || target->titleString.substr(0,2) == "b_")
		return false;

	// don't merge anything with the designated hre title
	if (titleString == Configuration::getHRETitle() || target->titleString == Configuration::getHRETitle())
		return false;

	// can't merge a vassal with an independent entity
	if (isIndependent() != target->isIndependent())
		return false;

	// merged vassal titles must have the same liege
	if (!isIndependent())
	{
		if (getLiegeString() != target->getLiegeString())
			return false;
	}

	// if we are checking inheritance, merged titles must have the same heir, succession and gender laws, and neither can be elective, gavelkind, or bishoporic
	if (checkInheritance)
	{
		if (getHeir() != target->getHeir())
			return false;
		if (successionLaw != target->successionLaw || genderLaw != target->genderLaw)
			return false;
		if (successionLaw == "feudal_elective" || successionLaw == "open_elective" || successionLaw == "gavelkind" || successionLaw == "catholic_bishopric")
			return false;
	}

	// if we get here, it must be valid, so do the deed

	// steal all vassals and de jure vassals from target
	for (vector<CK2Title*>::reverse_iterator itr = target->vassals.rbegin(); itr != target->vassals.rend(); ++itr)
		(*itr)->setLiege(this);
	/*for (vector<CK2Title*>::reverse_iterator itr = target->deJureVassals.rbegin(); itr != target->deJureVassals.rend(); ++itr)
		(*itr)->setDeJureLiege(this);*/

	// remove the target from its holder and lieges
	target->holder->removeTitle(target);
	if (target->liege)
		target->liege->removeVassal(target);
	/*if (target->deJureLiege)
		target->deJureLiege->removeDeJureVassal(target);*/

	// destroy the target
	target->vassals.clear();
	/*target->deJureVassals.clear();
	target->deJureLiege = NULL;
	target->deJureLiegeString = "";*/
	target->liege = NULL;
	target->liegeString = "";
	/*target->holder = NULL;*/
	target->heir = NULL;

	log("\t%s absorbed %s\n", this->getTitleString().c_str(), target->getTitleString().c_str());

	return true;
}
