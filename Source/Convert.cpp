#include <fstream>
#include <string>
#include <sys/stat.h>
#include "Log.h"
#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Parsers/Object.h"
#include "EU3World\EU3World.h"
#include "EU3World\EU3Country.h"
#include	"CK2World\CK2World.h"
#include "Mappers.h"
using namespace std;



int main(int argc, char * argv[])
{
	initLog();

	Object*	obj;				// generic object
	ifstream	read;				// ifstream for reading files


	//Get CK2 install location
	string CK2Loc = Configuration::getCK2Path();
	struct stat st;
	if (CK2Loc.empty() || (stat(CK2Loc.c_str(), &st) != 0))
	{
		log("No Crusader King 2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Crusader King 2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}

	//Verify EU3 install location
	string EU3Loc = Configuration::getEU3Path();
	if (EU3Loc.empty() || (stat(EU3Loc.c_str(), &st) != 0))
	{
		log("No Europa Universalis 3 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Europa Universalis 3 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}


	//Get Input CK2 save 
	string inputFilename("input.ck2");
	if (argc >= 2)
	{
		inputFilename = argv[1];
		log("Using input file %s.\n", inputFilename.c_str());
		printf("Using input file %s.\n", inputFilename.c_str());
	}
	else
	{
		log("No input file given, defaulting to input.ck2\n");
		printf("No input file given, defaulting to input.ck2\n");
	}


	// Parse CK2 Save
	log("Getting CK2 data.\n");
	printf("Getting CK2 data.\n");

	printf("	Getting traits\n");
	initParser();
	obj = getTopLevel();
	read.open(Configuration::getCK2Path() + "/common/traits/00_traits.txt");
	if (!read.is_open())
	{
		log("Error: Could not open 00_traits.txt!\n");
		printf("Error: Could not open 00_traits.txt!\n");
		return 1;
	}
	readFile(read);
	read.close();
	read.clear();
	CK2World srcWorld;
	srcWorld.addTraits(obj);

	printf("	Adding dynasties from dynasties.txt\n");
	initParser();
	obj = getTopLevel();
	read.open(Configuration::getCK2Path() + "/common/dynasties.txt");
	if (!read.is_open())
	{
		log("Error: Could not open dynasties.txt!\n");
		printf("Error: Could not open dynasties.txt!\n");
		return 1;
	}
	readFile(read);
	read.close();
	read.clear();
	srcWorld.addDynasties(obj);
	
	printf("	Parsing CK2 save.\n");
	initParser();
	obj = getTopLevel();
	read.open(inputFilename.c_str());
	if (!read.is_open())
	{
		log("Error: Could not open CK2 save (%s).\n", inputFilename.c_str());
		printf("Error: Could not open CK2 save (%s).\n", inputFilename.c_str());
		return 1;
	}
	readFile(read);
	read.close();
	read.clear();
	srcWorld.init(obj);


	// Parse province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	const char* mappingFile = "province_mappings.txt";
	obj = doParseFile(mappingFile);
	provinceMapping			provinceMap				= initProvinceMap(obj);
	inverseProvinceMapping	inverseProvinceMap	= invertProvinceMap(provinceMap);

	EU3World destWorld;
	destWorld.init(&srcWorld);


	// Get potential EU3 countries
	log("Getting potential EU3 nations.\n");
	printf("Getting potential EU3 nations.\n");
	destWorld.addPotentialCountries();
	
	// Get list of blocked nations
	log("Getting blocked EU3 nations.\n");
	printf("Getting blocked EU3 nations.\n");
	obj = doParseFile("blocked_nations.txt");
	vector<string> blockedNations = processBlockedNations(obj);

	// Get country mappings
	log("Parsing country mappings.\n");
	printf("Parsing country mappings.\n");
	initParser();
	obj = doParseFile("country_mappings.txt");	

	// Map CK2 nations to EU3 nations
	log("Mapping CK2 nations to EU3 nations.\n");
	printf("Mapping CK2 nations to EU3 nations.\n");
	countryMapping countryMap;
	int leftoverNations = initCountryMap(countryMap, srcWorld.getIndependentTitles(), destWorld.getCountries(), blockedNations, obj);
	if (leftoverNations == -1)
	{
		return 1;
	}
	else if (leftoverNations > 0)
	{
		log("Error: Too many CK2 nations (%d). Aborting.\n", leftoverNations);
		printf("Error: Too many CK2 nations (%d). Aborting.\n", leftoverNations);
		return -1;
	}

	destWorld.convertProvinces(provinceMap, srcWorld.getProvinces(), countryMap);
	destWorld.setupRotwProvinces(inverseProvinceMap);
	destWorld.convertAdvisors(inverseProvinceMap, srcWorld);
	for (countryMapping::iterator i = countryMap.begin(); i != countryMap.end(); i++)
	{
		i->second->convert(i->first);
	}
	



	// Output results
	printf("Outputting save.\n");
	log("Outputting save.\n");
	FILE* output;
	if (fopen_s(&output, "output.eu3", "w") != 0)
	{
		log("Error: could not open output.v2.\n");
		printf("Error: could not open output.v2.\n");
	}
	destWorld.output(output);
	fclose(output);



	closeLog();
	return 0;
}