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



#include "Countries.h"
#include "EU4Country.h"



EU4::countries::countries(istream& theStream):
	theCountries()
{
	registerKeyword(std::regex("---"), commonItems::ignoreObject);
	registerKeyword(std::regex("REB"), commonItems::ignoreObject);
	registerKeyword(std::regex("PIR"), commonItems::ignoreObject);
	registerKeyword(std::regex("NAT"), commonItems::ignoreObject);
	registerKeyword(std::regex("[A-Z]{3}"), [this](const std::string& tag, std::istream& theStream)
		{
			auto country = make_shared<EU4::Country>(tag, theStream);
			theCountries.insert(make_pair(country->getTag(), country));
		}
	);
	registerKeyword(std::regex("[A-Z][0-9]{2}"), [this](const std::string& tag, std::istream& theStream)
		{
			auto country = make_shared<EU4::Country>(tag, theStream);
			theCountries.insert(make_pair(country->getTag(), country));
		}
	);

	parseStream(theStream);
}