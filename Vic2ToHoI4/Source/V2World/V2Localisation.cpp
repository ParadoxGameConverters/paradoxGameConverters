/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include "Log.h"
#include "..\WinUtils.h"
#include "Object.h"



const std::array<std::wstring, V2Localisation::numLanguages> V2Localisation::languages = 
	{ L"english", L"french", L"german", L"spanish" };


void V2Localisation::SetTag(const std::wstring& newTag)
{
	tag = newTag;
}


void V2Localisation::SetPartyKey(size_t partyIndex, const std::wstring& partyKey)
{
	if (parties.size() <= partyIndex)
	{
		parties.resize(partyIndex + 1);
	}
	parties[partyIndex].key = partyKey;
}


void V2Localisation::SetPartyName(size_t partyIndex, const std::wstring& language, const std::wstring& name)
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


void V2Localisation::ReadFromFile(const std::wstring& fileName)
{
	std::wifstream in(fileName);

	std::wstring line;			// the line being processed

	// Subsequent lines are 'KEY: "Text"'
	while (!in.eof())
	{
		wstring line;
		getline(in, line);

		if ((line[0] == '#') || (line.size() < 3)) // BE: The 3 here is arbitrary.
		{
			continue;
		}

		int division = line.find_first_of(';');
		wstring key = line.substr(0, division);

		unsigned length = division;
		for (auto language : languages)
		{
			int frontDivision = division + 1;
			division = line.find_first_of(';', frontDivision);
			localisations[key][language] = line.substr(frontDivision, division - frontDivision);

			// dash characters other than 0x2D break HoI4
			int dash = localisations[key][language].find_first_of('–');									// the first (if any) dask in the output name
			while (dash != string::npos)
			{
				localisations[key][language].replace(dash, 1, L"-");
				dash = localisations[key][language].find_first_of('–');
			}
		}
	}
}


void V2Localisation::ReadFromAllFilesInFolder(const std::wstring& folderPath)
{
	// Get all files in the folder.
	std::vector<std::wstring> fileNames;
	WIN32_FIND_DATA findData;	// the file data
	HANDLE findHandle = FindFirstFile((folderPath + L"\\*").c_str(), &findData);	// the find handle
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
		ReadFromFile(folderPath + L'\\' + fileName);
	}
}

const std::wstring& V2Localisation::GetText(const std::wstring& key, const std::wstring& language) const
{
	static const std::wstring noLocalisation = L"";	// used if there's no localisation

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


const std::map<std::wstring, std::wstring>& V2Localisation::GetTextInEachLanguage(const std::wstring& key) const
{
	static const std::map<std::wstring, std::wstring> noLocalisation;	// used if there's no localisation

	const auto keyFindIter = localisations.find(key);	// the localisation we want
	if (keyFindIter == localisations.end())
	{
		return noLocalisation;
	}

	return keyFindIter->second;
}

