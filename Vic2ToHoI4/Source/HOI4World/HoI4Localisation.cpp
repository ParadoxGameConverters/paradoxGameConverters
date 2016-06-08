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



#include "HoI4Localisation.h"
#include <Windows.h>
#include <fstream>
#include "..\V2World\V2Country.h"
#include "Log.h"
#include "../../../common_items/WinUtils.h"



const std::array<std::wstring, HoI4Localisation::numLanguages> HoI4Localisation::languages = 
	{ L"english", L"french", L"german", L"spanish" };



void HoI4Localisation::SetTag(const std::wstring& newTag)
{
	tag = newTag;
}


void HoI4Localisation::ReadFromCountry(const V2Country& source)
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


void HoI4Localisation::SetPartyKey(size_t partyIndex, const std::wstring& partyKey)
{
	if (parties.size() <= partyIndex)
	{
		parties.resize(partyIndex + 1);
	}
	parties[partyIndex].key = partyKey;
}


void HoI4Localisation::SetPartyName(size_t partyIndex, const std::wstring& language, const std::wstring& name)
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


void HoI4Localisation::WriteToStream(std::wostream& out) const
{
	out << tag;
	for (const auto& localisedName : name)
	{
		out << ';' << localisedName;
	}
	out << "x\n";

	out << tag << "_ADJ";
	for (const auto& localisedAdjective : adjective)
	{
		out << ';' << localisedAdjective;
	}
	out << "x\n";

	for (const auto& party : parties)
	{
		out << party.key;
		for (const auto& localisedPartyName : party.name)
		{
			out << ';' << localisedPartyName;
		}
		out << "x\n";
	}
}
