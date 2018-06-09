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



#ifndef VIC2_RELATIONS_H_
#define VIC2_RELATIONS_H_



#include "Date.h"
#include "newParser.h"



namespace Vic2
{

class Relations: commonItems::parser
{
	public:
		explicit Relations(const std::string& theTag, std::istream& theStream);

		std::string getTag() const { return tag; }
		int getRelations() const { return value; }
		int getLevel() const { return level; }

		bool hasMilitaryAccess() const { return militaryAccess; }
		const date getDiplomatLastSent() const { return lastSentDiplomat; }
		const date getLastWar() const { return lastWar; }
		const date getTruceUntil() const { return truceUntil; }

	private:
		Relations(const Relations&) = delete;
		Relations& operator=(const Relations&) = delete;

		std::string tag = "";
		int value = 0;
		int level = 2;

		bool militaryAccess = false;
		date lastSentDiplomat;
		date lastWar;
		date truceUntil;
};

}



#endif // VIC2_RELATIONS_H_