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



#ifndef EU4WORLD_H_
#define EU4WORLD_H_

#include <istream>
#include "EU4Army.h"
#include "../Mappers/Mapper.h"

class EU4Country;
class EU4Province;
class EU4Diplomacy;
class EU4Version;
class EU4Localisation;
struct EU4Agreement;



class EU4World {
	public:
		EU4World(Object* obj, map<string, int> armyInvIdeas, map<string, int> commerceInvIdeas, map<string, int> cultureInvIdeas, map<string, int> industryInvIdeas, map<string, int> navyInvIdeas, inverseUnionCulturesMap& inverseUnionCultures);
		void setNumbersOfDestinationProvinces();

		void readCommonCountries(istream&, const std::string& rootPath);

		EU4Country*						getCountry(string tag) const;
		EU4Province*					getProvince(int provNum) const;
		void								removeCountry(string tag);
		void								resolveRegimentTypes(const RegimentTypeMap& map);
		void								checkAllProvincesMapped() const;
		void								checkAllEU4CulturesMapped(const cultureMapping& cultureMap, const inverseUnionCulturesMap& inverseUnionCultures) const;
		void								checkAllEU4ReligionsMapped(const religionMapping& religionMap) const;
		void								setLocalisations(EU4Localisation& localisation);

		EU4Version*						getVersion()			const { return version; };
		map<string, EU4Country*>	getCountries()			const { return countries; };
		EU4Diplomacy*					getDiplomacy()			const { return diplomacy; };
		double							getWorldWeightSum()	const { return worldWeightSum; };

	private:
		map<int, EU4Province*>		provinces;	// the provinces
		map<string, EU4Country*>	countries;	// the countries
		EU4Diplomacy*					diplomacy;	// diplomatic relationships
		EU4Version*						version;		// the EU4 version for this save
		double							worldWeightSum;
};



#endif // EU4WORLD_H_
