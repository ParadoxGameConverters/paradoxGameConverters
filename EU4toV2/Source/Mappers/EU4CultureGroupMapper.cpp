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



#include "EU4CultureGroupMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"
#include "OSCompatibilityLayer.h"
#include "../Configuration.h"
#include <set>
using namespace std;



EU4CultureGroupMapper* EU4CultureGroupMapper::instance = nullptr;



EU4CultureGroupMapper::EU4CultureGroupMapper()
{
	addCulturesFromFile(Configuration::getEU4Path() + "/common/cultures/00_cultures.txt");

	for (auto itr: Configuration::getEU4Mods())
	{
		set<string> cultureFiles;
		Utils::GetAllFilesInFolder(itr + "/common/cultures/", cultureFiles);
		for (auto cultureFile: cultureFiles)
		{
			addCulturesFromFile(itr + "/common/cultures/" + cultureFile);
		}
	}
}


void EU4CultureGroupMapper::addCulturesFromFile(const string& filename)
{
	shared_ptr<Object> culturesObj = parser_UTF8::doParseFile(filename);
	if (culturesObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << filename;
		exit(-1);
	}
	initCultureGroups(culturesObj);
}


void EU4CultureGroupMapper::initCultureGroups(shared_ptr<Object> obj)
{
	for (auto cultureGroup: obj->getLeaves())
	{
		string group = cultureGroup->getKey();
		vector<string> cultures;
		for (auto item: cultureGroup->getLeaves())
		{
			if (!isIgnorableItem(item->getKey()))
			{
				cultures.push_back(item->getKey());
				cultureToGroupMap.insert(make_pair(item->getKey(), group));
			}
		}

		auto itr = groupToCulturesMap.find(group);
		if (itr != groupToCulturesMap.end())
		{
			vector<string> oldCultures = itr->second;
			for (auto oldCulture: oldCultures)
			{
				cultures.push_back(oldCulture);
			}
		}

		groupToCulturesMap[group] = cultures;
	}
}


bool EU4CultureGroupMapper::isIgnorableItem(const string& key)
{
	if ((key == "dynasty_names") || (key == "graphical_culture") || (key == "male_names") || (key == "female_names"))
	{
		return true;
	}
	else
	{
		return false;
	}
}


string EU4CultureGroupMapper::GetCulturalGroup(const string& culture)
{
	auto mapping = cultureToGroupMap.find(culture);
	if (mapping != cultureToGroupMap.end())
	{
		return mapping->second;
	}
	else
	{
		return "";
	}
}


vector<string> EU4CultureGroupMapper::GetCulturesInGroup(const string& group)
{
	auto mapping = groupToCulturesMap.find(group);
	if (mapping != groupToCulturesMap.end())
	{
		return mapping->second;
	}
	else
	{
		vector<string> empty;
		return empty;
	}
}