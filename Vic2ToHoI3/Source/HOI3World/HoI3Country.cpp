/*Copyright (c) 2015 The Paradox Game Converters Project

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



#include "HoI3Country.h"
#include <fstream>
#include "../Log.h"
#include "../Parsers/Parser.h"
#include "HoI3Leader.h"
#include "HoI3Minister.h"
#include "../V2World/V2Relations.h"
#include "../V2World/V2Party.h"




#define HOI3_IDEOLOGY_UNION(DO) \
	DO(national_socialist) \
	DO(fascistic) \
	DO(paternal_autocrat) \
	DO(social_conservative) \
	DO(market_liberal) \
	DO(social_liberal) \
	DO(social_democrat) \
	DO(left_wing_radical) \
	DO(leninist) \
	DO(stalinist)

#define MAKE_IDEOLOGY_UNION(VAR) VAR,
enum ideaologyType {
	HOI3_IDEOLOGY_UNION(MAKE_IDEOLOGY_UNION)
};

#define MAKE_IDEOLOGY_STRINGS(VAR) #VAR,
const char* const ideologyNames[stalinist + 1] = {
	HOI3_IDEOLOGY_UNION(MAKE_IDEOLOGY_STRINGS)
};


string removeAccented(string str)
 {
	for (unsigned i = 0; i < str.length(); ++i)
	{
		const char*
			//   "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
			tr = "AAAAAAECEEEEIIIIDNOOOOOx0UUUUYPsaaaaaaeceeeeiiiiOnooooo/0uuuuypy";
		unsigned char ch = (str[i]);
		if (ch >= 192) {
			str[i] = tr[ch - 192];
		}
		// http://stackoverflow.com/questions/14094621/
	}
	return str;
}


HoI3Country::HoI3Country(string _tag, string _commonCountryFile, HoI3World* _theWorld, bool _newCountry /* = false */)
{
	theWorld = _theWorld;
	newCountry = _newCountry;

	commonCountryFile = string(removeAccented(_commonCountryFile));

	tag					= _tag;

	provinces.clear();
	technologies.clear();

	capital			= 0;
	ideology			= "";
	government		= "";
	faction			= "";
	factionLeader	= false;

	neutrality		= 50;
	nationalUnity	= 70;

	seaModifier		= 1.0;
	tankModifier	= 1.0;
	airModifier		= 1.0;
	infModifier		= 1.0;

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

	srcCountry	= NULL;
}


void HoI3Country::output() const
{
	// output history file
	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\countries\\" + filename).c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create country history file " << filename;
		exit(-1);
	}

	if (capital > 0)
	{
		fprintf(output, "capital=%d\n", capital);
	}
	if (government != "")
	{
		fprintf(output, "government = %s\n", government.c_str());
	}
	if (ideology != "")
	{
		fprintf(output, "ideology = %s\n", ideology.c_str());
	}
	for (auto ministerItr: rulingMinisters)
	{
		fprintf(output, "%s = %d\n", ministerItr.getFirstJob().c_str(), ministerItr.getID());
	}
	if (faction != "")
	{
		fprintf(output, "join_faction = %s\n", faction.c_str());
	}
	fprintf(output, "alignment = { x = %.2f y = %.2f }\n", alignment.get2DX(), alignment.get2DY());
	fprintf(output, "neutrality = %.2f\n", neutrality);
	fprintf(output, "national_unity = %.2f\n", nationalUnity);

	fprintf(output, "civil_law = %s\n", civil_law.c_str());
	fprintf(output, "conscription_law = %s\n", conscription_law.c_str());
	fprintf(output, "economic_law = %s\n", economic_law.c_str());
	fprintf(output, "education_investment_law = %s\n", educational_investment_law.c_str());
	fprintf(output, "industrial_policy_laws = %s\n", industrial_policy_laws.c_str());
	fprintf(output, "press_laws = %s\n", press_laws.c_str());
	fprintf(output, "training_laws = %s\n", training_laws.c_str());
	outputPracticals(output);
	outputTech(output);
	outputParties(output);

	fprintf(output, "oob = \"%s\"\n", (tag + "_OOB.txt").c_str());
	fclose(output);

	// output OOB file
	outputOOB();

	// output leaders file
	outputLeaders();

	// Output common country file. 
	fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\common\\countries\\" + commonCountryFile).c_str(), "w");
	int red;
	int green;
	int blue;
	color.GetRGB(red, green, blue);
	fprintf(output, "color = { %d %d %d }\n", red, green, blue);
	fprintf(output, "graphical_culture = %s\n", graphicalCulture.c_str());
	fprintf(output, "\n");
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
	fprintf(output, "unit_names = {\n");
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
	fprintf(output, "}\n");
	fprintf(output, "\n");
	fprintf(output, "ministers = {\n");
	for (auto ministerItr: ministers)
	{
		ministerItr.output(output);
	}
	fprintf(output, "}\n");
	fclose(output);

	outputAIScript();
}


