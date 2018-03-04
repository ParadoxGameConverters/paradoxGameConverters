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



#include "V2Flags.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <random>
#include "../EU4World/EU4Country.h"
#include "V2Country.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../Mappers/CK2TitleMapper.h"
#include "../Mappers/ColonyFlagsetMapper.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/FlagColorMapper.h"
#include "../FlagUtils.h"



const vector<string> V2Flags::flagFileSuffixes = { ".tga", "_communist.tga", "_fascist.tga", "_monarchy.tga", "_republic.tga" };



void V2Flags::SetV2Tags(const map<string, V2Country*>& V2Countries)
{
	LOG(LogLevel::Debug) << "Initializing flags";
	tagMap.clear();

	static mt19937 generator(static_cast<int>(chrono::system_clock::now().time_since_epoch().count()));

	determineUseableFlags();
	getRequiredTags(V2Countries);
	mapTrivialTags();

	vector<V2Country*> colonialFail;
	
	// Get the CK2 and colonial flags.
	for (map<string, V2Country*>::const_iterator i = V2Countries.begin(); i != V2Countries.end(); i++)
	{
		V2Country* v2source = i->second;
		string religion = v2source->getReligion();

		if (i->second->getSourceCountry()
			&& requiredTags.find(i->first) != requiredTags.end())
		{
			auto ck2title = mappers::CountryMappings::getCK2Title(i->first,i->second->getLocalName(),usableFlagTags);
			if ((ck2title) && (usableFlagTags.find(*ck2title) != usableFlagTags.end()))
			{
				tagMap[i->first] = *ck2title;
				usableFlagTags.erase(*ck2title);
				requiredTags.erase(i->first);
			}
			else // try something patronymic
			{
				if (!isalpha(i->first[0]) || !isdigit(i->first[1]) || !isdigit(i->first[2]))
					continue;

				string religion = i->second->getReligion();
				std::optional<std::string> randomCK2title;

				// Yay hardcoded paths. If I get round to it, I'll point these at religion.txt instead.
				if (religion == "sunni" || religion == "shiite" || religion == "ibadi")
				{
					randomCK2title = mappers::CK2TitleMapper::getRandomIslamicFlag();
				}
				else if (religion == "mahayana" || religion == "gelugpa" || religion == "theravada" || religion == "sikh" || religion == "hindu" || religion == "jain")
				{
					randomCK2title = mappers::CK2TitleMapper::getRandomIndianFlag();
				}

				if (randomCK2title && (usableFlagTags.find(*randomCK2title) != usableFlagTags.end()))
				{
					LOG(LogLevel::Info) << "Country " << i->first << " (" << i->second->getLocalName() << ") has been given the CK2 flag " << *randomCK2title;
					tagMap[i->first] =* randomCK2title;
					usableFlagTags.erase(*randomCK2title);
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

	for (auto country : V2Countries)
	{
		V2Country* overlord = country.second->getColonyOverlord();
		if (NULL == overlord)
			continue;

		string name = country.second->getLocalName();
		name = Utils::convertUTF8To8859_15(name);
		transform(name.begin(), name.end(), name.begin(), ::tolower);

		auto colonialtitle = mappers::colonyFlagsetMapper::getFlag(name);
		if (!colonialtitle)
		{
			colonialFail.push_back(country.second);
			continue;
		}

		colonialtitle->setOverlord(overlord->getTag());
		colonialFlagMapping[country.first] = colonialtitle;
		LOG(LogLevel::Info) << "Country with tag " << country.first << " is " << colonialtitle->getName() << ", ruled by " << colonialtitle->getOverlord();

		usableFlagTags.erase(colonialtitle->getName());
		requiredTags.erase(country.first);
		mappers::colonyFlagsetMapper::removeFlag(colonialtitle->getName());
	}

	if (colonialFail.size() != 0)
	{
		vector<string> colonyFlagsKeys = mappers::colonyFlagsetMapper::getNames();

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(colonyFlagsKeys.begin(), colonyFlagsKeys.end(), g);

		for (string key : colonyFlagsKeys)
		{
			auto flag = mappers::colonyFlagsetMapper::getFlag(key);

			if (false == flag->getOverlord().empty())
			{
				continue;
			}

			for (vector<V2Country*>::iterator v2c = colonialFail.begin(); v2c != colonialFail.end(); ++v2c)
			{
				bool success = false;
				string region = (*v2c)->getColonialRegion();
				if ((region == "") || (flag->getRegion() == region))
				{
					success = true;
					colonialFlagMapping[(*v2c)->getTag()] = flag;
					V2Country* overlord = (*v2c)->getColonyOverlord();
					string overlordName = overlord->getTag();
					flag->setOverlord(overlordName);
					LOG(LogLevel::Info) << "Country with tag " << (*v2c)->getTag() << " is now " << key << ", ruled by " << overlordName;

					usableFlagTags.erase(flag->getName());
					requiredTags.erase((*v2c)->getTag());
					colonialFail.erase(v2c);
					break;
				}
			}
		}
	}

	// All the remaining tags now need one of the usable flags.
	size_t mappingsMade = 0;
	for (set<string>::const_iterator i = requiredTags.cbegin(); i != requiredTags.cend(); ++i)
	{
		const string& V2Tag = *i;
		size_t randomTagIndex = uniform_int_distribution<size_t>(0, usableFlagTags.size() - 1)(generator);
		set<string>::const_iterator randomTagIter = usableFlagTags.cbegin();
		advance(randomTagIter, randomTagIndex);
		const string& flagTag = *randomTagIter;
		tagMap[V2Tag] = flagTag;
		LOG(LogLevel::Debug) << "Country with tag " << V2Tag << " has no flag and will use the flag for " << flagTag << " instead";
		if (usableFlagTags.size() > requiredTags.size() - tagMap.size())
		{
			usableFlagTags.erase(flagTag);
		}
	}

	for (map<string, V2Country*>::const_iterator i = V2Countries.begin(); i != V2Countries.end(); i++)
	{
		auto eu4country = i->second->getSourceCountry();
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


inline bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}


void V2Flags::determineUseableFlags()
{
	set<string> availableFlags = determineAvailableFlags();

	while (!availableFlags.empty())
	{
		string flag = *availableFlags.begin();
		bool hasSuffix = false;
		for (vector<string>::const_reverse_iterator i = flagFileSuffixes.rbegin(); i != flagFileSuffixes.rend() && !hasSuffix; ++i)
		{
			auto suffix = *i;
			if (ends_with(flag, suffix))
			{
				string tag = flag.substr(0, flag.find(suffix));

				// Ensure we have flags for all suffixes of this tag.
				bool haveAllFlags = true;
				for (vector<string>::const_iterator j = flagFileSuffixes.begin(); j != flagFileSuffixes.end(); ++j)
				{
					set<string>::iterator findIter = availableFlags.find(tag + *j);
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
}


set<string> V2Flags::determineAvailableFlags()
{
	set<string> availableFlags;

	const vector<string> availableFlagFolders = { "flags", Configuration::getV2Path() + "/gfx/flags" };
	for (auto availableFlagFolder: availableFlagFolders)
	{
		Utils::GetAllFilesInFolder(availableFlagFolder, availableFlags);
	}

	return availableFlags;
}


void V2Flags::getRequiredTags(const map<string, V2Country*>& V2Countries)
{
	for (auto country: V2Countries)
	{
		requiredTags.insert(country.first);
	}
}


void V2Flags::mapTrivialTags()
{
	set<string> usableFlagTagsRemaining;
	set<string> requiredTagsRemaining;

	for (auto requiredTag: requiredTags)
	{
		auto flagTag = usableFlagTags.find(requiredTag);
		if (flagTag != usableFlagTags.end())
		{
			tagMap.insert(make_pair(requiredTag, *flagTag));
			usableFlagTags.erase(flagTag);
		}
		else
		{
			requiredTagsRemaining.insert(requiredTag);
		}
	}

	swap(requiredTags, requiredTagsRemaining);
}


void V2Flags::output() const
{
	LOG(LogLevel::Debug) << "Creating flags";
	createOutputFolders();
	copyFlags();
	createCustomFlags();
	createColonialFlags();
}


void V2Flags::createOutputFolders() const
{
	if (!Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/gfx"))
	{
		LOG(LogLevel::Error) << "Could not create Output/" << Configuration::getOutputName() << "/gfx";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/gfx/flags"))
	{
		LOG(LogLevel::Error) << "Could not create Output/" << Configuration::getOutputName() << "/gfx/flags";
		exit(-1);
	}
}


void V2Flags::copyFlags() const
{
	const vector<string> availableFlagFolders = { "flags", Configuration::getV2Path() + "/gfx/flags" };
	for (auto tagMapping: tagMap)
	{
		const string& V2Tag = tagMapping.first;
		const string& flagTag = tagMapping.second;
		for (vector<string>::const_iterator i = flagFileSuffixes.begin(); i != flagFileSuffixes.end(); ++i)
		{
			const string& suffix = *i;
			bool flagFileFound = false;
			for (vector<string>::const_iterator j = availableFlagFolders.begin(); j != availableFlagFolders.end() && !flagFileFound; ++j)
			{
				const string& folderPath = *j;
				string sourceFlagPath = folderPath + '/' + flagTag + suffix;
				flagFileFound = Utils::DoesFileExist(sourceFlagPath);
				if (flagFileFound)
				{
					string destFlagPath = "Output/" + Configuration::getOutputName() + "/gfx/flags/" + V2Tag + suffix;
					Utils::TryCopyFile(sourceFlagPath, destFlagPath);
				}
			}
		}
	}
}


void V2Flags::createCustomFlags() const
{
	string baseFlagFolder = "flags";

	for (auto cflag : customFlagMapping)
	{
		string V2Tag = cflag.first;

		string baseFlag = cflag.second.flag;
		string emblem = to_string(cflag.second.emblem);

		int colourcount = FlagColorMapper::getNumColors();

		if (get<0>(cflag.second.colours) > colourcount || get<1>(cflag.second.colours) > colourcount || get<2>(cflag.second.colours) > colourcount)
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

			const string& suffix = flagFileSuffixes[i];
			bool flagFileFound = false;
			string folderPath = baseFlagFolder;

			string sourceFlagPath = folderPath + "/CustomBases/" + baseFlag + ".tga";
			string sourceEmblemPath = folderPath + "/CustomEmblems/" + emblem + suffix;

			flagFileFound = (Utils::DoesFileExist(sourceFlagPath) && Utils::DoesFileExist(sourceEmblemPath));
			if (flagFileFound)
			{
				string destFlagPath = "Output/" + Configuration::getOutputName() + "/gfx/flags/" + V2Tag + suffix;

				CreateCustomFlag( 
					FlagColorMapper::getFlagColor(get<0>(cflag.second.colours)),
					FlagColorMapper::getFlagColor(get<1>(cflag.second.colours)),
					FlagColorMapper::getFlagColor(get<2>(cflag.second.colours)),
					sourceEmblemPath, sourceFlagPath, destFlagPath);
			}
			else
			{
				if (!Utils::DoesFileExist(sourceFlagPath))
				{
					LOG(LogLevel::Error) << "Could not find " << sourceFlagPath;
					exit(-1);
				}
				else
				{
					LOG(LogLevel::Error) << "Could not find " << sourceEmblemPath;
					exit(-1);
				}
			}
		}
	}
}


void V2Flags::createColonialFlags() const
{
	// I really shouldn't be hardcoding this...
	set<string> UniqueColonialFlags{ "alyeska", "newholland", "acadia", "kanata", "novascotia", "novahollandia", "vinland", "newspain" };

	//typedef map<string, shared_ptr<colonyFlag> > V2TagToColonyFlagMap; // tag, {base,overlordtag}
	for (auto i : colonialFlagMapping)
	{
		string V2Tag = i.first;
		string baseFlag = i.second->getName();
		transform(baseFlag.begin(), baseFlag.end(), baseFlag.begin(), ::tolower);
		baseFlag.erase(remove_if(baseFlag.begin(), baseFlag.end(), [](const char ch) { return !isalpha(ch); }), baseFlag.end());

		string overlord = i.second->getOverlord();

		for (int i = 0; i < 5; i++)
		{
			const string& suffix = flagFileSuffixes[i];
			bool flagFileFound = false;
			string folderPath = "flags";

			if ((i == 0 || i == 3) // monarchy or vanilla
				&& (UniqueColonialFlags.find(baseFlag) == UniqueColonialFlags.end()))
			{
				string sourceFlagPath = folderPath + '/' + baseFlag + suffix;

				auto overlordFlag = tagMap.find(overlord);
				if (overlordFlag == tagMap.end())
				{
					LOG(LogLevel::Error) << "No flag exists for overlord " << overlord << ". Cannot create colony flag";
					exit(-1);
				}
				string overlordFlagPath = folderPath + '/' + overlordFlag->second + ".tga";
				flagFileFound = (Utils::DoesFileExist(sourceFlagPath) && Utils::DoesFileExist(overlordFlagPath));
				if (flagFileFound)
				{
					string destFlagPath = "Output/" + Configuration::getOutputName() + "/gfx/flags/" + V2Tag + suffix;
					CreateColonialFlag(overlordFlagPath, sourceFlagPath, destFlagPath);
				}
				else
				{
					if (!Utils::DoesFileExist(sourceFlagPath))
					{
						LOG(LogLevel::Error) << "Could not find " << sourceFlagPath;
						exit(-1);
					}
					else
					{
						LOG(LogLevel::Error) << "Could not find " << overlordFlagPath;
						exit(-1);
					}
				}
			}
			else
			{
				string sourceFlagPath = folderPath + '/' + baseFlag + suffix;
				flagFileFound = Utils::DoesFileExist(sourceFlagPath);
				if (flagFileFound)
				{
					string destFlagPath = "Output/" + Configuration::getOutputName() + "/gfx/flags/" + V2Tag + suffix;
					Utils::TryCopyFile(sourceFlagPath, destFlagPath);
				}
				else
				{
					LOG(LogLevel::Error) << "Could not find " << sourceFlagPath;
				}
			}
		}
	}
}