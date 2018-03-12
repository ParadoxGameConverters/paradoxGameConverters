/*Copyright (c) 2018 The Paradox Game Converters Project

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
#include "../Mappers/ColonyFlagsetMapper.h"
#include "../CustomFlagMapper.h"
using namespace std;



class V2Country;



class V2Flags 
{
	public:
		void SetV2Tags(const map<string, V2Country*>& V2Countries);
		void output() const;

	private:
		void determineUseableFlags();
		set<string> determineAvailableFlags();
		void getRequiredTags(const map<string, V2Country*>& V2Countries);
		void mapTrivialTags();

		void createOutputFolders() const;
		void copyFlags() const;
		void createCustomFlags() const;
		void createColonialFlags() const;

		set<string> usableFlagTags;
		set<string> requiredTags;

		static const vector<string> flagFileSuffixes;

		typedef map<string, string> V2TagToFlagTagMap;
		typedef map<string, shared_ptr<mappers::colonyFlag> > V2TagToColonyFlagMap; // tag, {base,overlordtag}
		typedef map<string, CustomFlag> V2TagToCustomFlagMap;

		V2TagToFlagTagMap tagMap;
		V2TagToColonyFlagMap colonialFlagMapping;
		V2TagToCustomFlagMap customFlagMapping;
};



#endif