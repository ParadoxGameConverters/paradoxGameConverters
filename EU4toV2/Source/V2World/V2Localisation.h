#ifndef V2LOCALISATION_H_
#define V2LOCALISATION_H_

#include <array>
#include <iostream>
#include <string>
#include <vector>

class EU4Country;

// Holds translations for attributes of a specific V2 country.
class V2Localisation
{
public:
	// Sets the tag to use for creating the name and adjective key to use in the localisation output.
	void SetTag(const std::string& tag);
	// Populates the localised names and adjectives using information from the EU4 country.
	void ReadFromCountry(const EU4Country&);

	// Sets the key to use for the specified party in the localisation output.
	void SetPartyKey(size_t partyIndex, const std::string& partyKey);
	// Sets the localised party name for the specified party in the given language, e.g. "english".
	void SetPartyName(size_t partyIndex, const std::string& language, const std::string& name);

	// Writes a V2-formatted localisation info for all localised elements as:
	// key;translation0;translation1;...;;;x
	void WriteToStream(std::ostream&) const;

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
};

#endif