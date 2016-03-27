/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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



#ifndef CK2TRAIT_H_
#define CK2TRAIT_H_

#include <string>
#include <vector>
using namespace std;

class Object;

class CK2Trait
{
	public:
		CK2Trait(Object*);

		int	diplomacy;
		int	martial;
		int	stewardship;
		int	intrigue;
		int	learning;

		string name;
		vector<string> opposites;

		bool isOppositeOf(CK2Trait* other) const;

		// opinion values
		int vassal_opinion;
		int liege_opinion;
		int sex_appeal_opinion;
		int same_opinion;
		int opposite_opinion;
		int church_opinion;
		int same_opinion_if_same_religion;
		int twin_opinion;
		int spouse_opinion;
		int same_religion_opinion;
		int infidel_opinion;
		int muslim_opinion;
		int dynasty_opinion;
};



#endif // CK2TRAIT_H_