void HoI3Country::outputToCommonCountriesFile(FILE* output) const
{
	fprintf(output, "%s = \"countries%s\"\n", tag.c_str(), commonCountryFile.c_str());
}


void HoI3Country::outputPracticals(FILE* output) const
{
	fprintf(output, "\n");
	for (map<string, double>::const_iterator itr = practicals.begin(); itr != practicals.end(); ++itr)
	{
		if (itr->second > 0.0)
		{
			fprintf(output, "%s = %.2f\n", itr->first.c_str(), min(20.0, itr->second));
		}
	}
}


void HoI3Country::outputTech(FILE* output) const
{
	fprintf(output, "\n");
	for (map<string, int>::const_iterator itr = technologies.begin(); itr != technologies.end(); ++itr)
	{
		fprintf(output, "%s = %d\n", itr->first.c_str(), itr->second);
	}
}


void HoI3Country::outputParties(FILE* output) const
{
	fprintf(output, "popularity = {\n");
	for (auto party: parties)
	{
		fprintf(output, "\t%s = %d\n", party.ideology.c_str(), party.popularity);
	}
	fprintf(output, "}\n");
	fprintf(output, "\n");

	fprintf(output, "organization = {\n");
	for (auto party: parties)
	{
		fprintf(output, "\t%s = %d\n", party.ideology.c_str(), party.organization);
	}
	fprintf(output, "}\n");
	fprintf(output, "\n");

	FILE* partyLocalisations;
	fopen_s(&partyLocalisations, ("Output\\" + Configuration::getOutputName() + "\\localisation\\Parties.csv").c_str(), "a");
	for (auto party: parties)
	{
		fprintf(partyLocalisations, "%s;\n", party.localisationString.c_str());
	}
	fclose(partyLocalisations);
}


void HoI3Country::outputLeaders() const
{
	FILE* leadersFile;
	fopen_s(&leadersFile, ("Output\\" + Configuration::getOutputName() + "\\history\\leaders\\" + tag.c_str() + ".txt").c_str(), "w");
	int landLeaders	= 0;
	int seaLeaders		= 0;
	int airLeaders		= 0;
	for (auto leader: leaders)
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


void HoI3Country::outputOOB() const
{
	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\units\\" + tag + "_OOB.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create OOB file " << (tag + "_OOB.txt");
		exit(-1);
	}

	for (std::vector<HoI3RegGroup>::const_iterator armyItr = armies.begin(); armyItr != armies.end(); ++armyItr)
	{
		if (armyItr->getProductionQueue())
		{
			armyItr->outputIntoProductionQueue(output, tag);
		}
		else
		{
			armyItr->output(output);
		}
	}

	fclose(output);
}


