#include <fstream>
#include <string>
#include "Log.h"
#include "Parsers/Parser.h"
#include "Parsers/Object.h"
using namespace std;



int main(int argc, char * argv[])
{
	initLog();

	Object*	obj;				// generic object
	ifstream	read;				// ifstream for reading files


	//Get Input CK2 save 
	string inputFilename("input.ck2");
	if (argc >= 2)
	{
		inputFilename = argv[1];
		log("Using input file %s.\n", inputFilename.c_str());
		printf("Using input file %s.\n", inputFilename.c_str());
	}
	else
	{
		log("No input file given, defaulting to input.ck2\n");
		printf("No input file given, defaulting to input.ck2\n");
	}


	// Parse CK2 Save
	log("Parsing CK2 save.\n");
	printf("Parsing CK2 save.\n");
	
	initParser();
	obj = getTopLevel();
	read.open(inputFilename.c_str());
	if (!read.is_open())
	{
		log("Error: Could not open CK2 save (%s).\n", inputFilename.c_str());
		printf("Error: Could not open CK2 save (%s).\n", inputFilename.c_str());
		return 1;
	}
	readFile(read);
	read.close();
	read.clear();



	closeLog();
	return 0;
}