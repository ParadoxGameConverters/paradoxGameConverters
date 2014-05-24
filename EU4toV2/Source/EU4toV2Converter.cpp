#include <fstream>
#include <io.h>
#include <stdexcept>
#include <sys/stat.h>

#include <Windows.h>

#include "Configuration.h"
#include "Log.h"
#include "Parsers\Parser.h"
#include "EU4World\EU4World.h"
#include "EU4World\EU4Religion.h"
#include "EU4World\EU4Localisation.h"
#include "V2World\V2World.h"
#include "V2World\V2Factory.h"
#include "V2World\V2TechSchools.h"
#include "V2World\V2LeaderTraits.h"
#include "WinUtils.h"


// Converts the given EU4 save into a V2 mod.
// Returns 0 on success or a non-zero failure code on error.
int ConvertEU4ToV2(const std::string& EU4SaveFileName)
{
	Object*	obj;					// generic object
	ifstream	read;				// ifstream for reading files

	Configuration::getInstance();

	char curDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curDir);
	LOG(LogLevel::Debug) << "Current directory is " << curDir;

	// Get V2 install location
	LOG(LogLevel::Info) << "Get V2 Install Path";
	string V2Loc = Configuration::getV2Path();
	struct _stat st;
	if (V2Loc.empty() || (_stat(V2Loc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No Victoria 2 path was specified in configuration.txt, or the path was invalid";
		return (-2);
	}
	else
	{
		LOG(LogLevel::Debug) << "Victoria 2 install path is " << V2Loc;
	}

	// Get V2 Documents Directory
	LOG(LogLevel::Debug) << "Get V2 Documents directory";
	string V2DocLoc = Configuration::getV2DocumentsPath();
	if (V2DocLoc.empty() || (_stat(V2DocLoc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No Victoria 2 documents directory was specified in configuration.txt, or the path was invalid";
		return (-2);
	}
	else
	{
		LOG(LogLevel::Debug) << "Victoria 2 documents directory is " << V2DocLoc;
	}

	// Get EU4 install location
	LOG(LogLevel::Debug) << "Get EU4 Install Path";
	string EU4Loc = Configuration::getEU4Path();
	if (EU4Loc.empty() || (_stat(EU4Loc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No Europa Universalis 4 path was specified in configuration.txt, or the path was invalid";
		return (-2);
	}
	else
	{
		LOG(LogLevel::Debug) << "EU4 install path is " << EU4Loc;
	}

	// Get EU4 Mod directory
	map<string, string> possibleMods; // name, path
	LOG(LogLevel::Debug) << "Get EU4 Mod Directory";
	string EU4DocumentsLoc = Configuration::getEU4DocumentsPath();
	if (EU4DocumentsLoc.empty() || (_stat(EU4DocumentsLoc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No Europa Universalis 4 documents directory was specified in configuration.txt, or the path was invalid";
		return (-2);
	}
	else
	{
		LOG(LogLevel::Debug) << "EU4 Documents directory is " << EU4DocumentsLoc;
		set<string> fileNames;
		WinUtils::GetAllFilesInFolder(EU4DocumentsLoc + "/mod", fileNames);
		for (set<string>::iterator itr = fileNames.begin(); itr != fileNames.end(); itr++)
		{
			int pos = itr->find_last_of('.');
			if (itr->substr(pos, itr->length()) == ".mod")
			{
				Object* modObj = doParseFile((EU4DocumentsLoc + "\\mod\\" + *itr).c_str());
				string name = modObj->getLeaf("name");

				string path;
				vector<Object*> dirObjs = modObj->getValue("user_dir");
				if (dirObjs.size() > 0)
				{
					path = dirObjs[0]->getLeaf();
				}
				else
				{
					vector<Object*> dirObjs = modObj->getValue("archive");
					if (dirObjs.size() > 0)
					{
						path = dirObjs[0]->getLeaf();
					}
				}

				if (path != "")
				{
					possibleMods.insert(make_pair(name, EU4DocumentsLoc + "\\" + path));
				}
			}
		}
	}

	// Get CK2 Export directory
	LOG(LogLevel::Debug) << "Get CK2 Export Directory";
	string CK2ExportLoc = Configuration::getCK2ExportPath();
	if (CK2ExportLoc.empty() || (_stat(CK2ExportLoc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Warning) << "No Crusader Kings 2 mod directory was specified in configuration.txt, or the path was invalid - this will cause problems with CK2 converted saves";
	}
	else
	{
		LOG(LogLevel::Debug) << "CK2 export directory is " << CK2ExportLoc;
		set<string> fileNames;
		WinUtils::GetAllFilesInFolder(CK2ExportLoc, fileNames);
		for (set<string>::iterator itr = fileNames.begin(); itr != fileNames.end(); itr++)
		{
			int pos = itr->find_last_of('.');
			if (itr->substr(pos, itr->length()) == ".mod")
			{
				Object* modObj = doParseFile((CK2ExportLoc + "\\" + *itr).c_str());
				string name = modObj->getLeaf("name");

				string path;
				vector<Object*> dirObjs = modObj->getValue("user_dir");
				if (dirObjs.size() > 0)
				{
					path = dirObjs[0]->getLeaf();
				}
				else
				{
					vector<Object*> dirObjs = modObj->getValue("archive");
					if (dirObjs.size() > 0)
					{
						path = dirObjs[0]->getLeaf();
					}
				}

				if (path != "")
				{
					possibleMods.insert(make_pair(name, CK2ExportLoc + "\\" + path));
				}
			}
		}
	}

	//get output name
	int slash = EU4SaveFileName.find_last_of("\\");
	int length = EU4SaveFileName.find_first_of(".") - slash - 1;
	string outputName = EU4SaveFileName.substr(slash + 1, length);
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

	LOG(LogLevel::Info) << "* Importing EU4 save *";

	//	Parse EU4 Save
	LOG(LogLevel::Info) << "Parsing save";
	obj = doParseFile(EU4SaveFileName.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << EU4SaveFileName;
		exit(-1);
	}

	// Get EU4 Mod
	LOG(LogLevel::Debug) << "Get EU4 Mod";
	vector<string> fullModPaths;
	vector<Object*> modObj = obj->getValue("mod_enabled");
	if (modObj.size() > 0)
	{
		string modName = modObj[0]->getLeaf();
		while (modName != "")
		{
			string newMod;
			int space = modName.find("\" \"");
			if (space == std::string::npos)
			{
				newMod = modName.substr(1, modName.size() - 2);
				modName.clear();
			}
			else
			{
				newMod = modName.substr(1, space - 1);
				modName = modName.substr(space + 2, modName.size() - space - 1);
			}

			map<string, string>::iterator modItr = possibleMods.find(newMod);
			if (modItr != possibleMods.end())
			{
				string newModPath = modItr->second;
				if (newModPath.empty() || (_stat(newModPath.c_str(), &st) != 0))
				{
					LOG(LogLevel::Error) << modName << " could not be found in the specified mod directory - a valid mod directory must be specified. Tried " << newModPath;
				}
				else
				{
					LOG(LogLevel::Debug) << "EU4 Mod is at " << newModPath;
					fullModPaths.push_back(newModPath);
				}
			}
			else
			{
				LOG(LogLevel::Error) << "No path could be found for " << modName;
			}
		}
	}

	// Read all localisations.
	LOG(LogLevel::Info) << "Reading localisation";
	EU4Localisation localisation;
	localisation.ReadFromAllFilesInFolder(Configuration::getEU4Path() + "\\localisation");
	for (vector<string>::iterator itr = fullModPaths.begin(); itr != fullModPaths.end(); itr++)
	{
		LOG(LogLevel::Debug) << "Reading mod localisation";
		localisation.ReadFromAllFilesInFolder(*itr + "\\localisation");
	}

	// Construct world from EU4 save.
	LOG(LogLevel::Info) << "Building world";
	EU4World sourceWorld(localisation, obj);

	// Read EU4 common\countries
	LOG(LogLevel::Info) << "Reading EU4 common\\countries";
	{
		ifstream commonCountries(Configuration::getEU4Path() + "\\common\\country_tags\\00_countries.txt");
		sourceWorld.readCommonCountries(commonCountries, Configuration::getEU4Path());
		for (vector<string>::iterator itr = fullModPaths.begin(); itr != fullModPaths.end(); itr++)
		{
			// This only reads CK2 converted countries at the moment.
			// TBD: Read all txt files from the mod common\country_tags folder.
			if (WinUtils::DoesFileExist(*itr + "\\common\\country_tags\\converted_countries.txt"))
			{
				ifstream convertedCommonCountries(*itr + "\\common\\country_tags\\converted_countries.txt");
				sourceWorld.readCommonCountries(convertedCommonCountries, *itr);
			}
			if (WinUtils::DoesFileExist(*itr + "\\common\\country_tags\\01_special_tags.txt"))
			{
				ifstream specialCommonCountries(*itr + "\\common\\country_tags\\01_special_tags.txt");
				sourceWorld.readCommonCountries(specialCommonCountries, *itr);
			}
		}
	}

	// Resolve unit types
	/*log("Resolving unit types.\n");
	printf("Resolving unit types.\n");
	RegimentTypeMap rtm;
	read.open("unit_strength.txt");
	if (read.is_open())
	{
		read.close();
		read.clear();
		log("\tReading unit strengths from unit_strength.txt\n");
		obj = doParseFile("unit_strength.txt");
		if (obj == NULL)
		{
			log("Could not parse file unit_strength.txt\n");
			exit(-1);
		}
		for (int i = 0; i < num_reg_categories; ++i)
		{
			AddCategoryToRegimentTypeMap(obj, (RegimentCategory)i, RegimentCategoryNames[i], rtm);
		}
	}
	else
	{
		log("	Reading unit strengths from EU4 installation folder\n");
		struct _finddata_t unitFileData;
		intptr_t fileListing;
		if ( (fileListing = _findfirst( (EU4Loc + "\\common\\units\\*.txt").c_str(), &unitFileData)) == -1L)
		{
			log("	Could not open units directory.\n");
			return -1;
		}
		do
		{
			if (strcmp(unitFileData.name, ".") == 0 || strcmp(unitFileData.name, "..") == 0 )
			{
				continue;
			}
			string unitFilename = unitFileData.name;
			string unitName = unitFilename.substr(0, unitFilename.find_first_of('.'));
			AddUnitFileToRegimentTypeMap((EU4Loc + "\\common\\units"), unitName, rtm);
		} while(_findnext(fileListing, &unitFileData) == 0);
		_findclose(fileListing);
	}
	read.close();
	read.clear();
	sourceWorld.resolveRegimentTypes(rtm);*/


	// Merge nations
	LOG(LogLevel::Info) << "Merging nations";
	obj = doParseFile("merge_nations.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file merge_nations.txt";
		exit(-1);
	}
	mergeNations(sourceWorld, obj);

	// Parse V2 input file
	LOG(LogLevel::Info) << "Parsing Vicky2 data";
	V2World destWorld;


	//// Construct factory factory
	//log("Determining factory allocation rules.\n");
	//printf("Determining factory allocation rules.\n");
	//V2FactoryFactory factoryBuilder(V2Loc);

	// Parse province mappings
	LOG(LogLevel::Info) << "Parsing province mappings";
	obj = doParseFile("province_mappings.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file province_mappings.txt";
		exit(-1);
	}
	provinceMapping			provinceMap;
	inverseProvinceMapping	inverseProvinceMap;
	resettableMap				resettableProvinces;
	initProvinceMap(obj, sourceWorld.getVersion(), provinceMap, inverseProvinceMap, resettableProvinces);
	sourceWorld.checkAllProvincesMapped(inverseProvinceMap);

	// Get country mappings
	CountryMapping countryMap;
	countryMap.ReadRules("country_mappings.txt");
	countryMap.CreateMapping(sourceWorld, destWorld);

	// Get adjacencies
	LOG(LogLevel::Info) << "Importing adjacencies";
	adjacencyMapping adjacencyMap = initAdjacencyMap();

	// Generate continent mapping. Only the one from the last listed mod will be used
	continentMapping continentMap;
	for (vector<string>::iterator itr = fullModPaths.begin(); itr != fullModPaths.end(); itr++)
	{
		string continentFile = *itr + "\\map\\continent.txt";
		if ((_stat(continentFile.c_str(), &st) == 0))
		{
			obj = doParseFile(continentFile.c_str());
			if ((obj != NULL) && (obj->getLeaves().size() > 0))
			{
				initContinentMap(obj, continentMap);
			}
		}
	}
	if (continentMap.size() == 0)
	{
		obj = doParseFile((EU4Loc + "\\map\\continent.txt").c_str());
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file " << EU4Loc << "\\map\\continent.txt";
			exit(-1);
		}
		if (obj->getLeaves().size() < 1)
		{
			LOG(LogLevel::Error) << "Failed to parse continent.txt";
			return 1;
		}
		initContinentMap(obj, continentMap);
	}
	if (continentMap.size() == 0)
	{
		LOG(LogLevel::Warning) << "No continent mappings found - may lead to problems later";
	}
	
	// Generate region mapping
	LOG(LogLevel::Info) << "Parsing region structure";
	if (_stat(".\\blankMod\\output\\map\\region.txt", &st) == 0)
	{
		obj = doParseFile(".\\blankMod\\output\\map\\region.txt");
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file .\\blankMod\\output\\map\\region.txt";
			exit(-1);
		}
	}
	else
	{
		obj = doParseFile( (V2Loc + "\\map\\region.txt").c_str() );
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file " << V2Loc << "\\map\\region.txt";
			exit(-1);
		}
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Could not parse region.txt";
		return 1;
	}
	stateMapping		stateMap;
	stateIndexMapping	stateIndexMap;
	initStateMap(obj, stateMap, stateIndexMap);


	// Parse Culture Mappings
	LOG(LogLevel::Info) << "Parsing culture mappings";
	obj = doParseFile("cultureMap.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file cultureMap.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse cultureMap.txt";
		return 1;
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(obj->getLeaves()[0]);

	// find culture groups
	unionCulturesMap unionCultures;
	obj = doParseFile( (EU4Loc + "\\common\\cultures\\00_cultures.txt").c_str() );
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << EU4Loc << "\\common\\cultures\\00_cultures.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse cultures.txt";
		return 1;
	}
	initUnionCultures(obj, unionCultures);
	for (vector<string>::iterator itr = fullModPaths.begin(); itr != fullModPaths.end(); itr++)
	{
		struct _finddata_t	fileData;
		intptr_t					fileListing = NULL;
		if ((fileListing = _findfirst(string(*itr + "\\common\\cultures\\*").c_str(), &fileData)) != -1L)
		{
			do
			{
				if (strcmp(fileData.name, ".") == 0 || strcmp(fileData.name, "..") == 0)
				{
					continue;
				}
				else if (fileData.attrib & _A_SUBDIR)
				{
					continue;
				}
				else
				{
					string modCultureFile(*itr + "\\common\\cultures\\" + fileData.name);
					obj = doParseFile(modCultureFile.c_str());
					if (obj == NULL)
					{
						LOG(LogLevel::Error) << "Could not parse file " << modCultureFile;
						exit(-1);
					}
					if (obj->getLeaves().size() < 1)
					{
						LOG(LogLevel::Error) << "Failed to parse cultures file";
						return 1;
					}
					initUnionCultures(obj, unionCultures);
				}
			} while (_findnext(fileListing, &fileData) == 0);
			_findclose(fileListing);
		}
	}

	// Parse EU4 Religions
	LOG(LogLevel::Info) << "Parsing EU4 religions";
	obj = doParseFile((EU4Loc + "\\common\\religions\\00_religion.txt").c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << EU4Loc << "\\common\\religions\\00_religion.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse 00_religion.txt";
		return 1;
	}
	EU4Religion::parseReligions(obj);
	for (vector<string>::iterator itr = fullModPaths.begin(); itr != fullModPaths.end(); itr++)
	{
		struct _finddata_t	fileData;
		intptr_t					fileListing = NULL;
		if ((fileListing = _findfirst(string(*itr + "\\common\\religions\\*").c_str(), &fileData)) != -1L)
		{
			do
			{
				if (strcmp(fileData.name, ".") == 0 || strcmp(fileData.name, "..") == 0)
				{
					continue;
				}
				else if (fileData.attrib & _A_SUBDIR)
				{
					continue;
				}
				else
				{
					string modReligionFile(*itr + "\\common\\religions\\" + fileData.name);
					if ((_stat(modReligionFile.c_str(), &st) == 0))
					{
						obj = doParseFile(modReligionFile.c_str());
						if (obj == NULL)
						{
							LOG(LogLevel::Error) << "Could not parse file " << modReligionFile;
							exit(-1);
						}
						EU4Religion::parseReligions(obj);
					}
				}
			} while (_findnext(fileListing, &fileData) == 0);
			_findclose(fileListing);
		}
	}

	// Parse Religion Mappings
	LOG(LogLevel::Info) << "Parsing religion mappings";
	obj = doParseFile("religionMap.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file religionMap.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse religionMap.txt";
		return 1;
	}
	religionMapping religionMap;
	religionMap = initReligionMap(obj->getLeaves()[0]);


	//Parse unions mapping
	LOG(LogLevel::Info) << "Parsing union mappings";
	obj = doParseFile("unions.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file unions.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse unions.txt";
		return 1;
	}
	unionMapping unionMap;
	unionMap = initUnionMap(obj->getLeaves()[0]);


	//Parse government mapping
	LOG(LogLevel::Info) << "Parsing governments mappings";
	initParser();
	obj = doParseFile("governmentMapping.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file governmentMapping.txt";
		exit(-1);
	}
	governmentMapping governmentMap;
	governmentMap = initGovernmentMap(obj->getLeaves()[0]);


	////Parse tech schools
	/*log("Parsing tech schools.\n");
	printf("Parsing tech schools.\n");
	initParser();
	obj = doParseFile("blocked_tech_schools.txt");
	if (obj == NULL)
	{
		log("Could not parse file blocked_tech_schools.txt\n");
		exit(-1);
	}
	vector<string> blockedTechSchools;
	blockedTechSchools = initBlockedTechSchools(obj);
	initParser();
	obj = doParseFile( (V2Loc + "\\common\\technology.txt").c_str() );
	if (obj == NULL)
	{
		log("Could not parse file %s\n", (V2Loc + "\\common\\technology.txt").c_str());
		exit(-1);
	}*/
	vector<techSchool> techSchools;
	//techSchools = initTechSchools(obj, blockedTechSchools);


	//// Get Leader traits
	LOG(LogLevel::Info) << "Getting leader traits";
	V2LeaderTraits lt;
	map<int, int> leaderIDMap; // <EU4, V2>


	// Convert
	LOG(LogLevel::Info) << "Converting countries";
	destWorld.convertCountries(sourceWorld, countryMap, cultureMap, unionCultures, religionMap, governmentMap, inverseProvinceMap, techSchools, leaderIDMap, lt);
	LOG(LogLevel::Info) << "Converting diplomacy";
	destWorld.convertDiplomacy(sourceWorld, countryMap);
	LOG(LogLevel::Info) << "Converting provinces";
	destWorld.convertProvinces(sourceWorld, provinceMap, resettableProvinces, countryMap, cultureMap, religionMap, stateIndexMap);
	LOG(LogLevel::Info) << "Setting colonies";
	destWorld.setupColonies(adjacencyMap, continentMap);
	/*LOG(LogLevel::Info) << "Creating states";
	destWorld.setupStates(stateMap);*/
	LOG(LogLevel::Info) << "Setting unciv reforms";
	destWorld.convertUncivReforms();
	/*LOG(LogLevel::Info) << "Creating pops";
	destWorld.setupPops(sourceWorld);*/
	LOG(LogLevel::Info) << "Adding unions";
	destWorld.addUnions(unionMap);
	/*LOG(LogLevel::Info) << "Converting armies and navies";
	destWorld.convertArmies(sourceWorld, inverseProvinceMap, leaderIDMap);*/
	LOG(LogLevel::Info) << "Converting techs";
	destWorld.convertTechs(sourceWorld);
	/*LOG(LogLevel::Info) << "Allocating starting factories";
	destWorld.allocateFactories(sourceWorld, factoryBuilder);*/

	// Output results
	LOG(LogLevel::Info) << "Outputting mod";
	system("%systemroot%\\System32\\xcopy blankMod output /E /Q /Y /I");
	FILE* modFile;
	if (fopen_s(&modFile, ("Output\\" + Configuration::getOutputName() + ".mod").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create .mod file";
		exit(-1);
	}
	fprintf(modFile, "name = \"Converted - %s\"\n", Configuration::getOutputName().c_str());
	fprintf(modFile, "path = \"mod/%s\"\n", Configuration::getOutputName().c_str());
	fprintf(modFile, "replace = \"history/provinces\"\n");
	fprintf(modFile, "replace = \"history/countries\"\n");
	fprintf(modFile, "replace = \"history/diplomacy\"\n");
	fprintf(modFile, "replace = \"common/religion.txt\"\n");
	fprintf(modFile, "replace = \"gfx/interface/icon_religion.dds\"\n");
	fclose(modFile);
	string renameCommand = "move /Y output\\output output\\" + Configuration::getOutputName();
	system(renameCommand.c_str());
	destWorld.output();

	LOG(LogLevel::Info) << "* Conversion complete *";
	return 0;
}


int main(int argc, char * argv[])
{
	try
	{
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
	}
	catch (const std::exception& e)
	{
		LOG(LogLevel::Error) << e.what();
		return -1;
	}
}
