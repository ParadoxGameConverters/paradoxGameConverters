#include "stdafx.h"
#include <stdio.h>
#include "Parsers\Parser.h"
//#include "Parsers\InstructionsParser.h"
#include "Mapper.h"
#include "Log.h"
//#include "VariableCalculator.h"
#include "EU3World.h"
#include "V2World.h"
using namespace std;


int main(int argc, char * argv[]) //changed from TCHAR, no use when everything else in program is in ASCII...
{
	initLog();

	Object*	obj;				// generic object
	ifstream	read;				// ifstream for reading files


	// Get Input EU3 save
	string inputFilename("input.eu3");
	if (argc >= 2) {
		inputFilename = argv[1];
		log("Using input file %s.\n", inputFilename.c_str());
		printf("Using input file %s.\n", inputFilename.c_str());
	}
	else {
		log("No input file given, defaulting to input.eu3\n");
		printf("No input file given, defaulting to input.eu3\n");
	}


	// Parse EU3 Save
	log("Parsing EU3 save.\n");
	printf("Parsing EU3 save.\n");
	
	initParser();
	obj = Parser::topLevel;
	read.open(inputFilename.c_str());
	if (!read.is_open())
	{
		log("Error: Could not open EU3 save (%s).\n", inputFilename.c_str());
		printf("Error: Could not open EU3 save (%s).\n", inputFilename.c_str());
		return 1;
	}
	readFile(read);
	read.close();
	EU3World sourceWorld;
	sourceWorld.init(obj);


	// Parse V2 input file
	log("Parsing Vicky2 input save.\n");
	printf("Parsing Vicky2 input save.\n");
	
	initParser();
	obj = Parser::topLevel;
	read.open(std::string("input.v2").c_str());
	if (!read.is_open())
	{
		log("Error: Could not open input.v2\n");
		printf("Error: Could not open input.v2\n");
		return 1;
	}
	readFile(read);
	read.close();
	V2World destWorld;
	destWorld.init(obj);


	// Parsing province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	
	initParser();
	obj = Parser::topLevel;
	read.open(std::string("province_mappings.txt").c_str()); 
	if (!read.is_open())
	{
		log("Error: Could not open province_mappings.txt\n");
		printf("Error: Could not open province_mappings.txt\n");
		return 1;
	}
	readFile(read);  
	read.close();

	provinceMapping provinceMap = initProvinceMap(obj);


	// Parsing country mappings
	log("Parsing country mappings.\n");
	printf("Parsing country mappings.\n");
	
	initParser();
	obj = Parser::topLevel;
	read.open(std::string("country_mappings.txt").c_str());	
	if (!read.is_open())
	{
		log("Error: Could not open country_mappings.txt\n");
		printf("Error: Could not open country_mappings.txt\n");
		return 1;
	}
	readFile(read);  
	read.close();

	countryMapping countryMap = initCountryMap(obj);


	// Generate region mapping
	log("Parsing region structure.\n");
	printf("Parsing region structure.\n");

	initParser();
	obj = Parser::topLevel;
	read.open(std::string("region.txt").c_str());
	if (!read.is_open())
	{
		log("Error: Could not open region.txt\n");
		printf("Error: Could not open region.txt\n");
		return 1;
	}
	readFile(read);  
	read.close();

	if (obj->getLeaves().size() < 1)
	{
		log("This is where a TODO: error was. If you see this, bug a programmer so we can put in a useful message.\n");
		printf("This is where a TODO: error was. If you see this, bug a programmer so we can put in a useful message.\n");
		return 1;
	}
	stateMapping stateMap;
	stateMap = initStateMap(obj->getLeaves()[0]);


	// Convert
	destWorld.convertCountries(sourceWorld, countryMap);
	destWorld.convertProvinces(sourceWorld, provinceMap, countryMap);
	destWorld.setupStates(stateMap);
	  
	/*InstructionsParser insParser;
	InstructionsParser::Refresh();
	read.open(std::string("ins.txt").c_str());
	if (!read.is_open())
	{
		log("Error: Could not open ins.txt\n");
		printf("Error: Could not open ins.txt\n");
		return 1;
	}
	readInsFile(read);	
	read.close();

	VariableCalculator::Instance()->SetWorlds(&euWorld, &vickyWorld);
	VariableCalculator::Instance()->ProcessVariables(InstructionsParser::GetProcessedVars());
	VariableCalculator::Instance()->ProcessRules(InstructionsParser::GetProcessedRulesets());
	InstructionsParser::Refresh();*/

	// Output results
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

