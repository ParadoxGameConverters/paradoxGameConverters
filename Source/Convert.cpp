#include <fstream>
#include <string>
#include <sys/stat.h>
#include <io.h>
#include <functional>
#include "Log.h"
#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Parsers/Object.h"
#include "EU3World\EU3World.h"
#include "EU3World\EU3Country.h"
#include "EU3World\EU3Tech.h"
#include "CK2World\CK2World.h"
#include "CK2World\CK2Opinion.h"
#include "CK2World\CK2Religion.h"
#include "Mappers.h"
using namespace std;


bool doParseDirectoryContents(const std::string& directory, std::function<void(Object* obj)> predicate)
{
	Object*				obj;
	struct _finddata_t	data;
	intptr_t			fileListing;
	if ( (fileListing = _findfirst( (directory + "*").c_str(), &data)) == -1L)
	{
		return false;
	}
	do
	{
		if (strcmp(data.name, ".") == 0 || strcmp(data.name, "..") == 0 )
		{
			continue;
		}
		obj = doParseFile((directory + data.name).c_str());
		if (obj == NULL)
		{
			log("Error: Could not open %s\n", (directory + data.name).c_str());
			printf("Error: Could not open %s\n", (directory + data.name).c_str());
			exit(-1);
		}
		predicate(obj);
	} while(_findnext(fileListing, &data) == 0);
	_findclose(fileListing);

	return true;
}


