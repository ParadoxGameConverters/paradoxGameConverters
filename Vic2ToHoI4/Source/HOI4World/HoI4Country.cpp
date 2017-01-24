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

#include "HoI4Country.h"
#include "HoI4World.h"
#include <fstream>
#include "Log.h"
#include "ParadoxParserUTF8.h"
#include "HoI4Faction.h"
#include "HoI4Leader.h"
#include "HoI4Minister.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/V2Localisations.h"
#include "../V2World/V2Relations.h"
#include "../V2World/V2Party.h"
#include "../Mappers/ProvinceMapper.h"
#include "OSCompatibilityLayer.h"

enum ideaologyType {
	national_socialist = 0,
	fascistic = 1,
	paternal_autocrat = 2,
	social_conservative = 3,
	market_liberal = 4,
	social_liberal = 5,
	social_democrat = 6,
	left_wing_radical = 7,
	leninist = 8,
	stalinist = 9
};

const char* const ideologyNames[stalinist + 1] = {
	"national_socialist",
	"fascistic",
	"paternal_autocrat",
	"social_conservative",
	"market_libera",
	"social_libera",
	"social_democrat",
	"left_wing_radica",
	"leninist",
	"stalinist"
};

HoI4Country::HoI4Country(string _tag, string _commonCountryFile, HoI4World* _theWorld, bool _newCountry /* = false */)
{
	theWorld = _theWorld;
	newCountry = _newCountry;

	tag = _tag;
	commonCountryFile = _commonCountryFile;
	commonCountryFile.insert(1, tag + "-");
	provinces.clear();
	technologies.clear();

	capital = 0;
	ideology = "despotism";
	government = "";
	faction = nullptr;
	factionLeader = false;

	nationalUnity = 70;

	training_laws = "minimal_training";
	press_laws = "censored_press";
	industrial_policy_laws = "consumer_product_orientation";
	educational_investment_law = "minimal_education_investment";
	economic_law = "full_civilian_economy";
	conscription_law = "volunteer_army";
	civil_law = "limited_restrictions";

	relations.clear();
	allies.clear();
	practicals.clear();
	parties.clear();
	ministers.clear();
	rulingMinisters.clear();

	graphicalCulture = "Generic";
	majorNation = false;

	communismPopularity = 0;
	democraticPopularity = 80;
	facismPopularity = 0;
	neutralityPopularity = 20;
	liberalPopularity = 0;
	socialistPopularity = 0;
	ancapPopularity = 0;
	syndicalistPopularity = 0;
	autocraticPopularity = 0;

	greatPower = false;

	nationalFocus = nullptr;

	srcCountry = NULL;
}

