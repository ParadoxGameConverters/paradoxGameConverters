/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "V2Pop.h"
#include "../Log.h"



static int nextId = 23000;


V2Pop::V2Pop(string _type, int _size, string _culture, string _religion)
{
	id = nextId;
	nextId++;

	type						= _type;
	size						= _size;
	culture					= _culture;
	religion					= _religion;
	supportedRegiments	= 0;
	money						= 0.0;

	recalcMoney();
}

V2Pop::V2Pop(Object *obj)
{
	type = obj->getKey();

	vector<Object*> childObj = obj->getValue("id");
	if (childObj.size() > 0)
	{
		id = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("size");
	if (childObj.size() > 0)
	{
		size = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("money");
	if (childObj.size() > 0)
	{
		money = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("mil");
	if (childObj.size() > 0)
	{
		mil = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("con");
	if (childObj.size() > 0)
	{
		con = atoi(childObj[0]->getLeaf().c_str());
	}
	childObj = obj->getValue("literacy");
	if (childObj.size() > 0)
	{
		lit = atoi(childObj[0]->getLeaf().c_str());
	}
}

void V2Pop::output(FILE* output) const
{
	fprintf(output, "\t%s=\n", type.c_str());
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tid=%d\n", id);
	fprintf(output, "\t\tsize=%d\n", size);
	fprintf(output, "\t\t%s=%s\n", culture.c_str(), religion.c_str());
	fprintf(output, "\t\tmoney=%f\n", money);
	fprintf(output, "\t}\n");
}


bool V2Pop::combine(const V2Pop& rhs)
{
	if ((culture == rhs.culture) && (religion == rhs.religion) && (type == rhs.type))
	{
		size += rhs.size;
		recalcMoney();
		return true;
	}
	else
	{
		return false;
	}
}


void V2Pop::recalcMoney()
{
	money = 1000000.0;
	
	if (type == "aristocrats" || type == "capitalists")
	{
		money = 100.0 * size;
	}
	else if (type == "artisans" || type == "bureaucrats" || type == "clergymen" 
		  || type == "clerks" || type == "craftsmen" || type == "officers")
	{
		money = 10.0 * size;
	}
	else if (type == "farmers" || type == "labourers" || type == "soldiers")
	{
		money = 1.0 * size;
	}
	else if (type == "slaves")
	{
		money = 0.1 * size;
	}
	else
	{
		LOG(LogLevel::Warning) << "Unexpected pop type " << type;
	}
}


int getNextPopId()
{
	return nextId;
}
