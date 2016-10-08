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
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "ParadoxParser8859_15.h"
#include "EU4World/EU4World.h"
#include "EU4World/EU4Religion.h"
#include "EU4World/EU4Localisation.h"
#include "Mappers/CountryMapping.h"
#include "V2World/V2World.h"
#include "V2World/V2Factory.h"
#include "V2World/V2TechSchools.h"
#include "V2World/V2LeaderTraits.h"
#include "OSCompatibilityLayer.h"



void ConvertEU4ToV2(const string& EU4SaveFileName);
int main(const int argc, const char * argv[])
{
	try
	{
		LOG(LogLevel::Info) << "Converter version 1.0B";
		LOG(LogLevel::Info) << "Built " << __TIMESTAMP__;
		LOG(LogLevel::Debug) << "Current directory is " << Utils::getCurrentDirectory();

		const char* const defaultEU4SaveFileName = "input.eu4";
		string EU4SaveFileName;
		if (argc >= 2)
		{
			EU4SaveFileName = argv[1];
			LOG(LogLevel::Info) << "Using input file " << EU4SaveFileName;
		}
		else
		{
			EU4SaveFileName = defaultEU4SaveFileName;
			LOG(LogLevel::Info) << "No input file given, defaulting to " << defaultEU4SaveFileName;
		}

		ConvertEU4ToV2(EU4SaveFileName);

		return 0;
	}

	catch (const std::exception& e)
	{
		LOG(LogLevel::Error) << e.what();
		return -1;
	}
}


void verifySave(const string& EU4SaveFileName);
void getOutputName(const string& EU4SaveFileName);
map<string, string> getPossibleMods();
void convert(V2World& destWorld, const EU4World& sourceWorld);
void output(const V2World& destWorld);
void ConvertEU4ToV2(const string& EU4SaveFileName)
{
	Configuration::getInstance();

	verifySave(EU4SaveFileName);
	getOutputName(EU4SaveFileName);

	map<string, string> possibleMods = getPossibleMods();

	EU4World sourceWorld(EU4SaveFileName, possibleMods);
	V2World destWorld;
	convert(destWorld, sourceWorld);
	output(destWorld);

	LOG(LogLevel::Info) << "* Conversion complete *";
}


void verifySave(const string& EU4SaveFileName)
{
	FILE* saveFile = fopen(EU4SaveFileName.c_str(), "r");
	if (saveFile == NULL)
	{
		LOG(LogLevel::Error) << "Could not open save! Exiting!";
		exit(-1);
	}
	else
	{
		char buffer[7];
		fread(buffer, 1, 7, saveFile);
		if ((buffer[0] == 'P') && (buffer[1] == 'K'))
		{
			LOG(LogLevel::Error) << "Saves must be uncompressed to be converted.";
			exit(-1);
		}
		else if ((buffer[0] = 'E') && (buffer[1] == 'U') && (buffer[2] == '4') && (buffer[3] = 'b') && (buffer[4] == 'i') && (buffer[5] == 'n') && (buffer[6] == 'M'))
		{
			LOG(LogLevel::Error) << "Ironman saves cannot be converted.";
			exit(-1);
		}
	}
}


void getOutputName(const string& EU4SaveFileName)
{
	const int slash	= EU4SaveFileName.find_last_of("\\");				// the last slash in the save's filename
	string outputName	= EU4SaveFileName.substr(slash + 1, EU4SaveFileName.length());
	const int length	= outputName.find_first_of(".");						// the first period after the slash
	outputName			= outputName.substr(0, length);						// the name to use to output the mod
	int dash = outputName.find_first_of('-');									// the first (if any) dask in the output name
	while (dash != string::npos)
	{
		outputName.replace(dash, 1, "_");
		dash = outputName.find_first_of('-');
	}
	int space = outputName.find_first_of(' ');	// the first space (if any) in the output name
	while (space != string::npos)
	{
		outputName.replace(space, 1, "_");
		space = outputName.find_first_of(' ');
	}
	Configuration::setOutputName(outputName);
	LOG(LogLevel::Info) << "Using output name " << outputName;

	string outputFolder = Utils::getCurrentDirectory() + "/output/" + Configuration::getOutputName();
	if (Utils::doesFolderExist(outputFolder.c_str()))
	{
		if (!Utils::deleteFolder(outputFolder))
		{
			exit(-1);
		}
	}
}


