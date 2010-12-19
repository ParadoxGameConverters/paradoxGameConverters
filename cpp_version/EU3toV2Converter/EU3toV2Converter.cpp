#include "stdafx.h"
#include <stdio.h>
#include "Parsers\Parser.h"
#include "Parsers\InstructionsParser.h"
#include "World.h"
#include "Mapper.h"
#include "RegionListing.h"
#include "Log.h"
#include "VariableCalculator.h"
using namespace std;


int main(int argc, char * argv[]) //changed from TCHAR, no use when everything else in program is in ASCII...
{
	initLog();

	World vickyWorld;
	Mapper mapper;
	RegionListing regionListing;


	std::ifstream read2, read3, read4, read5, read6;
	

	string inputFilename("input.eu3");
	if (argc >= 2) {
		inputFilename = argv[1];
	}
	else {
		log("No input file given, defaulting to input.eu3\n");
		printf("No input file given, defaulting to input.eu3\n");
	}


	// Parsing EU World
	Object* obj;				//generic object

	log("Parsing EU3 save.\n");
	printf("Parsing EU3 save.\n");
	
	initParser();

	obj = Parser::topLevel;
	ifstream read;
	read.open(inputFilename.c_str());
	if (!read.is_open())
	{
		log("Error: Could not open EU3 save.\n");
		printf("Error: Could not open EU3 save.\n");
		return 1;
	}
	readFile(read);
	World euWorld;
	euWorld.Init(obj);
	read.close();

	log("Parsing Vicky2 input save.\n");
	printf("Parsing Vicky2 input save.\n");
	
	initParser();
	obj = Parser::topLevel;
	read2.open(std::string("input.v2").c_str());
	if (!read2.is_open())
	{
		log("Error: Could not open input.v2\n");
		printf("Error: Could not open input.v2\n");
	}
	readFile(read2);
	vickyWorld.Init(obj);
	read2.close(); 


	// Parsing province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	
	initParser();
	obj = Parser::topLevel;
	read3.open(std::string("province_mappings.txt").c_str()); 
	if (!read3.is_open())
	{
		log("Error: Could not open province_mappings.txt\n");
		printf("Error: Could not open province_mappings.txt\n");
		return 1;
	}
	readFile(read3);  
	read3.close();

	std::map<std::string, std::set<std::string> > vicFromEuProvinceMap = mapper.InitEUToVickyMap(obj);

	// Parsing country mappings
	log("Parsing country mappings.\n");
	printf("Parsing country mappings.\n");
	
	initParser();
	obj = Parser::topLevel;
	read4.open(std::string("country_mappings.txt").c_str());	
	if (!read4.is_open())
	{
		log("Error: Could not open country_mappings.txt\n");
		printf("Error: Could not open country_mappings.txt\n");
		return 1;
	}
	readFile(read4);  
	read4.close();

	std::map<std::string, std::set<std::string> > vicFromEuCountryMap = mapper.InitEUToVickyCountryMap(obj);

	// Generate region mapping
	log("Parsing region structure.\n");
	printf("Parsing region structure.\n");

	initParser();
	obj = Parser::topLevel;
	read6.open(std::string("region.txt").c_str());
	if (!read6.is_open())
	{
		log("Error: Could not open region.txt\n");
		printf("Error: Could not open region.txt\n");
	}
	readFile(read6);  
	read6.close();

	if (obj->getLeaves().size() < 1)
	{
		// TODO: error;
		return 1;
	}
	regionListing.Init(obj->getLeaves()[0]);


	mapper.MapProvinces(vicFromEuProvinceMap, euWorld, vickyWorld);
	mapper.MapCountries(vicFromEuCountryMap, euWorld, vickyWorld);
	mapper.AssignProvinceOwnership(euWorld, vickyWorld, regionListing);
	mapper.AssignCountryCapitals(euWorld, vickyWorld);
	mapper.SetupStates(vickyWorld, regionListing);
	  
	InstructionsParser insParser;
	InstructionsParser::Refresh();
	read5.open(std::string("ins.txt").c_str());
	if (!read5.is_open())
	{
		log("Error: Could not open ins.txt\n");
		printf("Error: Could not open ins.txt\n");
		return 1;
	}
	readInsFile(read5);	
	read5.close();

	VariableCalculator::Instance()->SetWorlds(&euWorld, &vickyWorld);
	VariableCalculator::Instance()->ProcessVariables(InstructionsParser::GetProcessedVars());
	VariableCalculator::Instance()->ProcessRules(InstructionsParser::GetProcessedRulesets());
	InstructionsParser::Refresh();

	std::ofstream write;
	Parser::topLevel = vickyWorld.GetSource();
	write.open((inputFilename + ".v2").c_str());
	if (!write.is_open())
	{
		log("Error: Could not open %s.v2\n", inputFilename.c_str());
		printf("Error: Could not open %s.v2\n", inputFilename.c_str());
		return 1;
	}
	write << *(vickyWorld.GetSource()); 
	write.close(); 

	int bob = 0;

	closeLog();
}

