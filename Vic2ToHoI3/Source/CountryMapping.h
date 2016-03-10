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



#ifndef COUNTRYMAPPING_H
#define COUNTRYMAPPING_H



#include <map>
#include <string>
#include <vector>
#include <boost/bimap.hpp>
using namespace std;



class V2World;
class HoI3World;



// Holds a mapping from Vic2 country tags to HoI3 country tags.
class CountryMapping
{
	public:
		// Initializes the rules to use for mapping tags using the rules found in the specified file.
		// Returns true if the rules were successfully read.
		bool ReadRules(const string& fileName);

		// Creates a new mapping from all countries in the Vic2 world to HoI3 tags using
		// the rules given in ReadRules(). HoI3 tags that already exists in the HoI3 world are given
		// priority over other tags. Countries with Vic2 tags that aren't in the rules or
		// have no available HoI3 tag (e.g. when multiple Vic2 countries would use the same HoI3 tag)
		// are given a generated tag "X00"-"X99".
		void CreateMapping(const V2World& srcWorld, const HoI3World& destWorld);

		// Returns the HoI3 tag that is mapped to by the given Vic2 tag. Returns an empty string
		// if there is no corresponding HoI3 tag. If CreateMapping() has been called then there
		// is guaranteed to be a HoI3 tag for every Vic2 country.
		const string& operator[](const string& V2Tag) const	{ return GetHoI3Tag(V2Tag); }

		// Returns the HoI3 tag that is mapped to by the given Vic2 tag. Returns an empty string
		// if there is no corresponding HoI3 tag. If CreateMapping() has been called then there
		// is guaranteed to be a HoI3 tag for every Vic2 country.
		const string& GetHoI3Tag(const string& V2Tag) const;

		// Returns the Vic2 tag that is mapped to by the given HoI3 tag. Returns an empty string
		// if there is no corresponding Vic2 tag. If CreateMapping() has been called then there
		// is guaranteed to be a Vic2 tag for every HoI3 country.
		const string& GetVic2Tag(const string& HoI3Tag) const;

	private:
		// Writes the given mapping to the log.
		static void LogMapping(const string& sourceTag, const string& targetTag, const string& reason);

		std::map<std::string, std::vector<std::string>>	V2TagToHoI3TagsRules;	// the possible mappings between V2 and HoI3 tags
		boost::bimap<std::string, std::string>				V2TagToHoI3TagMap;		// the current mappping between V2 and HoI3 tags
};

#endif