/*Copyright (c) 2017 The Paradox Game Converters Project

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


/*Copyright (c) 2010 Rolf Andreassen

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



#include "Object.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include <sstream> 
#include <fstream>
#include <algorithm>
#include <iostream>
#include <assert.h>



Object::Object(string k) :
objects(),
strVal(),
leaf(false),
isObjList(false)
{
	key = k;
}


Object::~Object()
{
}


Object::Object(shared_ptr<Object> other) :
objects(),
strVal(other->strVal),
leaf(other->leaf),
isObjList(other->isObjList)
{
	key = other->key;
	for (auto i: other->objects)
	{
		objects.push_back(i);
	}
}


void Object::setValue(string val)
{
	strVal = val;
	leaf = true;
}


void Object::setValue(shared_ptr<Object> val)
{
	objects.push_back(val);
	leaf = false;
}


void Object::unsetValue(string val)
{
	for (auto i: objects)
	{
		if (i->getKey() != val)
		{
			continue;
		}
		i = objects.back();
		objects.pop_back();
	}
}


void Object::setLeaf(string key, string val)
{
	shared_ptr<Object> leaf = make_shared<Object>(key);
	leaf->setValue(val);
	setValue(leaf);
}


void Object::setValue(vector<shared_ptr<Object>> val)
{
	objects = val;
}


void Object::addToList(string val)
{
	isObjList = true;
	if (strVal.size() > 0)
	{
		strVal += " \"";
	}
	else
	{
		strVal += "\"";
	}
	strVal += val;
	strVal += "\"";
	tokens.push_back(val);
}


void Object::addToList(vector<string>::iterator begin, vector<string>::iterator end)
{
	isObjList = true;
	for (auto itr = begin; itr != end; ++itr)
	{
		if (strVal.size() > 0)
		{
			strVal += "\" \"";
		}
		else
		{
			strVal += "\"";
		}
		strVal += *itr;
	}
	strVal += "\"";
	tokens.insert(tokens.end(), begin, end);
}


vector<shared_ptr<Object>> Object::getValue(string key) const
{
	vector<shared_ptr<Object>> ret;

	for (auto i: objects)
	{
		if (i->getKey() != key)
		{
			continue;
		}
		ret.push_back(i);
	}
	return ret;
}


string Object::getToken(const int index)
{
	if (!isObjList)
	{
		return "";
	}
	if (index >= (int)tokens.size())
	{
		return "";
	}
	if (index < 0)
	{
		return "";
	}
	return tokens[index];
}


int Object::numTokens()
{
	if (!isObjList)
	{
		return 0;
	}
	return tokens.size();
}


vector<string> Object::getKeys()
{
	vector<string> ret;	// the keys to return
	for (auto i: objects)
	{
		string curr = i->getKey();	// the current key
		if (find(ret.begin(), ret.end(), curr) != ret.end())
		{
			continue;
		}
		ret.push_back(curr);
	}
	return ret;
}


string Object::getLeaf(string leaf) const
{
	vector<shared_ptr<Object>> leaves = getValue(leaf); // the objects to return
	if (0 == leaves.size())
	{
		LOG(LogLevel::Error) << "Error: Cannot find leaf " << leaf << " in object\n" << *this;
		assert(leaves.size());
	}
	return leaves[0]->getLeaf();
}


ostream& operator<< (ostream& os, const Object& obj)
{
	static int indent = 0; // the level of indentation to output to
	for (int i = 0; i < indent; i++)
	{
		os << "\t";
	}
	if (obj.leaf) {
		os << obj.key << "=" << obj.strVal << "\n";
		return os;
	}
	if (obj.isObjList)
	{
		os << obj.key << "={" << obj.strVal << " }\n";
		return os;
	}

	if (obj.getKey() != "topLevel")
	{
		os << obj.key << "=\n";
		for (int i = 0; i < indent; i++)
		{
			os << "\t";
		}
		os << "{\n";
		indent++;
	}
	for (auto i: obj.objects)
	{
		os << *i;
	}
	if (obj.getKey() != "topLevel")
	{
		indent--;
		for (int i = 0; i < indent; i++)
		{
			os << "\t";
		}
		os << "}\n";
	}
	return os;
}


void Object::keyCount()
{
	if (leaf)
	{
		cout << key << " : 1\n";
		return;
	}

	map<string, int> refCount;	// the count of the references
	keyCount(refCount);
	vector<pair<string, int> > sortedCount; // an organized container for the counts
	for (auto i: refCount)
	{
		pair<string, int> curr(i.first, i.second);
		if (2 > curr.second)
		{
			continue;
		}
		if ((0 == sortedCount.size()) || (curr.second <= sortedCount.back().second))
		{
			sortedCount.push_back(curr);
			continue;
		}

		for (auto j = sortedCount.begin(); j != sortedCount.end(); ++j)
		{
			if (curr.second < (*j).second)
			{
				continue;
			}
			sortedCount.insert(j, 1, curr);
			break;
		}
	}

	for (auto j: sortedCount)
	{
		cout << j.first << " : " << j.second << "\n";
	}
}


void Object::keyCount(map<string, int>& counter)
{
	for (auto i: objects)
	{
		counter[i->key]++;
		if (i->leaf)
		{
			continue;
		}
		i->keyCount(counter);
	}
}


void Object::printTopLevel()
{
	for (auto i: objects)
	{
		cout << i->key << endl;
	}
}


void Object::removeObject(shared_ptr<Object> target)
{
	vector<shared_ptr<Object>>::iterator pos = find(objects.begin(), objects.end(), target);	// the position of the object to be removed
	if (pos == objects.end())
	{
		return;
	}
	objects.erase(pos);
}


void Object::addObject(shared_ptr<Object> target)
{
	objects.push_back(target);
}


void Object::addObjectAfter(shared_ptr<Object> target, string key)
{
	vector<shared_ptr<Object>>::iterator i;

	for (auto i = objects.begin(); i != objects.end(); ++i)
	{
		if ((*i)->getKey() == key)
		{
			objects.insert(i, target);
			break;
		}
	}

	if (i == objects.end())
	{
		objects.push_back(target);
	}
}



shared_ptr<Object> br = nullptr;	// the branch being set
void setVal(string name, const string val, shared_ptr<Object> branch)
{
	if ((branch) && (br != branch))
	{
		br = branch;
	}
	shared_ptr<Object> b = make_shared<Object>(name);
	b->setValue(val);
	br->setValue(b);
}


void setInt(string name, const int val, shared_ptr<Object> branch)
{
	if ((branch) && (br != branch))
	{
		br = branch;
	}
	static char strbuffer[1000];	// the text to add to the branch
	sprintf_s(strbuffer, 1000, "%i", val);
	shared_ptr<Object> b = make_shared<Object>(name);
	b->setValue(strbuffer);
	br->setValue(b);
}


void setFlt(string name, const double val, shared_ptr<Object> branch)
{
	if ((branch) && (br != branch))
	{
		br = branch;
	}
	static char strbuffer[1000];	// the text to add to the branch
	sprintf_s(strbuffer, 1000, "%.3f", val);
	shared_ptr<Object> b = make_shared<Object>(name);
	b->setValue(strbuffer);
	br->setValue(b);
}

double Object::safeGetFloat(string k, const double def)
{
	objvec vec = getValue(k);	// the objects with the keys to be returned
	if (0 == vec.size()) return def;
	return stof(vec[0]->getLeaf());
}

string Object::safeGetString(string k, string def)
{
	objvec vec = getValue(k);	// the objects with the strings to be returned
	if (0 == vec.size())
	{
		return def;
	}
	return vec[0]->getLeaf();
}

int Object::safeGetInt(string k, const int def)
{
	objvec vec = getValue(k);	// the objects with the ints to be returned
	if (0 == vec.size())
	{
		return def;
	}
	return stoi(vec[0]->getLeaf());
}

shared_ptr<Object> Object::safeGetObject(string k, shared_ptr<Object> def)
{
	objvec vec = getValue(k);	// the objects with the objects to be returned 
	if (0 == vec.size())
	{
		return def;
	}
	return vec[0];
}


string Object::toString() const
{
	ostringstream blah;	// the output string
	blah << *(this);
	return blah.str();
}

