// EU3toV2Converter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Parsers\CSVOneToManyParser.h"
#include "Parsers\Parser.h"
#include "Parsers\InstructionsParser.h"
#include "World.h"
#include "Mapper.h"

int _tmain(int argc, _TCHAR* argv[])
{
   std::string filename;
   CSVOneToManyParser parser;
   World euWorld;
   World vickyWorld;
   Object* obj;

  // std::vector<oneToMany> result = parser.ProcessFile(std::string("C:\\Documents and Settings\\mstseglo\\My Documents\\Visual Studio 2005\\Projects\\EU3toV2Converter\\Supplementary\\EU3_to_V2\\Book1.csv"));
   std::vector<oneToMany> provinceIDMap = parser.ProcessFile(std::string("1.csv"));

   std::ifstream read, read2, read3, read4;

   
   initParser();
   obj = Parser::topLevel;
   read.open(std::string("1.eu3").c_str());   
   readFile(read);
   euWorld.Init(obj);
   read.close();

   initParser();
   obj = Parser::topLevel;
   read2.open(std::string("1.v2").c_str());   
   readFile(read2);
   vickyWorld.Init(obj);
   read2.close(); 

   initParser();
   obj = Parser::topLevel;
   read3.open(std::string("province_mappings.txt").c_str());   
   readFile(read3);  
   read3.close();

   Mapper mapper;
   std::map<std::string, std::set<std::string> > vicFromEuProvinceMap = mapper.InitEUToVickyMap(obj);

   initParser();
   obj = Parser::topLevel;
   read4.open(std::string("country_mappings.txt").c_str());   
   readFile(read4);  
   read4.close();

   std::map<std::string, std::set<std::string> > vicFromEuCountryMap = mapper.InitEUToVickyMap(obj);

   mapper.MapProvinces(vicFromEuProvinceMap, euWorld, vickyWorld);
   mapper.MapCountries(vicFromEuCountryMap, euWorld, vickyWorld);
   mapper.AssignProvinceOwnership(euWorld, vickyWorld);
   
   std::ofstream write;
   char fname[200];
   Parser::topLevel = vickyWorld.GetSource();
   write.open(std::string("output.v2").c_str());   
   write << *(vickyWorld.GetSource()); 
   write.close(); 

   /*
   InstructionsParser insParser;
   read3.open(std::string("ins.txt").c_str());   
   readInsFile(read3);   
   read3.close();
*/

   int bob = 0;
}

