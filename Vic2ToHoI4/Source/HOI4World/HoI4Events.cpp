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



#include "HoI4Events.h"
#include <fstream>
#include "HoI4Country.h"
#include "HoI4Faction.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



HoI4Events::HoI4Events()
{
	newsEventNumber = 237;
	nationalFocusEventNumber = 0;
}


void HoI4Events::output() const
{
	string eventpath = "output/" + Configuration::getOutputName() + "/events";
	if (!Utils::TryCreateFolder(eventpath))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/events\"";
		exit(-1);
	}

	outputNationalFocusEvents();
	outputNewsEvents();
	outputPoliticalEvents();
}


void HoI4Events::outputNationalFocusEvents() const
{
	ofstream outEvents("output/" + Configuration::getOutputName() + "/events/NF_events.txt");
	if (!outEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not create NF_events.txt";
		exit(-1);
	}

	outEvents << "\xEF\xBB\xBF";
	outEvents << "add_namespace = NFEvents\n";
	for (auto theEvent: nationalFocusEvents)
	{
		outEvents << "\n";
		outEvents << theEvent;
	}

	outEvents.close();
}


void HoI4Events::outputNewsEvents() const
{
	ofstream outNewsEvents("output/" + Configuration::getOutputName() + "/events/newsEvents.txt");
	if (!outNewsEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not create newsEvents.txt";
		exit(-1);
	}

	outNewsEvents << "\xEF\xBB\xBF";
	outNewsEvents << "add_namespace = news\n" << flush;
	for (auto theEvent: newsEvents)
	{
		outNewsEvents << "\n" << flush;
		outNewsEvents << theEvent << flush;
	}

	outNewsEvents.close();
}


void HoI4Events::outputPoliticalEvents() const
{
	ofstream outPoliticalEvents("output/" + Configuration::getOutputName() + "/events/converterPoliticalEvents.txt");
	if (!outPoliticalEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not create converterPoliticalEvents.txt";
		exit(-1);
	}

	outPoliticalEvents << "\xEF\xBB\xBF";
	outPoliticalEvents << "add_namespace = conv.political\n" << flush;
	for (auto theEvent: politicalEvents)
	{
		outPoliticalEvents << "\n";
		outPoliticalEvents << theEvent;
	}

	outPoliticalEvents.close();
}


void HoI4Events::createFactionEvents(const HoI4Country* Leader, HoI4Country* newAlly)
{
	string leaderName = Leader->getSourceCountry()->getName("english");
	string newAllyname = newAlly->getSourceCountry()->getName("english");

	HoI4Event nfEvent;
	nfEvent.type = "country_event";
	nfEvent.id = "NFEvents." + to_string(nationalFocusEventNumber++);
	nfEvent.title = "\"Alliance?\"";
	nfEvent.description = "\"Alliance with " + leaderName + "?\"";
	nfEvent.picture = "news_event_generic_sign_treaty1";
	nfEvent.major = false;
	nfEvent.triggeredOnly = true;
	string yesOption = "name = \"Yes\"\n";
	if (Leader->getFaction() != nullptr)
	{
		yesOption += "		" + newAlly->getTag() + " = {\n";
		yesOption += "			add_ai_strategy = {\n";
		yesOption += "				type = alliance\n";
		yesOption += "				id = \"" + Leader->getTag() + "\"\n";
		yesOption += "				value = 200\n";
		yesOption += "			}\n";
		yesOption += "			dismantle_faction = yes";
		yesOption += "		}\n";
		yesOption += "		" + Leader->getTag() + " = {\n";
		yesOption += "			add_to_faction = " + newAlly->getTag() + "\n";
		yesOption += "		}\n";
	}
	yesOption += "		hidden_effect = {\n";
	yesOption += "			news_event = { id = news." + to_string(newsEventNumber) + " }\n";
	yesOption += "		}\n";
	nfEvent.options.push_back(yesOption);
	string noOption = "name = \"No\"\n";
	noOption += "		ai_chance = { factor = 0 }\n";
	noOption += "		hidden_effect = {\n";
	noOption += "			news_event = { id = news." + to_string(newsEventNumber + 1) + " }\n";
	noOption += "		}\n";
	nfEvent.options.push_back(noOption);
	nationalFocusEvents.push_back(nfEvent);

	HoI4Event newsEventYes;
	newsEventYes.type = "news_event";
	newsEventYes.id = "news." + to_string(newsEventNumber);
	newsEventYes.title = "\"" + newAllyname + " Now an Ally with " + leaderName + "!\"";
	newsEventYes.description = "\"They are now allies\"";
	newsEventYes.picture = "news_event_generic_sign_treaty1";
	newsEventYes.major = true;
	nfEvent.triggeredOnly = true;
	string interestingOption = "name = \"Interesting\"\n";
	newsEventYes.options.push_back(interestingOption);
	newsEvents.push_back(newsEventYes);

	HoI4Event newsEventNo;
	newsEventNo.type = "news_event";
	newsEventNo.id = "news." + to_string(newsEventNumber + 1);
	newsEventNo.title = "\"" + newAllyname + " Refused the Alliance offer of " + leaderName + "!\"";
	newsEventNo.description = "\"They are not allies\"";
	newsEventNo.picture = "news_event_generic_sign_treaty1";
	newsEventNo.major = true;
	nfEvent.triggeredOnly = true;
	interestingOption = "name = \"Interesting\"\n";
	newsEventNo.options.push_back(interestingOption);
	newsEvents.push_back(newsEventNo);

	newsEventNumber += 2;
}


