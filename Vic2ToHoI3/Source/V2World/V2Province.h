/*Copyright (c) 2015 The Paradox Game Converters Project

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



#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_



#include "../Configuration.h"
#include "Object.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class V2Country;
class V2Pop;
class V2Factory;


class V2Province
{
	public:
		V2Province(Object* obj);
		void	addCore(string);
		void	removeCore(string tag);

		int						getTotalPopulation() const;
		vector<V2Country*>	getCores(const map<string, V2Country*>& countries) const;
		int						getPopulation(string type = "") const;
		int						getLiteracyWeightedPopulation(string type = "") const;

		void						setOwner(V2Country* _owner)	{ owner = _owner; }

		string					getOwnerString()	const { return ownerString; }
		V2Country*				getOwner()			const { return owner; }
		int						getInfra()			const { return railLevel; }
		int						getFort()			const { return fortLevel; }
		int						getNavalBase()		const { return navalBaseLevel; }
		int						getNum()				const { return num; }


	private:
		int				num;
		string			ownerString;			// a string with the owner's tag
		V2Country*		owner;
		vector<string>	cores;
		vector<V2Pop*>	pops;
		int				fortLevel;
		int				navalBaseLevel;
		int				railLevel;
};



#endif // V2PROVINCE_H_