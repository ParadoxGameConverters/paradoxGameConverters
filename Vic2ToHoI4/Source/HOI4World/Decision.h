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



#ifndef HOI4_DECISION_H
#define HOI4_DECISION_H


#include "newParser.h"
#include <optional>
#include <string>



namespace HoI4
{

class decision: commonItems::parser
{
	public:
		decision(const std::string& decisionName, std::istream& theStream);

		std::string getName() const { return name; }
		std::string getTimeoutEffect() const { return timeoutEffect; }

		void setAvailable(const std::string& newAvailable) { available = newAvailable; }
		void setCompleteEffect(const std::string& newEffect) { completeEffect = newEffect; }
		void setTimeoutEffect(const std::string& newEffect) { timeoutEffect = newEffect; }
		void setModifier(const std::string& newModifier) { modifier = newModifier; }

		friend std::ostream& operator<<(std::ostream& outStream, const HoI4::decision& outDecision);
		friend bool operator==(const HoI4::decision& decisionOne, const HoI4::decision& decisionTwo);

	private:
		std::string name = "";
		std::string icon = "";
		std::string isGood = "";
		std::string allowed = "";
		std::string available = "";
		std::string daysMissionTimeout = "";
		std::string activation = "";
		std::string targetTrigger = "";
		std::string removeTrigger = "";
		std::string visible = "";
		std::string cancelTrigger = "";
		std::string remove = "";
		std::string completeEffect = "";
		std::string removeEffect = "";
		std::string timeoutEffect = "";
		std::string aiWillDo = "";
		std::string fireOnlyOnce = "";
		std::optional<int> daysRemove = {};
		std::optional<int> daysReEnable = {};
		std::string cost = "";
		std::string modifier = "";
};

std::ostream& operator<<(std::ostream& outStream, const HoI4::decision& outDecision);
bool operator==(const HoI4::decision& decisionOne, const HoI4::decision& decisionTwo);

}




#endif // HOI4_DECISION_H