void HoI3Country::initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _vic2ideology, vector<string> outputOrder, const CountryMapping& countryMap, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
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
		string filesearch = Configuration::getHoI3Path() + "\\tfh\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename = fileData.name;
		}
		else
		{
			_findclose(fileListing);
			filesearch = Configuration::getHoI3Path() + "\\history\\countries\\" + tag + "*.txt";
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
		government = getGovernmentForCountry(srcCountry, _vic2ideology, governmentMap);
		if (government.empty())
		{
			government = "";
			LOG(LogLevel::Warning) << "No government mapping defined for " << srcGovernment << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	// Political parties
	convertParties(_srcCountry, _srcWorld.getActiveParties(_srcCountry), _srcWorld.getRulingParty(_srcCountry), ideology);
	for (auto partyItr = parties.begin(); partyItr != parties.end(); partyItr++)
	{
		auto oldLocalisation = V2Localisations.GetTextInEachLanguage(partyItr->name);
		partyItr->localisationString = partyItr->ideology + "_" + tag;
		auto localisationItr = oldLocalisation.begin();
		localisationItr++;
		for (; localisationItr != oldLocalisation.end(); localisationItr++)
		{
			partyItr->localisationString += ";" + localisationItr->second;
		}
	}

	// Ministers
	vector<string> firstNames;
	vector<string> lastNames;
	auto namesItr = namesMap.find(srcCountry->getPrimaryCulture());
	if (namesItr != namesMap.end())
	{
		firstNames	= namesItr->second.first;
		lastNames	= namesItr->second.second;
	}
	else
	{
		firstNames.push_back("null");
		lastNames.push_back("null");
	}
	for (unsigned int ideologyIdx = 0; ideologyIdx <= stalinist; ideologyIdx++)
	{
		for (auto job: governmentJobs)
		{
			HoI3Minister newMinister(firstNames, lastNames, ideologyNames[ideologyIdx], job, governmentJobs, portraitMap[graphicalCulture]);
			ministers.push_back(newMinister);

			if (ideologyNames[ideologyIdx] == ideology)
			{
				rulingMinisters.push_back(newMinister);
			}
		}
	}

	// Faction is handled in HoI3World::configureFactions

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

	nationalUnity = 70.0 + (_srcCountry->getRevanchism()/0.05) - (_srcCountry->getWarExhaustion()/2.5);
	
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
	for (auto srcLeader: srcLeaders)
	{
		HoI3Leader newLeader(srcLeader, tag, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}

	Configuration::setLeaderIDForNextCountry();

	// Relations
	map<string, V2Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (map<string, V2Relations*>::iterator itr = srcRelations.begin(); itr != srcRelations.end(); ++itr)
		{
			const std::string& HoI3Tag = countryMap[itr->second->getTag()];
			if (!HoI3Tag.empty())
			{
				HoI3Relations* hoi2r = new HoI3Relations(HoI3Tag, itr->second);
				relations.insert(make_pair(HoI3Tag, hoi2r));
			}
		}
	}

	// Capital
	int oldCapital = srcCountry->getCapital();
	inverseProvinceMapping::iterator itr = inverseProvinceMap.find(oldCapital);
	if (itr != inverseProvinceMap.end())
	{
		capital = itr->second[0];
	}
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void HoI3Country::initFromHistory()
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
		string filesearch = Configuration::getHoI3Path() + "\\tfh\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename			= fileData.name;
			fullFilename = Configuration::getHoI3Path() + "\\tfh\\history\\countries\\" + fileData.name;
		}
		else
		{
			_findclose(fileListing);
			filesearch = Configuration::getHoI3Path() + "\\history\\countries\\" + tag + "*.txt";
			if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
			{
				filename			= fileData.name;
				fullFilename = Configuration::getHoI3Path() + "\\history\\countries\\" + fileData.name;
			}
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

	Object* obj = doParseFile(fullFilename.c_str());
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
		capital = atoi(results[0]->getLeaf().c_str());
	}
}


