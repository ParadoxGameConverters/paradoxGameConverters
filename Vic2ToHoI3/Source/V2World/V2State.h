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



#ifndef V2Province_H_
#define V2Province_H_



#include "../EU4World/EU4World.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
using namespace std;

class V2Province;
class V2Factory;
class EU4World;



class V2State
{
	public:
		V2State(int newId, V2Province* firstProvince);

		void addRailroads();
		void setupPops(string primaryCulture, set<string> acceptedCultures, string religion);

		bool	isCoastal() const;
		bool	hasLocalSupply(string product) const;
		int	getCraftsmenPerFactory() const;
		bool	provInState(int id) const;

		void	addProvince(V2Province* newProvince)	{ provinces.push_back(newProvince); };
		void	addFactory(const V2Factory* factory)	{ factories.push_back(factory); };
		void	setColonial(bool isIt)						{ colonial = isIt; };
		void	setColonised(bool isIt)						{ colonised = isIt; };

		bool						isColonial()		const { return colonial; };
		int						getFactoryCount()	const { return factories.size(); };
		int						getID()				const { return id; };
		vector<V2Province*>	getProvinces()		const { return provinces; };
	private:
		int	getStatePopulation() const;
		int								id;
		bool								colonial;
		bool								colonised;
		vector<V2Province*>			provinces;
		vector<const V2Factory*>	factories;
		int employedCraftsmen;
};


#endif	// V2Province_H_