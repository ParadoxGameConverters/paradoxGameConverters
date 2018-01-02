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
#include "HoI4Localisation.h"
#include "HoI4OnActions.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../V2World/V2Party.h"



HoI4Events::HoI4Events():
	newsEvents(),
	newsEventNumber(237),
	nationalFocusEvents(),
	nationalFocusEventNumber(0),
	politicalEvents(),
	politicalEventNumber(1),
	warJustificationEvents(),
	electionEvents(),
	electionEventNumber(4)
{
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
	outputWarJustificationEvents();
	outputElectionEvents();
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
	for (auto& theEvent: nationalFocusEvents)
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
	for (auto& theEvent: newsEvents)
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
	for (auto& theEvent: politicalEvents)
	{
		outPoliticalEvents << "\n";
		outPoliticalEvents << theEvent;
	}

	outPoliticalEvents.close();
}


void HoI4Events::outputWarJustificationEvents() const
{
	ofstream outWarJustificationEvents("output/" + Configuration::getOutputName() + "/events/WarJustification.txt", ios_base::app);
	if (!outWarJustificationEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not open WarJustification.txt";
		exit(-1);
	}

	for (auto& theEvent: warJustificationEvents)
	{
		outWarJustificationEvents << "\n";
		outWarJustificationEvents << theEvent;
	}

	outWarJustificationEvents.close();
}


void HoI4Events::outputElectionEvents() const
{
	ofstream outElectionEvents("output/" + Configuration::getOutputName() + "/events/ElectionEvents.txt", ios_base::app);
	if (!outElectionEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not open ElectionEvents.txt";
		exit(-1);
	}

	for (auto& theEvent: electionEvents)
	{
		outElectionEvents << "\n";
		outElectionEvents << theEvent;
	}

	outElectionEvents.close();
}


void HoI4Events::createFactionEvents(shared_ptr<HoI4Country> Leader, shared_ptr<HoI4Country> newAlly)
{
	auto possibleLeaderName = Leader->getSourceCountry()->getName("english");
	string leaderName;
	if (possibleLeaderName)
	{
		leaderName = *possibleLeaderName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine leader name for faction events";
		leaderName = "";
	}

	auto possibleNewAllyName = newAlly->getSourceCountry()->getName("english");
	string newAllyName;
	if (possibleNewAllyName)
	{
		newAllyName = *possibleNewAllyName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine new ally name for faction events";
		newAllyName = *possibleNewAllyName;
	}

	HoI4Event nfEvent;
	nfEvent.type = "country_event";
	nfEvent.id = "NFEvents." + to_string(nationalFocusEventNumber++);
	nfEvent.title = "\"Alliance?\"";
	nfEvent.description = "\"Alliance with " + leaderName + "?\"";
	nfEvent.picture = "news_event_generic_sign_treaty1";
	nfEvent.majorEvent = false;
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
	newsEventYes.title = "\"" + newAllyName + " Now an Ally with " + leaderName + "!\"";
	newsEventYes.description = "\"They are now allies\"";
	newsEventYes.picture = "news_event_generic_sign_treaty1";
	newsEventYes.majorEvent = true;
	newsEventYes.triggeredOnly = true;
	string interestingOption = "name = \"Interesting\"\n";
	newsEventYes.options.push_back(interestingOption);
	newsEvents.push_back(newsEventYes);

	HoI4Event newsEventNo;
	newsEventNo.type = "news_event";
	newsEventNo.id = "news." + to_string(newsEventNumber + 1);
	newsEventNo.title = "\"" + newAllyName + " Refused the Alliance offer of " + leaderName + "!\"";
	newsEventNo.description = "\"They are not allies\"";
	newsEventNo.picture = "news_event_generic_sign_treaty1";
	newsEventNo.majorEvent = true;
	newsEventNo.triggeredOnly = true;
	interestingOption = "name = \"Interesting\"\n";
	newsEventNo.options.push_back(interestingOption);
	newsEvents.push_back(newsEventNo);

	newsEventNumber += 2;
}


void HoI4Events::createAnnexEvent(shared_ptr<HoI4Country> Annexer, shared_ptr<HoI4Country> Annexed)
{
	auto possibleAnnexerName = Annexer->getSourceCountry()->getName("english");
	string annexerName;
	if (possibleAnnexerName)
	{
		annexerName = *possibleAnnexerName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine annexer name for annexation events";
		annexerName = "";
	}

	auto possibleAnnexedName = Annexed->getSourceCountry()->getName("english");
	string annexedName;
	if (possibleAnnexedName)
	{
		annexedName = *possibleAnnexedName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine annexed country name for annexation events";
		annexedName = "";
	}

	HoI4Event annexEvent;
	annexEvent.type = "country_event";
	annexEvent.id = "NFEvents." + to_string(nationalFocusEventNumber);
	annexEvent.title = "\"" + annexerName + " Demands " + annexedName + "!\"";
	annexEvent.description = "\"Today " + annexerName + " sent an envoy to us with a proposition of an union. We are alone and in this world, and a union with " + annexerName + " might prove to be fruiteful.";
	annexEvent.description += " Our people would be safe with the mighty army of " + annexerName + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"";
	annexEvent.picture = "GFX_report_event_hitler_parade";
	annexEvent.majorEvent = false;
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
	refusedEvent.title = "\"" + annexedName + " Refuses!\"";
	refusedEvent.description = "\"" + annexedName + " Refused our proposed union! This is an insult to us that cannot go unanswered!\"";
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.majorEvent = false;
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
	acceptedEvent.title = "\"" + annexedName + " accepts!\"";
	acceptedEvent.description = "\"" + annexedName + " accepted our proposed union, their added strength will push us to greatness!\"";
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.majorEvent = false;
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
	acceptedOption += "		add_named_threat = { threat = 2 name = \"" + annexedName + " annexed " + annexedName + "\" }\n";
	acceptedOption += "		set_country_flag = " + Annexed->getTag() + "_annexed\n";
	acceptedEvent.options.push_back(acceptedOption);

	nationalFocusEvents.push_back(acceptedEvent);


	nationalFocusEventNumber += 3;
}


