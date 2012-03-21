#include "EU3Country.h"
#include "..\Log.h"
#include "..\Parsers\Parser.h"
#include "..\Configuration.h"
#include <fstream>
using namespace std;


void EU3Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	fprintf(output, "}\n");
}


void EU3Country::init(string newTag, string newHistoryFile)
{
	tag			= newTag;
	historyFile	= newHistoryFile;

	Object* obj;
	ifstream read;
	string EU3Loc = Configuration::getEU3Path();

	initParser();
	obj = getTopLevel();
	read.open((EU3Loc + "/history/countries/" + historyFile).c_str());
	if (!read.is_open())
	{
		log("Error: Could not open EU3 country file (%s).\n", historyFile.c_str());
		printf("Error: Could not open EU3 country file (%s).\n", historyFile.c_str());
	}
	readFile(read);
	read.close();
	read.clear();


}


string EU3Country::getTag()
{
	return tag;
}