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



#include "Events.h"
#include "HoI4Country.h"
#include "HoI4Faction.h"
#include "HoI4Localisation.h"
#include "OnActions.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "../V2World/Party.h"
#include <fstream>



void HoI4::Events::output() const
{
	std::string eventpath = "output/" + Configuration::getOutputName() + "/events";
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
	outputStabilityEvents();
}


void HoI4::Events::outputNationalFocusEvents() const
{
	std::ofstream outEvents("output/" + Configuration::getOutputName() + "/events/NF_events.txt");
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


void HoI4::Events::outputNewsEvents() const
{
	std::ofstream outNewsEvents("output/" + Configuration::getOutputName() + "/events/newsEvents.txt");
	if (!outNewsEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not create newsEvents.txt";
		exit(-1);
	}

	outNewsEvents << "\xEF\xBB\xBF";
	outNewsEvents << "add_namespace = news\n";
	for (auto& theEvent: newsEvents)
	{
		outNewsEvents << "\n";
		outNewsEvents << theEvent;
	}

	outNewsEvents.close();
}


void HoI4::Events::outputPoliticalEvents() const
{
	std::ofstream outPoliticalEvents("output/" + Configuration::getOutputName() + "/events/converterPoliticalEvents.txt");
	if (!outPoliticalEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not create converterPoliticalEvents.txt";
		exit(-1);
	}

	outPoliticalEvents << "\xEF\xBB\xBF";
	outPoliticalEvents << "add_namespace = conv.political\n";
	for (auto& theEvent: politicalEvents)
	{
		outPoliticalEvents << "\n";
		outPoliticalEvents << theEvent;
	}

	outPoliticalEvents.close();
}


void HoI4::Events::outputWarJustificationEvents() const
{
	std::ofstream outWarJustificationEvents("output/" + Configuration::getOutputName() + "/events/WarJustification.txt", std::ios_base::app);
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


void HoI4::Events::outputElectionEvents() const
{
	std::ofstream outElectionEvents("output/" + Configuration::getOutputName() + "/events/ElectionEvents.txt");
	if (!outElectionEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not open ElectionEvents.txt";
		exit(-1);
	}

	outElectionEvents << "\xEF\xBB\xBF";
	outElectionEvents << "add_namespace = election\n\n";

	for (auto& theEvent: electionEvents)
	{
		outElectionEvents << "\n";
		outElectionEvents << theEvent;
	}

	outElectionEvents.close();
}


void HoI4::Events::outputStabilityEvents() const
{
	std::ofstream outStabilityEvents("output/" + Configuration::getOutputName() + "/events/stability_events.txt");
	if (!outStabilityEvents.is_open())
	{
		LOG(LogLevel::Error) << "Could not open StabilityEvents.txt";
		exit(-1);
	}

	outStabilityEvents << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy

	outStabilityEvents << "###########################\n";
	outStabilityEvents << "#stability events\n";
	outStabilityEvents << "###########################\n";
	outStabilityEvents << "\n";
	outStabilityEvents << "add_namespace = stability\n";
	outStabilityEvents << "\n";

	for (auto& theEvent: stabilityEvents)
	{
		outStabilityEvents << "\n";
		outStabilityEvents << theEvent.second;
	}

	outStabilityEvents << "add_namespace = strikes_event\n";
	for (auto& theEvent: strikesEvents)
	{
		outStabilityEvents << "\n";
		outStabilityEvents << theEvent.second;
	}

	outStabilityEvents << "add_namespace = mutiny_event\n";
	for (auto& theEvent: mutinyEvents)
	{
		outStabilityEvents << "\n";
		outStabilityEvents << theEvent.second;
	}

	outStabilityEvents.close();
}


void HoI4::Events::createFactionEvents(std::shared_ptr<HoI4Country> Leader, std::shared_ptr<HoI4Country> newAlly)
{
	auto possibleLeaderName = Leader->getSourceCountry()->getName("english");
	std::string leaderName;
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
	std::string newAllyName;
	if (possibleNewAllyName)
	{
		newAllyName = *possibleNewAllyName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine new ally name for faction events";
		newAllyName = *possibleNewAllyName;
	}

	Event nfEvent;
	nfEvent.type = "country_event";
	nfEvent.id = "NFEvents." + std::to_string(nationalFocusEventNumber++);
	nfEvent.title = "\"Alliance?\"";
	nfEvent.descriptions.push_back("desc = \"Alliance with " + leaderName + "?\"");
	nfEvent.picture = "news_event_generic_sign_treaty1";
	nfEvent.majorEvent = false;
	nfEvent.triggeredOnly = true;
	std::string yesOption = "= {\n";
	yesOption += "		name = \"Yes\"\n";
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
	yesOption += "	}";
	nfEvent.options.push_back(yesOption);
	std::string noOption = "= {\n";
	noOption += "		name = \"No\"\n";
	noOption += "		ai_chance = { factor = 0 }\n";
	noOption += "		hidden_effect = {\n";
	noOption += "			news_event = { id = news." + to_string(newsEventNumber + 1) + " }\n";
	noOption += "		}\n";
	noOption += "	}";
	nfEvent.options.push_back(noOption);
	nationalFocusEvents.push_back(nfEvent);

	Event newsEventYes;
	newsEventYes.type = "news_event";
	newsEventYes.id = "news." + to_string(newsEventNumber);
	newsEventYes.title = "\"" + newAllyName + " Now an Ally with " + leaderName + "!\"";
	newsEventYes.descriptions.push_back("desc = \"They are now allies\"");
	newsEventYes.picture = "news_event_generic_sign_treaty1";
	newsEventYes.majorEvent = true;
	newsEventYes.triggeredOnly = true;
	std::string interestingOption = "= {\n";
	interestingOption += "		name = \"Interesting\"\n";
	interestingOption += "	}";
	newsEventYes.options.push_back(interestingOption);
	newsEvents.push_back(newsEventYes);

	Event newsEventNo;
	newsEventNo.type = "news_event";
	newsEventNo.id = "news." + to_string(newsEventNumber + 1);
	newsEventNo.title = "\"" + newAllyName + " Refused the Alliance offer of " + leaderName + "!\"";
	newsEventNo.descriptions.push_back("desc = \"They are not allies\"");
	newsEventNo.picture = "news_event_generic_sign_treaty1";
	newsEventNo.majorEvent = true;
	newsEventNo.triggeredOnly = true;
	interestingOption = "= {\n";
	interestingOption += "		name = \"Interesting\"\n";
	interestingOption += "	}";
	newsEventNo.options.push_back(interestingOption);
	newsEvents.push_back(newsEventNo);

	newsEventNumber += 2;
}


void HoI4::Events::createAnnexEvent(std::shared_ptr<HoI4Country> Annexer, std::shared_ptr<HoI4Country> Annexed)
{
	auto possibleAnnexerName = Annexer->getSourceCountry()->getName("english");
	std::string annexerName;
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
	std::string annexedName;
	if (possibleAnnexedName)
	{
		annexedName = *possibleAnnexedName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine annexed country name for annexation events";
		annexedName = "";
	}

	Event annexEvent;
	annexEvent.type = "country_event";
	annexEvent.id = "NFEvents." + to_string(nationalFocusEventNumber);
	annexEvent.title = "\"" + annexerName + " Demands " + annexedName + "!\"";
	std::string description = "desc = \"Today " + annexerName + " sent an envoy to us with a proposition of an union. We are alone and in this world, and a union with " + annexerName + " might prove to be fruiteful.";
	description += " Our people would be safe with the mighty army of " + annexerName + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"";
	annexEvent.descriptions.push_back(description);
	annexEvent.picture = "GFX_report_event_hitler_parade";
	annexEvent.majorEvent = false;
	annexEvent.triggeredOnly = true;

	std::string acceptOption = "= {\n";
	acceptOption += "		name = \"We accept the Union\"\n";
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
	acceptOption += "	}";
	annexEvent.options.push_back(acceptOption);

	std::string refuseOption = "= {\n";
	refuseOption += "		name = \"We Refuse!\"\n";
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
	refuseOption += "			country_event = { hours = 2 id = NFEvents." + to_string(nationalFocusEventNumber + 2) + " }\n";//+2 refuse
	refuseOption += "			if = {\n";
	refuseOption += "				limit = { is_in_faction_with = " + Annexed->getTag() + " }\n";
	refuseOption += "				remove_from_faction = " + Annexed->getTag() + "\n";
	refuseOption += "			}\n";
	refuseOption += "		}\n";
	refuseOption += "	}";
	annexEvent.options.push_back(refuseOption);

	nationalFocusEvents.push_back(annexEvent);


	Event refusedEvent;
	refusedEvent.type = "country_event";
	refusedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 2);
	refusedEvent.title = "\"" + annexedName + " Refuses!\"";
	refusedEvent.descriptions.push_back("desc = \"" + annexedName + " Refused our proposed union! This is an insult to us that cannot go unanswered!\"");
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.majorEvent = false;
	refusedEvent.triggeredOnly = true;

	std::string refusedOption = "= {\n";
	refusedOption += "		name = \"It's time for war\"\n";
	refusedOption += "		create_wargoal = {\n";
	refusedOption += "			type = annex_everything\n";
	refusedOption += "			target = " + Annexed->getTag() + "\n";
	refusedOption += "		}\n";
	refusedOption += "	}";
	refusedEvent.options.push_back(refusedOption);

	nationalFocusEvents.push_back(refusedEvent);


	Event acceptedEvent;
	acceptedEvent.type = "country_event";
	acceptedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 1);
	acceptedEvent.title = "\"" + annexedName + " accepts!\"";
	acceptedEvent.descriptions.push_back("desc = \"" + annexedName + " accepted our proposed union, their added strength will push us to greatness!\"");
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.majorEvent = false;
	acceptedEvent.triggeredOnly = true;

	std::string acceptedOption = "= {\n";
	acceptedOption += "		name = \"A stronger Union!\"\n";
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
	acceptedOption += "	}";
	acceptedEvent.options.push_back(acceptedOption);

	nationalFocusEvents.push_back(acceptedEvent);


	nationalFocusEventNumber += 3;
}


