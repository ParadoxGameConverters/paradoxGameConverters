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


void HoI4::decisionsCategory::updatePoliticalDecisions(const std::set<std::string>& majorIdeologies)
{
	std::for_each(theDecisions.begin(), theDecisions.end(), [majorIdeologies](auto& theDecision){
		if (theDecision.getName().substr(0, 28) == "open_up_political_discourse_")
		{
			std::string available = "= {\n";
			for (auto ideology: majorIdeologies)
			{
				available += "			" + ideology + " < 0.9\n";
			}
			available += "		}";
			theDecision.setAvailable(available);
		}
		if (theDecision.getName().substr(0, 21) == "discredit_government_")
		{
			std::string decisionIdeology = theDecision.getName().substr(21, theDecision.getName().length());
			std::string available = "= {\n";
			for (auto ideology: majorIdeologies)
			{
				available += "			" + ideology + " < 0.8\n";
			}
			available += "			has_idea_with_trait = " + decisionIdeology + "_minister\n";
			available += "		}";
			theDecision.setAvailable(available);
			std::string completeEffect = "= {\n";
			completeEffect += "			add_stability = -0.010\n";
			for (auto ideology: majorIdeologies)
			{
				if (ideology == decisionIdeology)
				{
					continue;
				}
				completeEffect += "			if = {\n";
				completeEffect += "				limit = {\n";
				completeEffect += "					has_government = " + ideology + "\n";
				completeEffect += "				}\n";
				completeEffect += "				add_popularity = {\n";
				completeEffect += "					ideology = " + ideology + "\n";
				completeEffect += "					popularity = -0.1\n";
				completeEffect += "				}\n";
				completeEffect += "			}\n";
			}
			completeEffect += "		}";
			theDecision.setCompleteEffect(completeEffect);
		}
		if (theDecision.getName().substr(0, 27) == "institute_press_censorship_")
		{
			std::string decisionIdeology = theDecision.getName().substr(27, theDecision.getName().length());
			decisionIdeology = decisionIdeology.substr(0, decisionIdeology.find_last_of('_'));
			std::string modifier = "= {\n";
			for (auto ideology: majorIdeologies)
			{
				if (ideology == decisionIdeology)
				{
					modifier += "			" + ideology + "_drift = 0.03\n";
				}
				else
				{
					modifier += "			" + ideology + "_drift = -0.01\n";
				}
			}
			modifier += "		}";
			theDecision.setModifier(modifier);
		}
		if (theDecision.getName().substr(0, 11) == "ignite_the_")
		{
			std::string decisionIdeology = theDecision.getName().substr(11, theDecision.getName().length());
			decisionIdeology = decisionIdeology.substr(0, decisionIdeology.find_first_of('_'));
			std::string completeEffect = "= {\n";
			for (auto ideology: majorIdeologies)
			{
				if (ideology == decisionIdeology)
				{
					continue;
				}
				completeEffect += "			if = {\n";
				completeEffect += "				limit = {\n";
				completeEffect += "					has_government = " + ideology + "\n";
				completeEffect += "				}\n";
				completeEffect += "				set_variable = {\n";
				completeEffect += "					var = civil_war_size_var\n";
				completeEffect += "					value = party_popularity@" + ideology + "\n";
				completeEffect += "				}\n";
				completeEffect += "			}\n";
			}
			completeEffect += "			subtract_from_variable = {\n";
			completeEffect += "				var = civil_war_size_var\n";
			completeEffect += "				value = army_support_var\n";
			completeEffect += "			}\n";
			completeEffect += "			if = {\n";
			completeEffect += "				limit = {\n";
			completeEffect += "					check_variable = {\n";
			completeEffect += "						var = civil_war_size_var\n";
			completeEffect += "						value = 0.3\n";
			completeEffect += "						compare = less_than\n";
			completeEffect += "					}\n";
			completeEffect += "				}\n";
			completeEffect += "				set_variable = {\n";
			completeEffect += "					var = civil_war_size_var\n";
			completeEffect += "					value = 0.3\n";
			completeEffect += "				}\n";
			completeEffect += "			}\n";
			completeEffect += "			start_civil_war = {\n";
			completeEffect += "				ruling_party = " + decisionIdeology + "\n";
			completeEffect += "				ideology = ROOT\n";
			completeEffect += "				size = civil_war_size_var\n";
			completeEffect += "				keep_unit_leaders_trigger = {\n";
			completeEffect += "					has_trait = hidden_sympathies\n";
			completeEffect += "				}\n";
			completeEffect += "			}\n";
			completeEffect += "			clr_country_flag = preparation_for_" + decisionIdeology + "_civil_war\n";
			completeEffect += "			clr_country_flag = military_support_for_" + decisionIdeology + "_civil_war\n";
			completeEffect += "			clr_country_flag = civil_support_for_" + decisionIdeology + "_civil_war\n";
			completeEffect += "			set_country_flag = ideology_civil_war\n";
			completeEffect += "		}";
			theDecision.setCompleteEffect(completeEffect);
		}
	});
}


std::ostream& HoI4::operator<<(std::ostream& outStream, const decisionsCategory& outCategory)
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
	updatePoliticalDecisions(majorIdeologies);
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


void HoI4::decisions::updatePoliticalDecisions(const std::set<std::string>& majorIdeologies)
{
	std::for_each(majorIdeologies.begin(), majorIdeologies.end(), [this](auto ideology){
		auto categories = ideologicalDecisions.equal_range(ideology);
		std::for_each(categories.first, categories.second, [this](auto category){
			auto existingCategory = std::find(politicalDecisions.begin(), politicalDecisions.end(), category.second);
			if (existingCategory == politicalDecisions.end())
			{
				politicalDecisions.push_back(category.second);
			}
			else
			{
				auto theDecisions = category.second.getDecisions();
				std::for_each(theDecisions.begin(), theDecisions.end(), [&existingCategory](auto& theDecision){
					existingCategory->addDecision(theDecision);
				});
			}
		});
	});

	for (auto& decisionsByIdeology: politicalDecisions)
	{
		decisionsByIdeology.updatePoliticalDecisions(majorIdeologies);
	}
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