#include <io.h>
#include <stdexcept>
#include <fstream>
#include <sys/stat.h>
#include <io.h>
#include <Windows.h>
#include "Configuration.h"
#include "Log.h"
#include "Parsers\Parser.h"
#include "EU3World/EU3World.h"
#include "EU3World/EU3Religion.h"
#include "EU3World/EU3Localisation.h"
#include "V2World/V2World.h"
#include "V2World/V2Factory.h"
#include "V2World/V2TechSchools.h"
#include "V2World/V2LeaderTraits.h"



// Converts the given EU3 save into a V2 mod.
// Returns 0 on success or a non-zero failure code on error.
int ConvertEU3ToV2(const std::string& EU3SaveFileName)
{
	Object*	obj;				// generic object
	ifstream	read;				// ifstream for reading files

	Configuration::getInstance();

	char curDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curDir);
	LOG(LogLevel::Debug) << "Current directory is " << curDir;


	//Get V2 install location
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

	//Get EU3 install location
	LOG(LogLevel::Debug) << "Get EU3 Install Path";
	string EU3Loc = Configuration::getEU3Path();
	if (EU3Loc.empty() || (_stat(EU3Loc.c_str(), &st) != 0))
	{
		LOG(LogLevel::Error) << "No Europa Universalis 3 path was specified in configuration.txt, or the path was invalid";
		return (-2);
	}
	else
	{
		LOG(LogLevel::Debug) << "EU3 install path is " << EU3Loc;
	}

	// Get EU3 Mod
	LOG(LogLevel::Debug) << "Get EU3 Mod";
	string fullModPath;
	string modName = Configuration::getEU3Mod();
	if (modName != "")
	{
		fullModPath = EU3Loc + "\\mod\\" + modName;
		if (fullModPath.empty() || (_stat(fullModPath.c_str(), &st) != 0))
		{
			LOG(LogLevel::Error) << modName << " could not be found at the specified directory.  A valid path and mod must be specified.";
			return (-2);
		}
		else
		{
			LOG(LogLevel::Debug) << "EU3 Mod directory is " << fullModPath;
		}
	}

	//get output name
	int slash = EU3SaveFileName.find_last_of("\\");
	int length = EU3SaveFileName.find_first_of(".") - slash - 1;
	string outputName = EU3SaveFileName.substr(slash + 1, length);
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

	LOG(LogLevel::Info) << "* Importing EU3 save *";

	// Parse EU3 Save
	LOG(LogLevel::Info) << "Parsing save";
	obj = doParseFile(EU3SaveFileName.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << EU3SaveFileName;
		exit(-1);
	}

	// Read all localisations.
	LOG(LogLevel::Info) << "Reading localisation";
	EU3Localisation localisation;
	localisation.ReadFromAllFilesInFolder(Configuration::getEU3Path() + "\\localisation");
	if (!fullModPath.empty())
	{
		LOG(LogLevel::Debug) << "Reading mod localisation";
		localisation.ReadFromAllFilesInFolder(fullModPath + "\\localisation");
	}

	// Construct world from EU3 save.
	LOG(LogLevel::Info) << "Building world";
	EU3World sourceWorld(localisation, obj);

	// Read EU3 common\countries
	LOG(LogLevel::Info) << "Reading EU3 common\\countries";
	{
		ifstream commonCountries(Configuration::getEU3Path() + "\\common\\countries.txt");
		sourceWorld.readCommonCountries(commonCountries, Configuration::getEU3Path());
		if (!fullModPath.empty())
		{
			ifstream convertedCommonCountries(fullModPath + "\\common\\countries.txt");
			sourceWorld.readCommonCountries(convertedCommonCountries, fullModPath);
		}
	}

	// Figure out what EU3 gametype we're using
	WorldType game = sourceWorld.getWorldType();
	switch (game)
	{
		case VeryOld:
			LOG(LogLevel::Error) << "EU3 game appears to be from an old version; only IN, HttT, and DW are supported.";
			exit(1);
		case InNomine:
			LOG(LogLevel::Info) << "Game type is: EU3 In Nomine.  EXPERIMENTAL.";
			break;
		case HeirToTheThrone:
			LOG(LogLevel::Info) << "Game type is: EU3 Heir to the Throne.";
			break;
		case DivineWind:
			LOG(LogLevel::Info) << "Game type is: EU3 Divine Wind.";
			break;
		default:
			LOG(LogLevel::Error) << "Error: Could not determine savegame type.";
			exit(1);
	}


	// Resolve unit types
	LOG(LogLevel::Info) << "Resolving unit types.";
	RegimentTypeMap rtm;
	read.open("unit_strength.txt");
	if (read.is_open())
	{
		read.close();
		read.clear();
		LOG(LogLevel::Info) << "\tReading unit strengths from unit_strength.txt";
		obj = doParseFile("unit_strength.txt");
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file unit_strength.txt";
			exit(-1);
		}
		for (int i = 0; i < num_reg_categories; ++i)
		{
			AddCategoryToRegimentTypeMap(obj, (RegimentCategory)i, RegimentCategoryNames[i], rtm);
		}
	}
	else
	{
		LOG(LogLevel::Info) << "Reading unit strengths from EU3 installation folder";
		struct _finddata_t unitFileData;
		intptr_t fileListing;
		if ( (fileListing = _findfirst( (EU3Loc + "\\common\\units\\*.txt").c_str(), &unitFileData)) == -1L)
		{
			LOG(LogLevel::Info) << "Could not open units directory.";
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
			AddUnitFileToRegimentTypeMap((EU3Loc + "\\common\\units"), unitName, rtm);
		} while(_findnext(fileListing, &unitFileData) == 0);
		_findclose(fileListing);
	}
	read.close();
	read.clear();
	sourceWorld.resolveRegimentTypes(rtm);


	// Merge nations
	LOG(LogLevel::Info) << "Merging nations.";
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


	// Construct factory factory
	LOG(LogLevel::Info) << "Determining factory allocation rules.";
	V2FactoryFactory factoryBuilder;


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
	initProvinceMap(obj, sourceWorld.getWorldType(), provinceMap, inverseProvinceMap, resettableProvinces);
	sourceWorld.checkAllProvincesMapped(inverseProvinceMap);


	// Get country mappings
	CountryMapping countryMap;
	countryMap.ReadRules("country_mappings.txt");
	countryMap.CreateMapping(sourceWorld, destWorld);

	// Get adjacencies
	LOG(LogLevel::Info) << "Importing adjacencies";
	adjacencyMapping adjacencyMap = initAdjacencyMap();

	// Generate continent mapping
	string EU3Mod = Configuration::getEU3Mod();
	continentMapping continentMap;
	if (EU3Mod != "")
	{
		string continentFile = Configuration::getEU3Path() + "\\mod\\" + EU3Mod + "\\map\\continent.txt";
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
		obj = doParseFile((EU3Loc + "\\map\\continent.txt").c_str());
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file " << EU3Loc << "\\map\\continent.txt";
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
		obj = doParseFile((V2Loc + "\\map\\region.txt").c_str());
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
	stateIndexMapping stateIndexMap;
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

	unionCulturesMap unionCultures;
	if (EU3Mod != "")
	{
		string modCultureFile = Configuration::getEU3Path() + "\\mod\\" + EU3Mod + "\\common\\cultures.txt";
		if ((_stat(modCultureFile.c_str(), &st) == 0))
		{
			obj = doParseFile(modCultureFile.c_str());
			if ((obj != NULL) && (obj->getLeaves().size() > 0))
			{
				initUnionCultures(obj, unionCultures);
			}
		}
	}
	if (unionCultures.size() == 0)
	{
		obj = doParseFile((EU3Loc + "\\common\\cultures.txt").c_str());
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file " << EU3Loc << "\\common\\cultures.txt";
			exit(-1);
		}
		if (obj->getLeaves().size() < 1)
		{
			LOG(LogLevel::Error) << "Failed to parse cultures.txt";
			return 1;
		}
		initUnionCultures(obj, unionCultures);
	}

	// Parse EU3 Religions
	LOG(LogLevel::Info) << "Parsing EU3 religions";
	bool parsedReligions = false;
	if (EU3Mod != "")
	{
		string modReligionFile = Configuration::getEU3Path() + "\\mod\\" + EU3Mod + "\\common\\religion.txt";
		if ((_stat(modReligionFile.c_str(), &st) == 0))
		{
			obj = doParseFile(modReligionFile.c_str());
			if ((obj != NULL) && (obj->getLeaves().size() > 0))
			{
				EU3Religion::parseReligions(obj);
				parsedReligions = true;
			}
		}
	}
	if (!parsedReligions)
	{
		obj = doParseFile((EU3Loc + "\\common\\religion.txt").c_str());
		if (obj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file " << EU3Loc << "\\common\\religion.txt";
			exit(-1);
		}
		if (obj->getLeaves().size() < 1)
		{
			LOG(LogLevel::Error) << "Failed to parse religion.txt.";
			return 1;
		}
		EU3Religion::parseReligions(obj);
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


	//Parse tech schools
	LOG(LogLevel::Info) << "Parsing tech schools.";
	initParser();
	obj = doParseFile("blocked_tech_schools.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file blocked_tech_schools.txt";
		exit(-1);
	}
	vector<string> blockedTechSchools;
	blockedTechSchools = initBlockedTechSchools(obj);
	initParser();
	obj = doParseFile( (V2Loc + "\\common\\technology.txt").c_str() );
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << V2Loc << "\\common\\technology.txt";
		exit(-1);
	}
	vector<techSchool> techSchools;
	techSchools = initTechSchools(obj, blockedTechSchools);


	// Get Leader traits
	LOG(LogLevel::Info) << "Getting leader traits";
	V2LeaderTraits lt;
	map<int, int> leaderIDMap; // <EU3, V2>


	// Convert
	LOG(LogLevel::Info) << "Converting countries";
	destWorld.convertCountries(sourceWorld, countryMap, cultureMap, unionCultures, religionMap, governmentMap, inverseProvinceMap, techSchools, leaderIDMap, lt);
	LOG(LogLevel::Info) << "Converting diplomacy";
	destWorld.convertDiplomacy(sourceWorld, countryMap);
	LOG(LogLevel::Info) << "Converting provinces";
	destWorld.convertProvinces(sourceWorld, provinceMap, resettableProvinces, countryMap, cultureMap, religionMap, stateIndexMap);
	LOG(LogLevel::Info) << "Setting colonies";
	destWorld.setupColonies(adjacencyMap, continentMap);
	LOG(LogLevel::Info) << "Creating states";
	destWorld.setupStates(stateMap);
	LOG(LogLevel::Info) << "Setting unciv reforms";
	destWorld.convertUncivReforms();
	LOG(LogLevel::Info) << "Creating pops";
	//destWorld.setupPops(sourceWorld);
	LOG(LogLevel::Info) << "Adding unions";
	destWorld.addUnions(unionMap);
	LOG(LogLevel::Info) << "Converting armies and navies";
	//destWorld.convertArmies(sourceWorld, inverseProvinceMap, leaderIDMap);
	LOG(LogLevel::Info) << "Converting techs";
	destWorld.convertTechs(sourceWorld);
	LOG(LogLevel::Info) << "Allocating starting factories";
	destWorld.allocateFactories(sourceWorld, factoryBuilder);

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
	fprintf(modFile, "replace = \"common/religion.txt\"\n");
	fprintf(modFile, "replace = \"common/cultures.txt\"\n");
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
		const char* const defaultEU3SaveFileName = "input.EU3";
		string EU3SaveFileName;
		if (argc >= 2)
		{
			EU3SaveFileName = argv[1];
			LOG(LogLevel::Info) << "Using input file " << EU3SaveFileName;
		}
		else
		{
			EU3SaveFileName = defaultEU3SaveFileName;
			LOG(LogLevel::Info) << "No input file given, defaulting to " << defaultEU3SaveFileName;
		}
		ConvertEU3ToV2(EU3SaveFileName);
	}
	catch (const std::exception& e)
	{
		LOG(LogLevel::Error) << e.what();
		return -1;
	}
}