#include <fstream>
#include <string>
#include <sys/stat.h>
#include <io.h>
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


	// Input CK2 Data
	log("Getting CK2 data.\n");
	printf("Getting CK2 data.\n");
	CK2World srcWorld;

	log("\tGetting traits\n");
	printf("\tGetting traits\n");
	struct _finddata_t	traitsData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (CK2Loc + "\\common\\traits\\*").c_str(), &traitsData)) == -1L)
	{
		log("\t\tError: Could not open traits directory.\n");
		printf("\t\tError: Could not open traits directory.\n");
		exit(1);
	}
	do
	{
		if (strcmp(traitsData.name, ".") == 0 || strcmp(traitsData.name, "..") == 0 )
		{
			continue;
		}
		obj = doParseFile((Configuration::getCK2Path() + "\\common\\traits\\" + traitsData.name).c_str());
		srcWorld.addTraits(obj);
	} while(_findnext(fileListing, &traitsData) == 0);
	_findclose(fileListing);

	log("\tAdding dynasties from CK2 Install\n");
	printf("\tAdding dynasties from CK2 Install\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/dynasties.txt").c_str()); // for pre-1.06 installs
	struct _finddata_t	dynastiesData;
	if ( (fileListing = _findfirst( (CK2Loc + "\\common\\dynasties\\*").c_str(), &dynastiesData)) == -1L)
	{
		log("\t\tError: Could not open dynasties directory.\n");
		printf("\t\tError: Could not open dynasties directory.\n");
		exit(1);
	}
	do
	{
		if (strcmp(dynastiesData.name, ".") == 0 || strcmp(dynastiesData.name, "..") == 0 )
		{
			continue;
		}
		obj = doParseFile((Configuration::getCK2Path() + "\\common\\dynasties\\" + dynastiesData.name).c_str());
		srcWorld.addDynasties(obj);
	} while(_findnext(fileListing, &dynastiesData) == 0);
	_findclose(fileListing);

	
	log("Parsing CK2 save.\n");
	printf("Parsing CK2 save.\n");
	obj = doParseFile(inputFilename.c_str());

	log("Importing parsed data.\n");
	printf("Importing parsed data.\n");
	srcWorld.init(obj);


	// Parse province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	const char* mappingFile = "province_mappings.txt";
	obj = doParseFile(mappingFile);
	provinceMapping			provinceMap				= initProvinceMap(obj, srcWorld.getVersion());
	inverseProvinceMapping	inverseProvinceMap	= invertProvinceMap(provinceMap);
	map<int, CK2Province*> srcProvinces				= srcWorld.getProvinces();
	for (map<int, CK2Province*>::iterator i = srcProvinces.begin(); i != srcProvinces.end(); i++)
	{
		inverseProvinceMapping::iterator p = inverseProvinceMap.find(i->first);
		if ( p == inverseProvinceMap.end() )
		{
			log("	Error: CK2 province %d has no mapping specified!\n", i->first);
		}
		else if ( p->second.size() == 0 )
		{
			log("	Warning: CK2 province %d is not mapped to any EU3 provinces!\n", i->first);
		}
	}

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

	// Get culture mappings
	log("Parsing culture mappings.\n");
	printf("Parsing culture mappings.\n");
	obj = doParseFile("culture_mappings.txt");
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse culture_mappings.txt.\n");
		printf("Error: Failed to parse culture_mappings.txt.\n");
		return 1;
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(obj->getLeaves()[0]);


	// Convert
	log("Converting countries.\n");
	printf("Converting countries.\n");
	destWorld.convertCountries(countryMap);

	log("Converting provinces.\n");
	printf("Converting provinces.\n");
	destWorld.convertProvinces(provinceMap, srcWorld.getProvinces(), countryMap, cultureMap);

	log("Setting up ROTW provinces.\n");
	printf("Setting up ROTW provinces.\n");
	destWorld.setupRotwProvinces(inverseProvinceMap);

	log("Converting advisors.\n");
	printf("Converting advisors.\n");
	destWorld.convertAdvisors(inverseProvinceMap, provinceMap, srcWorld);
	


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


	log("Complete.\n");
	printf("Complete.\n");
	closeLog();
	return 0;
}
