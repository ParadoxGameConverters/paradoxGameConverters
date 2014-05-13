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
	KeyToLocalisationsMap localisations;
};

#endif