void HoI4::Events::createSudetenEvent(std::shared_ptr<HoI4Country> Annexer, std::shared_ptr<HoI4Country> Annexed, const std::vector<int>& claimedStates)
{
	//flesh out this event more, possibly make it so allies have a chance to help?
	auto possibleAnnexerName = Annexer->getSourceCountry()->getName("english");
	std::string annexerName;
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
	std::string annexerAdjctive;
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
	std::string annexedName;
	if (possibleAnnexedName)
	{
		annexedName = *possibleAnnexedName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine annexed country name for sudeten events";
		annexedName = "";
	}

	Event sudetenEvent;
	sudetenEvent.type = "country_event";
	sudetenEvent.id = "NFEvents." + to_string(nationalFocusEventNumber);
	sudetenEvent.title = annexerName + " Demands " + annexedName + "!";
	std::string description = "desc = \"" + annexerName + " has recently been making claims to our bordering states, saying that these states are full of " + annexerAdjctive + " people and that the territory should be given to them. Although it ";
	description += "is true that recently our neighboring states have had an influx of " + annexerAdjctive + " people in the recent years, we cannot give up our lands because a few " + annexerAdjctive + " settled down in our land. ";
	description += "In response " + annexerName + " has called for a conference, demanding their territory in exchange for peace. How do we resond? ";
	description += " Our people would be safe with the mighty army of " + annexerName + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"";
	sudetenEvent.descriptions.push_back(description);
	sudetenEvent.picture = "GFX_report_event_hitler_parade";
	sudetenEvent.majorEvent = false;
	sudetenEvent.triggeredOnly = true;

	std::string acceptOption = "= {\n";
	acceptOption += "		name = \"We Accept\"\n";
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
	acceptOption += "	}";
	sudetenEvent.options.push_back(acceptOption);

	std::string refuseOption = "= {\n";
	refuseOption += "		name = \"We Refuse!\"\n";
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
	refuseOption += "	}";
	sudetenEvent.options.push_back(refuseOption);

	nationalFocusEvents.push_back(sudetenEvent);


	Event refusedEvent;
	refusedEvent.type = "country_event";
	refusedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 2);
	refusedEvent.title = "\"" + annexedName + " Refuses!\"";
	refusedEvent.descriptions.push_back("desc = \"" + annexedName + " Refused our proposed proposition! This is an insult to us that cannot go unanswered!\"");
	refusedEvent.picture = "GFX_report_event_german_troops";
	refusedEvent.majorEvent = false;
	refusedEvent.triggeredOnly = true;

	std::string refusedOption = "= {\n";
	refusedOption += "		name = \"It's time for war\"\n";
	refusedOption += "		create_wargoal = {\n";
	refusedOption += "				type = annex_everything\n";
	refusedOption += "			target = " + Annexed->getTag() + "\n";
	refusedOption += "		}\n";
	refusedOption += "	}";
	refusedEvent.options.push_back(refusedOption);

	nationalFocusEvents.push_back(refusedEvent);


	Event acceptedEvent;
	acceptedEvent.type = "country_event";
	acceptedEvent.id = "NFEvents." + to_string(nationalFocusEventNumber + 1);
	acceptedEvent.title = "\"" + annexedName + " accepts!\"";
	acceptedEvent.descriptions.push_back("desc = \"" + annexedName + " accepted our proposed demands, the added lands will push us to greatness!\"");
	acceptedEvent.picture = "GFX_report_event_german_speech";
	acceptedEvent.majorEvent = false;
	acceptedEvent.triggeredOnly = true;

	std::string acceptedOption = "= {\n";
	acceptedOption += "		name = \"A stronger Union!\"\n";
	for (unsigned int i = 0; i <= 1 && i < claimedStates.size(); i++)
	{
		acceptedOption += "		" + to_string(claimedStates[i]) + " = { add_core_of = " + Annexer->getTag() + " }\n";
		acceptedOption += "		" + Annexer->getTag() + " = { transfer_state =  " + to_string(claimedStates[i]) + " }\n";
	}
	acceptedOption += "		set_country_flag = " + Annexed->getTag() + "_demanded\n";
	acceptedOption += "	}";
	acceptedEvent.options.push_back(acceptedOption);

	nationalFocusEvents.push_back(acceptedEvent);


	nationalFocusEventNumber += 3;
}


