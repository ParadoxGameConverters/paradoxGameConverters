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
#include "Division.h"
#include "../Configuration.h"
#include "../Mappers/ProvinceMapper.h"
#include "../V2World/Army.h"
#include "Log.h"



void HoI4::Army::convertArmies(const militaryMappings& theMilitaryMappings, int backupLocation)
{
	std::map<std::string, double> remainingBattalionsAndCompanies;

	for (auto army: sourceArmies)
	{
		auto provinceMapping = theProvinceMapper.getVic2ToHoI4ProvinceMapping(army->getLocation());
		if (!provinceMapping)
		{
			continue;
		}

		std::map<std::string, double> localBattalionsAndCompanies;
		for (auto regiment : army->getRegiments())
		{
			std::string type = regiment->getType();

			if (theMilitaryMappings.getUnitMap().count(type) > 0)
			{
				HoI4::UnitMap unitInfo = theMilitaryMappings.getUnitMap().at(type);

				if (unitInfo.getCategory() == "land")
				{
					// Calculate how many Battalions and Companies are available after mapping Vic2 armies
					localBattalionsAndCompanies[unitInfo.getType()] += (unitInfo.getSize() * theConfiguration.getForceMultiplier());
				}
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown unit type: " << type;
			}
		}

		convertArmyDivisions(theMilitaryMappings, localBattalionsAndCompanies, *provinceMapping->begin());
		for (auto unit: localBattalionsAndCompanies)
		{
			auto remainingUnit = remainingBattalionsAndCompanies.find(unit.first);
			if (remainingUnit != remainingBattalionsAndCompanies.end())
			{
				remainingUnit->second += unit.second;
			}
			else
			{
				remainingBattalionsAndCompanies.insert(unit);
			}
		}
	}

	convertArmyDivisions(theMilitaryMappings, remainingBattalionsAndCompanies, backupLocation);
}


void HoI4::Army::convertArmyDivisions(const militaryMappings& theMilitaryMappings, std::map<std::string, double>& BattalionsAndCompanies, int location)
{
	for (auto divTemplate: theMilitaryMappings.getDivisionTemplates())
	{
		// For each template determine the Battalion and Company requirements.
		int divisionCounter = 1;

		std::map<std::string, int> templateRequirements;
		for (auto regiment : divTemplate.getRegiments())
		{
			templateRequirements[regiment.getType()] = templateRequirements[regiment.getType()] + 1;
		}
		for (auto regiment : divTemplate.getSupportRegiments())
		{
			templateRequirements[regiment.getType()] = templateRequirements[regiment.getType()] + 1;
		}

		// Create new divisions as long as sufficient Victoria units exist, otherwise move on to next template.
		while (sufficientUnits(BattalionsAndCompanies, theMilitaryMappings.getSubstitutes(), templateRequirements))
		{
			HoI4::DivisionType newDivision(std::to_string(divisionCounter) + ". " + divTemplate.getName(), divTemplate.getName(), location);

			for (auto& unit : templateRequirements)
			{
				for (int i = 0; i < unit.second; ++i)
				{
					if (BattalionsAndCompanies[unit.first] > 0)
					{
						BattalionsAndCompanies[unit.first]--;
					}
					else
					{
						BattalionsAndCompanies[theMilitaryMappings.getSubstitutes().at(unit.first)]--;
					}
				}
			}
			divisionCounter++;
			divisions.push_back(newDivision);
		}
	}
}


bool HoI4::Army::sufficientUnits(const std::map<std::string, double>& units, const std::map<std::string, std::string>& substitutes, const std::map<std::string, int>& requiredUnits)
{
	for (auto requiredUnit: requiredUnits)
	{
		int available = 0;
		if (units.find(requiredUnit.first) != units.end())
		{
			available += static_cast<int>(units.at(requiredUnit.first));
		}
		if (substitutes.find(requiredUnit.first) != substitutes.end())
		{
			if (units.find(substitutes.at(requiredUnit.first)) != units.end())
			{
				available += static_cast<int>(units.at(substitutes.at(requiredUnit.first)));
			}
		}
		if (available < requiredUnit.second)
		{
			return false;
		}
	}

	return true;
}


std::ostream& HoI4::operator << (std::ostream& output, const HoI4::Army& theArmy)
{
	for (auto& division: theArmy.divisions)
	{
		output << division;
	}

	return output;
}