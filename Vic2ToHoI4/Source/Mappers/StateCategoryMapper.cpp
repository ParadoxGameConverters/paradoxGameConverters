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



#include "StateCategoryMapper.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "../Configuration.h"
#include <set>
using namespace std;



stateCategoryMapper* stateCategoryMapper::instance;


stateCategoryMapper::stateCategoryMapper():
	stateCategories()
{
	readCategoriesFromDirectory(Configuration::getHoI4Path() + "/common/state_category");
}


void stateCategoryMapper::readCategoriesFromDirectory(const string& directory)
{
	set<string> categoryFiles;
	Utils::GetAllFilesInFolder(directory, categoryFiles);
	for (auto file: categoryFiles)
	{
		readCategoriesFromFile(Configuration::getHoI4Path() + "/common/state_category/" + file);
	}
}


void stateCategoryMapper::readCategoriesFromFile(const string& file)
{
	shared_ptr<Object> parsedFile = parser_UTF8::doParseFile(file);
	if (parsedFile)
	{
		vector<shared_ptr<Object>> StateCategoryObjs = parsedFile->getLeaves();
		vector<shared_ptr<Object>> categoryObjs = StateCategoryObjs[0]->getLeaves();

		for (auto categoryObj: categoryObjs)
		{
			string category = categoryObj->getKey();
			int slots = categoryObj->safeGetInt("local_building_slots");
			stateCategories.insert(make_pair(slots, category));
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse " << file;
		exit(-1);
	}
}