void HoI4::Events::createTradeEvent(std::shared_ptr<HoI4Country> leader, std::shared_ptr<HoI4Country> GC)
{
	auto possibleAggressorName = GC->getSourceCountry()->getName("english");
	std::string aggressorName;
	if (possibleAggressorName)
	{
		aggressorName = *possibleAggressorName;
	}
	else
	{
		Log(LogLevel::Warning) << "Could not determine aggressor name for trade events";
		aggressorName = "";
	}

	Event tradeIncidentEvent;
	tradeIncidentEvent.type = "country_event";
	tradeIncidentEvent.id = "NFEvents." + std::to_string(nationalFocusEventNumber++);
	tradeIncidentEvent.title = "\"Trade Incident\"";
	tradeIncidentEvent.descriptions.push_back("desc = \"One of our convoys was sunk by " + aggressorName + "\"");
	tradeIncidentEvent.picture = "GFX_report_event_chinese_soldiers_fighting";
	tradeIncidentEvent.majorEvent = false;
	tradeIncidentEvent.triggeredOnly = true;
	tradeIncidentEvent.trigger = "= {\n";
	tradeIncidentEvent.trigger += "		has_country_flag = established_traders\n";
	tradeIncidentEvent.trigger += "		NOT = { has_country_flag = established_traders_activated }\n";
	tradeIncidentEvent.trigger += "	}";

	std::string option = "= {\n";
	option += "		name = \"They will Pay!\"\n";
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
	option += "	}";
	tradeIncidentEvent.options.push_back(option);

	nationalFocusEvents.push_back(tradeIncidentEvent);
}


void HoI4::Events::createPoliticalEvents(const std::set<std::string>& majorIdeologies)
{
	addMinisterRevolutionEvents(majorIdeologies);
	addFiftyPercentEvents(majorIdeologies);
	addRevolutionEvents(majorIdeologies);
	addSuppressedEvents(majorIdeologies);
}


