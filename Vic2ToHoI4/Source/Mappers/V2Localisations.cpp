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



#include "V2Localisations.h"
#include <fstream>
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



V2Localisations* V2Localisations::instance = NULL;



V2Localisations::V2Localisations() noexcept:
	localisations(),
	localisationToKeyMap()
{
	LOG(LogLevel::Info) << "Reading localisation";

	ReadFromAllFilesInFolder(theConfiguration.getVic2Path() + "/localisation");

	for (auto mod: theConfiguration.getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod localisation";
		ReadFromAllFilesInFolder(theConfiguration.getVic2Path() + "/mod/" + mod + "/localisation");
	}
}


void V2Localisations::ReadFromAllFilesInFolder(const string& folderPath)
{
	set<string> fileNames;
	Utils::GetAllFilesInFolder(folderPath, fileNames);

	for (const auto& fileName: fileNames)
	{
		ReadFromFile(folderPath + '/' + fileName);
	}
}


void V2Localisations::ReadFromFile(const string& fileName)
{
	ifstream in(fileName);

	while (!in.eof())
	{
		string line;
		getline(in, line);
		if (line[0] != '#')
		{
			processLine(line);
		}
	}

	in.close();
}


const string languages[] = { "english", "french", "german", "polish", "spanish", "italian", "swedish", "czech", "hungarian", "dutch", "braz_por", "russian", "finnish" };
void V2Localisations::processLine(string line)
{
	int division = line.find_first_of(';');
	string key = line.substr(0, division);

	bool pause = false;
	for (auto language: languages)
	{
		pause = false;
		string result = getNextLocalisation(line, division);
		auto UTF8Result = Utils::convertWin1252ToUTF8(result);

		if (language == "english")
		{
			localisationToKeyMap[UTF8Result] = key;
		}
		if (!UTF8Result.empty())
		{
			localisations[key][language] = UTF8Result;
		}
		else if ((language != "english") || (language == "x"))
		{
			localisations[key][language] = localisations[key]["english"];
		}
	}
}


string V2Localisations::getNextLocalisation(string line, int& division)
{
	int frontDivision = division + 1;
	division = line.find_first_of(';', frontDivision);
	return line.substr(frontDivision, division - frontDivision);
}


void V2Localisations::ActuallyUpdateDomainCountry(const string& tag, const string& domainName)
{
	LanguageToLocalisationMap regionLocalisations;
	const auto domainKey = localisationToKeyMap.find(domainName);
	if (domainKey != localisationToKeyMap.end())
	{
		const auto domainLocalisations = localisations.find(domainKey->second);
		if (domainLocalisations != localisations.end())
		{
			for (auto regionInLanguage: domainLocalisations->second)
			{
				regionLocalisations.insert(regionInLanguage);
			}
		}
	}

	const auto KeyToLocalisationsMappings = localisations.find(tag);
	if (KeyToLocalisationsMappings == localisations.end())
	{
		return;
	}
	auto nameInAllLanguages = KeyToLocalisationsMappings->second;

	for (auto nameInLanguage : nameInAllLanguages)
	{
		string replacementName = domainName;
		auto replacementLocalisation = regionLocalisations.find(nameInLanguage.first);
		if (replacementLocalisation != regionLocalisations.end())
		{
			replacementName = replacementLocalisation->second;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not find regions localisation for " << domainName << " in " << nameInLanguage.first;
		}
		string updatedName = nameInLanguage.second;
		size_t regionPos = updatedName.find("$REGION$");
		if (regionPos != string::npos)
		{
			updatedName.replace(regionPos, 8, replacementName);
			KeyToLocalisationsMappings->second[nameInLanguage.first] = updatedName;
		}
	}
}


const optional<string> V2Localisations::ActuallyGetTextInLanguage(const string& key, const string& language) const
{
	const auto KeyToLocalisationsMapping = localisations.find(key);
	if (KeyToLocalisationsMapping == localisations.end())
	{
		return {};
	}

	const auto LanguageToLocalisationMapping = KeyToLocalisationsMapping->second.find(language);
	if (LanguageToLocalisationMapping == KeyToLocalisationsMapping->second.end())
	{
		return {};
	}

	return LanguageToLocalisationMapping->second;
}


const map<string, string> V2Localisations::ActuallyGetTextInEachLanguage(const string& key) const
{
	static const map<string, string> noLocalisations;

	const auto KeyToLocalisationsMappings = localisations.find(key);
	if (KeyToLocalisationsMappings == localisations.end())
	{
		return noLocalisations;
	}
	else
	{
		return KeyToLocalisationsMappings->second;
	}
}
