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



#ifndef CK2DYNASTY_H_
#define CK2DYNASTY_H_


#include <string>
#include <vector>
using namespace std;



class Object;
class CK2Character;

class CK2Dynasty
{
	public:
		CK2Dynasty(Object*);
		CK2Dynasty(int, string);

		void				addMember(CK2Character*);
		CK2Character*	getSenoirityHeir(string genderLaw);

		string			getName()	const { return name; };
	private:
		int							num;
		string						name;
		vector<CK2Character*>	members;
};



#endif // CK2DYNASTY_H_