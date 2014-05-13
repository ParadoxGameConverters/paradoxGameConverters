#include <fstream>
#include <sys/stat.h>
#include <io.h>
#include <Windows.h>
#include "Parsers\Parser.h"
#include "Log.h"
#include "EU4World\EU4World.h"
#include "EU4World\EU4Religion.h"
#include "EU4World\EU4Localisation.h"
#include "V2World\V2World.h"
#include "V2World\V2Factory.h"
#include "V2World\V2TechSchools.h"
#include "V2World\V2LeaderTraits.h"
#include "Configuration.h"



int main(int argc, char * argv[]) //changed from TCHAR, no use when everything else in program is in ASCII...
{
	initLog();

	Object*	obj;					// generic object
	ifstream	read;				// ifstream for reading files

	Configuration::getInstance();

	char curDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curDir);
	log("Current directory is %s\n", curDir);

	// Get V2 install location
	log("Get V2 Install Path.\n");
	string V2Loc = Configuration::getV2Path();
	struct _stat st;
	if (V2Loc.empty() || (_stat(V2Loc.c_str(), &st) != 0))
	{
		log("\tNo Victoria 2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Victoria 2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}
	else
	{
		log("\tVictoria 2 install path is %s\n", V2Loc.c_str());
	}

	// Get V2 Documents Directory
	log("Get V2 Documents directory\n");
	string V2DocLoc = Configuration::getV2DocumentsPath();
	if (V2DocLoc.empty() || (_stat(V2DocLoc.c_str(), &st) != 0))
	{
		log("\tNo Victoria 2 documents directory was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Victoria 2 documents directory was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}
	else
	{
		log("\tVictoria 2 documents directory is %s\n", V2DocLoc.c_str());
	}

	// Get EU4 install location
	log("Get EU4 Install Path.\n");
	string EU4Loc = Configuration::getEU4Path();
	if (EU4Loc.empty() || (_stat(EU4Loc.c_str(), &st) != 0))
	{
		log("\tNo Europa Universalis 4 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Europa Universalis 4 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}
	else
	{
		log("\tEU4 install path is %s\n", EU4Loc.c_str());
	}

	// Get EU4 Mod directory
	log("Get EU4 Mod Directory\n");
	string EU4ModLoc = Configuration::getEU4ModPath();
	if (EU4ModLoc.empty() || (_stat(EU4ModLoc.c_str(), &st) != 0))
	{
		log("\tNo Europa Universalis 4 mod directory was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Europa Universalis 4 mod directory was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}
	else
	{
		log("\tEU4 Mod directory is %s\n", EU4ModLoc.c_str());
	}

	// Get CK2 Export directory
	log("Get CK2 Export Directory\n");
	string CK2ExportLoc = Configuration::getCK2ExportPath();
	if (CK2ExportLoc.empty() || (_stat(CK2ExportLoc.c_str(), &st) != 0))
	{
		log("\tNo Crusader Kings 2 mod directory was specified in configuration.txt, or the path was invalid.  This will cause problems with CK2 converted saves.\n");
	}
	else
	{
		log("\tCK2 export direcotry is %s\n", CK2ExportLoc.c_str());
	}	

	//Get Input EU4 save 
	string inputFilename("input.eu4");
	if (argc >= 2)
	{
		inputFilename = argv[1];
		log("Using input file %s.\n", inputFilename.c_str());
		printf("Using input file %s.\n", inputFilename.c_str());
	}
	else
	{
		log("No input file given, defaulting to input.eu4\n");
		printf("No input file given, defaulting to input.eu4\n");
	}

	//get output name
	int slash			= inputFilename.find_last_of("\\");
	int length			= inputFilename.find_first_of(".") - slash - 1;
	string outputName = inputFilename.substr(slash + 1, length);
	int dash				= outputName.find_first_of('-');
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
	log("Using output name %s\n", outputName.c_str());
	printf("Using output name %s\n", outputName.c_str());

	//	Parse EU4 Save
	log("Importing EU4 save.\n");
	printf("Importing EU4 save.\n");
	log("\tParsing save.\n");
	printf("\tParsing save.\n");
	obj = doParseFile(inputFilename.c_str());
	if (obj == NULL)
	{
		log("Could not parse file %s\n", inputFilename.c_str());
		exit(-1);
	}

	// Get EU4 Mod
	log("Get EU4 Mod\n");
	string fullModPath = "";
	vector<Object*> modObj = obj->getValue("mod_enabled");
	if (modObj.size() > 0)
	{
		string modName = modObj[0]->getLeaf();
		if (modName != "")
		{
			if (Configuration::getCK2Converted())
			{
				fullModPath = CK2ExportLoc + "\\" + modName;
			}
			else
			{
				fullModPath = EU4ModLoc + "\\" + modName;
			}
			if (fullModPath.empty() || (_stat(fullModPath.c_str(), &st) != 0))
			{
				log("%s could not be found in the specified mod directory.  A valid mod directory must be specified.\n", modName.c_str());
				printf("%s could not be found in the specified mod directory.  A valid mod directory must be specified.\n", modName.c_str());
				return (-2);
			}
			else
			{
				log("EU4 Mod is at %s\n", fullModPath.c_str());
			}
		}
	}

	// Read all localisations.
	log("Reading localisation.\n");
	printf("Reading localisation.\n");
	EU4Localisation localisation;
	localisation.ReadFromAllFilesInFolder(Configuration::getEU4Path() + "\\localisation");
	if (!fullModPath.empty())
	{
		log("\tReading mod localisation.\n");
		printf("\tReading mod localisation.\n");
		localisation.ReadFromAllFilesInFolder(fullModPath + "\\localisation");
	}

	// Construct world from EU4 save.
	log("Building world.\n");
	printf("Building world.\n");
	EU4World sourceWorld(localisation, obj);

	// Read EU4 common\countries
	log("Reading EU4 common\\countries.\n");
	printf("Reading EU4 common\\countries.\n");
	{
		ifstream commonCountries(Configuration::getEU4Path() + "\\common\\country_tags\\00_countries.txt");
		sourceWorld.readCommonCountries(commonCountries, Configuration::getEU4Path());
		if (!fullModPath.empty())
		{
			// This only reads CK2 converted countries at the moment.
			// TBD: Read all txt files from the mod common\country_tags folder.
			ifstream convertedCommonCountries(fullModPath + "\\common\\country_tags\\converted_countries.txt");
			sourceWorld.readCommonCountries(convertedCommonCountries, fullModPath);
			ifstream specialCommonCountries(fullModPath + "\\common\\country_tags\\01_special_tags.txt");
			sourceWorld.readCommonCountries(specialCommonCountries, fullModPath);
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
	log("Merging nations.\n");
	printf("Merging nations.\n");
	obj = doParseFile("merge_nations.txt");
	if (obj == NULL)
	{
		log("Could not parse file merge_nations.txt\n");
		exit(-1);
	}
	mergeNations(sourceWorld, obj);

	// Parse V2 input file
	log("Parsing Vicky2 data.\n");
	printf("Parsing Vicky2 data.\n");
	V2World destWorld;


	//// Construct factory factory
	//log("Determining factory allocation rules.\n");
	//printf("Determining factory allocation rules.\n");
	//V2FactoryFactory factoryBuilder(V2Loc);

	// Parse province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	obj = doParseFile("province_mappings.txt");
	if (obj == NULL)
	{
		log("Could not parse file province_mappings.txt\n");
		exit(-1);
	}
	provinceMapping			provinceMap;
	inverseProvinceMapping	inverseProvinceMap;
	resettableMap				resettableProvinces;
	initProvinceMap(obj, sourceWorld.getVersion(), provinceMap, inverseProvinceMap, resettableProvinces);
	sourceWorld.checkAllProvincesMapped(inverseProvinceMap);

	// Get list of blocked nations
	log("Getting blocked V2 nations.\n");
	printf("Getting blocked V2 nations.\n");
	obj = doParseFile("blocked_nations.txt");
	if (obj == NULL)
	{
		log("Could not parse file blocked_nations.txt\n");
		exit(-1);
	}
	vector<string> blockedNations = processBlockedNations(obj);

	// Get country mappings
	CountryMapping countryMap;
	countryMap.ReadRules("country_mappings.txt");
	countryMap.CreateMapping(sourceWorld, destWorld);

	// Get adjacencies
	log("Importing adjacencies\n");
	printf("Importing adjacencies\n");
	adjacencyMapping adjacencyMap = initAdjacencyMap();

	// Generate continent mapping
	continentMapping continentMap;
	if (fullModPath != "")
	{
		string continentFile = fullModPath + "\\map\\continent.txt";
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
			log("Could not parse file %s\n", (EU4Loc + "\\map\\continent.txt").c_str());
			exit(-1);
		}
		if (obj->getLeaves().size() < 1)
		{
			log("Error: Failed to parse continent.txt.\n");
			printf("Error: Failed to parse continent.txt.\n");
			return 1;
		}
		initContinentMap(obj, continentMap);
	}
	if (continentMap.size() == 0)
	{
		log("Warning: no continent mappings found. May lead to problems later\n");
	}
	
	// Generate region mapping
	log("Parsing region structure.\n");
	printf("Parsing region structure.\n");
	if (_stat(".\\blankMod\\output\\map\\region.txt", &st) == 0)
	{
		obj = doParseFile(".\\blankMod\\output\\map\\region.txt");
		if (obj == NULL)
		{
			log("Could not parse file .\\blankMod\\output\\map\\region.txt\n");
			exit(-1);
		}
	}
	else
	{
		obj = doParseFile( (V2Loc + "\\map\\region.txt").c_str() );
		if (obj == NULL)
		{
			log("Could not parse file %s\n", (V2Loc + "\\map\\region.txt").c_str());
			exit(-1);
		}
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Could not parse region.txt.\n");
		printf("Error: Could not parse region.txt.\n");
		return 1;
	}
	stateMapping		stateMap;
	stateIndexMapping	stateIndexMap;
	initStateMap(obj, stateMap, stateIndexMap);


	// Parse Culture Mappings
	log("Parsing culture mappings.\n");
	printf("Parsing culture mappings.\n");
	obj = doParseFile("cultureMap.txt");
	if (obj == NULL)
	{
		log("Could not parse file cultureMap.txt\n");
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse cultureMap.txt.\n");
		printf("Error: Failed to parse cultureMap.txt.\n");
		return 1;
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(obj->getLeaves()[0]);

	unionCulturesMap unionCultures;
	obj = doParseFile( (EU4Loc + "\\common\\cultures\\00_cultures.txt").c_str() );
	if (obj == NULL)
	{
		log("Could not parse file %s\n", (EU4Loc + "\\common\\cultures\\00_cultures.txt").c_str());
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse cultures.txt.\n");
		printf("Error: Failed to parse cultures.txt.\n");
		return 1;
	}
	initUnionCultures(obj, unionCultures);
	if (fullModPath != "")
	{
		struct _finddata_t	fileData;
		intptr_t					fileListing = NULL;
		if ((fileListing = _findfirst(string(fullModPath + "\\common\\cultures\\*").c_str(), &fileData)) != -1L)
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
					string modCultureFile(fullModPath + "\\common\\cultures\\" + fileData.name);
					obj = doParseFile(modCultureFile.c_str());
					if (obj == NULL)
					{
						log("Could not parse file %s\n", modCultureFile.c_str());
						exit(-1);
					}
					if (obj->getLeaves().size() < 1)
					{
						log("Error: Failed to parse cultures file.\n");
						printf("Error: Failed to parse cultures file.\n");
						return 1;
					}
					initUnionCultures(obj, unionCultures);
				}
			} while (_findnext(fileListing, &fileData) == 0);
			_findclose(fileListing);
		}
	}

	// Parse EU4 Religions
	log("Parsing EU4 religions.\n");
	printf("Parsing EU4 religions.\n");
	obj = doParseFile((EU4Loc + "\\common\\religions\\00_religion.txt").c_str());
	if (obj == NULL)
	{
		log("Could not parse file %s\n", (EU4Loc + "\\common\\religions\\00_religion.txt").c_str());
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse 00_religion.txt.\n");
		printf("Error: Failed to parse 00_religion.txt.\n");
		return 1;
	}
	EU4Religion::parseReligions(obj);
	if (fullModPath != "")
	{
		struct _finddata_t	fileData;
		intptr_t					fileListing = NULL;
		if ((fileListing = _findfirst(string(fullModPath + "\\common\\religions\\*").c_str(), &fileData)) != -1L)
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
					string modReligionFile(fullModPath + "\\common\\religions\\" + fileData.name);
					if ((_stat(modReligionFile.c_str(), &st) == 0))
					{
						obj = doParseFile(modReligionFile.c_str());
						if (obj == NULL)
						{
							log("Could not parse file %s\n", modReligionFile.c_str());
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
	log("Parsing religion mappings.\n");
	printf("Parsing religion mappings.\n");
	obj = doParseFile("religionMap.txt");
	if (obj == NULL)
	{
		log("Could not parse file religionMap.txt\n");
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse religionMap.txt.\n");
		printf("Error: Failed to parse religionMap.txt.\n");
		return 1;
	}
	religionMapping religionMap;
	religionMap = initReligionMap(obj->getLeaves()[0]);


	//Parse unions mapping
	log("Parsing union mappings.\n");
	printf("Parsing union mappings.\n");
	obj = doParseFile("unions.txt");
	if (obj == NULL)
	{
		log("Could not parse file unions.txt\n");
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse unions.txt.\n");
		printf("Error: Failed to parse unions.txt.\n");
		return 1;
	}
	unionMapping unionMap;
	unionMap = initUnionMap(obj->getLeaves()[0]);


	//Parse government mapping
	log("Parsing governments mappings.\n");
	printf("Parsing governments mappings.\n");
	initParser();
	obj = doParseFile("governmentMapping.txt");
	if (obj == NULL)
	{
		log("Could not parse file governmentMapping.txt\n");
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
	log("Getting leader traits.\n");
	printf("Getting leader traits.\n");
	V2LeaderTraits lt;
	map<int, int> leaderIDMap; // <EU4, V2>


	// Convert
	printf("Converting countries.\n");
	log("Converting countries.\n");
	destWorld.convertCountries(sourceWorld, countryMap, cultureMap, unionCultures, religionMap, governmentMap, inverseProvinceMap, techSchools, leaderIDMap, lt);
	printf("Converting diplomacy.\n");
	log("Converting diplomacy.\n");
	destWorld.convertDiplomacy(sourceWorld, countryMap);
	printf("Converting provinces.\n");
	log("Converting provinces.\n");
	destWorld.convertProvinces(sourceWorld, provinceMap, resettableProvinces, countryMap, cultureMap, religionMap, stateIndexMap);
	printf("Setting colonies\n");
	log("Setting colonies\n");
	destWorld.setupColonies(adjacencyMap, continentMap);
	/*printf("Creating states.\n");
	log("Creating states.\n");
	destWorld.setupStates(stateMap);*/
	printf("Setting unciv reforms.\n");
	log("Setting unciv reforms.\n");
	destWorld.convertUncivReforms();
	/*printf("Creating pops.\n");
	log("Creating pops.\n");
	destWorld.setupPops(sourceWorld);*/
	printf("Adding unions.\n");
	log("Adding unions.\n");
	destWorld.addUnions(unionMap);
	/*printf("Converting armies and navies.\n");
	log("Converting armies and navies.\n");
	destWorld.convertArmies(sourceWorld, inverseProvinceMap, leaderIDMap);*/
	printf("Converting techs.\n");
	log("Converting techs.\n");
	destWorld.convertTechs(sourceWorld);
	/*printf("Allocating starting factories.\n");
	log("Allocating starting factories.\n");
	destWorld.allocateFactories(sourceWorld, factoryBuilder);*/

	// Output results
	printf("Outputting mod\n");
	log("Outputting mod\n");
	system("%systemroot%\\System32\\xcopy blankMod output /E /Q /Y /I");
	FILE* modFile;
	if (fopen_s(&modFile, ("Output\\" + Configuration::getOutputName() + ".mod").c_str(), "w") != 0)
	{
		log("\tError: Could not create .mod file\n");
		exit(-1);
	}
	fprintf(modFile, "name = \"Converted - %s\"\n", Configuration::getOutputName().c_str());
	fprintf(modFile, "path = \"mod/%s\"\n", Configuration::getOutputName().c_str());
	fprintf(modFile, "replace = \"history/provinces\"\n");
	fprintf(modFile, "replace = \"history/countries\"\n");
	fprintf(modFile, "replace = \"history/diplomacy\"\n");
	fprintf(modFile, "replace = \"common/religion.txt\"\n");
	fclose(modFile);
	string renameCommand = "move /Y output\\output output\\" + Configuration::getOutputName();
	system(renameCommand.c_str());
	destWorld.output();

	closeLog();
	return 0;
}