// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "..\stdafx.h"
#include "CSVOneToManyParser.h"

std::vector<oneToMany> CSVOneToManyParser::ProcessFile(std::string& filename)
{   
   std::vector<oneToMany> values;
   std::vector<std::string> tokens;
   std::ifstream inputFile;
   std::string line;

   inputFile.open(filename.c_str());

   while (inputFile.good())
   {
      oneToMany entry;

      std::getline(inputFile, line);
      tokens = tokenize_str(line);
      
      if (tokens.size() > 1)
      {
	 entry.key = tokens[0];
	 for (int i = 1; i < tokens.size(); i++)
	 {
	    entry.values.push_back(tokens[i]);
	 }

	 values.push_back(entry);
      }
      else
      {
	 // TODO - Error logging
      }
   }

   return values;
}
