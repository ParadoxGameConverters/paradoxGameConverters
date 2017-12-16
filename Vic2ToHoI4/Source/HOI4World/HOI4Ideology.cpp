/*Copyright (c) 2017 The Paradox Game Converters Project

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



#include "HOI4Ideology.h"
#include "../Color.h"
#include "Object.h"
#include <iomanip>



HoI4Ideology::HoI4Ideology(shared_ptr<Object> obj):
	ideologyName(obj->getKey()),
	types(),
	dynamicFactionNames(),
	theColor(nullptr),
	rules(),
	warImpactOnWorldTension(static_cast<float>(obj->safeGetFloat("war_impact_on_world_tension"))),
	factionImpactOnWorldTension(static_cast<float>(obj->safeGetFloat("faction_impact_on_world_tension"))),
	modifiers(),
	factionModifiers(),
	cans(),
	AI("")
{
	auto typesObj = obj->safeGetObject("types");
	if (typesObj != nullptr)
	{
		for (auto typeObj: typesObj->getLeaves())
		{
			types.push_back(typeObj->getKey());
		}
	}

	for (auto dynamicFactionNameObj: obj->safeGetTokens("dynamic_faction_names"))
	{
		dynamicFactionNames.push_back(dynamicFactionNameObj);
	}

	auto colorsObj = obj->safeGetObject("color");
	if (colorsObj != nullptr)
	{
		theColor = new ConverterColor::Color(colorsObj);
	}

	auto rulesObj = obj->safeGetObject("rules");
	if (rulesObj != nullptr)
	{
		for (auto ruleObj: rulesObj->getLeaves())
		{
			auto rule = ruleObj->getKey();
			auto onOff = ruleObj->getLeaf();
			rules.insert(make_pair(rule, onOff));
		}
	}

	auto modifiersObj = obj->safeGetObject("modifiers");
	if (modifiersObj != nullptr)
	{
		for (auto modifierObj: modifiersObj->getLeaves())
		{
			auto modifier = modifierObj->getKey();
			auto value = stof(modifierObj->getLeaf());
			modifiers.insert(make_pair(modifier, value));
		}
	}

	auto factionModifiersObj = obj->safeGetObject("faction_modifiers");
	if (factionModifiersObj != nullptr)
	{
		for (auto factionModifierObj: factionModifiersObj->getLeaves())
		{
			auto modifier = factionModifierObj->getKey();
			auto value = stof(factionModifierObj->getLeaf());
			factionModifiers.insert(make_pair(modifier, value));
		}
	}

	for (auto object: obj->getLeaves())
	{
		if (object->getKey().substr(0, 3) == "can")
		{
			auto can = object->getKey();
			auto yesNo = object->getLeaf();
			cans.insert(make_pair(can, yesNo));
		}

		if (object->getKey().substr(0, 3) == "ai_")
		{
			AI = object->getKey();
		}
	}
}


void HoI4Ideology::output(ofstream& file) const
{
	file << "\t" << ideologyName << " = {\n";
	file << "\t\n";
	outputTypes(file);
	outputDynamicFactionNames(file);
	outputTheColor(file);
	outputRules(file);
	outputOnWorldTension(file);
	outputModifiers(file);
	outputFactionModifiers(file);
	outputCans(file);
	outputAI(file);
	file << "\t}\n";
	file << "\n\n\n";
}


void HoI4Ideology::outputTypes(ofstream& file) const
{
	file << "\t\ttypes = {\n";
	file << "\t";
	for (auto type: types)
	{
		file << "\t\t\n";
		file << "\t\t\t" << type << " = {\n";
		file << "\t\t\t}\n";
	}
	file << "\t\t}\n";
	file << "\t\t\n";
}


void HoI4Ideology::outputDynamicFactionNames(ofstream& file) const
{
	file << "\t\tdynamic_faction_names = {\n";
	for (auto dynamicFactionName: dynamicFactionNames)
	{
		file << "\t\t\t\"" << dynamicFactionName << "\"\n";
	}
	file << "\t\t}\n";
	file << "\t\t\n";
}


void HoI4Ideology::outputTheColor(ofstream& file) const
{
	file << "\t\tcolor = { " << (*theColor) << " }\n";
	file << "\t\t\n";
}


void HoI4Ideology::outputRules(ofstream& file) const
{
	file << "\t\trules = {\n";
	for (auto rule: rules)
	{
		file << "\t\t\t" << rule.first << " = " << rule.second << "\n";
	}
	file << "\t\t}\n";
	file << "\t\t\n";
}


void HoI4Ideology::outputOnWorldTension(ofstream& file) const
{
	file << "\t\twar_impact_on_world_tension = " << warImpactOnWorldTension << "\n";
	file << "\t\tfaction_impact_on_world_tension = " << factionImpactOnWorldTension << "\n";
	file << "\t\t\n";
}


void HoI4Ideology::outputModifiers(ofstream& file) const
{
	file << "\t\tmodifiers = {\n";
	for (auto modifier: modifiers)
	{
		file << "\t\t\t" << modifier.first << " = " << modifier.second << "\n";
	}
	file << "\t\t}\n";
	file << "\t\t\n";
}


void HoI4Ideology::outputFactionModifiers(ofstream& file) const
{
	file << "\t\tfaction_modifiers = {\n";
	for (auto factionModifier: factionModifiers)
	{
		file << "\t\t\t" << factionModifier.first << " = " << factionModifier.second << "\n";
	}
	file << "\t\t}\n";
}


void HoI4Ideology::outputCans(ofstream& file) const
{
	if (cans.size() > 0)
	{
		file << "\n";
	}
	for (auto can: cans)
	{
		file << "\t\t" << can.first << " = " << can.second << "\n";
	}
	if (cans.size() > 0)
	{
		file << "\n";
	}
}


void HoI4Ideology::outputAI(ofstream& file) const
{
	file << "\t\t" << AI << " = yes\n";
}