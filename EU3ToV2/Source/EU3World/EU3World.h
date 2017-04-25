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


#ifndef EU3WORLD_H_
#define EU3WORLD_H_


#include <istream>
#include "EU3Army.h"
#include "../Mapper.h"

class EU3Country;
class EU3Province;
class EU3Diplomacy;
class EU3Localisation;
struct EU3Agreement;



enum WorldType
{
	unknown = 0,
	VeryOld,
	InNomine,
	HeirToTheThrone,
	DivineWind
};


class EU3World {
	public:
		EU3World(Object* obj);
		void setEU3WorldProvinceMappings(const inverseProvinceMapping& inverseProvinceMap);

		void	readCommonCountries(istream&, const std::string& rootPath);
		void	readCountryLocalisation(istream&);

		EU3Country*						getCountry(string tag) const;
		EU3Province*					getProvince(int provNum) const;
		void								removeCountry(string tag);
		void								resolveRegimentTypes(const RegimentTypeMap& map);
		WorldType						getWorldType();
		void								checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const;
		void								checkAllEU3CulturesMapped(const cultureMapping& cultureMap, const inverseUnionCulturesMap& inverseUnionCultures) const;
		void								checkAllEU3ReligionsMapped(const religionMapping& religionMap) const;
		void								setLocalisations(EU3Localisation& localisation);

		map<string, EU3Country*>	getCountries()	const { return countries; };
		EU3Diplomacy*					getDiplomacy()	const { return diplomacy; };
		double							getWorldWeightSum() const { return worldWeightSum; };
	private:
		WorldType						cachedWorldType;
		map<int, EU3Province*>		provinces;
		map<string, EU3Country*>	countries;
		EU3Diplomacy*					diplomacy;
		double							worldWeightSum;
};



#endif // EU3WORLD_H_