void HoI4Events::createSudetenEvent(shared_ptr<HoI4Country> Annexer, shared_ptr<HoI4Country> Annexed, const vector<int>& claimedStates)
{
	//flesh out this event more, possibly make it so allies have a chance to help?
	auto possibleAnnexerName = Annexer->getSourceCountry()->getName("english");
	string annexerName;
	if (possibleAnnexerName)
	{
		annexerName = *possibleAnnexerName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine annexer name for sudeten events";
		annexerName = "";
	}

	auto possibleAnnexerAdjective = Annexer->getSourceCountry()->getName("english");
	string annexerAdjctive;
	if (possibleAnnexerAdjective)
	{
		annexerAdjctive = *possibleAnnexerAdjective;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine annexer adjective for sudeten events";
		annexerAdjctive = "";
	}

	auto possibleAnnexedName = Annexed->getSourceCountry()->getName("english");
	string annexedName;
	if (possibleAnnexedName)
	{
		annexedName = *possibleAnnexedName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine annexed country name for sudeten events";
		annexedName = "";
	}

	HoI4Event sudetenEvent;
	sudetenEvent.type = "country_event";
	sudetenEvent.id = "NFEvents." + to_string(nationalFocusEventNumber);
	sudetenEvent.title = annexerName + " Demands " + annexedName + "!";
	sudetenEvent.description = "\"" + annexerName + " has recently been making claims to our bordering states, saying that these states are full of " + annexerAdjctive + " people and that the territory should be given to them. Although it ";
	sudetenEvent.description += "is true that recently our neighboring states have had an influx of " + annexerAdjctive + " people in the recent years, we cannot give up our lands because a few " + annexerAdjctive + " settled down in our land. ";
	sudetenEvent.description += "In response " + annexerName + " has called for a conference, demanding their territory in exchange for peace. How do we resond? ";
	sudetenEvent.description += " Our people would be safe with the mighty army of " + annexerName + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"";
	sudetenEvent.picture = "GFX_report_event_hitler_parade";
	sudetenEvent.majorEvent = false;
	sudetenEvent.triggeredOnly = true;

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
	sudetenEvent.options.push_back(acceptOption);

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
	sudetenEvent.options.push_back(refuseOption);

	nationalFocusEvents.push_back(sudetenEvent);


	HoI4Event refusedEvent;
	refusedEvent.type = "country_event";
	refusedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 2);
	refusedEvent.title = "\"" + annexedName + " Refuses!\"";
	refusedEvent.description = "\"" + annexedName + " Refused our proposed proposition! This is an insult to us that cannot go unanswered!\"";
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.majorEvent = false;
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
	acceptedEvent.title = "\"" + annexedName + " accepts!\"";
	acceptedEvent.description = "\"" + annexedName + " accepted our proposed demands, the added lands will push us to greatness!\"";
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.majorEvent = false;
	acceptedEvent.triggeredOnly = true;

	string acceptedOption = "name = \"A stronger Union!\"\n";
	for (unsigned int i = 0; i <= 1 && i < claimedStates.size(); i++)
	{
		acceptedOption += "		" + to_string(claimedStates[i]) + " = { add_core_of = " + Annexer->getTag() + " }\n";
		acceptedOption += "		" + Annexer->getTag() + " = { transfer_state =  " + to_string(claimedStates[i]) + " }\n";
	}
	acceptedOption += "		set_country_flag = " + Annexed->getTag() + "_demanded\n";
	acceptedEvent.options.push_back(acceptedOption);

	nationalFocusEvents.push_back(acceptedEvent);


	nationalFocusEventNumber += 3;
}


