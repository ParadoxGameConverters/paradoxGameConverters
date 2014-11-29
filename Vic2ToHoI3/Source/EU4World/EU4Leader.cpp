/*Copyright(c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


#include "EU4Leader.h"
#include "../Log.h"
#include "../Parsers/Object.h"



EU4Leader::EU4Leader(Object *obj)
{
	vector<Object*> subObj = obj->getValue("name");	// the field under consideration
	(subObj.size() > 0) ? name = subObj[0]->getLeaf() : name = "";

	subObj = obj->getValue("manuever");
	(subObj.size() > 0) ?  manuever = atoi(subObj[0]->getLeaf().c_str()) : manuever = 0;

	subObj = obj->getValue("fire");
	(subObj.size() > 0) ? fire = atoi(subObj[0]->getLeaf().c_str()) : fire = 0;

	subObj = obj->getValue("shock");
	(subObj.size() > 0) ? shock = atoi(subObj[0]->getLeaf().c_str()) : shock = 0;

	subObj = obj->getValue("siege");
	(subObj.size() > 0) ? siege = atoi(subObj[0]->getLeaf().c_str()) : siege = 0;

	subObj = obj->getValue("id");
	if (subObj.size() > 0)
	{
		subObj = subObj[0]->getValue("id");
		id = atoi(subObj[0]->getLeaf().c_str());
	}
	else
	{
		id = 0;
	}

	subObj = obj->getValue("type");
	(subObj.size() > 0) ? type = subObj[0]->getLeaf() : type = "";

	subObj = obj->getValue("activation");
	(subObj.size() > 0) ? activationDate = date(subObj[0]->getLeaf()) : activationDate = date();
}


bool EU4Leader::isLand() const
{
	if (type == "general" || type == "conquistador")
	{
		return true;
	}
	else if (type == "explorer" || type == "admiral")
	{
		return false;
	}
	else
	{
		LOG(LogLevel::Warning) << "Unknown leader type " << type;
	}
	return false;
}