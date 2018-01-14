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



#ifndef HOI4_FOCUS_H
#define HOI4_FOCUS_H



#include <string>
#include <vector>
#include "Object.h"
using namespace std;



class HoI4Focus
{
	public:
		HoI4Focus();
		explicit HoI4Focus(shared_ptr<Object> obj);
		HoI4Focus(const HoI4Focus&) = default;

		friend ostream& operator << (ostream& output, const HoI4Focus& focus);

		shared_ptr<HoI4Focus> makeCustomizedCopy(const string& country) const;

		string id;
		string icon;
		string text;
		vector<string> prerequisites;
		string mutuallyExclusive;
		string bypass;
		int xPos;
		int yPos;
		int cost;
		bool availableIfCapitulated;
		string available;
		string cancelIfInvalid;
		string continueIfInvalid;
		string completeTooltip;
		string completionReward;
		string aiWillDo;

	private:
		HoI4Focus& operator=(const HoI4Focus&) = delete;

		void customizeMutualExclusion(shared_ptr<HoI4Focus> newFocus, const string& country) const;
		void customizePrerequisite(shared_ptr<HoI4Focus> newFocus, string& prerequisite, const string& country) const;
};



#endif // HOI4_FOCUS_H