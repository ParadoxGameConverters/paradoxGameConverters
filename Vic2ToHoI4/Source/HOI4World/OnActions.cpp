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



#include "OnActions.h"
#include "Log.h"
#include "../Configuration.h"
#include <fstream>



HoI4::OnActions::OnActions(): electionEvents{ "election.2" }
{
}


void HoI4::OnActions::output(const std::set<std::string>& majorIdeologies) const
{
	std::ofstream onActionsFile("output/" + Configuration::getOutputName() + "/common/on_actions/99_converter_on_actions.txt");
	if (!onActionsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create NF_events.txt";
		exit(-1);
	}

	onActionsFile << "on_actions = {\n";
	onActionsFile << "	# country\n";
	onActionsFile << "	on_new_term_election = {\n";
	onActionsFile << "		random_events = {\n";
	for (auto event: electionEvents)
	{
		onActionsFile << "			100 = " << event << "\n";
	}
	onActionsFile << "		}\n";
	onActionsFile << "	}\n";
	onActionsFile << "	on_government_change = {\n";
	onActionsFile << "		effect = {\n";
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		onActionsFile << "			if = {\n";
		onActionsFile << "				limit = { has_government = " + ideology + " }\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = " + ideology + "_partisans_recruiting }\n";
		onActionsFile << "					remove_ideas = " + ideology + "_partisans_recruiting\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = " + ideology + "_revolutionaries }\n";
		onActionsFile << "					remove_ideas = " + ideology + "_revolutionaries\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = " + ideology + "_defeated }\n";
		onActionsFile << "					remove_ideas = " + ideology + "_defeated\n";
		onActionsFile << "				}\n";
		onActionsFile << "			}\n";
	}
	if (majorIdeologies.count("democratic") > 0)
	{
		onActionsFile << "			if = {\n";
		onActionsFile << "				limit = { has_government = democratic }\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = democratic_opposition_voicing_protests }\n";
		onActionsFile << "					remove_ideas = democratic_opposition_voicing_protests\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = democratic_revolutionaries }\n";
		onActionsFile << "					remove_ideas = democratic_revolutionaries\n";
		onActionsFile << "				}\n";
		onActionsFile << "				if = {\n";
		onActionsFile << "					limit = { has_idea = reign_of_terror }\n";
		onActionsFile << "					remove_ideas = reign_of_terror\n";
		onActionsFile << "				}\n";
		onActionsFile << "			}\n";
	}
	onActionsFile << "		}\n";
	onActionsFile << "	}\n";

	if (majorIdeologies.count("democratic") > 0)
	{
		onActionsFile << "	on_coup_succeeded = {\n";
		onActionsFile << "		effect = {\n";
		onActionsFile << "			#Turn elections on for democracies created from coup\n";
		onActionsFile << "			random_other_country = {\n";
		onActionsFile << "				limit = {\n";
		onActionsFile << "					has_government = democratic\n";
		onActionsFile << "					original_tag = ROOT\n";
		onActionsFile << "				}\n";
		onActionsFile << "				set_politics = {\n";
		onActionsFile << "					elections_allowed = yes\n";
		onActionsFile << "				}\n";
		onActionsFile << "			}\n";
		onActionsFile << "		}\n";
		onActionsFile << "	}\n";
	}

	onActionsFile << "	on_justifying_wargoal_pulse = {\n";
	onActionsFile << "		random_events = {\n";
	for (unsigned int i = 1; i < (majorIdeologies.size() * majorIdeologies.size()); i++)
	{
		onActionsFile << "			100 = war_justification." << i << "\n";
	}
	onActionsFile << "			100 = war_justification.37\n";
	onActionsFile << "			100 = war_justification.38\n";
	onActionsFile << "			100 = war_justification.39\n";
	onActionsFile << "			17000 = 0\n";
	onActionsFile << "		}\n";
	onActionsFile << "	}\n";

	onActionsFile << "	on_startup = {\n";
	onActionsFile << "		effect = {\n";
	onActionsFile << "			set_province_name = { id = 587 name = \"Köln\"} #Cologne\n";
	onActionsFile << "			set_province_name = { id = 957 name = \"Vladivostok\"} #Haishenwai\n";
	onActionsFile << "			set_province_name = { id = 1025 name = \"Kokura\"} #Fukuoka\n";
	onActionsFile << "			set_province_name = { id = 1047 name = \"Guangzhou\"} #Canton\n";
	onActionsFile << "			set_province_name = { id = 1182 name = \"Tokyo\"} #Edo\n";
	onActionsFile << "			set_province_name = { id = 1440 name = \"San Juan\"} #Puerto Rico\n";
	onActionsFile << "			set_province_name = { id = 1843 name = \"Miami\"} #Tampa\n";
	onActionsFile << "			set_province_name = { id = 3151 name = \"Leningrad\"} #Saint Petersburg\n";
	onActionsFile << "			set_province_name = { id = 3152 name = \"Tallinn\"} #Reval\n";
	onActionsFile << "			set_province_name = { id = 3529 name = \"Stalingrad\"} #Tsaritsyn\n";
	onActionsFile << "			set_province_name = { id = 4180 name = \"Honolulu\"} #Hawaii\n";
	onActionsFile << "			set_province_name = { id = 4268 name = \"Nouméa\"} #New Caledonia\n";
	onActionsFile << "			set_province_name = { id = 4333 name = \"Astana\"} #Qaraganda\n";
	onActionsFile << "			set_province_name = { id = 4709 name = \"Ürümqi\"} #Díhuà\n";
	onActionsFile << "			set_province_name = { id = 4801 name = \"Ulaanbaatar\"} #Urga\n";
	onActionsFile << "			set_province_name = { id = 6115 name = \"Oslo\"} #Christiania\n";
	onActionsFile << "			set_province_name = { id = 7371 name = \"Kuching\"} #Brunei\n";
	onActionsFile << "			set_province_name = { id = 11437 name = \"Dnipropetrovsk\"} #Ekaterinoslav\n";
	onActionsFile << "			set_province_name = { id = 12674 name = \"Reykjavik\"} #Iceland\n";
	onActionsFile << "		}\n";
	onActionsFile << "	}\n";

	onActionsFile << "}\n";

	onActionsFile.close();
}