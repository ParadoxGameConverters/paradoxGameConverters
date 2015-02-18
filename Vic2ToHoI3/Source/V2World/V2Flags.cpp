/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "V2Flags.h"

#include <chrono>
#include <iostream>
#include <iterator>
#include <random>

#include <boost/algorithm/string/predicate.hpp>

#include "V2Country.h"
#include "..\Configuration.h"
#include "..\Log.h"
#include "..\WinUtils.h"

const std::vector<std::string> V2Flags::flagFileSuffixes = { ".tga", "_communist.tga", "_fascist.tga", "_monarchy.tga", "_republic.tga" };


bool V2Flags::Output() const
{
	LOG(LogLevel::Debug) << "Copying flags";

	// Create output folders.
	std::string outputGraphicsFolder = "Output\\" + Configuration::getOutputName() + "\\gfx";
	if (!WinUtils::TryCreateFolder(outputGraphicsFolder))
	{
		return false;
	}
	std::string outputFlagFolder = outputGraphicsFolder + "\\flags";
	if (!WinUtils::TryCreateFolder(outputFlagFolder))
	{
		return false;
	}

	// Copy files.
	const std::vector<std::string> availableFlagFolders = { "blankMod\\output\\gfx\\flags", Configuration::getV2Path() + "\\gfx\\flags" };
	for (V2TagToFlagTagMap::const_iterator i = tagMapping.begin(); i != tagMapping.end(); ++i)
	{
		const std::string& V2Tag = i->first;
		const std::string& flagTag = i->second;
		for (std::vector<std::string>::const_iterator i = flagFileSuffixes.begin(); i != flagFileSuffixes.end(); ++i)
		{
			const std::string& suffix = *i;
			bool flagFileFound = false;
			for (std::vector<std::string>::const_iterator j = availableFlagFolders.begin(); j != availableFlagFolders.end() && !flagFileFound; ++j)
			{
				const std::string& folderPath = *j;
				std::string sourceFlagPath = folderPath + '\\' + flagTag + suffix;
				flagFileFound = WinUtils::DoesFileExist(sourceFlagPath);
				if (flagFileFound)
				{
					std::string destFlagPath = outputFlagFolder + '\\' + V2Tag + suffix;
					WinUtils::TryCopyFile(sourceFlagPath, destFlagPath);
				}
			}
		}
	}

	return true;
}
