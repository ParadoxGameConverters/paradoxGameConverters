#include "V2Flags.h"

#include <iostream>
#include <iterator>
#include <random>

#include <boost/algorithm/string/predicate.hpp>

#include <Windows.h>

#include "V2Country.h"
#include "..\Configuration.h"
#include "..\Log.h"

const std::vector<std::string> V2Flags::flagFileSuffixes = { ".tga", "_communist.tga", "_fascist.tga", "_monarchy.tga", "_republic.tga" };

void V2Flags::SetV2Tags(const std::map<std::string, V2Country*>& V2Countries)
{
	LogUpdate("Initializing flags", false);
	tagMapping.clear();

	// Generate a list of all flags that we can use.
	const std::vector<std::string> availableFlagFolders = { "blankMod\\output\\gfx\\flags", Configuration::getV2Path() + "\\gfx\\flags" };
	std::set<std::string> availableFlags;
	for (size_t i = 0; i < availableFlagFolders.size(); ++i)
	{
		GetAllFilesInFolder(availableFlagFolders[i], availableFlags);
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
	std::mt19937 generator;
	size_t mappingsMade = 0;
	for (std::set<std::string>::const_iterator i = requiredTags.cbegin(); i != requiredTags.cend(); ++i)
	{
		const std::string& V2Tag = *i;
		size_t randomTagIndex = std::uniform_int_distribution<size_t>(0, usableFlagTags.size() - 1)(generator);
		std::set<std::string>::const_iterator randomTagIter = usableFlagTags.cbegin();
		std::advance(randomTagIter, randomTagIndex);
		const std::string& flagTag = *randomTagIter;
		tagMapping[V2Tag] = flagTag;
		LogUpdate("\tCountry with tag " + V2Tag + " has no flag and will use the flag for " + flagTag + " instead", false);
		if (usableFlagTags.size() > requiredTags.size() - tagMapping.size())
		{
			usableFlagTags.erase(flagTag);
		}
	}
}

bool V2Flags::Output() const
{
	LogUpdate("Copying flags", false);

	// Create output folders.
	std::string outputGraphicsFolder = "Output\\" + Configuration::getOutputName() + "\\gfx";
	if (!TryCreateFolder(outputGraphicsFolder))
	{
		return false;
	}
	std::string outputFlagFolder = outputGraphicsFolder + "\\flags";
	if (!TryCreateFolder(outputFlagFolder))
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
				flagFileFound = DoesFileExist(sourceFlagPath);
				if (flagFileFound)
				{
					std::string destFlagPath = outputFlagFolder + '\\' + V2Tag + suffix;
					TryCopyFile(sourceFlagPath, destFlagPath);
				}
			}
		}
	}

	return true;
}

void V2Flags::LogUpdate(const std::string& text, bool includeConsole)
{
	log("%s\n", text.c_str());
	if (includeConsole)
	{
		std::cout << text << '\n';
	}
}

void V2Flags::GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames)
{
	WIN32_FIND_DATA findData;
	HANDLE findHandle = FindFirstFile((path + "\\*").c_str(), &findData);
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			fileNames.insert(findData.cFileName);
		}
	} while (FindNextFile(findHandle, &findData) != 0);
	FindClose(findHandle);
}

bool V2Flags::TryCreateFolder(const std::string& path)
{
	BOOL success = ::CreateDirectory(path.c_str(), NULL);
	if (success || GetLastError() == 183)
	{
		return true;
	}
	else
	{
		LogUpdate("Error: Could not create folder " + path + " - " + GetLastWindowsError());
		return false;
	}
}

bool V2Flags::TryCopyFile(const std::string& sourcePath, const std::string& destPath)
{
	BOOL success = ::CopyFile(sourcePath.c_str(), destPath.c_str(), FALSE);
	if (success)
	{
		return true;
	}
	else
	{
		LogUpdate("Error: Could not copy file " + sourcePath + " to " + destPath + " - " + GetLastWindowsError());
		return false;
	}
}

bool V2Flags::DoesFileExist(const std::string& path)
{
	DWORD attributes = GetFileAttributes(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::string V2Flags::GetLastWindowsError()
{
	DWORD errorCode = ::GetLastError();
	const DWORD errorBufferSize = 256;
	CHAR errorBuffer[errorBufferSize];
	BOOL success = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
											 NULL,
											 errorCode,
											 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
											 errorBuffer,
											 errorBufferSize - 1,
											 NULL);
	if (success)
	{
		return errorBuffer;
	}
	else
	{
		return "Unknown error";
	}
}