void HoI4Events::createAnnexEvent(const HoI4Country* Annexer, const HoI4Country* Annexed)
{
	string annexername = Annexer->getSourceCountry()->getName("english");
	string annexedname = Annexed->getSourceCountry()->getName("english");

	HoI4Event annexEvent;
	annexEvent.type = "country_event";
	annexEvent.id = "NFEvents." + to_string(nationalFocusEventNumber);
	annexEvent.title = "\"" + annexername + " Demands " + annexedname + "!\"";
	annexEvent.description = "\"Today " + annexername + " sent an envoy to us with a proposition of an union. We are alone and in this world, and a union with " + annexername + " might prove to be fruiteful.";
	annexEvent.description += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"";
	annexEvent.picture = "GFX_report_event_hitler_parade";
	annexEvent.major = false;
	annexEvent.triggeredOnly = true;

	string acceptOption = "name = \"We accept the Union\"\n";
	acceptOption += "		ai_chance = {\n";
	acceptOption += "			base = 30\n";
	acceptOption += "			modifier = {\n";
	acceptOption += "				add = -15\n";
	acceptOption += "				" + Annexer->getTag() + " = { has_army_size = { size < 40 } }\n";
	acceptOption += "			}\n";
	acceptOption += "			modifier = {\n";
	acceptOption += "				add = 45\n";
	acceptOption += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\n";
	acceptOption += "			}\n";
	acceptOption += "		}\n";
	acceptOption += "		" + Annexer->getTag() + " = {\n";
	acceptOption += "			country_event = { hours = 2 id = NFEvents." + to_string(nationalFocusEventNumber + 1) + " }\n";//+1 accept
	acceptOption += "		}\n";
	acceptOption += "		custom_effect_tooltip = GAME_OVER_TT\n";
	annexEvent.options.push_back(acceptOption);

	string refuseOption = "name = \"We Refuse!\"\n";
	refuseOption += "		ai_chance = {\n";
	refuseOption += "			base = 10\n";
	refuseOption += "\n";
	refuseOption += "			modifier = {\n";
	refuseOption += "				factor = 0\n";
	refuseOption += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\n";
	refuseOption += "			}\n";
	refuseOption += "			modifier = {\n";
	refuseOption += "				add = 20\n";
	refuseOption += "				" + Annexer->getTag() + " = { has_army_size = { size < 30 } }\n";
	refuseOption += "			}\n";
	refuseOption += "		}\n";
	refuseOption += "		" + Annexer->getTag() + " = {\n";
	//refuseOption += "			add_opinion_modifier = { target = ROOT modifier = " + Annexer->getTag() + "_anschluss_rejected }\n";
	refuseOption += "			country_event = { hours = 2 id = NFEvents." + to_string(nationalFocusEventNumber + 2) + " }\n";//+2 refuse
	refuseOption += "			if = {\n";
	refuseOption += "				limit = { is_in_faction_with = " + Annexed->getTag() + " }\n";
	refuseOption += "				remove_from_faction = " + Annexed->getTag() + "\n";
	refuseOption += "			}\n";
	refuseOption += "		}\n";
	annexEvent.options.push_back(refuseOption);

	nationalFocusEvents.push_back(annexEvent);


	HoI4Event refusedEvent;
	refusedEvent.type = "country_event";
	refusedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 2);
	refusedEvent.title = "\"" + annexedname + " Refuses!\"";
	refusedEvent.description = "\"" + annexedname + " Refused our proposed union! This is an insult to us that cannot go unanswered!\"";
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.major = false;
	refusedEvent.triggeredOnly = true;

	string refusedOption = "name = \"It's time for war\"\n";
	refusedOption += "		create_wargoal = {\n";
	refusedOption += "			type = annex_everything\n";
	refusedOption += "			target = " + Annexed->getTag() + "\n";
	refusedOption += "		}\n";
	refusedEvent.options.push_back(refusedOption);

	nationalFocusEvents.push_back(refusedEvent);


	HoI4Event acceptedEvent;
	acceptedEvent.type = "country_event";
	acceptedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 1);
	acceptedEvent.title = "\"" + annexedname + " accepts!\"";
	acceptedEvent.description = "\"" + annexedname + " accepted our proposed union, their added strength will push us to greatness!\"";
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.major = false;
	acceptedEvent.triggeredOnly = true;

	string acceptedOption = "name = \"A stronger Union!\"\n";
	for (auto state: Annexed->getStates())
	{
		acceptedOption += "		" + to_string(state.first) + " = {\n";
		acceptedOption += "			if = {\n";
		acceptedOption += "				limit = { is_owned_by = " + Annexed->getTag() + " }\n";
		acceptedOption += "				add_core_of = " + Annexer->getTag() + "\n";
		acceptedOption += "			}\n";
		acceptedOption += "		}\n";

	}
	acceptedOption += "\n";
	acceptedOption += "		annex_country = { target = " + Annexed->getTag() + " transfer_troops = yes }\n";
	acceptedOption += "		add_political_power = 50\n";
	acceptedOption += "		add_named_threat = { threat = 2 name = \"" + annexername + " annexed " + annexedname + "\" }\n";
	acceptedOption += "		set_country_flag = " + Annexed->getTag() + "_annexed\n";
	acceptedEvent.options.push_back(acceptedOption);

	nationalFocusEvents.push_back(acceptedEvent);


	nationalFocusEventNumber += 3;
}


