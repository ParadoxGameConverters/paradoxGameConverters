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



#ifndef V2FACTORY_H_
#define V2FACTORY_H_



#include "V2Inventions.h"
#include <deque>
#include <vector>
#include <map>
using namespace std;

class Object;



struct V2FactoryType
{
	V2FactoryType(Object* factory);

	string						name;
	bool							requireCoastal;
	string						requireTech;
	vanillaInventionType		vanillaRequiredInvention;
	HODInventionType			HODRequiredInvention;
	bool							requireLocalInput;
	map<string,float>			inputs;
};


class V2Factory
{
	public:
		V2Factory(const V2FactoryType* _type) : type(_type) {};
		void						output(FILE* output) const;
		map<string,float>		getRequiredRGO() const;

		bool						requiresCoastal()					const { return type->requireCoastal; }
		string					getRequiredTech()					const { return type->requireTech; }
		vanillaInventionType	getVanillaRequiredInvention()	const { return type->vanillaRequiredInvention; }
		HODInventionType		getHODRequiredInvention()		const { return type->HODRequiredInvention; }
		string					getTypeName()						const { return type->name; }
	private:
		const V2FactoryType* type;
};


class V2FactoryFactory
{
	public:
		V2FactoryFactory();
		deque<V2Factory*>	buildFactories() const;
	private:
		void					loadRequiredTechs(string filename);
		void					loadRequiredInventions(string filename);
		vector<pair<V2FactoryType*, int>>	factoryCounts;
		map<string, V2FactoryType*>			factoryTypes;
		map<string, string>						factoryTechReqs;
		map<string, string>						factoryInventionReqs;
};



#endif // V2FACTORY_H_