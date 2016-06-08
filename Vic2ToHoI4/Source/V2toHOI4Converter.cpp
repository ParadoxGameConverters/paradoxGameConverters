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
#include <io.h>
#include <stdexcept>
#include <sys/stat.h>
#include <Windows.h>
#include "Configuration.h"
#include "Log.h"
#include "ParadoxParser.h"
#include "ParadoxParserUTF8.h"
#include "HoI4World\HoI4World.h"
#include "V2World\V2World.h"
#include "V2World\V2Factory.h"
#include "V2World\V2Localisation.h"
#include "WinUtils.h"



// Converts the given V2 save into a HoI4 mod.
// Returns 0 on success or a non-zero failure code on error.
int ConvertV2ToHoI4(const std::wstring& V2SaveFileName)
{
	LOG(LogLevel::Info) << "Converter version 1.2";
	Object*	obj;					// generic object

	Configuration::getInstance();

	wchar_t curDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curDir);
	LOG(LogLevel::Debug) << "Current directory is " << curDir;

	// Get HoI4 install location
	LOG(LogLevel::Debug) << "Get HoI4 Install Path";
	wstring HoI4Loc = Configuration::getHoI4Path();	// the HoI4 install location as stated in the configuration file
	struct _stat st;										// the file info
	if (HoI4Loc.empty() || (_wstat(HoI4Loc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No HoI4 path was specified in configuration.txt, or the path was invalid";
		return (-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "HoI4 path install path is " << HoI4Loc;
	}

	// Get HoI4 Documents Directory
	LOG(LogLevel::Debug) << "Get HoI4 Documents directory";
	wstring HoI4DocLoc = Configuration::getHoI4DocumentsPath();	// the HoI4 My Documents location as stated in the configuration file
	if (HoI4DocLoc.empty() || (_wstat(HoI4DocLoc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No HoI4 documents directory was specified in configuration.txt, or the path was invalid";
		return (-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "HoI4 documents directory is " << HoI4DocLoc;
	}

	// Get V2 install location
	LOG(LogLevel::Debug) << "Get V2 Install Path";
	wstring V2Loc = Configuration::getV2Path();	// the V2 install location as stated in the configuration file
	if (V2Loc.empty() || (_wstat(V2Loc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No Victoria 2 path was specified in configuration.txt, or the path was invalid";
		return (-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "Victoria 2 install path is " << V2Loc;
	}

	// Get V2 Mod directory
	map<string, string> possibleMods; // name, path
	LOG(LogLevel::Debug) << "Get V2 Documents Directory";
	wstring V2DocumentsLoc = Configuration::getV2DocumentsPath();	// the Victoria 2 My Documents location as stated in the configuration file
	if (V2DocumentsLoc.empty() || (_wstat(V2DocumentsLoc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No Victoria 2 documents directory was specified in configuration.txt, or the path was invalid";
		return (-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "Victoria 2 Documents directory is " << V2DocumentsLoc;
	}

	// Sanity check Vic2 mods
	LOG(LogLevel::Debug) << "Double-check Vic2 mods";
	vector<wstring> vic2Mods = Configuration::getVic2Mods();
	for (auto itr: vic2Mods)
	{
		LOG(LogLevel::Debug) << "\tExpecting a mod with name " << itr;
	}

	set<wstring> fileNames;
	WinUtils::GetAllFilesInFolder(Configuration::getV2Path() + L"\\mod", fileNames);
	for (auto fileName: fileNames)
	{
		const int pos = fileName.find_last_of('.');	// the position of the last period in the filename
		if (fileName.substr(pos, fileName.length()) == L".mod")
		{
			wstring folderName = fileName.substr(0, pos);
			if (WinUtils::doesFolderExist(Configuration::getV2Path() + L"\\mod\\" + folderName))
			{
				LOG(LogLevel::Debug) << "\tFound mod with name " << folderName;
			}
		}
	}

	// get inventions
	LOG(LogLevel::Info) << "Getting inventions";
	inventionNumToName iNumToname;
	getInventionNums(iNumToname);

	// parse technologies
	LOG(LogLevel::Info) << "Parsing Vic2 technologies";
	map<wstring, wstring> armyTechs;
	obj = parser_8859_15::doParseFile((Configuration::getV2Path() + L"\\technologies\\army_tech.txt").c_str());
	if (obj != NULL)
	{
		for (auto tech: obj->getLeaves())
		{
			armyTechs.insert(make_pair(tech->getKey(), tech->getKey()));
		}
	}
	map<wstring, wstring> navyTechs;
	obj = parser_8859_15::doParseFile((Configuration::getV2Path() + L"\\technologies\\navy_tech.txt").c_str());
	if (obj != NULL)
	{
		for (auto tech: obj->getLeaves())
		{
			navyTechs.insert(make_pair(tech->getKey(), tech->getKey()));
		}
	}

	// parse continents
	LOG(LogLevel::Info) << "Parsing continents";
	continentMapping continentMap;
	obj = parser_8859_15::doParseFile((Configuration::getV2Path() + L"\\map\\continent.txt").c_str());
	if (obj != NULL)
	{
		initContinentMap(obj, continentMap);
	}

	//get output name
	const int slash		= V2SaveFileName.find_last_of(L"\\");				// the last slash in the save's filename
	wstring outputName	= V2SaveFileName.substr(slash + 1, V2SaveFileName.length());
	const int length		= outputName.find_first_of(L".");					// the first period after the slash
	outputName				= outputName.substr(0, length);						// the name to use to output the mod
	int dash					= outputName.find_first_of('-');						// the first (if any) dask in the output name
	while (dash != string::npos)
	{
		outputName.replace(dash, 1, L"_");
		dash = outputName.find_first_of(L'-');
	}
	int space = outputName.find_first_of(' ');	// the first space (if any) in the output name
	while (space != string::npos)
	{
		outputName.replace(space, 1, L"_");
		space = outputName.find_first_of(' ');
	}
	Configuration::setOutputName(outputName);
	LOG(LogLevel::Info) << "Using output name " << outputName;

	wstring outputFolder = wstring(curDir) + L"\\output\\" + Configuration::getOutputName();
	if (WinUtils::doesFolderExist(outputFolder.c_str()))
	{
		LOG(LogLevel::Error) << "Output folder " << Configuration::getOutputName() << " already exists! Clear the output folder before running again!";
		exit(0);
	}

	// Parse government mapping
	LOG(LogLevel::Info) << "Parsing governments mappings";
	parser_UTF8::initParser();
	obj = parser_UTF8::doParseFile(L"governmentMapping.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file governmentMapping.txt";
		exit(-1);
	}
	governmentMapper::getInstance()->initGovernmentMap(obj->getLeaves()[0]);

	// Parse issues
	LOG(LogLevel::Info) << "Parsing governments reforms";
	for (auto itr : vic2Mods)
	{
		if (WinUtils::DoesFileExist(Configuration::getV2Path() + L"\\mod\\" + itr + L"\\common\\issues.txt"))
		{
			obj = parser_8859_15::doParseFile((Configuration::getV2Path() + L"\\mod\\" + itr + L"\\common\\issues.txt").c_str());
			if (obj != NULL)
			{
				governmentMapper::getInstance()->initReforms(obj);
				break;
			}
		}
	}
	if (!governmentMapper::getInstance()->areReformsInitialized())
	{
		obj = parser_8859_15::doParseFile((Configuration::getV2Path() + L"\\common\\issues.txt").c_str());
		if (obj != NULL)
		{
			governmentMapper::getInstance()->initReforms(obj);
		}
	}

	LOG(LogLevel::Info) << "* Importing V2 save *";

	//	Parse V2 Save
	LOG(LogLevel::Info) << "Parsing save";
	obj = parser_8859_15::doParseFile(V2SaveFileName.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << V2SaveFileName << ". File is likely missing.";
		exit(-1);
	}

	// Construct world from V2 save.
	LOG(LogLevel::Info) << "Building world";
	V2World sourceWorld(obj, iNumToname, armyTechs, navyTechs, continentMap);

	// Read all localisations.
	LOG(LogLevel::Info) << "Reading localisation";
	V2Localisation localisation;
	localisation.ReadFromAllFilesInFolder(Configuration::getV2Path() + L"\\localisation");
	for (auto itr: vic2Mods)
	{
		LOG(LogLevel::Debug) << "Reading mod localisation";
		localisation.ReadFromAllFilesInFolder(Configuration::getV2Path() + L"\\mod\\" + itr + L"\\localisation");
	}

	sourceWorld.setLocalisations(localisation);


	// Merge nations
	LOG(LogLevel::Info) << "Merging nations";
	obj = parser_UTF8::doParseFile(L"merge_nations.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file merge_nations.txt";
		exit(-1);
	}
	mergeNations(sourceWorld, obj);

	// Parse province mappings
	LOG(LogLevel::Info) << "Parsing province mappings";
	obj = parser_8859_15::doParseFile(L"province_mappings.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file province_mappings.txt";
		exit(-1);
	}
	provinceMapping			provinceMap;
	inverseProvinceMapping	inverseProvinceMap;
	resettableMap				resettableProvinces;
	initProvinceMap(obj, provinceMap, inverseProvinceMap, resettableProvinces);
	sourceWorld.checkAllProvincesMapped(inverseProvinceMap);

	// Parse HoI4 data files
	LOG(LogLevel::Info) << "Parsing HoI4 data";
	HoI4World destWorld;
	destWorld.importProvinces(provinceMap);
	destWorld.checkCoastalProvinces();

	// Get country mappings
	CountryMapping countryMap;
	countryMap.ReadRules(L"country_mappings.txt");
	destWorld.importPotentialCountries();
	countryMap.CreateMapping(sourceWorld, destWorld);

	// Get adjacencies
	LOG(LogLevel::Info) << "Importing HoI4 adjacencies";
	HoI4AdjacencyMapping HoI4AdjacencyMap = initHoI4AdjacencyMap();

	// Leaders
	map<int, int> leaderIDMap; // <V2, HoI4>

	// Parse government jobs
	/*LOG(LogLevel::Info) << "Parsing government jobs";
	parser_UTF8::initParser();
	obj = parser_UTF8::doParseFile(L"governmentJobs.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file governmentJobs.txt";
		exit(-1);
	}*/
	governmentJobsMap governmentJobs;
	//initGovernmentJobTypes(obj->getLeaves()[0], governmentJobs);

	// Parse leader traits
	LOG(LogLevel::Info) << "Parsing government jobs";
	/*parser_UTF8::initParser();
	obj = parser_UTF8::doParseFile(L"leader_traits.txt");
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
	vic2Mods = Configuration::getVic2Mods();
	for (auto itr: vic2Mods)
	{
		LOG(LogLevel::Debug) << "Reading mod cultures";
		obj = parser_8859_15::doParseFile((Configuration::getV2Path() + L"\\mod\\" + itr + L"\\common\\cultures.txt").c_str());
		if (obj != NULL)
		{
			initNamesMapping(obj, namesMap);
		}
	}
	obj = parser_8859_15::doParseFile((Configuration::getV2Path() + L"\\common\\cultures.txt").c_str());
	if (obj != NULL)
	{
		initNamesMapping(obj, namesMap);
	}

	// parse portraits list
	LOG(LogLevel::Info) << "Parsing portraits list";
	portraitMapping portraitMap;
	obj = parser_UTF8::doParseFile(L"portraits.txt");
	if (obj != NULL)
	{
		initPortraitMapping(obj, portraitMap);
	}

	// parse culture mapping
	LOG(LogLevel::Info) << "Parsing culture mappings";
	obj = parser_UTF8::doParseFile(L"culture_map.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file culture_map.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse culture_map.txt";
		return 1;
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(obj->getLeaves()[0]);

	// parse personality mapping
	LOG(LogLevel::Info) << "Parsing personality mappings";
	/*obj = parser_UTF8::doParseFile(L"personality_map.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file personality_map.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse personality_map.txt";
		return 1;
	}*/
	personalityMap landPersonalityMap;
	personalityMap seaPersonalityMap;
	//initLeaderPersonalityMap(obj->getLeaves()[0], landPersonalityMap, seaPersonalityMap);

	// parse background mapping
	LOG(LogLevel::Info) << "Parsing background mappings";
	/*obj = parser_UTF8::doParseFile(L"background_map.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file background_map.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse background_map.txt";
		return 1;
	}*/
	backgroundMap landBackgroundMap;
	backgroundMap seaBackgroundMap;
	//initLeaderBackgroundMap(obj->getLeaves()[0], landBackgroundMap, seaBackgroundMap);

	// parse AI focus data
	LOG(LogLevel::Info) << "Parsing AI focuses";
	/*obj = parser_UTF8::doParseFile(L"ai_focus.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file ai_focus.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse ai_focus.txt";
		return 1;
	}*/
	AIFocusModifiers focusModifiers;
	//initAIFocusModifiers(obj, focusModifiers);

	// Convert
	LOG(LogLevel::Info) << "Converting countries";
	destWorld.convertCountries(sourceWorld, countryMap, inverseProvinceMap, leaderIDMap, localisation, governmentJobs, leaderTraits, namesMap, portraitMap, cultureMap, landPersonalityMap, seaPersonalityMap, landBackgroundMap, seaBackgroundMap);
	LOG(LogLevel::Info) << "Converting states";
	destWorld.convertProvinceOwners(sourceWorld, inverseProvinceMap, countryMap);
	destWorld.convertNavalBases(sourceWorld, inverseProvinceMap);
	destWorld.convertProvinceItems(sourceWorld, provinceMap, inverseProvinceMap, countryMap, HoI4AdjacencyMap);
	destWorld.consolidateProvinceItems(inverseProvinceMap);
	LOG(LogLevel::Info) << "Converting diplomacy";
	destWorld.convertDiplomacy(sourceWorld, countryMap);
	LOG(LogLevel::Info) << "Converting techs";
	destWorld.convertTechs(sourceWorld);
	LOG(LogLevel::Info) << "Adding minimal levels of airbase and port";
	destWorld.addMinimalItems(inverseProvinceMap);
	LOG(LogLevel::Info) << "Converting armies and navies";
	destWorld.convertArmies(sourceWorld, inverseProvinceMap, HoI4AdjacencyMap);
	LOG(LogLevel::Info) << "Setting up factions";
	destWorld.configureFactions(sourceWorld, countryMap);
	LOG(LogLevel::Info) << "Generating Leaders";
	destWorld.generateLeaders(leaderTraits, namesMap, portraitMap);
	LOG(LogLevel::Info) << "Converting victory points";
	destWorld.convertVictoryPoints(sourceWorld, countryMap);
	LOG(LogLevel::Info) << "Setting AI focuses";
	destWorld.setAIFocuses(focusModifiers);

	// Output results
	LOG(LogLevel::Info) << "Outputting mod";
	system("%systemroot%\\System32\\xcopy blankMod output /E /Q /Y /I");

	FILE* modFile;	// the .mod file for this mod
	if (_wfopen_s(&modFile, (L"Output\\" + Configuration::getOutputName() + L".mod").c_str(), L"w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create .mod file";
		exit(-1);
	}
	fwprintf(modFile, L"name = \"Converted - %s\"\n", Configuration::getOutputName().c_str());
	fwprintf(modFile, L"path = \"mod/%s\"\n", Configuration::getOutputName().c_str());
	fwprintf(modFile, L"user_dir = \"%s_user_dir\"\n", Configuration::getOutputName().c_str());
	fwprintf(modFile, L"replace = \"history/countries\"\n");
	//fwprintf(modFile, L"replace = \"history/diplomacy\"\n");
	fwprintf(modFile, L"replace = \"history/states\"\n");
	fclose(modFile);
	wstring renameCommand = L"move /Y output\\output output\\" + Configuration::getOutputName();	// the command to rename the mod correctly
	_wsystem(renameCommand.c_str());
	LOG(LogLevel::Info) << "Copying flags";
	destWorld.copyFlags(sourceWorld, countryMap);

	LOG(LogLevel::Info) << "Outputting world";
	destWorld.output();

	LOG(LogLevel::Info) << "* Conversion complete *";
	return 0;
}


int wmain(const int argc, const wchar_t* argv[])
{
	try
	{
		const wchar_t* const defaultV2SaveFileName = L"input.v2";	// the default name for a save to convert
		wstring V2SaveFileName;													// the actual name for the save to convert
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
		return ConvertV2ToHoI4(V2SaveFileName);
	}
	catch (const std::exception& e)	// catch any errors
	{
		LOG(LogLevel::Error) << e.what();
		return -1;
	}
}