/*Copyright (c) 2015 The Paradox Game Converters Project

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




#include "V2Inventions.h"
#include "../Configuration.h"
#include "../WinUtils.h"
#include "../Parsers/Object.h"
#include "../Parsers/Parser.h"



void getInventionNums(inventionNumToName& numToName)
{
	// find the relevant inventions files
	string path;
	vector<string> vic2Mods = Configuration::getVic2Mods();
	for (auto itr: vic2Mods)
	{
		string possiblePath = Configuration::getV2Path() + "\\mod\\" + itr + "\\inventions\\";
		if (WinUtils::doesFolderExist(possiblePath))
		{
			path = possiblePath;
		}
	}
	if (path == "")
	{
		path = Configuration::getV2Path() + "\\inventions\\";
	}

	//get the inventions
	numToName.clear();
	int num = 1;

	set<string> techFiles;
	WinUtils::GetAllFilesInFolder(path, techFiles);
	for (auto fileItr: techFiles)
	{
		Object* obj = doParseFile((path + "\\" + fileItr).c_str());
		vector<Object*> techObjs = obj->getLeaves();
		for (auto techItr: techObjs)
		{
			string name = techItr->getKey();
			numToName.insert(make_pair(num, name));
			num++;
		}
	}
}