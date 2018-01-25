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



#ifndef HoI4RELATIONS_H_
#define HoI4RELATIONS_H_



#include "Date.h"

class V2Relations;



class HoI4Relations
{
	public:
		explicit HoI4Relations(const string& newTag);
		HoI4Relations(const string& newTag, const V2Relations* oldRelations);

		string	getTag()				const { return tag; };
		int		getRelations()		const { return value; };
		bool		getGuarantee()		const { return guarantee; };
		bool		getSphereLeader()		const { return sphereLeader; };
		date		getLastWar()		const { return lastWar; };
		date		getTruceUntil()	const { return truceUntil; };
		bool		atWar()				const { return lastWar > truceUntil; };

	private:
		HoI4Relations(const HoI4Relations&) = delete;
		HoI4Relations& operator=(const HoI4Relations&) = delete;

		string	tag;
		int		value;
		bool		militaryAccess;
		date		lastSendDiplomat;
		date		lastWar;
		date		truceUntil;
		bool		guarantee;
		bool		sphereLeader;
};



#endif // HoI4RELATIONS_H_