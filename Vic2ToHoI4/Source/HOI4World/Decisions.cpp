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



#include "Decisions.h"
#include "Decision.h"
#include "../Configuration.h"
#include <fstream>



HoI4::decisions::decisions()
{
	std::vector<decisionsCategory> tempDecisionCategories;

	registerKeyword(std::regex("[A-Za-z\\_]+"), [this, &tempDecisionCategories](const std::string& categoryName, std::istream& theStream)
	{
		decisionsCategory category(categoryName, theStream);
		tempDecisionCategories.push_back(category);
	});

	parseFile("blankmod/output/common/decisions/stability_war_support.txt");
	stabilityDecisions.insert(stabilityDecisions.end(), tempDecisionCategories.begin(), tempDecisionCategories.end());

	tempDecisionCategories.clear();
}


HoI4::decisionsCategory::decisionsCategory(const std::string& categoryName, std::istream& theStream):
	name(categoryName)
{
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), [this](const std::string& decisionName, std::istream& theStream)
	{
		decision theDecision(decisionName, theStream);
		theDecisions.push_back(theDecision);
	});

	parseStream(theStream);
}


ostream& HoI4::operator<<(ostream& outStream, const decisionsCategory& outCategory)
{
	outStream << outCategory.name << " = {\n";
	for (auto decision: outCategory.theDecisions)
	{
		outStream << decision << "\n";
	}
	if (outCategory.theDecisions.size() == 0)
	{
		outStream << "\n";
	}
	outStream << "}\n";
	outStream << "\n";

	return outStream;
}


void HoI4::decisions::output()
{
	std::ofstream outStream("output/" + Configuration::getOutputName() + "/common/decisions/stability_war_support.txt");
	for (auto category: stabilityDecisions)
	{
		outStream << category;
	}

	outStream.close();
}