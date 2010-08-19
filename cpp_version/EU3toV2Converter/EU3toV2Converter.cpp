// EU3toV2Converter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Parsers\CSVOneToManyParser.h"
#include "Parsers\Parser.h"
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

   std::ifstream read, read2;

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

   Mapper mapper;

   mapper.MapProvinces(provinceIDMap, euWorld, vickyWorld);
   mapper.AssignProvinceOwnership(vickyWorld);

   std::ofstream write;
   char fname[200];
//   std::string date = vickyWorld->GetSource()->getLeaf("date");
//   saveYear = atoi(date.substr(1, 4).c_str());
//   sprintf(fname, "converted%i.eug", saveYear); 
//   write.open(fname);
   Parser::topLevel = vickyWorld.GetSource();
   write.open(std::string("1a.v2").c_str());   
   write << *(vickyWorld.GetSource()); 
   write.close(); 


   int bob = 0;
}

