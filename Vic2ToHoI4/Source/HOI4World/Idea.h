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



#ifndef HOI4_IDEA_H
#define HOI4_IDEA_H


#include "newParser.h"



namespace HoI4
{

class Idea: commonItems::parser
{
	public:
		Idea(const std::string& ideaName, std::istream& theStream);

		friend std::ostream& operator<<(std::ostream& outStream, const HoI4::Idea& outIdea);

		std::string getName() const { return name; }

		void setAvailable(const std::string& newAvailable) { available = newAvailable; }
		void setAllowedCivilWar(const std::string& newAllowedCivilWar) { allowedCivilWar = newAllowedCivilWar; }

	private:
		std::string name;

		std::optional<int> cost;
		std::optional<int> removalCost;
		std::optional<int> level;

		std::string allowed;
		std::string allowedCivilWar;
		std::string cancel;
		std::string available;
		std::string aiWillDo;
		std::string picture;
		std::string rule;
		std::string modifier;
		std::string researchBonus;
		std::string equipmentBonus;
		std::string traits;
		std::string onAdd;
		std::string allowedToRemove;

		bool isDefault = false;
		std::optional<bool> cancelIfInvalid;
};

std::ostream& operator<<(std::ostream& outStream, const HoI4::Idea& outIdea);

}



#endif // HOI4_IDEA_H