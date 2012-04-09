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
	for (unsigned int i = 0; i < history.size(); i++)
	{
		history[i]->output(output);
	}
	fprintf(output, "	}\n");
	if (government != "")
	{
		fprintf(output, "	government=%s\n", government.c_str());
	}
	else
	{
		fprintf(output, "	government=tribal_despotism\n");
	}
	if (monarch != NULL)
	{
		fprintf(output, "	monarch=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		id=%d\n", monarch->getID());
		fprintf(output, "		type=37\n");
		fprintf(output, "	}\n");
	}
	for (unsigned int i = 0; i < previousMonarchs.size(); i++)
	{
		fprintf(output, "	previous_monarch=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		id=%d\n", previousMonarchs[i]->getID());
		fprintf(output, "		type=37\n");
		fprintf(output, "	}\n");
	}
	if (heir != NULL)
	{
		fprintf(output, "	heir=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		id=%d\n", heir->getID());
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

	heir = NULL;
}


void EU3Country::convert(CK2Title* src)
{
	government = "";
	monarch = NULL;

	vector<CK2History*> oldHistory = src->getHistory();
	for (unsigned int i = 0; i < oldHistory.size(); i++)
	{
		EU3History* newHistory = new EU3History();
		newHistory->init(oldHistory[i]);
		history.push_back(newHistory);

		if (newHistory->getMonarch() != NULL)
		{
			previousMonarchs.push_back(newHistory->getMonarch());
		}

		if ( (oldHistory[i]->getHolder() != NULL) && (src->getHolder() == oldHistory[i]->getHolder()) )
		{
			monarch = newHistory->getMonarch();
		}
	}
	previousMonarchs.pop_back();

	CK2Character* newHeir = src->getHeir();
	if (newHeir != NULL)
	{
		heir = new EU3Ruler(newHeir);

		EU3History* newHistory = new EU3History();
		newHistory->initHeir(heir);
		history.push_back(newHistory);
	}
	
}


string EU3Country::getTag()
{
	return tag;
}