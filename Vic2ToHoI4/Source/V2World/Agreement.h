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



#ifndef VIC2_AGREEMENT_H_
#define VIC2_AGREEMENT_H_



#include "Date.h"
#include "newParser.h"
#include <memory>
#include <string>



namespace Vic2
{

class Agreement: commonItems::parser
{
	public:
		explicit Agreement(const std::string& agreementType, std::istream& theStream);

		std::string getType() const { return type; }
		std::string getCountry1() const { return country1; }
		std::string getCountry2() const { return country2; }
		date getDate() const { return startDate; }

	private:
		Agreement(const Agreement&) = delete;
		Agreement& operator=(const Agreement&) = delete;

		std::string type = "";
		std::string country1 = "";
		std::string country2 = "";
		date startDate;
};

}



#endif // VIC2_AGREEMENT_H_