void HoI4Events::createSudatenEvent(const HoI4Country* Annexer, const HoI4Country* Annexed, const set<string>& claimedStates)
{
	//flesh out this event more, possibly make it so allies have a chance to help?
	string annexername = Annexer->getSourceCountry()->getName("english");
	string annexedname = Annexed->getSourceCountry()->getName("english");

	HoI4Event sudatenEvent;
	sudatenEvent.type = "country_event";
	sudatenEvent.id = "NFEvents." + to_string(nationalFocusEventNumber);
	sudatenEvent.title = "\"" + annexername + " Demands " + annexedname + "!\"";
	sudatenEvent.description = "\"" + annexername + " has recently been making claims to our bordering states, saying that these states are full of " + Annexer->getSourceCountry()->getAdjective("english") + " people and that the territory should be given to them. Although it ";
	sudatenEvent.description = "is true that recently our neighboring states have had an influx of " + Annexer->getSourceCountry()->getAdjective("english") + " people in the recent years, we cannot give up our lands because a few " + Annexer->getSourceCountry()->getAdjective("english") + " settled down in our land. ";
	sudatenEvent.description += "In response " + annexername + " has called for a conference, demanding their territory in exchange for peace. How do we resond? ";
	sudatenEvent.description += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"";
	sudatenEvent.picture = "GFX_report_event_hitler_parade";
	sudatenEvent.major = false;
	sudatenEvent.triggeredOnly = true;

	string acceptOption = "name = \"We Accept\"\n";
	acceptOption += "		ai_chance = {\n";
	acceptOption += "			base = 30\n";
	acceptOption += "			modifier = {\n";
	acceptOption += "				add = -15\n";
	acceptOption += "				" + Annexer->getTag() + " = { has_army_size = { size < 40 } }\n";
	acceptOption += "			}\n";
	acceptOption += "			modifier = {\n";
	acceptOption += "				add = 45\n";
	acceptOption += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\n";
	acceptOption += "			}\n";
	acceptOption += "		}\n";
	acceptOption += "		" + Annexer->getTag() + " = {\n";
	acceptOption += "			country_event = { hours = 2 id = NFEvents." + to_string(nationalFocusEventNumber + 1) + " }\n";
	acceptOption += "		}\n";
	sudatenEvent.options.push_back(acceptOption);

	string refuseOption = "name = \"We Refuse!\"\n";
	refuseOption += "		ai_chance = {\n";
	refuseOption += "			base = 10\n";
	refuseOption += "\n";
	refuseOption += "			modifier = {\n";
	refuseOption += "				factor = 0\n";
	refuseOption += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\n";
	refuseOption += "			}\n";
	refuseOption += "			modifier = {\n";
	refuseOption += "				add = 20\n";
	refuseOption += "				" + Annexer->getTag() + " = { has_army_size = { size < 30 } }\n";
	refuseOption += "			}\n";
	refuseOption += "		}\n";
	refuseOption += "		" + Annexer->getTag() + " = {\n";
	//refuseOption += "			add_opinion_modifier = { target = ROOT modifier = " + Annexer->getTag() + "_anschluss_rejected }\n";
	refuseOption += "			country_event = { hours = 2 id = NFEvents." + to_string(nationalFocusEventNumber + 2) + " }\n";
	refuseOption += "			if = {\n";
	refuseOption += "				limit = { is_in_faction_with = " + Annexed->getTag() + " }\n";
	refuseOption += "				remove_from_faction = " + Annexed->getTag() + "\n";
	refuseOption += "			}\n";
	refuseOption += "		}\n";
	sudatenEvent.options.push_back(refuseOption);

	nationalFocusEvents.push_back(sudatenEvent);


	HoI4Event refusedEvent;
	refusedEvent.type = "country_event";
	refusedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 2);
	refusedEvent.title = "\"" + annexedname + " Refuses!\"";
	refusedEvent.description = "\"" + annexedname + " Refused our proposed proposition! This is an insult to us that cannot go unanswered!\"";
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.major = false;
	refusedEvent.triggeredOnly = true;

	string refusedOption = "name = \"It's time for war\"\n";
	refusedOption += "		create_wargoal = {\n";
	refusedOption += "				type = annex_everything\n";
	refusedOption += "			target = " + Annexed->getTag() + "\n";
	refusedOption += "		}\n";
	refusedEvent.options.push_back(refusedOption);

	nationalFocusEvents.push_back(refusedEvent);


	HoI4Event acceptedEvent;
	acceptedEvent.type = "country_event";
	acceptedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 1);
	acceptedEvent.title = "\"" + annexedname + " accepts!\"";
	acceptedEvent.description = "\"" + annexedname + " accepted our proposed demands, the added lands will push us to greatness!\"";
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.major = false;
	acceptedEvent.triggeredOnly = true;

	string acceptedOption = "name = \"A stronger Union!\"\n";
	for (auto state: claimedStates)
	{
		acceptedOption += "		" + state + " = { add_core_of = " + Annexer->getTag() + " }\n";
		acceptedOption += "		" + Annexer->getTag() + " = { transfer_state =  " + state + " }\n";
	}
	acceptedOption += "		set_country_flag = " + Annexed->getTag() + "_demanded\n";
	acceptedEvent.options.push_back(acceptedOption);

	nationalFocusEvents.push_back(acceptedEvent);


	nationalFocusEventNumber += 3;
}


