#include "CK2Building.h"
#include "CK2Character.h"
#include "..\Parsers\Object.h"



CK2Building::CK2Building(Object* obj)
{
	vector<Object*> fortObj = obj->getValue("fort_level");
	if (fortObj.size() > 0)
	{
		fortLevel = atof( fortObj[0]->getLeaf().c_str() );
	}
	else
	{
		fortLevel = 0.0f;
	}

	vector<Object*> levyMultObj = obj->getValue("levy_size");
	if (levyMultObj.size() > 0)
	{
		levyMultiplier = atof( levyMultObj[0]->getLeaf().c_str() );
	}
	else
	{
		levyMultiplier = 0.0f;
	}

	numSoldiers = 0.0f;
	vector<Object*> soldiersObj = obj->getValue("light_infantry");
	if (soldiersObj.size() > 0)
	{
		numSoldiers += atof( soldiersObj[0]->getLeaf().c_str() );
	}
	soldiersObj = obj->getValue("heavy_infantry");
	if (soldiersObj.size() > 0)
	{
		numSoldiers += atof( soldiersObj[0]->getLeaf().c_str() );
	}
	soldiersObj = obj->getValue("pikemen");
	if (soldiersObj.size() > 0)
	{
		numSoldiers += atof( soldiersObj[0]->getLeaf().c_str() );
	}
	soldiersObj = obj->getValue("archers");
	if (soldiersObj.size() > 0)
	{
		numSoldiers += atof( soldiersObj[0]->getLeaf().c_str() );
	}
	soldiersObj = obj->getValue("light_cavalry");
	if (soldiersObj.size() > 0)
	{
		numSoldiers += atof( soldiersObj[0]->getLeaf().c_str() );
	}
	soldiersObj = obj->getValue("knights");
	if (soldiersObj.size() > 0)
	{
		numSoldiers += atof( soldiersObj[0]->getLeaf().c_str() );
	}
	soldiersObj = obj->getValue("horse_archers");
	if (soldiersObj.size() > 0)
	{
		numSoldiers += atof( soldiersObj[0]->getLeaf().c_str() );
	}

	vector<Object*> taxObj = obj->getValue("tax_income");
	if (taxObj.size() > 0)
	{
		taxIncome = atof( taxObj[0]->getLeaf().c_str() );
	}
	else
	{
		taxIncome = 0.0f;
	}

	forbiddenReligion			= "";
	requiredReligion			= "";
	acceptableCultures.clear();
	acceptableCultureGroups.clear();
	vector<Object*> potentialObj = obj->getValue("potential");
	vector<Object*> potentialObjs;
	if (potentialObj.size() > 0)
	{
		potentialObjs = potentialObj[0]->getLeaves();
	}
	for (vector<Object*>::iterator potentialItr = potentialObjs.begin(); potentialItr != potentialObjs.end(); potentialItr++)
	{
		if ( (*potentialItr)->getKey() == "religion_group")
		{
			requiredReligion = (*potentialItr)->getLeaf();
		}
		if ( (*potentialItr)->getKey() == "FROM")
		{
			vector<Object*> fromObjs = (*potentialItr)->getLeaves();
			for (vector<Object*>::iterator fromItr = fromObjs.begin(); fromItr != fromObjs.end(); fromItr++)
			{
				if ( (*fromItr)->getKey() == "OR")
				{
					vector<Object*> orObjs = (*fromItr)->getLeaves();
					for (vector<Object*>::iterator orItr = orObjs.begin(); orItr != orObjs.end(); orItr++)
					{
						if ( (*orItr)->getKey() == "culture")
						{
							acceptableCultures.push_back( (*orItr)->getLeaf() );
						}
						if ( (*orItr)->getKey() == "culture_group")
						{
							acceptableCultureGroups.push_back( (*orItr)->getLeaf() );
						}
					}
				}
				if ( (*fromItr)->getKey() == "culture")
				{
					acceptableCultures.push_back( (*fromItr)->getLeaf() );
				}
				if ( (*fromItr)->getKey() == "culture_group")
				{
					acceptableCultureGroups.push_back((*fromItr)->getLeaf());
				}
				if ( (*fromItr)->getKey() == "religion_group")
				{
					requiredReligion = (*fromItr)->getLeaf();
				}
			}
		}

		if ((*potentialItr)->getKey() == "NOT")
		{
			vector<Object*> fromObj = (*potentialItr)->getValue("FROM");
			if (fromObj.size() > 0)
			{
				vector<Object*> groupObj = fromObj[0]->getValue("religion_group");
				if (groupObj.size() > 0)
				{
					forbiddenReligion = groupObj[0]->getLeaf();
				}
			}
		}
	}
}


CK2BuildingFactory::CK2BuildingFactory()
{
	buildings.clear();
}


