/*Copyright (c) 2016 The Paradox Game Converters Project

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



#ifndef HOI4_FOCUS_TREE
#define HOI4_FOCUS_TREE



#include <string>
#include <vector>
using namespace std;



class HoI4Country;



class HoI4FocusTree
{
	public:
		HoI4FocusTree(HoI4Country* CreatingCountr);

		void addDemocracyNationalFocuses(HoI4Country* Home, vector<HoI4Country*> CountriesToContain, int XStart);
		void addMonarchyEmpireNationalFocuses(HoI4Country* Home, HoI4Country* Annexed1, HoI4Country* Annexed2, HoI4Country* Annexed3, HoI4Country* Annexed4, int ProtectorateNumber, int AnnexNumber, int x);

		void output();

		void addLine(string line) { text += line; }
		char* operator += (char* newText) { addLine(newText); return newText; }
		string operator += (const string newText) { addLine(newText); return newText; }
	private:
		string srcCountryTag;

		string text;
};





#endif // HOI4_FOCUS_TREE