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



#include "newParser.h"
#include "Log.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include <cctype>
#include <fstream>



namespace commonItems
{
	std::string getNextLexeme(std::istream& theStream);
}


commonItems::parser::parser():
	registeredKeywords(),
	nextToken(),
	braceDepth(0)
{
}
			

void commonItems::parser::registerKeyword(std::regex keyword, parsingFunction function)
{
	registeredKeywords.push_back(make_pair(keyword, function));
}


void commonItems::parser::parseStream(std::istream& theStream)
{
	int braceDepth = 0;

	while (true)
	{
		auto token = getNextToken(theStream);
		if (token)
		{
			if (*token == "=")
			{
				continue;
			}

			else if (*token == "{")
			{
				braceDepth++;
			}

			else if (*token == "}")
			{
				braceDepth--;
				if (braceDepth == 0)
				{
					break;
				}
			}

			else
			{
				LOG(LogLevel::Warning) << "Unknown token while parsing stream: " << *token;
			}
		}
		else
		{
			break;
		}
	}
}


void commonItems::parser::parseFile(const std::string& filename)
{
	std::ifstream theFile(filename);
	if (!theFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << filename << " for parsing.";
	}

	char firstChar = theFile.peek();
	if (firstChar == (char)0xEF)
	{
		char bitBucket[3];
		theFile.read(bitBucket, 3);
	}

	parseStream(theFile);
	theFile.close();
}


std::optional<std::string> commonItems::parser::getNextToken(std::istream& theStream)
{
	theStream >> std::noskipws;

	std::string toReturn;

	bool gotToken = false;
	while (!gotToken)
	{
		if (theStream.eof())
		{
			return {};
		}

		toReturn = getNextLexeme(theStream);

		bool matched = false;
		for (auto registration: registeredKeywords)
		{
			std::smatch match;
			if (std::regex_match(toReturn, match, registration.first))
			{
				registration.second(toReturn, theStream);
				matched = true;
				break;
			}
		}

		if (!matched)
		{
			gotToken = true;
		}
	}

	if (toReturn.size() > 0)
	{
		return toReturn;
	}
	else
	{
		return {};
	}
}


std::optional<std::string> commonItems::parser::getNextTokenWithoutMatching(std::istream& theStream)
{
	theStream >> std::noskipws;

	std::string toReturn;

	bool gotToken = false;
	while (!gotToken)
	{
		if (theStream.eof())
		{
			return {};
		}

		toReturn = getNextLexeme(theStream);
		gotToken = true;
	}

	if (toReturn.size() > 0)
	{
		return toReturn;
	}
	else
	{
		return {};
	}
}


std::string commonItems::getNextLexeme(std::istream& theStream)
{
	std::string toReturn;

	bool inString = false;
	while (true)
	{
		char inputChar;
		theStream >> inputChar;
		if (theStream.eof())
		{
			break;
		}
		else if (!inString && (inputChar == '#'))
		{
			std::string bitbucket;
			std::getline(theStream, bitbucket);
			if (toReturn.size() > 0)
			{
				break;
			}
		}
		else if (inputChar == '\n')
		{
			if (toReturn.size() > 0)
			{
				break;
			}
		}
		else if ((inputChar == '\"') && !inString && (toReturn.size() == 0))
		{
			inString = true;
			toReturn += inputChar;
		}
		else if ((inputChar == '\"') && inString)
		{
			inString = false;
			toReturn += inputChar;
			break;
		}
		else if (!inString && std::isspace(inputChar))
		{
			if (toReturn.size() > 0)
			{
				break;
			}
		}
		else if (!inString && (inputChar == '{'))
		{
			if (toReturn.size() == 0)
			{
				toReturn += inputChar;
			}
			else
			{
				theStream.putback('{');
			}
			break;
		}
		else if (!inString && (inputChar == '}'))
		{
			if (toReturn.size() == 0)
			{
				toReturn += inputChar;
			}
			else
			{
				theStream.putback('}');
			}
			break;
		}
		else if (!inString && (inputChar == '='))
		{
			if (toReturn.size() == 0)
			{
				toReturn += inputChar;
			}
			else
			{
				theStream.putback('=');
			}
			break;
		}
		else
		{
			toReturn += inputChar;
		}
	}

	return toReturn;
}


