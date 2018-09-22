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



#include "AllMilitaryMappings.h"



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