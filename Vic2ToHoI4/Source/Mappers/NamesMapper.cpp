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



#include "NamesMapper.h"
#include "Log.h"
#include "ParadoxParser8859_15.h"
#include "../Configuration.h"



namesMapper* namesMapper::instance = nullptr;



namesMapper::namesMapper()
{
	LOG(LogLevel::Info) << "Parsing names";

	for (auto mod: Configuration::getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod cultures from " << mod;
		processNamesFile((Configuration::getV2Path() + "/mod/" + mod + "/common/cultures.txt"));
	}

	processNamesFile((Configuration::getV2Path() + "/common/cultures.txt"));
}


void namesMapper::processNamesFile(string filename)
{
	Object* obj = parser_8859_15::doParseFile(filename);
	if (obj == nullptr)
	{
		return;
	}

	vector<Object*> groupsObj = obj->getLeaves();
	for (auto groupsItr: groupsObj)
	{
		vector<Object*> culturesObj = groupsItr->getLeaves();
		for (auto culturesItr: culturesObj)
		{
			string key = culturesItr->getKey();
			if ((key == "union") || (key == "leader") || (key == "unit") || (key == "is_overseas"))
			{
				continue;
			}

			vector<Object*> firstNamesObj = culturesItr->getValue("first_names");
			vector<Object*> lastNamesObj = culturesItr->getValue("last_names");
			if ((firstNamesObj.size() > 0) && (lastNamesObj.size() > 0))
			{
				firstNamesMap.insert(make_pair(key, firstNamesObj[0]->getTokens()));
				surnamesMap.insert(make_pair(key, lastNamesObj[0]->getTokens()));
			}
			else
			{
				LOG(LogLevel::Error) << "No names for " << key;
			}	
		}
	}
}


vector<string> namesMapper::GetFirstNames(string culture) const
{
	vector<string> firstNames;

	auto namesItr = firstNamesMap.find(culture);
	if (namesItr != firstNamesMap.end())
	{
		firstNames = namesItr->second;
	}
	else
	{
		firstNames.push_back("null");
	}

	return firstNames;
}


vector<string> namesMapper::GetSurnames(string culture) const
{
	vector<string> surnames;

	auto namesItr = surnamesMap.find(culture);
	if (namesItr != surnamesMap.end())
	{
		surnames = namesItr->second;
	}
	else
	{
		surnames.push_back("null");
	}

	return surnames;
}


string namesMapper::GetFirstName(string culture)
{
	vector<string> firstNames = GetFirstNames(culture);

	std::uniform_int_distribution<int> firstNameGen(0, firstNames.size() - 1);
	return firstNames[firstNameGen(rng)];
}


string namesMapper::GetSurname(string culture)
{
	vector<string> surnames = GetSurnames(culture);

	std::uniform_int_distribution<int> surnameGen(0, surnames.size() - 1);
	return surnames[surnameGen(rng)];
}