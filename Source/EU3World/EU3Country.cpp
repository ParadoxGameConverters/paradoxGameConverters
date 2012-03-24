#include "EU3Country.h"
#include "..\Log.h"
#include "..\Parsers\Parser.h"
#include "..\Configuration.h"
#include "..\Date.h"
#include <fstream>
using namespace std;


void EU3Country::output(FILE* output)
{
	fprintf(output, "%s=\n", tag.c_str());
	fprintf(output, "{\n");
	fprintf(output, "	history=\n");
	fprintf(output, "	{\n");
	fprintf(output, "		1399.10.13=\n");
	fprintf(output, "		{\n");
	if (monarch != NULL)
	{
		monarch->output(output);
	}
	fprintf(output, "		}\n");
	fprintf(output, "	}\n");
	if(government != "")
	{
		fprintf(output, "	government=%s\n", government.c_str());
	}
	if (monarch != NULL)
	{
		fprintf(output, "	monarch=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		id=%d\n", monarch->getID());
		fprintf(output, "		type=37\n");
		fprintf(output, "	}\n");
	}
	fprintf(output, "}\n");
}


void EU3Country::init(string newTag, string newHistoryFile, date startDate)
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

	vector<Object*> leaves = obj->getValue("government");
	if (leaves.size() > 0)
	{
		government = leaves[0]->getLeaf();
	}

	monarch = NULL;
	vector<Object*> objectList = obj->getLeaves();
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		string key = objectList[i]->getKey();
		if (key[0] == '1')
		{
			date histDate(key);
			if (histDate <= startDate)
			{
				vector<Object*> newMonarchObj = objectList[i]->getValue("monarch");
				if (newMonarchObj.size() > 0)
				{
					monarch = new EU3Ruler(newMonarchObj[0]);
				}
			}
		}
	}
}


void EU3Country::convert(CK2Title* src)
{
	government = "";

	//delete monarch; TODO: find out why this crashes things
	string	name		= "";
	int		dip		= 1;
	int		adm		= 1;
	int		mil		= 1;
	string	dynasty	= "blank";
	CK2Character* holder = src->getHolder();
	if (holder != NULL)
	{
		name = holder->getName();
		CK2Dynasty* dynPointer = holder->getDynasty();
		if (dynPointer != NULL)
		{
			dynasty = dynPointer->getName();
		}
		else
		{
			log("Error: %s (holder of %s) does not have a dynasty!\n", name.c_str(), tag.c_str());
		}
	}
	else
	{
		log("Error: %s does not have a holder!\n", tag.c_str());
	}
	monarch = new EU3Ruler(name, dip, adm, mil, dynasty);
}


string EU3Country::getTag()
{
	return tag;
}