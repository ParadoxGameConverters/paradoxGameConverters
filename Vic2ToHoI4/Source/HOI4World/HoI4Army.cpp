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



#include "HoI4Army.h"
#include "ParserHelpers.h"



HoI4::RegimentType::RegimentType(const std::string& _type, std::istream& theStream):
	type(_type)
{
	registerKeyword(std::regex("x"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt xInt(theStream);
		x = xInt.getInt();
	});
	registerKeyword(std::regex("y"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt yInt(theStream);
		y = yInt.getInt();
	});

	parseStream(theStream);
}


std::ostream& HoI4::operator << (std::ostream& out, const HoI4::RegimentType& regiment)
{
	out << "\t\t" << regiment.type << " = { x = " << regiment.x << " y = " << regiment.y << " }\n";

	return out;
}


namespace HoI4
{

class RegimentTypeGroup: commonItems::parser
{
	public:
	RegimentTypeGroup(std::istream& theStream);

		auto getRegimentTypes() const { return regimentTypes; }

	private:
		std::vector<RegimentType> regimentTypes;
};

}


HoI4::RegimentTypeGroup::RegimentTypeGroup(std::istream& theStream)
{
	registerKeyword(std::regex("[a-zA-Z0-9_]+"), [this](const std::string& name, std::istream& theStream){
		HoI4::RegimentType regimentType(name, theStream);
		regimentTypes.push_back(regimentType);
	});

	parseStream(theStream);
}


HoI4::DivisionTemplateType::DivisionTemplateType(std::istream& theStream)
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString nameString(theStream);
		name = nameString.getString();
	});
	registerKeyword(std::regex("regiments"), [this](const std::string& unused, std::istream& theStream){
		HoI4::RegimentTypeGroup regimentsGroup(theStream);
		regiments = regimentsGroup.getRegimentTypes();
	});
	registerKeyword(std::regex("support"), [this](const std::string& unused, std::istream& theStream){
		HoI4::RegimentTypeGroup supportRegimentsGroup(theStream);
		supportRegiments = supportRegimentsGroup.getRegimentTypes();
	});
	registerKeyword(std::regex("priority"),  commonItems::ignoreItem);

	parseStream(theStream);
}


std::ostream& HoI4::operator << (std::ostream& out, const HoI4::DivisionTemplateType& rhs)
{
	out << "division_template = {\n";
	out << "\tname = \"" << rhs.name << "\"\n";
	out << "\n";
	out << "\tregiments = {\n";
	for (auto regiment: rhs.regiments)
	{
		out << regiment;
	}
	out << "\t}\n";
	out << "\tsupport = {\n";
	for (auto regiment: rhs.supportRegiments)
	{
		out << regiment;
	}
	out << "\t}\n";
	out << "}\n";

	return out;
}


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
