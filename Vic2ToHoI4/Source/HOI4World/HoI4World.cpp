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

#include <string>
#include "HoI4World.h"
#include <fstream>
#include <algorithm>
#include <list>
#include <queue>
#include <cmath>
#include <cfloat>
#include "ParadoxParserUTF8.h"
#include "Log.h"
#include "../Configuration.h"
#include "../../../common_items/OSCompatibilityLayer.h"
#include "../V2World/V2Province.h"
#include "../V2World/V2Party.h"
#include "HoI4Relations.h"
#include "HoI4State.h"
#include <cmath>
#include <random>  
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 
#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>
#include<IL\config.h>
#include<IL\devil_internal_exports.h>
#include<IL\ilu_region.h>
#include<IL\ilut_config.h>
#include <GL/gl.h>
#include <GL/glu.h>
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment(lib, "IL/DevIL.lib")
#pragma comment(lib, "IL/ILU.lib")
#pragma comment(lib, "IL/ILUT.lib")

typedef struct fileWithCreateTime
{
	string	filename;
	time_t	createTime;
	bool operator < (const fileWithCreateTime &rhs) const
	{
		return createTime < rhs.createTime;
	};
} fileWithCreateTime;


void HoI4World::importStates()
{
	LOG(LogLevel::Info) << "Importing states";

	set<string> statesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/history/states", statesFiles);
	for (auto stateFile : statesFiles)
	{
		int num = stoi(stateFile.substr(0, stateFile.find_first_of('-')));
		stateFilenames.insert(make_pair(num, stateFile));
	}
}


void HoI4World::checkCoastalProvinces()
{
	// determine whether each province is coastal or not by checking if it has a naval base
	// if it's not coastal, we won't try to put any navies in it (otherwise HoI4 crashes)
	//Object*	obj2 = parser_UTF8::doParseFile((Configuration::getHoI4Path() + "/tfh/map/positions.txt").c_str());
	//vector<Object*> objProv = obj2->getLeaves();
	//if (objProv.size() == 0)
	//{
	//	LOG(LogLevel::Error) << "map/positions.txt failed to parse.";
	//	exit(1);
	//}
	//for (auto itr: objProv)
	//{
	//	int provinceNum = _wtoi(itr->getKey().c_str());
	//	vector<Object*> objPos = itr->getValue("building_position");
	//	if (objPos.size() == 0)
	//	{
	//		continue;
	//	}
	//	vector<Object*> objNavalBase = objPos[0]->getValue("naval_base");
	//	if (objNavalBase.size() != 0)
	//	{
	//		// this province is coastal
	//		map<int, HoI4Province*>::iterator pitr = provinces.find(provinceNum);
	//		if (pitr != provinces.end())
	//		{
	//			pitr->second->setCoastal(true);
	//		}
	//	}
	//}
}


void HoI4World::importPotentialCountries()
{
	potentialCountries.clear();
	countries.clear();

	LOG(LogLevel::Info) << "Getting potential countries";
	set<string> countryFilenames;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/common/country_tags", countryFilenames);

	for (auto countryFilename : countryFilenames)
	{
		ifstream HoI4CountriesInput(Configuration::getHoI4Path() + "/common/country_tags/" + countryFilename);
		if (!HoI4CountriesInput.is_open())
		{
			LOG(LogLevel::Error) << "Could not open " << countryFilename;
			exit(1);
		}

		while (!HoI4CountriesInput.eof())
		{
			string line;
			getline(HoI4CountriesInput, line);

			if ((line[0] == '#') || (line.size() < 3))
			{
				continue;
			}
			if (line.substr(0, 19) == "dynamic_tags  = yes")
			{
				break;
			}

			string tag;
			tag = line.substr(0, 3);

			string countryFileName;
			int start = line.find_first_of('/');
			int size = line.find_last_of('\"') - start;
			countryFileName = line.substr(start, size);
			HoI4Country* newCountry = new HoI4Country(tag, countryFileName, this);
			potentialCountries.insert(make_pair(tag, newCountry));
		}
		HoI4CountriesInput.close();
	}
}


void HoI4World::output() const
{
	outputCommonCountries();
	outputColorsfile();
	//outputAutoexecLua();
	outputLocalisations();
	outputHistory();
	//output events
	Utils::copyFolder("events", "output/" + Configuration::getOutputName());
	outputMap();
}


