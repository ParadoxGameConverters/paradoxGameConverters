/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "newParser.h"
#include <fstream>
#include <string>
#include <vector>



namespace HoI4
{

class Event: commonItems::parser
{
	public:
		Event() = default;
		Event(const std::string& type, std::istream& theStream);
		Event(const Event&) = default;

		friend std::ofstream& operator << (std::ofstream& out, const Event& theEvent);

		std::string type = "";
		std::string id = "";
		std::string title = "";
		std::vector<std::string> descriptions;
		std::string picture = "";
		bool majorEvent = false;
		bool triggeredOnly = false;
		bool hidden = false;
		std::string trigger = "";
		std::string meanTimeToHappen = "";
		std::string immediate = "";
		std::vector<std::string> options;

	private:
		Event& operator=(const Event&) = delete;
};

std::ofstream& operator << (std::ofstream& out, const Event& theEvent);

}



#endif // #ifndef HOI4_EVENT_H