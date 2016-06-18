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
#include "../V2World/V2Country.h"
#include "Log.h"
#include "../../../common_items/OSCompatibilityLayer.h"



const std::array<std::string, HoI4Localisation::numLanguages> HoI4Localisation::languages =
{ "english", "french", "german", "spanish" };



void HoI4Localisation::SetTag(const std::string& newTag)
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


void HoI4Localisation::SetPartyKey(size_t partyIndex, const std::string& partyKey)
{
	if (parties.size() <= partyIndex)
	{
		parties.resize(partyIndex + 1);
	}
	parties[partyIndex].key = partyKey;
}


void HoI4Localisation::SetPartyName(size_t partyIndex, const std::string& language, const std::string& name)
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


void HoI4Localisation::WriteToStream(std::ostream& out) const
{
	out << " " << tag << "_fascism:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_fascism_DEF:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_democratic:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_democratic_DEF:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_neutrality:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_neutrality_DEF:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_communism:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_communism_DEF:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_fascism_ADJ:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_democratic_ADJ:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_neutrality_ADJ:0 \"" << name.front() << "\"" << endl;
	out << " " << tag << "_communism_ADJ:0 \"" << name.front() << "\"" << endl;
	//thatsgerman: parties are always 0 atm
	/*for (const auto& party : parties)
	{
		out << party.key;
		for (const auto& localisedPartyName : party.name)
		{
			out << ';' << localisedPartyName;
		}
		out << "x\n";
	}*/
}
