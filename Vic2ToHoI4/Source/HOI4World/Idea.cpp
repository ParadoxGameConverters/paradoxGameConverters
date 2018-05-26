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



#include "Idea.h"
#include "ParserHelpers.h"



HoI4::Idea::Idea(const std::string& ideaName, std::istream& theStream):
	name(ideaName)
{
	registerKeyword(std::regex("cost"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt costString(theStream);
		cost = costString.getInt();
	});
	registerKeyword(std::regex("removal_cost"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt costString(theStream);
		removalCost = costString.getInt();
	});
	registerKeyword(std::regex("level"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt levelString(theStream);
		level = levelString.getInt();
	});
	registerKeyword(std::regex("available"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem availableString(theStream);
		available = availableString.getString();
	});
	registerKeyword(std::regex("ai_will_do"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem aiWillDoString(theStream);
		aiWillDo = aiWillDoString.getString();
	});
	registerKeyword(std::regex("modifier"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem modifierString(theStream);
		modifier = modifierString.getString();
	});
	registerKeyword(std::regex("default"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString defaultString(theStream);
		isDefault = (defaultString.getString() == "yes");
	});
	registerKeyword(std::regex("cancel_if_invalid"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString cancelIfInvalidString(theStream);
		cancelIfInvalid = (cancelIfInvalidString.getString() == "yes");
	});

	parseStream(theStream);
}


std::ostream& HoI4::operator<<(std::ostream& outStream, const HoI4::Idea& outIdea)
{
	outStream << "		" << outIdea.name << " = {\n";
	outStream << "			cost = " << outIdea.cost << "\n";
	outStream << "			removal_cost = " << outIdea.removalCost << "\n";
	outStream << "			level = " << outIdea.level << "\n";
	outStream << "			available " << outIdea.available << "\n";
	if (outIdea.aiWillDo != "")
	{
		outStream << "			ai_will_do " << outIdea.aiWillDo << "\n";
	}
	if (outIdea.modifier != "")
	{
		outStream << "			modifier " << outIdea.modifier << "\n";
	}
	if (outIdea.isDefault)
	{
		outStream << "			default = yes\n";
	}
	if (outIdea.cancelIfInvalid)
	{
		outStream << "			cancel_if_invalid = yes\n";
	}
	else
	{
		outStream << "			cancel_if_invalid = no\n";
	}
	outStream << "		}\n";

	return outStream;
}