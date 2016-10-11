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



#include <fstream>
#include <stdexcept>
#include "Configuration.h"
#include "Flags.h"
#include "Log.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include "HOI4World/HoI4Buildings.h"
#include "HoI4World/HoI4World.h"
#include "V2World/V2World.h"
#include "OSCompatibilityLayer.h"



void ConvertV2ToHoI4(const string& V2SaveFileName);
int main(const int argc, const char* argv[])
{
	try
	{
		LOG(LogLevel::Info) << "Converter version 0.1A";
		LOG(LogLevel::Info) << "Built on " << __DATE__ << " at " << __TIME__;
		LOG(LogLevel::Info) << "Current directory is " << Utils::getCurrentDirectory();

		const char* const defaultV2SaveFileName = "input.v2";
		string V2SaveFileName;
		if (argc >= 2)
		{
			V2SaveFileName = argv[1];
			LOG(LogLevel::Info) << "Using input file " << V2SaveFileName;
		}
		else
		{
			V2SaveFileName = defaultV2SaveFileName;
			LOG(LogLevel::Info) << "No input file given, defaulting to " << defaultV2SaveFileName;
		}

		ConvertV2ToHoI4(V2SaveFileName);

		return 0;
	}

	catch (const std::exception& e)
	{
		LOG(LogLevel::Error) << e.what();
		return -1;
	}
}


void checkMods();
void getOutputName(const string& V2SaveFileName);
void convert(HoI4States* theStates, HoI4World& destWorld, const map<int, int>& provinceToSupplyZoneMap, const V2World& sourceWorld);
void output(HoI4World& destWorld, const HoI4Buildings& buildings);
void ConvertV2ToHoI4(const string& V2SaveFileName)
{
	Configuration::getInstance();

	checkMods();
	getOutputName(V2SaveFileName);

	V2World sourceWorld(V2SaveFileName);
	HoI4World destWorld(&sourceWorld);

	map<int, vector<int>> HoI4DefaultStateToProvinceMap;
	HoI4States* theStates = new HoI4States(&sourceWorld);
	theStates->importStates(HoI4DefaultStateToProvinceMap);

	map<int, int> provinceToSupplyZoneMap;
	destWorld.importSuppplyZones(HoI4DefaultStateToProvinceMap, provinceToSupplyZoneMap);
	destWorld.importStrategicRegions();
	destWorld.checkAllProvincesMapped();
	destWorld.checkCoastalProvinces();

	convert(theStates, destWorld, provinceToSupplyZoneMap, sourceWorld);
	HoI4Buildings buildings(theStates->getProvinceToStateIDMap());
	output(destWorld, buildings);
	LOG(LogLevel::Info) << "* Conversion complete *";
}


void checkMods()
{
	LOG(LogLevel::Info) << "Double-checking Vic2 mods";

	for (auto expectedMod: Configuration::getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Expecting a mod with name " << expectedMod;
	}

	set<string> fileNames;
	Utils::GetAllFilesInFolder(Configuration::getV2Path() + "/mod", fileNames);
	for (auto fileName: fileNames)
	{
		const int lastPeriodPos = fileName.find_last_of('.');
		if (fileName.substr(lastPeriodPos, fileName.length()) == ".mod")
		{
			string folderName = fileName.substr(0, lastPeriodPos);
			if (Utils::doesFolderExist(Configuration::getV2Path() + "/mod/" + folderName))
			{
				LOG(LogLevel::Debug) << "Found mod with name " << folderName;
			}
		}
	}
}


void getOutputName(const string& V2SaveFileName)
{
	const int slash = V2SaveFileName.find_last_of("\\");
	string outputName = V2SaveFileName.substr(slash + 1, V2SaveFileName.length());
	const int length = outputName.find_first_of(".");
	outputName = outputName.substr(0, length);

	int dash = outputName.find_first_of('-');
	while (dash != string::npos)
	{
		outputName.replace(dash, 1, "_");
		dash = outputName.find_first_of('-');
	}

	int space = outputName.find_first_of(' ');
	while (space != string::npos)
	{
		outputName.replace(space, 1, "_");
		space = outputName.find_first_of(' ');
	}

	Configuration::setOutputName(outputName);
	LOG(LogLevel::Info) << "Using output name " << outputName;

	string outputFolder = Utils::getCurrentDirectory() + "/output/" + Configuration::getOutputName();
	if (Utils::doesFolderExist(outputFolder))
	{
		if (!Utils::deleteFolder(outputFolder))
		{
			exit(-1);
		}
	}
}


