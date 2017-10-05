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



#include "ReligionMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"




religionMapper* religionMapper::instance = nullptr;



religionMapper::religionMapper()
{
	LOG(LogLevel::Info) << "Parsing religion mappings";

	shared_ptr<Object> religionMapObj = parser_UTF8::doParseFile("religionMap.txt");
	if (religionMapObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file religionMap.txt";
		exit(-1);
	}
	if (religionMapObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse religionMap.txt";
		exit(-1);
	}

	initReligionMap(religionMapObj->getLeaves()[0]);
}


void religionMapper::initReligionMap(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> rules = obj->getLeaves();

	for (auto rule: rules)
	{
		string Vic2Religion;
		vector<string> EU4Religions;

		for (auto ruleItem: rule->getLeaves())
		{
			if (ruleItem->getKey() == "v2")
			{
				Vic2Religion = ruleItem->getLeaf();
			}
			if (ruleItem->getKey() == "eu4")
			{
				EU4Religions.push_back(ruleItem->getLeaf());
			}
		}

		for (auto EU4Religion: EU4Religions)
		{
			EU4ToVic2ReligionMap.insert(make_pair(EU4Religion, Vic2Religion));
		}
	}
}


string religionMapper::GetVic2Religion(const string& EU4Religion)
{
	auto mapping = EU4ToVic2ReligionMap.find(EU4Religion);
	if (mapping != EU4ToVic2ReligionMap.end())
	{
		return mapping->second;
	}
	else
	{
		return "";
	}
}