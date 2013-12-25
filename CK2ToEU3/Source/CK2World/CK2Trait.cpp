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