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

class EU4World;
class V2World;
class HoI3World;

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
	void CreateMapping(const V2World& srcWorld, const HoI3World& destWorld);

	// Returns the V2 tag that is mapped to by the given EU4 tag. Returns an empty string
	// if there is no corresponding V2 tag. If CreateMapping() has been called then there
	// is guaranteed to be a V2 tag for every EU4 country.
	const std::string& operator[](const std::string& V2Tag) const	{ return GetHoI3Tag(V2Tag); }
	// Returns the V2 tag that is mapped to by the given EU4 tag. Returns an empty string
	// if there is no corresponding V2 tag. If CreateMapping() has been called then there
	// is guaranteed to be a V2 tag for every EU4 country.
	const std::string& GetV2Tag(const std::string& HoI3Tag) const;
	// Returns the EU4 tag that maps to the given V2 tag. Returns an empty string if there
	// is no such EU4 tag.
	const std::string& GetEU4Tag(const std::string& V2Tag) const;
	const std::string& GetHoI3Tag(const std::string& V2Tag) const;

private:
	// Writes the given mapping to the log.
	static void LogMapping(const std::string& sourceTag, const std::string& targetTag, const std::string& reason);

	std::map<std::string, std::vector<std::string>> EU4TagToV2TagsRules;		// the possible mappings between EU4 and V2 tags
	boost::bimap<std::string, std::string> EU4TagToV2TagMap;						// the current mappping between EU4 and V2 tage
	std::map<std::string, std::vector<std::string>> V2TagToHoI3TagsRules;		// the possible mappings between V2 and HoI3 tags
	boost::bimap<std::string, std::string> V2TagToHoI3TagMap;						// the current mappping between V2 and HoI3 tage
};

#endif