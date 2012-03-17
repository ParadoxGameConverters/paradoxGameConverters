#include "EU3Province.h"


void EU3Province::init(int newNum, Object* obj, date startDate)
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
			}
		}
	}

	inHRE = false;
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