void HoI4::Events::addMinisterRevolutionEvents(const std::set<std::string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if ((ideology == "neutrality") || (ideology == "democratic"))
		{
			continue;
		}

		Event addPopularity;
		addPopularity.type = "country_event";
		addPopularity.id = "conv.political." + std::to_string(politicalEventNumber);
		addPopularity.title = "conv.political." + std::to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_add_popularity.t", addPopularity.title);
		std::string description = "conv.political." + std::to_string(politicalEventNumber) + ".d";
		addPopularity.descriptions.push_back("desc = " + description);
		HoI4Localisation::copyEventLocalisations(ideology + "_add_popularity.d", description);
		addPopularity.picture = getIdeologicalPicture(ideology);
		addPopularity.majorEvent = false;
		addPopularity.triggeredOnly = false;
		addPopularity.trigger = "= {\n";
		addPopularity.trigger += "		has_idea_with_trait = " + ideology + "_minister\n";
		addPopularity.trigger += "		NOT = { has_government = " + ideology + " }\n";
		addPopularity.trigger += "		" + ideology + " > 0.2\n";
		addPopularity.trigger += "	}";
		addPopularity.meanTimeToHappen = "= {\n";
		addPopularity.meanTimeToHappen += "		days = 365\n";
		addPopularity.meanTimeToHappen += "		modifier = {\n";
		addPopularity.meanTimeToHappen += "			factor = 0.5\n";
		addPopularity.meanTimeToHappen += "			has_country_flag = " + ideology + "_popular_movement\n";
		addPopularity.meanTimeToHappen += "		}\n";
		addPopularity.meanTimeToHappen += "	}";
		std::string optionA = "= {\n";
		optionA += "		name = ";
		std::string optionName = "conv.political." + std::to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_add_popularity.a", optionName);
		optionA += optionName + "\n";
		optionA += "		add_popularity = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			popularity = 0.05\n";
		optionA += "		}\n";
		optionA += "		add_stability = -0.05\n";
		optionA += "	}";
		addPopularity.options.push_back(optionA);
		politicalEvents.push_back(addPopularity);
		politicalEventNumber++;

		Event militaryDefections;
		militaryDefections.type = "country_event";
		militaryDefections.id = "conv.political." + std::to_string(politicalEventNumber);
		militaryDefections.title = "conv.political." + std::to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_military_defections.t", militaryDefections.title);
		std::string description2 = "conv.political." + std::to_string(politicalEventNumber) + ".d";
		militaryDefections.descriptions.push_back("desc = " + description2);
		HoI4Localisation::copyEventLocalisations(ideology + "_military_defections.d", description2);
		militaryDefections.picture = getIdeologicalPicture(ideology);
		militaryDefections.majorEvent = false;
		militaryDefections.triggeredOnly = false;
		militaryDefections.trigger = "= {\n";
		militaryDefections.trigger += "		has_idea_with_trait = " + ideology + "_minister\n";
		militaryDefections.trigger += "		NOT = { has_government = " + ideology + " }\n";
		militaryDefections.trigger += "		" + ideology + " > 0.3\n";
		militaryDefections.trigger += "		NOT = { has_idea = " + ideology + "_partisans_recruiting }\n";
		militaryDefections.trigger += "	}";
		militaryDefections.meanTimeToHappen = "= {\n";
		militaryDefections.meanTimeToHappen += "		days = 365\n";
		militaryDefections.meanTimeToHappen += "		modifier = {\n";
		militaryDefections.meanTimeToHappen += "			factor = 0.5\n";
		militaryDefections.meanTimeToHappen += "			has_country_flag = " + ideology + "_popular_movement\n";
		militaryDefections.meanTimeToHappen += "		}\n";
		militaryDefections.meanTimeToHappen += "	}";
		optionA = "= {\n";
		optionA += "		name = ";
		optionName = "conv.political." + std::to_string(politicalEventNumber) + ".a";
		HoI4Localisation::copyEventLocalisations(ideology + "_military_defections.a", optionName);
		optionA += optionName + "\n";
		optionA += "		add_popularity = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			popularity = 0.05\n";
		optionA += "		}\n";
		optionA += "		add_ideas = " + ideology + "_partisans_recruiting\n";
		optionA += "	}";
		militaryDefections.options.push_back(optionA);
		politicalEvents.push_back(militaryDefections);
		politicalEventNumber++;
	}

	if (majorIdeologies.count("democratic") > 0)
	{
		addDemocraticMinisterRevolutionEvents(majorIdeologies);
	}
}


