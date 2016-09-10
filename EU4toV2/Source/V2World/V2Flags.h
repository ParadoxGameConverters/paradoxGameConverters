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



#ifndef V2FLAGS_H
#define V2FLAGS_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "../Mappers/Mapper.h"

class V2Country;

// Holds information about all the flags for V2 countries and copies over
// the appropriate flags with Output().
class V2Flags 
{
public:
	// Tries to find appropriate flags for all the countries specified.
	void SetV2Tags(const std::map<std::string, V2Country*>& V2Countries, const colonyFlagset& colonyFlagset);
	// Copies all necessary flags to the output folder. Returns true if successful.
	bool Output() const;

private:
	static const std::vector<std::string> flagFileSuffixes;
	colonyFlagset colonyFlags;

	typedef std::map<std::string, std::string> V2TagToFlagTagMap;
	typedef std::map<std::string, shared_ptr<colonyFlag> > V2TagToColonyFlagMap; // tag, {base,overlordtag}
	typedef std::map<std::string, CustomFlag> V2TagToCustomFlagMap;

	V2TagToFlagTagMap tagMapping;
	V2TagToColonyFlagMap colonialFlagMapping;
	V2TagToCustomFlagMap customFlagMapping;
};

#endif