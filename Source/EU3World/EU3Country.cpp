#include "EU3Country.h"
#include "..\Log.h"
#include "..\Parsers\Parser.h"
#include "..\Parsers\Object.h"
#include "..\Configuration.h"
#include "..\Date.h"
#include "..\CK2World\CK2Title.h"
#include "..\CK2World\CK2History.h"
#include "..\CK2World\CK2Character.h"
#include "EU3Ruler.h"
#include "EU3History.h"
#include <fstream>
using namespace std;


EU3Country::EU3Country()
{
	tag				= "";
	historyFile		= "";
	government		= "";
	monarch			= NULL;
	heir				= NULL;
	history.clear();
	previousMonarchs.clear();
}


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

	// Parse history file
	Object* obj;
	obj = doParseFile( (Configuration::getEU3Path() + "/history/countries/" + historyFile).c_str() );

	// Set objects from top of history file
	vector<Object*> govLeaves = obj->getValue("government");
	if (govLeaves.size() > 0)
	{
		government = govLeaves[0]->getLeaf();
	}

	monarch	= NULL;
	heir		= NULL;

	vector<Object*> techLeaves = obj->getValue("technology_group");
	if (techLeaves.size() > 0)
	{
		techGroup = techLeaves[0]->getLeaf();
	}

	// update items based on history
	vector<Object*> objectList = obj->getLeaves();
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		string key = objectList[i]->getKey();
		if (key[0] == '1')
		{
			date histDate(key);
			if (histDate <= startDate)
			{
				EU3History* newHistory = new EU3History;

				vector<Object*> newMonarchObj = objectList[i]->getValue("monarch");
				if (newMonarchObj.size() > 0)
				{
					monarch = new EU3Ruler(newMonarchObj[0]);
					previousMonarchs.push_back(monarch);
					newHistory->initMonarch(monarch, histDate);
					history.push_back(newHistory);
				}

				vector<Object*> newHeirObj = objectList[i]->getValue("heir");
				if (newHeirObj.size() > 0)
				{
					heir = new EU3Ruler(newHeirObj[0]);
					newHistory->initHeir(heir, histDate);
					history.push_back(newHistory);
				}

				vector<Object*> techLeaves = obj->getValue("technology_group");
				if (techLeaves.size() > 0)
				{
					techGroup = techLeaves[0]->getLeaf();
				}
			}
		}
	}
	if (previousMonarchs.size() > 0)
	{
		previousMonarchs.pop_back();
	}
}


void EU3Country::convert(CK2Title* src)
{
	government = "";
	monarch = NULL;
	history.clear();
	previousMonarchs.clear();

	date ascensionDate;
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
			ascensionDate = newHistory->getWhen();
		}
	}
	for (vector<EU3Ruler*>::iterator i = previousMonarchs.begin(); i != previousMonarchs.end(); i++)
	{
		for(vector<EU3Ruler*>::iterator j = previousMonarchs.begin(); j != i; j++)
		{
			if ( (*i)->getName() == (*j)->getName())
			{
				(*i)->setRegnalNum( (*j)->getRegnalNum() + 1 );
			}
		}
	}
	if (previousMonarchs.size() > 0)
	{
		previousMonarchs.pop_back();
	}

	CK2Character* newHeir = src->getHeir();
	if (newHeir != NULL)
	{
		heir = new EU3Ruler(newHeir);

		date when = newHeir->getBirthDate();
		if (when < ascensionDate)
		{
			when = ascensionDate;
		}

		EU3History* newHistory = new EU3History();
		newHistory->initHeir(heir, when);
		history.push_back(newHistory);
	}
}


string EU3Country::getTag()
{
	return tag;
}


string EU3Country::getTechGroup()
{
	return techGroup;
}