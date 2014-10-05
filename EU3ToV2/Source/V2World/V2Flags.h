#ifndef V2FLAGS_H
#define V2FLAGS_H

#include <map>
#include <set>
#include <string>
#include <vector>

class V2Country;

// Holds information about all the flags for V2 countries and copies over
// the appropriate flags with Output().
class V2Flags 
{
public:
	// Tries to find appropriate flags for all the countries specified.
	void SetV2Tags(const std::map<std::string, V2Country*>& V2Countries);
	// Copies all necessary flags to the output folder. Returns true if successful.
	bool Output() const;

private:
	static const std::vector<std::string> flagFileSuffixes;

	typedef std::map<std::string, std::string> V2TagToFlagTagMap;
	V2TagToFlagTagMap tagMapping;
};

#endif