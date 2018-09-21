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



#include "Division.h"



HoI4::DivisionType::DivisionType(const std::string& _name, const std::string& _type, int _location):
	name(_name),
	type(_type),
	location(_location)
{
}


std::ostream& HoI4::operator << (std::ostream& out, const HoI4::DivisionType& division)
{
	out << "\tdivision = {\n";
	out << "\t\tname = \"" << division.name << "\"\n";
	out << "\t\tlocation = " << division.location << "\n";
	out << "\t\tdivision_template = \"" << division.type << "\"\n";
	out << "\t\tstart_experience_factor = 0.3\n";
	out << "\t\tstart_equipment_factor = 0.7\n";
	out << "\t}\n";

	return out;
}
