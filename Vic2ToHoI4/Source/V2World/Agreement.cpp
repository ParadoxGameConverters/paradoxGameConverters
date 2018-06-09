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



#include "Agreement.h"
#include "ParserHelpers.h"



Vic2::Agreement::Agreement(const std::string& agreementType, std::istream& theStream):
	type(agreementType)
{
	registerKeyword(std::regex("first"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString countryString(theStream);
		country1 = countryString.getString();
		if (country1.substr(0,1) == "\"")
		{
			country1 = country1.substr(1, country1.size()-2);
		}
	});
	registerKeyword(std::regex("second"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString countryString(theStream);
		country2 = countryString.getString();
		if (country2.substr(0,1) == "\"")
		{
			country2 = country2.substr(1, country2.size()-2);
		}
	});
	registerKeyword(std::regex("start_date"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString dateString(theStream);
		startDate = date(dateString.getString());
	});
	registerKeyword(std::regex("[A-Za-z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}