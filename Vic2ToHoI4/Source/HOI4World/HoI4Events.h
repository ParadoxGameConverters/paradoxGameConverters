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



#ifndef HOI4_EVENTS_H
#define HOI4_EVENTS_H



#include "HoI4Event.h"
#include <set>
#include <string>
using namespace std;



class HoI4Country;



class HoI4Events
{
	public:
		HoI4Events();

		void output() const;
		void createFactionEvents(const HoI4Country* Leader, HoI4Country* newAlly);
		void createAnnexEvent(const HoI4Country* Annexer, const HoI4Country* Annexed);
		void createSudatenEvent(const HoI4Country* Annexer, const HoI4Country* Annexed, const set<string>& claimedStates);
		void createTradeEvent(const HoI4Country* leader, const HoI4Country* GC);

		int getCurrentNationFocusEventNum() const { return nationalFocusEventNumber; }


	private:
		void outputNationalFocusEvents() const;
		void outputNewsEvents() const;

		vector<HoI4Event> newsEvents;
		int newsEventNumber;
		vector<HoI4Event> nationalFocusEvents;
		int nationalFocusEventNumber;
};




#endif // HOI4_EVENTS_H