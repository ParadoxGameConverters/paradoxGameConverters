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



#include "HoI4Country.h"
#include <fstream>
#include "Log.h"
#include "ParadoxParserUTF8.h"
#include "HoI4Leader.h"
#include "HoI4Minister.h"
#include "../V2World/V2Relations.h"
#include "../V2World/V2Party.h"
#include "../../../common_items/WinUtils.h"



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
	commonCountryFile = _commonCountryFile;

	tag = _tag;

	provinces.clear();
	technologies.clear();

	capital			= 0;
	ideology			= "";
	government		= "";
	faction			= "";
	factionLeader	= false;

	neutrality = 50;
	nationalUnity = 70;

	seaModifier = 1.0;
	tankModifier = 1.0;
	airModifier = 1.0;
	infModifier = 1.0;

	training_laws					= "minimal_training";
	press_laws						= "censored_press";
	industrial_policy_laws		= "consumer_product_orientation";
	educational_investment_law = "minimal_education_investment";
	economic_law					= "full_civilian_economy";
	conscription_law				= "volunteer_army";
	civil_law						= "limited_restrictions";

	relations.clear();
	armies.clear();
	allies.clear();
	practicals.clear();
	parties.clear();
	ministers.clear();
	rulingMinisters.clear();


	graphicalCulture = "Generic";

	srcCountry = NULL;

	majorNation = false;
}


