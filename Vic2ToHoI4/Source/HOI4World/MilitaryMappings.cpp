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



HoI4::UnitMap::UnitMap(const std::string& _category, const std::string& _type, const std::string& _equipment, int _size):
	category(_category),
	type(_type),
	equipment(_equipment),
	size(_size)
{
}


HoI4::militaryMappings::militaryMappings()
{
	importUnitMap();
	importDivisionTemplates();
}


void HoI4::militaryMappings::importUnitMap()
{
	/* HARDCODED! TO DO : IMPLEMENT PARSING of unit_mapping.txt */

	unitMap["irregular"] = HoI4::UnitMap();

	unitMap["infantry"] = HoI4::UnitMap("land","infantry","infantry_equipment_0",3);
	unitMap["regular"] = HoI4::UnitMap("land","infantry","infantry_equipment_0",3);
	unitMap["engineer"] = HoI4::UnitMap("land", "infantry", "infantry_equipment_0", 3);
	unitMap["guard"] = HoI4::UnitMap("land", "infantry", "infantry_equipment_0", 3);
	unitMap["specops"] = HoI4::UnitMap("land", "infantry", "infantry_equipment_0", 3);

	unitMap["artillery"] = HoI4::UnitMap("land", "artillery_brigade", "artillery_equipment_1", 3);
	unitMap["horse_artillery"] = HoI4::UnitMap("land", "artillery_brigade", "artillery_equipment_1", 3);

	unitMap["cavalry"] = HoI4::UnitMap();

	unitMap["hussar"] = HoI4::UnitMap("land", "cavalry", "infantry_equipment_0", 3);
	unitMap["cuirassier"] = HoI4::UnitMap("land", "cavalry", "infantry_equipment_0", 3);
	unitMap["dragoon"] = HoI4::UnitMap("land", "cavalry", "infantry_equipment_0", 3);

	unitMap["tank"] = HoI4::UnitMap("land", "light_armor", "gw_tank_equipment", 1);

	unitMap["plane"] = HoI4::UnitMap("air", "fighter", "fighter_equipment_0", 20);
	unitMap["bomber"] = HoI4::UnitMap("air", "tac_bomber", "tac_bomber_equipment_0", 20);
	unitMap["transport_plane"] = HoI4::UnitMap("air", "transport_plane", "transport_plane_equipment_0", 20);

	unitMap["manowar"] = HoI4::UnitMap();
	unitMap["frigate"] = HoI4::UnitMap();
	unitMap["commerce_raider"] = HoI4::UnitMap("naval", "destroyer", "destroyer_1", 1);
	unitMap["ironclad"] = HoI4::UnitMap();
	unitMap["monitor"] = HoI4::UnitMap();
	unitMap["cruiser"] = HoI4::UnitMap("naval", "light_cruiser", "light_cruiser_1", 1);
	unitMap["battleship"] = HoI4::UnitMap("naval", "heavy_cruiser", "heavy_cruiser_1", 1);
	unitMap["dreadnought"] = HoI4::UnitMap("naval", "battleship", "battleship_1", 1);
	unitMap["submarine"] = HoI4::UnitMap("naval", "submarine", "submarine_1", 1);
	unitMap["carrier"] = HoI4::UnitMap("naval", "carrier", "carrier", 1);
	unitMap["clipper_transport"] = HoI4::UnitMap();
	unitMap["steam_transport"] = HoI4::UnitMap("convoy", "convoy", "convoy_1", 1);
}

