#include <fstream>
#include <sys/stat.h>
#include "Parsers\Parser.h"
#include "Log.h"
#include "Configuration.h"
#include "V2World.h"
#include "HoI3World.h"
#include "Mapper.h"

int main(int argc, char * argv[])
{
	initLog();

	Object*	obj;				// generic object
	ifstream	read;			// ifstream for reading files

	//Get V2 install location
	string HoI3Loc = Configuration::getHoI3Path();
	struct stat st;
	if (HoI3Loc.empty() || (stat(HoI3Loc.c_str(), &st) != 0))
	{
		log("No HoI3 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No HoI3 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}

	//Get Input V2 save 
	string inputFilename("input.v2");
	if (argc >= 2)
	{
		inputFilename = argv[1];
		log("Using input file %s.\n", inputFilename.c_str());
		printf("Using input file %s.\n", inputFilename.c_str());
	}
	else
	{
		log("No input file given, defaulting to input.v2\n");
		printf("No input file given, defaulting to input.v2\n");
	}

	// Parse V2 Save
	log("Parsing Vic2 save.\n");
	printf("Parsing Vic2 save.\n");
	
	initParser();
	obj = getTopLevel();
	read.open(inputFilename.c_str());
	if (!read.is_open())
	{
		log("Error: Could not open Vic2 save (%s).\n", inputFilename.c_str());
		printf("Error: Could not open Vic2 save (%s).\n", inputFilename.c_str());
		return 1;
	}
	readFile(read);
	read.close();
	read.clear();
	V2World sourceWorld;
	sourceWorld.init(obj);

	// Merge nations
	log("Merging nations.\n");
	printf("Merging nations.\n");
	obj = doParseFile("merge_nations.txt");
	mergeNations(sourceWorld, obj);
	
	// Parse HoI3 data files
	log("Parsing HoI3 data.\n");
	printf("Parsing HoI3 data.\n");
	HoI3World destWorld;
	destWorld.init(HoI3Loc);

	// Parse province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	const char* mappingFile = "province_mappings.txt";
	// if province_mappings.txt exists, always use it (e.g. as an override for mods);
	// otherwise use the appropriate mapping for the game type
	FILE* testfp = NULL;
	if (fopen_s(&testfp, mappingFile, "rb") != 0)
	{
		//XXX: placeholder: evaluate gametype (& target?) and choose appropriate mappings
		mappingFile = "province_mappings.txt";
	}
	else
	{
		fclose(testfp);
	}
	obj = doParseFile(mappingFile);
	provinceMapping provinceMap = initProvinceMap(obj);
	sourceWorld.checkAllProvincesMapped(provinceMap);

	// Get potential HoI3 countries
	log("Getting potential HoI3 nations.\n");
	printf("Getting potential HoI3 nations.\n");
	ifstream HoI3CountriesInput;
	HoI3CountriesInput.open( (HoI3Loc + "\\common\\countries.txt").c_str() );
	if (!HoI3CountriesInput.is_open())
	{
		log("Error: Could not open countries.txt\n");
		printf("Error: Could not open countries.txt\n");
		return 1;
	}
	destWorld.addPotentialCountries(HoI3CountriesInput, HoI3Loc);
	HoI3CountriesInput.close();
	
	// Get list of blocked nations
	log("Getting blocked HoI3 nations.\n");
	printf("Getting blocked HoI3 nations.\n");
	obj = doParseFile("blocked_nations.txt");
	vector<string> blockedNations = processBlockedNations(obj);

	// Parse unions mapping
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

	// Get country mappings
	log("Parsing country mappings.\n");
	printf("Parsing country mappings.\n");
	initParser();
	obj = doParseFile("country_mappings.txt");	

	// Map V2 nations to HoI3 nations
	log("Mapping V2 nations to HoI3 nations.\n");
	printf("Mapping V2 nations to HoI3 nations.\n");
	removeEmptyNations(sourceWorld);
	if (Configuration::getRemovetype() == "absorbed")
	{
		removeCulturallyAbsorbedNations(sourceWorld, unionMap);
	}
	else if (Configuration::getRemovetype() == "all")
	{
		removeLandlessNations(sourceWorld);
	}
	countryMapping countryMap;
	vector<string> V2Tags = getV2Tags(sourceWorld);
	int leftoverNations = initCountryMap(countryMap, V2Tags, destWorld.getPotentialTags(), blockedNations, obj);
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("Too many V2 nations (%d). Removing culturally-absorbed nations.\n", leftoverNations);
		printf("Too many V2 nations (%d). Removing culturally-absorbed nations.\n", leftoverNations);
		removeCulturallyAbsorbedNations(sourceWorld, unionMap);
		V2Tags = getV2Tags(sourceWorld);
		leftoverNations = initCountryMap(countryMap, V2Tags, destWorld.getPotentialTags(), blockedNations, obj);
	}
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("Too many V2 nations (%d). Removing landless nations.\n", leftoverNations);
		printf("Too many V2 nations (%d). Removing landless nations.\n", leftoverNations);
		removeLandlessNations(sourceWorld);
		V2Tags = getV2Tags(sourceWorld);
		leftoverNations = initCountryMap(countryMap, V2Tags, destWorld.getPotentialTags(), blockedNations, obj);
	}

	// Convert
	printf("Converting countries.\n");
	log("Converting countries.\n");
	destWorld.convertCountries(sourceWorld, countryMap);
	printf("Converting diplomacy.\n");
	log("Converting diplomacy.\n");
	destWorld.convertDiplomacy(sourceWorld, countryMap);
	printf("Converting provinces.\n");
	log("Converting provinces.\n");
	destWorld.convertProvinces(sourceWorld, provinceMap, countryMap);
	printf("Converting capitals.\n");
	log("Converting capitals.\n");
	destWorld.convertCapitals(sourceWorld, provinceMap);

	map<int, int> leaderIDMap; // <V2, HoI3>
	printf("Converting generals and admirals.\n");
	log("Converting generals and admirals.\n");
	//XXX: destWorld.convertLeaders(sourceWorld, leaderIDMap);
	printf("Converting armies and navies.\n");
	log("Converting armies and navies.\n");
	destWorld.convertArmies(sourceWorld, provinceMap, leaderIDMap);

	printf("Converting techs.\n");
	log("Converting techs.\n");
	destWorld.convertTechs(sourceWorld);

	printf("Setting up factions.\n");
	log("Setting up factions.\n");
	destWorld.configureFactions(countryMap);

	// Output results
	printf("Outputting save.\n");
	log("Outputting save.\n");
	FILE* output;
	if (fopen_s(&output, "output.hoi3", "w") != 0)
	{
		log("Error: could not open output.hoi3.\n");
		printf("Error: could not open output.hoi3.\n");
	}
	destWorld.output(output);
	fclose(output);

	closeLog();
	return 0;
}