void HoI4Events::createTradeEvent(const HoI4Country* leader, const HoI4Country* GC)
{
	HoI4Event tradeIncidentEvent;
	tradeIncidentEvent.type = "country_event";
	tradeIncidentEvent.id = "NFEvents." + to_string(nationalFocusEventNumber++);
	tradeIncidentEvent.title = "\"Trade Incident\"";
	tradeIncidentEvent.description = "\"One of our convoys was sunk by " + GC->getSourceCountry()->getName("english") + "\"";
	tradeIncidentEvent.picture = "GFX_report_event_chinese_soldiers_fighting";
	tradeIncidentEvent.major = false;
	tradeIncidentEvent.triggeredOnly = true;
	tradeIncidentEvent.trigger = "has_country_flag = established_traders\n";
	tradeIncidentEvent.trigger += "		NOT = { has_country_flag = established_traders_activated }";

	string option = "name = \"They will Pay!\"\n";
	option += "		ai_chance = { factor = 85 }\n";
	option += "		effect_tooltip = {\n";
	option += "			" + leader->getTag() + " = {\n";
	option += "				set_country_flag = established_traders_activated\n";
	option += "				create_wargoal = {\n";
	option += "					type = annex_everything\n";
	option += "					target = " + GC->getTag() + "\n";
	option += "				}\n";
	option += "			}\n";
	option += "		}";
	tradeIncidentEvent.options.push_back(option);

	nationalFocusEvents.push_back(tradeIncidentEvent);
}


void HoI4Events::createPoliticalEvents(const set<string>& majorIdeologies)
{
	politicalEventNumber = 1;
	addMinisterRevolutionEvents(majorIdeologies);
	addFiftyPercentEvents(majorIdeologies);
	addRevolutionEvents(majorIdeologies);
	addSuppressedEvents(majorIdeologies);
}