void HoI4::militaryMappings::importDivisionTemplates()
{
	/* HARDCODED! TO DO : IMPLEMENT PARSING of unit_mapping.txt */

	HoI4::DivisionTemplateType armoredTemplate("Armored Division");

	armoredTemplate.addRegiment(HoI4::RegimentType("light_armor", 0, 0));
	armoredTemplate.addRegiment(HoI4::RegimentType("light_armor", 0, 1));
	armoredTemplate.addRegiment(HoI4::RegimentType("light_armor", 0, 2));

	armoredTemplate.addRegiment(HoI4::RegimentType("light_armor", 1, 0));
	armoredTemplate.addRegiment(HoI4::RegimentType("light_armor", 1, 1));
	armoredTemplate.addRegiment(HoI4::RegimentType("light_armor", 1, 2));

	armoredTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 0));
	armoredTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 1));
	armoredTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 2));

	armoredTemplate.addSupportRegiment(HoI4::RegimentType("artillery",0,0));

	divisionTemplates.push_back(armoredTemplate);

	HoI4::DivisionTemplateType mechanizedTemplate("Mechanized Division");

	mechanizedTemplate.addRegiment(HoI4::RegimentType("light_armor", 0, 0));
	mechanizedTemplate.addRegiment(HoI4::RegimentType("light_armor", 0, 1));
	mechanizedTemplate.addRegiment(HoI4::RegimentType("light_armor", 0, 2));

	mechanizedTemplate.addRegiment(HoI4::RegimentType("motorized", 1, 0));
	mechanizedTemplate.addRegiment(HoI4::RegimentType("motorized", 1, 1));
	mechanizedTemplate.addRegiment(HoI4::RegimentType("motorized", 1, 2));

	mechanizedTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 0));
	mechanizedTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 1));
	mechanizedTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 2));

	mechanizedTemplate.addSupportRegiment(HoI4::RegimentType("artillery", 0, 0));

	divisionTemplates.push_back(mechanizedTemplate);

	HoI4::DivisionTemplateType motorizedTemplate("Motorized Division");

	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 0, 0));
	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 0, 1));
	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 0, 2));

	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 1, 0));
	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 1, 1));
	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 1, 2));

	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 0));
	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 1));
	motorizedTemplate.addRegiment(HoI4::RegimentType("motorized", 2, 2));

	motorizedTemplate.addSupportRegiment(HoI4::RegimentType("artillery", 0, 0));

	divisionTemplates.push_back(motorizedTemplate);

	HoI4::DivisionTemplateType assaultDivTemplate("Assault Division");

	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 0));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 1));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 2));

	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 0));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 1));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 2));

	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 0));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 1));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 2));

	assaultDivTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 3, 0));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 3, 1));
	assaultDivTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 3, 2));

	assaultDivTemplate.addRegiment(HoI4::RegimentType("light_armor", 4, 0));

	divisionTemplates.push_back(assaultDivTemplate);

	HoI4::DivisionTemplateType assaultBrigadeTemplate("Assault Brigade");

	assaultBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 0));
	assaultBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 1));
	assaultBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 2));

	assaultBrigadeTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 1, 0));

	assaultBrigadeTemplate.addRegiment(HoI4::RegimentType("light_armor", 2, 0));

	divisionTemplates.push_back(assaultBrigadeTemplate);

	HoI4::DivisionTemplateType infantryDivisionTemplate("Infantry Division");

	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 0));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 1));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 2));

	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 0));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 1));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 2));

	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 0));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 1));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 2));

	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 3, 0));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 3, 1));
	infantryDivisionTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 3, 2));

	divisionTemplates.push_back(infantryDivisionTemplate);

	HoI4::DivisionTemplateType infantryBrigadeTemplate("Infantry Brigade");

	infantryBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 0));
	infantryBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 1));
	infantryBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 2));

	infantryBrigadeTemplate.addRegiment(HoI4::RegimentType("artillery_brigade", 1, 0));

	divisionTemplates.push_back(infantryBrigadeTemplate);

	HoI4::DivisionTemplateType lightInfantryDivisionTemplate("Light Infantry Division");

	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 0));
	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 1));
	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 2));

	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 0));
	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 1));
	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 1, 2));

	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 0));
	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 1));
	lightInfantryDivisionTemplate.addRegiment(HoI4::RegimentType("infantry", 2, 2));

	divisionTemplates.push_back(lightInfantryDivisionTemplate);

	HoI4::DivisionTemplateType lightInfantryBrigadeTemplate("Light Infantry Brigade");

	lightInfantryBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 0));
	lightInfantryBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 1));
	lightInfantryBrigadeTemplate.addRegiment(HoI4::RegimentType("infantry", 0, 2));

	divisionTemplates.push_back(lightInfantryBrigadeTemplate);

	HoI4::DivisionTemplateType cavalryDivisionTemplate("Cavalry Division");

	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 0, 0));
	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 0, 1));
	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 0, 2));

	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 1, 0));
	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 1, 1));
	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 1, 2));

	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 2, 0));
	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 2, 1));
	cavalryDivisionTemplate.addRegiment(HoI4::RegimentType("cavalry", 2, 2));

	divisionTemplates.push_back(cavalryDivisionTemplate);

	HoI4::DivisionTemplateType cavalryBrigadeTemplate("Cavalry Brigade");

	cavalryBrigadeTemplate.addRegiment(HoI4::RegimentType("cavalry", 0, 0));
	cavalryBrigadeTemplate.addRegiment(HoI4::RegimentType("cavalry", 0, 1));
	cavalryBrigadeTemplate.addRegiment(HoI4::RegimentType("cavalry", 0, 2));

	divisionTemplates.push_back(cavalryBrigadeTemplate);
}