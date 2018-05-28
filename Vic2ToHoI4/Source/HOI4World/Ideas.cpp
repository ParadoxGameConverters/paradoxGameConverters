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



#include "Ideas.h"
#include "IdeaGroup.h"
#include "../Configuration.h"
#include "ParadoxParserUTF8.h"
#include <fstream>



HoI4::Ideas::Ideas()
{
	importIdeologicalIdeas();
	importGeneralIdeas();
}


void HoI4::Ideas::importIdeologicalIdeas()
{
	auto fileObject = parser_UTF8::doParseFile("ideologicalIdeas.txt");
	if (fileObject)
	{
		auto ideologyObjects = fileObject->getLeaves();
		for (auto ideologyObject: ideologyObjects)
		{
			std::string ideaName = ideologyObject->getKey();
			ideologicalIdeas.insert(make_pair(ideaName, ideologyObject->getLeaves()));
		}
	}
}


void HoI4::Ideas::importGeneralIdeas()
{
	registerKeyword(std::regex("[a-zA-Z_]+"), [this](const std::string& ideaGroupName, std::istream& theStream){
		generalIdeas.push_back(std::make_unique<IdeaGroup>(ideaGroupName, theStream));
	});

	parseFile("converterIdeas.txt");
}


void HoI4::Ideas::updateIdeas(std::set<std::string> majorIdeologies)
{
	auto foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup){ return (theGroup->getName() == "mobilization_laws"); });
	auto serviceByRequirement = (*foundGroup)->getIdea("service_by_requirement");
	if (serviceByRequirement)
	{
		std::string available = "= {\n";
		available += "				#has_manpower_for_recruit_change_to =  { value = 0.1 group = mobilization_laws }\n";
		available += "				OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology != "neutrality") && (ideology != "democratic"))
			{
				available += "					has_government = " + ideology + "\n";
			}
		}
		available += "					AND = {\n";
		available += "						has_war = yes\n";
		available += "						enemies_strength_ratio > 0.6\n";
		available += "						#any_enemy_country = {\n";
		available += "						#	strength_ratio = {\n";
		available += "						#		tag = ROOT \n";
		available += "						#		ratio > 0.6\n";
		available += "						#	}\n";
		available += "						#}\n";
		available += "					}\n";
		available += "				}\n";
		available += "				OR = {\n";
		available += "					has_war_support > 0.6\n";
		available += "					surrender_progress > 0\n";
		available += "				}\n";
		available += "			}";
		serviceByRequirement->setAvailable(available);
		(*foundGroup)->replaceIdea(*serviceByRequirement);
	}

	auto extensiveConscription = (*foundGroup)->getIdea("extensive_conscription");
	if (extensiveConscription)
	{
		std::string available = "= {\n";
		available += "				#has_manpower_for_recruit_change_to = { value = 0.05 group = mobilization_laws }\n";
		available += "				OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology != "neutrality") && (ideology != "democratic"))
			{
				available += "					has_government = " + ideology + "\n";
			}
		}
		available += "					AND = {\n";
		available += "						has_war = yes\n";
		available += "						enemies_strength_ratio > 0.5\n";
		available += "						#any_enemy_country = {\n";
		available += "						#	strength_ratio = {\n";
		available += "						#		tag = ROOT \n";
		available += "						#		ratio > 0.5\n";
		available += "						#	}\n";
		available += "						#}\n";
		available += "					}\n";
		available += "				}\n";
		available += "				has_war_support > 0.2\n";
		available += "			}";
		extensiveConscription->setAvailable(available);
		(*foundGroup)->replaceIdea(*extensiveConscription);
	}

	foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup){ return (theGroup->getName() == "economy"); });
	auto warEconomy = (*foundGroup)->getIdea("war_economy");
	if (warEconomy)
	{
		std::string available = "= {\n";
		available += "				has_war_support > 0.5\n";
		available += "				OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology != "neutrality") && (ideology != "democratic"))
			{
				available += "					has_government = " + ideology + "\n";
			}
		}
		available += "					custom_trigger_tooltip = { tooltip = or_clarification_tooltip always = no }\n";
		available += "					AND = {\n";
		available += "						has_war = yes\n";
		available += "						any_enemy_country = {\n";
		available += "							ic_ratio = { \n";
		available += "								tag = ROOT \n";
		available += "								ratio > 0.4\n";
		available += "							}\n";
		available += "						}\n";
		available += "					}\n";
		available += "				}\n";
		available += "			}";
		warEconomy->setAvailable(available);
		(*foundGroup)->replaceIdea(*warEconomy);
	}

	foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup){ return (theGroup->getName() == "trade_laws"); });
	auto closedEconomy = (*foundGroup)->getIdea("closed_economy");
	if (closedEconomy)
	{
		std::string available = "= {\n";
		available += "				has_war = yes\n";
		available += "				OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology != "neutrality") && (ideology != "democratic"))
			{
				available += "					has_government = " + ideology + "\n";
			}
		}
		available += "				}\n";
		available += "				OR = {\n";
		available += "					has_idea = war_economy\n";
		available += "					has_idea = tot_economic_mobilisation\n";
		available += "				}\n";
		available += "			}";
		closedEconomy->setAvailable(available);
		(*foundGroup)->replaceIdea(*closedEconomy);
	}

	auto limitedExports = (*foundGroup)->getIdea("limited_exports");
	if (limitedExports)
	{
		std::string available = "= {\n";
		if (majorIdeologies.count("democratic") > 0)
		{
			available += "				OR = {\n";
			available += "					AND = {\n";
			available += "						has_government = democratic\n";
			available += "						has_war = yes\n";
			available += "						any_enemy_country = {\n";
			available += "							ic_ratio = { \n";
			available += "								tag = ROOT \n";
			available += "								ratio > 0.2\n";
			available += "							}\n";
			available += "						}\n";
			available += "					}\n";
			available += "					AND = {\n";
			available += "						NOT = { has_government = democratic }\n";
			available += "						OR = {\n";
			available += "							has_idea = partial_economic_mobilisation\n";
			available += "							has_idea = war_economy\n";
			available += "							has_idea = tot_economic_mobilisation\n";
			available += "						}\n";
			available += "					}\n";
			available += "				}\n";
		}
		else
		{
			available += "				OR = {\n";
			available += "					has_idea = partial_economic_mobilisation\n";
			available += "					has_idea = war_economy\n";
			available += "					has_idea = tot_economic_mobilisation\n";
			available += "				}\n";
		}
		available += "			}";
		limitedExports->setAvailable(available);
		(*foundGroup)->replaceIdea(*limitedExports);
	}

	foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup){ return (theGroup->getName() == "country"); });
	auto militaryYouthFocus = (*foundGroup)->getIdea("military_youth_focus");
	if (militaryYouthFocus)
	{
		std::string allowedCivilWar = "= {\n";
		allowedCivilWar += "				OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology != "neutrality") && (ideology != "democratic"))
			{
				allowedCivilWar += "					has_government = " + ideology + "\n";
			}
		}
		allowedCivilWar += "				}\n";
		allowedCivilWar += "			}";
		militaryYouthFocus->setAllowedCivilWar(allowedCivilWar);
		(*foundGroup)->replaceIdea(*militaryYouthFocus);
	}

	auto paramilitarismFocus = (*foundGroup)->getIdea("paramilitarism_focus");
	if (paramilitarismFocus)
	{
		std::string allowedCivilWar = "= {\n";
		allowedCivilWar += "				OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology != "neutrality") && (ideology != "democratic"))
			{
				allowedCivilWar += "					has_government = " + ideology + "\n";
			}
		}
		allowedCivilWar += "				}\n";
		allowedCivilWar += "			}";
		paramilitarismFocus->setAllowedCivilWar(allowedCivilWar);
		(*foundGroup)->replaceIdea(*paramilitarismFocus);
	}

	auto indoctrinationFocus = (*foundGroup)->getIdea("indoctrination_focus");
	if (indoctrinationFocus)
	{
		std::string allowedCivilWar = "= {\n";
		allowedCivilWar += "				OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology != "neutrality") && (ideology != "democratic"))
			{
				allowedCivilWar += "					has_government = " + ideology + "\n";
			}
		}
		allowedCivilWar += "				}\n";
		allowedCivilWar += "			}";
		indoctrinationFocus->setAllowedCivilWar(allowedCivilWar);
		(*foundGroup)->replaceIdea(*indoctrinationFocus);
	}
}


