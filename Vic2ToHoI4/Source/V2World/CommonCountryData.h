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



#ifndef COMMON_COUNTRY_DATA_H_
#define COMMON_COUNTRY_DATA_H_



#include "newParser.h"
#include "Party.h"
#include "../Color.h"
#include <map>
#include <string>
#include <vector>



namespace Vic2
{

class commonCountryData: commonItems::parser
{
	public:
		commonCountryData(const std::string& filename, const std::string& mod);

		auto getColor() { return std::move(theColor); }
		auto getUnitNames() { return std::move(unitNames); }
		auto getParties() { return std::move(parties); }

	private:
		ConverterColor::Color theColor;
		std::map<std::string, std::vector<std::string>> unitNames;
		std::vector<Party> parties;
};

}



#endif // COMMON_COUNTRY_DATA_H_