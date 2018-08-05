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


#include "HoI4Leader.h"
#include "../Configuration.h"



HoI4::General::General(const Vic2::Leader* srcLeader, const std::string& portrait):
	name(srcLeader->getName()),
	skill(static_cast<int>(srcLeader->getPrestige() * 22.5f) + 1),
	picture(portrait)
{
	if (skill > 4)
	{
		skill = 4;
	}

	/*auto possiblePersonalities = landPersonalityMap.find(srcLeader->getPersonality());
	if ((possiblePersonalities != landPersonalityMap.end()) && (possiblePersonalities->second.size() > 0))
	{
		traits.push_back(possiblePersonalities->second[rand() % possiblePersonalities->second.size()]);
	}
	auto possibleBackgrounds = landBackgroundMap.find(srcLeader->getBackground());
	if ((possibleBackgrounds != landBackgroundMap.end()) && (possibleBackgrounds->second.size() > 0))
	{
		traits.push_back(possibleBackgrounds->second[rand() % possibleBackgrounds->second.size()]);
	}*/
}


std::ofstream& HoI4::operator<< (std::ofstream& output, const HoI4::General& instance)
{
	output << "create_corps_commander = {\n";
	output << "\tname = \"" << instance.name << "\"\n";
	output << "\tpicture = \"" << instance.picture << "\"\n";
	output << "\ttraits = { ";
	for (auto trait: instance.traits)
	{
		output << trait << " ";
	}
	output << "}\n";
	output << "\tskill = " << instance.skill << "\n";
	output << "\tattack_skill = " << instance.attackSkill << "\n";
	output << "\tdefense_skill = " << instance.defenseSkill << "\n";
	output << "\tplanning_skill = " << instance.planningSkill << "\n";
	output << "\tlogistics_skill = " << instance.logisticsSkill << "\n";
	output << "}\n";

	return output;
}


HoI4::Admiral::Admiral(const Vic2::Leader* srcLeader, const std::string& portrait):
	name(srcLeader->getName()),
	skill(static_cast<int>(srcLeader->getPrestige() * 22.5f) + 1),
	picture(portrait)
{
	if (skill > 4)
	{
		skill = 4;
	}

	/*auto possiblePersonalities = seaPersonalityMap.find(srcLeader->getPersonality());
	if ((possiblePersonalities != seaPersonalityMap.end()) && (possiblePersonalities->second.size() > 0))
	{
		traits.push_back(possiblePersonalities->second[rand() % possiblePersonalities->second.size()]);
	}
	auto possibleBackgrounds = seaBackgroundMap.find(srcLeader->getBackground());
	if ((possibleBackgrounds != seaBackgroundMap.end()) && (possibleBackgrounds->second.size() > 0))
	{
		traits.push_back(possibleBackgrounds->second[rand() % possibleBackgrounds->second.size()]);
	}*/
}


std::ofstream& HoI4::operator<< (std::ofstream& output, const HoI4::Admiral& instance)
{
	output << "create_navy_leader = {\n";
	output << "\tname = \"" << instance.name << "\"\n";
	output << "\tpicture = \"" << instance.picture << "\"\n";
	output << "\ttraits = { ";
	for (auto trait: instance.traits)
	{
		output << trait << " ";
	}
	output << "}\n";
	output << "\tskill = " << instance.skill << "\n";
	output << "}\n";

	return output;
}