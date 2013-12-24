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



#ifndef EU3RULER_H_
#define EU3RULER_H_


#include <string>
#include <vector>
#include "..\Date.h"
using namespace std;


class Object;
class CK2Character;

class EU3Ruler
{
	public:
		EU3Ruler(Object* obj);
		EU3Ruler(CK2Character* src);

		void		outputAsMonarch(FILE*);
		void		outputAsRegent(FILE*);
		void		outputAsHeir(FILE*);

		void		setRegnalNum(vector<EU3Ruler*> previousMonarchs);
		void		setRegnalNum(int num)	{ regnalNum = num; };

		string	getName()			const { return name; };
		int		getRegnalNum()		const { return regnalNum; };
		int		getID()				const { return id; };
		int		getAdmin()			const { return administration; };
		
	private:
		void outputRegnalNum(FILE*);

		string	name;
		int		regnalNum;
		int		diplomacy;
		int		administration;
		int		military;
		int		id;
		string	dynasty;
		date		birthDate;
		date		deathDate;
		int		claim;
		string	monarchName;
		bool		female;
};



#endif // EU3RULER_H_