void HoI4Events::addMinisterRevolutionEvents(const set<string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "neutrality") || (ideology == "democratic"))
		{
			continue;
		}

		HoI4Event onTheRise;
		onTheRise.type = "country_event";
		onTheRise.id = "conv.political." + ideology + ".1";
		onTheRise.title = "conv.political." + ideology + ".1.t";
		onTheRise.description = "conv.political." + ideology + ".1.d";
		onTheRise.picture = getIdeologicalPicture(ideology);
		onTheRise.major = false;
		onTheRise.triggeredOnly = true;
		onTheRise.trigger = "NOT = { has_government = " + ideology + " }";
		string optionA = "name = conv.political." + ideology + "." + to_string(politicalEventNumber) + ".a\n";
		optionA += "		custom_effect_tooltip = conv.political." + ideology + "." + to_string(politicalEventNumber) + ".a.tt\n";
		optionA += "		clr_country_flag = " + ideology + "_popular_movement\n";
		optionA += "		set_country_flag = " + ideology + "_political_movement";
		onTheRise.options.push_back(optionA);
		string optionB = "name = conv.political." + ideology + "." + to_string(politicalEventNumber) + ".b\n";
		optionB += "		custom_effect_tooltip = conv.political." + ideology + "." + to_string(politicalEventNumber) + ".b.tt\n";
		optionB += "		clr_country_flag = " + ideology + "_political_movement\n";
		optionB += "		clr_country_flag = " + ideology + "_government_support\n";
		optionB += "		set_country_flag = " + ideology + "_popular_movement";
		onTheRise.options.push_back(optionB);
		politicalEvents.push_back(onTheRise);
		politicalEventNumber++;

		HoI4Event ideologicalSympathies;
		ideologicalSympathies.type = "country_event";
		ideologicalSympathies.id = "conv.political." + to_string(politicalEventNumber);
		ideologicalSympathies.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		ideologicalSympathies.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		ideologicalSympathies.picture = getIdeologicalPicture(ideology);
		ideologicalSympathies.major = false;
		ideologicalSympathies.triggeredOnly = false;
		ideologicalSympathies.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		ideologicalSympathies.trigger += "		NOT = { has_government = " + ideology + " }\n";
		ideologicalSympathies.trigger += "		NOT = { has_country_flag = " + ideology + "_government_support }";
		ideologicalSympathies.meanTimeToHappen = "days = 500\n";
		ideologicalSympathies.meanTimeToHappen += "		modifier = {\n";
		ideologicalSympathies.meanTimeToHappen += "			factor = 0.5\n";
		ideologicalSympathies.meanTimeToHappen += "			has_country_flag = " + ideology + "_political_movement\n";
		ideologicalSympathies.meanTimeToHappen += "		}";
		optionA = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		optionA += "		custom_effect_tooltip = conv.political." + to_string(politicalEventNumber) + ".a.tt\n";
		optionA += "		clr_country_flag = " + ideology + "_political_movement\n";
		optionA += "		set_country_flag = " + ideology + "_government_support";
		ideologicalSympathies.options.push_back(optionA);
		politicalEvents.push_back(ideologicalSympathies);
		politicalEventNumber++;

		HoI4Event coup;
		coup.type = "country_event";
		coup.id = "conv.political." + to_string(politicalEventNumber);
		coup.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		coup.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		coup.picture = getIdeologicalPicture(ideology);
		coup.major = false;
		coup.triggeredOnly = false;
		coup.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		coup.trigger += "		NOT = { has_government = " + ideology + " }\n";
		coup.trigger += "		has_country_flag = " + ideology + "_government_support\n";
		coup.trigger += "		" + ideology + " > 0.4\n";
		coup.trigger += "		is_major = no\n";
		coup.trigger += "		is_puppet = no";
		coup.meanTimeToHappen = "days = 100\n";
		coup.meanTimeToHappen += "		modifier = {\n";
		coup.meanTimeToHappen += "			factor = 0.5\n";
		coup.meanTimeToHappen += "			" + ideology + " > 0.5\n";
		coup.meanTimeToHappen += "		}";
		coup.immediate = "hidden_effect = { remove_ideas = " + ideology + "_revolutionaries }";
		optionA = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		optionA += "		set_politics = {\n";
		optionA += "			ruling_party = " + ideology + "\n";
		optionA += "			elections_allowed = no\n";
		optionA += "		}\n";
		optionA += "		add_timed_idea = { idea = reign_of_terror days = 730 }";
		coup.options.push_back(optionA);
		politicalEvents.push_back(coup);
		politicalEventNumber++;

		HoI4Event addPopularity;
		addPopularity.type = "country_event";
		addPopularity.id = "conv.political." + to_string(politicalEventNumber);
		addPopularity.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		addPopularity.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		addPopularity.picture = getIdeologicalPicture(ideology);
		addPopularity.major = false;
		addPopularity.triggeredOnly = false;
		addPopularity.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		addPopularity.trigger += "		NOT = { has_government = " + ideology + " }\n";
		addPopularity.trigger += "		" + ideology + " > 0.2";
		addPopularity.meanTimeToHappen = "days = 365\n";
		addPopularity.meanTimeToHappen += "		modifier = {\n";
		addPopularity.meanTimeToHappen += "			factor = 0.5\n";
		addPopularity.meanTimeToHappen += "			has_country_flag = " + ideology + "_popular_movement\n";
		addPopularity.meanTimeToHappen += "		}";
		optionA = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		optionA += "		add_popularity = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			popularity = 0.15\n";
		optionA += "		}";
		addPopularity.options.push_back(optionA);
		politicalEvents.push_back(addPopularity);
		politicalEventNumber++;

		HoI4Event militaryDefections;
		militaryDefections.type = "country_event";
		militaryDefections.id = "conv.political." + to_string(politicalEventNumber);
		militaryDefections.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		militaryDefections.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		militaryDefections.picture = getIdeologicalPicture(ideology);
		militaryDefections.major = false;
		militaryDefections.triggeredOnly = false;
		militaryDefections.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		militaryDefections.trigger += "		NOT = { has_government = " + ideology + " }\n";
		militaryDefections.trigger += "		" + ideology + " > 0.3\n";
		militaryDefections.trigger += "		NOT = { has_idea = " + ideology + "_partisans_recruiting }";
		militaryDefections.meanTimeToHappen = "days = 365\n";
		militaryDefections.meanTimeToHappen += "		modifier = {\n";
		militaryDefections.meanTimeToHappen += "			factor = 0.5\n";
		militaryDefections.meanTimeToHappen += "			has_country_flag = " + ideology + "_popular_movement\n";
		militaryDefections.meanTimeToHappen += "		}\n";
		optionA = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		optionA += "		add_popularity = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			popularity = 0.15\n";
		optionA += "		}\n";
		optionA += "		add_ideas = " + ideology + "_partisans_recruiting";
		militaryDefections.options.push_back(optionA);
		politicalEvents.push_back(militaryDefections);
		politicalEventNumber++;

		HoI4Event civilWar;
		civilWar.type = "country_event";
		civilWar.id = "conv.political." + to_string(politicalEventNumber);
		civilWar.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		civilWar.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		civilWar.picture = getIdeologicalPicture(ideology);
		civilWar.major = false;
		civilWar.triggeredOnly = false;
		civilWar.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		civilWar.trigger += "		NOT = { has_government = " + ideology + " }\n";
		civilWar.trigger += "		" + ideology + " > 0.4";
		civilWar.meanTimeToHappen = "days = 365\n";
		civilWar.meanTimeToHappen += "		modifier = {\n";
		civilWar.meanTimeToHappen += "			factor = 0.75\n";
		civilWar.meanTimeToHappen += "			has_country_flag = " + ideology + "_popular_movement\n";
		civilWar.meanTimeToHappen += "		}\n";
		optionA = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		for (auto ideology2: majorIdeologies)
		{
			if (ideology == ideology2)
			{
				continue;
			}
			optionA += "		if = {\n";
			optionA += "			limit = {\n";
			optionA += "				has_government = " + ideology2 + "\n";
			optionA += "			}\n";
			optionA += "			start_civil_war = {\n";
			optionA += "				ruling_party = " + ideology + "\n";
			optionA += "				ideology = " + ideology2 + "\n";
			optionA += "				size = 0.6\n";
			optionA += "			}\n";
			optionA += "		}\n";
		}
		optionA += "		add_timed_idea = { idea = reign_of_terror days = 730 }\n";
		optionA += "		remove_ideas = " + ideology + "_revolutionaries";
		civilWar.options.push_back(optionA);
		optionB = "name = conv.political." + to_string(politicalEventNumber) + ".b\n";
		civilWar.options.push_back(optionB);
		politicalEvents.push_back(civilWar);
		politicalEventNumber++;
	}

	if (majorIdeologies.count("democratic") > 0)
	{
		addDemocraticMinisterRevolutionEvents(majorIdeologies);
	}
}


