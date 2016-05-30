/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "EU3Religion.h"
#include "../Parsers/Object.h"


map<string, EU3Religion*> EU3Religion::all_religions;


void EU3Religion::parseReligions(Object* obj)
{
	vector<Object*> groups = obj->getLeaves();
	for (vector<Object*>::iterator groupsItr = groups.begin(); groupsItr < groups.end(); groupsItr++)
	{
		vector<Object*> religions = (*groupsItr)->getLeaves();
		string group = (*groupsItr)->getKey();
		for (vector<Object*>::iterator religionsItr = religions.begin(); religionsItr < religions.end(); religionsItr++)
		{
			string key = (*religionsItr)->getKey();
			if ((key == "defender_of_faith") || (key == "crusade_name"))
			{
				continue;
			}
			EU3Religion* religion = new EU3Religion((*religionsItr), group);
			map<string, EU3Religion*>::iterator itr = all_religions.find(group);
			if (itr != all_religions.end())
			{
				delete itr->second;
			}
			all_religions[religion->name] = religion;
		}
	}
}


EU3Religion::EU3Religion(Object* obj, string _group)
{
	group = _group;
	name = obj->getKey();
}


// e.g. catholic <-> catholic
bool EU3Religion::isSameReligion(const EU3Religion* other) const
{
	if (name == other->name)
		return true;

	return false;
}


// e.g. orthodox <-> catholic
bool EU3Religion::isRelatedTo(const EU3Religion* other) const
{
	// not the same religion
	if (isSameReligion(other))
		return false;

	// but in the same group
	if (group == other->group)
		return true;

	return false;
}


// e.g. sunni <-> catholic
bool EU3Religion::isInfidelTo(const EU3Religion* other) const
{
	// different religious groups == infidel
	if (group != other->group)
		return true;

	return false;
}


EU3Religion* EU3Religion::getReligion(string name)
{
	map<string, EU3Religion*>::iterator itr = all_religions.find(name);
	if (itr == all_religions.end())
		return NULL;

	return itr->second;
}
