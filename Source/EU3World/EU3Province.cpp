#include "EU3Province.h"
#include "EU3History.h"
#include "..\Parsers\Object.h"


EU3Province::EU3Province()
{
	num	= -1;
	owner	= "";
	cores.clear();
	inHRE	= false;
	discoveredBy.clear();
	history.clear();
}


void EU3Province::init(int newNum, Object* obj, date startDate, map< string, vector<string> >& mapSpreadStrings)
{
	num = newNum;
	vector<Object*> ownerObj = obj->getValue("owner");
	if (ownerObj.size() > 0)
	{
		owner = ownerObj[0]->getLeaf();
	}
	else
	{
		owner = "";
	}

	vector<Object*> discoveredByObj = obj->getValue("discovered_by");
	for (unsigned int i = 0; i < discoveredByObj.size(); i++)
	{
		vector<string> discoverers = mapSpreadStrings[ discoveredByObj[i]->getLeaf() ];
		for (unsigned int j = 0; j < discoverers.size(); j++)
		{
			discoveredBy.push_back( discoverers[j] );
		}
	}

	vector<Object*> objectList = obj->getLeaves();
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		string key = objectList[i]->getKey();
		if (key[0] == '1')
		{
			date histDate(key);
			if (histDate <= startDate)
			{
				vector<Object*> newOwnerObj = objectList[i]->getValue("owner");
				if (newOwnerObj.size() > 0)
				{
					owner = newOwnerObj[0]->getLeaf();
				}

				vector<Object*> discoveredByObj = obj->getValue("discovered_by");
				for (unsigned int i = 0; i < discoveredByObj.size(); i++)
				{
					vector<string> discoverers = mapSpreadStrings[ discoveredByObj[i]->getLeaf() ];
					for (unsigned int j = 0; j < discoverers.size(); j++)
					{
						discoveredBy.push_back( discoverers[j] );
					}
				}
			}
		}
	}

	inHRE = false;

	history.clear();
}


void EU3Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	if (owner != "")
	{
		fprintf(output, "	owner=\"%s\"\n", owner.c_str());
	}
	for (unsigned int i = 0; i < cores.size(); i++)
	{
		fprintf(output, "	core=\"%s\"\n", cores[i].c_str());
	}
	if (inHRE)
	{
		fprintf(output, "	hre=yes\n");
	}
	fprintf(output, "	history=\n");
	fprintf(output, "	{\n");
	for (unsigned int i = 0; i < cores.size(); i++)
	{
		fprintf(output, "\t\tadd_core=\"%s\"\n", cores[i].c_str());
	}
	if (owner != "")
	{
		fprintf(output, "\t\towner=\"%s\"\n", owner.c_str());
	}
	if (inHRE)
	{
		fprintf(output, "\t\thre=yes\n");
	}
	for (unsigned int i = 0; i < history.size(); i++)
	{
		history[i]->output(output);
	}
	fprintf(output, "	}\n");
	fprintf(output, "	discovery_dates={9999.1.1 9999.1.1 1458.4.30 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 }\n");
	fprintf(output, "	discovery_religion_dates={9999.1.1 1458.4.30 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 }\n");
	fprintf(output, "	discovered_by={ ");
	for (unsigned int i = 0; i < discoveredBy.size(); i++)
	{
		fprintf(output, "%s ", discoveredBy[i].c_str());
	}
	fprintf(output, "	}\n");
	fprintf(output, "}\n");
}



void EU3Province::setNumber(int newNumber)
{
	num = newNumber;
}


void EU3Province::addCore(string newCore)
{
	cores.push_back(newCore);
}


void EU3Province::setOwner(string newOwner)
{
	owner = newOwner;
}


void EU3Province::setInHRE(bool input)
{
	inHRE = input;
}


void EU3Province::setDiscoveredBy(vector<string> input)
{
	for (unsigned int i = 0; i < input.size(); i++)
	{
		discoveredBy.push_back(input[i]);
	}
}


void EU3Province::addAdvisor(EU3Advisor* newAdvisor)
{
	EU3History* newHistory = new EU3History;
	newHistory->initAdvisor(newAdvisor);
	history.push_back(newHistory);
}


string EU3Province::getOwner()
{
	return owner;
}