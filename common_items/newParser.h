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



#ifndef NEW_PARSER_H
#define NEW_PARSER_H



#include <istream>
#include <functional>
#include <list>
#include <optional>
#include <regex>
#include <string>
#include <utility>
#include "Object.h"



namespace commonItems
{
	typedef std::function<void(const std::string&, std::istream&)> parsingFunction;

	class parser
	{
		public:
			parser();

			~parser() = default;
			parser(const parser&) = default;
			parser& operator=(const parser&) = default;

			void registerKeyword(std::regex keyword, parsingFunction);
			void parseStream(std::istream& theStream);
			void parseFile(const std::string& filename);

			std::optional<std::string> getNextToken(std::istream& theStream);
			std::optional<std::string> getNextTokenWithoutMatching(std::istream& theStream);

		private:
			std::list<std::pair<std::regex, parsingFunction>> registeredKeywords;
			std::string nextToken;
			int braceDepth;
	};


	void ignoreItem(const std::string& unused, std::istream& theStream);
	void ignoreObject(const std::string& unused, std::istream& theStream);
	void ignoreString(const std::string& unused, std::istream& theStream);

	std::shared_ptr<Object> convert8859Object(const std::string& top, std::istream& theStream);
	std::shared_ptr<Object> convertUTF8Object(const std::string& top, std::istream& theStream);

	std::shared_ptr<Object> convert8859String(const std::string& top, std::istream& theStream);
	std::shared_ptr<Object> convertUTF8String(const std::string& top, std::istream& theStream);


	class intList: commonItems::parser
	{
		public:
			intList(std::istream& theStream);

			std::vector<int> getInts() const { return ints; }

		private:
			std::vector<int> ints;
	};


	class singleInt: commonItems::parser
	{
		public:
			singleInt(std::istream& theStream);

		int getInt() const { return theInt; }

		private:
			int theInt;
	};


	class singleDouble: commonItems::parser
	{
		public:
			singleDouble(std::istream& theStream);

			double getDouble() const { return theDouble; }

		private:
			double theDouble;
	};


	class stringList: commonItems::parser
	{
		public:
			stringList(std::istream& theStream);

			std::vector<std::string> getStrings() const { return strings; }

		private:
			std::vector<std::string> strings;
	};


	class singleString: commonItems::parser
	{
		public:
			singleString(std::istream& theStream);

			std::string getString() const { return theString; }

		private:
			std::string theString;
	};
}



#endif // NEW_PARSER_H