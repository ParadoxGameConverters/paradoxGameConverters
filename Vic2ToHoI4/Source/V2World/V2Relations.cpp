/*Copyright (c) 2017 The Paradox Game Converters Project

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



#include "V2Relations.h"
#include "Object.h"



V2Relations::V2Relations(shared_ptr<Object> relationsObj)
{
	tag = relationsObj->getKey();

	setValue(relationsObj);
	setMilitaryAccess(relationsObj);
	setLastDiplomat(relationsObj);
	setLastWar(relationsObj);
	setTruce(relationsObj);
	setLevel(relationsObj);
}


void V2Relations::setValue(shared_ptr<Object> relationsObj)
{
	vector<shared_ptr<Object>> valueObjs = relationsObj->getValue("value");
	if (valueObjs.size() > 0)
	{
		value = stoi(valueObjs[0]->getLeaf());
	}
	else
	{
		value = 0;
	}
}


void V2Relations::setMilitaryAccess(shared_ptr<Object> relationsObj)
{
	vector<shared_ptr<Object>> maObjs = relationsObj->getValue("military_access");
	if (maObjs.size() > 0)
	{
		militaryAccess = (maObjs[0]->getLeaf() == "yes");
	}
	else
	{
		militaryAccess = false;
	}
}


void V2Relations::setLastDiplomat(shared_ptr<Object> relationsObj)
{
	vector<shared_ptr<Object>> lastSendObjs = relationsObj->getValue("last_send_diplomat");
	if (lastSendObjs.size() > 0)
	{
		lastSentDiplomat = date(lastSendObjs[0]->getLeaf());
	}
}


void V2Relations::setLastWar(shared_ptr<Object> relationsObj)
{
	vector<shared_ptr<Object>> lastWarObjs = relationsObj->getValue("last_war");
	if (lastWarObjs.size() > 0)
	{
		lastWar = date(lastWarObjs[0]->getLeaf());
	}
}


void V2Relations::setTruce(shared_ptr<Object> relationsObj)
{
	vector<shared_ptr<Object>> truceUntilObjs = relationsObj->getValue("truce_until");
	if (truceUntilObjs.size() > 0)
	{
		truceUntil = date(truceUntilObjs[0]->getLeaf());
	}
}


void V2Relations::setLevel(shared_ptr<Object> relationsObj)
{
	vector<shared_ptr<Object>> levelObjs = relationsObj->getValue("level");
	if (levelObjs.size() > 0)
	{
		level = stoi(levelObjs[0]->getLeaf());
	}
	else
	{
		level = 2;
	}
}
