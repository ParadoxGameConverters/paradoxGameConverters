/*Copyright (c) 2018 The Paradox Game Converters Project

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



#ifndef VIC2_WORLD_H_
#define VIC2_WORLD_H_



#include "newParser.h"
#include "Party.h"
#include <map>
#include <optional>
#include <string>
#include <vector>



namespace Vic2
{

class Country;
class Diplomacy;
class Party;
class Province;


class World: commonItems::parser
{
	public:
		explicit World(const std::string& filename);

		std::optional<const Province*> getProvince(int provNum) const;
		
		std::map<std::string, Country*> getCountries() const { return countries; }
		const Diplomacy* getDiplomacy() const { return diplomacy; }
		std::vector<std::string> getGreatPowers() const	{ return greatPowers; }
		std::vector<Party> getParties() const { return parties; }
		auto getProvinces() const { return provinces; }

	private:
		World(const World&) = delete;
		World& operator=(const World&) = delete;

		void setLocalisations();
		void handleMissingCountryCultures();

		void setGreatPowerStatus(const std::vector<int>& GPIndexes, const std::vector<std::string>& tagsInOrder);

		void setProvinceOwners();
		void addProvinceCoreInfoToCountries();
		void removeSimpleLandlessNations();
		bool shouldCoreBeRemoved(const Province* core, const Country* country) const;
		void determineEmployedWorkers();
		void removeEmptyNations();
		void determinePartialStates();

		void overallMergeNations();
		void mergeNations(const std::string& masterTag, const std::vector<std::string>& slaveTags);

		void checkAllProvincesMapped() const;

		void readCountryFiles();
		bool processCountriesDotTxt(const std::string& countryListFile, const std::string& mod);
		bool shouldLineBeSkipped(const std::string& line) const;
		std::string extractCountryFileName(const std::string& countryFileLine) const;

		std::optional<Country*> getCountry(const std::string& tag) const;


		std::map<int, Province*> provinces;
		std::map<std::string, Country*> countries;
		const Diplomacy* diplomacy = nullptr;
		std::vector<Party> parties;
		std::vector<std::string> greatPowers;
};


}



#endif // VIC2_WORLD_H_