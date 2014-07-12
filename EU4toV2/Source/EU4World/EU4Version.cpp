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



#include "EU4Version.h"
#include "..\Log.h"



EU4Version::EU4Version()
{
	first		= 0;
	second	= 0;
	third		= 0;
	fourth	= 0;
}


EU4Version::EU4Version(Object* obj)
{
	vector<Object*> numObj;	// the number in this part of the version
	numObj = obj->getValue("first");
	if (numObj.size() > 0)
	{
		first = atoi(numObj[0]->getLeaf().c_str());
	}
	numObj = obj->getValue("second");
	if (numObj.size() > 0)
	{
		second = atoi(numObj[0]->getLeaf().c_str());
	}
	numObj = obj->getValue("third");
	if (numObj.size() > 0)
	{
		third = atoi(numObj[0]->getLeaf().c_str());
	}
	numObj = obj->getValue("forth");
	if (numObj.size() > 0)
	{
		fourth = atoi(numObj[0]->getLeaf().c_str());
	}
}


EU4Version::EU4Version(string version)
{
	int dot = version.find_first_of('.');	// the dots separating the version parts
	first = atoi(version.substr(0, dot).c_str());

	version = version.substr(dot + 1, version.size());
	dot = version.find_first_of('.');
	second = atoi(version.substr(0, dot).c_str());

	version = version.substr(dot + 1, version.size());
	dot = version.find_first_of('.');
	third = atoi(version.substr(0, dot).c_str());

	version = version.substr(dot + 1, version.size());
	dot = version.find_first_of('.');
	fourth = atoi(version.substr(0, dot).c_str());
}


bool EU4Version::operator >= (EU4Version& rhs) const
{
	if (first > rhs.first)
	{
		return true;
	}
	else if ((first == rhs.first) && (second > rhs.second))
	{
		return true;
	}
	else if ((first == rhs.first) && (second == rhs.second) && (third > rhs.third))
	{
		return true;
	}
	else if ((first == rhs.first) && (second == rhs.second) && (third == rhs.third) && (fourth > rhs.fourth))
	{
		return true;
	}
	else if ((first == rhs.first) && (second == rhs.second) && (third == rhs.third) && (fourth == rhs.fourth))
	{
		return true;
	}
	else
	{
		return false;
	}
}