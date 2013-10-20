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



#ifndef EU3DIPLOMACY_H_
#define EU3DIPLOMACY_H_


#include "..\Date.h"
#include <vector>
using namespace std;



class Object;
class EU3Country;



struct EU3Agreement
{
	EU3Agreement();
	void output(FILE*);

	string		type;
	EU3Country*	country1;
	EU3Country*	country2;
	date			startDate;
};


class EU3Diplomacy
{
	public:
		EU3Diplomacy();
		void output(FILE*);
		void addAgreement(EU3Agreement* agr) { agreements.push_back(agr); }
		void removeAgreement(EU3Agreement* agr);
	private:
		vector<EU3Agreement*>	agreements;
};

// Types of agreements:
//open_market - care
//trade_agreement - care
//casus_belli - care
//royal_marriage - care
//guarantee - care
//alliance - care
//union - care
//sphere - care
//vassal - care

#endif