#ifndef COUNTRYMAPPING_H
#define COUNTRYMAPPING_H

#include <map>
#include <string>
#include <vector>

#include <boost/bimap.hpp>

class EU4World;
class V2World;

// Holds a mapping from EU4 country tags to V2 country tags.
class CountryMapping
{
public:
	// Initializes the rules to use for mapping tags using the rules found in the specified file.
	// Returns true if the rules were successfully read.
	bool ReadRules(const std::string& fileName);
	// Creates a new mapping from all countries in the EU4 world to V2 tags using
	// the rules given in ReadRules(). V2 tags that already exists in the V2 world are given
	// priority over other tags. Countries with EU4 tags that aren't in the rules or
	// have no available V2 tag (e.g. when multiple EU4 countries would use the same V2 tag)
	// are given a generated tag "X00"-"X99".
	void CreateMapping(const EU4World& srcWorld, const V2World& destWorld);

	// Returns the V2 tag that is mapped to by the given EU4 tag. Returns an empty string
	// if there is no corresponding V2 tag. If CreateMapping() has been called then there
	// is guaranteed to be a V2 tag for every EU4 country.
	const std::string& operator[](const std::string& EU4Tag) const	{ return GetV2Tag(EU4Tag); }
	// Returns the V2 tag that is mapped to by the given EU4 tag. Returns an empty string
	// if there is no corresponding V2 tag. If CreateMapping() has been called then there
	// is guaranteed to be a V2 tag for every EU4 country.
	const std::string& GetV2Tag(const std::string& EU4Tag) const;
	// Returns the EU4 tag that maps to the given V2 tag. Returns an empty string if there
	// is no such EU4 tag.
	const std::string& GetEU4Tag(const std::string& V2Tag) const;

private:
	// Writes the given mapping to the log.
	static void LogMapping(const std::string& EU4Tag, const std::string& V2Tag, const std::string& reason);

	std::map<std::string, std::vector<std::string>> EU4TagToV2TagsRules;
	boost::bimap<std::string, std::string> EU4TagToV2TagMap;
};

#endif