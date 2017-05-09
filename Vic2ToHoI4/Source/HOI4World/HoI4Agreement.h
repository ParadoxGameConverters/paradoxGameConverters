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



#ifndef HOI4_AGREEMENT_H_
#define HOI4_AGREEMENT_H_


#include "Date.h"
#include "../V2World/Vic2Agreement.h"
#include <string>
using namespace std;



class HoI4Agreement
{
	public:
		HoI4Agreement(const string& _country1, const string& _country2, const V2Agreement* oldAgreement);
		HoI4Agreement(const string& _country1, const string& _country2, const string& _type, int _relationshipValue, const date& _startDate);

		bool operator==(const HoI4Agreement &rhs) const
		{
			return ((type == rhs.type) && (country1 == rhs.country1) && (country2 == rhs.country2));
		}

		friend ofstream& operator << (ofstream& output, HoI4Agreement& instance);

		string getType() const { return type; }

	private:
		string type;
		string country1;
		string country2;
		int relationshipValue;
		date startDate;
};



#endif // HOI4_AGREEMENT_H_