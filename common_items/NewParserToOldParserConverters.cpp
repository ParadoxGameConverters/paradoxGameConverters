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



#include "NewParserToOldParserConverters.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include <sstream>



namespace commonItems
{
	std::string getNextLexeme(std::istream& theStream);
}



std::shared_ptr<Object> commonItems::convert8859Object(const std::string& top, std::istream& theStream)
{
	std::stringstream objectStream;
	objectStream << top << ' ';

	int braceDepth = 0;
	while (true)
	{
		if (theStream.eof())
		{
			break;
		}

		std::string token = getNextLexeme(theStream);
		objectStream << token << ' ';

		if (token == "{")
		{
			braceDepth++;
		}
		else if (token == "}")
		{
			braceDepth--;
			if (braceDepth == 0)
			{
				break;
			}
		}
	}

	return parser_8859_15::doParseStream(objectStream);
}


std::shared_ptr<Object> commonItems::convert8859String(const std::string& top, std::istream& theStream)
{
	std::stringstream objectStream;
	objectStream << top << ' ';
	objectStream << getNextLexeme(theStream) << ' ';
	objectStream << getNextLexeme(theStream) << ' ';

	return parser_8859_15::doParseStream(objectStream);
}


std::shared_ptr<Object> commonItems::convertUTF8String(const std::string& top, std::istream& theStream)
{
	std::stringstream objectStream;
	objectStream << top << ' ';
	objectStream << getNextLexeme(theStream) << ' ';
	objectStream << getNextLexeme(theStream) << ' ';

	return parser_8859_15::doParseStream(objectStream);
}


std::shared_ptr<Object> commonItems::convertUTF8Object(const std::string& top, std::istream& theStream)
{
	std::stringstream objectStream;
	objectStream << top;

	int braceDepth = 0;
	while (true)
	{
		if (theStream.eof())
		{
			break;
		}

		std::string token = getNextLexeme(theStream);
		objectStream << token << ' ';

		if (token == "{")
		{
			braceDepth++;
		}
		else if (token == "}")
		{
			braceDepth--;
			if (braceDepth == 0)
			{
				break;
			}
		}
	}

	return parser_UTF8::doParseStream(objectStream);
}