void HoI4::Ideas::output(std::set<std::string> majorIdeologies) const
{
	outputIdeologicalIdeas(majorIdeologies);
	outputGeneralIdeas();
}


void HoI4::Ideas::outputIdeologicalIdeas(std::set<std::string> majorIdeologies) const
{
	std::ofstream ideasFile("output/" + Configuration::getOutputName() + "/common/ideas/convertedIdeas.txt");
	ideasFile << "ideas = {\n";
	ideasFile << "\tcountry = {\n";
	for (auto majorIdeology: majorIdeologies)
	{
		auto ideologicalIdea = ideologicalIdeas.find(majorIdeology);
		if (ideologicalIdea != ideologicalIdeas.end())
		{
			for (auto idea: ideologicalIdea->second)
			{
				ideasFile << *idea;
				ideasFile << "\n";
			}
		}
	}
	ideasFile << "\t}\n";
	ideasFile << "}";
	ideasFile.close();
}


void HoI4::Ideas::outputGeneralIdeas() const
{
	auto manpowerFile = openIdeaFile("output/" + Configuration::getOutputName() + "/common/ideas/_manpower.txt");
	auto economicFile = openIdeaFile("output/" + Configuration::getOutputName() + "/common/ideas/_economic.txt");
	auto genericFile = openIdeaFile("output/" + Configuration::getOutputName() + "/common/ideas/zzz_generic.txt");

	std::for_each(generalIdeas.begin(), generalIdeas.end(), [&manpowerFile, &economicFile, &genericFile](auto& theGroup){
		if (theGroup->getName() == "mobilization_laws")
		{
			manpowerFile << *theGroup;
		}
		else if ((theGroup->getName() == "economy") || (theGroup->getName() == "trade_laws"))
		{
			economicFile << *theGroup;
		}
		else
		{
			genericFile << *theGroup;
		}
	});

	closeIdeaFile(manpowerFile);
	closeIdeaFile(economicFile);
	closeIdeaFile(genericFile);
}


std::ofstream HoI4::Ideas::openIdeaFile(const std::string& fileName) const
{
	std::ofstream theFile(fileName);
	theFile << "ideas = {\n";
	return theFile;
}


void HoI4::Ideas::closeIdeaFile(std::ofstream& fileStream) const
{
	fileStream << "}";
	fileStream.close();
}