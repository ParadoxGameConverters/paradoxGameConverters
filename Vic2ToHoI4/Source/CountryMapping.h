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
class HoI4World;



// Holds a mapping from Vic2 country tags to HoI4 country tags.
class CountryMapping
{
	public:
		// Initializes the rules to use for mapping tags using the rules found in the specified file.
		// Returns true if the rules were successfully read.
		bool ReadRules(const wstring& fileName);

		// Creates a new mapping from all countries in the Vic2 world to HoI4 tags using
		// the rules given in ReadRules(). HoI4 tags that already exists in the HoI4 world are given
		// priority over other tags. Countries with Vic2 tags that aren't in the rules or
		// have no available HoI4 tag (e.g. when multiple Vic2 countries would use the same HoI4 tag)
		// are given a generated tag "X00"-"X99".
		void CreateMapping(const V2World& srcWorld, const HoI4World& destWorld);

		// Returns the HoI4 tag that is mapped to by the given Vic2 tag. Returns an empty wstring
		// if there is no corresponding HoI4 tag. If CreateMapping() has been called then there
		// is guaranteed to be a HoI4 tag for every Vic2 country.
		const wstring& operator[](const wstring& V2Tag) const	{ return GetHoI4Tag(V2Tag); }

		// Returns the HoI4 tag that is mapped to by the given Vic2 tag. Returns an empty wstring
		// if there is no corresponding HoI4 tag. If CreateMapping() has been called then there
		// is guaranteed to be a HoI4 tag for every Vic2 country.
		const wstring& GetHoI4Tag(const wstring& V2Tag) const;

		// Returns the Vic2 tag that is mapped to by the given HoI4 tag. Returns an empty wstring
		// if there is no corresponding Vic2 tag. If CreateMapping() has been called then there
		// is guaranteed to be a Vic2 tag for every HoI4 country.
		const wstring& GetVic2Tag(const wstring& HoI4Tag) const;

	private:
		// Writes the given mapping to the log.
		static void LogMapping(const wstring& sourceTag, const wstring& targetTag, const wstring& reason);

		std::map<std::wstring, std::vector<std::wstring>>	V2TagToHoI4TagsRules;	// the possible mappings between V2 and HoI4 tags
		boost::bimap<std::wstring, std::wstring>				V2TagToHoI4TagMap;		// the current mappping between V2 and HoI4 tags
};

#endif