void HoI4Events::addDemocraticMinisterRevolutionEvents(const set<string>& majorIdeologies)
{
	HoI4Event onTheRise;
	onTheRise.type = "country_event";
	onTheRise.id = "conv.political.democratic.1";
	onTheRise.title = "conv.political.democratic.1.t";
	onTheRise.description = "conv.political.democratic.1.d";
	onTheRise.picture = getIdeologicalPicture("democratic");
	onTheRise.major = false;
	onTheRise.triggeredOnly = true;
	onTheRise.trigger = "NOT = { has_government = democratic }";
	string optionA = "name = conv.political.democratic.1.a\n";
	optionA += "		custom_effect_tooltip = conv.political.democratic.1.a.tt\n";
	optionA += "		clr_country_flag = democracy_popular_movement\n";
	optionA += "		clr_country_flag = democracy_opposition_formed\n";
	optionA += "		set_country_flag = democracy_political_movement";
	onTheRise.options.push_back(optionA);
	string optionB = "name = conv.political.democratic.1.b\n";
	optionB += "		custom_effect_tooltip = conv.political.democratic.1.b.tt\n";
	optionB += "		clr_country_flag = democracy_political_movement\n";
	optionB += "		clr_country_flag = democracy_government_support\n";
	optionB += "		set_country_flag = democracy_popular_movement";
	onTheRise.options.push_back(optionB);
	politicalEvents.push_back(onTheRise);
	politicalEventNumber++;

	HoI4Event ideologicalSympathies;
	ideologicalSympathies.type = "country_event";
	ideologicalSympathies.id = "conv.political." + to_string(politicalEventNumber);
	ideologicalSympathies.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	ideologicalSympathies.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	ideologicalSympathies.picture = getIdeologicalPicture("democratic");
	ideologicalSympathies.major = false;
	ideologicalSympathies.triggeredOnly = false;
	ideologicalSympathies.trigger = "has_idea_with_trait = democratic_minister\n";
	ideologicalSympathies.trigger += "		NOT = { has_government = democratic }\n";
	ideologicalSympathies.trigger += "		NOT = { has_country_flag = democracy_government_support }";
	ideologicalSympathies.meanTimeToHappen = "days = 400\n";
	ideologicalSympathies.meanTimeToHappen += "		modifier = {\n";
	ideologicalSympathies.meanTimeToHappen += "			factor = 0.5\n";
	ideologicalSympathies.meanTimeToHappen += "			has_country_flag = democracy_political_movement\n";
	ideologicalSympathies.meanTimeToHappen += "		}\n";
	ideologicalSympathies.meanTimeToHappen += "		modifier = {\n";
	ideologicalSympathies.meanTimeToHappen += "			factor = 0.5\n";
	ideologicalSympathies.meanTimeToHappen += "			democratic > 0.5\n";
	ideologicalSympathies.meanTimeToHappen += "		}";
	optionA = "name = political." + to_string(politicalEventNumber) + ".a\n";
	optionA += "		custom_effect_tooltip = political." + to_string(politicalEventNumber) + ".a.tt\n";
	optionA += "		clr_country_flag = democracy_political_movement\n";
	optionA += "		set_country_flag = democracy_government_support";
	ideologicalSympathies.options.push_back(optionA);
	politicalEvents.push_back(ideologicalSympathies);
	politicalEventNumber++;

	HoI4Event opposition;
	opposition.type = "country_event";
	opposition.id = "conv.political." + to_string(politicalEventNumber);
	opposition.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	opposition.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	opposition.picture = getIdeologicalPicture("democratic");
	opposition.major = false;
	opposition.triggeredOnly = false;
	opposition.trigger = "has_idea_with_trait = democratic_reformer\n";
	opposition.trigger += "		NOT = { has_government = democratic }\n";
	opposition.trigger += "		NOT = { has_country_flag = democracy_opposition_formed }\n";
	opposition.trigger += "		is_puppet = no";
	opposition.meanTimeToHappen = "days = 300\n";
	opposition.meanTimeToHappen += "		modifier = {\n";
	opposition.meanTimeToHappen += "			factor = 0.5\n";
	opposition.meanTimeToHappen += "			has_country_flag = democracy_popular_movement\n";
	opposition.meanTimeToHappen += "		}";
	optionA = "name = political." + to_string(politicalEventNumber) + ".a\n";
	optionA += "		add_popularity = {\n";
	optionA += "			ideology = democratic\n";
	optionA += "			popularity = 0.1\n";
	optionA += "		}\n";
	optionA += "		add_national_unity = -0.02\n";
	optionA += "		set_country_flag = democracy_opposition_formed";
	opposition.options.push_back(optionA);
	politicalEvents.push_back(opposition);
	politicalEventNumber++;

	HoI4Event callForElections;
	callForElections.type = "country_event";
	callForElections.id = "conv.political." + to_string(politicalEventNumber);
	callForElections.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	callForElections.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	callForElections.picture = getIdeologicalPicture("democratic");
	callForElections.major = false;
	callForElections.triggeredOnly = false;
	callForElections.trigger = "has_idea_with_trait = democratic_reformer\n";
	callForElections.trigger += "		NOT = { has_government = democratic }\n";
	callForElections.trigger += "		has_country_flag = democracy_opposition_formed\n";
	callForElections.trigger += "		NOT = { has_idea = democratic_opposition_voicing_protests }";
	callForElections.meanTimeToHappen = "days = 150";
	optionA = "name = political." + to_string(politicalEventNumber) + ".a";
	callForElections.options.push_back(optionA);
	optionB = "name = political." + to_string(politicalEventNumber) + ".b\n";
	optionB += "		add_political_power = -20\n";
	optionB += "		add_ideas = democratic_opposition_voicing_protests";
	callForElections.options.push_back(optionB);
	politicalEvents.push_back(callForElections);
	politicalEventNumber++;

	HoI4Event election;
	election.type = "country_event";
	election.id = "conv.political." + to_string(politicalEventNumber);
	election.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	election.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	election.picture = getIdeologicalPicture("democratic");
	election.major = false;
	election.triggeredOnly = false;
	election.trigger = "has_idea_with_trait = democratic_reformer\n";
	election.trigger += "		NOT = { has_government = democratic }\n";
	election.trigger += "		democratic > 0.5\n";
	election.trigger += "		has_country_flag = democracy_government_support\n";
	election.trigger += "		is_puppet = no";
	election.meanTimeToHappen = "days = 200";
	optionA = "name = political." + to_string(politicalEventNumber) + ".a\n";
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}
		optionA += "		if = {\n";
		optionA += "			limit = { has_government = " + ideology + " }\n";
		optionA += "			add_timed_idea = { idea = " + ideology + "_defeated days = 730 }\n";
		optionA += "		}\n";
	}
	optionA += "		set_politics = {\n";
	optionA += "			ruling_party = democratic\n";
	optionA += "			elections_allowed = yes\n";
	optionA += "		}\n";
	optionA += "		hold_election = ROOT\n";
	optionA += "		remove_ideas = democratic_revolutionaries\n";
	election.options.push_back(optionA);
	optionB = "name = political." + to_string(politicalEventNumber) + ".b";
	election.options.push_back(optionB);
	politicalEvents.push_back(election);
	politicalEventNumber++;
}


