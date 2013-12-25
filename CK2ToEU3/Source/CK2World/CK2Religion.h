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



#ifndef CK2RELIGION_H_
#define CK2RELIGION_H_

#include <string>
#include <map>
using namespace std;

class Object;

class CK2Religion
{
public:
	CK2Religion(Object*, string group);

	// exactly one of these four functions should return true for any given pairing
	bool isSameReligion(const CK2Religion* other) const;
	bool isHereticTo(const CK2Religion* other) const;
	bool isRelatedTo(const CK2Religion* other) const;
	bool isInfidelTo(const CK2Religion* other) const;

	string getName() const { return name; }
	string getGroup() const { return group; }

	static void parseReligions(Object* obj);
	static CK2Religion* getReligion(string name);

private:
	string name;
	string parent;
	string group;

	static map<string, CK2Religion*> all_religions;
};

#endif // CK2RELIGION_H_