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



#ifndef HOI4_AI_PEACE
#define HOI4_AI_PEACE



#include "newParser.h"
#include <string>



namespace HoI4
{

class AIPeace: commonItems::parser
{
	public:
		AIPeace(const std::string& theName, std::istream& theStream);

		std::string getName() const { return name; }

		void replaceEnable(const std::string& newEnable) { enable = newEnable; }

		friend std::ostream& operator<<(std::ostream& outStream, const HoI4::AIPeace& outPeace);

	private:
		std::string name;

		std::string enable;

		int annexRandomness = 0;
		int liberateRandomness = 0;
		int puppetRandomness = 0;
		int takeStatesRandomness = 0;
		int forceGovernmentRandomness = 0;

		std::string annex;
		std::string liberate;
		std::string puppet;
		std::string takeStates;
		std::string forceGovernment;
};

std::ostream& operator<<(std::ostream& outStream, const HoI4::AIPeace& outPeace);

}



#endif // HOI4_AI_PEACES