void HoI4World::outputCommonCountries() const
{
	// Create common\countries path.
	string countriesPath = "Output/" + Configuration::getOutputName() + "/common";
	/*if (!Utils::TryCreateFolder(countriesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common\"";
		exit(-1);
	}*/
	if (!Utils::TryCreateFolder(countriesPath + "/countries"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common/countries\"";
		exit(-1);
	}
	if (!Utils::TryCreateFolder(countriesPath + "/country_tags"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common/country_tags\"";
		exit(-1);
	}

	// Output common\countries.txt
	LOG(LogLevel::Debug) << "Writing countries file";
	FILE* allCountriesFile;
	if (fopen_s(&allCountriesFile, ("Output/" + Configuration::getOutputName() + "/common/country_tags/01_countries.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create countries file";
		exit(-1);
	}

	for (auto countryItr : countries)
	{
		if (potentialCountries.find(countryItr.first) == potentialCountries.end())
		{
			countryItr.second->outputToCommonCountriesFile(allCountriesFile);
		}
	}
	fprintf(allCountriesFile, "\n");
	fclose(allCountriesFile);
}


void HoI4World::outputColorsfile() const
{

	ofstream output2;
	output2.open(("Output/" + Configuration::getOutputName() + "/common/countries/colors.txt"));
	if (!output2.is_open())
	{
		Log(LogLevel::Error) << "Could not open " << "Output/" << Configuration::getOutputName() << "/common/countries/colors.txt";
		exit(-1);
	}
	string s = "#reload countrycolors\r\n";
	for (auto countryItr : countries)
	{
		if (potentialCountries.find(countryItr.first) == potentialCountries.end())
		{
			s += countryItr.second->outputColors();
			//countryItr.second->outputColors(allCountriesFile);
		}
	}
	output2 << s;
	output2.close();
}


void HoI4World::outputAutoexecLua() const
{
	// output autoexec.lua
	FILE* autoexec;
	if (fopen_s(&autoexec, ("Output/" + Configuration::getOutputName() + "/script/autoexec.lua").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create autoexec.lua";
		exit(-1);
	}

	ifstream sourceFile;
	sourceFile.open("autoexecTEMPLATE.lua", ifstream::in);
	if (!sourceFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open autoexecTEMPLATE.lua";
		exit(-1);
	}
	while (!sourceFile.eof())
	{
		string line;
		getline(sourceFile, line);
		fprintf(autoexec, "%s\n", line.c_str());
	}
	sourceFile.close();

	for (auto country : potentialCountries)
	{
		fprintf(autoexec, "require('%s')\n", country.first.c_str());
	}
	fprintf(autoexec, "\n");
	fclose(autoexec);
}


void HoI4World::outputLocalisations() const
{
	// Create localisations for all new countries. We don't actually know the names yet so we just use the tags as the names.
	LOG(LogLevel::Debug) << "Writing localisation text";
	string localisationPath = "Output/" + Configuration::getOutputName() + "/localisation";
	if (!Utils::TryCreateFolder(localisationPath))
	{
		LOG(LogLevel::Error) << "Could not create localisation folder";
		exit(-1);
	}

	localisation.output(localisationPath);
}
void HoI4World::outputMap() const
{
	LOG(LogLevel::Debug) << "Writing Map Info";
	string mappath = "Output/" + Configuration::getOutputName() + "/map";
	if (!Utils::TryCreateFolder(mappath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/map";
		exit(-1);
	}
	string filename("Output/" + Configuration::getOutputName() + "/map/rocketsites.txt");
	ofstream out;
	out.open(filename);

	for (auto state : states)
	{
		vector<int> provinces = state.second->getProvinces();
		out << state.second->getID() << " = { " << provinces.front() << " }\r\n";
	}
	out.close();
	string filename2("Output/" + Configuration::getOutputName() + "/map/airports.txt");
	ofstream out2;
	out2.open(filename2);

	for (auto state : states)
	{
		vector<int> provinces2 = state.second->getProvinces();
		out2 << state.second->getID() << " = { " << provinces2.front() << " }\r\n";
	}
	out2.close();

	string filename3("Output/" + Configuration::getOutputName() + "/map/buildings.txt");
	ofstream out3;
	out3.open(filename3);
	out3.close();
}

void HoI4World::outputHistory() const
{

	LOG(LogLevel::Debug) << "Writing states";
	string statesPath = "Output/" + Configuration::getOutputName() + "/history/states";
	if (!Utils::TryCreateFolder(statesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history/states";
		exit(-1);
	}
	for (auto state : states)
	{
		string filename;
		auto nameItr = stateFilenames.find(state.first);
		if (nameItr == stateFilenames.end())
		{
			filename = to_string(state.first) + "-convertedState.txt";
		}
		else
		{
			filename = nameItr->second;
		}
		state.second->output(filename);
	}
	for (auto nameItr = stateFilenames.find(states.size() + 1); nameItr != stateFilenames.end(); nameItr++)
	{
		ofstream emptyStateFile("Output/" + Configuration::getOutputName() + "/history/states/" + nameItr->second);
		if (!emptyStateFile.is_open())
		{
			LOG(LogLevel::Warning) << "Could not create " << "Output/" << Configuration::getOutputName() << "/history/states/" << nameItr->second;
		}
		emptyStateFile.close();
	}

	LOG(LogLevel::Debug) << "Writing countries";
	string unitsPath = "Output/" + Configuration::getOutputName() + "/history/units";
	if (!Utils::TryCreateFolder(unitsPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history/units";
		exit(-1);
	}
	//old states
	/*for (auto countryItr : countries)
	{
		countryItr.second->output(states.size(), states);
	}*/
	// Override vanilla history to suppress vanilla OOB and faction membership being read
	vector<vector<HoI4Country*>> empty;
	for (auto potentialItr : potentialCountries)
	{
		if (countries.find(potentialItr.first) == countries.end())
		{
			potentialItr.second->output(states.size(), states, empty, "asdf");
		}
	}
	LOG(LogLevel::Debug) << "Writing diplomacy";
	//diplomacy.output();
}


void HoI4World::getProvinceLocalizations(const string& file)
{
	ifstream read;
	string line;
	read.open(file.c_str());
	while (read.good() && !read.eof())
	{
		getline(read, line);
		if (line.substr(0, 4) == "PROV" && isdigit(line.c_str()[4]))
		{
			int position = line.find_first_of(';');
			int num = atoi(line.substr(4, position - 4).c_str());
			string name = line.substr(position + 1, line.find_first_of(';', position + 1) - position - 1);
			provinces[num]->setName(name);
		}
	}
	read.close();
}


void HoI4World::convertCountries(const V2World &sourceWorld, const CountryMapping& countryMap, const inverseProvinceMapping& inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, const governmentJobsMap& governmentJobs, const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap, const HoI4StateMapping& stateMap)
{
	for (auto sourceItr : sourceWorld.getCountries())
	{
		// don't convert rebels
		if (sourceItr.first == "REB")
		{
			continue;
		}

		HoI4Country* destCountry = NULL;
		const std::string& HoI4Tag = countryMap[sourceItr.first];
		if (!HoI4Tag.empty())
		{
			auto candidateDestCountry = potentialCountries.find(HoI4Tag);
			if (candidateDestCountry != potentialCountries.end())
			{
				destCountry = candidateDestCountry->second;
			}
			if (destCountry == NULL) // No such HoI4 country exists yet for this tag so make a new one
			{
				std::string countryFileName = '/' + sourceItr.second->getName() + ".txt";
				destCountry = new HoI4Country(HoI4Tag, countryFileName, this, true);
			}
			V2Party* rulingParty = sourceWorld.getRulingParty(sourceItr.second);
			if (rulingParty == NULL)
			{
				LOG(LogLevel::Error) << "Could not find the ruling party for " << sourceItr.first << ". Were all mods correctly included?";
				exit(-1);
			}
			destCountry->initFromV2Country(sourceWorld, sourceItr.second, rulingParty->ideology, countryMap, inverseProvinceMap, leaderMap, V2Localisations, governmentJobs, namesMap, portraitMap, cultureMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap, stateMap, states);
			countries.insert(make_pair(HoI4Tag, destCountry));
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not convert V2 tag " << sourceItr.first << " to HoI4";
		}

		localisation.readFromCountry(sourceItr.second, HoI4Tag);
	}

	// initialize all potential countries
	// ALL potential countries should be output to the file, otherwise some things don't get initialized right in HoI4
	for (auto potentialItr : potentialCountries)
	{
		map<string, HoI4Country*>::iterator citr = countries.find(potentialItr.first);
		if (citr == countries.end())
		{
			potentialItr.second->initFromHistory();
		}
	}
}


struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<V2Province*> provinces;
	int totalPopulation;
};
void HoI4World::outputSupply(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, HoI4StateMapping& stateMap, V2Localisation& Vic2Localisations)
{
	int zoneid = 1;
	Object* objzones = parser_UTF8::doParseFile("supply.txt");
	auto obj2zones = objzones->getValue("zones");
	auto defaultzones = obj2zones[0]->getValue("link");
	string filename("Output/" + to_string(zoneid) + ".txt");
	vector<HoI4State*> usedStates;
	string supplypath = "Output/" + Configuration::getOutputName() + "/map/supplyareas";
	if (!Utils::TryCreateFolder(supplypath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/map/supplyareas";
		exit(-1);
	}

	for (auto zone : defaultzones)
	{
		vector<HoI4State*> ZoneStates;
		auto zonekeys = zone->getLeaves();
		string HoI4defaultZone = zonekeys[0]->getLeaf();
		string zonevalue = zonekeys[zonekeys.size() - 1]->getLeaf();
		for (int i = 1; i < zonekeys.size() - 1; i++)
		{
			auto province = zonekeys[i]->getLeaf();
			for (auto state : states)
			{
				vector<int> provinces = state.second->getProvinces();
				for (auto prov : provinces)
				{
					if (to_string(prov) == province)
					{
						if (find(usedStates.begin(), usedStates.end(), state.second) != usedStates.end())
						{

						}
						else
						{
							ZoneStates.push_back(state.second);
							usedStates.push_back(state.second);
						}
					}
				}
			}
		}
		ofstream out;
		filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-SupplyArea.txt");
		if (zoneid == 346)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-FaroeIslands.txt");
		if (zoneid == 347)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-JohnstonAtoll.txt");
		if (zoneid == 348)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-Jamaica.txt");
		if (zoneid == 349)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-Ascension.txt");
		if (zoneid == 350)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-DiegoGarcia.txt");
		if (zoneid == 351)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-ChristmasIslands.txt");
		if (zoneid == 352)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-CocosIslands.txt");
		if (zoneid == 353)
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(zoneid) + "-KerguelenIslands.txt");
		if (ZoneStates.size() > 0)
		{
			out.open(filename);
			{
				out << "supply_area={" << endl;
				out << "	id=" << zoneid << endl;
				//out << "	name = \"foo\"" << endl;
				out << "name=\"STATE_" << ZoneStates.front()->getID() << "\"" << endl;
				out << "	value = " << zonevalue << endl;
				out << "	states={" << endl;
				out << "		";
				for (auto state : ZoneStates)
				{
					string s = to_string(state->getID()) + " ";
					out << s;
				}
				out << endl;
				out << "	}" << endl;
				out << "}" << endl;

				zoneid++;
				out.close();
			}
		}

	}
	if (zoneid < 354)
	{
		for (int i = zoneid; i < 355; i++)
		{
			ofstream out;
			filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-SupplyArea.txt");
			if (i == 346)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-FaroeIslands.txt");
			if (i == 347)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-JohnstonAtoll.txt");
			if (i == 348)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-Jamaica.txt");
			if (i == 349)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-Ascension.txt");
			if (i == 350)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-DiegoGarcia.txt");
			if (i == 351)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-ChristmasIslands.txt");
			if (i == 352)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-CocosIslands.txt");
			if (i == 353)
				filename = ("Output/" + Configuration::getOutputName() + "/map/supplyareas/" + to_string(i) + "-KerguelenIslands.txt");
			out.open(filename);
			{

				out.close();
			}
		}
	}
}

void HoI4World::convertProvinceOwners(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, HoI4StateMapping& stateMap, V2Localisation& Vic2Localisations)
{
	// TODO - determine province owners for all HoI4 provinces
	Object* obj = parser_UTF8::doParseFile("Resources.txt");
	auto obj2 = obj->getValue("resources");
	auto obj3 = obj2[0]->getValue("link");
	auto obj4 = obj3[0]->getValue("province");
	Object* navalobj = parser_UTF8::doParseFile("navalprovinces.txt");
	auto navalobj2 = navalobj->getValue("navalprovinces");
	auto navalobj3 = navalobj2[0]->getValue("link");
	auto navalprovinces = navalobj3[0]->getValue("province");
	//	loop through the vic2 countries
	int stateID = 1;
	for (auto country : sourceWorld.getCountries())
	{

		//	determine the relevant HoI4 country
		string HoI4Tag = countryMap.GetHoI4Tag(country.first);
		double employedworkersadjusted = 0;
		for (auto vic2State : country.second->getStates())
		{
			for (auto prov : vic2State->getProvinces())
			{
				//takes employed workers and divides by 100,000 to convert
				V2Province* sourceProvince = sourceWorld.getProvince(prov);
				employedworkersadjusted += sourceProvince->getEmployedWorkers()*.00001;
			}
		}
		//calculating the conversion between Vic2Employed Workers to HoI4 factories
		double percentage = 0;
		double sinner = sin(employedworkersadjusted / 150) * 100;
		double logger = log10(employedworkersadjusted) * 15;
		double HoI4TotalFactories = sinner + logger + 5;
		HoI4Country* hoi4con = countries.find(HoI4Tag)->second;
		if (employedworkersadjusted != 0)
		{
			percentage = HoI4TotalFactories / employedworkersadjusted;
		}
		//	loop through the states in the vic2 country
		for (auto vic2State : country.second->getStates())
		{
			if (vic2State->getStateID() == "BRZ_2408")
			{
				int x;
				x = 6;
				Object* obj = parser_UTF8::doParseFile("Resources.txt");
			}
			bool lookfornavalbase = true;
			string resources = "";
			double stateWorkers = 0;
			double stateFac = 0;
			float population = 0;
			double raillevel = 0;
			double provinces = 0;
			int navalbase = 0;
			int navalbaselocation = 0;
			for (auto prov : vic2State->getProvinces())
			{

				//gets population, raillevel, and workers in every state to convert slots and states*conversion percentage
				V2Province* sourceProvince = sourceWorld.getProvince(prov);
				if (sourceProvince->getNavalBase() > 0 && prov < 2800)
				{
					navalbase += sourceProvince->getNavalBase();
					auto loc = inverseProvinceMap.find(prov);
					if (loc->second.size() < 100)
					{
						for (auto HoI4ProvNum : loc->second)
						{
							if (HoI4ProvNum < 14000 && lookfornavalbase == true)
							{
								for (auto navalprov : navalprovinces)
								{
									string navalprovince = navalprov->getLeaf();
									if (navalprovince == to_string(HoI4ProvNum))
									{
										navalbaselocation = HoI4ProvNum;
										lookfornavalbase = false;
										break;
									}
								}
							}
						}
					}
				}
				population += sourceProvince->getLiteracyWeightedPopulation();
				raillevel = sourceProvince->getInfra();
				stateWorkers += sourceProvince->getEmployedWorkers()*.00001*percentage;
				provinces++;
			}
			//used to make sure no negative factories
			if (stateWorkers < 0)
				stateWorkers = 0;

			//slots is given per 120,000 people (need to change)
			int stateSlots = population / 120000;
			//make sure not larger then 12 so stateFac is properly limited to max state level
			if (stateSlots > 12)
				stateSlots = 12;
			stateFac = round(stateWorkers);
			//limits factories by max slots
			if (stateFac > 12)
				stateFac = 12;
			if (stateFac >= stateSlots)
				stateSlots = stateFac + 2;
			//better rails for better industry
			if (stateFac > 10)
				raillevel += 3;
			else if (stateFac > 6)
				raillevel += 2;
			else if (stateFac > 4)
				raillevel++;
			string catagory = "";
			//names of city size to give correct number of slots
			if (stateSlots >= 12)
				catagory = "megalopolis";
			else if (stateSlots >= 10)
				catagory = "metropolis";
			else if (stateSlots >= 8)
				catagory = "large_city";
			else if (stateSlots >= 6)
				catagory = "city";
			else if (stateSlots >= 5)
				catagory = "large_town";
			else if (stateSlots >= 4)
				catagory = "town";
			else if (stateSlots >= 2)
				catagory = "rural";
			else if (stateSlots >= 1)
				catagory = "pastoral";
			else
				catagory = "enclave";

			int civFac = 0;
			int milFac = 0;
			//0-6 gives a civ factory, 7-9 gives mil factory
			for (int i = 0; i < stateFac; i++)
			{
				int randnmb = rand() % 10;
				if (randnmb > 6)
					milFac++;
				else
					civFac++;
			}
			//	create a matching HoI4 state
			int provincecount = 0;
			float newManpower = 1;
			for (auto prov : vic2State->getProvinces())
			{
				V2Province* sourceProvince = sourceWorld.getProvince(prov);
				newManpower += sourceProvince->getTotalPopulation() * 4;
			}
			if (newManpower <= 0)
			{
				newManpower = 1;
			}

			HoI4State* newState = new HoI4State(stateID, HoI4Tag, newManpower, civFac, milFac, catagory, raillevel, navalbase, navalbaselocation);

			//	loop through the provinces in the vic2 state
			for (auto vic2Province : vic2State->getProvinces())
			{
				//	TODO - if the matching HoI4 provinces are owned by this country, add it to the HoI4 state
				auto provMapping = inverseProvinceMap.find(vic2Province);
				if (provMapping != inverseProvinceMap.end())
				{
					for (auto HoI4ProvNum : provMapping->second)
					{
						for (auto itr : obj3)
						{
							auto keys = itr->getLeaves();
							auto province = keys[0]->getLeaf();
							if (to_string(HoI4ProvNum) == province)
							{
								for (int i = 1; i < keys.size(); i++)
								{
									auto key = keys[i]->getKey();
									auto value = keys[i]->getLeaf();
									resources += key + " = " + value + "\r\n";

								}
								newState->setResources(resources);
							}

						}
						if (HoI4ProvNum != 0)
						{
							newState->addProvince(HoI4ProvNum);
							stateMap.insert(make_pair(HoI4ProvNum, stateID));
							provincecount++;
						}
					}
				}
			}

			//	if the state is not empty, add it to this list of states
			if (provincecount != 0)
			{
				localisation.addStateLocalisation(stateID, vic2State->getStateID(), Vic2Localisations);
				states.insert(make_pair(stateID, newState));
				stateID++;
			}
		}
	}

	//for (auto provItr : provinces)
	//{
	//	// get the appropriate mapping
	//	provinceMapping::const_iterator provinceLink = provinceMap.find(provItr.first);
	//	if ((provinceLink == provinceMap.end()) || (provinceLink->second.size() == 0))
	//	{
	//		LOG(LogLevel::Warning) << "No source for " << provItr.second->getName() << " (province " << provItr.first << ')';
	//		continue;
	//	}
	//	else if (provinceLink->second[0] == 0)
	//	{
	//		continue;
	//	}

	//	provItr.second->clearCores();

	//	V2Province*	oldProvince = NULL;
	//	V2Country*	oldOwner = NULL;

	//	// determine ownership by province count, or total population (if province count is tied)
	//	map<string, MTo1ProvinceComp> provinceBins;
	//	double newProvinceTotalPop = 0;
	//	for (auto srcProvItr : provinceLink->second)
	//	{
	//		V2Province* srcProvince = sourceWorld.getProvince(srcProvItr);
	//		if (!srcProvince)
	//		{
	//			LOG(LogLevel::Warning) << "Old province " << provinceLink->second[0] << " does not exist (bad mapping?)";
	//			continue;
	//		}
	//		V2Country* owner = srcProvince->getOwner();
	//		string tag;
	//		if (owner != NULL)
	//		{
	//			tag = owner->getTag();
	//		}
	//		else
	//		{
	//			tag = "";
	//		}

	//		if (provinceBins.find(tag) == provinceBins.end())
	//		{
	//			provinceBins[tag] = MTo1ProvinceComp();
	//		}
	//		provinceBins[tag].provinces.push_back(srcProvince);
	//		provinceBins[tag].totalPopulation += srcProvince->getTotalPopulation();
	//		newProvinceTotalPop += srcProvince->getTotalPopulation();
	//		// I am the new owner if there is no current owner, or I have more provinces than the current owner,
	//		// or I have the same number of provinces, but more population, than the current owner
	//		if ((oldOwner == NULL)
	//			|| (provinceBins[tag].provinces.size() > provinceBins[oldOwner->getTag()].provinces.size())
	//			|| ((provinceBins[tag].provinces.size() == provinceBins[oldOwner->getTag()].provinces.size())
	//				&& (provinceBins[tag].totalPopulation > provinceBins[oldOwner->getTag()].totalPopulation)))
	//		{
	//			oldOwner = owner;
	//			oldProvince = srcProvince;
	//		}
	//	}
	//	if (oldOwner == NULL)
	//	{
	//		provItr.second->setOwner("");
	//		continue;
	//	}

	//	// convert from the source provinces
	//	const string HoI4Tag = countryMap[oldOwner->getTag()];
	//	if (HoI4Tag.empty())
	//	{
	//		LOG(LogLevel::Warning) << "Could not map provinces owned by " << oldOwner->getTag();
	//	}
	//	else
	//	{
	//		provItr.second->setOwner(HoI4Tag);
	//		map<string, HoI4Country*>::iterator ownerItr = countries.find(HoI4Tag);
	//		if (ownerItr != countries.end())
	//		{
	//			ownerItr->second->addProvince(provItr.second);
	//		}
	//		provItr.second->convertFromOldProvince(oldProvince);

	//		for (auto srcOwnerItr : provinceBins)
	//		{
	//			for (auto srcProvItr : srcOwnerItr.second.provinces)
	//			{
	//				// convert cores
	//				vector<V2Country*> oldCores = srcProvItr->getCores(sourceWorld.getCountries());
	//				for (auto oldCoreItr : oldCores)
	//				{
	//					// skip this core if the country is the owner of the V2 province but not the HoI4 province
	//					// (i.e. "avoid boundary conflicts that didn't exist in V2").
	//					// this country may still get core via a province that DID belong to the current HoI4 owner
	//					if ((oldCoreItr->getTag() == srcOwnerItr.first) && (oldCoreItr != oldOwner))
	//					{
	//						continue;
	//					}

	//					const string coreOwner = countryMap[oldCoreItr->getTag()];
	//					if (coreOwner != "")
	//					{
	//						provItr.second->addCore(coreOwner);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}


void HoI4World::convertNavalBases(const V2World &sourceWorld, const inverseProvinceMapping& inverseProvinceMap)
{
	// convert naval bases. There should only be one per Vic2 naval base
	for (auto mapping : inverseProvinceMap)
	{
		auto sourceProvince = sourceWorld.getProvince(mapping.first);
		int navalBaseLevel = sourceProvince->getNavalBase();
		navalBaseLevel = max(0, (navalBaseLevel - 3) * 2 + 1);
		if (mapping.second.size() > 0)
		{
			auto destProvince = provinces.find(mapping.second[0]);
			if (destProvince != provinces.end())
			{
				destProvince->second->requireNavalBase(navalBaseLevel);
			}
		}
	}
}


void HoI4World::convertProvinceItems(const V2World& sourceWorld, const provinceMapping& provinceMap, const inverseProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
{
	for (auto state : states)
	{
		for (auto prov : state.second->getProvinces())
		{


		}
	}
	// now that all provinces have had owners and cores set, convert their other items
	for (auto mapping : inverseProvinceMap)
	{
		// get the source province
		int srcProvinceNum = mapping.first;
		V2Province* sourceProvince = sourceWorld.getProvince(srcProvinceNum);
		if (sourceProvince == NULL)
		{
			continue;
		}

		// convert items that apply to all destination provinces
		for (auto dstProvinceNum : mapping.second)
		{
			// get the destination province
			auto dstProvItr = provinces.find(dstProvinceNum);
			if (dstProvItr == provinces.end())
			{
				continue;
			}

			// source provinces from other owners should not contribute to the destination province
			if (countryMap[sourceProvince->getOwnerString()] != dstProvItr->second->getOwner())
			{
				continue;
			}

			// determine if this is a border province or not
			bool borderProvince = false;
			if (HoI4AdjacencyMap.size() > static_cast<unsigned int>(dstProvinceNum))
			{
				const vector<adjacency> adjacencies = HoI4AdjacencyMap[dstProvinceNum];
				for (auto adj : adjacencies)
				{
					auto province = provinces.find(dstProvinceNum);
					auto adjacentProvince = provinces.find(adj.to);
					if ((province != provinces.end()) && (adjacentProvince != provinces.end()) && (province->second->getOwner() != adjacentProvince->second->getOwner()))
					{
						borderProvince = true;
						break;
					}
				}
			}

			// convert forts, naval bases, and infrastructure
			int fortLevel = sourceProvince->getFort();
			fortLevel = max(0, (fortLevel - 5) * 2 + 1);
			if (dstProvItr->second->getNavalBase() > 0)
			{
				dstProvItr->second->requireCoastalFort(fortLevel);
			}
			if (borderProvince)
			{
				dstProvItr->second->requireLandFort(fortLevel);
			}
			dstProvItr->second->requireInfrastructure((int)Configuration::getMinInfra());
			if (sourceProvince->getInfra() > 0) // No infra stays at minInfra
			{
				dstProvItr->second->requireInfrastructure(sourceProvince->getInfra() + 4);
			}

			if ((Configuration::getLeadershipConversion() == "linear") || (Configuration::getLeadershipConversion() == "squareroot"))
			{
				dstProvItr->second->setLeadership(0.0);
			}
			if ((Configuration::getManpowerConversion() == "linear") || (Configuration::getManpowerConversion() == "squareroot"))
			{
				dstProvItr->second->setManpower(0.0);
			}
			if ((Configuration::getIcConversion() == "squareroot") || (Configuration::getIcConversion() == "linear") || (Configuration::getIcConversion() == "logarithmic"))
			{
				dstProvItr->second->setRawIndustry(0.0);
				dstProvItr->second->setActualIndustry(0);
			}
		}

		// convert items that apply to only one destination province
		if (mapping.second.size() > 0)
		{
			// get the destination province
			auto dstProvItr = provinces.find(mapping.second[0]);
			if (dstProvItr == provinces.end())
			{
				continue;
			}

			// convert industry
			double industry = sourceProvince->getEmployedWorkers();
			if (Configuration::getIcConversion() == "squareroot")
			{
				industry = sqrt(double(industry)) * 0.01294;
				dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
			}
			else if (Configuration::getIcConversion() == "linear")
			{
				industry = double(industry) * 0.0000255;
				dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
			}
			else if (Configuration::getIcConversion() == "logarithmic")
			{
				industry = log(max(1.0, industry / 70000)) / log(2) * 5.33;
				dstProvItr->second->addRawIndustry(industry * Configuration::getIcFactor());
			}

			// convert manpower
			double newManpower = sourceProvince->getPopulation("soldiers")
				+ sourceProvince->getPopulation("craftsmen") * 0.25 // Conscripts
				+ sourceProvince->getPopulation("labourers") * 0.25 // Conscripts
				+ sourceProvince->getPopulation("farmers") * 0.25; // Conscripts
			if (Configuration::getManpowerConversion() == "linear")
			{
				newManpower *= 0.00003 * Configuration::getManpowerFactor();
				newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
				dstProvItr->second->addManpower(newManpower);
			}
			else if (Configuration::getManpowerConversion() == "squareroot")
			{
				newManpower = sqrt(newManpower);
				newManpower *= 0.0076 * Configuration::getManpowerFactor();
				newManpower = newManpower + 0.005 < 0.01 ? 0 : newManpower;	// Discard trivial amounts
				dstProvItr->second->addManpower(newManpower);
			}

			// convert leadership
			double newLeadership = sourceProvince->getLiteracyWeightedPopulation("clergymen") * 0.5
				+ sourceProvince->getPopulation("officers")
				+ sourceProvince->getLiteracyWeightedPopulation("clerks") // Clerks representing researchers
				+ sourceProvince->getLiteracyWeightedPopulation("capitalists") * 0.5
				+ sourceProvince->getLiteracyWeightedPopulation("bureaucrats") * 0.25
				+ sourceProvince->getLiteracyWeightedPopulation("aristocrats") * 0.25;
			if (Configuration::getLeadershipConversion() == "linear")
			{
				newLeadership *= 0.00001363 * Configuration::getLeadershipFactor();
				newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
				dstProvItr->second->addLeadership(newLeadership);
			}
			else if (Configuration::getLeadershipConversion() == "squareroot")
			{
				newLeadership = sqrt(newLeadership);
				newLeadership *= 0.00147 * Configuration::getLeadershipFactor();
				newLeadership = newLeadership + 0.005 < 0.01 ? 0 : newLeadership;	// Discard trivial amounts
				dstProvItr->second->addLeadership(newLeadership);
			}
		}
	}
}


void HoI4World::convertTechs(const V2World& sourceWorld)
{
	map<string, vector<pair<string, int> > > techTechMap;
	map<string, vector<pair<string, int> > > invTechMap;

	// build tech maps - the code is ugly so the file can be pretty
	Object* obj = parser_UTF8::doParseFile("tech_mapping.txt");
	vector<Object*> objs = obj->getValue("tech_map");
	if (objs.size() < 1)
	{
		LOG(LogLevel::Error) << "Could not read tech map!";
		exit(1);
	}
	objs = objs[0]->getValue("link");
	for (auto itr : objs)
	{
		vector<string> keys = itr->getKeys();
		int status = 0; // 0 = unhandled, 1 = tech, 2 = invention
		vector<pair<string, int> > targetTechs;
		string tech = "";
		for (auto master : keys)
		{
			if ((status == 0) && (master == "v2_inv"))
			{
				tech = itr->getLeaf("v2_inv");
				status = 2;
			}
			else if ((status == 0) && (master == "v2_tech"))
			{
				tech = itr->getLeaf("v2_tech");
				status = 1;
			}
			else
			{
				int value = atoi(itr->getLeaf(master).c_str());
				targetTechs.push_back(pair<string, int>(master, value));
			}
		}
		switch (status)
		{
		case 0:
			LOG(LogLevel::Error) << "unhandled tech link with first key " << keys[0].c_str() << "!";
			break;
		case 1:
			techTechMap[tech] = targetTechs;
			break;
		case 2:
			invTechMap[tech] = targetTechs;
			break;
		}
	}


	for (auto dstCountry : countries)
	{
		const V2Country*	sourceCountry = dstCountry.second->getSourceCountry();
		vector<string>	techs = sourceCountry->getTechs();

		for (auto techName : techs)
		{
			auto mapItr = techTechMap.find(techName);
			if (mapItr != techTechMap.end())
			{
				for (auto HoI4TechItr : mapItr->second)
				{
					dstCountry.second->setTechnology(HoI4TechItr.first, HoI4TechItr.second);
				}
			}
		}

		vector<string> srcInventions = sourceCountry->getInventions();
		for (auto invItr : srcInventions)
		{
			auto mapItr = invTechMap.find(invItr);
			if (mapItr == invTechMap.end())
			{
				continue;
			}
			else
			{
				for (auto HoI4TechItr : mapItr->second)
				{
					dstCountry.second->setTechnology(HoI4TechItr.first, HoI4TechItr.second);
				}
			}
		}
	}
}


static string CardinalToOrdinal(int cardinal)
{
	int hundredRem = cardinal % 100;
	int tenRem = cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}


vector<int> HoI4World::getPortProvinces(const vector<int>& locationCandidates)
{
	vector<int> newLocationCandidates;
	for (auto litr : locationCandidates)
	{
		map<int, HoI4Province*>::const_iterator provinceItr = provinces.find(litr);
		if ((provinceItr != provinces.end()) && (provinceItr->second->hasNavalBase()))
		{
			newLocationCandidates.push_back(litr);
		}
	}

	return newLocationCandidates;
}


unitTypeMapping HoI4World::getUnitMappings()
{
	// parse the mapping file
	map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMap; // <vic, hoi>
	Object* obj = parser_UTF8::doParseFile("unit_mapping.txt");
	vector<Object*> leaves = obj->getLeaves();
	if (leaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No unit mapping definitions loaded.";
		exit(-1);
	}

	// figure out which set of mappings to use
	int modIndex = -1;
	int defaultIndex = 0;
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ((Configuration::getVic2Mods().size() > 0) && (Configuration::getVic2Mods()[0] == key))
		{
			modIndex = i;
		}
		if (key == "default")
		{
			defaultIndex = i;
		}
	}
	if (modIndex != -1)
	{
		leaves = leaves[modIndex]->getLeaves();
	}
	else
	{
		leaves = leaves[defaultIndex]->getLeaves();
	}

	// read the mappings
	for (auto leaf : leaves)
	{
		vector<Object*> vicKeys = leaf->getValue("vic");
		if (vicKeys.size() < 1)
		{
			LOG(LogLevel::Error) << "invalid unit mapping(no source).";
		}
		else
		{
			// multimap allows multiple mapping and ratio mapping (e.g. 4 irregulars converted to 3 militia brigades and 1 infantry brigade)
			multimap<HoI4RegimentType, unsigned> hoiList;
			vector<Object*> hoiKeys = leaf->getValue("hoi0");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 0));
			}

			hoiKeys = leaf->getValue("hoi1");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 1));
			}

			hoiKeys = leaf->getValue("hoi2");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 2));
			}

			hoiKeys = leaf->getValue("HoI4");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 3));
			}

			hoiKeys = leaf->getValue("hoi4");
			for (auto hoiKey : hoiKeys)
			{
				hoiList.insert(make_pair(HoI4RegimentType(hoiKey->getLeaf()), 4));
			}

			for (auto vicKey : vicKeys)
			{
				if (unitTypeMap.find(vicKey->getLeaf()) == unitTypeMap.end())
				{
					unitTypeMap[vicKey->getLeaf()] = hoiList;
				}
				else
				{
					for (auto listItr : hoiList)
					{
						unitTypeMap[vicKey->getLeaf()].insert(listItr);
					}
				}
			}
		}
	}

	return unitTypeMap;
}