void convert(HoI4States* theStates, HoI4World& destWorld, const map<int, int>& provinceToSupplyZoneMap, const V2World& sourceWorld)
{
	// Parse leader traits
	LOG(LogLevel::Info) << "Parsing government jobs";
	/*parser_UTF8::initParser();
	obj = parser_UTF8::doParseFile("leader_traits.txt");
	if (obj == NULL)
	{
	LOG(LogLevel::Error) << "Could not parse file leader_traits.txt";
	exit(-1);
	}*/
	leaderTraitsMap leaderTraits;
	//initLeaderTraitsMap(obj->getLeaves()[0], leaderTraits);

	// parse names
	LOG(LogLevel::Info) << "Parsing names";
	namesMapping namesMap;
	for (auto itr: Configuration::getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod cultures";
		Object* obj = parser_8859_15::doParseFile((Configuration::getV2Path() + "/mod/" + itr + "/common/cultures.txt"));
		if (obj != NULL)
		{
			initNamesMapping(obj, namesMap);
		}
	}
	Object* obj = parser_8859_15::doParseFile((Configuration::getV2Path() + "/common/cultures.txt"));
	if (obj != NULL)
	{
		initNamesMapping(obj, namesMap);
	}

	// parse portraits list
	LOG(LogLevel::Info) << "Parsing portraits list";
	portraitMapping portraitMap;
	obj = parser_UTF8::doParseFile("portraits.txt");
	if (obj != NULL)
	{
		initPortraitMapping(obj, portraitMap);
	}

	theStates->convertStates();
	destWorld.addStates(theStates);
	destWorld.convertNavalBases();
	destWorld.convertCountries(leaderTraits, namesMap, portraitMap);
	theStates->addLocalisations();
	destWorld.convertIndustry();
	destWorld.convertResources();
	destWorld.convertSupplyZones(provinceToSupplyZoneMap);
	destWorld.convertStrategicRegions();
	destWorld.convertDiplomacy();
	destWorld.convertTechs();
	destWorld.configureFactions();
	destWorld.generateLeaders(leaderTraits, namesMap, portraitMap);
	destWorld.convertArmies();
	destWorld.convertNavies();
	destWorld.convertAirforces();
	destWorld.convertCapitalVPs();
	destWorld.thatsgermanWarCreator(sourceWorld);
}


void createModFile();
void output(HoI4World& destWorld, const HoI4Buildings& buildings)
{
	createModFile();

	if (!Utils::renameFolder("output/output", "output/" + Configuration::getOutputName()))
	{
		LOG(LogLevel::Error) << "Could not rename output folder!";
		exit(-1);
	}

	LOG(LogLevel::Info) << "Copying flags";
	copyFlags(destWorld.getCountries());
	LOG(LogLevel::Info) << "Outputting world";
	destWorld.output();
	buildings.output();
}


void createModFile()
{
	LOG(LogLevel::Info) << "Outputting mod";
	if (!Utils::copyFolder("blankMod/output", "output/output"))
	{
		exit(-1);
	}

	ofstream modFile("Output/" + Configuration::getOutputName() + ".mod");
	if (!modFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create .mod file";
		exit(-1);
	}
	modFile << "name = \"Converted - " << Configuration::getOutputName() << "\"\n";
	modFile << "path = \"mod/" << Configuration::getOutputName() << "/\"\n";
	modFile << "user_dir = \"" << Configuration::getOutputName() << "_user_dir\"\n";
	modFile << "replace = \"history/countries\"\n";
	//modFile << "replace = \"history/diplomacy\"\n";
	modFile << "replace = \"history/states\"\n";
	modFile.close();
}