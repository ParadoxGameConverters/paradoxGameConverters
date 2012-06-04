#ifndef PARSER_H
#define PARSER_H

#include "Object.h"

Object*	getTopLevel();
void	clearStack(); 
bool	readFile(ifstream& read);
void	initParser();
Object* doParseFile(const char* filename);

#endif	// PARSER_H