vector<int> HoI4World::getPortLocationCandidates(const vector<int>& locationCandidates, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
{
	vector<int> portLocationCandidates = getPortProvinces(locationCandidates);
	if (portLocationCandidates.size() == 0)
	{
		// if none of the mapped provinces are ports, try to push the navy out to sea
		for (auto candidate : locationCandidates)
		{
			if (HoI4AdjacencyMap.size() > static_cast<unsigned int>(candidate))
			{
				auto newCandidates = HoI4AdjacencyMap[candidate];
				for (auto newCandidate : newCandidates)
				{
					auto candidateProvince = provinces.find(newCandidate.to);
					if (candidateProvince == provinces.end())	// if this was not an imported province but has an adjacency, we can assume it's a sea province
					{
						portLocationCandidates.push_back(newCandidate.to);
					}
				}
			}
		}
	}
	return portLocationCandidates;
}


int HoI4World::getAirLocation(HoI4Province* locationProvince, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string owner)
{
	queue<int>		openProvinces;
	map<int, int>	closedProvinces;
	openProvinces.push(locationProvince->getNum());
	closedProvinces.insert(make_pair(locationProvince->getNum(), locationProvince->getNum()));
	while (openProvinces.size() > 0)
	{
		int provNum = openProvinces.front();
		openProvinces.pop();

		auto province = provinces.find(provNum);
		if ((province != provinces.end()) && (province->second->getOwner() == owner) && (province->second->getAirBase() > 0))
		{
			return provNum;
		}
		else
		{
			auto adjacencies = HoI4AdjacencyMap[provNum];
			for (auto thisAdjacency : adjacencies)
			{
				auto closed = closedProvinces.find(thisAdjacency.to);
				if (closed == closedProvinces.end())
				{
					openProvinces.push(thisAdjacency.to);
					closedProvinces.insert(make_pair(thisAdjacency.to, thisAdjacency.to));
				}
			}
		}
	}

	return -1;
}


vector<HoI4Regiment*> HoI4World::convertRegiments(const unitTypeMapping& unitTypeMap, vector<V2Regiment*>& sourceRegiments, map<string, unsigned>& typeCount, const pair<string, HoI4Country*>& country)
{
	vector<HoI4Regiment*> destRegiments;

	for (auto regItr : sourceRegiments)
	{
		HoI4Regiment* destReg = new HoI4Regiment();
		destReg->setName(regItr->getName());

		auto typeMap = unitTypeMap.find(regItr->getType());
		if (typeMap == unitTypeMap.end())
		{
			LOG(LogLevel::Debug) << "Regiment " << regItr->getName() << " has unmapped unit type " << regItr->getType() << ", dropping.";
			continue;
		}
		else if (typeMap->second.empty()) // Silently skip the ones that purposefully have no mapping
		{
			continue;
		}

		const multimap<HoI4RegimentType, unsigned>& hoiMapList = typeMap->second;
		unsigned destMapIndex = typeCount[regItr->getType()]++ % hoiMapList.size();
		multimap<HoI4RegimentType, unsigned>::const_iterator destTypeItr = hoiMapList.begin();

		// Count down from destMapIndex to iterate to the correct destination type
		for (; destMapIndex > 0 && destTypeItr != hoiMapList.end(); --destMapIndex)
		{
			++destTypeItr;
		}

		if (!destTypeItr->first.getUsableBy().empty()) // This unit type is exclusive
		{
			unsigned skippedCount = 0;
			while (skippedCount < hoiMapList.size())
			{
				const set<string> &usableByCountries = destTypeItr->first.getUsableBy();

				if (usableByCountries.empty() || usableByCountries.find(country.first) != usableByCountries.end())
				{
					break;
				}

				++skippedCount;
				++destTypeItr;
				++typeCount[regItr->getType()];
				if (destTypeItr == hoiMapList.end())
				{
					destTypeItr = hoiMapList.begin();
				}
			}

			if (skippedCount == hoiMapList.size())
			{
				LOG(LogLevel::Warning) << "Regiment " << regItr->getName() << " has unit type " << regItr->getType() << ", but it is mapped only to units exclusive to other countries. Dropping.";
				continue;
			}
		}

		destReg->setType(destTypeItr->first);
		destReg->setHistoricalModel(destTypeItr->second);
		destReg->setReserve(true);
		destRegiments.push_back(destReg);

		// Contribute to country's practicals
		country.second->getPracticals()[destTypeItr->first.getPracticalBonus()] += Configuration::getPracticalsScale() * destTypeItr->first.getPracticalBonusFactor();
	}

	return destRegiments;
}


HoI4RegGroup* HoI4World::createArmy(const inverseProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap, string tag, const V2Army* oldArmy, vector<HoI4Regiment*>& regiments, int& airForceIndex)
{
	HoI4RegGroup* destArmy = new HoI4RegGroup();

	// determine if an army or navy
	if (oldArmy->getNavy())
	{
		destArmy->setForceType(navy);
		destArmy->setAtSea(oldArmy->getAtSea());
	}
	else
	{
		destArmy->setForceType(land);
	}

	// get potential locations
	vector<int> locationCandidates = getHoI4ProvinceNums(inverseProvinceMap, oldArmy->getLocation());
	if (locationCandidates.size() == 0)
	{
		LOG(LogLevel::Warning) << "Army or Navy " << oldArmy->getName() << " assigned to unmapped province " << oldArmy->getLocation() << "; placing units in the production queue.";
		destArmy->setProductionQueue(true);
	}

	// guarantee that navies are assigned to sea provinces, or land provinces with naval bases
	if ((locationCandidates.size() > 0) && (oldArmy->getNavy()) && (!oldArmy->getAtSea()))
	{
		map<int, HoI4Province*>::const_iterator pitr = provinces.find(locationCandidates[0]);
		if (pitr != provinces.end() && pitr->second->isLand())
		{
			locationCandidates = getPortLocationCandidates(locationCandidates, HoI4AdjacencyMap);
			if (locationCandidates.size() == 0)
			{
				LOG(LogLevel::Warning) << "Navy " << oldArmy->getName() << " assigned to V2 province " << oldArmy->getLocation() << " which has no corresponding HoI4 port provinces or adjacent sea provinces; placing units in the production queue.";
				destArmy->setProductionQueue(true);
			}
		}
	}

	// pick the actual location
	int selectedLocation;
	HoI4Province* locationProvince = NULL;
	if (locationCandidates.size() > 0)
	{
		selectedLocation = locationCandidates[rand() % locationCandidates.size()];
		destArmy->setLocation(selectedLocation);
		map<int, HoI4Province*>::iterator pitr = provinces.find(selectedLocation);
		if (pitr != provinces.end())
		{
			locationProvince = pitr->second;
		}
		else if (!oldArmy->getNavy())
		{
			destArmy->setProductionQueue(true);
		}
	}

	// handle navies
	if (oldArmy->getNavy())
	{
		for (auto regiment : regiments)
		{
			destArmy->addRegiment(*regiment, true);
		}
		return destArmy;
	}

	// air units need to be split into air forces, so create an air force regiment to hold any air units
	HoI4RegGroup destWing;
	destWing.setForceType(air);
	HoI4Province* airLocationProvince = NULL;
	if (!destArmy->getProductionQueue())
	{
		int airLocation = -1;
		if (locationProvince != NULL)
		{
			airLocation = getAirLocation(locationProvince, HoI4AdjacencyMap, tag);
		}
		destWing.setLocation(airLocation);
		map<int, HoI4Province*>::iterator pitr = provinces.find(airLocation);
		if (pitr != provinces.end())
		{
			airLocationProvince = pitr->second;
		}
		else
		{
			destWing.setProductionQueue(true);
		}
	}
	else
	{
		destWing.setProductionQueue(true);
	}

	// separate all air regiments into the air force
	vector<HoI4Regiment*> unprocessedRegiments;
	for (auto regiment : regiments)
	{
		// Add to army/navy or newly created air force as appropriate
		if (regiment->getForceType() == air)
		{
			destWing.addRegiment(*regiment, true);
		}
		else
		{
			unprocessedRegiments.push_back(regiment);
		}
	}
	if (!destWing.isEmpty())
	{
		stringstream name;
		name << ++airForceIndex << CardinalToOrdinal(airForceIndex) << " Air Force";
		destWing.setName(name.str());

		if (!destWing.getProductionQueue())
		{
			// make sure an airbase is waiting for them
			airLocationProvince->requireAirBase(min(10, airLocationProvince->getAirBase() + (destWing.size() * 2)));
		}
		destArmy->addChild(destWing, true);
	}
	regiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// put all 'armored' type regiments together
	HoI4RegGroup armored;
	armored.setForceType(land);
	armored.setLocation(selectedLocation);
	for (auto regiment : regiments)
	{
		// Add to army/navy or newly created air force as appropriate
		if ((regiment->getType().getName() == "light_armor_brigade") ||
			(regiment->getType().getName() == "armor_brigade") ||
			(regiment->getType().getName() == "armored_car_brigade") ||
			(regiment->getType().getName() == "tank_destroyer_brigade") ||
			(regiment->getType().getName() == "motorized_brigade")
			)
		{
			armored.addRegiment(*regiment, true);
		}
		else
		{
			unprocessedRegiments.push_back(regiment);
		}
	}
	if (!armored.isEmpty())
	{
		destArmy->addChild(armored, true);
	}
	regiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// put all 'specialist' type regiments together
	HoI4RegGroup specialist;
	specialist.setForceType(land);
	specialist.setLocation(selectedLocation);
	for (auto regiment : regiments)
	{
		// Add to army/navy or newly created air force as appropriate
		if ((regiment->getType().getName() == "bergsjaeger_brigade") ||
			(regiment->getType().getName() == "marine_brigade") ||
			(regiment->getType().getName() == "police_brigade")
			)
		{
			armored.addRegiment(*regiment, true);
		}
		else
		{
			unprocessedRegiments.push_back(regiment);
		}
	}
	if (!specialist.isEmpty())
	{
		destArmy->addChild(specialist, true);
	}
	regiments.swap(unprocessedRegiments);
	unprocessedRegiments.clear();

	// make infantry and militia divisions, with support regiments added
	while (true)
	{
		HoI4RegGroup newGroup;
		newGroup.setForceType(land);
		newGroup.setLocation(selectedLocation);

		// get the first regiment (required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "infantry_brigade") ||
				((*regiment)->getType().getName() == "militia_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}
		if (newGroup.isEmpty())
		{
			break;
		}

		// get the second regiment (not required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "infantry_brigade") ||
				((*regiment)->getType().getName() == "militia_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}

		// get the support first regiment (not required, non-engineer preferred)
		int numSupport = 0;
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "anti_air_brigade") ||
				((*regiment)->getType().getName() == "anti_tank_brigade") ||
				((*regiment)->getType().getName() == "artillery_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				numSupport++;
				break;
			}
		}

		// get the second support regiment (not required, non-engineer preferred)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "anti_air_brigade") ||
				((*regiment)->getType().getName() == "anti_tank_brigade") ||
				((*regiment)->getType().getName() == "artillery_brigade")
				)
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				numSupport++;
				break;
			}
		}

		// add engineers as needed and available
		if (numSupport < 2)
		{
			for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
			{
				if ((*regiment)->getType().getName() == "engineer_brigade")
				{
					newGroup.addRegiment(**regiment, true);
					regiments.erase(regiment);
					numSupport++;
					break;
				}
			}
		}
		if (numSupport < 2)
		{
			for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
			{
				if ((*regiment)->getType().getName() == "engineer_brigade")
				{
					newGroup.addRegiment(**regiment, true);
					regiments.erase(regiment);
					break;
				}
			}
		}

		destArmy->addChild(newGroup, true);
	}

	// put together cavalry units (allow engineers to be included)
	while (true)
	{
		HoI4RegGroup newGroup;
		newGroup.setForceType(land);
		newGroup.setLocation(selectedLocation);

		// get the first regiment (required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "cavalry_brigade"))
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}
		if (newGroup.isEmpty())
		{
			break;
		}

		// get the second regiment (required to continue)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if (((*regiment)->getType().getName() == "cavalry_brigade"))
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}
		if (newGroup.size() < 2)
		{
			destArmy->addChild(newGroup, true);
			break;
		}

		// get the first engineer regiment (not required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if ((*regiment)->getType().getName() == "engineer_brigade")
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}

		// get the second engineer regiment (not required)
		for (auto regiment = regiments.begin(); regiment != regiments.end(); regiment++)
		{
			if ((*regiment)->getType().getName() == "engineer_brigade")
			{
				newGroup.addRegiment(**regiment, true);
				regiments.erase(regiment);
				break;
			}
		}

		destArmy->addChild(newGroup, true);
	}

	if (regiments.size() > 0)
	{
		LOG(LogLevel::Warning) << "Leftover regiments in " << tag << "'s army " << oldArmy->getName() << ". Likely too many support units.";
		for (auto regiment : regiments)
		{
			HoI4RegGroup newGroup;
			newGroup.setForceType(land);
			newGroup.setLocation(selectedLocation);
			newGroup.addRegiment(*regiment, true);
			destArmy->addChild(newGroup, true);
		}
	}


	return destArmy;
}


void HoI4World::convertArmies(const V2World& sourceWorld, const inverseProvinceMapping& inverseProvinceMap, const HoI4AdjacencyMapping& HoI4AdjacencyMap)
{
	//unitTypeMapping unitTypeMap = getUnitMappings();

	//// define the headquarters brigade type
	//HoI4RegimentType hqBrigade("hq_brigade");

	//// convert each country's armies
	//for (auto country: countries)
	//{
	//	const V2Country* oldCountry = country.second->getSourceCountry();
	//	if (oldCountry == NULL)
	//	{
	//		continue;
	//	}

	//	int airForceIndex = 0;
	//	HoI4RegGroup::resetHQCounts();
	//	HoI4RegGroup::resetRegGroupNameCounts();

	//	// A V2 unit type counter to keep track of how many V2 units of this type were converted.
	//	// Used to distribute HoI4 unit types in case of multiple mapping
	//	map<string, unsigned> typeCount;

	//	// Convert actual armies
	//	for (auto oldArmy: oldCountry->getArmies())
	//	{
	//		// convert the regiments
	//		vector<HoI4Regiment*> regiments = convertRegiments(unitTypeMap, oldArmy->getRegiments(), typeCount, country);

	//		// place the regiments into armies
	//		HoI4RegGroup* army = createArmy(inverseProvinceMap, HoI4AdjacencyMap, country.first, oldArmy, regiments, airForceIndex);
	//		army->setName(oldArmy->getName());

	//		// add the converted units to the country
	//		if ((army->getForceType() == land) && (!army->isEmpty()) && (!army->getProductionQueue()))
	//		{
	//			army->createHQs(hqBrigade); // Generate HQs for all hierarchies
	//			country.second->addArmy(army);
	//		}
	//		else if (!army->isEmpty())
	//		{
	//			country.second->addArmy(army);
	//		}
	//	}

	//	// Anticipate practical points being awarded for completing the unit constructions
	//	for (auto armyItr: country.second->getArmies())
	//	{
	//		if (armyItr->getProductionQueue())
	//		{
	//			armyItr->undoPracticalAddition(country.second->getPracticals());
	//		}
	//	}
	//}
}


void HoI4World::checkManualFaction(const CountryMapping& countryMap, const vector<string>& candidateTags, string leader, const string& factionName)
{
	bool leaderSet = false;
	for (auto candidate : candidateTags)
	{
		// get HoI4 tag from V2 tag
		string hoiTag = countryMap[candidate];
		if (hoiTag.empty())
		{
			LOG(LogLevel::Warning) << "Tag " << candidate << " requested for " << factionName << " faction, but is unmapped!";
			continue;
		}

		// find HoI4 nation and ensure that it has land
		auto citr = countries.find(hoiTag);
		if (citr != countries.end())
		{
			if (citr->second->getProvinces().size() == 0)
			{
				LOG(LogLevel::Warning) << "Tag " << candidate << " requested for " << factionName << " faction, but is landless!";
			}
			else
			{
				LOG(LogLevel::Debug) << candidate << " added to " << factionName << " faction";
				citr->second->setFaction(factionName);
				if (leader == "")
				{
					leader = citr->first;
				}
				if (!leaderSet)
				{
					citr->second->setFactionLeader();
					leaderSet = true;
				}
			}
		}
		else
		{
			LOG(LogLevel::Warning) << "Tag " << candidate << " requested for " << factionName << " faction, but does not exist!";
		}
	}
}


void HoI4World::factionSatellites()
{
	// make sure that any vassals are in their master's faction
	const vector<HoI4Agreement>& agreements = diplomacy.getAgreements();
	for (auto agreement : agreements)
	{
		if (agreement.type == "vassa")
		{
			auto masterCountry = countries.find(agreement.country1);
			auto satelliteCountry = countries.find(agreement.country2);
			if ((masterCountry != countries.end()) && (masterCountry->second->getFaction() != "") && (satelliteCountry != countries.end()))
			{
				satelliteCountry->second->setFaction(masterCountry->second->getFaction());
			}
		}
	}
}


void HoI4World::setFactionMembers(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	// find faction memebers
	if (Configuration::getFactionLeaderAlgo() == "manua")
	{
		LOG(LogLevel::Info) << "Manual faction allocation requested.";
		checkManualFaction(countryMap, Configuration::getManualAxisFaction(), axisLeader, "axis");
		checkManualFaction(countryMap, Configuration::getManualAlliesFaction(), alliesLeader, "allies");
		checkManualFaction(countryMap, Configuration::getManualCominternFaction(), cominternLeader, "comintern");
	}
	else if (Configuration::getFactionLeaderAlgo() == "auto")
	{
		LOG(LogLevel::Info) << "Auto faction allocation requested.";

		const vector<string>& greatCountries = sourceWorld.getGreatCountries();
		for (auto countryItr : greatCountries)
		{
			auto itr = countries.find(countryMap[countryItr]);
			if (itr != countries.end())
			{
				HoI4Country* country = itr->second;
				const string government = country->getGovernment();
				const string ideology = country->getIdeology();
				if (
					(government == "national_socialism" || government == "fascist_republic" || government == "germanic_fascist_republic" ||
						government == "right_wing_republic" || government == "hungarian_right_wing_republic" || government == "right_wing_autocrat" ||
						government == "absolute_monarchy" || government == "imperia"
						) &&
					(ideology == "national_socialist" || ideology == "fascistic" || ideology == "paternal_autocrat")
					)
				{
					if (axisLeader == "")
					{
						axisLeader = itr->first;
						country->setFaction("axis");
						country->setFactionLeader();
					}
					else
					{
						// Check if ally of leader
						const set<string>& allies = country->getAllies();
						if (allies.find(axisLeader) != allies.end())
						{
							country->setFaction("axis");
						}
					}
				}
				else if (
					(government == "social_conservatism" || government == "constitutional_monarchy" || government == "spanish_social_conservatism" ||
						government == "market_liberalism" || government == "social_democracy" || government == "social_liberalism"
						) &&
					(ideology == "social_conservative" || ideology == "market_libera" || ideology == "social_libera" || ideology == "social_democrat")
					)
				{
					if (alliesLeader == "")
					{
						alliesLeader = itr->first;
						country->setFaction("allies");
						country->setFactionLeader();
					}
					else
					{
						// Check if ally of leader
						const set<string> &allies = country->getAllies();
						if (allies.find(alliesLeader) != allies.end())
						{
							country->setFaction("allies");
						}
					}
				}
				// Allow left_wing_radicals, absolute monarchy and imperial. Being more tolerant for great powers, because we want comintern to be powerful
				else if (
					(
						government == "left_wing_radicals" || government == "socialist_republic" || government == "federal_socialist_republic" ||
						government == "absolute_monarchy" || government == "imperia"
						) &&
					(ideology == "left_wing_radica" || ideology == "leninist" || ideology == "stalinist")
					)
				{
					if (cominternLeader == "")
					{
						cominternLeader = itr->first; // Faction leader
						country->setFaction("comintern");
						country->setFactionLeader();
					}
					else
					{
						// Check if ally of leader
						const set<string> &allies = country->getAllies();
						if (allies.find(alliesLeader) != allies.end())
						{
							country->setFaction("comintern");
						}
					}
				}
			}
			else
			{
				LOG(LogLevel::Error) << "V2 great power " << countryItr << " not found.";
			}
		}

		// Comintern get a boost to its membership for being internationalistic
		// Go through all stalinist, leninist countries and add them to comintern if they're not hostile to leader
		for (auto country : countries)
		{
			const string government = country.second->getGovernment();
			const string ideology = country.second->getIdeology();
			if (
				(government == "socialist_republic" || government == "federal_socialist_republic") &&
				(ideology == "left_wing_radica" || ideology == "leninist" || ideology == "stalinist")
				)
			{
				if (country.second->getFaction() == "") // Skip if already a faction member
				{
					if (cominternLeader == "")
					{
						cominternLeader = country.first; // Faction leader
						country.second->setFaction("comintern");
						country.second->setFactionLeader();
					}
					else
					{
						// Check if enemy of leader
						bool enemy = false;
						const map<string, HoI4Relations*>& relations = country.second->getRelations();
						auto relationItr = relations.find(cominternLeader);
						if (relationItr != relations.end() && relationItr->second->atWar())
						{
							enemy = true;
						}

						if (!enemy)
						{
							country.second->setFaction("comintern");
						}
					}
				}
			}
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Error: unrecognized faction algorithm \"" << Configuration::getFactionLeaderAlgo() << "\"!";
		exit(-1);
	}
}


void HoI4World::setAlignments()
{
	// set alignments
	for (auto country : countries)
	{
		const string countryFaction = country.second->getFaction();

		// force alignment for faction members
		if (countryFaction == "axis")
		{
			country.second->getAlignment()->alignToAxis();
		}
		else if (countryFaction == "allies")
		{
			country.second->getAlignment()->alignToAllied();
		}
		else if (countryFaction == "comintern")
		{
			country.second->getAlignment()->alignToComintern();
		}
		else
		{
			// scale for positive relations - 230 = distance from corner to circumcenter, 200 = max relations
			static const double positiveScale = (230.0 / 200.0);
			// scale for negative relations - 116 = distance from circumcenter to side opposite corner, 200 = max relations
			static const double negativeScale = (116.0 / 200.0);

			// weight alignment for non-members based on relations with faction leaders
			HoI4Alignment axisStart;
			HoI4Alignment alliesStart;
			HoI4Alignment cominternStart;
			if (axisLeader != "")
			{
				HoI4Relations* relObj = country.second->getRelations(axisLeader);
				if (relObj != NULL)
				{
					double axisRelations = relObj->getRelations();
					if (axisRelations >= 0.0)
					{
						axisStart.moveTowardsAxis(axisRelations * positiveScale);
					}
					else // axisRelations < 0.0
					{
						axisStart.moveTowardsAxis(axisRelations * negativeScale);
					}
				}
			}
			if (alliesLeader != "")
			{
				HoI4Relations* relObj = country.second->getRelations(alliesLeader);
				if (relObj != NULL)
				{
					double alliesRelations = relObj->getRelations();
					if (alliesRelations >= 0.0)
					{
						alliesStart.moveTowardsAllied(alliesRelations * positiveScale);
					}
					else // alliesRelations < 0.0
					{
						alliesStart.moveTowardsAllied(alliesRelations * negativeScale);
					}
				}
			}
			if (cominternLeader != "")
			{
				HoI4Relations* relObj = country.second->getRelations(cominternLeader);
				if (relObj != NULL)
				{
					double cominternRelations = relObj->getRelations();
					if (cominternRelations >= 0.0)
					{
						cominternStart.moveTowardsComintern(cominternRelations * positiveScale);
					}
					else // cominternRelations < 0.0
					{
						cominternStart.moveTowardsComintern(cominternRelations * negativeScale);
					}
				}
			}
			(*(country.second->getAlignment())) = HoI4Alignment::getCentroid(axisStart, alliesStart, cominternStart);
		}
	}
}


void HoI4World::configureFactions(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	setFactionMembers(sourceWorld, countryMap);
	factionSatellites(); // push satellites into the same faction as their parents
	setAlignments();
}


void HoI4World::generateLeaders(const leaderTraitsMap& leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap)
{
	for (auto country : countries)
	{
		country.second->generateLeaders(leaderTraits, namesMap, portraitMap);
	}
}
void HoI4World::calculateArmies(const inverseProvinceMapping& inverseProvinceMap)
{
	for (auto country : countries)
	{
		country.second->CalculateArmyDivisions(inverseProvinceMap);
	}
}

void HoI4World::consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap)
{
	double totalManpower = 0.0;
	double totalLeadership = 0.0;
	double totalIndustry = 0.0;
	for (auto countryItr : countries)
	{
		countryItr.second->consolidateProvinceItems(inverseProvinceMap, totalManpower, totalLeadership, totalIndustry);
	}

	double suggestedManpowerConst = 3302.8 * Configuration::getManpowerFactor() / totalManpower;
	LOG(LogLevel::Debug) << "Total manpower was " << totalManpower << ". Changing the manpower factor to " << suggestedManpowerConst << " would match vanilla HoI4.";

	double suggestedLeadershipConst = 212.0 * Configuration::getLeadershipFactor() / totalLeadership;
	LOG(LogLevel::Debug) << "Total leadership was " << totalLeadership << ". Changing the leadership factor to " << suggestedLeadershipConst << " would match vanilla HoI4.";

	double suggestedIndustryConst = 1746.0 * Configuration::getIcFactor() / totalIndustry;
	LOG(LogLevel::Debug) << "Total IC was " << totalIndustry << ". Changing the IC factor to " << suggestedIndustryConst << " would match vanilla HoI4.";
}


