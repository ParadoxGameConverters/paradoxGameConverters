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



#include "V2Localisation.h"

#include <Windows.h>
#include <fstream>

#include "..\EU4World\EU4Country.h"
#include "..\Log.h"
#include "..\WinUtils.h"

const std::array<std::string, V2Localisation::numLanguages> V2Localisation::languages = 
	{ "english", "french", "german", "spanish" };

void V2Localisation::SetTag(const std::string& newTag)
{
	tag = newTag;
}

void V2Localisation::ReadFromCountry(const EU4Country& source)
{
	for (size_t i = 0; i < numLanguages; ++i)
	{
		if (!languages[i].empty())
		{
			name[i] = source.getName(languages[i]);
			adjective[i] = source.getAdjective(languages[i]);
		}
	}
}

void V2Localisation::SetPartyKey(size_t partyIndex, const std::string& partyKey)
{
	if (parties.size() <= partyIndex)
	{
		parties.resize(partyIndex + 1);
	}
	parties[partyIndex].key = partyKey;
}

void V2Localisation::SetPartyName(size_t partyIndex, const std::string& language, const std::string& name)
{
	if (parties.size() <= partyIndex)
	{
		parties.resize(partyIndex + 1);
	}
	auto languageIter = std::find(languages.begin(), languages.end(), language);
	if (languageIter != languages.end())
	{
		size_t languageIndex = std::distance(languages.begin(), languageIter);
		parties[partyIndex].name[languageIndex] = name;
	}
}

void V2Localisation::WriteToStream(std::ostream& out) const
{
	out << Convert(tag);
	for (const auto& localisedName : name)
	{
		out << ';' << Convert(localisedName);
	}
	out << "x\n";

	out << Convert(tag) << "_ADJ";
	for (const auto& localisedAdjective : adjective)
	{
		out << ';' << Convert(localisedAdjective);
	}
	out << "x\n";

	for (const auto& party : parties)
	{
		out << Convert(party.key);
		for (const auto& localisedPartyName : party.name)
		{
			out << ';' << Convert(localisedPartyName);
		}
		out << "x\n";
	}
}

std::string V2Localisation::convertCountryFileName(const std::string countryFileName) const
{
	return Convert(countryFileName);
}


std::string V2Localisation::Convert(const std::string& text)
{
	if (text.empty())
	{
		return "";
	}

	int utf16Size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), text.size(), NULL, 0);
	if (utf16Size == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to UTF-16: " << WinUtils::GetLastWindowsError();
		return "";
	}
	std::vector<wchar_t> utf16Text(utf16Size, L'\0');
	int result = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), text.size(), &utf16Text[0], utf16Size);
	if (result == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to UTF-16: " << WinUtils::GetLastWindowsError();
		return "";
	}
	int latin1Size = WideCharToMultiByte(1252, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR, &utf16Text[0], utf16Size, NULL, 0, "0", NULL);
	if (latin1Size == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to Latin-1: " << WinUtils::GetLastWindowsError();
		return "";
	}
	std::vector<char> latin1Text(latin1Size, '\0');
	result = WideCharToMultiByte(1252, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR, &utf16Text[0], utf16Size, &latin1Text[0], latin1Size, "0", NULL);
	if (result == 0)
	{
		LOG(LogLevel::Warning) << "Can't convert \"" << text << "\" to Latin-1: " << WinUtils::GetLastWindowsError();
		return "";
	}
	return std::string(latin1Text.begin(), latin1Text.end());
}

// BE: Copied from EU4Localisation for V2toHoI3
void V2Localisation::ReadFromFile(const std::string& fileName)
{
	std::ifstream in(fileName);

	std::string line;			// the line being processed

	//std::string language = "english";

	// Subsequent lines are 'KEY: "Text"'
	while (!in.eof())
	{
		string line;
		getline(in, line);

		if ((line[0] == '#') || (line.size() < 3)) // BE: The 3 here is arbitrary.
		{
			continue;
		}

		int division = line.find_first_of(';');
		string key = line.substr(0, division);

		unsigned length = division;
		for (auto language : languages)
		{
			int frontDivision = division + 1;
			division = line.find_first_of(';', frontDivision);
			localisations[key][language] = line.substr(frontDivision, division - frontDivision);
		}
	}
}

void V2Localisation::ReadFromAllFilesInFolder(const std::string& folderPath)
{
	// Get all files in the folder.
	std::vector<std::string> fileNames;
	WIN32_FIND_DATA findData;	// the file data
	HANDLE findHandle = FindFirstFile((folderPath + "\\*").c_str(), &findData);	// the find handle
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			fileNames.push_back(findData.cFileName);
		}
	} while (FindNextFile(findHandle, &findData) != 0);
	FindClose(findHandle);

	// Read all these files.
	for (const auto& fileName : fileNames)
	{
		ReadFromFile(folderPath + '\\' + fileName);
	}
}

const std::string& V2Localisation::GetText(const std::string& key, const std::string& language) const
{
	static const std::string noLocalisation = "";	// used if there's no localisation

	const auto keyFindIter = localisations.find(key);	// the localisations for this key
	if (keyFindIter == localisations.end())
	{
		return noLocalisation;
	}
	const auto& localisationsByLanguage = keyFindIter->second;	// the localisations for this language
	const auto languageFindIter = localisationsByLanguage.find(language);	// the localisation we want
	if (languageFindIter == localisationsByLanguage.end())
	{
		return noLocalisation;
	}

	return languageFindIter->second;
}

const std::map<std::string, std::string>& V2Localisation::GetTextInEachLanguage(const std::string& key) const
{
	static const std::map<std::string, std::string> noLocalisation;	// used if there's no localisation

	const auto keyFindIter = localisations.find(key);	// the localisation we want
	if (keyFindIter == localisations.end())
	{
		return noLocalisation;
	}

	return keyFindIter->second;
}

