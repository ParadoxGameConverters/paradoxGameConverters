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
#include "ParserHelpers.h"



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
	registerKeyword(std::regex("substitutes"), [this](const std::string& unused, std::istream& theStream){
		importSubstitutes(theStream);
	});

	parseStream(theStream);
}


namespace HoI4
{

class UnitMappingsImporter: commonItems::parser
{
	public:
	UnitMappingsImporter(std::istream& theStream);

		auto getUnitMap() const { return unitMap; }

	private:
		std::map<std::string, HoI4::UnitMap> unitMap;
};

}


HoI4::UnitMappingsImporter::UnitMappingsImporter(std::istream& theStream)
{
	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream&theStream)
	{
		UnitMapping newMapping(theStream);
		unitMap.insert(make_pair(newMapping.getVic2Type(), newMapping.getHoI4Type()));
	});

	parseStream(theStream);
}


void HoI4::militaryMappings::importUnitMap(std::istream& theStream)
{
	UnitMappingsImporter importer(theStream);
	unitMap = importer.getUnitMap();
}


namespace HoI4
{

class DivisionTemplatesImporter: commonItems::parser
{
	public:
		DivisionTemplatesImporter(std::istream& theStream);

		auto getDivisionTemplates() const { return divisionTemplates; }

	private:
		std::vector<HoI4::DivisionTemplateType> divisionTemplates;
};

}

HoI4::DivisionTemplatesImporter::DivisionTemplatesImporter(std::istream& theStream)
{
	registerKeyword(std::regex("division_template"), [this](const std::string& unused, std::istream& theStream)
	{
		HoI4::DivisionTemplateType newTemplate(theStream);
		divisionTemplates.push_back(newTemplate);
	});

	parseStream(theStream);
}


void HoI4::militaryMappings::importDivisionTemplates(std::istream& theStream)
{
	DivisionTemplatesImporter importer(theStream);
	divisionTemplates = importer.getDivisionTemplates();
}


namespace HoI4
{

class substitutesImporter: commonItems::parser
{
	public:
		substitutesImporter(std::istream& theStream);

		auto getSubstitutes() const { return substitutes; }

	private:
		std::map<std::string, std::string> substitutes;
};

}


HoI4::substitutesImporter::substitutesImporter(std::istream& theStream)
{
	registerKeyword(std::regex("[a-zA-Z_]+"), [this](const std::string& left, std::istream& theStream)
	{
		commonItems::singleString right(theStream);
		substitutes.insert(std::make_pair(left, right.getString()));
	});

	parseStream(theStream);
}


void HoI4::militaryMappings::importSubstitutes(std::istream& theStream)
{
	substitutesImporter importer(theStream);
	substitutes = importer.getSubstitutes();
}