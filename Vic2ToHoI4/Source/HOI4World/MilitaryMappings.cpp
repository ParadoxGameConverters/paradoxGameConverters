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



#include "MilitaryMappings.h"



HoI4::UnitMap::UnitMap(std::istream& theStream)
{
	registerKeyword(std::regex("category"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString categoryString(theStream);
		category = categoryString.getString();
	});
	registerKeyword(std::regex("type"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString typeString(theStream);
		type = typeString.getString();
	});
	registerKeyword(std::regex("equipment"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString equipmentString(theStream);
		equipment = equipmentString.getString();
	});
	registerKeyword(std::regex("size"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt sizeInt(theStream);
		size = sizeInt.getInt();
	});

	parseStream(theStream);
}


namespace HoI4
{

class UnitMapping: commonItems::parser
{
	public:
		UnitMapping(std::istream& theStream);

		std::string getVic2Type() const { return Vic2Type; }
		auto getHoI4Type() const { return HoI4Type; }

	private:
		std::string Vic2Type;
		UnitMap HoI4Type;
};

}


HoI4::UnitMapping::UnitMapping(std::istream& theStream)
{
	registerKeyword(std::regex("vic"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString typeString(theStream);
		Vic2Type = typeString.getString();
	});
	registerKeyword(std::regex("hoi"), [this](const std::string& unused, std::istream& theStream)
	{
		HoI4::UnitMap theUnit(theStream);
		HoI4Type = theUnit;
	});

	parseStream(theStream);
}


HoI4::militaryMappings::militaryMappings(const std::string& name, std::istream& theStream):
	mappingsName(name)
{
	registerKeyword(std::regex("map"), [this](const std::string& unused, std::istream& theStream){
		importUnitMap(theStream);
	});
	registerKeyword(std::regex("division_templates"), [this](const std::string& unused, std::istream& theStream){
		importDivisionTemplates(theStream);
	});

	parseStream(theStream);
}


void HoI4::militaryMappings::importUnitMap(std::istream& theStream)
{
	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream&theStream)
	{
		UnitMapping newMapping(theStream);
		unitMap.insert(make_pair(newMapping.getVic2Type(), newMapping.getHoI4Type()));
	});

	parseStream(theStream);
}

void HoI4::militaryMappings::importDivisionTemplates(std::istream& theStream)
{
	registerKeyword(std::regex("division_template"), [this](const std::string& unused, std::istream& theStream)
	{
		HoI4::DivisionTemplateType newTemplate(theStream);
		divisionTemplates.push_back(newTemplate);
	});

	parseStream(theStream);
}


HoI4::allMilitaryMappings::allMilitaryMappings()
{
	registerKeyword(std::regex("[a-zA-Z0-9]+"), [this](const std::string& mod, std::istream& theStream)
	{
		militaryMappings newMappings(mod, theStream);
		theMappings.insert(make_pair(mod, newMappings));
	});

	parseFile("unit_mappings.txt");
}


HoI4::militaryMappings HoI4::allMilitaryMappings::getMilitaryMappings(const std::vector<std::string>& Vic2Mods) const
{
	for (auto mod: Vic2Mods)
	{
		if (auto mapping = theMappings.find(mod); mapping != theMappings.end())
		{
			return mapping->second;
		}
	}

	return theMappings.at("default");
}