void HoI4::Events::addDemocraticMinisterRevolutionEvents(const std::set<std::string>& majorIdeologies)
{
	Event opposition;
	opposition.type = "country_event";
	opposition.id = "conv.political." + std::to_string(politicalEventNumber);
	opposition.title = "conv.political." + std::to_string(politicalEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("democratic_opposition_forming.t", opposition.title);
	std::string description = "conv.political." + std::to_string(politicalEventNumber) + ".d";
	opposition.descriptions.push_back("desc = " + description);
	HoI4Localisation::copyEventLocalisations("democratic_opposition_forming.d", description);
	opposition.picture = getIdeologicalPicture("democratic");
	opposition.majorEvent = false;
	opposition.triggeredOnly = false;
	opposition.trigger = "= {\n";
	opposition.trigger += "		has_idea_with_trait = democratic_reformer\n";
	opposition.trigger += "		NOT = { has_government = democratic }\n";
	opposition.trigger += "		NOT = { has_country_flag = democracy_opposition_formed }\n";
	opposition.trigger += "		is_puppet = no\n";
	opposition.trigger += "	}";
	opposition.meanTimeToHappen = "= {\n";
	opposition.meanTimeToHappen += "		days = 300\n";
	opposition.meanTimeToHappen += "		modifier = {\n";
	opposition.meanTimeToHappen += "			factor = 0.5\n";
	opposition.meanTimeToHappen += "			has_country_flag = democracy_popular_movement\n";
	opposition.meanTimeToHappen += "		}\n";
	opposition.meanTimeToHappen += "	}";
	std::string optionA = "= {\n";
	optionA += "		name = ";
	std::string optionName = "conv.political." + std::to_string(politicalEventNumber) + ".a";
	optionA += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_opposition_forming.a", optionName);
	optionA += "		add_popularity = {\n";
	optionA += "			ideology = democratic\n";
	optionA += "			popularity = 0.05\n";
	optionA += "		}\n";
	optionA += "		add_stability = -0.05\n";
	optionA += "		set_country_flag = democracy_opposition_formed\n";
	optionA += "	}";
	opposition.options.push_back(optionA);
	politicalEvents.push_back(opposition);
	politicalEventNumber++;

	Event callForElections;
	callForElections.type = "country_event";
	callForElections.id = "conv.political." + std::to_string(politicalEventNumber);
	callForElections.title = "conv.political." + to_string(politicalEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.t", callForElections.title);
	std::string description2 = "conv.political." + to_string(politicalEventNumber) + ".d";
	callForElections.descriptions.push_back("desc = " + description2);
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.d", description2);
	callForElections.picture = getIdeologicalPicture("democratic");
	callForElections.majorEvent = false;
	callForElections.triggeredOnly = false;
	callForElections.trigger = "= {\n";
	callForElections.trigger += "		has_idea_with_trait = democratic_reformer\n";
	callForElections.trigger += "		NOT = { has_government = democratic }\n";
	callForElections.trigger += "		has_country_flag = democracy_opposition_formed\n";
	callForElections.trigger += "		NOT = { has_idea = democratic_opposition_voicing_protests }\n";
	callForElections.trigger += "	}";
	callForElections.meanTimeToHappen = "= {\n";
	callForElections.meanTimeToHappen += "		days = 150\n";
	callForElections.meanTimeToHappen += "	}";
	optionA = "= {\n";
	optionA += "		name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
	optionA += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.a", optionName);
	optionA += "	}";
	callForElections.options.push_back(optionA);
	std::string optionB = "= {\n";
	optionB += "		name = ";
	optionName = "conv.political." + to_string(politicalEventNumber) + ".b";
	optionB += optionName + "\n";
	HoI4Localisation::copyEventLocalisations("democratic_call_for_elections.b", optionName);
	optionB += "		add_political_power = -20\n";
	optionB += "		add_ideas = democratic_opposition_voicing_protests\n";
	optionB += "	}";
	callForElections.options.push_back(optionB);
	politicalEvents.push_back(callForElections);
	politicalEventNumber++;
}


void HoI4::Events::addFiftyPercentEvents(const std::set<std::string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if (ideology == "neutrality")
		{
			continue;
		}

		Event fiftyPercentEvent;
		fiftyPercentEvent.type = "country_event";
		fiftyPercentEvent.id = "conv.political." + std::to_string(politicalEventNumber);
		fiftyPercentEvent.title = "conv.political." + std::to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_fifty_percent.t", fiftyPercentEvent.title);
		std::string description = "conv.political." + std::to_string(politicalEventNumber) + ".d";
		fiftyPercentEvent.descriptions.push_back("desc = " + description);
		HoI4Localisation::copyEventLocalisations(ideology + "_fifty_percent.d", description);
		fiftyPercentEvent.picture = getIdeologicalPicture(ideology);
		fiftyPercentEvent.majorEvent = false;
		fiftyPercentEvent.triggeredOnly = true;
		std::string optionC = "= {\n";
		optionC += "		name = ";
		std::string optionName = "conv.political." + std::to_string(politicalEventNumber) + ".c";
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
		optionC += "		add_stability = -0.05\n";
		optionC += "	}";
		fiftyPercentEvent.options.push_back(optionC);
		politicalEvents.push_back(fiftyPercentEvent);
		politicalEventNumber++;
	}
}


void HoI4::Events::addRevolutionEvents(const std::set<std::string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if (ideology == "neutrality")
		{
			continue;
		}

		Event revolutionEvent;
		revolutionEvent.type = "country_event";
		revolutionEvent.id = "conv.political." + std::to_string(politicalEventNumber);
		revolutionEvent.title = "conv.political." + std::to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_revolution_event.t", revolutionEvent.title);
		std::string description = "conv.political." + std::to_string(politicalEventNumber) + ".d";
		revolutionEvent.descriptions.push_back("desc = " + description);
		HoI4Localisation::copyEventLocalisations(ideology + "_revolution_event.d", description);
		revolutionEvent.picture = getIdeologicalPicture(ideology);
		revolutionEvent.majorEvent = false;
		revolutionEvent.triggeredOnly = false;
		revolutionEvent.trigger = "= {\n";
		revolutionEvent.trigger += "		" + ideology + " > 0.7\n";
		revolutionEvent.trigger += "		NOT = { has_government = " + ideology + " }\n";
		revolutionEvent.trigger += "		has_civil_war = no\n";
		revolutionEvent.trigger += "	}";

		revolutionEvent.meanTimeToHappen = "= {\n";
		revolutionEvent.meanTimeToHappen += "		days = 2\n";
		revolutionEvent.meanTimeToHappen += "	}";
		revolutionEvent.immediate = "= {\n";
		revolutionEvent.immediate += "		hidden_effect = { remove_ideas = " + ideology + "_revolutionaries }\n";
		revolutionEvent.immediate += "	}";
		std::string optionA = "= {\n";
		optionA += "		name = ";
		string optionName = "conv.political." + to_string(politicalEventNumber) + ".a";
		optionA += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_revolution_event.a", optionName);
		optionA += "		ai_chance = {\n";
		optionA += "			factor = 0\n";
		optionA += "		}\n";
		optionA += "		start_civil_war = {\n";
		optionA += "			ideology = " + ideology + "\n";
		optionA += "			size = 0.5\n";
		optionA += "		}\n";
		optionA += "	}";
		revolutionEvent.options.push_back(optionA);
		std::string optionB = "= {\n";
		optionB += "		name = ";
		optionName = "conv.political." + std::to_string(politicalEventNumber) + ".b";
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
		optionB += "	}";
		revolutionEvent.options.push_back(optionB);
		politicalEvents.push_back(revolutionEvent);
		politicalEventNumber++;
	}
}


