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
	std::map<std::string, std::vector<sizedRegiment>> remainingBattalionsAndCompanies;

	for (auto army: sourceArmies)
	{
		auto provinceMapping = theProvinceMapper.getVic2ToHoI4ProvinceMapping(army->getLocation());
		if (!provinceMapping)
		{
			continue;
		}

		std::map<std::string, std::vector<sizedRegiment>> localBattalionsAndCompanies;
		for (auto regiment: army->getRegiments())
		{
			std::string type = regiment->getType();

			if (theMilitaryMappings.getUnitMap().count(type) > 0)
			{
				HoI4::UnitMap unitInfo = theMilitaryMappings.getUnitMap().at(type);

				if (unitInfo.getCategory() == "land")
				{
					// Calculate how many Battalions and Companies are available after mapping Vic2 armies
					sizedRegiment theRegiment;
					theRegiment.unitSize = unitInfo.getSize() * theConfiguration.getForceMultiplier();
					theRegiment.regiment = regiment;
					localBattalionsAndCompanies[unitInfo.getType()].push_back(theRegiment);
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
			std::vector<sizedRegiment> remainingRegiments;
			for (auto regiment: unit.second)
			{
				if (regiment.unitSize > 0)
				{
					remainingRegiments.push_back(regiment);
				}
			}
			auto remainingUnit = remainingBattalionsAndCompanies.find(unit.first);
			if (remainingUnit != remainingBattalionsAndCompanies.end())
			{
				for (auto regiment: remainingRegiments)
				{
					remainingUnit->second.push_back(regiment);
				}
			}
			else
			{
				remainingBattalionsAndCompanies.insert(make_pair(unit.first, remainingRegiments));
			}
		}
	}

	convertArmyDivisions(theMilitaryMappings, remainingBattalionsAndCompanies, backupLocation);
}


void HoI4::Army::convertArmyDivisions(const militaryMappings& theMilitaryMappings, std::map<std::string, std::vector<sizedRegiment>>& BattalionsAndCompanies, int location)
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
			double totalExperience = 0.0;
			double totalRequirement = 0.0;
			for (auto requirement: templateRequirements)
			{
				totalRequirement += requirement.second;
				double remainingRequirement = requirement.second;
				for (auto& regiment: BattalionsAndCompanies[requirement.first])
				{
					if (regiment.unitSize > 0)
					{
						auto decreaseAmount = std::min(regiment.unitSize, remainingRequirement);
						regiment.unitSize -= decreaseAmount;
						remainingRequirement -= decreaseAmount;

						totalExperience += decreaseAmount * regiment.regiment->getExperience();
					}
				}
				if ((theMilitaryMappings.getSubstitutes().count(requirement.first)) &&
					 (BattalionsAndCompanies.count(theMilitaryMappings.getSubstitutes().at(requirement.first)))
					)
				{
					for (auto& regiment: BattalionsAndCompanies[theMilitaryMappings.getSubstitutes().at(requirement.first)])
					{
						if (regiment.unitSize > 0)
						{
							auto decreaseAmount = std::min(regiment.unitSize, remainingRequirement);
							regiment.unitSize -= decreaseAmount;
							remainingRequirement -= decreaseAmount;

							totalExperience += decreaseAmount * regiment.regiment->getExperience();
						}
					}
				}
			}

			double actualExperience = totalExperience / totalRequirement / 100.0;
			if (actualExperience > 1.0)
			{
				actualExperience = 1.0;
			}
			HoI4::DivisionType newDivision(std::to_string(divisionCounter) + ". " + divTemplate.getName(), divTemplate.getName(), location, actualExperience);
			divisionCounter++;
			divisions.push_back(newDivision);
		}
	}
}


bool HoI4::Army::sufficientUnits(const std::map<std::string, std::vector<sizedRegiment>>& units, const std::map<std::string, std::string>& substitutes, const std::map<std::string, int>& requiredUnits)
{
	for (auto requiredUnit: requiredUnits)
	{
		double available = 0;
		if (units.find(requiredUnit.first) != units.end())
		{
			for (auto unit: units.at(requiredUnit.first))
			{
				available += unit.unitSize;
			}
		}
		if (substitutes.find(requiredUnit.first) != substitutes.end())
		{
			if (units.find(substitutes.at(requiredUnit.first)) != units.end())
			{
				for (auto unit: units.at(substitutes.at(requiredUnit.first)))
				{
					available += unit.unitSize;
				}
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