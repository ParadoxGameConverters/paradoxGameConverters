/*Copyright (c) 2015 The Paradox Game Converters Project

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



#ifndef V2DIPLOMACY_H_
#define V2DIPLOMACY_H_



#include "../Date.h"
#include "../Parsers/Object.h"
#include <vector>



struct V2Agreement
{
	V2Agreement(Object* obj);

	string	type;
	string	country1;
	string	country2;
	date		start_date;
};


class V2Diplomacy
{
	public:
		V2Diplomacy()	{ agreements.clear(); };
		V2Diplomacy(Object *obj);

		vector<V2Agreement>&	getAgreements()	{ return agreements; };

	private:
		vector<V2Agreement>	agreements;
};



#endif