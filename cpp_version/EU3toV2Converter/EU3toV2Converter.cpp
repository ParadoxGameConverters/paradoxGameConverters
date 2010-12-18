// EU3toV2Converter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Parsers\CSVOneToManyParser.h"
#include "Parsers\Parser.h"
#include "Parsers\InstructionsParser.h"
#include "World.h"
#include "Mapper.h"
#include "RegionListing.h"
#include "Logger.h"
#include "VariableCalculator.h"

int main(int argc, char * argv[]) //changed from TCHAR, no use when everything else in program is in ASCII...
{
   std::string filename;
   CSVOneToManyParser parser;
   World euWorld;
   World vickyWorld;
   Mapper mapper;
   RegionListing regionListing;
   Object* obj;

   std::vector<oneToMany> provinceIDMap = parser.ProcessFile(std::string("1.csv"));

   std::ifstream read, read2, read3, read4, read5, read6;
   std::ostringstream stream;

   std::string inputFilename("input.eu3");
   if (argc >= 2) {
       inputFilename = argv[1];
   }
   else {
       std::cout << "No input file given, defaulting to input.eu3" << std::endl;
   }

   // Parsing EU World
   stream.str("");
   stream << "Main parsing EU World.";
   Logger::WriteLine(stream.str());
   
   initParser();
   obj = Parser::topLevel;
   read.open(inputFilename.c_str());
   if (!read.is_open())
   {
	   std::cout << "Could not open EU3 save." << std::endl;
   }
   readFile(read);
   euWorld.Init(obj);
   read.close();

   stream.str("");
   stream << "Main parsing Vicky World.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read2.open(std::string("input.v2").c_str());
	if (!read2.is_open())
	{
		std::cout << "Could not open input.v2" << std::endl;
	}
   readFile(read2);
   vickyWorld.Init(obj);
   read2.close(); 


   // Parsing province mappings
   stream.str("");
   stream << "Main parsing Province Mappings.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read3.open(std::string("province_mappings.txt").c_str()); 
	if (!read3.is_open())
	{
		std::cout << "Could not open province_mappings.txt" << std::endl;
	}
   readFile(read3);  
   read3.close();

   std::map<std::string, std::set<std::string> > vicFromEuProvinceMap = mapper.InitEUToVickyMap(obj);

   // Parsing country mappings
   stream.str("");
   stream << "Main parsing Country Mappings.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read4.open(std::string("country_mappings.txt").c_str());   
	if (!read4.is_open())
	{
		std::cout << "Could not open country_mappings.txt" << std::endl;
	}
   readFile(read4);  
   read4.close();

   std::map<std::string, std::set<std::string> > vicFromEuCountryMap = mapper.InitEUToVickyCountryMap(obj);

   // Generate region mapping

   stream.str("");
   stream << "Main parsing Region Structure.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read6.open(std::string("region.txt").c_str());
	if (!read6.is_open())
	{
		std::cout << "Could not open region.txt" << std::endl;
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
		std::cout << "Could not open ins.txt" << std::endl;
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
		std::cout << "Could not open " << (inputFilename + ".v2").c_str() << std::endl;
	}
   write << *(vickyWorld.GetSource()); 
   write.close(); 

   int bob = 0;
}