void HoI4World::convertVictoryPoints(const V2World& sourceWorld, const CountryMapping& countryMap)
{
	// all country capitals get five VP
	for (auto countryItr : countries)
	{
		auto capitalItr = countryItr.second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(5);
		}
	}

	// Great Power capitals get another five
	const std::vector<string>& greatCountries = sourceWorld.getGreatCountries();
	for (auto country : sourceWorld.getGreatCountries())
	{
		const std::string& HoI4Tag = countryMap[country];
		auto countryItr = countries.find(HoI4Tag);
		if (countryItr != countries.end())
		{
			auto capitalItr = countryItr->second->getCapital();
			if (capitalItr != NULL)
			{
				capitalItr->addPoints(5);
			}
		}
	}

	// alliance leaders get another ten
	auto countryItr = countries.find(axisLeader);
	if (countryItr != countries.end())
	{
		auto capitalItr = countryItr->second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(10);
		}
	}
	countryItr = countries.find(alliesLeader);
	if (countryItr != countries.end())
	{
		auto capitalItr = countryItr->second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(10);
		}
	}
	countryItr = countries.find(cominternLeader);
	if (countryItr != countries.end())
	{
		auto capitalItr = countryItr->second->getCapital();
		if (capitalItr != NULL)
		{
			capitalItr->addPoints(10);
		}
	}
}


void HoI4World::convertDiplomacy(const V2World& sourceWorld, const CountryMapping& countryMap)
{
	for (auto agreement : sourceWorld.getDiplomacy()->getAgreements())
	{
		string HoI4Tag1 = countryMap[agreement.country1];
		if (HoI4Tag1.empty())
		{
			continue;
		}
		string HoI4Tag2 = countryMap[agreement.country2];
		if (HoI4Tag2.empty())
		{
			continue;
		}

		map<string, HoI4Country*>::iterator HoI4Country1 = countries.find(HoI4Tag1);
		map<string, HoI4Country*>::iterator HoI4Country2 = countries.find(HoI4Tag2);
		if (HoI4Country1 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI4 country " << HoI4Tag1 << " used in diplomatic agreement doesn't exist";
			continue;
		}
		if (HoI4Country2 == countries.end())
		{
			LOG(LogLevel::Warning) << "HoI4 country " << HoI4Tag2 << " used in diplomatic agreement doesn't exist";
			continue;
		}

		// shared diplo types
		if ((agreement.type == "alliance") || (agreement.type == "vassa"))
		{
			// copy agreement
			HoI4Agreement HoI4a;
			HoI4a.country1 = HoI4Tag1;
			HoI4a.country2 = HoI4Tag2;
			HoI4a.start_date = agreement.start_date;
			HoI4a.type = agreement.type;
			diplomacy.addAgreement(HoI4a);

			if (agreement.type == "alliance")
			{
				HoI4Country1->second->editAllies().insert(HoI4Tag2);
				HoI4Country2->second->editAllies().insert(HoI4Tag1);
			}
		}
	}

	// Relations and guarantees
	for (auto country : countries)
	{
		for (auto relationItr : country.second->getRelations())
		{
			HoI4Agreement HoI4a;
			if (country.first < relationItr.first) // Put it in order to eliminate duplicate relations entries
			{
				HoI4a.country1 = country.first;
				HoI4a.country2 = relationItr.first;
			}
			else
			{
				HoI4a.country2 = relationItr.first;
				HoI4a.country1 = country.first;
			}

			HoI4a.value = relationItr.second->getRelations();
			HoI4a.start_date = date("1930.1.1"); // Arbitrary date
			HoI4a.type = "relation";
			diplomacy.addAgreement(HoI4a);

			if (relationItr.second->getGuarantee())
			{
				HoI4Agreement HoI4a;
				HoI4a.country1 = country.first;
				HoI4a.country2 = relationItr.first;
				HoI4a.start_date = date("1930.1.1"); // Arbitrary date
				HoI4a.type = "guarantee";
				diplomacy.addAgreement(HoI4a);
			}
			if (relationItr.second->getSphereLeader())
			{
				HoI4Agreement HoI4a;
				HoI4a.country1 = country.first;
				HoI4a.country2 = relationItr.first;
				HoI4a.start_date = date("1930.1.1"); // Arbitrary date
				HoI4a.type = "sphere";
				diplomacy.addAgreement(HoI4a);
			}
		}
	}

	// decrease neutrality for countries with unowned cores
	map<string, string> hasLoweredNeutrality;
	for (auto province : provinces)
	{
		for (auto core : province.second->getCores())
		{
			if (province.second->getOwner() != core)
			{
				auto repeat = hasLoweredNeutrality.find(core);
				if (repeat == hasLoweredNeutrality.end())
				{
					auto country = countries.find(core);
					if (country != countries.end())
					{
						country->second->lowerNeutrality(20.0);
						hasLoweredNeutrality.insert(make_pair(core, core));
					}
				}
			}
		}
	}
}
void HoI4World::createIdeologyFiles()
{
	Utils::copyFolder("NeededFiles/interface", "output/" + Configuration::getOutputName()+"/gfx");
	Utils::copyFolder("NeededFiles/ideologies", "output/" + Configuration::getOutputName() + "/common");
	std::string outputGraphicsFolder = "Output/" + Configuration::getOutputName() + "/interface";
	if (!Utils::TryCreateFolder(outputGraphicsFolder))
	{
		return;
	}
	Utils::TryCopyFile("NeededFiles/countrypoliticsview.gfx", "Output/" + Configuration::getOutputName() + "/interface/countrypoliticsview.gfx");
}
void HoI4World::copyFlags(const V2World &sourceWorld, const CountryMapping& countryMap)
{


	LOG(LogLevel::Debug) << "Copying flags";

	// Create output folders.
	std::string outputGraphicsFolder = "Output/" + Configuration::getOutputName() + "/gfx";
	if (!Utils::TryCreateFolder(outputGraphicsFolder))
	{
		return;
	}
	std::string outputFlagFolder = outputGraphicsFolder + "/flags";
	if (!Utils::TryCreateFolder(outputFlagFolder))
	{
		return;
	}

	const std::string folderPath = Configuration::getV2Path() + "/gfx/flags";
	for (auto country : sourceWorld.getCountries())
	{
		std::string V2Tag = country.first;
		V2Country* sourceCountry = country.second;
		std::string V2FlagFile = sourceCountry->getFlagFile();
		const std::string& HoI4Tag = countryMap[V2Tag];

		bool flagCopied = false;
		vector<string> mods = Configuration::getVic2Mods();
		for (auto mod : mods)
		{
			string sourceFlagPath = Configuration::getV2Path() + "/mod/" + mod + "/gfx/flags/" + V2FlagFile;
			if (Utils::DoesFileExist(sourceFlagPath))
			{
				std::string destFlagPath = outputFlagFolder + '/' + HoI4Tag + ".tga";
				flagCopied = Utils::TryCopyFile(sourceFlagPath, destFlagPath);
				if (flagCopied)
				{
					break;
				}
			}
		}
		if (!flagCopied)
		{
			std::string sourceFlagPath = folderPath + '/' + V2FlagFile;
			if (Utils::DoesFileExist(sourceFlagPath))
			{
				std::string destFlagPath = outputFlagFolder + '/' + HoI4Tag + ".tga";
				Utils::TryCopyFile(sourceFlagPath, destFlagPath);
			}
		}
	}
}


void HoI4World::recordAllLandProvinces()
{
	ifstream definitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	while (true)
	{
		string line;
		getline(definitions, line);
		int pos = line.find_first_of(';');
		if (pos == string::npos)
		{
			break;
		}
		int provNum = atoi(line.substr(0, pos).c_str());

		line = line.substr(pos + 1, line.length());
		int pos2 = line.find_first_of(';');
		line = line.substr(pos2 + 1, line.length());
		int pos3 = line.find_first_of(';');
		line = line.substr(pos3 + 1, line.length());
		int pos4 = line.find_first_of(';');
		line = line.substr(pos4 + 1, line.length());
		int pos5 = line.find_first_of(';');
		line = line.substr(0, pos5);

		if (line == "land")
		{
			landProvinces.insert(provNum);
		}
	}
}


void HoI4World::checkAllProvincesMapped(const provinceMapping& provinceMap)
{
	ifstream definitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	while (true)
	{
		string line;
		getline(definitions, line);
		int pos = line.find_first_of(';');
		if (pos == string::npos)
		{
			break;
		}
		int provNum = atoi(line.substr(0, pos).c_str());

		provinceMapping::const_iterator num = provinceMap.find(provNum);
		if (num == provinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for HoI4 province " << provNum;
		}
	}

	definitions.close();
}


void HoI4World::setAIFocuses(const AIFocusModifiers& focusModifiers)
{
	for (auto countryItr : countries)
	{
		countryItr.second->setAIFocuses(focusModifiers);
	}
}


