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
	std::for_each(outCategory.theDecisions.begin(), outCategory.theDecisions.end(), [&outStream](auto decision){ outStream << decision << "\n"; });
	if (outCategory.theDecisions.size() == 0)
	{
		outStream << "\n";
	}
	outStream << "}\n";
	outStream << "\n";

	return outStream;
}


bool HoI4::operator==(const decisionsCategory& categoryOne, const decisionsCategory& categoryTwo)
{
	return (categoryOne.name == categoryTwo.name);
}


namespace HoI4
{
	class decisionsCategorySet: commonItems::parser
	{
		public:
			decisionsCategorySet(std::istream& theStream)
			{
				registerKeyword(std::regex("[A-Za-z\\_]+"), [this](const std::string& categoryName, std::istream& theStream)
				{
					decisionsCategory category(categoryName, theStream);
					theCategories.push_back(category);
				});
				parseStream(theStream);
			}

			std::vector<decisionsCategory> takeCategories() { return std::move(theCategories); }

		private:
			std::vector<decisionsCategory> theCategories;
	};
}


HoI4::decisions::decisions()
{
	registerKeyword(std::regex("[A-Za-z\\_]+"), [this](const std::string& categoryName, std::istream& theStream)
	{
		decisionsCategory category(categoryName, theStream);
		stabilityDecisions.push_back(category);
	});
	parseFile("blankmod/output/common/decisions/stability_war_support.txt");

	clearRegisteredKeywords();

	registerKeyword(std::regex("[A-Za-z\\_]+"), [this](const std::string& ideologyName, std::istream& theStream)
	{
		decisionsCategorySet categorySet(theStream);
		auto categories = categorySet.takeCategories();
		std::for_each(categories.begin(), categories.end(), [this, ideologyName](auto& category){
			ideologicalDecisions.insert(std::make_pair(ideologyName, category));
		});
	});
	parseFile("ideologicalDecisions.txt");
}


void HoI4::decisions::updateDecisions(const std::set<std::string>& majorIdeologies)
{
	updateStabilityDecisions(majorIdeologies);
}


void HoI4::decisions::updateStabilityDecisions(const std::set<std::string>& majorIdeologies)
{
	auto ideologiesForStabilityDecisions = determineIdeologiesForStabilityDecisions(majorIdeologies);

	for (auto category: stabilityDecisions)
	{
		bool updated = false;
		for (auto decision: category.getDecisions())
		{
			if (stabilityDecisionToUpdate(decision.getName()))
			{
				auto timeoutEffect = decision.getTimeoutEffect();
				timeoutEffect = updateTimeoutEffect(timeoutEffect, ideologiesForStabilityDecisions);
				decision.setTimeoutEffect(timeoutEffect);
				category.replaceDecision(decision);
				updated = true;
			}
		}
		if (updated)
		{
			std::replace(stabilityDecisions.begin(), stabilityDecisions.end(), category, category);
		}
	}
}


bool HoI4::decisions::stabilityDecisionToUpdate(const std::string& decisionName)
{
	if (
			(decisionName == "draft_dodging_mission") || 
			(decisionName == "strikes_mission") || 
			(decisionName == "mutiny_mission") || 
			(decisionName == "demob_economic_mission") || 
			(decisionName == "demob_manpower_mission")
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}


std::pair<std::string, std::string> HoI4::decisions::determineIdeologiesForStabilityDecisions(const std::set<std::string>& majorIdeologies)
{
	std::pair<std::string, std::string> theIdeologies;

	if (majorIdeologies.count("communism") > 0)
	{
		theIdeologies.first = "communism";
	}
	else if (majorIdeologies.count("absolutist") > 0)
	{
		theIdeologies.first = "absolutist";
	}
	else
	{
		theIdeologies.first = "neutrality";
	}

	if (majorIdeologies.count("democratic") > 0)
	{
		theIdeologies.second = "democratic";
	}
	else if (majorIdeologies.count("radical") > 0)
	{
		theIdeologies.second = "radical";
	}
	else
	{
		theIdeologies.second = "neutrality";
	}

	return theIdeologies;
}


std::string HoI4::decisions::updateTimeoutEffect(std::string& originalEffect, const std::pair<std::string, std::string>& ideologiesForStabilityDecisions)
{
	auto index = originalEffect.find("communism");
	while (index != std::string::npos)
	{
		originalEffect.replace(index, 9, ideologiesForStabilityDecisions.first);
		index = originalEffect.find("communism", index + ideologiesForStabilityDecisions.first.length());
	}

	index = originalEffect.find("democratic");
	while (index != std::string::npos)
	{
		originalEffect.replace(index, 10, ideologiesForStabilityDecisions.second);
		index = originalEffect.find("democratic", index + ideologiesForStabilityDecisions.second.length());
	}

	return originalEffect;
}


void HoI4::decisions::output()
{
	std::ofstream outStream("output/" + Configuration::getOutputName() + "/common/decisions/stability_war_support.txt");
	std::for_each(stabilityDecisions.begin(), stabilityDecisions.end(), [&outStream](auto category) { outStream << category; });
	outStream.close();

	outStream.open("output/" + Configuration::getOutputName() + "/common/decisions/political_decisions.txt");
	std::for_each(politicalDecisions.begin(), politicalDecisions.end(), [&outStream](auto category) { outStream << category; });
	outStream.close();
}