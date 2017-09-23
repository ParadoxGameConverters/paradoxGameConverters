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



#include "HoI4Navy.h"



HoI4Ship::HoI4Ship(const string& _name, const string& _type, const string& _equipment, const string& _owner):
	name(_name),
	type(_type),
	equipment(_equipment),
	owner(_owner)
{}


ofstream& operator << (ofstream& output, const HoI4Ship& instance)
{
	output << "\t\tship = {" << endl;
	output << "\t\t\tname = \"" << instance.name << "\"" << endl;
	output << "\t\t\tdefinition = " << instance.type << endl;
	output << "\t\t\tequipment = {" << endl;
	output << "\t\t\t\t" << instance.equipment << " = {" << endl;
	output << "\t\t\t\t\tamount = 1" << endl;
	output << "\t\t\t\t\towner = " << instance.owner << endl;
	output << "\t\t\t\t}" << endl;
	output << "\t\t\t}" << endl;
	if (instance.type == "carrier")
	{
		output << "\t\t\tair_wings = {" << endl;
		output << "\t\t\t\tcv_fighter_equipment_0 =  { owner = \"" << instance.owner << "\" amount = 8 } # historical: 36" << endl;
		output << "\t\t\t\tcv_CAS_equipment_1 =  { owner = \"" << instance.owner << "\" amount = 14 } # historical: 15" << endl;
		output << "\t\t\t\tcv_nav_bomber_equipment_1 =  { owner = \"" << instance.owner << "\" amount = 8 } # historical: 12" << endl;
		output << "\t\t\t}" << endl;
	}

	output << "\t\t}" << endl;

	return output;
}