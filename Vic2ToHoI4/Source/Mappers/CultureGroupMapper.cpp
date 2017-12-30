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



#include "CultureGroupMapper.h"
#include "../Configuration.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParser8859_15.h"



cultureGroupMapper* cultureGroupMapper::instance = nullptr;


cultureGroupMapper::cultureGroupMapper():
	mappings()
{
	LOG(LogLevel::Info) << "Determining culture groups";

	for (auto mod: Configuration::getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod cultures from " << mod;
		processVic2CulturesFile((Configuration::getV2Path() + "/mod/" + mod + "/common/cultures.txt"));
	}

	processVic2CulturesFile((Configuration::getV2Path() + "/common/cultures.txt"));
}


void cultureGroupMapper::processVic2CulturesFile(string culturesFile)
{
	auto obj = parser_8859_15::doParseFile(culturesFile);
	if (obj)
	{
		vector<shared_ptr<Object>> groupsObj = obj->getLeaves();
		for (auto groupsItr: groupsObj)
		{
			string group = groupsItr->getKey();

			vector<shared_ptr<Object>> culturesObj = groupsItr->getLeaves();
			for (auto culturesItr: culturesObj)
			{
				string key = culturesItr->getKey();
				if ((key != "union") && (key != "leader") && (key != "unit") && (key != "is_overseas") && (mappings.find(key) == mappings.end()))
				{
					mappings.insert(make_pair(key, group));
				}
			}
		}
	}
	else
	{
		return;
	}
}


optional<string> cultureGroupMapper::GetCultureGroup(const string& culture) const
{
	auto mapping = mappings.find(culture);
	if (mapping != mappings.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}