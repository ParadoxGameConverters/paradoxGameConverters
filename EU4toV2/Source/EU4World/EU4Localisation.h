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



#ifndef EU4LOCALISATION_H_
#define EU4LOCALISATION_H_

#include <map>
#include <string>
#include <unordered_map>
#include <utility>

// Holds a map from key to localised text for all languages in which 
// the localisation is provided.
class EU4Localisation
{
public:
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
	// Returns the language name from text in the form "l_english:". Returns an empty string
	// if the text doesn't fit this format.
	static std::string DetermineLanguageForFile(const std::string& text);
	// Returns the localisation from text in the form 'KEY: "Localisation"'. Returns a pair
	// with empty strings if the text doesn't fit this format. Additional spaces around the
	// elements can be included and are ignored.
	static std::pair<std::string, std::string> DetermineKeyLocalisationPair(const std::string& text);
	// Removes a UTF-8 BOM from the beginning of the text, if present. (These are added by the
	// CK2-EU4 converter.)
	static std::string RemoveUTF8BOM(const std::string& text);

	typedef std::map<std::string, std::string> LanguageToLocalisationMap;
	typedef std::unordered_map<std::string, LanguageToLocalisationMap> KeyToLocalisationsMap;
	KeyToLocalisationsMap localisations;	// a map between keys and localisations
};

#endif