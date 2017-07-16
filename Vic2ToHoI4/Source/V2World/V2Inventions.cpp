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




#include "V2Inventions.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"
#include "Object.h"
#include "ParadoxParser8859_15.h"



string getInventionPath();
inventionNumToName generateNums(string path);
inventionNumToName getInventionNums()
{
	string path = getInventionPath();
	return generateNums(path);
}


string getInventionPath()
{
	for (auto mod: Configuration::getVic2Mods())
	{
		string possiblePath = Configuration::getV2Path() + "/mod/" + mod + "/inventions/";
		if (Utils::doesFolderExist(possiblePath))
		{
			return possiblePath;
		}
	}

	return Configuration::getV2Path() + "/inventions/";
}


void processTechFile(string filename, inventionNumToName& numToName);
inventionNumToName generateNums(string path)
{
	inventionNumToName numToName;

	set<string> techFiles;
	Utils::GetAllFilesInFolder(path, techFiles);
	for (auto fileItr: techFiles)
	{
		processTechFile(path + "/" + fileItr, numToName);
	}

	return numToName;
}


void processTechFile(string filename, inventionNumToName& numToName)
{
	shared_ptr<Object> obj = parser_8859_15::doParseFile(filename);
	vector<shared_ptr<Object>> techObjs = obj->getLeaves();

	for (auto techObj: techObjs)
	{
		string name = techObj->getKey();
		numToName.insert(make_pair(numToName.size() + 1, name));
	}
}