/*Copyright(c) 2017 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */



#ifndef EU4DIPLOMACY_H_
#define EU4DIPLOMACY_H_



#include "Date.h"
#include <memory>
#include <vector>
using namespace std;



class Object;



struct EU4Agreement
{
	EU4Agreement(shared_ptr<Object> obj);

	string	type;			// the type of agreement
	string	country1;	// the first country
	string	country2;	// the second country
	date		startDate;	// the date this agreement began
};


class EU4Diplomacy
{
	public:
		EU4Diplomacy();
		EU4Diplomacy(shared_ptr<Object> obj);
		vector<EU4Agreement>	getAgreements() const { return agreements; };
	private:
		vector<EU4Agreement>	agreements;	// all the agreements
};



#endif