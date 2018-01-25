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
#include "../EU4World/EU4Country.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <algorithm>



const std::array<std::string, V2Localisation::numLanguages> V2Localisation::languages = 
	{ "english", "french", "german", "polish", "spanish", "italian", "hungarian", "czech" };

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

	return Utils::convertUTF8To8859_15(text);
}

std::string V2Localisation::GetLocalName()
{
	for (std::string thisname : name)
	{
		if (!thisname.empty())
		{
			return thisname;
		}
	}
	return "";
}


std::string V2Localisation::GetLocalAdjective()
{
	for (std::string thisname : adjective)
	{
		if (!thisname.empty())
		{
			return thisname;
		}
	}
	return "";
}

std::string V2Localisation::StripAccents(const std::string& text)
{
	std::string accents = "àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
	std::string without = "aaaaaaaceeeeiiiidnooooo ouuuuy y";
	std::string out(text);

	for (unsigned int i = 0; i < accents.size(); i++)
	{
		std::replace(out.begin(), out.end(), accents[i], without[i]);
	}
	return out;
}