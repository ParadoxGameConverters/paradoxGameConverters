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



#include "DivisionTemplate.h"
#include "ParserHelpers.h"



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