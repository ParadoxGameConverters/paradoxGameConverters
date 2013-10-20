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



#ifndef EU3HISTORY_H_
#define EU3HISTORY_H_


#include "..\Date.h"
#include "..\Mappers.h"
#include <fstream>


class CK2History;
class EU3Ruler;
class EU3Advisor;

class EU3History {
	public:
		EU3History(date when);
		EU3History(CK2History*);

		void				output(FILE*);

		date				when;
		EU3Ruler*		monarch;
		EU3Ruler*		regent;
		EU3Ruler*		heir;
		EU3Advisor*		advisor;
		string			capital;
		string			tradeGood;
		double			baseTax;
		double			population;
		int				manpower;
		string			owner;
		string			add_core;
		string			remove_core;
		string			government;
		string			culture;
		string			religion;
		string			primaryCulture;
		vector<string>	acceptedCultures;
		string			techGroup;
		vector<string>	discoverers;
		double			shogunPower;
};


#endif // EU3HISTORY_H_