void HoI4::Events::addSuppressedEvents(const std::set<std::string>& majorIdeologies)
{
	for (auto ideology: majorIdeologies)
	{
		if (ideology == "neutrality")
		{
			continue;
		}
		Event suppressedEvent;
		suppressedEvent.type = "country_event";
		suppressedEvent.id = "conv.political." + std::to_string(politicalEventNumber);
		suppressedEvent.title = "conv.political." + std::to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations(ideology + "_suppressed_event.t", suppressedEvent.title);
		std::string description = "conv.political." + std::to_string(politicalEventNumber) + ".d";
		suppressedEvent.descriptions.push_back("desc = " + description);
		HoI4Localisation::copyEventLocalisations(ideology + "_suppressed_event.d", description);
		suppressedEvent.picture = getIdeologicalPicture(ideology);
		suppressedEvent.majorEvent = false;
		suppressedEvent.triggeredOnly = false;
		suppressedEvent.trigger = "= {\n";
		suppressedEvent.trigger += "		" + ideology + " < 0.3\n";
		suppressedEvent.trigger += "		has_idea = " + ideology + "_revolutionaries\n";
		suppressedEvent.trigger += "	}";
		suppressedEvent.meanTimeToHappen = "= {\n";
		suppressedEvent.meanTimeToHappen += "		days = 2\n";
		suppressedEvent.meanTimeToHappen += "	}";
		std::string option = "= {\n";
		option += "		name = ";
		std::string optionName = "conv.political." + std::to_string(politicalEventNumber) + ".a";
		option += optionName + "\n";
		HoI4Localisation::copyEventLocalisations(ideology + "_suppressed_event.a", optionName);
		option += "		remove_ideas = " + ideology + "_revolutionaries\n";
		option += "	}";
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
		Event removeNeutral;
		removeNeutral.type = "country_event";
		removeNeutral.id = "conv.political." + std::to_string(politicalEventNumber);
		removeNeutral.title = "conv.political." + std::to_string(politicalEventNumber) + ".t";
		HoI4Localisation::copyEventLocalisations("abandon_neutral.t", removeNeutral.title);
		for (auto ideology: majorIdeologies)
		{
			if ((ideology == "democratic") || (ideology == "neutrality"))
			{
				continue;
			}
			std::string description = "desc = {\n";
			description += "		text = political." + std::to_string(politicalEventNumber) + ".d_" + ideology + "\n";
			description += "		trigger = { has_government = " + ideology + " }\n";
			description += "	}";
			removeNeutral.descriptions.push_back(description);
			HoI4Localisation::copyEventLocalisations("abandon_neutral.d_" + ideology, "conv.political." + std::to_string(politicalEventNumber) + "_" + ideology);
		}
		/*if (removeNeutral.description.size() > 0)
		{
			removeNeutral.description = removeNeutral.description.substr(8, removeNeutral.description.size() - 9);
		}*/
		removeNeutral.picture = "GFX_report_event_journalists_speech";
		removeNeutral.majorEvent = false;
		removeNeutral.triggeredOnly = false;
		removeNeutral.trigger = "= {\n";
		removeNeutral.trigger += "		OR = {\n";
		for (auto ideology: majorIdeologies)
		{
			if ((ideology == "democratic") || (ideology == "neutrality"))
			{
				continue;
			}
			removeNeutral.trigger += "			has_government = " + ideology + "\n";
		}
		removeNeutral.trigger += "		}\n";
		removeNeutral.trigger += "		has_idea = neutrality_idea\n";
		removeNeutral.trigger += "	}";
		removeNeutral.meanTimeToHappen = "= {\n";
		removeNeutral.meanTimeToHappen += "		days = 2\n";
		removeNeutral.meanTimeToHappen += "	}";
		std::string option = "= {\n";
		option += "		name = ";
		std::string optionName = "conv.political." + std::to_string(politicalEventNumber) + ".a";
		option += optionName + "\n";
		HoI4Localisation::copyEventLocalisations("abandon_neutral.a", optionName);
		option += "		remove_ideas = neutrality_idea\n";
		option += "	}";
		removeNeutral.options.push_back(option);
		politicalEvents.push_back(removeNeutral);
	}
}


std::string HoI4::Events::getIdeologicalPicture(const std::string& ideology) const
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


