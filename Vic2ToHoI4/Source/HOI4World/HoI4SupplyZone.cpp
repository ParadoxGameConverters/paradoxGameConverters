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



#include "HoI4SupplyZone.h"
#include "Log.h"
#include "../Configuration.h"
#include <fstream>



HoI4SupplyZone::HoI4SupplyZone(int _ID, int _value)
{
	ID		= _ID;
	value	= _value;
}


void HoI4SupplyZone::output(string _filename)
{
	string filename("output/" + Configuration::getOutputName() + "/map/supplyareas/" + _filename);
	ofstream out(filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open \"output/input/map/supplyareas/" + _filename;
		exit(-1);
	}
	out << "supply_area={" << endl;
	out << "\tid=" << ID << endl;
	out << "\tname= \"SUPPLYAREA_" << ID << "\"" << endl;
	out << "\tvalue = " << value << endl;
	out << "\tstates={" << endl;
	out << "\t\t";
	for (auto stateNum: states)
	{
		out << stateNum << " ";
	}
	out << endl;
	out << "\t}" << endl;
	out << "}" << endl;

	out.close();
}