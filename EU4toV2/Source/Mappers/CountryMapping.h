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



#ifndef COUNTRYMAPPING_H
#define COUNTRYMAPPING_H

#include <map>
#include <string>
#include <vector>
#include <boost/bimap.hpp>
#include "Mapper.h"

class EU4World;
class EU4Country;
class V2World;
class V2Country;

// Holds a mapping from EU4 country tags to V2 country tags.
class CountryMapping
{
public:
	// Initializes the rules to use for mapping tags using the rules found in the specified file.
	// Returns true if the rules were successfully read.
	bool ReadRules(const std::string& fileName);

	void readEU4Regions(Object*);
	void readV2Regions(Object*);

	// Creates a new mapping from all countries in the EU4 world to V2 tags using
	// the rules given in ReadRules(). V2 tags that already exists in the V2 world are given
	// priority over other tags. Countries with EU4 tags that aren't in the rules or
	// have no available V2 tag (e.g. when multiple EU4 countries would use the same V2 tag)
	// are given a generated tag "X00"-"X99".
	void CreateMapping(const EU4World& srcWorld, const V2World& destWorld, const colonyMapping& colonyMap, const inverseUnionCulturesMap& inverseUnionCultures);

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

	// Returns the CK2 title name that maps to the given country name. Returns an empty
	// string if there is no corresponding CK2 title.
	static std::string GetCK2Title(const std::string& EU4Tag, const std::string& countryName, const std::set<std::string>& availableFlags);

private:
	// Writes the given mapping to the log.
	static void	LogMapping(const std::string& EU4Tag, const std::string& V2Tag, const std::string& reason);

	// if there is a valid colonial replacement, uses it
	bool			attemptColonialReplacement(EU4Country* country, const EU4World& srcWorld, const map<string, V2Country*> V2Countries, const colonyMapping& colonyMap, const inverseUnionCulturesMap& inverseUnionCultures);

	void			oneMapping(EU4Country* country, const map<string, V2Country*> V2Countries, char& generatedV2TagPrefix, int& generatedV2TagSuffix);

	std::map<std::string, std::vector<std::string>> EU4TagToV2TagsRules;		// the possible mappings between EU4 and V2 tags
	boost::bimap<std::string, std::string> EU4TagToV2TagMap;						// the current mappping between EU4 and V2 tags
	std::map<std::string, std::set<int>> EU4ColonialRegions;						// the colonial regions in EU4
	std::map<std::string, std::set<int>> V2Regions;									// the regions in V2

	std::set<std::string> availableFlags;                  // the flags available to the converter
};

#endif