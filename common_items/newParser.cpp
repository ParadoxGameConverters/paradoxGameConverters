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
#include <fstream>



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
			for (auto registration: registeredKeywords)
			{
				std::smatch match;
				if (std::regex_match(*token, match, registration.first))
				{
					registration.second(theStream);
					continue;
				}
			}

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
	}
}


void commonItems::parser::parseFile(const std::string& filename)
{
	std::ifstream theFile(filename);
	if (!theFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << filename << " for parsing.";
	}

	parseStream(theFile);
	theFile.close();
}


std::optional<std::string> commonItems::parser::getNextToken(std::istream& theStream)
{
	if (theStream.eof())
	{
		return {};
	}

	std::string toReturn;
	bool gotToken = false;
	while (!gotToken)
	{
		theStream >> toReturn;
		if (toReturn.substr(0, 1) == "#")
		{
			std::string bitbucket;
			std::getline(theStream, bitbucket);
		}
		else
		{
			bool matched = false;
			for (auto registration: registeredKeywords)
			{
				std::smatch match;
				if (std::regex_match(toReturn, match, registration.first))
				{
					registration.second(theStream);
					matched = true;
					break;
				}
			}

			if (!matched)
			{
				gotToken = true;
			}
		}
	}

	return toReturn;
}


void commonItems::ignoreObject(std::istream& theStream)
{
	int braceDepth = 0;
	while (true)
	{
		if (theStream.eof())
		{
			return;
		}

		std::string token;
		theStream >> token;
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