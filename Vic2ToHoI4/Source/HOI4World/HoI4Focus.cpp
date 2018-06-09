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



#include "HoI4Focus.h"
#include "HoI4Localisation.h"
#include "ParserHelpers.h"



HoI4Focus::HoI4Focus(std::istream& theStream)
{
	registerKeyword(std::regex("id"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString idString(theStream);
			id = idString.getString();
		}
	);
	registerKeyword(std::regex("icon"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString iconString(theStream);
			icon = iconString.getString();
		}
	);
	registerKeyword(std::regex("text"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString textString(theStream);
			text = textString.getString();
		}
	);
	registerKeyword(std::regex("mutually_exclusive"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject mutuallyExclusiveString(theStream);
			mutuallyExclusive = mutuallyExclusiveString.getString();
		}
	);
	registerKeyword(std::regex("bypass"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject bypassString(theStream);
			bypass = bypassString.getString();
		}
	);
	registerKeyword(std::regex("x"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt xPosInt(theStream);
			xPos = xPosInt.getInt();
		}
	);
	registerKeyword(std::regex("y"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt yPosInt(theStream);
			yPos = yPosInt.getInt();
		}
	);
	registerKeyword(std::regex("relative_position_id"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString relativePositionIdString(theStream);
			relativePositionId = relativePositionIdString.getString();
		}
	);
	registerKeyword(std::regex("cost"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleInt costInt(theStream);
			cost = costInt.getInt();
		}
	);
	registerKeyword(std::regex("available_if_capitulated"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString availableIfCapitulatedString(theStream);
			availableIfCapitulated = (availableIfCapitulatedString.getString() == "yes");
		}
	);
	registerKeyword(std::regex("available"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject availableString(theStream);
			available = availableString.getString();
		}
	);
	registerKeyword(std::regex("cancel_if_invalid"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString cancelIfInvalidString(theStream);
			cancelIfInvalid = cancelIfInvalidString.getString();
		}
	);
	registerKeyword(std::regex("continue_if_invalid"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::singleString continueIfInvalidString(theStream);
			continueIfInvalid = continueIfInvalidString.getString();
		}
	);
	registerKeyword(std::regex("complete_tooltip"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject completeTooltipString(theStream);
			completeTooltip = completeTooltipString.getString();
		}
	);
	registerKeyword(std::regex("completion_reward"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject completionRewardString(theStream);
			completionReward = completionRewardString.getString();
		}
	);
	registerKeyword(std::regex("ai_will_do"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject aiWillDoString(theStream);
			aiWillDo = aiWillDoString.getString();
		}
	);
	registerKeyword(std::regex("prerequisite"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::stringOfObject prerequisiteString(theStream);
			prerequisites.push_back(prerequisiteString.getString());
		}
	);

	parseStream(theStream);
}


std::shared_ptr<HoI4Focus> HoI4Focus::makeCustomizedCopy(const std::string& country) const
{
	auto newFocus = std::make_shared<HoI4Focus>(*this);

	newFocus->id += country;

	if (newFocus->relativePositionId != "")
	{
		newFocus->relativePositionId += country;
	}
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


void HoI4Focus::customizeMutualExclusion(std::shared_ptr<HoI4Focus> newFocus, const std::string& country) const
{
	//have to account for several foci in one mututal exclusion, so need to look for occurences of " focus" and insert country before that
	unsigned int stringPosition = 0;
	do
	{
		int focusPosition = newFocus->mutuallyExclusive.find(" focus = ", stringPosition);
		if (focusPosition != std::string::npos)
		{
			int spacePosition = newFocus->mutuallyExclusive.find(" ", focusPosition + 9);
			if (spacePosition == std::string::npos)
			{
				spacePosition = newFocus->mutuallyExclusive.find("}", focusPosition + 9);
			}
			newFocus->mutuallyExclusive.insert(spacePosition, country);
			stringPosition = focusPosition + country.size() + 9;
		}
		else
		{
			stringPosition = newFocus->mutuallyExclusive.size();
		}
	}
	while(stringPosition < newFocus->mutuallyExclusive.size());
}


void HoI4Focus::customizePrerequisite(std::shared_ptr<HoI4Focus> newFocus, std::string& prerequisite, const std::string& country) const
{
	//have to account for several foci in one prerequisite, so need to look for occurences of " focus" and insert country before that
	unsigned int stringPosition = 0;
	do
	{
		int focusPosition = prerequisite.find(" focus = ", stringPosition);
		if (focusPosition != std::string::npos)
		{
			auto spacePosition = prerequisite.find(" ", focusPosition + 9);
			prerequisite.insert(spacePosition, country);
			stringPosition = focusPosition + country.size() + 9;
		}
		else
		{
			stringPosition = prerequisite.size();
		}
	}
	while(stringPosition < prerequisite.size());
	newFocus->prerequisites.push_back(prerequisite);
}


std::ostream& operator << (std::ostream& output, const HoI4Focus& focus)
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
		output << "		prerequisite " << prerequisite << "\n";
	}
	if (focus.mutuallyExclusive != "")
	{
		output << "		mutually_exclusive " << focus.mutuallyExclusive << "\n";
	}
	if (focus.bypass != "")
	{
		output << "		bypass " << focus.bypass << "\n";
	}
	output << "		x = " << focus.xPos << "\n";
	output << "		y = " << focus.yPos << "\n";
	if (focus.relativePositionId != "")
	{
		output << "		relative_position_id = " << focus.relativePositionId << "\n";
	}
	output << "		cost = " << focus.cost << "\n";
	if (focus.availableIfCapitulated)
	{
		output << "		available_if_capitulated = yes\n";
	}
	if (focus.available != "")
	{
		output << "		available " << focus.available << "\n";
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
		output << "		complete_tooltip " << focus.completeTooltip << "\n";
	}
	output << "		completion_reward " << focus.completionReward << "\n";
	if (focus.aiWillDo != "")
	{
		output << "		ai_will_do " << focus.aiWillDo << "\n";
	}

	output << "	}\n";

	return output;
}