void HoI4Events::createTradeEvent(shared_ptr<HoI4Country> leader, shared_ptr<HoI4Country> GC)
{
	auto possibleAggressorName = GC->getSourceCountry()->getName("english");
	string aggressorName;
	if (possibleAggressorName)
	{
		aggressorName = *possibleAggressorName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine aggressor name for trade events";
		aggressorName = "";
	}

	HoI4Event tradeIncidentEvent;
	tradeIncidentEvent.type = "country_event";
	tradeIncidentEvent.id = "NFEvents." + to_string(nationalFocusEventNumber++);
	tradeIncidentEvent.title = "\"Trade Incident\"";
	tradeIncidentEvent.description = "\"One of our convoys was sunk by " + aggressorName + "\"";
	tradeIncidentEvent.picture = "GFX_report_event_chinese_soldiers_fighting";
	tradeIncidentEvent.majorEvent = false;
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
		onTheRise.id = "conv.political." + to_string(politicalEventNumber);
		onTheRise.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_on_the_rise.t", onTheRise.title);
		onTheRise.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_on_the_rise.d", onTheRise.description);
		onTheRise.picture = getIdeologicalPicture(ideology);
		onTheRise.majorEvent = false;
		onTheRise.triggeredOnly = true;
		onTheRise.trigger = "NOT = { has_government = " + ideology + " }";
		string optionA = "name = ";
		string optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_on_the_rise.a", optionName);
		HoI4Localisation::copyEventLocalisations(ideology + "_on_the_rise.a.tt", optionName + ".tt");
		optionA += optionName + "\n";
		optionA += "		custom_effect_tooltip = conv.political." + to_string(politicalEventNumber) + ".a.tt\n";
		optionA += "		clr_country_flag = " + ideology + "_popular_movement\n";
		optionA += "		set_country_flag = " + ideology + "_political_movement";
		onTheRise.options.push_back(optionA);
		string optionB = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
		HoI4Localisation::copyEventLocalisations(ideology + "_on_the_rise.b", optionName);
		HoI4Localisation::copyEventLocalisations(ideology + "_on_the_rise.b.tt", optionName + ".tt");
		optionB += optionName + "\n";
		optionB += "		custom_effect_tooltip = conv.political." + to_string(politicalEventNumber) + ".b.tt\n";
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
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_sympathies.t", ideologicalSympathies.title);
		ideologicalSympathies.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_sympathies.d", ideologicalSympathies.description);
		ideologicalSympathies.picture = getIdeologicalPicture(ideology);
		ideologicalSympathies.majorEvent = false;
		ideologicalSympathies.triggeredOnly = false;
		ideologicalSympathies.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		ideologicalSympathies.trigger += "		NOT = { has_government = " + ideology + " }\n";
		ideologicalSympathies.trigger += "		NOT = { has_country_flag = " + ideology + "_government_support }";
		ideologicalSympathies.meanTimeToHappen = "days = 500\n";
		ideologicalSympathies.meanTimeToHappen += "		modifier = {\n";
		ideologicalSympathies.meanTimeToHappen += "			factor = 0.5\n";
		ideologicalSympathies.meanTimeToHappen += "			has_country_flag = " + ideology + "_political_movement\n";
		ideologicalSympathies.meanTimeToHappen += "		}";
		optionA = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_sympathies.a", optionName);
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_sympathies.a.tt", optionName + ".tt");
		optionA += optionName + "\n";
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
		HoI4Localisation::copyEventLocalisations(ideology + "_coup.t", coup.title);
		coup.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_coup.d", coup.description);
		coup.picture = getIdeologicalPicture(ideology);
		coup.majorEvent = false;
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
		optionA = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_coup.a", optionName);
		optionA += optionName + "\n";
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
		HoI4Localisation::copyEventLocalisations(ideology + "_add_popularity.t", addPopularity.title);
		addPopularity.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_add_popularity.d", addPopularity.description);
		addPopularity.picture = getIdeologicalPicture(ideology);
		addPopularity.majorEvent = false;
		addPopularity.triggeredOnly = false;
		addPopularity.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		addPopularity.trigger += "		NOT = { has_government = " + ideology + " }\n";
		addPopularity.trigger += "		" + ideology + " > 0.2";
		addPopularity.meanTimeToHappen = "days = 365\n";
		addPopularity.meanTimeToHappen += "		modifier = {\n";
		addPopularity.meanTimeToHappen += "			factor = 0.5\n";
		addPopularity.meanTimeToHappen += "			has_country_flag = " + ideology + "_popular_movement\n";
		addPopularity.meanTimeToHappen += "		}";
		optionA = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_add_popularity.a", optionName);
		optionA += optionName + "\n";
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
		HoI4Localisation::copyEventLocalisations(ideology + "_military_defections.t", militaryDefections.title);
		militaryDefections.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_military_defections.d", militaryDefections.description);
		militaryDefections.picture = getIdeologicalPicture(ideology);
		militaryDefections.majorEvent = false;
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
		optionA = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_military_defections.a", optionName);
		optionA += optionName + "\n";
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
		HoI4Localisation::copyEventLocalisations(ideology + "_civil_war.t", civilWar.title);
		civilWar.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_civil_war.t", civilWar.description);
		civilWar.picture = getIdeologicalPicture(ideology);
		civilWar.majorEvent = false;
		civilWar.triggeredOnly = false;
		civilWar.trigger = "has_idea_with_trait = " + ideology + "_minister\n";
		civilWar.trigger += "		NOT = { has_government = " + ideology + " }\n";
		civilWar.trigger += "		" + ideology + " > 0.4";
		civilWar.meanTimeToHappen = "days = 365\n";
		civilWar.meanTimeToHappen += "		modifier = {\n";
		civilWar.meanTimeToHappen += "			factor = 0.75\n";
		civilWar.meanTimeToHappen += "			has_country_flag = " + ideology + "_popular_movement\n";
		civilWar.meanTimeToHappen += "		}\n";
		optionA = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_civil_war.a", optionName);
		optionA += optionName + "\n";
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
		optionB = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
		HoI4Localisation::copyEventLocalisations(ideology + "_civil_war.b", optionName);
		optionB += optionName + "\n";
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
	onTheRise.id = "conv.political." + to_string(politicalEventNumber);
	onTheRise.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("democratic_on_the_rise.t", onTheRise.title);
	onTheRise.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("democratic_on_the_rise.d", onTheRise.description);
	onTheRise.picture = getIdeologicalPicture("democratic");
	onTheRise.majorEvent = false;
	onTheRise.triggeredOnly = true;
	onTheRise.trigger = "NOT = { has_government = democratic }";
	string optionA = "name = ";
	string optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
	HoI4Localisation::copyEventLocalisations("democratic_on_the_rise.a", optionName);
	HoI4Localisation::copyEventLocalisations("democratic_on_the_rise.a.tt", optionName + ".tt");
	optionA += optionName + "\n";
	optionA += "		custom_effect_tooltip = conv.political." + to_string(politicalEventNumber) + ".a.tt\n";
	optionA += "		clr_country_flag = democracy_popular_movement\n";
	optionA += "		clr_country_flag = democracy_opposition_formed\n";
	optionA += "		set_country_flag = democracy_political_movement";
	onTheRise.options.push_back(optionA);
	string optionB = "name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
	optionB += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_on_the_rise.b", optionName);
	HoI4Localisation::copyEventLocalisations("democratic_on_the_rise.b.tt", optionName + ".tt");
	optionB += "		custom_effect_tooltip = conv.political." + to_string(politicalEventNumber) + ".b.tt\n";
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
	HoI4Localisation::copyEventLocalisations("democratic_ideological_sympathies.t", ideologicalSympathies.title);
	ideologicalSympathies.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("democratic_ideological_sympathies.d", ideologicalSympathies.description);
	ideologicalSympathies.picture = getIdeologicalPicture("democratic");
	ideologicalSympathies.majorEvent = false;
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
	optionA = "name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
	optionA += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_ideological_sympathies.a", optionName);
	HoI4Localisation::copyEventLocalisations("democratic_ideological_sympathies.a.tt", optionName + ".tt");
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
	HoI4Localisation::copyEventLocalisations("democratic_opposition_forming.t", opposition.title);
	opposition.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("democratic_opposition_forming.d", opposition.description);
	opposition.picture = getIdeologicalPicture("democratic");
	opposition.majorEvent = false;
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
	optionA = "name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
	optionA += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_opposition_forming.a", optionName);
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
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.t", callForElections.title);
	callForElections.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.d", callForElections.description);
	callForElections.picture = getIdeologicalPicture("democratic");
	callForElections.majorEvent = false;
	callForElections.triggeredOnly = false;
	callForElections.trigger = "has_idea_with_trait = democratic_reformer\n";
	callForElections.trigger += "		NOT = { has_government = democratic }\n";
	callForElections.trigger += "		has_country_flag = democracy_opposition_formed\n";
	callForElections.trigger += "		NOT = { has_idea = democratic_opposition_voicing_protests }";
	callForElections.meanTimeToHappen = "days = 150";
	optionA = "name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
	optionA += optionName;
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.a", optionName);
	callForElections.options.push_back(optionA);
	optionB = "name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
	optionB += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.b", optionName);
	optionB += "		add_political_power = -20\n";
	optionB += "		add_ideas = democratic_opposition_voicing_protests";
	callForElections.options.push_back(optionB);
	politicalEvents.push_back(callForElections);
	politicalEventNumber++;

	HoI4Event election;
	election.type = "country_event";
	election.id = "conv.political." + to_string(politicalEventNumber);
	election.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("democratic_election.t", election.title);
	election.description = "conv.political." + to_string(politicalEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("democratic_election.d", election.description);
	election.picture = getIdeologicalPicture("democratic");
	election.majorEvent = false;
	election.triggeredOnly = false;
	election.trigger = "has_idea_with_trait = democratic_reformer\n";
	election.trigger += "		NOT = { has_government = democratic }\n";
	election.trigger += "		democratic > 0.5\n";
	election.trigger += "		has_country_flag = democracy_government_support\n";
	election.trigger += "		is_puppet = no";
	election.meanTimeToHappen = "days = 200";
	optionA = "name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
	optionA += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_election.a", optionName);
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
	optionB = "name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
	optionB += optionName;
	HoI4Localisation::copyEventLocalisations("democratic_election.b", optionName);
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
		HoI4Localisation::copyEventLocalisations(ideology + "_fifty_percent.t", fiftyPercentEvent.title);
		fiftyPercentEvent.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_fifty_percent.d", fiftyPercentEvent.description);
		fiftyPercentEvent.picture = getIdeologicalPicture(ideology);
		fiftyPercentEvent.majorEvent = false;
		fiftyPercentEvent.triggeredOnly = false;
		fiftyPercentEvent.trigger = ideology + " > 0.5\n";
		fiftyPercentEvent.trigger += "		NOT = { has_government = " + ideology + " }\n";
		fiftyPercentEvent.trigger += "		NOT = { has_idea = " + ideology + "_revolutionaries }\n";
		fiftyPercentEvent.trigger += "		NOT = { has_idea = reign_of_terror }\n";
		fiftyPercentEvent.trigger += "		is_puppet = no\n";
		fiftyPercentEvent.meanTimeToHappen = "days = 30";
		string optionA = "name = ";
		string optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		optionA += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_fifty_percent.a", optionName);
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
		string optionB = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
		optionB += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_fifty_percent.b", optionName);
		optionB += "		add_timed_idea = {\n";
		optionB += "			idea = " + ideology + "_revolutionaries\n";
		optionB += "			days = 365\n";
		optionB += "		}";
		fiftyPercentEvent.options.push_back(optionB);
		string optionC = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".c";
		optionC += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_fifty_percent.c", optionName);
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
		if (ideology == "democratic")
		{
			optionC += "		hold_election = ROOT\n";
		}
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
		HoI4Localisation::copyEventLocalisations(ideology + "_revolution_event.t", revolutionEvent.title);
		revolutionEvent.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_revolution_event.d", revolutionEvent.description);
		revolutionEvent.picture = getIdeologicalPicture(ideology);
		revolutionEvent.majorEvent = false;
		revolutionEvent.triggeredOnly = false;
		revolutionEvent.trigger = ideology + " > 0.7\n";
		revolutionEvent.trigger += "		has_idea = " + ideology + "_revolutionaries";
		revolutionEvent.meanTimeToHappen = "days = 2";
		revolutionEvent.immediate = "hidden_effect = { remove_ideas = " + ideology + "_revolutionaries }";
		string optionA = "name = ";
		string optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		optionA += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_revolution_event.a", optionName);
		optionA += "		start_civil_war = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			size = 0.5\n";
		optionA += "		}";
		revolutionEvent.options.push_back(optionA);
		string optionB = "name = ";
		optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
		optionB += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_revolution_event.b", optionName);
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
			if (ideology == "democratic")
			{
				optionB += "			hold_election = ROOT\n";
			}
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
		HoI4Localisation::copyEventLocalisations(ideology + "_suppressed_event.t", suppressedEvent.title);
		suppressedEvent.description = "conv.political." + to_string(politicalEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_suppressed_event.d", suppressedEvent.description);
		suppressedEvent.picture = getIdeologicalPicture(ideology);
		suppressedEvent.majorEvent = false;
		suppressedEvent.triggeredOnly = false;
		suppressedEvent.trigger = ideology + " < 0.3\n";
		suppressedEvent.trigger += "		has_idea = " + ideology + "_revolutionaries";
		suppressedEvent.meanTimeToHappen = "days = 2";
		string option = "name = ";
		string optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		option += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_suppressed_event.a", optionName);
		option += "		remove_ideas = " + ideology + "_revolutionaries\n";
		suppressedEvent.options.push_back(option);
		politicalEvents.push_back(suppressedEvent);
		politicalEventNumber++;
	}

	int numRelevantIdeologies = 0;
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}
		numRelevantIdeologies++;
	}
	if (numRelevantIdeologies > 0)
	{
		HoI4Event removeNeutral;
		removeNeutral.type = "country_event";
		removeNeutral.id = "conv.political." + to_string(politicalEventNumber);
		removeNeutral.title = "conv.political." + to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations("abandon_neutral.t", removeNeutral.title);
		for (auto ideology: majorIdeologies)
		{
			if ((ideology == "democratic") || (ideology == "neutrality"))
			{
				continue;
			}
			removeNeutral.description += "	desc = {\n";
			removeNeutral.description += "		text = political." + to_string(politicalEventNumber) + ".d_" + ideology + "\n";
			removeNeutral.description += "		trigger = { has_government = " + ideology + " }\n";
			removeNeutral.description += "	}\n";
			HoI4Localisation::copyEventLocalisations("abandon_neutral.d_" + ideology, "conv.political." + to_string(politicalEventNumber) + "_" + ideology);
		}
		if (removeNeutral.description.size() > 0)
		{
			removeNeutral.description = removeNeutral.description.substr(8, removeNeutral.description.size() - 9);
		}
		removeNeutral.picture = "GFX_report_event_journalists_speech";
		removeNeutral.majorEvent = false;
		removeNeutral.triggeredOnly = false;
		removeNeutral.trigger = "OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology == "democratic") || (ideology == "neutrality"))
			{
				continue;
			}
			removeNeutral.trigger += "			has_government = " + ideology + "\n";
		}
		removeNeutral.trigger += "		}\n";
		removeNeutral.trigger += "		has_idea = neutrality_idea";
		removeNeutral.meanTimeToHappen = "days = 2";
		string option = "name = ";
		string optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		option += optionName + "\n";
		HoI4Localisation::copyEventLocalisations("abandon_neutral.a", optionName);
		option += "		remove_ideas = neutrality_idea";
		removeNeutral.options.push_back(option);
		politicalEvents.push_back(removeNeutral);
	}
}