void commonItems::ignoreItem(const std::string& unused, std::istream& theStream)
{
	std::string equals = getNextLexeme(theStream);
	std::string next = getNextLexeme(theStream);
	if (next == "{")
	{
		int braceDepth = 1;
		while (true)
		{
			if (theStream.eof())
			{
				return;
			}

			std::string token = getNextLexeme(theStream);
			if (token == "{")
			{
				braceDepth++;
			}
			else if (token == "}")
			{
				braceDepth--;
				if (braceDepth == 0)
				{
					return;
				}
			}
		}
	}
}


void commonItems::ignoreObject(const std::string& unused, std::istream& theStream)
{
	int braceDepth = 0;
	while (true)
	{
		if (theStream.eof())
		{
			return;
		}

		std::string token = getNextLexeme(theStream);
		if (token == "{")
		{
			braceDepth++;
		}
		else if (token == "}")
		{
			braceDepth--;
			if (braceDepth == 0)
			{
				return;
			}
		}
	}
}


void commonItems::ignoreString(const std::string& unused, std::istream& theStream)
{
	commonItems::singleString ignore(theStream);
}


std::shared_ptr<Object> commonItems::convert8859Object(const std::string& top, std::istream& theStream)
{
	stringstream objectStream;
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
	stringstream objectStream;
	objectStream << top << ' ';
	objectStream << getNextLexeme(theStream) << ' ';
	objectStream << getNextLexeme(theStream) << ' ';

	return parser_8859_15::doParseStream(objectStream);
}


std::shared_ptr<Object> commonItems::convertUTF8String(const std::string& top, std::istream& theStream)
{
	stringstream objectStream;
	objectStream << top << ' ';
	objectStream << getNextLexeme(theStream) << ' ';
	objectStream << getNextLexeme(theStream) << ' ';

	return parser_8859_15::doParseStream(objectStream);
}


std::shared_ptr<Object> commonItems::convertUTF8Object(const std::string& top, std::istream& theStream)
{
	stringstream objectStream;
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


commonItems::intList::intList(std::istream& theStream):
	ints()
{
	registerKeyword(std::regex("\\d+"), [this](const std::string& theInt, std::istream& theStream)
		{
			ints.push_back(std::stoi(theInt));
		}
	);
	registerKeyword(std::regex("\\\"\\d+\\\""), [this](const std::string& theInt, std::istream& theStream)
		{
			ints.push_back(std::stoi(theInt));
		}
	);

	parseStream(theStream);
}


commonItems::singleInt::singleInt(std::istream& theStream):
	theInt()
{
	auto equals = getNextToken(theStream);
	auto token = *getNextToken(theStream);
	if (token.substr(0,1) == "\"")
	{
		token = token.substr(1, token.length() - 2);
	}
	try
	{
		theInt = stoi(token);
	}
	catch (std::exception& e)
	{
		LOG(LogLevel::Warning) << "Expected an int, but instead got " << token;
		theInt = 0;
	}
}


commonItems::singleDouble::singleDouble(std::istream& theStream):
	theDouble()
{
	auto equals = getNextToken(theStream);
	auto token = *getNextToken(theStream);
	if (token.substr(0,1) == "\"")
	{
		token = token.substr(1, token.length() - 2);
	}
	try
	{
		theDouble = stof(token);
	}
	catch (std::exception& e)
	{
		LOG(LogLevel::Warning) << "Expected a double, but instead got " << token;
		theDouble = 0.0;
	}
}


commonItems::stringList::stringList(std::istream& theStream):
	strings()
{
	std::string iso8859_15("[\\w\\x20\\x27\\x2C-\\x2E\\x3A\\x3F\\x60\\x8A\\x92\\x9A\\x9E\\xA0\\xA6\\xA8\\xB4\\xB8\\xBC-\\xBE\\xC0-\\xD6\\xD8-\\xF6\\xF8-\\xFF\\xED]+");
	registerKeyword(std::regex(iso8859_15), [this](const std::string& theString, std::istream& theStream)
		{
			strings.push_back(theString);
		}
	);
	registerKeyword(std::regex("\\\"" + iso8859_15 + "\\\""), [this](const std::string& theString, std::istream& theStream)
		{
			strings.push_back(theString);
		}
	);

	parseStream(theStream);
}


commonItems::singleString::singleString(std::istream& theStream):
	theString()
{
	auto equals = getNextToken(theStream);
	theString = *getNextToken(theStream);
}