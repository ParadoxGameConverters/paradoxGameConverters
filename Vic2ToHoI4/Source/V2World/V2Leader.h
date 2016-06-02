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


#ifndef V2_LEADER_H_
#define V2_LEADER_H_



#include <string>
#include "Object.h"
using namespace std;


class V2Leader
{
	public:
		V2Leader(Object* obk);

		wstring	getName()			const	{ return name; }
		wstring	getType()			const { return type; }
		double	getPrestige()		const { return prestige; }
		wstring	getPersonality()	const { return personality; }
		wstring	getBackground()	const { return background; }

	private:
		wstring	name;
		wstring	type;
		wstring	personality;
		wstring	background;
		double	prestige;
};



#endif V2LEADER_H_