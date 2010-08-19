// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"

/// Taken from RosettaCode.org and modified

std::vector<std::string> tokenize_str(const std::string & origStr,
				      const std::string & delims, 
				      const std::string & endpoints)
{
   using namespace std;
   string str = origStr;
   // Strip everything past endpoints
   string::size_type endPos = str.find_first_of(endpoints, 0);

   if (string::npos != endPos)
   {
      str = origStr.substr(0, endPos);
   }

   // Skip delims at beginning, find start of first token
   string::size_type lastPos = str.find_first_not_of(delims, 0);
   // Find next delimiter @ end of token
   string::size_type pos     = str.find_first_of(delims, lastPos);

   // output vector
   vector<string> tokens;

   while (string::npos != pos || string::npos != lastPos)
   {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delims.  Note the "not_of". this is beginning of token
      lastPos = str.find_first_not_of(delims, pos);
      // Find next delimiter at end of token.
      pos     = str.find_first_of(delims, lastPos);
   }

   return tokens;
}