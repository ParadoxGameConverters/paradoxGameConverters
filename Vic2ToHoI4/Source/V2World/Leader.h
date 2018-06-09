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


#ifndef VIC2_LEADER_H_
#define VIC2_LEADER_H_



#include "newParser.h"
#include <string>



namespace Vic2
{

class Leader: commonItems::parser
{
	public:
		explicit Leader(std::istream& theStream);

		std::string getName() const { return name; }
		std::string getType() const { return type; }
		double getPrestige() const { return prestige; }
		std::string getPersonality() const { return personality; }
		std::string getBackground() const { return background; }

	private:
		Leader(const Leader&) = delete;
		Leader& operator=(const Leader&) = delete;

		std::string name = "";
		std::string type = "";
		std::string personality = "";
		std::string background = "";
		double prestige = 0.0;
};

}



#endif // VIC2_LEADER_H_