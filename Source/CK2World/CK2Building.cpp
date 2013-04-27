#include "CK2Building.h"
#include "CK2Character.h"
#include "CK2Religion.h"
#include "..\Parsers\Object.h"
#include "..\Log.h"



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

	vector<Object*> techObj = obj->getValue("tech_growth_modifier");
	if (techObj.size() > 0)
	{
		techBonus = atof( techObj[0]->getLeaf().c_str() );
	}
	else
	{
		techBonus = 0.0f;
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


map<string, const CK2Building*> CK2BuildingFactory::buildings;


CK2BuildingFactory::CK2BuildingFactory(const cultureGroupMapping* _cultureGroupMap)
{
	cultureGroupMap = _cultureGroupMap;
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

	string type2;

	map<string, const CK2Building*>::const_iterator itr = buildings.find(type);
	if (itr == buildings.end())
	{
		type2 = type.substr(0, type.find("_mus"));
		itr = buildings.find(type2);
	}
	if (itr != buildings.end())
	{
		if ( (itr->second->getAcceptableCultureGroups().size() == 0) && (itr->second->getAcceptableCultures().size() == 0) )
		{
			returnMe = itr->second;
		}
		else
		{
			vector<string> acceptableCultureGroups = itr->second->getAcceptableCultureGroups();
			for (vector<string>::iterator cultureGroupsItr = acceptableCultureGroups.begin(); cultureGroupsItr < acceptableCultureGroups.end(); cultureGroupsItr++)
			{
				if ( *cultureGroupsItr == cultureGroupMap->find(baronyHolder->getCulture())->second )
				{
					returnMe = itr->second;
					break;
				}
			}

			vector<string> acceptableCultures = itr->second->getAcceptableCultures();
			for (vector<string>::iterator cultureItr = acceptableCultures.begin(); cultureItr < acceptableCultures.end(); cultureItr++)
			{
				if (*cultureItr == baronyHolder->getCulture())
				{
					returnMe = itr->second;
					break;
				}
			}
		}

		if (itr->second->getForbiddenReligion() != "")
		{
			if	( itr->second->getForbiddenReligion() == baronyHolder->getReligion()->getGroup() )
			{
				returnMe = NULL;
			}
		}

		if (itr->second->getRequiredReligion() != "")
		{
			if	( itr->second->getRequiredReligion() != baronyHolder->getReligion()->getGroup() )
			{
				returnMe = NULL;
			}
		}
	}
	else
	{
		log("\tError: Could not find building %s. Also tried %s\n", type.c_str(), type2.c_str());
	}

	return returnMe;
}