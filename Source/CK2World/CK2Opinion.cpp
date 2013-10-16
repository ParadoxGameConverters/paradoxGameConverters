/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



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
		int value = 0;
		if (opinionObjs.size() > 0)
		{
				value = atoi(opinionObjs[0]->getLeaf().c_str());
		}
		if (!name.empty() && value != 0)
		{
			opinionVals[name] = value;
		}
	}
}


int CK2Opinion::getBaseValue(string opinion)
{
	map<string,int>::const_iterator itr = opinionVals.find(opinion);
	if (itr == opinionVals.end())
		return 0;
	return itr->second;
}