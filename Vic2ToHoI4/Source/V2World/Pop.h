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



#ifndef VIC2_POP_H_
#define VIC2_POP_H_



#include "newParser.h"
#include <map>
#include <memory>
#include <string>



namespace Vic2
{
	class Pop: commonItems::parser
	{
		public:
			explicit Pop(const std::string& typeString, std::istream& theStream);

			int getSize() const { return size; }
			std::string getType() const { return type; }
			std::string getCulture() const { return culture; }
			double getLiteracy() const { return literacy; }
			double getMilitancy() const { return militancy; }
			std::map<std::string, float> getIssues() const { return popIssues; }

			float getIssue(const std::string& issueName) const;

		private:
			Pop(const Pop&) = delete;
			Pop& operator=(const Pop&) = delete;

			int size = 0;
			std::string type = "";
			std::string culture = "no_culture";
			std::string religion = "";
			double literacy = 0.0;
			double consciousness = 0.0;
			double militancy = 0.0;
			std::map<std::string, float> popIssues;
	};
}



#endif // VIC2_POP_H_