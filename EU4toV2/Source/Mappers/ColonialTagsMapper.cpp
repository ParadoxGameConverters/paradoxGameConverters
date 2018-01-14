/*Copyright (c) 2017 The Paradox Game Converters Project

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



colonialTagMapper* colonialTagMapper::instance = nullptr;



colonialTagMapper::colonialTagMapper()
{
	LOG(LogLevel::Info) << "Parsing colony naming rules.";
	shared_ptr<Object> colonialObj = parser_UTF8::doParseFile("colonial_tags.txt");
	if (colonialObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse colonial.txt";
		exit(-1);
	}
	initColonyMap(colonialObj);
}



void colonialTagMapper::initColonyMap(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> colonialRules	= obj->getLeaves();
	for (auto ruleObj: colonialRules[0]->getLeaves())
	{
		colonyStruct rule;
		for (auto item: ruleObj->getLeaves())
		{
			if (item->getKey() == "tag")
			{
				rule.tag = item->getLeaf();
			}
			if (item->getKey() == "EU4_region")
			{
				rule.EU4Region = item->getLeaf();
			}
			if (item->getKey() == "V2_region")
			{
				rule.V2Region = item->getLeaf();
			}
			if (item->getKey() == "is_culture_group")
			{
				rule.cultureGroup = item->getLeaf();
			}
		}

		colonyMap.push_back(rule);
	}
}