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



#ifndef HOI4_IDEAGROUP_H
#define HOI4_IDEAGROUP_H


#include "newParser.h"
#include "Idea.h"
#include <optional>
#include <vector>



namespace HoI4
{

class IdeaGroup: commonItems::parser
{
	public:
		IdeaGroup(const std::string& ideaGroupName, std::istream& theStream);

		std::string getName() const { return name; }
		std::optional<Idea> getIdea(const std::string& ideaName);
		void replaceIdea(Idea newIdea);

		friend std::ostream& operator<<(std::ostream& outStream, const HoI4::IdeaGroup& outIdeaGroup);

	private:
		std::string name;

		bool law = false;
		bool designer = false;

		std::vector<Idea> ideas;
};

std::ostream& operator<<(std::ostream& outStream, const HoI4::IdeaGroup& outIdeaGroup);

}



#endif // HOI4_IDEAGROUP_H