void HoI4::Events::createWarJustificationEvents(const std::set<std::string>& majorIdeologies)
{
	int i = 1;
	for (auto majorIdeology: majorIdeologies)
	{
		for (auto majorIdeology2: majorIdeologies)
		{
			Event warJustification;
			warJustification.type = "country_event";
			warJustification.id = "war_justification." + std::to_string(i);
			warJustification.title = "war_justification." + std::to_string(i) + ".t";
			warJustification.descriptions.push_back("desc = war_justification." + std::to_string(i) + ".d");
			warJustification.picture = "GFX_report_event_iww_demonstration";
			warJustification.majorEvent = false;
			warJustification.triggeredOnly = true;
			warJustification.trigger = "= {\n";
			warJustification.trigger += "		has_government = " + majorIdeology + "\n";
			warJustification.trigger += "		FROM = { has_government = " + majorIdeology2 + " }\n";
			warJustification.trigger += "		NOT = { has_country_flag = war_justification_last_event_political }\n";
			warJustification.trigger += "	}";
			std::string option = "= {\n";
			option += "		name = war_justification." + std::to_string(i) + ".a\n";
			option += "		FROM = { country_event = { id = war_justification." + std::to_string(i + 200) + " } }\n";
			option += "		set_country_flag = war_justification_last_event_political\n";
			option += "		clr_country_flag = war_justification_last_event_generic\n";
			option += "		clr_country_flag = war_justification_last_event_neighbor\n";
			option += "		clr_country_flag = war_justification_last_event_mobilized\n";
			option += "		clr_country_flag = war_justification_last_event_borderthreat\n";
			option += "	}";
			warJustification.options.push_back(option);
			warJustificationEvents.push_back(warJustification);
			HoI4Localisation::copyEventLocalisations("war_justification." + majorIdeology + majorIdeology2 + ".t", "war_justification." + std::to_string(i) + ".t");
			HoI4Localisation::copyEventLocalisations("war_justification." + majorIdeology + majorIdeology2 + ".d", "war_justification." + std::to_string(i) + ".d");
			HoI4Localisation::copyEventLocalisations("war_justification." + majorIdeology + majorIdeology2 + ".a", "war_justification." + std::to_string(i) + ".a");

			Event warJustification2;
			warJustification2.type = "country_event";
			warJustification2.id = "war_justification." + std::to_string(200 + i);
			warJustification2.title = "war_justification." + std::to_string(200 + i) + ".t";
			warJustification2.descriptions.push_back("desc = war_justification." + std::to_string(200 + i) + ".d");
			warJustification2.picture = "GFX_report_event_iww_demonstration";
			warJustification2.majorEvent = false;
			warJustification2.triggeredOnly = true;
			std::string option2 = "= {\n";
			option2 += "		name = war_justification." + std::to_string(200 + i) + ".a\n";
			option2 += "	}";
			warJustification2.options.push_back(option2);
			warJustificationEvents.push_back(warJustification2);
			HoI4Localisation::copyEventLocalisations("war_justification." + majorIdeology + majorIdeology2 + "200.t", "war_justification." + std::to_string(200 + i) + ".t");
			HoI4Localisation::copyEventLocalisations("war_justification." + majorIdeology + majorIdeology2 + "200.d", "war_justification." + std::to_string(200 + i) + ".d");
			HoI4Localisation::copyEventLocalisations("war_justification." + majorIdeology + majorIdeology2 + "200.a", "war_justification." + std::to_string(200 + i) + ".a");

			i++;
		}
	}

	Event wargoalExpired;
	wargoalExpired.type = "country_event";
	wargoalExpired.id = "war_justification.301";
	wargoalExpired.title = "war_justification.301.t";
	wargoalExpired.descriptions.push_back("desc = war_justification.301.d");
	wargoalExpired.picture = "GFX_report_event_iww_demonstration";
	wargoalExpired.majorEvent = false;
	wargoalExpired.triggeredOnly = true;
	wargoalExpired.trigger = "= {\n";
	wargoalExpired.trigger += "		has_war = no\n";
	wargoalExpired.trigger += "	}";

	char letter = 'a';
	for (auto ideology: majorIdeologies)
	{
		std::string option = "= {\n";
		option += "		name = war_justification.301." + std::string(1, letter) + "\n";
		option += "		trigger = { has_government = " + ideology + " }\n";
		option += "		add_political_power = -30\n";
		option += "		add_war_support = -0.03\n";
		if (ideology != "neutrality")
		{
			option += "		add_popularity = {\n";
			option += "			ideology = " + ideology + "\n";
			option += "			popularity = -0.05\n";
			option += "		}\n";
		}
		option += "	}";
		wargoalExpired.options.push_back(option);
		letter++;
	}
	warJustificationEvents.push_back(wargoalExpired);
}


void HoI4::Events::importElectionEvents(const std::set<std::string>& majorIdeologies, HoI4::OnActions& onActions)
{
	clearRegisteredKeywords();
	registerKeyword(std::regex("country_event"), [this, majorIdeologies](const std::string& type, std::istream& theStream){
		Event electionEvent(type, theStream);
		if ((majorIdeologies.count("democratic") > 0) || (electionEvent.id != "election.3"))
		{
			electionEvents.push_back(electionEvent);
		}
	});
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), commonItems::ignoreItem);

	parseFile("blankmod/output/events/ElectionEvents.txt");

	if (majorIdeologies.count("democratic") > 0)
	{
		onActions.addElectionEvent("election.3");
	}
}


void HoI4::Events::addPartyChoiceEvent(const std::string& countryTag, const std::set<Vic2::Party, std::function<bool (const Vic2::Party&, const Vic2::Party&)>>& parties, HoI4::OnActions& onActions, const set<string>& majorIdeologies)
{
	Event partyChoiceEvent;

	partyChoiceEvent.type = "country_event";
	partyChoiceEvent.id = "election." + std::to_string(electionEventNumber);
	partyChoiceEvent.title = "election." + std::to_string(electionEventNumber) + ".t";
	HoI4Localisation::copyEventLocalisations("party_choice.t", partyChoiceEvent.title);
	std::string description = "election." + std::to_string(electionEventNumber) + ".d";
	partyChoiceEvent.descriptions.push_back("desc = " + description);
	HoI4Localisation::copyEventLocalisations("party_choice.d", description);
	partyChoiceEvent.picture = "GFX_report_event_usa_election_generic";
	partyChoiceEvent.triggeredOnly = true;
	partyChoiceEvent.trigger = "= {\n";
	partyChoiceEvent.trigger += "		tag = " + countryTag + "\n";
	if (majorIdeologies.count("democratic") > 0)
	{
		partyChoiceEvent.trigger += "		OR = {\n";
		partyChoiceEvent.trigger += "			democratic > 0.5\n";
		partyChoiceEvent.trigger += "			neutrality > 0.5\n";
		partyChoiceEvent.trigger += "		}\n";
	}
	else
	{
		partyChoiceEvent.trigger += "		neutrality > 0.5\n";
	}
	partyChoiceEvent.trigger += "	}";

	char optionLetter = 'a';
	for (auto party: parties)
	{
		if ((party.getIdeology() == "conservative") || (party.getIdeology() == "liberal") || (party.getIdeology() == "socialist"))
		{
			std::string partyName = party.getName();
			std::string trimmedName = partyName.substr(4, partyName.size());

			std::string optionName = "election." + std::to_string(electionEventNumber) + optionLetter;
			std::string option = "= {\n";
			option += "		name = " + optionName + "\n";
			if (majorIdeologies.count("democratic") > 0)
			{
				option += "		set_party_name = { ideology = democratic long_name = " + countryTag + "_" + trimmedName + "_party " + "name = " + countryTag + "_" + trimmedName + "_party }\n";
			}
			else
			{
				option += "		set_party_name = { ideology = neutrality long_name = " + countryTag + "_" + trimmedName + "_party " + "name = " + countryTag + "_" + trimmedName + "_party }\n";
			}
			option += "		retire_country_leader = yes\n";
			option += "	}";
			partyChoiceEvent.options.push_back(option);
			HoI4Localisation::addEventLocalisationFromVic2(partyName, optionName);
			optionLetter++;
		}
	}

	onActions.addElectionEvent(partyChoiceEvent.id);
	electionEvents.push_back(partyChoiceEvent);
	electionEventNumber++;
}