void getEU4ModDirectory(map<string, string>& possibleMods);
void getCK2ExportDirectory(map<string, string>& possibleMods);
map<string, string> getPossibleMods()
{
	map<string, string> possibleMods;
	getEU4ModDirectory(possibleMods);
	getCK2ExportDirectory(possibleMods);

	return possibleMods;
}


void getEU4ModDirectory(map<string, string>& possibleMods)
{
	LOG(LogLevel::Debug) << "Get EU4 Mod Directory";
	string EU4DocumentsLoc = Configuration::getEU4DocumentsPath();	// the EU4 My Documents location as stated in the configuration file
	if (Utils::DoesFileExist(EU4DocumentsLoc))
	{
		LOG(LogLevel::Error) << "No Europa Universalis 4 documents directory was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "EU4 Documents directory is " << EU4DocumentsLoc;
		set<string> fileNames;
		Utils::GetAllFilesInFolder(EU4DocumentsLoc + "/mod", fileNames);
		for (set<string>::iterator itr = fileNames.begin(); itr != fileNames.end(); itr++)
		{
			const int pos = itr->find_last_of('.');	// the position of the last period in the filename
			if (itr->substr(pos, itr->length()) == ".mod")
			{
				Object* modObj = parser_UTF8::doParseFile((EU4DocumentsLoc + "/mod/" + *itr).c_str());	// the parsed mod file

				string name;	// the name of the mod
				vector<Object*> nameObj = modObj->getValue("name");
				if (nameObj.size() > 0)
				{
					name = nameObj[0]->getLeaf();
				}
				else
				{
					name = "";
				}

				string path;	// the path of the mod
				vector<Object*> dirObjs = modObj->getValue("path");	// the possible paths of the mod
				if (dirObjs.size() > 0)
				{
					path = dirObjs[0]->getLeaf();
				}
				else
				{
					vector<Object*> dirObjs = modObj->getValue("archive");	// the other possible paths of the mod (if its zipped)
					if (dirObjs.size() > 0)
					{
						path = dirObjs[0]->getLeaf();
					}
				}

				if ((name != "") && (path != ""))
				{
					possibleMods.insert(make_pair(name, EU4DocumentsLoc + "/" + path));
					Log(LogLevel::Debug) << "\t\tFound a mod named " << name << " claiming to be at " << EU4DocumentsLoc << "/" << path;
				}
			}
		}
	}
}


void getCK2ExportDirectory(map<string, string>& possibleMods)
{
	LOG(LogLevel::Debug) << "Get CK2 Export Directory";
	string CK2ExportLoc = Configuration::getCK2ExportPath();		// the CK2 converted mods location as stated in the configuration file
	if (Utils::DoesFileExist(CK2ExportLoc))
	{
		LOG(LogLevel::Warning) << "No Crusader Kings 2 mod directory was specified in configuration.txt, or the path was invalid - this will cause problems with CK2 converted saves";
	}
	else
	{
		LOG(LogLevel::Debug) << "CK2 export directory is " << CK2ExportLoc;
		set<string> fileNames;
		Utils::GetAllFilesInFolder(CK2ExportLoc, fileNames);
		for (set<string>::iterator itr = fileNames.begin(); itr != fileNames.end(); itr++)
		{
			const int pos = itr->find_last_of('.');	// the last period in the filename
			if ((pos != string::npos) && (itr->substr(pos, itr->length()) == ".mod"))
			{
				Object* modObj = parser_UTF8::doParseFile((CK2ExportLoc + "/" + *itr).c_str());	// the parsed mod file
				vector<Object*> nameObj = modObj->getValue("name");
				string name;
				if (nameObj.size() > 0)
				{
					name = nameObj[0]->getLeaf();
				}

				string path;	// the path of the mod
				vector<Object*> dirObjs = modObj->getValue("user_dir");	// the possible paths for the mod
				if (dirObjs.size() > 0)
				{
					path = dirObjs[0]->getLeaf();
				}
				else
				{
					vector<Object*> dirObjs = modObj->getValue("archive");	// the other possible paths for the mod (if it's zipped)
					if (dirObjs.size() > 0)
					{
						path = dirObjs[0]->getLeaf();
					}
				}

				if (path != "")
				{
					possibleMods.insert(make_pair(name, CK2ExportLoc + "/" + path));
				}
			}
		}
	}
}