void HoI4World::addMinimalItems(const inverseProvinceMapping& inverseProvinceMap)
{
	for (auto country : countries)
	{
		country.second->addMinimalItems(inverseProvinceMap);
	}
}
void HoI4World::setSphereLeaders(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	const vector<string>& greatCountries = sourceWorld.getGreatCountries();
	for (auto countryItr : greatCountries)
	{
		auto itr = countries.find(countryMap[countryItr]);
		if (itr != countries.end())
		{
			HoI4Country* country = itr->second;
			auto relations = country->getRelations();
			for (auto relation : relations)
			{
				if (relation.second->getSphereLeader())
				{
					string tag = relation.second->getTag();
					auto spheredcountry = countries.find(tag);
					if (spheredcountry != countries.end())
					{
						spheredcountry->second->setSphereLeader(country->getTag());
					}
				}
			}
		}
	}
}
string HoI4World::createAnnexEvent(HoI4Country* Annexer, HoI4Country* Annexed, int eventnumber)
{
	string Events;
	string annexername = Annexer->getSourceCountry()->getName();
	string annexedname = Annexed->getSourceCountry()->getName();
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "annex." + to_string(eventnumber) + "\r\n";
	Events += "	title = \"" + annexername + " Demands " + annexedname + "!\"\r\n";
	Events += "	desc = \"Today " + annexername + " sent an envoy to us with a proposition of an union. We are alone and in this world, and a union with " + annexername + " might prove to be fruiteful.";
	Events += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"\r\n";
	Events += "	picture = GFX_report_event_hitler_parade\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = { # Accept\r\n";
	Events += "		name = \"We accept the Union\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 30\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = -15\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size < 40 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 45\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "annex." + to_string(eventnumber + 1) + " }\r\n";//+1 accept
	Events += "		}\r\n";
	Events += "		custom_effect_tooltip = GAME_OVER_TT\r\n";
	Events += "	}\r\n";
	Events += "	option = { # Refuse\r\n";
	Events += "		name = \"We Refuse!\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 10 \r\n";
	Events += "\r\n";
	Events += "			modifier = {\r\n";
	Events += "				factor = 0\r\n";
	Events += "				GER = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 20\r\n";
	Events += "				GER = { has_army_size = { size < 30 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	//Events += "			add_opinion_modifier = { target = ROOT modifier = " + Annexer->getTag() + "_anschluss_rejected }\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "annex." + to_string(eventnumber + 2) + " }\r\n";//+2 refuse
	Events += "			if = { limit = { is_in_faction_with = " + Annexed->getTag() + " }\r\n";
	Events += "				remove_from_faction = " + Annexed->getTag() + "\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	Events += "\r\n";
	//Country Refuses!
	Events += "# Austria refuses Anschluss\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "annex." + to_string(eventnumber+2) + "\r\n";
	Events += "	title = \"" + annexedname + " Refuses!\"\r\n";
	Events += "	desc = \"" + annexedname + " Refused our proposed union! This is an insult to us that cannot go unanswered!\"\r\n";
	Events += "	picture = GFX_report_event_german_troops\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"It's time for war\"\r\n";
	Events += "		create_wargoal = {\r\n";
	Events += "				type = puppet_wargoal_focus\r\n";
	Events += "			target = " + Annexed->getTag() + "\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}";
	//accepts
	Events += "# Austrian Anschluss Completed\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "annex." + to_string(eventnumber+1) + "\r\n";
	Events += "	title = \"" + annexedname + " accepts!\"\r\n";
	Events += "	desc = \"" + annexedname + " accepted our proposed union, their added strength will push us to greatness!\"\r\n";
	Events += "	picture = GFX_report_event_german_speech\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"A stronger Union!\"\r\n";
	for (auto cstate : countriesStates.find(Annexed->getTag())->second)
	{
		Events += "		" + to_string(cstate) + " = {\r\n";
		Events += "			if = {\r\n";
		Events += "				limit = { is_owned_by = " + Annexed->getTag() + " }\r\n";
		Events += "				add_core_of = " + Annexer->getTag() + "\r\n";
		Events += "			}\r\n";
		Events += "		}\r\n";

	}
	Events += "\r\n";
	Events += "		annex_country = { target = " + Annexed->getTag() + " transfer_troops = yes }\r\n";
	Events += "		add_political_power = 50\r\n";
	Events += "		add_named_threat = { threat = 2 name = \"" + annexername + " annexed " + annexedname + "\" }\r\n";
	Events += "		set_country_flag = " + Annexed->getTag() + "_annexed\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	return Events;
}
string HoI4World::createSudatenEvent(HoI4Country* Annexer, HoI4Country* Annexed, int eventnumber, vector<int> claimedStates)
{
	string Events;
	//flesh out this event more, possibly make it so allies have a chance to help?
	string annexername = Annexer->getSourceCountry()->getName();
	string annexedname = Annexed->getSourceCountry()->getName();
	Events += "#Sudaten Events\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber) + "\r\n";
	Events += "	title = \"" + annexername + " Demands " + annexedname + "!\"\r\n";
	Events += "	desc = \"" + annexername + " has recently been making claims to our bordering states, saying that these states are full of " + Annexer->getSourceCountry()->getAdjective("english") + " people and that the territory should be given to them. Although it ";
	Events += "is true that recently our neighboring states have had an influx of " + Annexer->getSourceCountry()->getAdjective("english") + " people in the recent years, we cannot give up our lands because a few "+ Annexer->getSourceCountry()->getAdjective("english") +" settled down in our land. " ;
	Events += "In response " + annexername + " has called for a conference, demanding their territory in exchange for peace. How do we resond? ";
	Events += " Our people would be safe with the mighty army of " + annexername + " and we could possibly flourish with their established economy. Or we could refuse the union which would surely lead to war, but maybe we can hold them off!\"\r\n";
	Events += "	picture = GFX_report_event_hitler_parade\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = { # Accept\r\n";
	Events += "		name = \"We Accept\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 30\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = -15\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size < 40 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 45\r\n";
	Events += "				" + Annexer->getTag() + " = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 1) + " }\r\n";//+1 accept
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "	option = { # Refuse\r\n";
	Events += "		name = \"We Refuse!\"\r\n";
	Events += "		ai_chance = {\r\n";
	Events += "			base = 10 \r\n";
	Events += "\r\n";
	Events += "			modifier = {\r\n";
	Events += "				factor = 0\r\n";
	Events += "				GER = { has_army_size = { size > 39 } }\r\n";
	Events += "			}\r\n";
	Events += "			modifier = {\r\n";
	Events += "				add = 20\r\n";
	Events += "				GER = { has_army_size = { size < 30 } }\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "		" + Annexer->getTag() + " = {\r\n";
	//Events += "			add_opinion_modifier = { target = ROOT modifier = " + Annexer->getTag() + "_anschluss_rejected }\r\n";
	Events += "			country_event = { hours = 2 id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 2) + " }\r\n";//+2 refuse
	Events += "			if = { limit = { is_in_faction_with = " + Annexed->getTag() + " }\r\n";
	Events += "				remove_from_faction = " + Annexed->getTag() + "\r\n";
	Events += "			}\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	Events += "\r\n";
	//Country Refuses!
	Events += "# refuses Sudaten\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 2) + "\r\n";
	Events += "	title = \"" + annexedname + " Refuses!\"\r\n";
	Events += "	desc = \"" + annexedname + " Refused our proposed proposition! This is an insult to us that cannot go unanswered!\"\r\n";
	Events += "	picture = GFX_report_event_german_troops\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"It's time for war\"\r\n";
	Events += "		create_wargoal = {\r\n";
	Events += "				type = puppet_wargoal_focus\r\n";
	Events += "			target = " + Annexed->getTag() + "\r\n";
	Events += "		}\r\n";
	Events += "	}\r\n";
	Events += "}";
	//accepts
	Events += "#  Sudaten Completed\r\n";
	Events += "country_event = {\r\n";
	Events += "	id = " + Annexer->getTag() + "sudaten." + to_string(eventnumber + 1) + "\r\n";
	Events += "	title = \"" + annexedname + " accepts!\"\r\n";
	Events += "	desc = \"" + annexedname + " accepted our proposed demands, the added lands will push us to greatness!\"\r\n";
	Events += "	picture = GFX_report_event_german_speech\r\n";
	Events += "	\r\n";
	Events += "	is_triggered_only = yes\r\n";
	Events += "	\r\n";
	Events += "	option = {\r\n";
	Events += "		name = \"A stronger Union!\"\r\n";
	for (auto cstate : claimedStates)
	{
		Events += "		" + to_string(cstate) + " = { add_core_of = " + Annexer->getTag() + " }\r\n";
		Events += "		" + Annexer->getTag() + " = { transfer_state =  " + to_string(cstate) + " }\r\n";
	}
	Events += "		set_country_flag = " + Annexed->getTag() + "_demanded\r\n";
	Events += "	}\r\n";
	Events += "}\r\n";
	return Events;
}
void HoI4World::fillProvinceNeighbors()
{
	std::ifstream file("adj.txt");
	std::string str;
	while (std::getline(file, str))
	{
		//	char output[100];
		//myReadFile >> output;
		vector<string> parts;
		stringstream ss(str); // Turn the string into a stream.
		string tok;
		char delimiter = ';';
		//crashes
		while (getline(ss, tok, delimiter))
			parts.push_back(tok);
		vector<int> provneighbors;
		for (int i = 5; i < parts.size(); i++)
		{
			//crashes
			int neighborprov = stoi(parts[i]);
			provneighbors.push_back(neighborprov);
		}
		provinceNeighbors.insert(make_pair(stoi(parts[0]), provneighbors));
	}
	file.close();
}
string HoI4World::genericFocusTreeCreator(HoI4Country* CreatingCountry)
{
	string s;
//DOES NOT INCLUDE LAST BRACKET!
	s += "focus_tree = { \r\n";
	s += "	id = german_focus\r\n";
	s += "	\r\n";
	s += "	country = {\r\n";
	s += "		factor = 0\r\n";
	s += "		\r\n";
	s += "		modifier = {\r\n";
	s += "			add = 10\r\n";
	s += "			tag = " + CreatingCountry->getTag() + "\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	default = no\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = army_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
	s += "		x = 1\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			army_experience = 5\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = land_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = land_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = equipment_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_small_arms\r\n";
	s += "		prerequisite = { focus = army_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 0\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = infantry_weapons_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = infantry_weapons\r\n";
	s += "				category = artillery\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = motorization_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_motorized\r\n";
	s += "		prerequisite = { focus = army_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		bypass = { has_tech = motorised_infantry }\r\n";
	s += "		x = 2\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = motorized_bonus\r\n";
	s += "				bonus = 0.75\r\n";
	s += "				technology = motorised_infantry\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = doctrine_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_doctrines\r\n";
	s += "		prerequisite = { focus = army_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 1\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			army_experience = 5\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = land_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = land_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = equipment_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_artillery\r\n";
	s += "		prerequisite = { focus = equipment_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 0\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = infantry_artillery_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = infantry_weapons\r\n";
	s += "				category = artillery\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = mechanization_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_build_tank\r\n";
	s += "		prerequisite = { focus = motorization_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 2\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = motorized_bonus\r\n";
	s += "				ahead_reduction = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = motorized_equipment\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = doctrine_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_doctrines\r\n";
	s += "		prerequisite = { focus = doctrine_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 1\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			army_experience = 5\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = land_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = land_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = equipment_effort_3" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_artillery2\r\n";
	s += "		prerequisite = { focus = equipment_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 0\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = infantry_artillery_bonus\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				category = infantry_weapons\r\n";
	s += "				category = artillery\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = armor_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_army_tanks\r\n";
	s += "		prerequisite = { focus = mechanization_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 2\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = armor_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				category = armor\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = special_forces" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_special_forces\r\n";
	s += "		prerequisite = { focus = equipment_effort_3" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = doctrine_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = armor_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 1\r\n";
	s += "		y = 6\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = special_forces_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = paratroopers\r\n";
	s += "				technology = paratroopers2\r\n";
	s += "				technology = marines\r\n";
	s += "				technology = marines2\r\n";
	s += "				technology = tech_mountaineers\r\n";
	s += "				technology = tech_mountaineers2\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = aviation_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_build_airforce\r\n";
	s += "		x = 5\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "			if = { limit = { has_country_flag = aviation_effort_AB }\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = air_base\r\n";
	s += "					level = 2\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}			\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						NOT = {\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = air_base\r\n";
	s += "								size > 1\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				capital_scope = {\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = fighter_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_fighter\r\n";
	s += "		prerequisite = { focus = aviation_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = bomber_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 4\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = fighter_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				technology = early_fighter\r\n";
	s += "				technology = fighter1\r\n";
	s += "				technology = fighter2\r\n";
	s += "				technology = fighter3\r\n";
	s += "				technology = heavy_fighter1\r\n";
	s += "				technology = heavy_fighter2\r\n";
	s += "				technology = heavy_fighter3\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = bomber_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_bomber\r\n";
	s += "		prerequisite = { focus = aviation_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = fighter_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 6\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = bomber_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				technology = strategic_bomber1\r\n";
	s += "				technology = strategic_bomber2\r\n";
	s += "				technology = strategic_bomber3\r\n";
	s += "				category = tactical_bomber\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = aviation_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_doctrine\r\n";
	s += "		prerequisite = { focus = bomber_focus focus = fighter_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 5\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "			if = { limit = { has_country_flag = aviation_effort_2_AB }\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = air_base\r\n";
	s += "					level = 2\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name =  air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		completion_reward = {\r\n";
	s += "			air_experience = 25\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						NOT = {\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = air_base\r\n";
	s += "								size > 1\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_2_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					capital_scope = {\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = air_base\r\n";
	s += "							size > 1\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				capital_scope = {\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = air_base\r\n";
	s += "						level = 2\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}				\r\n";
	s += "					ROOT = { set_country_flag = aviation_effort_2_AB }\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name =  air_doc_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = air_doctrine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = CAS_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_CAS\r\n";
	s += "		prerequisite = { focus = aviation_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = motorization_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 4\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = CAS_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				category = cas_bomber\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = rocket_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_rocketry\r\n";
	s += "		prerequisite = { focus = aviation_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = infrastructure_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 5\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = jet_rocket_bonus\r\n";
	s += "				ahead_reduction = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				category = rocketry\r\n";
	s += "				category = jet_technology\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.25\r\n";
	s += "				always = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = NAV_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_air_naval_bomber\r\n";
	s += "		prerequisite = { focus = aviation_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		prerequisite = { focus = flexible_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 6\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = nav_bomber_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				category = naval_bomber\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = naval_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_naval_dockyard\r\n";
	s += "		x = 9\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		available = {\r\n";
	s += "			any_state = {\r\n";
	s += "				is_coastal = yes\r\n";
	s += "				is_controlled_by = ROOT\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			navy_experience = 25\r\n";
	s += "			add_extra_state_shared_building_slots = 3\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = dockyard\r\n";
	s += "				level = 3\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		\r\n";
	s += "		completion_reward = {\r\n";
	s += "			navy_experience = 25\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					NOT = {\r\n";
	s += "						any_owned_state = {\r\n";
	s += "							dockyard > 0\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = dockyard\r\n";
	s += "								size > 2\r\n";
	s += "								include_locked = yes\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					any_owned_state = {\r\n";
	s += "						is_coastal = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						is_coastal = yes\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_extra_state_shared_building_slots = 3\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = dockyard\r\n";
	s += "						level = 3\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				set_country_flag = naval_effort_built\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					NOT = { has_country_flag = naval_effort_built }\r\n";
	s += "					any_owned_state = {\r\n";
	s += "						dockyard > 0\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_owned_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						dockyard > 0\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_extra_state_shared_building_slots = 3\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = dockyard\r\n";
	s += "						level = 3\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				set_country_flag = naval_effort_built\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					NOT = { has_country_flag = naval_effort_built }\r\n";
	s += "					NOT = {\r\n";
	s += "						any_owned_state = {\r\n";
	s += "							free_building_slots = {\r\n";
	s += "								building = dockyard\r\n";
	s += "								size > 2\r\n";
	s += "								include_locked = yes\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				random_state = {\r\n";
	s += "					limit = {\r\n";
	s += "						controller = { tag = ROOT }\r\n";
	s += "						free_building_slots = {\r\n";
	s += "							building = dockyard\r\n";
	s += "							size > 2\r\n";
	s += "							include_locked = yes\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "					add_extra_state_shared_building_slots = 3\r\n";
	s += "					add_building_construction = {\r\n";
	s += "						type = dockyard\r\n";
	s += "						level = 3\r\n";
	s += "						instant_build = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = flexible_navy" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_build_navy\r\n";
	s += "		prerequisite = { focus = naval_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = large_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 8\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = sub_op_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				technology = convoy_interdiction_ti\r\n";
	s += "				technology = unrestricted_submarine_warfare\r\n";
	s += "				technology = wolfpacks\r\n";
	s += "				technology = advanced_submarine_warfare\r\n";
	s += "				technology = combined_operations_raiding\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = large_navy" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_doctrines_tactics\r\n";
	s += "		prerequisite = { focus = naval_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = flexible_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = fleet_in_being_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 2\r\n";
	s += "				category = fleet_in_being_tree\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = submarine_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_submarine\r\n";
	s += "		prerequisite = { focus = flexible_navy focus = large_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 8\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = ss_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = early_submarine\r\n";
	s += "				technology = basic_submarine\r\n";
	s += "				technology = improved_submarine\r\n";
	s += "				technology = advanced_submarine\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = cruiser_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_cruiser\r\n";
	s += "		prerequisite = { focus = large_navy focus = flexible_navy" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = cr_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = improved_light_cruiser\r\n";
	s += "				technology = advanced_light_cruiser\r\n";
	s += "				technology = improved_heavy_cruiser\r\n";
	s += "				technology = advanced_heavy_cruiser\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = destroyer_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_wolf_pack\r\n";
	s += "		prerequisite = { focus = submarine_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 8\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = dd_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = early_destroyer\r\n";
	s += "				technology = basic_destroyer\r\n";
	s += "				technology = improved_destroyer\r\n";
	s += "				technology = advanced_destroyer\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = capital_ships_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_navy_battleship\r\n";
	s += "		prerequisite = { focus = cruiser_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					OR = {\r\n";
	s += "						is_coastal = no\r\n";
	s += "						dockyard < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			navy_experience = 25\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = capital_ships_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				ahead_reduction = 1\r\n";
	s += "				uses = 1\r\n";
	s += "				technology = basic_battlecruiser\r\n";
	s += "				technology = basic_battleship\r\n";
	s += "				technology = improved_battleship\r\n";
	s += "				technology = advanced_battleship\r\n";
	s += "				technology = heavy_battleship\r\n";
	s += "				technology = heavy_battleship2\r\n";
	s += "				technology = early_carrier\r\n";
	s += "				technology = basic_carrier\r\n";
	s += "				technology = improved_carrier\r\n";
	s += "				technology = advanced_carrier\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = industrial_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_production\r\n";
	s += "		x = 13\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = industrial_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 1\r\n";
	s += "				category = industry\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 3\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				date < 1939.1.1\r\n";
	s += "				OR = { \r\n";
	s += "					# we dont want chinese minors to go crazy on slots early since they get eaten\r\n";
	s += "					tag = GXC \r\n";
	s += "					tag = YUN\r\n";
	s += "					tag = SHX\r\n";
	s += "					tag = XSM\r\n";
	s += "					tag = BEL\r\n";
	s += "					tag = LUX\r\n";
	s += "					tag = HOL\r\n";
	s += "					tag = DEN\r\n";
	s += "\r\n";
	s += "					# we also dont want tiny nations to go crazy with slots right away\r\n";
	s += "					num_of_controlled_states < 2\r\n";
	s += "				}				\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = construction_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_civ_factory\r\n";
	s += "		prerequisite = { focus = industrial_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = construction_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}					\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = industrial_complex\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = industrial_complex\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = industrial_complex\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = production_effort"+CreatingCountry->getTag()+"\r\n";
	s += "		icon = GFX_goal_generic_construct_mil_factory\r\n";
	s += "		prerequisite = { focus = industrial_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2			\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = production_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = arms_factory\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = arms_factory\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = arms_factory\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = construction_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_civ_factory\r\n";
	s += "		prerequisite = { focus = construction_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = construction_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = industrial_complex\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = industrial_complex\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = industrial_complex\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = production_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_mil_factory\r\n";
	s += "		prerequisite = { focus = production_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = production_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = arms_factory\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = arms_factory\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = arms_factory\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = infrastructure_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_infrastructure\r\n";
	s += "		prerequisite = { focus = construction_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = infrastructure_effort_tt\r\n";
	s += "				all_owned_state = {			\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = production_effort_3" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_mil_factory\r\n";
	s += "		prerequisite = { focus = production_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = production_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size < 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}					\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 1\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = arms_factory\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = arms_factory\r\n";
	s += "						size > 0\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = arms_factory\r\n";
	s += "										size > 0\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 1\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = arms_factory\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = infrastructure_effort_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_infrastructure\r\n";
	s += "		prerequisite = { focus = infrastructure_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = infrastructure_effort_tt\r\n";
	s += "				all_owned_state = {			\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size < 1\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = infrastructure\r\n";
	s += "				level = 1\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = infrastructure\r\n";
	s += "						size > 0\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = infrastructure\r\n";
	s += "										size > 0\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = infrastructure\r\n";
	s += "					level = 1\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = construction_effort_3" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_construct_civ_factory\r\n";
	s += "		prerequisite = { focus = infrastructure_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 2\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		bypass = {\r\n";
	s += "			custom_trigger_tooltip = {\r\n";
	s += "				tooltip = construction_effort_tt\r\n";
	s += "				all_owned_state = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size < 2\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		complete_tooltip = {\r\n";
	s += "			add_extra_state_shared_building_slots = 2\r\n";
	s += "			add_building_construction = {\r\n";
	s += "				type = industrial_complex\r\n";
	s += "				level = 2\r\n";
	s += "				instant_build = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			random_owned_state = {\r\n";
	s += "				limit = {\r\n";
	s += "					free_building_slots = {\r\n";
	s += "						building = industrial_complex\r\n";
	s += "						size > 1\r\n";
	s += "						include_locked = yes\r\n";
	s += "					}\r\n";
	s += "					OR = {\r\n";
	s += "						is_in_home_area = yes\r\n";
	s += "						NOT = {\r\n";
	s += "							owner = {\r\n";
	s += "								any_owned_state = {\r\n";
	s += "									free_building_slots = {\r\n";
	s += "										building = industrial_complex\r\n";
	s += "										size > 1\r\n";
	s += "										include_locked = yes\r\n";
	s += "									}\r\n";
	s += "									is_in_home_area = yes\r\n";
	s += "								}\r\n";
	s += "							}\r\n";
	s += "						}\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				add_extra_state_shared_building_slots = 2\r\n";
	s += "				add_building_construction = {\r\n";
	s += "					type = industrial_complex\r\n";
	s += "					level = 2\r\n";
	s += "					instant_build = yes\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = nuclear_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_wonderweapons\r\n";
	s += "		prerequisite = { focus = infrastructure_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 10\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = nuclear_bonus\r\n";
	s += "				ahead_reduction = 0.5\r\n";
	s += "				category = nuclear\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.25\r\n";
	s += "				always = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = extra_tech_slot" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_research\r\n";
	s += "		prerequisite = { focus = infrastructure_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 12\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_research_slot = 1\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	focus = {\r\n";
	s += "		id = extra_tech_slot_2" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_focus_research\r\n";
	s += "		prerequisite = { focus = extra_tech_slot" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			num_of_factories > 50\r\n";
	s += "		}\r\n";
	s += "		cancel_if_invalid = no\r\n";
	s += "		continue_if_invalid = yes\r\n";
	s += "		x = 12\r\n";
	s += "		y = 6\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_research_slot = 1\r\n";
	s += "		}\r\n";
	s += "	}	\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = secret_weapons" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_secret_weapon\r\n";
	s += "		prerequisite = { focus = infrastructure_effort_2" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 14\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_tech_bonus = {\r\n";
	s += "				name = secret_bonus\r\n";
	s += "				bonus = 0.5\r\n";
	s += "				uses = 4\r\n";
	s += "				category = electronics\r\n";
	s += "				category = nuclear\r\n";
	s += "				category = rocketry\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.25\r\n";
	s += "				always = yes\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = political_effort" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_demand_territory\r\n";
	s += "		x = 19\r\n";
	s += "		y = 0\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_political_power = 120\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = collectivist_ethos" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_national_unity #icon = GFX_goal_tripartite_pact\r\n";
	s += "		prerequisite = { focus = political_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = liberty_ethos" + CreatingCountry->getTag() + "}\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = fascism\r\n";
	s += "				has_government = communism\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 18\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 5\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				OR = {\r\n";
	s += "					is_historical_focus_on = yes\r\n";
	s += "					has_idea = neutrality_idea\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = collectivist_ethos_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = nationalism_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_support_fascism #icon = GFX_goal_tripartite_pact\r\n";
	s += "		prerequisite = { focus = collectivist_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = internationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = fascism\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 16\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 5\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 2\r\n";
	s += "				any_neighbor_country = {\r\n";
	s += "					is_major = yes\r\n";
	s += "					has_government = fascism\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = nationalism\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	focus = {\r\n";
	s += "		id = internationalism_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_support_communism #icon = GFX_goal_tripartite_pact\r\n";
	s += "		prerequisite = { focus = collectivist_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = nationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = communism\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 18\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 5\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 2\r\n";
	s += "				any_neighbor_country = {\r\n";
	s += "					is_major = yes\r\n";
	s += "					has_government = communism\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = internationalism\r\n";
	s += "		}\r\n";
	s += "	}	\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = liberty_ethos" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_support_democracy\r\n";
	s += "		prerequisite = { focus = political_effort" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = collectivist_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			OR = {\r\n";
	s += "				has_government = democratic\r\n";
	s += "				has_government = neutrality\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "		x = 20\r\n";
	s += "		y = 1\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 95\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0.1\r\n";
	s += "				any_neighbor_country = {\r\n";
	s += "					is_major = yes\r\n";
	s += "					OR = {\r\n";
	s += "						has_government = communism\r\n";
	s += "						has_government = fascism\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "				NOT = {\r\n";
	s += "					any_neighbor_country = {\r\n";
	s += "						is_major = yes\r\n";
	s += "						has_government = democratic\r\n";
	s += "					}\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = liberty_ethos_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = militarism" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_political_pressure\r\n";
	s += "		prerequisite = { focus = nationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 16\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_idea = neutrality_idea }\r\n";
	s += "				remove_ideas = neutrality_idea\r\n";
	s += "			}			\r\n";
	s += "			add_ideas = militarism_focus\r\n";
	s += "			army_experience = 20\r\n";
	s += "			set_rule = { can_send_volunteers = yes }\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = political_correctness" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_dangerous_deal\r\n";
	s += "		prerequisite = { focus = internationalism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 18\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_idea = neutrality_idea }\r\n";
	s += "				remove_ideas = neutrality_idea\r\n";
	s += "			}		\r\n";
	s += "			add_political_power = 200\r\n";
	s += "			add_ideas = idea_political_correctness\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = neutrality_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_neutrality_focus\r\n";
	s += "		prerequisite = { focus = liberty_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = interventionism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 20\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { NOT = { has_idea = neutrality_idea } }\r\n";
	s += "				add_ideas = neutrality_idea\r\n";
	s += "			}\r\n";
	s += "			add_political_power = 150\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = interventionism_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_political_pressure\r\n";
	s += "		prerequisite = { focus = liberty_ethos" + CreatingCountry->getTag() + " }\r\n";
	s += "		mutually_exclusive = { focus = neutrality_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 22\r\n";
	s += "		y = 2\r\n";
	s += "		cost = 10\r\n";
	s += "\r\n";
	s += "		ai_will_do = {\r\n";
	s += "			factor = 1\r\n";
	s += "			modifier = {\r\n";
	s += "				factor = 0\r\n";
	s += "				has_idea = neutrality_idea\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_idea = neutrality_idea }\r\n";
	s += "				remove_ideas = neutrality_idea\r\n";
	s += "			}	\r\n";
	s += "			set_rule = { can_send_volunteers = yes }\r\n";
	s += "			add_political_power = 150\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = military_youth" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_more_territorial_claims\r\n";
	s += "		prerequisite = { focus = militarism" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 16\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = military_youth_focus\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = fascism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = fascism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = communism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = communism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = deterrence" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_defence\r\n";
	s += "		prerequisite = { focus = neutrality_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 20\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = deterrence\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = volunteer_corps" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
	s += "		prerequisite = { focus = interventionism_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 22\r\n";
	s += "		y = 3\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = volunteer_corps_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = paramilitarism" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_military_sphere\r\n";
	s += "		prerequisite = { focus = military_youth" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 16\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = paramilitarism_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = indoctrination_focus" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_propaganda\r\n";
	s += "		prerequisite = { focus = political_correctness" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 18\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = indoctrination_focus\r\n";
	s += "			add_political_power = 150\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = foreign_expeditions" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_more_territorial_claims\r\n";
	s += "		prerequisite = { focus = volunteer_corps" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 22\r\n";
	s += "		y = 4\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = foreign_expeditions_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = why_we_fight" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_propaganda\r\n";
	s += "		prerequisite = { focus = foreign_expeditions focus = deterrence" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = { \r\n";
	s += "			OR = { \r\n";
	s += "				threat > 0.75 \r\n";
	s += "				has_defensive_war = yes \r\n";
	s += "			}\r\n";
	s += "		}\r\n";
	s += "\r\n";
	s += "		continue_if_invalid = yes\r\n";
	s += "		\r\n";
	s += "		x = 20\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = { NOT = { has_idea = neutrality_idea } }\r\n";
	s += "				set_rule = { can_create_factions = yes }\r\n";
	s += "			}\r\n";
	s += "			add_ideas = why_we_fight_focus\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = political_commissars" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_forceful_treaty\r\n";
	s += "		prerequisite = { focus = indoctrination_focus" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "		}\r\n";
	s += "		x = 18\r\n";
	s += "		y = 5\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = political_commissars_focus\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = fascism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = fascism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = { has_government = communism }\r\n";
	s += "				add_popularity = {\r\n";
	s += "					ideology = communism\r\n";
	s += "					popularity = 0.2\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "			add_political_power = 200\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "\r\n";
	s += "	focus = {\r\n";
	s += "		id = ideological_fanaticism" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_demand_territory\r\n";
	s += "		prerequisite = { focus = paramilitarism" + CreatingCountry->getTag() + " focus = political_commissars" + CreatingCountry->getTag() + " }\r\n";
	s += "		x = 17\r\n";
	s += "		y = 6\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			add_ideas = ideological_fanaticism_focus\r\n";
	s += "			set_rule = {\r\n";
	s += "				can_create_factions = yes\r\n";
	s += "			}\r\n";
	s += "			hidden_effect = {\r\n";
	s += "				set_rule = { can_use_kamikaze_pilots = yes }\r\n";
	s += "			}\r\n";
	s += "			custom_effect_tooltip = kamikaze_focus_tooltip\r\n";
	s += "		}\r\n";
	s += "	}\r\n";
	s += "	\r\n";
	s += "	focus = {\r\n";
	s += "		id = technology_sharing" + CreatingCountry->getTag() + "\r\n";
	s += "		icon = GFX_goal_generic_scientific_exchange\r\n";
	s += "		prerequisite = { focus = ideological_fanaticism" + CreatingCountry->getTag() + " focus = why_we_fight" + CreatingCountry->getTag() + " }\r\n";
	s += "		available = {\r\n";
	s += "			has_war = yes\r\n";
	s += "			is_in_faction = yes\r\n";
	s += "			OR = {\r\n";
	s += "				num_of_factories > 50\r\n";
	s += "				any_country = {\r\n";
	s += "					is_in_faction_with = ROOT\r\n";
	s += "					num_of_factories > 50\r\n";
	s += "				}\r\n";
	s += "			}\r\n";
	s += "		}		\r\n";
	s += "		x = 19\r\n";
	s += "		y = 7\r\n";
	s += "		cost = 10\r\n";
	s += "		completion_reward = {\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					original_research_slots < 3\r\n";
	s += "				}\r\n";
	s += "				add_research_slot = 1\r\n";
	s += "			}\r\n";
	s += "			if = {\r\n";
	s += "				limit = {\r\n";
	s += "					original_research_slots > 2\r\n";
	s += "				}\r\n";
	s += "				add_tech_bonus = {\r\n";
	s += "					name = electronics_bonus\r\n";
	s += "					bonus = 0.5\r\n";
	s += "					uses = 1\r\n";
	s += "					category = electronics\r\n";
	s += "				}\r\n";
	s += "				add_tech_bonus = {\r\n";
	s += "					name = industrial_bonus\r\n";
	s += "					bonus = 0.5\r\n";
	s += "					uses = 1\r\n";
	s += "					category = industry\r\n";
	s += "				}	\r\n";
	s += "				add_tech_bonus = {\r\n";
	s += "					name = infantry_weapons_bonus\r\n";
	s += "					bonus = 0.5\r\n";
	s += "					uses = 1\r\n";
	s += "					category = infantry_weapons\r\n";
	s += "					category = artillery\r\n";
	s += "				}				\r\n";
	s += "			}			\r\n";
	s += "		}\r\n";
	s += "	}	\r\n";
	s += "\r\n";
	return s;
}
void HoI4World::fillCountryIC()
{

	for (auto country : countries)
	{
		vector<int> countrystates;
		int countryIC = 0;
		for (auto state : states)
		{
			if (state.second->getOwner() == country.second->getTag())
			{
				countryIC += state.second->getMilFactories();
				countrystates.push_back(state.first);
			}
		}
		countriesStates.insert(make_pair(country.second->getTag(), countrystates));
		countriesIC.insert(make_pair(country.second->getTag(), countryIC));
	}
}
double HoI4World::getStrengthOverTime(HoI4Country* Country, double years)
{
	double strength = 0;
	strength = Country->getArmyStrength() + countriesIC.find(Country->getTag())->second*2.5*years+ countriesIC.find(Country->getTag())->second*2*.469*2.5*years;
	return strength;
}
void HoI4World::outputRelations()
{
	string opinion_modifiers;
	for (auto country : countries)
	{
		string countryrelation;
		for (auto country2 : countries)
		{
			if (country.second->getTag() != country2.second->getTag() && country.second->getRelations(country2.second->getTag()) != NULL &&country2.second->getRelations(country2.second->getTag()) != NULL)
			{
				opinion_modifiers += country.second->getTag() + "_" + country2.second->getTag() + " = {\r\n\tvalue = " + to_string(country.second->getRelations(country2.second->getTag())->getRelations()*1.5) + "\r\n}\r\n";
				countryrelation += "add_opinion_modifier = { target = " + country2.second->getTag() + " modifier = " + country.second->getTag() + "_" + country2.second->getTag() + "}\r\n";
			}
		}
		country.second->setRelations(countryrelation);
	}
	std::string outputcommon = "Output/" + Configuration::getOutputName() + "/common";
	if (!Utils::TryCreateFolder(outputcommon))
	{
		return;
	}

	std::string outputopinionfolder = "Output/" + Configuration::getOutputName() + "/common/opinion_modifiers";
	if (!Utils::TryCreateFolder(outputopinionfolder))
	{
		return;
	}
	Utils::copyFolder("NeededFiles/defines", "output/" + Configuration::getOutputName() + "/common");
	string filename("Output/" + Configuration::getOutputName() + "/common/opinion_modifiers/01_opinion_modifiers.txt");
	ofstream out;
	out.open(filename);
	{
		out << "opinion_modifiers = {\r\n";
		out << opinion_modifiers;
		out << "}\r\n";
	}
	out.close();
}
void HoI4World::thatsgermanWarCreator(const V2World &sourceWorld, const CountryMapping& countryMap)
{

	//FIX ALL FIXMES AND ADD CONQUEST GOAL
	//MAKE SURE THIS WORKS
	//IMPROVE
	//MAKE ARMY STRENGTH CALCS MORE ACCURATE!!
	fillCountryIC();
	LOG(LogLevel::Info) << "Filling province neighbors";
	fillProvinceNeighbors();
	LOG(LogLevel::Info) << "Creating Factions";
	Factions = CreateFactions(sourceWorld, countryMap);
	for (auto country : countries)
	{
		int i = 1;
		string FactionName;
		for (auto faction : Factions)
		{
			if (country.second->getTag() == faction.front()->getTag())
			{
				FactionName = to_string(i++);
			}
		}
		country.second->output(states.size(), states, Factions, FactionName);
	}
	bool fascismrelevant = false;
	bool communismrelevant = false;
	double WorldStrength = 0;
	//GetDistance(Factions.front().front(), Factions.back().back());
	//get World Strength
	LOG(LogLevel::Info) << "Filling Provinces";
	fillProvinces();
	string NFpath = "Output/" + Configuration::getOutputName() + "/common/national_focus";
	if (!Utils::TryCreateFolder(NFpath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common/national_focus\"";
		exit(-1);
	}
	string eventpath = "Output/" + Configuration::getOutputName() + "/events";
	if (!Utils::TryCreateFolder(eventpath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/events\"";
		exit(-1);
	}
	string filename("Output/AI.txt");
	ofstream out;
	out.open(filename);
	{
		for (auto Faction : Factions)
		{
			WorldStrength += GetFactionStrength(Faction);
		}
		out << WorldStrength << endl;
		//check relevancies
		for (auto Faction : Factions)
		{
			//this might need to change to add factions together
			HoI4Country* Leader = GetFactionLeader(Faction);
			if (Leader->getGovernment() == "absolute_monarchy" || Leader->getGovernment() == "fascism")
				if (GetFactionStrength(Faction) > WorldStrength*0.1)
					fascismrelevant = true;

			if (Leader->getGovernment() == "communism" || Leader->getGovernment() == "syndicalism")
				if (GetFactionStrength(Faction) > WorldStrength*0.1)
					communismrelevant = true;

		}
		if (fascismrelevant)
			out << "Fascism is Relevant" << endl;
		if (communismrelevant)
			out << "Communist is Relevant" << endl;
		out << endl;
		//time to do events for coms and fascs if they are relevant
		LOG(LogLevel::Info) << "Calculating Fasc/Com AI";
		for (auto Faction : Factions)
		{
			HoI4Country* Leader = GetFactionLeader(Faction);
			if ((Leader->getGovernment() == "fascism" || Leader->getGovernment() == "absolute_monarchy") && fascismrelevant)
			{
				LOG(LogLevel::Info) << "Calculating AI for " + Leader->getSourceCountry()->getName();
				out << Leader->getSourceCountry()->getName() << endl;
				vector<HoI4Country*> Targets;
				vector<HoI4Country*> Anchluss;
				vector<HoI4Country*> Sudaten;
				vector<HoI4Country*> DifficultTargets;
				vector<int> leaderProvs = getCountryProvinces(Leader);
				LOG(LogLevel::Info) << "Calculating Neighbors for " + Leader->getSourceCountry()->getName();
				map<string, HoI4Country*> Neighbors = findNeighbors(leaderProvs, Leader);
				set<string> Allies = Leader->getAllies();
				//should add method to look for cores you dont own
				//should add method to look for more allies

				//lets look for weak neighbors
				LOG(LogLevel::Info) << "Doing Neighbor calcs for " + Leader->getSourceCountry()->getName();
				for (auto neigh : Neighbors)
				{
					//lets check to see if they are our ally and not a great country
					if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !checkIfGreatCountry(neigh.second, sourceWorld, countryMap))
					{
						//if not, lets see their strength is at least < 20%
						if (getStrengthOverTime(neigh.second,1.5) < getStrengthOverTime(Leader,1.5)*0.2)
						{
							//they are very weak and we can get 1 of these countries in an anchluss
							//AnchlussTargets.push_back(neigh);
							Anchluss.push_back(neigh.second);
						
						}
						//if not, lets see their strength is at least < 60%
						else if (getStrengthOverTime(neigh.second, 1.5) < getStrengthOverTime(Leader, 1)*0.6 && getStrengthOverTime(neigh.second, 1) > getStrengthOverTime(Leader, 1)*0.2)
						{
							//they are weak and we can get 1 of these countries in sudaten deal
							//SudatenTargets.push_back(neigh);
							Sudaten.push_back(neigh.second);
				
						}
						//if not, lets see their strength is at least = to ours%
						else if (getStrengthOverTime(neigh.second, 1) < getStrengthOverTime(Leader, 1))
						{
							//EqualTargets.push_back(neigh);
							DifficultTargets.push_back(neigh.second);
						
						}
						//if not, lets see their strength is at least < 120%
						else if (getStrengthOverTime(neigh.second, 1) < getStrengthOverTime(Leader, 1)*1.2)
						{
							//StrongerTargets.push_back(neigh);
							DifficultTargets.push_back(neigh.second);

						}
						
					}
				}
				string Events;
				string s;
				map<string, vector<HoI4Country*>> TargetMap;
				vector<HoI4Country*> anchlussnan;
				vector<HoI4Country*> sudatennan;
				vector<HoI4Country*> nan;
				vector<HoI4Country*> fn;
				vector<HoI4Country*> man;
				vector<HoI4Country*> coup;
				int EventNumber = 0;
				vector<int> takenSpots;
				vector<int> takenSpotsy;
				int x = 22;
				takenSpots.push_back(x);
				//look through every anchluss and see its difficulty
				for (auto target : Anchluss)
				{
					string type;
					//outputs are
					//noactionneeded -  Can take target without any help
					//factionneeded - can take target and faction with attackers faction helping
					//morealliesneeded - can take target with more allies, comes with "newallies" in map
					//coup - cant take over, need to coup
					type = HowToTakeLand(target, Leader, 1.5);
					out << type + " " + target->getSourceCountry()->getName() << endl;
					if (type == "noactionneeded")
					{
						nan.push_back(target);
						anchlussnan.push_back(target);
					}
				}
				
				string FocusTree = genericFocusTreeCreator(Leader);
				if (nan.size() >= 1)
				{
					//if it can easily take these targets as they are not in an alliance, you can get annexation event
						
						if (nan.size() == 1)
						{
							x = 24;
							takenSpots.push_back(x);
						}
						if (nan.size() >= 2)
						{
							x = 25;
							takenSpots.push_back(x);
						}
						takenSpotsy.push_back(2);
						//Focus to increase fascist support and prereq for anschluss
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = The_third_way" + Leader->getTag() + "\r\n";
						FocusTree += "		icon = GFX_goal_support_fascism\r\n";
						FocusTree += "		text = \"The Third Way!\"\r\n";
						FocusTree += "		x = " + to_string(x) + "\r\n";
						FocusTree += "		y = 0\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						//FIXME 
						//Need to get Drift Defense to work
						//FocusTree += "			drift_defence_factor = 0.5\r\n";
						FocusTree += "			add_ideas = fascist_influence\r\n";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";

						//Focus to increase army support
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = mil_march" + Leader->getTag() + "\r\n";
						FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
						FocusTree += "		text = \"Establish Military March Day\"\r\n";
						FocusTree += "		prerequisite = { focus = The_third_way" + Leader->getTag() + " }\r\n";
						FocusTree += "		x = " + to_string(x) + "\r\n";
						FocusTree += "		y = 1\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						FocusTree += "			army_experience = 20\r\n";
						FocusTree += "		add_tech_bonus = { \r\n";
						FocusTree += "				bonus = 0.5\r\n";
						FocusTree += "				uses = 2\r\n";
						FocusTree += "				category = land_doctrine\r\n";
						FocusTree += "			}";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";
						Events += "add_namespace = " + Leader->getTag() + "annex\r\n";
						
						for (int i = 0; i < 2; i++)
						{
							if (i < nan.size())
							{
								//int x = i * 3;
								string annexername = Leader->getSourceCountry()->getName();
								string annexedname = nan[i]->getSourceCountry()->getName();
								int v1 = rand() % 5 + 1;
								int v2 = rand() % 5 + 1;
								//focus for anschluss
								FocusTree += "		focus = { \r\n";
								FocusTree += "		id = "+Leader->getTag() +"_anschluss_"+nan[i]->getTag()+"\r\n";
								FocusTree += "		icon = GFX_goal_anschluss\r\n";
								FocusTree += "		text = \"Union with "+annexedname+"\"\r\n";
								FocusTree += "		prerequisite = { focus = mil_march" + Leader->getTag() + " }\r\n";
								FocusTree += "		available = {\r\n";
								FocusTree += "			is_puppet = no\r\n";
								FocusTree += "		    date > 1937." + to_string(v1+5) + "." + to_string(v2+5) + "\r\n";
								FocusTree += "		}\r\n";
								FocusTree += "		\r\n";
								FocusTree += "		x = " + to_string(x + i*2) + "\r\n";
								FocusTree += "		y = 2\r\n";
								FocusTree += "		cost = 10\r\n";
								FocusTree += "		ai_will_do = {\r\n";
								FocusTree += "			factor = 10\r\n";
								FocusTree += "			modifier = {\r\n";
								FocusTree += "				factor = 0\r\n";
								FocusTree += "				date < 1937.6.6\r\n";
								FocusTree += "			}\r\n";
								FocusTree += "		}	\r\n";
								FocusTree += "		completion_reward = {\r\n";
								FocusTree += "			army_experience = 10\r\n";
								FocusTree += "			if = {\r\n";
								FocusTree += "				limit = {\r\n";
								FocusTree += "					country_exists = " + nan[i]->getTag() + "\r\n";
								FocusTree += "				}\r\n";
								FocusTree += "				" + nan[i]->getTag() + " = {\r\n";
								FocusTree += "					country_event = " + Leader->getTag() + "annex."+to_string(EventNumber)+"\r\n";
								FocusTree += "				}\r\n";
								FocusTree += "			}\r\n";
								FocusTree += "		}\r\n";
								FocusTree += "	}";

								//events
								
								Events += createAnnexEvent(Leader, nan[i], EventNumber);
								EventNumber += 3;
							}
						}
					nan.clear();

				}
				for (auto target : Sudaten)
				{
					string type;
					//outputs are
					//noactionneeded -  Can take target without any help
					//factionneeded - can take target and faction with attackers faction helping
					//morealliesneeded - can take target with more allies, comes with "newallies" in map
					//coup - cant take over, need to coup
					type = HowToTakeLand(target, Leader, 2.5);
					out << type + " " + target->getSourceCountry()->getName() << endl;
					if (type == "noactionneeded")
					{
						nan.push_back(target);
					}
				}
				if (nan.size() >= 1)
				{
					//if it can easily take these targets as they are not in an alliance, you can get annexation event
					
					//Focus to increase empire size more
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Expand_the_Reich" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_political_pressure\r\n";//something about claiming land
					FocusTree += "		text = \"Expand the Reich\"\r\n";
					if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
					{
						FocusTree += "		prerequisite = { ";
						for (int i = 0; i < 2; i++)
						{
							if (i < anchlussnan.size())
							{
								FocusTree += " focus = " + Leader->getTag() + "_anschluss_" + anchlussnan[i]->getTag() + " ";
							}
						}
						FocusTree += "\r\n }\r\n";
						FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
						FocusTree += "		y = 3\r\n";
						takenSpotsy.push_back(5);
					}
					else
					{
						int x = takenSpots.back();
						takenSpots.push_back(x);
						if (nan.size() == 1)
						{
							x += 2;
							takenSpots.push_back(x);
						}
						if (nan.size() >= 2)
						{
							x += 3;
							takenSpots.push_back(x);
						}
						FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
						FocusTree += "		y = 0\r\n";
						takenSpotsy.push_back(2);
					}
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "			add_named_threat = { threat = 2 name = \"Fascist Expansion\" }\r\n";//give some claims or cores
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
					for (int i = 0; i < 1; i++)
					{
						if (i < nan.size())
						{
							int x = i * 3;
							string annexername = Leader->getSourceCountry()->getName();
							string annexedname = nan[i]->getSourceCountry()->getName();
							int v1 = rand() % 8 + 1;
							int v2 = rand() % 8 + 1;
							//focus for sudaten
							FocusTree += "		focus = { \r\n";
							FocusTree += "		id = " + Leader->getTag() + "_sudaten_" + nan[i]->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_anschluss\r\n";
							FocusTree += "		text = \"Demand Territory from " + annexedname + "\"\r\n";
							FocusTree += "		prerequisite = { focus = Expand_the_Reich" + Leader->getTag() + " }\r\n";
							FocusTree += "		available = {\r\n";
							FocusTree += "			is_puppet = no\r\n";
							FocusTree += "		    date > 1938." + to_string(v1) + "." + to_string(v2) + "\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "		\r\n";
							if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
							{
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 4\r\n";
							}
							else
							{
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 1\r\n";
							}
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 10\r\n";
							FocusTree += "			modifier = {\r\n";
							FocusTree += "				factor = 0\r\n";
							FocusTree += "				date < 1937.6.6\r\n";
							FocusTree += "			}\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			army_experience = 10\r\n";
							FocusTree += "			if = {\r\n";
							FocusTree += "				limit = {\r\n";
							FocusTree += "					country_exists = " + nan[i]->getTag() + "\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				" + nan[i]->getTag() + " = {\r\n";
							FocusTree += "					country_event = " + Leader->getTag() + "sudaten." + to_string(EventNumber) + "\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "			}\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}";

							//FINISH HIM
							FocusTree += "		focus = { \r\n";
							FocusTree += "		id = " + Leader->getTag() + "_finish_" + nan[i]->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_territory_or_war\r\n";
							FocusTree += "		text = \"Fate of " + annexedname + "\"\r\n";
							FocusTree += "		prerequisite = { focus =  " + Leader->getTag() + "_sudaten_" + nan[i]->getTag() + " }\r\n";
							FocusTree += "		available = {\r\n";
							FocusTree += "			is_puppet = no\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "		\r\n";
							if (anchlussnan.size() == 1 || anchlussnan.size() >= 2)
							{
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 5\r\n";
							}
							else
							{
								FocusTree += "		x = " + to_string(takenSpots.back()) + "\r\n";
								FocusTree += "		y = 2\r\n";
							}
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 10\r\n";
							FocusTree += "			modifier = {\r\n";
							FocusTree += "				factor = 0\r\n";
							FocusTree += "				date < 1937.6.6\r\n";
							FocusTree += "			}\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "		create_wargoal = {\r\n";
							FocusTree += "				type = annex_everything\r\n";
							FocusTree += "			target = " + nan[i]->getTag() + "\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}";

							//events
							//find neighboring states
							vector<int> demandedstates;
							for (auto leaderprov : leaderProvs)
							{
								vector<int> thisprovNeighbors = provinceNeighbors.find(leaderprov)->second;
								for (int prov : thisprovNeighbors)
								{
									if (provincemap.find(prov) == provincemap.end())
									{
									}
									else
									{
										vector<string> stuff = provincemap.find(prov)->second;
										if (stuff.size() >= 1)
										{
											int statenumber = stoi(stuff.front());
											//string ownertag = "";
											auto ownertag = provincemap.find(prov);
											if (ownertag != provincemap.end())
											{
												vector<string> tags = ownertag->second;
												if (tags.size() >= 1)
												{
													if (tags[1] == nan[i]->getTag())
													{
									
															/* v does not contain x */
															demandedstates.push_back(statenumber);
														
													}
												}
											}
										}
									}
								}
							}
							Events += "add_namespace = " + Leader->getTag() + "sudaten\r\n";
							Events += createSudatenEvent(Leader, nan[0], EventNumber, demandedstates);
							EventNumber += 3;
						}
					}
					nan.clear();

				}
				//events for allies
				vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);
				if (takenSpots.size() == 0)
					takenSpots.push_back(22);
				if (newAllies.size() > 0)
				{
					//Focus to call summit, maybe have events from summit
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Fas_Summit" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Call for the Fascist Summit\"\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back()+3) + "\r\n";
					FocusTree += "		y = 0\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 0\r\n";
					FocusTree += "			modifier = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "			date > 1938.1.1\r\n";
					FocusTree += "			}";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					//FocusTree += "			opinion_gain_monthly_factor = 1.0 \r\n";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				for (int i = 0; i < newAllies.size(); i++)
				{
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Alliance_"+newAllies[i]->getTag() + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Alliance with "+newAllies[i]->getSourceCountry()->getName()+"\"\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 3 + i) + "\r\n";
					FocusTree += "		y = 1\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 10\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		bypass = { \r\n";
					FocusTree += "			\r\n";
					FocusTree += "			OR = {\r\n";
					FocusTree += "				is_in_faction_with = "+newAllies[i]->getTag()+"\r\n";
					FocusTree += "				has_war_with = " + newAllies[i]->getTag() + "\r\n";
					FocusTree += "				NOT = { country_exists = " + newAllies[i]->getTag() + " }\r\n";
					FocusTree += "			}\r\n";
					FocusTree += "		}\r\n";
					
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "		" + newAllies[i]->getTag() + " = {\r\n";
					FocusTree += "			add_opinion_modifier = { target = " + Leader->getTag() + " modifier = ger_ita_alliance_focus } \r\n";
					FocusTree += "		}";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				//Declaring war with Great Country
				vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);
				map<double, HoI4Country*> GCDistance;
				vector<HoI4Country*> GCDistanceSorted;
				//get great countries with a distance
				for each (auto GC in GreatCountries)
				{
					GCDistance.insert(make_pair(GetDistance(Leader, GC), GC));
				}
				//put them into a vector so we know their order
				for (auto iterator = GCDistance.begin(); iterator != GCDistance.end(); ++iterator)
				{
					GCDistanceSorted.push_back(iterator->second);
				}
				sort(GCDistanceSorted.begin(), GCDistanceSorted.end());
				vector<HoI4Country*> GCTargets;
				for each (auto GC in GCDistanceSorted)
				{
					if (HowToTakeLand(GC, Leader, 3) == "noactionneeded" || HowToTakeLand(GC, Leader, 3) == "factionneeded" || HowToTakeLand(GC, Leader, 3) == "morealliesneeded")
					{
						if(GC != Leader)
							GCTargets.push_back(GC);
					}

				}
				int maxGCWars = 0;
				int start = 0;
				if (GCTargets.size() >= 4)
				{
					start = -3;
				}
				else if (GCTargets.size() == 3)
				{
					start = -2;
				}
				else if (GCTargets.size() == 2)
				{
					start = -1;
				}

				for each (auto GC in GCTargets)
				{
					if (maxGCWars < 4)
					{
						//figuring out location of WG
						string prereq;
						if (takenSpotsy.size() > 0)
						{
							if (takenSpotsy.back() == 5)
							{
								prereq = Leader->getTag() + "_finish_" + GC->getTag();
							}
							else if (takenSpotsy.back() == 2)
							{
								prereq = Leader->getTag() + "_anschluss_" + GC->getTag();
							}
							else
								prereq = "";
						}
						else
						{
							prereq = "";
							takenSpotsy.push_back(0);
						}
						int v1 = rand() % 12 + 1;
						int v2 = rand() % 12 + 1;
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = War" + GC->getTag() + Leader->getTag() + "\r\n";
						FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
						FocusTree += "		text = \"War with " + GC->getSourceCountry()->getName() + "\"\r\n";//change to faction name later
					//	if(prereq != "")
						//	FocusTree += "		prerequisite = { focus = "+prereq+" }\r\n";
						FocusTree += "		available = {   date > 1938." + to_string(v1) + "." + to_string(v2) + "} \r\n";
						FocusTree += "		x = " + to_string(takenSpots.back() + start + maxGCWars*2) + "\r\n";
						FocusTree += "		y = " + to_string(0) + "\r\n";
						//FocusTree += "		y = " + to_string(takenSpotsy.back() + 1) + "\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "			modifier = {\r\n";
						FocusTree += "			factor = 0\r\n";
						FocusTree += "			strength_ratio = { tag = " + GC->getTag() + " ratio < 1 }\r\n";
						FocusTree += "			}";
						if (GCTargets.size() > 1)
						{
							FocusTree += "modifier = {\r\n	factor = 0\r\n	OR = {";
							for (int i2 = 0; i2 < 3; i2++)
							{
								if (GC != GCTargets[i2])
								{
									FocusTree += "has_war_with = " + GC->getTag()+"\r\n";
								}

							}
							FocusTree += "}\r\n}";
						}
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						FocusTree += "			create_wargoal = {\r\n";
						FocusTree += "				type = puppet_wargoal_focus\r\n";
						FocusTree += "				target = " + GC->getTag() + "\r\n";
						FocusTree += "			}";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";
						maxGCWars++;
					}
				}
				//insert these values in targetmap for use later possibly?
				TargetMap.insert(make_pair("noactionneeded", nan));
				TargetMap.insert(make_pair("factionneeded", fn));
				TargetMap.insert(make_pair("morealliesneeded", man));
				TargetMap.insert(make_pair("coup", coup));
				//actual eventoutput
				FocusTree += "\r\n}";
				
				//output National Focus
				string filenameNF("Output/" + Configuration::getOutputName() + "/common/national_focus/" + Leader->getSourceCountry()->getTag() + "_NF.txt");
				//string filename2("Output/NF.txt");
				ofstream out2;
				out2.open(filenameNF);
				{
					out2 << FocusTree;
				}
				out2.close();
				//output events
				string filenameevents("Output/" + Configuration::getOutputName() + "/events/" + Leader->getSourceCountry()->getTag() + "_events.txt");
				//string filename2("Output/NF.txt");
				ofstream outevents;
				outevents.open(filenameevents);
				{
					outevents << Events;
				}
				outevents.close();

				out << s;
				out << endl;
			}
			if ((Leader->getGovernment() == "communism") && communismrelevant)
			{
				LOG(LogLevel::Info) << "Calculating AI for " + Leader->getSourceCountry()->getName();
				out << Leader->getSourceCountry()->getName() << endl;
				vector<int> leaderProvs = getCountryProvinces(Leader);
				LOG(LogLevel::Info) << "Calculating Neighbors for " + Leader->getSourceCountry()->getName();
				map<string, HoI4Country*> Neighbors = findNeighbors(leaderProvs, Leader);
				set<string> Allies = Leader->getAllies();
				vector<HoI4Country*> Targets;
				map<string, vector<HoI4Country*>> NationalFocusesMap;
				vector<HoI4Country*> coups;
				vector<HoI4Country*> forcedtakeover;

				//check if has "Long live the king" and add events if they do

				//if (Permanant Revolution)
					//Decide between Anti - Democratic Focus, Anti - Monarch Focus, or Anti - Fascist Focus(Look at all great powers and get average relation between each ideology, the one with the lowest average relation leads to that focus).
					//Attempt to ally with other Communist Countries(with Permanant Revolution)
				LOG(LogLevel::Info) << "Doing Neighbor calcs for " + Leader->getSourceCountry()->getName();
				for (auto neigh : Neighbors)
				{
					//lets check to see if they are our ally and not a great country
					if (std::find(Allies.begin(), Allies.end(), neigh.second->getTag()) == Allies.end() && !checkIfGreatCountry(neigh.second, sourceWorld, countryMap))
					{
						double com = 0;

						for (auto party : neigh.second->getParties())
						{
							if (party.name.find("socialist") != string::npos || party.name.find("communist") != string::npos)
								com += party.popularity;
						}
						if (com > 25)
						{
							//look for neighboring countries to spread communism too(Need 25 % or more Communism support), Prioritizing those with "Communism Allowed" Flags, prioritizing those who are weakest
							//	Method() Influence Ideology and Attempt Coup
							out << "/t" + neigh.second->getSourceCountry()->getName() + " Attempt Coup" << endl;
							coups.push_back(neigh.second);
						}
						else
						{
							//	Then look for neighboring countries to spread communism by force, prioritizing weakest first
							forcedtakeover.push_back(neigh.second);
							//	Depending on Anti - Ideology Focus, look for allies in alternate ideologies to get to ally with to declare war against Anti - Ideology Country.
						}
					}
				}
				//if (Socialism in One State)
				//	Events / Focuses to increase Industrialization and defense of the country, becomes Isolationist
				//	Eventually gets events to drop Socialism in One state and switch to permanant revolution(Maybe ? )

				string s;
				map<string, vector<HoI4Country*>> TargetMap;
				vector<HoI4Country*> nan;
				vector<HoI4Country*> fn;
				vector<HoI4Country*> man;
				vector<HoI4Country*> coup;
				for (auto target : forcedtakeover)
				{
					string type;
					//outputs are
					//noactionneeded -  Can take target without any help
					//factionneeded - can take target and faction with attackers faction helping
					//morealliesneeded - can take target with more allies, comes with "newallies" in map
					//coup - cant take over, need to coup
					type = HowToTakeLand(target, Leader, 2.5);
					out << type + " " + target->getSourceCountry()->getName() << endl;
					if (type == "noactionneeded")
						nan.push_back(target);
					else if (type == "factionneeded")
						fn.push_back(target);
					else if (type == "morealliesneeded")
						man.push_back(target);
					else if (type == "coup")
						coup.push_back(target);
				}
				//insert these values in targetmap for use later possibly?
				TargetMap.insert(make_pair("noactionneeded", nan));
				TargetMap.insert(make_pair("factionneeded", fn));
				TargetMap.insert(make_pair("morealliesneeded", man));
				TargetMap.insert(make_pair("coup", coup));
				//actual eventoutput
				vector<int> takenSpots;
				string FocusTree = genericFocusTreeCreator(Leader);
				if (coups.size() > 0)
				{
					int x = 22;
					takenSpots.push_back(x);
					if (coups.size() == 1)
					{
						x = 24;
						takenSpots.push_back(x);
					}
					if (coups.size() >= 2)
					{
						x = 25;
						takenSpots.push_back(x);
					}
					//Focus to increase Comm support and prereq for coups
						FocusTree += "focus = {\r\n";
						FocusTree += "		id = Home_of_Revolution"+Leader->getTag()+"\r\n";
						FocusTree += "		icon = GFX_goal_support_communism\r\n";
						FocusTree += "		text = \"Home of the Revolution\"\r\n";
						FocusTree += "		x = "+to_string(x)+"\r\n";
						FocusTree += "		y = 0\r\n";
						FocusTree += "		cost = 10\r\n";
						FocusTree += "		ai_will_do = {\r\n";
						FocusTree += "			factor = 5\r\n";
						FocusTree += "		}	\r\n";
						FocusTree += "		completion_reward = {\r\n";
						//FIXME 
						//Need to get Drift Defense to work
						//FocusTree += "			drift_defence_factor = 0.5\r\n";
						FocusTree += "			add_ideas = communist_influence\r\n";
						FocusTree += "		}\r\n";
						FocusTree += "	}\r\n";
						
					for (int i = 0; i < 2; i++)
					{
						if (i < coups.size())
						{
							FocusTree += "focus = {\r\n";
							FocusTree += "		id = Influence_" +coups[i]->getTag()+"_" +Leader->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_propaganda\r\n";
							FocusTree += "		text = \"Influence "+ coups[i]->getSourceCountry()->getName() +"\"\r\n";
							FocusTree += "		prerequisite = { focus = Home_of_Revolution" + Leader->getTag() + " }\r\n";
							FocusTree += "		x = " + to_string(24+i*2) + "\r\n";
							FocusTree += "		y = 1\r\n";
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 5\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			" + coups[i]->getTag()+" = {\r\n";
							FocusTree += "				if = {\r\n";
							FocusTree += "					limit = {\r\n";
							FocusTree += "						" + Leader->getTag() + " = {\r\n";
							FocusTree += "							has_government = fascism\r\n";
							FocusTree += "						}\r\n";
							FocusTree += "					}\r\n";
							FocusTree += "					add_ideas = fascist_influence\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				if = {\r\n";
							FocusTree += "					limit = {\r\n";
							FocusTree += "						" + Leader->getTag() + " = {\r\n";
							FocusTree += "							has_government = communism\r\n";
							FocusTree += "						}\r\n";
							FocusTree += "					}\r\n";
							FocusTree += "					add_ideas = communist_influence\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				if = {\r\n";
							FocusTree += "					limit = {\r\n";
							FocusTree += "						" + Leader->getTag() + " = {\r\n";
							FocusTree += "							has_government = democratic\r\n";
							FocusTree += "						}\r\n";
							FocusTree += "					}\r\n";
							FocusTree += "					add_ideas = democratic_influence\r\n";
							FocusTree += "				}\r\n";
							FocusTree += "				country_event = { id = generic.1 }";
							FocusTree += "			}\r\n";
							FocusTree += "			\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}\r\n";
							//Civil War
							FocusTree += "focus = {\r\n";
							FocusTree += "		id = Coup_" + coups[i]->getTag() + "_" + Leader->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_demand_territory\r\n";
							FocusTree += "		text = \"Civil War in " + coups[i]->getSourceCountry()->getName() + "\"\r\n";
							FocusTree += "		prerequisite = { focus = Influence_" + coups[i]->getTag() + "_" + Leader->getTag() + " }\r\n";
							FocusTree += "		available = {\r\n";
							FocusTree += "		"+ coups[i]->getTag() +" = { communism > 0.5 } ";
							FocusTree += "		}";
							FocusTree += "		x = " + to_string(24 + i * 2) + "\r\n";
							FocusTree += "		y = 2\r\n";
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 5\r\n";
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			" + coups[i]->getTag() + " = {\r\n";
							FocusTree += "						start_civil_war = {\r\n";
							FocusTree += "							ideology = communism\r\n";
							FocusTree += "							size = 0.5\r\n";
							FocusTree += "					}";
							FocusTree += "			}\r\n";
							FocusTree += "			\r\n";
							FocusTree += "		}\r\n";
							FocusTree += "	}\r\n";
						}
					}
				}
				if (forcedtakeover.size() > 0)
				{
					
					//Strengthen Commitern
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = StrengthCom" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_support_communism\r\n";
					FocusTree += "		text = \"Strengthen The Comintern\"\r\n";//change to faction name later
					FocusTree += "		x = " + to_string(takenSpots.back() + 5) + "\r\n";//fixme
					FocusTree += "		y = 0\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "			army_experience = 20\r\n";
					FocusTree += "		add_tech_bonus = { \r\n";
					FocusTree += "				bonus = 0.5\r\n";
					FocusTree += "				uses = 2\r\n";
					FocusTree += "				category = land_doctrine\r\n";
					FocusTree += "			}";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";

					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Inter_Com_Pres" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_dangerous_deal\r\n";
					FocusTree += "		text = \"International Communist Pressure\"\r\n";//change to faction name later
					FocusTree += "		prerequisite = { focus = StrengthCom" + Leader->getTag() + " }\r\n";
					FocusTree += "		available = {  date > 1937.1.1 } \r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 5) + "\r\n";
					FocusTree += "		y = 1\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "			add_named_threat = { threat = 2 name = \"Socialist World Republic\" }\r\n";
					//FIXME
					//maybe add some claims?
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
					vector<HoI4Country*> TargetsbyIC; //its actually by tech lol
					bool first = true;
					//FIXME 
					//Right now just uses everyone in forcedtakover, doesnt use nan, fn, ect...
					for (auto country : forcedtakeover)
					{
						if (first)
						{
							TargetsbyIC.push_back(country);
							first = false;
						}
						else
						{
							//makes sure not a coup target
							if (find(coups.begin(), coups.end(), country) == coups.end())
							{
								if (TargetsbyIC.front()->getTechnologyCount() < country->getTechnologyCount())
								{
									TargetsbyIC.insert(TargetsbyIC.begin(), country);
								}
								else
									TargetsbyIC.push_back(country);
							}
						}
					}
					for (int i = 0; i < 3; i++)
					{
						if (i < TargetsbyIC.size())
						{
							int v1 = rand() % 12 + 1;
							int v2 = rand() % 12 + 1;
							FocusTree += "focus = {\r\n";
							FocusTree += "		id = War"+ TargetsbyIC[i]->getTag() + Leader->getTag() + "\r\n";
							FocusTree += "		icon = GFX_goal_generic_major_war\r\n";
							FocusTree += "		text = \"War with "+ TargetsbyIC[i]->getSourceCountry()->getName()+"\"\r\n";//change to faction name later
							FocusTree += "		prerequisite = { focus = Inter_Com_Pres" + Leader->getTag() + " }\r\n";
							FocusTree += "		available = {   date > 1938."+to_string(v1)+"."+to_string(v2)+"} \r\n";
							FocusTree += "		x = " + to_string(takenSpots.back() + 3 + i*2) + "\r\n";
							FocusTree += "		y = 2\r\n";
							FocusTree += "		cost = 10\r\n";
							FocusTree += "		ai_will_do = {\r\n";
							FocusTree += "			factor = 5\r\n";
							FocusTree += "			modifier = {\r\n";
							FocusTree += "			factor = 0\r\n";
							FocusTree += "			strength_ratio = { tag = " + TargetsbyIC[i]->getTag() + " ratio < 1 }\r\n";
							FocusTree += "			}";
							if (TargetsbyIC.size() > 1)
							{
								FocusTree += "modifier = {\r\n	factor = 0\r\n	OR = {";
									for (int i2 = 0; i2 < 3; i2++)
									{
										if (i != i2)
											FocusTree += "has_war_with = " + TargetsbyIC[i]->getTag()+"\r\n";
									}
									FocusTree += "}\r\n}";
							}
							FocusTree += "		}	\r\n";
							FocusTree += "		completion_reward = {\r\n";
							FocusTree += "			create_wargoal = {\r\n";
							FocusTree += "				type = puppet_wargoal_focus\r\n";
							FocusTree += "				target = "+ TargetsbyIC[i]->getTag()+"\r\n";
							FocusTree += "			}";
							FocusTree += "		}\r\n";
							FocusTree += "	}\r\n";
							
						}
					}
					takenSpots.push_back(takenSpots.back() + 5);
				}
				//events for allies
				vector<HoI4Country*> newAllies = GetMorePossibleAllies(Leader);
				if (newAllies.size() > 0)
				{
					//Focus to call summit, maybe have events from summit
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Com_Summit" + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Call for the Communist Summit\"\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 3) + "\r\n";
					FocusTree += "		y = 0\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 0\r\n";
					FocusTree += "			modifier = {\r\n";
					FocusTree += "			factor = 5\r\n";
					FocusTree += "			date > 1938.1.1\r\n";
					FocusTree += "			}";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					//FocusTree += "			opinion_gain_monthly_factor = 1.0 \r\n";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				for (int i = 0; i < newAllies.size(); i++)
				{
					FocusTree += "focus = {\r\n";
					FocusTree += "		id = Alliance_" + newAllies[i]->getTag() + Leader->getTag() + "\r\n";
					FocusTree += "		icon = GFX_goal_generic_allies_build_infantry\r\n";
					FocusTree += "		text = \"Alliance with " + newAllies[i]->getSourceCountry()->getName() + "\"\r\n";
					FocusTree += "		x = " + to_string(takenSpots.back() + 3 + i) + "\r\n";
					FocusTree += "		y = 1\r\n";
					FocusTree += "		cost = 10\r\n";
					FocusTree += "		ai_will_do = {\r\n";
					FocusTree += "			factor = 10\r\n";
					FocusTree += "		bypass = { \r\n";
					FocusTree += "			\r\n";
					FocusTree += "			OR = {\r\n";
					FocusTree += "				is_in_faction_with = " + newAllies[i]->getTag() + "\r\n";
					FocusTree += "				has_war_with = " + newAllies[i]->getTag() + "\r\n";
					FocusTree += "				NOT = { country_exists = " + newAllies[i]->getTag() + " }\r\n";
					FocusTree += "			}\r\n";
					FocusTree += "		}\r\n";
					FocusTree += "		}	\r\n";
					FocusTree += "		completion_reward = {\r\n";
					FocusTree += "		" + newAllies[i]->getTag() + " = {\r\n";
					FocusTree += "			add_opinion_modifier = { target = "+Leader->getTag() +" modifier = ger_ita_alliance_focus } \r\n";
					FocusTree += "		}";
					FocusTree += "		}\r\n";
					FocusTree += "	}\r\n";
				}
				FocusTree += "\r\n}";
				string filename2("Output/" + Configuration::getOutputName() + "/common/national_focus/"+Leader->getSourceCountry()->getTag()+"_NF.txt");
				//string filename2("Output/NF.txt");
				ofstream out2;
				out2.open(filename2);
				{
					out2 << FocusTree;
				}
				out2.close();
				out << s;
				out << endl;
			}
		}
		out.close();
	}
}
string HoI4World::HowToTakeLand(HoI4Country* TargetCountry, HoI4Country* AttackingCountry, double time)
{
	string s;
	string type;
		if (TargetCountry != AttackingCountry)
		{
			vector<HoI4Country*> targetFaction = findFaction(TargetCountry);
			vector<HoI4Country*> moreAllies = GetMorePossibleAllies(AttackingCountry);
			vector<HoI4Country*> myFaction = findFaction(AttackingCountry);
			//right now assumes you are stronger then them

			double myFactionDisStrength = GetFactionStrengthWithDistance(AttackingCountry, myFaction, time);
			double enemyFactionDisStrength = GetFactionStrengthWithDistance(TargetCountry, targetFaction, time);
			//lets check if I am stronger then their faction
			if (getStrengthOverTime(AttackingCountry,time) >= GetFactionStrength(targetFaction))
			{
				//we are stronger, and dont even need ally help
				//ADD CONQUEST GOAL
				type = "noactionneeded";
				s += "Can kill " + TargetCountry->getSourceCountry()->getName() + " by ourselves\r\n\t I have a strength of " + to_string(getStrengthOverTime(AttackingCountry, time));
				s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName() + " has a strength of " + to_string(getStrengthOverTime(TargetCountry, time));
				s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + " \r\n";
			}
			else
			{
				//lets check if my faction is stronger
				
				if (myFactionDisStrength >= enemyFactionDisStrength)
				{
					//ADD CONQUEST GOAL
					type = "factionneeded";
					s += "Can kill " + TargetCountry->getSourceCountry()->getName() + " with our faction\r\n\t I have a strength of " + to_string(getStrengthOverTime(AttackingCountry, time));
					s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName() + " has a strength of "+ to_string(getStrengthOverTime(TargetCountry, time));
					s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + " \r\n";
				}
				else
				{
					//FIXME
					//hmm I am still weaker, maybe need to look for allies?
					type = "morealliesneeded";
					//targettype.insert(make_pair("newallies", moreAllies));
					myFactionDisStrength = GetFactionStrengthWithDistance(AttackingCountry, myFaction, time) + GetFactionStrengthWithDistance(AttackingCountry, moreAllies, time);
					enemyFactionDisStrength = GetFactionStrengthWithDistance(TargetCountry, targetFaction, time);
					if (GetFactionStrengthWithDistance(AttackingCountry, myFaction, time) >= GetFactionStrengthWithDistance(TargetCountry, targetFaction, time))
					{
						//ADD CONQUEST GOAL
						s += "Can kill " + TargetCountry->getSourceCountry()->getName() + " with our faction Once I have more allies\r\n\t I have a strength of " + to_string(getStrengthOverTime(AttackingCountry,1));
						s += " and my faction has a strength of " + to_string(myFactionDisStrength) + ", while " + TargetCountry->getSourceCountry()->getName() + " has a strength of " + to_string(getStrengthOverTime(TargetCountry, 1));
						s += " and has a faction strength of " + to_string(enemyFactionDisStrength) + " \r\n";
					}
					else
					{
						//Time to Try Coup
						type = "coup";
						s += "Cannot kill " + TargetCountry->getSourceCountry()->getName() + ", time to try coup\r\n";
					}
				}

			}
		}
		return type;
}
vector<HoI4Country*> HoI4World::GetMorePossibleAllies(HoI4Country* CountryThatWantsAllies)
{
	int maxcountries = 0;
	vector<HoI4Country*> newPossibleAllies;
	set<string> currentAllies = CountryThatWantsAllies->getAllies();
	//set<string> currentAllies = CountryThatWantsAllies->getAllies();
	vector<HoI4Country*> CountriesWithin500Miles; //Rename to actual distance
	for (auto country : countries)
	{
		HoI4Country* country2 = country.second;
		if (GetDistance(CountryThatWantsAllies, country2) <= 500)
			if (std::find(currentAllies.begin(), currentAllies.end(), country2->getTag()) == currentAllies.end())
			{
				CountriesWithin500Miles.push_back(country2);
			}
	}
	string yourgovernment = CountryThatWantsAllies->getGovernment();
	volatile vector<HoI4Country*> vCountriesWithin500Miles = CountriesWithin500Miles;
	//look for all capitals within a distance of Berlin to Tehran
	for (int i = 0; i < CountriesWithin500Miles.size(); i++)
	{
		string allygovernment = CountriesWithin500Miles[i]->getGovernment();
		//possible government matches
		if (allygovernment == yourgovernment
			|| (yourgovernment == "absolute_monarchy" && (allygovernment == "fascism" || allygovernment == "democratic" || allygovernment == "prussian_constitutionalism" || allygovernment == "hms_government"))
			|| (yourgovernment == "democratic" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
			|| (yourgovernment == "prussian_constitutionalism" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "democratic" || allygovernment == "fascism"))
			|| (yourgovernment == "hms_government" && (allygovernment == "democratic" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
			|| (yourgovernment == "communism" && (allygovernment == "syndicalism"))
			|| (yourgovernment == "syndicalism" && (allygovernment == "communism" || allygovernment == "fascism"))
			|| (yourgovernment == "fascism" && (allygovernment == "syndicalism" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism" || allygovernment == "hms_government")))
		{

			if (maxcountries < 2)
			{
				//FIXME
				//check if we are friendly at all?
				HoI4Relations* relationswithposally = CountryThatWantsAllies->getRelations(CountriesWithin500Miles[i]->getTag());
				volatile int rel = relationswithposally->getRelations();
				volatile int size = findFaction(CountriesWithin500Miles[i]).size();
				volatile int armysize = getStrengthOverTime(CountriesWithin500Miles[i], 1);
				//for now can only ally with people not in a faction, and must be worth adding
				if (relationswithposally->getRelations() >= -50 && findFaction(CountriesWithin500Miles[i]).size() <= 1)
				{
					//ok we dont hate each other, lets check how badly we need each other, well I do, the only reason I am here is im trying to conquer a neighbor and am not strong enough!
					//if (GetFactionStrength(findFaction(country)) < 20000) //maybe also check if he has any fascist/comm neighbors he doesnt like later?
					
						//well that ally is weak, he probably wants some friends
						if (relationswithposally->getRelations() >= -50 && relationswithposally->getRelations() < 0)
						{
							//will take some NF to ally
							newPossibleAllies.push_back(CountriesWithin500Miles[i]);
							maxcountries++;
						}
						if(relationswithposally->getRelations() >= 0)
						{
							//well we are positive, 1 NF to add to ally should be fine
							newPossibleAllies.push_back(CountriesWithin500Miles[i]);
							maxcountries++;
						}
					
					/*else if (relationswithposally->getRelations() > 0)
					{
						//we are friendly, add 2 NF for ally? Good way to decide how many alliances there will be
						newPossibleAllies.push_back(country);
						i++;
					}*/
					
				}
			}
			
		}
	}
	return newPossibleAllies;
}
double HoI4World::GetDistance(HoI4Country* Country1, HoI4Country* Country2)
{
	map<string, multimap<HoI4RegimentType, unsigned> > unitTypeMap; // <vic, hoi>
	volatile HoI4Country* Country23 = Country2;
	auto C1state = states.find(Country1->getCapitalProv());
	auto C2state = states.find(Country2->getCapitalProv());
	int C1prov;
	int C2prov;
	if (Country1->getCapitalProv() == 0 || Country2->getCapitalProv() == 0)
		return 100000;
	else
	{
		C1prov = C1state->second->getProvinces().front();
		C2prov = C2state->second->getProvinces().front();
	}
	double C1x = 0;
	double C1y = 0;
	double C2x = 0;
	double C2y = 0;
	std::ifstream file("positions.txt");
	std::string str;
	while (std::getline(file, str))
	{
		vector<string> parts;
		stringstream ss(str);
		string tok;
		char delimiter = ';';
		while (getline(ss, tok, delimiter))
		{
			parts.push_back(tok);
		}
		if (stoi(parts[0]) == C1prov)
		{
			C1x = stoi(parts[2]);
			C1y = stoi(parts[4]);
		}
		else if (stoi(parts[0]) == C2prov)
		{
			C2x = stoi(parts[2]);
			C2y = stoi(parts[4]);
		}
		else if (C1x != 0 && C1y != 0 && C2x != 0 && C2y != 0)
			break;

	}
	double xdist = abs(C2x - C1x);
	if (xdist > 2625)
		xdist = 5250 - xdist;
	double distance = sqrt(pow(xdist, 2) + pow(C2y - C1y, 2));
	return distance;
}
double HoI4World::GetFactionStrengthWithDistance(HoI4Country* HomeCountry, vector<HoI4Country*> Faction, double time)
{
	double strength;
	for (auto country : Faction)
	{
		double distanceMulti = 1;
		if (country == HomeCountry)
		{
			 distanceMulti = 1;
		}
		else
			 distanceMulti = GetDistance(HomeCountry, country);

		if (distanceMulti < 300)
			distanceMulti = 1;
		else if (distanceMulti < 500)
			distanceMulti = 0.9;
		else if (distanceMulti < 750)
			distanceMulti = 0.8;
		else if (distanceMulti < 1000)
			distanceMulti = 0.7;
		else if (distanceMulti < 1500)
			distanceMulti = 0.5;
		else if (distanceMulti < 2000)
			distanceMulti = 0.3;
		else
			distanceMulti = 0.2;

		strength += getStrengthOverTime(country, time) * distanceMulti;
	}
	return strength;
}
vector<HoI4Country*> HoI4World::findFaction(HoI4Country* CheckingCountry)
{
	vector<HoI4Country*> ourFaction;
	ourFaction.push_back(CheckingCountry);
	for (auto faction : Factions)
	{
		if (std::find(faction.begin(), faction.end(), CheckingCountry) != faction.end())
		{
			//if country is in faction list, it is part of that faction
			ourFaction = faction;
		}
	}
	return ourFaction;
}
bool HoI4World::checkIfGreatCountry(HoI4Country* checkingCountry, const V2World &sourceWorld, const CountryMapping& countryMap)
{
	bool isGreatCountry = false;
	vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);

	if (std::find(GreatCountries.begin(), GreatCountries.end(), checkingCountry) != GreatCountries.end())
	{
		isGreatCountry = true;
	}
	return isGreatCountry;

}
map<string, HoI4Country*> HoI4World::findNeighbors(vector<int> CountryProvs, HoI4Country* CheckingCountry)
{
	map<string, HoI4Country*> Neighbors;
	string filename("Output/" + CheckingCountry->getSourceCountry()->getName() + "neigh.txt");
	ofstream out;
	vector<HoI4Province> provinces2;
	out.open(filename);
	{
		for (auto prov : CountryProvs)
		{
			vector<int> thisprovNeighbors = provinceNeighbors.find(prov)->second;
			for (int prov : thisprovNeighbors)
			{
				//string ownertag = "";
				auto ownertag = provincemap.find(prov);
				if (ownertag != provincemap.end())
				{
					vector<string> tags = ownertag->second;
					if (tags.size() >= 1)
					{
						HoI4Country* ownerCountry = countries.find(tags[1])->second;
						if (ownerCountry != CheckingCountry)
						{
							//if not already in neighbors
							if (Neighbors.find(tags[1]) == Neighbors.end())
							{
								Neighbors.insert(make_pair(tags[1], ownerCountry));
							}
						}
					}
				}
			}		
		}
		if (Neighbors.size() == 0)
		{

			for (auto country : countries)
			{

				HoI4Country* country2 = country.second;
				if (country2->getCapitalProv() != 0)
				{
					//IMPROVE
					//need to get further neighbors, as well as countries without capital in an area
					double distance = GetDistance(CheckingCountry, country2);
					if (distance <= 500)
						Neighbors.insert(country);
				}
			}
		}
		out << CheckingCountry->getSourceCountry()->getName() + " " << endl;

		for (auto neigh : Neighbors)
			out << neigh.second->getSourceCountry()->getName() + " ";
		out << endl;
		out.close();
	}
	return Neighbors;
}
void HoI4World::fillProvinces()
{
	for (auto state : states)
	{
		for (auto prov : state.second->getProvinces())
		{
			string owner = state.second->getOwner();
			int stateID = state.second->getID();
			vector<string> provinceinfo;
			provinceinfo.push_back(to_string(stateID));
			provinceinfo.push_back(owner);
			provincemap.insert(pair<int, vector<string>>(prov, provinceinfo));
			// HoI4Province* newprov = new HoI4Province(owner, stateID);
			//provinces.insert(pair<int, HoI4Province*>(prov, newprov));
		}
	}
}
vector<int> HoI4World::getCountryProvinces(HoI4Country* Country)
{
	vector<int> provinces;
	for (auto state : states)
	{
		if (state.second->getOwner() == Country->getTag())
		{
			for (auto prov : state.second->getProvinces())
			{

				provinces.push_back(prov);
			}
		}
	}
	return provinces;
}
vector<vector<HoI4Country*>> HoI4World::CreateFactions(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	vector<vector<HoI4Country*>> Factions2;
	string filename("Output/Factions.txt");
	ofstream out;
	out.open(filename);
	{
		vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);
		vector<string> usedCountries;
		for (auto country : GreatCountries)
		{
			if (std::find(usedCountries.begin(), usedCountries.end(), country->getTag()) == usedCountries.end())
			{
				//checks to make sure its not creating a faction when already in one
				vector<HoI4Country*> Faction;
				double FactionMilStrength = 0;
				Faction.push_back(country);
				string yourgovernment = country->getGovernment();
				auto allies = country->getAllies();
				vector<int> yourbrigs = country->getBrigs();
				auto yourrelations = country->getRelations();
				out << country->getSourceCountry()->getName() << " " + yourgovernment + " " + to_string(getStrengthOverTime(country, 1)) + " allies: \n";
				usedCountries.push_back(country->getTag());
				FactionMilStrength = getStrengthOverTime(country, 1);
				for (auto ally : allies)
				{

					auto itrally = countries.find(ally);
					if (itrally != countries.end())
					{
						HoI4Country* allycountry = itrally->second;
						string allygovernment = allycountry->getGovernment();
						string name = "";
						vector<int> allybrigs = allycountry->getBrigs();
						for (auto country : countries)
						{
							if (country.second->getTag() == ally)
								name = country.second->getSourceCountry()->getName();
						}
						string sphere = returnIfSphere(country, allycountry, sourceWorld, countryMap);

						if (allygovernment == yourgovernment || sphere == country->getTag()
							|| (yourgovernment == "absolute_monarchy" && (allygovernment == "fascism" || allygovernment == "democratic" || allygovernment == "prussian_constitutionalism" || allygovernment == "hms_government"))
							|| (yourgovernment == "democratic" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
							|| (yourgovernment == "prussian_constitutionalism" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "democratic" || allygovernment == "fascism"))
							|| (yourgovernment == "hms_government" && (allygovernment == "democratic" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
							|| (yourgovernment == "communism" && (allygovernment == "syndicalism"))
							|| (yourgovernment == "syndicalism" && (allygovernment == "communism" || allygovernment == "fascism"))
							|| (yourgovernment == "fascism" && (allygovernment == "syndicalism" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism")))
						{
							bool canally = false;
							//if there is a sphere leader, it is not blank
							if (sphere != "")
							{
								//if sphere is equal to great power in question, can ally
								if (sphere == country->getTag())
								{
									canally = true;
								}
							}
							else
								canally = true;

							if (canally)
							{
								usedCountries.push_back(allycountry->getTag());
								out << "\t" + name + " " + allygovernment + " Strength: " + to_string(getStrengthOverTime(allycountry, 1)) << endl;
								FactionMilStrength += getStrengthOverTime(allycountry, 1);
								Faction.push_back(allycountry);
							}
						}
					}

				}
				out << "\tFaction Strength: " + to_string(FactionMilStrength) << endl;
				out << endl;
				Factions2.push_back(Faction);
			}

		}


		out.close();
	}
	return Factions2;
}
HoI4Country* HoI4World::GetFactionLeader(vector<HoI4Country*> Faction)
{
	return Faction.front();
}
double HoI4World::GetFactionStrength(vector<HoI4Country*> Faction)
{
	double strength = 0;
	for (auto country : Faction)
	{
		strength += getStrengthOverTime(country, 1);
	}
	return strength;
}
vector<HoI4Country*> HoI4World::returnGreatCountries(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	const vector<string>& greatCountries = sourceWorld.getGreatCountries();
	vector<HoI4Country*> GreatCountries;
	for (auto countryItr : greatCountries)
	{
		auto itr = countries.find(countryMap[countryItr]);
		if (itr != countries.end())
		{
			GreatCountries.push_back(itr->second);
		}
	}
	return GreatCountries;
}
string HoI4World::returnIfSphere(HoI4Country* leadercountry, HoI4Country* posLeaderCountry, const V2World &sourceWorld, const CountryMapping& countryMap)
{
	vector<HoI4Country*> GreatCountries = returnGreatCountries(sourceWorld, countryMap);
	for (auto country : GreatCountries)
	{
		auto relations = country->getRelations();
		for (auto relation : relations)
		{
			if (relation.second->getSphereLeader())
			{

				string tag = relation.second->getTag();
				auto spheredcountry = countries.find(tag);
				if (spheredcountry != countries.end())
				{
					if (posLeaderCountry->getTag() == spheredcountry->second->getTag())
					{
						return country->getTag();
					}

				}

			}
		}
	}

}
