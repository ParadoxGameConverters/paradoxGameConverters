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



#include "HOI4Version.h"
#include "Log.h"



HOI4Version::HOI4Version():
	first(1),
	second(4),
	third(2)
{
}


HOI4Version::HOI4Version(shared_ptr<Object> obj):
	first(obj->safeGetInt("first")),
	second(obj->safeGetInt("second")),
	third(obj->safeGetInt("third"))
{
}


HOI4Version::HOI4Version(string version):
	first(0),
	second(0),
	third(0)
{
	int dot = version.find_first_of('.');
	first = stoi(version.substr(0, dot));

	version = version.substr(dot + 1, version.size());
	dot = version.find_first_of('.');
	second = stoi(version.substr(0, dot));

	version = version.substr(dot + 1, version.size());
	dot = version.find_first_of('.');
	third = stoi(version.substr(0, dot));
}


bool HOI4Version::operator >= (const HOI4Version& rhs) const
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
	else if ((first == rhs.first) && (second == rhs.second) && (third == rhs.third))
	{
		return true;
	}
	else
	{
		return false;
	}
}


ostream& operator << (ostream& out, const HOI4Version& version)
{
	out << version.first << '.' << version.second << '.' << version.third;

	return out;
}