string HoI4Events::getIdeologicalPicture(const string& ideology) const
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


void HoI4Events::createWarJustificationEvents(const set<string>& majorIdeologies)
{
	HoI4Event wargoalExpired;
	wargoalExpired.type = "country_event";
	wargoalExpired.id = "war_justification.301";
	wargoalExpired.title = "war_justification.301.t";
	wargoalExpired.description = "war_justification.301.d";
	wargoalExpired.picture = "GFX_report_event_iww_demonstration";
	wargoalExpired.majorEvent = false;
	wargoalExpired.triggeredOnly = true;
	wargoalExpired.trigger = "has_war = no";

	char letter = 'a';
	for (auto ideology: majorIdeologies)
	{
		string option = "name = war_justification.301." + string(1, letter) + "\n";
		option += "		trigger = { has_government = " + ideology + " }\n";
		option += "		add_political_power = -30\n";
		option += "		add_national_unity = -0.03\n";
		if (ideology != "neutrality")
		{
			option += "		add_popularity = {\n";
			option += "			ideology = " + ideology + "\n";
			option += "			popularity = -0.05\n";
			option += "		}";
		}
		wargoalExpired.options.push_back(option);
		letter++;
	}
	warJustificationEvents.push_back(wargoalExpired);
}


void HoI4Events::createElectionEvents(const set<string>& majorIdeologies, HoI4OnActions* onActions)
{
	if (majorIdeologies.count("democratic") > 0)
	{
		addIdeologyInGovernmentEvents(majorIdeologies, onActions);
		addIdeologyInfluenceForeignPolicyEvents(majorIdeologies);
		addDemocraticPartiesInMinorityEvent(majorIdeologies, onActions);
		addIdeologicalMajorityEvent(majorIdeologies, onActions);
		addWartimeExceptionEvent(majorIdeologies, onActions);
		addGovernmentContestedEvent(majorIdeologies, onActions);
	}
}


