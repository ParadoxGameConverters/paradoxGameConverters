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



#ifndef V2WORLD_H_
#define V2WORLD_H_



#include "V2Inventions.h"
#include "V2Localisation.h"
#include "V2Diplomacy.h"
#include "../Mapper.h"
#include <set>
#include <vector>
#include <string>
using namespace std;


class		V2Country;
class		V2Province;
struct	V2Party;


class V2World
{
	public:
		V2World(Object* obj, const inventionNumToName& iNumToName, map<string, string>& armyTechs, map<string, string>& navyTechs, const continentMapping& continentMap);

		V2Country*					getCountry(string tag) const;
		void							removeCountry(string tag);
		V2Province*					getProvince(int provNum) const;
		void							checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const;
		void							setLocalisations(V2Localisation& localisation);
		V2Party*						getRulingParty(const V2Country* country) const;
		vector<V2Party*>			getActiveParties(const V2Country* country) const;
		
		map<string, V2Country*>	getCountries()	const			{ return countries; }
		const V2Diplomacy*		getDiplomacy()	const			{ return &diplomacy; }
		const vector<string>&	getGreatCountries() const	{ return greatCountries; }
	private:
		void							readCountryFiles(string countryListFile, string mod);
		void							removeEmptyNations();

		map<int, V2Province*>	provinces;
		map<string, V2Country*>	countries;
		V2Diplomacy					diplomacy;
		vector<V2Party*>			parties;
		vector<string>				greatCountries; // Tags of great nations in order of ranking
};



#endif // V2WORLD_H_