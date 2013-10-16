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



#ifndef EU3Advisor_H_
#define EU3Advisor_H_


#include <string>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;


class Object;
class CK2Character;
class EU3Province;
class EU3Country;

class EU3Advisor
{
	public:
		EU3Advisor(Object*, const map<int, EU3Province*>&);
		EU3Advisor(CK2Character*, const inverseProvinceMapping&, const map<int, EU3Province*>&);

		void	outputInProvince(FILE*);
		void	outputInActive(FILE*);

		void	setLocation(int _location)	{ location = _location; };
		void	setHome(EU3Country* _home)	{ home = _home; };

		int	getLocation()	const { return location; };
		date	getStartDate()	const { return startDate; };
		date	getDeathDate()	const { return deathDate; };

	private:
		string		name;
		int			id;
		string		dynasty;

		string		advisorType;
		int			advisorSkill;
		int			location;
		EU3Country*	home;

		date			startDate;
		date			deathDate;
};



#endif // EU3Advisor_H_