void HoI4Events::addIdeologyInGovernmentEvents(const set<string>& majorIdeologies, HoI4OnActions* onActions)
{
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		HoI4Event ideologyInGovernment;
		ideologyInGovernment.type = "country_event";
		ideologyInGovernment.id = "election." + to_string(electionEventNumber);
		ideologyInGovernment.title = "election." + to_string(electionEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_in_government.t", ideologyInGovernment.title);
		ideologyInGovernment.description = "election." + to_string(electionEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_in_government.d", ideologyInGovernment.description);
		ideologyInGovernment.picture = getIdeologicalPicture(ideology);
		ideologyInGovernment.triggeredOnly = true;
		ideologyInGovernment.trigger = "NOT = { has_government = " + ideology + " }\n";
		ideologyInGovernment.trigger += "		NOT = { has_idea = " + ideology + "_defeated }\n";
		ideologyInGovernment.trigger += "		" + ideology + " > 0.15";
		string optionAName = "election." + to_string(electionEventNumber) + ".a";
		string optionA = "name = " + optionAName + "\n";
		optionA += "		ai_chance = {\n";
		optionA += "			base = 0\n";
		optionA += "			modifier = {\n";
		optionA += "				add = 2\n";
		optionA += "				can_lose_democracy_support = yes\n";
		optionA += "			}\n";
		optionA += "			modifier = {\n";
		optionA += "				add = 1\n";
		optionA += "				can_lose_unity = no\n";
		optionA += "			}\n";
		optionA += "		}\n";
		optionA += "		add_popularity = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			popularity = 0.1\n";
		optionA += "		}\n";
		optionA += "		set_country_flag = coalition_with_" + ideology;
		ideologyInGovernment.options.push_back(optionA);
		HoI4Localisation::copyEventLocalisations(ideology + "_in_government.a", optionAName);
		string optionBName = "election." + to_string(electionEventNumber) + ".b";
		string optionB = "name = " + optionBName + "\n";
		optionB += "		ai_chance = {\n";
		optionB += "			base = 0\n";
		optionB += "			modifier = {\n";
		optionB += "				add = 10\n";
		optionB += "				can_lose_unity = yes\n";
		optionB += "			}\n";
		optionB += "		}\n";
		optionB += "		add_national_unity = -0.05\n";
		optionB += "		add_popularity = {\n";
		optionB += "			ideology = democratic\n";
		optionB += "			popularity = 0.03\n";
		optionB += "		}\n";
		optionB += "		set_country_flag = government_declined_" + ideology;
		ideologyInGovernment.options.push_back(optionB);
		HoI4Localisation::copyEventLocalisations(ideology + "_in_government.b", optionBName);

		electionEvents.push_back(ideologyInGovernment);
		onActions->addElectionEvent(ideologyInGovernment.id);
		electionEventNumber++;
	}
}


void HoI4Events::addIdeologyInfluenceForeignPolicyEvents(const set<string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		HoI4Event ideologyInfluenceForeignPolicy;
		ideologyInfluenceForeignPolicy.type = "country_event";
		ideologyInfluenceForeignPolicy.id = "election." + to_string(electionEventNumber);
		ideologyInfluenceForeignPolicy.title = "election." + to_string(electionEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_influence_foreign_policy.t", ideologyInfluenceForeignPolicy.title);
		ideologyInfluenceForeignPolicy.description = "election." + to_string(electionEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_influence_foreign_policy.d", ideologyInfluenceForeignPolicy.description);
		ideologyInfluenceForeignPolicy.picture = getIdeologicalPicture(ideology);
		ideologyInfluenceForeignPolicy.trigger = "NOT = { has_government = " + ideology + " }\n";
		ideologyInfluenceForeignPolicy.trigger += "		has_country_flag = coalition_with_" + ideology + "\n";
		ideologyInfluenceForeignPolicy.trigger += "		any_other_country = {\n";
		ideologyInfluenceForeignPolicy.trigger += "			has_government = " + ideology + "\n";
		ideologyInfluenceForeignPolicy.trigger += "			is_faction_leader = yes\n";
		ideologyInfluenceForeignPolicy.trigger += "		}";
		ideologyInfluenceForeignPolicy.meanTimeToHappen = "days = 730";
		string optionAName = "election." + to_string(electionEventNumber) + ".a";
		string optionA = "name = " + optionAName + "\n";
		optionA += "		ai_chance = {\n";
		optionA += "			base = 0\n";
		optionA += "			modifier = {\n";
		optionA += "				add = 2\n";
		optionA += "				can_lose_democracy_support = yes\n";
		optionA += "			}\n";
		optionA += "			modifier = {\n";
		optionA += "				add = 1\n";
		optionA += "				can_lose_unity = no\n";
		optionA += "			}\n";
		optionA += "		}\n";
		optionA += "		random_other_country = {\n";
		optionA += "			limit = {\n";
		optionA += "				has_government = " + ideology + "\n";
		optionA += "				is_faction_leader = yes\n";
		optionA += "			}\n";
		optionA += "			add_opinion_modifier = {\n";
		optionA += "				target = ROOT\n";
		optionA += "				modifier = " + ideology + "_in_government\n";
		optionA += "			}\n";
		optionA += "		}\n";
		optionA += "		add_popularity = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			popularity = 0.05\n";
		optionA += "		}";
		ideologyInfluenceForeignPolicy.options.push_back(optionA);
		HoI4Localisation::copyEventLocalisations(ideology + "_influence_foreign_policy.a", optionAName);
		string optionBName = "election." + to_string(electionEventNumber) + ".b";
		string optionB = "name = " + optionBName + "\n";
		optionB += "		ai_chance = {\n";
		optionB += "			base = 10\n";
		optionB += "			modifier = {\n";
		optionB += "				factor = 0\n";
		optionB += "				can_lose_unity = no\n";
		optionB += "			}\n";
		optionB += "		}\n";
		optionB += "		add_national_unity = -0.05\n";
		optionB += "		add_popularity = {\n";
		optionB += "			ideology = democratic\n";
		optionB += "			popularity = 0.03\n";
		optionB += "		}";
		ideologyInfluenceForeignPolicy.options.push_back(optionB);
		HoI4Localisation::copyEventLocalisations(ideology + "_influence_foreign_policy.b", optionBName);

		electionEvents.push_back(ideologyInfluenceForeignPolicy);
		electionEventNumber++;
	}
}


void HoI4Events::addDemocraticPartiesInMinorityEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions)
{
	HoI4Event democraticPartiesInMinority;

	democraticPartiesInMinority.type = "country_event";
	democraticPartiesInMinority.id = "election." + to_string(electionEventNumber);
	democraticPartiesInMinority.title = "election." + to_string(electionEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("democratic_parties_in_minority.t", democraticPartiesInMinority.title);
	democraticPartiesInMinority.description = "election." + to_string(electionEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("democratic_parties_in_minority.d", democraticPartiesInMinority.description);
	democraticPartiesInMinority.picture = "GFX_report_event_journalists_speech";
	democraticPartiesInMinority.triggeredOnly = true;
	democraticPartiesInMinority.trigger = "has_government = democratic";

	char optionLetter = 'a';
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		democraticPartiesInMinority.trigger += "\n		" + ideology + " < 0.5";

		string optionName = "election." + to_string(electionEventNumber) + "." + optionLetter;
		string option = "name = " + optionName + "\n";
		option += "		ai_chance = {\n";
		option += "			base = 0\n";
		option += "			modifier = {\n";
		option += "				add = 10\n";
		option += "				can_lose_democracy_support = yes\n";
		option += "			}\n";
		option += "		}\n";
		option += "		add_popularity = {\n";
		option += "			ideology = " + ideology + "\n";
		option += "			popularity = 0.1\n";
		option += "		}\n";
		option += "		set_country_flag = coalition_with_" + ideology;
		democraticPartiesInMinority.options.push_back(option);
		HoI4Localisation::copyEventLocalisations("democratic_parties_in_minority." + ideology, optionName);
		optionLetter++;
	}

	string optionName = "election." + to_string(electionEventNumber) + "." + optionLetter;
	string option = "name = " + optionName + "\n";
	option += "		ai_chance = {\n";
	option += "			base = 0\n";
	option += "			modifier = {\n";
	option += "				add = 1\n";
	option += "				can_lose_democracy_support = no\n";
	option += "			}\n";
	option += "		}\n";
	option += "		add_popularity = {\n";
	option += "			ideology = democratic\n";
	option += "			popularity = 0.05\n";
	option += "		}\n";
	option += "		add_political_power = -80";
	democraticPartiesInMinority.options.push_back(option);
	HoI4Localisation::copyEventLocalisations("democratic_parties_in_minority.democratic", optionName);

	electionEvents.push_back(democraticPartiesInMinority);
	onActions->addElectionEvent(democraticPartiesInMinority.id);
	electionEventNumber++;
}


void HoI4Events::addIdeologicalMajorityEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions)
{
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		HoI4Event ideologicalMajority;

		ideologicalMajority.type = "country_event";
		ideologicalMajority.id = "election." + to_string(electionEventNumber);
		ideologicalMajority.title = "election." + to_string(electionEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_majority.t", ideologicalMajority.title);
		ideologicalMajority.description = "election." + to_string(electionEventNumber) + ".d";
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_majority.d", ideologicalMajority.description);
		ideologicalMajority.picture = "GFX_report_event_gathering_protest";
		ideologicalMajority.triggeredOnly = true;
		ideologicalMajority.trigger = "NOT = { has_government = " + ideology + " }\n";
		ideologicalMajority.trigger += "		" + ideology + " > 0.5\n";
		ideologicalMajority.trigger += "		is_puppet = no";

		string optionAName = "election." + to_string(electionEventNumber) + ".a";
		string optionA = "name = " + optionAName + "\n";
		optionA += "		ai_chance = {\n";
		optionA += "			base = 75\n";
		optionA += "		}\n";
		optionA += "		set_politics = {\n";
		optionA += "			ruling_party = " + ideology + "\n";
		optionA += "			elections_allowed = no\n";
		optionA += "		}";
		ideologicalMajority.options.push_back(optionA);
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_majority.a", optionAName);

		string optionBName = "election." + to_string(electionEventNumber) + ".b";
		string optionB = "name = " + optionBName + "\n";
		optionB += "		ai_chance = {\n";
		optionB += "			base = 35\n";
		optionB += "		}\n";
		optionB += "		start_civil_war = {\n";
		optionB += "			ideology = " + ideology + "\n";
		optionB += "			size = 0.5\n";
		optionB += "		}";
		ideologicalMajority.options.push_back(optionB);
		HoI4Localisation::copyEventLocalisations(ideology + "_ideological_majority.b", optionBName);

		electionEvents.push_back(ideologicalMajority);
		onActions->addElectionEvent(ideologicalMajority.id);
		electionEventNumber++;
	}
}


void HoI4Events::addWartimeExceptionEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions)
{
	HoI4Event wartimeException;

	wartimeException.type = "country_event";
	wartimeException.id = "election." + to_string(electionEventNumber);
	wartimeException.title = "election." + to_string(electionEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("wartime_exception.t", wartimeException.title);
	wartimeException.description = "election." + to_string(electionEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("wartime_exception.d", wartimeException.description);
	wartimeException.picture = "GFX_report_event_tank_factory";
	wartimeException.triggeredOnly = true;
	wartimeException.trigger = "has_government = democratic\n";
	wartimeException.trigger += "		has_war = yes";

	string optionAName = "election." + to_string(electionEventNumber) + ".a";
	string optionA = "name = " + optionAName + "\n";
	optionA += "		ai_chance = {\n";
	optionA += "			base = 0\n";
	optionA += "			modifier = {\n";
	optionA += "				add = 10\n";
	optionA += "				can_lose_unity = yes\n";
	optionA += "			}\n";
	optionA += "		}\n";
	optionA += "		add_national_unity = 0.05";
	wartimeException.options.push_back(optionA);
	HoI4Localisation::copyEventLocalisations("wartime_exception.a", optionAName);

	string optionBName = "election." + to_string(electionEventNumber) + ".b";
	string optionB = "name = " + optionBName + "\n";
	optionB += "		ai_chance = {\n";
	optionB += "			base = 0\n";
	optionB += "			modifier = {\n";
	optionB += "				add = 3\n";
	optionB += "				can_lose_democracy_support = yes\n";
	optionB += "			}\n";
	optionB += "			modifier = {\n";
	optionB += "				add = 1\n";
	optionB += "				can_lose_unity = no\n";
	optionB += "			}\n";
	optionB += "		}\n";
	optionB += "		add_political_power = 20\n";
	optionB += "		add_national_unity = -0.05";
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		optionB += "\n";
		optionB += "		add_popularity = {\n";
		optionB += "			ideology = " + ideology + "\n";
		optionB += "			popularity = 0.05\n";
		optionB += "		}";
	}
	wartimeException.options.push_back(optionB);
	HoI4Localisation::copyEventLocalisations("wartime_exception.b", optionBName);

	electionEvents.push_back(wartimeException);
	onActions->addElectionEvent(wartimeException.id);
	electionEventNumber++;
}


void HoI4Events::addGovernmentContestedEvent(const set<string>& majorIdeologies, HoI4OnActions* onActions)
{
	HoI4Event governmentContested;

	governmentContested.type = "country_event";
	governmentContested.id = "election." + to_string(electionEventNumber);
	governmentContested.title = "election." + to_string(electionEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("government_contested.t", governmentContested.title);
	governmentContested.description = "election." + to_string(electionEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("government_contested.d", governmentContested.description);
	governmentContested.picture = "GFX_report_event_gathering_protest";
	governmentContested.triggeredOnly = true;
	governmentContested.trigger = "has_government = democratic\n";
	governmentContested.trigger += "		has_war = yes\n";
	governmentContested.trigger += "		any_war_score < 20";

	string optionAName = "election." + to_string(electionEventNumber) + ".a";
	string optionA = "name = " + optionAName + "\n";
	optionA += "		ai_chance = {\n";
	optionA += "			base = 0\n";
	optionA += "			modifier = {\n";
	optionA += "				add = 3\n";
	optionA += "				can_lose_unity = no\n";
	optionA += "			}\n";
	optionA += "		}\n";
	optionA += "		add_political_power = -50\n";
	optionA += "		add_national_unity = -0.05";
	governmentContested.options.push_back(optionA);
	HoI4Localisation::copyEventLocalisations("government_contested.a", optionAName);

	string optionBName = "election." + to_string(electionEventNumber) + ".b";
	string optionB = "name = " + optionBName + "\n";
	optionB += "		ai_chance = {\n";
	optionB += "			base = 0\n";
	optionB += "			modifier = {\n";
	optionB += "				add = 10\n";
	optionB += "				can_lose_democracy_support = yes\n";
	optionB += "			}\n";
	optionB += "			modifier = {\n";
	optionB += "				add = 1\n";
	optionB += "				can_lose_unity = no\n";
	optionB += "			}\n";
	optionB += "		}\n";
	optionB += "		add_political_power = 25\n";
	optionB += "		add_national_unity = 0.05";
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "democratic") || (ideology == "neutrality"))
		{
			continue;
		}

		optionB += "\n";
		optionB += "		add_popularity = {\n";
		optionB += "			ideology = " + ideology + "\n";
		optionB += "			popularity = 0.07\n";
		optionB += "		}";
	}
	governmentContested.options.push_back(optionB);
	HoI4Localisation::copyEventLocalisations("government_contested.b", optionBName);

	electionEvents.push_back(governmentContested);
	onActions->addElectionEvent(governmentContested.id);
	electionEventNumber++;
}


