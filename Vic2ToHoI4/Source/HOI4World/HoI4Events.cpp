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


void HoI4Events::createFactionEvents(const HoI4Country* Leader, HoI4Country* newAlly)
{
	string leaderName = Leader->getSourceCountry()->getName("english");
	string newAllyname = newAlly->getSourceCountry()->getName("english");

	HoI4Event nfEvent;
	nfEvent.type = "country_event";
	nfEvent.id = "NFEvents." + to_string(nationalFocusEventNumber++);
	nfEvent.title = "Alliance?";
	nfEvent.description = "Alliance with " + leaderName + "?";
	nfEvent.picture = "news_event_generic_sign_treaty1";
	nfEvent.major = false;

	string yesOption = "		name = \"Yes\"\n";
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

	string noOption = "		name = \"No\"\n";
	noOption += "		ai_chance = { factor = 0 }\n";
	noOption += "		hidden_effect = {\n";
	noOption += "			news_event = { id = news." + to_string(newsEventNumber + 1) + " }\n";
	noOption += "		}\n";
	nfEvent.options.push_back(noOption);
	nationalFocusEvents.push_back(nfEvent);

	HoI4Event newsEventYes;
	newsEventYes.type = "news_event";
	newsEventYes.id = "news." + to_string(newsEventNumber);
	newsEventYes.title = newAllyname + " Now an Ally with " + leaderName + "!";
	newsEventYes.description = "They are now allies";
	newsEventYes.picture = "news_event_generic_sign_treaty1";
	newsEventYes.major = true;
	string interestingOption = "		name = \"Interesting\"\n";
	newsEventYes.options.push_back(interestingOption);
	newsEvents.push_back(newsEventYes);

	HoI4Event newsEventNo;
	newsEventNo.type = "news_event";
	newsEventNo.id = "news." + to_string(newsEventNumber + 1);
	newsEventNo.title = newAllyname + " Refused the Alliance offer of " + leaderName + "!";
	newsEventNo.description = "They are not allies";
	newsEventNo.picture = "news_event_generic_sign_treaty1";
	newsEventNo.major = true;
	interestingOption = "		name = \"Interesting\"\n";
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
	annexEvent.title = annexername + " Demands " + annexedname + "!";
	annexEvent.description = "Today " + annexername + " sent an envoy to us with a proposition of an union. We are alone and in this world, and a union with " + annexername + " might prove to be fruiteful.";
	annexEvent.description += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!";
	annexEvent.picture = "GFX_report_event_hitler_parade";
	annexEvent.major = false;

	string acceptOption = "		name = \"We accept the Union\"\n";
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

	string refuseOption = "		name = \"We Refuse!\"\n";
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
	refusedEvent.title = annexedname + " Refuses!";
	refusedEvent.description = annexedname + " Refused our proposed union! This is an insult to us that cannot go unanswered!";
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.major = false;

	string refusedOption = "		name = \"It's time for war\"\n";
	refusedOption += "		create_wargoal = {\n";
	refusedOption += "			type = annex_everything\n";
	refusedOption += "			target = " + Annexed->getTag() + "\n";
	refusedOption += "		}\n";
	refusedEvent.options.push_back(refusedOption);

	nationalFocusEvents.push_back(refusedEvent);


	HoI4Event acceptedEvent;
	acceptedEvent.type = "country_event";
	acceptedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 1);
	acceptedEvent.title = annexedname + " accepts!";
	acceptedEvent.description = annexedname + " accepted our proposed union, their added strength will push us to greatness!";
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.major = false;

	string acceptedOption = "		name = \"A stronger Union!\"\n";
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
	sudatenEvent.title = annexername + " Demands " + annexedname + "!";
	sudatenEvent.description = annexername + " has recently been making claims to our bordering states, saying that these states are full of " + Annexer->getSourceCountry()->getAdjective("english") + " people and that the territory should be given to them. Although it ";
	sudatenEvent.description = "is true that recently our neighboring states have had an influx of " + Annexer->getSourceCountry()->getAdjective("english") + " people in the recent years, we cannot give up our lands because a few " + Annexer->getSourceCountry()->getAdjective("english") + " settled down in our land. ";
	sudatenEvent.description += "In response " + annexername + " has called for a conference, demanding their territory in exchange for peace. How do we resond? ";
	sudatenEvent.description += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!";
	sudatenEvent.picture = "GFX_report_event_hitler_parade";
	sudatenEvent.major = false;

	string acceptOption = "		name = \"We Accept\"\n";
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

	string refuseOption = "		name = \"We Refuse!\"\n";
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
	refusedEvent.title = annexedname + " Refuses!";
	refusedEvent.description = annexedname + " Refused our proposed proposition! This is an insult to us that cannot go unanswered!";
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.major = false;

	string refusedOption = "		name = \"It's time for war\"\n";
	refusedOption += "		create_wargoal = {\n";
	refusedOption += "				type = annex_everything\n";
	refusedOption += "			target = " + Annexed->getTag() + "\n";
	refusedOption += "		}\n";
	refusedEvent.options.push_back(refusedOption);

	nationalFocusEvents.push_back(refusedEvent);


	HoI4Event acceptedEvent;
	acceptedEvent.type = "country_event";
	acceptedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 1);
	acceptedEvent.title = annexedname + " accepts!";
	acceptedEvent.description = annexedname + " accepted our proposed demands, the added lands will push us to greatness!";
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.major = false;

	string acceptedOption = "		name = \"A stronger Union!\"\n";
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
	tradeIncidentEvent.title = "Trade Incident";
	tradeIncidentEvent.description = "One of our convoys was sunk by " + GC->getSourceCountry()->getName("english");
	tradeIncidentEvent.picture = "GFX_report_event_chinese_soldiers_fighting";
	tradeIncidentEvent.major = false;
	tradeIncidentEvent.trigger = "		has_country_flag = established_traders\n";
	tradeIncidentEvent.trigger += "		NOT = { has_country_flag = established_traders_activated }\n";

	string option = "		name = \"They will Pay!\"\n";
	option += "		ai_chance = { factor = 85 }\n";
	option += "		effect_tooltip = {\n";
	option += "			" + leader->getTag() + " = {\n";
	option += "				set_country_flag = established_traders_activated\n";
	option += "				create_wargoal = {\n";
	option += "					type = annex_everything\n";
	option += "					target = " + GC->getTag() + "\n";
	option += "				}\n";
	option += "			}\n";
	option += "		}\n";
	tradeIncidentEvent.options.push_back(option);

	nationalFocusEvents.push_back(tradeIncidentEvent);
}