void HoI4Events::addFiftyPercentEvents(const set<string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if (ideology == "neutrality")
		{
			continue;
		}

		HoI4Event fiftyPercentEvent;
		fiftyPercentEvent.type = "country_event";
		fiftyPercentEvent.id = "conv.political." + to_string(politicalEventNumber);
		fiftyPercentEvent.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		fiftyPercentEvent.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		fiftyPercentEvent.picture = getIdeologicalPicture(ideology);
		fiftyPercentEvent.trigger = ideology + " > 0.5\n";
		fiftyPercentEvent.trigger += "		NOT = { has_government = " + ideology + " }\n";
		fiftyPercentEvent.trigger += "		NOT = { has_idea = " + ideology + "_revolutionaries }\n";
		fiftyPercentEvent.trigger += "		NOT = { has_idea = reign_of_terror }\n";
		fiftyPercentEvent.trigger += "		is_puppet = no\n";
		fiftyPercentEvent.meanTimeToHappen = "days = 30";
		string optionA = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		optionA += "		add_political_power = -100\n";
		optionA += "		add_popularity = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			popularity = -0.10\n";
		optionA += "		}\n";
		optionA += "		add_timed_idea = {\n";
		optionA += "			idea = " + ideology + "_revolutionaries\n";
		optionA += "			days = 365\n";
		optionA += "		}";
		fiftyPercentEvent.options.push_back(optionA);
		string optionB = "name = political." + to_string(politicalEventNumber) + ".b\n";
		optionB += "		add_timed_idea = {\n";
		optionB += "			idea = " + ideology + "_revolutionaries\n";
		optionB += "			days = 365\n";
		optionB += "		}";
		fiftyPercentEvent.options.push_back(optionB);
		string optionC = "name = political." + to_string(politicalEventNumber) + ".c\n";
		if (ideology == "democratic")
		{
			for (auto ideology2: majorIdeologies)
			{
				if ((ideology2 == "neutrality") || (ideology2 == ideology))
				{
					continue;
				}
				optionC += "		if = {\n";
				optionC += "			limit = { has_government = " + ideology2 + " }\n";
				optionC += "			add_timed_idea = { idea = " + ideology2 + "_defeated days = 730 }\n";
				optionC += "		}\n";
			}
		}
		optionC += "		set_politics = {\n";
		optionC += "			ruling_party = " + ideology + "\n";
		optionC += "			elections_allowed = ";
		(ideology == "democratic") ? optionC += "yes\n" : optionC += "no\n";
		optionC += "		}\n";
		optionC += "		add_national_unity = -0.05\n";
		fiftyPercentEvent.options.push_back(optionC);
		politicalEvents.push_back(fiftyPercentEvent);
		politicalEventNumber++;
	}
}


