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

#include "..\EU4World\EU4Country.h"
#include "V2Country.h"
#include "..\Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../Mappers/CK2TitleMapper.h"
#include "../Mappers/FlagColorMapper.h"
#include "..\FlagUtils.h"

const std::vector<std::string> V2Flags::flagFileSuffixes = { ".tga", "_communist.tga", "_fascist.tga", "_monarchy.tga", "_republic.tga" };

void V2Flags::SetV2Tags(const std::map<std::string, V2Country*>& V2Countries, const colonyFlagset& colonyFlagset)
{
	LOG(LogLevel::Debug) << "Initializing flags";
	tagMapping.clear();
	colonyFlags = colonyFlagset;

	static std::mt19937 generator(static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count()));

	// Generate a list of all flags that we can use.
	const std::vector<std::string> availableFlagFolders = { "blankMod\\output\\gfx\\flags", Configuration::getV2Path() + "\\gfx\\flags" };
	std::set<std::string> availableFlags;
	for (size_t i = 0; i < availableFlagFolders.size(); ++i)
	{
		Utils::GetAllFilesInFolder(availableFlagFolders[i], availableFlags);
	}
	std::set<std::string> usableFlagTags;
	while (!availableFlags.empty())
	{
		std::string flag = *availableFlags.begin();
		bool hasSuffix = false;
		for (std::vector<std::string>::const_reverse_iterator i = flagFileSuffixes.rbegin(); i != flagFileSuffixes.rend() && !hasSuffix; ++i)
		{
			const std::string& suffix = *i;
			hasSuffix = (boost::algorithm::iends_with(flag, suffix));
			if (hasSuffix)
			{
				std::string tag = flag.substr(0, flag.find(suffix));

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

	std::vector<V2Country*> colonialFail;
	
	// Get the CK2 and colonial flags.
	for (std::map<std::string, V2Country*>::const_iterator i = V2Countries.begin(); i != V2Countries.end(); i++)
	{
		V2Country* v2source = i->second;
		std::string religion = v2source->getReligion();

		if (i->second->getSourceCountry()
			&& requiredTags.find(i->first) != requiredTags.end())
		{
			std::string ck2title = CountryMapping::GetCK2Title(i->first,i->second->getLocalName(),usableFlagTags);
			if ((ck2title != "") && (usableFlagTags.find(ck2title) != usableFlagTags.end()))
			{
				LOG(LogLevel::Info) << "Country " << i->first << " (" << i->second->getLocalName() << ") has the CK2 title " << ck2title;
				tagMapping[i->first] = ck2title;
				usableFlagTags.erase(ck2title);
				requiredTags.erase(i->first);
			}
			else // try something patronymic
			{
				if (!isalpha(i->first[0]) || !isdigit(i->first[1]) || !isdigit(i->first[2]))
					continue;

				std::string religion = i->second->getReligion();
				string randomCK2title = "";

				// Yay hardcoded paths. If I get round to it, I'll point these at religion.txt instead.
				if (religion == "sunni" || religion == "shiite" || religion == "ibadi")
				{
					randomCK2title = CK2TitleMapper::getRandomIslamicFlag();
				}
				else if (religion == "mahayana" || religion == "gelugpa" || religion == "theravada" || religion == "sikh" || religion == "hindu" || religion == "jain")
				{
					randomCK2title = CK2TitleMapper::getRandomIndianFlag();
				}

				if (usableFlagTags.find(randomCK2title) != usableFlagTags.end())
				{
					LOG(LogLevel::Info) << "Country " << i->first << " (" << i->second->getLocalName() << ") has been given the CK2 flag " << randomCK2title;
					tagMapping[i->first] = randomCK2title;
					usableFlagTags.erase(randomCK2title);
					requiredTags.erase(i->first);
				}
			}
		}
	}

	for (set<string>::iterator i = usableFlagTags.begin(); i != usableFlagTags.end();) 
	{
		const string& tag = (*i);
		if (tag.length() > 3 && tag[1] != '_') {
			usableFlagTags.erase(i++);
		}
		else {
			++i;
		}
	}

	// All the remaining tags now need one of the usable flags.
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

	std::set<std::string> duplicateColonyFlag;
	
	
	for (auto colonialtitle = colonyFlags.begin(); colonialtitle != colonyFlags.end();)
	{
		if (duplicateColonyFlag.find(colonialtitle->second->name) != duplicateColonyFlag.end())
		{
			LOG(LogLevel::Info) << "Duplicate " << colonialtitle->second->name;
			colonyFlags.erase(colonialtitle++);
		}
		else
		{
			duplicateColonyFlag.insert(colonialtitle->second->name);
			++colonialtitle;
		}
	}

	for (std::map<std::string, V2Country*>::const_iterator i = V2Countries.begin(); i != V2Countries.end(); i++)
	{
		V2Country* overlord = i->second->getColonyOverlord();
		if (NULL == overlord)
			continue;

		std::string name = i->second->getLocalName();
		name = V2Localisation::Convert(name);

		std::transform(name.begin(), name.end(), name.begin(), ::tolower);

		auto colonialtitle = colonyFlags.begin();
		for (; colonialtitle != colonyFlags.end(); ++colonialtitle)
		{
			if (name.find(colonialtitle->second->name) != string::npos)
			{
				break;
			}
		}

		if (colonialtitle == colonyFlags.end())
		{
			colonialFail.push_back(i->second);
			continue;
		}

		colonialtitle->second->overlord = overlord->getTag();
		colonialFlagMapping[i->first] = colonialtitle->second;
		LOG(LogLevel::Info) << "Country with tag " << i->first << " is " << colonialtitle->second->name << ", ruled by " << colonialtitle->second->overlord;

		usableFlagTags.erase(colonialtitle->second->name);
		requiredTags.erase(i->first); 
		colonyFlags.erase(colonialtitle);
	}

	if (colonialFail.size() != 0)
	{
		std::vector<string> colonyFlagsKeys;
		for (auto flag : colonyFlags)
		{
			colonyFlagsKeys.push_back(flag.first);
		}
		std::random_shuffle(colonyFlagsKeys.begin(), colonyFlagsKeys.end());

		for (string key : colonyFlagsKeys)
		{
			shared_ptr<colonyFlag> flag = colonyFlags[key];

			if (false == flag->overlord.empty())
				continue;

			if (flag->unique)
				continue;

			for (std::vector<V2Country*>::iterator v2c = colonialFail.begin(); v2c != colonialFail.end(); ++v2c)
			{
				bool success = false;
				string region = (*v2c)->getColonialRegion();
				if (flag->region == region || region == "")
				{
					success = true;
					colonialFlagMapping[(*v2c)->getTag()] = flag;
					flag->overlord = (*v2c)->getColonyOverlord()->getTag();
					LOG(LogLevel::Info) << "Country with tag " << (*v2c)->getTag() << " is now " << flag->name << ", ruled by " << flag->overlord;

					usableFlagTags.erase(flag->name);
					requiredTags.erase((*v2c)->getTag());
					colonialFail.erase(v2c);
					break;
				}
			}
		}
	}


	for (std::map<std::string, V2Country*>::const_iterator i = V2Countries.begin(); i != V2Countries.end(); i++)
	{
		EU4Country* eu4country = i->second->getSourceCountry();
		if (!eu4country)
			continue;

		string tag = i->second->getTag();
		CustomFlag flag = eu4country->getCustomFlag();
		if (eu4country->isRevolutionary())
		{
			flag.flag = "tricolor";
			flag.emblem = 0;
			flag.colours = eu4country->getRevolutionaryTricolour();
		}

		if (flag.flag != "-1")
		{
			customFlagMapping[tag] = flag;
		}

	}
}

bool V2Flags::Output() const
{
	LOG(LogLevel::Debug) << "Copying flags";

	// Create output folders.
	std::string outputGraphicsFolder = "Output\\" + Configuration::getOutputName() + "\\gfx";
	std::string outputFlagFolder = outputGraphicsFolder + "\\flags";

	//Utils::DeleteFolder(outputFlagFolder); 

	if (!Utils::TryCreateFolder(outputGraphicsFolder))
	{
		return false;
	}

	if (!Utils::TryCreateFolder(outputFlagFolder))
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
				flagFileFound = Utils::DoesFileExist(sourceFlagPath);
				if (flagFileFound)
				{
					std::string destFlagPath = outputFlagFolder + '\\' + V2Tag + suffix;
					Utils::TryCopyFile(sourceFlagPath, destFlagPath);
				}
			}
		}
	}

	std::string baseFlagFolder = "blankMod\\output\\gfx\\flags";

	for (auto cflag : customFlagMapping)
	{
		string V2Tag = cflag.first;

		string baseFlag = cflag.second.flag;
		string emblem = std::to_string(cflag.second.emblem);

		int colourcount = FlagColorMapper::getNumColors();
		
		if (std::get<0>(cflag.second.colours) > colourcount || std::get<1>(cflag.second.colours) > colourcount || std::get<2>(cflag.second.colours) > colourcount)
		{
			LOG(LogLevel::Error) << V2Tag << "'s flag has some missing colours.";
			continue;
		}
		
		for (int i = 0; i<5; i++)
		{
			if (baseFlag == "-1")
				continue;

			if (baseFlag == "tricolor" && i != 0 && i != 4)
				continue;

			const std::string& suffix = flagFileSuffixes[i];
			bool flagFileFound = false;
			std::string folderPath = baseFlagFolder;
			
			std::string sourceFlagPath = folderPath + "\\CustomBases\\" + baseFlag + ".tga";
			std::string sourceEmblemPath = folderPath + "\\CustomEmblems\\" + emblem + suffix;
			
			flagFileFound = (Utils::DoesFileExist(sourceFlagPath) && Utils::DoesFileExist(sourceEmblemPath));
			if (flagFileFound)
			{
				std::string destFlagPath = outputFlagFolder + '\\' + V2Tag + suffix;
				
				CreateCustomFlag( 
					FlagColorMapper::getFlagColor(std::get<0>(cflag.second.colours)),
					FlagColorMapper::getFlagColor(std::get<1>(cflag.second.colours)),
					FlagColorMapper::getFlagColor(std::get<2>(cflag.second.colours)),
					sourceEmblemPath, sourceFlagPath, destFlagPath);
			}
			else
			{
				if (!Utils::DoesFileExist(sourceFlagPath))
					LOG(LogLevel::Error) << "Could not find " << sourceFlagPath;
				else
					LOG(LogLevel::Error) << "Could not find " << sourceEmblemPath;
			}
			
		}
	}


	// I really shouldn't be hardcoding this...
	std::set<std::string> UniqueColonialFlags{ "alyeska", "newholland", "acadia", "kanata", "novascotia", "novahollandia", "vinland", "newspain" };

	//typedef std::map<std::string, shared_ptr<colonyFlag> > V2TagToColonyFlagMap; // tag, {base,overlordtag}
	for (auto i : colonialFlagMapping)
	{
		string V2Tag = i.first;
		string baseFlag = i.second->name;
		std::transform(baseFlag.begin(), baseFlag.end(), baseFlag.begin(), ::tolower);
		baseFlag.erase(std::remove_if(baseFlag.begin(), baseFlag.end(), [](const char ch) { return !isalpha(ch); }), baseFlag.end());

		string overlord = i.second->overlord;

		for (int i = 0; i < 5; i++)
		{
			const std::string& suffix = flagFileSuffixes[i];
			bool flagFileFound = false;
			std::string folderPath = outputFlagFolder;

			if ((i == 0 || i == 3) // monarchy or vanilla
				&& (UniqueColonialFlags.find(baseFlag) == UniqueColonialFlags.end()))
			{
				std::string sourceFlagPath = folderPath + '\\' + baseFlag + suffix;
				std::string overlordFlagPath = folderPath + '\\' + overlord + ".tga";
				flagFileFound = (Utils::DoesFileExist(sourceFlagPath) && Utils::DoesFileExist(overlordFlagPath));
				if (flagFileFound)
				{
					std::string destFlagPath = outputFlagFolder + '\\' + V2Tag + suffix;
					CreateColonialFlag(overlordFlagPath, sourceFlagPath, destFlagPath);
				}
				else
				{
					if (!Utils::DoesFileExist(sourceFlagPath))
						LOG(LogLevel::Error) << "Could not find " << sourceFlagPath;
					else
						LOG(LogLevel::Error) << "Could not find " << overlordFlagPath;
				}
			}
			else
			{
				std::string sourceFlagPath = folderPath + '\\' + baseFlag + suffix;
				flagFileFound = Utils::DoesFileExist(sourceFlagPath);
				if (flagFileFound)
				{
					std::string destFlagPath = outputFlagFolder + '\\' + V2Tag + suffix;
					Utils::TryCopyFile(sourceFlagPath, destFlagPath);
				}
				else
				{
					LOG(LogLevel::Error) << "Could not find " << sourceFlagPath;
				}
			}
		}
	}

	return true;
}
