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



#ifndef HOI4_ADVISOR
#define HOI4_ADVISOR



#include <fstream>
#include <memory>
#include <set>
#include <string>
using namespace std;



class Object;


class HoI4Advisor
{
	public:
		explicit HoI4Advisor(shared_ptr<Object> object);
		void output(ofstream& output, const string& tag) const;
		void addEventNum(const int num);

		string getIdeology() const { return ideology; }

	private:
		HoI4Advisor(const HoI4Advisor&) = delete;
		HoI4Advisor& operator=(const HoI4Advisor&) = delete;

		string trait;
		string picture;
		string event;
		string ideology;
};


struct advisorCompare {
	bool operator() (const HoI4Advisor* lhs, const HoI4Advisor* rhs) const {
		return lhs->getIdeology() < rhs->getIdeology();
	}
};



#endif // HOI4_ADVISOR