void HoI4Events::addPartyChoiceEvent(const string& countryTag, set<const V2Party*, function<bool (const V2Party*, const V2Party*)>> parties, HoI4OnActions* onActions, const set<string>& majorIdeologies)
{
	HoI4Event partyChoiceEvent;

	partyChoiceEvent.type = "country_event";
	partyChoiceEvent.id = "election." + to_string(electionEventNumber);
	partyChoiceEvent.title = "election." + to_string(electionEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("party_choice.t", partyChoiceEvent.title);
	partyChoiceEvent.description = "election." + to_string(electionEventNumber) + ".d";
	HoI4Localisation::copyEventLocalisations("party_choice.d", partyChoiceEvent.description);
	partyChoiceEvent.picture = "GFX_report_event_usa_election_generic";
	partyChoiceEvent.triggeredOnly = true;
	partyChoiceEvent.trigger = "tag = " + countryTag + "\n";
	if (majorIdeologies.count("democratic") > 0)
	{
		partyChoiceEvent.trigger += "		OR = {";
		partyChoiceEvent.trigger += "			democratic > 0.5";
		partyChoiceEvent.trigger += "			neutrality > 0.5";
		partyChoiceEvent.trigger += "		}";
	}
	else
	{
		partyChoiceEvent.trigger += "		neutrality > 0.5";
	}

	char optionLetter = 'a';
	for (auto party: parties)
	{
		if ((party->ideology == "conservative") || (party->ideology == "liberal") || (party->ideology == "socialist"))
		{
			string trimmedName = party->name.substr(4, party->name.size());

			string optionName = "election." + to_string(electionEventNumber) + optionLetter;
			string option = "name = " + optionName + "\n";
			if (majorIdeologies.count("democratic") > 0)
			{
				option += "		set_party_name = { ideology = democratic long_name = " + countryTag + "_" + trimmedName + "_party " + "name = " + countryTag + "_" + trimmedName + "_party }\n";
			}
			else
			{
				option += "		set_party_name = { ideology = neutrality long_name = " + countryTag + "_" + trimmedName + "_party " + "name = " + countryTag + "_" + trimmedName + "_party }\n";
			}
			option += "		retire_country_leader = yes";
			partyChoiceEvent.options.push_back(option);
			HoI4Localisation::addEventLocalisationFromVic2(party->name, optionName);
			optionLetter++;
		}
	}

	onActions->addElectionEvent(partyChoiceEvent.id);
	electionEvents.push_back(partyChoiceEvent);
	electionEventNumber++;
}