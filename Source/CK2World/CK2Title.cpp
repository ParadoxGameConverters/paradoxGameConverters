#include "CK2Title.h"



void CK2Title::init(Object* obj)
{
	titleString = obj->getKey();
	vector<Object*> liegeObjs = obj->getValue("liege");
	if (liegeObjs.size() > 0)
	{
		liegeString = liegeObjs[0]->getLeaf();
	}
}


void CK2Title::addLiege(CK2Title* newLiege)
{
	liege = newLiege;
}


void CK2Title::addVassal(CK2Title* vassal)
{
	vassals.push_back(vassal);
}


string CK2Title::getTitleString()
{
	return titleString;
}


string CK2Title::getLiegeString()
{
	return liegeString;
}