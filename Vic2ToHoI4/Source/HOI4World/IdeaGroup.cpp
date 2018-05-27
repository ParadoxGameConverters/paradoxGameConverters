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



#include "IdeaGroup.h"
#include "ParserHelpers.h"



HoI4::IdeaGroup::IdeaGroup(const std::string& ideaGroupName, std::istream& theStream):
	name(ideaGroupName)
{
	registerKeyword(std::regex("law"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString lawString(theStream);
		law = (lawString.getString() == "yes");
	});
	registerKeyword(std::regex("designer"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString designerString(theStream);
		designer = (designerString.getString() == "yes");
	});
	registerKeyword(std::regex("[a-zA-Z0-9\\_]+"), [this](const std::string& ideaName, std::istream& theStream){
		Idea newIdea(ideaName, theStream);
		ideas.push_back(newIdea);
	});

	parseStream(theStream);
}


std::optional<HoI4::Idea> HoI4::IdeaGroup::getIdea(const std::string& ideaName)
{
	for (auto idea: ideas)
	{
		if (idea.getName() == ideaName)
		{
			return idea;
		}
	}

	return {};
}


void HoI4::IdeaGroup::replaceIdea(Idea newIdea)
{
	std::replace_if(ideas.begin(), ideas.end(), [newIdea](const Idea& theIdea){ return theIdea.getName() == newIdea.getName(); }, newIdea);
}


std::ostream& HoI4::operator<<(std::ostream& outStream, const HoI4::IdeaGroup& outIdeaGroup)
{
	outStream << "	" << outIdeaGroup.name << " = {\n";
	if (outIdeaGroup.law)
	{
		outStream << "		law = yes\n";
	}
	if (outIdeaGroup.designer)
	{
		outStream << "		designer = yes\n";
	}
	for (auto idea: outIdeaGroup.ideas)
	{
		outStream << "\n";
		outStream << idea;
	}
	outStream << "	}\n";

	return outStream;
}