void HoI4Country::output(const map<int, HoI4State*>& states, const vector<HoI4Faction*>& Factions) const
{
	// output history file
	ofstream output;

	if (((capital > 0) && (capital <= static_cast<int>(states.size())))
		&& newCountry
		)
	{
		output.open("Output/" + Configuration::getOutputName() + "/history/countries/" + Utils::convertUTF8ToASCII(filename));
		if (!output.is_open())
		{
			Log(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/history/countries" << Utils::convertUTF8ToASCII(filename);
			exit(-1);
		}
		output << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy
		if (newCountry)
		{
			output << "capital = " << capital << endl;
		}
		else
		{
			output << "capital =  1" << endl;
		}
		if (majorNation)
		{
			output << "set_research_slots = 4" << endl;
		}

		output << "" << endl;
		output << "oob = \"" << tag << "_OOB\"" << endl;
		output << "" << endl;
		output << "# Starting tech" << endl;
		output << "set_technology = {" << endl;
		for (auto tech : technologies)
		{
			output << tech.first << " = 1" << endl;
		}
		output << "}" << endl;
		output << "set_convoys = " + to_string(convoys) << endl;
		output << "1939.1.1 = {" << endl;
		output << "" << endl;
		output << "    " << endl;
		output << "}" << endl;
		output << "" << endl;
		output << "set_politics = {" << endl;
		output << "" << endl;
		output << "    parties = {" << endl;
		output << "        democratic = { " << endl;
		output << "            popularity = " << democraticPopularity << endl;
		output << "        }" << endl;
		output << "" << endl;
		output << "        liberal = {" << endl;
		output << "            popularity = " << liberalPopularity << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        socialist = {" << endl;
		output << "            popularity = " << socialistPopularity << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        syndicalism = {" << endl;
		output << "            popularity = " << syndicalistPopularity << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        ancap = {" << endl;
		output << "            popularity = " << ancapPopularity << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        fascism = {" << endl;
		output << "            popularity = " << facismPopularity << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        communism = {" << endl;
		output << "            popularity = " << communismPopularity << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        autocratic = {" << endl;
		output << "            popularity = " << autocraticPopularity << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        neutrality = { " << endl;
		output << "            popularity = " << neutralityPopularity << endl;
		output << "        }" << endl;
		output << "    }" << endl;
		output << "    " << endl;

		if (rulingHoI4Ideology == "")
		{
			output << "    ruling_party = neutrality" << endl;
		}
		else
		{
			output << "    ruling_party = " << rulingHoI4Ideology << endl;
		}

		output << "    last_election = \"1936.1.1\"" << endl;
		output << "    election_frequency = 48" << endl;
		output << "    elections_allowed = no" << endl;
		output << "}" << endl;

		outputRelations(output);

		output << "" << endl;
		for (auto Faction : Factions)
		{
			if (Faction->getLeader()->getTag() == tag)
			{
				output << "create_faction = \"Alliance of " + getSourceCountry()->getName("english") + "\"\n";
				for (auto factionmem : Faction->getMembers())
				{
					output << "add_to_faction = " + factionmem->getTag() + "\n";
				}
			}
		}
		output << endl;
		output << "add_ideas = {\n";
		if (majorNation)
			output << "great_power\n";
		if (!civilized)
			output << "uncivilized\n";
		if (RulingPartyModel.war_pol == "jingoism")
			output << "partial_economic_mobilisation\n";
		if (RulingPartyModel.war_pol == "pro_military")
			output << "low_economic_mobilisation\n";
		output << "}\n";
		output << "create_country_leader = {" << endl;
		output << "    name = \"Jigme Wangchuck\"" << endl;
		output << "    desc = \"POLITICS_JIGME_WANGCHUCK_DESC\"" << endl;
		output << "    picture = \"gfx / leaders / Asia / Portrait_Asia_Generic_2.dds\"" << endl;
		output << "    expire = \"1965.1.1\"" << endl;
		output << "    ideology = " << ideology << endl;
		output << "    traits = {" << endl;
		output << "        #" << endl;
		output << "    }" << endl;
		output << "}" << endl;
		output << "" << endl;
		output << "1939.1.1 = {" << endl;
		output << "    " << endl;
		output << "}" << endl;
		output.close();

		// output OOB file
		outputOOB();

		// output leaders file
		//outputLeaders();
		outputCommonCountryFile();

		/*fprintf(output, "graphical_culture = %s\n", graphicalCulture.c_str());
		fprintf(output, "\n");
		if (majorNation)
		{
		fprintf(output, "major = yes\n");
		fprintf(output, "\n");
		}
		fprintf(output, "default_templates = {\n");
		fprintf(output, "	generic_infantry = {\n");
		fprintf(output, "		infantry_brigade\n");
		fprintf(output, "		infantry_brigade\n");
		fprintf(output, "		infantry_brigade\n");
		fprintf(output, "	}\n");
		fprintf(output, "	generic_milita = {\n");
		fprintf(output, "		militia_brigade\n");
		fprintf(output, "		militia_brigade\n");
		fprintf(output, "		militia_brigade\n");
		fprintf(output, "	}\n");
		fprintf(output, "	generic_armoured = {\n");
		fprintf(output, "		armor_brigade\n");
		fprintf(output, "		motorized_brigade\n");
		fprintf(output, "		motorized_brigade\n");
		fprintf(output, "	}\n");
		fprintf(output, "	generic_cavalry = {\n");
		fprintf(output, "		cavalry_brigade\n");
		fprintf(output, "		cavalry_brigade\n");
		fprintf(output, "	}\n");
		fprintf(output, "}\n");
		fprintf(output, "\n");
		fprintf(output, "unit_names = {\n");*/
		//fprintf(output, "	infantry_brigade = {\n");
		//fprintf(output, "		\"Faizabad Division\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	cavalry_brigade = {\n");
		//fprintf(output, "		\"Faizabad Cavalry\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	motorized_brigade = {\n");
		//fprintf(output, "		\"Faizabad Motor Div.\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	mechanized_brigade = {\n");
		//fprintf(output, "		\"Faizabad Half Track Div.\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	light_armor_brigade = {\n");
		//fprintf(output, "		\"Faizabad Armoured Div.\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	armor_brigade = {\n");
		//fprintf(output, "		\"Faizabad Armoured Div.\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	paratrooper_brigade = {\n");
		//fprintf(output, "		\"Faizabad Para Division\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	marine_brigade = {\n");
		//fprintf(output, "		\"Faizabad Marine Division\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	bergsjaeger_brigade = {\n");
		//fprintf(output, "		\"Faizabad Mountain Division\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	garrison_brigade = {\n");
		//fprintf(output, "		\"Faizabad Division\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	hq_brigade = {\n");
		//fprintf(output, "		\"1st Afghanestani Army\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	militia_brigade = {\n");
		//fprintf(output, "		\"Faizabad Militia\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	multi_role = {\n");
		//fprintf(output, "		\"I.Fighter Group\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	interceptor = {\n");
		//fprintf(output, "		\"I.Fighter Group\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	strategic_bomber = {\n");
		//fprintf(output, "		\"I.Strategic Group\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	tactical_bomber = {\n");
		//fprintf(output, "		\"I.Tactical Group\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	naval_bomber = {\n");
		//fprintf(output, "		\"I.Naval Bomber Group\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	cas = {\n");
		//fprintf(output, "		\"I.Dive Bomber Group\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	transport_plane = {\n");
		//fprintf(output, "		\"I.Air Transport Group\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	battleship = {\n");
		//fprintf(output, "		\"RAS Afghanistan\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	heavy_cruiser = {\n");
		//fprintf(output, "		\"RAS Faizabad\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	destroyer = {\n");
		//fprintf(output, "		\"D1 / D2 / D3\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	carrier = {\n");
		//fprintf(output, "		\"RAS Zahir Shah\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	submarine = {\n");
		//fprintf(output, "		\"1. Submarine Flotilla\"\n");
		//fprintf(output, "	}\n");
		//fprintf(output, "	transport_ship = {\n");
		//fprintf(output, "		\"1. Troop Transport Flotilla\"\n");
		//fprintf(output, "	}\n");
		/*fprintf(output, "}\n");
		fprintf(output, "\n");
		fprintf(output, "ministers = {\n");
		for (auto ministerItr: ministers)
		{
		ministerItr.output(output);
		}
		fprintf(output, "}\n");*/
		//output.close();
	}

	if (nationalFocus != nullptr)
	{
		nationalFocus->output();
	}
}

void HoI4Country::outputCommonCountryFile() const
{
	ofstream output;
	output.open("Output/" + Configuration::getOutputName() + "/common/countries/" + Utils::convertUTF8ToASCII(commonCountryFile));
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/common/countries/" << Utils::convertUTF8ToASCII(commonCountryFile);
		exit(-1);
	}

	int red = 0;
	int green = 0;
	int blue = 0;
	color.GetRGB(red, green, blue);
	output << "color = { " << red << " " << green << " " << blue << " }" << endl;

	output.close();
}

void HoI4Country::outputColors(ofstream& out) const
{
	int red;
	int green;
	int blue;
	color.GetRGB(red, green, blue);

	out << tag << " = {\n";
	out << "\tcolor = rgb { " << red << " " << green << " " << blue << " }\n";
	out << "\tcolor_ui = rgb { " << red << " " << green << " " << blue << " }";
	out << "}\n";
}


void HoI4Country::outputToCommonCountriesFile(ofstream& countriesFile) const
{
	countriesFile << tag.c_str() << " = \"countries" << Utils::convertUTF8ToASCII(commonCountryFile) << "\"\n";
}


void HoI4Country::outputPracticals(FILE* output) const
{
	fprintf(output, "\n");
	for (auto itr : practicals)
	{
		if (itr.second > 0.0)
		{
			fprintf(output, "%s = %.2f\n", itr.first.c_str(), min(20.0, itr.second));
		}
	}
}

void HoI4Country::outputTech(FILE* output) const
{
	fprintf(output, "\n");
	for (auto itr : technologies)
	{
		fprintf(output, "%s = %d\n", itr.first.c_str(), itr.second);
	}
}

void HoI4Country::outputParties(FILE* output) const
{
	/*fprintf(output, "popularity = {\n");
	for (auto party: parties)
	{
	fprintf(output, "\t%s = %d\n", party.ideology.c_str(), party.popularity);
	}
	fprintf(output, "}\n");
	fprintf(output, "\n");

	fprintf(output, "organization = {\n");
	for (auto party : parties)
	{
	fprintf(output, "\t%s = %d\n", party.ideology.c_str(), party.organization);
	}
	fprintf(output, "}\n");
	fprintf(output, "\n");

	FILE* partyLocalisations;
	if (fopen_s(&partyLocalisations, ("Output/" + Configuration::getOutputName() + "/localisation/Parties.csv").c_str(), "a") != 0)
	{
	LOG(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/localisation/Parties.csv";
	exit(-1);
	}
	for (auto party: parties)
	{
	fprintf(partyLocalisations, "%s;\n", party.localisationString.c_str());
	}
	fclose(partyLocalisations);*/
}

void HoI4Country::outputLeaders() const
{
	FILE* leadersFile;
	if (fopen_s(&leadersFile, ("Output/" + Configuration::getOutputName() + "/history/leaders/" + tag + ".txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/history/leaders/" << tag << ".txt";
	}
	int landLeaders = 0;
	int seaLeaders = 0;
	int airLeaders = 0;
	for (auto leader : leaders)
	{
		leader.output(leadersFile);

		if (leader.getType() == "land")
		{
			landLeaders++;
		}
		else if (leader.getType() == "sea")
		{
			seaLeaders++;
		}
		else if (leader.getType() == "air")
		{
			airLeaders++;
		}
		else
		{
			LOG(LogLevel::Warning) << "Leader of unknown type in " << tag;
		}
	}
	fclose(leadersFile);

	LOG(LogLevel::Info) << tag << " has " << landLeaders << " land leaders, " << seaLeaders << " sea leaders, and " << airLeaders << " air leaders.";
}


void HoI4Country::outputRelations(ofstream& output) const
{
	for (auto relation: relations)
	{
		if (relation.first != tag)
		{
			output << "add_opinion_modifier = { target = " << relation.first << " modifier = ";
			if (relation.second->getRelations() < 0)
			{
				output << "negative_";
			}
			else
			{
				output << "positive_";
			}
			output << abs(relation.second->getRelations()) << " }\n";
		}
	}
}


void HoI4Country::outputOOB() const
{
	ofstream output("Output/" + Configuration::getOutputName() + "/history/units/" + tag + "_OOB.txt");
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open Output/" << Configuration::getOutputName() << "/history/units/" << tag << "_OOB.txt";
		exit(-1);
	}
	output << "\xEF\xBB\xBF";	// add the BOM to make HoI4 happy

								/*for (auto armyItr: armies)
								{
								if (armyItr->getProductionQueue())
								{
								armyItr->outputIntoProductionQueue(output, tag);
								}
								else
								{
								armyItr->output(output);
								}
								}*/
	output << "start_equipment_factor = 0\n";
	for (auto divisionTemplate : divisionTemplates)
	{
		output << divisionTemplate;
		output << endl;
	}
	output << "### No BHU air forces ###\n";
	output << "instant_effect = {\n";
	output << "\tadd_equipment_production = {\n";
	output << "\t\tequipment = {\n";
	output << "\t\t\ttype = infantry_equipment_0\n";
	output << "\t\t\tcreator = \"" << tag << "\"\n";
	output << "\t\t}\n";
	output << "\t\trequested_factories = 1\n";
	output << "\t\tprogress = 0.88\n";
	output << "\t\tefficiency = 100\n";
	output << "\t}\n";
	output << "}\n";
	output << "units = {\n";
	for (auto division : divisions)
	{
		output << division;
	}
	if (ships.size() > 0)
	{
		output << "\tnavy = {" << endl;
		output << "\t\tname = \"Grand Fleet\"" << endl;
		output << "\t\tlocation = " << navalLocation << endl;
		for (auto ship : ships)
		{
			output << ship;
		}
		output << "\t}" << endl;
	}
	output << "}\n";
	if (planes.size() > 0)
	{
		output << "air_wings = {\n";
		output << "\t" << capital << " = {\n";
		for (auto plane : planes)
		{
			output << plane;
		}
		output << "\t}\n";
		output << "}\n";
	}
	output.close();
}

void HoI4Country::initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _vic2ideology, map<int, int>& leaderMap, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const map<int, int>& stateMap, map<int, HoI4State*> states)
{
	srcCountry = _srcCountry;
	filename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (filename == "")
	{
		filename = Utils::GetFileFromTag(Configuration::getHoI4Path() + "/tfh/history/countries/", tag);
	}
	if (filename == "")
	{
		string countryName = commonCountryFile;
		int lastSlash = countryName.find_last_of("/");
		countryName = countryName.substr(lastSlash + 1, countryName.size());
		filename = tag + " - " + countryName;
	}

	// Color
	color = srcCountry->getColor();

	//Civilized
	civilized = srcCountry->isCivilized();

	// graphical culture type
	auto cultureItr = cultureMap.find(srcCountry->getPrimaryCulture());
	if (cultureItr != cultureMap.end())
	{
		graphicalCulture = cultureItr->second;
	}
	else
	{
		graphicalCulture = "Generic";
	}

	// Government
	string srcGovernment = srcCountry->getGovernment();
	if (srcGovernment.size() > 0)
	{
		government = governmentMapper::getInstance()->getGovernmentForCountry(srcCountry, _vic2ideology);
		if (government.empty())
		{
			government = "";
			LOG(LogLevel::Warning) << "No government mapping defined for " << srcGovernment << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}
	if (tag == "X64")
	{
		ideology = "fascism";
	}
	// Political parties
	convertParties(_srcCountry, _srcCountry->getActiveParties(_srcWorld.getParties()), _srcCountry->getRulingParty(_srcWorld.getParties()), ideology);
	for (auto partyItr : parties)
	{
		auto oldLocalisation = V2Localisations::GetTextInEachLanguage(partyItr.name);
		partyItr.localisationString = partyItr.ideology + "_" + tag;
		auto localisationItr = oldLocalisation.begin();
		localisationItr++;
		for (; localisationItr != oldLocalisation.end(); localisationItr++)
		{
			partyItr.localisationString += ";" + localisationItr->second;
		}
	}

	// Ministers
	vector<string> firstNames;
	vector<string> lastNames;
	auto namesItr = namesMap.find(srcCountry->getPrimaryCulture());
	if (namesItr != namesMap.end())
	{
		firstNames = namesItr->second.first;
		lastNames = namesItr->second.second;
	}
	else
	{
		firstNames.push_back("nul");
		lastNames.push_back("nul");
	}
	for (unsigned int ideologyIdx = 0; ideologyIdx <= stalinist; ideologyIdx++)
	{
		for (auto job : governmentJobs)
		{
			HoI4Minister newMinister(firstNames, lastNames, ideologyNames[ideologyIdx], job, governmentJobs, portraitMap[graphicalCulture]);
			ministers.push_back(newMinister);

			if (ideologyNames[ideologyIdx] == ideology)
			{
				rulingMinisters.push_back(newMinister);
			}
		}
	}

	// Faction is handled in HoI4World::configureFactions

	nationalUnity = 70.0 + (_srcCountry->getRevanchism() / 0.05) - (_srcCountry->getWarExhaustion() / 2.5);

	// civil law - democracies get open society, communist dicatorships get totalitarian, everyone else gets limited restrictions
	if (srcGovernment == "democracy" || srcGovernment == "hms_government")
	{
		civil_law = "open_society";
	}
	else if (srcGovernment == "proletarian_dictatorship")
	{
		civil_law = "totalitarian_system";
	}
	else
	{
		if (nationalUnity > 50.0)
		{
			civil_law = "limited_restrictions";
		}
		else
		{
			civil_law = "open_society";
		}
	}

	// conscription law - everyone starts with volunteer armies
	conscription_law = "volunteer_army";

	// economic law - everyone starts with full civilian economy
	economic_law = "full_civilian_economy";

	// educational investment law - from educational spending
	if (srcCountry->getEducationSpending() > 0.90)
	{
		educational_investment_law = "big_education_investment";
	}
	else if (srcCountry->getEducationSpending() > 0.70)
	{
		educational_investment_law = "medium_large_education_investment";
	}
	else if (srcCountry->getEducationSpending() > 0.40)
	{
		educational_investment_law = "average_education_investment";
	}
	else
	{
		educational_investment_law = "minimal_education_investment";
	}

	// industrial policy laws - everyone starts with consumer product orientation
	industrial_policy_laws = "consumer_product_orientation";

	// press laws - set from press reforms
	if (srcCountry->getReform("press_rights") == "free_press")
	{
		press_laws = "free_press";
	}
	else if (srcCountry->getReform("press_rights") == "censored_press")
	{
		press_laws = "censored_press";
	}
	else // press_rights == state_press
	{
		if ((srcGovernment == "proletarian_dictatorship") ||
			(srcGovernment == "fascist_dictatorship"))
		{
			press_laws = "propaganda_press";
		}
		else
		{
			press_laws = "state_press";
		}
	}

	// training laws - from military spending
	if (srcCountry->getMilitarySpending() > 0.90)
	{
		training_laws = "specialist_training";
	}
	else if (srcCountry->getMilitarySpending() > 0.70)
	{
		training_laws = "advanced_training";
	}
	else if (srcCountry->getMilitarySpending() > 0.40)
	{
		training_laws = "basic_training";
	}
	else
	{
		training_laws = "minimal_training";
	}

	// leaders
	vector<V2Leader*> srcLeaders = srcCountry->getLeaders();
	for (auto srcLeader : srcLeaders)
	{
		HoI4Leader newLeader(srcLeader, tag, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}

	Configuration::setLeaderIDForNextCountry();

	// Relations
	map<string, V2Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (auto itr : srcRelations)
		{
			const std::string& HoI4Tag = CountryMapper::getHoI4Tag(itr.second->getTag());
			if (!HoI4Tag.empty())
			{
				HoI4Relations* hoi2r = new HoI4Relations(HoI4Tag, itr.second);
				relations.insert(make_pair(HoI4Tag, hoi2r));
			}
		}
	}

	determineCapitalFromVic2(stateMap, states);
	auto state = states.find(capital)->second;
	if (isThisStateOwnedByUs(state))
	{
		state->setAsCapitalState();
	}

	// major nation
	majorNation = srcCountry->isGreatNation();
}

void HoI4Country::determineCapitalFromVic2(const map<int, int>& provinceToStateIDMap, const map<int, HoI4State*>& states)
{
	int oldCapital = srcCountry->getCapital();
	auto itr = provinceMapper::getVic2ToHoI4ProvinceMapping().find(oldCapital);
	if (itr != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
	{
		auto capitalState = provinceToStateIDMap.find(itr->second[0]);
		if (capitalState != provinceToStateIDMap.end() && isStateValidForCapital(capitalState, states))
		{
			capital = capitalState->second;
			setCapitalInCapitalState(itr->second[0], states);
		}
		else
		{
			findBestCapital();
		}
	}
}

bool HoI4Country::isStateValidForCapital(map<int, int>::const_iterator capitalState, const map<int, HoI4State*>& states)
{
	auto state = states.find(capitalState->second)->second;
	return (isThisStateOwnedByUs(state) || isThisStateACoreWhileWeOwnNoStates(state));
}

bool HoI4Country::isThisStateOwnedByUs(const HoI4State* state) const
{
	return (state->getOwner() == tag);
}

bool HoI4Country::isThisStateACoreWhileWeOwnNoStates(const HoI4State* state) const
{
	for (auto core : state->getCores())
	{
		if (core == tag)
		{
			return true;
		}
	}

	return false;
}

void HoI4Country::setCapitalInCapitalState(int capitalProvince, const map<int, HoI4State*>& states)
{
	auto capitalState = states.find(capital);
	if (capitalState != states.end())
	{
		capitalState->second->setVPLocation(capitalProvince);
	}
}

void HoI4Country::findBestCapital()
{
	capital = 1;
	LOG(LogLevel::Warning) << "Could not properly set capital for " << tag;
}

// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void HoI4Country::initFromHistory()
{
	string fullFilename;
	filename = Utils::GetFileFromTag("./blankMod/output/history/countries/", tag);
	if (filename == "")
	{
		filename = Utils::GetFileFromTag(Configuration::getHoI4Path() + "/history/countries/", tag);
	}
	else
	{
		fullFilename = string("./blankMod/output/history/countries/") + filename;
	}

	if (filename == "")
	{
		string countryName = commonCountryFile;
		int lastSlash = countryName.find_last_of("/");
		countryName = countryName.substr(lastSlash + 1, countryName.size());
		filename = tag + " - " + countryName;
		return;
	}
	else
	{
		fullFilename = Configuration::getHoI4Path() + "/history/countries/" + filename;
	}

	LOG(LogLevel::Debug) << "Parsing " << fullFilename;
	Object* obj = parser_UTF8::doParseFile(fullFilename);
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << fullFilename;
		exit(-1);
	}

	vector<Object*> results = obj->getValue("government");
	if (results.size() > 0)
	{
		government = results[0]->getLeaf();
	}

	results = obj->getValue("ideology");
	if (results.size() > 0)
	{
		ideology = results[0]->getLeaf();
	}

	results = obj->getValue("capital");
	if (results.size() > 0)
	{
		capital = stoi(results[0]->getLeaf());
	}
}

void HoI4Country::generateLeaders(leaderTraitsMap leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap)
{
	vector<string> firstNames;
	vector<string> lastNames;
	auto namesItr = namesMap.find(srcCountry->getPrimaryCulture());
	if (namesItr != namesMap.end())
	{
		firstNames = namesItr->second.first;
		lastNames = namesItr->second.second;
	}
	else
	{
		firstNames.push_back("nul");
		lastNames.push_back("nul");
	}

	// generated leaders
	int totalOfficers = 0;
	/*vector<V2Province*> srcProvinces = srcCountry->getCores();
	for (auto province : srcProvinces)
	{
		totalOfficers += province->getPopulation("officers");
	}*/

	unsigned int totalLand = 0;
	totalLand = totalOfficers / 300;
	if (totalLand > 350)
	{
		totalLand = 350;
	}
	if (totalLand < 10)
	{
		totalLand = 10;
	}
	if (factionLeader)
	{
		totalLand += 300;
	}
	for (unsigned int i = 0; i <= totalLand; i++)
	{
		HoI4Leader newLeader(firstNames, lastNames, tag, "land", leaderTraits, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}

	unsigned int totalSea = 0;
	if (totalOfficers <= 1000)
	{
		totalSea = totalOfficers / 100;
	}
	else
	{
		totalSea = static_cast<int>(totalOfficers / 822.0 + 8.78);
	}
	if (totalSea > 100)
	{
		totalSea = 100;
	}
	if (totalSea < 1)
	{
		totalSea = 1;
	}
	if (factionLeader)
	{
		totalSea += 20;
	}
	for (unsigned int i = 0; i <= totalSea; i++)
	{
		HoI4Leader newLeader(firstNames, lastNames, tag, "sea", leaderTraits, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}

	unsigned int totalAir = 0;
	if (totalOfficers <= 1000)
	{
		totalAir = totalOfficers / 100;
	}
	else
	{
		totalAir = static_cast<int>(totalOfficers / 925.0 + 12.62);
	}
	if (totalAir > 90)
	{
		totalAir = 90;
	}
	if (totalAir < 3)
	{
		totalAir = 3;
	}
	if (factionLeader)
	{
		totalAir += 20;
	}
	for (unsigned int i = 0; i <= totalAir; i++)
	{
		HoI4Leader newLeader(firstNames, lastNames, tag, "air", leaderTraits, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}
}

void HoI4Country::convertNavy(map<int, HoI4State*> states)
{
	int navalport = 0;

	// count the heavy and light Vic2 ships
	double heavyShip = 0;
	double lightShip = 0;
	for (auto army : srcCountry->getArmies())
	{
		for (auto regiment : army->getRegiments())
		{
			string type = regiment->getType();
			if (type == "battleship")
			{
				heavyShip += 0.08;
			}
			if (type == "dreadnought")
			{
				heavyShip += 0.1;
			}
			if (type == "cruiser")
			{
				lightShip += 0.1;
			}
		}
	}

	// determine the HoI4 ships
	double BB = 0;
	double BC = 0;
	double HC = 0;
	double LC = 0;
	double DD = 0;
	double CV = 0;
	double SB = 0;
	convoys = static_cast<int>(Configuration::getForceMultiplier() * ((heavyShip * 0.21945 * 40) + (lightShip * 1.88 * 4)));

	for (auto tech : technologies)
	{
		if ((tech.first == "early_light_cruiser") && (tech.second == 1))
		{
			LC = Configuration::getForceMultiplier() * lightShip * .47;
		}
		if ((tech.first == "early_destroyer") && (tech.second == 1))
		{
			DD = Configuration::getForceMultiplier() * lightShip * 1.88;
		}
		if ((tech.first == "early_submarine") && (tech.second == 1))
		{
			SB = Configuration::getForceMultiplier() * lightShip * .705;
		}
		if ((tech.first == "early_heavy_cruiser") && (tech.second == 1))
		{
			HC = Configuration::getForceMultiplier() * heavyShip * 0.2926;
		}
		if ((tech.first == "early_battlecruiser") && (tech.second == 1))
		{
			BC = Configuration::getForceMultiplier() * heavyShip * 0.073;
		}
		if ((tech.first == "early_battleship") && (tech.second == 1))
		{
			CV = Configuration::getForceMultiplier() * heavyShip * 0.073;
			BB = Configuration::getForceMultiplier() * heavyShip * 0.21945;
		}
	}

	for (int i = 0; i < CV; i++)
	{
		if (technologies.find("CAS1") != technologies.end() && technologies.find("naval_bomber1") != technologies.end() && technologies.find("early_carrier") != technologies.end())
		{
			HoI4Ship newShip("Carrier", "carrier", tag);
			ships.push_back(newShip);
		}
	}
	for (int i = 0; i < BB; i++)
	{
		HoI4Ship newShip("Battleship", "battleship", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < BC; i++)
	{
		HoI4Ship newShip("Battle Cruiser", "battle_cruiser", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < HC; i++)
	{
		HoI4Ship newShip("Heavy Cruiser", "heavy_cruiser", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < LC; i++)
	{
		HoI4Ship newShip("Light Cruiser", "light_cruiser", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < DD; i++)
	{
		HoI4Ship newShip("Destroyer", "destroyer", tag);
		ships.push_back(newShip);
	}
	for (int i = 0; i < SB; i++)
	{
		HoI4Ship newShip("Submarine", "submarine", tag);
		ships.push_back(newShip);
	}

	for (auto state : states)
	{
		if ((state.second->getOwner() == tag) && (state.second->getMainNavalLocation() != 0))
		{
			navalLocation = state.second->getMainNavalLocation();
		}
	}
}

void HoI4Country::convertAirforce()
{
	int airplanes = 0;
	for (auto army : srcCountry->getArmies())
	{
		for (auto regiment : army->getRegiments())
		{
			string type = regiment->getType();
			if (type == "plane")
			{
				airplanes += 1;
			}
		}
	}

	if (airplanes > 0)
	{
		auto techItr = technologies.find("early_fighter");
		if (techItr != technologies.end())
		{
			HoI4Airplane newPlane(string("fighter_equipment_0"), tag, Configuration::getForceMultiplier() * airplanes);
			planes.push_back(newPlane);
		}
		techItr = technologies.find("early_bomber");
		if (techItr != technologies.end())
		{
			HoI4Airplane newPlane(string("tac_bomber_equipment_0"), tag, Configuration::getForceMultiplier() * airplanes);
			planes.push_back(newPlane);
		}
		techItr = technologies.find("CAS1");
		if (techItr != technologies.end())
		{
			HoI4Airplane newPlane(string("CAS_equipment_1"), tag, Configuration::getForceMultiplier() * airplanes);
			planes.push_back(newPlane);
		}
	}
}

void HoI4Country::convertArmyDivisions()
{
	// get the total number of source brigades and the number of source brigades per location
	int infantryBrigades = 0;
	int artilleryBrigades = 0;
	int supportBrigades = 0;
	int tankBrigades = 0;
	int cavalryBrigades = 0;
	int cavalrySupportBrigades = 0;
	int mountainBrigades = 0;
	const double adjustment = 0.1 * Configuration::getForceMultiplier();

	map<int, double> locations;
	int totalRegiments = 0;
	for (auto army : srcCountry->getArmies())
	{
		// get the number of source brigades per location
		int HoI4location = 0;
		auto provMapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(army->getLocation());
		if (provMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
		{
			for (auto HoI4ProvNum : provMapping->second)
			{
				if (HoI4ProvNum != 0 && provinces.find(HoI4ProvNum) != provinces.end())
				{
					HoI4location = HoI4ProvNum;
				}
			}
		}

		// no weight for locations we don't own
		totalRegiments += army->getRegiments().size();
		if (provinces.find(HoI4location) != provinces.end()) {
			locations[HoI4location] += army->getRegiments().size();
		}

		// get the total number of source brigades
		for (auto regiment : army->getRegiments())
		{
			string type = regiment->getType();
			if (type == "artillery")
			{
				infantryBrigades += 2;
				artilleryBrigades++;
			}
			else if (type == "cavalry")
			{
				cavalryBrigades += 3;
			}
			else if (type == "cuirassier" || type == "dragoon" || type == "hussar")
			{
				cavalryBrigades += 3;
				cavalrySupportBrigades++;
			}
			else if (type == "engineer")
			{
				supportBrigades += 3;
			}
			else if (type == "guard")
			{
				mountainBrigades += 2;
			}
			else if (type == "infantry")
			{
				infantryBrigades += 3;
			}
			else if (type == "irregular")
			{
				infantryBrigades += 1;
			}
			else if (type == "tank")
			{
				tankBrigades++;
			}
		}
	}
	double InfWep = 0;
	if (technologies.find("infantry_weapons1") == technologies.end())
	{
		InfWep = 0.3;
	}
	if (technologies.find("infantry_weapons1") != technologies.end())
	{
		InfWep = 0.5;
	}
	armyStrength = 0;
	armyStrength = static_cast<long>((100 * InfWep*infantryBrigades) + (supportBrigades * 175) + (artilleryBrigades * 126) + (tankBrigades * 1135) + (cavalryBrigades * 120 * InfWep) + (mountainBrigades * 140 * InfWep));
	// calculate the number of brigades in different types of divisions
	int infantryPerDivision = 0;
	if (infantryBrigades <= 45)
	{
		infantryPerDivision = 3;
	}
	else if (infantryBrigades <= 90)
	{
		infantryPerDivision = 6;
	}
	else
	{
		infantryPerDivision = 9;
	}

	int tanksPerDivision = 0;
	if (tankBrigades <= 5)
	{
		tanksPerDivision = 1;
	}
	else if (tankBrigades <= 10)
	{
		tanksPerDivision = 2;
	}
	else
	{
		tanksPerDivision = 3;
	}

	int cavalryPerDivision = 0;
	if (cavalryBrigades <= 9)
	{
		cavalryPerDivision = 1;
	}
	else if (cavalryBrigades <= 18)
	{
		cavalryPerDivision = 2;
	}
	else
	{
		cavalryPerDivision = 3;
	}

	int mountainPerDivision = 0;
	if (mountainBrigades <= 9)
	{
		mountainPerDivision = 1;
	}
	else if (mountainBrigades <= 18)
	{
		mountainPerDivision = 2;
	}
	else
	{
		mountainPerDivision = 3;
	}

	int advancedIndex = -1;
	int mediumIndex = -1;
	int basicIndex = -1;
	int tankIndex = -1;
	bool mediumSupport = false;
	// create division templates
	if (tankBrigades > 0)
	{
		HoI4DivisionTemplateType newDivisionTemplate("Tank Division");
		for (int i = 0; i < tanksPerDivision; i++)
		{
			HoI4RegimentType lightArmorRegimentOne("light_armor", 0, i);
			newDivisionTemplate.addRegiment(lightArmorRegimentOne);
			HoI4RegimentType lightArmorRegimentTwo("light_armor", 1, i);
			newDivisionTemplate.addRegiment(lightArmorRegimentTwo);
			HoI4RegimentType motorizedrRegiment("motorized", 2, i);
			newDivisionTemplate.addRegiment(motorizedrRegiment);
		}
		tankIndex = divisionTemplates.size();
		divisionTemplates.push_back(newDivisionTemplate);
	}
	if (cavalryBrigades > 0)
	{
		HoI4DivisionTemplateType newDivisionTemplate("Cavalry Division");
		for (int i = 0; i < cavalryPerDivision; i++)
		{
			HoI4RegimentType cavalryRegimentOne("cavalry", 0, i);
			newDivisionTemplate.addRegiment(cavalryRegimentOne);
			HoI4RegimentType cavalryRegimentTwo("cavalry", 1, i);
			newDivisionTemplate.addRegiment(cavalryRegimentTwo);
			HoI4RegimentType cavalryRegimentThree("cavalry", 2, i);
			newDivisionTemplate.addRegiment(cavalryRegimentThree);
		}
		divisionTemplates.push_back(newDivisionTemplate);
	}
	if (mountainBrigades > 0)
	{
		HoI4DivisionTemplateType newDivisionTemplate("Mountaineers");
		for (int i = 0; i < cavalryPerDivision; i++)
		{
			HoI4RegimentType mountainRegimentOne("mountaineers", 0, i);
			newDivisionTemplate.addRegiment(mountainRegimentOne);
			HoI4RegimentType mountainRegimentTwo("mountaineers", 1, i);
			newDivisionTemplate.addRegiment(mountainRegimentTwo);
			HoI4RegimentType mountainRegimentThree("mountaineers", 2, i);
			newDivisionTemplate.addRegiment(mountainRegimentThree);
		}
		divisionTemplates.push_back(newDivisionTemplate);
	}
	if ((artilleryBrigades > 0) || (supportBrigades > 0))
	{
		if (3 * artilleryBrigades > infantryPerDivision * supportBrigades)
		{
			//there are more brigades with artillery than with support, meddiv will have only art
			HoI4DivisionTemplateType newDivisionTemplate("Support Infantry Division");
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegiment("infantry", 0, i);
				newDivisionTemplate.addRegiment(infantryRegiment);
			}
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegimentOne("infantry", i, 0);
				newDivisionTemplate.addRegiment(infantryRegimentOne);
				HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
				newDivisionTemplate.addRegiment(infantryRegimentTwo);
				HoI4RegimentType infantryRegimentThree("infantry", i, 2);
				newDivisionTemplate.addRegiment(infantryRegimentThree);
				HoI4RegimentType artilleryRegiment("artillery_brigade", i, 3);
				newDivisionTemplate.addRegiment(artilleryRegiment);
			}
			mediumIndex = divisionTemplates.size();
			divisionTemplates.push_back(newDivisionTemplate);

			if (supportBrigades > 0)
			{
				//have both support brigades and artillery, have superdiv
				HoI4DivisionTemplateType newDivisionTemplate("Advance Infantry Division");
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegiment("infantry", 0, i);
					newDivisionTemplate.addRegiment(infantryRegiment);
				}
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegimentOne("infantry", i, 0);
					newDivisionTemplate.addRegiment(infantryRegimentOne);
					HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
					newDivisionTemplate.addRegiment(infantryRegimentTwo);
					HoI4RegimentType infantryRegimentThree("infantry", i, 2);
					newDivisionTemplate.addRegiment(infantryRegimentThree);
					HoI4RegimentType artilleryRegiment("artillery_brigade", i, 3);
					newDivisionTemplate.addRegiment(artilleryRegiment);
				}
				HoI4RegimentType engineerRegiment("engineer", 0, 0);
				newDivisionTemplate.addSupportRegiment(engineerRegiment);
				HoI4RegimentType reconRegiment("recon", 0, 1);
				newDivisionTemplate.addSupportRegiment(reconRegiment);

				advancedIndex = divisionTemplates.size();
				divisionTemplates.push_back(newDivisionTemplate);
			}
		}
		else
		{
			//there are more brigades with support then artillery, meddiv will have only support
			HoI4DivisionTemplateType newDivisionTemplate("Support Infantry Division");
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegiment("infantry", 0, i);
				newDivisionTemplate.addRegiment(infantryRegiment);
			}
			for (int i = 0; i < (infantryPerDivision / 3); i++)
			{
				HoI4RegimentType infantryRegimentOne("infantry", i, 0);
				newDivisionTemplate.addRegiment(infantryRegimentOne);
				HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
				newDivisionTemplate.addRegiment(infantryRegimentTwo);
				HoI4RegimentType infantryRegimentThree("infantry", i, 2);
				newDivisionTemplate.addRegiment(infantryRegimentThree);
			}
			HoI4RegimentType engineerRegiment("engineer", 0, 0);
			newDivisionTemplate.addSupportRegiment(engineerRegiment);
			HoI4RegimentType reconRegiment("recon", 0, 1);
			newDivisionTemplate.addSupportRegiment(reconRegiment);
			mediumSupport = true;
			mediumIndex = divisionTemplates.size();
			divisionTemplates.push_back(newDivisionTemplate);

			if (artilleryBrigades != 0)
			{
				//have both supportbrigs and artillery, have superdiv
				HoI4DivisionTemplateType newDivisionTemplate("Advance Infantry Division");
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegiment("infantry", 0, i);
					newDivisionTemplate.addRegiment(infantryRegiment);
				}
				for (int i = 0; i < (infantryPerDivision / 3); i++)
				{
					HoI4RegimentType infantryRegimentOne("infantry", i, 0);
					newDivisionTemplate.addRegiment(infantryRegimentOne);
					HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
					newDivisionTemplate.addRegiment(infantryRegimentTwo);
					HoI4RegimentType infantryRegimentThree("infantry", i, 2);
					newDivisionTemplate.addRegiment(infantryRegimentThree);
					HoI4RegimentType artilleryRegiment("artillery_brigade", i, 3);
					newDivisionTemplate.addRegiment(artilleryRegiment);
				}
				HoI4RegimentType engineerRegiment("engineer", 0, 0);
				newDivisionTemplate.addSupportRegiment(engineerRegiment);
				HoI4RegimentType reconRegiment("recon", 0, 1);
				newDivisionTemplate.addSupportRegiment(reconRegiment);
				advancedIndex = divisionTemplates.size();
				divisionTemplates.push_back(newDivisionTemplate);
			}
		}
	}

	// basic infantry division
	HoI4DivisionTemplateType newDivisionTemplate("Basic Infantry Division");
	for (int i = 0; i < (infantryPerDivision / 3); i++)
	{
		HoI4RegimentType infantryRegiment("infantry", 0, i);
		newDivisionTemplate.addRegiment(infantryRegiment);
	}
	for (int i = 0; i < (infantryPerDivision / 3); i++)
	{
		HoI4RegimentType infantryRegimentOne("infantry", i, 0);
		newDivisionTemplate.addRegiment(infantryRegimentOne);
		HoI4RegimentType infantryRegimentTwo("infantry", i, 1);
		newDivisionTemplate.addRegiment(infantryRegimentTwo);
		HoI4RegimentType infantryRegimentThree("infantry", i, 2);
		newDivisionTemplate.addRegiment(infantryRegimentThree);
	}
	basicIndex = divisionTemplates.size();
	divisionTemplates.push_back(newDivisionTemplate);

	// calculate number of units per location
	double totalWeight = 0;
	if (0 == locations.size())
	{
		locations[capital] = totalRegiments;
	}
	for (auto const location : locations)
	{
		totalWeight += location.second;
	}
	int numberOfDivisions = infantryBrigades / infantryPerDivision;
	for (auto& location : locations)
	{
		if (totalWeight > 0)
		{
			// Use ceiling here to avoid losing units to, eg, numberOfDivisions = 12,
			// totalWeight = 13. This can happen in the presence of aircraft.
			location.second = ceil(location.second * adjustment * numberOfDivisions / totalWeight);
		}
	}

	// place units
	int numCav = 1;
	int numTank = 1;
	int numMountain = 1;
	int numAdvanced = 1;
	int numMedium = 1;
	int numBasic = 1;

	infantryBrigades = static_cast<int>(0.5 + adjustment * infantryBrigades);
	artilleryBrigades = static_cast<int>(0.5 + adjustment * artilleryBrigades);
	supportBrigades = static_cast<int>(0.5 + adjustment * supportBrigades);
	tankBrigades = static_cast<int>(0.5 + adjustment * tankBrigades);
	cavalryBrigades = static_cast<int>(0.5 + adjustment * cavalryBrigades);
	cavalrySupportBrigades = static_cast<int>(0.5 + adjustment * cavalrySupportBrigades);
	mountainBrigades = static_cast<int>(0.5 + adjustment * mountainBrigades);

	for (auto const location : locations)
	{
		int unitsInProv = 0;
		while (unitsInProv < location.second)
		{
			if (infantryBrigades >= infantryPerDivision)
			{
				int infLocation = 0;
				if (location.first != 0)
				{
					infLocation = location.first;
				}
				else if (capital != 0)
				{
					infLocation = capital;
				}
				else
				{
					LOG(LogLevel::Warning) << "When converting units for " << tag << ", one of the locations for unit placement was undefined!";
					break;
				}
				if (tankBrigades > 0)
				{
					HoI4DivisionType newDivision(to_string(numTank++) + ". Tank Division", "Tank Division", infLocation);
					divisions.push_back(newDivision);
					tankBrigades -= tanksPerDivision;
				}
				if (cavalryBrigades > 0)
				{
					HoI4DivisionType newDivision(to_string(numCav++) + ". Cavalry Division", "Cavalry Division", infLocation);
					divisions.push_back(newDivision);
					cavalryBrigades -= cavalryPerDivision;
				}
				if (mountainBrigades > 0)
				{
					HoI4DivisionType newDivision(to_string(numMountain++) + ". Mountaineers", "Mountaineers", infLocation);
					divisions.push_back(newDivision);
					mountainBrigades -= mountainPerDivision;
				}

				if ((3 * artilleryBrigades >= infantryPerDivision) && (supportBrigades >= 1))
				{
					// Super Placement
					HoI4DivisionType newDivision(to_string(numAdvanced++) + ". Advance Infantry Division", "Advance Infantry Division", infLocation);
					divisions.push_back(newDivision);
					artilleryBrigades -= (infantryPerDivision / 3);
					supportBrigades--;
				}
				else if ((3 * artilleryBrigades >= infantryPerDivision) || (supportBrigades >= 1))
				{
					// Med Placement
					HoI4DivisionType newDivision(to_string(numMedium++) + ". Support Infantry Division", "Support Infantry Division", infLocation);
					divisions.push_back(newDivision);
					artilleryBrigades -= (infantryPerDivision / 3);
					supportBrigades--;
				}
				else
				{
					// Bad Placement
					HoI4DivisionType newDivision(to_string(numBasic++) + ". Basic Infantry Division", "Basic Infantry Division", infLocation);
					divisions.push_back(newDivision);
				}
				infantryBrigades -= infantryPerDivision;
				unitsInProv++;
			}
			else
				break;
		}
	}
	if (artilleryBrigades >= numAdvanced && advancedIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, 2);
		divisionTemplates[advancedIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numAdvanced;
	}
	if (artilleryBrigades >= numMedium && mediumIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, mediumSupport ? 2 : 0);
		divisionTemplates[mediumIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numMedium;
	}
	if (artilleryBrigades >= numBasic && basicIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, 0);
		divisionTemplates[basicIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numBasic;
	}
	if (artilleryBrigades >= numTank && tankIndex != -1)
	{
		HoI4RegimentType artilleryRegiment("artillery", 0, 0);
		divisionTemplates[tankIndex].addSupportRegiment(artilleryRegiment);
		artilleryBrigades -= numTank;
	}
}

void HoI4Country::addProvince(int _province)
{
	provinces.insert(_province);
	if (capital == 0)
	{
		capital = _province;
	}
}

void HoI4Country::addState(HoI4State* _state)
{
	states.insert(make_pair(_state->getID(), _state));
	for (const auto province : _state->getProvinces()) {
		addProvince(province);
	}
}

HoI4Relations* HoI4Country::getRelations(string withWhom) const
{
	map<string, HoI4Relations*>::const_iterator i = relations.find(withWhom);
	if (i != relations.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}

vector<int> HoI4Country::getPortProvinces(vector<int> locationCandidates, map<int, HoI4Province*> allProvinces)
{
	// hack for naval bases.  not ALL naval bases are in port provinces, and if you spawn a navy at a naval base in
	// a non-port province, Vicky crashes....
	static vector<int> port_blacklist;
	if (port_blacklist.size() == 0)
	{
		int temp = 0;
		ifstream s("port_blacklist.txt");
		while (s.good() && !s.eof())
		{
			s >> temp;
			port_blacklist.push_back(temp);
		}
		s.close();
	}

	for (auto litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		auto black = std::find(port_blacklist.begin(), port_blacklist.end(), *litr);
		if (black != port_blacklist.end())
		{
			locationCandidates.erase(litr);
			break;
		}
	}
	for (auto litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		auto pitr = allProvinces.find(*litr);
		if (pitr != allProvinces.end())
		{
			if (!pitr->second->hasNavalBase())
			{
				locationCandidates.erase(litr);
				--pitr;
				break;
			}
		}
	}

	return locationCandidates;
}

void HoI4Country::convertParties(const V2Country* srcCountry, vector<V2Party*> V2Parties, V2Party* rulingParty, string& rulingIdeology)
{
	// sort Vic2 parties by ideology
	map<string, vector<V2Party*>> V2Ideologies;
	for (auto partyItr : V2Parties)
	{
		string ideology = partyItr->ideology;
		auto ideologyItr = V2Ideologies.find(ideology);
		if (ideologyItr == V2Ideologies.end())
		{
			vector<V2Party*> newPartyVector;
			newPartyVector.push_back(partyItr);
			V2Ideologies.insert(make_pair(ideology, newPartyVector));
		}
		else
		{
			ideologyItr->second.push_back(partyItr);
		}
	}
	// prep unmapped HoI4 parties
	map<string, string> unmappedParties;
	unmappedParties.insert(make_pair("national_socialist", "fascist_group"));
	unmappedParties.insert(make_pair("fascistic", "fascist_group"));
	unmappedParties.insert(make_pair("paternal_autocrat", "fascist_group"));
	unmappedParties.insert(make_pair("social_conservative", "democratic_group"));
	unmappedParties.insert(make_pair("market_liberal", "democratic_group"));
	unmappedParties.insert(make_pair("social_liberal", "democratic_group"));
	unmappedParties.insert(make_pair("social_democrat", "democratic_group"));
	unmappedParties.insert(make_pair("left_wing_radical", "communist_group"));
	unmappedParties.insert(make_pair("leninist", "communist_group"));
	unmappedParties.insert(make_pair("stalinist", "communist_group"));
	// map all the simplistic cases
	auto ideologyItr = V2Ideologies.find("fascist");
	if ((ideologyItr != V2Ideologies.end()))
	{
		for (unsigned int i = 0; i < ideologyItr->second.size(); i++)
		{
			HoI4Party newParty;
			newParty.name = ideologyItr->second[i]->name;
			newParty.war_pol = ideologyItr->second[i]->war_policy;
			newParty.ideology = "fascistic";
			newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage("fascist") * 100 + 0.5);
			newParty.organization = newParty.popularity;
			parties.push_back(newParty);

			if (rulingParty == ideologyItr->second[i])
			{
				RulingPartyModel = newParty;
				rulingIdeology = "fascistic";
				rulingHoI4Ideology = "fascism";
			}

			V2Ideologies.erase(ideologyItr);
			auto itr = unmappedParties.find("fascistic");
			unmappedParties.erase(itr);
		}
	}
	ideologyItr = V2Ideologies.find("reactionary");
	if ((ideologyItr != V2Ideologies.end()))
	{
		for (unsigned int i = 0; i < ideologyItr->second.size(); i++)
		{
			HoI4Party newParty;
			newParty.name = ideologyItr->second[i]->name;
			newParty.war_pol = ideologyItr->second[i]->war_policy;
			newParty.ideology = "paternal_autocrat";
			newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage("reactionary") * 100 + 0.5);
			newParty.organization = newParty.popularity;
			parties.push_back(newParty);

			if (rulingParty == ideologyItr->second[i])
			{
				RulingPartyModel = newParty;
				rulingIdeology = "paternal_autocrat";
				rulingHoI4Ideology = "autocratic";
			}

			V2Ideologies.erase(ideologyItr);
			auto itr = unmappedParties.find("paternal_autocrat");
			unmappedParties.erase(itr);
		}
	}
	ideologyItr = V2Ideologies.find("conservative");
	if ((ideologyItr != V2Ideologies.end()))
	{
		for (unsigned int i = 0; i < ideologyItr->second.size(); i++)
		{
			HoI4Party newParty;
			newParty.name = ideologyItr->second[i]->name;
			newParty.war_pol = ideologyItr->second[i]->war_policy;
			newParty.ideology = "social_conservative";
			newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage("conservative") * 100 + 0.5);
			newParty.organization = newParty.popularity;
			parties.push_back(newParty);

			if (rulingParty == ideologyItr->second[i])
			{
				RulingPartyModel = newParty;
				rulingIdeology = "social_conservative";
				rulingHoI4Ideology = "democratic";
			}

			V2Ideologies.erase(ideologyItr);
			auto itr = unmappedParties.find("social_conservative");
			unmappedParties.erase(itr);
		}
	}
	ideologyItr = V2Ideologies.find("socialist");
	if ((ideologyItr != V2Ideologies.end()))
	{
		for (unsigned int i = 0; i < ideologyItr->second.size(); i++)
		{
			HoI4Party newParty;
			newParty.name = ideologyItr->second[i]->name;
			newParty.war_pol = ideologyItr->second[i]->war_policy;
			newParty.ideology = "left_wing_radical";
			newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage("socialist") * 100 + 0.5);
			newParty.organization = newParty.popularity;
			parties.push_back(newParty);

			if (rulingParty == ideologyItr->second[i])
			{
				RulingPartyModel = newParty;
				rulingIdeology = "left_wing_radical";
				rulingHoI4Ideology = "socialist";
			}

			V2Ideologies.erase(ideologyItr);
			auto itr = unmappedParties.find("left_wing_radical");
			unmappedParties.erase(itr);
		}
	}
	ideologyItr = V2Ideologies.find("communist");
	if ((ideologyItr != V2Ideologies.end()))
	{
		for (unsigned int i = 0; i < ideologyItr->second.size(); i++)
		{
			HoI4Party newParty;
			newParty.name = ideologyItr->second[i]->name;
			newParty.war_pol = ideologyItr->second[i]->war_policy;
			newParty.ideology = "stalinist";
			newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage("communist") * 100 + 0.5);
			newParty.organization = newParty.popularity;
			parties.push_back(newParty);

			if (rulingParty == ideologyItr->second[i])
			{
				RulingPartyModel = newParty;
				rulingIdeology = "stalinist";
				rulingHoI4Ideology = "communism";
			}

			V2Ideologies.erase(ideologyItr);
			auto itr = unmappedParties.find("stalinist");
			unmappedParties.erase(itr);
		}
	}
	ideologyItr = V2Ideologies.find("liberal");
	if ((ideologyItr != V2Ideologies.end()))
	{
		for (unsigned int i = 0; i < ideologyItr->second.size(); i++)
		{
			HoI4Party newParty;
			newParty.name = ideologyItr->second[i]->name;
			newParty.war_pol = ideologyItr->second[i]->war_policy;
			newParty.ideology = "social_liberal";
			newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage("liberal") * 100 + 0.5);
			newParty.organization = newParty.popularity;
			parties.push_back(newParty);

			if (rulingParty == ideologyItr->second[i])
			{
				RulingPartyModel = newParty;
				rulingIdeology = "social_liberal";
				rulingHoI4Ideology = "liberal";
			}

			V2Ideologies.erase(ideologyItr);
			auto itr = unmappedParties.find("social_liberal");
			unmappedParties.erase(itr);
		}
	}
	ideologyItr = V2Ideologies.find("anarcho_liberal");
	if ((ideologyItr != V2Ideologies.end()))
	{
		for (unsigned int i = 0; i < ideologyItr->second.size(); i++)
		{
			HoI4Party newParty;
			newParty.name = ideologyItr->second[i]->name;
			newParty.war_pol = ideologyItr->second[i]->war_policy;
			newParty.ideology = "market_liberal";
			newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage("anarcho_liberal") * 100 + 0.5);
			newParty.organization = newParty.popularity;
			parties.push_back(newParty);

			if (rulingParty == ideologyItr->second[i])
			{
				RulingPartyModel = newParty;
				rulingIdeology = "market_liberal";
				rulingHoI4Ideology = "ancap";
			}

			V2Ideologies.erase(ideologyItr);
			auto itr = unmappedParties.find("market_liberal");
			unmappedParties.erase(itr);
		}
	}

	if (V2Ideologies.size() == 0)
	{
		setPartyPopularity();
		return;
	}

	// map the simple excess cases
	map<string, vector<V2Party*>> V2IdeologyGroups;
	ideologyItr = V2Ideologies.find("fascist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("reactionary");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("conservative");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("socialist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("communist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("anarcho_liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("democratic_group", parties));
			}
		}
	}

	map<string, vector<string>> HoI4IdeologyGroups;
	for (auto HoI4PartyItr : unmappedParties)
	{
		auto groupItr = HoI4IdeologyGroups.find(HoI4PartyItr.second);
		if (groupItr != HoI4IdeologyGroups.end())
		{
			groupItr->second.push_back(HoI4PartyItr.first);
		}
		else
		{
			vector<string> parties;
			parties.push_back(HoI4PartyItr.first);
			HoI4IdeologyGroups.insert(make_pair(HoI4PartyItr.second, parties));
		}
	}

	for (auto V2GroupItr : V2IdeologyGroups)
	{
		auto HoI4GroupItr = HoI4IdeologyGroups.find(V2GroupItr.first);
		if ((HoI4GroupItr != HoI4IdeologyGroups.end()) && (V2GroupItr.second.size() <= HoI4GroupItr->second.size()))
		{
			for (auto V2PartyItr : V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);

				HoI4Party newParty;
				newParty.name = V2PartyItr->name;
				newParty.ideology = HoI4GroupItr->second[0];
				newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage(ideologyItr->first) * 100 / ideologyItr->second.size() + 0.5);
				newParty.organization = newParty.popularity;
				parties.push_back(newParty);

				if (rulingParty->name == V2PartyItr->name)
				{
					rulingIdeology = HoI4GroupItr->second[0];
				}

				HoI4GroupItr->second.erase(HoI4GroupItr->second.begin());

				auto itr = unmappedParties.find(newParty.ideology);
				unmappedParties.erase(itr);
			}
			for (auto V2PartyItr : V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);
				if (ideologyItr != V2Ideologies.end())
				{
					V2Ideologies.erase(ideologyItr);
				}
			}
			V2GroupItr.second.clear();
		}
		if (HoI4GroupItr->second.size() == 0)
		{
			HoI4IdeologyGroups.erase(HoI4GroupItr);
		}
	}
	if (V2Ideologies.size() == 0)
	{
		setPartyPopularity();
		return;
	}

	// merge Vic2 parties by ideology, then map those cases
	for (auto ideologyItr : V2Ideologies)
	{
		while (ideologyItr.second.size() > 1)
		{
			ideologyItr.second.pop_back();
		}
	}

	V2IdeologyGroups.clear();
	ideologyItr = V2Ideologies.find("fascist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("reactionary");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("conservative");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("socialist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("communist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find("anarcho_liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr : ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find("democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair("democratic_group", parties));
			}
		}
	}

	for (auto V2GroupItr : V2IdeologyGroups)
	{
		auto HoI4GroupItr = HoI4IdeologyGroups.find(V2GroupItr.first);
		if ((HoI4GroupItr != HoI4IdeologyGroups.end()) && (V2GroupItr.second.size() <= HoI4GroupItr->second.size()))
		{
			for (auto V2PartyItr : V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);

				HoI4Party newParty;
				newParty.name = V2PartyItr->name;
				newParty.ideology = HoI4GroupItr->second[0];
				newParty.popularity = static_cast<unsigned int>(srcCountry->getUpperHousePercentage(ideologyItr->first) * 100 / ideologyItr->second.size() + 0.5);
				newParty.organization = newParty.popularity;
				parties.push_back(newParty);

				HoI4GroupItr->second.erase(HoI4GroupItr->second.begin());

				auto itr = unmappedParties.find(newParty.ideology);
				unmappedParties.erase(itr);

				if (rulingParty->ideology == V2PartyItr->ideology)
				{
					rulingIdeology = HoI4GroupItr->second[0];
				}
			}
			for (auto V2PartyItr : V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);
				if (ideologyItr != V2Ideologies.end())
				{
					V2Ideologies.erase(ideologyItr);
				}
			}
			V2GroupItr.second.clear();
		}
		if (HoI4GroupItr->second.size() == 0)
		{
			HoI4IdeologyGroups.erase(HoI4GroupItr);
		}
	}

	if (rulingHoI4Ideology == "")
	{
		rulingHoI4Ideology = "neutrality";
	}

	if (V2Ideologies.size() > 0)
	{
		LOG(LogLevel::Warning) << "Unmapped Vic2 parties for " << tag;
	}
	setPartyPopularity();
}
void HoI4Country::setPartyPopularity()
{
	communismPopularity = 0;
	democraticPopularity = 0;
	facismPopularity = 0;
	neutralityPopularity = 0;
	liberalPopularity = 0;
	socialistPopularity = 0;
	syndicalistPopularity = 0;
	ancapPopularity = 0;
	autocraticPopularity = 0;

	if (rulingHoI4Ideology == "fascism")
		ideology = "fascism_ideology";
	else if (rulingHoI4Ideology == "democratic")
		ideology = "democratic_conservative";
	else if (rulingHoI4Ideology == "ancap")
		ideology = "anarcho_cap";
	else if (rulingHoI4Ideology == "communism")
		ideology = "marxism";
	else if (rulingHoI4Ideology == "syndicalism")
		ideology = "national_syndicalist";
	else if (rulingHoI4Ideology == "liberal")
		ideology = "democratic_liberal";
	else if (rulingHoI4Ideology == "autocratic")
		ideology = "absolute_monarchy";
	else if (rulingHoI4Ideology == "socialist")
		ideology = "democratic_socialist ";
	else
		ideology = "despotism";

	for (auto party : parties)
	{
		if (party.name.find("fascist") != string::npos)
		{
			facismPopularity += party.popularity;
		}
		if (party.name.find("communist") != string::npos)
		{
			communismPopularity += party.popularity;
		}
		if (party.name.find("liberal") != string::npos)
		{
			liberalPopularity += party.popularity;
		}
		if (party.name.find("conservative") != string::npos && (government == "democratic" || government == "hms_government "))
		{
			democraticPopularity += party.popularity;
		}
		if (party.name.find("conservative") != string::npos && government != "democratic" && government != "hms_government ")
		{
			autocraticPopularity += party.popularity;
		}
		if (party.name.find("socialist") != string::npos && (party.war_pol == "anti_military" || party.war_pol == "pacifism"))
		{
			socialistPopularity += party.popularity;
		}
		if (party.name.find("socialist") != string::npos && (party.war_pol == "pro_military" || party.war_pol == "jingoism"))
		{
			syndicalistPopularity += party.popularity;
		}
		if (party.name.find("anarcho_liberal") != string::npos)
		{
			ancapPopularity += party.popularity;
		}
		if (party.name.find("reactionary") != string::npos)
		{
			autocraticPopularity += party.popularity;
		}
	}
}

