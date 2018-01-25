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



#ifndef EU4RELIGION_H_
#define EU4RELIGION_H_



#include <string>
#include <map>
#include <memory>
using namespace std;



class Object;



class EU4Religion
{
	public:
		EU4Religion(shared_ptr<Object>, string group);

		// exactly one of these four functions should return true for any given pairing
		bool isSameReligion(const EU4Religion* other) const;	// e.g. catholic <-> catholic
		bool isRelatedTo(const EU4Religion* other) const;		// e.g. orthodox <-> catholic
		bool isInfidelTo(const EU4Religion* other) const;		// e.g. sunni <-> catholic

		static void createSelf();
		static void parseReligions(shared_ptr<Object> obj);
		static EU4Religion* getReligion(string name);

		static map<string, EU4Religion*> getAllReligions() { return all_religions; }

	private:
		string name;		// the religion name
		string group;		// the religion group

		static map<string, EU4Religion*> all_religions;
};

#endif // EU4RELIGION_H_