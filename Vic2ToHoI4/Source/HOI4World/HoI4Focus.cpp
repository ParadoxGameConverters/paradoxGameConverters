/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "HoI4Focus.h"



HoI4Focus::HoI4Focus()
{
}


ostream& operator << (ostream& output, HoI4Focus& focus)
{
	output << "	focus = {\n";
	output << "		id = " << focus.id << "\n";
	output << "		icon = " << focus.icon << "\n";
	if (focus.text != "")
	{
		output << "		text = \"" << focus.text << "\"\n";
	}
	for (auto prerequisite: focus.prerequisites)
	{
		output << "		prerequisite = { " << prerequisite << " }\n";
	}
	if (focus.mutuallyExclusive != "")
	{
		output << "		mutually_exclusive = { " << focus.mutuallyExclusive << " }\n";
	}
	if (focus.bypass != "")
	{
		output << "		bypass = {\n";
		output << focus.bypass << "\n";
		output << "		}\n";
	}
	output << "		x = " << focus.xPos << "\n";
	output << "		y = " << focus.yPos << "\n";
	output << "		cost = " << focus.cost << "\n";
	if (focus.available != "")
	{
		output << "		available = {\n";
		output << focus.available;
		output << "\n";
		output << "		}\n";
	}
	if (focus.cancelIfInvalid != "")
	{
		output << "		cancel_if_invalid = " << focus.cancelIfInvalid << "\n";
	}
	if (focus.continueIfInvalid != "")
	{
		output << "		continue_if_invalid = " << focus.continueIfInvalid << "\n";
	}
	if (focus.completeTooltip != "")
	{
		output << "		complete_tooltip = {\n";
		output << focus.completeTooltip << "\n";
		output << "		}\n";
	}
	output << "		completion_reward = {\n";
	output << focus.completionReward << "\n";
	output << "		}\n";
	if (focus.aiWillDo != "")
	{
		output << "		ai_will_do = {\n";
		output << focus.aiWillDo << "\n";
		output << "		}\n";
	}

	output << "	}\n";

	return output;
}