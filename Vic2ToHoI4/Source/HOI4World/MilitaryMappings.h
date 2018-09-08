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



#ifndef MILITARY_MAPPINGS
#define MILITARY_MAPPINGS



#include "HoI4Army.h"
#include "newParser.h"
#include "ParserHelpers.h"
#include <map>
#include <string>
#include <vector>



namespace HoI4
{


class UnitMap: commonItems::parser
{
	public:
		UnitMap(std::istream& theStream);
		UnitMap() = default;
		UnitMap(const UnitMap&) = default;
		UnitMap& operator=(const UnitMap&) = default;

		std::string getCategory() const { return category; }
		std::string getType() const { return type; }
		std::string getEquipment() const { return equipment; }
		int getSize() const { return size; }

	private:
		std::string category;
		std::string type;
		std::string equipment;
		int size = 0;
};


class militaryMappings: commonItems::parser
{
	public:
		militaryMappings(const std::string& name, std::istream& theStream);

		auto getMappingsName() const { return mappingsName; }
		auto getUnitMap() const { return unitMap; }
		auto getDivisionTemplates() const { return divisionTemplates; }

	private:
		void importUnitMap(std::istream& theStream);
		void importDivisionTemplates(std::istream& theStream);

		std::string mappingsName = "";
		std::map<std::string, HoI4::UnitMap> unitMap;
		std::vector<HoI4::DivisionTemplateType> divisionTemplates;
};


class allMilitaryMappings: commonItems::parser
{
	public:
		allMilitaryMappings();

		militaryMappings getMilitaryMappings(const std::vector<std::string>& mods) const;

	private:
		std::map<std::string, militaryMappings> theMappings;
};


}



#endif // MILITARY_MAPPINGS