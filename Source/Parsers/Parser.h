#ifndef PARSER_H
#define PARSER_H



#include <fstream>
using namespace std;



class Object;

Object*	getTopLevel();
void	clearStack(); 
bool	readFile(ifstream& read);
void	initParser();
Object* doParseFile(const char* filename);

#endif	// PARSER_H
