// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

// TODO: reference additional headers your program requires here

typedef struct  
{
   std::string key;
   std::vector<std::string> values;
} oneToMany;

// Supplementary stuff goes here
std::vector<std::string> tokenize_str(const std::string & origStr,
				      const std::string & delims=",; \t", 
				      const std::string & endpoints="#");