void HoI4Events::addRevolutionEvents(const set<string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if (ideology == "neutrality")
		{
			continue;
		}

		HoI4Event revolutionEvent;
		revolutionEvent.type = "country_event";
		revolutionEvent.id = "conv.political." + to_string(politicalEventNumber);
		revolutionEvent.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		revolutionEvent.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		revolutionEvent.picture = getIdeologicalPicture(ideology);
		revolutionEvent.trigger = ideology + " > 0.7\n";
		revolutionEvent.trigger += "		has_idea = " + ideology + "_revolutionaries";
		revolutionEvent.meanTimeToHappen = "days = 2";
		revolutionEvent.immediate = "hidden_effect = { remove_ideas = " + ideology + "_revolutionaries }";
		string optionA = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		optionA += "		start_civil_war = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			size = 0.5\n";
		optionA += "		}";
		revolutionEvent.options.push_back(optionA);
		string optionB = "name = conv.political." + to_string(politicalEventNumber) + ".b\n";
		for (auto ideology2: majorIdeologies)
		{
			if (ideology2 == ideology)
			{
				continue;
			}
			optionB += "		if = {\n";
			optionB += "			limit = {\n";
			optionB += "				has_government = " + ideology2 + "\n";
			optionB += "			}\n";
			if ((ideology == "democratic") && (ideology2 != "neutrality"))
			{
				optionB += "			add_timed_idea = { idea = " + ideology2 + "_defeated days = 730 }\n";
			}
			optionB += "			start_civil_war = {\n";
			optionB += "				ruling_party = " + ideology + "\n";
			optionB += "				ideology = " + ideology2 + "\n";
			optionB += "				size = 0.5\n";
			optionB += "			}\n";
			optionB += "		}\n";
		}
		revolutionEvent.options.push_back(optionB);
		politicalEvents.push_back(revolutionEvent);
		politicalEventNumber++;
	}
}


void HoI4Events::addSuppressedEvents(const set<string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if (ideology == "neutrality")
		{
			continue;
		}
		HoI4Event suppressedEvent;
		suppressedEvent.type = "country_event";
		suppressedEvent.id = "conv.political." + to_string(politicalEventNumber);
		suppressedEvent.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		suppressedEvent.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		suppressedEvent.picture = getIdeologicalPicture(ideology);
		suppressedEvent.trigger = ideology + " < 0.3\n";
		suppressedEvent.trigger += "		has_idea = " + ideology + "_revolutionaries";
		suppressedEvent.meanTimeToHappen = "days = 2";
		string option = "name = conv.political." + to_string(politicalEventNumber) + ".a\n";
		option += "		remove_ideas = " + ideology + "_revolutionaries\n";
		suppressedEvent.options.push_back(option);
		politicalEvents.push_back(suppressedEvent);
		politicalEventNumber++;
	}

	HoI4Event removeNeutral;
	removeNeutral.type = "country_event";
	removeNeutral.id = "conv.political." + to_string(politicalEventNumber);
	removeNeutral.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}
		removeNeutral.description += "	desc = {\n";
		removeNeutral.description += "		text = political.27.d_" + ideology + "\n";
		removeNeutral.description += "		trigger = { has_government = " + ideology + " }\n";
		removeNeutral.description += "	}\n";
	}
	removeNeutral.description = removeNeutral.description.substr(8, removeNeutral.description.size() - 9);
	removeNeutral.picture = "GFX_report_event_journalists_speech";
	removeNeutral.trigger = "OR = {\n";
	for (auto ideology: majorIdeologies)
	{
		removeNeutral.trigger += "		has_government = " + ideology + "\n";
	}
	removeNeutral.trigger += "		}";
	removeNeutral.meanTimeToHappen = "days = 2";
	string optionA = "name = political.27.a\n";
	optionA += "		remove_ideas = neutrality_idea";
	removeNeutral.options.push_back(optionA);
	politicalEvents.push_back(removeNeutral);
}


string HoI4Events::getIdeologicalPicture(const string& ideology)
{
	if (ideology == "communism")
	{
		return "GFX_report_event_worker_protests";
	}
	else if (ideology == "fascism")
	{
		return "GFX_report_event_fascist_gathering";
	}
	else if (ideology == "democratic")
	{
		return "GFX_report_event_journalists_speech";
	}
	else if (ideology == "radical")
	{
		return "GFX_report_event_gathering_protests";
	}
	else if (ideology == "absolutist")
	{
		return "GFX_report_event_generic_factory";
	}
	else
	{
		return "GFX_report_event_gathering_protests";
	}
}