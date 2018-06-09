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



#include "Leader.h"
#include "ParserHelpers.h"



Vic2::Leader::Leader(std::istream& theStream)
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString nameString(theStream);
		name = nameString.getString();
		if (name[0] == '\"')
		{
			name	= name.substr(1, name.length() - 2);
		}
	});
	registerKeyword(std::regex("type"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString typeString(theStream);
		type = typeString.getString();
		if (type[0] == '\"')
		{
			type	= type.substr(1, type.length() - 2);
		}
	});
	registerKeyword(std::regex("personality"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString personalityString(theStream);
		personality = personalityString.getString();
		if (personality[0] == '\"')
		{
			personality	= personality.substr(1, personality.length() - 2);
		}
	});
	registerKeyword(std::regex("background"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleString backgroundString(theStream);
		background = backgroundString.getString();
		if (background[0] == '\"')
		{
			background	= background.substr(1, background.length() - 2);
		}
	});
	registerKeyword(std::regex("prestige"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleDouble prestigeString(theStream);
		prestige = prestigeString.getDouble();
	});
	registerKeyword(std::regex("[A-Za-z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}