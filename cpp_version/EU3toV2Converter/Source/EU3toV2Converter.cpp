#include <fstream>
#include <sys/stat.h>
#include <io.h>
#include "Parsers\Parser.h"
#include "Log.h"
#include "V2World.h"
#include "V2Factory.h"
#include "V2TechSchools.h"
#include "Configuration.h"


int main(int argc, char * argv[]) //changed from TCHAR, no use when everything else in program is in ASCII...
{
	initLog();

	Object*	obj;				// generic object
	ifstream	read;				// ifstream for reading files


	//Get V2 install location
	string V2Loc = Configuration::getV2Path();
	struct stat st;
	if (V2Loc.empty() || (stat(V2Loc.c_str(), &st) != 0))
	{
		log("No Victoria2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Victoria2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}

	//Get EU3 install location
	string EU3Loc = Configuration::getEU3Path();
	if (EU3Loc.empty() || (stat(EU3Loc.c_str(), &st) != 0))
	{
		log("No EuropaUniversalis3 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No EuropaUniversalis3 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}


	//Get Input EU3 save 
	string inputFilename("input.eu3");
	if (argc >= 2)
	{
		inputFilename = argv[1];
		log("Using input file %s.\n", inputFilename.c_str());
		printf("Using input file %s.\n", inputFilename.c_str());
	}
	else
	{
		log("No input file given, defaulting to input.eu3\n");
		printf("No input file given, defaulting to input.eu3\n");
	}


	// Parse EU3 Save
	log("Parsing EU3 save.\n");
	printf("Parsing EU3 save.\n");
	
	initParser();
	obj = getTopLevel();
	read.open(inputFilename.c_str());
	if (!read.is_open())
	{
		log("Error: Could not open EU3 save (%s).\n", inputFilename.c_str());
		printf("Error: Could not open EU3 save (%s).\n", inputFilename.c_str());
		return 1;
	}
	readFile(read);
	read.close();
	read.clear();
	EU3World sourceWorld;
	sourceWorld.init(obj);


	// Figure out what EU3 gametype we're using
	WorldType game = sourceWorld.getWorldType();
	switch (game)
	{
	case VeryOld:
		printf("	Error: EU3 game appears to be from an old version; only IN, HttT, and DW are supported.\n");
		log("	Error: EU3 game appears to be from an old version; only IN, HttT, and DW are supported.\n");
		exit(1);
	case InNomine:
		printf("	Game type is: EU3 In Nomine.  EXPERIMENTAL.\n");
		log("	Game type is: EU3 In Nomine.  EXPERIMENTAL.\n");
		break;
	case HeirToTheThrone:
		printf("	Game type is: EU3 Heir to the Throne.\n");
		log("	Game type is: EU3 Heir to the Throne.\n");
		break;
	case DivineWind:
		printf("	Game type is: EU3 Divine Wind.\n");
		log("	Game type is: EU3 Divine Wind.\n");
		break;
	default:
		printf("	Error: Could not determine savegame type.\n");
		log("	Error: Could not determine savegame type.\n");
		exit(1);
	}


	// Resolve unit types
	log("Resolving unit types.\n");
	printf("Resolving unit types.\n");
	RegimentTypeMap rtm;
	read.open("unit_strength.txt");
	if (read.is_open())
	{
		read.close();
		read.clear();
		log("	Reading unit strengths from unit_strength.txt\n");
		obj = doParseFile("unit_strength.txt");
		for (int i = 0; i < num_reg_categories; ++i)
		{
			AddCategoryToRegimentTypeMap(obj, (RegimentCategory)i, RegimentCategoryNames[i], rtm);
		}
	}
	else
	{
		log("	Reading unit strengths from EU3 installation folder\n");
		struct _finddata_t unitFileData;
		intptr_t fileListing;
		if ( (fileListing = _findfirst( (EU3Loc + "\\common\\units\\*.txt").c_str(), &unitFileData)) == -1L)
		{
			log("	Could not open units directory.\n");
			return -1;
		}
		do
		{
			if (strcmp(unitFileData.name, ".") == 0 || strcmp(unitFileData.name, "..") == 0 )
				continue;
			string unitFilename = unitFileData.name;
			string unitName = unitFilename.substr(0, unitFilename.find_first_of('.'));
			AddUnitFileToRegimentTypeMap((EU3Loc + "\\common\\units"), unitName, rtm);
		} while(_findnext(fileListing, &unitFileData) == 0);
		_findclose(fileListing);
	}
	read.close();
	read.clear();
	sourceWorld.resolveRegimentTypes(rtm);


	// Merge nations
	log("Merging nations.\n");
	printf("Merging nations.\n");
	obj = doParseFile("merge_nations.txt");
	mergeNations(sourceWorld, obj);


	// Parse V2 input file
	log("Parsing Vicky2 data.\n");
	printf("Parsing Vicky2 data.\n");
	V2World destWorld;
	destWorld.init(V2Loc);


	// Construct factory factory
	log("Determining factory allocation rules.\n");
	printf("Determining factory allocation rules.\n");
	V2FactoryFactory factoryBuilder;
	factoryBuilder.init(V2Loc);


	// Parse province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	const char* mappingFile = "province_mappings.txt";
	// if province_mappings.txt exists, always use it (e.g. as an override for mods);
	// otherwise use the appropriate mapping for the game type
	FILE* testfp = NULL;
	if (fopen_s(&testfp, mappingFile, "rb") != 0)
	{
		if (game == HeirToTheThrone)
			mappingFile = "HttT_province_mappings.txt";
		else if (game == DivineWind)
			mappingFile = "DW_province_mappings.txt";
		else if (game == InNomine)
			mappingFile = "IN_province_mappings.txt";
	}
	else
	{
		fclose(testfp);
	}
	obj = doParseFile(mappingFile);
	provinceMapping provinceMap = initProvinceMap(obj);
	sourceWorld.checkAllProvincesMapped(provinceMap);


	// Get potential V2 countries
	log("Getting potential V2 nations.\n");
	printf("Getting potential V2 nations.\n");
	ifstream V2CountriesInput;
	V2CountriesInput.open( (V2Loc + "\\common\\countries.txt").c_str() );
	if (!V2CountriesInput.is_open())
	{
		log("	Error: Could not open countries.txt\n");
		printf("	Error: Could not open countries.txt\n");
		return 1;
	}
	destWorld.addPotentialCountries(V2CountriesInput, V2Loc);
	V2CountriesInput.close();
	
	// Get list of blocked nations
	log("Getting blocked V2 nations.\n");
	printf("Getting blocked V2 nations.\n");
	obj = doParseFile("blocked_nations.txt");
	vector<string> blockedNations = processBlockedNations(obj);

	// Get country mappings
	log("Parsing country mappings.\n");
	printf("Parsing country mappings.\n");
	initParser();
	obj = doParseFile("country_mappings.txt");	

	// Map EU3 nations to V2 nations
	log("Mapping EU3 nations to V2 nations.\n");
	printf("Mapping EU3 nations to V2 nations.\n");
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
	vector<string> EU3Tags = getEU3Tags(sourceWorld);
	int leftoverNations = initCountryMap(countryMap, EU3Tags, destWorld.getPotentialTags(), blockedNations, obj);
		if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("Too many EU3 nations (%d). Removing dead landless nations.\n", leftoverNations);
		printf("Too many EU3 nations (%d). Removing dead landless nations.\n", leftoverNations);
		removeDeadLandlessNations(sourceWorld);
		EU3Tags = getEU3Tags(sourceWorld);
		leftoverNations = initCountryMap(countryMap, EU3Tags, destWorld.getPotentialTags(), blockedNations, obj);
	}
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("Too many EU3 nations (%d). Removing older landless nations.\n", leftoverNations);
		printf("Too many EU3 nations (%d). Removing older landless nations.\n", leftoverNations);
		removeOlderLandlessNations(sourceWorld, leftoverNations + blockedNations.size());
		EU3Tags = getEU3Tags(sourceWorld);
		leftoverNations = initCountryMap(countryMap, EU3Tags, destWorld.getPotentialTags(), blockedNations, obj);
	}
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("Too many EU3 nations (%d). Removing landless nations.\n", leftoverNations);
		printf("Too many EU3 nations (%d). Removing landless nations.\n", leftoverNations);
		removeLandlessNations(sourceWorld);
		EU3Tags = getEU3Tags(sourceWorld);
		leftoverNations = initCountryMap(countryMap, EU3Tags, destWorld.getPotentialTags(), blockedNations, obj);
	}
	
	// Generate region mapping
	log("Parsing region structure.\n");
	printf("Parsing region structure.\n");

	obj = doParseFile( (V2Loc + "\\map\\region.txt").c_str() );
	if (obj->getLeaves().size() < 1)
	{
		log("This is where a TODO: error was. If you see this, bug a programmer so we can put in a useful message.\n");
		printf("This is where a TODO: error was. If you see this, bug a programmer so we can put in a useful message.\n");
		return 1;
	}
	stateMapping stateMap;
	stateMap = initStateMap(obj);


	// Parse Culture Mappings
	log("Parsing culture mappings.\n");
	printf("Parsing culture mappings.\n");

	obj = doParseFile("cultureMap.txt");
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse cultureMap.txt.\n");
		printf("Error: Failed to parse cultureMap.txt.\n");
		return 1;
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(obj->getLeaves()[0]);

	obj = doParseFile( (EU3Loc + "\\common\\cultures.txt").c_str() );
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse cultures.txt.\n");
		printf("Error: Failed to parse cultures.txt.\n");
		return 1;
	}
	unionCulturesList unionCultures;
	unionCultures = initUnionCultures(obj);

	// Parse Religion Mappings
	log("Parsing religion mappings.\n");
	printf("Parsing religion mappings.\n");

	obj = doParseFile("religionMap.txt");
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
	governmentMapping governmentMap;
	governmentMap = initGovernmentMap(obj->getLeaves()[0]);


	//Parse tech schools
	log("Parsing tech schools.\n");
	printf("Parsing tech schools.\n");
	initParser();
	obj = doParseFile("blocked_tech_schools.txt");
	vector<string> blockedTechSchools;
	blockedTechSchools = initBlockedTechSchools(obj);
	initParser();
	obj = doParseFile( (V2Loc + "\\common\\technology.txt").c_str() );
	vector<techSchool> techSchools;
	techSchools = initTechSchools(obj, blockedTechSchools);


	// Convert
	printf("Converting countries.\n");
	log("Converting countries.\n");
	destWorld.convertCountries(sourceWorld, countryMap, cultureMap, unionCultures, religionMap, governmentMap);
	printf("Converting diplomacy.\n");
	log("Converting diplomacy.\n");
	destWorld.convertDiplomacy(sourceWorld, countryMap);
	printf("Converting provinces.\n");
	log("Converting provinces.\n");
	destWorld.convertProvinces(sourceWorld, provinceMap, countryMap, cultureMap, religionMap);
	printf("Creating states.\n");
	log("Creating states.\n");
	destWorld.setupStates(stateMap);
	printf("Converting capitals.");
	log("Converting capitals.");
	destWorld.convertCapitals(sourceWorld, provinceMap);
	printf("Creating pops.\n");
	log("Creating pops.\n");
	destWorld.setupPops(sourceWorld);
	printf("Adding unions.\n");
	log("Adding unions.\n");
	destWorld.addUnions(unionMap);

	map<int, int> leaderIDMap; // <EU3, V2>
	printf("Converting generals and admirals.\n");
	log("Converting generals and admirals.\n");
	destWorld.convertLeaders(sourceWorld, leaderIDMap);
	printf("Converting armies and navies.\n");
	log("Converting armies and navies.\n");
	destWorld.convertArmies(sourceWorld, provinceMap, leaderIDMap);

	printf("Converting techs.\n");
	log("Converting techs.\n");
	destWorld.convertTechs(sourceWorld);
	printf("Converting tech schools.\n");
	log("Converting tech schools.\n");
	destWorld.convertTechSchools(sourceWorld, techSchools);
	printf("Allocating starting factories.\n");
	log("Allocating starting factories.\n");
	destWorld.allocateFactories(sourceWorld, factoryBuilder);

	// Output results
	printf("Outputting save.\n");
	log("Outputting save.\n");
	FILE* output;
	if (fopen_s(&output, "output.v2", "w") != 0)
	{
		log("Error: could not open output.v2.\n");
		printf("Error: could not open output.v2.\n");
	}
	destWorld.output(output);
	fclose(output);

	closeLog();
	return 0;
}

