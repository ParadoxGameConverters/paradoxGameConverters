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



#ifndef V2LOCALISATION_H_
#define V2LOCALISATION_H_

#include <array>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>



// Holds translations for attributes of a specific V2 country.
class V2Localisation
{
public:
	// Sets the tag to use for creating the name and adjective key to use in the localisation output.
	void SetTag(const std::string& tag);

	// Sets the key to use for the specified party in the localisation output.
	void SetPartyKey(size_t partyIndex, const std::string& partyKey);
	// Sets the localised party name for the specified party in the given language, e.g. "english".
	void SetPartyName(size_t partyIndex, const std::string& language, const std::string& name);

	// Writes a V2-formatted localisation info for all localised elements as:
	// key;translation0;translation1;...;;;x
	void WriteToStream(std::ostream&) const;

	// Converts the country file name
	std::string convertCountryFileName(const std::string) const;

	// Adds all localisations found in the specified file. The file should begin with
	// a line like "l_english:" to indicate what language the texts are in.
	void ReadFromFile(const std::string& fileName);
	// Adds all localisations found in files in the specified folder as per ReadFromFile().
	void ReadFromAllFilesInFolder(const std::string& folderPath);

	// Returns the localised text for the given key in the specified language. Returns
	// an empty string if no such localisation is available.
	const std::string& GetText(const std::string& key, const std::string& language) const;
	// Returns the localised text for the given key in each language - the returned map is from
	// language to localised text.
	const std::map<std::string, std::string>& GetTextInEachLanguage(const std::string& key) const;

private:
	// All localisations are stored internally as UTF-8. However V2 doesn't seem to handle Unicode
	// so we convert the text to ANSI Latin-1 before writing it.
	static std::string Convert(const std::string&);

	static const size_t numLanguages = 14;
	static const std::array<std::string, numLanguages> languages;

	typedef std::array<std::string, numLanguages> Localisations;

	std::string tag;
	Localisations name;
	Localisations adjective;

	struct Party
	{
		std::string key;
		Localisations name;
	};
	std::vector<Party> parties;


	typedef std::map<std::string, std::string> LanguageToLocalisationMap;
	typedef std::unordered_map<std::string, LanguageToLocalisationMap> KeyToLocalisationsMap;
	KeyToLocalisationsMap localisations;	// a map between keys and localisations
};

#endif