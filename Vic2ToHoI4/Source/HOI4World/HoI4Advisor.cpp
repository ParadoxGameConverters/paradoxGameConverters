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



#include "HoI4Advisor.h"
#include "Object.h"



HoI4Advisor::HoI4Advisor(shared_ptr<Object> object):
	trait(object->safeGetString("traits")),
	picture(object->safeGetString("picture")),
	event(""),
	ideology(object->getKey())
{
	auto onAddObj = object->safeGetObject("on_add");
	if (onAddObj != nullptr)
	{
		auto eventObjs = onAddObj->getLeaves();
		event = eventObjs[0]->getLeaf();
	}
}


void HoI4Advisor::output(ofstream& output, const string& tag) const
{
	output << "\t\t" << tag << "_" << ideology << "_advisor = {\n";
	output << "\t\t\tallowed = {\n";
	output << "\t\t\t\toriginal_tag = \"" << tag << "\"\n";
	output << "\t\t\t}\n";
	output << "\t\t\ttraits = { " << trait << " }\n";
	if (event != "")
	{
		output << "\t\t\ton_add = {\n";
		output << "\t\t\t\tcountry_event = " << event << "\n";
		output << "\t\t\t}\n";
	}
	if (picture != "")
	{
		output << "\t\t\tpicture = " << picture << "\n";
	}
	output << "\t\t\tdo_effect = {\n";
	output << "\t\t\t\tNOT = {\n";
	output << "\t\t\t\t\thas_government = " << ideology << "\n";
	output << "\t\t\t\t}\n";
	output << "\t\t\t}\n";
	output << "\t\t\tai_will_do = {\n";
	output << "\t\t\t\tfactor = 0\n";
	output << "\t\t\t}\n";
	output << "\t\t}\n";
}


void HoI4Advisor::addEventNum(const int num)
{
	event += to_string(num);
}