void HoI4Country::output(int statenumber) const
{
	// output history file
	ofstream output;
	string con = tag;
	if (capital > 0 && capital <= statenumber)
	{
		output.open(("Output/" + Configuration::getOutputName() + "/history/countries/" + WinUtils::convertToASCII(filename)).c_str());
		if (!output.is_open())
		{
			Log(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/common/history/" << filename;
			exit(-1);
		}
		output << "\xEF\xBB\xBF";    // add the BOM to make HoI4 happy

		output << "capital = " << capital << endl;

		output << "" << endl;
		output << "oob = \"" << tag << "_OOB\"" << endl;
		output << "" << endl;
		output << "# Starting tech" << endl;
		output << "set_technology = {" << endl;
		for (auto tech : technologies)
		{
			output << tech.first<< " = 1" << endl;
		}
		output << "}" << endl;
		output << "" << endl;
		output << "1939.1.1 = {" << endl;
		output << "" << endl;
		output << "    " << endl;
		output << "}" << endl;
		output << "" << endl;
		output << "set_politics = {" << endl;
		output << "" << endl;
		output << "    parties = {" << endl;
		output << "        democratic = { " << endl;
		output << "            popularity = 0" << endl;
		output << "        }" << endl;
		output << "" << endl;
		output << "        fascism = {" << endl;
		output << "            popularity = 0" << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        communism = {" << endl;
		output << "            popularity = 0" << endl;
		output << "        }" << endl;
		output << "        " << endl;
		output << "        neutrality = { " << endl;
		output << "            popularity = 100" << endl;
		output << "        }" << endl;
		output << "    }" << endl;
		output << "    " << endl;
		output << "    ruling_party = neutrality" << endl;
		output << "    last_election = \"1936.1.1\"" << endl;
		output << "    election_frequency = 48" << endl;
		output << "    elections_allowed = no" << endl;
		output << "}" << endl;
		output << "" << endl;
		output << "create_country_leader = {" << endl;
		output << "    name = \"Jigme Wangchuck\"" << endl;
		output << "    desc = \"POLITICS_JIGME_WANGCHUCK_DESC\"" << endl;
		output << "    picture = \"gfx / leaders / Asia / Portrait_Asia_Generic_2.dds\"" << endl;
		output << "    expire = \"1965.1.1\"" << endl;
		output << "    ideology = despotism" << endl;
		output << "    traits = {" << endl;
		output << "        #" << endl;
		output << "    }" << endl;
		output << "}" << endl;
		output << "" << endl;
		output << "1939.1.1 = {" << endl;
		output << "    " << endl;
		output << "}" << endl;
		output.close();
	}

	//// output OOB file
	//outputOOB();

	//// output leaders file
	//outputLeaders();

	// Output common country file
	output.open(("Output\\" + Configuration::getOutputName() + "\\common\\countries\\" + WinUtils::convertToASCII(commonCountryFile)).c_str());
	if (!output.is_open())
	{
		Log(LogLevel::Error) << "Could not open " << "Output\\" << Configuration::getOutputName() << "\\common\\countries\\" << commonCountryFile;
		exit(-1);
	}
	int red;
	int green;
	int blue;
	color.GetRGB(red, green, blue);
	output << "color = { " << red << " " << green << " " << blue << "}" << endl;
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
	output.close();

	//outputAIScript();
}


void HoI4Country::outputToCommonCountriesFile(FILE* output) const
{
	//removes countries with 0 capital, sorry that its in here and not in HoI4World.cpp :(
	if (capital != 0)
	{
		fprintf(output, "%s = \"countries%s\"\n", tag.c_str(), WinUtils::convertToASCII(commonCountryFile).c_str());
	}
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
	fprintf(output, "popularity = {\n");
	for (auto party : parties)
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
	if (fopen_s(&partyLocalisations, ("Output\\" + Configuration::getOutputName() + "\\localisation\\Parties.csv").c_str(), "a") != 0)
	{
		LOG(LogLevel::Error) << "Could not open " << "Output\\" << Configuration::getOutputName() << "\\localisation\\Parties.csv";
		exit(-1);
	}
	for (auto party : parties)
	{
		fprintf(partyLocalisations, "%s;\n", party.localisationString.c_str());
	}
	fclose(partyLocalisations);
}


void HoI4Country::outputLeaders() const
{
	FILE* leadersFile;
	if (fopen_s(&leadersFile, ("Output\\" + Configuration::getOutputName() + "\\history\\leaders\\" + tag.c_str() + ".txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not open " << "Output\\" << Configuration::getOutputName() << "\\history\\leaders\\" << tag.c_str() << ".txt";
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


void HoI4Country::outputOOB() const
{
	ofstream output(("Output/" + Configuration::getOutputName() + "/history/units/" + tag + "_1936.txt").c_str());
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
	output << "division_template = {\n";
	output << "\tname = \"Infantry Division\"        # Bhutanese Army never expanded past battalion size (in 1943)\n";
	output << "\t\n";
	output << "\tregiments = {\n";
	output << "\t\tinfantry = { x = 0 y = 0 }\n";
	output << "\t\tinfantry = { x = 0 y = 1 }\n";
	output << "\t}\n";
	output << "}\n";
	output << "\n";
	output << "\n";
	output << "units = {\n";
	output << "\t##### No standing army #####\n";
	output << "}\n";
	output << "\n";
	output << "\n";
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

	output.close();
}


void HoI4Country::initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _vic2ideology, const CountryMapping& countryMap, inverseProvinceMapping inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const HoI4StateMapping& stateMap, map<int, HoI4State*> states)
{
	srcCountry = _srcCountry;

	struct _finddata_t	fileData;
	intptr_t					fileListing;
	string filesearch = ".\\blankMod\\output\\history\\countries\\" + tag + "*.txt";
	if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename = fileData.name;
	}
	_findclose(fileListing);
	if (filename == "")
	{
		string filesearch = Configuration::getHoI4Path() + "\\tfh\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename = fileData.name;
		}
		else
		{
			_findclose(fileListing);
			filesearch = Configuration::getHoI4Path() + "\\history\\countries\\" + tag + "*.txt";
			if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
			{
				filename = fileData.name;
			}
		}
		_findclose(fileListing);
	}
	if (filename == "")
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
	}

	// Color
	color = srcCountry->getColor();

	// Localisation
	localisation.SetTag(tag);
	localisation.ReadFromCountry(*srcCountry);

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

	// Political parties
	convertParties(_srcCountry, _srcWorld.getActiveParties(_srcCountry), _srcWorld.getRulingParty(_srcCountry), ideology);
	for (auto partyItr : parties)
	{
		auto oldLocalisation = V2Localisations.GetTextInEachLanguage(partyItr.name);
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

	string warPolicy = _srcWorld.getRulingParty(_srcCountry)->war_policy;
	if (warPolicy == "jingoism")
	{
		neutrality = 60;
	}
	else if (warPolicy == "pro_military")
	{
		neutrality = 73.3;
	}
	else if (warPolicy == "anti_military")
	{
		neutrality = 86.6;
	}
	else if (warPolicy == "pacifism")
	{
		neutrality = 90;
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not find war policy for Vic2 country " << _srcCountry->getTag() << ". Settting neutrality to 100%";
		neutrality = 100;
	}

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
			const std::string& HoI4Tag = countryMap[itr.second->getTag()];
			if (!HoI4Tag.empty())
			{
				HoI4Relations* hoi2r = new HoI4Relations(HoI4Tag, itr.second);
				relations.insert(make_pair(HoI4Tag, hoi2r));
			}
		}
	}
	//GETCAPITALHERE
	// Capital
	int oldCapital = srcCountry->getCapital();
	inverseProvinceMapping::iterator itr = inverseProvinceMap.find(oldCapital);
	if (itr != inverseProvinceMap.end())
	{
		auto capitalState = stateMap.find(itr->second[0]);
		if (capitalState != stateMap.end() && (states.find(capitalState->second)->second->getOwner() == tag))
		{
			capital = capitalState->second;
		}
	}

	// major nation
	majorNation = srcCountry->getGreatNation();
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void HoI4Country::initFromHistory()
{
	string fullFilename;
	struct _finddata_t	fileData;
	intptr_t					fileListing;
	string filesearch = ".\\blankMod\\output\\history\\countries\\" + tag + "*.txt";
	if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename			= fileData.name;
		fullFilename	= string(".\\blankMod\\output\\history\\countries\\") + fileData.name;
	}
	_findclose(fileListing);
	if (fullFilename == "")
	{
		filesearch = Configuration::getHoI4Path() + "\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename = fileData.name;
			fullFilename = Configuration::getHoI4Path() + "\\history\\countries\\" + fileData.name;
		}
		_findclose(fileListing);
	}
	if (fullFilename == "")
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
		return;
	}

	Object* obj = parser_UTF8::doParseFile(fullFilename.c_str());
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

	results = obj->getValue("capita");
	if (results.size() > 0)
	{
		capital = atoi(results[0]->getLeaf().c_str());
	}
}


