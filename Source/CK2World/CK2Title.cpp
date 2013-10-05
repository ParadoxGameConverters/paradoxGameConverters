#include "CK2Title.h"
#include "..\Parsers\Object.h"
#include "CK2World.h"
#include "CK2Character.h"
#include "CK2Title.h"
#include "CK2Dynasty.h"
#include "CK2History.h"
#include "CK2Barony.h"
#include "..\Log.h"
#include <algorithm>
#include "..\Configuration.h"
#include "..\EU3World\EU3Country.h"


CK2Title::CK2Title(string _titleString, int* _color)
{
	titleString			= _titleString;
	holder				= NULL;
	heir					= NULL;
	successionLaw		= "";
	genderLaw			= "";
	nominees.clear();
	history.clear();
	CA						= "";
	feudalContract		= 0;
	templeContract		= 0;
	cityContract		= 0;
	liegeString			= "";
	liege					= NULL;
	vassals.clear();
	deJureLiegeString	= "";
	deJureLiege			= NULL;
	independent			= true;
	inHRE					= false;
	active					= false;
	dstCountry			= NULL;
	settlement			= NULL;
	dynamic				= false;

	color[0]				= _color[0];
	color[1]				= _color[1];
	color[2]				= _color[2];
}


void CK2Title::init(Object* obj,  map<int, CK2Character*>& characters, const CK2BuildingFactory* buildingFactory)
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

	CA					= "";
	feudalContract	= 0;
	templeContract	= 0;
	cityContract	= 0;
	vector<Object*> lawObj = obj->getValue("law");
	for (unsigned int i = 0; i < lawObj.size(); i++)
	{
		if (lawObj[i]->getLeaf().substr(0, 14) == "centralization")
		{
			CA = lawObj[i]->getLeaf();
		}
		else if (lawObj[i]->getLeaf().substr(0, 15) == "feudal_contract")
		{
			feudalContract = atoi( lawObj[i]->getLeaf().substr(16,1).c_str() );
		}
		else if (lawObj[i]->getLeaf().substr(0, 15) == "temple_contract")
		{
			templeContract = atoi( lawObj[i]->getLeaf().substr(16,1).c_str() );
		}
		else if (lawObj[i]->getLeaf().substr(0, 13) == "city_contract")
		{
			cityContract = atoi( lawObj[i]->getLeaf().substr(14,1).c_str() );
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

	active = true;
	vector<Object*> activeObjs = obj->getValue("active");
	if (activeObjs.size() > 0)
	{
		if (activeObjs[0]->getLeaf() == "no")
			active = false;
	}

	vector<Object*> dynObjs = obj->getValue("dynamic");
	if (dynObjs.size() > 0)
	{
		if (dynObjs[0]->getLeaf() == "yes")
			dynamic = true;
	}

	vector<Object*> settlementObjs = obj->getValue("settlement");
	if (settlementObjs.size() > 0)
	{
		settlement = new CK2Barony(settlementObjs[0], this, NULL, buildingFactory);
		if (holder != NULL)
		{
			holder->addHolding(settlement);
			if (holder->getPrimaryHolding() == NULL)	// if no other capital, a title holding will work
				holder->setPrimaryHolding(settlement);
		}
	}

	independent = true;
	inHRE			= false;
}


void CK2Title::setLiege(CK2Title* newLiege)
{
	if (liege)
	{
		liege->removeVassal(this);
	}

	liege = newLiege;
	liege->addVassal(this);
	liegeString = newLiege->getTitleString();

	independent = false;
}


void CK2Title::addVassal(CK2Title* vassal)
{
	vassals.push_back(vassal);
	if ((vassal->getTitleString().substr(0,2) != "e_") || (vassal->getTitleString().substr(0,2) != "k_"))
	{
		vassal->setSuccessionLaw(successionLaw);
	}
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
		else if (successionLaw == "ultimogeniture")
		{
			CK2Character* tempHolder = holder;
			do
			{
				heir = tempHolder->getUltimogenitureHeir(genderLaw, holder);
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
		else if ( (successionLaw == "feudal_elective") || (successionLaw == "tanistry") )
		{
			heir = getElectiveHeir(characters);
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


void CK2Title::setSuccessionLaw(string _successionLaw)
{
	successionLaw = _successionLaw;
	for (vector<CK2Title*>::iterator vassalItr = vassals.begin(); vassalItr != vassals.end(); vassalItr++)
	{
		if (((*vassalItr)->getTitleString().substr(0,2) != "e_") || ((*vassalItr)->getTitleString().substr(0,2) != "k_"))
		{
			(*vassalItr)->setSuccessionLaw(successionLaw);
		}
	}
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
		else
		{
			log("Error: Could not find de jure liege %s for %s.\n", deJureLiegeString.c_str(), titleString.c_str());
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
			int color[3] = {0, 0, 0};
			vector<Object*> colorObjs = (*itr)->getValue("color");
			if (colorObjs.size() > 0)
			{
				color[0] = atoi(colorObjs[0]->getTokens()[0].c_str() );
				color[1] = atoi(colorObjs[0]->getTokens()[1].c_str() );
				color[2] = atoi(colorObjs[0]->getTokens()[2].c_str() );
			}
			CK2Title* newTitle = new CK2Title( (*itr)->getKey(), color);
			titles.insert( make_pair((*itr)->getKey(), newTitle) );
			titleItr = titles.find( (*itr)->getKey() );
		}
		else
		{
			log("\t\tMultiple definitions of de jure title! %s\n", titleItr->first.c_str());
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

	if (holder != NULL)
	{
		CK2Province* capital = holder->getCapital();
		if (capital != NULL)
		{
			string culture = determineEU3Culture(holder->getCulture(), cultureMap, capital);
			cultureWeights[culture] += weight;
		}
	}
}

CK2Character* CK2Title::getElectiveHeir(map<int, CK2Character*>& characters)
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
	target->holder = NULL;
	target->heir = NULL;

	target->setLiege(this);

	return true;
}

bool CK2Title::hasUnionWith(CK2Title* other, bool& otherDominant) const
{
	// no unions between different holders
	if (this->holder != other->holder)
		return false;

	// if this is the primary title, yes, and other is not dominant
	if (holder->getPrimaryTitle() == this)
	{
		otherDominant = false;
		return true;
	}
	// if the other title is the primary title, yes, but other is dominant
	else if (holder->getPrimaryTitle() == other)
	{
		otherDominant = true;
		return true;
	}

	// no unions between junior titles (senior title has union with ALL junior titles)
	return false;
}

bool CK2Title::hasRMWith(CK2Title* other) const
{
	return (this->holder->isRMWith(other->holder));
}

bool CK2Title::hasAllianceWith(CK2Title* other) const
{
	return (this->holder->isAlliedWith(other->holder));
}

int CK2Title::getRelationsWith(CK2Title* other, CK2Version& version) const
{
	// FIXME: forum suggests taking vassal relations into account too
	int ltr = this->holder->getOpinionOf(other->holder, version);
	int rtl = other->holder->getOpinionOf(this->holder, version);
	return ((ltr + rtl) / 2);
}

// sometimes we NEED a valid holder, but might not have one (if the title was destroyed by the player or eaten by the merge process)
// this gets used for religion/culture/etc...they need to be set for even dead titles with de jure territory, so that released cores in EU3 work right.
CK2Character* CK2Title::getLastHolder() const
{
	if (holder)
		return holder;

	for (vector<CK2History*>::const_reverse_iterator itr = history.rbegin(); itr != history.rend(); ++itr)
	{
		if ((*itr)->getHolder())
			return (*itr)->getHolder();
	}

	// title never had a holder?  that's a small problem.
	log("\tWarning: title %s never had a holder.  Using arbitrary de jure vassal for EU3 country parameters.\n", titleString.c_str());
	// since we mostly just need a valid character to set religion, culture, etc, let's go through the de jure vassals until we find one.
	for (vector<CK2Title*>::const_iterator itr = deJureVassals.begin(); itr != deJureVassals.end(); ++itr)
	{
		CK2Character* ret = (*itr)->getLastHolder(); // can recurse
		if (ret)
			return ret;
	}

	log("Error: couldn't find any character to use for title %s conversion.  I will probably crash now.\n", titleString.c_str());
	return NULL;
}