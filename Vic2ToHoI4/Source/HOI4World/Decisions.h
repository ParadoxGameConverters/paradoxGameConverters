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



#ifndef HOI4_DECISIONS_H
#define HOI4_DECISIONS_H


#include "newParser.h"
#include "Decision.h"
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>



namespace HoI4
{

class decisionsCategory: commonItems::parser
{
	public:
		decisionsCategory(const std::string& categoryName, std::istream& theStream);

		std::vector<decision> getDecisions() const { return theDecisions; }
		std::string getName() const { return name; }

		void replaceDecision(decision theDecision) { std::replace(theDecisions.begin(), theDecisions.end(), theDecision, theDecision); }
		void addDecision(decision& theDecision) { theDecisions.push_back(theDecision); }

		void updatePoliticalDecisions(const std::set<std::string>& majorIdeologies);

		friend std::ostream& operator<<(std::ostream& outStream, const decisionsCategory& outCategory);
		friend bool operator==(const decisionsCategory& categoryOne, const decisionsCategory& categoryTwo);

	private:
		std::string name = "";
		std::vector<decision> theDecisions;
};

std::ostream& operator<<(std::ostream& outStream, const decisionsCategory& outCategory);
bool operator==(const decisionsCategory& categoryOne, const decisionsCategory& categoryTwo);


class decisions: commonItems::parser
{
	public:
		decisions();

		void updateDecisions(const std::set<std::string>& majorIdeologies);

		void output();

	private:
		void updateStabilityDecisions(const std::set<std::string>& majorIdeologies);
		bool stabilityDecisionToUpdate(const std::string& decisionName);
		std::pair<std::string, std::string> determineIdeologiesForStabilityDecisions(const std::set<std::string>& majorIdeologies);
		std::string updateTimeoutEffect(std::string& originalEffect, const std::pair<std::string, std::string>& ideologiesForStabilityDecisions);

		void updatePoliticalDecisions(const std::set<std::string>& majorIdeologies);

		std::vector<decisionsCategory> stabilityDecisions;
		std::vector<decisionsCategory> politicalDecisions;

		std::multimap<std::string, decisionsCategory> ideologicalDecisions;
};

}




#endif // HOI4_DECISIONS_H