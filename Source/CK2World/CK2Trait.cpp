#include "CK2Trait.h"
#include "..\Parsers\Object.h"


static inline int GetOrZero(Object* obj, string name)
{
	vector<Object*> propObj = obj->getValue(name);
	if (propObj.size() > 0)
		return atoi( propObj[0]->getLeaf().c_str());
	else
		return 0;
}

CK2Trait::CK2Trait(Object* obj)
{
	name = obj->getKey();
	vector<Object*> oppositeObjs = obj->getValue("opposites");
	if (oppositeObjs.size() > 0)
	{
		opposites = oppositeObjs[0]->getTokens();
	}

	// stats
	diplomacy = GetOrZero(obj, "diplomacy");
	martial = GetOrZero(obj, "martial");
	stewardship = GetOrZero(obj, "stewardship");
	intrigue = GetOrZero(obj, "intrigue");
	learning = GetOrZero(obj, "learning");

	// opinion scores
	vassal_opinion = GetOrZero(obj, "vassal_opinion");
	liege_opinion = GetOrZero(obj, "liege_opinion");
	sex_appeal_opinion = GetOrZero(obj, "sex_appeal_opinion");
	same_opinion = GetOrZero(obj, "same_opinion");
	opposite_opinion = GetOrZero(obj, "opposite_opinion");
	church_opinion = GetOrZero(obj, "church_opinion");
	same_opinion_if_same_religion = GetOrZero(obj, "same_opinion_if_same_religion");
	twin_opinion = GetOrZero(obj, "twin_opinion");
	spouse_opinion = GetOrZero(obj, "spouse_opinion");
	same_religion_opinion = GetOrZero(obj, "same_religion_opinion");
	infidel_opinion = GetOrZero(obj, "infidel_opinion");
	muslim_opinion = GetOrZero(obj, "muslim_opinion");
	dynasty_opinion = GetOrZero(obj, "dynasty_opinion");
}


bool CK2Trait::isOppositeOf(CK2Trait* other) const
{
	for (vector<string>::const_iterator itr = opposites.begin(); itr != opposites.end(); ++itr)
	{
		if ((*itr) == other->name)
			return true;
	}
	return false;
}