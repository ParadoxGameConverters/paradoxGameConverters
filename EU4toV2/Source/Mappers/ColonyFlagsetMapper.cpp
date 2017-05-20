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



#include "ColonyFlagsetMapper.h"
#include "Log.h"
#include "ParadoxParserUTF8.h"
#include "../V2World/V2Localisation.h"
#include <set>
#include <algorithm>



colonyFlagsetMapper* colonyFlagsetMapper::instance = nullptr;



colonyFlagsetMapper::colonyFlagsetMapper()
{
	LOG(LogLevel::Info) << "Parsing colony naming rules.";

	Object* colonialObj = parser_UTF8::doParseFile("colonial_flags.txt");
	if (colonialObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse colonial_flags.txt";
		exit(-1);
	}

	initColonyFlagset(colonialObj);
	removeDuplicates();
}


void colonyFlagsetMapper::initColonyFlagset(Object* obj)
{
	vector<Object*> colonialFlagRules = obj->getLeaves();
	vector<Object*> regionObjs = colonialFlagRules[0]->getLeaves();

	for (auto regionObj: regionObjs)
	{
		string region = regionObj->getKey();
		vector<Object*> flagObjs = regionObj->getLeaves();
		for (auto flagObj: flagObjs)
		{
			shared_ptr<colonyFlag> flag(new colonyFlag());
			flag->region = region;
			flag->unique = false;
			flag->overlord = "";

			for (auto item : flagObj->getLeaves())
			{
				if (item->getKey() == "name")
				{
					string name = item->getLeaf();
					name = V2Localisation::Convert(name);
					std::transform(name.begin(), name.end(), name.begin(), ::tolower);
					if (flag->name == "")
					{
						flag->name = name;
					}

					colonyFlagset[name] = flag;
				}
				if (item->getKey() == "unique")
				{
					flag->unique = true;
				}
			}
		}
	}
}


void colonyFlagsetMapper::removeDuplicates()
{
	set<string> duplicateColonyFlag;

	for (auto colonialtitle = colonyFlagset.begin(); colonialtitle != colonyFlagset.end();)
	{
		if (duplicateColonyFlag.find(colonialtitle->second->name) != duplicateColonyFlag.end())
		{
			LOG(LogLevel::Info) << "Duplicate " << colonialtitle->second->name;
			colonyFlagset.erase(colonialtitle++);
		}
		else
		{
			duplicateColonyFlag.insert(colonialtitle->second->name);
			++colonialtitle;
		}
	}
}