/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "FlagColorMapper.h"
#include "../Configuration.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"



FlagColorMapper* FlagColorMapper::instance = NULL;


FlagColorMapper::FlagColorMapper()
{
	LOG(LogLevel::Info) << "Parsing EU4 flag colours";

	string colorFile = Configuration::getEU4Path() + "/common/custom_country_colors/00_custom_country_colors.txt";
	Object* colorsObj = parser_UTF8::doParseFile(colorFile);
	if (colorsObj == NULL)
	{
		LOG(LogLevel::Warning) << "Could not parse file " << colorFile;
	}
	else if (colorsObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Warning) << "Failed to parse 00_custom_country_colors.txt";
	}
	else
	{
		initFlagColours(colorsObj);
	}
}


void FlagColorMapper::initFlagColours(Object* obj)
{
	for (Object* color: obj->getLeaves())
	{
		if (color->getKey() != "flag_color")
		{
			continue;
		}

		vector<string> values = color->getTokens();
		if (values.size() < 3)
		{
			continue;
		}

		flagColorMapping.push_back(FlagColour(stoi(values[0]), stoi(values[1]), stoi(values[2])));
	}
}


FlagColour FlagColorMapper::GetFlagColor(int index)
{
	return flagColorMapping[index];
}