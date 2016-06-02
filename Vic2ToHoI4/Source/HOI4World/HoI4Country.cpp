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



enum ideaologyType {
	national_socialist	= 0,
	fascistic				= 1,
	paternal_autocrat		= 2,
	social_conservative	= 3,
	market_liberal			= 4,
	social_liberal			= 5,
	social_democrat		= 6,
	left_wing_radical		= 7,
	leninist					= 8,
	stalinist				= 9
};


const wchar_t* const ideologyNames[stalinist + 1] = {
	L"national_socialist",
	L"fascistic",
	L"paternal_autocrat",
	L"social_conservative",
	L"market_liberal",
	L"social_liberal",
	L"social_democrat",
	L"left_wing_radical",
	L"leninist",
	L"stalinist"
};



HoI4Country::HoI4Country(wstring _tag, wstring _commonCountryFile, HoI4World* _theWorld, bool _newCountry /* = false */)
{
	theWorld = _theWorld;
	newCountry = _newCountry;

	commonCountryFile = _commonCountryFile;

	tag					= _tag;

	provinces.clear();
	technologies.clear();

	capital			= 0;
	ideology			= L"";
	government		= L"";
	faction			= L"";
	factionLeader	= false;

	neutrality		= 50;
	nationalUnity	= 70;

	seaModifier		= 1.0;
	tankModifier	= 1.0;
	airModifier		= 1.0;
	infModifier		= 1.0;

	training_laws					= L"minimal_training";
	press_laws						= L"censored_press";
	industrial_policy_laws		= L"consumer_product_orientation";
	educational_investment_law = L"minimal_education_investment";
	economic_law					= L"full_civilian_economy";
	conscription_law				= L"volunteer_army";
	civil_law						= L"limited_restrictions";

	relations.clear();
	armies.clear();
	allies.clear();
	practicals.clear();
	parties.clear();
	ministers.clear();
	rulingMinisters.clear();


	graphicalCulture = L"Generic";

	srcCountry	= NULL;

	majorNation = false;
}


