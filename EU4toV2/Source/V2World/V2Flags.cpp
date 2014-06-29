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

void V2Flags::SetV2Tags(const std::map<std::string, V2Country*>& V2Countries)
{
	LOG(LogLevel::Debug) << "Initializing flags";
	tagMapping.clear();

	// Generate a list of all flags that we can use.
	const std::vector<std::string> availableFlagFolders = { "blankMod\\output\\gfx\\flags", Configuration::getV2Path() + "\\gfx\\flags" };
	std::set<std::string> availableFlags;
	for (size_t i = 0; i < availableFlagFolders.size(); ++i)
	{
		WinUtils::GetAllFilesInFolder(availableFlagFolders[i], availableFlags);
	}
	std::set<std::string> usableFlagTags;
	while (!availableFlags.empty())
	{
		std::string flag = *availableFlags.begin();
		bool hasSuffix = false;
		for (std::vector<std::string>::const_reverse_iterator i = flagFileSuffixes.rbegin(); i != flagFileSuffixes.rend() && !hasSuffix; ++i)
		{
			const std::string& suffix = *i;
			hasSuffix = (flag.size() == 3 + suffix.size()) && boost::algorithm::iends_with(flag, suffix);
			if (hasSuffix)
			{
				std::string tag = flag.substr(0, 3);
				// Ensure we have flags for all suffixes of this tag.
				bool haveAllFlags = true;
				for (std::vector<std::string>::const_iterator j = flagFileSuffixes.begin(); j != flagFileSuffixes.end(); ++j)
				{
					std::set<std::string>::iterator findIter = availableFlags.find(tag + *j);
					if (findIter != availableFlags.end())
					{
						availableFlags.erase(findIter);	// no need to consider this flag again
					}
					else
					{
						haveAllFlags = false;
					}
				}
				if (haveAllFlags)
				{
					usableFlagTags.insert(tag);
				}
			}
		}
		availableFlags.erase(flag);	// in case we didn't remove it before, we don't want to consider it again
	}

	// Now get all tags that we want to have flags.
	std::set<std::string> requiredTags;
	for (std::map<std::string, V2Country*>::const_iterator i = V2Countries.begin(); i != V2Countries.end(); i++)
	{
		if (i->second->getSourceCountry())
		{
			requiredTags.insert(i->first);
		}
	}

	// The tags in common between these two sets (usableFlagTags and requiredTags) are already good - we calculate
	// the set of tags left over from each set.
	{
		std::set<std::string> usableFlagTagsRemaining;
		std::set_difference(usableFlagTags.begin(), usableFlagTags.end(), requiredTags.begin(), requiredTags.end(), std::inserter(usableFlagTagsRemaining, usableFlagTagsRemaining.end()));
		std::set<std::string> requiredTagsRemaining;
		std::set_difference(requiredTags.begin(), requiredTags.end(), usableFlagTags.begin(), usableFlagTags.end(), std::inserter(requiredTagsRemaining, requiredTagsRemaining.end()));
		std::swap(usableFlagTags, usableFlagTagsRemaining);
		std::swap(requiredTags, requiredTagsRemaining);
	}

	// All the remaining tags now need one of the usable flags.
	static std::mt19937 generator(static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count()));
	size_t mappingsMade = 0;
	for (std::set<std::string>::const_iterator i = requiredTags.cbegin(); i != requiredTags.cend(); ++i)
	{
		const std::string& V2Tag = *i;
		size_t randomTagIndex = std::uniform_int_distribution<size_t>(0, usableFlagTags.size() - 1)(generator);
		std::set<std::string>::const_iterator randomTagIter = usableFlagTags.cbegin();
		std::advance(randomTagIter, randomTagIndex);
		const std::string& flagTag = *randomTagIter;
		tagMapping[V2Tag] = flagTag;
		LOG(LogLevel::Debug) << "Country with tag " << V2Tag << " has no flag and will use the flag for " << flagTag << " instead";
		if (usableFlagTags.size() > requiredTags.size() - tagMapping.size())
		{
			usableFlagTags.erase(flagTag);
		}
	}
}

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