void HoI3Country::consolidateProvinceItems(inverseProvinceMapping& inverseProvinceMap, double& totalManpower, double& totalLeadership, double& totalIndustry)
{
	bool convertManpower		= (Configuration::getManpowerConversion() != "no");
	bool convertLeadership	= (Configuration::getLeadershipConversion() != "no");
	bool convertIndustry		= (Configuration::getIcConversion() != "no");

	double leftoverManpower		= 0.0;
	double leftoverLeadership	= 0.0;
	double leftoverIndustry		= 0.0;

	V2State capitalState;

	vector<V2State> states = srcCountry->getStates();
	for (auto stateItr: states)
	{
		double stateManpower		= 0.0;
		double stateLeadership	= 0.0;
		double stateIndustry		= 0.0;
		for (auto srcProvinceItr: stateItr.provinces)
		{
			auto itr = inverseProvinceMap.find(srcProvinceItr);
			if (itr != inverseProvinceMap.end())
			{
				for (auto dstProvinceNum: itr->second)
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
		totalManpower		+= stateManpower;
		totalLeadership	+= stateLeadership;
		totalIndustry		+= stateIndustry;

		if (stateItr.provinces.size() > 0)
		{
			auto itr = inverseProvinceMap.find(stateItr.provinces[0]);
			if (itr != inverseProvinceMap.end())
			{
				auto provinceItr = provinces.find(itr->second[0]);
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
				for (auto jtr: stateItr.provinces)
				{
					auto itr = inverseProvinceMap.find(jtr);
					if (itr != inverseProvinceMap.end())
					{
						for (auto ktr: itr->second)
						{
							auto provinceItr = provinces.find(ktr);
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
		auto provinceItr = provinces.find(capital);
		if (provinceItr == provinces.end())
		{
			provinceItr = provinces.begin();
		}

		if (convertManpower)
		{
			leftoverManpower += provinceItr->second->getManpower();
			provinceItr->second->setManpower(leftoverManpower);
		}

		if (convertLeadership)
		{
			leftoverLeadership += provinceItr->second->getLeadership();
			provinceItr->second->setLeadership(leftoverLeadership);
		}

		if (convertIndustry)
		{
			leftoverIndustry += provinceItr->second->getActualIndustry();
			int intIndustry = static_cast<int>(leftoverIndustry + 0.5);
			if (intIndustry > 10)
			{
				intIndustry = 10;
			}
			provinceItr->second->setActualIndustry(intIndustry);
			leftoverIndustry -= intIndustry;
			for (auto jtr: capitalState.provinces)
			{
				auto itr = inverseProvinceMap.find(jtr);
				if (itr != inverseProvinceMap.end())
				{
					for (auto ktr: itr->second)
					{
						auto provinceItr = provinces.find(ktr);
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


void HoI3Country::generateLeaders(leaderTraitsMap leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap)
{
	vector<string> firstNames;
	vector<string> lastNames;
	auto namesItr = namesMap.find(srcCountry->getPrimaryCulture());
	if (namesItr != namesMap.end())
	{
		firstNames	= namesItr->second.first;
		lastNames	= namesItr->second.second;
	}
	else
	{
		firstNames.push_back("null");
		lastNames.push_back("null");
	}

	// generated leaders
	int totalOfficers = 0;
	vector<V2Province*> srcProvinces = srcCountry->getCores();
	for (auto province: srcProvinces)
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
		HoI3Leader newLeader(firstNames, lastNames, tag, "land", leaderTraits, portraitMap[graphicalCulture]);
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
		HoI3Leader newLeader(firstNames, lastNames, tag, "sea", leaderTraits, portraitMap[graphicalCulture]);
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
		HoI3Leader newLeader(firstNames, lastNames, tag, "air", leaderTraits, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}
}


void HoI3Country::setAIFocuses(const AIFocusModifiers& focusModifiers)
{
	for (auto currentModifier: focusModifiers)
	{
		double modifierAmount = 1.0;
		for (auto modifier: currentModifier.second)
		{
			bool modifierActive = false;
			if (modifier.modifierType == "lacks_port")
			{
				modifierActive = true;
				for (auto province: provinces)
				{
					if (province.second->hasNavalBase())
					{
						modifierActive = false;
					}
				}
			}
			else if (modifier.modifierType == "tech_school")
			{
				if (modifier.modifierRequirement == srcCountry->getTechSchool())
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == "coast_border_percent")
			{
				int navalBases = 0;
				for (auto province: provinces)
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



void HoI3Country::addProvince(HoI3Province* _province)
{
	provinces.insert(make_pair(_province->getNum(), _province));
	if (capital == 0)
	{
		capital = _province->getNum();
	}
}


HoI3Relations* HoI3Country::getRelations(string withWhom) const
{
	map<string, HoI3Relations*>::const_iterator i = relations.find(withWhom);
	if (i != relations.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}

vector<int> HoI3Country::getPortProvinces(vector<int> locationCandidates, map<int, HoI3Province*> allProvinces)
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

	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		vector<int>::iterator black = std::find(port_blacklist.begin(), port_blacklist.end(), *litr);
		if (black != port_blacklist.end())
		{
			locationCandidates.erase(litr);
			break;
		}
	}
	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		map<int, HoI3Province*>::iterator pitr = allProvinces.find(*litr);
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


void HoI3Country::convertParties(const V2Country* srcCountry, vector<V2Party*> V2Parties, V2Party* rulingParty, string& rulingIdeology)
{
	// 
	// sort Vic2 parties by ideology
	map<string, vector<V2Party*>> V2Ideologies;
	for (auto partyItr: V2Parties)
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

	// prep unmapped HoI3 parties
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
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI3Party newParty;
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
		HoI3Party newParty;
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
		HoI3Party newParty;
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
		HoI3Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "left_wing_radical";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("socialist") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "left_wing_radical";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("left_wing_radical");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("communist");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI3Party newParty;
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
	ideologyItr = V2Ideologies.find("liberal");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "social_liberal";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("liberal") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "social_liberal";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("social_liberal");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find("anarcho_liberal");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= "market_liberal";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("anarcho_liberal") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = "market_liberal";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find("market_liberal");
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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

	map<string, vector<string>> HoI3IdeologyGroups;
	for (auto HoI3PartyItr: unmappedParties)
	{
		auto groupItr = HoI3IdeologyGroups.find(HoI3PartyItr.second);
		if (groupItr != HoI3IdeologyGroups.end())
		{
			groupItr->second.push_back(HoI3PartyItr.first);
		}
		else
		{
			vector<string> parties;
			parties.push_back(HoI3PartyItr.first);
			HoI3IdeologyGroups.insert(make_pair(HoI3PartyItr.second, parties));
		}
	}

	for (auto V2GroupItr: V2IdeologyGroups)
	{
		auto HoI3GroupItr = HoI3IdeologyGroups.find(V2GroupItr.first);
		if ((HoI3GroupItr != HoI3IdeologyGroups.end()) && (V2GroupItr.second.size() <= HoI3GroupItr->second.size()))
		{
			for (auto V2PartyItr: V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);

				HoI3Party newParty;
				newParty.name				= V2PartyItr->name;
				newParty.ideology			= HoI3GroupItr->second[0];
				newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(ideologyItr->first) * 100 / ideologyItr->second.size() + 0.5);
				newParty.organization	= newParty.popularity;
				parties.push_back(newParty);

				HoI3GroupItr->second.erase(HoI3GroupItr->second.begin());

				auto itr = unmappedParties.find(newParty.ideology);
				unmappedParties.erase(itr);

				if (rulingParty->name == V2PartyItr->name)
				{
					rulingIdeology = HoI3GroupItr->second[0];
				}
			}
			for (auto V2PartyItr: V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);
				if (ideologyItr != V2Ideologies.end())
				{
					V2Ideologies.erase(ideologyItr);
				}
			}
			V2GroupItr.second.clear();
		}
		if (HoI3GroupItr->second.size() == 0)
		{
			HoI3IdeologyGroups.erase(HoI3GroupItr);
		}
	}

	if (V2Ideologies.size() == 0)
	{
		return;
	}

	// merge Vic2 parties by ideology, then map those cases
	for (map<string, vector<V2Party*>>::iterator ideologyItr = V2Ideologies.begin(); ideologyItr != V2Ideologies.end(); ideologyItr++)
	{
		while (ideologyItr->second.size() > 1)
		{
			ideologyItr->second.pop_back();
		}
	}

	V2IdeologyGroups.clear();
	ideologyItr = V2Ideologies.find("fascist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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
		for (auto partyItr: ideologyItr->second)
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

	for (auto V2GroupItr: V2IdeologyGroups)
	{
		auto HoI3GroupItr = HoI3IdeologyGroups.find(V2GroupItr.first);
		if ((HoI3GroupItr != HoI3IdeologyGroups.end()) && (V2GroupItr.second.size() <= HoI3GroupItr->second.size()))
		{
			for (auto V2PartyItr: V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);

				HoI3Party newParty;
				newParty.name				= V2PartyItr->name;
				newParty.ideology			= HoI3GroupItr->second[0];
				newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(ideologyItr->first) * 100 / ideologyItr->second.size() + 0.5);
				newParty.organization	= newParty.popularity;
				parties.push_back(newParty);

				HoI3GroupItr->second.erase(HoI3GroupItr->second.begin());

				auto itr = unmappedParties.find(newParty.ideology);
				unmappedParties.erase(itr);

				if (rulingParty->ideology == V2PartyItr->ideology)
				{
					rulingIdeology = HoI3GroupItr->second[0];
				}
			}
			for (auto V2PartyItr: V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);
				if (ideologyItr != V2Ideologies.end())
				{
					V2Ideologies.erase(ideologyItr);
				}
			}
			V2GroupItr.second.clear();
		}
		if (HoI3GroupItr->second.size() == 0)
		{
			HoI3IdeologyGroups.erase(HoI3GroupItr);
		}
	}

	if (V2Ideologies.size() > 0)
	{
		LOG(LogLevel::Warning) << "Unmapped Vic2 parties for " << tag;
	}
}


void HoI3Country::outputLocalisation(FILE* output) const
{
	std::ostringstream localisationStream;
	localisation.WriteToStream(localisationStream);
	std::string localisationString = localisationStream.str();
	fwrite(localisationString.c_str(), sizeof(std::string::value_type), localisationString.size(), output);
}


void HoI3Country::outputAIScript() const
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


void HoI3Country::setTechnology(string tech, int level)
{
	// don't allow downgrades
	map<string, int>::iterator techEntry = technologies.find(tech);
	if (techEntry == technologies.end() || technologies[tech] < level)
		technologies[tech] = level;
}


void HoI3Country::addArmy(HoI3RegGroup _army)
{
	armies.push_back(_army);
}


HoI3Province* HoI3Country::getCapital(void)
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