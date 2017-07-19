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



HoI4Focus::HoI4Focus()
{
	availableIfCapitulated = false;
}


HoI4Focus::HoI4Focus(shared_ptr<Object> obj)
{
	id = obj->getLeaf("id");

	icon = obj->getLeaf("icon");

	auto textObjs = obj->getValue("text");

	if (textObjs.size() > 0)
	{
		text = textObjs[0]->getLeaf();
	}
	else
	{
		text = "";
	}

	auto mutuallyExclusiveObjs = obj->getValue("mutuallyExclusive");
	if (mutuallyExclusiveObjs.size() > 0)
	{
		mutuallyExclusive = mutuallyExclusiveObjs[0]->getLeaf();
	}
	else
	{
		mutuallyExclusive = "";
	}

	auto bypassObjs = obj->getValue("bypass");
	if (bypassObjs.size() > 0)
	{
		bypass = bypassObjs[0]->getLeaf();
	}
	else
	{
		bypass = "";
	}

	xPos = stoi(obj->getLeaf("x"));
	yPos = stoi(obj->getLeaf("y"));
	cost = stoi(obj->getLeaf("cost"));

	auto availableIfcapitulatedObjs = obj->getValue("available_if_capitulated");
	if (
			(availableIfcapitulatedObjs.size() > 0) &&
			(availableIfcapitulatedObjs[0]->getLeaf() == "yes")
		)
	{
		availableIfCapitulated = true;
	}

	auto availableObjs = obj->getValue("available");
	if (availableObjs.size() > 0)
	{
		available = availableObjs[0]->getLeaf();
	}
	else
	{
		available = "";
	}

	auto cancelIfInvalidObjs = obj->getValue("cancelIfInvalid");
	if (cancelIfInvalidObjs.size() > 0)
	{
		cancelIfInvalid = cancelIfInvalidObjs[0]->getLeaf();
	}
	else
	{
		cancelIfInvalid = "";
	}

	auto continueIfInvalidObjs = obj->getValue("continueIfInvalid");
	if (continueIfInvalidObjs.size() > 0)
	{
		continueIfInvalid = continueIfInvalidObjs[0]->getLeaf();
	}
	else
	{
		continueIfInvalid = "";
	}

	auto completeTooltipObjs = obj->getValue("completeTooltip");
	if (completeTooltipObjs.size() > 0)
	{
		completeTooltip = completeTooltipObjs[0]->getLeaf();
	}
	else
	{
		completeTooltip = "";
	}

	auto completionRewardObjs = obj->getValue("completion_reward");
	if (completionRewardObjs.size() > 0)
	{
		completionReward = completionRewardObjs[0]->getLeaf();
	}
	else
	{
		completionReward = "";
	}

	auto aiWillDoObjs = obj->getValue("aiWillDo");
	if (aiWillDoObjs.size() > 0)
	{
		aiWillDo = aiWillDoObjs[0]->getLeaf();
	}
	else
	{
		aiWillDo = "";
	}

	for (auto prereqObj: obj->getValue("prerequisite"))
	{
		prerequisites.push_back(prereqObj->getLeaf());
	}
}


HoI4Focus* HoI4Focus::makeCustomizedCopy(const string& country) const
{
	HoI4Focus* newFocus = new HoI4Focus(*this);

	newFocus->id += country;

	if (newFocus->mutuallyExclusive != "")
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

	newFocus->prerequisites.clear();
	for (auto prerequisite: prerequisites)
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

	HoI4Localisation::copyFocusLocalisations(id, newFocus->id);

	return newFocus;
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