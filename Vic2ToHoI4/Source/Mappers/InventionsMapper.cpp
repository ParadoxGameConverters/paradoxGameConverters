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



#include "InventionsMapper.h"
#include "Log.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParser8859_15.h"



inventionsMapper* inventionsMapper::instance = nullptr;



inventionsMapper::inventionsMapper()
{
	string path = getInventionPath();
	generateNums(path);
}


string inventionsMapper::getInventionPath()
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


void inventionsMapper::generateNums(string path)
{
	set<string> techFiles;
	Utils::GetAllFilesInFolder(path, techFiles);
	for (auto fileItr: techFiles)
	{
		processTechFile(path + "/" + fileItr);
	}
}


void inventionsMapper::processTechFile(string filename)
{
	auto obj = parser_8859_15::doParseFile(filename);
	if (obj)
	{
		vector<shared_ptr<Object>> techObjs = obj->getLeaves();

		for (auto techObj: techObjs)
		{
			string name = techObj->getKey();
			inventionNumsToNames.insert(make_pair(inventionNumsToNames.size() + 1, name));
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse " << filename;
		exit(-1);
	}
}


optional<string> inventionsMapper::GetInventionName(int inventionNum)
{
	auto inventionName = inventionNumsToNames.find(inventionNum);
	if (inventionName == inventionNumsToNames.end())
	{
		LOG(LogLevel::Warning) << "Invalid invention. Is this using a mod that changed inventions?";
		return {};
	}
	else
	{
		return inventionName->second;
	}
}