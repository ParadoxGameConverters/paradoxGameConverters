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



#ifndef HOI4_EVENT_H
#define HOI4_EVENT_H



#include <fstream>
#include <string>
#include <vector>
using namespace std;



class HoI4Event
{
	public:
		HoI4Event();
		friend ofstream& operator << (ofstream& out, const HoI4Event& theEvent);
		HoI4Event(const HoI4Event&) = default;

		string type;
		string id;
		string title;
		string description;
		string picture;
		bool majorEvent;
		bool triggeredOnly;
		string trigger;
		string meanTimeToHappen;
		string immediate;
		vector<string> options;

	private:
		HoI4Event& operator=(const HoI4Event&) = delete;
};



#endif // #ifndef HOI4_EVENT_H