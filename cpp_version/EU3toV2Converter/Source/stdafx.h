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
#include <set>
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

// Instructions, variables and rulesets

typedef enum
{
   INS_DO_NOTHING = 0,
   INS_SET_VALUE,	// Sets value specified by dblVal
   INS_ADD_VALUE,	// Adds value specified by dblVal
   INS_MUL_VALUE,	// Adds value specified by dblVal
   INS_SET_FLAG,	// Sets value specified by strVal's flag, multiplied by dblVal
   INS_ADD_FLAG,	// Adds value specified by strVal's flag, multiplied by dblVal
   INS_MUL_FLAG,	// Adds value specified by strVal's flag, multiplied by dblVal
   INS_SUM_PROVINCES,	// Returns the total of all provinces where a 'strVal' flag is set - in either the world or country
   INS_SUM_COUNTRIES,	// Returns the total of all countries where a 'strVal' flag is set
   INS_SET_MAX_VALUE,	// Forces a maximum value on variable
   INS_SET_MIN_VALUE	// Forces a minimum value on variable
} instructionType;

typedef enum
{
   WORLD_ILLEGAL = 0,
   WORLD_EU,
   WORLD_VICKY,
   WORLD_HOI
} worldType;

typedef enum
{
   VAR_ILLEGAL = 0,
   VAR_WORLD,
   VAR_COUNTRY,
   VAR_PROVINCE
} varType;

typedef struct  
{
   instructionType   type;
   double	     dblVal;
   std::string	     strVal;
} instruction;

typedef struct  
{
   varType	     type;
   worldType	     world;
   std::string	     name;

   std::vector<instruction> instructions;
} varDefinition;

// At present, ruleset is a mapping from A (rule\parameter name) to B (value)
typedef struct  
{
   std::string				  name;
   std::map<std::string, std::string>	  rules;
} ruleset;