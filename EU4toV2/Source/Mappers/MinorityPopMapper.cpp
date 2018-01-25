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



#include "MinorityPopMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"
#include "../V2World/V2Pop.h"



minorityPopMapper* minorityPopMapper::instance = nullptr;



minorityPopMapper::minorityPopMapper()
{
	LOG(LogLevel::Info) << "Parsing minority pops mappings";

	shared_ptr<Object> minoritiesObj = parser_UTF8::doParseFile("minorityPops.txt");
	if (minoritiesObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file minorityPops.txt";
		exit(-1);
	}
	if (minoritiesObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse minorityPops.txt";
		exit(-1);
	}

	initMinorityPopMap(minoritiesObj->getLeaves()[0]);
}


void minorityPopMapper::initMinorityPopMap(shared_ptr<Object> obj)
{
	for (auto rule: obj->getLeaves())
	{
		string minorityReligion;
		string minorityCulture;
		for (auto ruleItem: rule->getLeaves())
		{
			if (ruleItem->getKey() == "religion")
			{
				minorityReligion = ruleItem->getLeaf();
			}
			else if (ruleItem->getKey() == "culture")
			{
				minorityCulture = ruleItem->getLeaf();
			}
		}

		minorityPopMap.push_back(make_pair(minorityCulture, minorityReligion));
	}
}


bool minorityPopMapper::MatchMinorityPop(V2Pop* pop)
{
	for (auto minorityItr: minorityPopMap)
	{
		if ((pop->getCulture() == minorityItr.first) && (pop->getReligion() == minorityItr.second))
		{
			return true;
		}
		else if ((minorityItr.first == "") && (pop->getReligion() == minorityItr.second))
		{
			pop->setCulture("");
			return true;
		}
		else if ((pop->getCulture() == minorityItr.first) && (minorityItr.second == ""))
		{
			pop->setReligion("");
			return true;
		}
	}

	return false;
}