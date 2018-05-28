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



#include "AIPeaces.h"
#include "../Configuration.h"
#include <fstream>



HoI4::AIPeaces::AIPeaces()
{
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), [this](const std::string& peaceName, std::istream& theStream)
	{
		thePeaces.push_back(AIPeace(peaceName, theStream));
	});

	parseFile("ai_peaces.txt");
}


void HoI4::AIPeaces::updateAIPeaces(std::set<std::string> majorIdeologies)
{
}


void HoI4::AIPeaces::output()
{
	std::for_each(thePeaces.begin(), thePeaces.end(), [](auto& thePeace){
		if (thePeace.getName() == "communist_peace")
		{
			std::ofstream outFile("output/" + Configuration::getOutputName() + "/common/ai_peace/1_communist.txt");
			outFile << thePeace;
			outFile.close();
		}
		else if (thePeace.getName() == "fascist_peace")
		{
			std::ofstream outFile("output/" + Configuration::getOutputName() + "/common/ai_peace/1_fascist.txt");
			outFile << thePeace;
			outFile.close();
		}
	});
}