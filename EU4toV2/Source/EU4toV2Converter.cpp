#include <fstream>
#include <sys/stat.h>
#include <io.h>
#include <Windows.h>
#include "Parsers\Parser.h"
#include "Log.h"
#include "EU4World\EU4World.h"
#include "EU4World\EU4Religion.h"
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


	//Get V2 install location
	log("Get V2 Install Path.\n");
	string V2Loc = Configuration::getV2Path();
	struct stat st;
	if (V2Loc.empty() || (stat(V2Loc.c_str(), &st) != 0))
	{
		log("No Victoria2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Victoria2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}

	//Get EU4 install location
	log("Get EU4 Install Path.\n");
	string EU4Loc = Configuration::getEU4Path();
	if (EU4Loc.empty() || (stat(EU4Loc.c_str(), &st) != 0))
	{
		log("No Europa Universalis 4 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Europa Universalis 4 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
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
	Configuration::setOutputName(outputName);
	log("Using output name %s\n", outputName.c_str());

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
	log("\tExtracting data.\n");
	printf("\tExtracting data.\n");
	EU4World sourceWorld(obj);


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
	log("Parsing country mappings.\n");
	printf("Parsing country mappings.\n");
	initParser();
	obj = doParseFile("country_mappings.txt");	
	if (obj == NULL)
	{
		log("Could not parse file country_mappings.txt\n");
		exit(-1);
	}

	// Map EU4 nations to V2 nations
	log("Mapping EU4 nations to V2 nations.\n");
	printf("Mapping EU4 nations to V2 nations.\n");
	removeEmptyNations(sourceWorld);
	if (Configuration::getRemovetype() == "dead")
	{
		removeDeadLandlessNations(sourceWorld);
	}
	else if (Configuration::getRemovetype() == "all")
	{
		removeLandlessNations(sourceWorld);
	}
	countryMapping countryMap;
	int leftoverNations = initCountryMap(countryMap, sourceWorld, destWorld, blockedNations, obj);
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("\tToo many EU4 nations (%d). Removing dead landless nations.\n", leftoverNations);
		printf("\tToo many EU4 nations (%d). Removing dead landless nations.\n", leftoverNations);
		removeDeadLandlessNations(sourceWorld);
		leftoverNations = initCountryMap(countryMap, sourceWorld, destWorld, blockedNations, obj);
	}
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("\tToo many EU4 nations (%d). Removing older landless nations.\n", leftoverNations);
		printf("\tToo many EU4 nations (%d). Removing older landless nations.\n", leftoverNations);
		removeOlderLandlessNations(sourceWorld, leftoverNations + blockedNations.size());
		leftoverNations = initCountryMap(countryMap, sourceWorld, destWorld, blockedNations, obj);
	}
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("\tToo many EU4 nations (%d). Removing all landless nations.\n", leftoverNations);
		printf("\tToo many EU4 nations (%d). Removing all landless nations.\n", leftoverNations);
		removeLandlessNations(sourceWorld);
		leftoverNations = initCountryMap(countryMap, sourceWorld, destWorld, blockedNations, obj);
	}
	if (leftoverNations > 0)
	{
		log("\tToo many EU4 nations (%d). Nothing left to remove. Exiting\n", leftoverNations);
		exit(1);
	}

	// Get adjacencies
	log("Importing adjacencies\n");
	printf("Importing adjacencies\n");
	adjacencyMapping adjacencyMap = initAdjacencyMap();

	// Generate continent mapping
	string EU4Mod = Configuration::getEU4Mod();
	continentMapping continentMap;
	if (EU4Mod != "")
	{
		string continentFile = Configuration::getEU4DocumentsPath() + "\\mod\\" + EU4Mod + "\\map\\continent.txt";
		if ((stat(continentFile.c_str(), &st) != 0))
		{
			obj = doParseFile(continentFile.c_str());
			if (obj == NULL)
			{
				log("Could not parse file %s\n", continentFile.c_str());
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
	
	// Generate region mapping
	log("Parsing region structure.\n");
	printf("Parsing region structure.\n");
	if ((Configuration::getUseV2Mod()) && (stat(".\\blankMod\\output\\map\\region.txt", &st) != 0))
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
	if (EU4Mod != "")
	{
		string modCultureFile = Configuration::getEU4DocumentsPath() + "\\mod\\" + EU4Mod + "\\common\\cultures\\00_cultures.txt";
		if ((stat(modCultureFile.c_str(), &st) != 0))
		{
			obj = doParseFile(modCultureFile.c_str());
			if (obj == NULL)
			{
				log("Could not parse file %s\n", modCultureFile.c_str());
				exit(-1);
			}
			if (obj->getLeaves().size() < 1)
			{
				log("Error: Failed to parse cultures.txt.\n");
				printf("Error: Failed to parse cultures.txt.\n");
				return 1;
			}
			unionCultures = initUnionCultures(obj);
		}
	}
	if (unionCultures.size() == 0)
	{
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
		unionCultures = initUnionCultures(obj);
	}

	// Parse EU4 Religions
	log("Parsing EU4 religions.\n");
	printf("Parsing EU4 religions.\n");
	if (EU4Mod != "")
	{
		string modReligionFile = Configuration::getEU4DocumentsPath() + "\\mod\\" + EU4Mod + "\\common\\religions\\00_religion.txt";
		if ((stat(modReligionFile.c_str(), &st) != 0))
		{
			obj = doParseFile(modReligionFile.c_str());
			if (obj == NULL)
			{
				log("Could not parse file %s\n", modReligionFile.c_str());
				exit(-1);
			}
			if (obj->getLeaves().size() < 1)
			{
				log("Error: Failed to parse 00_cultures.txt.\n");
				printf("Error: Failed to parse 00_cultures.txt.\n");
				return 1;
			}
			EU4Religion::parseReligions(obj);
		}
	}
	if (unionCultures.size() == 0)
	{
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
	//log("Parsing tech schools.\n");
	//printf("Parsing tech schools.\n");
	//initParser();
	//obj = doParseFile("blocked_tech_schools.txt");
	//if (obj == NULL)
	//{
	//	log("Could not parse file blocked_tech_schools.txt\n");
	//	exit(-1);
	//}
	//vector<string> blockedTechSchools;
	//blockedTechSchools = initBlockedTechSchools(obj);
	//initParser();
	//obj = doParseFile( (V2Loc + "\\common\\technology.txt").c_str() );
	//if (obj == NULL)
	//{
	//	log("Could not parse file %s\n", (V2Loc + "\\common\\technology.txt").c_str());
	//	exit(-1);
	//}
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
	/*printf("Converting diplomacy.\n");
	log("Converting diplomacy.\n");
	destWorld.convertDiplomacy(sourceWorld, countryMap);*/
	printf("Converting provinces.\n");
	log("Converting provinces.\n");
	destWorld.convertProvinces(sourceWorld, provinceMap, resettableProvinces, countryMap, cultureMap, religionMap, stateIndexMap);
	printf("Setting colonies\n");
	log("Setting colonies\n");
	destWorld.setupColonies(adjacencyMap, continentMap);
	/*printf("Creating states.\n");
	log("Creating states.\n");
	destWorld.setupStates(stateMap);*/
	/*printf("Setting unciv reforms.\n");
	log("Setting unciv reforms.\n");
	destWorld.convertUncivReforms();*/
	/*printf("Creating pops.\n");
	log("Creating pops.\n");
	destWorld.setupPops(sourceWorld);*/
	printf("Adding unions.\n");
	log("Adding unions.\n");
	destWorld.addUnions(unionMap);
	/*printf("Converting armies and navies.\n");
	log("Converting armies and navies.\n");
	destWorld.convertArmies(sourceWorld, inverseProvinceMap, leaderIDMap);*/
	/*printf("Converting techs.\n");
	log("Converting techs.\n");
	destWorld.convertTechs(sourceWorld);*/
	/*printf("Allocating starting factories.\n");
	log("Allocating starting factories.\n");
	destWorld.allocateFactories(sourceWorld, factoryBuilder);*/

	//// Generate Mod Directory Structure
	//printf("Outputting mod.\n");
	//log("Outputting mod.\n");
	//printf("\tGenerating mod directory structure.\n");
	//log("\tGenerating mod directory structure.\n");
	//CreateDirectory("output", NULL);
	//CreateDirectory(("output\\" + Configuration::getOutputName()).c_str(), NULL);
	////CreateDirectory(("output\\" + Configuration::getOutputName() + "\\common").c_str(), NULL);
	//// Function to Generate bookmark so there is only 1 time period 1836.1.1
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\common\\countries").c_str(), NULL);
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\gfx").c_str(), NULL);
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\gfx\\flags").c_str(), NULL);
	//// Function to create country files for V2 that only exist in EU4 directory.  Move base flag and make generic for all parties.
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\history").c_str(), NULL);
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\history\\countries").c_str(), NULL);
	//// Generate country history files
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\history\\diplomacy").c_str(), NULL);
	//// Current diplomatic relations goes here
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\history\\pops").c_str(), NULL);
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\history\\pops\\1836.1.1").c_str(), NULL);
	//// Run province generation script for the new owners and cores on these provinces base on the V2_province_reference file.
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\history\\units").c_str(), NULL);
	//// Generate file for the armies and navies
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\history\\wars").c_str(), NULL);
	//// Transfer ongoing wars
	//CreateDirectory((V2Loc + "\\mods\\"+ OutputFolderName + "\\localisation").c_str(), NULL);
	//// Generate english localization file for the names of the countries and anything else needed.

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
	fclose(modFile);
	string renameCommand = "move /Y output\\output output\\" + Configuration::getOutputName();
	system(renameCommand.c_str());
	destWorld.output();

	closeLog();
	return 0;
}