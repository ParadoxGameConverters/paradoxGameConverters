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



#include "Decision.h"
#include "ParserHelpers.h"



HoI4::decision::decision(const std::string& decisionName, std::istream& theStream):
	name(decisionName)
{
	registerKeyword(std::regex("icon"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString theIcon(theStream);
		icon = theIcon.getString();
	});
	registerKeyword(std::regex("is_good"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString theIsGood(theStream);
		isGood = theIsGood.getString();
	});
	registerKeyword(std::regex("allowed"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theAllowed(theStream);
		allowed = theAllowed.getString();
	});
	registerKeyword(std::regex("available"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theAvailable(theStream);
		available = theAvailable.getString();
	});
	registerKeyword(std::regex("days_mission_timeout"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString theTimeout(theStream);
		daysMissionTimeout = theTimeout.getString();
	});
	registerKeyword(std::regex("activation"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theActivation(theStream);
		activation = theActivation.getString();
	});
	registerKeyword(std::regex("target_trigger"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theTargetTrigger(theStream);
		targetTrigger = theTargetTrigger.getString();
	});
	registerKeyword(std::regex("visible"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theVisible(theStream);
		visible = theVisible.getString();
	});
	registerKeyword(std::regex("cancel_trigger"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theCancelTrigger(theStream);
		cancelTrigger = theCancelTrigger.getString();
	});
	registerKeyword(std::regex("remove_trigger"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theRemoveTrigger(theStream);
		removeTrigger = theRemoveTrigger.getString();
	});
	registerKeyword(std::regex("remove"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theRemove(theStream);
		remove = theRemove.getString();
	});
	registerKeyword(std::regex("complete_effect"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theCompleteEffect(theStream);
		completeEffect = theCompleteEffect.getString();
	});
	registerKeyword(std::regex("remove_effect"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theRemoveEffect(theStream);
		removeEffect = theRemoveEffect.getString();
	});
	registerKeyword(std::regex("timeout_effect"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theTimeoutEffect(theStream);
		timeoutEffect = theTimeoutEffect.getString();
	});
	registerKeyword(std::regex("ai_will_do"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject theAiWillDo(theStream);
		aiWillDo = theAiWillDo.getString();
	});
	registerKeyword(std::regex("days_remove"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleInt theDaysRemove(theStream);
		daysRemove = theDaysRemove.getInt();
	});
	registerKeyword(std::regex("days_re_enable"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleInt theDaysReEnable(theStream);
		daysReEnable = theDaysReEnable.getInt();
	});
	registerKeyword(std::regex("cost"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString theCost(theStream);
		cost = theCost.getString();
	});
	registerKeyword(std::regex("fire_only_once"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString theFire(theStream);
		fireOnlyOnce = theFire.getString();
	});
	registerKeyword(std::regex("modifier"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::stringOfObject themodifier(theStream);
		modifier = themodifier.getString();
	});

	parseStream(theStream);
}


std::ostream& HoI4::operator<<(std::ostream& outStream, const decision& outDecision)
{
	outStream << "\n\t" << outDecision.name << " = {\n";
	if (outDecision.icon != "")
	{
		outStream << "\n\t\ticon = " << outDecision.icon << "\n";
	}
	if (outDecision.isGood != "")
	{
		outStream << "\n\t\tis_good = " << outDecision.isGood << "\n";
	}
	if (outDecision.allowed != "")
	{
		outStream << "\n\t\tallowed " << outDecision.allowed << "\n";
	}
	if (outDecision.available != "")
	{
		outStream << "\n\t\tavailable " << outDecision.available << "\n";
	}
	if (outDecision.daysMissionTimeout != "")
	{
		outStream << "\n\t\tdays_mission_timeout = " << outDecision.daysMissionTimeout << "\n";
	}
	if (outDecision.activation != "")
	{
		outStream << "\n\t\tactivation " << outDecision.activation << "\n";
	}
	if (outDecision.targetTrigger != "")
	{
		outStream << "\n\t\ttarget_trigger " << outDecision.targetTrigger << "\n";
	}
	if (outDecision.visible != "")
	{
		outStream << "\n\t\tvisible " << outDecision.visible << "\n";
	}
	if (outDecision.cancelTrigger != "")
	{
		outStream << "\n\t\tcancel_trigger " << outDecision.cancelTrigger << "\n";
	}
	if (outDecision.removeTrigger != "")
	{
		outStream << "\n\t\tremove_trigger " << outDecision.removeTrigger << "\n";
	}
	if (outDecision.remove != "")
	{
		outStream << "\n\t\tremove " << outDecision.remove << "\n";
	}
	if (outDecision.fireOnlyOnce != "")
	{
		outStream << "\n\t\tfire_only_once = " << outDecision.fireOnlyOnce << "\n";
	}
	if (outDecision.modifier != "")
	{
		outStream << "\n\t\tmodifier " << outDecision.modifier << "\n";
	}
	if (outDecision.daysRemove)
	{
		outStream << "\n\t\tdays_remove = " << *outDecision.daysRemove << "\n";
	}
	if (outDecision.daysReEnable)
	{
		outStream << "\t\tdays_re_enable = " << *outDecision.daysReEnable << "\n";
	}
	if (outDecision.cost != "")
	{
		outStream << "\n\t\tcost = " << outDecision.cost << "\n";
	}
	if (outDecision.completeEffect != "")
	{
		outStream << "\n\t\tcomplete_effect " << outDecision.completeEffect << "\n";
	}
	if (outDecision.removeEffect != "")
	{
		outStream << "\n\t\tremove_effect " << outDecision.removeEffect << "\n";
	}
	if (outDecision.timeoutEffect != "")
	{
		outStream << "\n\t\ttimeout_effect " << outDecision.timeoutEffect << "\n";
	}
	if (outDecision.aiWillDo != "")
	{
		outStream << "\n\t\tai_will_do " << outDecision.aiWillDo << "\n";
	}

	outStream << "\t}";

	return outStream;
}


bool HoI4::operator==(const HoI4::decision& decisionOne, const HoI4::decision& decisionTwo)
{
	return (decisionOne.name == decisionTwo.name);
}