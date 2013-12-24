/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "CK2Dynasty.h"
#include "CK2Character.h"
#include "..\Parsers\Object.h"



CK2Dynasty::CK2Dynasty(Object* obj)
{
	num = atoi( obj->getKey().c_str() );
	vector<Object*> nameLeaves = obj->getValue("name");
	if (nameLeaves.size() > 0)
	{
		name = nameLeaves[0]->getLeaf();
	}
	else
	{
		name = "";
	}
	members.clear();
}


CK2Dynasty::CK2Dynasty(int newNum, string newName)
{
	num	= newNum;
	name	= newName;
}


void CK2Dynasty::addMember(CK2Character* newMember)
{
	members.push_back(newMember);
}


CK2Character* CK2Dynasty::getSenoirityHeir(string genderLaw)
{
	CK2Character* heir = NULL;
	date heirBirthDate = "1500.12.31";

	// unless absolute cognatic, consider only males
	for(unsigned int i = 0; i < members.size(); i++)
	{
		if ( (!members[i]->isDead()) && (members[i]->getBirthDate() < heirBirthDate) && (!members[i]->isBastard()) && (!members[i]->isFemale() || (genderLaw == "true_cognatic")) )
		{
			heir				= members[i];
			heirBirthDate	= heir->getBirthDate();
		}
	}

	if (heir != NULL)
	{
		return heir;
	}

	// no heirs in male lines, so consider females
	for(unsigned int i = 0; i < members.size(); i++)
	{
		if ( (!members[i]->isDead()) && (members[i]->getBirthDate() < heirBirthDate) && (!members[i]->isBastard()) )
		{
			heir				= members[i];
			heirBirthDate	= heir->getBirthDate();
		}
	}

	return heir;
}