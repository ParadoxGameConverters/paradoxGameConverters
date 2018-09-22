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



#include "HoI4Navy.h"



HoI4::Ship::Ship(const std::string& _name, const std::string& _type, const std::string& _equipment, const std::string& _owner):
	name(_name),
	type(_type),
	equipment(_equipment),
	owner(_owner)
{}


std::ofstream& HoI4::operator << (std::ofstream& output, const HoI4::Ship& instance)
{
	output << "\t\tship = {\n";
	output << "\t\t\tname = \"" << instance.name << "\"\n";
	output << "\t\t\tdefinition = " << instance.type << "\n";
	output << "\t\t\tequipment = {\n";
	output << "\t\t\t\t" << instance.equipment << " = {\n";
	output << "\t\t\t\t\tamount = 1\n";
	output << "\t\t\t\t\towner = " << instance.owner << "\n";
	output << "\t\t\t\t}\n";
	output << "\t\t\t}\n";
	if (instance.type == "carrier")
	{
		output << "\t\t\tair_wings = {\n";
		output << "\t\t\t\tcv_fighter_equipment_0 =  { owner = \"" << instance.owner << "\" amount = 8 } # historical: 36\n";
		output << "\t\t\t\tcv_CAS_equipment_1 =  { owner = \"" << instance.owner << "\" amount = 14 } # historical: 15\n";
		output << "\t\t\t\tcv_nav_bomber_equipment_1 =  { owner = \"" << instance.owner << "\" amount = 8 } # historical: 12\n";
		output << "\t\t\t}\n";
	}

	output << "\t\t}\n";

	return output;
}


HoI4::Navy::Navy(const std::string& _name, int _location, int _base):
	name(_name),
	location(_location),
	base(_base)
{}


std::ofstream& HoI4::operator << (std::ofstream& output, const HoI4::Navy& instance)
{
	output << "\tnavy = {\n";
	output << "\t\tname = \"" << instance.name << "\"\n";
	output << "\t\tbase = " << instance.base << "\n";
	output << "\t\tlocation = " << instance.location << "\n";
	for (auto& ship: instance.ships)
	{
		output << ship;
	}
	output << "\t}\n";

	return output;
}