void HoI4Country::output() const
{
	// output history file
	FILE* output;
	if (_wfopen_s(&output, (L"Output\\" + Configuration::getOutputName() + L"\\history\\countries\\" + filename).c_str(), L"w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create country history file " << filename;
		exit(-1);
	}

	if (capital > 0)
	{
		fwprintf(output, L"capital=%d\n", capital);
	}
	if (government != L"")
	{
		fwprintf(output, L"government = %s\n", government.c_str());
	}
	if (ideology != L"")
	{
		fwprintf(output, L"ideology = %s\n", ideology.c_str());
	}
	for (auto ministerItr: rulingMinisters)
	{
		fwprintf(output, L"%s = %d\n", ministerItr.getFirstJob().c_str(), ministerItr.getID());
	}
	if (faction != L"")
	{
		fwprintf(output, L"join_faction = %s\n", faction.c_str());
	}
	fwprintf(output, L"alignment = { x = %.2f y = %.2f }\n", alignment.get2DX(), alignment.get2DY());
	fwprintf(output, L"neutrality = %.2f\n", neutrality);
	fwprintf(output, L"national_unity = %.2f\n", nationalUnity);

	fwprintf(output, L"civil_law = %s\n", civil_law.c_str());
	fwprintf(output, L"conscription_law = %s\n", conscription_law.c_str());
	fwprintf(output, L"economic_law = %s\n", economic_law.c_str());
	fwprintf(output, L"education_investment_law = %s\n", educational_investment_law.c_str());
	fwprintf(output, L"industrial_policy_laws = %s\n", industrial_policy_laws.c_str());
	fwprintf(output, L"press_laws = %s\n", press_laws.c_str());
	fwprintf(output, L"training_laws = %s\n", training_laws.c_str());
	outputPracticals(output);
	outputTech(output);
	outputParties(output);

	fwprintf(output, L"oob = \"%s\"\n", (tag + L"_OOB.txt").c_str());
	fclose(output);

	// output OOB file
	outputOOB();

	// output leaders file
	outputLeaders();

	// Output common country file. 
	if (_wfopen_s(&output, (L"Output\\" + Configuration::getOutputName() + L"\\common\\countries\\" + commonCountryFile).c_str(), L"w") != 0)
	{
		Log(LogLevel::Error) << "Could not open " << "Output\\" << Configuration::getOutputName() << "\\common\\countries\\" << commonCountryFile;
		exit(-1);
	}
	int red;
	int green;
	int blue;
	color.GetRGB(red, green, blue);
	fwprintf(output, L"color = { %d %d %d }\n", red, green, blue);
	fwprintf(output, L"graphical_culture = %s\n", graphicalCulture.c_str());
	fwprintf(output, L"\n");
	if (majorNation)
	{
		fwprintf(output, L"major = yes\n");
		fwprintf(output, L"\n");
	}
	fwprintf(output, L"default_templates = {\n");
	fwprintf(output, L"	generic_infantry = {\n");
	fwprintf(output, L"		infantry_brigade\n");
	fwprintf(output, L"		infantry_brigade\n");
	fwprintf(output, L"		infantry_brigade\n");
	fwprintf(output, L"	}\n");
	fwprintf(output, L"	generic_milita = {\n");
	fwprintf(output, L"		militia_brigade\n");
	fwprintf(output, L"		militia_brigade\n");
	fwprintf(output, L"		militia_brigade\n");
	fwprintf(output, L"	}\n");
	fwprintf(output, L"	generic_armoured = {\n");
	fwprintf(output, L"		armor_brigade\n");
	fwprintf(output, L"		motorized_brigade\n");
	fwprintf(output, L"		motorized_brigade\n");
	fwprintf(output, L"	}\n");
	fwprintf(output, L"	generic_cavalry = {\n");
	fwprintf(output, L"		cavalry_brigade\n");
	fwprintf(output, L"		cavalry_brigade\n");
	fwprintf(output, L"	}\n");
	fwprintf(output, L"}\n");
	fwprintf(output, L"\n");
	fwprintf(output, L"unit_names = {\n");
	//fwprintf(output, L"	infantry_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Division\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	cavalry_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Cavalry\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	motorized_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Motor Div.\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	mechanized_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Half Track Div.\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	light_armor_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Armoured Div.\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	armor_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Armoured Div.\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	paratrooper_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Para Division\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	marine_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Marine Division\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	bergsjaeger_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Mountain Division\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	garrison_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Division\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	hq_brigade = {\n");
	//fwprintf(output, L"		\"1st Afghanestani Army\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	militia_brigade = {\n");
	//fwprintf(output, L"		\"Faizabad Militia\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	multi_role = {\n");
	//fwprintf(output, L"		\"I.Fighter Group\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	interceptor = {\n");
	//fwprintf(output, L"		\"I.Fighter Group\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	strategic_bomber = {\n");
	//fwprintf(output, L"		\"I.Strategic Group\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	tactical_bomber = {\n");
	//fwprintf(output, L"		\"I.Tactical Group\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	naval_bomber = {\n");
	//fwprintf(output, L"		\"I.Naval Bomber Group\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	cas = {\n");
	//fwprintf(output, L"		\"I.Dive Bomber Group\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	transport_plane = {\n");
	//fwprintf(output, L"		\"I.Air Transport Group\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	battleship = {\n");
	//fwprintf(output, L"		\"RAS Afghanistan\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	heavy_cruiser = {\n");
	//fwprintf(output, L"		\"RAS Faizabad\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	destroyer = {\n");
	//fwprintf(output, L"		\"D1 / D2 / D3\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	carrier = {\n");
	//fwprintf(output, L"		\"RAS Zahir Shah\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	submarine = {\n");
	//fwprintf(output, L"		\"1. Submarine Flotilla\"\n");
	//fwprintf(output, L"	}\n");
	//fwprintf(output, L"	transport_ship = {\n");
	//fwprintf(output, L"		\"1. Troop Transport Flotilla\"\n");
	//fwprintf(output, L"	}\n");
	fwprintf(output, L"}\n");
	fwprintf(output, L"\n");
	fwprintf(output, L"ministers = {\n");
	for (auto ministerItr: ministers)
	{
		ministerItr.output(output);
	}
	fwprintf(output, L"}\n");
	fclose(output);

	outputAIScript();
}


void HoI4Country::outputToCommonCountriesFile(FILE* output) const
{
	fwprintf(output, L"%s = \"countries%s\"\n", tag.c_str(), commonCountryFile.c_str());
}


void HoI4Country::outputPracticals(FILE* output) const
{
	fwprintf(output, L"\n");
	for (map<wstring, double>::const_iterator itr = practicals.begin(); itr != practicals.end(); ++itr)
	{
		if (itr->second > 0.0)
		{
			fwprintf(output, L"%s = %.2f\n", itr->first.c_str(), min(20.0, itr->second));
		}
	}
}


void HoI4Country::outputTech(FILE* output) const
{
	fwprintf(output, L"\n");
	for (map<wstring, int>::const_iterator itr = technologies.begin(); itr != technologies.end(); ++itr)
	{
		fwprintf(output, L"%s = %d\n", itr->first.c_str(), itr->second);
	}
}


void HoI4Country::outputParties(FILE* output) const
{
	fwprintf(output, L"popularity = {\n");
	for (auto party: parties)
	{
		fwprintf(output, L"\t%s = %d\n", party.ideology.c_str(), party.popularity);
	}
	fwprintf(output, L"}\n");
	fwprintf(output, L"\n");

	fwprintf(output, L"organization = {\n");
	for (auto party: parties)
	{
		fwprintf(output, L"\t%s = %d\n", party.ideology.c_str(), party.organization);
	}
	fwprintf(output, L"}\n");
	fwprintf(output, L"\n");

	FILE* partyLocalisations;
	if (_wfopen_s(&partyLocalisations, (L"Output\\" + Configuration::getOutputName() + L"\\localisation\\Parties.csv").c_str(), L"a") != 0)
	{
		LOG(LogLevel::Error) << "Could not open " << "Output\\" << Configuration::getOutputName() << "\\localisation\\Parties.csv";
		exit(-1);
	}
	for (auto party: parties)
	{
		fwprintf(partyLocalisations, L"%s;\n", party.localisationString.c_str());
	}
	fclose(partyLocalisations);
}


void HoI4Country::outputLeaders() const
{
	FILE* leadersFile;
	if (_wfopen_s(&leadersFile, (L"Output\\" + Configuration::getOutputName() + L"\\history\\leaders\\" + tag.c_str() + L".txt").c_str(), L"w") != 0)
	{
		LOG(LogLevel::Error) << "Could not open " << "Output\\" << Configuration::getOutputName() << "\\history\\leaders\\" << tag.c_str() << ".txt";
	}
	int landLeaders	= 0;
	int seaLeaders		= 0;
	int airLeaders		= 0;
	for (auto leader: leaders)
	{
		leader.output(leadersFile);

		if (leader.getType() == L"land")
		{
			landLeaders++;
		}
		else if (leader.getType() == L"sea")
		{
			seaLeaders++;
		}
		else if (leader.getType() == L"air")
		{
			airLeaders++;
		}
		else
		{
			LOG(LogLevel::Warning) << "Leader of unknown type in " << tag;
		}
	}
	fclose(leadersFile);

	LOG(LogLevel::Info) << tag << " has " << landLeaders << " land leaders, L" << seaLeaders << " sea leaders, and " << airLeaders << " air leaders.";
}


void HoI4Country::outputOOB() const
{
	FILE* output;
	if (_wfopen_s(&output, (L"Output\\" + Configuration::getOutputName() + L"\\history\\units\\" + tag + L"_OOB.txt").c_str(), L"w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create OOB file " << (tag + L"_OOB.txt");
		exit(-1);
	}

	for (auto armyItr: armies)
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


void HoI4Country::initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const wstring _vic2ideology, vector<wstring> outputOrder, const CountryMapping& countryMap, inverseProvinceMapping inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	srcCountry = _srcCountry;

	struct _wfinddata_t	fileData;
	intptr_t					fileListing;
	wstring filesearch = L".\\blankMod\\output\\history\\countries\\" + tag + L"*.txt";
	if ((fileListing = _wfindfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename = fileData.name;
	}
	_findclose(fileListing);
	if (filename == L"")
	{
		wstring filesearch = Configuration::getHoI4Path() + L"\\tfh\\history\\countries\\" + tag + L"*.txt";
		if ((fileListing = _wfindfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename = fileData.name;
		}
		else
		{
			_findclose(fileListing);
			filesearch = Configuration::getHoI4Path() + L"\\history\\countries\\" + tag + L"*.txt";
			if ((fileListing = _wfindfirst(filesearch.c_str(), &fileData)) != -1L)
			{
				filename = fileData.name;
			}
		}
		_findclose(fileListing);
	}
	if (filename == L"")
	{
		wstring countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of(L"/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + L" - " + countryName;
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
		graphicalCulture = L"Generic";
	}

	// Government
	wstring srcGovernment = srcCountry->getGovernment();
	if (srcGovernment.size() > 0)
	{
		government = governmentMapper::getInstance()->getGovernmentForCountry(srcCountry, _vic2ideology);
		if (government.empty())
		{
			government = L"";
			LOG(LogLevel::Warning) << "No government mapping defined for " << srcGovernment << " (L" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	// Political parties
	convertParties(_srcCountry, _srcWorld.getActiveParties(_srcCountry), _srcWorld.getRulingParty(_srcCountry), ideology);
	for (auto partyItr = parties.begin(); partyItr != parties.end(); partyItr++)
	{
		auto oldLocalisation = V2Localisations.GetTextInEachLanguage(partyItr->name);
		partyItr->localisationString = partyItr->ideology + L"_" + tag;
		auto localisationItr = oldLocalisation.begin();
		localisationItr++;
		for (; localisationItr != oldLocalisation.end(); localisationItr++)
		{
			partyItr->localisationString += L";" + localisationItr->second;
		}
	}

	// Ministers
	vector<wstring> firstNames;
	vector<wstring> lastNames;
	auto namesItr = namesMap.find(srcCountry->getPrimaryCulture());
	if (namesItr != namesMap.end())
	{
		firstNames	= namesItr->second.first;
		lastNames	= namesItr->second.second;
	}
	else
	{
		firstNames.push_back(L"null");
		lastNames.push_back(L"null");
	}
	for (unsigned int ideologyIdx = 0; ideologyIdx <= stalinist; ideologyIdx++)
	{
		for (auto job: governmentJobs)
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

	wstring warPolicy = _srcWorld.getRulingParty(_srcCountry)->war_policy;
	if (warPolicy == L"jingoism")
	{
		neutrality = 60;
	}
	else if (warPolicy == L"pro_military")
	{
		neutrality = 73.3;
	}
	else if (warPolicy == L"anti_military")
	{
		neutrality = 86.6;
	}
	else if (warPolicy == L"pacifism")
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
	if (srcGovernment == L"democracy" || srcGovernment == L"hms_government")
	{
		civil_law = L"open_society";
	}
	else if (srcGovernment == L"proletarian_dictatorship")
	{
		civil_law = L"totalitarian_system";
	}
	else
	{
		if (nationalUnity > 50.0)
		{
			civil_law = L"limited_restrictions";
		}
		else
		{
			civil_law = L"open_society";
		}
	}

	// conscription law - everyone starts with volunteer armies
	conscription_law = L"volunteer_army";

	// economic law - everyone starts with full civilian economy
	economic_law = L"full_civilian_economy";

	// educational investment law - from educational spending
	if (srcCountry->getEducationSpending() > 0.90)
	{
		educational_investment_law = L"big_education_investment";
	}
	else if (srcCountry->getEducationSpending() > 0.70)
	{
		educational_investment_law = L"medium_large_education_investment";
	}
	else if (srcCountry->getEducationSpending() > 0.40)
	{
		educational_investment_law = L"average_education_investment";
	}
	else
	{
		educational_investment_law = L"minimal_education_investment";
	}

	// industrial policy laws - everyone starts with consumer product orientation
	industrial_policy_laws = L"consumer_product_orientation";

	// press laws - set from press reforms
	if (srcCountry->getReform(L"press_rights") == L"free_press")
	{
		press_laws = L"free_press";
	}
	else if (srcCountry->getReform(L"press_rights") == L"censored_press")
	{
		press_laws = L"censored_press";
	}
	else // press_rights == state_press
	{
		if ((srcGovernment == L"proletarian_dictatorship") ||
			(srcGovernment == L"fascist_dictatorship"))
		{
			press_laws = L"propaganda_press";
		}
		else
		{
			press_laws = L"state_press";
		}
	}

	// training laws - from military spending
	if (srcCountry->getMilitarySpending() > 0.90)
	{
		training_laws = L"specialist_training";
	}
	else if (srcCountry->getMilitarySpending() > 0.70)
	{
		training_laws = L"advanced_training";
	}
	else if (srcCountry->getMilitarySpending() > 0.40)
	{
		training_laws = L"basic_training";
	}
	else
	{
		training_laws = L"minimal_training";
	}

	// leaders
	vector<V2Leader*> srcLeaders = srcCountry->getLeaders();
	for (auto srcLeader: srcLeaders)
	{
		HoI4Leader newLeader(srcLeader, tag, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}

	Configuration::setLeaderIDForNextCountry();

	// Relations
	map<wstring, V2Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (map<wstring, V2Relations*>::iterator itr = srcRelations.begin(); itr != srcRelations.end(); ++itr)
		{
			const std::wstring& HoI4Tag = countryMap[itr->second->getTag()];
			if (!HoI4Tag.empty())
			{
				HoI4Relations* hoi2r = new HoI4Relations(HoI4Tag, itr->second);
				relations.insert(make_pair(HoI4Tag, hoi2r));
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

	// major nation
	majorNation = srcCountry->getGreatNation();
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void HoI4Country::initFromHistory()
{
	wstring fullFilename;
	struct _wfinddata_t	fileData;
	intptr_t					fileListing;
	wstring filesearch = L".\\blankMod\\output\\history\\countries\\" + tag + L"*.txt";
	if ((fileListing = _wfindfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename			= fileData.name;
		fullFilename	= wstring(L".\\blankMod\\output\\history\\countries\\") + fileData.name;
	}
	_findclose(fileListing);
	if (fullFilename == L"")
	{
		wstring filesearch = Configuration::getHoI4Path() + L"\\tfh\\history\\countries\\" + tag + L"*.txt";
		if ((fileListing = _wfindfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename			= fileData.name;
			fullFilename = Configuration::getHoI4Path() + L"\\tfh\\history\\countries\\" + fileData.name;
		}
		else
		{
			_findclose(fileListing);
			filesearch = Configuration::getHoI4Path() + L"\\history\\countries\\" + tag + L"*.txt";
			if ((fileListing = _wfindfirst(filesearch.c_str(), &fileData)) != -1L)
			{
				filename			= fileData.name;
				fullFilename = Configuration::getHoI4Path() + L"\\history\\countries\\" + fileData.name;
			}
		}
		_findclose(fileListing);
	}
	if (fullFilename == L"")
	{
		wstring countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of(L"/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + L" - " + countryName;
		return;
	}

	Object* obj = parser_UTF8::doParseFile(fullFilename.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << fullFilename;
		exit(-1);
	}

	vector<Object*> results = obj->getValue(L"government");
	if (results.size() > 0)
	{
		government = results[0]->getLeaf();
	}

	results = obj->getValue(L"ideology");
	if (results.size() > 0)
	{
		ideology = results[0]->getLeaf();
	}

	results = obj->getValue(L"capital");
	if (results.size() > 0)
	{
		capital = _wtoi(results[0]->getLeaf().c_str());
	}
}


void HoI4Country::consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap, double& totalManpower, double& totalLeadership, double& totalIndustry)
{
	bool convertManpower		= (Configuration::getManpowerConversion() != L"no");
	bool convertLeadership	= (Configuration::getLeadershipConversion() != L"no");
	bool convertIndustry		= (Configuration::getIcConversion() != L"no");

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
			auto possibleHoI4Provinces = inverseProvinceMap.find(srcProvinceItr);
			if (possibleHoI4Provinces != inverseProvinceMap.end())
			{
				for (auto dstProvinceNum: possibleHoI4Provinces->second)
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
			auto possibleHoI4Provinces = inverseProvinceMap.find(stateItr.provinces[0]);
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
				for (auto vic2ProvNum: stateItr.provinces)
				{
					auto possibleHoI4Provinces = inverseProvinceMap.find(vic2ProvNum);
					if (possibleHoI4Provinces != inverseProvinceMap.end())
					{
						for (auto HoI4ProvNum: possibleHoI4Provinces->second)
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
			for (auto vic2ProvinceNum: capitalState.provinces)
			{
				auto possibleHoI4Provinces = inverseProvinceMap.find(vic2ProvinceNum);
				if (possibleHoI4Provinces != inverseProvinceMap.end())
				{
					for (auto HoI4ProvinceNum: possibleHoI4Provinces->second)
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
	vector<wstring> firstNames;
	vector<wstring> lastNames;
	auto namesItr = namesMap.find(srcCountry->getPrimaryCulture());
	if (namesItr != namesMap.end())
	{
		firstNames	= namesItr->second.first;
		lastNames	= namesItr->second.second;
	}
	else
	{
		firstNames.push_back(L"null");
		lastNames.push_back(L"null");
	}

	// generated leaders
	int totalOfficers = 0;
	vector<V2Province*> srcProvinces = srcCountry->getCores();
	for (auto province: srcProvinces)
	{
		totalOfficers += province->getPopulation(L"officers");
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
		HoI4Leader newLeader(firstNames, lastNames, tag, L"land", leaderTraits, portraitMap[graphicalCulture]);
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
		HoI4Leader newLeader(firstNames, lastNames, tag, L"sea", leaderTraits, portraitMap[graphicalCulture]);
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
		HoI4Leader newLeader(firstNames, lastNames, tag, L"air", leaderTraits, portraitMap[graphicalCulture]);
		leaders.push_back(newLeader);
	}
}


void HoI4Country::setAIFocuses(const AIFocusModifiers& focusModifiers)
{
	for (auto currentModifier: focusModifiers)
	{
		double modifierAmount = 1.0;
		for (auto modifier: currentModifier.second)
		{
			bool modifierActive = false;
			if (modifier.modifierType == L"lacks_port")
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
			else if (modifier.modifierType == L"tech_school")
			{
				if (modifier.modifierRequirement == srcCountry->getTechSchool())
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"coast_border_percent")
			{
				int navalBases = 0;
				for (auto province: provinces)
				{
					if (province.second->hasNavalBase())
					{
						navalBases++;
					}
				}
				if ((1.0 * navalBases / provinces.size()) >= _wtof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"navy_tech_ahead")
			{
				if ((srcCountry->getNumNavyTechs() - srcCountry->getNumArmyTechs()) >= _wtoi(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"capital_continent")
			{
				if (srcCountry->getCapitalContinent() == modifier.modifierRequirement)
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"ship_composition_percent")
			{
				int totalUnits		= 0;
				int numSeaUnits	= 0;
				for (auto army: srcCountry->getArmies())
				{
					for (auto regiment: army->getRegiments())
					{
						totalUnits++;
						wstring type = regiment->getType();
						if (
								(type == L"artillery") || (type == L"cavalry") || (type == L"cuirassier") || (type == L"dragoon") || (type == L"engineer") || 
								(type == L"guard") || (type == L"hussar") || (type == L"infantry") || (type == L"irregular") || (type == L"plane") || (type == L"tank")
							)
						{
							numSeaUnits++;
						}
					}
				}
				if ((1.0 * numSeaUnits / totalUnits) > _wtof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"tank_composition_percent")
			{
				int totalUnits	= 0;
				int numTanks	= 0;
				for (auto army: srcCountry->getArmies())
				{
					for (auto regiment: army->getRegiments())
					{
						totalUnits++;
						wstring type = regiment->getType();
						if (type == L"tank")
						{
							numTanks++;
						}
					}
				}
				if ((1.0 * numTanks / totalUnits) > _wtof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"plane_composition_percent")
			{
				int totalUnits	= 0;
				int numPlanes	= 0;
				for (auto army: srcCountry->getArmies())
				{
					for (auto regiment: army->getRegiments())
					{
						totalUnits++;
						wstring type = regiment->getType();
						if (type == L"plane")
						{
							numPlanes++;
						}
					}
				}
				if ((1.0 * numPlanes / totalUnits) > _wtof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"manpower_to_IC_ratio")
			{
				double totalManpower	= 0.0;
				double totalIC			= 0.0;
				for (auto province: provinces)
				{
					totalManpower	+= province.second->getManpower();
					totalIC			+= province.second->getActualIndustry();
				}
				if ((totalManpower / totalIC) > _wtof(modifier.modifierRequirement.c_str()))
				{
					modifierActive = true;
				}
			}
			else if (modifier.modifierType == L"default")
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
	for (auto province: provinces)
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
		if (state.provinces.size() > 0)
		{
			auto possibleHoI4Provinces = inverseProvinceMap.find(state.provinces[0]);
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


HoI4Relations* HoI4Country::getRelations(wstring withWhom) const
{
	map<wstring, HoI4Relations*>::const_iterator i = relations.find(withWhom);
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
		ifstream s(L"port_blacklist.txt");
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
		map<int, HoI4Province*>::iterator pitr = allProvinces.find(*litr);
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


void HoI4Country::convertParties(const V2Country* srcCountry, vector<V2Party*> V2Parties, V2Party* rulingParty, wstring& rulingIdeology)
{
	// sort Vic2 parties by ideology
	map<wstring, vector<V2Party*>> V2Ideologies;
	for (auto partyItr: V2Parties)
	{
		wstring ideology = partyItr->ideology;
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
	map<wstring, wstring> unmappedParties;
	unmappedParties.insert(make_pair(L"national_socialist", L"fascist_group"));
	unmappedParties.insert(make_pair(L"fascistic", L"fascist_group"));
	unmappedParties.insert(make_pair(L"paternal_autocrat", L"fascist_group"));
	unmappedParties.insert(make_pair(L"social_conservative", L"democratic_group"));
	unmappedParties.insert(make_pair(L"market_liberal", L"democratic_group"));
	unmappedParties.insert(make_pair(L"social_liberal", L"democratic_group"));
	unmappedParties.insert(make_pair(L"social_democrat", L"democratic_group"));
	unmappedParties.insert(make_pair(L"left_wing_radical", L"communist_group"));
	unmappedParties.insert(make_pair(L"leninist", L"communist_group"));
	unmappedParties.insert(make_pair(L"stalinist", L"communist_group"));

	// map all the simplistic cases
	auto ideologyItr = V2Ideologies.find(L"fascist");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= L"fascistic";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(L"fascist") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = L"fascistic";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find(L"fascistic");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find(L"reactionary");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= L"paternal_autocrat";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(L"reactionary") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = L"paternal_autocrat";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find(L"paternal_autocrat");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find(L"conservative");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= L"social_conservative";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(L"conservative") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = L"social_conservative";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find(L"social_conservative");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find(L"socialist");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= L"left_wing_radical";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(L"socialist") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = L"left_wing_radical";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find(L"left_wing_radical");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find(L"communist");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= L"stalinist";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(L"communist") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = L"stalinist";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find(L"stalinist");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find(L"liberal");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= L"social_liberal";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(L"liberal") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = L"social_liberal";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find(L"social_liberal");
		unmappedParties.erase(itr);
	}
	ideologyItr = V2Ideologies.find(L"anarcho_liberal");
	if ((ideologyItr != V2Ideologies.end()) && (ideologyItr->second.size() == 1))
	{
		HoI4Party newParty;
		newParty.name				= ideologyItr->second[0]->name;
		newParty.ideology			= L"market_liberal";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(L"anarcho_liberal") * 100 + 0.5);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		if (rulingParty->ideology == ideologyItr->first)
		{
			rulingIdeology = L"market_liberal";
		}

		V2Ideologies.erase(ideologyItr);
		auto itr = unmappedParties.find(L"market_liberal");
		unmappedParties.erase(itr);
	}

	if (V2Ideologies.size() == 0)
	{
		return;
	}

	// map the simple excess cases
	map<wstring, vector<V2Party*>> V2IdeologyGroups;
	ideologyItr = V2Ideologies.find(L"fascist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"reactionary");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"conservative");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"socialist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"communist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"anarcho_liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"democratic_group", parties));
			}
		}
	}

	map<wstring, vector<wstring>> HoI4IdeologyGroups;
	for (auto HoI4PartyItr: unmappedParties)
	{
		auto groupItr = HoI4IdeologyGroups.find(HoI4PartyItr.second);
		if (groupItr != HoI4IdeologyGroups.end())
		{
			groupItr->second.push_back(HoI4PartyItr.first);
		}
		else
		{
			vector<wstring> parties;
			parties.push_back(HoI4PartyItr.first);
			HoI4IdeologyGroups.insert(make_pair(HoI4PartyItr.second, parties));
		}
	}

	for (auto V2GroupItr: V2IdeologyGroups)
	{
		auto HoI4GroupItr = HoI4IdeologyGroups.find(V2GroupItr.first);
		if ((HoI4GroupItr != HoI4IdeologyGroups.end()) && (V2GroupItr.second.size() <= HoI4GroupItr->second.size()))
		{
			for (auto V2PartyItr: V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);

				HoI4Party newParty;
				newParty.name				= V2PartyItr->name;
				newParty.ideology			= HoI4GroupItr->second[0];
				newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(ideologyItr->first) * 100 / ideologyItr->second.size() + 0.5);
				newParty.organization	= newParty.popularity;
				parties.push_back(newParty);

				if (rulingParty->name == V2PartyItr->name)
				{
					rulingIdeology = HoI4GroupItr->second[0];
				}

				HoI4GroupItr->second.erase(HoI4GroupItr->second.begin());

				auto itr = unmappedParties.find(newParty.ideology);
				unmappedParties.erase(itr);
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
	for (map<wstring, vector<V2Party*>>::iterator ideologyItr = V2Ideologies.begin(); ideologyItr != V2Ideologies.end(); ideologyItr++)
	{
		while (ideologyItr->second.size() > 1)
		{
			ideologyItr->second.pop_back();
		}
	}

	V2IdeologyGroups.clear();
	ideologyItr = V2Ideologies.find(L"fascist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"reactionary");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"fascist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"fascist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"conservative");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"socialist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"communist");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"communist_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"communist_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"democratic_group", parties));
			}
		}
	}
	ideologyItr = V2Ideologies.find(L"anarcho_liberal");
	if (ideologyItr != V2Ideologies.end())
	{
		for (auto partyItr: ideologyItr->second)
		{
			auto groupItr = V2IdeologyGroups.find(L"democratic_group");
			if (groupItr != V2IdeologyGroups.end())
			{
				groupItr->second.push_back(partyItr);
			}
			else
			{
				vector<V2Party*> parties;
				parties.push_back(partyItr);
				V2IdeologyGroups.insert(make_pair(L"democratic_group", parties));
			}
		}
	}

	for (auto V2GroupItr: V2IdeologyGroups)
	{
		auto HoI4GroupItr = HoI4IdeologyGroups.find(V2GroupItr.first);
		if ((HoI4GroupItr != HoI4IdeologyGroups.end()) && (V2GroupItr.second.size() <= HoI4GroupItr->second.size()))
		{
			for (auto V2PartyItr: V2GroupItr.second)
			{
				ideologyItr = V2Ideologies.find(V2PartyItr->ideology);

				HoI4Party newParty;
				newParty.name				= V2PartyItr->name;
				newParty.ideology			= HoI4GroupItr->second[0];
				newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage(ideologyItr->first) * 100 / ideologyItr->second.size() + 0.5);
				newParty.organization	= newParty.popularity;
				parties.push_back(newParty);

				HoI4GroupItr->second.erase(HoI4GroupItr->second.begin());

				auto itr = unmappedParties.find(newParty.ideology);
				unmappedParties.erase(itr);

				if (rulingParty->ideology == V2PartyItr->ideology)
				{
					rulingIdeology = HoI4GroupItr->second[0];
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
	if (_wfopen_s(&output, (L"Output\\" + Configuration::getOutputName() + L"\\script\\country\\" + tag + L".lua").c_str(), L"w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create country script file for " << tag;
		exit(-1);
	}

	fwprintf(output, L"local P = {}\n");
	fwprintf(output, L"AI_%s = P\n", tag.c_str());

	wifstream sourceFile;
	LOG(LogLevel::Debug) << tag << ": air modifier - " << airModifier << ", tankModifier - " << tankModifier << ", seaModifier - " << seaModifier << ", infModifier - " << infModifier;
	if ((airModifier > seaModifier) && (airModifier > tankModifier) && (airModifier > infModifier)) // air template
	{
		sourceFile.open(L"airTEMPLATE.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open airTEMPLATE.lua";
			exit(-1);
		}
	}
	else if ((seaModifier > tankModifier) && (seaModifier > infModifier))	// sea template
	{
		sourceFile.open(L"shipTemplate.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open shipTemplate.lua";
			exit(-1);
		}
	}
	else if (tankModifier > infModifier) // tank template
	{
		sourceFile.open(L"tankTemplate.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open tankTemplate.lua";
			exit(-1);
		}
	}	
	else	// infantry template
	{
		sourceFile.open(L"infatryTEMPLATE.lua", ifstream::in);
		if (!sourceFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not open infatryTEMPLATE.lua";
			exit(-1);
		}
	}

	while (!sourceFile.eof())
	{
		wstring line;
		getline(sourceFile, line);
		fwprintf(output, L"%s\n", line.c_str());
	}
	sourceFile.close();

	fwprintf(output, L"return AI_%s\n", tag.c_str());

	fclose(output);
}


void HoI4Country::setTechnology(wstring tech, int level)
{
	// don't allow downgrades
	map<wstring, int>::iterator techEntry = technologies.find(tech);
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