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



#include "ColonialTagsMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"



mappers::colonialTagMapper* mappers::colonialTagMapper::instance = nullptr;



mappers::colonialTagMapper::colonialTagMapper()
{
	LOG(LogLevel::Info) << "Parsing colony naming rules.";

	commonItems::parsingFunction mappingFunction = std::bind(&mappers::colonialTagMapper::initMapping, this, std::placeholders::_1, std::placeholders::_2);
	registerKeyword(std::regex("link"), mappingFunction);

	parseFile("colonial_tags.txt");
}


void mappers::colonialTagMapper::initMapping(const std::string& unused, std::istream& theStream)
{
	colonyStruct rule;

	registerKeyword(std::regex("tag"), [this, &rule](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto temp = getNextToken(theStream);
			rule.tag = *temp;
		}
	);
	registerKeyword(std::regex("EU4_region"), [this, &rule](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto temp = getNextToken(theStream);
			rule.EU4Region = *temp;
		}
	);
	registerKeyword(std::regex("V2_region"), [this, &rule](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto temp = getNextToken(theStream);
			rule.V2Region = *temp;
		}
	);
	registerKeyword(std::regex("is_culture_group"), [this, &rule](const std::string& unused, std::istream& theStream)
		{
			auto equals = getNextToken(theStream);
			auto temp = getNextToken(theStream);
			rule.cultureGroup = *temp;
		}
	);

	parseStream(theStream);
	colonyMap.push_back(rule);
}