void HoI4Country::setTechnology(string tech, int level)
{
	// don't allow downgrades
	map<string, int>::iterator techEntry = technologies.find(tech);
	if (techEntry == technologies.end() || technologies[tech] < level)
		technologies[tech] = level;
}

void HoI4Country::calculateIndustry()
{
	militaryFactories = 0.0;
	civilianFactories = 0.0;
	dockyards = 0.0;
	for (auto state : states)
	{
		civilianFactories += state.second->getCivFactories();
		militaryFactories += state.second->getMilFactories();
		dockyards += state.second->getDockyards();
	}

	if (RulingPartyModel.war_pol == "jingoism")
	{
		civilianFactories *= 1.1;
	}
	else if (RulingPartyModel.war_pol == "pro_military")
	{
		civilianFactories *= 0.9;
	}
	else
	{
		civilianFactories *= 0.7;
	}
}

void HoI4Country::reportIndustry(ofstream& out)
{
	if (states.size() > 0)
	{
		double actualCivilianFactories;
		if (RulingPartyModel.war_pol == "jingoism")
		{
			actualCivilianFactories = civilianFactories / 1.1;
		}
		else if (RulingPartyModel.war_pol == "pro_military")
		{
			actualCivilianFactories = civilianFactories / 0.9;
		}
		else
		{
			actualCivilianFactories = civilianFactories / 0.7;
		}

		out << tag << ',';
		out << militaryFactories << ',';
		out << actualCivilianFactories << ',';
		out << dockyards << ',';
		out << militaryFactories + actualCivilianFactories + dockyards << '\n';
	}
}

void HoI4Country::addVPsToCapital(int VPs)
{
	auto capital = getCapital();
	if (capital != NULL)
	{
		capital->addVictoryPointValue(VPs);
	}
}

HoI4State* HoI4Country::getCapital(void)
{
	auto capitalItr = states.find(capital);
	if (capitalItr == states.end())
	{
		if (states.size() > 0)
		{
			capitalItr = states.begin();
		}
		else
		{
			return nullptr;
		}
	}

	return capitalItr->second;
}

double HoI4Country::getStrengthOverTime(double years) const
{
	return getMilitaryStrength() + getEconomicStrength(years);
}

double HoI4Country::getMilitaryStrength() const
{
	return armyStrength;
}

double HoI4Country::getEconomicStrength(double years) const
{
	double militarySectorStrength = militaryFactories * 3 * 365 * years;
	double civilianSectorStrength = civilianFactories * 0.469 * 0.5 * 3 * 365 * 0.5* years * years; /*.469 is milfac per year, .5 since half are used by consumer goods*/

	return militarySectorStrength + civilianSectorStrength;
}