#include "V2LeaderTraits.h"
#include "Parsers/Parser.h"
#include "Log.h"



V2TraitConversion::V2TraitConversion(Object* obj)
{
	trait = obj->getKey();

	vector<Object*> reqObj = obj->getValue("fire");
	if (reqObj.size() > 0)
	{
		req_fire = atoi(reqObj[0]->getLeaf().c_str());
	}
	else
	{
		req_fire = 0;
	}

	reqObj = obj->getValue("shock");
	if (reqObj.size() > 0)
	{
		req_shock = atoi(reqObj[0]->getLeaf().c_str());
	}
	else
	{
		req_shock = 0;
	}

	reqObj = obj->getValue("manuever");
	if (reqObj.size() > 0)
	{
		req_manuever = atoi(reqObj[0]->getLeaf().c_str());
	}
	else
	{
		req_manuever = 0;
	}

	reqObj = obj->getValue("siege");
	if (reqObj.size() > 0)
	{
		req_siege = atoi(reqObj[0]->getLeaf().c_str());
	}
	else
	{
		req_siege = 0;
	}

	reqObj = obj->getValue("other");
	if (reqObj.size() > 0)
	{
		req_other = atoi(reqObj[0]->getLeaf().c_str());
	}
	else
	{
		req_other = 0;
	}
}


bool V2TraitConversion::matches(int fire, int shock, int manuever, int siege) const
{
	if ( (fire < req_fire) || (shock < req_shock) || (manuever < req_manuever) || (siege < req_siege) )
	{
		return false;
	}

	// other consists of the sum of all non-required attributes
	if (req_other > 0)
	{
		int other	= (req_fire			!= 0 ? 0 : fire);
		other			+= (req_shock		!= 0 ? 0 : shock);
		other			+= (req_manuever	!= 0 ? 0 : manuever);
		other			+= (req_siege		!= 0 ? 0 : siege);

		if (other < req_other)
		{
			return false;
		}
	}

	return true;
}


V2LeaderTraits::V2LeaderTraits()
{
	Object* obj = doParseFile("leader_traits.txt");
	if (obj == NULL)
	{
		log("Could not parse file leader_traits.txt\n");
		exit(-1);
	}

	backgrounds.clear();
	vector<Object*> traitObjs = obj->getValue("background");
	if (traitObjs.size() > 0)
	{
		traitObjs = traitObjs[0]->getLeaves();
		for (vector<Object*>::iterator itr = traitObjs.begin(); itr != traitObjs.end(); ++itr)
		{
			V2TraitConversion tc(*itr);
			backgrounds.push_back(tc);
		}
	}

	personalities.clear();
	traitObjs = obj->getValue("personality");
	if (traitObjs.size() > 0)
	{
		traitObjs = traitObjs[0]->getLeaves();
		for (vector<Object*>::iterator itr = traitObjs.begin(); itr != traitObjs.end(); ++itr)
		{
			V2TraitConversion tc(*itr);
			personalities.push_back(tc);
		}
	}

	if (backgrounds.size() == 0 || personalities.size() == 0)
	{
		printf("Error: Trait conversion failed to initialize!\n");
		log("Error: Trait conversion failed to initialize!\n");
		exit(1);
	}
}


string V2LeaderTraits::getBackground(int fire, int shock, int manuever, int siege) const
{
	for (vector<V2TraitConversion>::const_iterator itr = backgrounds.begin(); itr != backgrounds.end(); ++itr)
	{
		if (itr->matches(fire, shock, manuever, siege))
		{
			return itr->trait;
		}
	}
	return "";
}

string V2LeaderTraits::getPersonality(int fire, int shock, int manuever, int siege) const
{
	for (vector<V2TraitConversion>::const_iterator itr = personalities.begin(); itr != personalities.end(); ++itr)
	{
		if (itr->matches(fire, shock, manuever, siege))
		{
			return itr->trait;
		}
	}
	return "";
}
