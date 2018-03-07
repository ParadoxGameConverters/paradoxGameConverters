#ifndef PARADOX_PARSER_GENERIC_SWITCH_H
#define PARADOX_PARSER_GENERIC_SWITCH_H

/*
* The preprocessor define USE_GENERIC_PARADOX_PARSER controls whether all parsing is delegated to the generic parser
* Defining USE_GENERIC_PARADOX_PARSER will cause both ParadoxParserUTF8 and ParadoxParser8859_15 (both of which are not portable to linux at this point) to delegate all parsing to the generic parser
* Currently, USE_GENERIC_PARADOX_PARSER is set to 1 by default on linux builds, while Windows builds use the windows specific parsers
* Testing of the generic parser on windows can then be enabled by setting USE_GENERIC_PARADOX_PARSER on the compiler command line or using cmake
* The generic parser is currently implemented only on linux and is still a dummy but is designed to be portable to both Windows and linux
* It should also be able to handle all encodings used in Paradox game files
*/

#ifdef __linux__

//#define USE_GENERIC_PARADOX_PARSER 1

#endif

#ifdef USE_GENERIC_PARADOX_PARSER

#include "ParadoxParserGeneric.h"

#endif

#endif
