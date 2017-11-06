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



#include "GovernmentMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"
#include <vector>
using namespace std;



governmentMapper* governmentMapper::instance = nullptr;



governmentMapper::governmentMapper()
{
	LOG(LogLevel::Info) << "Parsing governments mappings";

	shared_ptr<Object> governmentMapObj = parser_UTF8::doParseFile("governmentMapping.txt");
	if (governmentMapObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file governmentMapping.txt";
		exit(-1);
	}

	initGovernmentMap(governmentMapObj->getLeaves()[0]);
}


void governmentMapper::initGovernmentMap(shared_ptr<Object> obj)
{
	for (auto rule: obj->getLeaves())
	{
		processRule(rule);
	}
}


void governmentMapper::processRule(shared_ptr<Object> rule)
{
	string dstGovernment;
	vector<string> sourceGovernments;

	for (auto item: rule->getLeaves())
	{
		if (item->getKey() == "v2")
		{
			dstGovernment = item->getLeaf();
		}
		if (item->getKey() == "eu4")
		{
			sourceGovernments.push_back(item->getLeaf());
		}
	}

	for (auto sourceGovernment: sourceGovernments)
	{
		governmentMap.insert(make_pair(sourceGovernment, dstGovernment));
	}
}



string governmentMapper::MatchGovernment(const string& sourceGovernment)
{
	auto mapping = governmentMap.find(sourceGovernment);
	if (mapping != governmentMap.end())
	{
		return mapping->second;
	}
	else
	{
		LOG(LogLevel::Warning) << "No government mapping defined for " << sourceGovernment;
		return "";
	}
}