void CK2BuildingFactory::addBuildingTypes(Object* obj)
{
	vector<Object*> holdingObjs = obj->getLeaves();
	for (vector<Object*>::iterator holdingItr = holdingObjs.begin(); holdingItr != holdingObjs.end(); holdingItr++)
	{
		vector<Object*> buildingObjs = (*holdingItr)->getLeaves();
		for (vector<Object*>::iterator buildingsItr = buildingObjs.begin(); buildingsItr != buildingObjs.end(); buildingsItr++)
		{
			string			newBuildingtype	= (*buildingsItr)->getKey();
			CK2Building*	newBulding			= new CK2Building( *buildingsItr );
			buildings.insert( make_pair(newBuildingtype, newBulding) );
		}
	}
}


const CK2Building* CK2BuildingFactory::getBuilding(string type, const CK2Character* baronyHolder) const
{
	const CK2Building* returnMe = NULL;

	map<string, const CK2Building*>::const_iterator itr = buildings.find(type);
	if (itr != buildings.end())
	{
		returnMe = itr->second;
		if (itr->second->getForbiddenReligion() != "")
		{
			if	(itr->second->getForbiddenReligion() == "christian")
			{
				if	(
						(baronyHolder->getReligion() == "catholic") ||
						(baronyHolder->getReligion() == "cathar") ||
						(baronyHolder->getReligion() == "fraticelli") ||
						(baronyHolder->getReligion() == "waldensian") ||
						(baronyHolder->getReligion() == "lollard") ||
						(baronyHolder->getReligion() == "orthodox") ||
						(baronyHolder->getReligion() == "bogomilist") ||
						(baronyHolder->getReligion() == "iconoclast") ||
						(baronyHolder->getReligion() == "monophysite") ||
						(baronyHolder->getReligion() == "nestorian")
					)
				{
					returnMe = NULL;
				}
			}
			else if (itr->second->getForbiddenReligion() == "muslim")
			{
				if	(
						(baronyHolder->getReligion() == "sunni") ||
						(baronyHolder->getReligion() == "zikri") ||
						(baronyHolder->getReligion() == "yazidi") ||
						(baronyHolder->getReligion() == "ibadi") ||
						(baronyHolder->getReligion() == "shiite") ||
						(baronyHolder->getReligion() == "bektashi") ||
						(baronyHolder->getReligion() == "druze") ||
						(baronyHolder->getReligion() == "hurufi")
					)
				{
					returnMe = NULL;
				}
			}
			else if (itr->second->getForbiddenReligion() == "pagan_group")
			{
				if	(
						(baronyHolder->getReligion() == "pagan") ||
						(baronyHolder->getReligion() == "norse_pagan") ||
						(baronyHolder->getReligion() == "tengri_pagan") ||
						(baronyHolder->getReligion() == "baltic_pagan") ||
						(baronyHolder->getReligion() == "finnish_pagan")
					)
				{
					returnMe = NULL;
				}
			}
			else if (itr->second->getForbiddenReligion() == "zoroastrian_group")
			{
				if	(baronyHolder->getReligion() == "zoroastrian")
				{
					returnMe = NULL;
				}
			}
		}

		if (itr->second->getRequiredReligion() != "")
		{
			if	(itr->second->getRequiredReligion() == "christian")
			{
				if	(
						(baronyHolder->getReligion() != "catholic") &&
						(baronyHolder->getReligion() != "cathar") &&
						(baronyHolder->getReligion() != "fraticelli") &&
						(baronyHolder->getReligion() != "waldensian") &&
						(baronyHolder->getReligion() != "lollard") &&
						(baronyHolder->getReligion() != "orthodox") &&
						(baronyHolder->getReligion() != "bogomilist") &&
						(baronyHolder->getReligion() != "iconoclast") &&
						(baronyHolder->getReligion() != "monophysite") &&
						(baronyHolder->getReligion() != "nestorian")
					)
				{
					returnMe = NULL;
				}
			}
			else if (itr->second->getRequiredReligion() == "muslim")
			{
				if	(
						(baronyHolder->getReligion() != "sunni") &&
						(baronyHolder->getReligion() != "zikri") &&
						(baronyHolder->getReligion() != "yazidi") &&
						(baronyHolder->getReligion() != "ibadi") &&
						(baronyHolder->getReligion() != "shiite") &&
						(baronyHolder->getReligion() != "bektashi") &&
						(baronyHolder->getReligion() != "druze") &&
						(baronyHolder->getReligion() != "hurufi")
					)
				{
					returnMe = NULL;
				}
			}
			else if (itr->second->getRequiredReligion() == "pagan_group")
			{
				if	(
						(baronyHolder->getReligion() != "pagan") &&
						(baronyHolder->getReligion() != "norse_pagan") &&
						(baronyHolder->getReligion() != "tengri_pagan") &&
						(baronyHolder->getReligion() != "baltic_pagan") &&
						(baronyHolder->getReligion() != "finnish_pagan")
					)
				{
					returnMe = NULL;
				}
			}
			else if (itr->second->getRequiredReligion() == "zoroastrian_group")
			{
				if	(baronyHolder->getReligion() != "zoroastrian")
				{
					returnMe = NULL;
				}
			}
		}
	}

	return returnMe;
}