void HoI4::Events::createStabilityEvents(const std::set<std::string>& majorIdeologies)
{
	clearRegisteredKeywords();
	registerKeyword(std::regex("add_namespace"), commonItems::ignoreString);
	registerKeyword(std::regex("country_event"), [this](const std::string& type, std::istream& theStream)
		{
			Event newEvent(type, theStream);
			if (newEvent.id.substr(0, 9) == "stability")
			{
				stabilityEvents.insert(make_pair(newEvent.id, newEvent));
			}
			else if (newEvent.id.substr(0, 6) == "mutiny")
			{
				mutinyEvents.insert(make_pair(newEvent.id, newEvent));
			}
			else
			{
				strikesEvents.insert(make_pair(newEvent.id, newEvent));
			}
		}
	);

	parseFile(Configuration::getHoI4Path() + "/events/stability_events.txt");

	if (majorIdeologies.count("democratic") == 0)
	{
		auto lowStabilitySelector = stabilityEvents.find("stability.3");
		lowStabilitySelector->second.meanTimeToHappen = "= {\n";
		lowStabilitySelector->second.meanTimeToHappen += "		days = 360\n";
		lowStabilitySelector->second.meanTimeToHappen += "		modifier = {\n";
		lowStabilitySelector->second.meanTimeToHappen += "			factor = 0.75\n";
		lowStabilitySelector->second.meanTimeToHappen += "			OR = {\n";
		lowStabilitySelector->second.meanTimeToHappen += "				has_stability < 0.4\n";
		lowStabilitySelector->second.meanTimeToHappen += "				has_war_support < 0.4\n";
		lowStabilitySelector->second.meanTimeToHappen += "			}\n";
		lowStabilitySelector->second.meanTimeToHappen += "		}\n";
		lowStabilitySelector->second.meanTimeToHappen += "		modifier = {\n";
		lowStabilitySelector->second.meanTimeToHappen += "			factor = 0.75\n";
		lowStabilitySelector->second.meanTimeToHappen += "			OR = {\n";
		lowStabilitySelector->second.meanTimeToHappen += "				has_stability < 0.2\n";
		lowStabilitySelector->second.meanTimeToHappen += "				has_war_support < 0.2\n";
		lowStabilitySelector->second.meanTimeToHappen += "			}\n";
		lowStabilitySelector->second.meanTimeToHappen += "		}\n";
		lowStabilitySelector->second.meanTimeToHappen += "	}";
	}

	auto conscriptionRebellion = stabilityEvents.find("stability.33");
	conscriptionRebellion->second.options.clear();
	std::string option = "= {\n";
	option += "		name = stability.33.a\n";
	option += "		if = {\n";
	if (majorIdeologies.count("democratic") > 0)
	{
		option += "			limit = { has_government = democratic }\n";
	}
	else
	{
		option += "			limit = { has_government = neutrality }\n";
	}
	if (majorIdeologies.count("communism") > 0)
	{
		option += "			start_civil_war = { ideology = communism size = 0.35 }\n";
	}
	else if (majorIdeologies.count("radical") > 0)
	{
		option += "			start_civil_war = { ideology = radical size = 0.35 }\n";
	}
	else if (majorIdeologies.count("fascism") > 0)
	{
		option += "			start_civil_war = { ideology = fascism size = 0.35 }\n";
	}
	else if (majorIdeologies.count("absolutist") > 0)
	{
		option += "			start_civil_war = { ideology = absolutist size = 0.35 }\n";
	}
	option += "		}\n";
	if (majorIdeologies.count("democratic") > 0)
	{
		option += "		if = {\n";
		option += "			limit = { \n";
		option += "				OR = {\n";
		for (auto majorIdeology: majorIdeologies)
		{
			if ((majorIdeology == "democratic") || (majorIdeology == "neutrality"))
			{
				continue;
			}

			option += "					has_government = " + majorIdeology + "\n";
		}
		option += "				} \n";
		option += "			}\n";
		option += "			start_civil_war = { ideology = democratic size = 0.45 }\n";
		option += "		}\n";
	}
	option += "		clr_country_flag = crisis_in_progress\n";
	option += "		clr_country_flag = crisis_solution_in_progress\n";
	option += "		set_country_flag = { flag = draft_dodging_resolved days = 90 }\n";
	option += "	}";
	conscriptionRebellion->second.options.push_back(option);
}