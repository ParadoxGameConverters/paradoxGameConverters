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



#include "EU4Religion.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "../Configuration.h"
#include <set>
using namespace std;



map<string, EU4Religion*> EU4Religion::all_religions;	// the set of all religions



void EU4Religion::createSelf()
{
	LOG(LogLevel::Info) << "Parsing EU4 religions";
	shared_ptr<Object> religionsObj = parser_UTF8::doParseFile((Configuration::getEU4Path() + "/common/religions/00_religion.txt").c_str());
	if (religionsObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << Configuration::getEU4Path() << "/common/religions/00_religion.txt";
		exit(-1);
	}
	if (religionsObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse 00_religion.txt";
		exit(-1);
	}
	EU4Religion::parseReligions(religionsObj);
	for (auto itr: Configuration::getEU4Mods())
	{
		set<string> filenames;
		Utils::GetAllFilesInFolder(itr + "/common/religions/", filenames);
		for (auto filename: filenames)
		{
			string modReligionFile(itr + "/common/religions/" + filename);	// the path and name of the religions file in this mod
			if (Utils::DoesFileExist(modReligionFile))
			{
				religionsObj = parser_UTF8::doParseFile(modReligionFile.c_str());
				if (religionsObj == NULL)
				{
					LOG(LogLevel::Error) << "Could not parse file " << modReligionFile;
					exit(-1);
				}
				EU4Religion::parseReligions(religionsObj);
			}
		}
	}
}


void EU4Religion::parseReligions(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> groups = obj->getLeaves();	// the objects holding the religion groups
	for (vector<shared_ptr<Object>>::iterator groupsItr = groups.begin(); groupsItr < groups.end(); groupsItr++)
	{
		vector<shared_ptr<Object>> religions = (*groupsItr)->getLeaves();	// the objects holding the religions
		string group = (*groupsItr)->getKey();							// the name of the religion group
		for (vector<shared_ptr<Object>>::iterator religionsItr = religions.begin(); religionsItr < religions.end(); religionsItr++)
		{
			string key = (*religionsItr)->getKey();
			vector<shared_ptr<Object>> colorObj = (*religionsItr)->getValue("color");
			if (colorObj.size() == 0)
			{
				continue;
			}

			EU4Religion* religion = new EU4Religion((*religionsItr), group);	// the religion
			map<string, EU4Religion*>::iterator itr = all_religions.find(group);
			if (itr != all_religions.end())
			{
				delete itr->second;
			}			
			all_religions[religion->name] = religion;
		}
	}
}


EU4Religion::EU4Religion(shared_ptr<Object> obj, string _group)
{
	group = _group;
	name = obj->getKey();
}


// e.g. catholic <-> catholic
bool EU4Religion::isSameReligion(const EU4Religion* other) const
{
	return (name == other->name) ? true : false;
}


// e.g. orthodox <-> catholic
bool EU4Religion::isRelatedTo(const EU4Religion* other) const
{
	// not the same religion
	if (isSameReligion(other))
	{
		return false;
	}

	// but in the same group
	if (group == other->group)
	{
		return true;
	}

	return false;
}


// e.g. sunni <-> catholic
bool EU4Religion::isInfidelTo(const EU4Religion* other) const
{
	// different religious groups == infidel
	return (group != other->group) ? true : false;
}


EU4Religion* EU4Religion::getReligion(string name)
{
	map<string, EU4Religion*>::iterator itr = all_religions.find(name);
	return (itr == all_religions.end()) ? NULL : itr->second;
}
