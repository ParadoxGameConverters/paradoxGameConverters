/*Copyright (c) 2017 The Paradox Game Converters Project

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



#ifndef V2POP_H_
#define V2POP_H_



#include <string>
#include <vector>
#include "Object.h"
using namespace std;



class V2Pop
{
	public:
		V2Pop(string type, int size, string culture, string religion);
		V2Pop(shared_ptr<Object> popObj);

		void output(FILE*) const;
		bool combine(const V2Pop& rhs);

		void	changeSize(int delta)					{ size += delta; }
		void	incrementSupportedRegimentCount()	{ supportedRegiments++; }
		void	setCulture(string _culture)			{ culture = _culture; }
		void	setReligion(string _religion)			{ religion = _religion; }

		int		getSize()							const	{ return size; }
		string	getType()							const	{ return type; }
		string	getCulture()						const	{ return culture; }
		string	getReligion()						const	{ return religion; }
		int		getSupportedRegimentCount()	const	{ return supportedRegiments; }
		bool isSlavePop() const { return ((type == "slaves") || (culture.substr(0, 4) == "afro")); }

	private:
		string	type;
		int		size;
		string	culture;
		string	religion;
		int		supportedRegiments;
};


int getNextPopId();



#endif