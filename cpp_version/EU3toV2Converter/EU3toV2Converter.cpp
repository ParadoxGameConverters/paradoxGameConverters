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

int _tmain(int argc, _TCHAR* argv[])
{
   std::string filename;
   CSVOneToManyParser parser;
   World euWorld;
   World vickyWorld;
   Mapper mapper;
   RegionListing regionListing;
   Object* obj;

  // std::vector<oneToMany> result = parser.ProcessFile(std::string("C:\\Documents and Settings\\mstseglo\\My Documents\\Visual Studio 2005\\Projects\\EU3toV2Converter\\Supplementary\\EU3_to_V2\\Book1.csv"));
   std::vector<oneToMany> provinceIDMap = parser.ProcessFile(std::string("1.csv"));

   std::ifstream read, read2, read3, read4, read5, read6;
   std::ostringstream stream;

   // Parsing EU World
   stream.str("");
   stream << "Main parsing EU World.";
   Logger::WriteLine(stream.str());
   
   initParser();
   obj = Parser::topLevel;
   read.open(std::string("input.eu3").c_str());   
   readFile(read);
   euWorld.Init(obj);
   read.close();

   stream.str("");
   stream << "Main parsing Vicky World.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read2.open(std::string("input.v2").c_str());   
   readFile(read2);
   vickyWorld.Init(obj);
   read2.close(); 

   stream.str("");
   stream << "Main parsing Province Mappings.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read3.open(std::string("province_mappings.txt").c_str());   
   readFile(read3);  
   read3.close();

   
   std::map<std::string, std::set<std::string> > vicFromEuProvinceMap = mapper.InitEUToVickyMap(obj);

   stream.str("");
   stream << "Main parsing Country Mappings.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read4.open(std::string("country_mappings.txt").c_str());   
   readFile(read4);  
   read4.close();

   std::map<std::string, std::set<std::string> > vicFromEuCountryMap = mapper.InitEUToVickyMap(obj);

   // Generate region mapping

   stream.str("");
   stream << "Main parsing Region Structure.";
   Logger::WriteLine(stream.str());

   initParser();
   obj = Parser::topLevel;
   read6.open(std::string("region.txt").c_str());   
   readFile(read6);  
   read6.close();

   regionListing.Init(obj);

   mapper.MapProvinces(vicFromEuProvinceMap, euWorld, vickyWorld);
   mapper.MapCountries(vicFromEuCountryMap, euWorld, vickyWorld);
   mapper.AssignProvinceOwnership(euWorld, vickyWorld, regionListing);
   mapper.SetupStates(vickyWorld, regionListing);
     
   InstructionsParser insParser;
   InstructionsParser::Refresh();
   read5.open(std::string("ins.txt").c_str());   
   readInsFile(read5);   
   read5.close();

   VariableCalculator::Instance()->SetWorlds(&euWorld, &vickyWorld);
   VariableCalculator::Instance()->ProcessVariables(InstructionsParser::GetProcessedVars());
   VariableCalculator::Instance()->ProcessRules(InstructionsParser::GetProcessedRulesets());
   InstructionsParser::Refresh();

   std::ofstream write;
   Parser::topLevel = vickyWorld.GetSource();
   write.open(std::string("output.v2").c_str());   
   write << *(vickyWorld.GetSource()); 
   write.close(); 

   int bob = 0;
}

