/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include "HoI4Localisation.h"



HoI4Focus::HoI4Focus():
	id(""),
	icon(""),
	text(""),
	prerequisites(),
	mutuallyExclusive(""),
	bypass(""),
	xPos(0),
	yPos(0),
	cost(0),
	availableIfCapitulated(false),
	available(""),
	cancelIfInvalid(""),
	continueIfInvalid(""),
	completeTooltip(""),
	completionReward(""),
	aiWillDo("")
{
}


HoI4Focus::HoI4Focus(shared_ptr<Object> obj):
	id(obj->safeGetString("id")),
	icon(obj->safeGetString("icon")),
	text(obj->safeGetString("text")),
	prerequisites(),
	mutuallyExclusive(obj->safeGetString("mutuallyExclusive")),
	bypass(obj->safeGetString("bypass")),
	xPos(obj->safeGetInt("x")),
	yPos(obj->safeGetInt("y")),
	cost(obj->safeGetInt("cost")),
	availableIfCapitulated(obj->safeGetString("available_if_capitulated", "no") == "yes"),
	available(obj->safeGetString("available")),
	cancelIfInvalid(obj->safeGetString("cancelIfInvalid")),
	continueIfInvalid(obj->safeGetString("continueIfInvalid")),
	completeTooltip(obj->safeGetString("completeTooltip")),
	completionReward(obj->safeGetString("completion_reward")),
	aiWillDo(obj->safeGetString("aiWillDo"))
{
	for (auto prereqObj: obj->getValue("prerequisite"))
	{
		prerequisites.push_back(prereqObj->getLeaf());
	}
}


shared_ptr<HoI4Focus> HoI4Focus::makeCustomizedCopy(const string& country) const
{
	auto newFocus = make_shared<HoI4Focus>(*this);

	newFocus->id += country;

	if (newFocus->mutuallyExclusive != "")
	{
		customizeMutualExclusion(newFocus, country);
	}

	newFocus->prerequisites.clear();
	for (auto prerequisite: prerequisites)
	{
		customizePrerequisite(newFocus, prerequisite, country);
	}

	HoI4Localisation::copyFocusLocalisations(id, newFocus->id);

	return newFocus;
}


void HoI4Focus::customizeMutualExclusion(shared_ptr<HoI4Focus> newFocus, const string& country) const
{
	//have to account for several foci in one mututal exclusion, so need to look for occurences of " focus" and insert country before that
	unsigned int stringPosition = 0;
	do
	{
		int focusPosition = newFocus->mutuallyExclusive.find(" focus", stringPosition);
		if (focusPosition != string::npos)
		{
			newFocus->mutuallyExclusive.insert(focusPosition, country);
			stringPosition = focusPosition + country.size() + 6;
		}
		else
		{
			stringPosition = newFocus->mutuallyExclusive.size();
		}
	}
	while(stringPosition < newFocus->mutuallyExclusive.size());
	newFocus->mutuallyExclusive += country;
}


void HoI4Focus::customizePrerequisite(shared_ptr<HoI4Focus> newFocus, string& prerequisite, const string& country) const
{
	//have to account for several foci in one prerequisite, so need to look for occurences of " focus" and insert country before that
	unsigned int stringPosition = 0;
	do
	{
		int focusPosition = prerequisite.find(" focus", stringPosition);
		if (focusPosition != string::npos)
		{
			prerequisite.insert(focusPosition, country);
			stringPosition = focusPosition + country.size() + 6;
		}
		else
		{
			stringPosition = prerequisite.size();
		}
	}
	while(stringPosition < prerequisite.size());
	newFocus->prerequisites.push_back(prerequisite + country);
}


ostream& operator << (ostream& output, const HoI4Focus& focus)
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
	if (focus.availableIfCapitulated)
	{
		output << "		available_if_capitulated = yes\n";
	}
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