void HoI4Country::consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap, double& totalManpower, double& totalLeadership, double& totalIndustry)
{
	bool convertManpower		= (Configuration::getManpowerConversion() != "no");
	bool convertLeadership	= (Configuration::getLeadershipConversion() != "no");
	bool convertIndustry		= (Configuration::getIcConversion() != "no");

	double leftoverManpower = 0.0;
	double leftoverLeadership = 0.0;
	double leftoverIndustry = 0.0;

	Vic2State capitalState;

	vector<Vic2State> states = srcCountry->getStates();
	for (auto stateItr : states)
	{
		double stateManpower = 0.0;
		double stateLeadership = 0.0;
		double stateIndustry = 0.0;
		for (auto srcProvinceItr : stateItr.getProvinces())
		{
			auto possibleHoI4Provinces = inverseProvinceMap.find(srcProvinceItr);
			if (possibleHoI4Provinces != inverseProvinceMap.end())
			{
				for (auto dstProvinceNum : possibleHoI4Provinces->second)
				{
					auto provinceItr = provinces.find(dstProvinceNum);
					if (provinceItr != provinces.end())
					{
						if (provinceItr->first == capital)
						{
							capitalState = stateItr;
						}

						if (convertManpower)
						{
							stateManpower += provinceItr->second->getManpower();
							provinceItr->second->setManpower(0.0);
						}

						if (convertLeadership)
						{
							stateLeadership += provinceItr->second->getLeadership();
							provinceItr->second->setLeadership(0.0);
						}

						if (convertIndustry)
						{
							stateIndustry += provinceItr->second->getRawIndustry();
							provinceItr->second->setRawIndustry(0.0);
							provinceItr->second->setActualIndustry(0);
						}
					}
				}
			}
		}
		totalManpower += stateManpower;
		totalLeadership += stateLeadership;
		totalIndustry += stateIndustry;

		if (stateItr.getProvinces().size() > 0)
		{
			auto possibleHoI4Provinces = inverseProvinceMap.find(stateItr.getProvinces()[0]);
			if (possibleHoI4Provinces != inverseProvinceMap.end())
			{
				auto provinceItr = provinces.find(possibleHoI4Provinces->second[0]);
				if (provinceItr != provinces.end())
				{
					if (convertManpower)
					{
						int intManpower = static_cast<int>(stateManpower * 4);
						double discreteManpower = intManpower / 4.0;
						provinceItr->second->setManpower(discreteManpower);
						leftoverManpower += (stateManpower - discreteManpower);
					}

					if (convertLeadership)
					{
						int intLeadership = static_cast<int>(stateLeadership * 20);
						double discreteLeadership = intLeadership / 20.0;
						provinceItr->second->setLeadership(discreteLeadership);
						leftoverLeadership += (stateLeadership - discreteLeadership);
					}
				}
			}
			if (convertIndustry)
			{
				for (auto vic2ProvNum : stateItr.getProvinces())
				{
					auto possibleHoI4Provinces = inverseProvinceMap.find(vic2ProvNum);
					if (possibleHoI4Provinces != inverseProvinceMap.end())
					{
						for (auto HoI4ProvNum : possibleHoI4Provinces->second)
						{
							auto provinceItr = provinces.find(HoI4ProvNum);
							if (provinceItr != provinces.end())
							{
								int intIndustry = static_cast<int>(stateIndustry + 0.5);
								if (intIndustry > 10)
								{
									intIndustry = 10;
								}
								provinceItr->second->setActualIndustry(intIndustry);
								stateIndustry -= intIndustry;
							}
						}
					}
				}
				leftoverIndustry += stateIndustry;
			}
		}
	}

	if (provinces.size() > 0)
	{
		auto capitalItr = provinces.find(capital);
		if (capitalItr == provinces.end())
		{
			capitalItr = provinces.begin();
		}

		if (convertManpower)
		{
			leftoverManpower += capitalItr->second->getManpower();
			capitalItr->second->setManpower(leftoverManpower);
		}

		if (convertLeadership)
		{
			leftoverLeadership += capitalItr->second->getLeadership();
			capitalItr->second->setLeadership(leftoverLeadership);
		}

		if (convertIndustry)
		{
			leftoverIndustry += capitalItr->second->getActualIndustry();
			int intIndustry = static_cast<int>(leftoverIndustry + 0.5);
			if (intIndustry > 10)
			{
				intIndustry = 10;
			}
			if (intIndustry < 5)
			{
				intIndustry = 5;
			}
			capitalItr->second->setActualIndustry(intIndustry);
			leftoverIndustry -= intIndustry;
			for (auto vic2ProvinceNum : capitalState.getProvinces())
			{
				auto possibleHoI4Provinces = inverseProvinceMap.find(vic2ProvinceNum);
				if (possibleHoI4Provinces != inverseProvinceMap.end())
				{
					for (auto HoI4ProvinceNum : possibleHoI4Provinces->second)
					{
						if (HoI4ProvinceNum == capitalItr->first)
						{
							continue;
						}
						auto provinceItr = provinces.find(HoI4ProvinceNum);
						if (provinceItr != provinces.end())
						{
							int intIndustry = static_cast<int>(leftoverIndustry + 0.5);
							if (intIndustry > 10)
							{
								intIndustry = 10;
							}
							provinceItr->second->setActualIndustry(intIndustry);
							leftoverIndustry -= intIndustry;
						}
					}
				}
			}
			if (leftoverIndustry > 0.5)
			{
				LOG(LogLevel::Warning) << "Leftover IC is " << leftoverIndustry;
			}
		}
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
	vector<V2Province*> srcProvinces = srcCountry->getCores();
	for (auto province : srcProvinces)
	{
		totalOfficers += province->getPopulation("officers");
	}

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


void HoI4Country::setAIFocuses(const AIFocusModifiers& focusModifiers)
{
	for (auto currentModifier : focusModifiers)
	{
		double modifierAmount = 1.0;
		for (auto modifier : currentModifier.second)
		{
			bool modifierActive = false;
			if (modifier.modifierType == "lacks_port")
			{
				modifierActive = true;
				for (auto province : provinces)
				{
					if (province.second->hasNavalBase())
					{
						modifierActive = false;
					}
				}
			}
			else if (modifier.modifierType == "tech_schoo")
			{
				if (modifier.modifierRequirement == srcCountry->getTechSchool())
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "coast_border_percent")
			{
				int navalBases = 0;
				for (auto province : provinces)
				{
					if (province.second->hasNavalBase())
					{
						navalBases++;
					}
				}
				if ((1.0 * navalBases / provinces.size()) >= atof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "navy_tech_ahead")
			{
				if ((srcCountry->getNumNavyTechs() - srcCountry->getNumArmyTechs()) >= atoi(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "capital_continent")
			{
				if (srcCountry->getCapitalContinent() == modifier.modifierRequirement)
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "ship_composition_percent")
			{
				int totalUnits = 0;
				int numSeaUnits = 0;
				for (auto army : srcCountry->getArmies())
				{
					for (auto regiment : army->getRegiments())
					{
						totalUnits++;
						string type = regiment->getType();
						if (
								(type == "artillery") || (type == "cavalry") || (type == "cuirassier") || (type == "dragoon") || (type == "engineer") || 
								(type == "guard") || (type == "hussar") || (type == "infantry") || (type == "irregular") || (type == "plane") || (type == "tank")
							)
						{
							numSeaUnits++;
						}
					}
				}
				if ((1.0 * numSeaUnits / totalUnits) > atof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "tank_composition_percent")
			{
				int totalUnits = 0;
				int numTanks = 0;
				for (auto army : srcCountry->getArmies())
				{
					for (auto regiment : army->getRegiments())
					{
						totalUnits++;
						string type = regiment->getType();
						if (type == "tank")
						{
							numTanks++;
						}
					}
				}
				if ((1.0 * numTanks / totalUnits) > atof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "plane_composition_percent")
			{
				int totalUnits = 0;
				int numPlanes = 0;
				for (auto army : srcCountry->getArmies())
				{
					for (auto regiment : army->getRegiments())
					{
						totalUnits++;
						string type = regiment->getType();
						if (type == "plane")
						{
							numPlanes++;
						}
					}
				}
				if ((1.0 * numPlanes / totalUnits) > atof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "manpower_to_IC_ratio")
			{
				double totalManpower = 0.0;
				double totalIC = 0.0;
				for (auto province : provinces)
				{
					totalManpower += province.second->getManpower();
					totalIC += province.second->getActualIndustry();
				}
				if ((totalManpower / totalIC) > atof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "default")
			{
				modifierActive = true;
			}

			if (modifierActive)
			{
				modifierAmount *= modifier.modifierAmount;
			}
		}

		if (currentModifier.first == SEA_FOCUS)
		{
			seaModifier = modifierAmount;
		}
		else if (currentModifier.first == TANK_FOCUS)
		{
			tankModifier = modifierAmount;
		}
		else if (currentModifier.first == AIR_FOCUS)
		{
			airModifier = modifierAmount;
		}
		else if (currentModifier.first == INF_FOCUS)
		{
			infModifier = modifierAmount;
		}
	}
}


void HoI4Country::addMinimalItems(const inverseProvinceMapping& inverseProvinceMap)
{
	if (provinces.size() == 0)
	{
		return;
	}

	// determine if there's anything to add
	bool hasPort = false;
	for (auto province : provinces)
	{
		if (province.second->getNavalBase() > 0)
		{
			hasPort = true;
		}
	}

	auto capitalItr = provinces.find(capital);
	if (capitalItr == provinces.end())
	{
		capitalItr = provinces.begin();
	}

	// if necessary, add an airbase to the capital province
	capitalItr->second->requireAirBase(10);

	// if necessary, add a port as near to the capital as possible
	//		impossible currently, as we don't have a way to know where ports are valid

	for (auto state : srcCountry->getStates())
	{
		if (state.getProvinces().size() > 0)
		{
			auto possibleHoI4Provinces = inverseProvinceMap.find(state.getProvinces()[0]);
			if (possibleHoI4Provinces != inverseProvinceMap.end())
			{
				if (possibleHoI4Provinces->second.size() > 0)
				{
					auto provinceItr = provinces.find(possibleHoI4Provinces->second[0]);
					if (provinceItr != provinces.end())
					{
						provinceItr->second->requireAirBase(2);
					}
				}
			}
		}
	}
}


void HoI4Country::addProvince(HoI4Province* _province)
{
	provinces.insert(make_pair(_province->getNum(), _province));
	if (capital == 0)
	{
		capital = _province->getNum();
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
	unmappedParties.insert(make_pair("market_libera", "democratic_group"));
	unmappedParties.insert(make_pair("social_libera", "democratic_group"));
	unmappedParties.insert(make_pair("social_democrat", "democratic_group"));
	unmappedParties.insert(make_pair("left_wing_radica", "communist_group"));
	unmappedParties.insert(make_pair("leninist", "communist_group"));
	unmappedParties.insert(make_pair("stalinist", "communist_group"));

	// map all the simplistic cases
	auto ideologyItr = V2Ideologies.find("fascist");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "fascistic";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("fascist") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "fascistic";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("fascistic");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("reactionary");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "paternal_autocrat";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("reactionary") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "paternal_autocrat";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("paternal_autocrat");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("conservative");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "social_conservative";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("conservative") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "social_conservative";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("social_conservative");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("socialist");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "left_wing_radica";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("socialist") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "left_wing_radica";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("left_wing_radica");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("communist");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "stalinist";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("communist") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "stalinist";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("stalinist");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("libera");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "social_libera";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("libera") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "social_libera";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("social_libera");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("anarcho_libera");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "market_libera";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("anarcho_libera") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "market_libera";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("market_libera");
		unmappedParties.erase(itr);
	}

	if (V2Ideologies.size() == 0)
	{
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
	ideologyItr = V2Ideologies.find("libera");
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
	ideologyItr = V2Ideologies.find("anarcho_libera");
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
	ideologyItr = V2Ideologies.find("libera");
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
	ideologyItr = V2Ideologies.find("anarcho_libera");
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

	if (V2Ideologies.size() > 0)
	{
		LOG(LogLevel::Warning) << "Unmapped Vic2 parties for " << tag;
	}
}


void HoI4Country::outputLocalisation(FILE* output) const
{
	std::ostringstream localisationStream;
	localisation.WriteToStream(localisationStream);
	std::string localisationString = localisationStream.str();
	fwrite(localisationString.c_str(), sizeof(std::string::value_type), localisationString.size(), output);
}


void HoI4Country::outputAIScript() const
{
	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\script\\country\\" + tag + ".lua").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create country script file for " << tag;
		exit(-1);
	}

	fprintf(output, "local P = {}\n");
	fprintf(output, "AI_%s = P\n", tag.c_str());

	ifstream sourceFile;
	LOG(LogLevel::Debug) << tag << ": air modifier - " << airModifier << ", tankModifier - " << tankModifier << ", seaModifier - " << seaModifier << ", infModifier - " << infModifier;
	if ((airModifier > seaModifier) && (airModifier > tankModifier) && (airModifier > infModifier)) // air template
	{
		sourceFile.open("airTEMPLATE.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open airTEMPLATE.lua";
			exit(-1);
		}
	}
	else if ((seaModifier > tankModifier) && (seaModifier > infModifier))	// sea template
	{
		sourceFile.open("shipTemplate.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open shipTemplate.lua";
			exit(-1);
		}
	}
	else if (tankModifier > infModifier) // tank template
	{
		sourceFile.open("tankTemplate.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open tankTemplate.lua";
			exit(-1);
		}
	}
	else	// infantry template
	{
		sourceFile.open("infatryTEMPLATE.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open infatryTEMPLATE.lua";
			exit(-1);
		}
	}

	while (!sourceFile.eof())
	{
		string line;
		getline(sourceFile, line);
		fprintf(output, "%s\n", line.c_str());
	}
	sourceFile.close();

	fprintf(output, "return AI_%s\n", tag.c_str());

	fclose(output);
}


void HoI4Country::setTechnology(string tech, int level)
{
	// don't allow downgrades
	map<string, int>::iterator techEntry = technologies.find(tech);
	if (techEntry == technologies.end() || technologies[tech] < level)
		technologies[tech] = level;
}


void HoI4Country::addArmy(HoI4RegGroup* _army)
{
	armies.push_back(_army);
}


void HoI4Country::lowerNeutrality(double amount)
{
	neutrality -= amount;
	if (neutrality < 0)
	{
		neutrality = 0.0;
	}
}


HoI4Province* HoI4Country::getCapital(void)
{
	auto capitalItr = provinces.find(capital);
	if (capitalItr == provinces.end())
	{
		if (provinces.size() > 0)
		{
			capitalItr = provinces.begin();
		}
		else
		{
			return NULL;
		}
	}

	return capitalItr->second;
}