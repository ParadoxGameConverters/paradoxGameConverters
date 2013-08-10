#include "CK2Opinion.h"
#include "../Parsers/Object.h"
#include "../Log.h"


map<string, int> CK2Opinion::opinionVals;


CK2Opinion::CK2Opinion(Object* obj) : value(0), multiplier(1)
{
	if (obj->getKey() == "truce")
	{
		// FIXME: handle truces?
	}
	else if (obj->getKey() == "casus_belli")
	{
		// FIXME: handle CBs?
	}
	else if (obj->getKey() == "raised_days")
	{
		int raised_days = atoi(obj->getLeaf().c_str());
		value = -raised_days / 90; // FIXME: there's some other factor here that I don't know.
	}
	else if (obj->getKey() == "ruled_years")
	{
		int yearsRuled = atoi(obj->getLeaf().c_str());
		if (yearsRuled < 10)
		{
			value = 2 * (10 - yearsRuled); // TODO: legalism decreases this penalty by one year per point
		}
		else
		{
			value = yearsRuled - 10;
		}
	}
	else if (obj->getKey() == "looter_hostility_days")
	{
		// do nothing
	}
	else if (obj->getKey() == "is_looter")
	{
		// do nothing
	}
	else // opinion modifier
	{
		vector<Object*> modifierObjs = obj->getValue("modifier");
		if (modifierObjs.size() > 0)
		{
			value = getBaseValue(modifierObjs[0]->getLeaf());
		}
		else
		{
			log("\tError: bad relationship modifier in %s\n", obj->getKey().c_str());
		}

		vector<Object*> multObjs = obj->getValue("multiplier");
		if (multObjs.size() > 0)
		{
			multiplier = atoi(multObjs[0]->getLeaf().c_str());
		}
	}
}


void CK2Opinion::initOpinions(Object* root)
{
	vector<Object*> opinions = root->getLeaves();
	for (vector<Object*>::iterator itr = opinions.begin(); itr != opinions.end(); ++itr)
	{
		string name = (*itr)->getKey();
		vector<Object*> opinionObjs = (*itr)->getValue("opinion");
		int value = atoi(opinionObjs[0]->getLeaf().c_str());
		if (!name.empty() && value != 0)
			opinionVals[name] = value;
	}
}


int CK2Opinion::getBaseValue(string opinion)
{
	map<string,int>::const_iterator itr = opinionVals.find(opinion);
	if (itr == opinionVals.end())
		return 0;
	return itr->second;
}