void convert(V2World& destWorld, const EU4World& sourceWorld)
{
	CountryMapping::createMappings(sourceWorld, destWorld.getPotentialCountries());

	map<int, int> leaderIDMap; // <EU4, V2>

	// Parse tech schools
	LOG(LogLevel::Info) << "Parsing tech schools.";
	Object* techSchoolObj = parser_UTF8::doParseFile("blocked_tech_schools.txt");
	if (techSchoolObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file blocked_tech_schools.txt";
		exit(-1);
	}
	vector<string> blockedTechSchools;	// the list of disallowed tech schools
	blockedTechSchools = initBlockedTechSchools(techSchoolObj);
	Object* technologyObj = parser_8859_15::doParseFile( (Configuration::getV2Path() + "/common/technology.txt").c_str() );
	if (technologyObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << Configuration::getV2Path() << "/common/technology.txt";
		exit(-1);
	}
	vector<techSchool> techSchools;
	techSchools = initTechSchools(technologyObj, blockedTechSchools);

	// Construct factory factory
	LOG(LogLevel::Info) << "Determining factory allocation rules.";
	V2FactoryFactory factoryBuilder;

	LOG(LogLevel::Info) << "Converting countries";
	destWorld.convertCountries(sourceWorld, techSchools, leaderIDMap);
	LOG(LogLevel::Info) << "Converting provinces";
	destWorld.convertProvinces(sourceWorld);
	LOG(LogLevel::Info) << "Converting diplomacy";
	destWorld.convertDiplomacy(sourceWorld);
	LOG(LogLevel::Info) << "Setting colonies";
	destWorld.setupColonies();
	LOG(LogLevel::Info) << "Creating states";
	destWorld.setupStates();
	LOG(LogLevel::Info) << "Setting unciv reforms";
	destWorld.convertUncivReforms();
	LOG(LogLevel::Info) << "Converting techs";
	destWorld.convertTechs(sourceWorld);
	LOG(LogLevel::Info) << "Allocating starting factories";
	destWorld.allocateFactories(sourceWorld, factoryBuilder);
	LOG(LogLevel::Info) << "Creating pops";
	destWorld.setupPops(sourceWorld);
	LOG(LogLevel::Info) << "Adding unions";
	destWorld.addUnions();
	LOG(LogLevel::Info) << "Converting armies and navies";
	destWorld.convertArmies(sourceWorld, leaderIDMap);
}


void createModFile();
void output(const V2World& destWorld)
{
	LOG(LogLevel::Info) << "Outputting mod";
	Utils::copyFolder("blankMod/output", "output/output");

	createModFile();

	Utils::renameFolder("output/output", "output/" + Configuration::getOutputName());
	destWorld.output();
}


void createModFile()
{
	ofstream modFile("Output/" + Configuration::getOutputName() + ".mod");
	if (!modFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create " << Configuration::getOutputName() << ".mod";
		exit(-1);
	}

	modFile << "name = \"Converted - " << Configuration::getOutputName() << "\"\n";
	modFile << "path = \"mod/" << Configuration::getOutputName() << "\"\n";
	modFile << "user_dir = \"" << Configuration::getOutputName() << "\"\n";
	modFile << "replace = \"history/provinces\"\n";
	modFile << "replace = \"history/countries\"\n";
	modFile << "replace = \"history/diplomacy\"\n";
	modFile << "replace = \"history/units\"\n";
	modFile << "replace = \"history/pops/1836.1.1\"\n";
	modFile << "replace = \"common/religion.txt\"\n";
	modFile << "replace = \"common/cultures.txt\"\n";
	modFile << "replace = \"common/countries.txt\"\n";
	modFile << "replace = \"common/countries/\"\n";
	modFile << "replace = \"gfx/interface/icon_religion.dds\"\n";
	modFile << "replace = \"localisation/0_Names.csv\"\n";
	modFile << "replace = \"localisation/0_Cultures.csv\"\n";
	modFile << "replace = \"localisation/0_Religions.csv\"\n";
	modFile << "replace = \"history/wars\"\n";

	modFile.close();
}