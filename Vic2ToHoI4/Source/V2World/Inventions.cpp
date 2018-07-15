/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "Inventions.h"
#include "Log.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"



Vic2::inventions::inventions() noexcept
{
	std::string path = getInventionPath();
	generateNums(path);
}


std::string Vic2::inventions::getInventionPath() const
{
	for (auto mod: theConfiguration.getVic2Mods())
	{
		std::string possiblePath = theConfiguration.getVic2Path() + "/mod/" + mod + "/inventions/";
		if (Utils::doesFolderExist(possiblePath))
		{
			return possiblePath;
		}
	}

	return theConfiguration.getVic2Path() + "/inventions/";
}


void Vic2::inventions::generateNums(const std::string& path)
{
	std::set<std::string> techFiles;
	Utils::GetAllFilesInFolder(path, techFiles);
	for (auto fileItr: techFiles)
	{
		processTechFile(path + "/" + fileItr);
	}
}


void Vic2::inventions::processTechFile(const std::string& filename)
{
	registerKeyword(std::regex("[a-zA-Z0-9_\\.\\è\\é\\ö\\ü\\:]+"), [this](const std::string& inventionName, std::istream& theStream){
		inventionNumsToNames.insert(make_pair(inventionNumsToNames.size() + 1, inventionName));
		commonItems::ignoreItem(inventionName, theStream);
	});

	parseFile(filename);
}


std::optional<std::string> Vic2::inventions::getInventionName(int inventionNum) const
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