int main(int argc, char * argv[])
{
	initLog();

	Object*	obj;				// generic object
	ifstream	read;				// ifstream for reading files


	//Get CK2 install location
	string CK2Loc = Configuration::getCK2Path();
	if (CK2Loc[CK2Loc.length() - 1] == '\\')
	{
		CK2Loc = CK2Loc.substr(0, CK2Loc.length() - 1);
	}
	struct stat st;
	if (CK2Loc.empty() || (stat(CK2Loc.c_str(), &st) != 0))
	{
		log("No Crusader King 2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		printf("No Crusader King 2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.\n");
		return (-2);
	}

	//Verify EU3 install location
	string EU3Loc = Configuration::getEU3Path();
	if (EU3Loc[EU3Loc.length() - 1] == '\\')
	{
		EU3Loc = EU3Loc.substr(0, EU3Loc.length() - 1);
	}
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

	log("\tGetting building types.\n");
	printf("\tGetting building types.\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/buildings.txt").c_str()); // for pre-1.06 installs
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/buildings.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/buildings.txt").c_str());
		exit(-1);
	}
	srcWorld.addBuildingTypes(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\buildings\\"), [&](Object* eachobj) { srcWorld.addBuildingTypes(eachobj); }))
	{
		log("\t\tError: Could not open buildings directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open buildings directory (ok for pre-1.06).\n");
	}

	log("\tGetting CK2 religions\n");
	printf("\tGetting CK2 religions\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/religion.txt").c_str()); // for pre-1.06 installs
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/religion.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/religion.txt").c_str());
		exit(-1);
	}
	CK2Religion::parseReligions(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\religions\\"), [&](Object* eachobj) { CK2Religion::parseReligions(eachobj); }))
	{
		log("\t\tError: Could not open religions directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open religions directory (ok for pre-1.06).\n");
	}

	log("\tGetting CK2 cultures\n");
	printf("\tGetting CK2 cultures\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/cultures.txt").c_str()); // for pre-1.06 installs
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/cultures.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/cultures.txt").c_str());
		exit(-1);
	}
	cultureGroupMapping CK2CultureGroupMap;
	addCultureGroupMappings(obj, CK2CultureGroupMap);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\cultures\\"), [&](Object* eachobj) { addCultureGroupMappings(eachobj, CK2CultureGroupMap); }))
	{
		log("\t\tError: Could not open cultures directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open cultures directory (ok for pre-1.06).\n");
	}

	log("\tParsing landed titles.\n");
	printf("\tParsing landed titles.\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/landed_titles.txt").c_str()); // for pre-1.06 installs
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/landed_titles.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/landed_titles.txt").c_str());
		exit(-1);
	}
	srcWorld.addPotentialTitles(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\landed_titles\\"), [&](Object* eachobj) { srcWorld.addPotentialTitles(eachobj); }))
	{
		log("\t\tError: Could not open landed_titles directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open landed_titles directory (ok for pre-1.06).\n");
	}

	log("\tGetting traits\n");
	printf("\tGetting traits\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/traits.txt").c_str()); // for pre-1.06 installs
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/traits.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/traits.txt").c_str());
		exit(-1);
	}
	srcWorld.addTraits(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\traits\\"), [&](Object* eachobj) { srcWorld.addTraits(eachobj); }))
	{
		log("\t\tError: Could not open traits directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open traits directory (ok for pre-1.06).\n");
	}


	log("\tGetting opinion modifiers\n");
	printf("\tGetting opinion modifiers\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/opinion_modifiers.txt").c_str()); // for pre-1.06 installs
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/opinion_modifiers.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/opinion_modifiers.txt").c_str());
		exit(-1);
	}
	CK2Opinion::initOpinions(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\opinion_modifiers\\"), [&](Object* eachobj) { CK2Opinion::initOpinions(eachobj); }))
	{
		log("\t\tError: Could not open opinion_modifiers directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open opinion_modifiers directory (ok for pre-1.06).\n");
	}


	log("\tAdding dynasties from CK2 Install\n");
	printf("\tAdding dynasties from CK2 Install\n");
	obj = doParseFile((Configuration::getCK2Path() + "/common/dynasties.txt").c_str()); // for pre-1.06 installs
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/dynasties.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/dynasties.txt").c_str());
		exit(-1);
	}
	srcWorld.addDynasties(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\dynasties\\"), [&](Object* eachobj) { srcWorld.addDynasties(eachobj); }))
	{
		log("\t\tError: Could not open dynasties directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open dynasties directory (ok for pre-1.06).\n");
	}
	
	log("Parsing CK2 save.\n");
	printf("Parsing CK2 save.\n");
	obj = doParseFile(inputFilename.c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", inputFilename.c_str());
		printf("Error: Could not open %s\n", inputFilename.c_str());
		exit(-1);
	}

	log("Importing parsed data.\n");
	printf("Importing parsed data.\n");
	srcWorld.init(obj, CK2CultureGroupMap);

	log("Merging top-level titles.\n");
	printf("Merging top-level titles.\n");
	srcWorld.mergeTitles();


	// Parse province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	const char* mappingFile = "province_mappings.txt";
	obj = doParseFile(mappingFile);
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", mappingFile);
		printf("Error: Could not open %s\n", mappingFile);
		exit(-1);
	}
	provinceMapping			provinceMap				= initProvinceMap(obj, srcWorld.getVersion());
	inverseProvinceMapping	inverseProvinceMap	= invertProvinceMap(provinceMap);
	//map<int, CK2Province*> srcProvinces				= srcWorld.getProvinces();
	//for (map<int, CK2Province*>::iterator i = srcProvinces.begin(); i != srcProvinces.end(); i++)
	//{
	//	inverseProvinceMapping::iterator p = inverseProvinceMap.find(i->first);
	//	if ( p == inverseProvinceMap.end() )
	//	{
	//		log("\tError: CK2 province %d has no mapping specified!\n", i->first);
	//	}
	//	else if ( p->second.size() == 0 )
	//	{
	//		log("\tWarning: CK2 province %d is not mapped to any EU3 provinces!\n", i->first);
	//	}
	//}

	// Parse techs
	log("Setting up tech groups.\n");
	printf("Setting up tech groups.\n");
	obj					= doParseFile( (Configuration::getEU3Path() + "/common/technology.txt").c_str() );
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technology.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technology.txt").c_str());
		exit(-1);
	}
	Object* govObj		= doParseFile( (Configuration::getEU3Path() + "/common/technologies/government.txt").c_str() );
	if (govObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/government.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/government.txt").c_str());
		exit(-1);
	}
	Object* prodObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/production.txt").c_str() );
	if (prodObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/production.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/production.txt").c_str());
		exit(-1);
	}
	Object* tradeObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/trade.txt").c_str() );
	if (tradeObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/trade.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/trade.txt").c_str());
		exit(-1);
	}
	Object* navalObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/naval.txt").c_str() );
	if (navalObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/naval.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/naval.txt").c_str());
		exit(-1);
	}
	Object* landObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/land.txt").c_str() );
	if (landObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/land.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/land.txt").c_str());
		exit(-1);
	}
	EU3Tech* techData = new EU3Tech(srcWorld.getEndDate(), obj, govObj, prodObj, tradeObj, navalObj, landObj);

	EU3World destWorld(&srcWorld, techData);
	
	// Add historical EU3 countries
	log("Adding historical EU3 nations.\n");
	printf("Adding historical EU3 nations.\n");
	destWorld.addHistoricalCountries();
	
	// Get list of blocked nations
	log("Getting blocked EU3 nations.\n");
	printf("Getting blocked EU3 nations.\n");
	obj = doParseFile("blocked_nations.txt");
	if (obj == NULL)
	{
		log("Error: Could not open blocked_nations.txt\n");
		printf("Error: Could not open blocked_nations.txt\n");
		exit(-1);
	}
	vector<string> blockedNations = processBlockedNations(obj);

	// Get EU3 Culture Groups
	log("Getting EU3 cultures\n");
	printf("Getting EU3 cultures\n");
	obj = doParseFile((Configuration::getEU3Path() + "/common/cultures.txt").c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/cultures.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/cultures.txt").c_str());
		exit(-1);
	}
	cultureGroupMapping EU3CultureGroupMap;
	addCultureGroupMappings(obj, EU3CultureGroupMap);

	// Get EU3 Religion Groups
	log("Getting EU3 religions\n");
	printf("Getting EU3 religions\n");
	obj = doParseFile((Configuration::getEU3Path() + "/common/religion.txt").c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/religion.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/religion.txt").c_str());
		exit(-1);
	}
	religionGroupMapping EU3ReligionGroupMap;
	addReligionGroupMappings(obj, EU3ReligionGroupMap);
	obj = doParseFile((Configuration::getEU3Path() + "/mod/Converter/common/religion.txt").c_str()); // in case the mod is being used
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/mod/Converter/common/religion.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/mod/Converter/common/religion.txt").c_str());
	}
	else
	{
		addReligionGroupMappings(obj, EU3ReligionGroupMap);
	}

	// Get culture mappings
	log("Parsing culture mappings.\n");
	printf("Parsing culture mappings.\n");
	obj = doParseFile("culture_mappings.txt");
	if (obj == NULL)
	{
		log("Error: Could not open culture_mappings.txt\n");
		printf("Error: Could not open culture_mappings.txt\n");
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse culture_mappings.txt.\n");
		printf("Error: Failed to parse culture_mappings.txt.\n");
		return 1;
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(obj->getLeaves()[0]);

	// Get religion mappings
	log("Parsing religion mappings.\n");
	printf("Parsing religion mappings.\n");
	obj = doParseFile("religion_mappings.txt");
	if (obj == NULL)
	{
		log("Error: Could not open religion_mappings.txt\n");
		printf("Error: Could not open religion_mappings.txt\n");
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse religion_mappings.txt.\n");
		printf("Error: Failed to parse religion_mappings.txt.\n");
		return 1;
	}
	religionMapping religionMap;
	religionMap = initReligionMap(obj->getLeaves()[0]);

	// Get continents
	log("Parsing continents.\n");
	printf("Parsing continents.\n");
	obj = doParseFile((Configuration::getEU3Path() + "\\map\\continent.txt").c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\map\\continent.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\map\\continent.txt").c_str());
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse continent.txt.\n");
		printf("Error: Failed to parse continent.txt.\n");
		return 1;
	}
	continentMapping continentMap;
	continentMap = initContinentMap(obj);

	// Get adjacencies
	log("Importing adjacencies\n");
	printf("Importing adjacencies\n");
	adjacencyMapping adjacencyMap = initAdjacencyMap();

	// Get trade good data
	log("Importing trade good data.\n");
	printf("Importing trade good data.\n");
	obj = doParseFile((Configuration::getEU3Path() + "\\common\\Prices.txt").c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\common\\Prices.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\common\\Prices.txt").c_str());
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse Prices.txt.\n");
		printf("Error: Failed to parse Prices.txt.\n");
		return 1;
	}
	tradeGoodMapping tradeGoodMap = initTradeGoodMapping(obj);


	// Convert
	log("Converting countries.\n");
	printf("Converting countries.\n");
	destWorld.convertCountries(srcWorld.getAllTitles(), religionMap, cultureMap, inverseProvinceMap);

	log("Setting up provinces.\n");
	printf("Setting up provinces.\n");
	destWorld.setupProvinces(provinceMap);

	log("Converting provinces.\n");
	printf("Converting provinces.\n");
	Object* positionsObj = doParseFile((Configuration::getEU3Path() + "\\map\\positions.txt").c_str());
	destWorld.convertProvinces(provinceMap, srcWorld.getProvinces(), cultureMap, religionMap, continentMap, adjacencyMap, tradeGoodMap, EU3ReligionGroupMap, positionsObj);

	// Map CK2 nations to EU3 nations
	log("Parsing country mappings.\n");
	printf("Parsing country mappings.\n");
	obj = doParseFile("country_mappings.txt");
	if (obj == NULL)
	{
		log("Error: Could not open country_mappings.txt\n");
		printf("Error: Could not open country_mappings.txt\n");
		exit(-1);
	}
	log("Mapping CK2 nations to EU3 nations.\n");
	printf("Mapping CK2 nations to EU3 nations.\n");
	destWorld.assignTags(obj, blockedNations, provinceMap);

	log("Adding accepted cultures.\n");
	printf("Adding accepted cultures.\n");
	destWorld.addAcceptedCultures();

	log("Converting centes of trade\n");
	printf("Converting centes of trade\n");
	destWorld.convertCoTs();
	
	log("Converting economies.\n");
	printf("Converting economies.\n");
	destWorld.convertEconomies(EU3CultureGroupMap, tradeGoodMap);

	log("Converting tech.\n");
	printf("Converting tech.\n");
	destWorld.convertTech(srcWorld);

	log("Converting governments.\n");
	printf("Converting governments.\n");
	destWorld.convertGovernments();

	log("Converting advisors.\n");
	printf("Converting advisors.\n");
	destWorld.convertAdvisors(inverseProvinceMap, provinceMap, srcWorld);

	log("Converting diplomatic relations.\n");
	printf("Converting diplomatic relations.\n");
	destWorld.convertDiplomacy();

	log("Converting armies and navies.\n");
	printf("Converting armies and navies.\n");
	destWorld.convertArmies(inverseProvinceMap);

	// Output results
	printf("Outputting save.\n");
	log("Outputting save.\n");
	string outputFilename;
	if (argc >= 2)
	{
		outputFilename = inputFilename.substr(0, inputFilename.find_last_of('.')) + ".eu3";
	}
	else
	{
		outputFilename = "output.eu3";
	}
	FILE* output;
	if (fopen_s(&output, outputFilename.c_str(), "w") != 0)
	{
		log("Error: could not open output.eu3.\n");
		printf("Error: could not open output.eu3.\n");
	}
	destWorld.output(output);
	fclose(output);


	log("Complete.\n");
	printf("Complete.\n");
	closeLog();
	return 0;
}