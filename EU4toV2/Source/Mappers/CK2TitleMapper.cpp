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



#include "CK2TitleMapper.h"
#include "log.h"
#include "Object.h"
#include "paradoxParserUTF8.h"
#include "../V2World/V2Localisation.h"



CK2TitleMapper* CK2TitleMapper::instance = NULL;


CK2TitleMapper::CK2TitleMapper()
{
	LOG(LogLevel::Info) << "Getting CK2 titles";
	Object* CK2TitleObj = parser_UTF8::doParseFile("ck2titlemap.txt");
	initCK2TitleMap(CK2TitleObj);
}


void CK2TitleMapper::initCK2TitleMap(Object* obj)
{
	generator();

	vector<Object*> titles = obj->getLeaves();
	if (titles.size() == 0)
	{
		LOG(LogLevel::Error) << "Could not process ck2titlemap.txt";
		exit(-1);
	}

	for (auto link: titles[0]->getLeaves())
	{
		vector<Object*> titles = link->getLeaves();
		string name;
		string titleID;
		string region = "";

		for (auto title: titles)
		{
			if (title->getKey() == "name" )
			{
				name = title->getLeaf();
				name = V2Localisation::Convert(name);
			}
			if (title->getKey() == "title" )
			{
				titleID = title->getLeaf();
			}
			if (title->getKey() == "region")
			{
				region = title->getLeaf();
			}
		}
		if (region == "e_persia" || region == "e_arabia")
		{
			islamicFlags.push_back(titleID);
		}
		else if (region == "e_rajastan" || region == "e_bengal" || region == "e_deccan")
		{
			indianFlags.push_back(titleID);
		}

		titleMap[name] = titleID;
	}
}


string CK2TitleMapper::GetTitle(string name)
{
	auto mapping = titleMap.find(name);
	if (mapping != titleMap.end())
	{
		return mapping->second;
	}
	else
	{
		return "";
	}
}


bool CK2TitleMapper::DoesTitleExist(string title)
{
	if (titles.count(title) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


string CK2TitleMapper::GetRandomIslamicFlag()
{
	size_t randomTagIndex = uniform_int_distribution<size_t>(0, islamicFlags.size() - 1)(generator);
	return islamicFlags[randomTagIndex];
}


string CK2TitleMapper::GetRandomIndianFlag()
{
	size_t randomTagIndex = uniform_int_distribution<size_t>(0, indianFlags.size() - 1)(generator);
	return indianFlags[randomTagIndex];
}