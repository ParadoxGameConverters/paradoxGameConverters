/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "HoI4Agreement.h"
#include <fstream>



HoI4Agreement::HoI4Agreement(const string& _country1, const string& _country2, const V2Agreement* oldAgreement):
	type(oldAgreement->getType()),
	country1(_country1),
	country2(_country2),
	relationshipValue(0),
	startDate(oldAgreement->getDate())
{
}


HoI4Agreement::HoI4Agreement(const string& _country1, const string& _country2, const string& _type, int _relationshipValue, const date& _startDate):
	type(_type),
	country1(_country1),
	country2(_country2),
	relationshipValue(_relationshipValue),
	startDate(_startDate)
{
}


ofstream& operator << (ofstream& output, HoI4Agreement& instance)
{
	output << instance.type << "=\n";
	output << "{\n";
	output << "\tfirst=\"" << instance.country1 << "\"\n";
	output << "\tsecond=\"" << instance.country2 << "\"\n";
	if (instance.type == "relation")
	{
		output << "\tvalue=\"%i\"\n" << instance.relationshipValue;
	}
	output << "\tstart_date=\"%s\"\n" << instance.startDate;
	output << "\tend_date=\"1949.1.1\"\n";
	output << "}\n";
	output << "\n";
}