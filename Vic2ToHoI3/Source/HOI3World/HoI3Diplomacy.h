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



#ifndef HoI3DIPLOMACY_H_
#define HoI3DIPLOMACY_H_



#include "../Date.h"
#include <vector>



struct HoI3Agreement
{
	string	type;
	string	country1;
	string	country2;
	int value; // For relation value
	date		start_date;

	// Ignore value and start_date, because value might not be set, and start_date is irrelevant
	bool operator==(const HoI3Agreement &rhs) const { return this->type == rhs.type && this->country1 == rhs.country1 && this->country2 == rhs.country2; };
};


class HoI3Diplomacy
{
	public:
		HoI3Diplomacy() { agreements.clear(); };
		void output() const;

		void addAgreement(const HoI3Agreement agr);
		const vector<HoI3Agreement> &getAgreements() const { return agreements; };
	private:
		vector<HoI3Agreement>	agreements;
};



#endif