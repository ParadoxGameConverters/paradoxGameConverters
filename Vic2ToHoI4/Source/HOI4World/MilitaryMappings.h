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
#include <map>
#include <string>
#include <vector>



namespace HoI4
{


class UnitMap
{
	public: 
		UnitMap(const std::string& category, const std::string& type, const std::string& equipment, int size);
		UnitMap() = default;
		UnitMap(const UnitMap&) = default;
		UnitMap& operator=(const UnitMap&) = default;

		std::string getCategory() const { return category; }
		std::string getType() const { return type; }
		std::string getEquipment() const { return equipment; }
		int getSize() const {return size; }

	private:
		std::string category;
		std::string type;
		std::string equipment;
		int size = 0;
};


class militaryMappings
{
	public:
		militaryMappings();

		auto getUnitMap() { return unitMap; }
		auto getDivisionTemplates() { return divisionTemplates; }

	private:
		void importUnitMap();
		void importDivisionTemplates();

		std::map<std::string, HoI4::UnitMap> unitMap;
		std::vector<HoI4::DivisionTemplateType> divisionTemplates;
};

}



#endif // MILITARY_MAPPINGS