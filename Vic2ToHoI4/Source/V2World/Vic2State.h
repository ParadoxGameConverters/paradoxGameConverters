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



#ifndef VIC2STATE_H_
#define VIC2STATE_H_



#include <vector>
using namespace std;



class Vic2State
{
	public:
		void	addProvince(int provNum)	{ provinces.push_back(provNum); }
		void	setFactoryLevel(int level)	{ factoryLevel = level; }
		void	setID(int id)					{ stateID = id; }
		void	setName(string _name)		{ name = _name; }


		vector<int>	getProvinces() const		{ return provinces; }
		int			getFactoryLevel() const	{ return factoryLevel; }
		int			getStateID() const		{ return stateID; }
		string		getName() const			{ return name; }

	private:
		vector<int>		provinces;
		int				factoryLevel;
		int				stateID;
		string			name;
};



#endif // VIC2STATE_H_
