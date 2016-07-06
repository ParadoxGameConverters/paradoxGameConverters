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
	if (!Utils::TryCreateFolder(countriesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/common\"";
		exit(-1);
	}
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
	for (auto countryItr : countries)
	{
		countryItr.second->output(states.size(), states);
	}
	// Override vanilla history to suppress vanilla OOB and faction membership being read
	for (auto potentialItr : potentialCountries)
	{
		if (countries.find(potentialItr.first) == countries.end())
		{
			potentialItr.second->output(states.size(), states);
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
void HoI4World::thatsgermanWarCreator(const V2World &sourceWorld, const CountryMapping& countryMap)
{
	//FIX ALL FIXMES AND ADD CONQUEST GOAL
	//MAKE SURE THIS WORKS
	Factions = CreateFactions(sourceWorld, countryMap);
	bool fascismrelevant = false;
	bool communismrelevant = false;
	double WorldStrength = 0;
	//get World Strength
	fillProvinces();
	for (auto Faction : Factions)
	{
		WorldStrength += GetFactionStrength(Faction);
	}

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
	//time to do events for coms and fascs if they are relevant
	for (auto Faction : Factions)
	{
		HoI4Country* Leader = GetFactionLeader(Faction);
		if (Leader->getGovernment == "fascism" || Leader->getGovernment == "absolute_monarchy")
		{
			vector<HoI4Country*> AnchlussTargets;
			vector<HoI4Country*> SudatenTargets;
			vector<HoI4Country*> EqualTargets;
			vector<HoI4Country*> StrongerTargets;
			vector<int> leaderProvs = getCountryProvinces(Leader);
			vector<HoI4Country*> Neighbors = findNeighbors(leaderProvs, Leader);
			set<string> Allies = Leader->getAllies();
			//should add method to look for cores you dont own
			//should add method to look for more allies
			
			//lets look for weak neighbors
			for (auto neigh : Neighbors)
			{
				//lets check to see if they are our ally and not a great country
				if (std::find(Allies.begin(), Allies.end(), neigh->getTag()) == Allies.end() && !checkIfGreatCountry(neigh, sourceWorld, countryMap))
				{
					//if not, lets see their strength is at least < 20%
					if (neigh->getArmyStrength() < Leader->getArmyStrength()*0.2)
					{
						//they are very weak and we can get 1 of these countries in an anchluss
						AnchlussTargets.push_back(neigh);
					}
					//if not, lets see their strength is at least < 60%
					if (neigh->getArmyStrength() < Leader->getArmyStrength()*0.6)
					{
						//they are weak and we can get 1 of these countries in sudaten deal
						SudatenTargets.push_back(neigh);
					}
					//if not, lets see their strength is at least = to ours%
					if (neigh->getArmyStrength() < Leader->getArmyStrength())
					{
						EqualTargets.push_back(neigh);
					}
					//if not, lets see their strength is at least < 120%
					if (neigh->getArmyStrength() < Leader->getArmyStrength()*1.2)
					{
						StrongerTargets.push_back(neigh);
					}
				}

			}
		}
		
	}
}
void HoI4World::HowToTakeLand(HoI4Country* TargetCountry, HoI4Country* AttackingCountry)
{
	vector<HoI4Country*> targetFaction = findFaction(TargetCountry);
	vector<HoI4Country*> myFaction = findFaction(AttackingCountry);
	//right now assumes you are stronger then them

	//lets check if I am stronger then their faction
	if (AttackingCountry->getArmyStrength() >= GetFactionStrength(targetFaction))
	{
		//we are stronger, and dont even need ally help
		//ADD CONQUEST GOAL
	}
	else
	{
		//lets check if my faction is stronger
		if (GetFactionStrengthWithDistance(AttackingCountry, myFaction) >= GetFactionStrengthWithDistance(TargetCountry, targetFaction))
		{
			//ADD CONQUEST GOAL
		}
		else
		{
			//FIXME
			//hmm I am still weaker, maybe need to look for allies?
			//Function that looks for more allies and adds to faction through events/ NF
			//if (GetFactionStrengthWithDistance(me) >= GetFactionStrengthWithDistance(Them))
				//ADD CONQUEST GOAL
			//else
				//Time to Try Coup
		}
		
	}
}
vector<HoI4Country*> HoI4World::GetMorePossibleAllies(HoI4Country* CountryThatWantsAllies)
{
	vector<HoI4Country*> newPossibleAllies;
	vector<HoI4Country*> CountriesWithin1000Miles; //Rename to actual distance
	string yourgovernment = CountryThatWantsAllies->getGovernment();
	//look for all capitals within a distance of Berlin to Tehran
	for (auto country : CountriesWithin1000Miles)
	{
		string allygovernment = country->getGovernment();
		//possible government matches
		if (allygovernment == yourgovernment 
			|| (yourgovernment == "absolute_monarchy" && (allygovernment == "fascism" || allygovernment == "democratic" || allygovernment == "prussian_constitutionalism" || allygovernment == "hms_government"))
			|| (yourgovernment == "democratic" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
			|| (yourgovernment == "prussian_constitutionalism" && (allygovernment == "hms_government" || allygovernment == "absolute_monarchy" || allygovernment == "democratic" || allygovernment == "fascism"))
			|| (yourgovernment == "hms_government" && (allygovernment == "democratic" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism"))
			|| (yourgovernment == "communism" && (allygovernment == "syndicalism"))
			|| (yourgovernment == "syndicalism" && (allygovernment == "communism" || allygovernment == "fascism"))
			|| (yourgovernment == "fascism" && (allygovernment == "syndicalism" || allygovernment == "absolute_monarchy" || allygovernment == "prussian_constitutionalism")))
		{
			//FIXME
			//check if we are friendly at all?
			//if(relations between our countries > -100)
				//ok we dont hate each other, lets check how badly we need each other, well I do, the only reason I am here is im trying to conquer a neighbor and am not strong enough!
				//if(possible country has a weak faction, or is by himself and weak) maybe also check if he has any fascist/comm neighbors he doesnt like later?
					//well that ally is weak, he probably wants some friends
					//if(relations between our countries is > -100 && < -50)
						//will take several NF to ally with them
					//else if (relations between our countries is >= -50 && < 0)
						//will take some NF to ally
					//else
						//well we are positive, 1 NF to add to ally should be fine
				//else if(relations between countries > 0)
					//we are friendly
			newPossibleAllies.push_back(country);
		}
	}
}
double HoI4World::GetFactionStrengthWithDistance(HoI4Country* HomeCountry, vector<HoI4Country*> Faction)
{
	//FIXME
	//Distance is assuming Home Country is Germany, distance determined is capital
	//Berlin to Paris is 1.0x
	//Berlin to London is 0.9x
	//Berlin to Moscow is 0.8x
	//Berlin to Jerusalem is 0.7x
	//Berlin to Terhan is 0.6x
	//Berlin to Delhi is 0.5x
	//Berlin to Nanjing is 0.3x
	//Berlin to Tokyo is 0.2x
	//This is a dummy Function that needs to be completed
	return GetFactionStrength(Faction);
}
vector<HoI4Country*> HoI4World::findFaction(HoI4Country* CheckingCountry)
{
	vector<HoI4Country*> ourFaction;
	ourFaction.push_back(CheckingCountry);
	for (auto faction : Factions)
	{
		if (std::find(faction.begin(), faction.end(), CheckingCountry->getTag()) == faction.end())
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

		if (std::find(GreatCountries.begin(), GreatCountries.end(), checkingCountry->getTag()) != GreatCountries.end())
		{
			isGreatCountry = true;
		}
		return isGreatCountry;
	
}
vector<HoI4Country*> HoI4World::findNeighbors(vector<int> CountryProvs, HoI4Country* CheckingCountry)
{
	vector<HoI4Country*> Neighbors;
	string filename("Output/neigh.txt");
	ofstream out;
	vector<HoI4Province> provinces2;
	out.open(filename);
	{
		for (auto prov : CountryProvs)
		{

			ifstream myReadFile;
			myReadFile.open("adj.txt");
			char output[100];
			if (myReadFile.is_open())
			{
				while (!myReadFile.eof())
				{
					myReadFile >> output;
					vector<string> parts;
					stringstream ss(output); // Turn the string into a stream.
					string tok;
					char delimiter = ';';
					while (getline(ss, tok, delimiter))
					{
						parts.push_back(tok);
					}
					if (stoi(parts[0]) == prov)
					{
						for (int i = 5; i < parts.size(); i++)
						{
							int neighborprov = stoi(parts[i]);
							//int provfind = provinces.find(neighborprov)->first;
							//if (provfind == prov)
							{
								string ownertag = provinces.find(neighborprov)->second->getOwner();
								if (ownertag != CheckingCountry->getTag())
								{
									if (std::find(Neighbors.begin(), Neighbors.end(), ownertag) == Neighbors.end())
									{
										Neighbors.push_back(countries.find(ownertag)->second);
									}
								}
							}
						}
					}
				}
			}
			myReadFile.close();
		}
		out << CheckingCountry->getSourceCountry()->getName() + " " << endl;
		for (auto neigh : Neighbors)
			out << neigh->getSourceCountry()->getName() + " ";
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
			HoI4Province* newprov = new HoI4Province(owner, stateID);
			provinces.insert(pair<int, HoI4Province*>(prov, newprov));
		}
	}
}
/*HoI4Country*  HoI4World::FindProvOwner(int findprov)
{
	for (auto state : states)
	{
		for (auto prov : state.second->getProvinces())
		{
			if (findprov = prov)
			{
				auto itr = countries.find(state.second->getOwner());
				if (itr != countries.end())
				{
					return itr->second;
				}
			}
		}
	}
}*/
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
		string filename("Output/wars.txt");
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
					out << country->getSourceCountry()->getName() << " " + yourgovernment + " " + to_string(country->getArmyStrength()) + " allies: \n";
					usedCountries.push_back(country->getTag());
					FactionMilStrength = country->getArmyStrength();
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
									out << "\t" + name + " " + allygovernment + " Strength: " + to_string(allycountry->getArmyStrength()) << endl;
									FactionMilStrength += allycountry->getArmyStrength();
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
			strength += country->getArmyStrength();
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
