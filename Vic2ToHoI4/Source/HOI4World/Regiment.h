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



#ifndef HOI4_REGIMENT_H
#define HOI4_REGIMENT_H



#include "newParser.h"
#include <istream>
#include <ostream>
#include <string>



namespace HoI4
{

class RegimentType: commonItems::parser
{
	public:
		explicit RegimentType(const std::string& _type, std::istream& theStream);
		RegimentType(const RegimentType&) = default;

		const std::string getType() const { return type; }

		friend std::ostream& operator << (std::ostream& out, const RegimentType& regiment);

	private:
		RegimentType& operator=(const RegimentType&) = delete;

		std::string	type;
		int x;
		int y;
};


std::ostream& operator << (std::ostream& out, const RegimentType& regiment);

}



#endif // HOI4_REGIMENT_H
