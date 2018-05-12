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



#include "CommonCountryData.h"
#include "ParserHelpers.h"
#include "NewParserToOldParserConverters.h"
#include "../Color.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"



Vic2::commonCountryData::commonCountryData(const std::string& filename, const std::string& mod)
{
	registerKeyword(std::regex("color"), [this](const std::string& unused, std::istream& theStream){
		commonItems::intList colorInts(theStream);
		theColor = ConverterColor::Color(ConverterColor::red(colorInts.getInts()[0]), ConverterColor::green(colorInts.getInts()[1]), ConverterColor::blue(colorInts.getInts()[2]));
	});
	registerKeyword(std::regex("unit\\_names"), [this](const std::string& unused, std::istream& theStream){
		auto equals = getNextTokenWithoutMatching(theStream);
		auto bracket = getNextTokenWithoutMatching(theStream);
		auto token = getNextTokenWithoutMatching(theStream);
		while (token != "}")
		{
			commonItems::stringList unitNamesStrings(theStream);
			for (auto name: unitNamesStrings.getStrings())
			{
				if (name.substr(0,1) == "\"")
				{
					name = name.substr(1, name.length() - 2);
				}
				unitNames[*token].emplace_back(Utils::convert8859_15ToUTF8(name));
			}

			token = getNextTokenWithoutMatching(theStream);
		}
	});
	registerKeyword(std::regex("party"), [this](const std::string& unused, std::istream& theStream){
		auto party = Party(theStream);
		parties.emplace_back(party);
	});
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), commonItems::ignoreItem);

	bool parsedFile = false;
	if (mod != "")
	{
		std::string file = Configuration::getV2Path() + "/mod/" + mod + "/common/countries/" + filename;
		if (Utils::DoesFileExist(file))
		{
			parseFile(file);
			parsedFile = true;
		}
	}
	if (!parsedFile)
	{
		std::string file = Configuration::getV2Path() +  "/common/countries/" + filename;
		if (Utils::DoesFileExist(file))
		{
			parseFile(file);
		}
	}
}