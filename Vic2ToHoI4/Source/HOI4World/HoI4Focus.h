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



#ifndef HOI4_FOCUS_H
#define HOI4_FOCUS_H



#include <string>
#include <vector>
#include "newParser.h"



class HoI4Focus: commonItems::parser
{
	public:
		HoI4Focus() = default;
		explicit HoI4Focus(std::istream& theStream);
		HoI4Focus(const HoI4Focus&) = default;

		friend std::ostream& operator << (std::ostream& output, const HoI4Focus& focus);

		std::shared_ptr<HoI4Focus> makeCustomizedCopy(const std::string& country) const;

		std::string id;
		std::string icon;
		std::string text;
		std::vector<std::string> prerequisites;
		std::string mutuallyExclusive;
		std::string bypass;
		int xPos = 0;
		int yPos = 0;
		std::string relativePositionId;
		int cost = 0;
		bool availableIfCapitulated = false;
		std::string available;
		std::string cancelIfInvalid;
		std::string continueIfInvalid;
		std::string completeTooltip;
		std::string completionReward;
		std::string aiWillDo;

	private:
		HoI4Focus& operator=(const HoI4Focus&) = delete;

		void customizeMutualExclusion(std::shared_ptr<HoI4Focus> newFocus, const std::string& country) const;
		void customizePrerequisite(std::shared_ptr<HoI4Focus> newFocus, std::string& prerequisite, const std::string& country) const;
};



#endif // HOI4_FOCUS_H